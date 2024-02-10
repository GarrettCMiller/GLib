///////////////////////////////////////////////////////////////////////////
//		File:				Light.h
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		The structural representation of a light
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "D3DWrapper.h"
#include "Color.h"

//////////////////////////////////////////////////////////////////////////

class Light
{
public:
	enum ELightType
	{
		eLT_Point,
		eLT_Spot,
		eLT_Directional,

		eLT_NumTypes
	};

	enum ELightIntensity
	{
		eLI_NoLight,
		eLI_VeryLowLight,
		eLI_LowLight,
		eLI_MediumLight,
		eLI_BrightLight,
		eLI_FullLight,

		eLI_IntensityCount
	};

	static const Color	ms_NoLight;
	static const Color	ms_VeryLowLight;
	static const Color	ms_LowLight;
	static const Color	ms_MediumLight;
	static const Color	ms_BrightLight;
	static const Color	ms_FullLight;

	static const Float	ms_DefaultUmbra;
	static const Float	ms_DefaultPenumbra;

	static const Int32	ms_MaxLights = 8;

protected:
	Bool			m_bActive;

	Vec3			m_Position;
	Vec3			m_Direction;
	Vec3			m_vAttenuation;

	Vec2			m_vAngles;
	
	Color			m_cDiffuse;
	Color			m_cAmbient;
	Color			m_cSpecular;

	Float			m_fRange;
	Float			m_fPower;

	//Distance from position where light begins to fade
	Float			m_Penumbra;

	//Distance from position where light fully fades
	Float			m_Umbra;

	ELightType		m_eType;

public:
	Light(ELightType eType = eLT_Point, Bool bActive = true,
			Vec3 pos = k_v3Zero, Color color = Color::km_White,
			Float range = 500.0f,
			Float umbra = ms_DefaultUmbra, Float penumbra = ms_DefaultPenumbra);
	Light(Vec3 pos, Vec3 dir, Color diffuse, Color ambient, Bool bActive = true);
	~Light(void);

	Bool	IsActive() const				{ return m_bActive;	}
	void	SetActive(Bool bActive = true)	{ m_bActive = bActive; }

	Float	GetUmbra() const				{ return m_Umbra; }
	void	SetUmbra(Float umbra)			{ ASSERT(umbra > 0.0f); ASSERT(umbra > m_Penumbra); m_Umbra = umbra; }

	Float	GetPenumbra() const				{ return m_Penumbra; }
	void	SetPenumbra(Float penumbra)		{ ASSERT(penumbra > 0.0f); /*ASSERT(penumbra < m_Umbra);*/ m_Penumbra = penumbra; }

	Float	GetRange() const				{ return m_fRange; }
	void	SetRange(Float range)			{ m_fRange = Max(0.0f, range); }

	Float	GetPower() const				{ return m_fPower; }
	void	SetPower(Float power)			{ m_fPower = Max(0.0f, power); }

	Vec3	GetPosition() const				{ return m_Position; }
	void	SetPosition(Vec3 pos)			{ m_Position = pos; }

	Vec3	GetDirection() const			{ return m_Direction; }
	void	SetDirection(Vec3 pos)			{ m_Direction = pos; }

	Color	GetDiffuse() const				{ return m_cDiffuse; }
	void	SetDiffuse(Color color)			{ m_cDiffuse = color;}

	Color	GetAmbient() const				{ return m_cAmbient; }
	void	SetAmbient(Color color)			{ m_cAmbient = color;}

	Color	GetSpecular() const				{ return m_cSpecular; }
	void	SetSpecular(Color color)		{ m_cSpecular = color;}

	ELightType GetType() const				{ return m_eType;	}
	void	SetType(ELightType eType)		{ m_eType = eType;	}

	//For storage in a list
	Bool		operator != (const Light& rhs) const
	{
		return memcmp(this, &rhs, sizeof(Light));
	}

	Bool		operator == (const Light& rhs) const
	{
		return !(*this != rhs);
	}

	operator D3DLight ()
	{
		D3DLight d3dl; ZeroMemory(&d3dl, sizeof(D3DLight));
		d3dl.Ambient	= m_cAmbient;
		d3dl.Diffuse	= m_cDiffuse;
		d3dl.Direction	= m_Direction;
		d3dl.Position	= m_Position;
		d3dl.Type		= static_cast<D3DLIGHTTYPE>(m_eType);
		return d3dl;
	}
};
