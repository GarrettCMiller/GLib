///////////////////////////////////////////////////////////////////////////
//		File:				ProfileLogHandler.cpp
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		A logger that spits out profiling info
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include ".\profileloghandler.h"

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_SINGLETON(CProfileLogHandler);

//////////////////////////////////////////////////////////////////////////

void CProfileLogHandler::BeginOutput(String FileName)
{
	if (!profileDump.IsValid())
		profileDump.OpenNewLog(FileName.empty() ? "Profile" : FileName);
	
	profileDump.SetMode(eLM_Table);

	profileDump << "Min"	<< eLFF_NewTableData << "Avg"				<< eLFF_NewTableData << "Max" << eLFF_NewTableData 
				<< "Calls"	<< eLFF_NewTableData << "Profile Name\n"	<< eLFF_NewTableRow;
}

//////////////////////////////////////////////////////////////////////////

void CProfileLogHandler::EndOutput()
{
	profileDump.SetMode();
	profileDump << "\n\n";
}

//////////////////////////////////////////////////////////////////////////

void CProfileLogHandler::Sample(Float fMin, Float fAvg, Float fMax, 
								Int32 callCount, String name, Int32 parentCount)
{
	profileDump		<< fMin			<< eLFF_NewTableData << fAvg << eLFF_NewTableData << fMax << eLFF_NewTableData
					<< callCount	<< eLFF_NewTableData;
	
	for (Int32 i = 0; i < parentCount; i++)
	{
		profileDump	<< "-----";
	}

	profileDump << name << eLFF_NewTableRow; 
}

//////////////////////////////////////////////////////////////////////////