///////////////////////////////////////////////////////////////////////////////
//		File:				GameMacros.h
//
//		Programmer:			Garrett Miller (GM)
//
//		Date created:		August 11, 2004
//
//		Description:		Implementation of various commonly used game macros
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////
//MACROS
///////////////////////////////////////////////////////////
//	These read the keyboard asynchronously
#define KEYDOWN(vk_code)		((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEYUP(vk_code)			((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)
	
#define ALPHA(a)				((a << 24) | 0xFFFFFF)

#define SAFE_RELEASE(obj)		if (obj) { obj->Release(); obj = NULL; }
#define SAFE_RELEASE_NORM(obj)	if (obj) { obj->Release(); /*obj = NULL;*/ }
//#define OBJECT_RELEASE
#define SAFE_DELETE(obj)		if (obj) { delete obj;	obj = NULL; }
#define SAFE_DELETE_ARRAY(obj)	if (obj) { delete [] obj;	obj = NULL; }

#define RGB32(a,r,g,b)			(b + (g << 8) + (r << 16) + (a << 24))

#define FAILS(call)				(FAILED(g_hr = call))

//#ifndef LOGGER
//	#define GetDxErr(hr)		{ MessageBox(g_hWnd, DXGetErrorDescription9(hr), DXGetErrorString9(hr), MB_OK); /*return false;*/ }
//#else
	#define GetDxErr(hr)		g_Log << "\tError:\t" << DXGetErrorString(hr) << "\n\tDesc:\t" << DXGetErrorDescription(hr) << '\n';
//#endif			

#define DxErr					GetDxErr(g_hr);

#define VALID_PTR(ptr)			(ptr && !BAD_PTR(ptr))

#define BAD_PTR(ptr)			((	/*reinterpret_cast<UInt32>(ptr) == NULL ||*/ \
									reinterpret_cast<UInt32>(ptr) == 0xCDCDCDCD || \
									reinterpret_cast<UInt32>(ptr) == 0xCCCCCCCC || \
									reinterpret_cast<UInt32>(ptr) == 0xFeeeFeee || \
									reinterpret_cast<UInt32>(ptr) == 0xBaadF00d) \
									? true : false)

// Creates a variable inside a class which allows the class 
// itself to be cast to the member type in a smart way
// (Do a search through out the code base for examples)
#define AUTO_CLASS_CAST(type, name)	type name;	operator type () { return name; }