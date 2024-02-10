#include "StdAfx.h"
#include "D3DFrame.h"

#include "SpriteMgr.h"

//////////////////////////////////////////////////////////////////////////

CD3DFrame::CD3DFrame(void)
{
}

CD3DFrame::~CD3DFrame(void)
{
}

//////////////////////////////////////////////////////////////////////////

void CD3DFrame::Init()
{
	// Now initialize other data members of the frame to defaults
	D3DXMatrixIdentity(&TransformationMatrix);
	D3DXMatrixIdentity(&matCombined);

	pMeshContainer		= NULL;
	pFrameSibling		= NULL;
	pFrameFirstChild	= NULL;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CD3DMeshContainer::CD3DMeshContainer(void)
{
}

CD3DMeshContainer::~CD3DMeshContainer(void)
{
}

//////////////////////////////////////////////////////////////////////////

void CD3DMeshContainer::Init(	UInt32 numMaterials, const D3DXMATERIAL* pMats, 
								const D3DXMESHDATA* pMeshData, LPD3DXSKININFO SkinInfo,
								const D3DXEFFECTINSTANCE* pEffectInstances)
{
	MeshData.Type = D3DXMESHTYPE_MESH;

	// Adjacency data - holds information about triangle adjacency, required by the ID3DMESH object
	DWORD dwFaces = pMeshData->pMesh->GetNumFaces();
	pAdjacency = new DWORD[dwFaces * 3];
	memcpy(pAdjacency, pAdjacency, sizeof(DWORD) * dwFaces * 3);
	
	// Get the Direct3D device, luckily this is held in the mesh itself (Note: must release it when done with it)
	LPDIRECT3DDEVICE9 pd3dDevice = 0;
	pMeshData->pMesh->GetDevice(&pd3dDevice);

	// Changed 24/09/07 - can just assign pointer and add a ref rather than need to clone
	MeshData.pMesh = pMeshData->pMesh;
	MeshData.pMesh->AddRef();

	// Create material and texture arrays. Note that I always want to have at least one
	NumMaterials	= Max(numMaterials, 1U);
	pMaterials		= new D3DMaterial[NumMaterials];
	pTextures		= new D3DTexture[NumMaterials];

	ZeroMemory(pTextures, sizeof(D3DTexture) * NumMaterials);

	if (NumMaterials > 0)
	{
		// Load all the textures and copy the materials over		
		for(UInt32 i = 0; i < NumMaterials; ++i)
		{
			pTextures[i] = 0;	
			pMaterials[i] = pMats[i].MatD3D;

			if(pMats[i].pTextureFilename)
			{
				String texturePath("Res\\Images\\Skins\\");
				texturePath += pMats[i].pTextureFilename;

				UInt32 texID = g_SpriteMgr->LoadSprite(texturePath, false);
				
				if (texID == -1)
				{
					texturePath = "Res\\Images\\Level\\";
					texturePath += pMats[i].pTextureFilename;

					texID = g_SpriteMgr->LoadSprite(texturePath, false);
				}

				ASSERT(texID != -1);

				pTextures[i] = g_SpriteMgr->GetTexture(texID);
			}
		}
	}
	else    
	// make a default material in the case where the mesh did not provide one
	{
		ZeroMemory(&pMaterials[0], sizeof(D3DMaterial));
		pMaterials[0].Diffuse.r = 0.5f;
		pMaterials[0].Diffuse.g = 0.5f;
		pMaterials[0].Diffuse.b = 0.5f;
		pMaterials[0].Specular = pMaterials[0].Diffuse;
		pTextures[0] = NULL;
	}

	// If there is skin data associated with the mesh copy it over
	if (SkinInfo)
	{
		// save off the SkinInfo
	    pSkinInfo = SkinInfo;
	    SkinInfo->AddRef();

	    // Need an array of offset matrices to move the vertices from the figure space to the bone's space
	    UINT numBones = pSkinInfo->GetNumBones();
	    pBoneOffsets = new CMatrix[numBones];

		// Create the arrays for the bones and the frame matrices
		pFrameMatrices = new CMatrix*[numBones];

	    // get each of the bone offset matrices so that we don't need to get them later
	    for (UINT i = 0; i < numBones; i++)
	        pBoneOffsets[i] = *(pSkinInfo->GetBoneOffsetMatrix(i));

		g_Log << "Mesh has skinning info. Number of bones: " << numBones << "\n";
        // Note: in the Microsoft samples a GenerateSkinnedMesh function is called here in order to prepare
		// the skinned mesh data for optimial hardware acceleration. As mentioned in the notes this sample
		// does not do hardware skinning but instead uses software skinning.
	}
	else	
	{
		// No skin info so 0 all the pointers
		pSkinInfo		= NULL;
		pBoneOffsets	= NULL;
		pSkinMesh		= NULL;
		pFrameMatrices	= NULL;
	}

	// When we got the device we caused an internal reference count to be incremented
	// So we now need to release it
	pd3dDevice->Release();

	// The mesh may contain a reference to an effect file
	if (pEffectInstances)
	{
		//if (effectInstances->pEffectFilename)
	}
}

//////////////////////////////////////////////////////////////////////////

void CD3DMeshContainer::Release()
{
	// name
	SAFE_DELETE_ARRAY(Name);
	
	// material array
	SAFE_DELETE_ARRAY(pMaterials);

	// release the textures before deleting the array
	if(pTextures)
		for(UInt32 i = 0; i < NumMaterials; ++i)
			SAFE_RELEASE_NORM(pTextures[i]);

	// texture array
	SAFE_DELETE_ARRAY(pTextures);

	// adjacency data
	SAFE_DELETE_ARRAY(pAdjacency);
	
	// bone parts
	SAFE_DELETE_ARRAY(pBoneOffsets);
	
	// frame matrices
	SAFE_DELETE_ARRAY(pFrameMatrices);
	
	// release skin mesh
	SAFE_RELEASE(pSkinMesh);
	
	// release the main mesh
	SAFE_RELEASE(MeshData.pMesh);
		
	// release skin information
	SAFE_RELEASE(pSkinInfo);
}

//////////////////////////////////////////////////////////////////////////

D3DMesh CD3DMeshContainer::GetMesh()
{
	return this->MeshData.pMesh;
	//return pSkinMesh;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

HRESULT CD3DAllocateHierarchy::CreateFrame(LPCTSTR Name, LPD3DXFRAME *ppNewFrame)
{
	// Always a good idea to initialise a return pointer before proceeding
	*ppNewFrame = 0;

	// Create a new frame using the derived version of the structure
    CD3DFrame *newFrame = new CD3DFrame;

	if (!newFrame)
	{
		g_Log.LOG_ERROR("Couldn't allocate frame!\n");
		return E_OUTOFMEMORY;
	}

	ZeroMemory(newFrame, sizeof(CD3DFrame));

	// Now fill in the data members in the frame structure
	newFrame->Init();   

	// Assign the return pointer to our newly created frame
    *ppNewFrame = newFrame;
	
	// The frame name (note: may be 0 or zero length)
	if (Name && strlen(Name))
	{
		newFrame->Name = new char[strlen(Name) + 1];
		memcpy(newFrame->Name, Name, (strlen(Name) + 1) * sizeof(char));

		g_Log << "Added frame " << Name << "\n";
	}
	else
		g_Log << "Added frame: no name given\n";

	return D3D_OK;
}

//////////////////////////////////////////////////////////////////////////

HRESULT CD3DAllocateHierarchy::CreateMeshContainer(LPCSTR Name, 
													CONST D3DXMESHDATA *pMeshData, 
													CONST D3DXMATERIAL *pMaterials, 
													CONST D3DXEFFECTINSTANCE *pEffectInstances, 
													DWORD NumMaterials, 
													CONST DWORD *pAdjacency, 
													LPD3DXSKININFO pSkinInfo, 
													LPD3DXMESHCONTAINER *ppNewMeshContainer)
{
	// Create a mesh container structure to fill and initilaise to zero values
	// Note: I use my extended version of the structure (D3DXMESHCONTAINER_EXTENDED) defined in MeshStructures.h
	CD3DMeshContainer *newMeshContainer = new CD3DMeshContainer;

	if (!newMeshContainer)
	{
		g_Log.LOG_ERROR("Couldn't allocate mesh container!\n");
		return E_OUTOFMEMORY;
	}

	ZeroMemory(newMeshContainer, sizeof(CD3DMeshContainer));

	// Always a good idea to initialise return pointer before proceeding
	*ppNewMeshContainer = 0;

	// The mesh name (may be 0) needs copying over
	if (Name && strlen(Name))
	{
		newMeshContainer->Name = new char[strlen(Name) + 1];
		memcpy(newMeshContainer->Name, Name, (strlen(Name) + 1) * sizeof(char));
		
		g_Log << "Added mesh container " << Name << "\n";
	}
	else
		g_Log << "Added mesh container: no name given\n";

	//HACK
	pAdjacency;

	// The mesh type (D3DXMESHTYPE_MESH, D3DXMESHTYPE_PMESH or D3DXMESHTYPE_PATCHMESH)
	if (pMeshData->Type != D3DXMESHTYPE_MESH)
	{
		g_Log.LOG_ERROR("Mesh is of wrong type! Expected D3DXMESHTYPE_MESH.\n");

		// This demo does not handle mesh types other than the standard
		// Other types are D3DXMESHTYPE_PMESH (progressive mesh) and D3DXMESHTYPE_PATCHMESH (patch mesh)
		DestroyMeshContainer(newMeshContainer);
		
		return E_NOTIMPL;
	}

	newMeshContainer->Init(NumMaterials, pMaterials, pMeshData, pSkinInfo, pEffectInstances);
	
	// Set the output mesh container pointer to our newly created one
	*ppNewMeshContainer = newMeshContainer;    

	return D3D_OK;
}

//////////////////////////////////////////////////////////////////////////

HRESULT CD3DAllocateHierarchy::DestroyFrame(LPD3DXFRAME pFrameToFree)
{
	// Convert to our extended type. OK to do this as we know for sure it is:
	CD3DFrame *frame = (CD3DFrame*) pFrameToFree;

	SAFE_DELETE_ARRAY(frame->Name);
	SAFE_DELETE(frame);

	return D3D_OK;
}

//////////////////////////////////////////////////////////////////////////

HRESULT CD3DAllocateHierarchy::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase)
{
	// Convert to our extended type. OK as we know for sure it is:
    CD3DMeshContainer* meshContainer = (CD3DMeshContainer*) pMeshContainerBase;
	
	if (!meshContainer)
		return E_INVALIDARG;

	meshContainer->Release();

	// finally delete the mesh container itself
	SAFE_DELETE(meshContainer);

	return D3D_OK;
}