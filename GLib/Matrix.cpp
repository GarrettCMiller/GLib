#include "StdAfx.h"
#include "Matrix.h"

CMatrix::CMatrix(void)
{
	Identity();
}

CMatrix::CMatrix(const D3DMatrix& matRHS)
{
	memcpy(this, &matRHS,sizeof(D3DMatrix));
}

CMatrix::~CMatrix(void)
{
}

//////////////////////////////////////////////////////////////////////////

void CMatrix::AffineTransformation(Float fScale, const Vec3& RotCenter,
								   const D3DQuaternion& Rotation, const Vec3& Translation)
{
	D3DXMatrixAffineTransformation(this, fScale, &RotCenter, &Rotation, &Translation);
}

//////////////////////////////////////////////////////////////////////////

Float CMatrix::Determinant() const
{
	return D3DXMatrixDeterminant(this);
}

//////////////////////////////////////////////////////////////////////////

void CMatrix::Decompose(Vec3* pScale, D3DQuaternion* pRot, Vec3* pPos)
{
	D3DXMatrixDecompose(pScale, pRot, pPos, this);
}

//////////////////////////////////////////////////////////////////////////

Vec3 CMatrix::GetPosition()
{
	Vec3 pos; D3DQuaternion q; Vec3 scale;
	Decompose(&scale, &q, &pos);
	return pos;
}

//////////////////////////////////////////////////////////////////////////

void CMatrix::Identity()
{
	D3DXMatrixIdentity(this);
}

//////////////////////////////////////////////////////////////////////////

Bool CMatrix::IsIdentity() const
{
	return D3DXMatrixIsIdentity(this);
}

//////////////////////////////////////////////////////////////////////////

void CMatrix::LookAt(const Vec3 &vEye, const Vec3 &vAt, const Vec3 &vUp, Bool bLH /* = true */)
{
	if (bLH)
		D3DXMatrixLookAtLH(this, &vEye, &vAt, &vUp);
	else
		D3DXMatrixLookAtRH(this, &vEye, &vAt, &vUp);
}

//////////////////////////////////////////////////////////////////////////

void CMatrix::Ortho(Float fWidth, Float fHeight, Float fNear, Float fFar, Bool bLH /* = true */)
{
	if (bLH)
		D3DXMatrixOrthoLH(this, fWidth, fHeight, fNear, fFar);
	else
		D3DXMatrixOrthoRH(this, fWidth, fHeight, fNear, fFar);
}

//////////////////////////////////////////////////////////////////////////

void CMatrix::Ortho(Float fLeft, Float fRight, Float fBottom, Float fTop, Float fNear, Float fFar, Bool bLH /* = true */)
{
	if (bLH)
		D3DXMatrixOrthoOffCenterLH(this, fLeft, fRight, fBottom, fTop, fNear, fFar);
	else
		D3DXMatrixOrthoOffCenterRH(this, fLeft, fRight, fBottom, fTop, fNear, fFar);
}

//////////////////////////////////////////////////////////////////////////

void CMatrix::PerspectiveFOV(Float fFOV_Y, Float fAspect, Float fNear, Float fFar, Bool bLH /* = true */)
{
	if (bLH)
		D3DXMatrixPerspectiveFovLH(this, fFOV_Y, fAspect, fNear, fFar);
	else
		D3DXMatrixPerspectiveFovRH(this, fFOV_Y, fAspect, fNear, fFar);
}

//////////////////////////////////////////////////////////////////////////

void CMatrix::Perspective(Float fWidth, Float fHeight, Float fNear, Float fFar, Bool bLH /* = true */)
{
	if (bLH)
		D3DXMatrixPerspectiveLH(this, fWidth, fHeight, fNear, fFar);
	else
		D3DXMatrixPerspectiveRH(this, fWidth, fHeight, fNear, fFar);
}

//////////////////////////////////////////////////////////////////////////

void CMatrix::Reflect(const D3DPlane &plane)
{
	D3DXMatrixReflect(this, &plane);
}

//////////////////////////////////////////////////////////////////////////

void CMatrix::RotationAxis(const Vec3 &vAxis, Float fAngle)
{
	D3DXMatrixRotationAxis(this, &vAxis, fAngle);
}

//////////////////////////////////////////////////////////////////////////

void CMatrix::RotationQuaternion(const D3DQuaternion &qRot)
{
	D3DXMatrixRotationQuaternion(this, &qRot);
}

//////////////////////////////////////////////////////////////////////////

void CMatrix::RotationX(Float fAngle)
{
	D3DXMatrixRotationX(this, fAngle);
}

//////////////////////////////////////////////////////////////////////////

void CMatrix::RotationY(Float fAngle)
{
	D3DXMatrixRotationY(this, fAngle);
}

//////////////////////////////////////////////////////////////////////////

void CMatrix::RotationZ(Float fAngle)
{
	D3DXMatrixRotationZ(this, fAngle);
}

//////////////////////////////////////////////////////////////////////////

void CMatrix::RotationYawPitchRoll(Float fYaw, Float fPitch, Float fRoll)
{
	D3DXMatrixRotationYawPitchRoll(this, fYaw, fPitch, fRoll);
}

//////////////////////////////////////////////////////////////////////////

void CMatrix::Scaling(Float fX, Float fY, Float fZ)
{
	D3DXMatrixScaling(this, fX, fY, fZ);
}

//////////////////////////////////////////////////////////////////////////

void CMatrix::Scaling(const Vec3& vScale)
{
	Scaling(vScale.x, vScale.y, vScale.z);
}

//////////////////////////////////////////////////////////////////////////

void CMatrix::Transformation(const Vec3 &vScalingCenter, const D3DQuaternion &qScalingRotation, 
							 const Vec3 &vScaling, const Vec3 &vRotationCenter, 
							 const D3DQuaternion &qRotation, const Vec3 &vTranslation)
{
	D3DXMatrixTransformation(this, &vScalingCenter, &qScalingRotation, 
							&vScaling, &vRotationCenter, &qRotation, &vTranslation);
}

//////////////////////////////////////////////////////////////////////////

void CMatrix::Translation(Float fX, Float fY, Float fZ)
{
	D3DXMatrixTranslation(this, fX, fY, fZ);
}

//////////////////////////////////////////////////////////////////////////

void CMatrix::Translation(const Vec3 &vTranslation)
{
	Translation(vTranslation.x, vTranslation.y, vTranslation.z);
}

//////////////////////////////////////////////////////////////////////////

void CMatrix::Transpose()
{
	D3DXMatrixTranspose(this, this);
}

//////////////////////////////////////////////////////////////////////////

CMatrix CMatrix::GetTranspose() const
{
	CMatrix temp = *this;
	temp.Transpose();	
	return temp;
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////