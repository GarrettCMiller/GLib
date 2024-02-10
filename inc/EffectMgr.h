///////////////////////////////////////////////////////////////////////////////
//		File:				EffectMgr.h
//
//		Programmer:			Garrett Miller (GM)
//
//		Date created:		August 2, 2006
//
//		Description:		A manager that loads and stores HLSL effect files
///////////////////////////////////////////////////////////////////////////////
#pragma once

// #ifndef _STDAFX_INCLUDED_
// 	#include "stdafx.h"
// 	#pragma message("stdafx.h has been included in " __FILE__)
// #endif
#include "GameMacros.h"
#include "Singleton.h"
#include "DynamicArray.h"

#include "D3DWrapper.h"

#include <iostream>

//////////////////////////////////////////////////////////////////////////

#define	MAX_SHADER_TECHS	10

#define DEFAULT_PER_PIXEL_LIGHTING_SHADER_TECH	"PerPixelLighting"

//////////////////////////////////////////////////////////////////////////

struct Effect
{
	UInt32		ID;
	char		Name[80];

	D3DEffect	Shader;

	UInt32		TechCount;
	D3DXHANDLE	Tech[MAX_SHADER_TECHS];
};

//////////////////////////////////////////////////////////////////////////

class EffectMgr : public Singleton<EffectMgr>
{
public:
	typedef CDynamicArray<Effect> EffectList;

	EffectList	m_Effects;

	UInt32	m_nNextID;

	EffectMgr();
	virtual ~EffectMgr(void);

public:

	UInt32 LoadEffect(String path, Effect* pEffect = NULL);

	Bool Contains(String name);

	D3DEffect	GetShader(String name);
	D3DEffect	GetShader(UInt32 id);

	Effect& GetEffect(String name, Bool bLoadIfNotFound = false);
	Effect& GetEffect(UInt32 id);

	Int32 GetID(String name);

	UInt32 Size() const	{	return m_Effects.Size();	}
};

#define g_EffectMgr	EffectMgr::GetInstance()