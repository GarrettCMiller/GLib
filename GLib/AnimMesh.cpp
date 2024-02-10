#include "StdAfx.h"
#include "AnimMesh.h"

#include "EffectMgr.h"

#include "D3DWrapper.h"

#include "LightMgr.h"

//////////////////////////////////////////////////////////////////////////

// The time to change from one animation set to another
// To see how the merging works - increase this time value to slow it down
const float kMoveTransitionTime=0.25f;

//////////////////////////////////////////////////////////////////////////

CAnimMesh::CAnimMesh(void) : 
	m_fSpeedAdjust(1.0f),			m_pFirstMesh(NULL),
	m_CurrentTrack(0),				m_fCurrentTime(0),
	m_NumAnimationSets(0),			m_CurrentAnimationSet(0),
	m_MaxBones(0),					m_pBoneMatrices(NULL),
	m_pAnimController(NULL),		m_pFrameRoot(NULL),
	//m_sphereRadius(0),			m_sphereCentre(0,0,0),	
	m_nEffectID(-1),				m_nRenderRequests(0),
	m_pInstanceVertexBuffer(NULL),	m_pInstanceIndexBuffer(NULL),
	m_bCastShadow(true)
{
}

CAnimMesh::~CAnimMesh(void)
{
	//Release();
}

//////////////////////////////////////////////////////////////////////////

void CAnimMesh::Init()
{
	if (m_pAnimController)
		SetNumAnimSets(m_pAnimController->GetMaxNumAnimationSets());

	if (m_pFrameRoot)
	{
		SetupBoneMatrices((CD3DFrame*)m_pFrameRoot, NULL);

		// Create the bone matrices array for use during FrameMove to hold the final transform
		m_pBoneMatrices = new D3DMatrix[m_MaxBones];
		ZeroMemory(m_pBoneMatrices, sizeof(D3DMatrix) * m_MaxBones);

		UInt32 numVerts = 0;
		
		if (m_pFrameRoot->pFrameFirstChild)
			numVerts = m_pFrameRoot->pFrameFirstChild->pMeshContainer->MeshData.pMesh->GetNumVertices();
		else
			numVerts = m_pFrameRoot->pMeshContainer->MeshData.pMesh->GetNumVertices();
		
		g_D3D->GetDevice()->CreateVertexBuffer(mk_MaxInstances * numVerts * sizeof(CVertex), 0, 0,
												D3DPOOL_MANAGED, &m_pInstanceVertexBuffer, 0);

	}
}

//////////////////////////////////////////////////////////////////////////

void CAnimMesh::Release()
{
	SAFE_RELEASE(m_pAnimController);

	if (m_pFrameRoot)
	{
		CD3DAllocateHierarchy memAlloc;
		D3DXFrameDestroy(m_pFrameRoot, &memAlloc);		
		m_pFrameRoot = NULL;
	}

	SAFE_RELEASE(m_pInstanceVertexBuffer);
	SAFE_RELEASE(m_pInstanceIndexBuffer);

	SAFE_DELETE_ARRAY(m_pBoneMatrices);
}

//////////////////////////////////////////////////////////////////////////

void CAnimMesh::UpdateFrameMatrices(const D3DXFRAME *frameBase, const D3DMatrix* parentMatrix)
{
	CD3DFrame *currentFrame = (CD3DFrame*)frameBase;

	// If parent matrix exists multiply our frame matrix by it
    if (parentMatrix != NULL)
        D3DXMatrixMultiply(&currentFrame->matCombined, &currentFrame->TransformationMatrix, parentMatrix);
    else
        currentFrame->matCombined = currentFrame->TransformationMatrix;

	// If we have a sibling recurse 
    if (currentFrame->pFrameSibling != NULL)
        UpdateFrameMatrices(currentFrame->pFrameSibling, parentMatrix);

	// If we have a child recurse 
    if (currentFrame->pFrameFirstChild != NULL)
        UpdateFrameMatrices(currentFrame->pFrameFirstChild, &currentFrame->matCombined);
}

