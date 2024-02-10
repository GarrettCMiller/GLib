///////////////////////////////////////////////////////////////////////////
//		File:				LList.h
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		Templated singly-linked linked list with an iterator class
//////////////////////////////////////////////////////////////////////////

#pragma once

//#include "DataTypes.h"
//#include "SmartPointer.h"

#if 1
typedef				__int8			Int8;
typedef unsigned	__int8			UInt8;

typedef				__int16			Int16;
typedef unsigned	__int16			UInt16;

typedef				__int32			Int32;
typedef unsigned	__int32			UInt32;

typedef				__int64			Int64;
typedef unsigned	__int64			UInt64;

typedef				float			Float;
typedef				double			Double;

typedef				bool			Bool;

typedef				UInt32*			Pointer;

typedef unsigned	char			Char;
typedef UInt8						Byte;
#endif // 0

//////////////////////////////////////////////////////////////////////////

template <typename _Type>
class LinkedList
{

protected:
	struct Node;

#ifdef USING_SMART_POINTERS
	typedef CIntrusivePtr<Node>		NodePtr;
#else
	typedef Node*					NodePtr;
#endif

	struct Node/* : public CMemObj*/
	{	
		Node() : next(NULL)
		{	}

		_Type		item;
		NodePtr		next;

		/*AUTO_SIZE;*/
	};
	
	//////////////////////////////////////////////////////////////////////////	
	NodePtr		head;
	UInt32		size;

public:
	class Iterator
	{
		NodePtr	i;
		NodePtr	head;

		UInt32		CurrentPos;
		UInt32		MaxPos;

	public:
		//Default constructor
		Iterator() : i(NULL), head(NULL), CurrentPos(UInt32(-1)) {}

		Iterator(const LinkedList<_Type>& list)
		{
			head = list.head;
			CurrentPos = 0;
			MaxPos = list.Size();
		}

		void Begin()
		{
			i = head;
			CurrentPos = 0;
		}

		Bool End()
		{
			return (i == NULL);
		}

		void operator ++()
		{
			i = i->next;
			CurrentPos++;
		}

		_Type& Current()
		{
			return i->item;
		}

		UInt32 CurrentPosition()
		{
			return CurrentPos;
		}
	};

public:
	//ctr : set empty list initially
	LinkedList() : head(NULL), size(0)	{	}

	virtual ~LinkedList()
	{
		Clear();
	}

	//////////////////////////////////////
	//List manipulation funcs
	//////////////////////////////////////
	Bool IsEmpty() const
	{
		return (head == NULL);
	}

	UInt32 Size() const
	{
		return size;
	}

	void Add(const _Type& v, Bool bToHead = true)
	{
		NodePtr n	= new Node;

		ASSERT(n);
		
		n->item		= v;

		if (bToHead)
		{
			n->next		= head;
			head		= n;
		}
		else
		{
			NodePtr temp = head;

			while (temp->next)
				temp = temp->next;

			temp->next = n;
		}

		size++;
	}

	Bool Remove(const _Type& v)
	{
		NodePtr prev	= NULL, 
				cur		= NULL;

		for (	prev = NULL,	cur = head;
				cur;
				prev = cur,		cur = cur->next)
		{
			if (cur->item == v)
			{
				if (cur == head)
					head = head->next;
				else
					prev->next = cur->next;

#ifdef USING_SMART_POINTERS
				cur->Release();
#else
				delete cur;
				cur = NULL;
#endif

				size--;

				return true;
			}
		}

		return false;
	}

	Bool Contains(const _Type& v)
	{
		NodePtr temp = head;

		//Keep this as NOT an assert so it doesn't crash on empty lists
		if (!temp)
			return false;

		while (v != temp->item)
		{
			temp = temp->next;
			
			if (!temp)
				return false;
		}

		return true;
	}

	void Clear()
	{
		RClear(head);
		head = NULL;
		size = 0;
	}

	_Type& operator [] (UInt32 id) const
	{
		ASSERT(id < size);

		NodePtr node = head;

		for (UInt32 i = 0; i < id; i++)
			node = node->next;

		return (node->item);
	}

	Iterator Find(const _Type& v) const
	{
		Iterator i(*this);
		for (i.Begin(); !i.End(); ++i)
			if (memcmp(i.Current(), v, sizeof(_Type)) == 0)
				return i;

		ASSERT(false);

		//Just to quiet the warning of no return path
		return i;
	}

private:
	void RClear(Node *cur)
	{
		if (!cur)
			return;
		RClear(cur->next);
		delete cur;
		size--;
	}
};