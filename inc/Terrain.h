///////////////////////////////////////////////////////////////////////////
//		File:				Terrain.h
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		Terrain is basically a deformable heightmap
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "D3DWrapper.h"

#include "Functor.h"

#include "PhysicsObject.h"

//////////////////////////////////////////////////////////////////////////

#define MAX_ARGS			5
#define NUM_TEXTURES		3

#define DEF_TILE_SIZE		2.0f
#define DEF_WIDTH			32		//512
#define DEF_DEPTH			32		//512
#define WIDTH				DEF_WIDTH
#define DEPTH				DEF_DEPTH

#define DEF_SCALE			0.0666666f

//////////////////////////////////////////////////////////////////////////

class Terrain
{
public:
	typedef COpenFunctor3<Float, Float>	T3DFunction;
	
private:
	D3DVertexBuffer	m_VertexBuffer;  
	D3DIndexBuffer	m_IndexBuffer;

	D3DMesh		m_Mesh;

	Bool		m_bPhysics;

	CVertex*	m_Vertices;
	UInt32*		m_Indices;

	Float		m_fScale;
	Float		m_fTileSize;

	UInt32		m_Width; 
	UInt32		m_Depth;

	UInt32		m_NumVerts;
	UInt32		m_NumIndices;
	UInt32		m_NumPolys;
	UInt32		m_NumTiles;

	UInt32		m_NumTextures;
	UInt32		m_Texture[NUM_TEXTURES];

	Int32		m_ShaderID;
	String		m_ShaderTech;

	T3DFunction	m_pfnInputFunction;

	PhysicsRigidStaticPlane* m_Phys;

public:
	Terrain();
	~Terrain();

	RenderTarget	m_MainTexture;
	
	Terrain(const Terrain& rhs)
	{
		CopyFrom(rhs);
	}

	Terrain& operator = (const Terrain& rhs)
	{
		CopyFrom(rhs);
		return *this;
	}

	Bool		Init(UInt32 width, UInt32 depth, Float tilesize, Bool bPhysics = false);
	Bool		Shutdown();

	Bool		LoadHeightMap(String path, Float scale);

	Bool		Render();

	void		SetTextureID(Int32 texindex, UInt32 TexID);
	void		SetActiveTexture(UInt32 index);

	void		SetVertices();
	void		Set2DVertices();

	void		SetShaderID(Int32 sID);
	void		SetShaderTech(const String& tech);

	CVertex*	GetVertex(UInt32 x, UInt32 z)
	{
		return &m_Vertices[z * (m_Width + 1) + x];
	}

	CVertex*	GetVertex(UInt32 rawIndex)
	{
		return &m_Vertices[rawIndex];
	}

	void		SetInputFunction(const T3DFunction& pfnInput);

private:
	Bool		AllocateMemory( );
	Bool		FreeMemory( );

	void		RecalculateNormals();
	void		CreatePlasmaFractal( );

	Bool		PreRender();
	Bool		RenderTerrain();

	//void		SetVertices();
	void		SetTexcoords( );
	void		SetColors( );
	void		SetIndices( );

	void		CopyFrom(const Terrain& obj);
};