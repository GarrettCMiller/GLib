#include "StdAfx.h"
#include "MathParsingUnit.h"

#include "GlobalFuncs.h"

using namespace GCAS;

//////////////////////////////////////////////////////////////////////////

#define MATH_OPERATOR(fnName, op)	CValueType* fnName(CValueType* lhs, CValueType* rhs)	\
									{	\
										switch (lhs->GetValueType())	\
										{	\
										case CNode::eVT_Scalar:	\
											return new FConst(static_cast<FVal*>(lhs)->GetValue() op static_cast<FVal*>(rhs)->GetValue());	\
										case CNode::eVT_Vector:	\
											return new VConst(static_cast<VVal*>(lhs)->GetValue() op static_cast<VVal*>(rhs)->GetValue());	\
										case CNode::eVT_Matrix:	\
											return new MConst(static_cast<MVal*>(lhs)->GetValue() op static_cast<MVal*>(rhs)->GetValue());	\
										default: ASSERT(0);	return NULL;	\
										}	\
									}

MATH_OPERATOR(Add, +)

MATH_OPERATOR(Subtract, -)

//MATH_OPERATOR(Multiply, *)
CValueType* Multiply(CValueType* lhs, CValueType* rhs)
{
	switch (lhs->GetValueType())
	{
	case CNode::eVT_Scalar:
		{
			switch (rhs->GetValueType())
			{
			case CNode::eVT_Scalar:
				return new FConst(static_cast<FVal*>(lhs)->GetValue() * static_cast<FVal*>(rhs)->GetValue());
			case CNode::eVT_Vector:
				return new VConst(static_cast<FVal*>(lhs)->GetValue() * static_cast<VVal*>(rhs)->GetValue());
			case CNode::eVT_Matrix:
				return new MConst(static_cast<FVal*>(lhs)->GetValue() * static_cast<MVal*>(rhs)->GetValue());
			default: ASSERT(0); return NULL;
			}
		}

	case CNode::eVT_Vector:
		{
			switch (rhs->GetValueType())
			{
			case CNode::eVT_Scalar:
				return new VConst(static_cast<D3DXVECTOR3>(static_cast<VVal*>(lhs)->GetValue()) * static_cast<FVal*>(rhs)->GetValue());
			case CNode::eVT_Vector: //We consider this the cross product for now
				return new VConst(static_cast<VVal*>(lhs)->GetValue().Cross(static_cast<VVal*>(rhs)->GetValue()));
			case CNode::eVT_Matrix:
				return new VConst(static_cast<VVal*>(lhs)->GetValue().Transform(&static_cast<MVal*>(rhs)->GetValue()));
			default: ASSERT(0); return NULL;
			}
		}

	case CNode::eVT_Matrix:
		{
			switch (rhs->GetValueType())
			{
			case CNode::eVT_Scalar:
				return new MConst(static_cast<MVal*>(lhs)->GetValue() * static_cast<FVal*>(rhs)->GetValue());
			case CNode::eVT_Vector: ASSERT(0); return NULL;
				//return new VConst(static_cast<MVal*>(lhs)->GetValue().Dot(static_cast<VVal*>(rhs)->GetValue()));
			case CNode::eVT_Matrix:
				return new MConst(static_cast<MVal*>(lhs)->GetValue() * static_cast<MVal*>(rhs)->GetValue());
			default: ASSERT(0); return NULL;
			}
		}

	default: ASSERT(0);	return NULL;
	}
}

//MATH_OPERATOR(Divide, /)
CValueType* Divide(CValueType* lhs, CValueType* rhs)
{
	switch (lhs->GetValueType())
	{
	case CNode::eVT_Scalar:
		{
			switch (rhs->GetValueType())
			{
			case CNode::eVT_Scalar:
				return new FConst(static_cast<FVal*>(lhs)->GetValue() / static_cast<FVal*>(rhs)->GetValue());
			case CNode::eVT_Vector:
				//return new VConst(static_cast<FVal*>(lhs)->GetValue() * static_cast<VVal*>(rhs)->GetValue());
			case CNode::eVT_Matrix:
				//return new MConst(static_cast<FVal*>(lhs)->GetValue() * static_cast<MVal*>(rhs)->GetValue());
			default: ASSERT(0); return NULL;
			}
		}

	case CNode::eVT_Vector:
		{
			switch (rhs->GetValueType())
			{
			case CNode::eVT_Scalar:
				return new VConst(static_cast<VVal*>(lhs)->GetValue() / static_cast<FVal*>(rhs)->GetValue());
			case CNode::eVT_Vector: //We consider this the dot product for now
				return new FConst(static_cast<VVal*>(lhs)->GetValue().Dot(static_cast<VVal*>(rhs)->GetValue()));
			case CNode::eVT_Matrix: //Cannot even think of any analogue to dividing a vector by a matrix
				//return new MConst(static_cast<FVal*>(lhs)->GetValue() * static_cast<MVal*>(rhs)->GetValue());
			default: ASSERT(0); return NULL;
			}
		}

	case CNode::eVT_Matrix:
		//return new MConst(static_cast<MVal*>(lhs)->GetValue() * static_cast<MVal*>(rhs)->GetValue());

	default: ASSERT(0);	return NULL;
	}
}

