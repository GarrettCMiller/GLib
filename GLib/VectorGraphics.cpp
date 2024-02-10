#include "StdAfx.h"
#include "VectorGraphics.h"

#include "Globals.h"

CVectorGraphics::CVectorGraphics(void) : m_vb(NULL), m_numVerts(0), m_bUseDirectX(true), m_pTerrain(NULL)
{
}

CVectorGraphics::~CVectorGraphics(void)
{
	SAFE_DELETE(m_pTerrain);

	m_arrVerts.ReleaseBuffer();
	SAFE_RELEASE(m_vb);
}

//////////////////////////////////////////////////////////////////////////

Bool CVectorGraphics::Init(UInt32 numVerts /* = mk_MaxPossibleVerts */)
{
	SAFE_RELEASE(m_vb);

	if (FAILS(g_D3D->GetDevice()->CreateVertexBuffer(numVerts * sizeof(CVertex), 
		D3DUSAGE_WRITEONLY, 
		CVertex::FVF,
		D3DPOOL_DEFAULT,
		&m_vb, NULL)))
	{
		DxErr;
		return false;
	}

	m_MaxVerts = m_numVerts = numVerts;

	m_arrVerts.Resize(numVerts);

	return true;
}

//////////////////////////////////////////////////////////////////////////

void CVectorGraphics::PushVertex(CVertex& p)
{
	CVertex pt = p;
	m_arrVerts.Push(pt);

	if (m_MaxVerts > 0 && m_arrVerts.Size() > m_MaxVerts)
	{
		m_arrVerts.PopFront(pt);
	}
}

void CVectorGraphics::PurgeVertices()
{
	m_arrVerts.Clear();
}

//////////////////////////////////////////////////////////////////////////

void CVectorGraphics::SetMaxVerts(UInt32 maxVerts)
{
	m_MaxVerts = maxVerts;
}

//////////////////////////////////////////////////////////////////////////

void CVectorGraphics::CreateLine(Float x1, Float y1, Float z1, Float x2, Float y2, Float z2, Color color /* = Color::km_White */)
{
	CVertex p1(Vec3(x1, y1, z1), k_v3Zero, 0.0f, 0.0f, color),
			p2(Vec3(x2, y2, z2), k_v3Zero, 0.0f, 0.0f, color);

	CVertex* pData;

	if (FAILS(m_vb->Lock(0, sizeof(CVertex) * 2, (void **)&pData, 0)))
	{
		DxErr;
		return;
	}

	pData[0] = p1;
	pData[1] = p2;

	m_vb->Unlock();

	g_D3D->GetDevice()->SetStreamSource(0, m_vb, 0, sizeof(CVertex));

	g_D3D->GetDevice()->DrawPrimitive(D3DPT_LINELIST, 0, 1);
}

//////////////////////////////////////////////////////////////////////////

void CVectorGraphics::CreateLine(const Vec3& _p1, const Vec3& _p2, Color color /* = Color::km_White */)
{
	CVertex* pData;

	if (FAILS(m_vb->Lock(0, sizeof(CVertex) * 2, (void **)&pData, 0)))
	{
		DxErr;
		return;
	}

	pData[0] = CVertex(_p1, color);
	pData[1] = CVertex(_p2, color);

	m_vb->Unlock();

	g_D3D->GetDevice()->SetStreamSource(0, m_vb, 0, sizeof(CVertex));

	g_D3D->GetDevice()->DrawPrimitive(D3DPT_LINELIST, 0, 1);
}

//////////////////////////////////////////////////////////////////////////

void CVectorGraphics::DrawCube(const GLib::Rect3D& rect, Color color /*= Color::km_White*/)
{
	CVertex* pData = NULL;

	if (FAILS(m_vb->Lock(0, sizeof(CVertex) * 8, (void **)&pData, 0)))
	{
		DxErr;
		return;
	}

	pData[0] = CVertex(Vec3(rect.tlx, rect.tly, rect.tlz), color);
	pData[1] = CVertex(Vec3(rect.brx, rect.tly, rect.tlz), color);
	pData[2] = CVertex(Vec3(rect.tlx, rect.bry, rect.tlz), color);
	pData[3] = CVertex(Vec3(rect.tlx, rect.tly, rect.brz), color);
	pData[4] = CVertex(Vec3(rect.brx, rect.bry, rect.tlz), color);
	pData[5] = CVertex(Vec3(rect.brx, rect.tly, rect.brz), color);
	pData[6] = CVertex(Vec3(rect.tlx, rect.bry, rect.brz), color);
	pData[7] = CVertex(Vec3(rect.brx, rect.bry, rect.brz), color);

	m_vb->Unlock();

	g_D3D->GetDevice()->SetStreamSource(0, m_vb, 0, sizeof(CVertex));

	g_D3D->GetDevice()->DrawPrimitive(D3DPT_LINELIST, 0, 4);
}

//////////////////////////////////////////////////////////////////////////

void CVectorGraphics::DrawBezier2(const Vec3& A, const Vec3& B,
								  UInt32 numSegments /* = 0 */, Color color /* = Color::km_White */)
{
	TVertexArray verts;
	Vec3 temp;

	if (numSegments == 0)
		numSegments = 10;

	Float a = 0.0f, b = 1.0f - a, dt = 1.0f / numSegments;
	
	for (UInt32 i = 0; i < numSegments; i++)
	{
		a += dt;
		b = 1.0f - a;

		temp.x	= A.x * a	+	B.x * b;
		temp.y	= A.y * a	+	B.y * b;
		temp.z	= A.z * a	+	B.z * b;

		verts.Push(CVertex(temp, color));
	}

	DrawLineStrip(verts, false);
}

//////////////////////////////////////////////////////////////////////////

void CVectorGraphics::DrawBezier3(const Vec3& A, const Vec3& B, const Vec3& C, 
								  UInt32 numSegments /* = 0 */, Color color /* = Color::km_White */)
{
	TVertexArray verts;
	Vec3 temp;

	if (numSegments == 0)
		numSegments = 10;

	Float a = 0.0f, b = 1.0f - a, dt = 1.0f / numSegments;
	
	for (UInt32 i = 0; i < numSegments; i++)
	{
		a += dt;
		b = 1.0f - a;

		temp.x	= A.x * a * a	+	B.x * 2 * a * b		+	C.x * b * b;
		temp.y	= A.y * a * a	+	B.y * 2 * a * b		+	C.y * b * b;
		temp.z	= A.z * a * a	+	B.z * 2 * a * b		+	C.z * b * b;

		verts.Push(CVertex(temp, color));
	}

	DrawLineStrip(verts, false);
}

//////////////////////////////////////////////////////////////////////////