//////////////////////////////////////////////////////////////////////////

void CAnimMesh::DrawFrame(LPD3DXFRAME frame)
{
	// Draw all mesh containers in this frame
    LPD3DXMESHCONTAINER meshContainer = frame->pMeshContainer;
    while (meshContainer)
    {
        DrawMeshContainer(meshContainer, frame);
        meshContainer = meshContainer->pNextMeshContainer;
    }

	// Recurse for sibblings
    if (frame->pFrameSibling != NULL)
        DrawFrame(frame->pFrameSibling);

    // Recurse for children
	if (frame->pFrameFirstChild != NULL)
        DrawFrame(frame->pFrameFirstChild);
}

//////////////////////////////////////////////////////////////////////////

void CAnimMesh::DrawMeshContainer(LPD3DXMESHCONTAINER meshContainerBase, LPD3DXFRAME frameBase)
{
	if (m_nRenderRequests == 0)
		m_nRenderRequests = 1;

	// Cast to our extended frame type
	CD3DFrame *frame = (CD3DFrame*)frameBase;		

	// Cast to our extended mesh container
	CD3DMeshContainer *meshContainer = (CD3DMeshContainer*)meshContainerBase;

	// Select the mesh to draw, if there is skin then use the skinned mesh else the normal one
	D3DMesh pDrawMesh = (meshContainer->pSkinInfo) ? meshContainer->pSkinMesh : meshContainer->MeshData.pMesh;
	
	// Set the world transform
	g_D3D->SetWorldMatrix(frame->matCombined);

	D3DVertexBuffer pVertexBuffer = NULL;
	D3DIndexBuffer pIndexBuffer = NULL;

	UInt32 numPasses = 1;
	Effect& fx = g_EffectMgr->GetEffect(m_nEffectID);

	switch (BaseRenderRequest::mk_eRenderType)
	{
	case BaseRenderRequest::eRT_Shadow:
		fx.Shader->SetTechnique("ShadowMap");
		break;

	case BaseRenderRequest::eRT_Final:
		fx.Shader->SetTechnique(m_strTechnique.c_str());
		break;
	}

	pDrawMesh->GetVertexBuffer(&pVertexBuffer);
	pDrawMesh->GetIndexBuffer(&pIndexBuffer);
	
	g_D3D->GetDevice()->SetFVF(pDrawMesh->GetFVF());
	//g_D3D->GetDevice()->SetVertexDeclaration(&g_VBDecl_Geometry);

	if (FAILED(g_hr = g_D3D->GetDevice()->SetStreamSource(0, pVertexBuffer, 0, D3DXGetFVFVertexSize(pDrawMesh->GetFVF()))))
	{
		g_Log.LOG_ERROR_DX_DESC(g_hr, "Failed to set stream source 0!\n");
		return;
	}

	/*if (m_nRenderRequests > 1)
	{
		if (FAILED(g_hr = g_D3D->GetDevice()->SetStreamSourceFreq(0, D3DSTREAMSOURCE_INDEXEDDATA | m_nRenderRequests)))
		{
			g_Log.LOG_ERROR_DX_DESC(g_hr, "Failed to set stream source 0 freq!\n");
			return;
		}

		if (FAILED(g_hr = g_D3D->GetDevice()->SetStreamSource(1, m_pInstanceVertexBuffer, 0, D3DXGetFVFVertexSize(pDrawMesh->GetFVF()))))
		{
			g_Log.LOG_ERROR_DX_DESC(g_hr, "Failed to set stream source 1!\n");
			return;
		}

		if (FAILED(g_hr = g_D3D->GetDevice()->SetStreamSourceFreq(1, D3DSTREAMSOURCE_INSTANCEDATA | 1ul)))
		{
			g_Log.LOG_ERROR_DX_DESC(g_hr, "Failed to set stream source 1 freq!\n");
			return;
		}
	}*/

	if (FAILED(g_hr = g_D3D->GetDevice()->SetIndices(pIndexBuffer)))
	{
		g_Log.LOG_ERROR_DX_DESC(g_hr, "Failed to set indices!\n");
		return;
	}

	//g_D3D->GetDevice()->SetVertexDeclaration
	DWORD tableSize = 0;
	pDrawMesh->GetAttributeTable(NULL, &tableSize);

	ASSERT(tableSize == 1 || tableSize == 2);

	D3DXHANDLE	hDiffuse = fx.Shader->GetParameterByName(NULL, "g_Material.Diffuse"),
				hAmbient = fx.Shader->GetParameterByName(NULL, "g_Material.Ambient");

	D3DXVECTOR4	vDiffuse, vAmbient;

	//HACK OPTIMIZE - remember delete
	//D3DXATTRIBUTERANGE* pAttrTable = new D3DXATTRIBUTERANGE[tableSize];
	D3DXATTRIBUTERANGE pAttrTable[2];
	pDrawMesh->GetAttributeTable(pAttrTable, &tableSize);

	switch (BaseRenderRequest::mk_eRenderType)
	{
	case BaseRenderRequest::eRT_Shadow:
		{
			CMatrix matLight, matView;
			//matLight.Translation();
			matView.LookAt(g_LightMgr->GetLight(0).GetPosition(), k_v3XAxis, k_v3YAxis);
			matView = matView * g_D3D->matProj;
			fx.Shader->SetMatrix("matLightViewProj", &matView);
		}
		break;

	case BaseRenderRequest::eRT_Final:
		{
			fx.Shader->SetVector("g_vCamPos", &Vec4(g_D3D->GetCamera().GetPosition()));
			fx.Shader->SetTexture("pTexture", meshContainer->pTextures[0]);
			if (meshContainer->NumMaterials > 1)
				fx.Shader->SetTexture("pEmissiveMap", meshContainer->pTextures[1]);
			else
				fx.Shader->SetTexture("pEmissiveMap", NULL);
			fx.Shader->SetTexture("pShadowMap", g_D3D->GetShadowMap().texture);
			fx.Shader->SetMatrix("matViewProj", &g_D3D->GetViewProj());
		}
		break;
	}
	
	for (UInt32 iInstance = 0; iInstance < m_nRenderRequests; iInstance++)
	{
		CMatrix matWorld = m_arrInstanceTransforms[iInstance];

		CMatrix matView;
		//matView.LookAt(g_LightMgr->GetLight(0).GetPosition(), matWorld.GetPosition(), k_v3YAxis);
		matView.LookAt(g_LightMgr->GetLight(0).GetPosition(), k_v3Zero, k_v3YAxis);
		matView *= g_D3D->matProj;
		fx.Shader->SetMatrix("matLightViewProj", &matView);

		fx.Shader->SetMatrix("matWorld", &matWorld);

		switch (BaseRenderRequest::mk_eRenderType)
		{
		case BaseRenderRequest::eRT_Shadow:
			{
			}
			break;

		case BaseRenderRequest::eRT_Final:
			{
				g_LightMgr->SetupLightsInShader(fx, matWorld.GetPosition());
			}
			break;
		}

		fx.Shader->Begin(&numPasses, 0);

		// Loop through all the materials in the mesh rendering each subset
		for (UInt32 iMaterial = 0; iMaterial < meshContainer->NumMaterials; iMaterial++)
		{
			// use the material in our extended data rather than the one in meshContainer->pMaterials[iMaterial].MatD3D
			//g_D3D->GetDevice()->SetMaterial(&meshContainer->pMaterials[iMaterial]);
			vDiffuse = D3DXVECTOR4(Color(meshContainer->pMaterials[iMaterial].Diffuse));
			vAmbient = D3DXVECTOR4(Color(meshContainer->pMaterials[iMaterial].Ambient));
			fx.Shader->SetVector(hDiffuse, &vDiffuse);
			fx.Shader->SetVector(hAmbient, &vAmbient);
			
			//fx.Shader->SetTexture("pTexture", meshContainer->pTextures[iMaterial]);
			
			//g_D3D->GetDevice()->SetTexture(0, meshContainer->pTextures[iMaterial]);

			fx.Shader->CommitChanges();

			fx.Shader->BeginPass(iMaterial);

			// Finally Call the mesh draw function
 			g_D3D->GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0,
													pAttrTable[iMaterial].VertexStart, 
													pAttrTable[iMaterial].VertexCount, 
 													pAttrTable[iMaterial].FaceStart * 3,
													pAttrTable[iMaterial].FaceCount);

			fx.Shader->EndPass();
		}

		fx.Shader->End();
	}

	if (FAILED(g_hr = g_D3D->GetDevice()->SetStreamSourceFreq(0, 1)))
	{
		g_Log.LOG_ERROR_DX_DESC(g_hr, "Failed to set stream source 0 freq!\n");
		return;
	}
	
	if (FAILED(g_hr = g_D3D->GetDevice()->SetStreamSourceFreq(1, 1)))
	{
		g_Log.LOG_ERROR_DX_DESC(g_hr, "Failed to set stream source 1 freq!\n");
		return;
	}

	//HACK
	//delete pAttrTable;
}

