/////////////////////////////////////////////////////////////////////////////
//		File:			DirectInput.h
//
//		Programmer:		Garrett Miller (GM)
//
//		Date Created:	2/21/2006 12:52:10 AM
//
//		Description:	Wrapper class for DirectInput
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#define DIRECTINPUT_VERSION		0x0800

#include "dxerr.h"
#include "DInput.h"
#include "Singleton.h"

#include "UIImage.h"

#include <map>

//////////////////////////////////////////////////////////////////////////

#pragma comment (lib, "DInput8")
#pragma comment (lib, "dxerr")

//////////////////////////////////////////////////////////////////////////

//helper struct
// struct KeyPair
// {
// 	typedef std::pair<BYTE, BYTE>	TKeyPair;		//Second entry is modifiers like ctrl, shift, and alt
// 
// 	TKeyPair	keypair;
// 
// 	KeyPair(BYTE key)
// 	{
// 		keypair
// 	}
// };

enum EMappedActions;

//////////////////////////////////////////////////////////////////////////

class DirectInput : public Singleton<DirectInput>
{
public:
	typedef std::pair<BYTE, BYTE>				TKeyPair;		//Second entry is modifiers like ctrl, shift, and alt
	typedef std::map<EMappedActions, TKeyPair>	TKeyBindings;

private:
	LPDIRECTINPUT8			m_lpDInput;

	LPDIRECTINPUTDEVICE8	m_lpDIKeyboard;
	LPDIRECTINPUTDEVICE8	m_lpDIMouse;

	BYTE					m_strKeys[256];
	DIMOUSESTATE2			m_msMouse;

	Vec2					m_vMouseDelta;
	Vec2					m_vCursorPos;

	Bool					m_bBufferedMouse;
	Bool					m_bMouseInWindow;
	Bool					m_bShowCursor;

	Bool					m_bInvertMouseX;
	Bool					m_bInvertMouseY;

	UIImage					m_MouseCursor;

//hack
public:

	TKeyBindings			m_KeyBindings;

public:
	enum EMouseButton
	{
		eLeftButton,	eMiddleButton,		eRightButton,
		
		eNumMouseButtons
	};

	DirectInput(void);
	virtual ~DirectInput(void);

	Int32	Init();
	Int32	Update();
	Int32	Shutdown();

	Bool	KeyDown(BYTE key);
	Bool	MouseDown(EMouseButton btn);

	Bool	MappedKeyDown(EMappedActions eAction);

	void	ShowCursor(Bool bShow = true)
	{		m_bShowCursor = bShow;		}

	void	SetAcquire();

	Vec2	GetMouseDelta() const	
	{ 
		return Vec2(m_msMouse.lX * (m_bInvertMouseX ? -1.0f : 1.0f), m_msMouse.lY * (m_bInvertMouseY ? -1.0f : 1.0f));	
	}

	Vec2	GetMousePos() const		{ return m_vCursorPos; }

private:
	Bool	InitKeyBindings();

	Bool	InitKeyboard();
	Bool	InitMouse();

	Bool	ShutdownKeyboard();
	Bool	ShutdownMouse();

	Int32	ReadKeyboard();
	Int32	ReadMouse();

	void	DrawCursor();
};

#define g_Input	DirectInput::GetInstance()