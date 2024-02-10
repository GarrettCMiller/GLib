///////////////////////////////////////////////////////////////////////////
//		File:				MathConstants.h
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		Special math constants
//////////////////////////////////////////////////////////////////////////
#pragma once

//#include "Globals.h"
#include "DataTypes.h"

//////////////////////////////////////////////////////////////////////////

// Pi to 31 digits
const Float PI		= 3.1415926535897932384626433832795f;
const Float TWOPI	= 6.283185307179586476925286766559f;

//Golden ratio
const Float g_Phi	= 1.6180339f;

// The number o' degrees in a radian.
// 1 radian == Rad2Deg degrees
const Float Rad2Deg = 180.0f	/	PI;		//57.2957795130823;
const Float Deg2Rad = PI		/	180.0f;