//////////////////////////////////////////////////////////////////////////

Bool CAnimMesh::AddInstanceData(D3DMatrix matWorld, Bool bShadow /* = true */)
{
	m_arrInstanceTransforms.Push(matWorld);
	
	if (m_nRenderRequests == 0)
	{
		DWORD* pVertsTo = NULL;
		DWORD* pVertsFrom = NULL;
		//D3DMesh pMesh = m_pFrameRoot->pFrameFirstChild->pMeshContainer->MeshData.pMesh;
		D3DMesh pMesh = m_pFirstMesh->GetMesh();

		UInt32 sizeOfInstanceData = pMesh->GetNumBytesPerVertex() * pMesh->GetNumVertices();
		UInt32 sizeOfVertex = D3DXGetFVFVertexSize(pMesh->GetFVF());

		pMesh->LockVertexBuffer(0, (LPVOID*)&pVertsFrom);
		m_pInstanceVertexBuffer->Lock(0, 0, (LPVOID*)&pVertsTo, 0);

		memcpy_s(pVertsTo + m_nRenderRequests * sizeOfInstanceData, 
			mk_MaxInstances * sizeOfInstanceData * sizeOfVertex,
			pVertsFrom,
			sizeOfInstanceData);

		m_pInstanceVertexBuffer->Unlock();
		pMesh->UnlockVertexBuffer();

		g_D3D->PushRenderRequest(new AnimMeshRenderRequest(this, bShadow));
	}

	m_nRenderRequests++;

	return true;
}

