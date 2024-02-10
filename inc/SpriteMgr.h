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

#include "Vec.h"

#include "LListDbl.h"
#include "DynamicArray.h"

#include "Singleton.h"

//////////////////////////////////////////////////////////////////////////

#define	TOTAL				0
#define	LIST				1
#define	LIST_OR_TOTAL		TOTAL

//////////////////////////////////////////////////////////////////////////

struct Sprite
{
	char				texname[80];
	LPDIRECT3DTEXTURE9	texture;
	UInt32				ID;
	UInt32				width, height;

	void Release()
	{
		SAFE_RELEASE(texture);
	}
};

//////////////////////////////////////////////////////////////////////////

typedef Sprite*					SpritePtr;

//////////////////////////////////////////////////////////////////////////

struct DRAW_ITEM
{
	DRAW_ITEM()
	{
		ZeroMemory(this, sizeof(DRAW_ITEM));
	}

	Float		x, y, z;
	Float		rot;
	Float		scale;
	UInt32		mask;
	UInt32		id;
	Int32			shaderID;
	D3DXHANDLE	shaderTech;

	Bool operator == (const DRAW_ITEM& rhs) const
	{
		return (memcmp(this, &rhs, sizeof(DRAW_ITEM)) == 0);
	}

	Bool operator != (const DRAW_ITEM& di) const
	{
		return !((*this) == di);
	}
};

//////////////////////////////////////////////////////////////////////////

class UIObject;

//////////////////////////////////////////////////////////////////////////

#define USE_NEW_SPRITE_LIST

class SpriteMgr : public Singleton<SpriteMgr>
{
public:

	CDynamicArray<Sprite*>		m_Sprites;

	DblLinkedList<UIObject*>	m_UIObjectList;
	DblLinkedList<DRAW_ITEM>	m_UISpriteList;
	DblLinkedList<DRAW_ITEM>	m_DrawList;
	DblLinkedList<DRAW_ITEM>	m_RenderList;	//Require shaders or such

	UInt32						m_nNextID;
	UInt32						m_FontID;

	String						m_ImagesBasePath;

	SpriteMgr();
	virtual ~SpriteMgr(void);

public:
	
	UInt32 LoadSprite(const String& path, Bool bLoadFromPak = true);
	UInt32 LoadSpriteFromPakFile(const String& path);
	UInt32 LoadSpriteFromFile(const String& path);

	Bool Contains(const String& texture);

	UInt32 Add(Sprite* s);

	LPDIRECT3DTEXTURE9	GetTexture(const String& texture);
	LPDIRECT3DTEXTURE9	GetTexture(UInt32 id);

	Sprite& GetSprite(const String& texture);
	inline Sprite& GetSprite(UInt32 id);

	UInt32 GetID(const String& texture);

	void DrawSpriteEffect(UInt32 id, UInt32 shader, D3DXHANDLE tech = NULL, Float x = 0.0f, Float y = 0.0f, Float z = 0.5f, Float angle = 0.0f, Float scale = 1.0f, UInt32 mask = 0xFFFFFFFF);
	void RenderSprite(UInt32 id, Float x = 0.0f, Float y = 0.0f, Float z = 0.5f, Float angle = 0.0f, Float scale = 1.0f, UInt32 mask = 0xFFFFFFFF);
	void DrawSprite(UInt32 id, Float x = 0.0f, Float y = 0.0f, Float z = 0.5f, Float angle = 0.0f, Float scale = 1.0f, UInt32 mask = 0xFFFFFFFF);
	void RenderUISprite(UInt32 id, Float x = 0.0f, Float y = 0.0f, Float z = 0.0001f, Float angle = 0.0f, Float scale = 1.0f, UInt32 mask = 0xFFFFFFFF);

	void RenderSpriteEffect(UInt32 id, UInt32 shaderID, D3DXHANDLE tech, Float x = 0.0f, Float y = 0.0f, Float z = 0.5f, Float angle = 0.0f, Float scale = 1.0f, UInt32 mask = 0xFFFFFFFF);

	void DrawUIElement(UIObject* pObj);

	void RenderSprites();
	void RenderUI();

	Bool IsEmpty()
	{
		return (m_UIObjectList.IsEmpty()	&&
				m_UISpriteList.IsEmpty()	&&
				m_DrawList.IsEmpty()		&&
				m_RenderList.IsEmpty());
	}

	void Clear();

	UInt32 DebugGetUIObjectListSize() const
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
	}

private:

	friend class Map;

	void DrawList();
	void RenderList();
};

#define g_SpriteMgr	SpriteMgr::GetInstance()