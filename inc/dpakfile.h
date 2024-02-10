///////////////////////////////////////////////////////////////////////////
//		File:				DPakFile.h
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		Like a ZIP file, but made by me :)
//////////////////////////////////////////////////////////////////////////
#pragma once

#include <stdio.h>

#include "DynamicArray.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//Soon to be obsolete defines for loading resources
#define LOAD_FILES_FROM_PAK

#ifdef LOAD_FILES_FROM_PAK
	#define LOAD_IMAGES_FROM_PAK
	#define LOAD_SOUNDS_FROM_PAK
	#define LOAD_SCRIPTS_FROM_PAK
	#define LOAD_EFFECTS_FROM_PAK
#endif

//The first 4 bytes of the file need to equal FILE_MAGIC to 
#define FILE_MAGIC	('D' + ('P' << 8) + ('A' << 16) + ('K' << 24))

//Version numbering
#define CURRENT_DPAK_VERSION_MAJOR	1
#define CURRENT_DPAK_VERSION_MINOR	0

//hack
#define MAX_FILEPATH_LENGTH	128
#define MAX_FILENAME_LENGTH 128


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

enum DPakFileFlags
{
	DPakFlag_Folder,

	DPakFlag_Count
};

//////////////////////////////////////////////////////////////////////////

enum DPakFileSeekModes
{
	DPakSeek_FromStart,
	DPakSeek_FromCurrent,
	DPakSeek_FromEnd,

	DPakSeekModeCount
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// The header data structure of a complete DPak file
struct DPakFileHeader
{
	UInt32	Magic;				//Identifies this as a potential DPAK file
	UInt32	VersionMajor : 16;	//Version major
	UInt32	VersionMinor : 16;	//Version minor
	UInt32	FileCount;			//Number of entries
	UInt32	PakFileSize;		//Size of the dpak file
	//UInt32	DirOffset;			//Offset to the directory info from the beginning
};

//////////////////////////////////////////////////////////////////////////
// Used for the table of contents (TOC) (TODO: rename to avoid confusion)
//Used for TOC
struct DPakFileHeaderItem
{
	char	FilePath[MAX_FILEPATH_LENGTH];	//Path to the file, including the filename
	char	FileName[MAX_FILENAME_LENGTH];	//Filename without the path, just the extension
	UInt32	Offset			: 30;			//offset in bytes from the beginning of the data block
	UInt32	Compressed		: 1;			//Is this file compressed? (not right now, no compression yet)
	UInt32	CurrentlyLoaded	: 1;			//Has the file been loaded into memory already?
	UInt32	FileSizeUncompressed;			//Total size, in bytes, of the file as normal
	UInt32	FileSizeCompressed;				//Size, in bytes, of the file after compression

	//Just zero ourselves out
	DPakFileHeaderItem()	{	ZeroMemory(this, sizeof(DPakFileHeaderItem));	}
};

//////////////////////////////////////////////////////////////////////////

class CDPakFile;

struct DPakFile
{ 
	char	Name[MAX_FILENAME_LENGTH];	//Name of the file
	UInt32	bLocalFile	: 1;			//Is this file on a local hard drive?
	UInt32	FileSize	: 31;			//Size, in bytes, of the file
	UInt32	FilePointer;				//Keeps track of data pointer when reading data
	BYTE*	FileData;					//Actual file data, raw
	FILE*	File;						//If a local file, the normal file pointer

	DPakFile(String name = "", UInt32 filesize = 0, BYTE* data = NULL) : 
		bLocalFile(false), FileSize(filesize), FileData(data), FilePointer(0), File(NULL)
	{
		if (name.empty())
			ZeroMemory(Name, MAX_FILENAME_LENGTH);
		else
			strncpy_s(Name, MAX_FILENAME_LENGTH, name.c_str(), MAX_FILENAME_LENGTH);
	}

	~DPakFile()
	{
		if (bLocalFile && FileData)
			delete [] FileData;

		if (File)
			fclose(File);
	}

	Bool OpenLocalFile(const String& file, Bool bTextFile = false);

	Bool IsLocalFile() const
	{
		return bLocalFile;
	}

	Bool IsValid() const
	{
		return (bLocalFile ? (File != NULL) : (FileData != NULL && FileSize > 0));
	}

	Bool EoF() const
	{
		return (bLocalFile ? (feof(File) == 1) : FilePointer >= FileSize);
	}

	UInt32 Read(void* dest, UInt32 size)
	{
		UInt32 bytesRead = 0;

		ASSERT(dest);
		ASSERT(size);

		if (bLocalFile)
		{
			bytesRead = UInt32(fread(dest, 1, size, File));
			FilePointer += bytesRead;
		}
		else
		{
			ASSERT((FilePointer + size) <= FileSize);
			memcpy(dest, FileData + FilePointer, size);
			bytesRead = size;
			FilePointer += size;
		}

		return bytesRead;
	}

