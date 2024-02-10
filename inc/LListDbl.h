///////////////////////////////////////////////////////////////////////////////
//		File:				LListDbl.h
//
//		Programmer:			Garrett Miller (GM)
//
//		Date created:		August 11, 2004
//
//		Description:		Templated doubly-linked linked list with an iterator class
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DataTypes.h"

//////////////////////////////////////////////////////////////////////////

template <typename _Type>
class DblLinkedList
{
protected:
	struct Node
	{
		typedef Node*					NodePtr;

		Node() : next(NULL), prev(NULL)
		{ 	}

		_Type	item;
		
		NodePtr	next;
		NodePtr	prev;
	};

#ifdef USING_SMART_POINTERS
	typedef CIntrusivePtr<Node>		NodePtr;
#else
	typedef Node*					NodePtr;
#endif

	NodePtr	head;
	NodePtr	tail;

	UInt32	size;

public:
	class Iterator
	{
		NodePtr	i;
		NodePtr	head;
		NodePtr	tail;
		
		DblLinkedList<_Type>* list;

		UInt32	CurrentPos;
		UInt32	MaxPos;

	public:
		Iterator() : i(NULL), head(NULL), tail(NULL), CurrentPos(0xffffffff), MaxPos(0xffffffff) 
		{	}

		Iterator(DblLinkedList<_Type>& _list)
		{
			list = &_list;
			head = list->head;
			tail = list->tail;

			CurrentPos = 0;
			MaxPos = list->Size();
		}

		void Begin()
		{
			i = head;
			CurrentPos = 0;
		}

		void Tail()
		{
			i = tail;
			CurrentPos = MaxPos - 1;
		}

		Bool End()
		{
			return (i == (void *)NULL);
		}

		void operator ++()
		{
			ASSERT(i);
			MaxPos = list->Size();

			i = i->next;
			CurrentPos++;
		}

		void operator --()
		{
			ASSERT(i);
			MaxPos = list->Size();

			i = i->prev;
			CurrentPos--;
		}

		_Type& Current()
		{
			return i->item;
		}

		NodePtr Self()
		{
			return i;
		}

		UInt32 CurrentPosition()
		{
			return CurrentPos;
		}

		void Next(_Type& item)
		{
			ASSERT(i);
			ASSERT((i->next));
			item = i->next->item;
		}

		_Type* Next()
		{
			ASSERT(i);
			ASSERT((i->next));
			return &i->next->item;
		}

		void Prev(_Type& item)
		{
			ASSERT(i);
			ASSERT((i->prev));
			item = i->prev->item;
		}

		_Type* Prev()
		{
			ASSERT(i);
			ASSERT((i->prev));
			return &i->prev->item;
		}
	};

public:
	//////////////////////////////////////////////////////////
	// Function:			constructor
	//
	// Last modified:	TODO
	//
	// Input:				
	//
	// Output:			
	//
	// Returns:			
	//
	// Purpose:			
	//
	//////////////////////////////////////////////////////////
	DblLinkedList() : head(NULL), tail(NULL), size(0)	{	}

	//////////////////////////////////////////////////////////
	// Function:			destructor
	//
	// Last modified:	TODO
	//
	// Input:				
	//
	// Output:			
	//
	// Returns:			
	//
	// Purpose:			
	//
	//////////////////////////////////////////////////////////
	virtual ~DblLinkedList()
	{
		Clear();
	}

	//////////////////////////////////////
	//List manipulation funcs
	//////////////////////////////////////
	//////////////////////////////////////////////////////////
	// Function:			IsEmpty()
	//
	// Last modified:	TODO
	//
	// Input:				
	//
	// Output:			
	//
	// Returns:			
	//
	// Purpose:			
	//
	//////////////////////////////////////////////////////////
	Bool IsEmpty() const
	{
		return (head == (void *)NULL || tail == (void *)NULL);
	}

	//////////////////////////////////////////////////////////
	// Function:			Int32 ()
	//
	// Last modified:	TODO
	//
	// Input:				
	//
	// Output:			
	//
	// Returns:			
	//
	// Purpose:			
	//
	//////////////////////////////////////////////////////////
	UInt32 Size() const
	{
		return size;
	}

	//////////////////////////////////////////////////////////
	// Function:			Add()
	//
	// Last modified:	TODO
	//
	// Input:				
	//
	// Output:			
	//
	// Returns:			
	//
	// Purpose:			
	//
	//////////////////////////////////////////////////////////
	void Add(const _Type &v, Bool ToHead = false)
	{
		static const UInt32 Null = 0;

		NodePtr n = new Node;

		ASSERT(n);

		n->item	= v;

		if (ToHead)
		{
			n->next	= head;
			n->prev	= NULL;

			if (n->next)
				n->next->prev = n;

			head = n;

			if (!tail)
				tail = head;
		}
		else
		{
			n->next	= NULL;
			n->prev	= tail;
			
			if (n->prev)
				n->prev->next = n;
	
			tail = n;

			if (!head)
				head = tail;
		}

		size++;
	}

