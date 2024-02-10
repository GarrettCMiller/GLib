#include "StdAfx.h"
#include "Mesh3D.h"

#include "D3DWrapper.h"

//////////////////////////////////////////////////////////////////////////

CMesh3D::CMesh3D(Bool bAutoRelease /* = true */) : m_bReleaseOnDestruct(bAutoRelease)
{
	//m_RenderRequest = MeshRenderRequest(;
}

CMesh3D::~CMesh3D()
{
	if (m_bReleaseOnDestruct)
		Release();
}

//////////////////////////////////////////////////////////////////////////

Bool CMesh3D::Clone(DWORD Options, D3DMesh* ppCloneMesh,
				UInt32 FVF /* = CVertex::FVF */)
{
	if (FAILS(m_pMesh->CloneMeshFVF(Options, FVF, g_D3D->GetDevice(), ppCloneMesh)))
	{
		g_Log.LOG_ERROR_DX_DESC(g_hr, "Unable to clone mesh!\n");
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
	
Bool CMesh3D::Draw(UInt32 subset /* = 0 */)
{
	if (FAILS(m_pMesh->DrawSubset(subset)))
	{
		g_Log.LOG_ERROR_DX_DESC(g_hr, "Unable to draw!\n");
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

UInt32 CMesh3D::GetNumFaces() const
{
	return m_pMesh->GetNumFaces();
}

//////////////////////////////////////////////////////////////////////////

UInt32 CMesh3D::GetNumVerts() const
{
	return m_pMesh->GetNumVertices();
}

//////////////////////////////////////////////////////////////////////////

Bool CMesh3D::Lock(	Bool bIBuffer /* = true */, UInt32 nIFlags /* = 0 */, LPVOID* ppIData /* = NULL */,
					Bool bVBuffer /* = true */, UInt32 nVFlags /* = 0 */, LPVOID* ppVData /* = NULL */)
{
	if (bIBuffer)
	{
		if (FAILS(m_pMesh->LockIndexBuffer(nIFlags, ppIData)))
		{
			g_Log.LOG_ERROR_DX_DESC(g_hr, "Unable to lock index buffer!\n");
			return false;
		}
	}

	if (bVBuffer)
	{
		if (FAILS(m_pMesh->LockVertexBuffer(nVFlags, ppVData)))
		{
			g_Log.LOG_ERROR_DX_DESC(g_hr, "Unable to lock vertex buffer!\n");
			return false;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
	
UInt32 CMesh3D::Unlock(Bool bIBuffer /* = true */, Bool bVBuffer /* = true */)
{
	UInt32 errorCount = 0;

	if (bIBuffer)
	{
		if (FAILS(m_pMesh->UnlockIndexBuffer()))
		{
			g_Log.LOG_ERROR_DX_DESC(g_hr, "Unable to unlock index buffer!\n");
			errorCount++;
		}
	}

	if (bVBuffer)
	{
		if (FAILS(m_pMesh->UnlockVertexBuffer()))
		{
			g_Log.LOG_ERROR_DX_DESC(g_hr, "Unable to unlock vertex buffer!\n");
			errorCount++;
		}
	}

	return errorCount;
}

Bool CMesh3D::Release()
{
	SAFE_RELEASE(m_pMesh);

	return true;
}

//////////////////////////////////////////////////////////////////////////

Bool CMesh3D::CreateBox(Float fWidth /* = 1.0f */, Float fHeight /* = 1.0f */, Float fDepth /* = 1.0f */)
{
	if (FAILS(D3DXCreateBox(g_D3D->GetDevice(), fWidth, fHeight, fDepth, &m_pMesh, NULL)))
	{
		g_Log.LOG_ERROR_DX_DESC(g_hr, "Couldn't create box mesh!\n");
		return false;
	}

	D3DMesh tMesh;
	CVertex* pVerts;

	if (!(Clone(D3DXMESH_SYSTEMMEM, &tMesh)))
		return false;

	m_pMesh->Release();

	if (SUCCEEDED(tMesh->LockVertexBuffer(0, (LPVOID *)&pVerts)))
	{
		UInt32 numVerts = tMesh->GetNumVertices();

		for (UInt32 i = 0; i < numVerts; i++, pVerts++)
		{
			//Texture coords
			Float dir = 0.0f;
			dir = atan2f(pVerts->normal.z, pVerts->normal.x) * Rad2Deg;
			//pVerts->tu	= asinf(pVerts->normal.x) / (2 * PI) + 0.5f;
			pVerts->tu	= dir / 360.0f;
			pVerts->tv	= 1.0f - asinf(pVerts->normal.y) / PI + 0.5f;

			//Color
			pVerts->color = Color::km_White;
		}

		tMesh->UnlockVertexBuffer();
	}

	m_pMesh = tMesh;

	return true;
}

//////////////////////////////////////////////////////////////////////////

Bool CMesh3D::CreateCylinder(Float fRadius1 /* = 1.0f */, Float fRadius2 /* = 1.0f */, Float fLength /* = 1.0f */, UInt32 nStacks /* = 1 */, UInt32 nSlices /* = 3 */)
{
	if (FAILS(D3DXCreateCylinder(g_D3D->GetDevice(), fRadius1, fRadius2, fLength, nSlices, nStacks, &m_pMesh, NULL)))
	{
		g_Log.LOG_ERROR_DX_DESC(g_hr, "Couldn't create cylinder mesh!\n");
		return false;
	}

	D3DMesh tMesh;
	CVertex* pVerts;

	if (!(Clone(D3DXMESH_SYSTEMMEM, &tMesh)))
		return false;

	m_pMesh->Release();

	if (SUCCEEDED(tMesh->LockVertexBuffer(0, (LPVOID *)&pVerts)))
	{
		UInt32 numVerts = tMesh->GetNumVertices();

		for (UInt32 i = 0; i < numVerts; i++, pVerts++)
		{
			//Texture coords
			Float dir = 0.0f;
			dir = atan2f(pVerts->normal.z, pVerts->normal.x) * Rad2Deg;
			//pVerts->tu	= asinf(pVerts->normal.x) / (2 * PI) + 0.5f;
			pVerts->tu	= dir / 360.0f;
			pVerts->tv	= 1.0f - asinf(pVerts->normal.y) / PI + 0.5f;

			//Color
			pVerts->color = Color::km_White;
		}

		tMesh->UnlockVertexBuffer();
	}

	m_pMesh = tMesh;

	return true;
}

//////////////////////////////////////////////////////////////////////////

Bool CMesh3D::CreateSphere(Float fRadius /* = 1.0f */, UInt32 nStacks /* = 1 */, UInt32 nSlices /* = 3 */)
{
	if (FAILS(D3DXCreateSphere(g_D3D->GetDevice(), fRadius, nSlices, nStacks, &m_pMesh, NULL)))
	{
		g_Log.LOG_ERROR_DX_DESC(g_hr, "Couldn't create sphere mesh!\n");
		return false;
	}

	D3DMesh tMesh;
	CVertex* pVerts;

	if (!(Clone(D3DXMESH_SYSTEMMEM, &tMesh)))
		return false;

	m_pMesh->Release();

	if (SUCCEEDED(tMesh->LockVertexBuffer(0, (LPVOID *)&pVerts)))
	{
		UInt32 numVerts = tMesh->GetNumVertices();

		for (UInt32 i = 0; i < numVerts; i++, pVerts++)
		{
			//Texture coords
			Float dir = 0.0f;
			dir = atan2f(pVerts->normal.z, pVerts->normal.x) * Rad2Deg;
			//pVerts->tu	= asinf(pVerts->normal.x) / (2 * PI) + 0.5f;
			pVerts->tu	= dir / 360.0f;
			pVerts->tv	= 1.0f - asinf(pVerts->normal.y) / PI + 0.5f;

			//Color
			pVerts->color = Color::km_White;
		}

		tMesh->UnlockVertexBuffer();
	}

	m_pMesh = tMesh;

	return true;
}

//////////////////////////////////////////////////////////////////////////

Bool CMesh3D::CreateTorus(Float fInnerRadius /* = 0.5f */, Float fOuterRadius /* = 1.0f */, UInt32 nSides /* = 3 */, UInt32 nRings /* = 3 */)
{
	if (FAILS(D3DXCreateTorus(g_D3D->GetDevice(), fInnerRadius, fOuterRadius, nSides, nRings, &m_pMesh, NULL)))
	{
		g_Log.LOG_ERROR_DX_DESC(g_hr, "Couldn't create torus mesh!\n");
		return false;
	}

	D3DMesh tMesh;
	CVertex* pVerts;

	if (!(Clone(D3DXMESH_SYSTEMMEM, &tMesh)))
		return false;

	m_pMesh->Release();

	if (SUCCEEDED(tMesh->LockVertexBuffer(0, (LPVOID *)&pVerts)))
	{
		UInt32 numVerts = tMesh->GetNumVertices();

		for (UInt32 i = 0; i < numVerts; i++, pVerts++)
		{
			//Texture coords
			Float dir = 0.0f;
			dir = atan2f(pVerts->normal.z, pVerts->normal.x) * Rad2Deg;
			//pVerts->tu	= asinf(pVerts->normal.x) / (2 * PI) + 0.5f;
			pVerts->tu	= dir / 360.0f;
			pVerts->tv	= 1.0f - asinf(pVerts->normal.y) / PI + 0.5f;

			//Color
			pVerts->color = Color::km_White;
		}

		tMesh->UnlockVertexBuffer();
	}

	m_pMesh = tMesh;

	return true;
}

//////////////////////////////////////////////////////////////////////////