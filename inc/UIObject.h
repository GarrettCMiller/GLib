/////////////////////////////////////////////////////////////////////////////
//		File:			UIObject.h
//
//		Programmmer:	Garrett Miller (GM)
//
//		Date Created:	2/21/2006 12:47:07 AM
//
//		Description:	Base UI object	
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Vec.h"
#include "Color.h"

//////////////////////////////////////////////////////////////////////////

struct Script;
struct MenuScript;

//////////////////////////////////////////////////////////////////////////

enum EMenuAction
{
	eMA_RunScript,
	eMA_NewGame,
	eMA_NewGameServer,
	eMA_NewGameClient,
	eMA_Back,
	eMA_Exit,

	eMA_Max
};

//////////////////////////////////////////////////////////////////////////

class UIObject
{
	friend class SpriteMgr;

protected:
	Vec2				m_Position;
	Vec2				m_Size;
	Vec2				m_Scale;

	Script*				m_pScriptToRun;
	EMenuAction			m_eMenuAction;

	Bool				m_bVisible;

	MenuScript*			m_pParentScript;
    
public:
	UIObject(void);
	virtual ~UIObject(void);

	void		SetPosition(Float x, Float y)			{ m_Position = Vec2(x, y); }
	void		SetPosition(const Vec2& pos)			{ m_Position = pos; }
	Vec2		GetPosition() const						{ return m_Position; }

	void		SetSize(Float x, Float y)				{ m_Size = Vec2(x, y); }
	void		SetSize(const Vec2& size)				{ m_Size = size; }
	Vec2		GetSize() const							{ return m_Size; }

	void		SetScale(Float x, Float y)				{ m_Scale = Vec2(x, y); }
	void		SetScale(const Vec2& scale)				{ m_Scale = scale; }
	Vec2		GetScale() const						{ return m_Scale; }

	Bool		IsVisible() const						{ return m_bVisible; }
	void		SetVisible(Bool bVisible = true)		{ m_bVisible = bVisible; }
	Bool		ToggleVisible()							{ m_bVisible = !m_bVisible; return m_bVisible;	}

	virtual	Bool	Update()	= 0;

	void		SetScript(Script* script);
	void		SetScript(String file);
	Script*		GetScript() const;

	void		SetMenuAction(EMenuAction ema)			{ m_eMenuAction = ema; }
	EMenuAction	GetMenuAction()	const					{ return m_eMenuAction; }

	void		SetParentScript(MenuScript *parent)		{ m_pParentScript = parent; }
	MenuScript* GetParentScript()	const				{ return m_pParentScript; }

	void		TriggerScript();

private:
	virtual void	Draw()		= 0;
	virtual void	Render()	= 0;
};
