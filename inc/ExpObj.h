#pragma once

#include "Flag.h"
#include "Functor.h"

#include "Vec.h"
#include "Matrix.h"

#include "GlobalFuncs.h"

//////////////////////////////////////////////////////////////////////////
namespace GCAS
{

//////////////////////////////////////////////////////////////////////////

enum EMathFunctions
{
	eMF_Sin,	
	eMF_Cos,
	eMF_Tan,

	/*eMF_Csc,
	eMF_Sec,
	eMF_Cot,*/

	eMF_ASin,
	eMF_ACos,
	eMF_ATan,

	eMF_Sinh,
	eMF_Cosh,
	eMF_Tanh,

	eMF_Exp,
	eMF_Ln,
	eMF_Log,

	eMF_NumMathFuncs,

	eMF_None,
};

//static const EMathFunctions	gk_Derivatives[] = 
//{
//
//};

extern const char* gk_FunctionNames[];
extern const char* gk_FunctionSymbols[];

typedef COpenFunctor<Float, Float>	TMathFunctionF;
typedef COpenFunctor<Int32, Int32>	TMathFunctionI;

typedef	TMathFunctionF				TMathFunction;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

class CNode
{
public:
	enum ENodeType
	{
		eNT_Operator,
		eNT_Function,
		eNT_Value,
			eNT_Var,
			eNT_Const,

		eNT_Void,

		eNT_NumTypes
	};

	enum EValueType
	{
		eVT_Scalar,
		eVT_Vector,
		eVT_Matrix,
		eVT_Tensor,

		eVT_NumTypes,

		eVT_Undefined
	};

public:
	CNode(const String& str = "") : m_String(str), m_eNodeType(eNT_Void)
	{	}

	virtual ~CNode(void)
	{	}

	virtual String FromString(Bool bName = false)
	{
		bName;
		return m_String;
	}

	ENodeType GetType() const
	{
		return m_eNodeType;
	}

	Int32 SetName(const String& name)
	{
		ASSERT(!name.empty());
		m_strName = name;
		return 1;
	}

	const String& GetName() const
	{
		return m_strName;
	}

protected:
	String		m_String;
	String		m_strName;
	ENodeType	m_eNodeType;
};

//////////////////////////////////////////////////////////////////////////

class CValueType : public CNode
{
public:
	CValueType(const String& str = "") : CNode(str), m_bConstrained(false)
	{	m_eNodeType = eNT_Value;	}

	//virtual CNode* SetValue(Float val);
	//virtual CNode* SetValue(CNode* pRHS);
	//virtual Float GetValue() const;
	//virtual void Invert();

	virtual EValueType GetValueType() const
	{	return m_eValueType;	}
	
