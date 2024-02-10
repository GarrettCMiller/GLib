/////////////////////////////////////////////////////////////////////////////
//		File:			UIButton.cpp
//
//		Programmmer:	Garrett Miller (GM)
//
//		Date Created:	2/21/2006 12:47:49 AM
//
//		Description:	A general push-button	
//
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include ".\uibutton.h"
#include "SpriteMgr.h"
#include "DirectInput.h"

#include "Rect.h"

using namespace GLib;
//////////////////////////////////////////////////////////////////////////

UIButton::UIButton(void)
{
}

//////////////////////////////////////////////////////////////////////////

UIButton::~UIButton(void)
{
}

//////////////////////////////////////////////////////////////////////////

Bool UIButton::Update()
{
	static Bool Clicked = false;

	//m_Caption.SetPosition(m_Position);

	Vec2i mouse = g_Input->GetMousePos();
	Rect rect = Rect(m_Position, m_Position + m_Size);

	if (rect.PointInRect(mouse))
	{
		if (g_Input->MouseDown(DirectInput::eLeftButton))
		{
			Clicked = true;
			SetMode(eDM_Down);
		}
		else
		{
			if (Clicked ==  true)
			{
				TriggerScript();
			}

			Clicked = false;
			SetMode(eDM_Over);
		}
	}
	else
	{
		//Clicked = false;
		SetMode(eDM_Up);
	}

	//m_Caption.Update();

	UIObject::Update();

	return true;
}

//////////////////////////////////////////////////////////////////////////

void UIButton::Render()
{
}

//////////////////////////////////////////////////////////////////////////

void UIButton::Draw()
{
	UIImage* image = NULL;

	switch (m_eMode)
	{
	case eDM_Up:
		image = &m_Up;
		break;

	case eDM_Down:
		image = &m_Down;
		break;

	case eDM_Over:
		image = &m_Over;
		break;
	}

	image->SetPosition(m_Position);
	image->Update();
}

//////////////////////////////////////////////////////////////////////////

void UIButton::SetMode(EDrawMode mode)
{
	m_eMode = mode;
}

//////////////////////////////////////////////////////////////////////////

void UIButton::LoadUpImage(String file)
{
	m_Up.LoadImage(file);

	Sprite& img = g_SpriteMgr->GetSprite(m_Up.GetImageID());

	m_Size = Vec2(static_cast<Float>(img.width), static_cast<Float>(img.height));
}

//////////////////////////////////////////////////////////////////////////

void UIButton::LoadDownImage(String file)
{
	m_Down.LoadImage(file);
}

//////////////////////////////////////////////////////////////////////////

void UIButton::LoadOverImage(String file)
{
	m_Over.LoadImage(file);
}

//////////////////////////////////////////////////////////////////////////

void UIButton::SetCaption(String text)
{
	//m_Caption.SetText(text);
}

//////////////////////////////////////////////////////////////////////////