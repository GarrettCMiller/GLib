///////////////////////////////////////////////////////////////////////////////
//		File:				Vec2.h
//
//		Programmer:			Garrett Miller (GM)
//
//		Date created:		August 12, 2004
//
//		Description:		Implementation of a templated Vector class
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include <cmath>
#include <complex>
#include <stdlib.h>

#include "d3dx9core.h"

#include "MathConstants.h"
#include "MiscMath.h"

class CMatrix;

//////////////////////////////////////////////////////////////////////////

class CVec3;

//////////////////////////////////////////////////////////////////////////
template <typename _float>
class CVec2
{
public:
	_float		X;
	_float		Y;

	//////////////////////////////////////////////////////////////////////////
	explicit CVec2(_float x = 0.0f, _float y = 0.0f) 
		: 
	X(x), Y(y)
	{ 
	}

	explicit CVec2(const std::complex<_float>& c) : X(c.real()), Y(c.imag()) 
	{
	}

	CVec2(const POINT& pt)
	{
		X = static_cast<_float>(pt.x);
		Y = static_cast<_float>(pt.y);
	}
	//////////////////////////////////////////////////////////////////////////

	operator POINT ()
	{
		POINT p = {static_cast<UInt32>(X), static_cast<UInt32>(Y)};
		return p;
	}

	operator CVec3() const;

	operator D3DXVECTOR2()
	{
		return D3DXVECTOR2(X,Y);
	}

	operator std::complex<_float>()
	{
		return std::complex<_float>(X, Y);
	}

	//////////////////////////////////////////////////////////////////////////

	CVec2 Midpoint(const CVec2& rhs) const
	{
		CVec2 v(((rhs.X - X) / 2) + X, ((rhs.Y - Y) / 2) + Y);

		if (v.X < 0.00000000001)
			v.X = 0.0f;

		if (v.Y < 0.00000000001)
			v.Y = 0.0f;

		return v;
	}

	void SetSpeedDir(_float speed, _float dir)
	{
		if (dir < 0)
			dir = Angle();

		X	= speed * cos(dir * Deg2Rad);
		Y	= speed * sin(dir * Deg2Rad);
	}


	CVec2& ChooseRandomDirection()
	{
		SetSpeedDir(1.0f, Random() * 360.0f);
		return *this;
	}

	CVec2& ChooseRandomRadius()
	{
		SetSpeedDir(Random(), -1.0f);
		return *this;
	}

	CVec2& RandomUnitCircle()
	{
		SetSpeedDir(Random(), Random() * 360.0f);
		return *this;
	}

	CVec2& RandomUnitRectangle()
	{
		X = Random(1.0f, -1.0f);
		Y = Random(1.0f, -1.0f);
		return *this;
	}

	static CVec2 RandomDirection()
	{
		CVec2 temp;
		temp.ChooseRandomDirection();
		return temp;
	}

	static CVec2 RandomRadius()
	{
		CVec2 temp;
		temp.ChooseRandomRadius();
		return temp;
	}

	static CVec2 RandomCircle()
	{
		CVec2 temp;
		temp.RandomUnitCircle();
		return temp;
	}

	static CVec2 RandomRectangle()
	{
		CVec2 temp;
		temp.RandomUnitRectangle();
		return temp;
	}

	//////////////////////////////////////////////////////////////////////////

	CVec2 operator + (const CVec2 &rhs) const
	{
		return CVec2(X + rhs.X, Y + rhs.Y);
	}

	CVec2 &operator += (const CVec2 &rhs)
	{
		X += rhs.X;
		Y += rhs.Y;
		return *this;
	}

	CVec2 operator + (_float scalar) const
	{
        return CVec2(X + scalar, Y + scalar);
	}

	CVec2 &operator += (_float scalar)
	{
		X += scalar;
		Y += scalar;
		return *this;
	}

	CVec2 operator - (const CVec2 &rhs) const
	{
		return CVec2(X - rhs.X, Y - rhs.Y);
	}

