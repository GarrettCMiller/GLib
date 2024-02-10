///////////////////////////////////////////////////////////////////////////////
//		File:				D3DWrapper.h
//
//		Programmer:			Garrett Miller (GM)
//
//		Date created:		August 10, 2004
//
//		Description:		A Direct3D wrapper class with sprite functionality
///////////////////////////////////////////////////////////////////////////////
#pragma once

//#include "stdafx.h"

#include "GameMacros.h"

//#include "SpriteMgr.h"
#include "Singleton.h"
#include "Vec.h"
#include "Matrix.h"

#include <windows.h>
#include <time.h>
#include <fstream>

#include "d3d9.h"
#include "dxerr.h"
#include "d3dx9core.h"

#include "Palette.h"

#include "Vertex.h"

#include "CameraView.h"

//#include 

//////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
	#pragma comment (lib, "d3dx9d")
	//#define DEBUG_SHADERS
#else
	#pragma comment (lib, "d3dx9")
#endif

//For the sprite device
#define USE_OBJECT_SPACE

#ifdef DEBUG_SHADERS
	#define DEFAULT_VERTEX_PROC			D3DCREATE_SOFTWARE_VERTEXPROCESSING
	#define	DEFAULT_DEVICE_MODE			D3DDEVTYPE_REF
	#define DEFAULT_SHADER_FLAGS		D3DXSHADER_DEBUG | D3DXSHADER_SKIPOPTIMIZATION | D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY
#else
	#define	DEFAULT_VERTEX_PROC			D3DCREATE_HARDWARE_VERTEXPROCESSING
	#define	DEFAULT_DEVICE_MODE			D3DDEVTYPE_HAL
	#define DEFAULT_SHADER_FLAGS		D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY
#endif

//////////////////////////////////////////////////////////////////////////

#pragma comment (lib, "d3d9")
#pragma comment (lib, "dxerr")
#pragma comment (lib, "dxguid")
#pragma comment (lib, "winmm")

//////////////////////////////////////////////////////////////////////////

class CBaseVariable;
struct BaseRenderRequest;

typedef CDynamicArray<CMatrix>		TMatrixArray;
typedef CDynamicArray<CVertex>		TVertexArray;

typedef std::pair<D3DXHANDLE, CBaseVariable*>	TShaderVar;

typedef CDynamicArray<TShaderVar>	TShaderVarArray;

typedef CDynamicArray<BaseRenderRequest*>	TRenderQueue;

//typedef std::pair<D3DTexture, D3DSurface>	TRenderTarget;
class /*_GLIB_DLL_API_*/ RenderTarget
{
public:
	RenderTarget() : texture(NULL), surface(NULL)
	{	}

	D3DTexture	texture;
	D3DSurface	surface;
};

class /*_GLIB_DLL_API_*/ BaseRenderRequest
{
public:
	enum ERenderType
	{
		eRT_Shadow,
		eRT_Final,

		eRT_NumTypes
	};

	static ERenderType mk_eRenderType;

	BaseRenderRequest() : effectID(static_cast<UInt32>(-1)), effectTech(""), castShadow(false)
	{
	}

	BaseRenderRequest(CMatrix mWorld, 
						UInt32 fxID = static_cast<UInt32>(-1), D3DXHANDLE fxTech = NULL, const TShaderVarArray* fxVars = NULL)
						: matWorld(mWorld),
							effectID(fxID), effectTech(fxTech), castShadow(false)
	{
		if (fxVars)
			effectVars = *fxVars;
	}
	
	CMatrix				matWorld;

	UInt32				effectID;		//Default is -1, i.e., no effect
	D3DXHANDLE			effectTech;
	TShaderVarArray		effectVars;

	Bool				castShadow;

	////////////////////////////////
	virtual void PreRender();
	virtual void Render();
	virtual void Draw() = 0;
	virtual void Cleanup()	{	}
};


class Entity;

//////////////////////////////////////////////////////////////////////////

class /*_GLIB_DLL_API_*/ D3D : public Singleton<D3D>
{
public:
							D3D	();
	virtual					~D3D(void);

	CMatrix					matWorld;
	CMatrix					matView;
	CMatrix					matProj;

	D3DMatrix				GetViewProj() const
	{
		return matView * matProj;
	}

	Bool					m_bRenderMap;

private:
	LPDIRECT3D9				m_lpD3dObj;
	LPDIRECT3DDEVICE9		m_lpD3dDevice;
	LPD3DXSPRITE			m_lpD3dxSprite;
	D3DPRESENT_PARAMETERS	m_d3dpp;

	RenderTarget			m_pMainRender;
	RenderTarget			m_pUIRender;
	D3DSurface				m_pBackBuffer;

	D3DVertexBuffer			m_pVB;

	Color					m_cClearColor;

	D3DCAPS9				m_HALCaps;

	Vec3					vEyePt;
	Vec3					vLookatPt;
	Vec3					vUpVec;

	Float					m_fClipNear;
	Float					m_fClipFar;
	Float					m_fAspectRatio;
	Float					m_fFOV;

	HWND					m_hFocusWnd;
	HWND					m_hDeviceWnd;
	HWND					m_hDestWindowOverride;

