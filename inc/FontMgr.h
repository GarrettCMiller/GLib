///////////////////////////////////////////////////////////////////////////
//		File:				FontMgr.h
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		A manager that creates and stores D3D fonts.
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "Singleton.h"
#include "Font.h"

#include "DynamicArray.h"

//////////////////////////////////////////////////////////////////////////

class FontMgr : public Singleton<FontMgr>
{
public:
	typedef CDynamicArray<Font*>	TFontList;

private:
	TFontList	m_Fonts;

public:
	FontMgr(void);
	~FontMgr(void);

	Bool	Contains(Font* font, UInt32* pIndex = NULL) const;

	UInt32	LoadFontSet(UInt32 height = 14, 
						Bool bBold = false, Bool bItalic = false, 
						String fontname = "Comic Sans MS");

	Font&	GetByID(UInt32 id);
};

#define g_FontMgr	FontMgr::GetInstance()