#include "StdAfx.h"
#include "MeshMgr.h"

#include "D3DWrapper.h"

#include "D3DFrame.h"

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_SINGLETON(MeshMgr);

//////////////////////////////////////////////////////////////////////////

static String		k_ModelsBasePath = "";

//////////////////////////////////////////////////////////////////////////

MeshMgr::MeshMgr(void)
{
	m_nNextID		= 0;
	m_ModelBasePath = k_ModelsBasePath;
}

MeshMgr::~MeshMgr(void)
{
	g_Log << "MeshMgr::~MeshMgr() begin...\n";

	m_Meshes.FreeReleasePointers();
}

//////////////////////////////////////////////////////////////////////////

Bool MeshMgr::Contains(const String& meshname)
{
	FOR_EACH(i, m_Meshes)
		if (meshname == m_Meshes[i]->meshname)
			return true;

	return false;
}

//////////////////////////////////////////////////////////////////////////

UInt32 MeshMgr::Add(MeshResource* mesh)
{
	ASSERT(mesh);

	m_Meshes.Push(mesh);

	return m_nNextID++;
}

//////////////////////////////////////////////////////////////////////////

CAnimMesh* MeshMgr::GetMeshByName(const String& meshname)
{
	FOR_EACH(i, m_Meshes)
		if (meshname == m_Meshes[i]->meshname)
			return &(m_Meshes[i]->mesh);

	//Shouldn't be here
	ASSERT(false);
	return NULL;
}

//////////////////////////////////////////////////////////////////////////

CAnimMesh* MeshMgr::GetMeshByID(UInt32 id)
{
	return &(m_Meshes[id]->mesh);
}

//////////////////////////////////////////////////////////////////////////

MeshResource& MeshMgr::GetMeshResourceByName(const String& meshname)
{
	FOR_EACH(i, m_Meshes)
		if (meshname == m_Meshes[i]->meshname)
			return *(m_Meshes[i]);

	//Shouldn't be here
	ASSERT(false);

	return *(m_Meshes[0]);
}

//////////////////////////////////////////////////////////////////////////

UInt32 MeshMgr::GetID(const String& mesh)
{
	FOR_EACH(i, m_Meshes)
		if (mesh == m_Meshes[i]->meshname)
			return m_Meshes[i]->ID;

	//Shouldn't be here
	ASSERT(false);

	return static_cast<UInt32>(-1);
}

//////////////////////////////////////////////////////////////////////////

UInt32 MeshMgr::LoadMesh(const String& path, Bool bLoadFromPak /* = true*/)
{
	if (Contains(path))
		return GetID(path);

	g_ResLog << "Loading mesh ";

	/*if (bLoadFromPak)
		return LoadMeshFromPakFile(path);
	else*/
		return LoadMeshFromFile(path);
}

//////////////////////////////////////////////////////////////////////////

UInt32 MeshMgr::LoadMeshFromPakFile(const String& path)
{
	g_ResLog << path << " from pak file...\n";

	//D3DXLoadMeshHierarchyFromXInMemory(

	return static_cast<UInt32>(-1);
}

//////////////////////////////////////////////////////////////////////////

UInt32 MeshMgr::LoadMeshFromFile(const String& path)
{
	CD3DAllocateHierarchy* pAlloc = new CD3DAllocateHierarchy;
	MeshResource* meshRes = new MeshResource;

	LPD3DXFRAME& pFrameRoot = meshRes->mesh.GetFrameRoot();
	D3DAnimController& pAnimController = meshRes->mesh.GetAnimController();
	
	g_ResLog << path << " from file...\n";

	char meshPath[256];

	if (!m_ModelBasePath.empty() && path.find("Res\\") == String::npos)
	{
		strncpy_s(meshPath, m_ModelBasePath.c_str(), 256);
		strncat_s(meshPath, path.c_str(), 256);
	}
	else
	{
		strncpy_s(meshPath, path.c_str(), 256);
	}

	if (FAILS(D3DXLoadMeshHierarchyFromX(meshPath,
								D3DXMESH_SYSTEMMEM,
								g_D3D->GetDevice(),
								pAlloc,
								NULL,
								&pFrameRoot,
								&pAnimController)))
	{
		g_Log.LOG_ERROR_DX_DESC(g_hr, "Failed to load mesh!\t" + path + "\n");
		g_ResLog.LOG_ERROR_DX_DESC(g_hr, "Failed to load mesh!\t" + path + "\n");

		SAFE_DELETE(meshRes);
		SAFE_DELETE(pAlloc);

		return static_cast<UInt32>(-1);
	}

	SAFE_DELETE(pAlloc);

	meshRes->mesh.Init();

	meshRes->ID = m_nNextID;
	strncpy_s(meshRes->meshname, path.c_str(), sizeof(meshRes->meshname));

	g_Log << "Mesh successfully loaded from file!\n";

	return Add(meshRes);
}

//////////////////////////////////////////////////////////////////////////