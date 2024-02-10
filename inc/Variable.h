#pragma once

#include "dynamicarray.h"

//////////////////////////////////////////////////////////////////////////

class CBaseVariable
{
};


//////////////////////////////////////////////////////////////////////////

//		Float "array" because I don't know what else to use
template <typename _customType = Float*>
class CVariable : public CBaseVariable
{
public:
	enum EType
	{
		eInt8,		eUInt8,
		eInt16,		eUInt16,
		eInt32,		eUInt32,
		eInt64,		eUInt64,

		eFloat,		eDouble,

		eBool,

		eString,

		ePointer,	//	UInt32*

		eArray,		//CBaseDynamicArray*

		eCustom,

		eInvalid
	};

	union UValues
	{
		Int8				Int8Val;
		UInt8				UInt8Val;

		Int16				Int16Val;
		UInt16				UInt16Val;

		Int32				Int32Val;
		UInt32				UInt32Val;

		Int64				Int64Val;
		UInt64				UInt64Val;

		Float				FloatVal;
		Double				DoubleVal;

		Bool				BoolVal;

		Pointer				PointerVal;

		String*				pstrVal;
		CBaseDynamicArray*	parrVal;

		_customType*		pmyVal;
	};

	///////////////////////////////////////////////////////////////////////

public:
	CVariable(void) : m_eType(eInvalid)/*, m_uValue.pstrVal(NULL)*/
	{	m_uValue.pstrVal = NULL; }

#define CTOR(_Type)	explicit CVariable(_Type val) : m_eType(e##_Type), m_uValue.##_Type##Val(val)\
	{	DeletePointers();	}

	CTOR(Int8);
	CTOR(UInt8);
	CTOR(Int16);
	CTOR(UInt16);
	CTOR(Int32);
	CTOR(UInt32);
	CTOR(Int64);
	CTOR(UInt64);
	CTOR(Float);
	CTOR(Double);
	CTOR(Bool);
	CTOR(Pointer);

#undef CTOR

	explicit CVariable(const CVariable& var) : m_eType(var.m_eType)
	{
		*this = var;
	}

	~CVariable(void)
	{
		DeletePointers();
	}

	explicit CVariable(const String& strString) : m_eType(eString)
	{
		DeletePointers();
		m_uValue.pstrVal = new String(strString);
	}

// 	explicit CVariable(const CBaseDynamicArray& aArray) : m_eType(eArray)
// 	{
// 		DeletePointers();
// 		m_uValue.parrVal = new String(strString);
// 	}

