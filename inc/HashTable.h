///////////////////////////////////////////////////////////////////////////
//		File:				HashTable.h
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		A templated hash table
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "LListDbl.h"

//////////////////////////////////////////////////////////////////////////

#define DEFAULT_MAX_HASH_BUCKETS	8

//////////////////////////////////////////////////////////////////////////

template <typename _Type>
class CHashTable
{
	typedef DblLinkedList<_Type> TList;
	typedef Int32 (*pfnHashFunc)(_Type& item);

	pfnHashFunc		m_HashFunction;
	
	TList			m_Bucket[DEFAULT_MAX_HASH_BUCKETS];
	UInt32			m_NumBuckets;

public:

	CHashTable(void) : m_NumBuckets(DEFAULT_MAX_HASH_BUCKETS), m_HashFunction(NULL)
	{
	}

	virtual ~CHashTable(void)
	{
		Clear();
	}

	Int32 GetNumBuckets() const	{ return m_NumBuckets;	}

	/*DblLinkedList<_Type>::Iterator GetList(UInt32 bucket)
	{
		ASSERT(bucket < m_NumBuckets);
		DblLinkedList<_Type>::Iterator i(m_Bucket[bucket]);
		return i;
	}*/

	DblLinkedList<_Type>& GetList(UInt32 bucket)
	{
		ASSERT(bucket < m_NumBuckets);
		return m_Bucket[bucket];
	}

	void Clear(Int32 bucket = -1)
	{
		ASSERT(bucket < Int32(m_NumBuckets));

		if (bucket >= 0)
			m_Bucket[bucket].Clear();
		else
			for (UInt32 i = 0; i < m_NumBuckets; i++)
				m_Bucket[i].Clear();
	}

	void SetHashFunction(pfnHashFunc hashFunc)
	{
		m_HashFunction = hashFunc;
	}

	void Add(_Type& item)
	{
		ASSERT(m_HashFunction);

		Int32 bucket = m_HashFunction(item);

		ASSERT(bucket >= 0 && bucket < Int32(m_NumBuckets));

		m_Bucket[bucket].Add(item);
	}
};
