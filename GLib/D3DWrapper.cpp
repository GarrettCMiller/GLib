///////////////////////////////////////////////////////////////////////////////
//		File:				D3DWrapper.cpp
//
//		Programmer:			Garrett Miller (GM)
//
//		Date created:		August 10, 2004
//
//		Description:		A Direct3D wrapper class with sprite functionality
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "D3DWrapper.h"

#include "Globals.h"
#include "EffectMgr.h"
#include "FontMgr.h"

#include "SpriteMgr.h"

#include "ProfileSample.h"

//////////////////////////////////////////////////////////////////////////

BaseRenderRequest::ERenderType BaseRenderRequest::mk_eRenderType = BaseRenderRequest::eRT_Shadow;

//	\\HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Direct3D\\D3D9Debugging\\EnableCreationStack
//LPCWSTR CreationCallStack;
//////////////////////////////////////////////////////////////////////////

void BaseRenderRequest::PreRender()
{
	static Timer tTime(true);

	CMatrix matVP;
	//g_D3D->SetWorldMatrix(matWorld);

	if (effectID == -1)
		return;
	
	matVP = g_D3D->GetViewProj();

	Effect& fx = g_EffectMgr->GetEffect(effectID);
	fx.Shader->SetTechnique(effectTech);
	fx.Shader->SetMatrix("matWorld", &matWorld);
	fx.Shader->SetMatrix("matViewProj", &matVP);
	fx.Shader->SetFloat("g_Time", tTime.Get());

	//Set vars
	/*for (UInt32 var = 0; var < effectVars.Size(); var++)
	{
		fx.Shader->Set
	}*/
}

void BaseRenderRequest::Render()
{
	if (effectID == -1)
		Draw();
	else
	{
		UInt32 numPasses = 1;
		Effect& fx = g_EffectMgr->GetEffect(effectID);

		fx.Shader->Begin(&numPasses, 0);

		for (UInt32 rPass = 0; rPass < numPasses; rPass++)
		{
			fx.Shader->BeginPass(rPass);

			Draw();

			fx.Shader->EndPass();
		}

		fx.Shader->End();
	}
}

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_SINGLETON(D3D);

static HRESULT hr;

static const Float k_DefaultCameraDistance	= 500.0f;
static const Float mk_DefaultFarClip		= 500.0f;
static const Float mk_DefaultNearClip		= 1.0f;

//////////////////////////////////////////////////////////////////////////

D3D::D3D() :
	m_pVB(NULL),
	m_lpD3dObj(NULL),
	m_lpD3dDevice(NULL),
	m_pBackBuffer(NULL),
	m_lpD3dxSprite(NULL),
	m_fCameraDistance(k_DefaultCameraDistance),
	m_bD3DSceneBegun(false),
	m_bEnableShaders(false),
	m_bSpriteBegun(false),
	m_bSceneBegun(false),
	m_bRenderMap(true),
	m_nWidth(0),
	m_nHeight(0),
	m_fClipNear(0.0f),
	m_fClipFar(1000.0f),
	m_fAspectRatio(0.75f),
	m_fFOV(90),
	m_bSpriteDeviceEnabled(false),
	m_cClearColor(Color(0.0f, 0.0f, 0.0f, 1.0f)),
	m_hFocusWnd(NULL),
	m_hDeviceWnd(NULL),
	m_hDestWindowOverride(NULL),
	m_bLeftHandedCoords(true)
	
{
	m_dwSpriteDeviceFlags = //D3DXSPRITE_DONOTSAVESTATE			|
							D3DXSPRITE_ALPHABLEND				|
							D3DXSPRITE_SORT_TEXTURE				|
							//D3DXSPRITE_DONOTMODIFY_RENDERSTATE	| 
							D3DXSPRITE_SORT_DEPTH_BACKTOFRONT	
							;
}

//////////////////////////////////////////////////////////////////////////

D3D::~D3D(void) 
{
	g_Log << "D3D::~D3D() begin...\n";

	if (!Shutdown())
	{
		g_Log.LOG_ERROR("Failed to shutdown D3D!\n");
	}

	g_Log << "D3D::~D3D() end!\n";
}

//////////////////////////////////////////////////////////////////////////