void CVectorGraphics::DrawBezier4(const Vec3& A, const Vec3& B, const Vec3& C, const Vec3& D,
								  UInt32 numSegments /* = 0 */, Color color /* = Color::km_White */)
{
	TVertexArray verts;
	Vec3 temp;

	if (numSegments == 0)
		numSegments = 10;

	Float a = 0.0f, b = 1.0f - a, dt = 1.0f / numSegments;
	
	for (UInt32 i = 0; i < numSegments; i++)
	{
		a += dt;
		b = 1.0f - a;

		temp.x	=	A.x * a * a * a			+	B.x * 3 * a * a * b		+
					C.x * 3 * a * b * b		+	D.x * b * b * b;

		temp.y	=	A.y * a * a * a			+	B.y * 3 * a * a * b		+
					C.y * 3 * a * b * b		+	D.y * b * b * b;

		temp.z	=	A.z * a * a * a			+	B.z * 3 * a * a * b		+
					C.z * 3 * a * b * b		+	D.z * b * b * b;

		verts.Push(CVertex(temp, color));
	}

	DrawLineStrip(verts, false);
}

//////////////////////////////////////////////////////////////////////////

void CVectorGraphics::DrawLine(const CVertex& p1, const CVertex& p2)
{
	CVertex* pData;

	if (FAILS(m_vb->Lock(0, sizeof(CVertex) * 2, (void **)&pData, 0)))
	{
		DxErr;
		return;
	}

	pData[0] = p1;
	pData[1] = p2;

	m_vb->Unlock();

	g_D3D->GetDevice()->SetStreamSource(0, m_vb, 0, sizeof(CVertex));

	g_D3D->GetDevice()->DrawPrimitive(D3DPT_LINELIST, 0, 1);
}

//////////////////////////////////////////////////////////////////////////

void CVectorGraphics::DrawLineStrip(const TPointArray& arrVerts, Bool bConnectEndPoints /* = true */, UInt32 maxVerts /* = 0 */)
{
	UInt32 numVerts = arrVerts.Size();
	UInt32 numPrims = numVerts - (bConnectEndPoints ? 0 : 1);

	if (maxVerts > 0)
	{
		maxVerts = Min(maxVerts, numVerts);
		numPrims = Min(maxVerts, numPrims);
	}

	if (numVerts < 2 || numPrims == 0)
		return;

	ASSERT(!arrVerts.IsEmpty());
	ASSERT(arrVerts.Size() <= m_numVerts);

	if (IsUsingDirectX())
	{
		UInt32 memSize = sizeof(CVertex) * numVerts;

		CVertex* pData;

		if (FAILS(m_vb->Lock(0, memSize, (void **)&pData, 0)))
		{
			DxErr;
			return;
		}

		memcpy(pData, &arrVerts[0], memSize);
		
		if (bConnectEndPoints)
			pData[numVerts] = pData[0];

		m_vb->Unlock();

		g_D3D->GetDevice()->SetStreamSource(0, m_vb, 0, sizeof(CVertex));

		g_D3D->GetDevice()->DrawPrimitive(D3DPT_LINESTRIP, 0, numPrims);
	}
	else
	{
		ASSERT(m_pDC);

		/*m_pDC->MoveTo(arrVerts[0].position);

		for (UInt32 i = 1; i < numVerts; i++)
		{
			m_pDC->LineTo(arrVerts[i].position);
		}

		if (bConnectEndPoints)
			m_pDC->LineTo(arrVerts[0].position);*/
	}
}

//////////////////////////////////////////////////////////////////////////

void CVectorGraphics::DrawLineList(const TPointArray& arrVerts, UInt32 maxVerts /* = 0 */)
{
	UInt32 numVerts = arrVerts.Size();
	UInt32 numPrims = numVerts / 2;

	if (maxVerts > 0)
	{
		maxVerts = Min(maxVerts, numVerts);
		numPrims = Min(maxVerts, numPrims);
	}

	UInt32 memSize = sizeof(CVertex) * numPrims;

	ASSERT(!arrVerts.IsEmpty());

	CVertex* pData;

	if (FAILS(m_vb->Lock(0, 0, (void **)&pData, 0)))
	{
		DxErr;
		return;
	}

	memcpy(pData, &arrVerts[0], memSize);

	m_vb->Unlock();

	g_D3D->GetDevice()->SetStreamSource(0, m_vb, 0, sizeof(CVertex));

	g_D3D->GetDevice()->DrawPrimitive(D3DPT_LINELIST, 0, numPrims);
}

//////////////////////////////////////////////////////////////////////////

void CVectorGraphics::DrawPointList(const TPointArray& arrVerts, UInt32 maxVerts /* = 0 */)
{
	UInt32 numVerts = arrVerts.Size();
	UInt32 numPrims = numVerts;

	if (maxVerts > 0)
	{
		maxVerts = Min(maxVerts, numVerts);
		numPrims = Min(maxVerts, numPrims);
	}

	ASSERT(!arrVerts.IsEmpty());
	ASSERT(!arrVerts.Size() < 1024);

	if (IsUsingDirectX())
	{
		UInt32 memSize = sizeof(CVertex) * numPrims;

		CVertex* pData;

		if (FAILS(m_vb->Lock(0, memSize, (void **)&pData, 0)))
		{
			DxErr;
			return;
		}

		const CVertex & vTemp = arrVerts[0];

		memcpy(pData, &vTemp, memSize);

		m_vb->Unlock();

		g_D3D->GetDevice()->SetStreamSource(0, m_vb, 0, sizeof(CVertex));

		g_D3D->GetDevice()->DrawPrimitive(D3DPT_POINTLIST, 0, numPrims);
	}
	else
	{
		/*ASSERT(m_pDC);

		Float x = 0.0f, y = 0.0f;

		for (UInt32 i = 0; i < numVerts; i++)
		{
			x = arrVerts[i].position.x;
			y = arrVerts[i].position.y;
			m_pDC->Rectangle(x - 5, y - 5, x + 5, y + 5);
		}*/
	}
}

//////////////////////////////////////////////////////////////////////////

void CVectorGraphics::DrawTriangleList(const TPointArray& arrVerts, UInt32 maxVerts /* = 0 */)
{
	UInt32 numVerts = arrVerts.Size();
	UInt32 numPrims = numVerts / 3;

	if (maxVerts > 0)
	{
		maxVerts = Min(maxVerts, numVerts);
		numPrims = Min(maxVerts, numPrims);
	}

	UInt32 memSize = sizeof(CVertex) * numPrims;

	ASSERT(!arrVerts.IsEmpty());

	CVertex* pData;

	if (FAILS(m_vb->Lock(0, 0, (void **)&pData, 0)))
	{
		DxErr;
		return;
	}

	memcpy(pData, &arrVerts[0], memSize);

	m_vb->Unlock();

	g_D3D->GetDevice()->SetStreamSource(0, m_vb, 0, sizeof(CVertex));

	g_D3D->GetDevice()->DrawPrimitive(D3DPT_TRIANGLELIST, 0, numPrims);
}

//////////////////////////////////////////////////////////////////////////

void CVectorGraphics::DrawTriangleStrip(const TPointArray& arrVerts, UInt32 maxVerts /* = 0 */)
{
	UInt32 numVerts = arrVerts.Size();
	UInt32 numPrims = numVerts - 2;

	if (maxVerts > 0)
	{
		maxVerts = Min(maxVerts, numVerts);
		numPrims = Min(maxVerts, numPrims);
	}

	UInt32 memSize = sizeof(CVertex) * numPrims;

	ASSERT(!arrVerts.IsEmpty());

	CVertex* pData;

	if (FAILS(m_vb->Lock(0, 0, (void **)&pData, 0)))
	{
		DxErr;
		return;
	}

	memcpy(pData, &arrVerts[0], memSize);

	m_vb->Unlock();

	g_D3D->GetDevice()->SetStreamSource(0, m_vb, 0, sizeof(CVertex));

	g_D3D->GetDevice()->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, numPrims);
}

