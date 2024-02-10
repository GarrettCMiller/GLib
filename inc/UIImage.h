/////////////////////////////////////////////////////////////////////////////
//		File:			UIImage.h
//
//		Programmmer:	Garrett Miller (GM)
//
//		Date Created:	2/21/2006 12:47:36 AM
//
//		Description:	A UI image class	
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "uiobject.h"

//////////////////////////////////////////////////////////////////////////

class UIImage : public UIObject
{
	UInt32			m_uImageID;
	UInt32			m_uAlpha;

public:
	UIImage(String text = "");
	virtual ~UIImage(void);

	Bool	Update();

	void	SetImageID(UInt32 imageID)		{ m_uImageID = imageID; }
	UInt32	GetImageID()	const			{ return m_uImageID; }

	void	LoadImage(String file);
	void	Draw();

	void	SetAlpha(BYTE val)				{m_uAlpha = val;}
	UInt32	GetAlpha()		const			{ return m_uAlpha; }

private:
	void	Render();
};
