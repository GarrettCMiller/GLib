/////////////////////////////////////////////////////////////////////////////
//		File:			DirectInput.cpp
//
//		Programmer:		Garrett Miller (GM)
//
//		Date Created:	2/21/2006 12:52:12 AM
//
//		Description:	Wrapper class for DirectInput
//
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "DirectInput.h"
#include "D3DWrapper.h"
//#include "ProfileSample.h"

#include "Globals.h"

//////////////////////////////////////////////////////////////////////////

#define MOUSE_BUFFER_SIZE	16
#define INPUT_ERROR_TIMEOUT			5

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_SINGLETON(DirectInput);

//////////////////////////////////////////////////////////////////////////

DirectInput::DirectInput(void)
{
	m_lpDInput			= NULL;

	m_lpDIKeyboard		= NULL;
	m_lpDIMouse			= NULL;

	m_vCursorPos.X		= -1;
	m_vCursorPos.Y		= -1;

	m_bShowCursor		= true;

	m_bInvertMouseX		= false;
	m_bInvertMouseY		= false;

	//ZeroMemory(m_strKeys,	sizeof(wchar_t) * 256);
	//ZeroMemory(&m_msMouse,	sizeof(DIMOUSESTATE2));

	m_bMouseInWindow	= false;
}

//////////////////////////////////////////////////////////////////////////

DirectInput::~DirectInput(void)
{
	if (!Shutdown())
	{
		g_Log.LOG_ERROR("Failed to shutdown DirectInput!\n");
		//MessageBox(g_hWnd, "Error shutting DInput down!", "ERROR!", MB_ICONEXCLAMATION);
	}
}

//////////////////////////////////////////////////////////////////////////

Int32 DirectInput::Init()
{
	HRESULT hr;

	g_Log << "DirectInput::Init() begin...\n";

	if (FAILED(hr = DirectInput8Create(g_hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void **) &m_lpDInput, NULL)))
	{
		g_Log.LOG_ERROR_DX_DESC(hr, "Failed to create DirectInput object!\n");
		return 0;
	}

	if (!InitKeyboard())
	{
		g_Log.LOG_ERROR("Failed to initialize keyboard!\n");
		return 0;
	}

	if (!InitMouse())
	{
		g_Log.LOG_ERROR("Failed to initialize mouse!\n");
		return 0;
	}

	if (!InitKeyBindings())
	{
		g_Log.LOG_ERROR("Failed to initialize key bindings!\n");
		return 0;
	}

	g_Log << "DirectInput::Init() end!\n";
	return 1;
}

//////////////////////////////////////////////////////////////////////////

Bool DirectInput::InitKeyBindings()
{
	/*m_KeyBindings[eAction_MoveUp]				= TKeyPair(DIK_W, 0);
	m_KeyBindings[eAction_MoveDown]				= TKeyPair(DIK_S, 0);
	m_KeyBindings[eAction_MoveLeft]				= TKeyPair(DIK_A, 0);
	m_KeyBindings[eAction_MoveRight]			= TKeyPair(DIK_D, 0);

	m_KeyBindings[eAction_MoveRun]				= TKeyPair(DIK_LSHIFT, 0);

	m_KeyBindings[eAction_EquipWeapon1]			= TKeyPair(DIK_1, 0);
	m_KeyBindings[eAction_EquipWeapon2]			= TKeyPair(DIK_2, 0);
	m_KeyBindings[eAction_EquipWeapon3]			= TKeyPair(DIK_3, 0);
	m_KeyBindings[eAction_EquipWeapon4]			= TKeyPair(DIK_4, 0);
	m_KeyBindings[eAction_EquipWeapon5]			= TKeyPair(DIK_5, 0);
	m_KeyBindings[eAction_EquipWeapon6]			= TKeyPair(DIK_6, 0);
	
	m_KeyBindings[eAction_Suicide]				= TKeyPair(DIK_GRAVE, 0);
	
	m_KeyBindings[eAction_ChangeSpawnType]		= TKeyPair(DIK_0, 0);
	
	m_KeyBindings[eAction_DecreaseSpawnTime]	= TKeyPair(DIK_O, 0);
	m_KeyBindings[eAction_IncreaseSpawnTime]	= TKeyPair(DIK_P, 0);
	
	m_KeyBindings[eAction_StartBulletTime]		= TKeyPair(DIK_R, 0);
	m_KeyBindings[eAction_EndBulletTime]		= TKeyPair(DIK_T, 0);
	
	m_KeyBindings[eAction_Toggle_GodMode]		= TKeyPair(DIK_U, 0);
	m_KeyBindings[eAction_Toggle_FPS]			= TKeyPair(DIK_R, DIK_LCONTROL);

	m_KeyBindings[eAction_CreateRandomBarrels]	= TKeyPair(DIK_G, 0);
	
	m_KeyBindings[eAction_CycleAmbientLightUp]	= TKeyPair(DIK_UP, 0);
	m_KeyBindings[eAction_CycleAmbientLightDown]= TKeyPair(DIK_DOWN, 0);
	
	m_KeyBindings[eAction_Pause]				= TKeyPair(DIK_RETURN, 0);

	m_KeyBindings[eAction_Spawn4Soldiers]		= TKeyPair(DIK_H, 0);
	m_KeyBindings[eAction_Respawn]				= TKeyPair(DIK_SPACE, 0);

	m_KeyBindings[eAction_MemoryDump]			= TKeyPair(DIK_BACKSPACE, DIK_RCONTROL);
	m_KeyBindings[eAction_Quit]					= TKeyPair(DIK_ESCAPE, 0);*/

	return true;
}

