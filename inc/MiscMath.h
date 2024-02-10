///////////////////////////////////////////////////////////////////////////
//		File:				MiscMath.h
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		Miscellaneous math functions
//////////////////////////////////////////////////////////////////////////
#pragma once

#include <complex>

using std::complex;

//////////////////////////////////////////////////////////////////////////

template <typename _Type>
void Swap(_Type& a, _Type& b)
{
	_Type temp = a;
	a = b;
	b = temp;
}

//////////////////////////////////////////////////////////////////////////

template <typename _Type>
_Type Clamp(_Type val, const _Type& min, const _Type& max)
{
	if (val < min)
		val = min;
	else if (val > max)
		val = max;
	return val;
}

//////////////////////////////////////////////////////////////////////////

template <typename _Type>
const _Type& Min(const _Type& a, const _Type& b)
{
	if (a < b)
		return a;

	return b;
}

//////////////////////////////////////////////////////////////////////////

template <typename _Type>
const _Type& Max(const _Type &a, const _Type &b)
{
	if (a > b)
		return a;

	return b;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

template <typename Type = Float>
struct SineArgs
{
	SineArgs(Type f = 0, Type b = 0, Type a = 0, Type p = 0) : 
		freq(f), base(b), amp(a), phase(p)
	{	}

	/*operator Vec4() const
	{
		return Vec4(freq, base, amp, phase);
	}*/

	operator Float* ()
	{
		return &amp;
	}

	Type amp;
	Type phase;
	Type freq;
	Type base;
};

typedef SineArgs<Float>		SineArgsF;
typedef SineArgs<Int32>		SineArgsI;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

template <typename Type>
Type Sine( Type freq, Type base, Type amp, Type phase)
{
	return (amp * sin(phase * freq * TWOPI) + base);
}

Float Sine( Float freq, Float base, Float amp, Float phase);

template <typename Type>
Type Sine(const SineArgs<Type>& args)
{
	return (args.amp * sin(args.phase * args.freq * TWOPI) + args.base);
}

//////////////////////////////////////////////////////////////////////////

template <typename Type>
Type Cosine( Type freq, Type base, Type amp, Type phase)
{
	return (amp * cos(phase * freq * TWOPI) + base);
}

Float Cosine( Float freq, Float base, Float amp, Float phase);

template <typename Type>
Type Cosine(const SineArgs<Type>& args)
{
	return (args.amp * cos(args.phase * args.freq * TWOPI) + args.base);
}

//////////////////////////////////////////////////////////////////////////

template <typename Type>
Type Tangent( Type freq, Type base, Type amp, Type phase)
{
	return (amp * tan(phase * freq * TWOPI) + base);
}

template <typename Type>
Type Tangent(const SineArgs<Type>& args)
{
	return (args.amp * tan(args.phase * args.freq * TWOPI) + args.base);
}

//////////////////////////////////////////////////////////////////////////

template <typename Type>
Type ArcSine( Type freq, Type base, Type amp, Type phase)
{
	return (amp * asin(phase * freq * TWOPI) + base);
}

template <typename Type>
Type ArcSine(const SineArgs<Type>& args)
{
	return (args.amp * asin(args.phase * args.freq * TWOPI) + args.base);
}

//////////////////////////////////////////////////////////////////////////

template <typename Type>
Type ArcCosine( Type freq, Type base, Type amp, Type phase)
{
	return (amp * acos(phase * freq * TWOPI) + base);
}

template <typename Type>
Type ArcCosine(const SineArgs<Type>& args)
{
	return (args.amp * acos(args.phase * args.freq * TWOPI) + args.base);
}

//////////////////////////////////////////////////////////////////////////

template <typename Type>
Type ArcTangent( Type freq, Type base, Type amp, Type phase)
{
	return (amp * atan(phase * freq * TWOPI) + base);
}

template <typename Type>
Type ArcTangent(const SineArgs<Type>& args)
{
	return (args.amp * atan(args.phase * args.freq * TWOPI) + args.base);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

template <typename TypeTo, typename TypeFrom>
TypeTo ReinterpretCast(const TypeFrom& from)
{
	return (*((TypeTo *)(&from)));
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

Float	Round(Float value);

Int32	Random(Int32 high, Int32 low = 0);
UInt32	Random(UInt32 high, UInt32 low = 0);
Float	Random(Float high, Float low = 0.0f);
Float	Random();
Bool	RandomBool();

//Random Gaussian
Int32	RandomG(Int32 high, Int32 low, Int32 NumIter = 2);
Float	RandomG(Float high = 1.0f, Float low = 0.0f, Int32 NumIter = 2);

template <typename _float>
class CVec2;

Float	AngleBetween(Float x1, Float y1, Float x2, Float y2);
Float	AngleBetween(CVec2<Float>* p1, CVec2<Float>* p2);

Float	NextLargestPowerOfTwo(Float y);
Float	NextSmallestPowerOfTwo(Float y);

//////////////////////////////////////////////////////////////////////////

typedef complex<Float>	ComplexF;
typedef complex<Double>	ComplexD;

typedef ComplexF		Complex;

//////////////////////////////////////////////////////////////////////////