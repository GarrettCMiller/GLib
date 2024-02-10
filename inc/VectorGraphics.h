#pragma once

#include "D3DWrapper.h"

#include "Polynomial.h"
#include "2dDynamicArray.h"

#include "MathParsingUnit.h"

#include "Palette.h"

#include "Terrain.h"

#include "Rect.h"

//////////////////////////////////////////////////////////////////////////

typedef C2dDynamicArray<Float>	TDataPairArray;

//////////////////////////////////////////////////////////////////////////

class CVectorGraphics
{
public:
	typedef CDynamicArray<CVertex>	TPointArray;

	static const UInt32 mk_MaxPossibleVerts = 1024 * 32;

	struct GraphParams
	{
		GraphParams(Color c = Color::km_Black, Bool showPts = false, Bool showLine = true) :
			bShowLine(showLine), bShowPoints(showPts), 
				color(c), bgColor(Color::km_White)
		{	}

		Bool	bShowLine;
		Bool	bShowPoints;

		Palette	palette;

		Color	color;
		Color	bgColor;
	};

	struct GraphParamsParametric
	{
		GraphParamsParametric() {	}

		GraphParamsParametric(Float min, Float max, Float step) :
								tmin(min),	tmax(max), stept(step)
		{	}

		Float	tmin;
		Float	tmax;
		Float	stept;
	};

	struct GraphParams2D : public GraphParams
	{
		GraphParams2D()	{	}

		GraphParams2D(Float minx, Float maxx, Color c = Color::km_Black, Float stepx = 0.25f, 
					Bool showPts = false, Bool showLine = true) : GraphParams(c, showPts, showLine),
			xmin(minx), xmax(maxx), xstep(stepx)
		{	}

		Float	xmin;
		Float	xmax;
		Float	xstep;
	};

	struct GraphParams2DPara : public GraphParamsParametric, public GraphParams
	{
		GraphParams2DPara()	{	}

		GraphParams2DPara(Float min, Float max, Float step = 0.25f, Color c = Color::km_Black,
					Bool showPts = false, Bool showLine = true) : GraphParams(c, showPts, showLine), GraphParamsParametric(min, max, step)
		{	}
	};

	struct GraphParams3D : public GraphParams
	{
		GraphParams3D()	{	}

		GraphParams3D(Float minx, Float maxx, Float minz, Float maxz, 
						Color c = Color::km_Black, 
						Float stepx = 1.0f, Float stepz = 1.0f, 
					Bool showPts = false, Bool showLine = true, Bool showSurface = true) : GraphParams(c, showPts, showLine),
			xmin(minx), xmax(maxx), xstep(stepx), zmin(minz), zmax(maxz), zstep(stepz), bShowSurface(showSurface)
		{	}

		Bool	bShowSurface;

		Float	xmin;
		Float	xmax;
		Float	xstep;

		Float	zmin;
		Float	zmax;
		Float	zstep;
	};

	struct GraphParams3DPara : public GraphParamsParametric, public GraphParams3D
	{
		GraphParams3DPara()	{	}

		GraphParams3DPara(Float mint, Float maxt, Float minx, Float maxx, Float minz, Float maxz, 
							Color c = Color::km_Black,
							Float stept = 0.25f, Float stepx = 0.25f, Float stepz = 0.25f,
							Bool showPts = false, Bool showLine = true, Bool showSurface = true) : 
		GraphParams3D(minx, maxx, minz, maxz, c, stepx, stepz, showPts, showLine, showSurface),
			GraphParamsParametric(mint, maxt, stept)
		{	}
	};

	struct GraphParams2DF : public GraphParams
	{
		GraphParams2DF()	{	}

		GraphParams2DF(Float minx, Float maxx, Float miny, Float maxy, 
						Color c = Color::km_Red, 
						Float stepx = 1.0f, Float stepy = 1.0f, 
						Bool norm = true, Float len = 1.0f)
					: GraphParams(c),
			xmin(minx), xmax(maxx), xstep(stepx), ymin(miny), ymax(maxy), ystep(stepy),
			bNormalize(norm), fMultiplier(len)
		{	}

		Bool	bNormalize;

		Float	fMultiplier;

		Float	xmin;
		Float	xmax;
		Float	xstep;

		Float	ymin;
		Float	ymax;
		Float	ystep;
	};

	struct GraphParams3DF : public GraphParams
	{
		GraphParams3DF()	{	}

		GraphParams3DF(Float minx, Float maxx, Float miny, Float maxy, Float minz, Float maxz, 
						Color c = Color::km_Red, 
						Float stepx = 1.0f, Float stepy = 1.0f, Float stepz = 1.0f, 
						Bool norm = true, Float len = 1.0f)
					: GraphParams(c),
			xmin(minx), xmax(maxx), xstep(stepx), ymin(miny), ymax(maxy), ystep(stepy), zmin(minz), zmax(maxz), zstep(stepz),
			bNormalize(norm), fMultiplier(len)
		{	}

		Bool	bNormalize;

		Float	fMultiplier;

		Float	xmin;
		Float	xmax;
		Float	xstep;

		Float	ymin;
		Float	ymax;
		Float	ystep;

