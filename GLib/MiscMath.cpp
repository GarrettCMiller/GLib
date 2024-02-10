///////////////////////////////////////////////////////////////////////////
//		File:				MiscMath.cpp
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		Miscellaneous math functions
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "MiscMath.h"
#include "MathConstants.h"
#include "Vec.h"

//////////////////////////////////////////////////////////////////////////

Float Round(Float value)
{
	static const Float RoundVal = 10000.0f;

	// Get fractional part.
	Float fpart = value - static_cast<Int32>(value);

	if (fpart == 0.0f)
		return value;

	// Move decimal point five places.
	fpart = fpart * RoundVal;

	// Check if last digit if over five.
	if (fmodf(fpart, 10.0f) > 5.0f)
		// Round number up 1.
		fpart += 10.0f;

	// Knock off the trailing fractions
	fpart = static_cast<Float>(static_cast<Int32>(fpart) / 10);

	// Add new fractional part on whole part.
	value = static_cast<Int32>(value) + fpart / RoundVal;

	return value;
}

//////////////////////////////////////////////////////////////////////////

Bool RandomBool()
{
	return (Random(1) == 1);
}

//////////////////////////////////////////////////////////////////////////

Int32 Random(Int32 high, Int32 low /* = 0 */)
{
	if (high < low)
		Swap(high, low);

	return rand() % (high - low + 1) + low;
}

//////////////////////////////////////////////////////////////////////////

UInt32 Random(UInt32 high, UInt32 low /* = 0 */)
{
	if (high < low)
		Swap(high, low);

	return rand() % (high - low + 1) + low;
}

//////////////////////////////////////////////////////////////////////////

Float Random(Float high, Float low /* = 0 */)
{
	static Float prec = 100.0f;

	Int32 hi = static_cast<Int32>(high * prec),
		lo = static_cast<Int32>(low * prec);

	Int32 num = Random(hi, lo);

	return (num / prec);
}

//////////////////////////////////////////////////////////////////////////

Float Random()
{
	return (rand() / static_cast<Float>(0x7FFF));
}

//////////////////////////////////////////////////////////////////////////

Int32 RandomG(Int32 high, Int32 low, Int32 NumIter/* = 3 */)
{
	Int32 sum = 0;

	for (Int32 i = 0; i < NumIter; i++)
		sum += static_cast<Int32>(Random() * ((high - low) + low));

	return sum / NumIter;
}

//////////////////////////////////////////////////////////////////////////

Float RandomG(Float high/* = 1.0f */, Float low/* = 0.0f */, Int32 NumIter/* = 3 */)
{
	Float sum = 0.0f;
	for (Int32 i = 0; i < NumIter; i++)
		sum += Random() * ((high - low) + low);

	return sum / NumIter;
}

//////////////////////////////////////////////////////////////////////////

//Int32 RandomGauss(Int32 high, Int32 low, Int32 N)
//{
//	Int32 sum = 0;
//
//	for (Int32 i = 0; i < NumIter; i++)
//		sum += Random(A);
//
//	sum *= (1 / A);
//	sum *= sqrtf(12 / static_float<Float>(N));
//	sum -= sqrtf(3 * N);
//
//	return sum;
//}

//////////////////////////////////////////////////////////////////////////

//Float RandomGauss(Float high, Float low, Int32 NumIter)
//{
//	Float sum = 0.0f;
//
//	return sum;
//}

//////////////////////////////////////////////////////////////////////////

Float AngleBetween(Float x1, Float y1, Float x2, Float y2)
{
	Float	 angle;

	angle = atan2((y2 - y1), (x2 - x1));

	angle *= Rad2Deg;

	if (angle < 0)
		angle += 360.0f;

	return angle;
}

//////////////////////////////////////////////////////////////////////////

Float AngleBetween(CVec2<Float>* p1, CVec2<Float>* p2)
{
	return AngleBetween(p1->X, p1->Y, p2->X, p2->Y);
}

//////////////////////////////////////////////////////////////////////////

Float NextLargestPowerOfTwo(Float y)
{
	//y = a^x	--->	x = log<a>(y)
	Float	logA	= log(2.0f),
			logY	= log(y),
			x		= logY / logA;

	Int32		ix		= static_cast<Int32>(x);
	
	Float	val		= pow(2.0f, static_cast<Float>(ix)),
			vali	= pow(2.0f, x);
	
	if (val == vali)
		return vali;
	
	return	pow(2.0f, static_cast<Float>(ix + 1));
}

//////////////////////////////////////////////////////////////////////////

Float NextSmallestPowerOfTwo(Float y)
{
	//y = a^x	--->	x = log<a>(y)
	Float	logA	= log(2.0f),
			logY	= log(y),
			x		= logY / logA;

	Int32		ix		= static_cast<Int32>(x);
	
	Float	val		= pow(2.0f, static_cast<Float>(ix)),
			vali	= pow(2.0f, x);
	
	if (val == vali)
		return vali;
	
	return	pow(2.0f, static_cast<Float>(ix - 1));
}

//////////////////////////////////////////////////////////////////////////

Float Sine( Float freq, Float base, Float amp, Float phase)
{
	return (amp * sinf(phase * freq * 2 * PI) + base);
}

Float Cosine( Float freq, Float base, Float amp, Float phase)
{
	return (amp * cosf(phase * freq * 2 * PI) + base);
}

//////////////////////////////////////////////////////////////////////////