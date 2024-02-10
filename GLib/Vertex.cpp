#include "StdAfx.h"
#include "Vertex.h"
#include "Color.h"

//Full screen quad
const CVertex gk_RectVerts[4] = 
{
	//						Position					Normal						UV				Color
	/*	0	*/	CVertex(	Vec3(0.0f, 0.0f, 0.5f),		Vec3(0.0f, 0.0f, -1.0f),	0.0f, 1.0f,		Color(1.0f, 1.0f, 1.0f, 1.0f)),
	/*	1	*/	CVertex(	Vec3(1.0f, 0.0f, 0.5f),		Vec3(0.0f, 0.0f, -1.0f),	1.0f, 1.0f,		Color(1.0f, 1.0f, 1.0f, 1.0f)),
	/*	2	*/	CVertex(	Vec3(1.0f, 1.0f, 0.5f),		Vec3(0.0f, 0.0f, -1.0f),	1.0f, 0.0f,		Color(1.0f, 1.0f, 1.0f, 1.0f)),
	/*	3	*/	CVertex(	Vec3(0.0f, 1.0f, 0.5f),		Vec3(0.0f, 0.0f, -1.0f),	0.0f, 0.0f,		Color(1.0f, 1.0f, 1.0f, 1.0f)),
};

const WORD gk_RectIndices[6] =
{
 	0, 1, 3,
 	3, 1, 2,
};