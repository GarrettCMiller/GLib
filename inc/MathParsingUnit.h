#pragma once

#include "DynamicArray.h"

#include "Stack.h"
#include "Tree.h"

#include "ExpObj.h"

#include "Polynomial.h"

#include "CritSection.h"

//////////////////////////////////////////////////////////////////////////

#define	ASSERT_VAR(pVar)	ASSERT(	static_cast<FVar*>(pVar) ||	\
									static_cast<VVar*>(pVar) ||	\
									static_cast<MVar*>(pVar)	)

//////////////////////////////////////////////////////////////////////////

enum EOperatorPriority
{
	eOP_Parentheses		= 0,

	eOP_Function		= 1,

	eOP_Exponents,

	eOP_Multiply,		//eOP_Divide,

	eOP_Add,			//eOP_Subtract,

	eOP_First = eOP_Function,	//eOP_Exponents,	//eOP_Parentheses
	eOP_Last = eOP_Add,			//eOP_Subtract
};

const String k_strOperators[] =
{
	"(",	")",
	"^",
	"*/",	//"/",
	"+-",	//"-"
};

class CMathParsingUnit;

//////////////////////////////////////////////////////////////////////////

template <typename _Type>
class CMathParseTree : public CTree<_Type>
{
public:	//<_Type>
	CMathParseTree() : CTree()	{ pParser = NULL;	}
	virtual ~CMathParseTree()	{ CTree::~CTree();	}

	CMathParsingUnit*	pParser;

public:
	NodePtr GetRoot()
	{
		return m_pRoot;
	}

	Bool ContainsVar(NodePtr pNode, GCAS::CNode* pVar) const
	{
		ASSERT(m_pRoot);

		//If pNode is null, start from the root
		if (!pNode)
			pNode = m_pRoot;

		if (pNode->item->GetType() == GCAS::CNode::eNT_Var)
		{
			//If pVar is null, we are looking for any variable
			if (!pVar)
				return true;

			ASSERT_VAR(pVar);

			if (pNode->item->GetName() == pVar->GetName())
				return true;
		}

		Bool ret = false;

		for (UInt32 i = 0; i < pNode->numChildren; i++)
		{
			if (pNode->child[i])
			{
				ret = ContainsVar(pNode->child[i], pVar);

				if (ret)
					return true;
			}
		}

		return ret;
	}

	Bool SolveFor(GCAS::CNode* pVar)
	{
		if (!m_pRoot)
			return false;

		ASSERT_VAR(pVar);

		//0 = left, -1 = error, 1 = right
		Int32 iSide = -1;

		if (m_pRoot->child[0] && !BAD_PTR(m_pRoot->child[0]) && ContainsVar(m_pRoot->child[0], pVar))
			iSide = 0;

		if (m_pRoot->child[1] && !BAD_PTR(m_pRoot->child[1]) && ContainsVar(m_pRoot->child[1], pVar))
		{
			if (iSide == 0)
				ASSERT(0);

			iSide = 1;
		}

		//we didnt find the variable to solve for
		if (iSide == -1)
			return false;

		NodePtr pChild = m_pRoot->child[iSide];

		//reset
		Int32 iChildSide = -1;

		if (pChild->child[0] && !BAD_PTR(m_pRoot->child[0]) && ContainsVar(pChild->child[0], pVar))
			iChildSide = 0;

		if (pChild->child[1] && !BAD_PTR(m_pRoot->child[1]) && ContainsVar(pChild->child[1], pVar))
		{
			if (iChildSide == 0)
				ASSERT(0);

			iChildSide = 1;
		}

		//this node is the variable to solve for
		if (iChildSide == -1)
			return true;

		NodePtr pNextChild = pChild->child[iChildSide];

		NodePtr tempChild = pChild->child[!iChildSide];

		//swap op
		if (pChild->item->GetType() == GCAS::CNode::eNT_Operator)
		{
			static_cast<GCAS::COperator*>(pChild->item)->FlipOpType();

			switch (static_cast<GCAS::COperator*>(pChild->item)->GetOpType())
			{
			case GCAS::eOT_Add:
			case GCAS::eOT_Subtract:
			case GCAS::eOT_Divide:
				pChild->child[0] = m_pRoot->child[!iSide];
				pChild->child[1] = tempChild;
				break;

			case GCAS::eOT_Multiply:
				pChild->child[!iChildSide] = m_pRoot->child[!iSide];
				pChild->child[iChildSide] = tempChild;
				break;

			case GCAS::eOT_Power:
				pChild->child[0] = m_pRoot->child[!iSide];
				pChild->child[1] = tempChild;

				if (IsFraction(pChild->child[1], true))
				{
					FractionToReal(pChild->child[1]);
				}
				else
				{
					static_cast<GCAS::CConst<Float>*>(pChild->child[1]->item)->Invert();

					if(IsRational(pChild->child[1]))
						RealToFraction(pChild->child[1]);
				}

				break;
			}
		}

		m_pRoot->child[!iSide] = pChild;

		m_pRoot->child[iSide] = pNextChild;

		return true;
	}

