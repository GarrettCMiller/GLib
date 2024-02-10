#include "stdafx.h"

#include "StateMachine.h"

//////////////////////////////////////////////////////////////////////////

CStateMachine::CStateMachine(const String& strName) 
	:	m_pFSMCurrentState(NULL),
		m_pFSMNextState(NULL),
		m_bInternalQueueState(false)
{
	m_FSMLog.OpenNewLog("StateMachine_" + strName);

	m_FSMLog << "New state machine created\n";
}

CStateMachine::~CStateMachine()
{
	m_FSMLog << "Freeing pointers...\n";
	m_FSMStates.FreePointers();

	m_FSMLog << "Destroying processor...\n";
}

//////////////////////////////////////////////////////////////////////////

void CStateMachine::FSM_AddState(IFSMStateBase* pState)
{
	ASSERT(pState);
	ASSERT(pState->IsValid());

	m_FSMStates.Push(pState);

	m_FSMLog << "Added state " << pState->GetName() << "\n";
}

//////////////////////////////////////////////////////////////////////////

Int32 CStateMachine::FSM_Initialize(IFSMStateBase* pStartState)
{
	m_FSMLog << "Initializing state machine...\n";

	m_FSMLog << "Entering start state\n";

	FSM_EnterState(pStartState);

	return 0;
}

//////////////////////////////////////////////////////////////////////////

Int32 CStateMachine::FSM_Update()
{
	//ASSERT(m_FSMCritSection.GetRefCount() == 0);

	m_FSMCritSection.Lock();

	//ASSERT(m_FSMCritSection.GetRefCount() == 1);

	//If we have a next state, leave the current and enter the next
	if (m_pFSMNextState != NULL)
	{
		ASSERT(m_pFSMNextState);

		m_FSMLog	<< "Transitioning from state " << m_pFSMCurrentState->GetName()
					<< " to state " << m_pFSMNextState->GetName() << "\n";

		ASSERT(m_pFSMNextState);
		FSM_LeaveState();
		ASSERT(m_pFSMNextState);
		FSM_EnterState(m_pFSMNextState);
	}

	m_bInternalQueueState = true;

	m_pFSMCurrentState->OnUpdate();

	m_bInternalQueueState = false;

	//ASSERT(m_FSMCritSection.GetRefCount() == 1);

	m_FSMCritSection.Unlock();

	//ASSERT(m_FSMCritSection.GetRefCount() == 0);

	return 0;
}

//////////////////////////////////////////////////////////////////////////

Int32 CStateMachine::FSM_Shutdown()
{
	m_FSMLog << "Shutting state machine down...\n";

	return 0;
}

//////////////////////////////////////////////////////////////////////////

Int32 CStateMachine::FSM_EnterState(IFSMStateBase* pNewState)
{
	ASSERT(pNewState);
	ASSERT(pNewState->IsValid());

	m_pFSMCurrentState = pNewState;
	m_pFSMNextState = NULL;

	if (m_pFSMCurrentState->OnEnter() < 0)
		return -1;

	m_FSMLog << "Entered state " << m_pFSMCurrentState->GetName() << "\n";

	return 0;
}

//////////////////////////////////////////////////////////////////////////

Int32 CStateMachine::FSM_LeaveState()
{
	ASSERT(m_pFSMNextState);
	m_pFSMCurrentState->OnExit();
	ASSERT(m_pFSMNextState);

	String strName = m_pFSMCurrentState->GetName();

	m_FSMLog << "Left state " << strName << "\n";

	ASSERT(m_pFSMNextState);

	return 0;
}

//////////////////////////////////////////////////////////////////////////

Int32 CStateMachine::FSM_QueueState(const String& strStateName)
{
	if (m_pFSMNextState && m_pFSMNextState->GetName() == strStateName)
		return 1;

	//Make sure we don't have a next state already
	ASSERT(!m_pFSMNextState);

	if (!m_bInternalQueueState)
	{
		ASSERT(m_FSMCritSection.GetRefCount() == 0);
		m_FSMCritSection.Lock();
	}

	IFSMStateBase* pState = FSM_GetStateByName(strStateName);

	ASSERT(pState);

	if (m_pFSMCurrentState->GetName() != pState->GetName())
	{
		if (m_pFSMCurrentState->IsValidTransition(pState) == true)
			m_pFSMNextState = pState;
		else
			ASSERT(0);	//Invalid transition
	}
	else
		ASSERT(0);		//Transition to same state

	m_FSMLog << "Queued state " << strStateName << "\n";

	if (!m_bInternalQueueState)
	{
		ASSERT(m_FSMCritSection.GetRefCount() == 1);
		m_FSMCritSection.Unlock();
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////

IFSMStateBase* CStateMachine::FSM_GetStateByName(const String& strStateName)
{
	for (UInt32 stateIndex = 0; stateIndex < m_FSMStates.Size(); stateIndex++)
	{
		if (m_FSMStates[stateIndex]->GetName() == strStateName)
			return m_FSMStates[stateIndex];
	}

	//We shouldn't be looking up states that don't exist
	ASSERT(0);

	return NULL;
}

//////////////////////////////////////////////////////////////////////////

Int32 CStateMachine::FSM_SendMessage(TArgumentArray& args)
{
	m_FSMLog << "Message received\n";

	return m_pFSMCurrentState->OnMessage(&args);
}

//////////////////////////////////////////////////////////////////////////