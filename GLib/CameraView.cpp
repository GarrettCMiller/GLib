#include "StdAfx.h"
#include "CameraView.h"

#include "D3DWrapper.h"

//////////////////////////////////////////////////////////////////////////

CCameraView::CCameraView(void) : 
	m_bHasTarget(false), 
		m_fDesiredDistance(0.0f),
		m_fDesiredDirection(0.0f),
		m_fDesiredAltitude(90.0f),
		m_bAttached(false),
		m_bClampDistance(false),
		m_bClampDirection(false),
		m_bClampAltitude(false)
{
	m_vForward = k_v3ZAxis;
	m_vRight = k_v3XAxis;
	m_vUp = k_v3YAxis;

	m_fDistanceClamp[0]		= 0.0f;
	m_fDistanceClamp[1]		= 100.0f;
	m_fDirectionClamp[0]	= 0.0f;
	m_fDirectionClamp[1]	= 360.0f;
	m_fAltitudeClamp[0]		= -70.0f;
	m_fAltitudeClamp[1]		= 70.0f;
}

CCameraView::~CCameraView(void)
{
}

//////////////////////////////////////////////////////////////////////////

void CCameraView::Set(const Vec3* pos /* = NULL */, 
					  const Vec3* forward /* = NULL */, 
					  const Vec3* up /* = NULL */, 
					  const Vec3* right /* = NULL */)
{
	if (pos)		SetPosition(*pos);
	if (forward)	m_vForward = *forward;
	if (up)			m_vUp = *up;
	if (right)		m_vRight = *right;
}
//////////////////////////////////////////////////////////////////////////

void CCameraView::SetPosition(const Vec3& pos, Float fDist /* = 0.0f */, Float fDir /* = 0.0f */, Float fAlt /* = 90.0f */, Bool bZAxis /* = true */)
{
	m_vPosition = pos;

	if (fDist > 0.0f)
	{
		m_fDesiredDistance = fDist;
		m_fDesiredDirection = fDir;
		m_fDesiredAltitude = fAlt;

		Vec3 vPos;
		vPos.SetSpeedDir(	m_fDesiredDistance, 
							m_fDesiredDirection,	m_fDesiredAltitude,
							bZAxis);


		m_vPosition += vPos;
	}
}

//////////////////////////////////////////////////////////////////////////

void CCameraView::SetPolar(Float fDist /* = 50.0f */, Float fDir /* = 45.0f */, Float fAlt /* = 45.0f */, Bool bZAxis /* = true */)
{
	SetPosition(m_vTarget, fDist, fDir, fAlt, bZAxis);
}

//////////////////////////////////////////////////////////////////////////

void CCameraView::MovePosition(Float fDist /* = 0.0f */, Float fDir /* = 0.0f */, Float fAlt /* = 0.0f */, Bool bZAxis /* = true */)
{
	m_fDesiredDistance += fDist;
	m_fDesiredDirection += fDir;
	m_fDesiredAltitude += fAlt;

	if (m_bClampDistance)
		m_fDesiredDistance = Clamp(m_fDesiredDistance, m_fDistanceClamp[0], m_fDistanceClamp[1]);
	else
		if (m_fDesiredDistance < 0.0f) 
			m_fDesiredDistance = 0.0f;

	if (m_bClampAltitude)
		m_fDesiredAltitude = Clamp(m_fDesiredAltitude, m_fAltitudeClamp[0], m_fAltitudeClamp[1]);
	else
		m_fDesiredAltitude = Clamp(m_fDesiredAltitude, -89.9f, 89.9f);

	if (m_bClampDirection)
		m_fDesiredDirection = Clamp(m_fDesiredDirection, m_fDirectionClamp[0], m_fDirectionClamp[1]);
	else
	{
		if (m_fDesiredDirection < 0.0f)
			m_fDesiredDirection += 360.0f;
		else if (m_fDesiredDirection > 360.0f)
			m_fDesiredDirection -= 360.0f;
	}

	Vec3 vPos;
	vPos.SetSpeedDir(	m_fDesiredDistance, 
						m_fDesiredDirection,	m_fDesiredAltitude,
						bZAxis);


	m_vPosition = m_vTarget;
	m_vPosition += vPos;
}

//////////////////////////////////////////////////////////////////////////

void CCameraView::SetTarget(const Vec3& target)
{
	m_bHasTarget = true;
	m_vTarget = target;

	m_vForward = m_vTarget - m_vPosition;
	m_vForward.Normalize();

	UpdateAxes(true);
}

//////////////////////////////////////////////////////////////////////////

void CCameraView::Move(Float dx, Float dy, Float dz /* = 0.0f*/)
{
	m_vPosition += Vec3(dx, dy, dz);
}

//////////////////////////////////////////////////////////////////////////

void CCameraView::MoveForward(Float dz)
{
	if (m_bHasTarget)
		m_vTarget += m_vForward * dz;
	else
		m_vPosition += m_vForward * dz;
}

//////////////////////////////////////////////////////////////////////////

