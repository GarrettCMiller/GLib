///////////////////////////////////////////////////////////////////////////////
//		File:				EffectMgr.cpp
//
//		Programmer:			Garrett Miller (GM)
//
//		Date created:		August 11, 2004
//
//		Description:		A manager that loads and stores HLSL effect files
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "EffectMgr.h"
#include "d3dwrapper.h"
#include "Vec.h"
#include "MemMgr.h"

#include "DPakFile.h"

#include "Globals.h"

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_SINGLETON(EffectMgr);

static Int32 numdrawn = 0;

//////////////////////////////////////////////////////////////////////////

EffectMgr::EffectMgr()
{
	m_nNextID	= 0;
}

//////////////////////////////////////////////////////////////////////////

EffectMgr::~EffectMgr(void)
{
	UInt32 count = m_Effects.Size();

	for (UInt32 i = 0; i < count; i++)
	{
		SAFE_RELEASE(m_Effects[i].Shader);
		g_ResLog << "Freed effect " << m_Effects[i].Name << "\n";
	}

	//m_Effects.ReleasePointers();

	g_Log << "Freed " << count << " effects!\n";
	g_ResLog << "Freed " << count << " effects!\n";
}

//////////////////////////////////////////////////////////////////////////

Bool EffectMgr::Contains(String name)
{
	String strName;

	for (UInt32 i = 0; i < m_Effects.Size(); i++)
	{
		strName = m_Effects[i].Name;

		if (strName == name)
			return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////

D3DEffect EffectMgr::GetShader(UInt32 id)
{
	return m_Effects[id].Shader;
}

//////////////////////////////////////////////////////////////////////////

D3DEffect EffectMgr::GetShader(String name)
{
	String strName;

	for (UInt32 i = 0; i < m_Effects.Size(); i++)
	{
		strName = m_Effects[i].Name;

		if (strName == name)
			return m_Effects[i].Shader;
	}

	//Shouldn't be here
	ASSERT(false);

	return NULL;
}

//////////////////////////////////////////////////////////////////////////

Effect& EffectMgr::GetEffect(String name, Bool bLoadIfNotFound /* = false */)
{
	String strName;

	for (UInt32 i = 0; i < m_Effects.Size(); i++)
	{
		strName = m_Effects[i].Name;

		if (strName == name)
			return m_Effects[i];
	}

	if (bLoadIfNotFound)
	{
		LoadEffect(name);
		return m_Effects[m_nNextID - 1];
	}

	//Shouldn't be here
	ASSERT(false);

	return m_Effects[0];
}

//////////////////////////////////////////////////////////////////////////

Effect& EffectMgr::GetEffect(UInt32 id)
{
	return m_Effects[id];
}

//////////////////////////////////////////////////////////////////////////

Int32 EffectMgr::GetID(String name)
{
	String strName;

	for (UInt32 i = 0; i < m_Effects.Size(); i++)
	{
		strName = m_Effects[i].Name;

		if (strName == name)
			return m_Effects[i].ID;
	}

	//Shouldn't be here
	ASSERT(false);

	return -1;
}

//////////////////////////////////////////////////////////////////////////

UInt32 EffectMgr::LoadEffect(String path, Effect* pEffect /* = NULL */)
{
	HRESULT hr;

	if (Contains(path))
	{
		g_Log.LOG_ERROR("Effect manager tried to reload an effect!" + path);
		return GetID(path);
	}

	//g_ResLog << "Loading effect " << path << '\n';

	Effect effect;

	ZeroMemory(&effect, sizeof(Effect));

	strncpy_s(effect.Name,	path.c_str(), sizeof(effect.Name));
	effect.ID				= m_nNextID;

	DPakFile file;

	g_PakFile.GetFileByName(file, path.c_str());

	if (file.IsLocalFile())
	{
// 		String newpath = "Res\\Effects\\";
// 		newpath += file.Name;

		if (FAILED(hr = D3DXCreateEffectFromFile(g_D3D->GetDevice(),
												file.Name,//newpath.c_str(),
												NULL, NULL, 
												DEFAULT_SHADER_FLAGS, 			
												NULL, &effect.Shader, NULL)))
		{
			sprintf_s(g_ErrorBuffer, "Couldn't load effect %s", file.Name/*newpath*/);
			g_Log.LOG_ERROR_DX_DESC(hr, g_ErrorBuffer);
			g_ResLog.LOG_ERROR_DX_DESC(hr, g_ErrorBuffer);

			AfxThrowUserException();
			
			return UInt32(-1);
		}
	}
	else
	{
		if (FAILED(hr = D3DXCreateEffectEx(g_D3D->GetDevice(),
			file.FileData,
			file.FileSize, NULL, NULL, NULL,
			DEFAULT_SHADER_FLAGS, 			
			NULL, &effect.Shader, NULL)))
		{
			sprintf_s(g_ErrorBuffer, "Couldn't load effect %s", path.c_str());
			g_Log.LOG_ERROR_DX_DESC(hr, g_ErrorBuffer);
			g_ResLog.LOG_ERROR_DX_DESC(hr, g_ErrorBuffer);

			return UInt32(-1);
		}
	}

	if (FAILED(hr = effect.Shader->FindNextValidTechnique(NULL, &effect.Tech[0])))
	{
		g_Log.LOG_ERROR("Unable to find valid technique in shader " + path);
		SAFE_RELEASE(effect.Shader);
		return UInt32(-1);
	}

	effect.TechCount = 1;

	while (SUCCEEDED(hr = effect.Shader->FindNextValidTechnique(
						effect.Tech[effect.TechCount - 1], 
						&effect.Tech[effect.TechCount])) 						
						&& effect.Tech[effect.TechCount])
	{
		effect.TechCount++;
	}

	g_ResLog << "Loaded shader \"" << path << "\" containing " << effect.TechCount << " techniques!\n";
	
	m_Effects.Push(effect);

	if (pEffect)
		*pEffect = m_Effects[m_nNextID];

	return m_nNextID++;
}

//////////////////////////////////////////////////////////////////////////