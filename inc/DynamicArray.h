///////////////////////////////////////////////////////////////////////////
//		File:				DynamicArray.h
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		A contiguous memory (and range-safe) array
//////////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////////////////////////////////////////////

#include "GameMacros.h"

#include "DataTypes.h"
#include "MiscMath.h"

//////////////////////////////////////////////////////////////////////////

class CBaseDynamicArray
{
	//	Empty class to derive all DynamicArray's 
	//	from so that we can keep pointers to all

	//CBaseDynamicArray(String )
};

//////////////////////////////////////////////////////////////////////////

template <class T>
class CDynamicArray //: CBaseDynamicArray
{
protected:
	UInt32	m_NumData;		//How many objects have been pushed into the array
	UInt32	m_DataCount;	//How many memory slots are reserved
	T*		m_Buffer;		//The actual data

	UInt32	m_nSizeOf;		//The size of the individual slots

	static const UInt32 ms_ResizeAmount = 5;	//Default amount to resize the allocation by

public:
	static const UInt32 ms_None = static_cast<UInt32>(-1);	//Indicates that no items have been pushed into the array

	explicit CDynamicArray(UInt32 size = 0) : 
		m_Buffer(NULL), m_DataCount(0), m_NumData(ms_None)
	{
		m_nSizeOf = sizeof(T);

		if (size > 0)
			SetSize(size);
	}

	CDynamicArray(const CDynamicArray<T>& obj) : 
		m_Buffer(NULL), m_DataCount(0), m_NumData(ms_None)
	{
		m_nSizeOf = sizeof(T);

		CopyFrom(obj);
	}

	CDynamicArray<T>& operator = (const CDynamicArray<T>& rhs)
	{
		CopyFrom(rhs);
		return *this;
	}

	virtual ~CDynamicArray(void)
	{
		ReleaseBuffer();
	}

	void Clear()
	{
		if (m_Buffer != NULL)
		{
			ZeroMemory(m_Buffer, BufferSize() * m_nSizeOf);
			m_NumData = 0;//ms_None;
		}
	}

	//Helper function, use at your own risk
	void FreePointers(Bool bClearList = true)
	{
		UInt32 count = Size();

		for (UInt32 i = 0; i < count; i++)
			SAFE_DELETE(m_Buffer[i]);
		
		if (bClearList)
			Clear();
	}

	//Helper function, use at your own risk
	void ReleasePointers(Bool bClearList = true, Bool bReleaseNull = true)
	{
		for (UInt32 i = 0; i < Size(); i++)
		{
			if (bReleaseNull)
			{	SAFE_RELEASE(m_Buffer[i]);			}
			else
			{	SAFE_RELEASE_NORM(m_Buffer[i]);		}
		}
		
		if (bClearList)
			Clear();
	}

	void FreeReleasePointers()
	{
		ReleasePointers(false, false);
		FreePointers();
	}

	//
	//Change the size
	//
	virtual void Resize(UInt32 newSize, Bool bZeroArray = false, Bool bKeepData = false)
	{
		CDynamicArray<T> temp;

		if (bKeepData)
			temp.CopyFrom(*this);

		ReleaseBuffer();
		
		SetSize(newSize, bZeroArray);

		if (bKeepData)
			CopyFrom(temp, false);
	}

	void SetToEmpty()
	{
		m_NumData = 0;
	}

	//
	//The memory footprint
	//
	UInt32 MemSize(Bool bIncludeSelf = false) const
	{
		return m_DataCount * m_nSizeOf + (bIncludeSelf ? sizeof(this) : 0);
	}

	//
	//The number of items in the array
	//
	UInt32 Size() const
	{
		return (m_NumData != ms_None ? m_NumData : m_DataCount);
	}

	UInt32 BufferSize() const
	{
		return m_DataCount;
	}

	UInt32 NumItems() const
	{
		return m_NumData;
	}

	//
	//One dimensional array access
	//
	inline T&	operator [] (UInt32 index)
	{
		ASSERT(m_Buffer);
		ASSERT(index < m_DataCount);
		return m_Buffer[index];
	}