//////////////////////////////////////////////////////////////////////////

void CVectorGraphics::DrawTriangleFan(const TPointArray& arrVerts, UInt32 maxVerts /* = 0 */)
{
	UInt32 numVerts = arrVerts.Size();
	UInt32 numPrims = numVerts - 2;

	if (maxVerts > 0)
	{
		maxVerts = Min(maxVerts, numVerts);
		numPrims = Min(maxVerts, numPrims);
	}

	UInt32 memSize = sizeof(CVertex) * numPrims;

	ASSERT(!arrVerts.IsEmpty());

	CVertex* pData;

	if (FAILS(m_vb->Lock(0, 0, (void **)&pData, 0)))
	{
		DxErr;
		return;
	}

	memcpy(pData, &arrVerts[0], memSize);

	m_vb->Unlock();

	g_D3D->GetDevice()->SetStreamSource(0, m_vb, 0, sizeof(CVertex));

	g_D3D->GetDevice()->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, numPrims);
}

//////////////////////////////////////////////////////////////////////////

void CVectorGraphics::SetCurrentPointList(const TPointArray& arrVerts)
{
	m_arrVerts = arrVerts;
}

//////////////////////////////////////////////////////////////////////////

void CVectorGraphics::DrawCurrentLineStrip(Bool bConnectEndPoints /* = true */, UInt32 maxVerts /* = 0 */)
{
	DrawLineStrip(m_arrVerts, bConnectEndPoints, maxVerts);
}

//////////////////////////////////////////////////////////////////////////

void CVectorGraphics::DrawCurrentLineList(UInt32 maxVerts /* = 0 */)
{
	DrawLineList(m_arrVerts, maxVerts);
}

//////////////////////////////////////////////////////////////////////////

void CVectorGraphics::DrawCurrentTriangleStrip(UInt32 maxVerts /* = 0 */)
{
	DrawTriangleStrip(m_arrVerts, maxVerts);
}

//////////////////////////////////////////////////////////////////////////

void CVectorGraphics::DrawCurrentTriangleList(UInt32 maxVerts /* = 0 */)
{
	DrawTriangleList(m_arrVerts, maxVerts);
}

//////////////////////////////////////////////////////////////////////////

void CVectorGraphics::DrawCurrentTriangleFan(UInt32 maxVerts /* = 0 */)
{
	DrawTriangleList(m_arrVerts, maxVerts);
}

//////////////////////////////////////////////////////////////////////////

void CVectorGraphics::DrawCurrentPointList(UInt32 maxVerts /* = 0 */)
{
	DrawPointList(m_arrVerts, maxVerts);
}

//////////////////////////////////////////////////////////////////////////

void CVectorGraphics::InitGraphVerts()
{
	//m_arrVerts.Resize(500);
}

//////////////////////////////////////////////////////////////////////////

void CVectorGraphics::InitMeshObject(const GraphParams3D& params)
{
	UInt32	numStepsX = static_cast<UInt32>((params.xmax - params.xmin) / params.xstep),
			numStepsZ = static_cast<UInt32>((params.zmax - params.zmin) / params.zstep);

	m_pTerrain = new Terrain;
	m_pTerrain->Init(numStepsX, numStepsZ, params.zstep);
}

//////////////////////////////////////////////////////////////////////////

void CVectorGraphics::Graph2D(const String& expression /* = "" */)
{	
	Float	xi = -2.0f,
			xf = 2.0f,
			dx = 0.125f;

	UInt32 numSteps = static_cast<UInt32>((xf - xi) / dx);

	Float	x = xi,
			y = 0;

	static Float a = 0.125f;

	expression.c_str();

	//CVertex* temp = NULL;

	for (UInt32 StepX = 0; StepX < numSteps; StepX++, x += dx)
	{
		//temp = new CVertex;

		//y = sin(x);//Sine(1.0f, 0.0f, 1.0f, x);

		y = (1 / (a * sqrtf(2 * PI))) * exp(-((x*x) / (2*a) * (2*a)) );

		m_arrVerts[StepX].position = Vec3(x, y, 0.0f);
		m_arrVerts[StepX].color = Color(1.0f, 1.0f, 1.0f, 1.0f);

		//m_arrVerts[StepX] = pVerts[StepX];
		//PushVertex(pVerts[StepX]);
	}

	DrawCurrentLineStrip(false);
	DrawCurrentPointList();

	//m_arrVerts.FreePointers();
	//m_arrVerts.Clear();
}

//////////////////////////////////////////////////////////////////////////

void CVectorGraphics::Graph2D(const CPolynomial& poly, const GraphParams2D& params)
{
	Float	xi = params.xmin,
			xf = params.xmax,
			dx = params.xstep;

	UInt32 numSteps = static_cast<UInt32>((xf - xi) / dx);

	Float	x = xi,
			y = 0;

	static Float a = 0.125f;

	//CVertex* temp = NULL;

	for (UInt32 StepX = 0; StepX <= numSteps; StepX++, x += dx)
	{
		//y = sin(x);//Sine(1.0f, 0.0f, 1.0f, x);

		//y = (1 / (a * sqrtf(2 * PI))) * exp(-((x*x) / (2*a) * (2*a)) );
		y = poly.Evaluate(x);

		m_arrVerts[StepX].position = Vec3(x, y, 0.51f);
		m_arrVerts[StepX].color = params.color;

		//m_arrVerts[StepX] = pVerts[StepX];
		//PushVertex(pVerts[StepX]);
	}

	if (params.bShowLine)
		DrawCurrentLineStrip(false, numSteps);

	if (params.bShowPoints)
		DrawCurrentPointList(numSteps + 1);

	//m_arrVerts.FreePointers();
	//m_arrVerts.Clear();
}

//////////////////////////////////////////////////////////////////////////

void CVectorGraphics::Graph2D(CMathParsingUnit& solver, const GraphParams2D& params, CMathParsingUnit::TreeNodePtr pOverride /* = NULL */)
{
	Float	xi = params.xmin,
			xf = params.xmax,
			dx = params.xstep;

	UInt32 numSteps = static_cast<UInt32>((xf - xi) / dx);

	Float	x = xi,
			y = 0;

	if (solver.GetDependentVariables().IsEmpty())
		return;

	GCAS::CNode*	varInd = solver.GetIndependentVariables()[0];
	GCAS::CNode*	varDep = solver.GetDependentVariables()[0];

	CMathParsingUnit::TreeNodePtr pTemp = solver.GetCurrentExpression();

	if (pOverride)
		solver.SetExpression(pOverride);

	solver.SolveFor(varDep);

	for (UInt32 StepX = 0; StepX <= numSteps; StepX++, x += dx)
	{
		static_cast<GCAS::FVal*>(varInd)->SetValue(x);

		y = solver.EvaluateFor<Float>(varDep);
		
		//y = varDep->GetValue();

		m_arrVerts[StepX].position = Vec3(x, y, 0.51f);
		m_arrVerts[StepX].color = params.color;
	}

	if (pTemp)
		solver.SetExpression(pTemp);

	if (params.bShowLine)
		DrawCurrentLineStrip(false, numSteps);

	if (params.bShowPoints)
		DrawCurrentPointList(numSteps + 1);
}