	void AddAfter(const _Type &v, Node* after)
	{
		NodePtr n	= new Node;

		ASSERT(n);

		n->item	= v;

		n->next	= after->next;
		n->prev	= after;

		after->next = n;

		if (n->next)
			n->next->prev = n;

		size++;
	}

	//////////////////////////////////////////////////////////
	// Function:			Remove()
	//
	// Last modified:	TODO
	//
	// Input:				
	//
	// Output:			
	//
	// Returns:			
	//
	// Purpose:			
	//
	//////////////////////////////////////////////////////////
	void Remove(UInt32 node)
	{
		ASSERT(!IsEmpty());
		ASSERT(size > 0);
		ASSERT(node < size);

		NodePtr		temp	= GetNodeAt(node),
					tprev	= NULL,
					tnext	= NULL;

		ASSERT(temp);

		if (temp->prev)
			tprev = temp->prev;

		if (temp->next)
			tnext = temp->next;

		if (tprev)
			tprev->next = tnext;
		else if (temp == head)
			head = tnext;
		else
			ASSERT(false);	//Why would we hit this? Just in case

		if (tnext)
			tnext->prev	= tprev;
		else if (temp == tail)
			tail = tprev;
		else
			ASSERT(false);	//Why would we hit this? Just in case
		
#ifdef USING_SMART_POINTERS
		temp->Release();
#else
		delete temp;
		temp = NULL;
#endif

		size--;
	}

	void Remove(NodePtr pNode)
	{
		ASSERT(!IsEmpty());
		ASSERT(size > 0);
		ASSERT(Contains(pNode->item));

		NodePtr		temp	= pNode,
					tprev	= pNode->prev,
					tnext	= pNode->next;

		if (tprev)
			tprev->next	= tnext;
		else if (temp == head)
			head = tnext;
		else
			ASSERT(false);		//Why would we hit this? Just in case

		if (tnext)
			tnext->prev = tprev;
		else if (temp == tail)
			tail = tprev;
		else
			ASSERT(false);		//Why would we hit this? Just in case

#ifdef USING_SMART_POINTERS
		temp->Release();
#else
		delete temp;
		temp = NULL;
#endif

		size--;
	}

	void Remove(_Type &node)
	{
		ASSERT(!IsEmpty());
		ASSERT(size > 0);
		ASSERT(Contains(node));

		NodePtr		temp	= head,
					tprev	= NULL,
					tnext	= NULL;

		if (temp->prev)
			tprev = temp->prev;

		if (temp->next)
			tnext = temp->next;

		while (node != temp->item)
		{
			tprev = temp;
			
			temp = temp->next;
			
			ASSERT(temp);

			tnext = temp->next;
		}

		if (tprev)
			tprev->next	= tnext;
		else if (temp == head)
			head = tnext;
		else
			ASSERT(false);		//Why would we hit this? Just in case

		if (tnext)
			tnext->prev = tprev;
		else if (temp == tail)
			tail = tprev;
		else
			ASSERT(false);		//Why would we hit this? Just in case

#ifdef USING_SMART_POINTERS
		temp->Release();
#else
		delete temp;
		temp = NULL;
#endif

		size--;
	}

	//////////////////////////////////////////////////////////
	// Function:			Clear()
	//
	// Last modified:	TODO
	//
	// Input:				
	//
	// Output:			
	//
	// Returns:			
	//
	// Purpose:			
	//
	//////////////////////////////////////////////////////////
	void Clear()
	{
		NodePtr n = NULL;

		while (head)
		{
			n		= head;
			head	= head->next;

#ifdef USING_SMART_POINTERS
			n->Release();
#else
			delete	n;
			n		= NULL;
#endif
		}

		head = tail = NULL;

		size = 0;
	}

	//////////////////////////////////////////////////////////
	// Function:			bracket operator
	//
	// Last modified:	TODO
	//
	// Input:				
	//
	// Output:			
	//
	// Returns:			
	//
	// Purpose:			
	//
	//////////////////////////////////////////////////////////
	_Type &operator[](UInt32 id) const
	{
		ASSERT(id < size);

		NodePtr node = head;

		for (UInt32 i = 0; i < id; i++)
			node = node->next;

		return (node->item);
	}

	//////////////////////////////////////////////////////////
	// Function:			bracket operator
	//
	// Last modified:	TODO
	//
	// Input:				
	//
	// Output:			
	//
	// Returns:			
	//
	// Purpose:			
	//
	//////////////////////////////////////////////////////////
 	NodePtr	GetNodeAt(UInt32 node) const
 	{
 		ASSERT(node < size);
		ASSERT(node >= 0);
 
 		NodePtr temp = head;

 		for (UInt32 i = 0; i < node; i++)
		{
			ASSERT(temp);

			if (temp)
 				temp = temp->next;
		}

 		return temp;
 	}

	Bool Contains(const _Type& obj)
	{
		Iterator i(*this);
		
		for (i.Begin(); !i.End(); ++i)
		{
			_Type& temp = i.Current();

			if (obj == temp)
				return true;
		}

		return false;
	}
};