	inline const T&	operator [] (UInt32 index) const
	{
		ASSERT(m_Buffer);
		ASSERT(index < m_DataCount);
		return m_Buffer[index];
	}

	Bool CopyFrom(const CDynamicArray<T>& rhs, Bool bResizeToFit = true)
	{
		if (rhs.m_DataCount > 0)
		{
			ASSERT(rhs.m_Buffer);
			
			if ((rhs.m_DataCount < m_DataCount && bResizeToFit) || rhs.m_DataCount > m_DataCount)
				SetSize(rhs.m_DataCount);

			UInt32 buffSize = Min(BufferSize(), rhs.BufferSize());
			
			for (UInt32 i = 0; i < buffSize; i++)
				m_Buffer[i] = rhs[i];

			m_NumData = rhs.m_NumData;

			return true;
		}

		return false;
	}

	void ReleaseBuffer()
	{
		SAFE_DELETE_ARRAY(m_Buffer);
		m_DataCount = 0;
		m_NumData = ms_None;
	}

	void SetSize(UInt32 size, Bool bZeroArray = false)
	{
		ASSERT(size);

		if (m_Buffer)
			ReleaseBuffer();

		m_NumData	= ms_None;
		m_DataCount	= size;
		m_Buffer	= new T [size];

		ASSERT(m_Buffer);

		if (bZeroArray)
			Clear();
	}

	void Push(const T& item)
	{
		if (m_Buffer == NULL)
			Resize(ms_ResizeAmount);
		else if (m_NumData == m_DataCount)
			Resize(BufferSize() + ms_ResizeAmount, false, true);

		if (m_NumData == ms_None)
			m_NumData = 0;

		ASSERT(m_NumData < BufferSize());

		m_Buffer[m_NumData] = item;

		m_NumData++;
	}

	void Push(T& item)
	{
		if (m_Buffer == NULL)
			Resize(ms_ResizeAmount);
		else if (m_NumData == m_DataCount)
			Resize(BufferSize() + ms_ResizeAmount, false, true);

		if (m_NumData == ms_None)
			m_NumData = 0;

		ASSERT(m_NumData < BufferSize());

		m_Buffer[m_NumData] = item;

		m_NumData++;
	}

	void PushFront(T& item)
	{
		if (m_Buffer == NULL)
			Resize(ms_ResizeAmount);
		else if (m_NumData == m_DataCount)
			Resize(BufferSize() + ms_ResizeAmount, false, true);

		if (m_NumData == ms_None)
			m_NumData = 0;

		ASSERT(m_NumData < BufferSize());

		memmove(m_Buffer + 1, m_Buffer, Size() * m_nSizeOf);

		m_Buffer[0] = item;

		m_NumData++;
	}

	void PushFront(const T& item)
	{
		if (m_Buffer == NULL)
			Resize(ms_ResizeAmount);
		else if (m_NumData == m_DataCount)
			Resize(BufferSize() + ms_ResizeAmount, false, true);

		if (m_NumData == ms_None)
			m_NumData = 0;

		ASSERT(m_NumData < BufferSize());

		memmove(m_Buffer + 1, m_Buffer, Size() * m_nSizeOf);

		m_Buffer[0] = item;

		m_NumData++;
	}

	Bool Pop(T& item)
	{
		if (m_NumData == ms_None)
			return false;

		ASSERT(m_NumData > 0);

		m_NumData--;
		item = m_Buffer[m_NumData];

		return true;
	}

	Bool PopFront(T& item)
	{
		if (m_NumData == ms_None)
			return false;

		ASSERT(m_NumData > 0);

		m_NumData--;
		item = m_Buffer[0];

		memmove(m_Buffer, m_Buffer + 1, Size() * m_nSizeOf);

		return true;
	}

	Bool IsEmpty() const
	{
		return (Size() == 0 || m_Buffer == NULL);
	}

