#include "stdafx.h"
#include "GlobalFuncs.h"
#include "Vec.h"

TStringArray Tokenize(String& input, const String& delim)
{
	TStringArray strings;

	if (!input.empty())
	{
		Int32	firstpos = input.find_first_not_of(delim),
				lastpos = 0,
				temppos = 0;

		while (firstpos != String::npos)
		{
			lastpos = firstpos;
			temppos = lastpos;
			
retryp:
			lastpos = input.find_first_of(delim, temppos);

			if (lastpos == temppos && input.length() == lastpos + 1)
			{
				temppos++;
				goto retryp;
			}

			if (lastpos != String::npos)
			{
				String temp = input.substr(firstpos, lastpos - firstpos); 

				strings.Push(temp);
				
				firstpos = input.find_first_not_of(delim, lastpos);
			}
			else
				break;
		}

		if (firstpos != String::npos)
		{
			strings.Push(input.substr(firstpos));
		}
	}

	return strings;
}

TStringArray TokenizeGroups(String& input, const String& delim, const String& groupDelim /* = "" */)
{
	String newDelim = delim + "()";
	
	TStringArray strings;

	if (!input.empty() &&
		(input.find_first_of("(") != String::npos && input.length() < 3) ? false : true) //Length must have two parentheses () and at least one inner term
	{
		Int32	firstpos = input.find_first_not_of(delim),
				lastpos = 0,
				temppos = 0;

		Int32	parCount = 0,
				groupCount = 0,
				topGroupCount = 0;

		while (firstpos != String::npos)
		{
			if (input[firstpos] == '(')
			{
				if (parCount == 0)
					topGroupCount++;

				parCount++;
				groupCount++;

				lastpos = firstpos + 1;

				String temp = input.substr(0, firstpos);

				//check for implicit mult
				if (delim[0] == '*' && (IsAlpha(temp) || IsANumber(temp)))
				{
					strings.Push(temp);

					firstpos += temp.length();
					lastpos += temp.length();
				}

				do
				{
					lastpos = input.find_first_of("()", lastpos);

					if (lastpos != String::npos)
					{
						if (input[lastpos] == '(')
						{
							if (parCount == 0)
								topGroupCount++;

							parCount++;
							lastpos++;
							groupCount++;
						}
						else
						{
							parCount--;
							
							if (parCount > 0)
								lastpos++;
						}
					}
					else
						ASSERT(0);

				} while (parCount > 0);

				lastpos++;
			}
			else if (input[lastpos] == ')')
			{
				if (parCount <= 0)
					ASSERT(0);

				parCount--;

				lastpos++;
			}
			else
				lastpos = firstpos;

			temppos = lastpos;
			
retryp:
			lastpos = input.find_first_of(newDelim, temppos);

			if (lastpos == temppos && input.length() == lastpos + 1)
			{
				temppos++;
				goto retryp;
			}

			if (lastpos != String::npos)
			{
				if (input[lastpos] == '(')
				{
					if (parCount == 0)
						topGroupCount++;
					
					parCount++;
					groupCount++;

					//check for implicit mult
					String temp = input.substr(0, lastpos);
					if ((delim[0] == '*' || delim[0] == 'x')
						&& ((temp[0] == '_' || isalpha(temp[0])) || IsANumber(temp)))
					{
						strings.Push(temp);
						firstpos += temp.length();
						lastpos += temp.length();
					}

					do
					{
						temppos = lastpos + 1;
						lastpos = input.find_first_of("()", temppos);

						if (lastpos != String::npos)
						{
							if (input[lastpos] == '(')
							{
								if (parCount == 0)
									topGroupCount++;

								parCount++;
								groupCount++;
								//lastpos++;
							}
							else
							{
								parCount--;

								/*if (parCount > 0)
									lastpos++;*/
							}
						}
						else
							ASSERT(0);

					} while (parCount > 0);

					temppos = lastpos + 1;

					goto retryp;
				}
				else if (input[lastpos] == ')')
				{
					if (parCount <= 0)
						ASSERT(0);

					parCount--;
				}

				String temp = input.substr(firstpos, lastpos - firstpos); 

				strings.Push(temp);
				
				firstpos = input.find_first_not_of(delim, lastpos);
			}
			else
				break;
		}

		if (firstpos != String::npos)
		{
			if (topGroupCount == 1 && input[0] == '(' && input[input.length()-1] == ')')
			{
				input = input.substr(1, input.length() - 2);
				strings = Tokenize(input, delim);
			}
			else
				strings.Push(input.substr(firstpos));
		}
	}

	return strings;
}

