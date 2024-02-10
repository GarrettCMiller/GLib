/////////////////////////////////////////////////////////////////////////////
//		File:			UIImage.cpp
//
//		Programmmer:	Garrett Miller (GM)
//
//		Date Created:	2/21/2006 12:47:41 AM
//
//		Description:	A UI Image class	
//
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include ".\uiimage.h"
#include "SpriteMgr.h"
#include "DPakFile.h"

//////////////////////////////////////////////////////////////////////////

UIImage::UIImage(String text) : m_uImageID(-1)
{
	m_uAlpha = 255;

	if (!text.empty())
		LoadImage(text);
}

//////////////////////////////////////////////////////////////////////////

UIImage::~UIImage(void)
{
}

//////////////////////////////////////////////////////////////////////////

Bool UIImage::Update()
{
	if (m_bVisible)
		Draw();

	return true;
}

//////////////////////////////////////////////////////////////////////////

void UIImage::Render()
{
	g_SpriteMgr->RenderUISprite(m_uImageID, m_Position.X, m_Position.Y, 0.001f, 0.0f, 1.0f, ALPHA(m_uAlpha));
}

//////////////////////////////////////////////////////////////////////////

void UIImage::Draw()
{
	ASSERT(m_uImageID != -1);
	ASSERT(m_uImageID != 0xCDCDCDCD);
	g_SpriteMgr->DrawUIElement(this);
}

//////////////////////////////////////////////////////////////////////////

void UIImage::LoadImage(String file)
{
	char path[64] = {'\0'};

#ifndef LOAD_IMAGES_FROM_PAK
	strcpy(path, "UI\\");
	strncat(path, file.c_str(), 64);
#else
	strncpy_s(path, file.c_str(), 64);
#endif

	m_uImageID = g_SpriteMgr->LoadSprite(path);

	ASSERT(m_uImageID != -1);

	Sprite& img = g_SpriteMgr->GetSprite(m_uImageID);

	m_Size = Vec2(static_cast<Float>(img.width), static_cast<Float>(img.height));
}

//////////////////////////////////////////////////////////////////////////