#pragma once

//////////////////////////////////////////////////////////////////////////

class IMsgEvent
{
public:
	IMsgEvent()
	{	}
	virtual ~IMsgEvent()
	{	}

	void Proc() = 0;
};

//////////////////////////////////////////////////////////////////////////

class IMsgListener
{
public:
	IMsgListener(void)
	{	}
	virtual ~IMsgListener(void)
	{	}

	Int32 onEvent(IMsgEvent* pEvent) = 0;
};

//////////////////////////////////////////////////////////////////////////

class CEventManager
{
public:

protected:
private:
};