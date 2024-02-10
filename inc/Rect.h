///////////////////////////////////////////////////////////////////////////
//		File:				Rect.h
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		A rectangle class
//////////////////////////////////////////////////////////////////////////
#pragma once

// #ifndef _STDAFX_INCLUDED_
// 	#include "stdafx.h"
// 	#pragma message("stdafx.h has been included in " __FILE__)
// #endif

#include <cmath>
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////////

namespace GLib
{

//////////////////////////////////////////////////////////////////////////
template <typename _float>
class CRect
{
public:
	CRect()
	{
		tlx = tly = brx = bry = 0.0f;
	}

	CRect(const CRect& copy)
	{
		*this = copy;
	}

	CRect(_float left, _float top, _float right, _float bottom) : 
		tlx(left),
		tly(top),
		brx(right),
		bry(bottom)
	{
	}

// 	CRect(const CVec2<_float>& TopLeft, const CVec2<_float>& BottomRight) : 
// 		tlx(TopLeft.X),
// 		tly(TopLeft.Y),
// 		brx(BottomRight.X),
// 		bry(BottomRight.Y)
// 	{
// 	}

	CRect(const Vec2& TopLeft, const Vec2& BottomRight) : 
		tlx(static_cast<_float>(TopLeft.X)),
		tly(static_cast<_float>(TopLeft.Y)),
		brx(static_cast<_float>(BottomRight.X)),
		bry(static_cast<_float>(BottomRight.Y))
	{
	}

	CRect(const Vec2i& TopLeft, const Vec2i& BottomRight) :
		tlx(static_cast<_float>(TopLeft.X)),
		tly(static_cast<_float>(TopLeft.Y)),
		brx(static_cast<_float>(BottomRight.X)),
		bry(static_cast<_float>(BottomRight.Y))
	{
	}

	CRect(const Vec2u& TopLeft, const Vec2u& BottomRight) :
		tlx(static_cast<_float>(TopLeft.X)),
		tly(static_cast<_float>(TopLeft.Y)),
		brx(static_cast<_float>(BottomRight.X)),
		bry(static_cast<_float>(BottomRight.Y))
	{
	}

	CRect(const Vec2d& TopLeft, const Vec2d& BottomRight) :
		tlx(static_cast<_float>(TopLeft.X)),
		tly(static_cast<_float>(TopLeft.Y)),
		brx(static_cast<_float>(BottomRight.X)),
		bry(static_cast<_float>(BottomRight.Y))
	{
	}

	//////////////////////////////////////////////////////////////////////////

	Bool Intersect(const CRect &rhs) const
	{
		return	!(  brx	< rhs.tlx	||
					tlx	> rhs.brx	||
					bry	< rhs.tly	||
					tly	> rhs.bry	);
	}

	Bool Intersect2(const CRect &rhs) const
	{
		RECT r1 = 
		{
			static_cast<UInt32>(tlx),		static_cast<UInt32>(tly),
			static_cast<UInt32>(brx),		static_cast<UInt32>(bry),
		},
		
		r2 = 
		{
			static_cast<UInt32>(rhs.tlx),	static_cast<UInt32>(rhs.tly),
			static_cast<UInt32>(rhs.brx),	static_cast<UInt32>(rhs.bry),
		},

		temp;

		return (IntersectRect(&temp, &r1, &r2) != 0);
	}

	Bool PointInRect(const CVec2<_float>& p) const
	{
		return (p.X < brx && 
			p.X > tlx &&
			p.Y < bry &&
			p.Y > tly);
	}

	Float Width() const
	{
		return fabsf(brx - tlx);
	}

	Float Height() const
	{
		return fabsf(bry - tly);
	}

	void MoveTo(const CVec2<_float>& tlc)
	{
		tlx += tlc.X;
		tly += tlc.Y;
		brx += tlc.X;
		bry += tlc.Y;
	}

	void MoveTo(_float _tlx, _float _tly, _float _brx, _float _bry)
	{
		tlx += _tlx;
		tly += _tly;
		brx += _brx;
		bry += _bry;
	}

	void SetCenter(const CVec2<_float>& pos)
	{
		_float	width	= Width(),
				height	= Height();

		tlx = pos.X - width / 2;
		tly = pos.Y - height / 2;
		brx = pos.X + width / 2;
		bry = pos.Y + height / 2;
	}

	void SetCenter(_float x, _float y)
	{
		_float	width	= Width(),
				height	= Height();

		tlx = x - width / 2;
		tly = y - height / 2;
		brx = x + width / 2;
		bry = y + height / 2;
	}

	CVec2<_float> TopLeft() const
	{
		return CVec2<_float>(tlx, tly);
	}

	CVec2<_float> TopRight() const
	{
		return CVec2<_float>(brx, tly);
	}

	CVec2<_float> BottomRight() const
	{
		return CVec2<_float>(brx, bry);
	}

	CVec2<_float> BottomLeft() const
	{
		return CVec2<_float>(tlx, bry);
	}

	CVec2<_float> Top() const
	{
		return CVec2<_float>(tlx + Width() / 2, tly);
	}

	CVec2<_float> Right() const
	{
		return CVec2<_float>(brx, tly + Height() / 2);
	}

	CVec2<_float> Bottom() const
	{
		return CVec2<_float>(tlx + Width() / 2, bry);
	}

	CVec2<_float> Left() const
	{
		return CVec2<_float>(tlx, tly + Height() / 2);
	}

	operator RECT ()
	{
		RECT rect = 
		{
			static_cast<UInt32>(tlx),	static_cast<UInt32>(tly),
			static_cast<UInt32>(brx),	static_cast<UInt32>(bry)
		};
		return rect;
	}

