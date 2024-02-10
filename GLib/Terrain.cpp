///////////////////////////////////////////////////////////////////////////
//		File:				Terrain.cpp
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		Terrain is basically a deformable heightmap
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Terrain.h"

//#include "Globals.h"

#include "EffectMgr.h"

#include "SpriteMgr.h"

#include "LightMgr.h"

#include <complex>

//////////////////////////////////////////////////////////////////////////

#define NUM_COLORS	11

Float TexSmooth = 0.0f; //0.00002f;

//////////////////////////////////////////////////////////////////////////

Terrain::Terrain() : 
		m_VertexBuffer(NULL),
		m_IndexBuffer(NULL),
		m_fScale(DEF_SCALE),
		m_Width(0),
		m_Depth(0),
		m_fTileSize(0),
		m_NumTextures(0),
		m_NumVerts(0),
		m_NumTiles(0),
		m_ShaderID(0),
		m_bPhysics(false)
{

}

//////////////////////////////////////////////////////////////////////////

Terrain::~Terrain()
{
	//FreeMemory();
}

//////////////////////////////////////////////////////////////////////////

Bool Terrain::AllocateMemory()
{
	if (FAILS(g_D3D->GetDevice()->CreateVertexBuffer(
										m_NumVerts * sizeof(CVertex), 
										D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
										CVertex::FVF, D3DPOOL_DEFAULT,
										&m_VertexBuffer, NULL)))
	{
		g_Log.LOG_ERROR_DX_DESC(g_hr, "Failed to create terrain vertices!\n");
		return false;
	}

	if (FAILS(g_D3D->GetDevice()->CreateIndexBuffer(
										m_NumIndices * sizeof(UInt32),
										D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
										D3DFMT_INDEX32, D3DPOOL_DEFAULT,
										&m_IndexBuffer, NULL)))
	{
		g_Log.LOG_ERROR_DX_DESC(g_hr, "Failed to create terrain indices!\n");
		return false;
	}

	if (m_bPhysics)
	{
		m_Phys = new PhysicsRigidStaticPlane;
		m_Phys->Init();
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

void Terrain::SetShaderID(Int32 sID)
{
	m_ShaderID = sID;
}

//////////////////////////////////////////////////////////////////////////

void Terrain::SetShaderTech(const String &tech)
{
	m_ShaderTech = tech;
}

//////////////////////////////////////////////////////////////////////////

Bool Terrain::FreeMemory()
{
	if (m_bPhysics)
	{
		m_Phys->Shutdown();
		delete m_Phys;
	}

	SAFE_RELEASE(m_VertexBuffer);
	SAFE_RELEASE(m_IndexBuffer);

	SAFE_RELEASE(m_MainTexture.surface);
	SAFE_RELEASE(m_MainTexture.texture);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

Bool Terrain::Init(UInt32 width, UInt32 depth, Float tilesize, Bool bPhysics /* = false */)
{
	HRESULT hr;

	m_Width			= width;
	m_Depth			= depth;
	m_fTileSize		= tilesize;
	m_bPhysics		= bPhysics;
	
	m_NumVerts		= (m_Width + 1) * (m_Depth + 1);
	m_NumTiles		= m_Width * m_Depth;
	m_NumIndices	= m_NumTiles * 6;
	m_NumPolys		= m_NumTiles * 2;

	if (!AllocateMemory())
		return false;

	if (FAILED(hr = m_VertexBuffer->Lock(0, m_NumVerts * sizeof(CVertex), (void **) &m_Vertices, 0)))
	{
		g_Log.LOG_ERROR_DX_DESC(hr, "Failed to lock terrain vertex buffer!\n");
		return false;
	}

	SetVertices();
	//SetNormals();
	SetTexcoords();
	SetColors();

	m_VertexBuffer->Unlock();
	
	if (FAILED(hr = m_IndexBuffer->Lock(0, m_NumIndices * sizeof(UInt32), (void **)&m_Indices, 0)))
	{
		g_Log.LOG_ERROR_DX_DESC(hr, "Failed to lock terrain index buffer!\n");
		return false;
	}

	SetIndices();

	m_IndexBuffer->Unlock();

	//if (1)
	//{
	//	m_ShaderID		= g_EffectMgr->GetEffect("Map.fx").ID;
	//	m_ShaderTech	= "Map";//DEFAULT_PER_PIXEL_LIGHTING_SHADER_TECH;
	//}

	return true;
}

//////////////////////////////////////////////////////////////////////////

Bool Terrain::Shutdown()
{
	return FreeMemory();
}

//////////////////////////////////////////////////////////////////////////

// Vec2 GetWaveVector(Vec2 r, Float dt)
// {
// 	return Vec2()
// }
void Terrain::SetVertices()
{
	UInt32 i = 0;
	UInt32 iz = 0, ix = 0;
	Float x = 0.0f, z = 0.0f;
	
	Float baseX = 0.0f;//500.0f;
	Float baseY = 0.0f;//150.0f;

	Float y = 0;

	Float ts = m_fTileSize;

	Float	A,
			a,
			b,
			c,
			xo,
			zo;

	A = 1.0f;
	
	a = 1;
	b = 0;
	c = 1;

	static Timer elapsed(true);

	Float dt = elapsed.Get();
	Float t = fmodf(dt, 20.0f);

	Float hue = 0.0f;
	Float val = 0.0f;

	Color color(1.0f, 1.0f, 1.0f, 1.0f);

	for (z = ((Float)m_Depth / -2.0f) * ts;			iz < m_Depth + 1;	z += ts, iz++)
	{
		ix = 0;
		for (x = ((Float)m_Width / -2.0f) * ts;		ix < m_Width + 1;	x += ts, ix++)
		{
			if (m_pfnInputFunction)
				y = m_pfnInputFunction(x, z, t);
			else
				y = 0.0f;

// 			hue = fmodf(fabsf(y) * (360.0f) + 225, 360.0f);
// 			val = abs(y) / 1.5f;
// 
// 			color.ConvertFromHSV(hue, 1.0f, val);
// 
// 			if (val <= 0.001)
// 				color.SetAlpha(0.0f);
// 			else
// 				color.SetAlpha(val);

			i = iz * (m_Width + 1) + ix;
			
			m_Vertices[i].position = Vec3(x + baseX, y, z + baseY);
			m_Vertices[i].color = color;
			m_Vertices[i].normal = k_v3YAxis;
			m_Vertices[i].tu	= 10.0f * iz / Float(m_Width);
			m_Vertices[i].tv	= 10.0f * ix / Float(m_Depth);
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void Terrain::Set2DVertices()
{
	UInt32 i = 0;
	UInt32 ix = 0, iy = 0;
	Float x = 0.0f, y = 0.0f;
	
	Float ts = m_fTileSize;

	for (y = ((Float)m_Depth / -2.0f) * ts;			iy < m_Depth + 1;	y += ts, iy++)
	{
		ix = 0;
		for (x = ((Float)m_Width / -2.0f) * ts;		ix < m_Width + 1;	x += ts, ix++)
		{
			i = iy * (m_Width + 1) + ix;
			
			m_Vertices[i].position = Vec3(x, y, 0.0f);
			m_Vertices[i].color = Color(1.0f, 1.0f, 1.0f, 1.0f);
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void Terrain::SetTexcoords( )
{
	UInt32 i = 0;

	Float Scale =	//0.015889f * 2.0f;
					10.0f;
					//1.0f;

	for (UInt32 row = 0; row < m_Depth + 1; row++)
	{
		for (UInt32 col = 0; col < m_Width + 1; col++)
		{
			i					= (row	* (m_Width + 1)) + (col);
			m_Vertices[i].tu	= Scale * col / Float(m_Width);
			m_Vertices[i].tv	= Scale * row / Float(m_Depth);
		}
	}


}

//////////////////////////////////////////////////////////////////////////

void Terrain::SetColors( )
{
	for (UInt32 v = 0, av = 0; v < m_NumVerts; v++, av += 4)
		m_Vertices[v].color	= 0xFF111111;//Color(1.0f, 1.0f, 1.0f, 1.0f);//D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
}

//////////////////////////////////////////////////////////////////////////

void Terrain::SetIndices( )
{
	UInt32 i = 0, tx, tz;

	for (tz = 0; tz < m_Depth; tz++)
	{
		for (tx = 0; tx < m_Width; tx++)
		{
			i					= (tz * m_Width * 6) + (tx * 6);
			m_Indices[i + 0]	= tz * (m_Width + 1) + tx;
			m_Indices[i + 1]	= tz * (m_Width + 1) + (tx + 1);
			m_Indices[i + 2]	= (tz + 1) * (m_Width + 1) + tx;

			m_Indices[i + 3]	= (tz + 1) * (m_Width + 1) + tx;
			m_Indices[i + 4]	= tz * (m_Width + 1) + (tx + 1);
			m_Indices[i + 5]	= (tz + 1) * (m_Width + 1) + (tx + 1);
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void Terrain::SetTextureID(Int32 texindex, UInt32 TexID)
{
	m_Texture[texindex] = TexID;
	m_MainTexture.texture = g_SpriteMgr->GetTexture(TexID);
}

//////////////////////////////////////////////////////////////////////////

void Terrain::RecalculateNormals()
{
#if 0
	UInt32		n, ni;
	Float		norm[3];
	Float		p[3][3];

	memset(norm, 0, sizeof(Float) * 3);
	norm[1] = 1.0f;

	for (n = 0, ni = 0; n < NumTiles; n++, ni+= 3)
	{
		memcpy(p[0], &Vertices[ni], sizeof(Float) * 3);
		memcpy(p[1], &Vertices[ni+ 3], sizeof(Float) * 3);
		memcpy(p[2], &Vertices[ni + (3 * Width)], sizeof(Float) * 3);

		GetFaceNormal(&norm, p[0], p[1], p[2]);

		memcpy(&Normals[ni], norm, sizeof(Float) * 3);
	}
#endif
}

//////////////////////////////////////////////////////////////////////////

Bool Terrain::PreRender()
{
	static Bool bStreamsSet = false;

	//if (!bStreamsSet)
	{
		if (FAILED(g_hr = g_D3D->GetDevice()->SetStreamSource(0, m_VertexBuffer, 0, sizeof(CVertex))))
		{
			g_Log.LOG_ERROR_DX_DESC(g_hr, "Failed to set stream source!\n");
			return false;
		}

		if (FAILED(g_hr = g_D3D->GetDevice()->SetIndices(m_IndexBuffer)))
		{
			g_Log.LOG_ERROR_DX_DESC(g_hr, "Failed to set indices!\n");
			return false;
		}
	}

	g_D3D->GetDevice()->SetTexture(0, NULL);

	//g_D3D->GetDevice()->SetRenderState

	return true;
}

//////////////////////////////////////////////////////////////////////////

Bool Terrain::RenderTerrain()
{
	static HRESULT hr;

	D3DMaterial m;

	ZeroMemory(&m, sizeof(m));
	m.Ambient = Color::km_Gray;
	m.Diffuse = Color::km_White;
	g_D3D->GetDevice()->SetMaterial(&m);

	if (FAILED(hr = g_D3D->GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_NumVerts, 0, m_NumPolys)))
	{
		g_Log.LOG_ERROR_DX_DESC(hr, "Failed to render terrain!\n");
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

Bool Terrain::Render( )
{
	static Timer tTime(true);

	CMatrix mat;
	g_D3D->SetWorldMatrix(mat);

	if (!PreRender())
		return false;

 	if (g_D3D->AllowShaders() && g_EffectMgr->Size() && m_ShaderID >= 0 && m_ShaderTech != "")
 	{
 		Effect& effect = g_EffectMgr->GetEffect(m_ShaderID);
		//g_D3D->GetDevice()->SetVertexShader(&effect.Shader);

 		UInt32 NumPasses = 0;

 		effect.Shader->SetTechnique(m_ShaderTech.c_str());
 		effect.Shader->Begin(&NumPasses, 0);

		effect.Shader->SetFloat("g_Time", tTime.Get());
		effect.Shader->SetTexture("pTexture", m_MainTexture.texture);

		D3DMatrix effectMatrix = g_D3D->matView * g_D3D->matProj;
		effect.Shader->SetMatrix("matWorldViewProj", &effectMatrix);

		//effect.Shader->SetFloat("fPx", g_Player->m_vPosition / 

		g_LightMgr->SetupLightsInShader(effect, k_v3Zero);
 
 		for (UInt32 renderPass = 0; renderPass < NumPasses; renderPass++)
 		{
 			if (FAILED(g_hr = effect.Shader->BeginPass(renderPass)))
 			{
 				//sprintf_s(g_ErrorBuffer, "Failed to begin render pass %d while rendering terrain!\n", renderPass);
 				g_Log.LOG_ERROR_DX_DESC(g_hr, g_ErrorBuffer);
 				return false;
 			}
 
 			if (!RenderTerrain())
 				return false;
 
 			if (FAILED(g_hr = effect.Shader->EndPass()))
 			{
 				//sprintf_s(g_ErrorBuffer, "Failed to end render pass %d while rendering terrain!\n", renderPass);
 				g_Log.LOG_ERROR_DX_DESC(g_hr, g_ErrorBuffer);
 				return false;
 			}
 		}
 
 		effect.Shader->End();

		//g_D3D->GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
 	}
 	else
		return RenderTerrain();

	return true;
}

//////////////////////////////////////////////////////////////////////////

Bool Terrain::LoadHeightMap(String path, Float scale)
{
#if 0
	BYTE *pBits;

	Bool NewMapSize = FALSE;

	Int32 width, height;
	UInt32 cvert = 0, v, c;

	Float y = 0.0f;

	Int32 blue, green, red, grey;
	
	pBits = gltReadBMPBits(path, &width, &height);

	if (!pBits)
	{
		MessageBox(NULL, "Find the right file!", path, MB_OK);
		return FALSE;
	}

	Scale = scale;

	if (width != Width || height != Depth)
	{
		FreeMemory();

		Width	= width;
		Depth	= height;
		NewMapSize = TRUE;

		if (!AllocateMemory())
			return FALSE;
	}

	
////////////////////////
	for (v = 0, c = 0; v < NumVerts && v < (Width * Depth) ; v++, c += 3)
	{
		blue		= pBits[c];
		green	= pBits[c + 1];
		red		= pBits[c + 2];

		grey		= (red + green + blue) / 3;

		y = (grey * Scale);
		
		Vertices[v * 3 + 1]	= y;
	}

	//delete [] pBits;
	free(pBits);

	if (NewMapSize)
	{
		SetVertices();
		SetTexcoords();
		SetColors();
		SetIndices();
	}
#endif

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

void Terrain::CopyFrom(const Terrain& obj)
{
	m_VertexBuffer = obj.m_VertexBuffer;
	m_IndexBuffer = obj.m_IndexBuffer;
	m_Vertices = obj.m_Vertices;
	m_Indices = obj.m_Indices;
	m_fScale = obj.m_fScale;
	m_fTileSize = obj.m_fTileSize;
	m_Width = obj.m_Width;
	m_Depth = obj.m_Depth;
	m_NumVerts = obj.m_NumVerts;
	m_NumIndices = obj.m_NumIndices;
	m_NumPolys = obj.m_NumPolys;
	m_NumTiles = obj.m_NumTiles;
	m_NumTextures = obj.m_NumTextures;
	memcpy(m_Texture, obj.m_Texture, sizeof(UInt32) * NUM_TEXTURES);
	m_ShaderID = obj.m_ShaderID;
	m_ShaderTech = obj.m_ShaderTech;
	m_MainTexture= obj.m_MainTexture;
}

//////////////////////////////////////////////////////////////////////////

void Terrain::SetInputFunction(const T3DFunction& pfnInput)
{
	m_pfnInputFunction = pfnInput;
}

///////////////////////////////////////////////////////////////////////////////

void Terrain::CreatePlasmaFractal( )
{
	/*Float		*cell;
	Int32			i, x, y,
					MaxIter = 4;

	cell = malloc(NumVerts * sizeof(Float));

    for (i = 0; i < MaxIter; i++)
	{
		for (y = 0; y < 

	}*/
}

//////////////////////////////////////////////////////////////////////////