//////////////////////////////////////////////////////////////////////////

void CVectorGraphics::Graph2DPara(CMathParsingUnit& solver, const GraphParams2DPara& params, CMathParsingUnit::TreeNodePtr pOverride /* = NULL */)
{
	Float	ti			= params.tmin,
			tf			= params.tmax,
			dt			= params.stept;

	UInt32	numSteps	= static_cast<UInt32>((tf - ti) / dt);

	Float	t = ti,
			x = 0.0f,
			y = 0.0f;

	Vec3	r;

	UInt32	vIndex = 0;

	if (solver.GetDependentVariables().IsEmpty())
		return;

	CMathParsingUnit::TVariableArray indVars = solver.GetIndependentVariables();
	CMathParsingUnit::TVariableArray depVars = solver.GetDependentVariables();

	GCAS::CNode* varInd		= indVars[0];
	GCAS::CNode* varDep[3]	= {NULL, NULL, NULL};

	do varDep[vIndex] = depVars[vIndex];
	while (++vIndex < depVars.Size());

	CMathParsingUnit::TreeNodePtr pTemp = solver.GetCurrentExpression();

	//if (!solver.SolveFor(varDep))
		//return; //TODO: Make better error catching

	for (UInt32 StepT = 0; StepT <= numSteps; StepT++, t += dt)
	{
		static_cast<GCAS::FVal*>(varInd)->SetValue(t);
		
		solver.SetExpression(solver.GetEquation(0));
		x = solver.EvaluateFor<Float>(varDep[0]);

		solver.SetExpression(solver.GetEquation(1));
		y = solver.EvaluateFor<Float>(varDep[1]);
		
		if (pOverride)
			solver.SetExpression(pOverride);
		else
			solver.SetExpression(pTemp);

		r = solver.EvaluateFor<Vec3>(varDep[2]);

		m_arrVerts[StepT].position = r;
		m_arrVerts[StepT].position.z = 0.51f;
		m_arrVerts[StepT].color = params.color;
	}

	if (pTemp)
		solver.SetExpression(pTemp);

	if (params.bShowLine)
		DrawCurrentLineStrip(false, numSteps);

	if (params.bShowPoints)
		DrawCurrentPointList(numSteps + 1);
}

//////////////////////////////////////////////////////////////////////////

void CVectorGraphics::Graph3D(CMathParsingUnit& solver, const GraphParams3D& params, CMathParsingUnit::TreeNodePtr pOverride /* = NULL */)
{
	Float	xi = params.xmin,
			xf = params.xmax,
			zi = params.zmin,
			zf = params.zmax,
			dx = params.xstep,
			dz = params.zstep;

	UInt32	numStepsX = static_cast<UInt32>((xf - xi) / dx),
			numStepsZ = static_cast<UInt32>((zf - zi) / dz);

	Float	x = xi,
			y = 0,
			z = zi;

	if (solver.GetDependentVariables().IsEmpty())
		return;

	GCAS::CNode*	varInd1 = solver.GetIndependentVariables()[0];
	GCAS::CNode*	varInd2 = solver.GetIndependentVariables()[1];
	GCAS::CNode*	varDep = solver.GetDependentVariables()[0];

	CMathParsingUnit::TreeNodePtr pTemp = solver.GetCurrentExpression();

	if (pOverride)
		solver.SetExpression(pOverride);

	solver.SolveFor(varDep);

	for (UInt32 StepZ = 0; StepZ <= numStepsZ; StepZ++, z += dz)
	{
		static_cast<GCAS::FVal*>(varInd2)->SetValue(z);

		x = xi;

		for (UInt32 StepX = 0; StepX <= numStepsX; StepX++, x += dx)
		{
			static_cast<GCAS::FVal*>(varInd1)->SetValue(x);

			y = solver.EvaluateFor<Float>(varDep);
			
			//y = varDep->GetValue();

			m_pTerrain->GetVertex(StepX, StepZ)->position = Vec3(x, y, z);
			m_pTerrain->GetVertex(StepX, StepZ)->color = params.color;
		}
	}

	if (pTemp)
		solver.SetExpression(pTemp);

	DWORD prevFM;
	DWORD prevBT;

	g_D3D->GetDevice()->GetRenderState(D3DRS_ZENABLE, &prevBT);
	g_D3D->GetDevice()->GetRenderState(D3DRS_FILLMODE, &prevFM);

	if (params.bShowSurface)
	{
		g_D3D->GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		m_pTerrain->Render();
	}

	if (params.bShowLine)
	{
		if (params.bShowSurface)
			g_D3D->GetDevice()->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);

		g_D3D->GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		m_pTerrain->Render();
	}

	if (params.bShowPoints)
	{
		if (params.bShowLine)
			g_D3D->GetDevice()->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);

		g_D3D->GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);
		m_pTerrain->Render();
	}
	
	g_D3D->GetDevice()->SetRenderState(D3DRS_ZENABLE, prevBT);
	g_D3D->GetDevice()->SetRenderState(D3DRS_FILLMODE, prevFM);
}

//////////////////////////////////////////////////////////////////////////

void CVectorGraphics::Graph3DPara(CMathParsingUnit& solver, const GraphParams2DPara& params, CMathParsingUnit::TreeNodePtr pOverride /* = NULL */)
{
	Float	ti			= params.tmin,
			tf			= params.tmax,
			dt			= params.stept;

	UInt32	numSteps	= static_cast<UInt32>((tf - ti) / dt);

	Float	t = ti,
			x = 0.0f,
			y = 0.0f,
			z = 0.0f;

	Vec3	r;

	UInt32	vIndex = 0;

	if (solver.GetDependentVariables().IsEmpty())
		return;

	CMathParsingUnit::TVariableArray indVars = solver.GetIndependentVariables();
	CMathParsingUnit::TVariableArray depVars = solver.GetDependentVariables();

	GCAS::CNode* varInd		= indVars[0];
	GCAS::CNode* varDep[3]	= {NULL, NULL, NULL};

	do varDep[vIndex] = depVars[vIndex];
	while (++vIndex < depVars.Size());

	CMathParsingUnit::TreeNodePtr pTemp = solver.GetCurrentExpression();

	//if (!solver.SolveFor(varDep))
		//return; //TODO: Make better error catching

	for (UInt32 StepT = 0; StepT <= numSteps; StepT++, t += dt)
	{
		static_cast<GCAS::FVal*>(varInd)->SetValue(t);
		
		solver.SetExpression(solver.GetEquation(0));
		x = solver.EvaluateFor<Float>(varDep[0]);

		solver.SetExpression(solver.GetEquation(1));
		y = solver.EvaluateFor<Float>(varDep[1]);
		
		solver.SetExpression(solver.GetEquation(2));
		z = solver.EvaluateFor<Float>(varDep[2]);
		
		if (pOverride)
			solver.SetExpression(pOverride);
		else
			solver.SetExpression(pTemp);

		r = solver.EvaluateFor<Vec3>(varDep[3]);

		m_arrVerts[StepT].position = r;
		m_arrVerts[StepT].color = params.color;
	}

	if (pTemp)
		solver.SetExpression(pTemp);

	if (params.bShowLine)
		DrawCurrentLineStrip(false, numSteps);

	if (params.bShowPoints)
		DrawCurrentPointList(numSteps + 1);
}

