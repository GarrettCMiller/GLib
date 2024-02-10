///////////////////////////////////////////////////////////////////////////
//		File:				Dator.h
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		A wrapper to a variable somewhere
//							I like my new CVariable<> class much better
//////////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////////////////////////////////////////////

class CBaseDator
{
protected:
	CBaseDator(void)
	{	}
	CBaseDator(const String& name) : m_Name(name)
	{	}

	CBaseDator(const CBaseDator& b)	
	{
		(*this) = b;
	}

	String	m_Name;
public:
	virtual void GetValue() const
	{
		ASSERT(false);
	}

	String GetName() const
	{
		return m_Name;
	}

	//virtual CBaseDator&	operator =	(const CBaseDator& rhs) = 0;
	//virtual CBaseDator&	operator ++	(const CBaseDator& rhs) = 0;
	//virtual CBaseDator&	operator --	(const CBaseDator& rhs) = 0;
	//virtual CBaseDator&	operator +=	(const CBaseDator& rhs) = 0;
	//virtual CBaseDator&	operator -=	(const CBaseDator& rhs) = 0;
	//virtual CBaseDator&	operator *=	(const CBaseDator& rhs) = 0;
	//virtual CBaseDator&	operator /=	(const CBaseDator& rhs) = 0;
	//virtual CBaseDator&	operator +	(const CBaseDator& rhs) = 0;
	//virtual CBaseDator&	operator -	(const CBaseDator& rhs) = 0;
	//virtual CBaseDator&	operator *	(const CBaseDator& rhs) = 0;
	//virtual CBaseDator&	operator /	(const CBaseDator& rhs) = 0;

	//virtual Bool			operator ==	(const CBaseDator& rhs) = 0;
	//virtual Bool			operator !=	(const CBaseDator& rhs) = 0;
	//virtual Bool			operator >	(const CBaseDator& rhs) = 0;
	//virtual Bool			operator <	(const CBaseDator& rhs) = 0;
	//virtual Bool			operator >=	(const CBaseDator& rhs) = 0;
	//virtual Bool			operator <=	(const CBaseDator& rhs) = 0;
};

//////////////////////////////////////////////////////////////////////////

template <typename _Type>
class CDator : public CBaseDator
{
protected:
	_Type	m_Variable;

public:
	CDator()
	{	}
	explicit CDator(_Type var, String name = "") : CBaseDator(name), m_Variable(var)
	{	}

	virtual void GetValue(_Type& val) const { val = m_Variable; }

	operator _Type () const { return m_Variable; }

#if 1
	#define converter(_type) \
		operator CDator<_type> () const\
		{	return CDator<_type>(m_Name, static_cast<_type>(m_Variable));	}

		converter(Int8);
		converter(UInt8);
		converter(Int16);
		converter(UInt16);
		converter(Int32);
		converter(UInt32);
		converter(Int64);
		converter(UInt64);
		converter(Float);
		converter(Double);
		converter(Bool);
	#undef converter

#endif

	CDator<_Type>&	operator =	(const CDator<_Type>& rhs)
	{
		m_Variable = rhs.m_Variable;
		return *this;
	}

	CDator<_Type>&	operator =	(const _Type& rhs)
	{
		m_Variable = rhs;
		return *this;
	}

	CDator<_Type>&	operator ++	()
	{
		m_Variable++;
		return *this;
	}

	CDator<_Type>&	operator --	()
	{
		m_Variable--;
		return *this;
	}

	CDator<_Type>&	operator +=	(const CDator<_Type>& rhs)
	{
		m_Variable += rhs.m_Variable;
		return *this;
	}

	CDator<_Type>&	operator +=	(const _Type& rhs)
	{
		m_Variable += rhs;
		return *this;
	}

	CDator<_Type>&	operator -=	(const CDator<_Type>& rhs)
	{
		m_Variable -= rhs.m_Variable;
		return *this;
	}

	CDator<_Type>&	operator -=	(const _Type& rhs)
	{
		m_Variable -= rhs;
		return *this;
	}

	CDator<_Type>&	operator *=	(const CDator<_Type>& rhs)
	{
		m_Variable *= rhs.m_Variable;
		return *this;
	}

	CDator<_Type>&	operator *=	(const _Type& rhs)
	{
		m_Variable *= rhs;
		return *this;
	}

	CDator<_Type>&	operator /=	(const CDator<_Type>& rhs)
	{
		m_Variable /= rhs.m_Variable;
		return *this;
	}

	CDator<_Type>&	operator /=	(const _Type& rhs)
	{
		m_Variable /= rhs;
		return *this;
	}

	CDator<_Type>&	operator +	(const CDator<_Type>& rhs) const
	{
		return CDator<_Type>((m_Variable + rhs.m_Variable));
	}

	CDator<_Type>&	operator -	(const CDator<_Type>& rhs) const
	{
		return CDator<_Type>((m_Variable - rhs.m_Variable));
	}

	CDator<_Type>&	operator *	(const CDator<_Type>& rhs) const
	{
		return CDator<_Type>((m_Variable * rhs.m_Variable));
	}

	CDator<_Type>&	operator /	(const CDator<_Type>& rhs) const
	{
		return CDator<_Type>((m_Variable / rhs.m_Variable));
	}

