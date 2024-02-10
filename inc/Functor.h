///////////////////////////////////////////////////////////////////////////
//		File:				Functor.h
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		Different types of functors
//////////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////////////////////////////////////////////

//Base from which all are derived
//Used to store all functor types as single type
class CBaseFunctor
{
};

//////////////////////////////////////////////////////////////////////////
//	A basic functor that just wraps up a single 
//	function that takes a single parameter
//////////////////////////////////////////////////////////////////////////
template<typename returnType = Int32, typename argumentType = Int32>
class COpenFunctor
{
protected:
	typedef returnType (*pfnFunc)(argumentType arg1);

	pfnFunc		m_pfnFunction;

public:
	COpenFunctor() : m_pfnFunction(NULL)
	{	}

	explicit COpenFunctor(pfnFunc func) : m_pfnFunction(func)
	{	ASSERT(func);	}

	explicit COpenFunctor(const COpenFunctor<returnType, argumentType>& rhs)
	{
		m_pfnFunction = rhs.m_pfnFunction;
	}

	COpenFunctor<returnType, argumentType>&
		operator = (const COpenFunctor<returnType, argumentType>& rhs)
	{
		m_pfnFunction = rhs.m_pfnFunction;
		return *this;
	}

	returnType operator() (argumentType arg1)
	{
		ASSERT(m_pfnFunction);
		return m_pfnFunction(arg1);
	}

	operator Bool () const
	{
		return (m_pfnFunction != NULL);
	}
};

//////////////////////////////////////////////////////////////////////////

template<typename returnType = Int32, 
			typename argumentType1 = Int32, 
			typename argumentType2 = argumentType1>
class COpenFunctor2
{
protected:
	typedef returnType (*pfnFunc)(argumentType1 arg1, argumentType2 arg2);

	pfnFunc		m_pfnFunction;

public:
	COpenFunctor2() : m_pfnFunction(NULL)
	{	}

	COpenFunctor2(pfnFunc func) : m_pfnFunction(func)
	{	ASSERT(func);	}

	COpenFunctor2(const COpenFunctor2<returnType, argumentType1, argumentType2>& rhs)
	{
		m_pfnFunction = rhs.m_pfnFunction;
	}

	COpenFunctor2<returnType, argumentType1, argumentType2>
		operator = (const COpenFunctor2<returnType, argumentType1, argumentType2>& rhs)
	{
		m_pfnFunction = rhs.m_pfnFunction;
		return *this;
	}

	returnType operator() (argumentType1 arg1, argumentType2 arg2)
	{
		ASSERT(m_pfnFunction);
		return m_pfnFunction(arg1, arg2);
	}

	operator Bool ()
	{
		return (m_pfnFunction != NULL);
	}
};

//////////////////////////////////////////////////////////////////////////

template<	typename returnType = Int32, 
			typename argumentType1 = Int32, 
			typename argumentType2 = argumentType1, 
			typename argumentType3 = argumentType2>
class COpenFunctor3
{
protected:
	typedef returnType (*pfnFunc)(argumentType1 arg1, argumentType2 arg2, argumentType3 arg3);

	pfnFunc		m_pfnFunction;

public:
	COpenFunctor3() : m_pfnFunction(NULL)
	{	}

	COpenFunctor3(pfnFunc func) : m_pfnFunction(func)
	{	ASSERT(func);	}

	COpenFunctor3(const COpenFunctor3<returnType, argumentType1, argumentType2, argumentType3>& rhs)
	{
		m_pfnFunction = rhs.m_pfnFunction;
	}

	COpenFunctor3<returnType, argumentType1, argumentType2, argumentType3>
		operator = (const COpenFunctor3<returnType, argumentType1, argumentType2, argumentType3>& rhs)
	{
		m_pfnFunction = rhs.m_pfnFunction;
		return *this;
	}

	returnType operator() (argumentType1 arg1, argumentType2 arg2, argumentType3 arg3)
	{
		ASSERT(m_pfnFunction);
		return m_pfnFunction(arg1, arg2, arg3);
	}

	operator Bool ()
	{
		return (m_pfnFunction != NULL);
	}
};

//////////////////////////////////////////////////////////////////////////
// A functor that wraps up a function inside a class to be used as a 
// thread function
template<class T>
class CThreadFunctor : public CBaseFunctor
{
protected:
	typedef DWORD (T::*pfnFunc)(LPVOID param);

