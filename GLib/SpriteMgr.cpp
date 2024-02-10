///////////////////////////////////////////////////////////////////////////////
//		File:				SpriteMgr.cpp
//
//		Programmer:			Garrett Miller (GM)
//
//		Date created:		August 11, 2004
//
//		Description:		Implementation of a sprite manager class
///////////////////////////////////////////////////////////////////////////////
//#ifndef _STDAFX_INCLUDED_
	#include "stdafx.h"
	#pragma message("stdafx.h has been included in " __FILE__)
//#endif

#include "spritemgr.h"
#include "d3dwrapper.h"
#include "Vec.h"

#ifdef _DOOMEROIDS_
	#include "LightMgr.h"
	#include "Game.h"
#endif

#include "EffectMgr.h"
#include "MemMgr.h"

#include "ProfileSample.h"

#include "dpakfile.h"

#include "Globals.h"

#include "UIObject.h"

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_SINGLETON(SpriteMgr);

//////////////////////////////////////////////////////////////////////////

static UInt32			numdrawn = 0;
static String		k_ImagesBasePath = "";
const D3DXHANDLE	k_DefaultShaderTech = "PerPixelLighting";

//////////////////////////////////////////////////////////////////////////

SpriteMgr::SpriteMgr()
{
	m_nNextID		= 0;
	//m_Sprites.Resize(350, false);
	m_ImagesBasePath = k_ImagesBasePath;
}

//////////////////////////////////////////////////////////////////////////

SpriteMgr::~SpriteMgr(void)
{
	g_Log << "SpriteMgr::~SpriteMgr() begin...\n";

	g_Log << "Releasing sprite memory...\n";
	m_Sprites.FreeReleasePointers();
	g_Log << "Sprite memory successfully freed!\n";

	g_Log << "Clearing draw list...\n";
	m_DrawList.Clear();
	g_Log << "Draw list cleared!\n";
	g_Log << "Clearing render list...\n";
	m_RenderList.Clear();
	g_Log << "Render list cleared!\n";
	g_Log << "Clearing UI object list...\n";
	m_UIObjectList.Clear();
	g_Log << "UI object class cleared!\n";

	g_Log << "SpriteMgr::~SpriteMgr() done!\n";
}

//////////////////////////////////////////////////////////////////////////

Bool SpriteMgr::Contains(const String& texture)
{
	FOR_EACH(i, m_Sprites)
		if (texture == m_Sprites[i]->texname)
			return true;

	return false;
}

//////////////////////////////////////////////////////////////////////////

UInt32 SpriteMgr::Add(Sprite* s)
{
	ASSERT(s);

	m_Sprites.Push(s);

	return m_nNextID++;
}

//////////////////////////////////////////////////////////////////////////

LPDIRECT3DTEXTURE9	SpriteMgr::GetTexture(UInt32 id)
{
	return m_Sprites[id]->texture;
}

//////////////////////////////////////////////////////////////////////////

LPDIRECT3DTEXTURE9 SpriteMgr::GetTexture(const String& texture)
{
	FOR_EACH(i, m_Sprites)
		if (texture == m_Sprites[i]->texname)
			return m_Sprites[i]->texture;

	//Shouldn't be here
	ASSERT(false);

	return NULL;
}

//////////////////////////////////////////////////////////////////////////

Sprite& SpriteMgr::GetSprite(const String& texture)
{
	FOR_EACH(i, m_Sprites)
		if (texture == m_Sprites[i]->texname)
			return *(m_Sprites[i]);

	//Shouldn't be here
	ASSERT(false);

	return *(m_Sprites[0]);
}

//////////////////////////////////////////////////////////////////////////

Sprite& SpriteMgr::GetSprite(UInt32 id)
{
	//PROFILE_THIS;
	return *(m_Sprites[id]);
}

//////////////////////////////////////////////////////////////////////////

UInt32 SpriteMgr::GetID(const String& texture)
{
	FOR_EACH(i, m_Sprites)
		if (texture == m_Sprites[i]->texname)
			return m_Sprites[i]->ID;

	ASSERT(false);
	return static_cast<UInt32>(-1);
}