Int32 D3D::Init(HWND hFocusWnd, UInt32 width /* = 0 */, UInt32 height /* = 0 */, Bool bWindowed /* = true */)
{
	g_Log << "D3D::Init() begin...\n";

	ASSERT(::IsWindow(hFocusWnd));

	m_hFocusWnd = m_hDeviceWnd = hFocusWnd;

	m_bWindowed		= bWindowed;

	if (!CreateD3D())
		return 0;

	if (width == 0)
		width = mk_DefaultWidth;
	if (height == 0)
		height = mk_DefaultHeight;

	SetupPresentParams(width, height);

	if (!CreateDevice())
		return 0;

	if (!CreateSpriteDevice())
		return 0;

	SetInitialRenderStates();

	g_Log << "Testing Pixel Shader version...\n";

	ZeroMemory(&m_HALCaps, sizeof(D3DCAPS9)); // Can't hurt to zero the struct.

	hr = m_lpD3dDevice->GetDeviceCaps(&m_HALCaps);

	if (m_HALCaps.PixelShaderVersion < D3DPS_VERSION(3, 0)) 
	{
		g_Log.LOG_ERROR("Pixel Shader 3.0 not supported!!\n");
	}
	//else
	{
		m_bEnableShaders = true;

		g_Log << "Pixel Shader Version: " 
				<< UInt32(D3DSHADER_VERSION_MAJOR(m_HALCaps.PixelShaderVersion))
				<< "." 
				<< UInt32(D3DSHADER_VERSION_MINOR(m_HALCaps.PixelShaderVersion))
				<< "\n";
	}

	m_fAspectRatio = static_cast<Float>(GetScreenWidth()) / GetScreenHeight();
	
	g_Log << "Setting window aspect ratio. Ratio: " << m_fAspectRatio << "\n";
	
	g_Log << "Initializing camera...\n";
	g_Log << "Resetting camera position...\n";
	ResetCamera();

	g_Log << "Setting perspective matrix...\n";
	SetPerspective();
	g_Log << "Persepctive matrix set!\n";

	g_Log << "Setting world view coordinate system chirality...\n";

	if (m_bLeftHandedCoords)
	{
		g_Log << "Left-Handed coordinate system set!\n";
		GetSpriteObject()->SetWorldViewLH(&matWorld, &matView);
	}
	else
	{
		g_Log << "Right-Handed coordinate system set!\n";
		GetSpriteObject()->SetWorldViewRH(&matWorld, &matView);
	}

	g_Log << "Camera initialized!\n";

	//GetDevice()->GetRenderTarget(0, &m_pBackBuffer);
	CreateRenderSurfaces();

	g_Log << "D3D::Init() end!\n";

	return 1;
}

//////////////////////////////////////////////////////////////////////////

Int32 D3D::Shutdown()
{
	g_Log << "D3D::Shutdown() begin...\n";

	if (m_lpD3dDevice)
	{
		g_Log << "Nullifying texture...\n";
		GetDevice()->SetTexture(0, NULL);

		g_Log << "Nullifying vertex declaration...\n";
		GetDevice()->SetVertexDeclaration(NULL);
	}

	g_FontMgr->DeleteInstance();
	g_EffectMgr->DeleteInstance();

	g_Log << "Deleting sprite manager singleton...\n";
	g_SpriteMgr->DeleteInstance();
	g_Log << "Sprite manager singleton deleted!\n";

	g_Log << "Safely releasing render surface...\n";
	SAFE_RELEASE(m_pMainRender.surface);
	g_Log << "Render surface safely released!\n";
	g_Log << "Safely releasing render texture...\n";
	SAFE_RELEASE(m_pMainRender.texture);
	g_Log << "Render texture safely released!\n";

#ifdef _DOOMEROIDS_
	SAFE_RELEASE(m_pUIRenderSurface);
	SAFE_RELEASE(m_pUIRenderTexture);
#endif

	g_Log << "Safely releasing back buffer...\n";
	SAFE_RELEASE(m_pBackBuffer);
	g_Log << "Back buffer safely released!\n";

	g_Log << "Safely releasing vertex buffer...\n";
	SAFE_RELEASE(m_pVB);
	g_Log << "CVertex buffer safely released!\n";

	g_Log << "Safely releasing Direct3D sprite device...\n";
	SAFE_RELEASE(m_lpD3dxSprite);
	g_Log << "Direct3D sprite device safely released!\n";
	g_Log << "Safely releasing Direct3D device...\n";
	SAFE_RELEASE(m_lpD3dDevice);
	g_Log << "Direct3D device safely released!\n";
	g_Log << "Safely releasing Direct3D interface...\n";
	SAFE_RELEASE(m_lpD3dObj);
	g_Log << "Direct3D interface safely released!\n";

	g_Log << "D3D::Shutdown() end!\n";

	return 1;
}

//////////////////////////////////////////////////////////////////////////

