#include "StdAfx.h"
#include "Color.h"
#include "GameMacros.h"

//////////////////////////////////////////////////////////////////////////

const Color	Color::km_Black			= Color(0.0f, 0.0f, 0.0f, 1.0f);
const Color	Color::km_DarkGray		= Color(0.25f, 0.25f, 0.25f, 1.0f);
const Color	Color::km_Gray			= Color(0.5f, 0.5f, 0.5f, 1.0f);
const Color	Color::km_LightGray		= Color(0.75f, 0.75f, 0.75f, 1.0f);
const Color	Color::km_White			= Color(1.0f, 1.0f, 1.0f, 1.0f);
const Color	Color::km_Red			= Color(1.0f, 0.0f, 0.0f, 1.0f);
const Color	Color::km_Green			= Color(0.0f, 1.0f, 0.0f, 1.0f);
const Color	Color::km_Blue			= Color(0.0f, 0.0f, 1.0f, 1.0f);
const Color	Color::km_Yellow		= Color(1.0f, 1.0f, 0.0f, 1.0f);
const Color	Color::km_Magenta		= Color(1.0f, 0.0f, 1.0f, 1.0f);
const Color	Color::km_Cyan			= Color(0.0f, 1.0f, 1.0f, 1.0f);
const Color	Color::km_Transparent	= Color(0.0f, 0.0f, 0.0f, 0.0f);

//////////////////////////////////////////////////////////////////////////

Color::operator UInt32() const
{
	UInt8	r = GetRed(),
			g = GetGreen(),
			b = GetBlue(),
			a = GetAlpha();
	return RGB32(a, r, g, b);
}

Color::operator D3DXCOLOR() const
{
	return D3DXCOLOR(Red(), Green(), Blue(), Alpha());
}

Color::operator D3DCOLORVALUE() const
{
	D3DCOLORVALUE color = {Red(), Green(), Blue(), Alpha()};
	return color;
}

Color::operator Vec4() const
{
	return Vec4(Red(), Green(), Blue(), Alpha());
}

COLORREF Color::ToCOLORREF() const
{
	return RGB(GetRed(), GetGreen(), GetBlue());
}

//Color::operator D3DXVECTOR4() const
//{
//	return Vec4(Red(), Green(), Blue(), Alpha());
//}

Color Color::operator + (const Color& rhs)
{
	return Color(Red() + rhs.Red(), Green() + rhs.Green(), Blue() + rhs.Blue());
}

Color& Color::operator +=(const Color& rhs)
{
	SetRed(Red()		+ rhs.Red());
	SetGreen(Green()	+ rhs.Green());
	SetBlue(Blue()		+ rhs.Blue());

	return *this;
}

Color Color::operator - (const Color& rhs)
{
	return Color(Red() - rhs.Red(), Green() - rhs.Green(), Blue() - rhs.Blue());
}

Color& Color::operator -=(const Color& rhs)
{
	SetRed(Red()		- rhs.Red());
	SetGreen(Green()	- rhs.Green());
	SetBlue(Blue()		- rhs.Blue());

	return *this;
}

Color Color::operator *(const Color& rhs)
{
	return Color(rhs.Red() * Red(), rhs.Green() * Green(), rhs.Blue() * Blue());
}

Color& Color::operator *=(const Color& rhs)
{
	SetRed(Red()		* rhs.Red());
	SetGreen(Green()	* rhs.Green());
	SetBlue(Blue()		* rhs.Blue());

	return *this;
}

Color Color::operator *(Float scale)
{
	return Color(scale * Red(), scale * Green(), scale * Blue());
}

Color& Color::operator *=(Float scale)
{
	SetRed(Red()		* scale);
	SetGreen(Green()	* scale);
	SetBlue(Blue()		* scale);

	return *this;
}

void Color::Randomize(Bool bRandomizeAlpha /* = false */)
{
	SetRed(Random());
	SetGreen(Random());
	SetBlue(Random());

	if (bRandomizeAlpha)
		SetAlpha(Random());
}

void Color::Normalize()
{
	if (!bNormalizedRed)
		SetRed(Red());

	if (!bNormalizedGreen)
		SetGreen(Green());

	if (!bNormalizedBlue)
		SetBlue(Blue());

	if (!bNormalizedAlpha)
		SetAlpha(Alpha());
}

Color Color::GetGradient(Float delta, const Color& rhs)
{
	Float	r1		= Red()			* (1 - delta),
			r2		= rhs.Red()		* delta,

			g1		= Green()		* (1 - delta),
			g2		= rhs.Green()	* delta,

			b1		= Blue()		* (1 - delta),
			b2		= rhs.Blue()	* delta;

	return (Color(r1, g1, b1) + Color(r2, g2, b2));
}

void Color::SetNormalized(Bool bNormalized /* = true */)
{
	bNormalizedAlpha = 
		bNormalizedBlue = 
			bNormalizedGreen = 
				bNormalizedRed = bNormalized;
}

//////////////////////////////////////////////////////////////////////////

void Color::ConvertFromHSV(Float hue, Float saturation, Float value)
{
	if (saturation == 0)
	{
		SetRed(value);
		SetGreen(value);
		SetBlue(value);
		return;		
	}

	hue /= 60;

	Int32 i = static_cast<Int32>(hue);

	Float	f	= hue - i,
			p	= value * (1 - saturation),
			q	= value * (1 - saturation * f),
			t	= value * (1 - saturation * (1 - f));

	Float	r, g, b;

	switch (i)
	{
	case 0:
		r = value;
		g = t;
		b = p;
		break;

	case 1:
		r = q;
		g = value;
		b = t;
		break;

	case 2:
		r = p;
		g = value;
		b = t;
		break;

	case 3:
		r = p;
		g = q;
		b = value;
		break;

	case 4:
		r = t;
		g = p;
		b = value;
		break;

	default:
		r = value;
		g = p;
		b = q;
	}

	SetRed(r);
	SetGreen(g);
	SetBlue(b);
}