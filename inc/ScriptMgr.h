/////////////////////////////////////////////////////////////////////////////
//		File:			ScriptMgr.h
//
//		Programmmer:	Garrett Miller (GM)
//
//		Date Created:	2/21/2006 12:51:34 AM
//
//		Description:	A manager that handles loading and storing of scripts
//						as well as acting as a virtual machine to execute the
//						script commands.
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

//#include "Entity.h"
#include "Singleton.h"

#include "UIObject.h"
#include "UIImage.h"
#include "UIButton.h"

#include "Queue.h"
#include "Stack.h"

#include "Timer.h"

#include "variable.h"

//////////////////////////////////////////////////////////////////////////

#define MAX_BUFFER_SIZE						256
#define MAX_TOKEN_SIZE						128
#define MAX_MSG_DATA_SIZE					4
#define MAX_SCRIPT_NAME_LENGTH				MAX_TOKEN_SIZE

///////////////////////////////////////////////////////////////////////////////

enum EScriptCommand
{
	eCmd_Spawn,

	eCmd_Wait,

	eCmd_Send_Entity_Msg,

	eCmd_Script_Load,
	eCmd_Script_Run,

	eCmd_Menu_Cmd,

	////////////////////////////
	// Begin New Script Commands

	eCmd_Variable_Assign,
	eCmd_Variable_Increment,
	eCmd_Variable_Decrement,
	eCmd_Variable_Add,
	eCmd_Variable_Subtract,
	eCmd_Variable_Multiply,
	eCmd_Variable_Divide,

	// End New Script Commands
	////////////////////////////

	eCmd_MAX
};

//////////////////////////////////////////////////////////////////////////

class ScriptCommand
{
public:
	ScriptCommand()
	{ 
		//ZeroMemory(&m_Args[0], sizeof(Int32));
		m_Args.Resize(5);
	}

	Bool operator == (const ScriptCommand& sc) const
	{
		return (m_eMsg == sc.m_eMsg);
	}

	Bool operator != (const ScriptCommand& sc) const	{	return !((*this) == sc);	}

	EScriptCommand			m_eMsg;

	TArgumentArray			m_Args;
	
// 	union	
// 	{
// 		Bool					bData1[MAX_MSG_DATA_SIZE];
// 		Float					fData1[MAX_MSG_DATA_SIZE];
// 		UInt32					uData1[MAX_MSG_DATA_SIZE];
// 		Int32					nData1[MAX_MSG_DATA_SIZE];
// 		char					strData1[MAX_MSG_DATA_SIZE][MAX_BUFFER_SIZE];
// 		EntityPtr				pData1[MAX_MSG_DATA_SIZE];
// 		ENTITY_MESSAGE_ITEM		eData1[MAX_MSG_DATA_SIZE];
// 	};

// 	union	
// 	{
// 		Bool					bData2[MAX_MSG_DATA_SIZE];
// 		Float					fData2[MAX_MSG_DATA_SIZE];
// 		UInt32					uData2[MAX_MSG_DATA_SIZE];
// 		Int32					nData2[MAX_MSG_DATA_SIZE];
// 		char					strData2[MAX_MSG_DATA_SIZE][MAX_BUFFER_SIZE];
// 		EntityPtr				pData2[MAX_MSG_DATA_SIZE];
// 		ENTITY_MESSAGE_ITEM		eData2[MAX_MSG_DATA_SIZE];
// 	};
};

//////////////////////////////////////////////////////////////////////////

struct Script
{
	Script() : m_bRun(false), m_bWaiting(false), m_uFrameCounter(0), m_bMenuScript(false), m_bQueued(false)
	{ m_tTimer.Init();	}

	Script(String name) : m_bRun(false), m_bWaiting(false), m_uFrameCounter(0), m_bMenuScript(false), m_bQueued(false)
	{
		m_tTimer.Init();
		strncpy_s(m_strName, MAX_SCRIPT_NAME_LENGTH, name.c_str(), MAX_SCRIPT_NAME_LENGTH);
	}

	virtual ~Script()
	{	
		m_Commands.Clear();
	}

	Script& operator = (const Script& rhs)
	{
		strncpy_s(m_strName, MAX_SCRIPT_NAME_LENGTH, rhs.m_strName, MAX_SCRIPT_NAME_LENGTH);
		strncpy_s(m_strFile, MAX_SCRIPT_NAME_LENGTH, rhs.m_strFile, MAX_SCRIPT_NAME_LENGTH);

		m_bMenuScript = rhs.m_bMenuScript;

		UInt32 count = rhs.m_Commands.Size();

		for (UInt32 sci = 0; sci < count; sci++)
		{
			ScriptCommand sc;
			sc = rhs.m_Commands[sci];
			m_Commands.Enqueue(sc);
		}

		return *this;
	}

	virtual Int32 Update();

	char	m_strName[MAX_SCRIPT_NAME_LENGTH];
	char	m_strFile[MAX_SCRIPT_NAME_LENGTH];

	Queue<ScriptCommand>		m_Commands;
	
	Bool	m_bRun;
	Bool	m_bQueued;
	Bool	m_bWaiting;
	Bool	m_bMenuScript;

	UInt32	m_uFrameCounter;
	Timer	m_tTimer;
};

//////////////////////////////////////////////////////////////////////////

struct MenuScript : public Script
{
	MenuScript() : Script(), m_bDone(false)
	{
		m_bMenuScript = true;
	}

	MenuScript(String name) : Script(name), m_bDone(false)
	{
		m_bMenuScript = true;
	}

	virtual ~MenuScript()
	{
	}

	void Clear()
	{
		UIObject* obj = NULL;
		DblLinkedList<UIObject *>::Iterator	i(m_Objects);

		for (i.Begin(); !i.End(); ++i)
		{
			obj = i.Current();

			ASSERT(obj);

			delete obj;
		}

		m_Objects.Clear();
	}

	MenuScript& operator = (MenuScript& rhs)
	{
		Script::operator=(rhs);

		UInt32 count = rhs.m_Objects.Size();

		for (UInt32 obj = 0; obj < count; obj++)
		{
			UIObject* pObj = rhs.m_Objects[obj];
			ASSERT(pObj);
			m_Objects.Add(pObj);
		}

		return *this;
	}

	Int32		Update();

	Bool	m_bDone;

	DblLinkedList<UIObject*> m_Objects;
};

//////////////////////////////////////////////////////////////////////////

class ScriptMgr : public Singleton<ScriptMgr>
{
#ifdef LOAD_SCRIPTS_FROM_PAK
	//DPakFile		m_File;
	void ReadString(Int8* Buffer, DPakFile& m_File);
#else
	//FILE*			m_File;
	void ReadString(Int8* Buffer, FILE* m_File);
#endif

public:
	ScriptMgr(void);
	virtual ~ScriptMgr(void);

	Int32			Init();
	Int32			Update(Float dt);
	Int32			Shutdown();

	Bool			Contains(String FileName, Script** scr = NULL);

	Int32			RunScript(String ScriptName, Bool RunOnce = true);
	Int32			RunScript(Script *script, Bool RunOnce = true);

	Script*			LoadScript(String FileName);

	void			KillAllMenuScripts();

	Queue<Script *>					m_ScriptsToRun;
	DblLinkedList<Script *>			m_ScriptsLoaded;

	DblLinkedList<MenuScript *>		m_MenuScriptsLoaded;
	Queue<MenuScript *>				m_MenuScriptsToRun;
};

#define g_ScriptMgr	ScriptMgr::GetInstance()