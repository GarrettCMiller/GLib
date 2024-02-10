///////////////////////////////////////////////////////////////////////////
//		File:				XMLMgr.cpp
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		Handles reading of XML documents including
//							the game definition
//////////////////////////////////////////////////////////////////////////
//#ifndef _STDAFX_INCLUDED_
	#include "stdafx.h"
	#pragma message("stdafx.h has been included in " __FILE__)
//#endif

#include "xmlmgr.h"
#include "XMLTables.h"

#include "Globals.h"

#include "miscmath.h"

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_SINGLETON(XMLMgr);

//////////////////////////////////////////////////////////////////////////


#define SetFunctor(name)		{m_Functors.Push(TXMLFunctor(this, (&XMLMgr::Parse##name))); m_ParseModeNames.Push(#name); }

//////////////////////////////////////////////////////////////////////////

XMLMgr::XMLMgr(void)
{
	m_bSettingsRead = m_bStopReading = false;
	//m_bLoadAllSets	= true;

	m_Functors.Resize(10);
	m_ParseModeNames.Resize(10);

	TXMLFunctor functor;
	
	SetFunctor(Settings);
		SetFunctor(Video);
			SetFunctor(Resolution);
			SetFunctor(Advanced);
		SetFunctor(Performance);
		SetFunctor(Memory);
		SetFunctor(Debug);
			SetFunctor(Map);
			SetFunctor(Profiler);
/*
	SetFunctor(EntitySet);
		SetFunctor(Entity);
	
	SetFunctor(WeaponSet);
		SetFunctor(Weapon);

	SetFunctor(ImageSet);
		SetFunctor(Image);
		SetFunctor(Set);

	SetFunctor(Sound);

	SetFunctor(Equipment);
		SetFunctor(Equip);*/
}

//////////////////////////////////////////////////////////////////////////

XMLMgr::~XMLMgr(void)
{
	m_ParseModes.Clear();
}

//////////////////////////////////////////////////////////////////////////

void XMLMgr::ReadSettings()
{
	g_Log << "Reading settings...\n";

	m_bStopReading = false;
	m_ParseModes.Push(eMode_Root);

	TiXmlElement* root = m_GameDefDoc.RootElement();

	//g_Game->GameName = root->Attribute("name");

	Parse(root);

	m_ParseModes.Pop();

	ASSERT(m_ParseModes.IsEmpty());

	g_Log << "Settings read successfully!\n";
}

//////////////////////////////////////////////////////////////////////////

XMLMgr::TXMLFunctor* XMLMgr::GetFunctor(String name)
{
	for (UInt32 i = 0; i < m_ParseModeNames.Size(); i++)
		if (_stricmp(name.c_str(), m_ParseModeNames[i].c_str()) == 0)
			return &m_Functors[i];

	ASSERT(false);
	return NULL;
}

//////////////////////////////////////////////////////////////////////////

void XMLMgr::Parse(TiXmlElement* e)
{
	for (TiXmlElement* element = e->FirstChildElement(); element != NULL; element = element->NextSiblingElement())
	{
		if (m_bStopReading)
			return;

		String valName = element->Value();

		TXMLFunctor* functor = GetFunctor(valName);

		if (functor)
			(*functor)(element);
		else	
			Parse(element);
	}
}

//////////////////////////////////////////////////////////////////////////

void XMLMgr::ParseSettings(TiXmlElement* e)
{
	if (!m_bSettingsRead)
	{
		m_ParseModes.Push(eMode_Settings);

		Parse(e);

		m_ParseModes.Pop();

		m_bStopReading	= true;
		m_bSettingsRead = true;
	}
}

//////////////////////////////////////////////////////////////////////////

void XMLMgr::ParseVideo(TiXmlElement* e)
{
	if (!m_bSettingsRead)
	{
		m_ParseModes.Push(eMode_Video);
		Parse(e);
		m_ParseModes.Pop();
	}
}

//////////////////////////////////////////////////////////////////////////

void XMLMgr::ParseResolution(TiXmlElement* e)
{
	if (!m_bSettingsRead)
	{
		ASSERT(m_ParseModes[0] == eMode_Video);
		e->QueryIntAttribute("width",			&g_Settings.videoRes.resolution.width);
		e->QueryIntAttribute("height",			&g_Settings.videoRes.resolution.height);
		g_Settings.videoRes.resolution.windowed	= (AttributeIs("windowed", "true"));
	}
}

//////////////////////////////////////////////////////////////////////////

void XMLMgr::ParseAdvanced(TiXmlElement* e)
{
	if (!m_bSettingsRead)
	{
		ASSERT(m_ParseModes[0] == eMode_Video);
		g_Settings.videoRes.advanced.enableShaders = (AttributeIs("EnableShaders", "true"));
	}
}

//////////////////////////////////////////////////////////////////////////