	CVec2 &operator -= (const CVec2 &rhs)
	{
		X -= rhs.X;
		Y -= rhs.Y;

		return *this;
	}

	CVec2 operator - (_float scalar) const
	{
		return CVec2(X - scalar, Y - scalar);
	}

	CVec2 &operator -= (_float scalar)
	{
		X -= scalar;
		Y -= scalar;
		return *this;
	}

	friend CVec2 operator + (_float scalar, const CVec2& rhs);
	friend CVec2 operator - (_float scalar, const CVec2& rhs);
	friend CVec2 operator * (_float scalar, const CVec2& rhs);
	friend CVec2 operator / (_float scalar, const CVec2& rhs);

	CVec2 operator *(_float scalar) const
	{
		return CVec2(X * scalar, Y * scalar);
	}

	CVec2 operator *=(_float scalar)
	{
		X *= scalar;
		Y *= scalar;
		return *this;
	}

	CVec2 operator * (const CVec2 &rhs) const
	{
		return CVec2(X * rhs.X, Y * rhs.Y);
	}

	CVec2 &operator *= (const CVec2 &rhs)
	{
		X *= rhs.X;
		Y *= rhs.Y;
		return *this;
	}

	CVec2 operator / (_float scalar) const
	{
		return CVec2(X / scalar, Y / scalar);
	}

	CVec2 &operator /= (_float scalar)
	{
		X /= scalar;
		Y /= scalar;
		return *this;
	}

	CVec2 operator / (const CVec2 &rhs) const
	{
		return CVec2(X / rhs.X, Y / rhs.Y);
	}

	CVec2 &operator /= (const CVec2 &rhs)
	{
		X /= rhs.X;
		Y /= rhs.Y;
		return *this;
	}

#define converter(_type) \
	operator CVec2<_type> () \
	{\
		return CVec2<_type>(static_cast<_type>(X), static_cast<_type>(Y));\
	}

	converter(Float);
	converter(Int32);
	converter(UInt32);
	converter(Double);

#undef converter
	//////////////////////////////////////////////////////////////////////////

	_float Dot(const CVec2 &rhs) const
	{
		return ((X * rhs.X) + (Y * rhs.Y));
	}

	CVec2 GetInverted() const
	{
		return CVec2(-X, -Y);
	}

	CVec2& Invert()
	{
		X = -X;
		Y = -Y;
		return *this;
	}

	_float Magnitude() const
	{
		return sqrt(X * X + Y * Y);
	}

	_float MagnitudeSquared() const
	{
		return (X * X + Y * Y);
	}

	CVec2 GetNormalized() const
	{
		Float mag = Magnitude();
		return CVec2(X / mag, Y / mag);
	}

	CVec2 &Normalize()
	{
		Float mag = Magnitude();
		*this /= mag;
		return *this;
	}

	_float Angle() const
	{
		_float theta = atan2(_float(Y), _float(X));
		theta *= Rad2Deg;
		return theta;
	}

	_float FindAngleBetween(const CVec2 &rhs) const
	{
		_float theta = this->Dot(rhs) / (this->Magnitude()* rhs.Magnitude());
		theta = acos(theta) * Rad2Deg;

		return theta;
	}

	CVec2 LeftVector() const
	{
		return CVec2(-Y, X);
	}

	CVec2 RightVector() const
	{
		return CVec2(Y, -X);
	}

	Bool operator == (const CVec2& rhs) const
	{
		return (X == rhs.X && Y == rhs.Y);
	}

	Bool operator != (const CVec2& rhs) const
	{
		return (!((*this) == rhs));
	}
private:
};

//////////////////////////////////////////////////////////////////////////

template <typename _float>
CVec2<_float> operator + (_float scalar, const CVec2<_float>& rhs)
{
	return rhs + scalar;
}

template <typename _float>
CVec2<_float> operator - (_float scalar, const CVec2<_float>& rhs)
{
	return rhs - scalar;
}

template <typename _float>
CVec2<_float> operator * (_float scalar, const CVec2<_float>& rhs)
{
	return rhs * scalar;
}