//////////////////////////////////////////////////////////////////////////

Bool DirectInput::InitKeyboard()
{
	HRESULT hr;

	g_Log << "DirectInput::InitKeyboard() begin...\n";

	if (FAILED(hr = m_lpDInput->CreateDevice(GUID_SysKeyboard, &m_lpDIKeyboard, NULL)))
	{
		g_Log.LOG_ERROR_DX_DESC(hr, "Failed to create keyboard device!\n");
		Shutdown();
		return false;
	}

	if (!m_lpDIKeyboard)
	{
		g_Log.LOG_ERROR("No keyboard device!\n");
		return false;
	}

	if (FAILED(hr = m_lpDIKeyboard->SetDataFormat(&c_dfDIKeyboard)))
	{
		g_Log.LOG_ERROR_DX_DESC(hr, "Failed to set keyboard data format!\n");
		Shutdown();
		return false;
	}

	if (FAILED(hr = m_lpDIKeyboard->SetCooperativeLevel(g_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
	{
		g_Log.LOG_ERROR_DX_DESC(hr, "Failed to set keyboard cooperation level!\n");
		Shutdown();	
		return false;
	}

	if (FAILED(hr = m_lpDIKeyboard->Acquire()))
	{
		g_Log.LOG_ERROR_DX_DESC(hr, "Failed to acquire keyboard!\n");
		//Shutdown();
		return true; //false;
	}

	g_Log << "DirectInput::InitKeyboard() end!\n";

	return true;
}

//////////////////////////////////////////////////////////////////////////

Bool DirectInput::InitMouse()
{
	HRESULT hr;

	g_Log << "DirectInput::InitMouse() begin...\n";

	if (FAILED(hr = m_lpDInput->CreateDevice(GUID_SysMouse,  &m_lpDIMouse, NULL)))
	{
		g_Log.LOG_ERROR_DX_DESC(hr, "Failed to create mouse device!\n");
		Shutdown();
		return false;
	}

	if (FAILED(hr = m_lpDIMouse->SetDataFormat(&c_dfDIMouse2)))
	{
		g_Log.LOG_ERROR_DX_DESC(hr, "Failed to set mouse data format!\n");
		Shutdown();
		return false;
	}

	if (FAILED(hr = m_lpDIMouse->SetCooperativeLevel(g_hWnd,
													DISCL_NONEXCLUSIVE | DISCL_FOREGROUND)))
	{
		g_Log.LOG_ERROR_DX_DESC(hr, "Failed to set mouse cooperation level!\n");
		Shutdown();
		return false;
	}

	/*m_hMouseEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	if (!m_hMouseEvent)
	{
		g_Log.LOG_ERROR("Failed to create mouse event!\n";
		return false;
	}

	if (FAILED(hr = m_lpDIMouse->SetEventNotification(m_hMouseEvent)))
	{
		g_Log.LOG_ERROR_DX_DESC(hr, "Failed to set mouse event notification!\n";
		Shutdown();
		return false;
	}*/

	if (m_bBufferedMouse)
	{
		DIPROPDWORD	dipdw;

		dipdw.diph.dwSize				= sizeof(DIPROPDWORD);
		dipdw.diph.dwHeaderSize			= sizeof(DIPROPHEADER);
		dipdw.diph.dwObj				= 0;
		dipdw.diph.dwHow				= DIPH_DEVICE;

		dipdw.dwData					= MOUSE_BUFFER_SIZE;

		if (FAILED(hr = m_lpDIMouse->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph)))
		{
			g_Log.LOG_ERROR_DX_DESC(hr, "Failed to set mouse properties!\n");
			Shutdown();
			return false;
		}
	}

	if (FAILED(hr = m_lpDIMouse->Acquire()))
	{
		g_Log.LOG_ERROR_DX_DESC(hr, "Failed to acquire mouse!\n");
		//Shutdown();
		//return true; //false;
	}

	POINT m;
	GetCursorPos(&m);

	ScreenToClient(g_hWnd, &m);

	m_vCursorPos = m;

#ifndef _DEBUG
	ShowCursor(FALSE);
#endif

#ifdef _DOOMEROIDS_
	m_MouseCursor.LoadImage("Reticule.bmp");
#endif

	g_Log << "DirectInput::InitMouse() end!\n";

	return true;
}

//////////////////////////////////////////////////////////////////////////

Int32 DirectInput::Shutdown()
{
	g_Log << "DirectInput::Shutdown() begin...\n";

	if (m_lpDInput)
	{
		ShutdownKeyboard();
		ShutdownMouse();

		m_lpDInput->Release();
		m_lpDInput = NULL;
	}

	g_Log << "DirectInput::Shutdown() end!\n";

	return 1;
}

//////////////////////////////////////////////////////////////////////////

Bool DirectInput::ShutdownKeyboard()
{
	g_Log << "DirectInput::ShutdownKeyboard() begin...\n";

	if (m_lpDIKeyboard)
	{
		m_lpDIKeyboard->Unacquire();
		m_lpDIKeyboard->Release();
		m_lpDIKeyboard = NULL;
	}
	else
	{
		g_Log.LOG_ERROR("No keyboard device found to shutdown!\n");
		return false;
	}

	g_Log << "Keyboard shutdown successfully!\n";

	return true;
}

//////////////////////////////////////////////////////////////////////////

Bool DirectInput::ShutdownMouse()
{
	g_Log << "DirectInput::ShutdownMouse() begin...\n";

	if (m_lpDIMouse)
	{
		m_lpDIMouse->Unacquire();
		m_lpDIMouse->Release();
		m_lpDIMouse = NULL;
	}
	else
	{
		g_Log.LOG_ERROR("No mouse device found to shutdown!\n");
		return false;
	}

#ifndef _DEBUG
	ShowCursor(TRUE);
#endif

	g_Log << "Mouse shutdown successfully!\n";

	return true;
}

//////////////////////////////////////////////////////////////////////////

Int32 DirectInput::Update()
{
	//PROFILE("DirectInput::Update()");

	ReadKeyboard();
	ReadMouse();

	return 1;
}

//////////////////////////////////////////////////////////////////////////

Int32 DirectInput::ReadKeyboard()
{
	if (!m_lpDIKeyboard)
	{
		g_Log.LOG_ERROR("No keyboard device!\n");
		return 0;
	}

	HRESULT hr;

	ZeroMemory(m_strKeys, sizeof(char) * 256);

	if (FAILED(hr = m_lpDIKeyboard->GetDeviceState(sizeof(m_strKeys), (LPVOID) &m_strKeys)))
	{
		g_Log.LOG_ERROR_DX_DESC(hr, "Failed to get keyboard device state!\n");

		if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
		{
			g_Log.LOG_ERROR("Keyboard input was lost. Attempting reacquisition...\n");

			if (FAILED(hr = m_lpDIKeyboard->Acquire()))
			{
				g_Log.LOG_ERROR_DX_DESC(hr, "Error reacquiring keyboard device!\n");
				return 1;
			}

			g_Log << "Keyboard reacquired!\n";
			return 1;
		}

		return -1;
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////

Int32 DirectInput::ReadMouse()
{
	HRESULT hr;

	if (!m_lpDIMouse)
	{
		g_Log.LOG_ERROR("No mouse device!\n");
		return 0;
	}

	ZeroMemory(&m_msMouse, sizeof(DIMOUSESTATE2));

	if (FAILED(hr = m_lpDIMouse->GetDeviceState(sizeof(DIMOUSESTATE2), &m_msMouse)))
	{
		g_Log.LOG_ERROR_DX_DESC(hr, "Failed to get mouse device state!\n");
		
		if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
		{
			g_Log.LOG_ERROR("Mouse input was lost. Attempting reacquisition...\n");

			if (FAILED(hr = m_lpDIMouse->Acquire()))
			{
				g_Log.LOG_ERROR_DX_DESC(hr, "Failed to reacquire mouse!\n");
				return 1;
			}

			g_Log << "Mouse reacquired!\n";
			
			return 1;
		}

		return -1;
	}

	m_vCursorPos.X += m_msMouse.lX;
	m_vCursorPos.Y += m_msMouse.lY;

	POINT	m = m_vCursorPos, pt;

	RECT	rect;

	WINDOWPLACEMENT wp;

	GetCursorPos(&pt);
	GetClientRect(g_hWnd, &rect);
	GetWindowPlacement(g_hWnd, &wp);

	/*rect.left += wp.rcNormalPosition.left;
	rect.right += wp.rcNormalPosition.left;
	rect.top += wp.rcNormalPosition.top;
	rect.bottom += wp.rcNormalPosition.top;*/
	
	ClientToScreen(g_hWnd, &m);

	if (PtInRect(&rect, pt))
	{
		if (!m_bMouseInWindow)
		{
			ScreenToClient(g_hWnd, &pt);
			m_vCursorPos = pt;
		}
		else
			SetCursorPos(m.x, m.y);

		m_bMouseInWindow = true;
	}
	else
	{
		m_bMouseInWindow = false;

		ScreenToClient(g_hWnd, &pt);

		m_vCursorPos.X  = static_cast<Float>(min(pt.x, 800));
		m_vCursorPos.X	= static_cast<Float>(max(0, m_vCursorPos.X));

		m_vCursorPos.Y  = static_cast<Float>(min(pt.y, 250));
		m_vCursorPos.Y	= static_cast<Float>(max(0, m_vCursorPos.Y));
	}

	if (m_bShowCursor)
		DrawCursor();

	return 1;
}

//////////////////////////////////////////////////////////////////////////

void DirectInput::DrawCursor()
{
	if (m_MouseCursor.GetImageID() != -1)
	{
		m_MouseCursor.SetPosition(m_vCursorPos);
		m_MouseCursor.Update();
	}
}

//////////////////////////////////////////////////////////////////////////

Bool DirectInput::KeyDown(BYTE key)
{
	return Bool((m_strKeys[key] & 0x80) != 0);
}

//////////////////////////////////////////////////////////////////////////

Bool DirectInput::MouseDown(EMouseButton btn)
{
	return Bool((m_msMouse.rgbButtons[btn] & 0x80) != 0);
}

//////////////////////////////////////////////////////////////////////////

Bool DirectInput::MappedKeyDown(EMappedActions eAction)
{
	Bool bStatus = false;

	TKeyPair keypair = m_KeyBindings[eAction];

	if (keypair.first != 0 && KeyDown(keypair.first))
	{
		bStatus = true;

		if (keypair.second != 0 && !KeyDown(keypair.second))
			bStatus = false;
	}

	return bStatus;
}

//////////////////////////////////////////////////////////////////////////

void DirectInput::SetAcquire()
{
	if (g_bActive)
		m_lpDIMouse->Acquire();
	else
		m_lpDIMouse->Unacquire();
}

//////////////////////////////////////////////////////////////////////////