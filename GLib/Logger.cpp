/////////////////////////////////////////////////////////////////////////////
//		File:			Logger.cpp
//
//		Programmmer:	Garrett Miller (GM)
//
//		Date Created:	2/21/2006 12:49:38 AM
//
//		Description:	A logger than handles regular text and HTML
//
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include ".\logger.h"
#include "D3DWrapper.h"

//////////////////////////////////////////////////////////////////////////

#define MAX_BUFFER_SIZE				255
#define MAX_DIGITS					32

#define USE_TIMESTAMPS				false

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

Bool Logger::m_bMainLoggerOpen		= false;

static const char LineBreak[]		= "<BR>\n\t\t\t";
static const char EndTableData[]	= "</TD>\n";
static const char NewTableData[]	= "\t\t\t\t<TD>";
static const char EndListItem[]		= "</LI>\n";
static const char NewListItem[]		= "\t\t\t<LI>";

//////////////////////////////////////////////////////////////////////////

Logger::Logger(Bool bMainLogger /* = false */, String FileName /* =  */, Bool bHTML /* = true */) : 
	m_bPrintToConsole(false),
	m_bValid(false),
	m_hFile(NULL),
	m_bAutoNewLine(true),
	m_bOutputToFile(/*false*/true),
	m_pEditBox(NULL)
{	
	m_eLoggerMode = eLM_Initial;

	if (FileName.empty())
	{
		m_bOutputToFile = true;

		if (!m_bMainLoggerOpen && bMainLogger)
		{
			m_bMainLoggerOpen = true;

			GetLocalTime(&m_tTime);

			OpenNewLog(DEF_LOGGER_NAME, bHTML);

			//m_LoggerList.Add(*this);
		}
	}
	else
	{
		GetLocalTime(&m_tTime);

		OpenNewLog(FileName, bHTML);

		//m_LoggerList.Add(*this);
	}
}

//////////////////////////////////////////////////////////////////////////

Logger::~Logger(void)
{
	if (!m_bValid)
		return;

	if (m_pEditBox)
	{
		//(*this) << "Closing MFC logger...\n";
		DetachMFC();
	}
	else
	{
		if (!m_bHTML)
		{
			(*this) <<	"\n******************************\n"
						"Logger closed successfully!\n"
						"******************************";
		}
		else
		{
			DumpErrors();

			(*this) <<	"</BODY>\n</HTML>";
		}

		CloseHandle(m_hFile);

		m_hFile = NULL;
	}

	m_bValid = false;

	m_ErrorList.Clear();
}

//////////////////////////////////////////////////////////////////////////

void Logger::DumpErrors()
{
	LinkedList<Error>::Iterator i(m_ErrorList);
	Error error;

	if (m_ErrorList.Size() == 0)
		return;

	if (GetMode() != eLM_Text)
		SetMode(eLM_Text);

	(*this) << "\n\n\n";

	AddHeading("Error Dump");

	SetMode(eLM_Table);

	(*this)	<< "Name"		<< eLFF_NewTableData 
			<< "Function"	<< eLFF_NewTableData
			<< "Line"		<< eLFF_NewTableData 
			<< "Count"		<< eLFF_NewTableData 
			<< "DX String"	<< eLFF_NewTableData 
			<< "DX Desc"	<< eLFF_NewTableData
			<< "File"		<< eLFF_NewTableRow;

	for (i.Begin(); !i.End(); ++i)
	{
		error = i.Current();

		(*this)	<< error.Name							<< eLFF_NewTableData
				<< error.Function						<< eLFF_NewTableData 
				<< error.Line							<< eLFF_NewTableData
				<< error.Count							<< eLFF_NewTableData
				<< (FAILED(error.Result) ? DXGetErrorString(error.Result) : "N/A")		<< eLFF_NewTableData
				<< (FAILED(error.Result) ? DXGetErrorDescription(error.Result) : "N/A")	<< eLFF_NewTableData
				<< error.File							<< eLFF_NewTableRow;
	}

	SetMode(eLM_Text);
}

//////////////////////////////////////////////////////////////////////////