void XMLMgr::ParsePerformance(TiXmlElement* e)
{
	if (!m_bSettingsRead)
	{
		ASSERT(m_ParseModes[0] == eMode_Settings);
		g_Settings.performance.throttleFPS		= (AttributeIs("throttleFPS", "true"));
		e->QueryFloatAttribute("throttleLimit",	&g_Settings.performance.throttleLimit);
		//Invert
		g_Settings.performance.throttleLimit = 1.0f / g_Settings.performance.throttleLimit;
	}
}

//////////////////////////////////////////////////////////////////////////

void XMLMgr::ParseMemory(TiXmlElement* e)
{
	if (!m_bSettingsRead)
	{
		ASSERT(m_ParseModes[0] == eMode_Settings);
		e->QueryIntAttribute("reserve", &g_Settings.memory.reserve);
	}
}

//////////////////////////////////////////////////////////////////////////

void XMLMgr::ParseDebug(TiXmlElement* e)
{
	if (!m_bSettingsRead)
	{
		m_ParseModes.Push(eMode_Debug);

		g_Settings.debug.enabled = (AttributeIs("enabled", "true"));
		g_Settings.debug.showFPS = (AttributeIs("showFPS", "true"));

		Parse(e);

		m_ParseModes.Pop();
	}
}

//////////////////////////////////////////////////////////////////////////

void XMLMgr::ParseMap(TiXmlElement* e)
{
	if (!m_bSettingsRead)
	{
		ASSERT(m_ParseModes[0] == eMode_Debug);

		g_Settings.debug.mapInfo.render		= (AttributeIs("render",	"true"));
		g_Settings.debug.mapInfo.autospawn	= true;//(AttributeIs("autoSpawn", "true"));
	}
}

//////////////////////////////////////////////////////////////////////////

void XMLMgr::ParseProfiler(TiXmlElement* e)
{
	if (!m_bSettingsRead)
	{
		ASSERT(m_ParseModes[0] == eMode_Debug);

		g_Settings.debug.profiler.enabled	= (AttributeIs("enabled",	"true"));
	}
}

//////////////////////////////////////////////////////////////////////////
#if 0
void XMLMgr::ParseEntitySet(TiXmlElement* e)
{
	m_ParseModes.Push(eMode_EntitySet);
	Parse(e);
	m_ParseModes.Pop();
}

//////////////////////////////////////////////////////////////////////////

void XMLMgr::ParseEntity(TiXmlElement* e)
{
	m_ParseModes.Push(eMode_Entity);

	Float footpos	= 0.0f;
	Float bbox		= 0.0f;

	XMLEntity ent;

	wcsncpy_s(ent.name, e->Attribute("class"), 16);

	Creature::TypeIdNames[EntityNum] = ent.name;

	e->QueryFloatAttribute("footpos",		&footpos);
	e->QueryFloatAttribute("bbox",			&bbox);
	e->QueryIntAttribute("hp",				&ent.hp);
	e->QueryIntAttribute("maxhp",			&ent.maxhp);

	ent.maxhp		= max(ent.maxhp, ent.hp);

	ent.footpos		= Vec2(Float(Int32(footpos)), Float(Int32(((footpos - Int32(footpos)) * 100.0f) + 1.0f)));
	ent.bbox		= Vec2(Float(Int32(bbox)), Float(Int32(((bbox - Int32(bbox)) * 100.0f) + 1.0f)));

	ent.IsValid		= true;

	ASSERT(EntityNum < k_MaxPossibleEntityTypes);

	//Move the data to the table
	memcpy(g_EntityTable + EntityNum, &ent, sizeof(XMLEntity));

	//Check for child items like image sets, sounds, and weapons
	Parse(e);

	//Next entity
	EntityNum++;

	m_ParseModes.Pop();
}

//////////////////////////////////////////////////////////////////////////

void XMLMgr::ParseWeaponSet(TiXmlElement* e)
{
	m_ParseModes.Push(eMode_WeaponSet);
	Parse(e);
	m_ParseModes.Pop();
}

//////////////////////////////////////////////////////////////////////////

void XMLMgr::ParseWeapon(TiXmlElement* e)
{
	m_ParseModes.Push(eMode_Weapon);

	XMLWeapon wpn;

	wcsncpy_s(wpn.name, e->Attribute("Name"), 16);

	Weapon::WeaponNames[WeaponNum] = wpn.name;

	e->QueryIntAttribute("Damage",			&wpn.damage);
	e->QueryIntAttribute("SplashDamage",	&wpn.splashdmg);
	e->QueryIntAttribute("Shots",			&wpn.shotcount);
	e->QueryFloatAttribute("SplashRadius",	&wpn.splashradius);
	e->QueryFloatAttribute("Spread",		&wpn.spread);
	e->QueryFloatAttribute("FireDelay",		&wpn.delay);
	e->QueryFloatAttribute("BulletSpeed",	&wpn.speed);

	wpn.IsValid = true;

	ASSERT(WeaponNum < k_MaxPossibleWeapons);

	//Move the data to the table
	memcpy(g_WeaponTable + WeaponNum, &wpn, sizeof(XMLWeapon));

	//Check for any children (after the memcpy, so that the child data isn't written over)
	Parse(e);

	//Next weapon
	WeaponNum++;

	m_ParseModes.Pop();
}