	Bool IsRational(NodePtr pNode) const
	{
		static const Float epsilon = 0.00001;

		if (pNode->item->GetType() != GCAS::CNode::eNT_Const)
			return false;

		GCAS::CConst<>* pVal = static_cast<GCAS::CConst<>*>(pNode->item);
		Float val = pVal->GetValue();

		Float nVal = fabsf(static_cast<Int32>((1.0 / val) + epsilon));

		return (static_cast<Int32>(1.0 / val + epsilon) == nVal);
	}

	Bool IsFraction(NodePtr pNode, Bool bDivOnly = false) const
	{
		if (pNode->item->GetType() != GCAS::CNode::eNT_Operator)
		{
			if (bDivOnly)
				return false;

			return IsRational(pNode);
		}

		GCAS::COperator* pOp = static_cast<GCAS::COperator*>(pNode->item);

		if (pOp->GetOpType() != GCAS::eOT_Divide)
			return false;	//for now

		//other conditions maybe???
		//const children only?

		return true;
	}

	void RealToFraction(NodePtr pNode)
	{
		ASSERT(pParser);

		if (pNode->item->GetType() != GCAS::CNode::eNT_Const)
			return;

		Float val = static_cast<GCAS::CConst<>*>(pNode->item)->GetValue();

		val = 1.0f / val;
		
		NodePtr pParent = pNode->parent;

		delete pNode->item;

		pNode->item = new GCAS::COperator(pParser->m_arrOperators[eOT_Divide]);

		GCAS::CConst<> *num, *den;
		NodePtr pnum, pden;

		num = new GCAS::CConst<>(1.0f);
		den = new GCAS::CConst<>(val);

		pnum = new Node(num);
		pden = new Node(den);

		pNode->AddChild(pnum);
		pNode->AddChild(pden);
	}

	void FractionToReal(NodePtr pNode)
	{
		if (pNode->item->GetType() != GCAS::CNode::eNT_Operator)
			return;

		GCAS::COperator* pOp = static_cast<GCAS::COperator*>(pNode->item);

		if (!IsFraction(pNode, true))
			return;

		Float	num	= static_cast<GCAS::CConst<>*>(pNode->child[0]->item)->GetValue(),
				den = static_cast<GCAS::CConst<>*>(pNode->child[1]->item)->GetValue();

		Float	real = num / den;

		//simple (slow) way
		//Remove(pNode);
		//Remove all children
		pNode->RemoveChild();

		//delete item
		delete pNode->item;

		pNode->item = new GCAS::CConst<>(real);
	}