//////////////////////////////////////////////////////////////////////////

void CVectorGraphics::Graph3DParaSurface(CMathParsingUnit& solver, const GraphParams3DPara& params, CMathParsingUnit::TreeNodePtr pOverride /* = NULL */)
{
	Float	ti			= params.tmin,
			tf			= params.tmax,
			dt			= params.stept,
			xi			= params.xmin,
			xf			= params.xmin,
			dx			= params.xstep,
			zi			= params.zmin,
			zf			= params.zmax,
			dz			= params.zstep;

	UInt32	numStepsT	= static_cast<UInt32>((tf - ti) / dt),
			numStepsX	= static_cast<UInt32>((xf - xi) / dx),
			numStepsZ	= static_cast<UInt32>((zf - zi) / dz);

	Float	t = ti,
			x = xi,
			y = 0.0f,
			z = zi;

	Vec3	r;

	UInt32	vIndex = 0;

	if (solver.GetDependentVariables().IsEmpty())
		return;

	CMathParsingUnit::TVariableArray indVars = solver.GetIndependentVariables();
	CMathParsingUnit::TVariableArray depVars = solver.GetDependentVariables();

	GCAS::CNode* varInd		= indVars[0];
	GCAS::CNode* varDep[2]	= {NULL, NULL};

	do varDep[vIndex] = depVars[vIndex];
	while (++vIndex < depVars.Size());

	CMathParsingUnit::TreeNodePtr pTemp = solver.GetCurrentExpression();

	//if (!solver.SolveFor(varDep))
		//return; //TODO: Make better error catching

	for (UInt32 StepT = 0; StepT <= numStepsT; StepT++, t += dt)
	{
		static_cast<GCAS::FVal*>(varInd)->SetValue(t);

		for (UInt32 StepZ = 0; StepZ <= numStepsZ; StepZ++, z += dz)
		{
			static_cast<GCAS::FVal*>(varDep[1])->SetValue(z);

			for (UInt32 StepX = 0; StepX <= numStepsX; StepX++, x += dx)
			{
				static_cast<GCAS::FVal*>(varDep[0])->SetValue(x);

				y = solver.EvaluateFor<Float>(varDep[2]);

				m_pTerrain->GetVertex(StepX, StepZ)->position = Vec3(x, y, z);
				m_pTerrain->GetVertex(StepX, StepZ)->color = params.color;
			}
		}
	}

	DWORD prevFM;
	DWORD prevBT;

	g_D3D->GetDevice()->GetRenderState(D3DRS_ZENABLE, &prevBT);
	g_D3D->GetDevice()->GetRenderState(D3DRS_FILLMODE, &prevFM);

	if (params.bShowSurface)
	{
		g_D3D->GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		m_pTerrain->Render();
	}

	if (params.bShowLine)
	{
		if (params.bShowSurface)
			g_D3D->GetDevice()->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);

		g_D3D->GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		m_pTerrain->Render();
	}

	if (params.bShowPoints)
	{
		if (params.bShowLine)
			g_D3D->GetDevice()->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);

		g_D3D->GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);
		m_pTerrain->Render();
	}
	
	g_D3D->GetDevice()->SetRenderState(D3DRS_ZENABLE, prevBT);
	g_D3D->GetDevice()->SetRenderState(D3DRS_FILLMODE, prevFM);
}

//////////////////////////////////////////////////////////////////////////

void CVectorGraphics::GraphData2D(const TDataPairArray& data, const GraphParams2D& params)
{
	UInt32 numSteps = data.Width();

	Float	x = 0.0f,
			y = 0.0f;

	for (UInt32 StepX = 0; StepX < numSteps; StepX++)
	{
		x = data(StepX, 0);
		y = data(StepX, 1);

		m_arrVerts[StepX].position = Vec3(x, y, 0.51f);
		m_arrVerts[StepX].color = params.color;
	}

	if (params.bShowLine)
		DrawCurrentLineStrip(false, numSteps);

	if (params.bShowPoints)
		DrawCurrentPointList(numSteps + 1);
}

void CVectorGraphics::GraphData2D(const TPointArray& data, const GraphParams2D& params)
{
	UInt32 numSteps = data.Size();

	GraphParams2D gParams = params;

	Float	x = 0.0f,
			y = 0.0f;

	for (UInt32 axis = 0; axis < 3; axis++)
	{
		for (UInt32 StepX = 0; StepX < numSteps; StepX++)
		{
			x = StepX;

			switch (axis)
			{
			case 0:	y = data[StepX].position.x; break;
			case 1:	y = data[StepX].position.y; break;
			case 2:	y = data[StepX].position.z; break;
			}

			m_arrVerts[StepX].position = Vec3(x, y, 0.51f);
			
			switch (axis)
			{
			case 0: m_arrVerts[StepX].color = Color::km_Red; break;
			case 1: m_arrVerts[StepX].color = Color::km_Green; break;
			case 2: m_arrVerts[StepX].color = Color::km_Blue; break;
			}
		}

		if (params.bShowLine)
			DrawCurrentLineStrip(false, numSteps);

		if (params.bShowPoints)
			DrawCurrentPointList(numSteps + 1);
	}
}

void CVectorGraphics::DrawGrid(Float w, Float h, Float xs, Float ys, Bool bHiLiteAxes /* = true */)
{
	CVertex p1, p2;
	Color color;
	Float alph = 0.75f;

	//Draw horizontal lines
	for (Float y = -h; y <= h; y += ys)
	{
		if (y == 0.0f && bHiLiteAxes)
			color = Color::km_LightGray;
		else if (static_cast<Int32>(y) == y)
			color = Color::km_Gray;
		else
			color = Color::km_DarkGray;

		color.SetAlpha(alph);

		p1 = CVertex(Vec3(-w, y, 0.9f), color);
		p2 = CVertex(Vec3(w, y, 0.9f), color);
		DrawLine(p1, p2);
	}

	//Draw vertical lines
	for (Float x = -w; x <= w; x += xs)
	{
		if (x == 0.0f && bHiLiteAxes)
			color = Color::km_LightGray;
		else if (static_cast<Int32>(x) == x)
			color = Color::km_Gray;
		else
			color = Color::km_DarkGray;

		color.SetAlpha(alph);

		p1 = CVertex(Vec3(x, -h, 0.9f), color);
		p2 = CVertex(Vec3(x, h, 0.9f), color);
		DrawLine(p1, p2);
	}
}

//////////////////////////////////////////////////////////////////////////