//////////////////////////////////////////////////////////////////////////

UInt32 SpriteMgr::LoadSprite(const String& path, Bool bLoadFromPak /* = false */)
{
	if (Contains(path))
		return GetID(path);

	g_ResLog << "Loading texture ";

	if (bLoadFromPak)
		return LoadSpriteFromPakFile(path);
	else
		return LoadSpriteFromFile(path);
}

//////////////////////////////////////////////////////////////////////////

UInt32 SpriteMgr::LoadSpriteFromPakFile(const String& path)
{
	g_ResLog << "from pak file...\n";

	if (LIST_OR_TOTAL == LIST)
		g_Log << "Loading texture " << path << '\n';

	g_ResLog << "Loading texture " << path << '\n';

	LPDIRECT3DTEXTURE9	temptex;
	D3DXIMAGE_INFO		info;
	
	memset(&info, 0, sizeof(D3DXIMAGE_INFO));

	UInt32 color = D3DCOLOR_XRGB(0, 255, 255);

	DPakFile file;
	
	g_PakFile.GetFileByName(file, path.c_str());

	ASSERT(file.IsValid());

	if (FAILS(D3DXCreateTextureFromFileInMemoryEx(g_D3D->GetDevice(), 
												file.FileData, file.FileSize,
												D3DX_DEFAULT,
												D3DX_DEFAULT,
												1,
												0,
												D3DFMT_UNKNOWN,
												D3DPOOL_MANAGED,
												D3DX_DEFAULT,
												D3DX_DEFAULT,
												color,
												&info, 
												NULL,
												&temptex)))
	{
		//g_Log.LOG_ERROR_DX_DESC(hr, "Couldn't load texture!\t" + path + "\n");
		g_ResLog.LOG_ERROR_DX_DESC(g_hr, "Couldn't load texture!\t" + path + "\n");
		DxErr;

		return UINT_MAX;	//Indicate error with a -1
	}

	Sprite* sprite = new Sprite;

	ASSERT(reinterpret_cast<UInt32>(temptex) != 0xCDCDCDCD);

	sprite->texture			= temptex;
	sprite->ID				= m_nNextID;
	sprite->width			= info.Width;
	sprite->height			= info.Height;
	strncpy_s(sprite->texname, path.c_str(), sizeof(sprite->texname));

	g_ResLog << "Texture successfully loaded from pak file!\n";

	return Add(sprite);
}

//////////////////////////////////////////////////////////////////////////

UInt32 SpriteMgr::LoadSpriteFromFile(const String& path)
{
	HRESULT hr;

	g_Log << "from file...\n";

	if (LIST_OR_TOTAL == LIST)
		g_Log << "Loading texture " << path << '\n';

	g_ResLog << "Loading texture " << path << '\n';

	LPDIRECT3DTEXTURE9	temptex;
	D3DXIMAGE_INFO		info;

	memset(&info, 0, sizeof(D3DXIMAGE_INFO));

	UInt32 color =		D3DCOLOR_XRGB(0, 255, 255);

	char ImgPath[256];

	if (!m_ImagesBasePath.empty())
	{
		strncpy_s(ImgPath, m_ImagesBasePath.c_str(), 256);
		strncat_s(ImgPath, path.c_str(), 256);
	}
	else
	{
		strncpy_s(ImgPath, path.c_str(), 256);
	}

	if (FAILED(hr = D3DXCreateTextureFromFileEx(g_D3D->GetDevice(), 
		ImgPath, 
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		1,
		0,
		//D3DFMT_UNKNOWN,
		D3DFMT_X8R8G8B8,
		D3DPOOL_MANAGED,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		color,
		&info, 
		NULL,
		&temptex)))
	{
		g_Log.LOG_ERROR_DX_DESC(hr, "Couldn't load texture!\t" + path + "\n");
		g_ResLog.LOG_ERROR_DX_DESC(hr, "Couldn't load texture!\t" + path + "\n");

		return UINT_MAX;	//Indicate error with a -1
	}

	Sprite* sprite = new Sprite;

	//ZeroMemory(&sprite, sizeof(Sprite));

	sprite->texture			= temptex;
	sprite->ID				= m_nNextID;
	sprite->width			= info.Width;
	sprite->height			= info.Height;
	strncpy_s(sprite->texname, path.c_str(), sizeof(sprite->texname));

	g_Log << "Texture successfully loaded from file!\n";

	return Add(sprite);
}