//MATH_OPERATOR(Assignment, =)
CValueType* Assignment(CValueType* lhs, CValueType* rhs)
{
	switch (lhs->GetValueType())
	{
	case CNode::eVT_Scalar:
		{
			switch (rhs->GetValueType())
			{
			case CNode::eVT_Scalar:
				return new FVar(lhs->GetName(), static_cast<FVal*>(rhs)->GetValue());
			case CNode::eVT_Vector:
			case CNode::eVT_Matrix:
			default: ASSERT(0); return NULL;
			}
		}

	case CNode::eVT_Vector:
		{
			switch (rhs->GetValueType())
			{
			case CNode::eVT_Vector:
				{
					if (rhs->GetType() == CNode::eNT_Var)
						static_cast<VVar*>(rhs)->UpdateValue();
					return new VVar(lhs->GetName(), static_cast<VVal*>(rhs)->GetValue());
				}
			case CNode::eVT_Scalar:
			case CNode::eVT_Matrix: 
				//return new MConst(static_cast<FVal*>(lhs)->GetValue() * static_cast<MVal*>(rhs)->GetValue());
			default: ASSERT(0); return NULL;
			}
		}

	case CNode::eVT_Matrix:
		return new MVar(lhs->GetName(), static_cast<MVal*>(rhs)->GetValue());

	default: ASSERT(0);	return NULL;
	}
}

CValueType* Power(CValueType* lhs, CValueType* rhs)
{
	ASSERT(lhs->GetValueType() == CNode::EValueType::eVT_Scalar);
	return new FConst(pow(static_cast<FVal*>(lhs)->GetValue(), static_cast<FVal*>(rhs)->GetValue()));
}

CValueType* Function(CValueType* lhs, CValueType* rhs)
{
	UInt32 func = 0;
	COperator::GetSymbolFromName(lhs->FromString(true), &func);
	return new FConst(CMathParsingUnit::mk_MathFuncs[func](static_cast<FVal*>(rhs)->GetValue()));
}

//////////////////////////////////////////////////////////////////////////

CDynamicArray<TMathFunction> CMathParsingUnit::mk_MathFuncs(eMF_NumMathFuncs);

//////////////////////////////////////////////////////////////////////////

CMathParsingUnit::CMathParsingUnit(void) : m_bAllowMultiletterVars(false)
{
	m_arrOperators.Push(new COperator(eOT_Assignment,	COperator::TBinaryFunctor(&Assignment),	eOF_Default));
	m_arrOperators.Push(new COperator(eOT_Add,			COperator::TBinaryFunctor(&Add),		eOF_Default /*+ eOF_Associative*/));
	m_arrOperators.Push(new COperator(eOT_Subtract,		COperator::TBinaryFunctor(&Subtract),	eOF_Default /*+ eOF_Associative*/));
	m_arrOperators.Push(new COperator(eOT_Multiply,		COperator::TBinaryFunctor(&Multiply),	eOF_Default /*+ eOF_Associative*/));
	m_arrOperators.Push(new COperator(eOT_Divide,		COperator::TBinaryFunctor(&Divide),		eOF_Default));
	m_arrOperators.Push(new COperator(eOT_Power,		COperator::TBinaryFunctor(&Power),		eOF_Default));
	
	m_arrOperators.Push(new COperator(eOT_Function,		COperator::TBinaryFunctor(&Function),	eOF_Unary));

	m_Ops.pParser = this;
}

CMathParsingUnit::~CMathParsingUnit(void)
{
	m_arrVariables.FreePointers();
}

//////////////////////////////////////////////////////////////////////////

void CMathParsingUnit::InitMathFunctions()
{
	mk_MathFuncs.Clear();

	mk_MathFuncs.Push(TMathFunction(&sinf));
	mk_MathFuncs.Push(TMathFunction(&cosf));
	mk_MathFuncs.Push(TMathFunction(&tanf));
	
	/*mk_MathFuncs.Push(TMathFunction(&cscf));
	mk_MathFuncs.Push(TMathFunction(&secf));
	mk_MathFuncs.Push(TMathFunction(&cotf));*/
	
	mk_MathFuncs.Push(TMathFunction(&asinf));
	mk_MathFuncs.Push(TMathFunction(&acosf));
	mk_MathFuncs.Push(TMathFunction(&atanf));

	mk_MathFuncs.Push(TMathFunction(&sinhf));
	mk_MathFuncs.Push(TMathFunction(&coshf));
	mk_MathFuncs.Push(TMathFunction(&tanhf));

	mk_MathFuncs.Push(TMathFunction(&expf));
	mk_MathFuncs.Push(TMathFunction(&logf));
	mk_MathFuncs.Push(TMathFunction(&log10f));
}

//////////////////////////////////////////////////////////////////////////

Bool CMathParsingUnit::RemoveVariable(ExpNodePtr pVar)
{
	ASSERT(pVar);

	return m_arrVariables.Remove(pVar);
}

//////////////////////////////////////////////////////////////////////////

