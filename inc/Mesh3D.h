#pragma once

#include "D3DWrapper.h"

#include "D3DFrame.h"

//////////////////////////////////////////////////////////////////////////

struct MeshRenderRequest : public BaseRenderRequest
{
	MeshRenderRequest() : pMesh(NULL) { }

	MeshRenderRequest(D3DMesh* mesh) : BaseRenderRequest(), pMesh(mesh)
	{	}

	MeshRenderRequest(D3DMesh* mesh, D3DMatrix mWorld, 
		UInt32 fxID = -1, D3DXHANDLE fxTech = NULL, const TShaderVarArray* fxVars = NULL) 
		: BaseRenderRequest(mWorld, fxID, fxTech, fxVars), pMesh(mesh)
	{	}

	D3DMesh*	pMesh;

	////////////////////
	virtual void PreRender()
	{
		BaseRenderRequest::PreRender();
	}

	virtual void Render()
	{
		BaseRenderRequest::Render();
	}

	virtual void Draw()
	{
		ASSERT(pMesh);
		ASSERT(*pMesh);
		(*pMesh)->DrawSubset(0);
	}
};

//////////////////////////////////////////////////////////////////////////

class CMesh3D
{
public:
	CMesh3D(Bool bAutoRelease = true);
	virtual ~CMesh3D(void);

	void RenderRequest()
	{
		g_D3D->PushRenderRequest(new MeshRenderRequest(m_RenderRequest));
	}

	void SetRenderRequest(UInt32 fxID, D3DXHANDLE tech)
	{
		D3DMatrix matPos;
		D3DXMatrixTranslation(&matPos, m_vPos.x, m_vPos.y, m_vPos.z);
		m_RenderRequest = MeshRenderRequest(&m_pMesh, matPos, fxID, tech, NULL);
	}

	//D3DMesh functions
	Bool Clone(DWORD Options, D3DMesh* ppCloneMesh,
				UInt32 FVF = CVertex::FVF);
	
	Bool Draw(UInt32 subset = 0);

	UInt32 GetNumFaces() const;
	UInt32 GetNumVerts() const;

	Bool Lock(	Bool bIBuffer = true, UInt32 nIFlags = 0, LPVOID* ppIData = NULL,
				Bool bVBuffer = true, UInt32 nVFlags = 0, LPVOID* ppVData = NULL);
	
	UInt32 Unlock(Bool bIBuffer = true, Bool bVBuffer = true);

	Bool Release();

	//D3DX functions
	Bool CreateBox(Float fWidth = 1.0f, Float fHeight = 1.0f, Float fDepth = 1.0f);
	Bool CreateCylinder(Float fRadius1 = 1.0f, Float fRadius2 = 1.0f, Float fLength = 1.0f, UInt32 nStacks = 1, UInt32 nSlices = 3);
	Bool CreateSphere(Float fRadius = 1.0f, UInt32 nStacks = 1, UInt32 nSlices = 3);
	Bool CreateTorus(Float fInnerRadius = 0.5f, Float fOuterRadius = 1.0f, UInt32 nSides = 3, UInt32 nRings = 3);

	//////////////////////

	void SetPosition(const Vec3& pos)
	{
		m_vPos = pos;
	}

protected:
	Bool				m_bReleaseOnDestruct;

	D3DMesh				m_pMesh;

	Vec3				m_vPos;

	MeshRenderRequest	m_RenderRequest;
};