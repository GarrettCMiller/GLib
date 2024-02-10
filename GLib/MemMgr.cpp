/////////////////////////////////////////////////////////////////////////////
//		File:			MemMgr.cpp
//
//		Programmmer:	Garrett Miller (GM)
//
//		Date Created:	2/21/2006 12:51:48 AM
//
//		Description:	A memory node manager system
//
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include ".\memmgr.h"
#include "XMLTables.h"

#include "Globals.h"

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_SINGLETON(MemMgr);

//////////////////////////////////////////////////////////////////////////

MemMgr::MemMgr(void)
{
	m_pPool = NULL;
}

//////////////////////////////////////////////////////////////////////////

MemMgr::~MemMgr(void)
{
	UInt32	usedb	= m_Used.Size(),	//Used blocks
			ubytes	= 0,				//Used bytes
			freeb	= m_Free.Size(),	//Free blocks
			fbytes	= 0,				//Free bytes
			tbytes	= 0;				//Total bytes

	DblLinkedList<Node>::Iterator i(m_Used);

	for (i.Begin(); !i.End(); ++i)
	{
		ubytes += i.Current().uSize;
	}

	i = DblLinkedList<Node>::Iterator(m_Free);

	for (i.Begin(); !i.End(); ++i)
	{
		fbytes += i.Current().uSize;
	}

	m_Used.Clear();
	m_Free.Clear();

	tbytes = ubytes + fbytes;

	g_Log	<<	"Released " << usedb << " used memory block(s) totaling " << ubytes << " byte(s) and " 
			<<	freeb << " free memory block(s) totaling " << fbytes << " byte(s)!\n";
	
	g_Log	<<	"The total of all memory released is " << tbytes << " byte(s)!\n";

	free(m_pPool);
	
	m_pPool = NULL;
}

///////////////////////////////////////////////////////////////////////////////

UInt32 MemMgr::Init(UInt32 dwPoolSize)
{
	g_Log << "Initializing memory manager...\n";
	//g_Log.SetMode(eLM_UList, false);

	if (m_pPool)
	{
		g_Log.LOG_ERROR("Tried to create additional memory pool!\n");
		return 0;
	}

	m_uPoolSize = dwPoolSize;

	if (m_uPoolSize == 0)
	{
		g_Log.LOG_ERROR("Tried to create a NULL-sized memory pool!\n");
		return 0;
	}

	//Allocate the pool
	m_pPool = malloc(m_uPoolSize);

	if (!m_pPool)
	{
		g_Log.LOG_ERROR("Memory pool couldn't be created!!!!\n");
		return 0;
	}

	//Set memory pattern
	//ZeroMemory(m_pPool, PoolSize);
	for (	UInt32* mem = (UInt32 *) m_pPool, alt = 0; 
			mem < (UInt32 *) m_pPool + (m_uPoolSize / 4); 
			mem++, alt++)
	{
		*mem = (alt % 2 ? MEM_BLOCK_1 : MEM_BLOCK_2);
	}

	Node pool = Node(m_uPoolSize, m_pPool);

	m_Free.Add(pool);

	g_Log <<	"Memory pool created successfully!\n";
	g_Log <<	"Memory pool created with " << m_uPoolSize << " bytes allocated (" << Float(m_uPoolSize) / 1024.0f << " KB or " << Float(m_uPoolSize / Float(1024 * 1024)) << " MB)!\n";

	//g_Log.EndList();

	return 1;
}

//////////////////////////////////////////////////////////////////////////

void* MemMgr::_AllocateRel(size_t size)
{
	ASSERT(size <= m_uPoolSize);

	DblLinkedList<Node>::Iterator	i(m_Free);
	
	UInt32	nodeID	= 0;
	Node	temp;

	for (i.Begin(); !i.End(); ++i, nodeID++)
	{
		temp = i.Current();

		if (temp.uSize >= size)
		{
			//Take the node outta the free list
			m_Free.Remove(temp);
			
			//Split it up into two nodes
			Node newnode = Node(temp.uSize - size);

			newnode.pBlock = (void *)(UInt32 (temp.pBlock) + size);
			
			//Add the new one back into the free list
			m_Free.Add(newnode);

			//Adjust the size of the node
			temp.uSize = UInt32(size);

			//Put the one we found into the used list
			m_Used.Add(temp);

			return (temp.pBlock);
		}
	}

	char t2[MAX_PATH] = {'\0'};
	_itoa_s(size, t2, MAX_PATH, 10);
	g_Log.LOG_ERROR(String("Unable to allocate a block of ") + String(t2) + String(" bytes!\n"));

	return NULL;
}

//////////////////////////////////////////////////////////////////////////