//////////////////////////////////////////////////////////////////////////

void XMLMgr::ParseImageSet(TiXmlElement* e)
{
	m_ParseModes.Push(eMode_ImageSet);

	wchar_t base[32] = {'\0'};
	m_bLoadAllSets = true;

	wcsncpy_s(base, e->Attribute("src"), 32);

	if (AttributeIs("set", "custom"))
		Parse(e);	//Check for sub-set information (frame limits)
	else if (AttributeIs("set", "partial"))
	{
		m_bLoadAllSets = false;
		m_ImageSetBase = base;
		Parse(e);
	}

	if (m_bLoadAllSets)
	{
		switch (m_ParseModes[1])	//1 because the current mode is image set
		{
		case eMode_Entity:
			Entity::LoadFrameSet(g_EntityTable[EntityNum].images, g_EntityTable[EntityNum].maxframes, base, EAS_ALL);
			break;

		default:
			ASSERT(false);
		}
	}

	m_ParseModes.Pop();
}

//////////////////////////////////////////////////////////////////////////

void XMLMgr::ParseEquipment(TiXmlElement* e)
{
	m_ParseModes.Push(eMode_Equipment);
	Parse(e);
	m_ParseModes.Pop();
}

//////////////////////////////////////////////////////////////////////////

void XMLMgr::ParseEquip(TiXmlElement* e)
{
	Int32 index = -1;

	if (AttributeIs("type", "weapon") && m_ParseModes[1] == eMode_Entity)
	{
		index = Weapon::NameToType(e->Attribute("name"));
		g_EntityTable[EntityNum].weapons[index] = true;
		g_EntityTable[EntityNum].currentweapon = (EntityNum ? index : 1);
	}
	else
		ASSERT(false);
}

//////////////////////////////////////////////////////////////////////////

void XMLMgr::ParseSet(TiXmlElement* e)
{
	wchar_t src[32] = {'\0'};
	Int32 maxframes = 0;
	Int32 index = -1;
	Int32 directional = 1;

	wcsncpy_s(src, e->Attribute("src"),	32);
	e->QueryIntAttribute("maxframes",	&maxframes);
	e->QueryIntAttribute("directional",	&directional);

	switch (m_ParseModes[1])
	{
	case eMode_Entity:
		index = Entity::NameToAnimState(src);
		g_EntityTable[EntityNum].maxframes[index] = maxframes;

		if (!m_bLoadAllSets || !directional)
			Entity::LoadFrameSet(g_EntityTable[EntityNum].images, g_EntityTable[EntityNum].maxframes, m_ImageSetBase, Entity::NameToAnimState(src), !(Bool(directional)));
		break;

	default:
		ASSERT(false);
	}
}

//////////////////////////////////////////////////////////////////////////

void XMLMgr::ParseImage(TiXmlElement* e)
{
	Int32	width	= 0,
		height	= 0,
		image	= -1;

	wchar_t path[32] = {'\0'};

	e->QueryIntAttribute("width",	&width);
	e->QueryIntAttribute("height",	&height);
	
	wcsncpy_s(path, e->Attribute("src"), 32);
	wcsncat_s(path, ".bmp", 32);

	image = g_SpriteMgr->LoadSprite(path);

	switch (m_ParseModes[0])
	{
	case eMode_Weapon:
		g_WeaponTable[WeaponNum].bulletImage = image;
		g_WeaponTable[WeaponNum].bulletSize = Vec2(width, height);
		break;

	default:
		ASSERT(false);
	}
}

//////////////////////////////////////////////////////////////////////////

void XMLMgr::ParseSound(TiXmlElement* e)
{
	Int32 sound = -1;
	wchar_t src[32] = {'\0'};

	wcsncpy_s(src, e->Attribute("src"), 32);

	sound = g_SoundMgr->LoadSound(src);

	switch (m_ParseModes[0])
	{
	case eMode_Weapon:
		if (AttributeIs("type", "hit"))
			g_WeaponTable[WeaponNum].hitsound = sound;
		else
			g_WeaponTable[WeaponNum].firesound = sound;
		break;

	case eMode_Entity:
		if (AttributeIs("type", "hurt"))
			g_EntityTable[EntityNum].hurtsound = sound;
		else if (AttributeIs("type", "die"))
			g_EntityTable[EntityNum].diesound = sound;
		else
			g_EntityTable[EntityNum].alertsound = sound;
		break;

	default:
		ASSERT(false);
	}
}

//////////////////////////////////////////////////////////////////////////
#endif