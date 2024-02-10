#pragma once

#include "DynamicArray.h"
#include "Functor.h"

#include <map>

#include "ExpObj.h"

const String k_strAllOps = "+-*/^()=";

//////////////////////////////////////////////////////////////////////////

struct MathTermVar
{
	/*explicit MathTermVar(Float c, Float e, const String& name) : C(c), E(e), bFC(true), bFE(true), Name(name)
	{
	}*/
	static CDynamicArray<GCAS::TMathFunction>		mk_MathFuncs;
	static void InitMathFuncs();

	explicit MathTermVar(Int32 upperC = 1, Int32 upperE = 1, const String& name = "x", Int32 lowerC = 1, Int32 lowerE = 1) : 
		Cupper(upperC), Clower(lowerC), Eupper(upperE), Elower(lowerE), bFC(false), bFE(false), /*Name(name),*/ F(GCAS::eMF_None)
	{
		name;
		//ASSERT(lowerC != 0);
		//ASSERT(lowerE != 0);
	}

	explicit MathTermVar(GCAS::EMathFunctions fx, Int32 upperC = 1, Int32 upperE = 1, const String& name = "x", Int32 lowerC = 1, Int32 lowerE = 1) : 
		F(fx), Cupper(upperC), Clower(lowerC), Eupper(upperE), Elower(lowerE), bFC(false), bFE(false)/*, Name(name)*/
	{
		name;
		//ASSERT(lowerC != 0);
		//ASSERT(lowerE != 0);
	}

	Bool	bFC;
	Bool	bFE;

	/*union
	{
		Float	C;*/

		struct
		{
			Int32	Cupper;
			Int32	Clower;
		};
	//};

	/*union
	{
		Float	E;*/

		struct
		{
			Int32	Eupper;
			Int32	Elower;
		};
	//};

	GCAS::EMathFunctions	F;

	//String	Name;

	/////////////////////////////////////////////////

	Bool operator == (const MathTermVar& rhs) const
	{
		return (	
					Cupper	== rhs.Cupper	&&
					Clower	== rhs.Clower	&&
					Eupper	== rhs.Eupper	&&
					Elower	== rhs.Elower	&&
					F		== rhs.F
				);
	}

	Bool operator != (const MathTermVar& rhs) const
	{
		return !((*this) == rhs);
	}

	String ToString(Bool bPrintSign) const
	{
		String me;
		char tbuf[128];

		if (Cupper == 0)
		{
			if (Clower == 0)
			{
				me += "+";
				me += "x"/*Name*/;
			}
			
			return me;
		}

		if (bPrintSign)
		{
			if (Cupper > 0)
				me += "+";
			//else if (term.Cupper == -1)
			//	os << '-';
		}

		if (Cupper != 1 || Clower != 1)
		{
			me += _itoa(Cupper, tbuf, 10);

			if (Clower != 1)
			{
				me += "/";
				me += _itoa(Clower, tbuf, 10);
			}
		}

		if (F != GCAS::eMF_None)
		{
			me += GCAS::gk_FunctionNames[F];
			me += "(";
		}

		if (Eupper != 0)
		{
			me += "x";//Name;

			if (F == GCAS::eMF_None && 
				(Eupper != 1 || Elower != 1))
			{
				me += "^";
				me += _itoa(Eupper, tbuf, 10);

				if (Elower != 1)
				{
					me += "/";
					me += _itoa(Elower, tbuf, 10);
				}
			}
		}
		else if (Cupper == 1)
		{
			me += _itoa(Cupper, tbuf, 10);
		}

		if (F != GCAS::eMF_None)
		{
			me += ")";

			if (Eupper != 1 || Elower != 1)
			{
				me += "^";
				me += _itoa(Eupper, tbuf, 10);

				if (Elower != 1)
				{
					me += "/";
					me += _itoa(Elower, tbuf, 10);
				}
			}
		}

		return me;
	}

	/////////////////////////////////////////////////