void AnimMeshRenderRequest::PreRender()
{
	BaseRenderRequest::PreRender();
}

//////////////////////////////////////////////////////////////////////////

void CAnimMesh::SetupBoneMatrices(CD3DFrame* pFrame, D3DMatrix* pParentMatrix)
{
	// Cast to our extended structure first
	CD3DMeshContainer* pMesh = (CD3DMeshContainer*)pFrame->pMeshContainer;

	// If this frame has a mesh
	if(pMesh)
	{
		// We need to remember which is the first mesh in the hierarchy for later when we 
		// update (FrameMove)
		if(!m_pFirstMesh)
			m_pFirstMesh = pMesh;
		
		// if there is skin info, then setup the bone matrices
		if(pMesh->pSkinInfo && pMesh->MeshData.pMesh)
		{
			// Create a copy of the mesh to skin into later
			D3DVERTEXELEMENT9 Declaration[MAX_FVF_DECL_SIZE];
			if (FAILED(pMesh->MeshData.pMesh->GetDeclaration(Declaration)))
				return;

			pMesh->MeshData.pMesh->CloneMesh(D3DXMESH_SYSTEMMEM, 
												Declaration, g_D3D->GetDevice(), 
												&pMesh->pSkinMesh);

			// Max bones is calculated for later use (to know how big to make the bone matrices array)
			m_MaxBones = Max(m_MaxBones,(UInt32)pMesh->pSkinInfo->GetNumBones());

			// For each bone work out its matrix
			for (UInt32 i = 0; i < pMesh->pSkinInfo->GetNumBones(); i++)
			{   
				// Find the frame containing the bone
				CD3DFrame* pTempFrame = (CD3DFrame*)D3DXFrameFind(m_pFrameRoot, pMesh->pSkinInfo->GetBoneName(i));

				// set the bone part - point it at the transformation matrix
				pMesh->pFrameMatrices[i] = &pTempFrame->matCombined;
			}
		}
	}

	// Pass on to sibblings
	if(pFrame->pFrameSibling)
		SetupBoneMatrices((CD3DFrame*)pFrame->pFrameSibling, pParentMatrix);

	// Pass on to children
	if(pFrame->pFrameFirstChild)
		SetupBoneMatrices((CD3DFrame*)pFrame->pFrameFirstChild, &pFrame->matCombined);
}