void CCameraView::MoveUp(Float dy)
{
	if (m_bHasTarget)
		m_vTarget += m_vUp * dy;
	else
		m_vPosition += m_vUp * dy;
}

//////////////////////////////////////////////////////////////////////////

void CCameraView::MoveRight(Float dx)
{
	if (m_bHasTarget)
		m_vTarget += m_vRight * dx;
	else
		m_vPosition += m_vRight * dx;
}

//////////////////////////////////////////////////////////////////////////

void CCameraView::RotateX(Float theta)
{
	theta /= 57.2957795f;

	static D3DMatrix rotMat;

	D3DXMatrixRotationAxis(&rotMat, &m_vRight, theta);
	
	D3DXVec3TransformCoord(&m_vForward, &m_vForward, &rotMat);
	D3DXVec3TransformCoord(&m_vUp, &m_vUp, &rotMat);

	if (m_bHasTarget)
	{
		D3DXQUATERNION qRot;
		D3DXQuaternionRotationAxis(&qRot, &m_vRight, -theta);
		D3DXMatrixTransformation(&rotMat, NULL, NULL, NULL, &m_vTarget, &qRot, NULL);
		D3DXVec3TransformCoord(&m_vPosition, &m_vPosition, &rotMat);
	}

	m_vForward.Normalize();
	m_vUp.Normalize();
}

//////////////////////////////////////////////////////////////////////////

void CCameraView::RotateY(Float theta)
{
	theta /= 57.2957795f;

	static D3DMatrix rotMat;
	
	D3DXMatrixRotationAxis(&rotMat, &m_vUp, theta);
	
	D3DXVec3TransformCoord(&m_vForward, &m_vForward, &rotMat);
	D3DXVec3TransformCoord(&m_vRight, &m_vRight, &rotMat);

	if (m_bHasTarget)
	{
		D3DXQUATERNION qRot;
		D3DXQuaternionRotationAxis(&qRot, &m_vUp, -theta);
		D3DXMatrixTransformation(&rotMat, NULL, NULL, NULL, &m_vTarget, &qRot, NULL);
		D3DXVec3TransformCoord(&m_vPosition, &m_vPosition, &rotMat);
	}

	m_vForward.Normalize();
	m_vRight.Normalize();
}

//////////////////////////////////////////////////////////////////////////

void CCameraView::RotateZ(Float theta)
{
	theta /= 57.2957795f;

	static D3DMatrix rotMat;

	D3DXMatrixRotationAxis(&rotMat, &m_vForward, theta);
	
	D3DXVec3TransformCoord(&m_vUp, &m_vUp, &rotMat);
	D3DXVec3TransformCoord(&m_vRight, &m_vRight, &rotMat);

	if (m_bHasTarget)
	{
		D3DXQUATERNION qRot;
		D3DXQuaternionRotationAxis(&qRot, &m_vForward, theta);
		D3DXMatrixTransformation(&rotMat, NULL, NULL, NULL, &m_vTarget, &qRot, NULL);
		D3DXVec3TransformCoord(&m_vPosition, &m_vPosition, &rotMat);
	}

	m_vUp.Normalize();
	m_vRight.Normalize();
}

//////////////////////////////////////////////////////////////////////////

void CCameraView::Update()
{
	g_D3D->SetCamera(m_vPosition, m_vForward, m_vUp);
}

//////////////////////////////////////////////////////////////////////////

Vec3 CCameraView::GetForward() const
{
	return m_vForward;
}

//////////////////////////////////////////////////////////////////////////

Vec3 CCameraView::GetUp() const
{
	return m_vUp;
}

//////////////////////////////////////////////////////////////////////////

Vec3 CCameraView::GetRight() const
{
	return m_vRight;
}

//////////////////////////////////////////////////////////////////////////

Vec3 CCameraView::GetPosition() const
{
	return m_vPosition;
}

//////////////////////////////////////////////////////////////////////////

Vec3 CCameraView::GetTarget() const
{
	return m_vTarget;
}

//////////////////////////////////////////////////////////////////////////

Bool CCameraView::HasTarget() const
{
	return m_bHasTarget;
}

//////////////////////////////////////////////////////////////////////////

void CCameraView::UpdateAxes(Bool bForward, Bool bUp /* = true */, Bool bRight /* = true */)
{
	//if (bForward)
	//	return;

// 	if (bUp)
// 	{
// 		m_vUp = m_vForward.Cross(m_vRight);
// 	}

	//HACK
	bRight;
	bUp;
	bForward;

	Vec3 vRight = m_vForward.Cross(k_v3YAxis);

	if (fabsf(vRight.x) < 0.0001f && fabsf(vRight.y) < 0.0001f && fabsf(vRight.z) < 0.0001f)
		vRight = m_vForward.Cross(k_v3ZAxis);

	vRight.Normalize();

	Vec3 vUp = m_vForward.Cross(vRight);
	vUp.Normalize();

	if (vUp.y < 0)
		vUp *= -1.0f;

	m_vRight = vRight;
	m_vUp = vUp;

	/*if (m_vForward.Dot(m_vUp) != 0)
		m_vUp = m_vRight.Cross(m_vForward);*/
}