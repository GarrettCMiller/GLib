/////////////////////////////////////////////////////////////////////////////
//		File:			Queue.h
//
//		Programmmer:	Garrett Miller (GM)
//
//		Date Created:	2/21/2006 12:49:32 AM
//
//		Description:	A FIFO object	
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "LListDbl.h"

//////////////////////////////////////////////////////////////////////////

template <typename _Type>
class Queue : public DblLinkedList<_Type>
{
public:
	Queue()
	{
	}

	virtual ~Queue()
	{
	}

	void Enqueue(const _Type &v)
	{
		Add(v);
	}

	void Dequeue(_Type &v)
	{
		ASSERT(head);
		v = head->item;
		Remove(v);
	}
};