/////////////////////////////////////////////////////////////////////////////
//		File:			UIButton.h
//
//		Programmmer:	Garrett Miller (GM)
//
//		Date Created:	2/21/2006 12:47:46 AM
//
//		Description:	A general push-button	
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "uiobject.h"
#include "UIImage.h"
//#include "UIText.h"

class UIText;
//////////////////////////////////////////////////////////////////////////

class UIButton : public UIObject
{
	UIImage		m_Up;
	UIImage		m_Down;
	UIImage		m_Over;

	UIText*		m_Caption;

public:
	enum EDrawMode
	{
		eDM_Up,
		eDM_Over,
		eDM_Down,

		e_DM_Max
	}	m_eMode;

	UIButton(void);
	virtual ~UIButton(void);

	virtual Bool	Update();

	void	Draw();
	void	SetMode(EDrawMode mode);
	void	LoadUpImage(String file);
	void	LoadDownImage(String file);
	void	LoadOverImage(String file);

	void	SetCaption(String text);
	String	GetCaption() const				{ return /*m_Caption->GetText()*/String(""); }

private:
	void	Render();

};