	template <typename _Functor, typename _Parenth>
	void TraverseForString(NodePtr pNode, _Functor& functor, _Parenth& parenth)
	{
		if (!m_pRoot)
			return;

		if (!pNode)
			pNode = m_pRoot;

		GCAS::COperator* pOp = static_cast<GCAS::COperator*>(pNode->item);

		if (pOp->GetType() == CNode::eNT_Operator)
		{
			if (pOp->GetOpType() == GCAS::eOT_Multiply)
			{
				GCAS::COperator* pChildOp = static_cast<GCAS::COperator*>(pNode->child[0]->item);

				if (pChildOp->GetType() == CNode::eNT_Operator &&
					pChildOp->GetOpType() == GCAS::eOT_Add ||
					pChildOp->GetOpType() == GCAS::eOT_Subtract)
					parenth(true);
			}

			if (static_cast<GCAS::COperator*>(pNode->item)->GetOpType() == GCAS::eOT_Function)
			{
				functor(pNode);

				if (pNode->child[0])
					TraverseForString(pNode->child[0], functor, parenth);

				parenth(false);

				return;
			}
		}

		/////////////////
		if (pNode->child[0])
			TraverseForString(pNode->child[0], functor, parenth);
		/////////////////

		if (pOp->GetType() == CNode::eNT_Operator)
		{
			if (pOp->GetOpType() == GCAS::eOT_Multiply)
			{
				GCAS::COperator* pChildOp = static_cast<GCAS::COperator*>(pNode->child[0]->item);

				if (pChildOp->GetType() == CNode::eNT_Operator &&
					pChildOp->GetOpType() == GCAS::eOT_Add ||
					pChildOp->GetOpType() == GCAS::eOT_Subtract)
					parenth(false);
			}
		}
		
		/////////////////
		functor(pNode);
		/////////////////

		if (pOp->GetType() == CNode::eNT_Operator)
		{
			if (pOp->GetOpType() == GCAS::eOT_Multiply)
			{
				GCAS::COperator* pChildOp = static_cast<GCAS::COperator*>(pNode->child[1]->item);

				if (pChildOp->GetType() == CNode::eNT_Operator &&
					pChildOp->GetOpType() == GCAS::eOT_Add ||
					pChildOp->GetOpType() == GCAS::eOT_Subtract)
					parenth(true);
			}
		}
		
		/////////////////
		if (pNode->child[1])
			TraverseForString(pNode->child[1], functor, parenth);
		/////////////////

		if (pOp->GetType() == CNode::eNT_Operator)
		{
			if (pOp->GetOpType() == GCAS::eOT_Multiply)
			{
				GCAS::COperator* pChildOp = static_cast<GCAS::COperator*>(pNode->child[1]->item);

				if (pChildOp->GetType() == CNode::eNT_Operator &&
					pChildOp->GetOpType() == GCAS::eOT_Add ||
					pChildOp->GetOpType() == GCAS::eOT_Subtract)
					parenth(false);
			}
		}
	}
};

class CMathException : public CUserException
{
public:
	CMathException(const String& strMsg = "") : CUserException()
	{
		m_strMsg = strMsg;

		if (strMsg == "")
			m_strMsg = "Unknown Error!";
	}

	virtual int ReportError(UINT nType = 0, UINT nMessageID = 0)
	{
		nMessageID; nType;
		return MessageBox(NULL, m_strMsg.c_str(), "Math Error!", MB_OK);
	}

	String GetString() const
	{
		return m_strMsg;
	}

protected:
	String	m_strMsg;
};

//////////////////////////////////////////////////////////////////////////

class CMathParsingUnit
{
public:
	static const UInt32 mk_MaxTreeChildren = 8;

	//Simple function definitions
	static CDynamicArray<GCAS::TMathFunction>	mk_MathFuncs;
	static void InitMathFunctions();

	typedef GCAS::CNode					ExpNode;
	typedef ExpNode*					ExpNodePtr;
	typedef CMathParseTree<ExpNode*>	TMathParseTree;			friend TMathParseTree;
	typedef TMathParseTree::Node		TreeNode;
	typedef TMathParseTree::Node*		TreeNodePtr;

	typedef CDynamicArray<TreeNodePtr>		TExpressionArray;
	typedef CDynamicArray<ExpNodePtr>		TVariableArray;
	typedef CDynamicArray<ExpNodePtr*>		TVariablePtrArray;

	typedef CDynamicArray<GCAS::COperatorBase* >	TOperatorArray;
	
	typedef CFunctor1<CMathParsingUnit, void, TreeNodePtr>		TComputationFunctor;
	typedef CFunctor1<CMathParsingUnit, void, TreeNodePtr>		TStringFunctor;
	typedef CFunctor1<CMathParsingUnit, void, Bool>				TParenthesisFunctor;
	
