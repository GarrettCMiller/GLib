/////////////////////////////////////////////////////////////////////////////
//		File:			UIObject.cpp
//
//		Programmmer:	Garrett Miller (GM)
//
//		Date Created:	2/21/2006 12:47:31 AM
//
//		Description:	Base UI object	
//
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include ".\uiobject.h"

//#include "ScriptMgr.h"

//////////////////////////////////////////////////////////////////////////

UIObject::UIObject(void)
{
	m_bVisible		= true;

	m_pScriptToRun	= NULL;
	m_pParentScript	= NULL;
}

//////////////////////////////////////////////////////////////////////////

UIObject::~UIObject(void)
{
}

//////////////////////////////////////////////////////////////////////////

Bool UIObject::Update()
{
	if (m_bVisible)
		Draw();

	return true;
}

//////////////////////////////////////////////////////////////////////////

void UIObject::SetScript(Script *script)
{
	ASSERT(script);

	m_eMenuAction = eMA_RunScript;

	m_pScriptToRun = script;
}

//////////////////////////////////////////////////////////////////////////

void UIObject::SetScript(String file)
{
	ASSERT(!file.empty());

	m_eMenuAction = eMA_RunScript;

	//m_pScriptToRun = g_ScriptMgr->LoadScript(file);
}

//////////////////////////////////////////////////////////////////////////

Script* UIObject::GetScript() const
{
	return m_pScriptToRun;
}

//////////////////////////////////////////////////////////////////////////

void UIObject::TriggerScript()
{
	switch (m_eMenuAction)
	{
	case eMA_RunScript:
		//g_ScriptMgr->RunScript(m_pScriptToRun);
		break;

	case eMA_Back:
		{
// 			MenuScript* ms = g_ScriptMgr->m_MenuScriptsToRun[0];
// 			ms->m_bDone = true;
			break;
		}

	case eMA_Exit:
		//Game::CloseGame();
		break;
	}
}

//////////////////////////////////////////////////////////////////////////