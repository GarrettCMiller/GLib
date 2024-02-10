#include "StdAfx.h"
#include "MathFuncs.h"

//////////////////////////////////////////////////////////////////////////

Float Distance(Vec3& p1, Vec3& p2)
{
	Float d = (p2.x - p1.x) * (p2.x - p1.x) + (p2.y- p1.y) * (p2.y - p1.y) + (p2.z - p1.z) * (p2.z - p1.z);
	d = sqrt(d);
	return d;
}

//////////////////////////////////////////////////////////////////////////

Float DistanceSquared(Vec3& p1, Vec3& p2)
{
	return ((p2.x - p1.x) * (p2.x - p1.x) + (p2.y- p1.y) * (p2.y - p1.y) + (p2.z - p1.z) * (p2.z - p1.z));
}