	pfnFunc		m_pfnFunction;

public:
	CThreadFunctor() : m_pfnFunction(NULL)
	{	}

	CThreadFunctor(pfnFunc func) : m_pfnFunction(func)
	{	ASSERT(func);	}

	operator LPTHREAD_START_ROUTINE() const
	{
		return static_cast<LPTHREAD_START_ROUTINE>(this);
	}

	DWORD operator() (LPVOID param)
	{
		ASSERT(m_pfnFunction);
		return m_pfnFunction(param);
	}

	operator Bool () const
	{
		return (m_pfnFunction != NULL);
	}
};

//////////////////////////////////////////////////////////////////////////
// A functor for a static member function of a class, no arguments
template<class T, typename returnType = Int32>
class CStaticFunctor : public CBaseFunctor
{
protected:
	typedef returnType (T::*pfnFunc)();

	pfnFunc		m_pfnFunction;

public:
	CStaticFunctor() : m_pfnFunction(NULL)
	{	}

	CStaticFunctor(pfnFunc func) : m_pfnFunction(func)
	{	ASSERT(func);	}

	DWORD operator() ()
	{
		ASSERT(m_pfnFunction);
		return m_pfnFunction();
	}

	operator Bool ()
	{
		return (m_pfnFunction != NULL);
	}
};

//////////////////////////////////////////////////////////////////////////
// A thread functor for normal non-class/non-struct functions
class CStaticThreadFunctor : public CBaseFunctor
{
protected:
	typedef DWORD (*pfnFunc)(LPVOID param);

	LPTHREAD_START_ROUTINE		m_pfnFunction;

public:
	CStaticThreadFunctor() : m_pfnFunction(NULL)
	{	}

	CStaticThreadFunctor(pfnFunc func)
	{	
		ASSERT(func);
		m_pfnFunction = LPTHREAD_START_ROUTINE(func);
	}

	operator LPTHREAD_START_ROUTINE() const
	{
		return m_pfnFunction;
	}

	DWORD operator() (LPVOID param)
	{
		ASSERT(m_pfnFunction);
		return m_pfnFunction(param);
	}

	operator Bool () const
	{
		return (m_pfnFunction != NULL);
	}
};

//////////////////////////////////////////////////////////////////////////
// A functor for a member function of a class, no arguments
template<class T, typename returnType = Int32>
class CFunctor : public CBaseFunctor
{
protected:
	T*			m_pObject;

	typedef returnType (T::*pfnFunc)();

	pfnFunc		m_pfnFunction;

public:
	CFunctor(void) : m_pfnFunction(NULL), m_pObject(NULL)
	{	}

	CFunctor(T* obj, pfnFunc func) : m_pfnFunction(func), m_pObject(obj)
	{	ASSERT(obj);	ASSERT(func);	}

	returnType operator () ()
	{
		ASSERT(m_pfnFunction);
		return (m_pObject->*m_pfnFunction)();
	}

	operator Bool () const
	{
		return (m_pfnFunction != NULL);
	}
};

//////////////////////////////////////////////////////////////////////////
// A functor for a member function of a class, 1 argument
template<class T, typename returnType = void, typename argumentType = Int32>
class CFunctor1 : public CBaseFunctor
{
protected:
	T*			m_pObject;

	typedef returnType (T::*pfnFunc)(argumentType arg1);

	pfnFunc		m_pfnFunction;

public:
	CFunctor1(void) : m_pfnFunction(NULL), m_pObject(NULL)
	{	}

	CFunctor1(T* obj, pfnFunc func) : m_pfnFunction(func), m_pObject(obj)
	{	ASSERT(obj);	ASSERT(func);	}

	returnType operator () (argumentType arg1)
	{
		ASSERT(m_pfnFunction);
		return (m_pObject->*m_pfnFunction)(arg1);
	}

	operator Bool () const
	{
		return (m_pfnFunction != NULL);
	}
};

//////////////////////////////////////////////////////////////////////////
// A functor for a static member function of a class, 1 argument
template<class T, typename returnType = Int32, typename argumentType = Int32>
class CStaticFunctor1 : public CBaseFunctor
{
protected:
	typedef returnType (T::*pfnFunc)(argumentType arg1);

	pfnFunc		m_pfnFunction;

public:
	CStaticFunctor1() : m_pfnFunction(NULL)
	{	}

	CStaticFunctor1(pfnFunc func) : m_pfnFunction(func)
	{	ASSERT(func);	}

