#pragma once

#include "vec.h"

class CVertex
{
public:
	CVertex(Vec3 pos = Vec3(0.0f, 0.0f, 0.0f), 
			Vec3 norm = Vec3(0.0f, 0.0f, -1.0f), 
			Float _tu = 0.0f, Float _tv = 0.0f, 
			UInt32 _color = 0xFFFFFFFF)	:	 
		position(pos), 
		normal(norm),
		tu(_tu), tv(_tv),
		color(_color)

	{
	}

	CVertex(Vec3 pos, 
			UInt32 _color)	:	 
		position(pos), 
		normal(Vec3(0.0f, 0.0f, -1.0f)),
		tu(0.0f), tv(0.0f),
		color(_color)

	{
	}

	CVertex(Vec3 pos, 
			Float _tu, Float _tv, 
			UInt32 _color)	:	 
		position(pos), 
		normal(Vec3(0.0f, 0.0f, -1.0f)),
		tu(_tu), tv(_tv),
		color(_color)

	{
	}

	Vec3	position;	
	Vec3	normal;
	UInt32	color;
	Float	tu, tv;

	static const Int32 	FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX0;
};

class CVertexPoint
{
public:
	CVertexPoint(Vec3 pos = Vec3(0.0f, 0.0f, 0.0f), 
			UInt32 _color = 0xFFFFFFFF)	:	 
		position(pos),
		color(_color)

	{
	}

	Vec3	position;	
	UInt32	color;

	static const Int32 	FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
};

extern const CVertex gk_RectVerts[4];

extern const WORD gk_RectIndices[6];