void* MemMgr::_AllocateDbg(size_t size, String Desc, UInt32 lineNo, String func)
{
	ASSERT(size <= m_uPoolSize);

	DblLinkedList<Node >::Iterator	i(m_Free);
	UInt32	nodeID	= 0;
	Node	temp;

	for (i.Begin(); !i.End(); ++i, nodeID++)
	{
		temp = i.Current();

		if (temp.uSize >= size)
		{
			//Take the node outta the free list
			m_Free.Remove(temp);
			
			//Split it up into two nodes
			Node newnode = Node(temp.uSize - size);

			newnode.pBlock	= (void *)(UInt32(temp.pBlock) + size);
			
			//Add the new one back into the free list
			m_Free.Add(newnode);

			//Adjust the size of the node
			temp.uSize = UInt32(size);

#ifdef _DEBUG
			strncpy_s(temp.strDesc, Desc.c_str(), 256);
			strncpy_s(temp.strFunc, func.c_str(), 256);
			temp.uLineNo = lineNo;
#endif

			//Put the one we found into the used list
			m_Used.Add(temp);

			return (temp.pBlock);
		}
	}

	ASSERT(false);

	char tbuf[128] = {'\0'};

	sprintf_s(tbuf, "Unable to allocate a block of %d bytes!\n", size);
	g_Log.LOG_ERROR(tbuf);

	sprintf_s(tbuf, "Tried to allocate at line number %d in %s()!\n", lineNo, func);
	g_Log.LOG_ERROR(tbuf);

	return NULL;
}

//////////////////////////////////////////////////////////////////////////

Bool MemMgr::Release(void* mem)
{
	ASSERT(mem);

	DblLinkedList<Node>::Iterator	i(m_Used);
	
	UInt32	nodeID	= 0;
	Node	temp;

	for (i.Begin(); !i.End(); ++i, nodeID++)
	{
		temp = i.Current();

		//We found the block
		if (temp.pBlock == mem)
		{
			m_Used.Remove(temp);
			m_Free.Add(temp);
			
			//Merge(temp);
			//Merge(UInt32(0));
			//Merge(-1);

			return true;
		}
	}

	ASSERT(false);
	g_Log.LOG_ERROR("Unable to find node!\n");

	return false;
}

//////////////////////////////////////////////////////////////////////////

void MemMgr::Dump(String FilePath)
{
	Logger	memDump(false, FilePath);

	memDump << "Pool Size: " << m_uPoolSize << " Bytes (" << Float(m_uPoolSize) / 1024 << " KB or " << (Float(m_uPoolSize) / (1024 * 1024)) << " MB)\n";

	memDump.SetMode(eLM_Table);

	if (memDump.IsHTML())
	{
		memDump << "Node" << eLFF_NewTableData << "Address" << eLFF_NewTableData << "Desc" << eLFF_NewTableData 
				<< "Line" << eLFF_NewTableData << "Function" << eLFF_NewTableData << "Node Size" << eLFF_NewTableData << "Used?\n";

		memDump << eLFF_NewTableRow;
	}
	else
	{
		memDump <<	"---------------------------------------------------------------------------------------------------------------------------------\n"
					"Node	|	Address			|	Desc				|	Line		|	Function			|	Node size		|	Used		  \n"
					"---------------------------------------------------------------------------------------------------------------------------------\n";
	}

	char buffer[512];
	char addy[32];
	/*Int32 let = -1;
	Int32 flet = 0;*/
	UInt32 nodeid = 1;
	UInt32 memtotal = 0;
	Node	node;

	DblLinkedList<Node >::Iterator	i(m_Free);

	for (i.Begin(); !i.End(); ++i, nodeid++)
	{
		node = i.Current();

		memtotal += node.uSize;

		if (memDump.IsHTML())
		{
			sprintf_s(addy, "0x%.8X", UInt32(node.pBlock));

			memDump << nodeid << eLFF_NewTableData << addy << eLFF_NewTableData << 
#ifdef _DEBUG
				(node.strDesc[0] == 'Í' ? "N/A" : node.strDesc) << eLFF_NewTableData <<
#else
				"N/A" << eLFF_NewTableData << 
#endif
				"N/A" << eLFF_NewTableData << "N/A" << eLFF_NewTableData << node.uSize << eLFF_NewTableData << "False"
				<< eLFF_NewTableRow;
		}
		else
		{
			sprintf_s(buffer, "%.3d \t\t 0x%.8X \t\t\t %s \t\t\t\t\t %s \t\t %s \t\t\t\t\t %d \t\t %s \n",
									nodeid, UInt32(node.pBlock), 
#ifdef _DEBUG
									(node.strDesc[0] == 'Í' ? "N/A" : node.strDesc), 
#else
									"N/A",
#endif
									"N/A", "N/A",
									node.uSize,	 "False");
			
			memDump << buffer << eLFF_NewTableRow;
		}

		
	}

	memDump.SetMode();

	memDump << "\n\n";

	memDump << "Total Free Memory: " << memtotal << " Bytes (" << Float(memtotal) / 1024 << " KB or " << (Float(memtotal) / (1024 * 1024)) << " MB)\n\n";

	memDump << "\n\n";

	memDump.SetMode(eLM_Table);

	if (memDump.IsHTML())
	{
		memDump << "Node" << eLFF_NewTableData << "Address" << eLFF_NewTableData << "Desc" << eLFF_NewTableData 
						<< "Line" << eLFF_NewTableData << "Function" << eLFF_NewTableData << "Node Size" << eLFF_NewTableData << "Used?\n";

		memDump << eLFF_NewTableRow;
	}
	else
	{
		memDump <<	"--------------------------------------------------------------------------------------------------------------------------------\n"
							"Node	|	Address				|	Desc						|	Line		|	Function			|	Node size		|	Used		  \n"
							"--------------------------------------------------------------------------------------------------------------------------------\n";
	}

	i = DblLinkedList<Node>::Iterator(m_Used);
	nodeid = 1;
	memtotal = 0;

	for (i.Begin(); !i.End(); ++i, nodeid++)
	{
		node = i.Current();

		if (memDump.IsHTML())
		{
			sprintf_s(addy, "0x%.8X", reinterpret_cast<UInt32>(node.pBlock));

			memDump << nodeid << eLFF_NewTableData << addy << eLFF_NewTableData << 
#ifdef _DEBUG
				node.strDesc << eLFF_NewTableData << node.uLineNo << eLFF_NewTableData << node.strFunc << eLFF_NewTableData << 
#endif
				node.uSize << eLFF_NewTableData << "True" << eLFF_NewTableRow;
		}
		else
		{
			sprintf_s(buffer, "%.3d \t\t 0x%.8X \t\t %d \t\t %s \n",
								nodeid, reinterpret_cast<UInt32>(node.pBlock), node.uSize,	 "True");

			memDump << buffer << eLFF_NewTableRow;
		}

		memtotal += node.uSize;
	}

	memDump.SetMode();

	memDump << "\n\n";

	memDump << "Total Used Memory: " << memtotal << " Bytes (" << static_cast<Float>(memtotal) / 1024 << " KB or " << (static_cast<Float>(memtotal) / (1024 * 1024)) << " MB)\n";
}

