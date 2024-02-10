/////////////////////////////////////////////////////////////////////////////
//		File:			Singleton.h
//
//		Programmmer:	Garrett Miller (GM)
//
//		Date Created:	2/21/2006 12:52:32 AM
//
//		Description:	A templated singleton class
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include <typeinfo.h>

//#include "DataTypes.h"

#define CLASS_NAME(_className)		(typeid(_className).name())

//////////////////////////////////////////////////////////////////////////

template <class T>
class Singleton
{
protected:
	Singleton() { }
	Singleton(const Singleton &single);
	Singleton &operator = (const Singleton &single);

	static int	NumCreated;
	static	T*		m_pInstance;

public:
	virtual			~Singleton(void);

	static bool		Exists();
	static T*		GetInstance();
	static void		DeleteInstance();

	//statics
// public:
// 	static const UInt32 mk_MaxSingletons = 20;
// 
// 	static void CollectGarbage();
// 
// protected:
// 	static Singleton* mk_AllSingletons[mk_MaxSingletons];
};

//////////////////////////////////////////////////////////////////////////

template <class T>
int Singleton<T>::NumCreated = 0;

//////////////////////////////////////////////////////////////////////////

template <class T>
Singleton<T>::~Singleton()
{
}

//////////////////////////////////////////////////////////////////////////

template <class T>
bool Singleton<T>::Exists()
{
	return (m_pInstance != NULL);
}

//////////////////////////////////////////////////////////////////////////

template <class T>
T* Singleton<T>::GetInstance()
{
	if (!Exists() /*&& NumCreated == 0*/)
	{
		m_pInstance = new T;

		ASSERT(m_pInstance);

		///*mk_AllSingletons[*/NumCreated++/*] = m_pInstance*/;

		//g_Log << "New " << CLASS_NAME(T) << " instance created! (size: " << sizeof(T) << ")\n";
	}

	return m_pInstance;
}

//////////////////////////////////////////////////////////////////////////

template <class T>
void Singleton<T>::DeleteInstance()
{
	if (!Exists())
		return;

	delete m_pInstance;

	m_pInstance = NULL;

	//g_Log << CLASS_NAME(T) << " instance deleted!\n";
}

//////////////////////////////////////////////////////////////////////////

// template <class T>
// void Singleton<T>::CollectGarbage()
// {
// 	for (UInt32 i = 0; i < NumCreated; i++)
// 		mk_AllSingletons[i]->DeleteInstance();
// 
// 	NumCreated = 0;
// }

//////////////////////////////////////////////////////////////////////////

#define IMPLEMENT_SINGLETON( _className )  _className* Singleton< _className >::m_pInstance = NULL;