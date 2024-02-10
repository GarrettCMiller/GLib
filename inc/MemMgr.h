/////////////////////////////////////////////////////////////////////////////
//		File:			MemMgr.h
//
//		Programmmer:	Garrett Miller (GM)
//
//		Date Created:	2/21/2006 12:51:42 AM
//
//		Description:	A memory node manager system
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Singleton.h"

#include "DataTypes.h"
#include "LListDbl.h"


//////////////////////////////////////////////////////////////////////////

#define MEM_BLOCK_1			0xDEAFEA12
#define	MEM_BLOCK_2			0xDEADD00D

//////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
	#define AllocateMem(_numbytes, _desc, _line, _func) \
				_AllocateDbg(_numbytes, _desc, _line, _func) 						
#else
	#define AllocateMem(_numbytes, _desc) \
				_AllocateRel(_numbytes)
#endif

//////////////////////////////////////////////////////////////////////////

class MemMgr : public Singleton<MemMgr>
{
	struct Node
	{
		Node(UInt32 size = 0, void* block = NULL) : uSize(size), pBlock(block) 
		{
#ifdef _DEBUG
			ZeroMemory(&strDesc, 128);
			ZeroMemory(&strFunc, 64);
			uLineNo = UInt32(-1);
			//*(&pBlock) = 0xC0C0C0C0;
#endif
		}

		Bool operator == (const Node& rhs) const 
		{
			return (pBlock == rhs.pBlock && uSize == rhs.uSize);
		}

		Bool operator != (const Node& rhs) const
		{
			return (!((*this) == rhs));
		}

		UInt32	uSize;
		void*	pBlock;

#ifdef _DEBUG
		char	strDesc[256];
		char	strFunc[256];
		UInt32		uLineNo;
#endif
	};

#ifdef USING_SMART_POINTERS
	typedef CIntrusivePtr<Node>		NodePtr;
#else
	typedef Node*					NodePtr;
#endif

	DblLinkedList<Node>	m_Used;
	DblLinkedList<Node>	m_Free;

	void*	m_pPool;

	UInt32	m_uPoolSize;

	//Merges memnodes together for less nodes/fragmentation
	void Merge(Node& id);
	void Merge(UInt32 id = -1);

public:
	MemMgr(void);
	virtual ~MemMgr(void);

	//Initializes mem pool 
	UInt32	Init(UInt32 dwPoolSize);
	
	//Bool Shutdown();

	void* _AllocateRel(size_t size);
	void* _AllocateDbg(size_t size, String Desc, UInt32 lineNo, String func);

	Bool Release(void *mem);

	void Dump(String FilePath = "MemMgrDump");

	void Flush();

	Bool IsValid(void *mem);
};

#define g_MemMgr		MemMgr::GetInstance()