void D3D::SetInitialRenderStates()
{
	g_Log << "D3D::SetInitialRenderStates() begin...\n";

	g_Log << "Initializing render states...\n";

#define SRS(a, b)							m_lpD3dDevice->SetRenderState( a, b )

	SRS(D3DRS_ALPHABLENDENABLE,				TRUE);
	SRS(D3DRS_ALPHAFUNC,					D3DCMP_GREATER);
	SRS(D3DRS_ALPHATESTENABLE,				TRUE);
	SRS(D3DRS_LIGHTING,						FALSE);
	SRS(D3DRS_ZENABLE,						TRUE);
	
	SRS(D3DRS_CULLMODE,						D3DCULL_NONE);
	SRS(D3DRS_DESTBLEND,					D3DBLEND_INVSRCALPHA);
	SRS(D3DRS_SRCBLEND,						D3DBLEND_SRCALPHA);
	//SRS(D3DRS_DESTBLEND,					D3DBLEND_ZERO);
	//SRS(D3DRS_SRCBLEND,						D3DBLEND_ONE);
#undef  SRS

	g_Log << "Render states initialized!\n";

	g_Log << "Initializing texture stage states...\n";

#define STSS(n, a, b)						m_lpD3dDevice->SetTextureStageState(n, a, b)

	STSS(0, D3DTSS_ALPHAARG1,				D3DTA_TEXTURE);
	STSS(0, D3DTSS_ALPHAARG2,				D3DTA_DIFFUSE );
	STSS(0, D3DTSS_ALPHAOP,					D3DTOP_MODULATE );
	STSS(0, D3DTSS_COLORARG2,				D3DTA_DIFFUSE );
	STSS(0, D3DTSS_TEXCOORDINDEX,			0 );
	STSS(0, D3DTSS_TEXTURETRANSFORMFLAGS,	D3DTTFF_DISABLE );
	STSS(1, D3DTSS_ALPHAOP,					D3DTOP_DISABLE );
	STSS(1, D3DTSS_COLOROP,					D3DTOP_DISABLE );

#undef  STSS

	g_Log << "Texture stage states initialized!\n";

	g_Log << "Initializing world matrix...\n";
	D3DXMatrixIdentity(&matWorld);	//Just in case
	g_Log << "World matrix initialized!\n";

	g_Log << "Nullifying texture...\n";
	GetDevice()->SetTexture(0, NULL);
	g_Log << "Nullifying vertex declaration...\n";
	GetDevice()->SetVertexDeclaration(NULL);
	g_Log << "Setting Flexible CVertex Format (FVF)...\n";
	GetDevice()->SetFVF(CVertex::FVF);
	g_Log << "FVF set!\n";

	g_Log << "D3D::SetInitialRenderStates() end!\n";
}

//////////////////////////////////////////////////////////////////////////

Bool D3D::CreateD3D()
{
	g_Log << "Creating Direct3D9 interface...\n";

	m_lpD3dObj = Direct3DCreate9(D3D_SDK_VERSION);

	if (!m_lpD3dObj)
	{
		g_Log.LOG_ERROR("Error creating Direct3D object!\n");

		return false;
	}

	g_Log << "Direct3D9 interface created!\n";

	return true;
}

//////////////////////////////////////////////////////////////////////////

void D3D::SetupPresentParams(Int32 width, Int32 height)
{
	g_Log << "Initializing presentation parameters...\n";

	memset(&m_d3dpp, 0, sizeof(D3DPRESENT_PARAMETERS));

	g_Log << "Backbuffer format: " << ((m_bWindowed ? "D3DFMT_UNKNOWN\n" : "D3DFMT_X8R8G8B8\n"));
	m_d3dpp.BackBufferFormat			= (m_bWindowed ? D3DFMT_UNKNOWN : D3DFMT_X8R8G8B8);
	g_Log << "Swap effect: D3DSWAPEFFECT_DISCARD\n";
	m_d3dpp.SwapEffect					= D3DSWAPEFFECT_DISCARD;
	g_Log << "Flags: D3DPRESENTFLAG_LOCKABLE_BACKBUFFER\n";
	m_d3dpp.Flags						= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	g_Log << "Windowed: " << (m_bWindowed ? "true\n" : "false\n");
	m_d3dpp.Windowed					= m_bWindowed;
	g_Log << "Enable AutoDepthStencil: true\n";
	m_d3dpp.EnableAutoDepthStencil		= true;
	g_Log << "AutoDepthStencil format: D3DFMT_D24S8\n";
	m_d3dpp.AutoDepthStencilFormat		= D3DFMT_D24S8;

	g_Log << "Setting device HWND...\n";
	m_d3dpp.hDeviceWindow				= m_hDeviceWnd;

	g_Log << "Presentation interval: D3DPRESENT_INTERVAL_IMMEDIATE\n";
	m_d3dpp.PresentationInterval		= D3DPRESENT_INTERVAL_ONE;//IMMEDIATE;
	//m_d3dpp.MultiSampleType				= D3DMULTISAMPLE_4_SAMPLES;
	//m_d3dpp.MultiSampleQuality			= 0;
	g_Log << "Backbuffer count: 1\n";
	m_d3dpp.BackBufferCount				= 1;

	g_Log << "Backbuffer width: " << width;
	g_Log << "\nBackbuffer height: " << height << "\n";
	m_d3dpp.BackBufferWidth				= m_nWidth		= width;
	m_d3dpp.BackBufferHeight			= m_nHeight		= height;

	g_Log << "Presentation parameters initialized!\n";
}