//////////////////////////////////////////////////////////////////////////

void SpriteMgr::DrawSpriteEffect(UInt32 id, 
								 UInt32 shader, D3DXHANDLE tech, 
								 Float x, Float y, Float z, 
								 Float angle, Float scale, UInt32 mask)
{
	DRAW_ITEM di;

	ASSERT(id != -1);
	ASSERT(id != 0xCDCDCDCD);	

	di.id			= id;
	di.shaderID		= shader;
	di.shaderTech	= tech;
	di.x			= x;
	di.y			= y;
	di.z			= z;
	di.mask			= mask;
	di.rot			= angle;
	di.scale		= scale;

	//DblLinkedList<DRAW_ITEM>::Iterator	iter(m_RenderList);
	//DblLinkedList<DRAW_ITEM>::Iterator	iter2(m_RenderList);

	/*for ( UInt32 i = 1; i < m_RenderList.Size(); i++, ++iter, ++iter2 )
	{
		iter.Begin();
		++iter;

		while ( !iter.End() )
		{
			if (iter.Current().shaderID < iter.Prev()->shaderID)
			++iter;
		}
	}*/

	//m_RenderList.AddAfter(di, );
	m_RenderList.Add(di);

}

//////////////////////////////////////////////////////////////////////////

void SpriteMgr::DrawUIElement(UIObject* pObj)
{
	ASSERT(pObj);
	g_SpriteMgr->m_UIObjectList.Add(pObj);
}

//////////////////////////////////////////////////////////////////////////

void SpriteMgr::DrawSprite(UInt32 id, 
						   Float x, Float y, Float z, 
						   Float angle,		Float scale, 
						   UInt32 mask)
{
	DRAW_ITEM di;

	ASSERT(id != -1);
	ASSERT(id != 0xCDCDCDCD);

	di.id		= id;
	di.x		= x;
	di.y		= y;
	di.z		= z;
	di.mask		= mask;
	di.rot		= angle;
	di.scale	= scale;

	m_DrawList.Add(di);
}

//////////////////////////////////////////////////////////////////////////

void SpriteMgr::RenderUISprite(UInt32 id, 
							 Float x /* = 0.0f */, Float y /* = 0.0f */, Float z /* = 0.5f */, 
							 Float angle /* = 0.0f */, Float scale /* = 1.0f */, UInt32 mask /* = 0 */)
{
	DRAW_ITEM di;

	ASSERT(id != -1);
	ASSERT(id != 0xCDCDCDCD);

	di.id		= id;
	di.x		= x;
	di.y		= y;
	di.z		= z;
	di.mask		= mask;
	di.rot		= angle;
	di.scale	= scale;

	m_UISpriteList.Add(di);
}

//////////////////////////////////////////////////////////////////////////

void SpriteMgr::RenderSprite(UInt32 id, 
							 Float x /* = 0.0f */, 
							 Float y /* = 0.0f */, 
							 Float z /* = 0.5f */, 
							 Float angle /* = 0.0f */, 
							 Float scale /* = 1.0f */, 
							 UInt32 mask /* = 0xFFFFFFFF */)
{
	//PROFILE_THIS;

	LPD3DXSPRITE sprt	= g_D3D->GetSpriteObject();
	D3DXMATRIX matrix;
	HRESULT hr;

	Sprite&	sprite	= GetSprite(id);

	UInt32	width	= sprite.width,
			height	= sprite.height;

	Float	cx		= width		/ 2.0f,
			cy		= height	/ 2.0f;

	D3DXVECTOR2		p2(x, y);
	D3DXVECTOR3		p3(x, y, z);
	D3DXVECTOR2		rc2(cx / width, cy / height);
	D3DXVECTOR3		rc3(cx + x, cy + y, 0.0f);

	D3DXMatrixAffineTransformation2D(&matrix, scale, &rc2, angle * Deg2Rad, &p2);
	//sprt->SetTransform(&matrix);

	if (FAILED(hr = sprt->Draw(sprite.texture, NULL, &rc3, &p3, mask)))
	{
		g_Log.LOG_ERROR_DX(hr);
		return;
	}

	D3DXMatrixIdentity(&matrix);
	sprt->SetTransform(&matrix);
}