	virtual void SetValueType(EValueType eVT)
	{	m_eValueType = eVT;		}
	
protected:
	Bool		m_bConstrained;
	EValueType	m_eValueType;
};

//////////////////////////////////////////////////////////////////////////


#define SteppedVariableSetFunc(_Type, _Kind)	virtual CValue<_Type>* SetValue##_Kind(_Type val)	\
	{	m_Value##_Kind = val;	return this;	}	\
	virtual CValue<_Type>* SetValue##_Kind(CValue<_Type>* pRHS)	\
	{	ASSERT(pRHS);	m_Value##_Kind = pRHS->GetValue##_Kind();	return this;	}	\
	virtual _Type GetValue##_Kind() const	\
	{	return m_Value##_Kind;		}

#define	SteppedVariableSetFuncs(_Type)	SteppedVariableSetFunc(_Type,)	\
										SteppedVariableSetFunc(_Type, Min)	\
										SteppedVariableSetFunc(_Type, Max)	\
										SteppedVariableSetFunc(_Type, Step)
///////////////////////////////////////

template<typename _Type>
class CValue : public CValueType
{
public:
	explicit CValue(const String& str = "") : CValueType(str)
	{
		SetValueMin(-1.0f);
		SetValueMax(1.0f);
		SetValueStep(0.01f);
		SetValueType(eVT_Scalar);
	}

	virtual ~CValue(void)
	{	}

	SteppedVariableSetFuncs(_Type)

	void Invert()
	{
		m_Value = 1.0f / m_Value;
	}

	virtual String FromString(Bool bName = false)
	{
		if (bName && !GetName().empty())	//Name if var
			return GetName();

		//if (m_String.empty())
		{
			if (static_cast<Int32>(m_Value) == m_Value)
				m_String = IntToString(m_Value);
			else
				m_String = FloatToString(m_Value, 4);
		}

		return m_String;
	}

	virtual String GetValueTypeStr() const
	{	return "Real";			}

public:
	SteppedVariable(_Type, m_Value);	//TODO: Set def min, max vals
};

template<>
class CValue<Vec3> : public CValueType
{
public:
	explicit CValue(const String& str = "") : CValueType(str)
	{	SetValueType(eVT_Vector);	}

	virtual ~CValue(void)
	{	}

	SteppedVariableSetFuncs(Vec3)

	void Invert()
	{
		m_Value *= -1.0f;
	}

	virtual String FromString(Bool bName = false)
	{
		if (bName && !GetName().empty())
			return GetName();

		//if (m_String.empty())
		{
			m_String =	Vec3ToString(&m_Value);
		}

		return m_String;
	}

	void SetX(Float x)
	{	m_Value.x = x;	}

	void SetY(Float y)
	{	m_Value.y = y;	}

	void SetZ(Float z)
	{	m_Value.z = z;	}

	virtual String GetValueTypeStr() const
	{	return "Vector";		}

public:
	SteppedVariable(Vec3, m_Value);
};

template<>
class CValue<CMatrix> : public CValueType
{
public:
	explicit CValue(const String& str = "", EValueType eVT = eVT_Matrix) : CValueType(str)
	{	SetValueType(eVT);	}

	virtual ~CValue(void)
	{	}

	virtual CValue<CMatrix>* SetValue(const CMatrix& val)
	{
		m_Value = val;
		return this;
	}

	virtual CMatrix GetValue() const
	{
		return m_Value;
	}

	void Invert()
	{
		for (UInt32 i = 0; i < 4; i++)
			for (UInt32 j = 0; j < 4; j++)
				m_Value.m[i][j] *= -1.0f;
	}

	virtual String FromString(Bool bName = false)
	{
		if (bName)
			return GetName();

		//if (m_String.empty())
		{
			//m_String = FloatToString(m_Value.Magnitude());
		}

		return m_String;
	}

	virtual String GetValueTypeStr() const
	{	return "Matrix";		}

	/*virtual String ValueString()
	{
	}*/

public:
	CMatrix		m_Value;
}; 

//////////////////////////////////////////////////////////////////////////

class CBaseVariable
{
public:
	Bool IsIndependent() const
	{	return m_bIndependent;	}

	void SetIndependence(Bool bInd = true)
	{	m_bIndependent = bInd;	}

protected:
	Bool	m_bIndependent;
};

//////////////////////////////////////////////////////////////////////////

template<typename _Type>
class CVar : public CValue<_Type>, public CBaseVariable
{
public:
	explicit CVar(const String& str = "") : CValue(str)
	{
		m_eNodeType = eNT_Var;
		//SetName(name);
		SetIndependence(false);
	}

	explicit CVar(const String& name, _Type val) : CValue()
	{
		m_eNodeType = eNT_Var;
		SetName(name);
		SetValue(val);
		SetIndependence(false);
	}

	virtual ~CVar(void)
	{	}

	Bool operator == (const CVar<_Type>& rhs) const
	{
		return (GetName() == rhs.GetName());
	}

	Bool operator != (const CVar<_Type>& rhs) const
	{
		return !((*this) == rhs);
	}
};

template<>
class CVar<Vec3> : public CValue<Vec3>, public CBaseVariable
{
public:
	explicit CVar(const String& str = "", Bool temp = true) : CValue(str), m_bTemp(temp)
	{
		m_eNodeType = eNT_Var;
		if (!str.empty())	SetName(str);
		SetIndependence(false);
		m_X = m_Y = m_Z = NULL;
	}

	explicit CVar(const String& name, const Vec3& val, Bool temp = false) : CValue(), m_bTemp(temp)
	{
		m_eNodeType = eNT_Var;
		SetName(name);
		SetValue(val);
		SetIndependence(false);
		m_X = m_Y = m_Z = NULL;
	}

	virtual ~CVar(void)
	{	}

	Bool operator == (const CVar<Vec3>& rhs) const
	{
		return (GetName() == rhs.GetName());
	}

	Bool operator != (const CVar<Vec3>& rhs) const
	{
		return !((*this) == rhs);
	}

	virtual String FromString(Bool bName = false)
	{
		/*if (m_String.empty())
			m_String = m_strName;*/

		if (bName)
		{
			if (m_X && m_Y)
			{
				m_String =	"<";
				m_String +=	m_X->GetName();
				m_String += ",";
				m_String +=	m_Y->GetName();

				if (m_Z)
				{
					m_String += ",";
					m_String +=	m_Z->GetName();
				}

				m_String += ">";
			}
			else
				return GetName();
		}
		else
		{
			UpdateValue();
			m_String =	"<";
			m_String +=	FloatToString(m_Value.x);
			m_String += ",";
			m_String +=	FloatToString(m_Value.y);

			if (m_Z)
			{
				m_String += ",";
				m_String +=	FloatToString(m_Value.z);
			}

			m_String += ">";
		}
		return m_String;
	}

	Bool IsTempVec() const
	{
		return m_bTemp;
	}

	void UpdateValue()
	{
		if (m_X && m_Y)
		{
			if (m_Z)
				m_Value = Vec3(	m_X->GetValue(),
								m_Y->GetValue(),
								m_Z->GetValue());
			else
				m_Value = Vec2( m_X->GetValue(),
								m_Y->GetValue());
		}
	}

	void SetX(CValue<Float>* pX)
	{	m_X = pX;				}

	void SetY(CValue<Float>* pY)
	{	m_Y = pY;				}

	void SetZ(CValue<Float>* pZ)
	{	m_Z = pZ;				}

protected:

	Bool			m_bTemp;

	CValue<Float>*	m_X;
	CValue<Float>*	m_Y;
	CValue<Float>*	m_Z;
};

//////////////////////////////////////////////////////////////////////////

template<typename _Type = Float>
class CConst : public CValue<_Type>
{
public:
	explicit CConst(const String& str = "") : CValue(str)
	{
		m_eNodeType = eNT_Const;
	}

	explicit CConst(_Type val) : CValue()
	{
		SetValue(val);
		m_eNodeType = eNT_Const;
	}

	virtual ~CConst(void)
	{	}
};

//////////////////////////////////////////////////////////////////////////
enum EOperatorType
{
	eOT_Assignment,

	eOT_Add,		eOT_Subtract,
	eOT_Multiply,	eOT_Divide,

	eOT_Power,

	eOT_Function,

	eOT_NumOperatorTypes,

	eOT_Invalid
};

enum EOperatorFlags
{
	eOF_None,

	eOF_Binary,
	eOF_Unary,
	//eOF_Prefix,
	
	eOF_Left,
	eOF_Right,

	eOF_Associative,

	eOF_Default = eOF_Binary
};

class COperatorBase : public CNode
{
public:
	COperatorBase(const String& str = "") : CNode(str)	{	}
};

//template <typename _Type1 = CValue<Float>*, typename _Type2 = CValue<Float>*>
class COperator : public COperatorBase
{
public:

	typedef COpenFunctor<CValueType*, CValueType*>					TUnaryFunctor;
	typedef COpenFunctor2<CValueType*, CValueType*, CValueType*>	TBinaryFunctor;
	
public:
	explicit COperator(const String& str = "") : COperatorBase(str)
	{
		m_eNodeType = eNT_Operator;

		SetOpType(eOT_Invalid);
	}

	explicit COperator(EOperatorType opType, TBinaryFunctor functor, UInt32 flags = eOF_Default) : COperatorBase()
	{
		m_eNodeType = eNT_Operator;

		m_BinaryFunctor = functor;

		m_eFlags = flags;
		SetOpType(opType);
	}

	explicit COperator(EOperatorType opType, TUnaryFunctor functor, UInt32 flags = eOF_Default) : COperatorBase()
	{
		m_eNodeType = eNT_Operator;

		m_UnaryFunctor = functor;

		m_eFlags = flags;
		SetOpType(opType);
	}

	COperator(COperatorBase* pBase)
	{
		*this = *static_cast<COperator*>(pBase);
	}

	virtual ~COperator(void)
	{	}

	void SetString(const String& str)
	{
		m_String = str;
	}

	void SetOpType(EOperatorType opType)
	{
		m_eType = opType;
	}

	void FlipOpType()
	{
		switch (m_eType)
		{
		case eOT_Assignment:
			ASSERT(0);

		case eOT_Add:
			SetOpType(eOT_Subtract);
			break;

		case eOT_Subtract:
			SetOpType(eOT_Add);
			break;

		case eOT_Multiply:
			SetOpType(eOT_Divide);
			break;

		case eOT_Divide:
			SetOpType(eOT_Multiply);
			break;

		case eOT_Power:
			break;

		case eOT_Function:
			ASSERT(0);
			break;
		}

		m_String = "";
	}

	EOperatorType GetOpType() const
	{
		return m_eType;
	}

	const Flag<EOperatorFlags>& GetFlags() const
	{
		return m_eFlags;
	}

	virtual String FromString(Bool bName = false)
	{
		//if (m_String.empty())
		{
			switch (this->GetOpType())
			{
			case eOT_Assignment:	m_String = "=";		break;

			case eOT_Add:			m_String = "+";		break;
			case eOT_Subtract:		m_String = "-";		break;
			case eOT_Multiply:		m_String = "*";		break;
			case eOT_Divide:		m_String = "/";		break;

			case eOT_Power:			m_String = "^";		break;

			case eOT_Function:
				if (bName)
					m_String = GetNameFromSymbol(m_String);
				else
					m_String = GetSymbolFromName(m_String);
				break;
			}
		}

		return m_String;
	}

	String ToString()
	{
		if (m_String.empty())
		{
			switch (this->GetOpType())
			{
			case eOT_Assignment:	m_String = "=";		break;

			case eOT_Add:			m_String = "+";		break;
			case eOT_Subtract:		m_String = "-";		break;
			case eOT_Multiply:		m_String = "*";		break;
			case eOT_Divide:		m_String = "/";		break;

			case eOT_Power:			m_String = "^";		break;

			case eOT_Function:		m_String = gk_FunctionNames[0];		break;
			}
		}

		return m_String;
	}

	static String GetNameFromSymbol(const String& symbol, UInt32* pIndex = NULL)
	{
		for (UInt32 i = 0; i < eMF_NumMathFuncs; i++)
		{
			if (symbol == gk_FunctionSymbols[i])
			{
				if (pIndex)	*pIndex = i;
				return gk_FunctionNames[i];
			}

			if (symbol == gk_FunctionNames[i])
			{
				if (pIndex)	*pIndex = i;
				return symbol;
			}
		}

		ASSERT(0);
	}

	static String GetSymbolFromName(const String& name, UInt32* pIndex = NULL)
	{
		for (UInt32 i = 0; i < eMF_NumMathFuncs; i++)
		{
			if (name == gk_FunctionNames[i])
			{
				if (pIndex)	*pIndex = i;
				return gk_FunctionSymbols[i];
			}

			if (name == gk_FunctionSymbols[i])
			{
				if (pIndex)	*pIndex = i;
				return name;
			}
		}

		ASSERT(0);
	}

	CValueType* operator() (CValueType* arg)
	{
		return m_UnaryFunctor(arg);
	}

	CValueType* operator() (CValueType* arg1, CValueType* arg2)
	{
		return m_BinaryFunctor(arg1, arg2);
	}

protected:
	EOperatorType			m_eType;
	Flag<EOperatorFlags>	m_eFlags;

	TUnaryFunctor			m_UnaryFunctor;
	TBinaryFunctor			m_BinaryFunctor;
};

//////////////////////////////////////////////////////////////////////////

class CFunction : public CNode
{
public:
	explicit CFunction(const String& str = "") : CNode(str)
	{
		m_eNodeType = eNT_Function;
	}

	virtual ~CFunction(void)
	{	}
};

//////////////////////////////////////////////////////////////////////////

typedef GCAS::CValue<Float>		FVal;
typedef GCAS::CValue<Vec2>		V2Val;
typedef GCAS::CValue<Vec3>		V3Val;
typedef V3Val					VVal;
typedef GCAS::CValue<CMatrix>	MVal;

typedef GCAS::CVar<Float>		FVar;
typedef GCAS::CVar<Vec2>		V2Var;
typedef GCAS::CVar<Vec3>		V3Var;
typedef V3Var					VVar;
typedef GCAS::CVar<CMatrix>		MVar;

typedef GCAS::CConst<Float>		FConst;
typedef GCAS::CConst<Vec2>		V2Const;
typedef GCAS::CConst<Vec3>		V3Const;
typedef V3Const					VConst;
typedef GCAS::CConst<CMatrix>	MConst;
//////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////