//////////////////////////////////////////////////////////////////////////

Bool D3D::CreateDevice()
{
	g_Log << "Creating Direct3D device...\n";

	if (FAILED(hr = m_lpD3dObj->CreateDevice(D3DADAPTER_DEFAULT, 
											DEFAULT_DEVICE_MODE, 
											m_hFocusWnd,
											D3DCREATE_MULTITHREADED |
											DEFAULT_VERTEX_PROC, 
											&m_d3dpp, &m_lpD3dDevice)))
	{
		g_Log.LOG_ERROR_DX_DESC(hr, "Error creating D3D device!\n");
		return false;
	}

	g_Log << "Direct3D device created!\n";

	return true;
}

//////////////////////////////////////////////////////////////////////////

Bool D3D::CreateSpriteDevice()
{
	g_Log << "Creating Direct3D sprite device...\n";

	if (FAILED(hr = D3DXCreateSprite(m_lpD3dDevice, &m_lpD3dxSprite)))
	{
		g_Log.LOG_ERROR_DX_DESC(hr, "Error creating D3D sprite device!\n");
		return false;
	}

	g_Log << "Direct3D sprite device created!\n";

	return true;
}

//////////////////////////////////////////////////////////////////////////

Bool D3D::Reset()
{
	if (FAILED(hr = m_lpD3dxSprite->OnLostDevice()))
	{
		g_Log.LOG_ERROR_DX_DESC(hr, "Failed to check if sprite device lost after mode switch!\n");
	}

	ASSERT(m_d3dpp.hDeviceWindow == NULL || ::IsWindow(m_d3dpp.hDeviceWindow));

	if (FAILS(GetDevice()->Reset(&m_d3dpp)))
	{
		DxErr;
		return false;
	}

	if (FAILED(hr = m_lpD3dxSprite->OnResetDevice()))
	{
		g_Log.LOG_ERROR_DX_DESC(hr, "Failed to reset sprite device!\n");
	}

	SetInitialRenderStates();

	return true;
}

//////////////////////////////////////////////////////////////////////////

Bool D3D::SetDeviceWindow(HWND hDeviceWnd)
{
	ASSERT(hDeviceWnd == NULL || ::IsWindow(hDeviceWnd));
	
	m_d3dpp.hDeviceWindow = m_hDeviceWnd = hDeviceWnd;
	
	return Reset();
}

//////////////////////////////////////////////////////////////////////////

void D3D::SetDestWindowOverride(HWND hDestWnd)
{
	ASSERT(hDestWnd == NULL || ::IsWindow(hDestWnd));
	m_hDestWindowOverride = hDestWnd;
}

//////////////////////////////////////////////////////////////////////////

Bool D3D::IsDeviceWindowValid() const
{
	return (::IsWindow(m_hDeviceWnd) == TRUE);
}

//////////////////////////////////////////////////////////////////////////

