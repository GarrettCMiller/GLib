///////////////////////////////////////////////////////////////////////////
//		File:				LightMgr.h
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		A manager that stores all the lights in the game/map
//							and then when asked, finds the closest, most
//							"applicable" lights and then applies them to a given shader
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "Light.h"
#include "DynamicArray.h"
#include "Singleton.h"
#include "Rect.h"


/////////////////////////////////////////////////////////////////////////////

typedef CDynamicArray<Light>	LightList;
typedef CDynamicArray<Vec4>		LightPosList;

/////////////////////////////////////////////////////////////////////////////

struct Effect;

/////////////////////////////////////////////////////////////////////////////

class CLightMgr : public Singleton<CLightMgr>
{
	LightList	m_Lights;

	Bool		m_bActive;
	Bool		m_bTrackTarget;

	UInt32		m_MaxActiveLights;

	//EntityPtr	m_pTarget;

	Color	m_AmbientLight;

public:
	CLightMgr(void);
	~CLightMgr(void);

	Bool	Init(Bool bAutoTrackPlayer = true);
	Bool	Update();
	Bool	Shutdown();

	void	Add(Light& light, Bool bAddLightToHW = true);

	Light&	GetLight(UInt32 index)					{ return m_Lights[index]; }
	
	void	Remove(Light& light);
	void	RemoveAll();

	void	Activate(Bool bActivate = true)			{ m_bActive = bActivate;	}
	Bool	IsActive() const						{ return m_bActive;			}

	//EntityPtr GetTarget() const						{ return m_pTarget;			}
	//void	SetTarget(EntityPtr pTarget, Bool bTurnTrackingOn = true);

	void	TrackTarget(Bool bTrackTarget = true)	{ m_bTrackTarget = bTrackTarget;	}
	Bool	IsTrackingTarget() const				{ return m_bTrackTarget;	}

	Int32	GetAffectingLights(LightList& list, Vec3 pos, Bool bSort = true);
	Int32	GetAffectingLightPositions(LightPosList& list, Vec3 pos);

	Bool	SetupLightsInShader(Effect& effect, Vec3 pos);

	void	SetAmbientLight(Color cAmbient);

private:
	void	SetLightInShader(Effect& effect, Light& light, Int32 lightIndex = 0);
};

#define g_LightMgr		CLightMgr::GetInstance()