	explicit CVariable(const _customType& object) : m_eType(eCustom)
	{
		DeletePointers();
		m_uValue.pmyVal = new _customType(object);
	}

#define COMP(op, _Type)		Bool operator op (const _Type& val) \
							{	return (m_uValue.##_Type##Val op val);		}
#define COMP_TYPE(_Type)	COMP(>,		_Type)\
							COMP(>=,	_Type)\
							COMP(<,		_Type)\
							COMP(<=,	_Type)\
							COMP(==,	_Type)\
							COMP(!=,	_Type)

	COMP_TYPE(Int8);
	COMP_TYPE(UInt8);
	COMP_TYPE(Int16);
	COMP_TYPE(UInt16);
	COMP_TYPE(Int32);
	COMP_TYPE(UInt32);
	COMP_TYPE(Int64);
	COMP_TYPE(UInt64);
	COMP_TYPE(Float);
	COMP_TYPE(Double);
	COMP_TYPE(Bool);
	COMP_TYPE(Pointer);

#undef COMP_TYPE
#undef COMP

#define OP(op, _Type)		CVariable& operator op (const _Type& val) \
							{	m_uValue.##_Type##Val op val;	return *this;	}
 #define OP_TYPE(_Type)		OP(+,	_Type)\
							OP(+=,	_Type)\
							OP(-,	_Type)\
							OP(-=,	_Type)\
							OP(*,	_Type)\
							OP(*=,	_Type)\
							OP(/,	_Type)\
							OP(/=,	_Type)

	OP_TYPE(Int8);
	OP_TYPE(UInt8);
	OP_TYPE(Int16);
	OP_TYPE(UInt16);
	OP_TYPE(Int32);
	OP_TYPE(UInt32);
	OP_TYPE(Int64);
	OP_TYPE(UInt64);
	OP_TYPE(Float);
	OP_TYPE(Double);
	OP_TYPE(Bool);
	OP_TYPE(Pointer);
 
 #undef OP_TYPE
#undef OP

#define VAR_OP(op)	CVariable& operator op (const CVariable& val) {	\
						ASSERT(m_eType == val.m_eType);	\
						switch (m_eType) {	\
						case eInt8:	\
							m_uValue.Int8Val op val.m_uValue.Int8Val; break;	\
						case eUInt8:	\
							m_uValue.UInt8Val op val.m_uValue.UInt8Val; break;	\
						case eInt16:	\
							m_uValue.Int16Val op val.m_uValue.Int16Val; break;	\
						case eUInt16:	\
							m_uValue.UInt16Val op val.m_uValue.UInt16Val; break;	\
						case eInt32:	\
							m_uValue.Int32Val op val.m_uValue.Int32Val; break;	\
						case eUInt32:	\
							m_uValue.UInt32Val op val.m_uValue.UInt32Val; break;	\
						case eInt64:	\
							m_uValue.Int64Val op val.m_uValue.Int64Val; break;	\
						case eUInt64:	\
							m_uValue.UInt64Val op val.m_uValue.UInt64Val; break;	\
						case eFloat:	\
							m_uValue.FloatVal  op val.m_uValue.FloatVal; break;	\
						case eDouble:	\
							m_uValue.DoubleVal  op val.m_uValue.DoubleVal; break;	\
						default:	\
							ASSERT(false);	}	\
						return *this;	}
	
	VAR_OP(+	)
	VAR_OP(+=	)
	VAR_OP(-	)
	VAR_OP(-=	)
	VAR_OP(*	)
	VAR_OP(*=	)
	VAR_OP(/	)
	VAR_OP(/=	)

#undef VAR_OP

#define ASSIGNMENT(_Type)	CVariable& operator = (const _Type& val)\
							{	DeletePointers(); \
								m_uValue.##_Type##Val = val; m_eType = e##_Type; return *this;	}

	ASSIGNMENT(Int8);
	ASSIGNMENT(UInt8);
	ASSIGNMENT(Int16);
	ASSIGNMENT(UInt16);
	ASSIGNMENT(Int32);
	ASSIGNMENT(UInt32);
	ASSIGNMENT(Int64);
	ASSIGNMENT(UInt64);
	ASSIGNMENT(Float);
	ASSIGNMENT(Double);
	ASSIGNMENT(Bool);
	ASSIGNMENT(Pointer);
	//ASSIGNMENT(const String&)

#undef ASSIGNMENT

	CVariable& operator = (const String& strString)
	{	
		DeletePointers();

		m_uValue.pstrVal = new String(strString); 
		m_eType = eString;

		return *this;
	}

	CVariable& operator = (const _customType& val)
	{	
		DeletePointers();
	
		m_uValue.pmyVal = val; 
		m_eType = eCustom; 
		
		return *this;
	}

#define CONVERTER(_Type)	operator _Type () const	{	ASSERT(m_eType == e##_Type); return m_uValue.##_Type##Val;	}

	CONVERTER(Int8);
	CONVERTER(UInt8);
	CONVERTER(Int16);
	CONVERTER(UInt16);
	CONVERTER(Int32);
	CONVERTER(UInt32);
	CONVERTER(Int64);
	CONVERTER(UInt64);
	CONVERTER(Float);
	CONVERTER(Double);
	CONVERTER(Bool);
	CONVERTER(Pointer);
	//CONVERTER(const String&)

#undef CONVERTER

	operator String ()
	{
		ASSERT(m_eType == eString);
		return *(m_uValue.pstrVal);
	}

// 	operator CBaseDynamicArray ()
// 	{
// 		ASSERT(m_eType == eString);
// 		return *(m_uValue.pstrVal);
// 	}

	operator _customType ()
	{
		ASSERT(m_eType == eCustom);
		return *(m_uValue.pmyVal);
	}

#define TO_STRING(val)			String(#val)
	
	String ToString() const
	{
		return TO_STRING(m_eType);
	}

#undef TO_STRING

	/////////////////////////////////////////////////////

	CVariable& operator ++()
	{
		switch (m_eType)
		{
		case eInt8:
			m_uValue.Int8Val++; break;
		case eUInt8:
			m_uValue.UInt8Val++; break;
		case eInt16:
			m_uValue.Int16Val++; break;
		case eUInt16:
			m_uValue.UInt16Val++; break;
		case eInt32:
			m_uValue.Int32Val++; break;
		case eUInt32:
			m_uValue.UInt32Val++; break;
		case eInt64:
			m_uValue.Int64Val++; break;
		case eUInt64:
			m_uValue.UInt64Val++; break;

		case eFloat:
			m_uValue.FloatVal += 1.0f; break;
		case eDouble:
			m_uValue.DoubleVal += 1.0f; break;

		case ePointer:
			m_uValue.PointerVal++; break;

		default:	//We don't allow incrementing any other types
			ASSERT(false);
		}

		return *this;
	}

	CVariable& operator --()
	{
		switch (m_eType)
		{
		case eInt8:
			m_uValue.Int8Val--; break;
		case eUInt8:
			m_uValue.UInt8Val--; break;
		case eInt16:
			m_uValue.Int16Val--; break;
		case eUInt16:
			m_uValue.UInt16Val--; break;
		case eInt32:
			m_uValue.Int32Val--; break;
		case eUInt32:
			m_uValue.UInt32Val--; break;
		case eInt64:
			m_uValue.Int64Val--; break;
		case eUInt64:
			m_uValue.UInt64Val--; break;

		case eFloat:
			m_uValue.FloatVal -= 1.0f; break;
		case eDouble:
			m_uValue.DoubleVal -= 1.0f; break;

		case ePointer:
			m_uValue.PointerVal--; break;

		default:	//We don't allow incrementing any other types
			ASSERT(false);
		}

		return *this;
	}

// 	CVariable(const CBaseDynamicArray* val) : m_eType(eArray), pstrVal(NULL)
// 	{
// 		parrVal = new String(val);
// 		ASSERT(pstrVal);
// 	}

protected:
	UValues	m_uValue;
	EType	m_eType;

	void DeletePointers()
	{
		//if (m_eType == eString)		SAFE_DELETE(m_uValue.pstrVal);	
		if (m_eType == eArray)		SAFE_DELETE(m_uValue.parrVal);	
		if (m_eType == eCustom)		SAFE_DELETE(m_uValue.pmyVal);
	}
};

//////////////////////////////////////////////////////////////////////////

typedef CVariable<>					Var;

typedef Var							TArgument;
typedef CDynamicArray<TArgument>	TArgumentArray;