Bool D3D::CreateTexture(D3DTexture& texture, UInt32 nWidth, UInt32 nHeight,
						Bool bDepth /* = false */, UInt32 mipLevels /* = 0 */)
{	
	ASSERT(nWidth);
	ASSERT(nHeight);

	if (FAILS(GetDevice()->CreateTexture(	nWidth, nHeight, mipLevels, 
											D3DUSAGE_RENDERTARGET,
											bDepth ? D3DFMT_R32F : D3DFMT_X8R8G8B8,
											D3DPOOL_DEFAULT,
											&texture,
											NULL)))
	{
		DxErr;
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

Bool D3D::CreateRenderTarget(RenderTarget& rTarget, UInt32 nWidth, UInt32 nHeight,
							 Bool bDepth /* = false */, UInt32 mipLevels /* = 1 */)
{
	ASSERT(nWidth);
	ASSERT(nHeight);

	if (!CreateTexture(rTarget.texture, nWidth, nHeight, bDepth, mipLevels))
		return false;

	if (FAILS(rTarget.texture->GetSurfaceLevel(0, &rTarget.surface)))
	{
		DxErr;
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

Bool D3D::CreateRenderSurfaces()
{
	g_Log << "Initializing render surfaces...\n";

	CreateRenderTarget(m_ShadowMap, 512, 512, true);

	g_Log << "Render surfaces initialized!\n";

	return true;
}

//////////////////////////////////////////////////////////////////////////

Bool D3D::CreateVertexBuffer()
{
	HRESULT hr;

	g_Log << "Initializing vertex buffer...\n";

#if 0
	CVertex verts[6] = 
	{
		CVertex(D3DXVECTOR3(-1,	1,	0.00001f),	D3DXVECTOR3(0, 0, 0),	0, 0),
		CVertex(D3DXVECTOR3(1,	1,	0.00001f),	D3DXVECTOR3(0, 0, 0),	1, 0),
		CVertex(D3DXVECTOR3(-1,	-1,	0.00001f),	D3DXVECTOR3(0, 0, 0),	0, 1),

		CVertex(D3DXVECTOR3(-1,	-1,	0.00001f),	D3DXVECTOR3(0, 0, 0),	0, 1),
		CVertex(D3DXVECTOR3(1,	1,	0.00001f),	D3DXVECTOR3(0, 0, 0),	1, 0),
		CVertex(D3DXVECTOR3(1,	-1,	0.00001f),	D3DXVECTOR3(0, 0, 0),	1, 1)
	};
#else
	CVertex verts[6] = 
	{
		CVertex(Vec3(0,	0,	0.00001f),	Vec3(0, 0, 0),	0, 0),
		CVertex(Vec3(1024,	0,	0.00001f),	Vec3(0, 0, 0),	1, 0),
		CVertex(Vec3(0,	768,	0.00001f),	Vec3(0, 0, 0),	0, 1),

		CVertex(Vec3(0,	768,	0.00001f),	Vec3(0, 0, 0),	0, 1),
		CVertex(Vec3(1024,	0,	0.00001f),	Vec3(0, 0, 0),	1, 0),
		CVertex(Vec3(1024,	768,	0.00001f),	Vec3(0, 0, 0),	1, 1)
	};
#endif

	if (FAILED(hr = GetDevice()->CreateVertexBuffer(6 * sizeof(CVertex), 
													D3DUSAGE_WRITEONLY, 
													CVertex::FVF,
													D3DPOOL_DEFAULT,
													&m_pVB, NULL)))
	{
		g_Log.LOG_ERROR_DX(hr);
		return false;
	}

	void* pData;

	if (FAILED(hr = m_pVB->Lock(0, 0, &pData, 0)))
	{
		g_Log.LOG_ERROR_DX(hr);
		return false;
	}

	memcpy(pData, verts, sizeof(CVertex) * 6);

	m_pVB->Unlock();

	GetDevice()->SetFVF(CVertex::FVF);

	g_Log << "CVertex buffer initialized!\n";

	return true;
}

//////////////////////////////////////////////////////////////////////////

Bool D3D::TestCooperativeLevel()
{
	if (m_lpD3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		return Reset();
	
	return true;
}

//////////////////////////////////////////////////////////////////////////

Bool D3D::TestModeSwitch()
{
	//If user has pressed Alt + Enter to toggle fullscreen mode
	if (KEYDOWN(VK_MENU) && KEYDOWN(VK_RETURN))
	{
		if (m_d3dpp.Windowed)
		{
			m_d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
			m_d3dpp.Windowed = m_bWindowed = false;
		}
		else
		{
			m_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
			m_d3dpp.Windowed = m_bWindowed = true;
		}

		return Reset();
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

Bool D3D::BeginScene(Bool bBeginD3DScene /* = false */, Bool bClearBuffer /* = true */)
{	
	PROFILE_THIS;

	if (!TestCooperativeLevel())
		return false;

	//TestModeSwitch();

	//Clear the render texture
	//GetDevice()->SetRenderTarget(0, m_pRenderSurface);
	//Clear();

	m_bSceneBegun = true;

	if (bBeginD3DScene)
		return BeginD3DScene(bClearBuffer);

	if (bClearBuffer)
		Clear(m_cClearColor);

	return true;
}

//////////////////////////////////////////////////////////////////////////

void D3D::Clear(Color color, Bool bZBuffer /* = true */, Bool bRenderTarget /* = true */)
{
	PROFILE_THIS;

	ASSERT(bZBuffer || bRenderTarget);

	Int32 flags = 0;

	if (bZBuffer)
		flags |= D3DCLEAR_ZBUFFER;

	if (bRenderTarget)
		flags |= D3DCLEAR_TARGET;

	if (FAILED(hr = m_lpD3dDevice->Clear(NULL, NULL, flags, color, 1.0f, 0)))
		g_Log.LOG_ERROR_DX_DESC(hr, "Failed to clear the render target!\n");
}

//////////////////////////////////////////////////////////////////////////

Bool D3D::BeginSpriteDevice(Bool bInObjectSpace)
{
	Int32 flags =	m_dwSpriteDeviceFlags;

	if (bInObjectSpace)
		flags |= D3DXSPRITE_OBJECTSPACE;

	if (FAILS(m_lpD3dxSprite->Begin(flags)))
	{
		g_Log.LOG_ERROR_DX_DESC(g_hr, "Error beginning sprite object!\n");
		return false;
	}

	m_bSpriteBegun = true;

	return true;
}

//////////////////////////////////////////////////////////////////////////

Bool D3D::EndSpriteDevice()
{
	HRESULT hr;

	if (FAILED(hr = m_lpD3dxSprite->End()))
	{
		g_Log.LOG_ERROR_DX_DESC(hr, "Unable to end sprite object!\n");
		return false;
	}

	m_bSpriteBegun = false;

	return true;
}

//////////////////////////////////////////////////////////////////////////

Bool D3D::EndScene(Bool bPresent /* = true */)
{
	PROFILE_THIS;

	ASSERT(m_bSceneBegun);

	if (!m_bD3DSceneBegun)
		BeginD3DScene(false);

	if (m_bSpriteDeviceEnabled && !g_SpriteMgr->IsEmpty())
	{
		PROFILE("sprites render");
		RenderSprites();
		RenderUI();
	}
	
	{
		PROFILE("end scene");
	if (FAILED(g_hr = m_lpD3dDevice->EndScene()))
	{
		g_Log.LOG_ERROR_DX_DESC(g_hr, "Unable to end scene!\n");
		return false;
	}
	}

	{
		PROFILE("present");
	if (bPresent)
	{
		if (FAILED(g_hr = m_lpD3dDevice->Present(NULL, NULL, m_hDestWindowOverride, NULL)))
		{
			g_Log.LOG_ERROR_DX_DESC(g_hr, "Unable to present!\n");
			return false;
		}
	}
	}

	m_bSceneBegun = false;
	m_bD3DSceneBegun = false;

	return true;
}

//////////////////////////////////////////////////////////////////////////

Bool D3D::BeginD3DScene(Bool bClearBuffer /* = true */)
{
	PROFILE_THIS;

	if (FAILED(hr = m_lpD3dDevice->BeginScene()))
	{
		g_Log.LOG_ERROR_DX_DESC(hr, "Unable to begin scene!\n");

		m_lpD3dDevice->EndScene();

		m_bSceneBegun = false;
		m_bD3DSceneBegun = false;

		if (hr != D3DERR_INVALIDCALL)
			g_Log.LOG_ERROR_DX_DESC(hr, "Error with IDirect3DDevice::BeginScene() other than D3DERR_INVALIDCALL!\n");

		return false;
	}

	//Clear the screen
	if (bClearBuffer)
		Clear(m_cClearColor);

	m_bD3DSceneBegun = true;

	return true;
}

//////////////////////////////////////////////////////////////////////////

Bool D3D::RenderSprites()
{
	PROFILE_THIS;

	//3DCAM
	BeginSpriteDevice(/*false*/);

	ResetWorldMatrix();
	g_SpriteMgr->RenderSprites();

	EndSpriteDevice();

	return true;
}

//////////////////////////////////////////////////////////////////////////

Bool D3D::RenderUI()
{
	PROFILE_THIS;

	BeginSpriteDevice(false);

	ResetWorldMatrix();
	g_SpriteMgr->RenderUI();

	EndSpriteDevice();

	return true;
}

//////////////////////////////////////////////////////////////////////////
#ifdef _DOOMEROIDS_
void D3D::CenterCameraOn(Entity* ent)
{
	if (ent)
	{
		Float		xcenter = ent->m_vPosition.X, 
					ycenter = ent->m_vPosition.Y;

		vEyePt		= Vec3( xcenter,		ycenter,	300.0f );
		vLookatPt	= Vec3( xcenter,		ycenter,	0.0f );

		D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
		m_lpD3dDevice->SetTransform(D3DTS_VIEW, &matView );
	}
}
#endif

//////////////////////////////////////////////////////////////////////////

void D3D::CenterCameraOn(const Vec2& pos)
{
	Float		xcenter = pos.X, 
				ycenter = pos.Y;

	vEyePt		= Vec3(xcenter,		ycenter,	m_fCameraDistance);
	vLookatPt	= Vec3(xcenter,		ycenter,	0.0f);

	matView.LookAt(vEyePt, vLookatPt, vUpVec, m_bLeftHandedCoords);

	m_lpD3dDevice->SetTransform(D3DTS_VIEW, &matView);
}

//////////////////////////////////////////////////////////////////////////

void D3D::ResetCamera()
{
// 	vEyePt		= Vec3(0.0f,		0.0f,	m_fCameraDistance);
// 	vLookatPt	= Vec3(0.0f,		0.0f,	0.0f);
// 	vUpVec		= Vec3(0.0f,		1.0f,	0.0f);
// 
// 	if (m_bLeftHandedCoords)
// 		D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUpVec);
// 	else
// 		D3DXMatrixLookAtRH(&matView, &vEyePt, &vLookatPt, &vUpVec);
// 
// 	m_lpD3dDevice->SetTransform(D3DTS_VIEW, &matView);

	m_camDefault.SetPosition(Vec3(0.0f,		0.0f,	0.0f/*m_fCameraDistance*/));
	UpdateCamera();
}

//////////////////////////////////////////////////////////////////////////

void D3D::Set2DCameraDistance(Float dist)
{
	vEyePt.z = dist;

	matView.LookAt(vEyePt, vLookatPt, vUpVec, m_bLeftHandedCoords);
	
	m_lpD3dDevice->SetTransform( D3DTS_VIEW, &matView );

	m_fCameraDistance = dist; 
}

//////////////////////////////////////////////////////////////////////////

void D3D::SetLookAt(const Vec3& vec)
{
	//vEyePt		= vec;
// 	vLookatPt	= vec;
// 
// 	if (m_bLeftHandedCoords)
// 		D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUpVec);
// 	else
// 		D3DXMatrixLookAtRH(&matView, &vEyePt, &vLookatPt, &vUpVec);
// 
// 	m_lpD3dDevice->SetTransform( D3DTS_VIEW, &matView );
	m_camDefault.SetTarget(vec);
}

//////////////////////////////////////////////////////////////////////////

void D3D::SetEyePt(const Vec3& vec)
{
	vEyePt = vec;

	matView.LookAt(vEyePt, vLookatPt, vUpVec, m_bLeftHandedCoords);

	m_lpD3dDevice->SetTransform( D3DTS_VIEW, &matView );
}

//////////////////////////////////////////////////////////////////////////

void D3D::MoveCameraForward(Float dz)
{
	m_camDefault.MoveForward(dz);
}

//////////////////////////////////////////////////////////////////////////

void D3D::MoveCameraUp(Float dy)
{
	m_camDefault.MoveUp(dy);
}

//////////////////////////////////////////////////////////////////////////

void D3D::MoveCameraRight(Float dx)
{
	m_camDefault.MoveRight(dx);
}

//////////////////////////////////////////////////////////////////////////

void D3D::RotateCameraX(Float deg)
{
	m_camDefault.RotateX(deg);
}

//////////////////////////////////////////////////////////////////////////

void D3D::RotateCameraY(Float deg)
{
	m_camDefault.RotateY(deg);
}

//////////////////////////////////////////////////////////////////////////

void D3D::RotateCameraZ(Float deg)
{
	m_camDefault.RotateZ(deg);
}

//////////////////////////////////////////////////////////////////////////

void D3D::RotateWorldX(Float deg)
{
	D3DXMATRIX mat;
	D3DXQUATERNION rot;

	D3DXMatrixRotationX(&mat, deg * Deg2Rad);
	D3DXQuaternionRotationMatrix(&rot, &mat);
	D3DXMatrixAffineTransformation(&mat, 1.0f, &vLookatPt, &rot, NULL);
	matWorld *= mat;

	m_lpD3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
}

//////////////////////////////////////////////////////////////////////////

void D3D::RotateWorldY(Float deg)
{
	D3DXMATRIX mat;
	D3DXQUATERNION rot;

	D3DXMatrixRotationY(&mat, deg * Deg2Rad);
	D3DXQuaternionRotationMatrix(&rot, &mat);
	D3DXMatrixAffineTransformation(&mat, 1.0f, &vLookatPt, &rot, NULL);
	
	matWorld *= mat;

	m_lpD3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
}

//////////////////////////////////////////////////////////////////////////

void D3D::RotateWorldZ(Float deg)
{
	D3DXMATRIX mat;
	D3DXQUATERNION rot;

	D3DXMatrixRotationZ(&mat, deg * Deg2Rad);
	D3DXQuaternionRotationMatrix(&rot, &mat);
	D3DXMatrixAffineTransformation(&mat, 1.0f, &vLookatPt, &rot, NULL);
	matWorld *= mat;

	m_lpD3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
}

//////////////////////////////////////////////////////////////////////////

void D3D::SetFOV(Float fov)
{
	m_fFOV = Clamp(fov, 0.0f, 360.0f);

	matProj.PerspectiveFOV(m_fFOV * Deg2Rad, m_fAspectRatio, m_fClipNear, m_fClipFar, m_bLeftHandedCoords);

	GetDevice()->SetTransform(D3DTS_PROJECTION, &matProj);
}

//////////////////////////////////////////////////////////////////////////

void D3D::SetPerspective(Bool bOrtho /* = false */, Float fWidth /* = 1.0f */, Float fHeight /* = 1.0f */,
													Float fNear /* = 0.0f */, Float fFar /* = 1.0f */)
{
	m_bOrthoPerspective = bOrtho;

	if (bOrtho)
		matProj.Ortho(fWidth, fHeight, fNear, fFar, m_bLeftHandedCoords);
	else
		matProj.Perspective(fWidth, fHeight, m_fClipNear, m_fClipFar, m_bLeftHandedCoords);

	GetDevice()->SetTransform(D3DTS_PROJECTION, &matProj);
}

//////////////////////////////////////////////////////////////////////////

void D3D::SetPerspective(const CMatrix& mat)
{
	matProj = mat;
	GetDevice()->SetTransform(D3DTS_PROJECTION, &mat);
}

//////////////////////////////////////////////////////////////////////////

void D3D::SetOrthoDisplay(Float width /* = 1.0f */, Float height /* = 1.0f */, Float zNear /* = 0.0f */, Float zFar /* = 1.0f */)
{
	matProj.Ortho(width, height, zNear, zFar, m_bLeftHandedCoords);
	GetDevice()->SetTransform(D3DTS_PROJECTION, &matProj);
}

//////////////////////////////////////////////////////////////////////////

CMatrix D3D::CreateOrthoMatrix(Float width /* = 1.0f */, Float height /* = 1.0f */, Float zNear /* = 0.0f */, Float zFar /* = 1.0f */)
{
	CMatrix matP;
	matP.Ortho(width, height, zNear, zFar, m_bLeftHandedCoords);
	return matP;
}

//////////////////////////////////////////////////////////////////////////


void D3D::SetClipRange(Float fFar /* = mk_DefaultFarClip */, 
					   Float fNear /* = mk_DefaultNearClip */,
					   Bool bOrtho /* = false*/)
{
	//Just to be safe, take the higher number to be the far clip
	m_fClipFar = Max(fFar, fNear);
	m_fClipNear = Min(fFar, fNear);

	SetPerspective(bOrtho);
}

//////////////////////////////////////////////////////////////////////////

void D3D::SetCamera(const Vec3& vPos, const Vec3& vForward, const Vec3& vUp)
{
	m_camDefault.Set(&vPos, &vForward, &vUp);
}

//////////////////////////////////////////////////////////////////////////

void D3D::SetCamera(const CCameraView& camera)
{
	m_camDefault = camera;
}

//////////////////////////////////////////////////////////////////////////

void D3D::SetCameraPos(const Vec3 &vPos)
{
	m_camDefault.SetPosition(vPos);
}

//////////////////////////////////////////////////////////////////////////

void D3D::UpdateCamera()
{
	//m_camDefault.Update();

	if (m_camDefault.HasTarget())
		matView.LookAt(m_camDefault.GetPosition(), m_camDefault.GetTarget(), m_camDefault.GetUp(), m_bLeftHandedCoords);
	else
		matView.LookAt(m_camDefault.GetPosition(), m_camDefault.GetForward() + m_camDefault.GetPosition(), m_camDefault.GetUp(), m_bLeftHandedCoords);
	
	m_lpD3dDevice->SetTransform( D3DTS_VIEW, &matView );
}

//////////////////////////////////////////////////////////////////////////

void D3D::SetRenderTargetToBackBuffer()
{
	ASSERT(m_pBackBuffer);
	GetDevice()->SetRenderTarget(0, m_pBackBuffer);
}

//////////////////////////////////////////////////////////////////////////

Bool D3D::SetRenderTarget(D3DSurface surface, UInt32 slot /* = 0 */)
{
	ASSERT(slot < 4);

	if (slot == 0)
		ASSERT(surface);

	return SUCCEEDED(GetDevice()->SetRenderTarget(slot, surface));
}

//////////////////////////////////////////////////////////////////////////

Bool D3D::PushRenderTarget(D3DSurface surface)
{
	ASSERT(surface);

	ASSERT(m_pBackBuffer == NULL);

	GetDevice()->GetRenderTarget(0, &m_pBackBuffer);

	ASSERT(m_pBackBuffer);

	return SetRenderTarget(surface);
}

//////////////////////////////////////////////////////////////////////////

Bool D3D::PopRenderTarget(Bool bAssertEmpty /* = true */)
{
	if (bAssertEmpty)
		ASSERT(m_pBackBuffer);
	else if (m_pBackBuffer == NULL)
		return true;

	Bool ret = SetRenderTarget(m_pBackBuffer);
	m_pBackBuffer = NULL;
	return ret;
}