void CVectorGraphics::GraphVectorField2D(CMathParsingUnit& solver, const GraphParams2DF& params)
{
	Float	xi = params.xmin,
			xf = params.xmax,
			yi = params.ymin,
			yf = params.ymax,
			dx = params.xstep,
			dy = params.ystep;

	UInt32	numStepsX = static_cast<UInt32>((xf - xi) / dx),
			numStepsY = static_cast<UInt32>((yf - yi) / dy);

	Float	x = xi,
			y = yi,
			z = 0.51;

	Vec2	vR;

	//if (solver.GetDependentVariables().IsEmpty())
		//return;

	/*GCAS::CNode*	varInd1 = solver.GetIndependentVariables()[0];
	GCAS::CNode*	varInd2 = solver.GetIndependentVariables()[1];
	GCAS::CNode*	varDep = solver.GetDependentVariables()[0];*/

	Vec3 pos, post;
	UInt32 vIndex = 0;

	//CMathParsingUnit::TreeNodePtr pTemp = solver.GetCurrentExpression();

	/*if (pOverride)
		solver.SetExpression(pOverride);*/

	//solver.SolveFor(varDep);

	for (UInt32 StepY = 0; StepY <= numStepsY; StepY++, y += dy)
	{
		//static_cast<GCAS::FVal*>(varInd2)->SetValue(z);

		x = xi;

		for (UInt32 StepX = 0; StepX <= numStepsX; StepX++, x += dx)
		{
			//static_cast<GCAS::FVal*>(varInd1)->SetValue(x);

			//y = solver.EvaluateFor(varDep);
			
			//y = varDep->GetValue();
			vIndex = (StepY * numStepsX + StepX) * 2;
			
			pos = Vec3(x, y, 0.51f);

			m_arrVerts[vIndex].position = pos;
			m_arrVerts[vIndex].color = params.bgColor;

			post = Vec2(-y, x);
			
			if (params.bNormalize)
				post.Normalize();

			post *= float(params.fMultiplier);

			pos += post;
			
			m_arrVerts[vIndex + 1].position = pos;
			m_arrVerts[vIndex + 1].color = params.color;
		}
	}

	/*if (pTemp)
		solver.SetExpression(pTemp);*/

	DrawCurrentLineList();
}

void CVectorGraphics::GraphVectorField3D(CMathParsingUnit& exp, const GraphParams3DF& params)
{
	Float	xi = params.xmin,
			xf = params.xmax,
			yi = params.ymin,
			yf = params.ymax,
			zi = params.zmin,
			zf = params.zmax,
			dx = params.xstep,
			dy = params.ystep,
			dz = params.zstep;

	UInt32	numStepsX = static_cast<UInt32>((xf - xi) / dx),
			numStepsY = static_cast<UInt32>((yf - yi) / dy),
			numStepsZ = static_cast<UInt32>((zf - zi) / dz);

	Float	x = xi,
			y = yi,
			z = zi;

	/*if (solver.GetDependentVariables().IsEmpty())
		return;

	GCAS::CNode*	varInd1 = solver.GetIndependentVariables()[0];
	GCAS::CNode*	varInd2 = solver.GetIndependentVariables()[1];
	GCAS::CNode*	varDep = solver.GetDependentVariables()[0];*/

	Vec3 pos, post;
	UInt32 vIndex = 0;

	//CMathParsingUnit::TreeNodePtr pTemp = solver.GetCurrentExpression();

	/*if (pOverride)
		solver.SetExpression(pOverride);*/

	//solver.SolveFor(varDep);

	for (UInt32 StepZ = 0; StepZ <= numStepsZ; StepZ++, z += dz)
	{
		//static_cast<GCAS::FVal*>(varInd2)->SetValue(z);

		y = yi;

		for (UInt32 StepY = 0; StepY <= numStepsY; StepY++, y += dy)
		{
			//static_cast<GCAS::FVal*>(varInd2)->SetValue(y);

			x = xi;

			for (UInt32 StepX = 0; StepX <= numStepsX; StepX++, x += dx)
			{
				//static_cast<GCAS::FVal*>(varInd1)->SetValue(x);

				//y = solver.EvaluateFor(varDep);
				
				//y = varDep->GetValue();
				vIndex = (	StepZ * numStepsX * numStepsY + 
							StepY * numStepsX +
							StepX) * 2;
				
				pos = Vec3(x, y, z);

				m_arrVerts[vIndex].position = pos;
				m_arrVerts[vIndex].color = params.bgColor;

				///////////////////////////////////////////
				post = Vec3(-fabsf(x), -fabsf(y), -fabsf(z));
				///////////////////////////////////////////

				if (params.bNormalize)
					post.Normalize();

				post *= params.fMultiplier;

				pos += post;

				m_arrVerts[vIndex + 1].position = pos;
				m_arrVerts[vIndex + 1].color = params.color;
			}
		}
	}

	/*if (pTemp)
		solver.SetExpression(pTemp);*/

	DrawCurrentLineList();
}

//////////////////////////////////////////////////////////////////////////

void CVectorGraphics::GraphVectorField2DPara(CMathParsingUnit& exp, const GraphParams2DPara& params)
{
#if 0
	Float	xi = params.xmin,
			xf = params.xmax,
			yi = params.ymin,
			yf = params.ymax,
			zi = params.zmin,
			zf = params.zmax,
			dx = params.xstep,
			dy = params.ystep,
			dz = params.zstep;

	UInt32	numStepsX = static_cast<UInt32>((xf - xi) / dx),
			numStepsY = static_cast<UInt32>((yf - yi) / dy),
			numStepsZ = static_cast<UInt32>((zf - zi) / dz);

	Float	x = xi,
			y = yi,
			z = zi;

	/*if (solver.GetDependentVariables().IsEmpty())
		return;

	GCAS::CNode*	varInd1 = solver.GetIndependentVariables()[0];
	GCAS::CNode*	varInd2 = solver.GetIndependentVariables()[1];
	GCAS::CNode*	varDep = solver.GetDependentVariables()[0];*/

	Vec3 pos, post;
	UInt32 vIndex = 0;

	//CMathParsingUnit::TreeNodePtr pTemp = solver.GetCurrentExpression();

	/*if (pOverride)
		solver.SetExpression(pOverride);*/

	//solver.SolveFor(varDep);

	for (UInt32 StepZ = 0; StepZ <= numStepsZ; StepZ++, z += dz)
	{
		//static_cast<GCAS::FVal*>(varInd2)->SetValue(z);

		y = yi;

		for (UInt32 StepY = 0; StepY <= numStepsY; StepY++, y += dy)
		{
			//static_cast<GCAS::FVal*>(varInd2)->SetValue(y);

			x = xi;

			for (UInt32 StepX = 0; StepX <= numStepsX; StepX++, x += dx)
			{
				//static_cast<GCAS::FVal*>(varInd1)->SetValue(x);

				//y = solver.EvaluateFor(varDep);
				
				//y = varDep->GetValue();
				vIndex = (	StepZ * numStepsX * numStepsY + 
							StepY * numStepsX +
							StepX) * 2;
				
				pos = Vec3(x, y, z);

				m_arrVerts[vIndex].position = pos;
				m_arrVerts[vIndex].color = params.bgColor;

				///////////////////////////////////////////
				post = Vec3(-fabsf(x), -fabsf(y), -fabsf(z));
				///////////////////////////////////////////

				if (params.bNormalize)
					post.Normalize();

				post *= params.fMultiplier;

				pos += post;

				m_arrVerts[vIndex + 1].position = pos;
				m_arrVerts[vIndex + 1].color = params.color;
			}
		}
	}

	/*if (pTemp)
		solver.SetExpression(pTemp);*/

	DrawCurrentLineList();

#endif
}

