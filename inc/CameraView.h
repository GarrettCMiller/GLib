#pragma once

#include "Vec.h"

class CCameraView
{
public:
	CCameraView(void);
	virtual ~CCameraView(void);

public:

	void Set(	const Vec3* pos		= NULL, 
				const Vec3* forward	= NULL, 
				const Vec3* up		= NULL, 
				const Vec3* right	= NULL);

	void SetPosition(const Vec3& pos, Float fDist = 0.0f, Float fDir = 0.0f, Float fAlt = 90.0f, Bool bZAxis = true);
	void MovePosition(Float fDist = 0.0f, Float fDir = 0.0f, Float fAlt = 0.0f, Bool bZAxis = true);
	void SetTarget(const Vec3& target);
	void RemoveTarget();

	void SetPolar(Float fDist = 50.0f, Float fDir = 45.0f, Float fAlt = 45.0f, Bool bZAxis = true);

	void RotateX(Float theta);
	void RotateY(Float theta);
	void RotateZ(Float theta);

	void Move(Float dx, Float dy, Float dz = 0.0f);
	void MoveRight(Float dx);
	void MoveUp(Float dy);
	void MoveForward(Float dz);

//protected:
	void Update();

	Vec3 GetForward()	const;
	Vec3 GetUp()		const;
	Vec3 GetRight()		const;

	Vec3 GetPosition()	const;
	Vec3 GetTarget()	const;

	Bool HasTarget()	const;

	Float GetDistance()	const
	{	return m_fDesiredDistance;	}

	Float GetDirection()const
	{	return m_fDesiredDirection;	}

	Float GetAltitude()	const
	{	return m_fDesiredAltitude;	}

	void ClampDistance(Bool bClamp = true)
	{
		m_bClampDistance = bClamp;
	}

	void SetClampDistance(Float fMax, Float fMin = 0.0f, Bool bClampNow = true)
	{
		m_fDistanceClamp[0] = Max(0.0f, fMin);
		m_fDistanceClamp[1] = Max(0.0f, fMax);

		if (bClampNow)
			ClampDistance();
	}

	void ClampDirection(Bool bClamp = true)
	{
		m_bClampDirection = bClamp;
	}

	void SetClampDirection(Float fMax, Float fMin = 0.0f, Bool bClampNow = true)
	{
		m_fDirectionClamp[0] = Max(0.0f, fMin);
		m_fDirectionClamp[1] = Max(0.0f, fMax);

		if (bClampNow)
			ClampDirection();
	}

	void ClampAltitude(Bool bClamp = true)
	{
		m_bClampAltitude = bClamp;
	}

	void SetClampAltitude(Float fMax, Float fMin = 60.0f, Bool bClampNow = true)
	{
		m_fAltitudeClamp[0] = Max(0.0f, fMin);
		m_fAltitudeClamp[1] = Max(0.0f, fMax);

		if (bClampNow)
			ClampAltitude();
	}

protected:
	Bool	m_bHasTarget;
	Bool	m_bAttached;

	Bool	m_bClampDistance;
	Bool	m_bClampDirection;
	Bool	m_bClampAltitude;

	Vec3	m_vRight;
	Vec3	m_vUp;
	Vec3	m_vForward;

	Vec3	m_vPosition;
	Vec3	m_vTarget;

	Float	m_fDesiredDistance;
	Float	m_fDesiredDirection;
	Float	m_fDesiredAltitude;

	Float	m_fDistanceClamp[2];
	Float	m_fDirectionClamp[2];
	Float	m_fAltitudeClamp[2];

protected:
	void	UpdateAxes(Bool bForward, Bool bUp = true, Bool bRight = true);
};
