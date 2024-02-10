#pragma once

#include "Vec.h"

//////////////////////////////////////////////////////////////////////////

template <typename _float>
_float Distance(_float x1, _float y1, _float x2, _float y2)
{
	_float d = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
	d = sqrt(d);
	return d;
}

//////////////////////////////////////////////////////////////////////////

template <typename  _float>
_float DistanceSquared(_float x1, _float y1, _float x2, _float y2)
{
	return ((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

//////////////////////////////////////////////////////////////////////////

template <typename  _float>
_float Distance(CVec2<_float>& p1, CVec2<_float>& p2)
{
	_float d = (p2.X - p1.X) * (p2.X - p1.X) + (p2.Y - p1.Y) * (p2.Y - p1.Y);
	d = sqrt(d);
	return d;
}

//////////////////////////////////////////////////////////////////////////

template <typename  _float>
_float DistanceSquared(CVec2<_float>& p1, CVec2<_float>& p2)
{
	return ((p2.X - p1.X) * (p2.X - p1.X) + (p2.Y - p1.Y) * (p2.Y - p1.Y));
}

//////////////////////////////////////////////////////////////////////////

template <typename _float>
_float Distance(_float x1, _float y1, _float z1, _float x2, _float y2, _float z2)
{
	_float d = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1) + (z2 - z1) * (z2 - z1);
	d = sqrt(d);
	return d;
}

//////////////////////////////////////////////////////////////////////////

template <typename  _float>
_float DistanceSquared(_float x1, _float y1, _float z1, _float x2, _float y2, _float z2)
{
	return ((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1) + (z2 - z1) * (z2 - z1));
}

//////////////////////////////////////////////////////////////////////////

Float Distance(Vec3& p1, Vec3& p2);

//////////////////////////////////////////////////////////////////////////

Float DistanceSquared(Vec3& p1, Vec3& p2);