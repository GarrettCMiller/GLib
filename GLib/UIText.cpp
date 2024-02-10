/////////////////////////////////////////////////////////////////////////////
//		File:			UIText.cpp
//
//		Programmmer:	Garrett Miller (GM)
//
//		Date Created:	2/21/2006 12:46:54 AM
//
//		Description:	A text object	
//
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include ".\uitext.h"
#include "D3DWrapper.h"
#include "SpriteMgr.h"
#include "FontMgr.h"

#include "Rect.h"

#include "ProfileSample.h"

using namespace GLib;
//////////////////////////////////////////////////////////////////////////

UIText::UIText(String text) : 
	m_Alignment(DefaultAlignment),
	m_Color(0xFFFFFFFF),
	m_Font(0),
	m_bMultiline(false),
	m_FloatDecimalPrecision(8)
{
	if (!text.empty())
		SetText(text);
}

//////////////////////////////////////////////////////////////////////////

UIText::~UIText(void)
{
}

//////////////////////////////////////////////////////////////////////////

Bool UIText::Update()
{
	PROFILE_THIS;

	if (m_strText == "")
		return false;

	return UIObject::Update();
}

//////////////////////////////////////////////////////////////////////////

void UIText::Render()
{
	PROFILE_THIS;

	RECT rect = Rectf(m_Position.X, m_Position.Y, m_Position.X + m_Size.X, m_Position.Y + m_Size.Y);
// 	{
// 		m_Position.X,				m_Position.Y, 
// 		m_Position.X + m_Size.X,	m_Position.Y + m_Size.Y
// 	};

	UInt32 flags = DT_NOCLIP | DT_TOP;

	if (!m_bMultiline)
		flags |= DT_SINGLELINE;

	switch (m_Alignment)
	{
	case Left:
		break;

	case Center:
		flags |= DT_CENTER;
		break;

	case Right:
		flags |= DT_RIGHT;
	}

	Font& font = g_FontMgr->GetByID(m_Font);

	/*VERIFY((font.m_Font->DrawText(g_D3D->GetSpriteObject(),
											m_strText.c_str(), UInt32(m_strText.length()),
											&rect, 
											flags,
											m_Color)) > 0);*/

	static LPD3DXSPRITE k_sprt = g_D3D->GetSpriteObject();

	font.m_Font->DrawText(k_sprt,
							m_strText.c_str(), UInt32(m_strText.length()),
							&rect, 
							flags,
							m_Color);
}

//////////////////////////////////////////////////////////////////////////

void UIText::Draw()
{
	g_SpriteMgr->m_UIObjectList.Add(this);
}

//////////////////////////////////////////////////////////////////////////

void UIText::SetFont(UInt32 font)
{
	m_Font = font;
}

//////////////////////////////////////////////////////////////////////////

UInt32 UIText::GetFont() const
{
	return m_Font;
}

//////////////////////////////////////////////////////////////////////////

void UIText::SetText(String text)
{
	m_strText = text;
}

//////////////////////////////////////////////////////////////////////////

//void UIText::GetText(char *text) const
String UIText::GetText() const
{
	return m_strText;
}

//////////////////////////////////////////////////////////////////////////

void UIText::SetColor(UInt32 color)
{
	m_Color = color;
}

//////////////////////////////////////////////////////////////////////////

void UIText::SetColor(BYTE red, BYTE green, BYTE blue, BYTE alpha)
{
	m_Color = RGB32(alpha, red, green, blue);
}

//////////////////////////////////////////////////////////////////////////

void UIText::SetColor(Color color)
{
	m_Color = color;
}

//////////////////////////////////////////////////////////////////////////

UInt32 UIText::GetColor() const
{
	return m_Color;
}

//////////////////////////////////////////////////////////////////////////

Bool UIText::IsMultiline() const
{
	return m_bMultiline;
}

//////////////////////////////////////////////////////////////////////////

void UIText::SetMultiline(Bool bMultiline /* = true */)
{
	m_bMultiline = bMultiline;
}

//////////////////////////////////////////////////////////////////////////

void UIText::SetAlignment(UIText::TextAlignment alignment)
{
	m_Alignment = alignment;
}

//////////////////////////////////////////////////////////////////////////

UIText::TextAlignment UIText::GetAlignment() const
{
	return m_Alignment;
}

//////////////////////////////////////////////////////////////////////////

UInt8 UIText::GetFloatPrecision() const
{
	return m_Alignment;
}

//////////////////////////////////////////////////////////////////////////

void UIText::SetFloatPrecision(UInt8 prec /* = 8 */)
{
	if (prec > 16)
		prec = 16;

	m_FloatDecimalPrecision = prec;
}

//////////////////////////////////////////////////////////////////////////

UIText&	UIText::operator = (String text)
{
	SetText(text);
	return *this;
}

//////////////////////////////////////////////////////////////////////////

UIText& UIText::operator = (Int32 number)
{
	//m_strText += itoa_s(number;
	return *this;
}

//////////////////////////////////////////////////////////////////////////

UIText& UIText::operator = (Float number)
{
	//m_strText += number;
	return *this;
}

//////////////////////////////////////////////////////////////////////////

UIText& UIText::operator +=(String text)
{
	m_strText += text;
	return *this;
}

//////////////////////////////////////////////////////////////////////////

UIText& UIText::operator +=(Int32 number)
{
	m_strText += _itoa(number, g_ErrorBuffer, 10);;
	return *this;
}

//////////////////////////////////////////////////////////////////////////

UIText& UIText::operator +=(Float number)
{
	m_strText += _gcvt(number, m_FloatDecimalPrecision, g_ErrorBuffer);
	return *this;
}

//////////////////////////////////////////////////////////////////////////