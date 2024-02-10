///////////////////////////////////////////////////////////////////////////
//		File:				Light.cpp
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		A structuaral representation of a light
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "Light.h"

//////////////////////////////////////////////////////////////////////////

const Float Light::ms_DefaultUmbra		= 150.0f;
const Float Light::ms_DefaultPenumbra	= 75.0f;

const Color	Light::ms_NoLight		= Color(0.0f,	0.0f,	0.0f);
const Color	Light::ms_VeryLowLight	= Color(0.05f,	0.05f,	0.05f);
const Color	Light::ms_LowLight		= Color(0.25f,	0.25f,	0.25f);
const Color	Light::ms_MediumLight	= Color(0.5f,	0.5f,	0.5f);
const Color Light::ms_BrightLight	= Color(0.75f,	0.75f,	0.75f);
const Color	Light::ms_FullLight		= Color(1.0f,	1.0f,	1.0f);

//////////////////////////////////////////////////////////////////////////

Light::Light(ELightType eType /* = eLT_Point */,
			 Bool bActive /* = true */,
			 Vec3 pos /* = k_v3Zero */,
			 Color diffuse /* = Color(1.0f, 1.0f, 1.0f, 1.0f)*/,
			 Float range /* = 500.0f */,
			 Float umbra /* = ms_DefaultUmbra*/, 
			 Float penumbra /* = ms_DefaultPenumbra*/) 
			 : m_Position(pos), m_bActive(bActive), m_cDiffuse(diffuse),
			 m_fRange(range), m_Umbra(umbra), m_Penumbra(penumbra),
			 m_fPower(1.0f)
{
}

//////////////////////////////////////////////////////////////////////////

Light::Light(Vec3 pos, Vec3 dir, Color diffuse, Color ambient, Bool bActive /* = true */) : 
	m_eType(eLT_Directional), m_Position(pos), m_Direction(dir),
		m_cDiffuse(diffuse), m_cAmbient(ambient), m_bActive(bActive),
		m_fRange(1000.0f), m_fPower(1.0f)
{
}

//////////////////////////////////////////////////////////////////////////

Light::~Light(void)
{
}

//////////////////////////////////////////////////////////////////////////