//////////////////////////////////////////////////////////////////////////

void CAnimMesh::FrameMove(Float fTime, const D3DMatrix &matWorld)
{
	// Adjust animation speed
	fTime *= m_fSpeedAdjust;

	// Advance the time and set in the controller
    if (m_pAnimController != NULL)
        m_pAnimController->AdvanceTime(fTime, NULL);

	m_fCurrentTime += fTime;

	// Now update the model matrices in the hierarchy
    UpdateFrameMatrices(m_pFrameRoot, &matWorld);

	// If the model contains a skinned mesh update the vertices
	CD3DMeshContainer* pMesh = m_pFirstMesh;
	if(pMesh && pMesh->pSkinInfo)
	{
		UInt32 Bones = pMesh->pSkinInfo->GetNumBones();

		// Create the bone matrices that transform each bone from bone space into character space
		// (via exFrameCombinedMatrixPointer) and also wraps the mesh around the bones using the bone offsets
		// in exBoneOffsetsArray
		for (UInt32 i = 0; i < Bones; ++i)
			D3DXMatrixMultiply(&m_pBoneMatrices[i], &pMesh->pBoneOffsets[i], pMesh->pFrameMatrices[i]);

		// We need to modify the vertex positions based on the new bone matrices. This is achieved
		// by locking the vertex buffers and then calling UpdateSkinnedMesh. UpdateSkinnedMesh takes the
		// original vertex data (in pMesh->MeshData.pMesh), applies the matrices and writes the new vertices
		// out to skin mesh (pMesh->exSkinMesh). 

		// UpdateSkinnedMesh uses software skinning which is the slowest way of carrying out skinning 
		// but is easiest to describe and works on the majority of graphic devices. 
		// Other methods exist that use hardware to do this skinning - see the notes and the 
		// DirectX SDK skinned mesh sample for more details
		void *srcPtr=0;
		pMesh->MeshData.pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&srcPtr);

		void *destPtr=0;
		pMesh->pSkinMesh->LockVertexBuffer(0, (void**)&destPtr);

		// Update the skinned mesh 
		pMesh->pSkinInfo->UpdateSkinnedMesh(m_pBoneMatrices, NULL, srcPtr, destPtr);

		// Unlock the meshes vertex buffers
		pMesh->pSkinMesh->UnlockVertexBuffer();
		pMesh->MeshData.pMesh->UnlockVertexBuffer();
	}
}

//////////////////////////////////////////////////////////////////////////

void CAnimMesh::Render()
{
	if (m_pFrameRoot)
		DrawFrame(m_pFrameRoot);
}

//////////////////////////////////////////////////////////////////////////