	Bool Contains(const T& item, UInt32 startIndex = 0, UInt32* pIndexOfItem = NULL) const
	{
		for (UInt32 i = startIndex; i < Size(); i++)
		{
			if ((*this)[i] == item)
			{
				if (pIndexOfItem)	*pIndexOfItem = i;
				return true;
			}
		}

		if (pIndexOfItem)	*pIndexOfItem = ms_None;

		return false;
	}

	Bool Insert(const T& item, UInt32 insertAfter = 0)
	{
		if (insertAfter >= Size())
			return false;

		if (insertAfter == Size() - 1)
		{
			Push(item);
			return true;
		}
		/*else if (insertAfter == 0)
		{
			PushFront(item);
			return true;
		}*/

		CDynamicArray<T>	temp(BufferSize() + 1);

		UInt32	sizeA = (insertAfter + 1) * m_nSizeOf,
				sizeB = MemSize() - sizeA;

		UInt32	tBufSize = temp.MemSize();

		memcpy_s(temp.m_Buffer, tBufSize, m_Buffer, sizeA);
		temp.m_NumData = insertAfter + 1;
		temp.Push(item);

		tBufSize = temp.MemSize();

		memcpy_s(temp.m_Buffer + 1 + (sizeA / m_nSizeOf), tBufSize, m_Buffer + (sizeA / m_nSizeOf), sizeB);
		temp.m_NumData += (sizeB / m_nSizeOf);

		CopyFrom(temp);

		return false;
	}

	Bool Remove(const T& item, UInt32 startIndex = 0)
	{
		ASSERT(!IsEmpty());

		if (Size() == 1)
			ReleaseBuffer();

		UInt32 index = ms_None;

		if (!Contains(item, startIndex, &index))
			return false;

		memmove_s(	m_Buffer + index,		MemSize() - index * m_nSizeOf, 
					m_Buffer + index + 1,	MemSize() - index * sizeof(T));

		m_NumData--;

		return true;
	}


	void Sort(Bool bDescending = false)
	{
		UInt32 numEntries = Size();

		CDynamicArray<T> temp(numEntries);

		for (UInt32 i = 0; i < numEntries; i++)
		{
			T maxT = (*this)[0];

			for (UInt32 mc = 0; mc < Size(); mc++)
			{
				//if (i == mc) continue;

				T& obj = (*this)[mc];

				maxT = Max(obj, maxT);
			}

			Remove(maxT);

			if (bDescending)
				temp.Push(maxT);
			else
				temp.PushFront(maxT);
		}

		ASSERT(IsEmpty());
		
		CopyFrom(temp);
	}

	template <typename Type, Type* pVal>
	void Sort(Bool bDescending = false)
	{
		UInt32 numEntries = Size();

		CDynamicArray<T> temp(numEntries);

		for (UInt32 i = 0; i < numEntries; i++)
		{
			Type maxT = (Type)*( &((*this)[0]) + pVal);

			for (UInt32 mc = 0; mc < Size(); mc++)
			{
				//if (i == mc) continue;

				Type& obj = (Type)*( &((*this)[mc]) + pVal);

				maxT = Max(obj, maxT);
			}

			Remove(maxT);

			if (bDescending)
				temp.Push(maxT);
			else
				temp.PushFront(maxT);
		}

		ASSERT(IsEmpty());
		
		CopyFrom(temp);
	}

