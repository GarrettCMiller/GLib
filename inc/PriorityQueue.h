/////////////////////////////////////////////////////////////////////////////
//		File:			Queue.h
//
//		Programmmer:	Garrett Miller (GM)
//
//		Date Created:	2/21/2006 12:49:32 AM
//
//		Description:	A priority FIFO object	
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Queue.h"

#include "Functor.h"

//////////////////////////////////////////////////////////////////////////

template <typename _Type>
class PriorityQueue : public Queue<_Type>
{
public:

	typedef COpenFunctor2<Bool, const _Type&>	TCompFunction;

public:
	PriorityQueue()
	{
	}

	virtual ~PriorityQueue()
	{
	}

	void EnqueuePriority(const _Type &v)
	{
		Int32 result = 0;
		Node* insertAfter = NULL;

		DblLinkedList<_Type>::Iterator it(*this);

		for (it.Begin(); !it.End(); ++it)
		{
			if (m_CompFunc(v, it.Current()))
				insertAfter = it.Current();
			else
				break;
		}

		if (insertAfter)
			AddAfter(v, insertAfter->prev);
		else
			Add(v);
	}

	Bool HasFunctor() const
	{
		return m_CompFunc;
	}

	void SetFunctor(TCompFunction& compFunc)
	{
		m_CompFunc = compFunc;
	}

protected:
	TCompFunction	m_CompFunc;
};