void CAnimMesh::SetAnimationSet(UInt32 index)
{
	if (index == m_CurrentAnimationSet)
		return;

	if (index >= m_NumAnimationSets)
		index = 0;

	// Remember current animation
	m_CurrentAnimationSet = index;

	// Get the animation set from the controller
	LPD3DXANIMATIONSET set;
	m_pAnimController->GetAnimationSet(m_CurrentAnimationSet, &set);	

	// Note: for a smooth transition between animation sets we can use two tracks and assign the new set to the track
	// not currently playing then insert Keys into the KeyTrack to do the transition between the tracks
	// tracks can be mixed together so we can gradually change into the new animation

	// Alternate tracks
	DWORD newTrack = (m_CurrentTrack == 0 ? 1 : 0);

	// Assign to our track
	m_pAnimController->SetTrackAnimationSet(newTrack, set);
    set->Release();	

	// Clear any track events currently assigned to our two tracks
	m_pAnimController->UnkeyAllTrackEvents(m_CurrentTrack);
    m_pAnimController->UnkeyAllTrackEvents(newTrack);

	// Add an event key to disable the currently playing track kMoveTransitionTime seconds in the future
    m_pAnimController->KeyTrackEnable(m_CurrentTrack, FALSE, m_fCurrentTime + kMoveTransitionTime);
	// Add an event key to change the speed right away so the animation completes in kMoveTransitionTime seconds
    m_pAnimController->KeyTrackSpeed(m_CurrentTrack, 0.0f, m_fCurrentTime, kMoveTransitionTime, D3DXTRANSITION_LINEAR);
	// Add an event to change the weighting of the current track (the effect it has blended with the secon track)
    m_pAnimController->KeyTrackWeight(m_CurrentTrack, 0.0f, m_fCurrentTime, kMoveTransitionTime, D3DXTRANSITION_LINEAR);

	// Enable the new track
    m_pAnimController->SetTrackEnable(newTrack, TRUE);
	// Add an event key to set the speed of the track
    m_pAnimController->KeyTrackSpeed(newTrack, 1.0f, m_fCurrentTime, kMoveTransitionTime, D3DXTRANSITION_LINEAR);
	// Add an event to change the weighting of the current track (the effect it has blended with the first track)
	// As you can see this will go from 0 effect to total effect(1.0f) in kMoveTransitionTime seconds and the first track goes from 
	// total to 0.0f in the same time.
    m_pAnimController->KeyTrackWeight(newTrack, 1.0f, m_fCurrentTime, kMoveTransitionTime, D3DXTRANSITION_LINEAR);

	// Remember current track
    m_CurrentTrack = newTrack;
}

//////////////////////////////////////////////////////////////////////////

void CAnimMesh::NextAnimation()
{
	UInt32 newAnimationSet = m_CurrentAnimationSet + 1;
	
	if (newAnimationSet >= m_NumAnimationSets)
		newAnimationSet = 0;

	SetAnimationSet(newAnimationSet);
}

//////////////////////////////////////////////////////////////////////////

String CAnimMesh::GetAnimationSetName(UInt32 index)
{
	if (index >= m_NumAnimationSets)
		return "Error: No such set exists";

	// Get the animation set
	LPD3DXANIMATIONSET set;
	m_pAnimController->GetAnimationSet(m_CurrentAnimationSet, &set);

	String nameString(set->GetName());

	set->Release();

	return nameString;
}

//////////////////////////////////////////////////////////////////////////
#if 0
struct INSTANCEDATA
{	
	D3DXMATRIX matTransform;
	float fReflection;	
	D3DXCOLOR Color;
};
	
