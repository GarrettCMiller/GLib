///////////////////////////////////////////////////////////////////////////////
//		File:				SpriteMgr.h
//
//		Programmer:			Garrett Miller (GM)
//
//		Date created:		August 12, 2004
//
//		Description:		Implementation of a sprite manager class
///////////////////////////////////////////////////////////////////////////////
#pragma once

// #ifndef _STDAFX_INCLUDED_
// 	#include "stdafx.h"
// 	#pragma message("stdafx.h has been included in " __FILE__)
// #endif

#include "d3d9.h"
#include "dxerr.h"
#include "d3dx9core.h"
#include <iostream>

#include "GameMacros.h"

#include "DynamicArray.h"

#include "Singleton.h"

#include "AnimMesh.h"

//////////////////////////////////////////////////////////////////////////

struct MeshResource
{
	char		meshname[80];
	CAnimMesh	mesh;
	UInt32		ID;

	void Release()
	{
		mesh.Release();
	}
};

//////////////////////////////////////////////////////////////////////////

class MeshMgr : public Singleton<MeshMgr>
{
public:

	CDynamicArray<MeshResource*>	m_Meshes;

	UInt32						m_nNextID;
	String						m_ModelBasePath;

	MeshMgr();
	virtual ~MeshMgr(void);

public:
	
	UInt32 LoadMesh(const String& path, Bool bLoadFromPak = true);
	UInt32 LoadMeshFromPakFile(const String& path);
	UInt32 LoadMeshFromFile(const String& path);

	Bool Contains(const String& meshname);

	UInt32 Add(MeshResource* s);

	CAnimMesh*	GetMeshByName(const String& meshname);
	CAnimMesh*	GetMeshByID(UInt32 id);

	MeshResource& GetMeshResourceByName(const String& mesh);
	inline MeshResource& GetMeshResourceByID(UInt32 id);

	UInt32 GetID(const String& mesh);

	/*void DrawSpriteEffect(UInt32 id, UInt32 shader, D3DXHANDLE tech = NULL, Float x = 0.0f, Float y = 0.0f, Float z = 0.5f, Float angle = 0.0f, Float scale = 1.0f, UInt32 mask = 0xFFFFFFFF);
	void RenderSprite(UInt32 id, Float x = 0.0f, Float y = 0.0f, Float z = 0.5f, Float angle = 0.0f, Float scale = 1.0f, UInt32 mask = 0xFFFFFFFF);
	void DrawSprite(UInt32 id, Float x = 0.0f, Float y = 0.0f, Float z = 0.5f, Float angle = 0.0f, Float scale = 1.0f, UInt32 mask = 0xFFFFFFFF);
	void RenderUISprite(UInt32 id, Float x = 0.0f, Float y = 0.0f, Float z = 0.0001f, Float angle = 0.0f, Float scale = 1.0f, UInt32 mask = 0xFFFFFFFF);

	void RenderSpriteEffect(UInt32 id, UInt32 shaderID, D3DXHANDLE tech, Float x = 0.0f, Float y = 0.0f, Float z = 0.5f, Float angle = 0.0f, Float scale = 1.0f, UInt32 mask = 0xFFFFFFFF);

	void DrawUIElement(UIObject* pObj);

	void RenderSprites();
	void RenderUI();

	void Clear();*/

	/*UInt32 DebugGetUIObjectListSize() const
	{
		return m_UIObjectList.Size();
	}

	UInt32 DebugGetUISpriteListSize() const
	{
		return m_UISpriteList.Size();
	}

	UInt32 DebugGetDrawListSize() const
	{
		return m_DrawList.Size();
	}

	UInt32 DebugGetRenderListSize() const
	{
		return m_RenderList.Size();
	}*/

private:

	/*void DrawList();
	void RenderList();*/
};

#define g_MeshMgr	MeshMgr::GetInstance()