#pragma once

class CThread
{
	friend class CThreadMgr;

public:
	enum EThreadPriorityFlags
	{
		eTPF_Idle		= THREAD_PRIORITY_IDLE,

		eTPF_Lowest		= THREAD_PRIORITY_LOWEST,
		eTPF_Low,
		eTPF_Normal,
		eTPF_High,
		eTPF_Highest,

		eTPF_RealTime	= THREAD_PRIORITY_TIME_CRITICAL,

		eTPF_Default	= eTPF_Normal
	};

	explicit CThread(LPTHREAD_START_ROUTINE pfnFunc = NULL, LPVOID vpData = NULL, 
					Bool bStartNow = true, EThreadPriorityFlags ePriority = eTPF_Default);
	~CThread();

	Bool Init(LPTHREAD_START_ROUTINE pfnFunc = NULL, LPVOID vpData = NULL, 
				Bool bStartNow = true, EThreadPriorityFlags ePriority = eTPF_Default);

	void Shutdown();

	Int32 Start();
	Int32 Stop();

	EThreadPriorityFlags GetPriority() const { return static_cast<EThreadPriorityFlags>(GetThreadPriority(m_hHandle)); }

	Bool SetPriority(EThreadPriorityFlags ePriority)
	{
		return (SetThreadPriority(m_hHandle, ePriority) == 1);
	}

	Bool IsValid() const { return m_bIsValid; }
	Bool IsRunning() const { return m_bIsRunning; }
	Bool NeedsToClose() const { return m_bShutdownFlag; }

	HANDLE GetHandle() const { return m_hHandle; }

	void Invalidate()
	{
		m_bIsValid = false;
		m_bIsRunning = false;
	}

	UInt32 GetID() const
	{
		return m_ID;
	}

	void SetGroupID(UInt32 id)
	{
		m_GroupID = id;
	}

	UInt32 GetGroupID() const
	{
		return m_GroupID;
	}

protected:
	UInt32 GentleShutdown();

protected:
	Bool					m_bShutdownFlag;
	Bool					m_bIsValid;
	Bool					m_bIsRunning;

	static UInt32			mk_NextID;
	UInt32					m_ID;
	UInt32					m_GroupID;
	UInt32					m_ShutdownAttempts;

	HANDLE					m_hHandle;
	LPTHREAD_START_ROUTINE	m_pfnFunction;
};