	Float Evaluate(Float value) const
	{
		Float retVal = 0.0f;

		if (Elower != 0 && Clower != 0)
		{
			Float fE = Float(Eupper) / Float(Elower);

			if (F != GCAS::eMF_None)
			{
				value = mk_MathFuncs[F](value);
			}

			retVal = pow(value, fE);

			Float fC = Float(Cupper) / Float(Clower);

			retVal *= fC;
		}

		return retVal;
	}

	void Differentiate()
	{
		if (bFE)
		{
		}
		else
		{
			//if the exp is not 0
			if (Eupper != 0)
			{
				//Multiply the exp into the coeff
				Cupper *= Eupper;

				//Take the exp down by one
				Eupper--;
			}
			else
			{
				Cupper = 0;
			}
		}
	}

	void Integrate()
	{
		if (bFE)
		{
		}
		else
		{
			//if the exp is not 1 or 0
			if (Eupper != -1)
			{
				//Take the exp up by one
				Eupper++;

				//Divide the exp outta the coeff
				Cupper /= Eupper;
			}
			else
			{
				Cupper = 0;
				Clower = 0;
			}
		}
	}
};

typedef CDynamicArray<MathTermVar>		TMathTermArray;

class CMathFactor
{
	TMathTermArray	m_Factors;

	UInt32 NumFactors() const
	{
		return m_Factors.Size();
	}

	Float Evaluate(Float value) const
	{
		Float prod = 1.0f;

		for (UInt32 factor = 0; factor < NumFactors(); factor++)
		{
			prod *= m_Factors[factor].Evaluate(value);
		}

		return prod;
	}
};

class CMathExpression
{
public:
	CMathExpression(const String& name = "", UInt32 maxSize = 10)
	{
		if (name != "")
			m_Name = name;

		m_Terms.Resize(maxSize);
	}

	void FromString(const String& expression);

	String ToString() const
	{
		String me = GetName();
		
		me += "(";
		me += "x";//m_Terms[0].Name;
		me += ")=";

		for (UInt32 t = 0; t < NumTerms(); t++)
		{
			me += m_Terms[t].ToString(t != 0);
		}

		return me;
	}

	Float Evaluate(Float value) const
	{
		Float sum = 0.0f;

		for (UInt32 term = 0; term < NumTerms(); term++)
		{
			sum += m_Terms[term].Evaluate(value);
		}

		return sum;
	}

	virtual void Differentiate()
	{	ASSERT(0);	}

	virtual void Integrate()
	{	ASSERT(0);	}

	//////////////////////////////////

	void operator += (const MathTermVar& term)
	{
		Bool bTermFound = false;

		for (UInt32 t = 0; t < NumTerms(); t++)
		{
			if (term.Eupper == m_Terms[t].Eupper)
			{
				m_Terms[t].Cupper += term.Cupper;
				bTermFound = true;
				break;
			}
		}

		if (!bTermFound)
			AddTerm(term);
	}

	void operator += (const CMathExpression& poly)
	{
		Bool bTermFound = false;

		for (UInt32 t = 0; t < NumTerms(); t++)
		{
			bTermFound = false;

			for (UInt32 p = 0; p < poly.NumTerms(); p++)
			{
				if (poly[p].Eupper == m_Terms[t].Eupper)
				{
					m_Terms[t].Cupper += poly[p].Cupper;
					bTermFound = true;
					break;
				}
			}

			//TODO: FIX, ADDING WRONG TERM OR SOMETHING
			/*if (!bTermFound)
				AddTerm(poly[poly.NumTerms()-1]);*/
		}
	}

	CMathExpression operator + (const CMathExpression& rhs)
	{
		CMathExpression result(*this);
		Bool bTermFound = false;

		for (UInt32 t = 0; t < result.NumTerms(); t++)
		{
			bTermFound = false;

			for (UInt32 p = 0; p < rhs.NumTerms(); p++)
			{
				if (rhs[p].Eupper == result.m_Terms[t].Eupper)
				{
					result.m_Terms[t].Cupper += rhs[p].Cupper;
					bTermFound = true;
					break;
				}
			}

			/*if (!bTermFound)
				result.AddTerm(rhs[p]);*/
		}

		return result;
	}

