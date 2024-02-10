///////////////////////////////////////////////////////////////////////////
//		File:				DPakFile.cpp
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		Like a ZIP file, but made by me :)
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "DPakFile.h"

//////////////////////////////////////////////////////////////////////////

//CDPakFile pakFile;

TStringArray CDPakFile::m_SearchDirectories;

//////////////////////////////////////////////////////////////////////////

CDPakFile::CDPakFile(String  filename) : 
	m_PakFile(NULL),
	m_Current(NULL),
	m_bSearchLocally(true)
{
	if (filename.empty())
		filename = "Data.dpk";

	m_PakFileName = filename;

	m_SearchDirectories.Resize(5);
}

//////////////////////////////////////////////////////////////////////////

CDPakFile::~CDPakFile(void)
{
	ReleaseAllResources(true);
	m_SearchDirectories.ReleaseBuffer();
}

//////////////////////////////////////////////////////////////////////////

void CDPakFile::ReleaseAllResources(Bool ClearFileList)
{
	if (m_PakFile)
	{
		fclose(m_PakFile);
		m_PakFile = NULL;
	}

	if (m_Current)
	{
		fclose(m_Current);
		m_Current = NULL;
	}

	if (!m_FileData.IsEmpty())
	{
		DPakFile f;

		for (UInt32 i = 0; i < m_FileData.Size(); i++)
		{
			f = m_FileData[i];

			if (f.FileData)
			{
				delete [] f.FileData;
				f.FileData = NULL;
			}
		}

		m_FileData.Clear();
	}

	if (!m_FileItems.IsEmpty())
	{
		m_FileItems.Clear();
	}

	if (ClearFileList)
		m_Files.Clear();
}

//////////////////////////////////////////////////////////////////////////

void CDPakFile::Initialize()
{
	ZeroMemory(&m_Header, sizeof(DPakFileHeader));
	
	ReleaseAllResources(false);

	m_CurrentFileOffset = m_TotalBytesToWrite = m_TotalBytesWritten = 
		m_FileBytesToWrite = m_FileBytesWritten = m_FileCount = 0;
}

//////////////////////////////////////////////////////////////////////////

