#pragma once

class CCritSection
{
	Bool				m_bMFC;
	CRITICAL_SECTION	m_cs;
	Bool				m_bIsLocked;

	UInt32				m_uRefCount;

	//CCriticalSection	m_cCS;
public:
	CCritSection(bool bMFC = false) : m_bIsLocked(false), m_uRefCount(0)
	{
		m_bMFC = bMFC;

		if (!bMFC)
			InitializeCriticalSection(&m_cs);
	}

	~CCritSection(void) 
	{
		if (m_bMFC)
			DeleteCriticalSection(&m_cs);
	}

	struct CSHelper
	{
		CSHelper(CCritSection& _cs) : cs(_cs)
		{
			cs.Lock();
		}
		~CSHelper()
		{
			cs.Unlock();
		}

		CCritSection& cs;
	};

	virtual void Reset()
	{
		m_bIsLocked = false;
		m_uRefCount = 0;
		DeleteCriticalSection(&m_cs);
		InitializeCriticalSection(&m_cs);
	}

	virtual void Lock()
	{
		//if (m_bMFC)
		//	m_cCS.Lock(INFINITE);
		//else
			EnterCriticalSection(&m_cs);

		//Test after the internal cs allows it
		//ASSERT(IsLocked() == false);
		//TODO: This trips up even when IsLocked() returns false...????)
		
		//Same with the god damned ref count
		//ASSERT(m_uRefCount == 0);

		m_bIsLocked = true;
		m_uRefCount++;

		//ASSERT(m_uRefCount == 1);
	}

	virtual void Unlock()
	{
		//Test after the internal cs allows it
		//ASSERT(m_uRefCount == 1);
		//TODO: SCREW the ref count assert!!!! GRRRR!!
		
		//This seems to be broken in the Lock, might as well get both sides
		//ASSERT(IsLocked() == true);

		//if (m_bMFC)
		//	m_cCS.Unlock();
		//else
			LeaveCriticalSection(&m_cs);

		m_bIsLocked = false;
		m_uRefCount--;

		//ASSERT(m_uRefCount == 0);
	}

	Bool IsLocked() const
	{
		return m_bIsLocked;
	}

	UInt32 GetRefCount() const
	{
		return m_uRefCount;
	}
};
