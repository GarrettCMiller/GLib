#pragma once

#include "Functor.h"
#include "Variable.h"

///////////////////////////////////////////////////////////////////////////////

class CBaseGenotype;	typedef CBaseGenotype*	TGenotypePtr;

class CBaseGenotype
{
protected:
	static UInt32	mk_numElements;
	static UInt32	mk_numTotalBits;

	String			m_BitString;
	TArgumentArray	m_VarArray;

	Float			m_fNormFitness;

public:
	CBaseGenotype()	: m_fNormFitness(-1.0f)	{	}
	virtual ~CBaseGenotype()				{	}

	UInt32 GetNumElements() const	{ return mk_numElements;	}

	virtual void	SetValues(const TArgumentArray& arrVars) = 0;
	virtual Float	Evaluate() const = 0;
	virtual void	Randomize() = 0;
	virtual void	SetDefaults() = 0;

	virtual void CrossoverUnary(TGenotypePtr mother)
	{
		UInt32 swapIndex = Random(mk_numTotalBits);

		String str1 = mother->m_BitString;
		String str2 = m_BitString;

		mother->m_BitString = str1.substr(0, swapIndex);
		mother->m_BitString.append(str2.substr(swapIndex + 1));

		m_BitString = str2.substr(0, swapIndex);
		m_BitString.append(str1.substr(swapIndex + 1));
	}

	virtual void CrossoverBinary(TGenotypePtr mother)
	{
		UInt32	swapIndex1 = Random(mk_numTotalBits),
				swapIndex2 = Random(mk_numTotalBits);

		if (swapIndex1 > swapIndex2)
			Swap(swapIndex1, swapIndex2);

		String str1 = mother->m_BitString;
		String str2 = m_BitString;

		mother->m_BitString = str1.substr(0, swapIndex1);
		mother->m_BitString.append(str2.substr(swapIndex1 + 1, swapIndex2));
		mother->m_BitString.append(str1.substr(swapIndex2 + 1));

		m_BitString = str2.substr(0, swapIndex1);
		m_BitString.append(str1.substr(swapIndex1 + 1, swapIndex2));
		m_BitString.append(str2.substr(swapIndex2 + 1));
	}

	virtual void CrossoverDisruption(TGenotypePtr mother)
	{
		String str1 = mother->m_BitString;
		String str2 = m_BitString;

		mother->m_BitString = "";
		m_BitString = "";

		for (UInt32 v = 0; v < mk_numElements; v++)
		{
			UInt32	swapIndex = sizeof(Float) * (v + 1);

			if (RandomBool())
			{
				mother->m_BitString.append(str1.substr(v * swapIndex, (v + 1) * swapIndex));
				m_BitString.append(str2.substr(v * swapIndex, (v + 1) * swapIndex));
			}
			else
			{
				mother->m_BitString.append(str2.substr(v * swapIndex, (v + 1) * swapIndex));
				m_BitString.append(str1.substr(v * swapIndex, (v + 1) * swapIndex));
			}
		}
	}

	virtual void Mutate(UInt32 oneInChance = 1000)
	{
		for (UInt32 i = 0; i < m_BitString.length(); i++)
		{
			if (Random(oneInChance) == 0)
			{
				if (m_BitString[i] == '1')
					m_BitString[i] = '0';
				else
					m_BitString[i] = '1';
			}
		}
	}

	void SetNormalizedFitness(Float fFitness)
	{
		ASSERT(fFitness >= 0.0f && fFitness <= 1.0f);
		m_fNormFitness = fFitness;
	}

	Int32 ConvertFloatToInt(Float f)
	{
		return f * INT_MAX;
	}

protected:
	virtual Bool CreateBitString()
	{
		m_BitString.clear();

		for (UInt32 v = 0; v < mk_numElements; v++)
		{
			Float fVal = static_cast<Float>(m_VarArray[v * 3]);
			UInt32 val = 0;
			
			//if (fVal != 0.0f)
				val = ConvertFloatToInt(fVal);	//FloatToInt(fVal);
			/*else
				val = 0;*/

			for (UInt32 bit = 1; bit <= sizeof(Int32) * 8; bit++)
			{
				UInt32 bitVal = pow(2.0f, Float(bit - 1));

				if ((val & bitVal) == bitVal)
					m_BitString.append("1");
				else
					m_BitString.append("0");
			}
		}

		return true;
	}
};

typedef	CDynamicArray<TGenotypePtr>		TGenotypeArray;

///////////////////////////////////////////////////////////////////////////////

class CBaseGeneticAlgorithm
{
};

///////////////////////////////////////////////////////////////////////////////

template </*TGenotypePtr*/typename _geneType>
class CGeneticAlgorithm : public CBaseGeneticAlgorithm
{
public:
	typedef CFunctor<_geneType, Float>		TEvaluationFunction;

public:
	CGeneticAlgorithm(void)					{	}
	virtual ~CGeneticAlgorithm(void)		{	}

	virtual Int32 Init()
	{
		return 1;
	}

	virtual Int32 Update()
	{
		return 1;
	}

public:

	virtual void CreateInitialPopulation(UInt32 popSize)
	{
		for (UInt32 i = 0; i < popSize; i++)
		{
			TGenotypePtr pObj = new _geneType;

			pObj->Randomize();

			m_CurrentPopulation.Push(pObj);
		}
	}

	virtual void CheckFitness()
	{
		m_fCurrentTotalFitness = 0.0f;

		//m_CurrentPopulation.Sort(

		FOR_EACH(i, m_CurrentPopulation)
		{
			Float fitness = m_CurrentPopulation[i]->Evaluate();

			if (i == 0)
			{
				m_fCurrentTotalFitness += fitness;
				m_CurrentSurvivors.Push(m_CurrentPopulation[0]);
				continue;
			}

			FOR_EACH(Si, m_CurrentSurvivors)
			{
				if (fitness > m_CurrentSurvivors[Si]->Evaluate())
				{
					m_fCurrentTotalFitness += fitness;
					m_CurrentSurvivors.Push(m_CurrentPopulation[i]);
					break;
				}
			}
		}

		//
		// TODO: Add all members to survivors, then cull from there (i.e. pop least fit members)
		//

		m_PopulationHistory.Push(m_CurrentPopulation);
		m_CurrentPopulation.Clear();

		FOR_EACH(i, m_CurrentSurvivors)
		{
			Float fVal = m_CurrentSurvivors[i]->Evaluate() / m_fCurrentTotalFitness;
			m_CurrentSurvivors[i]->SetNormalizedFitness(fVal);

			m_CurrentSurvivors[i]->Mutate();
 
			m_CurrentPopulation.Push(m_CurrentSurvivors[i]);
		}

		if (m_CurrentPopulation.Size() >= 2)
		{
			TGenotypePtr	pChild1 = new CLSystemGenotype(*static_cast<CLSystemGenotype*>(m_CurrentPopulation[0])),
							pChild2 = new CLSystemGenotype(*static_cast<CLSystemGenotype*>(m_CurrentPopulation[1]));
			
			switch (Random(2))
			{
			case 0:	pChild1->CrossoverBinary(pChild2);		break;

			case 1:	pChild1->CrossoverUnary(pChild2);		break;

			case 2:	pChild1->CrossoverDisruption(pChild2);	break;
			}

			m_CurrentPopulation.Push(pChild1);
			m_CurrentPopulation.Push(pChild2);
		}
	}

protected:
	TGenotypeArray	m_CurrentPopulation;
	TGenotypeArray	m_CurrentSurvivors;

	Float			m_fCurrentTotalFitness;

	CDynamicArray<TGenotypeArray>	m_PopulationHistory;
};