Bool CDPakFile::OpenPakFile(Bool Read)
{
	fopen_s(&m_PakFile, m_PakFileName.c_str(), (Read ? "rb" : "wb"));

	if (!m_PakFile)
	{
		m_eErrorCode = DPakError_OpenPakFileFailed;
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

Bool CDPakFile::WritePakFile()
{
	if (m_Files.Size() == 0)
	{
		m_eErrorCode = DPakError_NoFilesToPack;
		return false;
	}

	Initialize();

	if (!OpenPakFile(false))
		return false;

	m_FileCount				= m_Files.Size();

	m_Header.Magic			= FILE_MAGIC;
	m_Header.VersionMajor	= CURRENT_DPAK_VERSION_MAJOR;
	m_Header.VersionMinor	= CURRENT_DPAK_VERSION_MINOR;
	m_Header.FileCount		= m_FileCount;
	//m_Header.DirOffset		= -1;

	m_CurrentFileOffset		= sizeof(DPakFileHeader) + (m_FileCount * sizeof(DPakFileHeaderItem));
	m_TotalBytesToWrite		= m_CurrentFileOffset;

	if (!ReadInAllFiles())
	{
		ReleaseAllResources();
		return false;
	}

	m_Header.PakFileSize	= m_TotalBytesToWrite;

	fwrite(&m_Header, sizeof(DPakFileHeader), 1, m_PakFile);

	m_TotalBytesWritten		+= sizeof(DPakFileHeader);

	if (!WriteFilesToPak())
	{
		ReleaseAllResources();
		return false;
	}

	if (m_TotalBytesWritten != m_TotalBytesToWrite)
		MessageBox(NULL, "Byte counts are different!!", "Error", MB_OK);

	ReleaseAllResources();

	return true;
}

//////////////////////////////////////////////////////////////////////////

Bool CDPakFile::ReadInAllFiles()
{
	String					path;
	UInt32					filesize;

	for (UInt32 i = 0, fileNum = 0; i < m_Files.Size(); i++, fileNum++)
	{
		path = m_Files[i];

		fopen_s(&m_Current, path.c_str(), "rb");

		if (!m_Current)
		{
			switch (MessageBox(NULL, "Couldn't open file! Continue with remaining files?", path.c_str(), MB_YESNO))
			{
			case IDYES:
				continue;

			case IDNO:
				ReleaseAllResources();
				m_eErrorCode = DPakError_OpenFileFailed;
				return false;
			}
		}

		//
		//Get file size
		//
		fseek(m_Current, 0, SEEK_END);
		filesize = ftell(m_Current);
		fseek(m_Current, 0, SEEK_SET);
		ASSERT(filesize);

		DPakFileHeaderItem hi;
		DPakFile pf;

		pf.FileData					= new BYTE [filesize];

		hi.FileSizeUncompressed		= filesize;
		hi.Offset					= m_CurrentFileOffset;

		pf.FileSize					= filesize;

		m_CurrentFileOffset			+= filesize;
		m_TotalBytesToWrite			+= filesize;
		m_FileBytesToWrite			+= filesize;

		CString basePath			= m_BasePath.c_str();
		CString tempPath			= path.c_str();
		CString  newpath			= tempPath.SpanIncluding(basePath);
		CString  filename;

		int nSize = tempPath.GetLength();

		int indexOfSlash = tempPath.ReverseFind('\\');
		filename = tempPath.Right(nSize - indexOfSlash - 1);

		if (filename.IsEmpty())
			filename = newpath.SpanIncluding("\\");

		strncpy_s(hi.FilePath, MAX_FILEPATH_LENGTH, newpath, MAX_FILEPATH_LENGTH);
		strncpy_s(hi.FileName, MAX_FILEPATH_LENGTH, filename, MAX_FILENAME_LENGTH);

		strncpy_s(pf.Name, MAX_FILEPATH_LENGTH, filename, MAX_FILENAME_LENGTH);

		fread(pf.FileData, 1, filesize, m_Current);

		fclose(m_Current);

		m_FileItems.Push(hi);
		m_FileData.Push(pf);
		//m_FileItems.Add(hi);
		//m_FileData.Add(pf);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

Bool CDPakFile::WriteFilesToPak()
{
	UInt32 fileNum;
	UInt32 itemsWritten = 0;

	//
	//First write the table of contents
	//
	for (fileNum = 0; fileNum < m_FileCount; fileNum++)
	{
		fwrite(&m_FileItems[fileNum], sizeof(DPakFileHeaderItem), 1, m_PakFile);
		m_TotalBytesWritten += sizeof(DPakFileHeaderItem);
	}

	//
	//Then write file data
	//
	for (fileNum = 0; fileNum < m_FileCount; fileNum++)
	{
		itemsWritten = (UInt32) fwrite(m_FileData[fileNum].FileData, 1, m_FileItems[fileNum].FileSizeUncompressed, m_PakFile);
		m_TotalBytesWritten += m_FileItems[fileNum].FileSizeUncompressed;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

Bool CDPakFile::ReadPakFile(Bool ReadAllFiles)
{
	if (!OpenPakFile(true))
		return false;

	fread(&m_Header, sizeof(DPakFileHeader), 1, m_PakFile);

	if (m_Header.Magic != FILE_MAGIC)
	{
		m_eErrorCode = DPakError_InvalidPakFile;
		ReleaseAllResources();
		return false;
	}

	m_FileCount			= m_Header.FileCount;
	m_CurrentFileOffset	= sizeof(DPakFileHeader) + (m_FileCount * sizeof(DPakFileHeaderItem));

	if (!ReadFileHeaders())
		return false;

	if (ReadAllFiles && !ReadAllFilesFromPak())
		return false;

	//ReleaseAllResources();

	return true;
}

//////////////////////////////////////////////////////////////////////////

Bool CDPakFile::ReadFileHeaders()
{
	String  path;
	UInt32 fileNum;
	DPakFileHeaderItem hi;

	for (fileNum = 0; fileNum < m_FileCount; fileNum++)
	{
		fread(&hi, sizeof(DPakFileHeaderItem), 1, m_PakFile);
		m_FileItems.Push(hi);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

Bool CDPakFile::ReadAllFilesFromPak()
{
	UInt32 fileNum;
	DPakFile pf;
	DPakFileHeaderItem hi;

	for (fileNum = 0; fileNum < m_FileCount; fileNum++)
	{
		hi = m_FileItems[fileNum];

		strncpy_s(pf.Name, MAX_FILEPATH_LENGTH, hi.FileName, MAX_FILENAME_LENGTH);

		pf.FileSize = hi.FileSizeUncompressed;
		pf.FileData = new BYTE [pf.FileSize];

		ASSERT(pf.FileData);

		fread(pf.FileData, 1, pf.FileSize, m_PakFile);

		m_FileData.Push(pf);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

DPakFile CDPakFile::LoadFileByName(const String& FileName)
{
	UInt32 fileNum;
	DPakFile pf;
	DPakFileHeaderItem hi;

	for (fileNum = 0; fileNum < m_FileCount; fileNum++)
	{
		hi = m_FileItems[fileNum];

		if (_stricmp(hi.FileName, FileName.c_str()) == 0)
		{
			strncpy_s(pf.Name, MAX_FILENAME_LENGTH, hi.FileName, MAX_FILENAME_LENGTH);

			pf.FileSize = hi.FileSizeUncompressed;
			pf.FileData = new BYTE [pf.FileSize];

			ASSERT(pf.FileData);

			fseek(m_PakFile, hi.Offset, SEEK_SET);
			fread(pf.FileData, 1, pf.FileSize, m_PakFile);

			m_FileData.Push(pf);

			break;
		}
	}

	return pf;
}

//////////////////////////////////////////////////////////////////////////

void CDPakFile::AddFile(const String& FileName)
{
	ASSERT(!FileName.empty());
	m_Files.Push(FileName);
}

//////////////////////////////////////////////////////////////////////////

Bool CDPakFile::RemoveFile(const String& FileName)
{
	//TODO: Add remove function to array
	ASSERT(!FileName.empty());

	String  temp;

	for (UInt32 i = 0; i < m_Files.Size(); i++)
	{
		temp = m_Files[i];

		if (temp == FileName)
		{
			m_Files.Remove(temp);
			return true;
		}
	}

	m_eErrorCode = DPakError_FileNotFoundInList;

	return false;
}

//////////////////////////////////////////////////////////////////////////

Bool CDPakFile::Contains(const String& FileName)
{
	return m_Files.Contains(FileName);
}

//////////////////////////////////////////////////////////////////////////

Bool CDPakFile::GetFileByName(DPakFile& dpf, const String& FileName, Bool bTextFile)
{
	//g_Log << "Beginning search for file: " << FileName << "...\n";

	for (UInt32 i = 0; i < m_FileCount; i++)
	{
		//g_Log << m_FileItems[i].FileName << "\n";
		
		if (_stricmp(m_FileItems[i].FileName, FileName.c_str()) == 0)
		{
			if (m_FileItems[i].CurrentlyLoaded)
				dpf = m_FileData[i];
			else
				dpf = LoadFileByName(FileName);

			return true;
		}
	}

	Bool ret = false;

	//File was not found in PAK table of contents, so if enabled, try to open the file locally...
	if (m_bSearchLocally)
	{
		if ((ret = dpf.OpenLocalFile(FileName, bTextFile)) != 0)
			dpf.ReadLocalFile();
	}

	//No file found
	if (ret)
		m_eErrorCode = DPakError_Success;
	else
		m_eErrorCode = DPakError_FileNotFoundInPak;

	return ret;
}

//////////////////////////////////////////////////////////////////////////

Bool DPakFile::OpenLocalFile(const String& file, Bool bTextFile)
{
	bLocalFile = true;

	strncpy_s(Name, MAX_FILENAME_LENGTH, file.c_str(), MAX_FILENAME_LENGTH);

	fopen_s(&File, file.c_str(), bTextFile ? "rt" : "rb");

	if (IsValid())
	{
		fseek(File, 0, DPakSeek_FromEnd);
		FileSize = ftell(File);
		fseek(File, 0, DPakSeek_FromStart);

		return true;
	}

	const TStringArray& strPaths = CDPakFile::GetSearchDirectories();

	for (UInt32 path = 0; path < strPaths.Size(); path++)
	{
		strncpy_s(Name, MAX_FILENAME_LENGTH, strPaths[path].c_str(), MAX_FILENAME_LENGTH);
		strncat_s(Name, MAX_FILENAME_LENGTH, "\\", 2);
		strncat_s(Name, MAX_FILENAME_LENGTH, file.c_str(), MAX_FILENAME_LENGTH);

		fopen_s(&File, Name, bTextFile ? "rt" : "rb");
	
		if (IsValid())
		{
			fseek(File, 0, DPakSeek_FromEnd);
			FileSize = ftell(File);
			fseek(File, 0, DPakSeek_FromStart);

			return true;
		}
	}

	return false;
}