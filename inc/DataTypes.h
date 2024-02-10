#pragma once

#include <string>

//////////////////////////////////////////////////////////////////////////
//
//	Data Type Defines For Easy Cross-Platform Support
//
//	I need to figure out a better way to declare __intN variables
//
//////////////////////////////////////////////////////////////////////////

typedef				__int8			Int8;
typedef unsigned	__int8			UInt8;

typedef				__int16			Int16;
typedef unsigned	__int16			UInt16;

typedef				__int32			Int32;
typedef unsigned	__int32			UInt32;

typedef				__int64			Int64;
typedef unsigned	__int64			UInt64;

typedef				float			Float;
typedef				double			Double;

typedef				bool			Bool;

typedef			std::string			String;

typedef				UInt32*			Pointer;

typedef unsigned	char			Char;
typedef UInt8						Byte;

//////////////////////////////////////////////////////////////////////////
struct D3DXMATRIX;
struct D3DXQUATERNION;
struct D3DXPLANE;
struct _D3DLIGHT9;
struct _D3DMATERIAL9;
struct _D3DXMATERIAL;
class ID3DXMesh;
class IDirect3DTexture9;
class IDirect3DVertexBuffer9;
class IDirect3DIndexBuffer9;
class IDirect3DSurface9;
class ID3DXEffect;
class ID3DXAnimationController;

typedef D3DXMATRIX					D3DMatrix;
typedef D3DXQUATERNION				D3DQuaternion;
typedef D3DXPLANE					D3DPlane;
typedef _D3DLIGHT9					D3DLight;
typedef _D3DMATERIAL9				D3DMaterial;
typedef _D3DXMATERIAL				D3DXMaterial;
typedef ID3DXMesh*					D3DMesh;
typedef IDirect3DTexture9*			D3DTexture;
typedef IDirect3DVertexBuffer9*		D3DVertexBuffer;
typedef IDirect3DIndexBuffer9*		D3DIndexBuffer;
typedef IDirect3DSurface9*			D3DSurface;
typedef ID3DXEffect*				D3DEffect;
typedef ID3DXAnimationController*	D3DAnimController;

//////////////////////////////////////////////////////////////////////////
//
//	END DEFINES
//
//////////////////////////////////////////////////////////////////////////