//#define D3DFVF_INSTANCING (D3DFVF_TEX0|D3DFVF_TEX1|D3DFVF_TEX2|D3DFVF_TEX3|D3DFVF_TEX4|D3DFVF_DIFFUSE)
D3DVERTEXELEMENT9 InstancingDecl[] =
{	
	{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
	{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
	{1, 0,  D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
	{1, 16, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},
	{1, 32, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2},
	{1, 48, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3},
	{1, 64, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 4},
	{1, 68, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,    0},
	D3DDECL_END()
};

void CreateInstancingData(IDirect3DDevice9* pd3dDevice, ID3DXMesh** ppMesh, int iWidth, int iHeight, int iLength, float fInterval)
{	
	HRESULT hr;	
	D3DVERTEXELEMENT9 declTemp[MAX_FVF_DECL_SIZE];
	V((*ppMesh)->GetDeclaration(declTemp));	
	//V(pd3dDevice->CreateVertexDeclaration(declTemp, &g_pVertexDeclHardware));
	//g_pVertexDeclHardware	V((*ppMesh)->GetVertexBuffer(&g_pVBMeshData));
	g_iNumInstances = iWidth * iHeight * iLength;
	V((*ppMesh)->GetIndexBuffer(&g_pIBMeshData));
	V(pd3dDevice->CreateVertexDeclaration(InstancingDecl, &g_pVertexDeclHardware));
	V(pd3dDevice->CreateVertexBuffer(g_iNumInstances * sizeof(INSTANCEDATA), 0, 0, D3DPOOL_MANAGED, &g_pVBInstanceData, NULL));
	INSTANCEDATA *pInstances = NULL;
	V(g_pVBInstanceData->Lock( 0, g_iNumInstances * sizeof(INSTANCEDATA), (void**)&pInstances, 0 ));
	for(int w = 0; w < iWidth; w++)
	{
		for(int h = 0; h < iHeight; h++)
		{			
			for(int l = 0; l < iLength; l++)
			{				
				D3DXMATRIX matTransl, matRot;
				D3DXMatrixIdentity(&matTransl);
				D3DXMatrixRotationYawPitchRoll(&matRot , (float)rand() / (float)RAND_MAX * 2.0f * D3DX_PI, (float)rand() / (float)RAND_MAX * 2.0f * D3DX_PI, (float)rand() / (float)RAND_MAX * 2.0f * D3DX_PI);
				matTransl._41 = ((float)w - (float)iWidth/2.0f + 0.5f) * fInterval;
				matTransl._42 = ((float)h - (float)iHeight/2.0f + 0.5f) * fInterval;
				matTransl._43 = ((float)l - (float)iLength/2.0f + 0.5f) * fInterval;
				pInstances->matTransform = matRot * matTransl;
				pInstances->fReflection = (float)rand() / (float)(RAND_MAX * 2) + 1.0f;
				pInstances->Color = D3DCOLOR_ARGB(rand()%256, rand()%256, rand()%256, rand()%256);
				pInstances++;
			}
		}
	}   

	g_pVBInstanceData->Unlock();
}

void RenderInstancing(IDirect3DDevice9* pd3dDevice, ID3DXMesh** ppMesh)
{   
	HRESULT hr;
	UINT iPass, cPasses;
	V( pd3dDevice->SetVertexDeclaration(g_pVertexDeclHardware));    // Stream zero is our model, and its frequency is how we communicate the number of instances required,
	// which in this case is the total number of boxes
	V( pd3dDevice->SetStreamSource( 0, g_pVBMeshData, 0, (*ppMesh)->GetNumBytesPerVertex() ) );
	V( pd3dDevice->SetStreamSourceFreq( 0, D3DSTREAMSOURCE_INDEXEDDATA | g_iNumInstances ) );            // Stream one is the Instancing buffer, so this advances to the next value
	// after each box instance has been drawn, so the divider is 1.
	V(pd3dDevice->SetStreamSource(1, g_pVBInstanceData, 0, sizeof(INSTANCEDATA)));
	V(pd3dDevice->SetStreamSourceFreq(1, D3DSTREAMSOURCE_INSTANCEDATA | 1ul ));

	int vcount = (*ppMesh)->GetNumVertices();
	int pcount = (*ppMesh)->GetNumFaces();
	V(pd3dDevice->SetIndices(g_pIBMeshData));
	V( g_pEffect->Begin(&cPasses, 0) );

	for (iPass = 0; iPass < cPasses; iPass++)
	{		
		V( g_pEffect->BeginPass(iPass) );
		//V( (*ppMesh)->DrawSubset(iPass) );
		V( pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,vcount,0,pcount));
		V( g_pEffect->EndPass() );
	}	

	V( g_pEffect->End() );
	V( pd3dDevice->SetStreamSourceFreq( 0, 1 ) );
	V( pd3dDevice->SetStreamSourceFreq( 1, 1 ) );
}
#endif