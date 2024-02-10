/////////////////////////////////////////////////////////////////////////////
//		File:			UIText.h
//
//		Programmmer:	Garrett Miller (GM)
//
//		Date Created:	2/21/2006 12:47:56 AM
//
//		Description:	A text object
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "uiobject.h"
#include "DataTypes.h"

//////////////////////////////////////////////////////////////////////////

class UIText : public UIObject
{
public:
	enum TextAlignment
	{
		Left,
		Center,
		Right,

		DefaultAlignment = Center
	};

private:
	static const UInt32		ms_uMaxChars = 512;

	String					m_strText;
	TextAlignment			m_Alignment;
	UInt32					m_Color;

	UInt8					m_FloatDecimalPrecision;

	Bool					m_bMultiline;

	UInt32					m_Font;

	void	Render();

public:
	UIText(String text = "");
	virtual ~UIText(void);

	Bool	Update();

	void	Draw();

	void	SetText(String text);
	//void	GetText(char *text)	const;
	String	GetText() const;

	void	SetFont(UInt32 font);
	UInt32	GetFont() const;

	void	SetColor(UInt32 color);
	void	SetColor(BYTE red, BYTE green, BYTE blue, BYTE alpha = 255);
	void	SetColor(Color color);
	UInt32	GetColor()	const;

	Bool	IsMultiline() const;
	void	SetMultiline(Bool bMultiline = true);

	UInt8	GetFloatPrecision() const;
	void	SetFloatPrecision(UInt8 prec = 8);

	UIText&	operator = (String text);
	UIText& operator = (Int32 number);
	UIText& operator = (Float number);
	UIText& operator +=(String text);
	UIText& operator +=(Int32 number);
	UIText& operator +=(Float number);

	void			SetAlignment(TextAlignment alignment);
	TextAlignment	GetAlignment()	const;
};
