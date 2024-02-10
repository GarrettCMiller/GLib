///////////////////////////////////////////////////////////////////////////
//		File:				ObjectFactory.h
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		An objcet factory
//////////////////////////////////////////////////////////////////////////
#pragma once

#include <list>
#include "LListDbl.h"

//////////////////////////////////////////////////////////////////////////

template <class _Type, class _TypePtr = _Type*>
class ObjectFactory
{
public:
	typedef std::pair<_TypePtr, Bool>	Object;
	typedef DblLinkedList<Object>		ObjectList;

private:
	ObjectList		m_List;

public:
	ObjectFactory(void)
	{
	}

	virtual ~ObjectFactory(void)
	{
		ObjectList::Iterator i(m_List);

		for (i.Begin(); !i.End(); ++i)
		{
			delete i.Current().first;
		}

		m_List.Clear();
	}

	_TypePtr Create()
	{
		if (m_List.Size() > 0)
		{
			ObjectList::Iterator i(m_List);
			for (i.Begin(); !i.End(); ++i)
			{
				Object& o = i.Current();

				if (o.second == false)
				{
					o.second = true;
					return o.first;
				}
			}
		}

		Object o;

		o.first		= new _Type;
		o.second	= true;

		m_List.Add(o);

		return o.first;
	}

	void Release(_TypePtr object)
	{
		ASSERT(m_List.Size() > 0);
		
		ObjectList::Iterator i(m_List);
		
		for (i.Begin(); !i.End(); ++i)
		{
			Object& o = i.Current();

			if (o.first == object)
			{
				ASSERT(o.second);
				o.second = false;
				return;
			}
		}
	}
};
