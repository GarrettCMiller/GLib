#pragma once

#include "DynamicArray.h"
#include "Color.h"

//////////////////////////////////////////////////////////////////////////

class Palette
{
public:
	Palette(UInt32 count = 0);
	//Palette(Palette& pal);

	~Palette(void);

	void	Init(UInt32 count, UInt32 offset = 0);
	void	DeletePalette();

	Color	GetColor(UInt32 index);
	Color	GetColorGradient(UInt32 index, Float delta);

	void	Copy(const CDynamicArray<Color>& colors, UInt32 count);

	Float	GetRedOffset()		const	{return m_Offset.Red();}
	Float	GetGreenOffset()	const	{return m_Offset.Green();}
	Float	GetBlueOffset()		const	{return m_Offset.Blue();}

	Color&	operator [] (UInt32 index)
	{
		return m_Palette[index];
	}

	Color	operator [] (UInt32 index) const
	{
		return m_Palette[index];
	}

private:
	CDynamicArray<Color>	m_Palette;

	UInt32	m_Count;

	Color	m_Offset;
};