	static void UnitTest()
	{
		CDynamicArray<Int32> test;
		CDynamicArray<Int32> test2;

		ASSERT(test.Size() == 0);
		ASSERT(test.IsEmpty());

		for (Int32 i = 0; i < 20; i++)
			test.Push(i);

		ASSERT(test.Size() == 20);
		ASSERT(test.IsEmpty() == false);

		UInt32 numItems = 0;
		for (Int32 i = 0; i < test.Size(); i++, numItems++)
		{
			ASSERT(test.m_Buffer[i] == i);
		}

		ASSERT(numItems == test.Size());

		for (Int32 i = 0; i < test.Size(); i++)
		{
			ASSERT(test.Contains(i));
		}

		ASSERT(test[4] == 4);
		ASSERT(test[5] == 5);
		
		ASSERT(test.Remove(5));
		
		ASSERT(test[4] == 4);
		ASSERT(test[5] == 6);

		ASSERT(test.Size() == 19);
		ASSERT(test.IsEmpty() == false);

		test.ReleaseBuffer();

		ASSERT(test.Size() == 0);
		ASSERT(test.IsEmpty());

		//Reset list
		for (Int32 i = 0; i < 20; i++)
			test.Push(i);

		ASSERT(test.Remove(33) == false);

		ASSERT(test.IsEmpty() == false);

		UInt32 size = test.Size();
		Int32 temp = -1;
		for (UInt32 i = 0; i < size; i++)
		{
			test.Pop(temp);
			ASSERT(temp == test.Size());
			ASSERT(test.Size() == size - i - 1);
		}

		ASSERT(test.Size() == 0);
		ASSERT(test.IsEmpty());

		test.SetSize(20);

		ASSERT(test.Size() == 20);
		ASSERT(test.IsEmpty() == false);

		test.SetSize(20, true);

		ASSERT(test.Size() == 20);
		ASSERT(test.IsEmpty() == false);

		for (UInt32 i = 0; i < test.Size(); i++)
		{
			ASSERT(test[i] == 0);
		}

		test.ReleaseBuffer();

		ASSERT(test.Size() == 0);
		ASSERT(test.IsEmpty());

		//Reset list
		for (Int32 i = 0; i < 20; i++)
			test2.Push(i);

		test.CopyFrom(test2);

		for (Int32 i = 0; i < test.Size(); i++, numItems++)
		{
			ASSERT(test.m_Buffer[i] == i);
			ASSERT(test.m_Buffer[i] == test2.m_Buffer[i]);
		}

		test.SetSize(10, true);

		test.CopyFrom(test2, false);

		for (Int32 i = 0; i < test.Size(); i++, numItems++)
		{
			ASSERT(test.m_Buffer[i] == i);
			ASSERT(test.m_Buffer[i] == test2.m_Buffer[i]);
		}
		
		test.CopyFrom(test2, true);

		for (Int32 i = 0; i < test.Size(); i++, numItems++)
		{
			ASSERT(test.m_Buffer[i] == i);
			ASSERT(test.m_Buffer[i] == test2.m_Buffer[i]);
		}

		//Reset list
		test.ReleaseBuffer();

		ASSERT(test.Size() == 0);
		ASSERT(test.IsEmpty());

		//Refill list
		for (Int32 i = 0; i < 10; i++)
			test.Push(i);

		for (Int32 i = 10; i < 20; i++)
			test.PushFront(i);

		ASSERT(test[0] == 19);

		test.PopFront(temp);

		ASSERT(test[0] == 18);

		test.ReleaseBuffer();

		ASSERT(test.Size() == 0);
		ASSERT(test.IsEmpty());

		//Refill list
		for (Int32 i = 0; i < 10; i++)
			test.Push(i);

		test.Insert(77, 3);

		ASSERT(test[4] == 77);

		test.Insert(420, 0);

		ASSERT(test[1] == 420);

		ASSERT(test.Insert(69, test.Size()) == false);

		test.Sort(true);

		ASSERT(test[4] == 4);
		ASSERT(test[test.Size()-1] == 420);

		test.ReleaseBuffer();

		ASSERT(test.Size() == 0);
		ASSERT(test.IsEmpty());

		//Refill list
		for (Int32 i = 0; i < 10; i++)
			test.Push(i);
	}
};

typedef CDynamicArray<Float>	TFloatArray;
typedef CDynamicArray<UInt32>	TUIntArray;
typedef CDynamicArray<Int32>	TIntArray;
typedef CDynamicArray<String>	TStringArray;

#define FOR_EACH(index, cDynArr)				for (UInt32 index = 0; index < cDynArr.Size(); index++)
//#define FOR_EACH_IN(index, object, cDynArr)		for (UInt32 index = 0; index < cDynArr.Size(); index++)