	_float tlx, tly, brx, bry;
};

//////////////////////////////////////////////////////////////////////////

typedef CRect<Int32>	Recti;
typedef CRect<Float>	Rectf;
typedef CRect<UInt32>	Rectu;
typedef CRect<Double>	Rectd;

typedef Recti			Rect;

//////////////////////////////////////////////////////////////////////////

template <typename _float>
class CRect3D
{
public:
	CRect3D()
	{
		tlx = tly = tlz = brx = bry = brz = 0.0f;
	}

	CRect3D(const CRect3D& copy)
	{
		*this = copy;
	}

	CRect3D(_float left, _float top, _float front, _float right, _float bottom, _float back) : 
		tlx(left),
		tly(top),
		tlz(front),
		brx(right),
		bry(bottom),
		brz(back)
	{
	}

	CRect3D(const Vec3& TopLeft, const Vec3& BottomRight) : 
		tlx(static_cast<_float>(TopLeft.x)),
		tly(static_cast<_float>(TopLeft.y)),
		tlz(static_cast<_float>(TopLeft.z)),
		brx(static_cast<_float>(BottomRight.x)),
		bry(static_cast<_float>(BottomRight.y)),
		brz(static_cast<_float>(BottomRight.z))
	{
	}

	//////////////////////////////////////////////////////////////////////////

	Bool Intersect(const CRect3D &rhs) const
	{
		const CVec3 t = rhs.GetCenter() - GetCenter();
		return	fabsf(t.x) <= (Width()	+ rhs.Width())	&&
				fabsf(t.y) <= (Height()	+ rhs.Height())	&&
				fabsf(t.z) <= (Depth()	+ rhs.Depth());
	}

	Bool Intersect2(const CRect3D &rhs) const
	{
		RECT r1 = 
		{
			static_cast<UInt32>(tlx),		static_cast<UInt32>(tly),
			static_cast<UInt32>(brx),		static_cast<UInt32>(bry),
		},

		r2 = 
			{
				static_cast<UInt32>(rhs.tlx),	static_cast<UInt32>(rhs.tly),
				static_cast<UInt32>(rhs.brx),	static_cast<UInt32>(rhs.bry),
			},

			temp;

				return (IntersectRect(&temp, &r1, &r2) != 0);
	}

	CVec3 GetCenter() const
	{
		return CVec3(	tlx + Width() / 2,
						tly + Height() / 2,
						tlz + Depth() / 2);
	}

	Bool PointInRect(const CVec3& p) const
	{
		return (p.X < brx && 
			p.X > tlx &&
			p.Y < bry &&
			p.Y > tly);
	}

	Float Width() const
	{
		return fabsf(brx - tlx);
	}

	Float Height() const
	{
		return fabsf(bry - tly);
	}

	Float Depth() const
	{
		return fabsf(brz - tlz);
	}

	void MoveTo(const CVec3& tlc)
	{
		tlx += tlc.x;
		tly += tlc.y;
		tlz += tlc.z;
		brx += tlc.x;
		bry += tlc.y;
		brz += tlc.z;
	}

	void MoveTo(_float _tlx, _float _tly,_float _tlz, _float _brx, _float _bry, _float _brz)
	{
		tlx += _tlx;
		tly += _tly;
		tlz += _tlz;
		brx += _brx;
		bry += _bry;
		brz += _brz;
	}

	void SetCenter(const CVec3& pos)
	{
		_float	width	= Width(),
				height	= Height(),
				depth	= Depth();

		tlx = pos.x - width / 2;
		tly = pos.y - height / 2;
		tlz = pos.z - depth / 2;
		brx = pos.x + width / 2;
		bry = pos.y + height / 2;
		brz = pos.z + depth / 2;
	}

	void SetCenter(_float x, _float y, _float z)
	{
		_float	width	= Width(),
				height	= Height(),
				depth	= Depth();

		tlx = x - width / 2;
		tly = y - height / 2;
		tlz = z - depth / 2;
		brx = x + width / 2;
		bry = y + height / 2;
		brz = z + depth / 2;
	}

	CVec3 TopLeft() const
	{
		return CVec3(tlx, tly, tlz);
	}

	CVec3 TopRight() const
	{
		return CVec2<_float>(brx, tly);
	}

	CVec2<_float> BottomRight() const
	{
		return CVec2<_float>(brx, bry);
	}

	CVec2<_float> BottomLeft() const
	{
		return CVec2<_float>(tlx, bry);
	}

	CVec2<_float> Top() const
	{
		return CVec2<_float>(tlx + Width() / 2, tly);
	}

	CVec2<_float> Right() const
	{
		return CVec2<_float>(brx, tly + Height() / 2);
	}

	CVec2<_float> Bottom() const
	{
		return CVec2<_float>(tlx + Width() / 2, bry);
	}

	CVec2<_float> Left() const
	{
		return CVec2<_float>(tlx, tly + Height() / 2);
	}

	/*operator RECT ()
	{
		RECT rect = 
		{
			static_cast<UInt32>(tlx),	static_cast<UInt32>(tly),
			static_cast<UInt32>(brx),	static_cast<UInt32>(bry)
		};
		return rect;
	}*/

	_float tlx, tly, tlz, brx, bry, brz;
};

//////////////////////////////////////////////////////////////////////////

typedef CRect3D<Int32>	Rect3Di;
typedef CRect3D<Float>	Rect3Df;
typedef CRect3D<UInt32>	Rect3Du;
typedef CRect3D<Double>	Rect3Dd;

typedef Rect3Df			Rect3D;

} //END Namespace GLib

//////////////////////////////////////////////////////////////////////////