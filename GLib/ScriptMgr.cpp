/////////////////////////////////////////////////////////////////////////////
//		File:			ScriptMgr.cpp
//
//		Programmmer:	Garrett Miller (GM)
//
//		Date Created:	2/21/2006 12:51:38 AM
//
//		Description:	A manager that handles loading and storing of scripts
//						as well as acting as a virtual machine to execute the
//						script commands.
//
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include ".\scriptmgr.h"

#include "D3DWrapper.h"

#ifdef _DOOMEROIDS_
	#include "Entity.h"
	#include "EntityMgr.h"
	#include "Map.h"
	#include "MapMgr.h"
#endif

#include "MiscMath.h"
#include "ProfileSample.h"
#include "Globals.h"

#include "..\..\Doomeroids Pack\Doomeroids\GameScript.h"

///////////////////////////////////////////////////////////////////////////////

#define EDIT_STR	\
	{\
		if (strlen(Buffer) > 1) { \
		temp = strchr(Buffer, '\r');\
		if (temp) *temp = '\0';\
		temp = strchr(Buffer, '\n');\
		if (temp) *temp = '\0';}\
	}	

#define TOKEN_IS(_phrase)			(!strcmp(Token, _phrase))

#define NEXT_TOKEN					strtok_s(NULL, delim, &Context)
#define GET_NEXT_TOKEN				Token = NEXT_TOKEN

///////////////////////////////////////////////////////////////////////////////
static String  ScriptsBasePath = "Res\\Scripts\\";

IMPLEMENT_SINGLETON(ScriptMgr);

const std::string strScriptCommands[eCmd_MAX] =
{
	"Spawn",

	"Wait",

	"SendEntityMessage",

	"LoadScript",
	"RunScript",

	"MenuCommand",

	////
	// New ops here
	//
	////
};

//////////////////////////////////////////////////////////////////////////

ScriptMgr::ScriptMgr(void)
{
}

/////////////////////////////////////////////////////////////////////////////

ScriptMgr::~ScriptMgr(void)
{
	if (!Shutdown())
		g_Log.LOG_ERROR("Error shutting Script Manager down!\n");
}

/////////////////////////////////////////////////////////////////////////////

#ifdef LOAD_SCRIPTS_FROM_PAK
void ScriptMgr::ReadString(Int8* Buffer, DPakFile& m_File)
#else
void ScriptMgr::ReadString(Int8* Buffer, FILE* m_File)
#endif
{
#ifndef LOAD_SCRIPTS_FROM_PAK
	
	fgets(Buffer, MAX_BUFFER_SIZE, m_File);

#else
	
	UInt32	BytesRead = 0;
	UInt8	LastByte = '\0';

	ASSERT(!m_File.EoF());

	do 
	{
		m_File.Read(Buffer + BytesRead++, 1);
		LastByte = Buffer[BytesRead - 1];

		if ((LastByte == '\n' || LastByte == '\r') && BytesRead == 1)
		{
			LastByte = '\0';
			BytesRead = 0;
			continue;
		}
	} 
	while (BytesRead < MAX_BUFFER_SIZE && LastByte != '\n' && LastByte != '\r' && !m_File.EoF());

	Buffer[BytesRead] = '\0';
	//Buffer[BytesRead + 1] = '\0';

#endif
}

/////////////////////////////////////////////////////////////////////////////

Int32 ScriptMgr::Init()
{
	g_Log << "ScriptMgr::Init() begin...\n";

	g_Log << "ScriptMgr::Init() end!\n";

	return 1;
}

/////////////////////////////////////////////////////////////////////////////

Int32 ScriptMgr::Update(Float /*dt*/)
{
	PROFILE_THIS;

	Script	*scr = NULL;
	MenuScript *menu = NULL;

	Int32 retVal = 0;

	if (!m_MenuScriptsToRun.IsEmpty())
	{
		UInt32 index = m_MenuScriptsToRun.Size() - 1;

		menu = m_MenuScriptsToRun[index];

		if (menu->Update() < 0)
		{
			//menu = m_MenuScriptsToRun.Pop();
			m_MenuScriptsToRun.Dequeue(menu);
			delete menu;
		}
	}
	else
		retVal = 1;
	
	if (!m_ScriptsToRun.IsEmpty())
	{
		for (UInt32 s = 0; s < m_ScriptsToRun.Size(); s++)
		{
			scr = m_ScriptsToRun[s];

			if (scr->Update() < 0)
			{
				m_ScriptsToRun.Remove(scr);
				delete scr;
			}
			else
				retVal = 0;
		}
	}

	return retVal;
}

