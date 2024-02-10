///////////////////////////////////////////////////////////////////////////
//		File:				2dDynamicArray.h
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		A templated contiguous memory 2D array
//////////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////////////////////////////////////////////

#include "DynamicArray.h"

#include "vec.h"

//////////////////////////////////////////////////////////////////////////

template <class T>
class C2dDynamicArray :	public CDynamicArray<T>
{
	UInt32	m_Width;
	UInt32	m_Height;

public:
	C2dDynamicArray() : CDynamicArray<T>(0), m_Width(0), m_Height(0) 
	{	}

	C2dDynamicArray(UInt32 width, UInt32 height) : CDynamicArray<T>()
	{
		Resize(width, height, false);
	}

	UInt32 Width() const		{ return m_Width; }
	UInt32 Height() const		{ return m_Height; }

	void Resize(UInt32 width, UInt32 height, Bool bKeepData = false)
	{
		ASSERT(width >= 1);
		ASSERT(height >= 1);

		CDynamicArray<T>::Resize(width * height, !bKeepData, bKeepData);

		Width(width);
		Height(height);
	}

	void Resize(const Vec2u& size)
	{
		Resize(size.X, size.Y);
	}

	//
	//Two dimenional array access
	//
	T&	operator () (UInt32 x, UInt32 y)
	{
		ASSERT(x < Width());
		ASSERT(y < Height());
		return (*this)[y * Width() + x];
	}

	T& operator () (const Vec2u& pos)
	{
		ASSERT(pos.X < Width());
		ASSERT(pos.Y < Height());
		return (*this)[pos.Y * Width() + pos.X];
	}

	const T&	operator () (UInt32 x, UInt32 y) const
	{
		ASSERT(x < Width());
		ASSERT(y < Height());
		return (*this)[y * Width() + x];
	}

	const T& operator () (const Vec2u& pos) const
	{
		ASSERT(pos.X < Width());
		ASSERT(pos.Y < Height());
		return (*this)[pos.Y * Width() + pos.X];
	}

protected:
	void Width(UInt32 val)	{ m_Width = val; }
	void Height(UInt32 val)	{ m_Height = val; }
};