//////////////////////////////////////////////////////////////////////////

void SpriteMgr::RenderSpriteEffect(UInt32 id, 
								   UInt32 shaderID,
								   D3DXHANDLE tech,
								 Float x /* = 0.0f */, 
								 Float y /* = 0.0f */, 
								 Float z /* = 0.5f */, 
								 Float angle /* = 0.0f */, 
								 Float scale /* = 1.0f */, 
								 UInt32 mask /* = 0xFFFFFFFF */)
{
	//PROFILE_THIS;

	LPD3DXSPRITE sprt	= g_D3D->GetSpriteObject();
	D3DXMATRIX matrix;

	Sprite&	sprite	= GetSprite(id);

	UInt32	width	= sprite.width,
			height	= sprite.height;

	Float	cx		= width		/ 2.0f,
			cy		= height	/ 2.0f;

	Effect&	effect	= g_EffectMgr->GetEffect(shaderID);

	D3DXVECTOR2		p2(x, y);
	D3DXVECTOR3		p3(x, y, z);
	D3DXVECTOR2		rc2(cx / width, cy / height);
	D3DXVECTOR3		rc3(cx + x, cy + y, 0.0f);

	D3DXMatrixAffineTransformation2D(&matrix, scale, &rc2, angle * Deg2Rad, &p2);
	sprt->SetTransform(&matrix);

	UInt32 numPasses = 0;

	effect.Shader->SetTechnique(tech);
	effect.Shader->Begin(&numPasses, 0);

	for (UInt32 renderPass = 0; renderPass < numPasses; renderPass++)
	{
		if (FAILS(effect.Shader->BeginPass(renderPass)))
		{
			DxErr;
			return;
		}

		if (FAILS(sprt->Draw(sprite.texture, NULL, &rc3, &p3, mask)))
		{
			DxErr;
			return;
		}

		if (FAILS(effect.Shader->EndPass()))
		{
			DxErr;
			return;
		}
	}

	effect.Shader->End();

	D3DXMatrixIdentity(&matrix);
	sprt->SetTransform(&matrix);
}

//////////////////////////////////////////////////////////////////////////

void SpriteMgr::RenderUI()
{
	PROFILE_THIS;

	if (m_UIObjectList.Size() == 0)
		return;

	//g_D3D->GetDevice()->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	DblLinkedList<UIObject*>::Iterator	iter(m_UIObjectList);

	//TODO: Re-enable UI rendering
 	for (iter.Begin(); !iter.End(); ++iter)
 		iter.Current()->Render();

	m_UIObjectList.Clear();

	ASSERT(m_UIObjectList.IsEmpty());

	//Make sure to get the sprite UI elements
	HRESULT hr;

	UInt32 count = m_UISpriteList.Size();

	if (count == 0)
		return;

	LPD3DXSPRITE sprt	= g_D3D->GetSpriteObject();
	D3DXMATRIX matrix;
	DRAW_ITEM di;

	DblLinkedList<DRAW_ITEM>::Iterator	i(m_UISpriteList);

	for (i.Begin(); !i.End(); ++i)
	{
		di				= i.Current();

		ASSERT(di.id	!= UINT_MAX);
		ASSERT(di.id	!= 0xCDCDCDCD);

		Sprite&	sprite	= GetSprite(di.id);

		UInt32	width	= sprite.width,
				height	= sprite.height;

		Float	cx		= width		/ 2.0f,
				cy		= height	/ 2.0f;

		D3DXVECTOR2		p2(di.x, di.y);
		D3DXVECTOR3		p3(di.x, di.y, di.z);
		D3DXVECTOR2		rc2(cx / width, cy / height);
		D3DXVECTOR3		rc3(cx + di.x, cy + di.y, 0.0f);

		D3DXMatrixAffineTransformation2D(&matrix, di.scale, &rc2, di.rot * Deg2Rad, &p2);
		sprt->SetTransform(&matrix);

		if (FAILED(hr = sprt->Draw(	sprite.texture,
									NULL,
									&rc3,
									&p3,
									(di.mask ? di.mask : 0xFFFFFFFF))))
		{
			g_Log.LOG_ERROR_DX(hr);
			return;
		}
	}

	D3DXMatrixIdentity(&matrix);
	sprt->SetTransform(&matrix);

	m_UISpriteList.Clear();

	ASSERT(m_UISpriteList.IsEmpty());
}

