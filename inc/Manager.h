/////////////////////////////////////////////////////////////////////////////
//		File:			Manager.h
//
//		Programmmer:	Garrett Miller (GM)
//
//		Date Created:	2/21/2006 12:51:51 AM
//
//		Description:	Base manager class, unused currently
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "LListDbl.h"

template <class T>
class Manager
{
public:
	DblLinkedList<T>		ManagedList;

	virtual				~Manager();
};