	Float					m_fCameraDistance;

	DWORD					m_dwStoredRenderState;

	Int32					m_dwSpriteDeviceFlags;

	Int32					m_nWidth;
	Int32					m_nHeight;
	
	Bool					m_bWindowed;
	
	Bool					m_bSpriteDeviceEnabled;
	Bool					m_bSpriteBegun;
	Bool					m_bSceneBegun;
	Bool					m_bD3DSceneBegun;

	Bool					m_bEnableShaders;

	Bool					m_bLeftHandedCoords;

	Bool					m_bOrthoPerspective;

	CCameraView				m_camDefault;

	//Shadow map
	RenderTarget			m_ShadowMap;

	TRenderQueue			m_arrRenderQueue;

	static const Float		mk_DefaultFarClip;
	static const Float		mk_DefaultNearClip;
	static const UInt32		mk_DefaultWidth		= 1024;
	static const UInt32		mk_DefaultHeight	= 768;
public:
	void PushRenderRequest(BaseRenderRequest* pRequest)
	{
		ASSERT(pRequest);
		m_arrRenderQueue.Push(pRequest);
	}

	Int32 ProcessRenderRequests()
	{
		if (m_arrRenderQueue.IsEmpty())
			return 1;

		BaseRenderRequest* pRequest = NULL;
		
		for (UInt32 renderPass = 1; renderPass < BaseRenderRequest::eRT_NumTypes; renderPass++)
		{
			BaseRenderRequest::mk_eRenderType = static_cast<BaseRenderRequest::ERenderType>(renderPass);

			switch (renderPass)
			{
			case BaseRenderRequest::eRT_Shadow:
				PushRenderTarget(m_ShadowMap.surface);
				BeginScene(true, false);
				//Clear(Color::km_Black);
				break;

			case BaseRenderRequest::eRT_Final:
				PopRenderTarget(false);
				BeginScene(true);
				break;
			}

			if (renderPass > 0)
			{
			for (UInt32 req = 0; req < m_arrRenderQueue.Size(); req++)
			{
				pRequest = m_arrRenderQueue[req];

				if (renderPass == 0 && !pRequest->castShadow)
					continue;

				//pRequest->PreRender();
				pRequest->Render();
			}
			}

			EndScene(renderPass == 1);
		}

		for (UInt32 req = 0; req < m_arrRenderQueue.Size(); req++)
			m_arrRenderQueue[req]->Cleanup();

		m_arrRenderQueue.FreePointers();

		return 0;
	}

	RenderTarget& GetShadowMap()
	{
		return m_ShadowMap;
	}

	//////////////////////////////////////////////////////////////////////////
	void PushRenderState(D3DRENDERSTATETYPE state, DWORD val)
	{
		GetDevice()->GetRenderState(state, &m_dwStoredRenderState);
		GetDevice()->SetRenderState(state, val);
	}

	void PopRenderState(D3DRENDERSTATETYPE state)
	{
		GetDevice()->SetRenderState(state, m_dwStoredRenderState);
	}

	void SetWorldMatrix(const CMatrix& mat)
	{	
		matWorld = mat;
		m_lpD3dDevice->SetTransform(D3DTS_WORLD, &mat);
	}

	void ResetWorldMatrix()
	{
		matWorld.Identity();
		SetWorldMatrix(matWorld);
	}

	LPDIRECT3DDEVICE9		GetDevice()				{ return m_lpD3dDevice; }
	LPD3DXSPRITE			GetSpriteObject() 		{ return m_lpD3dxSprite;}

	Float					GetAspectRatio() const	{ return m_fAspectRatio;	}

	Bool					Reset();

	D3DPRESENT_PARAMETERS	GetPresentParams()const	{ return m_d3dpp;	}
	D3DPRESENT_PARAMETERS&	GetPresentParams()		{ return m_d3dpp;	}

	UInt32					GetScreenWidth() const	{ return m_nWidth; }
	UInt32					GetScreenHeight() const	{ return m_nHeight;}

	Bool					IsWindowed() const		{ return m_bWindowed;}

	D3DTexture				GetRenderTexture() 		{ return m_pMainRender.texture; }
	D3DTexture				GetUIRenderTexture() 	{ return m_pUIRender.texture; }

	D3DSurface				GetRenderSurface() 		{ return m_pMainRender.surface; }
	D3DSurface				GetUIRenderSurface() 	{ return m_pUIRender.surface; }
	D3DSurface				GetBackBuffer()			{ return m_pBackBuffer; }

	//////////////////////////////////////////////////////////////////////////
	
	void EnableLighting(Bool bEnable = true)
	{
		GetDevice()->SetRenderState(D3DRS_LIGHTING, bEnable ? TRUE : FALSE);
	}

	//////////////////////////////////////////////////////////////////////////

	Bool	CreateTexture(D3DTexture& texture, UInt32 nWidth, UInt32 nHeight, Bool bDepth = false, UInt32 mipLevels = 0);
	Bool	CreateRenderTarget(RenderTarget& rTarget, UInt32 nWidth, UInt32 nHeight, Bool bDepth = false, UInt32 mipLevels = 1);

