/////////////////////////////////////////////////////////////////////////////
//		File:			UICounter.h
//
//		Programmmer:	Garrett Miller (GM)
//
//		Date Created:	7/21/2008 16:28:56
//
//		Description:	A text object with an internal counter
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "UIText.h"

//////////////////////////////////////////////////////////////////////////

class UICounter : public UIText
{
	Var		m_Value;

	void	Render();

public:
	UICounter(String text = "");
	virtual ~UICounter(void);

	Bool	Update();

	void	Draw();

	UIText&	operator = (String text);
	UIText& operator = (Int32 number);
	UIText& operator = (Float number);
	UIText& operator +=(String text);
	UIText& operator +=(Int32 number);
	UIText& operator +=(Float number);
};
