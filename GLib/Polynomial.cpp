#include "StdAfx.h"
#include "Polynomial.h"

#include "GlobalFuncs.h"

#include <iostream>
using namespace std;

CDynamicArray<GCAS::TMathFunction> MathTermVar::mk_MathFuncs(GCAS::eMF_NumMathFuncs);

//////////////////////////////////////////////////////////////////////////

void MathTermVar::InitMathFuncs()
{
	mk_MathFuncs.Push(GCAS::TMathFunction(&sinf));
	mk_MathFuncs.Push(GCAS::TMathFunction(&cosf));
	mk_MathFuncs.Push(GCAS::TMathFunction(&tanf));
	
	/*mk_MathFuncs.Push(GCAS::TMathFunction(&cscf));
	mk_MathFuncs.Push(GCAS::TMathFunction(&secf));
	mk_MathFuncs.Push(GCAS::TMathFunction(&cotf));*/
	
	mk_MathFuncs.Push(GCAS::TMathFunction(&asinf));
	mk_MathFuncs.Push(GCAS::TMathFunction(&acosf));
	mk_MathFuncs.Push(GCAS::TMathFunction(&atanf));

	mk_MathFuncs.Push(GCAS::TMathFunction(&sinhf));
	mk_MathFuncs.Push(GCAS::TMathFunction(&coshf));
	mk_MathFuncs.Push(GCAS::TMathFunction(&tanhf));

	mk_MathFuncs.Push(GCAS::TMathFunction(&expf));
	mk_MathFuncs.Push(GCAS::TMathFunction(&logf));
	mk_MathFuncs.Push(GCAS::TMathFunction(&log10f));
}

//////////////////////////////////////////////////////////////////////////

void CMathExpression::FromString(const String &expression)
{
	String e = expression;

	TSortedTokenArray groups = TokenizeSort(e, "+-");

	UInt32 numGroups = groups.Size();
	UInt32 numParts = groups[0].Size();

	if (numGroups == 1 && numParts == 1)	//We didnt find any ops
		return;

	m_Terms.Clear();

	TStringArray tokens;

	UInt32 uC = 1, uE = 1;

	for (UInt32 op = 0; op < 2; op++)
	{
		if (groups[op].IsEmpty())
			continue;

		for (UInt32 i = 0; i < numParts; i++)
		{
			if (groups[op][i] == "")
				continue;

			tokens = Tokenize(groups[op][i], "x^");

			if (tokens.IsEmpty())
				continue;

			if (tokens.Size() == 2)
			{
				uC = atoi(tokens[0].c_str()) * (op == 0 ? 1 : -1);
				uE = atoi(tokens[1].c_str());
			}
			else if (tokens[0].find_first_of("^") == String::npos)
			{
				uC = atoi(tokens[0].c_str()) * (op == 0 ? 1 : -1);
				uE = 1;
			}
			else
			{
				uC = (op == 0 ? 1 : -1);
				uE = atoi(tokens[0].c_str());
			}

			if (groups[op][i].find_first_of("x") == String::npos)
				uE = 0;

			AddTerm(MathTermVar(uC, uE));
		}
	}

	SortByExp();
}

//////////////////////////////////////////////////////////////////////////

//CPolynomial::CPolynomial(void)
//{
//}

CPolynomial::~CPolynomial(void)
{
}