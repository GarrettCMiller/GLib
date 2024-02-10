///////////////////////////////////////////////////////////////////////////
//		File:				RegEditor.cpp
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		A registry editor
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include ".\regeditor.h"

//////////////////////////////////////////////////////////////////////////

CRegEditor::CRegEditor(void)
{
	//Int32 size = 1024;
	//Int32 res;
	//Int32 type, size2;
	//HKEY key; 
	//Int32 disp;
	//char WMPPath[1024];

	//if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\MediaPlayer", 0, KEY_READ, &m_RegKey) != ERROR_SUCCESS)
	//{
	//	return S_FALSE;
	//}
	//
	//RegQueryValueEx(m_RegKey, "VisualizationsDir", 0, &type, (LPBYTE)WMPPath, &size2);

	//RegCloseKey(m_RegKey);

	//char defpath[] = "\\ParticleViz\\Particle.bmp";
	//char shaderpath[] = "\\ParticleViz\\Shader.fx";

	////strncpy(ImagePath, "c:\\Program Files\\Windows Media Player\\Visualizations\\Particle.bmp", 1024);

	//strncpy(ImagePath, WMPPath, 1024);
	//strncat(ImagePath, defpath, 1024);

	//strncpy(ShaderPath, WMPPath, 1024);
	//strncat(ShaderPath, shaderpath, 1024);

	//RegCreateKeyEx(	HKEY_CURRENT_USER, "Software\\GWare\\ParticleViz", 0, NULL, NULL, 
	//				KEY_ALL_ACCESS, NULL, &m_RegKey, &disp);

	//if (RegCreateKeyEx(m_RegKey, "Resources", 0, NULL, NULL, KEY_ALL_ACCESS, NULL, &key, &disp) == ERROR_SUCCESS)
	//{
	//	if (disp == REG_CREATED_NEW_KEY)
	//	{
	//		RegSetValueEx(key, "Particle Image", 0, REG_SZ, (const BYTE *)ImagePath, strlen(ImagePath) + 1);
	//		RegSetValueEx(key, "Shader File", 0, REG_SZ, (const BYTE *)ShaderPath, strlen(ShaderPath) + 1);
	//	}
	//	else
	//	{
	//		RegQueryValueEx(key, "Particle Image", 0, &type, (LPBYTE)ImagePath, &size2);
	//		RegQueryValueEx(key, "Shader File", 0, &type, (LPBYTE)ShaderPath, &size2);
	//	}
	//}

	////Close Resources key
	//RegCloseKey(key);

	//if (RegCreateKeyEx(m_RegKey, "Video Settings", 0, NULL, NULL, KEY_ALL_ACCESS, NULL, &key, &disp) == ERROR_SUCCESS)
	//{
	//	if (disp == REG_CREATED_NEW_KEY)
	//	{
	//		RegSetValueEx(key, "Width", 0, REG_DWORD, (const BYTE *)&VidWidth, sizeof(Int32));
	//		RegSetValueEx(key, "Height", 0, REG_DWORD, (const BYTE *)&VidHeight, sizeof(Int32));
	//	}
	//	else
	//	{
	//		RegQueryValueEx(key, "Width", 0, &type, (LPBYTE) &VidWidth, &size2);
	//		RegQueryValueEx(key, "Height", 0, &type, (LPBYTE) &VidHeight, &size2);
	//	}
	//}

	//RegCloseKey(m_RegKey);
}

//////////////////////////////////////////////////////////////////////////

CRegEditor::~CRegEditor(void)
{
}

//////////////////////////////////////////////////////////////////////////