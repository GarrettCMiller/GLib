///////////////////////////////////////////////////////////////////////////
//		File:				Stack.h
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		Templated singly-linked linked list stack with 
//							an iterator (from LList<>)
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "LListDbl.h"

//////////////////////////////////////////////////////////////////////////

template <typename _Type, Bool _bAddToHead = true>
class Stack : public DblLinkedList<_Type>
{
public:

	Stack(void)
	{
	}

	virtual ~Stack(void)
	{
	}

	void Push(const _Type &v)
	{
		Add(v, _bAddToHead);
	}

	_Type Pop()
	{
		ASSERT(size > 0);

		Iterator i(*this);

		if (_bAddToHead)
			i.Begin();
		else
			i.Tail();

		_Type val = i.Current();
		
		Remove(i.Self());

		return val;
	}
};