Int32 GetGroupCount(String& input, const String& groupDelim /* = "" */)
{
	String newDelim = groupDelim + "()";

	Int32	parCount = 0,
			groupCount = 0,
			topGroupCount = 0;

	if (!input.empty())
	{
		Int32	firstpos = input.find_first_not_of(newDelim),
				lastpos = 0,
				temppos = 0;

		while (firstpos != String::npos)
		{
			if (input[firstpos] == '(')
			{
				if (parCount == 0)
					topGroupCount++;

				parCount++;
				groupCount++;

				lastpos = firstpos + 1;

				String temp = input.substr(0, firstpos);

				do
				{
					lastpos = input.find_first_of("()", lastpos);

					if (lastpos != String::npos)
					{
						if (input[lastpos] == '(')
						{
							if (parCount == 0)
								topGroupCount++;

							parCount++;
							lastpos++;
							groupCount++;
						}
						else
						{
							parCount--;
							
							if (parCount > 0)
								lastpos++;
						}
					}
					else
						ASSERT(0);

				} while (parCount > 0);

				lastpos++;
			}
			else if (input[lastpos] == ')')
			{
				if (parCount <= 0)
					ASSERT(0);

				parCount--;

				lastpos++;
			}
			else
				lastpos = firstpos;

			temppos = lastpos;
			
retryp:
			lastpos = input.find_first_of(newDelim, temppos);

			if (lastpos == temppos && input.length() == lastpos + 1)
			{
				temppos++;
				goto retryp;
			}

			if (lastpos != String::npos)
			{
				if (input[lastpos] == '(')
				{
					if (parCount == 0)
						topGroupCount++;
					
					parCount++;
					groupCount++;

					String temp = input.substr(0, lastpos);

					do
					{
						temppos = lastpos + 1;
						lastpos = input.find_first_of("()", temppos);

						if (lastpos != String::npos)
						{
							if (input[lastpos] == '(')
							{
								if (parCount == 0)
									topGroupCount++;

								parCount++;
								groupCount++;
								//lastpos++;
							}
							else
							{
								parCount--;

								/*if (parCount > 0)
									lastpos++;*/
							}
						}
						else
							ASSERT(0);

					} while (parCount > 0);

					temppos = lastpos + 1;

					goto retryp;
				}
				else if (input[lastpos] == ')')
				{
					if (parCount <= 0)
						ASSERT(0);

					parCount--;
				}
				
				firstpos = input.find_first_not_of(newDelim, lastpos);
			}
			else
				break;
		}
	}
	else
		return -1;	//Empty string

	return topGroupCount;
}

TSortedTokenArray TokenizeSort(const String& input, const String& delim, const String& groupDelim /* =  ""*/)
{
	TSortedTokenArray strings(1);

	if (!input.empty() && !delim.empty())
	{
		Int32	firstpos = input.find_first_not_of(delim),
				lastpos = String::npos,
				temppos = String::npos,
				lastdpos = String::npos,
				dindex = String::npos;

		if (firstpos != String::npos)
		{
			lastdpos = temppos = input.find_first_of(delim);

			if (temppos != String::npos)
				strings.Resize(delim.size());
		}

		while (firstpos != String::npos)
		{
			temppos = input.find_first_of(delim, firstpos + 1);

			if (lastdpos != 0)
				lastdpos = temppos;
						
			if (temppos != String::npos)
			{
				if (lastpos != String::npos)
					dindex = delim.find_first_of(input[lastpos]);
				else
					dindex = delim.find_first_of(input[lastdpos]);

				if (firstpos == 0)
				{
					strings[0].Push(input.substr(firstpos, temppos - firstpos));
				}
				else
				{
					strings[dindex].Push(input.substr(firstpos, temppos - firstpos));
				}

				firstpos = input.find_first_not_of(delim, temppos);

				lastpos = temppos;
			}
			else
				break;
		}

		if (firstpos != String::npos)
			strings[0].Push(input.substr(firstpos));
	}

	return strings;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

Bool IsAlpha(const String& input)
{
	if (input.empty())
		return false;

	for (UInt32 i = 0; i < input.size(); i++)
	{
		if (isalpha(input[i]) == 0)
			return false;
	}

	return true;
}

Bool IsAlNum(const String& input, Bool bAllowSlash /* = false */)
{
	if (input.empty())
		return false;

	Int32 slashPos = input.find_first_of("/");

	if (!bAllowSlash && slashPos != String::npos)
		return false;

	if (bAllowSlash)
	{
		//rule out first pos div (no numerator)
		//	/23
		if (slashPos == 0)
			return false;

		//rule out last pos (no denominator)
		//	23/
		if (slashPos == input.size() - 1)
			return false;
	}

	for (UInt32 i = 0; i < input.size(); i++)
	{
		//we've already accounted for bad slash positions
		if (bAllowSlash)
			continue;

		//make sure we account for negative numbers as well
		if (i == 0 && input[0] == '-')
			continue;

		//allow for decimal
		if (input[i] == '.')
			continue;

		if (isalnum(input[i]) == 0)
			return false;
	}

	return true;
}

Bool IsANumber(const String& input, Bool bAllowSlash /* = false */)
{
	if (input.empty())
		return false;

	Int32 slashPos = input.find_first_of("/");

	if (!bAllowSlash && slashPos != String::npos)
		return false;

	if (bAllowSlash)
	{
		//rule out first pos div (no numerator)
		//	/23
		if (slashPos == 0)
			return false;

		//rule out last pos (no denominator)
		//	23/
		if (slashPos == input.size() - 1)
			return false;
	}

	for (UInt32 i = 0; i < input.size(); i++)
	{
		//we've already accounted for bad slash positions
		if (bAllowSlash)
			continue;

		//make sure we account for negative numbers as well
		if (i == 0 && input[0] == '-')
			continue;

		//allow for decimal
		if (input[i] == '.')
			continue;

		if (isdigit(input[i]) == 0)
			return false;
	}

	return true;
}

String Vec3ToString(CVec3* pVec, UInt32 precision /* = 8 */)
{
	String str =	"<" +	
					FloatToString(pVec->x, precision)
					+ "," +
					FloatToString(pVec->y, precision)
					+ "," +
					FloatToString(pVec->z, precision)
					+ ">";
	return str;
}

String IntToString(Int32 val)
{
	String str;
	char temp[32] = {'\0'};

	_itoa_s(val, temp, 32, 10);
	
	str = temp;

	return str;
}

String FloatToString(Float val, UInt32 precision /* = 8 */)
{
	String str;
	char temp[32] = {'\0'};

	_gcvt_s(temp, 32, val, precision);
	
	str = temp;

	return str;
}

Float StringToFloat(String str)
{
	return static_cast<Float>(atof(str.c_str()));
}