	//////////////////////////////////////////////////////////////////////////

	Bool	SetRenderTarget(D3DSurface surface, UInt32 slot = 0);

	Bool	PushRenderTarget(D3DSurface surface);
	Bool	PopRenderTarget(Bool bAssertEmpty = true);

	/////////////////////////////////////////////////////////////////////////////////

	Float	Get2DCameraDistance() const				{ return m_fCameraDistance; }
	void	Set2DCameraDistance(Float dist);

	Bool	SetDeviceWindow(HWND hDeviceWnd);
	void	SetDestWindowOverride(HWND hDestWnd);

	Int32	Init(HWND hFocusWnd, UInt32 width = 0, UInt32 height = 0, Bool bWindowed = true);
	Int32	Shutdown();

	void	SetClearColor(Color color)				{ m_cClearColor = color;	}
	Color	GetClearColor()		const				{ return m_cClearColor;		}

	Bool	TestCooperativeLevel();
	Bool	TestModeSwitch();

	Bool	IsDeviceWindowValid() const;

	void	Clear(Color color, Bool bZBuffer = true, Bool bRenderTarget = true);

	Bool	AllowShaders() 							{ return m_bEnableShaders; }

	Bool	BeginScene(Bool bBeginD3DScene = false, Bool bClearBuffer = true);
	Bool	EndScene(Bool bPresent = true);

	Bool	BeginSpriteDevice(Bool bInObjectSpace = true);
	Bool	EndSpriteDevice();

	//void	CenterCameraOn(Entity* ent);
	void	CenterCameraOn(const Vec2& pos);
	void	ResetCamera();

	Bool	SpriteDeviceEnabled() const				{ return m_bSpriteDeviceEnabled; }
	void	EnableSpriteDevice(Bool bEnable = true)	{ m_bSpriteDeviceEnabled = bEnable; }

	Int32	GetSpriteDeviceFlags() const			{ return m_dwSpriteDeviceFlags; }
	void	SetSpriteDeviceFlags(Int32 val)			{ m_dwSpriteDeviceFlags = val; }

	void	SetLookAt(const Vec3& vec);
	void	SetEyePt(const Vec3& vec);
	void	SetUp(const Vec3& vec);

	void	SetCamera(const Vec3& vPos, const Vec3& vForward, const Vec3& vUp);
	void	SetCamera(const CCameraView& camera);
	void	SetCameraPos(const Vec3& vPos);

	const	CCameraView& GetCamera() const			{ return m_camDefault; }

	void	UpdateCamera();

	void	SetClipRange(Float fFar = mk_DefaultFarClip, Float fNear = mk_DefaultNearClip, Bool bOrtho = false);

	void	SetOrthoDisplay(Float width = 1.0f, Float height = 1.0f, Float zNear = 0.0f, Float zFar = 1.0f);
	CMatrix	CreateOrthoMatrix(Float width = 1.0f, Float height = 1.0f, Float zNear = 0.0f, Float zFar = 1.0f);

	Vec3	GetLookAt() const						{ return vLookatPt; }
	Vec3	GetEyePt() const						{ return vEyePt; }

	void	MoveCameraForward(Float dz);
	void	MoveCameraUp(Float dy);
	void	MoveCameraRight(Float dx);

	void	RotateCameraX(Float deg);
	void	RotateCameraY(Float deg);
	void	RotateCameraZ(Float deg);

	void	RotateWorldX(Float deg);
	void	RotateWorldY(Float deg);
	void	RotateWorldZ(Float deg);

	void	SetInitialRenderStates();

	void	SetRenderTargetToBackBuffer();

	void	SetPerspective(Bool bOrtho = false, Float fWidth = 1.0f, Float fHeight = 1.0f, Float fNear = 0.0f, Float fFar = 1.0f);
	void	SetPerspective(const CMatrix& mat);
	
	void	SetFOV(Float fov);
	Float	GetFOV() const							{ return m_fFOV; }

protected:	//virtual overrides that must be defined for derived classes
	virtual	Int32	OnInit()					{	return 0;	}
	virtual	Int32	OnShutdown()				{	return 0;	}
	virtual	Int32	OnUpdate()					{	return 0;	}
	virtual	Int32	OnRender()					{	return 0;	}
	virtual	Int32	OnMsg()						{	return 0;	}
	virtual	Int32	OnInitDeviceObjects()		{	return 0;	}
	virtual	Int32	OnRestoreDeviceObjects()	{	return 0;	}
	virtual	Int32	OnInvalidateDeviceObjects()	{	return 0;	}
	virtual	Int32	OnDestroyDeviceObjects()	{	return 0;	}
	
private:
	Bool	CreateD3D();
	Bool	CreateDevice();
	Bool	CreateSpriteDevice();

	Bool	BeginD3DScene(Bool bClearBuffer = true);

	Bool	CreateRenderSurfaces();
	Bool	CreateVertexBuffer();

	Bool	RenderSprites();
	Bool	RenderUI();

	void	SetupPresentParams(Int32 width, Int32 height);
};

//////////////////////////////////////////////////////////////////////////

#define g_D3D		D3D::GetInstance()