//////////////////////////////////////////////////////////////////////////

void CVectorGraphics::GraphVectorField3DPara(CMathParsingUnit& exp, const GraphParams2DPara& params)
{
#if 0
	Float	xi = params.xmin,
			xf = params.xmax,
			yi = params.ymin,
			yf = params.ymax,
			zi = params.zmin,
			zf = params.zmax,
			dx = params.xstep,
			dy = params.ystep,
			dz = params.zstep;

	UInt32	numStepsX = static_cast<UInt32>((xf - xi) / dx),
			numStepsY = static_cast<UInt32>((yf - yi) / dy),
			numStepsZ = static_cast<UInt32>((zf - zi) / dz);

	Float	x = xi,
			y = yi,
			z = zi;

	/*if (solver.GetDependentVariables().IsEmpty())
		return;

	GCAS::CNode*	varInd1 = solver.GetIndependentVariables()[0];
	GCAS::CNode*	varInd2 = solver.GetIndependentVariables()[1];
	GCAS::CNode*	varDep = solver.GetDependentVariables()[0];*/

	Vec3 pos, post;
	UInt32 vIndex = 0;

	//CMathParsingUnit::TreeNodePtr pTemp = solver.GetCurrentExpression();

	/*if (pOverride)
		solver.SetExpression(pOverride);*/

	//solver.SolveFor(varDep);

	for (UInt32 StepZ = 0; StepZ <= numStepsZ; StepZ++, z += dz)
	{
		//static_cast<GCAS::FVal*>(varInd2)->SetValue(z);

		y = yi;

		for (UInt32 StepY = 0; StepY <= numStepsY; StepY++, y += dy)
		{
			//static_cast<GCAS::FVal*>(varInd2)->SetValue(y);

			x = xi;

			for (UInt32 StepX = 0; StepX <= numStepsX; StepX++, x += dx)
			{
				//static_cast<GCAS::FVal*>(varInd1)->SetValue(x);

				//y = solver.EvaluateFor(varDep);
				
				//y = varDep->GetValue();
				vIndex = (	StepZ * numStepsX * numStepsY + 
							StepY * numStepsX +
							StepX) * 2;
				
				pos = Vec3(x, y, z);

				m_arrVerts[vIndex].position = pos;
				m_arrVerts[vIndex].color = params.bgColor;

				///////////////////////////////////////////
				post = Vec3(-fabsf(x), -fabsf(y), -fabsf(z));
				///////////////////////////////////////////

				if (params.bNormalize)
					post.Normalize();

				post *= params.fMultiplier;

				pos += post;

				m_arrVerts[vIndex + 1].position = pos;
				m_arrVerts[vIndex + 1].color = params.color;
			}
		}
	}

	/*if (pTemp)
		solver.SetExpression(pTemp);*/

	DrawCurrentLineList();
#endif
}

//////////////////////////////////////////////////////////////////////////

void CVectorGraphics::GraphScalarField3DPara(CMathParsingUnit& exp, const GraphParams2DF& params)
{
#if 0
	Float	xi = params.xmin,
			xf = params.xmax,
			yi = params.ymin,
			yf = params.ymax,
			zi = params.zmin,
			zf = params.zmax,
			dx = params.xstep,
			dy = params.ystep,
			dz = params.zstep;

	UInt32	numStepsX = static_cast<UInt32>((xf - xi) / dx),
			numStepsY = static_cast<UInt32>((yf - yi) / dy),
			numStepsZ = static_cast<UInt32>((zf - zi) / dz);

	Float	x = xi,
			y = yi,
			z = zi;

	/*if (solver.GetDependentVariables().IsEmpty())
		return;

	GCAS::CNode*	varInd1 = solver.GetIndependentVariables()[0];
	GCAS::CNode*	varInd2 = solver.GetIndependentVariables()[1];
	GCAS::CNode*	varDep = solver.GetDependentVariables()[0];*/

	Vec3 pos, post;
	UInt32 vIndex = 0;

	//CMathParsingUnit::TreeNodePtr pTemp = solver.GetCurrentExpression();

	/*if (pOverride)
		solver.SetExpression(pOverride);*/

	//solver.SolveFor(varDep);

	for (UInt32 StepZ = 0; StepZ <= numStepsZ; StepZ++, z += dz)
	{
		//static_cast<GCAS::FVal*>(varInd2)->SetValue(z);

		y = yi;

		for (UInt32 StepY = 0; StepY <= numStepsY; StepY++, y += dy)
		{
			//static_cast<GCAS::FVal*>(varInd2)->SetValue(y);

			x = xi;

			for (UInt32 StepX = 0; StepX <= numStepsX; StepX++, x += dx)
			{
				//static_cast<GCAS::FVal*>(varInd1)->SetValue(x);

				//y = solver.EvaluateFor(varDep);
				
				//y = varDep->GetValue();
				vIndex = (	StepZ * numStepsX * numStepsY + 
							StepY * numStepsX +
							StepX) * 2;
				
				pos = Vec3(x, y, z);

				m_arrVerts[vIndex].position = pos;
				m_arrVerts[vIndex].color = params.bgColor;

				///////////////////////////////////////////
				post = Vec3(-fabsf(x), -fabsf(y), -fabsf(z));
				///////////////////////////////////////////

				if (params.bNormalize)
					post.Normalize();

				post *= params.fMultiplier;

				pos += post;

				m_arrVerts[vIndex + 1].position = pos;
				m_arrVerts[vIndex + 1].color = params.color;
			}
		}
	}

	/*if (pTemp)
		solver.SetExpression(pTemp);*/

	DrawCurrentLineList();
#endif
}

//////////////////////////////////////////////////////////////////////////

