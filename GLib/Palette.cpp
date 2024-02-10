#include "StdAfx.h"
#include ".\palette.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////////

Palette::Palette(UInt32 count) : m_Count(count)
{
	Init(count);
}

Palette::~Palette(void)
{
}

void Palette::Init(UInt32 count, UInt32 offset)
{
	m_Count = max(count, 256);

	Float freq = 100;//30;//12.75f;

	m_Palette.Resize(m_Count, false);

	Float	r = 0.0f,
			g = 0.0f,
			b = 0.0f;

	BYTE	br = 0,
			bg = 0,
			bb = 0;

	m_Offset.SetRed(static_cast<UInt8>(0 + offset));
	m_Offset.SetGreen(static_cast<UInt8>(85 + offset));
	m_Offset.SetBlue(static_cast<UInt8>(170 + offset));

	for (UInt32 i = 0; i < m_Count; i++)
	{
		br = m_Offset.GetRed();
		bg = m_Offset.GetGreen();
		bb = m_Offset.GetBlue();

		//68, 125, 182
		r = Sine( freq, 0.5f, 0.5f, fabsf(static_cast<Float>(i + br) / m_Count));
		g = Sine( freq, 0.5f, 0.5f, fabsf(static_cast<Float>(i + bg) / m_Count));
		b = Sine( freq, 0.5f, 0.5f, fabsf(static_cast<Float>(i + bb) / m_Count));

		/*r = Sine( freq, 0.666f, 0.333f, fabsf(Float(i) / m_Count));
		g = Sine( freq, 0.666f, 0.333f, fabsf(Float(i) / m_Count));
		b = Sine( freq, 0.666f, 0.333f, fabsf(Float(i) / m_Count));*/

		m_Palette[i] = Color(r, g, b);

		//freq *= 0.99;
	}
}  

Color Palette::GetColor(UInt32 index)
{
	if (index >= m_Count)
		index = 0;

	return m_Palette[index];
}

Color Palette::GetColorGradient(UInt32 index, Float delta)
{
	return GetColor(index).GetGradient(delta, GetColor(index + 1));
}

void Palette::Copy(const CDynamicArray<Color>& colors, UInt32 /*count*/)
{
	m_Palette.CopyFrom(colors);
}