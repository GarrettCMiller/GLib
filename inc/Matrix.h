#pragma once

#include <d3dx9math.h>
#include "Vec.h"

//_GLIB_DLL_API_
class CMatrix : public D3DMatrix
{
public:
	CMatrix(void);
	CMatrix(const D3DMatrix& matRHS);

	~CMatrix(void);

	void AffineTransformation(	Float fScale, const Vec3& RotCenter, 
								const D3DQuaternion& Rotation, const Vec3& Translation);
	
	void Decompose(Vec3* pScale, D3DQuaternion* pRot, Vec3* pPos);
	Vec3 GetPosition();

	Float Determinant() const;

	void Identity();
	Bool IsIdentity() const;

	void LookAt(const Vec3& vEye, const Vec3& vAt, const Vec3& vUp, Bool bLH = true);
	
	void Ortho(Float fWidth, Float fHeight, Float fNear, Float fFar, Bool bLH = true);
	void Ortho(Float fLeft, Float fRight, Float fBottom, Float fTop, Float fNear, Float fFar, Bool bLH = true);

	void PerspectiveFOV(Float fFOV_Y, Float fAspect, Float fNear, Float fFar, Bool bLH = true);
	void Perspective(Float fWidth, Float fHeight, Float fNear, Float fFar, Bool bLH = true);

	void Reflect(const D3DPlane& plane);

	void RotationAxis(const Vec3& vAxis, Float fAngle);
	void RotationQuaternion(const D3DQuaternion& qRot);
	
	void RotationX(Float fAngle);
	void RotationY(Float fAngle);
	void RotationZ(Float fAngle);

	void RotationYawPitchRoll(Float fYaw, Float fPitch, Float fRoll);

	void Scaling(Float fX, Float fY, Float fZ);
	void Scaling(const Vec3& vScale);

	void Transformation(const Vec3& vScalingCenter, const D3DQuaternion& qScalingRotation,
						const Vec3& vScaling, const Vec3& vRotationCenter,
						const D3DQuaternion& qRotation, const Vec3& vTranslation);

	void Translation(Float fX, Float fY, Float fZ);
	void Translation(const Vec3& vTranslation);

	void Transpose();
	CMatrix GetTranspose() const;

	Float operator () (UInt32 i, UInt32 j) const
	{
		return m[i][j];
	}

	Float& operator () (UInt32 i, UInt32 j)
	{
		return m[i][j];
	}

	//////////////////////////////////////////////////

	/*CMatrix& operator * (const CMatrix& rhs) const;
	CMatrix& operator * (Float rhs) const;

	CMatrix& operator *= (const CMatrix& rhs);
	CMatrix& operator *= (Float rhs);*/
};
