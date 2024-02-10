//==============================================================================
//	File:		VS2005Prefix.h
//
//	Copyright 1999-2006, Anark Corporation.
//	ALL RIGHTS RESERVED
//
//	UNPUBLISHED -- Rights reserved under the copyright laws of the United
//	States.   Use of a copyright notice is precautionary only and does not
//	imply publication or disclosure.
//
//	U.S. GOVERNMENT RESTRICTED RIGHTS LEGEND:
//	Use, duplication or disclosure by the Government is subject to restrictions
//	as set forth in FAR 52.227.19(c)(2) or subparagraph (c)(1)(ii) of the Rights
//	in Technical Data and Computer Software clause at DFARS 252.227-7013 and/or
//	in similar or successor clauses in the FAR, or the DOD or NASA FAR
//	Supplement.  Contractor/manufacturer is Anark Corporation, Boulder, CO
//
//	THE CONTENT OF THIS WORK CONTAINS CONFIDENTIAL AND PROPRIETARY
//	INFORMATION OF ANARK CORPORATION. ANY DUPLICATION, MODIFICATION,
//	DISTRIBUTION, OR DISCLOSURE IN ANY FORM, IN WHOLE, OR IN PART, IS STRICTLY
//	PROHIBITED WITHOUT THE PRIOR EXPRESS WRITTEN PERMISSION OF ANARK
//	CORPORATION.
//==============================================================================
#pragma once

#ifdef _WIN32

#if _MSC_VER >= 1400

// /Wall enables all warnings that are turned off by default, hence these have to be explicitly turned off
#pragma warning( disable : 4548 )	// "expression before comma has no effect; expected expression with side-effect"
#pragma warning( disable : 4365 )	// "'argument' : conversion from '...' to '...', signed/unsigned mismatch"
#pragma warning( disable : 4826 )	// "Conversion from 'const void *' to 'void *' is sign-extended. This may cause unexpected runtime behavior."
#pragma warning( disable : 4619 )	// "pragma warning : there is no warning number '4609'"
#pragma warning( disable : 4255 )	// "'_get_printf_count_output' : no function prototype given: converting '()' to '(void)'"
#pragma warning( disable : 4571 )	// Informational: catch(...) semantics changed since Visual C++ 7.1; structured exceptions (SEH) are no longer caught
#pragma warning( disable : 4738 )	// storing 32-bit float result in memory, possible loss of performance


#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#endif // _CRT_SECURE_NO_DEPRECATE

#ifndef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#endif // _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES

#ifdef _HAS_EXCEPTIONS
#if _HAS_EXCEPTIONS == 0

#ifdef _DEBUG

// Turn on exceptions for Debug to workaround the link errors.
#undef _HAS_EXCEPTIONS
#define _HAS_EXCEPTIONS  1

// This pleases the compiler, but if you do have exception handling code, specify yes to "Enable C++ Exceptions" in the project settings
#pragma warning( disable : 4530 ) // C++ exception handler used, but unwind semantics are not enabled. Specify /EHsc

#else

// Allow this to be turned off when necessary, e.g. toLua++ does not need this.
#ifndef NO_HAS_EXCEPTIONS_WORKAROUND
//==============================================================================
/*
 * 	This is a workaround for link errors that result from having _HAS_EXCEPTIONS=0 
 *	It is a known issue : http://connect.microsoft.com/VisualStudio/feedback/ViewFeedback.aspx?FeedbackID=98637
 *	and currently the only workaround according to the XDS forum, is via this hack.
 *	Microsoft should be releasing an official solution, but till then, this will suffice.
 */
#ifdef _SECURE_SCL
#undef _SECURE_SCL
#endif
#define _SECURE_SCL 0

#ifndef _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE
#endif

#ifndef _SCL_SECURE_NO_DEPRECATE
#define _SCL_SECURE_NO_DEPRECATE
#endif

#define DISABLE_EXCEPTIONS
// Raise will result in this warning
#pragma warning( disable : 4127 )	// conditional expression is constant
#include <xstddef>
#undef _RAISE
#undef _THROW
#undef _THROW_NCEE
#define _RAISE( x )			do { } while ( false )
#define _THROW( x, y )		do { } while ( false )
#define _THROW_NCEE( x, y )	do { } while ( false )

#endif // NO_HAS_EXCEPTIONS_WORKAROUND
//==============================================================================

#endif // _DEBUG

#endif // _HAS_EXCEPTIONS == 0
#endif // _HAS_EXCEPTIONS


#endif // _MSC_VER

#endif // _WIN32
