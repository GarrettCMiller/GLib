#pragma once

#include "FSMState.h"
#include "CritSection.h"

#include "Logger.h"

//////////////////////////////////////////////////////////////////////////

#define ASSERT_STATE_IS(op)		ASSERT(FSM_GetCurrentStateName() op)
#define ASSERT_STATE(name)		ASSERT(FSM_GetCurrentStateName() == name)

//////////////////////////////////////////////////////////////////////////

class CStateMachine
{
	enum EResult
	{
		eOK,
		eFailed,
	};

public:

	CStateMachine(const String& strName);
	virtual ~CStateMachine(void);

	void FSM_AddState(IFSMStateBase* pState);

	virtual Int32 FSM_Initialize(IFSMStateBase* pStartState);
	virtual Int32 FSM_Update();
	virtual Int32 FSM_Shutdown();

	virtual Int32 FSM_SendMessage(TArgumentArray& args);

	//////////////////////////////////////////

	IFSMStateBase* FSM_GetStateByName(const String& strStateName);

	IFSMStateBase* FSM_GetCurrentState()	const
	{	return m_pFSMCurrentState;					}

	String FSM_GetCurrentStateName()	const
	{	return m_pFSMCurrentState->GetName();		}

	Int32 FSM_QueueState(const String& strStateName);

protected:

	Logger			m_FSMLog;
	CCritSection	m_FSMCritSection;


private:

	Int32 FSM_EnterState(IFSMStateBase* pNewState);
	Int32 FSM_LeaveState();

	/////////////////////////////
	// Data
	/////////////////////////////
public:


protected:

	String			m_strFSMName;

private:

	TStateMap		m_FSMStates;

	Bool			m_bInternalQueueState;

	IFSMStateBase*	m_pFSMCurrentState;
	IFSMStateBase*	m_pFSMNextState;
};


#define ADD_FSM_FUNCTIONS	\
						Int32 FSM_Initialize(IFSMStateBase* pStartState); \
						Int32 FSM_Update(); \
						Int32 FSM_Shutdown(); \
						Int32 FSM_SendMessage(TArgumentArray& args);

#define MAKE_STATE_CLASS(_ownerClass, _stateName, _stateClass) \
						class _ownerClass::C##_stateName##State : public _stateClass \
						{ \
							friend _ownerClass; \
						public: \
							C##_stateName##State(_ownerClass* pFSM, const String& strName); \
							virtual Int32 OnEnter(TArgumentArray* pArgs = NULL); \
							virtual Int32 OnUpdate(TArgumentArray* pArgs = NULL); \
							virtual Int32 OnExit(TArgumentArray* pArgs = NULL); \
							virtual Int32 OnMessage(TArgumentArray* pArgs = NULL); \
						};