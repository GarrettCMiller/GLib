#pragma once

#include "D3DWrapper.h"
#include "D3DFrame.h"

#include "Rect.h"

//////////////////////////////////////////////////////////////////////////

struct AnimMeshRenderRequest;

class CAnimMesh
{
	friend AnimMeshRenderRequest;
public:
	CAnimMesh();
	~CAnimMesh();
	
	void Init();
	void Release();

	void FrameMove(Float fTime, const D3DMatrix& matWorld);
	
	void Render();

	void SetAnimationSet(UInt32 index);
	void NextAnimation();

	UInt32 GetCurrentAnimationSet() const {return m_CurrentAnimationSet;}
	String GetAnimationSetName(UInt32 index);

	LPD3DXFRAME& GetFrameRoot()
	{
		return m_pFrameRoot;
	}

	D3DAnimController& GetAnimController()
	{
		return m_pAnimController;
	}

	GLib::Rect3D GenerateBoundingBox()
	{
		CVertex* pData = NULL;
		D3DXVECTOR3 vMin, vMax;
		D3DMesh pMesh = GetMeshObject();

		ASSERT(pMesh);
		
		pMesh->LockVertexBuffer(D3DLOCK_READONLY, (LPVOID*)&pData);
		D3DXComputeBoundingBox((D3DXVECTOR3*)pData, pMesh->GetNumVertices(), sizeof(CVertex), &vMin, &vMax);
		pMesh->UnlockVertexBuffer();

		return GLib::Rect3D(vMin, vMax);
	}

	Bool SetShader(Int32 sID, const String& strTech = "")
	{
		if (sID >= 0)
			m_nEffectID = sID;

		if(!strTech.empty())
			m_strTechnique = strTech;

		return true;
	}

	Bool Duplicate(const CAnimMesh& rhs)
	{
		m_nEffectID				= rhs.m_nEffectID;
		m_strTechnique			= rhs.m_strTechnique;
		m_MaxBones				= rhs.m_MaxBones;
		m_CurrentAnimationSet	= rhs.m_CurrentAnimationSet;
		m_NumAnimationSets		= rhs.m_NumAnimationSets;
		m_CurrentTrack			= rhs.m_CurrentTrack;
		m_fCurrentTime			= rhs.m_fCurrentTime;
		m_fSpeedAdjust			= rhs.m_fSpeedAdjust;
	}

	Bool AddInstanceData(D3DMatrix matWorld, Bool bShadow = true);

protected:
	void UpdateFrameMatrices(const D3DXFRAME *frameBase, const D3DMatrix* parentMatrix);
	void DrawFrame(LPD3DXFRAME frame);
	void DrawMeshContainer(LPD3DXMESHCONTAINER meshContainerBase, LPD3DXFRAME frameBase);
	void SetupBoneMatrices(CD3DFrame* pFrame, D3DMatrix* pParentMatrix);

	void SetNumAnimSets(UInt32 numSets)
	{
		m_NumAnimationSets = numSets;
	}

	D3DMesh GetMeshObject()
	{
		return m_pFirstMesh->GetMesh();
	}

	void ClearRequests()
	{
		m_nRenderRequests = 0;
		m_arrInstanceTransforms.Clear();
	}

protected:
	LPD3DXFRAME					m_pFrameRoot;
	
	D3DAnimController			m_pAnimController;
	CD3DMeshContainer*			m_pFirstMesh;

	UInt32						m_nRenderRequests;
	D3DVertexBuffer				m_pInstanceVertexBuffer;
	D3DIndexBuffer				m_pInstanceIndexBuffer;
	CDynamicArray<CMatrix>		m_arrInstanceTransforms;

	Bool						m_bCastShadow;

	//Shader stuff
	Int32						m_nEffectID;
	String						m_strTechnique;

	// Bone data
	D3DMatrix*					m_pBoneMatrices;
	UInt32						m_MaxBones;

	// Animation variables
	UInt32						m_CurrentAnimationSet;	
	UInt32						m_NumAnimationSets;
	UInt32						m_CurrentTrack;
	Float						m_fCurrentTime;
	Float						m_fSpeedAdjust;

	static const UInt32			mk_MaxInstances = 20;

};

//////////////////////////////////////////////////////////////////////////

struct AnimMeshRenderRequest : public BaseRenderRequest
{
	AnimMeshRenderRequest() : pMesh(NULL) { }

	AnimMeshRenderRequest(CAnimMesh* mesh, Bool bShadow = true) : BaseRenderRequest(), pMesh(mesh)
	{	castShadow = bShadow;	}

	AnimMeshRenderRequest(CAnimMesh* mesh, const CMatrix& mWorld, 
		UInt32 fxID = -1, D3DXHANDLE fxTech = NULL, const TShaderVarArray* fxVars = NULL) 
		: BaseRenderRequest(mWorld, fxID, fxTech, fxVars), pMesh(mesh)
	{	castShadow = true;	}

	CAnimMesh*	pMesh;

	////////////////////
	virtual void PreRender();

	virtual void Render()
	{
		BaseRenderRequest::Render();
	}

	virtual void Draw()
	{
		ASSERT(pMesh);
		pMesh->Render();
	}
	
	virtual void Cleanup()
	{	
		pMesh->ClearRequests();
	}
};