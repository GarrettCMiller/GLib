/////////////////////////////////////////////////////////////////////////////
//		File:			Logger.h
//
//		Programmmer:	Garrett Miller (GM)
//
//		Date Created:	2/21/2006 12:49:35 AM
//
//		Description:	A logger than handles regular text and HTML
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include <afxcmn.h>
#include "dxerr.h"

#include "LList.h"
//#include <string>

#include "Timer.h"

//////////////////////////////////////////////////////////////////////////

typedef std::string				String;

//////////////////////////////////////////////////////////////////////////

#define LOGGER

#define DEF_LOGGER_NAME	"Logger"

//////////////////////////////////////////////////////////////////////////

enum ELoggerMode
{
	eLM_Text,		//Plain text
	eLM_OList,		//Ordered list
	eLM_UList,		//Unordered list
	eLM_Table,		//Table

	eLM_Count,

	eLM_Default = eLM_Text,
	eLM_Initial = eLM_Text
};

//////////////////////////////////////////////////////////////////////////

enum ELoggerFormatFlag
{
	eLFF_NewTableData,
	eLFF_NewTableRow,
};

enum ELoggerOutputLevel
{
	eLOL_Off,
	eLOL_Level1,
	eLOL_Level2,
	eLOL_Level3,
	eLOL_Level4,
	eLOL_Verbose,
};

class CRichEditCtrl;

//////////////////////////////////////////////////////////////////////////

class Logger
{
public:
	//Defined below
	struct Error;	

private:
	static Bool				m_bMainLoggerOpen;

	String		 			m_strFileName;

	Bool					m_bValid;
	Bool					m_bOutputToFile;
	Bool					m_bTimeStamp;
	Bool					m_bTimeStampOn;
	Bool					m_bHTML;
	Bool					m_bAutoNewLine;

	HANDLE					m_hFile;
	CRichEditCtrl*			m_pEditBox;

	ELoggerMode				m_eLoggerMode;

	ELoggerOutputLevel		m_eOutputLevel;
	ELoggerOutputLevel		m_eInputLevel;

	SYSTEMTIME				m_tTime;

	char					m_strTimeBuffer[64];

	LinkedList<Error>		m_ErrorList;

	char *GetTimeStamp(Bool getDate = false);

	void AddToLog(CString strString/*String strString*/)
	{
#if 1
		ASSERT(m_pEditBox);	//Make sure we have an edit box
		
		if (m_pEditBox->m_hWnd)
		{
			CString temp;
			m_pEditBox->GetWindowText(temp);
 			temp += strString;

 			Int32 pos = temp.Find('\n');
 			while (pos >= 0)
 			{
				if (pos == 0 || temp[pos - 1] != '\r')
				{
					temp.Insert(pos, '\r');
					pos = temp.Find('\n', pos+2);
				}
				else
					pos = temp.Find('\n', pos+1);
 			}

			//temp.Replace('\n', '\r');
	 		
			m_pEditBox->SetWindowText(temp);

			if (strString[strString.GetLength() - 1] == '\n')
			{
				m_pEditBox->GetParent()->Invalidate(false);
				//m_pEditBox->ScrollWindow(0, 12);
				m_pEditBox->LineScroll(m_pEditBox->GetLineCount() - 50/*m_pEditBox->GetFirstVisibleLine()*/, 0);
			}
		}
#endif
	}

public:
	Bool m_bPrintToConsole;

	//Logger(void);
	Logger(Bool bMainLogger = false, String FileName = "", Bool bHTML = true);
	~Logger(void);

	Bool		OpenNewLog(String FileName, Bool HTML = true, Bool bAppend = false);
	//Bool		OpenNewLogMFC(/*CWnd& pParentWnd,*/ CRichEditCtrl* pEditBox);

	Bool		IsHTML() const	{ return m_bHTML; }
	Bool		IsValid() const { return m_bValid; }

	Bool		BeginList(Bool bOrdered = false);
	Bool		EndList();

	void		LogError(Error& error);
	void		PrintError(Error& error);

	void		AddHeading(String text, UInt32 headingSize = 2);

	ELoggerMode	GetMode() const			{ return m_eLoggerMode; }
	Bool		SetMode(ELoggerMode eMode = eLM_Default, Bool bEndPrevMode = true);

	void		OpenNewLogFile();

	static void	CheckFileSizes();

	Bool		AttachMFC(CRichEditCtrl* pEditBox);
	void		DetachMFC();

	Logger&	operator << (ELoggerFormatFlag flag);
	Logger&	operator << (ELoggerOutputLevel outputLevel);
	
	Logger&	operator << (char cData);
	Logger&	operator << (const wchar_t* strData);
	Logger&	operator << (wchar_t cData);
	Logger&	operator << (const char* strData);
	Logger&	operator << (const String& strData);

	Logger& operator << (const Timer& timer);

	//Logger&	operator << (Int8 nData);
	Logger&	operator << (UInt8 nData);

	Logger&	operator << (Int16 nData);
	Logger&	operator << (UInt16 nData);

	Logger&	operator << (Int32 nData);
	Logger&	operator << (UInt32 nData);

	Logger&	operator << (Int64 nData);
	Logger&	operator << (UInt64 nData);
	
	Logger&	operator << (Float fData);
	
	Logger& operator << (Bool bData);
	
	void Flush();

private:
	void	DumpErrors();

	Bool	BeginTable();
	Bool	EndTable();
	Bool	NewTableRow();
};

//////////////////////////////////////////////////////////////////////////

struct Logger::Error
{
	Error()
		: Name("Empty"), File("None"), Function("None"), Line(UINT_MAX), Count(UINT_MAX), Result(S_OK)
	{		}

	Error(String name, String file = "", String function = "", UInt32 line = 0)
		: Name(name), File(file), Function(function), Line(line), Count(1), Result(S_OK)
	{		}

	Error(HRESULT hr, String name = "", String file = "", String function = "", UInt32 line = 0)
		: Result(hr), File(file), Function(function), Line(line), Count(1)
	{
#ifndef _USRDLL
		if (name.empty())
			Name = DXGetErrorString(hr) + function + file;
		else
			Name = DXGetErrorString(hr) + String(": ") + name;
#else
		Name = DXGetErrorStringA(hr);
#endif
	}

	Bool operator == (const Error& rhs) const
	{
		return (Function == rhs.Function && Line == rhs.Line);
	}

	String			Name;
	String			File;
	String			Function;
	UInt32			Line;
	UInt32			Count;
	HRESULT			Result;
};

//////////////////////////////////////////////////////////////////////////

#define LOG_ERROR(text)					LogError(Logger::Error((text), __FILE__, __FUNCTION__, __LINE__))
#define LOG_ERROR_DX(hr)				LogError(Logger::Error(hr, "", __FILE__, __FUNCTION__, __LINE__))
#define LOG_ERROR_DX_DESC(hr, text)		LogError(Logger::Error(hr, (text), __FILE__, __FUNCTION__, __LINE__))