/////////////////////////////////////////////////////////////////////////////

Int32 ScriptMgr::Shutdown()
{
	g_Log << "ScriptMgr::Shutdown() begin...\n";

	char scrname[80] = {'\0'};

	DblLinkedList<Script *>::Iterator		i(m_ScriptsLoaded);
	Script*									script = NULL;

	UInt32 count = m_MenuScriptsToRun.Size();
	for (UInt32 it = 0; it < count; it++)
	{
		script = m_MenuScriptsToRun[it];
		delete script;
	}

	count = m_ScriptsToRun.Size();
	for (UInt32 it = 0; it < count; it++)
	{
		script = m_ScriptsToRun[it];
		delete script;
	}

	count = m_ScriptsLoaded.Size();

	for (i.Begin(); !i.End(); ++i)
	{
		script = i.Current();

		strncpy_s(scrname, 80, script->m_strFile, 80);
		
		if (script->m_bMenuScript)
			(static_cast<MenuScript *>(script))->Clear();

		delete script;

		g_ResLog << "Freed script " << scrname << "!\n";
	}

	g_Log << "Freed " << count << " scripts!\n";
	g_ResLog << "Freed " << count << " scripts!\n";

	m_ScriptsLoaded.Clear();
	m_ScriptsToRun.Clear();

	g_Log << "ScriptMgr::Shutdown() end!\n";

	return 1;
}

/////////////////////////////////////////////////////////////////////////////