	void operator -= (const MathTermVar& term)
	{
		Bool bTermFound = false;

		for (UInt32 t = 0; t < NumTerms(); t++)
		{
			if (term.Eupper == m_Terms[t].Eupper)
			{
				m_Terms[t].Cupper -= term.Cupper;
				bTermFound = true;
				break;
			}
		}

		if (!bTermFound)
			AddTerm(term);
	}

	void operator -= (const CMathExpression& poly)
	{
		Bool bTermFound = false;

		for (UInt32 t = 0; t < NumTerms(); t++)
		{
			bTermFound = false;

			for (UInt32 p = 0; p < poly.NumTerms(); p++)
			{
				if (poly[p].Eupper == m_Terms[t].Eupper)
				{
					m_Terms[t].Cupper -= poly[p].Cupper;
					bTermFound = true;
					break;
				}
			}

			/*if (!bTermFound)
				AddTerm(poly[p]);*/
		}
	}

	CMathExpression operator - (const CMathExpression& rhs)
	{
		CMathExpression result(*this);
		Bool bTermFound = false;

		for (UInt32 t = 0; t < result.NumTerms(); t++)
		{
			bTermFound = false;

			for (UInt32 p = 0; p < rhs.NumTerms(); p++)
			{
				if (rhs[p].Eupper == result.m_Terms[t].Eupper)
				{
					result.m_Terms[t].Cupper -= rhs[p].Cupper;
					bTermFound = true;
					break;
				}
			}

			/*if (!bTermFound)
				result.AddTerm(rhs[p]);*/
		}

		return result;
	}

	inline CMathExpression& operator = (const CMathExpression& rhs)
	{
		m_Terms = rhs.m_Terms;

		return *this;
	}

	void SortByExp()
	{
		UInt32 count = NumTerms();

		TMathTermArray	temp;
		
		for (UInt32 i = 0; i < count; i++)
		{
			MathTermVar maxT = m_Terms[0];

			for (UInt32 mc = 0; mc < NumTerms(); mc++)
			{
				MathTermVar obj = m_Terms[mc];

				if (obj.Eupper > maxT.Eupper)
					maxT = obj;
			}

			m_Terms.Remove(maxT);

			temp.Push(maxT);
		}

		ASSERT(m_Terms.IsEmpty());
		
		m_Terms = temp;
	}

	////////////////////////

	UInt32 NumTerms() const				{	return m_Terms.Size();	}

	void SetName(const String& name)	{	m_Name = name;	}
	String GetName() const				{	return m_Name;			}

	////////////////////////

	void AddTerm(const MathTermVar& term)
	{
		m_Terms.Push(term);
	}

	inline MathTermVar& operator [] (UInt32 index)
	{
		return m_Terms[index];
	}

	inline const MathTermVar& operator [] (UInt32 index) const
	{
		return m_Terms[index];
	}

protected:
	TMathTermArray	m_Terms;

	String			m_Name;
};

//////////////////////////////////////////////////////////////////////////

class CPolynomial : public CMathExpression
{
public:
	//CPolynomial(void);
	
	explicit CPolynomial(const String& name = "", UInt32 size = 10) : 
		CMathExpression(name, size)
	{

	}

	~CPolynomial(void);

	/////////////////////////////////

	void Differentiate()
	{
		for (UInt32 term = 0; term < m_Terms.Size(); term++)
		{
			m_Terms[term].Differentiate();
		}

		m_Name.append("'");
	}

	void Integrate()
	{
		for (UInt32 term = 0; term < m_Terms.Size(); term++)
		{
			m_Terms[term].Integrate();
		}

		m_Terms.Push(MathTermVar(0, 0, "C", 0, 0));

		if (m_Name[m_Name.length() - 1] == '\'')
			m_Name.resize(m_Name.size() - 1);
	}

	/////////////////////////////////////////////////

	inline CPolynomial& operator = (const CPolynomial& rhs)
	{
		m_Terms = rhs.m_Terms;

		return *this;
	}

	/////////////////////////////////////////////////

};

//////////////////////////////////////////////////////////////////////////