	UInt32 ReadLocalFile()
	{
		UInt32 bytesRead = 0;

		FileData = new BYTE [FileSize];

		if (bLocalFile)
		{
			bytesRead = UInt32(fread(FileData, 1, FileSize, File));
			FilePointer += bytesRead;
		}
		else
			ASSERT(0); //We better be a local file

		ASSERT(bytesRead == FileSize);

		return bytesRead;
	}

	void Seek(UInt32 Bytes, DPakFileSeekModes SeekMode = DPakSeek_FromStart)
	{
		if (bLocalFile)
		{
			ASSERT(Bytes > LONG_MAX);
			fseek(File, Bytes, SeekMode);
		}
		
		switch (SeekMode)
		{
		case DPakSeek_FromStart:	//From beginning
			FilePointer = Bytes;
			break;

		case DPakSeek_FromCurrent:	//From current position
			ASSERT((FilePointer + Bytes) < FileSize);
			FilePointer += Bytes;
			break;

		case DPakSeek_FromEnd:		//Backwards from end of file
			ASSERT(FileSize - Bytes >= 0);
			FilePointer = FileSize - Bytes /* - 1*/;
			break;

		default:	//Should not get here (not an option)
			ASSERT(false);	
		}
	}
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

class CDPakFile
{
public:
	typedef CDynamicArray<String >	TFileVector;

	enum ErrorCode
	{
		DPakError_Success,
		DPakError_OpenPakFileFailed,
		DPakError_OpenFileFailed,
		DPakError_InvalidPakFile,
		DPakError_NoFilesToPack,
		DPakError_FileNotFoundInList,
		DPakError_FileNotFoundInPak,

		DPakError_Count
	}	m_eErrorCode;

private:
	Bool			m_bSearchLocally;		//If a file isn't found in the PAK file, should we try to open it locally on the HD?
	String 			m_PakFileName;
	String			m_BasePath;
	TFileVector		m_Files;

	static TStringArray	m_SearchDirectories;

	DPakFileHeader	m_Header;

	//DblLinkedList<DPakFileHeaderItem>	m_FileItems;
	//DblLinkedList<DPakFile>				m_FileData;
	CDynamicArray<DPakFileHeaderItem>	m_FileItems;
	CDynamicArray<DPakFile>				m_FileData;

	FILE*			m_PakFile;
	FILE*			m_Current;

	UInt32			m_CurrentFileOffset;

	UInt32			m_TotalBytesToWrite;	//What the final size of the archive should be
	UInt32			m_TotalBytesWritten;	//How many bytes have been written so far

	UInt32			m_FileBytesToWrite;		//What the total amount of bytes to write that are file only
	UInt32			m_FileBytesWritten;		//How many file bytes have been written

	UInt32			m_FileCount;

private:
	Bool			OpenPakFile(Bool Read = true);
	Bool			ReadInAllFiles();

	Bool			ReadFileHeaders();

	Bool			WriteFilesToPak();
	Bool			ReadAllFilesFromPak();

	void			ReleaseAllResources(Bool ClearFileList = false);

	void			Initialize();

	DPakFile		LoadFileByName(const String& FileName);

public:
	CDPakFile(String  filename = "Data.dpk");
	~CDPakFile(void);

	Bool			SearchLocally() const				{ return m_bSearchLocally; }
	void			SearchLocally(Bool bLocal = true)	{ m_bSearchLocally = bLocal; }

	void			AddFile(const String& FileName);
	Bool			RemoveFile(const String& FileName);

	Bool			Contains(const String& FileName);

	Bool			WritePakFile();
	Bool			ReadPakFile(Bool ReadAllFiles = false);

	void			ClearAllFiles()						{ m_Files.Clear(); }

	static void		AddSearchDirectory(const String& strDir)	{ m_SearchDirectories.Push(strDir);	}
	static String	GetSearchDirectory(UInt32 index = 0)		{ return m_SearchDirectories[index];	}
	
	static const TStringArray& GetSearchDirectories() 	{ return m_SearchDirectories;		}

	String 			GetBasePath() const					{ return m_BasePath; }
	void			SetBasePath(const String& basePath)	{ ASSERT(!basePath.empty()); m_BasePath = basePath; }

	String 			GetFileName() const					{ return m_PakFileName; }
	void			SetFileName(const String& name)		{ ASSERT(!name.empty()); m_PakFileName = name; }

	Bool			GetFileByName(DPakFile& dpf, const String& FileName, Bool bTextFile = false);
};