void CVectorGraphics::GraphScalarField2DPara(CMathParsingUnit& exp, const GraphParams2DPara& params)
{
#if 0
	Float	xi = params.xmin,
			xf = params.xmax,
			yi = params.ymin,
			yf = params.ymax,
			zi = params.zmin,
			zf = params.zmax,
			dx = params.xstep,
			dy = params.ystep,
			dz = params.zstep;

	UInt32	numStepsX = static_cast<UInt32>((xf - xi) / dx),
			numStepsY = static_cast<UInt32>((yf - yi) / dy),
			numStepsZ = static_cast<UInt32>((zf - zi) / dz);

	Float	x = xi,
			y = yi,
			z = zi;

	/*if (solver.GetDependentVariables().IsEmpty())
		return;

	GCAS::CNode*	varInd1 = solver.GetIndependentVariables()[0];
	GCAS::CNode*	varInd2 = solver.GetIndependentVariables()[1];
	GCAS::CNode*	varDep = solver.GetDependentVariables()[0];*/

	Vec3 pos, post;
	UInt32 vIndex = 0;

	//CMathParsingUnit::TreeNodePtr pTemp = solver.GetCurrentExpression();

	/*if (pOverride)
		solver.SetExpression(pOverride);*/

	//solver.SolveFor(varDep);

	for (UInt32 StepZ = 0; StepZ <= numStepsZ; StepZ++, z += dz)
	{
		//static_cast<GCAS::FVal*>(varInd2)->SetValue(z);

		y = yi;

		for (UInt32 StepY = 0; StepY <= numStepsY; StepY++, y += dy)
		{
			//static_cast<GCAS::FVal*>(varInd2)->SetValue(y);

			x = xi;

			for (UInt32 StepX = 0; StepX <= numStepsX; StepX++, x += dx)
			{
				//static_cast<GCAS::FVal*>(varInd1)->SetValue(x);

				//y = solver.EvaluateFor(varDep);
				
				//y = varDep->GetValue();
				vIndex = (	StepZ * numStepsX * numStepsY + 
							StepY * numStepsX +
							StepX) * 2;
				
				pos = Vec3(x, y, z);

				m_arrVerts[vIndex].position = pos;
				m_arrVerts[vIndex].color = params.bgColor;

				///////////////////////////////////////////
				post = Vec3(-fabsf(x), -fabsf(y), -fabsf(z));
				///////////////////////////////////////////

				if (params.bNormalize)
					post.Normalize();

				post *= params.fMultiplier;

				pos += post;

				m_arrVerts[vIndex + 1].position = pos;
				m_arrVerts[vIndex + 1].color = params.color;
			}
		}
	}

	/*if (pTemp)
		solver.SetExpression(pTemp);*/

	DrawCurrentLineList();
#endif
}

//////////////////////////////////////////////////////////////////////////

void CVectorGraphics::GraphScalarField2D(CMathParsingUnit& exp, const GraphParams2DF& params)
{
#if 0
	Float	xi = params.xmin,
			xf = params.xmax,
			yi = params.ymin,
			yf = params.ymax,
			zi = params.zmin,
			zf = params.zmax,
			dx = params.xstep,
			dy = params.ystep,
			dz = params.zstep;

	UInt32	numStepsX = static_cast<UInt32>((xf - xi) / dx),
			numStepsY = static_cast<UInt32>((yf - yi) / dy),
			numStepsZ = static_cast<UInt32>((zf - zi) / dz);

	Float	x = xi,
			y = yi,
			z = zi;

	/*if (solver.GetDependentVariables().IsEmpty())
		return;

	GCAS::CNode*	varInd1 = solver.GetIndependentVariables()[0];
	GCAS::CNode*	varInd2 = solver.GetIndependentVariables()[1];
	GCAS::CNode*	varDep = solver.GetDependentVariables()[0];*/

	Vec3 pos, post;
	UInt32 vIndex = 0;

	//CMathParsingUnit::TreeNodePtr pTemp = solver.GetCurrentExpression();

	/*if (pOverride)
		solver.SetExpression(pOverride);*/

	//solver.SolveFor(varDep);

	for (UInt32 StepZ = 0; StepZ <= numStepsZ; StepZ++, z += dz)
	{
		//static_cast<GCAS::FVal*>(varInd2)->SetValue(z);

		y = yi;

		for (UInt32 StepY = 0; StepY <= numStepsY; StepY++, y += dy)
		{
			//static_cast<GCAS::FVal*>(varInd2)->SetValue(y);

			x = xi;

			for (UInt32 StepX = 0; StepX <= numStepsX; StepX++, x += dx)
			{
				//static_cast<GCAS::FVal*>(varInd1)->SetValue(x);

				//y = solver.EvaluateFor(varDep);
				
				//y = varDep->GetValue();
				vIndex = (	StepZ * numStepsX * numStepsY + 
							StepY * numStepsX +
							StepX) * 2;
				
				pos = Vec3(x, y, z);

				m_arrVerts[vIndex].position = pos;
				m_arrVerts[vIndex].color = params.bgColor;

				///////////////////////////////////////////
				post = Vec3(-fabsf(x), -fabsf(y), -fabsf(z));
				///////////////////////////////////////////

				if (params.bNormalize)
					post.Normalize();

				post *= params.fMultiplier;

				pos += post;

				m_arrVerts[vIndex + 1].position = pos;
				m_arrVerts[vIndex + 1].color = params.color;
			}
		}
	}

	/*if (pTemp)
		solver.SetExpression(pTemp);*/

	DrawCurrentLineList();
#endif
}

//////////////////////////////////////////////////////////////////////////

void CVectorGraphics::GraphScalarField3D(CMathParsingUnit& exp, const GraphParams3DF& params)
{
#if 0
	Float	xi = params.xmin,
			xf = params.xmax,
			yi = params.ymin,
			yf = params.ymax,
			zi = params.zmin,
			zf = params.zmax,
			dx = params.xstep,
			dy = params.ystep,
			dz = params.zstep;

	UInt32	numStepsX = static_cast<UInt32>((xf - xi) / dx),
			numStepsY = static_cast<UInt32>((yf - yi) / dy),
			numStepsZ = static_cast<UInt32>((zf - zi) / dz);

	Float	x = xi,
			y = yi,
			z = zi;

	/*if (solver.GetDependentVariables().IsEmpty())
		return;

	GCAS::CNode*	varInd1 = solver.GetIndependentVariables()[0];
	GCAS::CNode*	varInd2 = solver.GetIndependentVariables()[1];
	GCAS::CNode*	varDep = solver.GetDependentVariables()[0];*/

	Vec3 pos, post;
	UInt32 vIndex = 0;

	//CMathParsingUnit::TreeNodePtr pTemp = solver.GetCurrentExpression();

	/*if (pOverride)
		solver.SetExpression(pOverride);*/

	//solver.SolveFor(varDep);

	for (UInt32 StepZ = 0; StepZ <= numStepsZ; StepZ++, z += dz)
	{
		//static_cast<GCAS::FVal*>(varInd2)->SetValue(z);

		y = yi;

		for (UInt32 StepY = 0; StepY <= numStepsY; StepY++, y += dy)
		{
			//static_cast<GCAS::FVal*>(varInd2)->SetValue(y);

			x = xi;

			for (UInt32 StepX = 0; StepX <= numStepsX; StepX++, x += dx)
			{
				//static_cast<GCAS::FVal*>(varInd1)->SetValue(x);

				//y = solver.EvaluateFor(varDep);
				
				//y = varDep->GetValue();
				vIndex = (	StepZ * numStepsX * numStepsY + 
							StepY * numStepsX +
							StepX) * 2;
				
				pos = Vec3(x, y, z);

				m_arrVerts[vIndex].position = pos;
				m_arrVerts[vIndex].color = params.bgColor;

				///////////////////////////////////////////
				post = Vec3(-fabsf(x), -fabsf(y), -fabsf(z));
				///////////////////////////////////////////

				if (params.bNormalize)
					post.Normalize();

				post *= params.fMultiplier;

				pos += post;

				m_arrVerts[vIndex + 1].position = pos;
				m_arrVerts[vIndex + 1].color = params.color;
			}
		}
	}

	/*if (pTemp)
		solver.SetExpression(pTemp);*/

	DrawCurrentLineList();
#endif
}