//////////////////////////////////////////////////////////////////////////

void MemMgr::Flush()
{
	g_Log << "Begin MemMgr::Flush()...\n";

	m_Used.Clear();
	m_Free.Clear();

	ZeroMemory(m_pPool, m_uPoolSize);

	Node pool = Node(m_uPoolSize, m_pPool);

	m_Free.Add(pool);

	g_Log << "End MemMgr::Flush()!\n";
}

//////////////////////////////////////////////////////////////////////////

Bool MemMgr::IsValid(void* mem)
{
	return (UInt32(mem) <= UInt32(m_pPool) + m_uPoolSize / 4 && UInt32(mem) >= UInt32(m_pPool));
}

//////////////////////////////////////////////////////////////////////////

void MemMgr::Merge(Node& node)
{
	DblLinkedList<Node >::Iterator	i(m_Free);
	
	UInt32		tid		= 0;
	Node		temp,
				front,
				back;

	for (i.Begin(); !i.End(); ++i, tid++)
	{
		temp = i.Current();

		i.Next(front);
		i.Prev(back);

		if (temp.pBlock == node.pBlock)
		{
			//Handle the node in front first
			if (/*front && */((void *)(UInt32(temp.pBlock) + temp.uSize)) == front.pBlock)		//If the next free block is contiguous with this
			{
				temp.uSize += front.uSize;
				m_Free.Remove(front);
			}

			//Next check behind us
			if (/*back && */((void *) (UInt32(back.pBlock) + back.uSize)) == temp.pBlock)			//If the previous block is contiguous with this
			{
				back.uSize += temp.uSize;
				m_Free.Remove(temp);
			}

			//return;
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void MemMgr::Merge(UInt32 id)
{
	DblLinkedList<Node >::Iterator	i(m_Free),
										j(m_Free);
	UInt32	tid		= 0,
			icount	= 0;

	Node	temp,
			front,
			back;

	Bool	removed = false;

	for (i.Begin(); !i.End(); ++i, icount++)
	{
		temp = i.Current();

		tid = 0;

		for (j.Begin(); !j.End(); ++j, tid++)
		{
			if (id == -1 || tid == id)
			{
				removed = false;

				//Handle the node in front first
				//If the next free block is contiguous with this
				if (tid < m_Free.Size() - 1 && ((void *) (UInt32(temp.pBlock) + temp.uSize)) == front.pBlock)
				{
					j.Next(front);

					temp.uSize += front.uSize;
					m_Free.Remove(front);
					removed = true;
				}

				//Next check behind us
				//If the previous block is contiguous with this
				if (tid != 0 && ((void *) (UInt32(back.pBlock) + back.uSize)) == temp.pBlock)
				{
					j.Prev(back);

					back.uSize += temp.uSize;
					m_Free.Remove(temp);
					removed = true;
				}

				if (removed && id != -1)
					return;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////