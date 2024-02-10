///////////////////////////////////////////////////////////////////////////////
//		File:				Flag.h
//
//		Programmer:			Garrett Miller (GM)
//
//		Date created:		September 14, 2004
//
//		Description:		Implementation of a templeted bit flag class
///////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////////////////////////////////////////////

#include <Math.h>

#include "DataTypes.h"

//Converts from enum val to bit val
//						back to bit number		convert back from float			convert to int to quiet pow()
#define FlagBit(_flag)	(static_cast<UInt32>(static_cast<Int32>(pow(2.0f, static_cast<Int32>(_flag - 1)))))
//////////////////////////////////////////////////////////////////////////

template <typename _FlagEnum>
class Flag
{
	UInt32	m_FlagBits;

public:
	Flag(void) { m_FlagBits = 0; }
	
	Flag(UInt32 val)
	{
		m_FlagBits = FlagBit(val);
	}

	inline Bool	SetFlag(_FlagEnum flag, Bool On = true)
	{
		if (flag == 0)
			m_FlagBits = 0;
		else
		{
			if (On)
				m_FlagBits |= FlagBit(flag);
			else
				m_FlagBits &= ~FlagBit(flag);
		}

		return On;
	}

	//A quick, concise way to check a flag
	inline Bool operator [ ] (_FlagEnum flag) const
	{
		return TestFlag(flag);
	}

	inline Bool	ToggleFlag(_FlagEnum flag)
	{
		m_FlagBits ^= FlagBit(flag);
		return ((m_FlagBits & FlagBit(flag)) == FlagBit(flag));
	}

	inline Bool	TestFlag(_FlagEnum flag) const
	{
		return ((m_FlagBits & FlagBit(flag)) == FlagBit(flag));
	}
};