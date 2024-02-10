//////////////////////////////////////////////////////////////////////////
//
//	D3DImage
//
//	Wraps a D3DSurface and D3DTexture into the same object easily enabling
//	rendering-to-texture and full screen quads. Also implements ATL CImage
//	class to easily enable saving image to file (esp JPEG).
//
//////////////////////////////////////////////////////////////////////////
#pragma once

#include <atlimage.h>

#include "D3DWrapper.h"

#include "Palette.h"
#include "Vertex.h"

//////////////////////////////////////////////////////////////////////////

class CD3DImage
{
public:
	CD3DImage(void);
	virtual ~CD3DImage(void);

	//Releases all resources
	void ReleaseAll();

	//must have
	CD3DImage& operator = (const CD3DImage& rhs);

	//BBuffer size and possible initial texture
	Int32 Init(Vec2u imgSize, const String& initImage = "");

	//Clears bbuffer and resets verts
	Int32 Reset();

	//Setup verts from global full screen quad
	Int32 InitVertices();
	//Setup palette
	void InitPalette(UInt32 size = 0, UInt32 offset = 0);
	
	//Renders texture to surface
	Int32 LoadTexture(const String& path);

	//Fill with color
	void Fill(Color color);

	//////////////////////////////////////////

	//Lock the vert/index buffers
	HRESULT Lock(CVertex** pV, UInt16** pI = NULL);

	//Unlock the vert/index buffers
	void Unlock(bool bVB = true, HRESULT* vb = NULL, 
				bool bIB = true, HRESULT* ib = NULL);

	///////////////////////////////////////////

	//Direct vertex access
	CVertex& operator () (UInt32 index)
	{	
		return m_Vertices[index];	
	}

	//////////////////////////////////////////

	Bool VertexBufferLocked() const
	{	return m_bVBLocked;		}

	Bool IndexBufferLocked() const
	{	return m_bIBLocked;		}

	const Vec2u& ImageSize() const
	{	return m_vImageSize;	}

	Palette&	GetPalette()
	{	return m_Palette;		}

protected:

	Int32 CreateSurface();
	Int32 CreateBuffers();
	Int32 CreateImageObject();

public:

	//Enables the class object to contain members that it can 
	//be automatically cast as to functions
	AUTO_CLASS_CAST(D3DVertexBuffer,	m_pVertexBuffer);	//Enables passing of this class as a vertex buffer
	AUTO_CLASS_CAST(D3DIndexBuffer,		m_pIndexBuffer);	//Enables passing of this class as a index buffer
	AUTO_CLASS_CAST(D3DTexture,			m_pTexture);		//Enables passing of this class as a texture
	AUTO_CLASS_CAST(D3DSurface,			m_pSurface);		//Enables passing of this class as a surface
	
	AUTO_CLASS_CAST(CImage,				m_Image);			//Enables passing of this class as an ATL CImage

	TVertexArray						m_Vertices;			//The static verts I keep track of

protected:
	Palette			m_Palette;

	Bool			m_bVBLocked;
	Bool			m_bIBLocked;

	Vec2u			m_vImageSize;
};