	typedef CFunctor1<CMathParsingUnit, Bool, TreeNodePtr>		TVarSearchFunctor;

	//////////////////////////////////////////////////////////////////////////////

	CMathParsingUnit(void);
	~CMathParsingUnit(void);

	void SetExpression(TreeNodePtr pNode);
	
	void SetEquation(const String& strEquation)		{	m_strEquation = strEquation; Parse();		}
	String GetEquation() const						{	return m_strEquation;						}

	Bool AddEquation(TreeNodePtr pExp);
	TreeNodePtr GetEquation(UInt32 index);

	Bool AddVariable(ExpNodePtr pVar, Bool bNoDupes = true);
	Bool RemoveVariable(ExpNodePtr pVar);
	Bool IsVariable(const String& exp)	const		
	{
		FOR_EACH(i, m_arrVariables)
		{
			if (m_arrVariables[i]->GetName() == exp)
				return true;
		}

		return false;
		//return (isalpha(exp[0]) || exp[0] == '_');
	}

	template <typename _Type>
	_Type EvaluateFor(ExpNodePtr pVar = NULL);

	TreeNodePtr GetCurrentExpression();
	
	Bool SolveFor(ExpNodePtr pVar);

	void GetSides(TStringArray& sides);

	void Evaluate(TreeNodePtr pNode);
	void Print(TreeNodePtr pNode);
	void PrintParenthesis(Bool bPreProc);

	String ToString(TreeNodePtr pNode = NULL);

	ExpNodePtr GetVariable(const String& name, Bool bCreateIfNotExist = false);
	ExpNodePtr GetVariable(UInt32 index);

	String BuildVariableString() const
	{
		String varString;
		for (UInt32 Vi = 0; Vi < m_arrVariables.Size(); Vi++)
			varString += m_arrVariables[Vi]->GetName();
		return varString;
	}

	//GCAS::COperatorBase* GetOperator

	///////////////////

	Bool IsFunctionSymbol(char c);

	Bool ContainsVar(TreeNodePtr searchRoot = NULL, ExpNodePtr pVar = NULL);

	Bool IsVarDefined(const String& name, UInt32* pIndex = NULL, Bool bCreateIfNot = false);
	
	Bool IsVar(TreeNodePtr pNode);

	template <typename _Functor>
	void ParseTree(_Functor& functor)
	{
		m_Ops.TraverseBreadthFirst<_Functor>(NULL, functor);
	}

	UInt32 NumVariables() const
	{
		return m_arrVariables.Size();
	}

	Bool ContainsFunctionSymbol(const String& exp, Int32* pPos = NULL)
	{
		Int32 parCtr = 0, pos = 0;
		String src;
		for (UInt32 i = 0; i < mk_MathFuncs.Size(); i++)
		{
			String symbol = GCAS::gk_FunctionSymbols[i];

			src = String(symbol);
			src += "()";

			if ((pos = exp.find_first_of(src)) != String::npos)
			{
				if (exp[pos] == '(')
					parCtr++;
				else if (exp[pos] == ')')
					parCtr--;
				else if (exp[pos] == symbol[0])
				{
					if (parCtr == 0)
					{
						if (pPos) *pPos = pos;
						return true;
					}
				}
			}
		}

		return false;
	}

	TVariableArray& GetIndependentVariables()
	{
		return m_arrIndependentVariables;
	}

	TVariableArray& GetDependentVariables()
	{
		return m_arrDependentVariables;
	}

	/*FVar* GetIndependantVariable(UInt32 index)
	{
		return &m_arrVariables[m_arrIndependentVariables[index]];
	}

	FVar* GetDependantVariable(UInt32 index)
	{
		return &m_arrVariables[m_arrDependentVariables[index]];
	}*/

	void CopyDataFromSolver(const CMathParsingUnit& solver)
	{
		m_arrVariables.CopyFrom(solver.m_arrVariables);
		m_arrOperators.CopyFrom(solver.m_arrOperators);
		m_arrEquationSet.CopyFrom(solver.m_arrEquationSet);

		SetVars();
	}