//////////////////////////////////////////////////////////////////////////

void SpriteMgr::RenderSprites()
{
	PROFILE_THIS;
	DrawList();
	RenderList();
}

//////////////////////////////////////////////////////////////////////////

void SpriteMgr::DrawList()
{
	HRESULT hr;

	UInt32 count = m_DrawList.Size();

	if (count == 0)
		return;

	LPD3DXSPRITE sprt	= g_D3D->GetSpriteObject();
	D3DXMATRIX matrix, temp;
	DRAW_ITEM di;

	D3DXVECTOR2		p2;
	D3DXVECTOR3		p3;
	D3DXVECTOR2		rc2;
	D3DXVECTOR3		rc3;
	D3DXQUATERNION	q;

	UInt32	width = 0,
			height = 0;
	
	Float	cx = 0,
			cy = 0;

	DblLinkedList<DRAW_ITEM>::Iterator	iter(m_DrawList);

	for (iter.Begin(); !iter.End(); ++iter)
	{
		di				= iter.Current();

		ASSERT(di.id	!= UINT_MAX);

		Sprite& sprite	= GetSprite(di.id);

		width			= sprite.width;
		height			= sprite.height;

		cx				= width		/ 2.0f,
		cy				= height	/ 2.0f;

		p2				= D3DXVECTOR2(di.x, di.y);
		p3				= D3DXVECTOR3(di.x, di.y, di.z);
		rc2				= D3DXVECTOR2(cx / width, cy / height);
		rc3				= D3DXVECTOR3(cx + di.x, cy + di.y, di.z);

		//D3DXMatrixAffineTransformation2D(&temp, di.scale * 0.1f, &rc2, di.rot * Deg2Rad, &p2);
		D3DXMatrixRotationX(&matrix, 180.0f * Deg2Rad);

		D3DXQuaternionRotationAxis(&q, &k_v3XAxis, di.rot * Deg2Rad);

		D3DXMatrixAffineTransformation(&temp, di.scale * 0.25f, &rc3, NULL, &p3);

		matrix *= temp;
		
		sprt->SetTransform(&matrix);

		ASSERT(reinterpret_cast<UInt32>(sprite.texture) != 0xCDCDCDCD);

		if (FAILED(hr = sprt->Draw(	sprite.texture,
									NULL,
									&rc3,
									&p3,
									(di.mask ? di.mask : 0xFFFFFFFF))))
		{
			g_Log.LOG_ERROR_DX(hr);;
			return;
		}
	}

	D3DXMatrixIdentity(&matrix);
	sprt->SetTransform(&matrix);

	m_DrawList.Clear();

	ASSERT(m_DrawList.IsEmpty());
}

//////////////////////////////////////////////////////////////////////////