template <typename _float>
CVec2<_float> operator / (_float scalar, const CVec2<_float>& rhs)
{
	return rhs / scalar;
}

//////////////////////////////////////////////////////////////////////////

typedef CVec2<Float>	Vec2f;
typedef CVec2<Int32>	Vec2i;
typedef CVec2<UInt32>	Vec2u;
typedef CVec2<Double>	Vec2d;

typedef Vec2f			Vec2;
//////////////////////////////////////////////////////////////////////////

const Vec2 k_v2Zero		= Vec2(0, 0);
const Vec2 k_v2One		= Vec2(1, 1);
const Vec2 k_v2XAxis	= Vec2(1, 0);
const Vec2 k_v2YAxis	= Vec2(0, 1);

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

class CVec3 : public D3DXVECTOR3
{
public:
	explicit CVec3(Float _x = 0.0f, Float _y = 0.0f, Float _z = 0.0f) : D3DXVECTOR3(_x, _y, _z) { }
	CVec3(const D3DXVECTOR3& vec) : D3DXVECTOR3(vec) { }
	CVec3(const D3DXVECTOR3* vec) : D3DXVECTOR3(*vec) { }

	operator CVec2<Float>()
	{
		return CVec2<Float>(x, y);
	}

	void SetSpeedDir(Float mag = 1.0f, Float theta = 0.0f, Float phi = 90.0f, Bool bZAxis = true)
	{
// 		if (theta < 0)
// 			theta = Theta();
// 
// 		if (phi < 0)
// 			phi = Phi();

		phi = Clamp<Float>(phi, 0.0f, 180.0f);

		theta *= Deg2Rad;
		phi *= Deg2Rad;
	
		if (bZAxis)
		{
			x = mag * sin(phi) * cos(theta);
 			z = mag * sin(phi) * sin(theta);
 			y = mag * cos(phi);
		}
		else
		{
			x = mag * sin(phi) * cos(theta);
			y = mag * sin(phi) * sin(theta);
			z = mag * cos(phi);
		}
	}

	CVec3 Cross(const CVec3& rhs) const
	{
		CVec3 out;
		D3DXVec3Cross(&out, this, &rhs);
		return out;
	}

	Float& operator [] (UInt32 index)
	{		
		switch (index)
		{
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		default:
			ASSERT(0);
		}
	}
	
	Float operator [] (UInt32 index) const
	{		
		switch (index)
		{
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		default:
			ASSERT(0);
		}
	}

	CVec3 ConserveMomentum(CVec3& rhs, Float m1 = 1.0f, Float m2 = 1.0f)
	{
		CVec3 out;

		Float mSum = m1 + m2;
		
		Float v1f = 0.0f, v2f = 0.0f;
		Float v1i = 0.0f, v2i = 0.0f;

		for (UInt32 v = 0; v < 3; v++)
		{
			v1i = (*this)[v];
			v2i = rhs[v];

			v1f = ((m1 - m2) / mSum) * v1i		+ ((2.0f * m2) / mSum) * v2i;
			v2f = ((2.0f * m1)  /mSum) * v1i	+ ((m2 - m1) / mSum) * v2i;

			(*this)[v] = v1f;
			rhs[v] = v2f;
		}

		return out;
	}

	CVec3 Midpoint(const CVec3& rhs) const
	{
		CVec3 v(((rhs.x - x) / 2) + x, ((rhs.y - y) / 2) + y, ((rhs.z - z) / 2) + z);

		if (v.x < 0.00000000001)
			v.x = 0.0f;

		if (v.y < 0.00000000001)
			v.y = 0.0f;

		if (v.z < 0.00000000001)
			v.z = 0.0f;

		return v;
	}

	CVec3 Transform(D3DXMATRIX* rhs)
	{
		CVec3 res;
		D3DXVec3TransformCoord(&res, this, rhs);
		return res;
	}

