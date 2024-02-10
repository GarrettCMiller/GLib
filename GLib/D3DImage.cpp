#include "StdAfx.h"
#include "D3DImage.h"

#include "SpriteMgr.h"

//////////////////////////////////////////////////////////////////////////

CD3DImage::CD3DImage(void) :	
			m_pVertexBuffer(NULL),	m_pIndexBuffer(NULL),
			m_pTexture(NULL),		m_pSurface(NULL),
			m_bVBLocked(false),		m_bIBLocked(false)
{
}

CD3DImage::~CD3DImage(void)
{
	ReleaseAll();
}

//////////////////////////////////////////////////////////////////////////

void CD3DImage::ReleaseAll()
{
	m_Image.Destroy();

	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pTexture);
	SAFE_RELEASE(m_pSurface);

	 m_bVBLocked = false;
	 m_bIBLocked = false;
}

//////////////////////////////////////////////////////////////////////////

Int32 CD3DImage::Init(Vec2u imgSize, const String& initImage /* = "" */)
{
	//To be safe
	ReleaseAll();

	m_vImageSize = imgSize;

	ASSERT(m_vImageSize.X > 0); 
	ASSERT(m_vImageSize.Y > 0);

	if (CreateSurface())
		return -1;

	if (CreateBuffers())
		return -2;

	if (InitVertices())
		return -3;

	if (!initImage.empty())
		if (LoadTexture(initImage))
			return -4;

	if (CreateImageObject())
		return -5;

	return 0;
}

//////////////////////////////////////////////////////////////////////////

Int32 CD3DImage::Reset()
{
	Fill(Color::km_Black);

	return InitVertices();
}

//////////////////////////////////////////////////////////////////////////

Int32 CD3DImage::CreateSurface()
{
	if (FAILS(g_D3D->GetDevice()->
		CreateTexture(	m_vImageSize.X,
						m_vImageSize.Y,
						1, 
						D3DUSAGE_RENDERTARGET,
						D3DFMT_A8R8G8B8,
						D3DPOOL_DEFAULT,
						&m_pTexture,
						NULL)))
	{
		DxErr;
		return -1;
	}

	if (FAILS(m_pTexture->GetSurfaceLevel(0, &m_pSurface)))
	{
		DxErr;
		return -2;
	}

	Fill(Color::km_Black);

	return 0;
}

//////////////////////////////////////////////////////////////////////////

void CD3DImage::Fill(Color color)
{
	g_D3D->PushRenderTarget(m_pSurface);
	
	g_D3D->Clear(color);
	
	g_D3D->PopRenderTarget();
}

//////////////////////////////////////////////////////////////////////////

Int32 CD3DImage::CreateBuffers()
{
	if (FAILS(g_D3D->GetDevice()->
		CreateVertexBuffer(	4 * sizeof(CVertex), 
							D3DUSAGE_WRITEONLY, 
							CVertex::FVF, 
							D3DPOOL_DEFAULT, 
							&m_pVertexBuffer, 
							NULL)))
	{
		DxErr;
		return -3;
	}

	if (FAILS(g_D3D->GetDevice()->
		CreateIndexBuffer(	6 * sizeof(WORD), 
							D3DUSAGE_WRITEONLY, 
							D3DFMT_INDEX16, 
							D3DPOOL_DEFAULT, 
							&m_pIndexBuffer, 
							NULL)))
	{
		DxErr;
		return -4;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////

Int32 CD3DImage::InitVertices()
{
	CVertex* pV = NULL;
	UInt16* pI = NULL;

	m_Vertices.Resize(4);

	for (UInt32 i = 0; i < 4; i++)
		m_Vertices[i] = gk_RectVerts[i];

	if (FAILS(Lock(&pV, &pI)))
	{
		DxErr;
		return -1;
	}

	ASSERT(pV);
	ASSERT(pI);

	memcpy(pV, &m_Vertices[0], sizeof(CVertex) * 4);
	memcpy(pI, gk_RectIndices, sizeof(UInt16) * 6);

	Unlock();

	return 0;
}

//////////////////////////////////////////////////////////////////////////

void CD3DImage::InitPalette(UInt32 size /* = 0 */, UInt32 offset /* = 0 */)
{
	if (size == 0)
		size = 1024;

	m_Palette.Init(size, offset);
}

//////////////////////////////////////////////////////////////////////////

Int32 CD3DImage::LoadTexture(const String& path)
{
	//TEMP HACK
	//ASSERT(CMRCM::m_pBackBuffer);

	if (FAILS(g_D3D->PushRenderTarget(m_pSurface)))
	{
		DxErr;
		return -7;
	}

	g_D3D->Clear(Color::km_Transparent);

	D3DTexture texture = g_SpriteMgr->GetTexture(g_SpriteMgr->LoadSprite(path));
	ASSERT(texture);

	if (g_D3D->BeginScene(true))
	{
		g_D3D->GetDevice()->SetTexture(0, texture);

		g_D3D->GetDevice()->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(CVertex));
		g_D3D->GetDevice()->SetIndices(m_pIndexBuffer);

		//ASSERT(0);

		if (FAILS(g_D3D->GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2)))
		{
			DxErr;
		}
	}
	else
		ASSERT(0);

	g_D3D->PopRenderTarget();

	g_D3D->EndScene();

	return 0;
}

//////////////////////////////////////////////////////////////////////////

Int32 CD3DImage::CreateImageObject()
{
	m_Image.Destroy();
	return (m_Image.Create(m_vImageSize.X, -(static_cast<Int32>(m_vImageSize.Y)), 24) == 0);
}

//////////////////////////////////////////////////////////////////////////

HRESULT CD3DImage::Lock(CVertex** pV, UInt16** pI /* = NULL */)
{
	ASSERT(pV || pI);

	if (pV)
	{
		g_hr = m_pVertexBuffer->Lock(0, 4 * sizeof(CVertex), (void **) pV, 0);
		if (FAILED(m_bVBLocked = SUCCEEDED(g_hr)))	return g_hr;
	}

	if (pI)
	{
		g_hr = m_pIndexBuffer->Lock(0, 6 * sizeof(UInt16), (void**) pI, 0);
		m_bIBLocked = SUCCEEDED(g_hr);
	}

	return g_hr;
}

//////////////////////////////////////////////////////////////////////////

void CD3DImage::Unlock(bool bVB /* = true */, HRESULT* vb /* = NULL */, 
						  bool bIB /* = true */, HRESULT* ib /* = NULL */)
{
	HRESULT temp;
	if (bVB && m_bVBLocked)
	{
		temp = m_pVertexBuffer->Unlock();
		if (vb) *vb = temp;
		m_bVBLocked = !SUCCEEDED(temp);
	}
	if (bIB && m_bIBLocked)
	{
		temp = m_pIndexBuffer->Unlock();
		if (ib) *ib = temp;
		m_bIBLocked = !SUCCEEDED(temp);
	}
}

//////////////////////////////////////////////////////////////////////////

CD3DImage& CD3DImage::operator = (const CD3DImage& rhs)
{
	Init(rhs.m_vImageSize);

	m_Vertices.CopyFrom(rhs.m_Vertices);

	m_bIBLocked		= false;
	m_bVBLocked		= false;

	return *this;
}