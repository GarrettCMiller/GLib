///////////////////////////////////////////////////////////////////////////
//		File:				Font.h
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		A wrapper for a D3D font
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "d3d9.h"
#include "d3dx9core.h"

//////////////////////////////////////////////////////////////////////////

class Font
{
public:
	LPD3DXFONT		m_Font;
	
	Bool			m_bBold;
	Bool			m_bItalic;
	UInt32			m_Height;

	String			m_Name;

public:
	Font(UInt32 height = 14, Bool bBold = false, Bool bItalic = false, String font = "Comic Sans MS");
	~Font(void);

	HRESULT Load(UInt32 height = 14, 
				Bool bBold = false, Bool bItalic = false, 
				String font = "Comic Sans MS");

	Bool operator == (const Font& rhs) const;
	Bool operator != (const Font& rhs) const;
};
