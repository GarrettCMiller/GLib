#pragma once

#include <d3dx9math.h>

#include "MiscMath.h"

#include "Vec.h"

//////////////////////////////////////////////////////////////////////////

class Color
{
public:
	static const UInt8 m_FloatAdjustment = 255;

	//////////////////////////////////////////////////////////////////////////

	explicit Color(UInt8 r, UInt8 g, UInt8 b, UInt8 a = 255) : 
	nRed(r),	bNormalizedRed(false),
		nGreen(g),	bNormalizedGreen(false),
		nBlue(b),	bNormalizedBlue(false),
		nAlpha(a),	bNormalizedAlpha(false)
	{	}

	explicit Color(Float r = 0.0f, Float g = 0.0f, Float b = 0.0f, Float a = 1.0f) : 
	fRed(r),	bNormalizedRed(true),
		fGreen(g),	bNormalizedGreen(true),
		fBlue(b),	bNormalizedBlue(true),
		fAlpha(a),	bNormalizedAlpha(true)
	{	}

	Color(COLORREF color, UInt8 a = 255) : 
	nRed(GetRValue(color)),		bNormalizedRed(false),
		nGreen(GetGValue(color)),	bNormalizedGreen(false),
		nBlue(GetBValue(color)),	bNormalizedBlue(false),
		nAlpha(a),					bNormalizedAlpha(false)
	{	}

	Color(const D3DCOLORVALUE& color) :
		fRed(color.r),	bNormalizedRed(true),
		fGreen(color.g),	bNormalizedGreen(true),
		fBlue(color.b),	bNormalizedBlue(true),
		fAlpha(color.a),	bNormalizedAlpha(true)
	{	}

	//////////////////////////////////////////////////////////////////////////

	operator UInt32() const;

	operator D3DXCOLOR() const;

	operator D3DCOLORVALUE() const;

	operator Vec4() const;

	COLORREF ToCOLORREF() const;

	//operator D3DXVECTOR4() const;

	Color	operator + (const Color& rhs);
	Color&	operator +=(const Color& rhs);

	Color	operator - (const Color& rhs);
	Color&	operator -=(const Color& rhs);

	Color	operator * (const Color& rhs);
	Color&	operator *=(const Color& rhs);

	Color	operator * (Float scale);
	Color&	operator *=(Float scale);

	Color	GetGradient(Float delta, const Color& rhs);

	void	ConvertFromHSV(Float hue, Float saturation, Float value);

	void	Randomize(Bool bRandomizeAlpha = false);

	void	Normalize();

	void	SetRed(Float red)		{	fRed	= red;		bNormalizedRed = true;		}
	void	SetRed(UInt8 red)		{	nRed	= red;		bNormalizedRed = false;		}

	void	SetGreen(Float green)	{	fGreen	= green;	bNormalizedGreen = true;	}
	void	SetGreen(UInt8 green)	{	nGreen	= green;	bNormalizedGreen = false;	}

	void	SetBlue(Float blue)		{	fBlue	= blue;		bNormalizedBlue = true;		}
	void	SetBlue(UInt8 blue)		{	nBlue	= blue;		bNormalizedBlue = false;	}

	void	SetAlpha(Float alpha)	{	fAlpha	= alpha;	bNormalizedAlpha = true;	}
	void	SetAlpha(UInt8 alpha)	{	nAlpha	= alpha;	bNormalizedAlpha = false;	}

	Float	Red() const				{	return (bNormalizedRed ? fRed : static_cast<Float>(nRed) / m_FloatAdjustment);			}
	UInt8	GetRed() const			{	return (bNormalizedRed ? static_cast<UInt8>(fRed * m_FloatAdjustment) : nRed);			}

	Float	Green() const			{	return (bNormalizedGreen ? fGreen : static_cast<Float>(nGreen) / m_FloatAdjustment);	}
	UInt8	GetGreen() const		{	return (bNormalizedGreen ? static_cast<UInt8>(fGreen * m_FloatAdjustment) : nGreen);		}

	Float	Blue() const			{	return (bNormalizedBlue ? fBlue : static_cast<Float>(nBlue) / m_FloatAdjustment);		}
	UInt8	GetBlue() const			{	return (bNormalizedBlue ? static_cast<UInt8>(fBlue * m_FloatAdjustment) : nBlue);		}

	Float	Alpha() const			{	return (bNormalizedAlpha ? fAlpha : static_cast<Float>(nAlpha) / m_FloatAdjustment);	}
	UInt8	GetAlpha() const		{	return (bNormalizedAlpha ? static_cast<UInt8>(fAlpha * m_FloatAdjustment) : nAlpha);		}

public:
	static const Color	km_Black;
	static const Color	km_DarkGray;
	static const Color	km_Gray;
	static const Color	km_LightGray;
	static const Color	km_White;
	static const Color	km_Red;
	static const Color	km_Green;
	static const Color	km_Blue;
	static const Color	km_Yellow;
	static const Color	km_Magenta;
	static const Color	km_Cyan;
	static const Color	km_Transparent;

protected:
	union
	{
		UInt8	nRed;
		Float	fRed;
	};

	union
	{
		UInt8	nGreen;
		Float	fGreen;
	};

	union
	{
		UInt8	nBlue;
		Float	fBlue;
	};

	union
	{
		UInt8	nAlpha;
		Float	fAlpha;
	};

	Bool	bNormalizedRed;
	Bool	bNormalizedGreen;
	Bool	bNormalizedBlue;
	Bool	bNormalizedAlpha;

	void	SetNormalized(Bool bNormalized = true);
};