Bool CMathParsingUnit::AddVariable(ExpNodePtr pVar, Bool bNoDupes /* = true */)
{
	ASSERT(pVar);

	UInt32 varIndex = 0;

	if (IsVarDefined(pVar->GetName(), &varIndex) == false)
		m_arrVariables.Push(pVar);
	else
	{
		if (bNoDupes)
			ThrowException("Tried to create existing variable!");	//Trying to resend a var as new
		else
		{
			switch (static_cast<FVal*>(pVar)->GetValueType())
			{
			case FVal::eVT_Scalar:
				static_cast<FVal*>(m_arrVariables[varIndex])->SetValue(static_cast<FVal*>(pVar)->GetValue());
				break;
			case FVal::eVT_Vector:
				static_cast<VVal*>(m_arrVariables[varIndex])->SetValue(static_cast<VVal*>(pVar)->GetValue());
				break;
			case FVal::eVT_Matrix:
				static_cast<MVal*>(m_arrVariables[varIndex])->SetValue(static_cast<MVal*>(pVar)->GetValue());
				break;
			case FVal::eVT_Tensor:
				ASSERT(0);
				break;
			}
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

CMathParsingUnit::ExpNodePtr CMathParsingUnit::GetVariable(const String& name, Bool bCreateIfNotExist /* = false */)
{
	FOR_EACH(i, m_arrVariables)
	{
		if (m_arrVariables[i]->GetName() == name)
		{
			return m_arrVariables[i];
		}
	}

	if (bCreateIfNotExist)
		AddVariable(new FVar(name, 0));
	else
		ThrowException("Tried to get non-existant variable!");

	return GetVariable(name);
}

//////////////////////////////////////////////////////////////////////////

Bool CMathParsingUnit::IsVarDefined(const String& name, UInt32* pIndex /* = NULL */, Bool bCreateIfNot /*= false*/)
{
	FOR_EACH(v, m_arrVariables)
	{
		if (m_arrVariables[v]->GetName() == name)
		{
			if (pIndex) *pIndex = v;
			return true;
		}
	}
	
	if (bCreateIfNot)
	{
		AddVariable(new FVar(name));
		return true;
	}

		return false;
}

//////////////////////////////////////////////////////////////////////////

CMathParsingUnit::TreeNodePtr CMathParsingUnit::GetCurrentExpression()
{
	return m_Ops.GetRoot();
}

//////////////////////////////////////////////////////////////////////////

CMathParsingUnit::ExpNodePtr CMathParsingUnit::GetVariable(UInt32 index)
{
	return m_arrVariables[index];
}

//////////////////////////////////////////////////////////////////////////

//Bool CMathParsingUnit::GetVariable(const String& name, const CVar<Float>& var) const
//{
//	for (UInt32 i = 0; i < m_arrVariables.Size(); i++)
//	{
//		if (m_arrVariables[i].GetName() == name)
//		{
//			var = m_arrVariables[i];
//			return true;
//		}
//	}
//
//	return false;
//}

//////////////////////////////////////////////////////////////////////////

void CMathParsingUnit::Parse()
{
	if (m_strEquation.empty())
		return;

	m_Ops.Clear();

	//parenthesis count
	UInt32	leftParCount		= 0,
			rightParCount		= 0,
			leftBracketCount	= 0,
			rightBracketCount	= 0,
			leftAngleCount		= 0,
			rightAngleCount		= 0;
	
	Int32	parCount			= 0,
			braCount			= 0,
			vecCount			= 0;

	for (UInt32 pl = 0; pl < m_strEquation.length(); pl++)
	{
		char letter = m_strEquation[pl];

		if (letter == '(')
		{
			//if (parCount <= 0)	ThrowException("Expecting \')\'!");

			leftParCount++;
			parCount++;
		}
		else if (letter == ')')
		{
			if (parCount <= 0)	ThrowException("Expecting \'(\'!");

			rightParCount++;
			parCount--;
		}
		else if (letter == '[')
		{
			leftBracketCount++;
			braCount++;
		}
		else if (letter == ']')
		{
			if (braCount <= 0)	ThrowException("Expecting \'[\'!");

			rightBracketCount++;
			braCount--;
		}
		else if (letter == '<')
		{
			leftAngleCount++;
			vecCount++;
		}
		else if (letter == '>')
		{
			if (vecCount <= 0)	ThrowException("Expecting \'<\'!");

			rightAngleCount++;
			vecCount--;
		}
	}

	if (parCount != 0)
	{
		String errString = "Incorrect number of \"( )\"\n\n\t";

		if (parCount > 0)
			errString += "Not enough \')\'.";
		else
			errString += "Not enough \')\'.";
		
		ThrowException(errString);
	}

	if (braCount != 0)
	{
		String errString = "Incorrect number of \"[ ]\"\n\n\t";

		if (braCount > 0)
			errString += "Not enough \']\'.";
		else
			errString += "Not enough \']\'.";
		
		ThrowException(errString);
	}

	if (vecCount != 0)
	{
		String errString = "Incorrect number of \"< >\"\n\n\t";

		if (vecCount > 0)
			errString += "Not enough \'>\'.";
		else
			errString += "Not enough \'<\'.";
		
		ThrowException(errString);
	}

	//Substitute symbols for known functions
	for (Int32 fn = mk_MathFuncs.Size()-1; fn >= 0; fn--)
	{
		String fnName = gk_FunctionNames[fn];
		Int32 fPos = m_strEquation.find(fnName);

		do
		{
			if (fPos != String::npos)
				m_strEquation.replace(fPos, fnName.length(), gk_FunctionSymbols[fn]);

			fPos = m_strEquation.find(fnName);
		} while (fPos != -1);
	}

	//Break the equation up by =
	TStringArray strSides = Tokenize(m_strEquation, "=");

	//We don't support multiple assignment/equalities ATM
	if (strSides.Size() > 2)
		return;

	if (strSides.Size() == 2)
	{
		m_strSides = strSides;

		TreeNodePtr	pLeft = NULL,
					pRight = NULL;
		
		m_Ops.Add(new COperator(m_arrOperators[0]), NULL);

		pLeft = m_Ops.Add(new CNode(strSides[0]), NULL);
		pRight = m_Ops.Add(new CNode(strSides[1]), NULL);

		for (UInt32 op = eOP_Last; op >= eOP_First; op--)
		{
			if (Parse(pLeft, EOperatorPriority(op)) == false)
				break;
		}

		for (UInt32 op = eOP_Last; op >= eOP_First; op--)
		{
			if (Parse(pRight, EOperatorPriority(op)) == false)
				break;
		}
	}
	else
	{
		m_strSides = strSides;

		TreeNodePtr	pExp = NULL;
		
		pExp = m_Ops.Add(new CNode(strSides[0]), NULL);

		for (UInt32 op = eOP_Last; op >= eOP_First; op--)
		{
			if (Parse(pExp, EOperatorPriority(op)) == false)
				break;
		}
	}

	SetVars();
}

//////////////////////////////////////////////////////////////////////////

void CMathParsingUnit::ParseVector(TreeNodePtr& pNode)
{
	TTokenArray tokens;

	//Get a string form of the expression
	String exp = pNode->item->FromString();

	tokens = Tokenize(exp, ",<>");
	
	if (tokens.Size() <= 1)
		ThrowException("Too few arguments! Need at least 2!");
	else if (tokens.Size() > 3)
		ThrowException("Too many arguments! Current max is 3!");

	delete pNode->item;

	Bool bVar = false;
	
	FOR_EACH(i, tokens)
	{
		if (IsAlpha(tokens[i]))
		{
			bVar = true;
			break;
		}
	}

	CValue<Vec3>* pVecNode = NULL;
	
	//TODO: DO TEMP VECTOR VAR CLASS FOR <x,y>
	if (bVar)
		pVecNode = m_pTempVec = new CVar<Vec3>(exp, true);
	else
		pVecNode = new CConst<Vec3>(exp);
	
	UInt32 index = -1;
	String val = tokens[0];

	if (IsAlpha(val))
	{
		if (IsVarDefined(val, &index))
			m_pTempVec->SetX(static_cast<FVar*>(m_arrVariables[index]));
		else
			AddVariable(new FVar(val, 0.0f));
	}
	else if (IsANumber(val))
		pVecNode->SetX(static_cast<Float>(atof(val.c_str())));
	else
		ThrowException("Invalid vector expression!");

	val = tokens[1];

	if (IsAlpha(val))
	{
		if (IsVarDefined(val, &index))
			m_pTempVec->SetY(static_cast<FVar*>(m_arrVariables[index]));
		else
			AddVariable(new FVar(val, 0.0f));
	}
	else if (IsANumber(val))
		pVecNode->SetY(static_cast<Float>(atof(val.c_str())));
	else
		ThrowException("Invalid vector expression!");

	if (tokens.Size() == 3)
	{
		val = tokens[2];

		if (IsAlpha(val))
		{
			if (IsVarDefined(val, &index))
				m_pTempVec->SetZ(static_cast<FVar*>(m_arrVariables[index]));
			else
				AddVariable(new FVar(val, 0.0f));
		}
		else if (IsANumber(val))
			pVecNode->SetZ(static_cast<Float>(atof(val.c_str())));
		else
			ThrowException("Invalid vector expression!");
	}

	pNode->item = pVecNode;
}

//////////////////////////////////////////////////////////////////////////

//void CMathParsingUnit::Parse(TreeNodePtr pNode, const String& delim)
Bool CMathParsingUnit::Parse(TreeNodePtr pNode, EOperatorPriority eOP)
{
	TTokenArray strSides(2);
	TTokenArray tokens;

	Bool bNegate = false;
	Bool bImplicitMult = false;

	//Get a string form of the expression
	String exp = pNode->item->FromString();

	//How stupid, I had no check for empty strings -- for how long? A couple years at least
	if (exp.empty())
		return false;

	//Do we have a constant number?
	if (IsANumber(exp))
	{
		delete pNode->item;
		pNode->item = new FConst(StringToFloat(exp));
		return false;
	}

	//Do we have a variable?
	if (IsVariable(exp) || IsAlpha(exp))
	{
		if (exp.length() == 1 || m_bAllowMultiletterVars)
		{
			CNode* pVarNode = NULL;
			
			if (isupper(exp[0]))
				pVarNode = new VVar(exp, false);
			else
				pVarNode = new FVar(exp, 0.0f);
			
			delete pNode->item;

			UInt32 varIndex = 0;

			if (IsVarDefined(pVarNode->GetName(), &varIndex) == false)
			{
				pNode->item = pVarNode;
				AddVariable(pVarNode);
			}
			else
			{
				pNode->item = m_arrVariables[varIndex];
			}

			return false;
		}
	}

	Int32 firstOpPos = -1;
	Int32 firstParPos = exp.find_first_of("()");
	Int32 opIndex = -1;

	//Extract our operators from the list
	String delim = k_strOperators[eOP];

	if (eOP == eOP_Function)
	{
		String	fnString;

		for (UInt32 i = 0; i < mk_MathFuncs.Size(); i++)
		{
			fnString += gk_FunctionSymbols[i];
		}

		delim = fnString;
	}

	//Remove out parenthesis if we have a single top level group
	//i.e. (3x+5) --> 3x+5
	tokens = TokenizeGroups(exp, "");

	//We might have a vector on our hands
	if (exp[0] == '<')
	{
		//TODO: Make sure this is a vector only and not a vector equation
		//Int32 closingPos = exp.find_first_of(">");

		ParseVector(pNode);

		return false;
	}

	//If it's addition or subtraction, it's associative and scan from the back
	if (eOP != eOP_Add && eOP != eOP_Exponents)
		firstOpPos = exp.find_first_of(delim);
	else
		firstOpPos = exp.find_last_of(delim);

	//Are we looking for implicit multiplication
	if (eOP == eOP_Multiply && firstParPos > 0 && firstOpPos > firstParPos)
	{
		String e = exp.substr(0, firstParPos);
		if (IsVariable(e) || IsANumber(e))
		{
			bImplicitMult = true;
			//firstOpPos = exp.find_last_of(delim);
			firstOpPos = firstParPos;
		}
		else
			ThrowException("Umm, I'm not sure!");
	}

	//if we don't find the operand we are looking for
	if (firstOpPos == String::npos 
		|| firstOpPos == 0	//account for negative numbers that look "negated"
		)
	{
		/*if (ContainsFunctionSymbol(exp, &firstOpPos))
		{
			if (exp.length() > 1 && firstOpPos == 0)
			{
				eOP = eOP_Function;

				firstOpPos = 1;

				goto skipTokenize;
			}
			else
			{
				return true;
			}
		}
		else*/ if (IsAlpha(exp))	//We have implicit multiplication between variables
		{
			if (exp.find_first_of(k_strAllOps) != String::npos)
				return true;
			
			bImplicitMult = true;

			firstOpPos = 1;
			opIndex = 0;
			eOP = eOP_Multiply;

			goto skipTokenize;
		}
		else //check implicit multiplication and negative vars
		{
			tokens = Tokenize(exp, "(");

			if (tokens.Size() >= 2 && IsFunctionSymbol(exp[0]) && exp[1] == '(')
				return true;

			String	varString, varParString;
			Bool bFuncSymbol = false;
			Bool bFailed = false;

			varString = BuildVariableString();

checkforvarsagain:

			varParString = varString + "(";

			tokens = Tokenize(exp, varParString);
			//tokens = TokenizeGroups(exp, "x(");

			if (tokens.Size() > 2)
				tokens = Tokenize(exp, "(");
			
			if (tokens.Size() <= 2)
			{
				if (ContainsFunctionSymbol(exp, &firstOpPos))
				{
					bFuncSymbol = true;

					if (eOP == eOP_Function)
					{
						if (exp.length() > 1 && firstOpPos == 0)
						{
							eOP = eOP_Function;

							firstOpPos = 1;

							goto skipTokenize;
						}
					}
					else if (eOP != eOP_Multiply)
						return true;
					else goto implicitFuncMult;
				}

				/*if (tokens[0].find_first_of(k_strAllOps) != String::npos)
				{
					if (tokens[0].find_first_of(delim) == String::npos)
						return true;
				}*/

				if (tokens.Size() == 1)
				{
					if (tokens[0] == exp && !bFailed)
					{
						varString = g_strLetters;

						bFailed = true;

						goto checkforvarsagain;
					}

					//check for negation
					if (tokens[0] == "-")
					{
						bNegate = true;

						goto skipTokenize;
					}
				}
			}

implicitFuncMult:
			// ???
			/*if (!IsVariable(tokens[0]) && !IsAlNum(tokens[0]) && tokens.Size() > 1)
				return true;*/

			if (bFuncSymbol)
			{
				//Determine if there is indeed multiplication OUTSIDE of the function argument
				if (firstOpPos == 0 && exp.length() >= 2
					&& GetGroupCount(exp) <= 1
					//&& exp.find_first_of(k_strAllOps, firstOpPos + 1) == String::npos
					)
					return true;

				firstOpPos = exp.find_last_of(exp[firstOpPos]);

				/*opIndex = 0;
				eOP = eOP_Multiply;*/
			}
			else
			{
				//We won't jump right to multiply now
				if (eOP > eOP_Multiply)
					return true;

				firstOpPos = exp.find_first_of("(");

				if (firstOpPos == 0)
				{
					tokens = Tokenize(exp, "()");

					if (tokens.Size() > 1)
					{
						//first check for other operators
						for (UInt32 o = 0; o < 5; o++)
						{
							if (tokens[1][0] == k_strOperators[o][0])
								return true;
						}
					}
					else
						firstOpPos = tokens[0].size() + 2;
				}
				else if (firstOpPos == String::npos)
				{
					firstOpPos = exp.find_first_of(varString);
				
					//check for multiple implicit multiplication
					UInt32 numMissing = exp.length() - tokens[0].length();
					UInt32 parOffset = 0;

					while ((parOffset = exp.find_first_of("(", ++parOffset)) != String::npos)
						numMissing--;

					if (numMissing == 1)
					{
						//first check for other operators
						for (UInt32 o = 0; o < 5; o++)
						{
							if (tokens[0][0] == k_strOperators[o][0])
								return true;
						}
					}
				}

				/*opIndex = 0;
				eOP = eOP_Multiply;*/
			}

			opIndex = 0;
			eOP = eOP_Multiply;

			if (exp.find_first_of('*', firstOpPos) == String::npos)
				bImplicitMult = true;

			goto skipTokenize;
		}

		return true;
	}
	else
	{
		if (bImplicitMult)
		{
			opIndex = 0;
		}
		else
		{
			char cOp[2] = {'\0'};
			cOp[0] = exp[firstOpPos];
			String tempOp = String(cOp);
			opIndex = delim.find_first_of(tempOp);
		}
	}

	UInt32 len = exp.length();

	tokens = TokenizeGroups(exp, delim);

	if (tokens.Size() == 1 /*&& !(eOP == eOP_Add && opIndex == 1)*/)
		return true;

	if (exp.length() < len)
		firstOpPos--;

skipTokenize:

	delete pNode->item;

	EOperatorType opType;
	UInt32 opFlags;

	opFlags = eOF_Binary;

	switch (eOP)
	{
	case eOP_Add:
		ASSERT(opIndex >= 0);
		pNode->item = new COperator(m_arrOperators[1 + opIndex]);
		break;

	case eOP_Multiply:
		ASSERT(opIndex >= 0);
		pNode->item = new COperator(m_arrOperators[3 + opIndex]);
		break;

	case eOP_Exponents:
		ASSERT(opIndex >= 0);
		pNode->item = new COperator(m_arrOperators[5]);
		break;

	case eOP_Function:
		opFlags = eOF_Unary;
		pNode->item = new COperator(m_arrOperators[6]);
		static_cast<COperator*>(pNode->item)->SetString(COperator::GetNameFromSymbol(exp.substr(0, firstOpPos)));
	}

	if (bNegate)
	{
		delete pNode->item;
		opFlags = eOF_Unary;
		pNode->item = new COperator(m_arrOperators[2]);

		firstOpPos = 0;

		tokens.Resize(2);
	}

resplit:
	ASSERT(firstOpPos < static_cast<Int32>(exp.length()));
	tokens[0] = exp.substr(0, firstOpPos);
	tokens[1] = exp.substr(firstOpPos + 1);

	//Correct for missing var from tokenizing
	if (bImplicitMult || tokens[1].empty())
		tokens[1] = exp.substr(firstOpPos);

	if (tokens[0].empty() && tokens[1] == exp)
	{
		firstOpPos = 1;
		goto resplit;
	}

	if (eOP == eOP_Function)
	{
		tokens[0] = exp.substr(firstOpPos);
		tokens[1] = exp.substr(0, firstOpPos);
	}

	if (bNegate)
	{
		tokens[0] = tokens[1];
	}

	strSides[0] = tokens[0];
	strSides[1] = tokens[1];

	TreeNodePtr	pLeft = NULL,
				pRight = NULL;
	
	pLeft = m_Ops.Add(new CNode(strSides[0]), pNode);

	if (opFlags == eOF_Binary)
		pRight = m_Ops.Add(new CNode(strSides[1]), pNode);

	UInt32 nDone = 0;

	for (UInt32 op = eOP_Last; op >= eOP_First; op--)
	{
		if (Parse(pLeft, EOperatorPriority(op)) == false)
		{
			nDone++;
			break;
		}
	}

	if (opFlags == eOF_Binary)
	{
		for (UInt32 op = eOP_Last; op >= eOP_First; op--)
		{
			if (Parse(pRight, EOperatorPriority(op)) == false)
			{
				nDone++;
				break;
			}
		}
	}
	
	return (nDone < (opFlags == eOF_Binary ? 2u : 1u));
}

//////////////////////////////////////////////////////////////////////////

void CMathParsingUnit::ParseGroups(TreeNodePtr pNode)
{
	//TODO
}

//////////////////////////////////////////////////////////////////////////

void CMathParsingUnit::GetSides(TStringArray &sides)
{
	sides = m_strSides;
}

//////////////////////////////////////////////////////////////////////////

template <typename _Type>
_Type CMathParsingUnit::EvaluateFor(ExpNodePtr pVar /* = NULL */)
{
	m_Ops.TraverseDepthFirst<TComputationFunctor>(NULL, TComputationFunctor(this, &CMathParsingUnit::Evaluate));

	//Remove final answer
	return mathStack.Pop();
}

//////////////////////////////////////////////////////////////////////////

template <>
CMathParsingUnit::ExpNodePtr CMathParsingUnit::EvaluateFor<CMathParsingUnit::ExpNodePtr>(ExpNodePtr pVar /* = NULL */)
{
	if (m_Ops.IsEmpty())
	{
		ThrowException("No expression in memory!\n\nYou must first parse an expression to evaluate it!");
		return NULL;
	}

	m_Ops.TraverseDepthFirst<TComputationFunctor>(NULL, TComputationFunctor(this, &CMathParsingUnit::Evaluate));

	//Remove final answer
	return mathStack.Pop();
}

//////////////////////////////////////////////////////////////////////////

template <>
Float CMathParsingUnit::EvaluateFor<Float>(ExpNodePtr pVar /* = NULL */)
{
	m_Ops.TraverseDepthFirst<TComputationFunctor>(NULL, TComputationFunctor(this, &CMathParsingUnit::Evaluate));

	ExpNodePtr pResult = mathStack.Pop();

	Float fResult = static_cast<FVal*>(pResult)->GetValue();

	delete pResult;
	return fResult;
}

//////////////////////////////////////////////////////////////////////////

template <>
Vec3 CMathParsingUnit::EvaluateFor<Vec3>(ExpNodePtr pVar /* = NULL */)
{
	m_Ops.TraverseDepthFirst<TComputationFunctor>(NULL, TComputationFunctor(this, &CMathParsingUnit::Evaluate));

	ExpNodePtr pResult = mathStack.Pop();

	Vec3 fResult = static_cast<VVal*>(pResult)->GetValue();

	delete pResult;
	return fResult;
}

//////////////////////////////////////////////////////////////////////////

Bool CMathParsingUnit::SolveFor(ExpNodePtr pVar)
{
	return m_Ops.SolveFor(pVar);
}

//////////////////////////////////////////////////////////////////////////

Bool CMathParsingUnit::ContainsVar(TreeNodePtr searchRoot /* = NULL */, ExpNodePtr pVar /* = NULL */)
{
	Bool retVal = false;

	if (pVar)
		m_searchVar = pVar->GetName();
	else
		m_searchVar = "";

	retVal = m_Ops.TraverseBreadthFirst<TVarSearchFunctor>(searchRoot, TVarSearchFunctor(this, &CMathParsingUnit::IsVar));

	return retVal;
}

//////////////////////////////////////////////////////////////////////////

Bool CMathParsingUnit::IsVar(TreeNodePtr pNode)
{
	ASSERT(pNode);
	
	if (!pNode)
		ThrowException();

	if (pNode->item->GetType() == GCAS::CNode::eNT_Var)
	{
		//If we are just searching for any variables
		if (m_searchVar.empty())
			return true;

		//Or compare the names if we are
		if (static_cast<GCAS::CVar<Float>*>(pNode->item)->GetName() == m_searchVar)
			return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////

void CMathParsingUnit::Evaluate(TreeNodePtr pNode)
{
	CValueType* pResult = NULL;

	switch (pNode->item->GetType())
	{
	case CNode::eNT_Operator:
		{
			COperator* pOp = static_cast<COperator*>(pNode->item);

			ExpNodePtr val1	= NULL;
			ExpNodePtr val2 = NULL;
			ExpNodePtr newVal = NULL;

			if (pOp->GetFlags().TestFlag(eOF_Binary))
			{
				if (mathStack.Size() < 2)
				{
					if (pOp->GetOpType() == eOT_Subtract)
					{
						val2 = mathStack.Pop();
						val1 = new CConst<Float>(0.0f);

						pResult = (*pOp)(static_cast<CValueType*>(val1), static_cast<CValueType*>(val2));

						goto stepDone;
					}
					else
						ThrowException("Not enough objects for binary operation!");
				}

				val2 = mathStack.Pop();
				val1 = mathStack.Pop();

				pResult = (*pOp)(static_cast<CValueType*>(val1), static_cast<CValueType*>(val2));
			}
			else if (pOp->GetFlags().TestFlag(eOF_Unary))
			{
				if (mathStack.Size() < 1)
				{
					ThrowException("Not enough objects for unary operation!");
				}

				val1 = mathStack.Pop();

				pResult = (*pOp)(&CValueType(pOp->FromString()), static_cast<CValueType*>(val1));
			}

			if (pOp->GetOpType() == eOT_Assignment)
				AddVariable(pResult, false);

stepDone:
			delete val1;
			delete val2;

			mathStack.Push(pResult);
		}
		break;

	case CNode::eNT_Function:
		ThrowException("Tried to call a function!");
		break;

	case CNode::eNT_Value:
		ThrowException("Tried to do something!");
		break;

	case CNode::eNT_Var:
		{
			ExpNodePtr pNewNode = NULL;

			switch (static_cast<CValueType*>(pNode->item)->GetValueType())
			{
			case CNode::eVT_Scalar:
				{
					FVar* tempVar = static_cast<FVar*>(GetVariable(pNode->item->GetName()));
					ASSERT(tempVar);
					pNewNode = new FVar(*tempVar);
				}
				break;

			case CNode::eVT_Vector:
				{
					VVar* pVec		= static_cast<VVar*>(pNode->item);
					VVar* tempVar	= NULL;
					
					if (pVec->IsTempVec())
						tempVar = pVec;
					else
						tempVar = static_cast<VVar*>(GetVariable(pVec->GetName()));

					ASSERT(tempVar);
					pNewNode = new VVar(*tempVar);
				}
				break;

			case CNode::eVT_Matrix:
				{
					MVar* tempVar = static_cast<MVar*>(GetVariable(pNode->item->GetName()));
					ASSERT(tempVar);
					pNewNode = new MVar(*tempVar);
				}
				break;

			default:
				ASSERT(0); //For now
			}

			mathStack.Push(pNewNode);
		}
		break;

	case CNode::eNT_Const:
		{
			ExpNodePtr pNewNode = NULL;

			switch (static_cast<FVal*>(pNode->item)->GetValueType())
			{
			case CNode::eVT_Scalar:	pNewNode = new FConst(static_cast<FVal*>(pNode->item)->GetValue());	break;
			case CNode::eVT_Vector:	pNewNode = new VConst(static_cast<VVal*>(pNode->item)->GetValue());	break;
			case CNode::eVT_Matrix:	pNewNode = new MConst(static_cast<MVal*>(pNode->item)->GetValue());	break;
			default:			ASSERT(0); //For now
			}

			mathStack.Push(pNewNode);
		}
		break;
	}
}

//////////////////////////////////////////////////////////////////////////

String CMathParsingUnit::ToString(TreeNodePtr pNode /* = NULL */)
{
	String tempEq = m_strEquation;

	m_strEquation = "";

	m_Ops.TraverseForString<TStringFunctor, TParenthesisFunctor>(pNode,
							TStringFunctor(this, &CMathParsingUnit::Print),
							TParenthesisFunctor(this, &CMathParsingUnit::PrintParenthesis));

	Swap(m_strEquation, tempEq);

	return tempEq;
}

//////////////////////////////////////////////////////////////////////////

void CMathParsingUnit::PrintParenthesis(Bool bPreProc)
{
	m_strEquation += bPreProc ? "(" : ")";
}

//////////////////////////////////////////////////////////////////////////

void CMathParsingUnit::Print(TreeNodePtr pNode)
{
	if (pNode->item->GetType() == CNode::eNT_Operator)
	{
		COperator* pOp = static_cast<COperator*>(pNode->item);

		if (pOp->GetOpType() == eOT_Multiply /*|| pOp->GetOpType() == eOT_Divide*/)
		{
			CNode::ENodeType	eType1 = pNode->child[0]->item->GetType(),
								eType2 = pNode->child[1]->item->GetType();
			if (eType1 == eType2)
			{
				switch (eType1)
				{
				case CNode::eNT_Operator:
					{
						COperator* pOp1 = static_cast<COperator*>(pNode->child[0]->item);
						COperator* pOp2 = static_cast<COperator*>(pNode->child[1]->item);

						if (pOp1->GetOpType() == eOT_Add || pOp1->GetOpType() == eOT_Subtract ||
							pOp2->GetOpType() == eOT_Add || pOp2->GetOpType() == eOT_Subtract)
							return;
					} break;
				case CNode::eNT_Var:
				case CNode::eNT_Const:
				default:
					break;
				}
				//== eNT_Operator
			}
			else
				return;
		}
	}

	if (pNode->item->GetType() == CNode::eNT_Var
		&& static_cast<FVar*>(pNode->item)->GetValueType() == FVar::eVT_Scalar)
		m_strEquation += pNode->item->GetName();
	else
		m_strEquation += pNode->item->FromString(true);
}

//////////////////////////////////////////////////////////////////////////

void CMathParsingUnit::SetExpression(CMathParsingUnit::TreeNodePtr pNode)
{
	//m_Ops.CopyFromNode(pNode, true);
	m_Ops.CopyFromNode(pNode);
}

//////////////////////////////////////////////////////////////////////////

Bool CMathParsingUnit::AddEquation(CMathParsingUnit::TreeNodePtr pExp)
{
	ASSERT(pExp);

	m_arrEquationSet.Push(pExp);

	return true;
}

//////////////////////////////////////////////////////////////////////////

CMathParsingUnit::TreeNodePtr CMathParsingUnit::GetEquation(UInt32 index)
{
	return m_arrEquationSet[index];
}

//////////////////////////////////////////////////////////////////////////

Bool CMathParsingUnit::IsFunctionSymbol(char c)
{
	FOR_EACH(i, mk_MathFuncs)
		if (c == gk_FunctionSymbols[i][0])
			return true;
	return false;
}

//////////////////////////////////////////////////////////////////////////