	DWORD operator() (argumentType arg1)
	{
		ASSERT(m_pfnFunction);
		return m_pfnFunction(arg1);
	}

	operator Bool () const
	{
		return (m_pfnFunction != NULL);
	}
};

//////////////////////////////////////////////////////////////////////////
// A functor for a member function of a class, 2 arguments
template<class T, typename returnType = Int32, 
					typename argumentType1 = Int32, 
					typename argumentType2 = argumentType1>
class CFunctor2 : public CBaseFunctor
{
protected:
	T*			m_pObject;

	typedef returnType (T::*pfnFunc)(argumentType1 arg1, argumentType2 arg2);

	pfnFunc		m_pfnFunction;

public:
	CFunctor2(void) : m_pfnFunction(NULL), m_pObject(NULL)
	{	}

	CFunctor2(T* obj, pfnFunc func) : m_pfnFunction(func), m_pObject(obj)
	{	ASSERT(obj);	ASSERT(func);	}

	returnType operator () (argumentType1 arg1, argumentType2 arg2)
	{
		ASSERT(m_pfnFunction);
		return (m_pObject->m_pfnFunction)(arg1, arg2);
	}

	operator Bool () const
	{
		return (m_pfnFunction != NULL);
	}
};

//////////////////////////////////////////////////////////////////////////
// A functor for a static member function of a class, 2 arguments
template<class T, typename returnType = Int32, 
					typename argumentType1 = Int32, 
					typename argumentType2 = argumentType1>
class CStaticFunctor2 : public CBaseFunctor
{
protected:
	typedef returnType (T::*pfnFunc)(argumentType1 arg1, argumentType2 arg2);

	pfnFunc		m_pfnFunction;

public:
	CStaticFunctor2() : m_pfnFunction(NULL)
	{	}

	CStaticFunctor2(pfnFunc func) : m_pfnFunction(func)
	{	ASSERT(func);	}

	DWORD operator() (argumentType1 arg1, argumentType2 arg2)
	{
		ASSERT(m_pfnFunction);
		return m_pfnFunction(arg1, arg2);
	}

	operator Bool () const
	{
		return (m_pfnFunction != NULL);
	}
};

//////////////////////////////////////////////////////////////////////////
// A functor for a member function of a class, 3 arguments
template<class T, typename returnType = Int32, 
					typename argumentType1 = Int32, 
					typename argumentType2 = argumentType1,
					typename argumentType3 = argumentType2>
class CFunctor3 : public CBaseFunctor
{
protected:
	T*			m_pObject;

	typedef returnType (T::*pfnFunc)(argumentType1 arg1, argumentType2 arg2, argumentType3 arg3);

	pfnFunc		m_pfnFunction;

public:
	CFunctor3(void) : m_pfnFunction(NULL), m_pObject(NULL)
	{	}

	CFunctor3(T* obj, pfnFunc func) : m_pfnFunction(func), m_pObject(obj)
	{	ASSERT(obj);	ASSERT(func);	}

	returnType operator () (argumentType1 arg1, argumentType2 arg2, argumentType3 arg3)
	{
		ASSERT(m_pfnFunction);
		return (m_pObject->m_pfnFunction)(arg1, arg2, arg3);
	}

	operator Bool () const
	{
		return (m_pfnFunction != NULL);
	}
};

//////////////////////////////////////////////////////////////////////////
// A functor for a static member function of a class, 3 arguments
template<class T, typename returnType = Int32, 
					typename argumentType1 = Int32, 
					typename argumentType2 = argumentType1,
					typename argumentType3 = argumentType2>
class CStaticFunctor3 : public CBaseFunctor
{
protected:
	typedef returnType (T::*pfnFunc)(argumentType1 arg1, argumentType2 arg2, argumentType3 arg3);

	pfnFunc		m_pfnFunction;

public:
	CStaticFunctor3() : m_pfnFunction(NULL)
	{	}

	CStaticFunctor3(pfnFunc func) : m_pfnFunction(func)
	{	ASSERT(func);	}

	DWORD operator() (argumentType1 arg1, argumentType2 arg2, argumentType3 arg3)
	{
		ASSERT(m_pfnFunction);
		return m_pfnFunction(arg1, arg2, arg3);
	}

	operator Bool () const
	{
		return (m_pfnFunction != NULL);
	}
};

//////////////////////////////////////////////////////////////////////////