		Float	zmin;
		Float	zmax;
		Float	zstep;
	};

private:
	Bool	m_bUseDirectX;

	CDC*	m_pDC;

	D3DVertexBuffer m_vb;

	TPointArray	m_arrVerts;

	Terrain*	m_pTerrain;

	UInt32 m_numVerts;
	UInt32 m_MaxVerts;

public:
	CVectorGraphics(void);
	virtual ~CVectorGraphics(void);

	void SetDC(CDC* pdc)				{	m_pDC = pdc;			}

	Bool IsUsingDirectX() const			{	return m_bUseDirectX;	}
	void UseDirectX(Bool bUse = true)	{	m_bUseDirectX = bUse;	}

	void CreateLine(Float x1, Float y1, Float z1, Float x2, Float y2, Float z2, Color color = Color::km_White);
	void CreateLine(const Vec3& _p1, const Vec3& _p2, Color color = Color::km_White);

	void DrawBezier2(const Vec3& A, const Vec3& B,
					 UInt32 numSegments = 0, Color color = Color::km_White);

	void DrawBezier3(const Vec3& A, const Vec3& B, const Vec3& C, 
					 UInt32 numSegments = 0, Color color = Color::km_White);

	void DrawBezier4(const Vec3& A, const Vec3& B, const Vec3& C, const Vec3& D,
					 UInt32 numSegments = 0, Color color = Color::km_White);

	void DrawLine(const CVertex& _p1, const CVertex& _p2);

	void PushVertex(CVertex& p);
	void PurgeVertices();

	void SetMaxVerts(UInt32 maxVerts);

	////////////////////////////////////////////

	void DrawPointList(const TPointArray& arrVerts, UInt32 maxVerts = 0);

	void DrawLineStrip(const TPointArray& arrVerts, Bool bConnectEndPoints = true, UInt32 maxVerts = 0);
	void DrawLineList(const TPointArray& arrVerts, UInt32 maxVerts = 0);

	void DrawTriangleStrip(const TPointArray& arrVerts, UInt32 maxVerts = 0);
	void DrawTriangleList(const TPointArray& arrVerts, UInt32 maxVerts = 0);
	void DrawTriangleFan(const TPointArray& arrVerts, UInt32 maxVerts = 0);
	
	////////////////////////////////////////////

	void DrawCurrentPointList(UInt32 maxVerts = 0);

	void DrawCurrentLineStrip(Bool bConnectEndPoints = true, UInt32 maxVerts = 0);
	void DrawCurrentLineList(UInt32 maxVerts = 0);
	
	void DrawCurrentTriangleList(UInt32 maxVerts = 0);
	void DrawCurrentTriangleStrip(UInt32 maxVerts = 0);
	void DrawCurrentTriangleFan(UInt32 maxVerts = 0);

	void SetCurrentPointList(const TPointArray& arrVerts);

	void DrawCube(const GLib::Rect3D& rect, Color color = Color::km_White);

	/////////////////////////////////////////////

	void InitGraphVerts();

	void InitMeshObject(const GraphParams3D& params);

	void GraphScalarField2D(CMathParsingUnit& exp, const GraphParams2DF& params);
	void GraphScalarField2DPara(CMathParsingUnit& exp, const GraphParams2DPara& params);
	void GraphScalarField3D(CMathParsingUnit& exp, const GraphParams3DF& params);
	void GraphScalarField3DPara(CMathParsingUnit& exp, const GraphParams2DF& params);
	void GraphVectorField2D(CMathParsingUnit& exp, const GraphParams2DF& params);
	void GraphVectorField2DPara(CMathParsingUnit& exp, const GraphParams2DPara& params);
	void GraphVectorField3D(CMathParsingUnit& exp, const GraphParams3DF& params);
	void GraphVectorField3DPara(CMathParsingUnit& exp, const GraphParams2DPara& params);

	void Graph2D(const String& expression = "");
	void Graph2D(const CPolynomial& poly, const GraphParams2D& params);
	
	void Graph2D(CMathParsingUnit& exp, const GraphParams2D& params, CMathParsingUnit::TreeNodePtr pOverride = NULL);
	void Graph2DPara(CMathParsingUnit& exp, const GraphParams2DPara& params, CMathParsingUnit::TreeNodePtr pOverride = NULL);
	void Graph3D(CMathParsingUnit& exp, const GraphParams3D& params, CMathParsingUnit::TreeNodePtr pOverride = NULL);
	void Graph3DPara(CMathParsingUnit& exp, const GraphParams2DPara& params, CMathParsingUnit::TreeNodePtr pOverride = NULL);
	void Graph3DParaSurface(CMathParsingUnit& exp, const GraphParams3DPara& params, CMathParsingUnit::TreeNodePtr pOverride = NULL);
	
	void GraphData2D(const TDataPairArray& data, const GraphParams2D& params);
	void GraphData2D(const TPointArray& data, const GraphParams2D& params);

	void DrawGrid(Float w, Float h, Float xs, Float ys, Bool bHiLiteAxes = true);

	Bool Init(UInt32 numVerts = mk_MaxPossibleVerts);
};
