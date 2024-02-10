///////////////////////////////////////////////////////////////////////////
//		File:				LightMgr.cpp
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		A manager that stores all the lights in the game/map
//							and then when asked, finds the closest, most
//							applicable lights and then applies them to a shader
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "LightMgr.h"
#include "EffectMgr.h"
#include "GlobalFuncs.h"

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_SINGLETON(CLightMgr);

//////////////////////////////////////////////////////////////////////////

CLightMgr::CLightMgr(void)
	: m_bActive(true),
		m_bTrackTarget(false)
		//m_pTarget(NULL)
{
	m_AmbientLight = Color(Light::ms_LowLight);
}

//////////////////////////////////////////////////////////////////////////

CLightMgr::~CLightMgr(void)
{
	if (!Shutdown())
	{
		g_Log.LOG_ERROR("Error!\n");
	}
}

//////////////////////////////////////////////////////////////////////////

void CLightMgr::Add(Light& light, Bool bAddLightToHW /* = true */)
{
	m_Lights.Push(light);

	if (bAddLightToHW)
	{
		g_D3D->GetDevice()->SetLight(m_Lights.Size() - 1, &((D3DLight)light));
		
		if (light.IsActive())
			g_D3D->GetDevice()->LightEnable(m_Lights.Size() - 1, TRUE);
	}
}

//////////////////////////////////////////////////////////////////////////

void CLightMgr::Remove(Light& light)
{
	m_Lights.Remove(light);
}

//////////////////////////////////////////////////////////////////////////

void CLightMgr::RemoveAll()
{
	m_Lights.Clear();
}

//////////////////////////////////////////////////////////////////////////

Bool CLightMgr::Init(Bool bAutoTrackPlayer /* = true */)
{
	//if (bAutoTrackPlayer)
		//TrackTarget(g_Player);

	return true;
}

//////////////////////////////////////////////////////////////////////////

Bool CLightMgr::Update()
{
	/*if (m_bTrackTarget && m_pTarget)
		m_Lights[0].SetPosition(m_pTarget->m_vPosition);*/

	static Vec3 sunDir;
	static Timer t(true);
	static Color ambient = Color(0.1f, 0.1f, 0.06f);
	static Color diffuse = Color(1.0f, 1.0f, 0.6f);

	sunDir.SetSpeedDir(1.0f, 80*t + 90.0f, 90.0f, false);
	
	ambient.SetRed(Sine(80.0f, 0.05f, 0.05f, t));
	ambient.SetGreen(Sine(80.0f, 0.05f, 0.05f, t));
	//ambient.SetBlue(Sine(80.0f, 0.00, 0.05, t));

	ambient.SetRed(Sine(80.0f, 0.5f, 0.5f, t));
	ambient.SetGreen(Sine(80.0f, 0.5f, 0.5f, t));
	ambient.SetBlue(Sine(80.0f, 0.3f, 0.3f, t));
	
	//m_Lights[0].SetDirection(sunDir);
	//m_Lights[0].SetAmbient(ambient);
	//m_Lights[0].SetDiffuse(diffuse);

	Vec3 disp0 = Vec3(150.0f + Sine(1.0f, 0.0f, 50.0f, t), 30.0f, 150.0f + Cosine(1.0f, 0.0f, 50.0f, t)),
		 disp1 = Vec3(-150.0f + Sine(1.0f, 0.0f, 50.0f, t), 30.0f, -150.0f + Sine(1.0f, 0.0f, 50.0f, t));
	
	m_Lights[0].SetPosition(disp0);
	//m_Lights[1].SetPosition(disp1);

	return true;
}

//////////////////////////////////////////////////////////////////////////

Bool CLightMgr::Shutdown()
{
	RemoveAll();

	return true;
}

//////////////////////////////////////////////////////////////////////////

Int32 CLightMgr::GetAffectingLights(LightList& list, Vec3 pos, Bool bSort)
{
	Vec3	lightvec;
	
	Light	light;
	
	Float	rad;
	Int32	lightCount = 0;

	FOR_EACH(i, m_Lights)
	{
		light = m_Lights[i];

		if (light.IsActive())
		{				
			//lightvec	= pos - light.GetPosition();
			//rad			= light.GetRange();

			//if (lightvec.MagnitudeSquared() <= rad * rad)
			{
				list.Push(light);
				lightCount++;
			}
		}
	}

	if (bSort)
	{
		//LATER
	}

	return lightCount;
}

//////////////////////////////////////////////////////////////////////////

Int32 CLightMgr::GetAffectingLightPositions(LightPosList& list, Vec3 pos)
{
	Vec3 lightPos;
	LightList lights;

	Int32 lightCount = GetAffectingLights(lights, pos);
	
	FOR_EACH(i, lights)
	{
		lightPos = m_Lights[i].GetPosition();

		Vec4 lightVec(lightPos);
		list.Push(lightVec);
	}

	return lightCount;
}

