#pragma once

#include "Matrix.h"

//////////////////////////////////////////////////////////////////////////

class CD3DFrame : public D3DXFRAME
{
	friend class CAnimMesh;

public:
	CD3DFrame(void);
	~CD3DFrame(void);

public:
	void Init();

//protected:
	//Total combined matrices
	CMatrix	matCombined;
};

//////////////////////////////////////////////////////////////////////////

class CD3DMeshContainer : public D3DXMESHCONTAINER
{
	friend class CAnimMesh;

public:
	CD3DMeshContainer();
	~CD3DMeshContainer();

public:
	void Init(	UInt32 numMaterials, const D3DXMATERIAL* pMats, 
				const D3DXMESHDATA* pMeshData, LPD3DXSKININFO SkinInfo,
				const D3DXEFFECTINSTANCE* pEffectInstances);

	void Release();

	D3DMesh GetMesh();

protected:
	//Mesh variables
	D3DTexture*		pTextures;		// Textures of the mesh
	D3DMaterial*	pMaterials;		// Use the DirectX 9 Material type

	//Skinned mesh variables
	D3DMesh			pSkinMesh;		// The original mesh
	CMatrix*		pBoneMatrices;	// The bones for the mesh 
	CMatrix*		pBoneOffsets;	// The bone matrix Offsets
	CMatrix**		pFrameMatrices;	// The Frame Matrix
};

//////////////////////////////////////////////////////////////////////////

class CD3DAllocateHierarchy : public ID3DXAllocateHierarchy
{
public:
	// Create a frame
	//1. The name of the frame
	//2. The output new frame
	STDMETHOD(CreateFrame)(THIS_ LPCTSTR Name, LPD3DXFRAME *ppNewFrame);

	// Create a Mesh Container
	//1. Name of the Mesh
	//2. The mesh Data
	//3. that materials of the mesh
	//4. the effects on the mesh
	//5. the number of materials in the mesh
	//6. the adjacency array for the mesh
	//7. the skin information for the mesh
	//8. the output mesh container
	STDMETHOD(CreateMeshContainer)(LPCSTR Name, 
									CONST D3DXMESHDATA* pMeshData, 
									CONST D3DXMATERIAL* pMaterials, 
									CONST D3DXEFFECTINSTANCE* pEffectInstances, 
									DWORD NumMaterials, 
									CONST DWORD* pAdjacency, 
									LPD3DXSKININFO pSkinInfo, 
									LPD3DXMESHCONTAINER* ppNewMeshContainer);

	// Destroy a frame
	//1. The frame to delete
	STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pFrameToFree);

	// Destroy a mesh container
	//1. The container to destroy
	STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER pMeshContainerBase);
};

//////////////////////////////////////////////////////////////////////////