	/////////////////////////////////////////////////////////

	virtual Bool	operator ==	(const CDator<_Type>& rhs) const
	{
		return (m_Variable == rhs.m_Variable);
	}

	virtual Bool	operator !=	(const CDator<_Type>& rhs) const
	{
		return (m_Variable != rhs.m_Variable);
	}

	virtual Bool	operator >	(const CDator<_Type>& rhs) const
	{
		return (m_Variable > rhs.m_Variable);
	}

	virtual Bool	operator <	(const CDator<_Type>& rhs) const
	{
		return (m_Variable < rhs.m_Variable);
	}

	virtual Bool	operator >=	(const CDator<_Type>& rhs) const
	{
		return (m_Variable >= rhs.m_Variable);
	}

	virtual Bool	operator <=	(const CDator<_Type>& rhs) const
	{
		return (m_Variable <= rhs.m_Variable);
	}

};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
template <typename _Type>
class CDatorRef : public CBaseDator
{
protected:
	_Type&	m_Variable;

public:
	CDatorRef(_Type& var, String name = "") : CBaseDator(name), m_Variable(var)
	{	}

	virtual void GetValue(_Type& val) const { val = m_Variable; }
	operator _Type () const { return m_Variable; }

	CDatorRef<_Type>&	operator =	(const CDatorRef<_Type>& rhs)
	{
		m_Variable = rhs.m_Variable;
		return *this;
	}

	CDatorRef<_Type>&	operator =	(const _Type& rhs)
	{
		m_Variable = rhs;
		return *this;
	}

	CDatorRef<_Type>&	operator ++	()
	{
		m_Variable++;
		return *this;
	}

	CDatorRef<_Type>&	operator --	()
	{
		m_Variable--;
		return *this;
	}

	CDatorRef<_Type>&	operator +=	(const CDatorRef<_Type>& rhs)
	{
		m_Variable += rhs.m_Variable;
		return *this;
	}

	CDatorRef<_Type>&	operator +=	(const _Type& rhs)
	{
		m_Variable += rhs;
		return *this;
	}

	CDatorRef<_Type>&	operator -=	(const CDatorRef<_Type>& rhs)
	{
		m_Variable -= rhs.m_Variable;
		return *this;
	}

	CDatorRef<_Type>&	operator -=	(const _Type& rhs)
	{
		m_Variable -= rhs;
		return *this;
	}

	CDatorRef<_Type>&	operator *=	(const CDatorRef<_Type>& rhs)
	{
		m_Variable *= rhs.m_Variable;
		return *this;
	}

	CDatorRef<_Type>&	operator *=	(const _Type& rhs)
	{
		m_Variable *= rhs;
		return *this;
	}

	CDatorRef<_Type>&	operator /=	(const CDatorRef<_Type>& rhs)
	{
		m_Variable /= rhs.m_Variable;
		return *this;
	}

	CDatorRef<_Type>&	operator /=	(const _Type& rhs)
	{
		m_Variable /= rhs;
		return *this;
	}

	CDatorRef<_Type>&	operator +	(const CDatorRef<_Type>& rhs) const
	{
		return CDator<_Type>((m_Variable + rhs.m_Variable));
	}

	CDatorRef<_Type>&	operator -	(const CDatorRef<_Type>& rhs) const
	{
		return CDator<_Type>((m_Variable - rhs.m_Variable));
	}

	CDatorRef<_Type>&	operator *	(const CDatorRef<_Type>& rhs) const
	{
		return CDator<_Type>((m_Variable * rhs.m_Variable));
	}

	CDatorRef<_Type>&	operator /	(const CDatorRef<_Type>& rhs) const
	{
		return CDator<_Type>((m_Variable / rhs.m_Variable));
	}

	virtual Bool	operator ==	(const CDatorRef<_Type>& rhs) const
	{
		return (m_Variable == rhs.m_Variable);
	}

	virtual Bool	operator !=	(const CDatorRef<_Type>& rhs) const
	{
		return (m_Variable != rhs.m_Variable);
	}

	virtual Bool	operator >	(const CDatorRef<_Type>& rhs) const
	{
		return (m_Variable > rhs.m_Variable);
	}

	virtual Bool	operator <	(const CDatorRef<_Type>& rhs) const
	{
		return (m_Variable < rhs.m_Variable);
	}

	virtual Bool	operator >=	(const CDatorRef<_Type>& rhs) const
	{
		return (m_Variable >= rhs.m_Variable);
	}

	virtual Bool	operator <=	(const CDatorRef<_Type>& rhs) const
	{
		return (m_Variable <= rhs.m_Variable);
	}

};

//////////////////////////////////////////////////////////////////////////

// typedef CDator<String>	StringD;
// typedef CDator<Int8>	StringD;
// typedef CDator<UInt8>	StringD;
// typedef CDator<Int16>	StringD;
// typedef CDator<UInt16>	StringD;
// typedef CDator<Int32>	StringD;
// typedef CDator<UInt32>	StringD;
// typedef CDator<Int64>	StringD;
// typedef CDator<UInt64>	StringD;
// typedef CDator<Float>	StringD;
// typedef CDator<Double>	StringD;
// typedef CDator<Bool>	StringD;