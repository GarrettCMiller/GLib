///////////////////////////////////////////////////////////////////////////
//		File:				Font.cpp
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		A D3D font wrapper
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "Font.h"
#include "D3DWrapper.h"

//////////////////////////////////////////////////////////////////////////

Font::Font(UInt32 height /* = 14 */, 
		   Bool bBold /* = false */, Bool bItalic /* = false */,
		   String font /* = "Comic Sans MS" */) 
		   : m_Height(height), m_bBold(bBold), m_bItalic(bItalic),
		    m_Name(font)
{
	m_Font = NULL;
}

//////////////////////////////////////////////////////////////////////////

Font::~Font(void)
{
	SAFE_RELEASE(m_Font);
}

//////////////////////////////////////////////////////////////////////////

HRESULT Font::Load(UInt32 height, Bool bBold, Bool bItalic, String font)
{
	SAFE_RELEASE(m_Font);

	return D3DXCreateFont(g_D3D->GetDevice(), 
							height, 0, 
							(bBold ? FW_BOLD : FW_NORMAL), 0, 
							bItalic, DEFAULT_CHARSET, 
							OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, 
							DEFAULT_PITCH | FF_DONTCARE,
							font.c_str(), &m_Font);
}

//////////////////////////////////////////////////////////////////////////

Bool Font::operator == (const Font& rhs) const
{
	Bool ret = false;

	ret = (	m_bBold	== rhs.m_bBold &&
			m_bItalic == rhs.m_bItalic &&
			m_Height == rhs.m_Height &&
			m_Name == rhs.m_Name);

	return ret;
}

//////////////////////////////////////////////////////////////////////////

Bool Font::operator != (const Font& rhs) const
{
	return !((*this) == rhs);
}