//////////////////////////////////////////////////////////////////////////

Bool CLightMgr::SetupLightsInShader(Effect& effect, Vec3 pos)
{
	LightList list;

	//Set the number of lights
	Int32 size = GetAffectingLights(list, pos);
	size = Min(size, Light::ms_MaxLights);
	effect.Shader->SetInt("g_NumActiveLights", size);

	//Set ambient light levels
	D3DXVECTOR4	colorVec(m_AmbientLight.Red(), m_AmbientLight.Green(), m_AmbientLight.Blue(), 1.0f);
	effect.Shader->SetVector("vAmbientLight", &colorVec);

	FOR_EACH(i, list)
		SetLightInShader(effect, list[i], i);

	return true;
}

//////////////////////////////////////////////////////////////////////////

void CLightMgr::SetLightInShader(Effect& effect, Light& light, Int32 lightIndex /* = 0 */)
{
	ASSERT(lightIndex < Light::ms_MaxLights);

	String	lightActive		= "g_Lights[" + IntToString(lightIndex) + "].bActive",
			lightType		= "g_Lights[" + IntToString(lightIndex) + "].eType",
			lightVec		= "g_Lights[" + IntToString(lightIndex) + "].vPos",
			lightDir		= "g_Lights[" + IntToString(lightIndex) + "].vDir",
			lightDiffuse	= "g_Lights[" + IntToString(lightIndex) + "].cDiffuse",
			lightAmbient	= "g_Lights[" + IntToString(lightIndex) + "].cAmbient",
			lightSpecular	= "g_Lights[" + IntToString(lightIndex) + "].cSpecular",
			lightPower		= "g_Lights[" + IntToString(lightIndex) + "].fPower",
			lightRange		= "g_Lights[" + IntToString(lightIndex) + "].fRange",
			lightPenumbra	= "g_Lights[" + IntToString(lightIndex) + "].fPenumbra";

	D3DXHANDLE	hActive		= effect.Shader->GetParameterByName(NULL, lightActive.c_str()),
				hType		= effect.Shader->GetParameterByName(NULL, lightType.c_str()),
				hVec		= effect.Shader->GetParameterByName(NULL, lightVec.c_str()),
				hDir		= effect.Shader->GetParameterByName(NULL, lightDir.c_str()),
				hDiffuse	= effect.Shader->GetParameterByName(NULL, lightDiffuse.c_str()),
				hAmbient	= effect.Shader->GetParameterByName(NULL, lightAmbient.c_str()),
				hSpecular	= effect.Shader->GetParameterByName(NULL, lightSpecular.c_str()),
				hPower		= effect.Shader->GetParameterByName(NULL, lightPower.c_str()),
				hRange		= effect.Shader->GetParameterByName(NULL, lightRange.c_str()),
				hPenumbra	= effect.Shader->GetParameterByName(NULL, lightPenumbra.c_str());

	Vec3	lightPos		= light.GetPosition();
	Color	diffuse			= light.GetDiffuse();
	Color	ambient			= light.GetAmbient();
	Color	specular		= light.GetSpecular();
	
	D3DXVECTOR4	posVec(lightPos);
	D3DXVECTOR4 dirVec(light.GetDirection());
	D3DXVECTOR4	diffuseVec(diffuse.Red(), diffuse.Green(), diffuse.Blue(), 1.0f);
	D3DXVECTOR4	ambientVec(ambient.Red(), ambient.Green(), ambient.Blue(), 1.0f);
	D3DXVECTOR4	specularVec(specular.Red(), specular.Green(), specular.Blue(), 1.0f);

	effect.Shader->SetBool(hActive, light.IsActive());
	effect.Shader->SetInt(hType, light.GetType());

	effect.Shader->SetVector(hVec,		&posVec);
	effect.Shader->SetVector(hDir,		&dirVec);
	effect.Shader->SetVector(hDiffuse,	&diffuseVec);
	effect.Shader->SetVector(hAmbient,	&ambientVec);
	effect.Shader->SetVector(hSpecular,	&specularVec);

	effect.Shader->SetFloat(hRange,		light.GetRange());

	effect.Shader->SetFloat(hPower,	light.GetPower());
	effect.Shader->SetFloat(hPenumbra,	light.GetPenumbra());
}

//////////////////////////////////////////////////////////////////////////

//void CLightMgr::SetTarget(EntityPtr pTarget, Bool bTurnTrackingOn /* = true */)
//{
//	m_pTarget = pTarget;
//
//	if (pTarget && bTurnTrackingOn)
//		TrackTarget();
//}

//////////////////////////////////////////////////////////////////////////

void CLightMgr::SetAmbientLight(Color cAmbient)
{
	m_AmbientLight = cAmbient;
}

//////////////////////////////////////////////////////////////////////////