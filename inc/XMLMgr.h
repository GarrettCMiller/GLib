///////////////////////////////////////////////////////////////////////////
//		File:				XMLMgr.h
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		Handles reading of XML documents, including
//							the game definition
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "TinyXML\tinyxml.h"
#include "TinyXML\tinystr.h"

#include "Singleton.h"

#include "GameMacros.h"
#include "DynamicArray.h"
#include "Stack.h"
#include "Functor.h"

//#include "Globals.h"

//////////////////////////////////////////////////////////////////////////

#define MAX_FUNCTORS 20

#define ElementIs(text)			(_stricmp(valName.c_str(),		text)	== 0)
#define AttributeIs(attr, text)	(_stricmp(e->Attribute(attr),	text)	== 0)
//////////////////////////////////////////////////////////////////////////

class XMLMgr// : public Singleton<XMLMgr>
{
protected:
	TiXmlDocument m_GameDefDoc;

	enum EParseMode
	{
		eMode_Root,

			eMode_Settings,
				eMode_Video,
				eMode_Debug,

		eMode_Max
	};

	Stack<Int32>	m_ParseModes;

	typedef CFunctor1<XMLMgr, void, TiXmlElement*>	TXMLFunctor;

	Bool			m_bSettingsRead;
	Bool			m_bStopReading;

	CDynamicArray<TXMLFunctor>	m_Functors;
	TStringArray		m_ParseModeNames;

public:
	XMLMgr(void);
	~XMLMgr(void);

	void ReadSettings();

protected:
	TXMLFunctor* GetFunctor(String name);

	void Parse(TiXmlElement* e);

	//////////////////////////////////////////////////////////////////////////
	void	ParseSettings(TiXmlElement* e);
		void	ParseVideo(TiXmlElement* e);
			void	ParseResolution(TiXmlElement* e);
			void	ParseAdvanced(TiXmlElement* e);
		void	ParsePerformance(TiXmlElement* e);
		void	ParseMemory(TiXmlElement* e);
		void	ParseDebug(TiXmlElement* e);
			void	ParseMap(TiXmlElement* e);
			void	ParseProfiler(TiXmlElement* e);

	//////////////////////////////////////////////////////////////////////////
};

//#define g_XMLMgr	XMLMgr::GetInstance()