Int32 ScriptMgr::RunScript(Script *script, Bool RunOnce)
{
	DblLinkedList<Script *>::Iterator		i(m_ScriptsLoaded);
	Script						*temp = NULL;

	for (i.Begin(); !i.End(); ++i)
	{
		temp = i.Current();

		//Compare names
		if (script == temp && !temp->m_bQueued && (RunOnce ? !temp->m_bRun : true))
		{
			Script* nc;

			if (temp->m_bMenuScript)
			{
				nc = new MenuScript;

				ASSERT(nc);

				*((MenuScript*) nc) = *((MenuScript*) temp);
			}
			else
			{
				nc = NULL;//new CGameScript;

				ASSERT(nc);

				(*nc) = (*temp);
			}

			nc->m_bRun = false;
			nc->m_bQueued = true;
			
			if (nc->m_bMenuScript)
				m_MenuScriptsToRun.Enqueue((MenuScript *) nc);//Push((MenuScript *) nc);
			else
				m_ScriptsToRun.Enqueue(nc);

			g_Log << "Running script " << script->m_strName << "...\n";
			
			return 1;
		}
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////

Int32 ScriptMgr::RunScript(String ScriptName, Bool RunOnce)
{
	DblLinkedList<Script *>::Iterator	i(m_ScriptsLoaded);
	Script						*temp = NULL;

	for (i.Begin(); !i.End(); ++i)
	{
		temp = i.Current();

		//Compare names
		if (!strcmp(ScriptName.c_str(), temp->m_strName) && !temp->m_bQueued && (RunOnce ? !temp->m_bRun : true))
		{
			Script* nc;

			if (temp->m_bMenuScript)
			{
				nc = new MenuScript;

				if (!nc)
					return -1;

				*((MenuScript*) nc) = *((MenuScript*) temp);
			}
			else
			{
				nc = NULL;//new CGameScript;

				if (!nc)
					return -1;

				(*nc) = (*temp);
			}

			nc->m_bRun = false;
			nc->m_bQueued = true;
			
			if (nc->m_bMenuScript)
			{
				if (!m_MenuScriptsToRun.IsEmpty() && !strcmp(m_MenuScriptsToRun[UInt32(m_MenuScriptsToRun.Size()-1)]->m_strName, nc->m_strName))
					return -1;
				
				m_MenuScriptsToRun.Enqueue((MenuScript *) nc);//Push((MenuScript *) nc);
			}
			else
			{
				if (!m_ScriptsToRun.IsEmpty() && !strcmp(m_ScriptsToRun[UInt32(m_ScriptsToRun.Size()-1)]->m_strName, nc->m_strName))
					return -1;

				m_ScriptsToRun.Enqueue(nc);
			}
			
			g_Log << "Running script " << ScriptName << "...\n";

			return 1;
		}
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////

Bool ScriptMgr::Contains(String FileName, Script** scr)
{
	DblLinkedList<Script *>::Iterator		i(m_ScriptsLoaded);
	Script*									temp = NULL;

	for (i.Begin(); !i.End(); ++i)
	{
		temp = i.Current();

		if (!strcmp(FileName.c_str(), temp->m_strFile))
		{
			*scr = temp;
			return true;
		}
	}

	/*DblLinkedList<MenuScript *>::Iterator j(m_MenuScriptsLoaded);

	for (j.Begin(); !j.End(); ++j)
	{
		temp = j.Current();

		if (!strcmp(FileName, temp->m_strFile))
		{
			*scr = temp;
			return true;
		}
	}*/

	scr = NULL;

	return false;
}

/////////////////////////////////////////////////////////////////////////////

Script *ScriptMgr::LoadScript(String FileName)
{
	Script		*scr = NULL;
	MenuScript	*menu = NULL;

	Bool		menuScript = false;
	
#ifdef _DEBUG	//To get rid of warning
	Int32 ret;

	char errbuf[MAX_BUFFER_SIZE];
#endif

	char Buffer[MAX_BUFFER_SIZE];

	char *Token;
	char *Context;
	char *delim = "	";		//Tab and space

	char *temp = NULL;

	ASSERT(!FileName.empty());

	if (Contains(FileName, &scr))
	{
		g_Log << "Script already loaded! File: " << FileName << '\n';
		return scr;
	}

#ifndef LOAD_SCRIPTS_FROM_PAK

	FILE* m_File = NULL;

	char ScriptPath[256];

	strcpy_s(ScriptPath, ScriptsBasePath.c_str());
	strcat_s(ScriptPath, FileName.c_str());

	fopen_s(&m_File, ScriptPath, "rt");

	ASSERT(m_File);

#else

	DPakFile m_File;
	g_PakFile.GetFileByName(m_File, FileName.c_str());
	ASSERT(m_File.IsValid());
	ASSERT(!_stricmp(m_File.Name, FileName.c_str()));

#endif

	//Load in script name
	ReadString(Buffer, m_File);
	EDIT_STR;

	if (strstr(Buffer, "MENU"))
	{
		menuScript = true;

		menu = new MenuScript(Buffer);

		ASSERT(menu);

		scr = menu;
	}
	else
	{
		scr = NULL;//new CGameScript(Buffer);

		ASSERT(scr);
	}

	strncpy_s(scr->m_strFile, MAX_SCRIPT_NAME_LENGTH, FileName.c_str(), MAX_SCRIPT_NAME_LENGTH);

	ReadString(Buffer, m_File);		//Remove opening brace

#ifdef _DEBUG	//To get rid of warning
	tryopeningbrace:
#endif

	if (Buffer[0] != '{')
	{
#ifdef _DEBUG
		sprintf_s(errbuf, MAX_SCRIPT_NAME_LENGTH, 
						"Script error! Invalid token after script name! Expected '{'\n"
						"File: \t%s\n"
						"Shader: \t%s\n"
						"Token: \t%s\n\n"
						"Ignore token and continue parsing?",
						FileName,
						scr->m_strName,
						Buffer);

		ret = MessageBox(g_hWnd, errbuf, "Token Error", MB_YESNOCANCEL);

		switch (ret)
		{
		case IDYES:
			ReadString(Buffer, m_File);		//Hopefully there is a brace after the error token
			goto tryopeningbrace;

		case IDNO:
			return NULL;

		case IDCANCEL:
			//g_Game->CloseGame();
			return NULL;
		}
#else
		g_Log.LOG_ERROR("Bad script: " + FileName + "\n");
		return NULL;
#endif
	}

	ReadString(Buffer, m_File);
	EDIT_STR;

	Token = strtok_s(Buffer, delim, &Context);

	while (!m_File.EoF())
	{
		if (Token)
		{
			ScriptCommand		sc;

			do
			{
				//Remove comment lines
				if (TOKEN_IS("//") || strstr(Token, "//"))
					goto next;

				if (!menuScript)
				{

					if (TOKEN_IS("spawn"))
					{
						sc.m_eMsg = eCmd_Spawn;
						
						GET_NEXT_TOKEN;

						if (TOKEN_IS("soldier") || TOKEN_IS("sergeant") || TOKEN_IS("imp") || TOKEN_IS("cacodemon") || TOKEN_IS("baron") || TOKEN_IS("cyberdemon"))
						{
							sc.m_Args[0] = String(Token);

							GET_NEXT_TOKEN;

							Float xp,	yp;

							const CCameraView& cam = g_D3D->GetCamera();

							if (TOKEN_IS("width"))
								xp = fabsf(cam.GetPosition().z) * 0.5f;
							else if (TOKEN_IS("random"))
								xp = Random(50.0f, -50.0f);
							else
								xp = Float(atof(Token));
							
							GET_NEXT_TOKEN;

							if (TOKEN_IS("height"))
								yp = fabsf(cam.GetPosition().z) * 0.5f;
							else if (TOKEN_IS("random"))
								yp = Random(50.0f, -50.0f);
							else
								yp = Float(atof(Token));

							sc.m_Args[1] = xp;
							sc.m_Args[2] = yp;

							scr->m_Commands.Enqueue(sc);
						}
						else
						{
							sprintf_s(g_ErrorBuffer, ERROR_BUFFER_SIZE, "Bad spawn type! Type: %s\n", Token);
							g_Log.LOG_ERROR(g_ErrorBuffer);
						}
					}
					else if (TOKEN_IS("wait"))
					{
						GET_NEXT_TOKEN;

						if ((sc.m_Args[0] = Float(atof(Token))) == 0.0f)
							g_Log.LOG_ERROR("Bad wait time!\n");

						GET_NEXT_TOKEN;
						
						sc.m_eMsg = eCmd_Wait;
						
						if (TOKEN_IS("seconds"))
							sc.m_Args[1] = true;	
						else if (TOKEN_IS("frames"))
							sc.m_Args[1] = false;
						else
						{
							sprintf_s(g_ErrorBuffer, ERROR_BUFFER_SIZE, "Bad wait type! Type: %s\n", Token);
							g_Log.LOG_ERROR(g_ErrorBuffer);
						}

						scr->m_Commands.Enqueue(sc);
					}
					else if (TOKEN_IS("load_script"))
					{
						sc.m_eMsg = eCmd_Script_Load;

						GET_NEXT_TOKEN;

						sc.m_Args[0] = String(Token);

						scr->m_Commands.Enqueue(sc);
					}
					else if (TOKEN_IS("run_script"))
					{
						sc.m_eMsg = eCmd_Script_Run;

						GET_NEXT_TOKEN;

						sc.m_Args[0] = String(Token);

						scr->m_Commands.Enqueue(sc);
					}
					else if (TOKEN_IS("menu_command"))
					{
						GET_NEXT_TOKEN;

						sc.m_eMsg = eCmd_Menu_Cmd;

						sc.m_Args[0] = String(Token);
						
						scr->m_Commands.Enqueue(sc);
					}
#if 0
					else if (TOKEN_IS("send_ent_msg"))
					{
						sc.m_eMsg = eCmd_Send_Entity_Msg;

						GET_NEXT_TOKEN;

						//strncpy(sc.strData1[0], Token, MAX_BUFFER_SIZE);

						sc.pData1[0] = g_EntityMgr->GetByName(Token);

						GET_NEXT_TOKEN;

						for (UInt32 string = 0; string < NUM_ENTITY_MESSAGES; string++)
						{
							if (!strcmp(EntityMessageText[string], Token))
							{
								sc.eData2[0].eMsg = ENTITY_MESSAGE(string);
								goto found;
							}
						}

						ASSERT(false);
						//"Not a valid entity message! Msg: " << Token << '\n';
						goto next;

	found:
						MessageTranslator mt = EntityMessageTranslators[sc.eData2[0].eMsg];

						if (mt.m_uCount == 0 || mt.m_eDataType == MessageTranslator::DT_NONE)
						{
							scr->m_Commands.Enqueue(sc);
							goto next;
						}

						for (UInt32 times = 0; times < mt.m_uCount; times++)
						{
							GET_NEXT_TOKEN;

							switch (mt.m_eDataType)
							{
							case MessageTranslator::DT_ENTITY:
								sc.eData2[0].pData[times] = g_EntityMgr->GetByName(Token);
								break;

							case MessageTranslator::DT_FLOAT:
								sc.eData2[0].fData[times] = Float(atof(Token));
								break;

							case MessageTranslator::DT_INT:
								sc.eData2[0].nData[times] = atoi(Token);
								break;

							case MessageTranslator::DT_UINT:
								sc.eData2[0].uData[times] = UInt32(atoi(Token));
								break;

							default:
								ASSERT(false);
								g_Log.LOG_ERROR("BAD MESSAGE TRANSLATOR DATA TYPE!\n");
							}
						}

						scr->m_Commands.Enqueue(sc);

					}
#endif
					else
					{
						ASSERT(false);
						sprintf_s(g_ErrorBuffer, MAX_BUFFER_SIZE, "Not a valid script command! Command: %s\n", Token);
						g_Log.LOG_ERROR(g_ErrorBuffer);
						goto next;
					}
				}
				else
				{
					if (TOKEN_IS("image"))
					{
						GET_NEXT_TOKEN;

						UIImage *image = new UIImage(Token);

						ASSERT(image);

						GET_NEXT_TOKEN;

						Vec2 pos;

						if (TOKEN_IS("CENTER"))
						{
							pos.X = ((g_D3D->GetScreenWidth() / 2) - (image->GetSize().X / 2));

							GET_NEXT_TOKEN;

							if (TOKEN_IS("CENTER"))
								pos.Y = ((g_D3D->GetScreenHeight() / 2) - (image->GetSize().Y / 2));
							else
								pos.Y = Float(atoi(Token));
						}
						else
						{
							pos.X = Float(atoi(Token));

							GET_NEXT_TOKEN;

							if (TOKEN_IS("CENTER"))
								pos.Y = ((g_D3D->GetScreenHeight() / 2) - (image->GetSize().Y / 2));
							else
								pos.Y = Float(atoi(Token));
						}

						image->SetPosition(pos);

						image->SetParentScript(menu);

						menu->m_Objects.Add(image);
					}
					else if (TOKEN_IS("button"))
					{
						GET_NEXT_TOKEN;

						UIButton *button = new UIButton;
						
						ASSERT(button);

						button->SetCaption(Token);

						GET_NEXT_TOKEN;

						button->LoadUpImage(Token);

						GET_NEXT_TOKEN;

						button->LoadDownImage(Token);

						GET_NEXT_TOKEN;

						button->LoadOverImage(Token);

						GET_NEXT_TOKEN;

						Vec2 pos;

						if (TOKEN_IS("CENTER"))
						{
							pos.X = ((g_D3D->GetScreenWidth() / 2) - (button->GetSize().X / 2));

							GET_NEXT_TOKEN;

							if (TOKEN_IS("CENTER"))
								pos.Y = ((g_D3D->GetScreenHeight() / 2) - (button->GetSize().Y / 2));
							else
								pos.Y = Float(atoi(Token));
						}
						else
						{
							pos.X = Float(atoi(Token));

							GET_NEXT_TOKEN;

							if (TOKEN_IS("CENTER"))
								pos.Y = ((g_D3D->GetScreenHeight() / 2) - (button->GetSize().Y / 2));
							else
								pos.Y = Float(atoi(Token));
						}

						button->SetPosition(pos);

						GET_NEXT_TOKEN;

						UInt32 len = UInt32(strlen(Token));

						if (Token[len - 4] == '.')
						{
							button->SetScript(Token);
						}
						else
						{
							if (TOKEN_IS("exit"))
							{
								button->SetMenuAction(eMA_Exit);
							}
							else if (TOKEN_IS("back"))
							{
								button->SetMenuAction(eMA_Back);
							}
							else if (TOKEN_IS("new_game"))
							{
								button->SetMenuAction(eMA_NewGame);
							}
							else if (TOKEN_IS("new_game_server"))
							{
								button->SetMenuAction(eMA_NewGameServer);
							}
							else if (TOKEN_IS("new_game_client"))
							{
								button->SetMenuAction(eMA_NewGameClient);
							}
						}

						button->SetParentScript(menu);

						menu->m_Objects.Add(button);
					}
					//else if (TOKEN_IS(
					else
					{
						ASSERT(false);
						sprintf_s(g_ErrorBuffer, MAX_BUFFER_SIZE, "Not a valid menu item! Item: %s.\n", Token);
						g_Log.LOG_ERROR(g_ErrorBuffer);
					}
				}

				GET_NEXT_TOKEN;

			} while (Token);
		}

next:
		ReadString(Buffer, m_File);
		EDIT_STR;

		Token = strtok_s(Buffer, delim, &Context);
	}

	g_ResLog << "Loading script \"" << scr->m_strName << "\" from file " << scr->m_strFile << '\n';

#ifndef LOAD_SCRIPTS_FROM_PAK
	fclose(m_File);
#endif

	if (menuScript)
	{
		m_ScriptsLoaded.Add(menu);
		m_MenuScriptsLoaded.Add(menu);
		return menu;
	}
	
	m_ScriptsLoaded.Add(scr);
	
	return scr;
}

/////////////////////////////////////////////////////////////////////////////

void ScriptMgr::KillAllMenuScripts()
{
	for (UInt32 s = 0; s < m_MenuScriptsToRun.Size(); s++)
		m_MenuScriptsToRun[UInt32(s)]->m_bDone = true;
}

///////////////////////////////////////////////////////////////////////////////

Int32 Script::Update()
{
#if 0
	static ScriptCommand	sc;

	if (!m_Commands.IsEmpty())
	{
		if (!m_bRun)
		{
			m_Commands.Dequeue(sc);
			m_bRun = true;
		}

		switch (sc.m_eMsg)
		{
		case eCmd_Spawn:
			//Monster::Spawn(sc.strData1[0], false, Vec2(sc.fData2[0], sc.fData2[1]));
			break;

		case eCmd_Wait:
			if (sc.m_Args[0])
			{
				if (!m_bWaiting)
				{
					m_tTimer.Reset();
					m_bWaiting = true;
					return 1;
				}
				else if (m_tTimer.Get() >= Float(sc.m_Args[1]))
					m_bWaiting = false;
				else
					return 1;
			}
			else
			{
				if (!m_bWaiting)
				{
					m_uFrameCounter = 0;
					m_bWaiting = true;
					return 1;
				}
				else if (++m_uFrameCounter >= UInt32 (sc.m_Args[1]))
					m_bWaiting = false;
				else
					return 1;
			}
			break;

		case eCmd_Script_Run:
			ASSERT(sc.m_Args[0]);
			g_ScriptMgr->RunScript(g_ScriptMgr->LoadScript(sc.m_Args[0]));
			break;

		case eCmd_Script_Load:
			g_ScriptMgr->LoadScript(sc.m_Args[0]);
			break;

		case eCmd_Send_Entity_Msg:
 			ASSERT(sc.m_Args[0]);
 			//sc.m_Args[0]->SendMessage(sc.m_Args[1]);
			break;

		case eCmd_Menu_Cmd:
			if (sc.m_Args[0] == "back")
			{
				g_ScriptMgr->KillAllMenuScripts();
			}
			else if (sc.m_Args[0] == "new_game")
			{
				g_ScriptMgr->KillAllMenuScripts();
				g_Game->NewGame();
			}
			else if (sc.m_Args[0] == "new_game_server")
			{
				g_ScriptMgr->KillAllMenuScripts();
				g_Game->NewGame(NGM_Server);
			}
			else if (sc.m_Args[0] == "new_game_client")
			{
				g_ScriptMgr->KillAllMenuScripts();
				g_Game->NewGame(NGM_Client);
			}
			break;

		default:
			ASSERT(false);
			g_Log.LOG_ERROR("Unable to process command!\n");
		}

		m_bRun = false;
	}
	else
		return -1;
#endif

	return 0;
}

/////////////////////////////////////////////////////////////////////////////

Int32 MenuScript::Update()
{
	if (m_bDone)
		return -1;

	DblLinkedList<UIObject*>::Iterator		i(m_Objects);

	UIObject *obj= NULL;

	for (i.Begin(); !i.End(); ++i)
	{
		obj = i.Current();

		obj->Update();
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////