	/*CVec3& operator *= (D3DXMATRIX* rhs)
	{
		D3DXVec3TransformCoord(this, this, rhs);
		return *this;
	}*/

	Float Dot(const CVec3& rhs) const
	{
		return ((x  * rhs.x) + (y * rhs.y) + (z * rhs.z));
	}

	Float FindAngleBetween(const CVec3 &rhs) const
	{
		Float theta = this->Dot(rhs) / (this->Magnitude()* rhs.Magnitude());
		theta = acos(theta) * Rad2Deg;

		return theta;
	}

	Float Magnitude() const
	{
		return sqrt((x * x) + (y * y) + (z * z));
	}

	Float MagnitudeSquared() const
	{
		return ((x * x) + (y * y) + (z * z));
	}

	CVec3 Normalize()
	{
		return *D3DXVec3Normalize(this, this);
	}

	CVec3 GetNormalized() const
	{
		D3DXVECTOR3 temp;
		return CVec3(D3DXVec3Normalize(&temp, this));
	}

	Float Theta() const
	{
		Float theta = atan2(y, x);
		theta *= Rad2Deg;
		return theta;
	}

	Float Phi() const
	{
		Float Phi = atan2(y, z);
		Phi *= Rad2Deg;
		return Phi;
	}

	/////////////////////////////////
	void ChooseRandomDirection()
	{
		SetSpeedDir(1.0f, Random() * 360.0f, Random() * 360.0f);
	}

	void ChooseRandomRadius()
	{
		SetSpeedDir(Random(), -1.0f, -1.0f);
	}

	void RandomUnitSphere()
	{
		SetSpeedDir(Random(), Random() * 360.0f, Random() * 360.0f);
	}

	void RandomUnitVolume()
	{
		x = Random(1.0f, -1.0f);
		y = Random(1.0f, -1.0f);
		z = Random(1.0f, -1.0f);
	}

	static CVec3 RandomDirection()
	{
		CVec3 temp;
		temp.ChooseRandomDirection();
		return temp;
	}

	static CVec3 RandomRadius()
	{
		CVec3 temp;
		temp.ChooseRandomRadius();
		return temp;
	}

	static CVec3 RandomSphere()
	{
		CVec3 temp;
		temp.RandomUnitSphere();
		return temp;
	}

	static CVec3 RandomVolume()
	{
		CVec3 temp;
		temp.RandomUnitVolume();
		return temp;
	}
};

//////////////////////////////////////////////////////////////////////////

template<typename _Type>
CVec2<_Type>::operator CVec3() const
{
	return CVec3(X, Y, 0);
}

typedef CVec3	Vec3;

//////////////////////////////////////////////////////////////////////////

template <typename _float = Float>
class CVec4 : public D3DXVECTOR4
{
public:
	CVec4(const CVec3& rhs)
	{
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
		w = 0.0f;
	}

	CVec4(const CVec2<_float>& rhs)
	{
		x = rhs.X;
		y = rhs.Y;
		z = 0;
		w = 0;
	}

	CVec4(_float _x = 0.0f, _float _y = 0.0f, _float _z = 0.0f, _float _w = 0.0f) :
		D3DXVECTOR4(_x, _y, _z, _w)
	{	}

	/*CVec4(const Color& color)
	{
		x = color.Red();
		y = color.Green();
		z = color.Blue();
		w = color.Alpha();
	}*/
};

//////////////////////////////////////////////////////////////////////////

typedef CVec4<>	Vec4;

//////////////////////////////////////////////////////////////////////////

const Vec3 k_v3Zero		= Vec3(0.0f, 0.0f, 0.0f);
const Vec3 k_v3One		= Vec3(1.0f, 1.0f, 1.0f);
const Vec3 k_v3XAxis	= Vec3(1.0f, 0.0f, 0.0f);
const Vec3 k_v3YAxis	= Vec3(0.0f, 1.0f, 0.0f);
const Vec3 k_v3ZAxis	= Vec3(0.0f, 0.0f, 1.0f);

//////////////////////////////////////////////////////////////////////////