Bool Logger::AttachMFC(CRichEditCtrl* pEditBox)
{
	if (OpenNewLog("MFC Logger", true) == false)
		return false;

	ASSERT(pEditBox);

	m_pEditBox = pEditBox;

	m_bHTML = false;
	m_bValid = true;

	AddToLog("Attaching edit control to logger...\n");

	AddToLog("MFC log connected!\n");

	return true;
}

//////////////////////////////////////////////////////////////////////////

void Logger::DetachMFC()
{
	m_pEditBox = NULL;
}

//////////////////////////////////////////////////////////////////////////

Bool Logger::OpenNewLog(String FileName, Bool HTML, Bool bAppend)
{
	ASSERT(!FileName.empty());

	m_strFileName = FileName;

	if (m_bOutputToFile)
	{
		char filename[256] = {'\0'};

		strncpy_s(filename, FileName.c_str(), 256);

		if (HTML)
			strncat_s(filename, ".html", 256);
		else
			strncat_s(filename, ".log", 256);

		UInt32 mode = CREATE_ALWAYS;

		if (bAppend)
			mode = OPEN_ALWAYS;

		m_hFile = CreateFile(filename, 
			GENERIC_WRITE,
			FILE_SHARE_READ,
			NULL,
			mode,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

		if (m_hFile == INVALID_HANDLE_VALUE)
		{
			m_bValid = false;
			UInt32 err = GetLastError();
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, 0, filename, 256, NULL);
			return false;
		}
		else
			m_bValid = true;

		m_bHTML = HTML;

		GetLocalTime(&m_tTime);

		GetTimeStamp(true);

		m_bTimeStamp = m_bTimeStampOn = USE_TIMESTAMPS;

		if (!m_bHTML)
		{
			(*this) <<	"******************************\n\r"
				"Logger opened successfully!\n"
				<< m_strTimeBuffer << "\n"
				"******************************\n\n";
		}
		else
		{
			(*this) <<	"<HTML>\n\t<HEAD>\n\t\t<TITLE>Log file: \"" << FileName.c_str() << "\" </TITLE>\n\t</HEAD>\n\t<BODY BGCOLOR=\"#000000\" TEXT=\"#008080\">\n\t\t";
			(*this) <<	"<H1>Log file: \"" << FileName.c_str() << "\"</H1>\n";
			(*this) <<	"\t\t<H3>Built at: " << m_strTimeBuffer << "</H3>\n\t\t";

			switch (m_eLoggerMode)
			{
			case eLM_OList:
				BeginList(true);
				break;

			case eLM_UList:
				BeginList();
				break;

			case eLM_Table:
				BeginTable();
				break;
			}
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

void Logger::LogError(Error& error)
{
	LinkedList<Error>::Iterator	i(m_ErrorList);

	for (i.Begin(); !i.End(); ++i)
	{
		if (error == i.Current())
		{
			i.Current().Count++;
			return;
		}
	}

	m_ErrorList.Add(error);

	PrintError(error);
}

//////////////////////////////////////////////////////////////////////////

void Logger::PrintError(Error& error)
{
	//m_bAutoNewLine = false;

	Int32 pos = -1;

	if ((pos = error.Name.find('\n')) != -1)
		error.Name.replace(pos, 1, 1, '\0');

	if (m_bHTML)
		(*this) << "<font color=\"red\"><b>" << error.Name << " in " << error.Function 
				<< " (line " << error.Line << ") (" << error.Count 
				<< " times)</b></font> \n";
	else
		(*this) << error.Name << " in " << error.Function 
				<< " (line " << error.Line << ") (" << error.Count 
				<< " times)\n";

	//m_bAutoNewLine = true;
}

//////////////////////////////////////////////////////////////////////////

char *Logger::GetTimeStamp(Bool getDate)
{
	GetLocalTime(&m_tTime);
	
	if (getDate)
		sprintf_s(m_strTimeBuffer, "%.2d\\%.2d\\%d\t%.2d:%.2d:%.2d.%.3d\t",
											m_tTime.wMonth, m_tTime.wDay, m_tTime.wYear,
											m_tTime.wHour, m_tTime.wMinute, m_tTime.wSecond, m_tTime.wMilliseconds);
	else
		sprintf_s(m_strTimeBuffer, "%.2d:%.2d:%.2d.%.3d\t",
											m_tTime.wHour, m_tTime.wMinute, m_tTime.wSecond, m_tTime.wMilliseconds);

	return (m_strTimeBuffer);
}

//////////////////////////////////////////////////////////////////////////

Logger &Logger::operator << (wchar_t cData)
{
	if (!m_bValid)
		return *this;

	if (m_eOutputLevel < m_eInputLevel)
		return *this;

	DWORD bytesWritten;

	if (m_pEditBox)
	{
		//AddToLog(String(cData));
	}

	if (m_bOutputToFile)
	{
		if (m_bHTML && cData == L'\n')
		{

			if (m_bTimeStamp)
			{
				GetTimeStamp();
				//WriteFile(m_hFile, &m_strTimeBuffer, (UInt32) wcslen(m_strTimeBuffer), &bytesWritten, NULL);
			}

			switch (m_eLoggerMode)
			{
			case eLM_Text:
				WriteFile(m_hFile, &LineBreak, 7, &bytesWritten, NULL);
				break;

			case eLM_OList:
			case eLM_UList:
				//WriteFile(m_hFile, &NewListItem, (UInt32) wcslen(NewListItem), &bytesWritten, NULL);
				break;

			case eLM_Table:
				//WriteFile(m_hFile, &NewTableData, (UInt32) wcslen(NewTableData), &bytesWritten, NULL);
				//NewTableRow();
				break;
			}

			return *this;
		}

		WriteFile(m_hFile, &cData, sizeof(wchar_t), &bytesWritten, NULL);
	}

#ifdef _DOOMEROIDS_
	if (m_bPrintToConsole)
		_cprintf("%c", cData);
#endif

	return *this;
}

//////////////////////////////////////////////////////////////////////////

Logger &Logger::operator << (char cData)
{
	if (!m_bValid)
		return *this;

	if (m_eOutputLevel < m_eInputLevel)
		return *this;

	DWORD bytesWritten;

	if (m_pEditBox)
	{
		//AddToLog(String(cData));
	}

	if (m_bOutputToFile)
	{
		if (m_bHTML && cData == '\n')
		{

			if (m_bTimeStamp)
			{
				GetTimeStamp();
				WriteFile(m_hFile, &m_strTimeBuffer, (UInt32) strlen(m_strTimeBuffer), &bytesWritten, NULL);
			}

			switch (m_eLoggerMode)
			{
			case eLM_Text:
				WriteFile(m_hFile, &LineBreak, 7, &bytesWritten, NULL);
				break;

			case eLM_OList:
			case eLM_UList:
				WriteFile(m_hFile, &NewListItem, (UInt32) strlen(NewListItem), &bytesWritten, NULL);
				break;

			case eLM_Table:
				//WriteFile(m_hFile, &NewTableData, (UInt32) strlen(NewTableData), &bytesWritten, NULL);
				//NewTableRow();
				break;
			}

			return *this;
		}

		WriteFile(m_hFile, &cData, sizeof(char), &bytesWritten, NULL);
	}

#ifdef _DOOMEROIDS_
	if (m_bPrintToConsole)
		_cprintf("%c", cData);
#endif

	return *this;
}

//////////////////////////////////////////////////////////////////////////

Logger &Logger::operator << (const String& strData)
{
	return ((*this) << strData.c_str());
}

//////////////////////////////////////////////////////////////////////////

Logger &Logger::operator << (const wchar_t* strData)
{
	if (!m_bValid || !strData)
		return *this;

	if (m_eOutputLevel < m_eInputLevel)
		return *this;

	DWORD	bytesWritten,
			bytesToWrite = (DWORD) wcslen(strData);

	/*if (!wcscmp(strData, "\t\t\t\t"))
		return *this;*/

	wchar_t	last = strData[bytesToWrite - 1],
			lastn = strData[bytesToWrite - 2];

	if (m_pEditBox)
	{
		//AddToLog(String(strData));
	}
	
	if (m_bOutputToFile)
	{
		if (last == L'\n')
		{
			if (m_bHTML && m_bAutoNewLine && lastn != L'>')
			{
				WriteFile(m_hFile, strData, bytesToWrite - 1, &bytesWritten, NULL);

				switch (m_eLoggerMode)
				{
				case eLM_Text:
					WriteFile(m_hFile, &LineBreak, 7, &bytesWritten, NULL);
					break;

				case eLM_OList:
				case eLM_UList:
					(*this) << EndListItem;
					bytesToWrite--;
					//WriteFile(m_hFile, &NewListItem, (UInt32) wcslen(NewListItem), &bytesWritten, NULL);
					break;

				case eLM_Table:
					//WriteFile(m_hFile, &NewTableData, wcslen(NewTableData), &bytesWritten, NULL);
					//NewTableRow();
					break;
				}

				return *this;
			}
		}

		WriteFile(m_hFile, strData, bytesToWrite, &bytesWritten, NULL);
	}

#ifdef _DOOMEROIDS_
	if (m_bPrintToConsole)
		_cprintf("%s", strData);
#endif

	return *this;
}

//////////////////////////////////////////////////////////////////////////

Logger &Logger::operator << (const char* strData)
{
	if (!m_bValid || !strData)
		return *this;

	if (m_eOutputLevel < m_eInputLevel)
		return *this;

	DWORD	bytesWritten,
			bytesToWrite = (DWORD) strlen(strData);

	/*if (!strcmp(strData, "\t\t\t\t"))
		return *this;*/

	char	last = strData[bytesToWrite - 1],
			lastn = strData[bytesToWrite - 2];

	if (m_pEditBox)
	{
		AddToLog(strData);
	}

	if (m_bOutputToFile)
	{
		if (last == '\n')
		{
			if (m_bHTML && m_bAutoNewLine && lastn != '>')
			{
				WriteFile(m_hFile, strData, bytesToWrite - 1, &bytesWritten, NULL);

				switch (m_eLoggerMode)
				{
				case eLM_Text:
					WriteFile(m_hFile, &LineBreak, 7, &bytesWritten, NULL);
					break;

				case eLM_OList:
				case eLM_UList:
					(*this) << EndListItem;
					bytesToWrite--;
					WriteFile(m_hFile, &NewListItem, (UInt32) strlen(NewListItem), &bytesWritten, NULL);
					break;

				case eLM_Table:
					//WriteFile(m_hFile, &NewTableData, strlen(NewTableData), &bytesWritten, NULL);
					//NewTableRow();
					break;
				}

				return *this;
			}
		}

		WriteFile(m_hFile, strData, bytesToWrite, &bytesWritten, NULL);
	}

#ifdef _DOOMEROIDS_
	if (m_bPrintToConsole)
		_cprintf("%s", strData);
#endif

	return *this;
}

//////////////////////////////////////////////////////////////////////////

Logger	&Logger::operator << (const Timer& timer)
{
	return ((*this) << timer.Get());
}

//////////////////////////////////////////////////////////////////////////

#if 0
Logger &Logger::operator << (Int8 nData)
{
	static char buffer[MAX_BUFFER_SIZE];

	if (!m_bValid)
		return *this;

	if (m_eOutputLevel < m_eInputLevel)
		return *this;

	DWORD bytesWritten;

	/*if (m_bTimeStamp)
	{
		GetTimeStamp();
		WriteFile(m_hFile, &m_strTimeBuffer, strlen(m_strTimeBuffer), &bytesWritten, NULL);
	}*/

	_itoa_s(static_cast<UInt32>(nData), buffer, MAX_BUFFER_SIZE, 10);

	if (m_pEditBox)
		AddToLog(buffer);
	if (m_bOutputToFile)
		WriteFile(m_hFile, buffer, (UInt32) strlen(buffer), &bytesWritten, NULL);

#ifdef _DOOMEROIDS_
	if (m_bPrintToConsole)
		_cprintf("%d", nData);
#endif

	return *this;
}

#endif

//////////////////////////////////////////////////////////////////////////

Logger &Logger::operator << (UInt8 nData)
{
	static char buffer[MAX_BUFFER_SIZE];

	if (!m_bValid)
		return *this;

	if (m_eOutputLevel < m_eInputLevel)
		return *this;

	DWORD bytesWritten;

	/*if (m_bTimeStamp)
	{
		GetTimeStamp();
		WriteFile(m_hFile, &m_strTimeBuffer, strlen(m_strTimeBuffer), &bytesWritten, NULL);
	}*/

	_itoa_s(static_cast<UInt32>(nData), buffer, MAX_BUFFER_SIZE, 10);

	if (m_pEditBox)
		AddToLog(buffer);
	if (m_bOutputToFile)
		WriteFile(m_hFile, buffer, (UInt32) strlen(buffer), &bytesWritten, NULL);

#ifdef _DOOMEROIDS_
	if (m_bPrintToConsole)
		_cprintf("%d", nData);
#endif

	return *this;
}

//////////////////////////////////////////////////////////////////////////

Logger &Logger::operator << (Int16 nData)
{
	static char buffer[MAX_BUFFER_SIZE];

	if (!m_bValid)
		return *this;

	if (m_eOutputLevel < m_eInputLevel)
		return *this;

	DWORD bytesWritten;

	/*if (m_bTimeStamp)
	{
		GetTimeStamp();
		WriteFile(m_hFile, &m_strTimeBuffer, strlen(m_strTimeBuffer), &bytesWritten, NULL);
	}*/

	_itoa_s(static_cast<UInt32>(nData), buffer, MAX_BUFFER_SIZE, 10);

	if (m_pEditBox)
		AddToLog(buffer);
	if (m_bOutputToFile)
		WriteFile(m_hFile, buffer, (UInt32) strlen(buffer), &bytesWritten, NULL);

#ifdef _DOOMEROIDS_
	if (m_bPrintToConsole)
		_cprintf("%d", nData);
#endif

	return *this;
}

//////////////////////////////////////////////////////////////////////////

Logger &Logger::operator << (UInt16 nData)
{
	static char buffer[MAX_BUFFER_SIZE];

	if (!m_bValid)
		return *this;

	if (m_eOutputLevel < m_eInputLevel)
		return *this;

	DWORD bytesWritten;

	/*if (m_bTimeStamp)
	{
		GetTimeStamp();
		WriteFile(m_hFile, &m_strTimeBuffer, strlen(m_strTimeBuffer), &bytesWritten, NULL);
	}*/

	_itoa_s(static_cast<UInt32>(nData), buffer, MAX_BUFFER_SIZE, 10);

	if (m_pEditBox)
		AddToLog(buffer);
	if (m_bOutputToFile)
		WriteFile(m_hFile, buffer, (UInt32) strlen(buffer), &bytesWritten, NULL);

#ifdef _DOOMEROIDS_
	if (m_bPrintToConsole)
		_cprintf("%d", nData);
#endif

	return *this;
}

//////////////////////////////////////////////////////////////////////////

Logger &Logger::operator << (Int32 nData)
{
	static char buffer[MAX_BUFFER_SIZE];

	if (!m_bValid)
		return *this;

	if (m_eOutputLevel < m_eInputLevel)
		return *this;

	DWORD bytesWritten;

	/*if (m_bTimeStamp)
	{
		GetTimeStamp();
		WriteFile(m_hFile, &m_strTimeBuffer, strlen(m_strTimeBuffer), &bytesWritten, NULL);
	}*/

	_itoa_s(nData, buffer, MAX_BUFFER_SIZE, 10);

	if (m_pEditBox)
		AddToLog(buffer);
	if (m_bOutputToFile)
		WriteFile(m_hFile, buffer, (UInt32) strlen(buffer), &bytesWritten, NULL);

#ifdef _DOOMEROIDS_
	if (m_bPrintToConsole)
		_cprintf("%d", nData);
#endif

	return *this;
}

//////////////////////////////////////////////////////////////////////////

Logger &Logger::operator << (UInt32 nData)
{
	static char buffer[MAX_BUFFER_SIZE];

	if (!m_bValid)
		return *this;

	if (m_eOutputLevel < m_eInputLevel)
		return *this;

	DWORD bytesWritten;

	/*if (m_bTimeStamp)
	{
		GetTimeStamp();
		WriteFile(m_hFile, &m_strTimeBuffer, strlen(m_strTimeBuffer), &bytesWritten, NULL);
	}*/

	_ui64toa_s(static_cast<UInt64>(nData), buffer, MAX_BUFFER_SIZE, 10);

	if (m_pEditBox)
		AddToLog(buffer);
	if (m_bOutputToFile)
		WriteFile(m_hFile, buffer, (UInt32) strlen(buffer), &bytesWritten, NULL);

#ifdef _DOOMEROIDS_
	if (m_bPrintToConsole)
		_cprintf("%d", nData);
#endif

	return *this;
}

//////////////////////////////////////////////////////////////////////////

Logger &Logger::operator << (Int64 nData)
{
	static char buffer[MAX_BUFFER_SIZE];

	if (!m_bValid)
		return *this;

	if (m_eOutputLevel < m_eInputLevel)
		return *this;

	DWORD bytesWritten;

	/*if (m_bTimeStamp)
	{
		GetTimeStamp();
		WriteFile(m_hFile, &m_strTimeBuffer, strlen(m_strTimeBuffer), &bytesWritten, NULL);
	}*/

	_i64toa_s(nData, buffer, MAX_BUFFER_SIZE, 10);

	if (m_pEditBox)
		AddToLog(buffer);
	if (m_bOutputToFile)
		WriteFile(m_hFile, buffer, (UInt32) strlen(buffer), &bytesWritten, NULL);

#ifdef _DOOMEROIDS_
	if (m_bPrintToConsole)
		_cprintf("%d", nData);
#endif

	return *this;
}

//////////////////////////////////////////////////////////////////////////

Logger &Logger::operator << (UInt64 nData)
{
	static char buffer[MAX_BUFFER_SIZE];

	if (!m_bValid)
		return *this;
	
	if (m_eOutputLevel < m_eInputLevel)
		return *this;

	DWORD bytesWritten;

	/*if (m_bTimeStamp)
	{
		GetTimeStamp();
		WriteFile(m_hFile, &m_strTimeBuffer, strlen(m_strTimeBuffer), &bytesWritten, NULL);
	}*/

	_ui64toa_s(nData, buffer, MAX_BUFFER_SIZE, 10);

	if (m_pEditBox)
		AddToLog(buffer);
	if (m_bOutputToFile)
		WriteFile(m_hFile, buffer, (UInt32) strlen(buffer), &bytesWritten, NULL);

#ifdef _DOOMEROIDS_
	if (m_bPrintToConsole)
		_cprintf("%d", nData);
#endif

	return *this;
}

//////////////////////////////////////////////////////////////////////////

Logger &Logger::operator << (Float fData)
{
	char buffer[32];

	if (!m_bValid)
		return *this;

	if (m_eOutputLevel < m_eInputLevel)
		return *this;

	DWORD bytesWritten;

	/*if (m_bTimeStamp)
	{
		GetTimeStamp();
		WriteFile(m_hFile, &m_strTimeBuffer, strlen(m_strTimeBuffer), &bytesWritten, NULL);
	}*/

	_gcvt_s(buffer, 32, fData, /*MAX_DIGITS*/7);

	if (m_pEditBox)
		AddToLog(buffer);
	if (m_bOutputToFile)
		WriteFile(m_hFile, buffer, (UInt32) strlen(buffer), &bytesWritten, NULL);

#ifdef _DOOMEROIDS_
	if (m_bPrintToConsole)
		_cprintf("%f", fData);
#endif

	return *this;
}

//////////////////////////////////////////////////////////////////////////

Logger &Logger::operator << (Bool bData)
{
	char buffer[6];
	DWORD bytesWritten;

	if (!m_bValid)
		return *this;

	if (m_eOutputLevel < m_eInputLevel)
		return *this;

	strcpy_s(buffer, 6, (bData ? "true\0" : "false\0"));
	
	if (m_pEditBox)
		AddToLog(buffer);
	if (m_bOutputToFile)
		WriteFile(m_hFile, buffer, (UInt32) strlen(buffer), &bytesWritten, NULL);

#ifdef _DOOMEROIDS_
	if (m_bPrintToConsole)
		_cprintf("%f", bData);
#endif

	return *this;
}


//////////////////////////////////////////////////////////////////////////

void Logger::Flush()
{
	DWORD bytesWritten;

	(*this) << "\n\n\t";

	GetTimeStamp();
	WriteFile(m_hFile, &m_strTimeBuffer, (UInt32) strlen(m_strTimeBuffer), &bytesWritten, NULL);

	(*this) << "Emergency flush!\n\n";

	DumpErrors();

	FlushFileBuffers(m_hFile);
}

//////////////////////////////////////////////////////////////////////////

Bool Logger::BeginTable()
{
	if (!m_bHTML)
		return false;

	(*this) <<	"\t\t<TABLE BORDER=1 CELLPADDING=10>\n\t\t\t<TR>\n\t\t\t\t<TD>";			

	m_eLoggerMode = eLM_Table;

	return true;
}

//////////////////////////////////////////////////////////////////////////

Bool Logger::EndTable()
{
	if (!m_bHTML || m_eLoggerMode != eLM_Table)
		return false;

	(*this) <<	"\t\t\t\t</TD>\n\t\t\t</TR>\n\t\t</TABLE>\n\t\t";

	m_eLoggerMode = eLM_Default;

	return true;
}

//////////////////////////////////////////////////////////////////////////

Bool Logger::NewTableRow()
{
	if (!m_bHTML || m_eLoggerMode != eLM_Table)
		return false;

	(*this) <<	"</TD>\n\t\t\t</TR>\n\t\t\t<TR>\n\t\t\t\t<TD>";

	return true;
}

//////////////////////////////////////////////////////////////////////////

Bool Logger::BeginList(Bool bOrdered)
{
	if (!m_bHTML)
		return false;

	if (bOrdered)
	{
		(*this) <<	"\t\t<OL>\n" << NewListItem;
		m_eLoggerMode = eLM_OList;
	}
	else
	{
		(*this) <<	"\t\t<UL>\n" << NewListItem;
		m_eLoggerMode = eLM_UList;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

Bool Logger::EndList()
{
	if (!m_bHTML || (m_eLoggerMode != eLM_OList && m_eLoggerMode != eLM_UList))
		return false;

	if (m_eLoggerMode == eLM_OList)
		(*this) <<	"\t\t\t</LI>\n\t\t</OL>\n\t\t";
	else
		(*this) <<	"\t\t\t</LI>\n\t\t</UL>\n\t\t";

	m_eLoggerMode = eLM_Default;

	return true;
}

//////////////////////////////////////////////////////////////////////////

Bool Logger::SetMode(ELoggerMode eMode, Bool bEndPrevMode)
{
	if (!m_bHTML)
		return false;

	if (bEndPrevMode)
	{
		switch (m_eLoggerMode)
		{
		case eLM_OList:
		case eLM_UList:
			EndList();
			break;

		case eLM_Table:
			EndTable();
			break;
		}
	}

	m_eLoggerMode = eMode;

	switch (m_eLoggerMode)
	{
	case eLM_OList:
		BeginList(true);
		break;

	case eLM_UList:
		BeginList(false);
		break;

	case eLM_Table:
		BeginTable();
		break;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

void Logger::AddHeading(String text, UInt32 headingSize)
{
	if (!m_bHTML || GetMode() != eLM_Text)
		return;

	(*this) <<	"<H" << headingSize << '>' << text << "</H" << headingSize << ">\n";
}

//////////////////////////////////////////////////////////////////////////

Logger &Logger::operator << (ELoggerFormatFlag flag)
{
	switch (flag)
	{
	case eLFF_NewTableData:
		(*this) << EndTableData << NewTableData;
		break;

	case eLFF_NewTableRow:
		NewTableRow();
		break;

	default:
		ASSERT(false);
		return *this;
	}

	return *this;
}

//////////////////////////////////////////////////////////////////////////

Logger &Logger::operator << (ELoggerOutputLevel outputLevel)
{
	//Just bear with me on the output/input level
	m_eInputLevel = outputLevel;

	return *this;
}

//////////////////////////////////////////////////////////////////////////

void Logger::OpenNewLogFile()
{
	static UInt32 LogCount = 0;

	(*this) << "File size limit reached! Trying to open new log entry file!\n";

	Flush();

	CloseHandle(m_hFile);

	//strncat(m_strFileName, "_%.2d", ++LogCount);

	OpenNewLog(m_strFileName, m_bHTML);
}

//////////////////////////////////////////////////////////////////////////

void Logger::CheckFileSizes()
{
 	//LinkedList<Logger&>::Iterator i;
 	//UInt32	size = 0;
 
 	//for (i.Begin(); !i.End(); ++i)
 	//{
 	//	GetFileSize(i.Current().m_hFile, &size);
 
 	//	if (size > 1024 * 1024 * 10)		//10 MB
		//	i.Current().OpenNewLogFile();
 	//}
}

//////////////////////////////////////////////////////////////////////////