	//Kind of a hack for now
	void SetVars()
	{
		if (IsVarDefined("x"))
		{
			ExpNodePtr pX = GetVariable("x");

			if (IsVarDefined("t"))
			{
				ExpNodePtr pT = GetVariable("t");

				static_cast<GCAS::FVar*>(pT)->SetIndependence();
				static_cast<GCAS::FVar*>(pX)->SetIndependence(false);

				if (!m_arrIndependentVariables.Contains(pT))
					m_arrIndependentVariables.Push(pT);
				if (!m_arrDependentVariables.Contains(pX))
					m_arrDependentVariables.Push(pX);
			}
			else
			{
				static_cast<GCAS::FVar*>(pX)->SetIndependence();
				if (!m_arrIndependentVariables.Contains(pX))
					m_arrIndependentVariables.Push(pX);
			}
		}

		if (IsVarDefined("y"))
		{
			ExpNodePtr pY = GetVariable("y");

			if (IsVarDefined("t"))
			{
				ExpNodePtr pT = GetVariable("t");

				static_cast<GCAS::FVar*>(pT)->SetIndependence();
				static_cast<GCAS::FVar*>(pY)->SetIndependence(false);

				if (!m_arrIndependentVariables.Contains(pT))
					m_arrIndependentVariables.Push(pT);
				if (!m_arrDependentVariables.Contains(pY))
					m_arrDependentVariables.Push(pY);
			}
			else if (IsVarDefined("x"))
			{
				ExpNodePtr pX = GetVariable("x");

				static_cast<GCAS::FVar*>(pX)->SetIndependence();
				static_cast<GCAS::FVar*>(pY)->SetIndependence(false);

				if (!m_arrIndependentVariables.Contains(pX))
					m_arrIndependentVariables.Push(pX);
				if (!m_arrDependentVariables.Contains(pY))
					m_arrDependentVariables.Push(pY);
			}
			else
			{
				static_cast<GCAS::FVar*>(pY)->SetIndependence();
				if (!m_arrIndependentVariables.Contains(pY))
					m_arrIndependentVariables.Push(pY);
			}
		}

		if (IsVarDefined("z"))
		{
			ExpNodePtr pZ = GetVariable("z");

			if (IsVarDefined("t"))
			{
				ExpNodePtr pT = GetVariable("t");

				static_cast<GCAS::FVar*>(pT)->SetIndependence();
				static_cast<GCAS::FVar*>(pZ)->SetIndependence(false);

				if (!m_arrIndependentVariables.Contains(pT))
					m_arrIndependentVariables.Push(pT);
				if (!m_arrDependentVariables.Contains(pZ))
					m_arrDependentVariables.Push(pZ);
			}
			else
			{
				static_cast<GCAS::FVar*>(pZ)->SetIndependence();
				if (!m_arrIndependentVariables.Contains(pZ))
					m_arrIndependentVariables.Push(pZ);
			}
		}
	}

protected:
	void ThrowException(const String& str = "")
	{
		throw new CMathException(str);
	}

private:
	void Parse();
	//void Parse(TreeNodePtr pNode, const String& delim);
	Bool Parse(TreeNodePtr pNode, EOperatorPriority eOP);
	
	void ParseVector(TreeNodePtr& pNode);

	void ParseGroups(TreeNodePtr pNode);

public:
	Stack<ExpNodePtr, false>		mathStack;

	CCritSection		m_CS;

protected:
	Bool				m_bAllowMultiletterVars;

	CMathExpression*	m_pEquation;
	String				m_strEquation;

	TMathParseTree		m_Ops;
	
	CMathExpression*	m_Sides[2];
	TStringArray		m_strSides;

	TVariableArray		m_arrVariables;
	TVariableArray		m_arrIndependentVariables;
	TVariableArray		m_arrDependentVariables;

	TExpressionArray	m_arrEquationSet;

	TOperatorArray		m_arrOperators;

	/////
	String				m_searchVar;

private:
	GCAS::CVar<Vec3>*	m_pTempVec;
};
