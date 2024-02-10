#pragma once

#include "DynamicArray.h"

typedef TStringArray				TTokenArray;
typedef CDynamicArray<TTokenArray>	TSortedTokenArray;

TStringArray Tokenize(String& input, const String& delim);
TStringArray TokenizeGroups(String& input, const String& delim, const String& groupDelim = "");

Int32 GetGroupCount(String& input, const String& groupDelim = "");

TSortedTokenArray TokenizeSort(const String& input, const String& delim, const String& groupDelim = "");

Bool IsAlpha(const String& input);
Bool IsAlNum(const String& input, Bool bAllowSlash = false);
Bool IsANumber(const String& input, Bool bAllowSlash = false);

class CVec3;
String Vec3ToString(CVec3* pVec, UInt32 precision = 8);
String IntToString(Int32 val);
String FloatToString(Float val, UInt32 precision = 8);
Float StringToFloat(String str);