void SpriteMgr::RenderList()
{
	PROFILE_THIS;

	if (m_RenderList.Size() == 0)
		return;

	HRESULT		hr;

	DRAW_ITEM	di;
	Sprite		sprite;

	D3DXHANDLE	tech		= NULL;
	
	Int32			shaderindex	= -1;
	
	UInt32		numPasses	= 0;
	UInt32		width		= 0, 
				height		= 0;
	
#ifdef _DOOMEROIDS_
	Float		fCurrTime	= g_Game->tElapsed.Get();
#else
	Float	fCurrTime		= 0.0f;
#endif

	Float		cx			= 0.0f,
				cy			= 0.0f;

	LPD3DXSPRITE	spriteDevice = g_D3D->GetSpriteObject();
	Effect			effect;
	
	D3DXMATRIX		spriteMatrix, effectMatrix;
	D3DXVECTOR2		p2, rc2;
	D3DXVECTOR3		p3, rc3;

	D3DXVECTOR4		light[4];

	effectMatrix = g_D3D->matView * g_D3D->matProj;

	if (g_D3D->AllowShaders())
 		spriteDevice->Flush();

	DblLinkedList<DRAW_ITEM>::Iterator	iter(m_RenderList);

	for (iter.Begin(); !iter.End(); ++iter)
	{
		di		= iter.Current();
		sprite	= GetSprite(di.id);

		width	= sprite.width;
		height	= sprite.height;

		cx		= width		/ 2.0f;
		cy		= height	/ 2.0f;

		p2		= D3DXVECTOR2(di.x, di.y);
		p3		= D3DXVECTOR3(di.x, di.y, di.z);
		rc2		= D3DXVECTOR2(cx / width, cy / height);
		rc3		= D3DXVECTOR3(cx + di.x, cy + di.y, di.z);

		D3DXMatrixAffineTransformation2D(&spriteMatrix, di.scale, &rc2, di.rot * Deg2Rad, &p2);
		spriteDevice->SetTransform(&spriteMatrix);

		if (g_D3D->AllowShaders())
		{
			if (di.shaderID != shaderindex)
			{
				shaderindex		= di.shaderID;
				effect			= g_EffectMgr->GetEffect(shaderindex);

				effect.Shader->SetMatrix("matWorldViewProj", &effectMatrix);
				effect.Shader->SetFloat("time", fCurrTime);
				//effect.Shader->SetVector("vLightVec[0]", &playerPos);
			}

			if (di.shaderTech != tech)
			{
				tech = di.shaderTech;

				if (FAILED(hr = effect.Shader->SetTechnique((tech ? tech : k_DefaultShaderTech))))
				{
					g_Log.LOG_ERROR_DX(hr);
					return;
				}
			}

			//g_LightMgr->SetupLightsInShader(effect, Rect(di.x, di.y, di.x + width, di.y + height));

			effect.Shader->Begin(&numPasses, 0);
			{
				ASSERT(numPasses == 1);

				for (UInt32 i = 0; i < numPasses; i++) 
				{
					if (FAILED(hr = spriteDevice->Draw(sprite.texture, NULL, &rc3, &p3, di.mask)))
					{
						g_Log.LOG_ERROR_DX(hr);;
						ASSERT(false);
						return;
					}

					hr = effect.Shader->BeginPass(i);
					{
						if (FAILED(hr))
						{
							g_Log.LOG_ERROR_DX(hr);;
							ASSERT(false);
							return;
						}
						else
						{
							spriteDevice->Flush();
						}
					}
					effect.Shader->EndPass();
				}
			}
			effect.Shader->End();
		}
		else
		{
			if (FAILED(hr = spriteDevice->Draw(sprite.texture, NULL, &rc3, &p3, di.mask)))
			{
				g_Log.LOG_ERROR_DX(hr);;
				ASSERT(false);
				return;
			}
		}
	}

	D3DXMatrixIdentity(&spriteMatrix);
	spriteDevice->SetTransform(&spriteMatrix);

	m_RenderList.Clear();

	ASSERT(m_RenderList.IsEmpty());

	//spriteDevice->Flush();
}

//////////////////////////////////////////////////////////////////////////

void SpriteMgr::Clear()
{
	m_UIObjectList.Clear();
	m_UISpriteList.Clear();
	m_DrawList.Clear();
	m_RenderList.Clear();
}

//////////////////////////////////////////////////////////////////////////