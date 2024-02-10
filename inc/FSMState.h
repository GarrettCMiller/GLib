#pragma once

#include <map>

#include "Variable.h"
#include "dynamicarray.h"

//////////////////////////////////////////////////////////////////////////

class IFSMStateBase;
typedef CDynamicArray<IFSMStateBase*>	TStateMap;

//////////////////////////////////////////////////////////////////////////

class IFSMStateBase
{
	friend class CStateMachine;

public:
	IFSMStateBase(void) : m_bValid(false)
	{	}

	virtual ~IFSMStateBase(void)
	{	}

	virtual Int32 Initialize(const String& name)
	{
		m_strName = name;
		m_bValid = true;
		return 0;
	}

	virtual Int32 OnEnter(TArgumentArray* pArgs = NULL)		= 0;
	virtual Int32 OnUpdate(TArgumentArray* pArgs = NULL)	= 0;
	virtual Int32 OnExit(TArgumentArray* pArgs = NULL)		= 0;

	virtual Int32 OnMessage(TArgumentArray* pArgs = NULL)	= 0;

	const String& GetName() const
	{	
		return m_strName;
	}

	Bool IsValid() const
	{	return m_bValid;		}

	Bool IsValidTransition(IFSMStateBase* pState)
	{
		for (UInt32 i = 0; i < m_ValidTransitions.Size(); i++)
			if (m_ValidTransitions[i] == pState) return true;

		return false;
	}

	void AddValidTransition(IFSMStateBase* pState)
	{
		ASSERT(pState);
		//ASSERT(pState->IsValid());

		m_ValidTransitions.Push(pState);
	}

protected:
	Bool		m_bValid;

	String		m_strName;

	TStateMap	m_ValidTransitions;
};

//////////////////////////////////////////////////////////////////////////

template<class _FSM>
class CFSMState : public IFSMStateBase
{
public:
	CFSMState()	{	}

	CFSMState(_FSM* pFSM, const String& strName)
	{
		ASSERT(pFSM);

		Initialize(strName);

		m_pFSM = pFSM;
		m_pFSM->FSM_AddState(this);
	}

	virtual Int32 OnEnter(TArgumentArray* pArgs = NULL);
	virtual Int32 OnUpdate(TArgumentArray* pArgs = NULL);
	virtual Int32 OnExit(TArgumentArray* pArgs = NULL);

	virtual Int32 OnMessage(TArgumentArray* pArgs = NULL);

protected:

public:

protected:
	_FSM*	m_pFSM;
};

template<class _FSM>
Int32 CFSMState<_FSM>::OnEnter(TArgumentArray* pArgs = NULL)
{
	return 0;
}

template<class _FSM>
Int32 CFSMState<_FSM>::OnUpdate(TArgumentArray* pArgs = NULL)
{
	return 0;
}

template<class _FSM>
Int32 CFSMState<_FSM>::OnExit(TArgumentArray* pArgs = NULL)
{
	return 0;
}

template<class _FSM>
Int32 CFSMState<_FSM>::OnMessage(TArgumentArray* pArgs = NULL)
{
	return 0;
}