///////////////////////////////////////////////////////////////////////////
//		File:				Tree.h
//
//		Programmer:			Garrett Miller (GM) (c) 2004
//
//		Date created:		N/A
//
//		Description:		A templated tree object
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "Functor.h"

//////////////////////////////////////////////////////////////////////////

template <typename _Type, UInt32 MaxNumChildren = 2>
class CTree
{
public:
	struct Node;
	typedef Node*	NodePtr;

protected:
	
	struct Node
	{
		Node() : numChildren(0), numSubChildren(0), customData(0), parent(NULL)
		{ ZeroMemory(child, MaxNumChildren * sizeof(NodePtr)); }

		Node(_Type _item) : numChildren(0), numSubChildren(0), item(_item), customData(0), parent(NULL)
		{ ZeroMemory(child, MaxNumChildren * sizeof(NodePtr)); }

		Node(const Node* pNode)
		{
			Copy(pNode);
		}

		_Type		item;

		UInt32		numChildren;
		UInt32		numSubChildren;
		UInt32		customData;
		NodePtr		parent;
		NodePtr		child[MaxNumChildren];

		void Copy(const Node* pNode)
		{
			if (pNode)
			{
				item			= pNode->item;
				customData		= pNode->customData;
				numSubChildren	= 0;
				parent			= pNode->parent;
				
				IncSubChild();

				for (numChildren = 0; numChildren < pNode->numChildren; numChildren++)
				{
					child[numChildren] = new Node(pNode->child[numChildren]);
				}
			}
		}

		UInt32 GetDepth() const
		{
			UInt32 depth	= 0;
			NodePtr pParent	= this->parent;

			while (pParent != NULL)
			{
				depth++;
				pParent = pParent->parent
			}
			
			return depth;
		}

		Bool HasChildren() const
		{
			return (numChildren > 0);
		}

		void IncSubChild()
		{
			if (parent)
			{
				parent->numSubChildren++;
				parent->IncSubChild();
			}

			//numSubChildren++;
		}

		void DecSubChild()
		{
			if (parent)
			{
				parent->numSubChildren--;
				parent->DecSubChild();
			}

			//numSubChildren--;
		}

		void AddChild(NodePtr pNode)
		{
			ASSERT(pNode);
			ASSERT(numChildren < MaxNumChildren);
			pNode->parent = this;
			child[numChildren++] = pNode;

			IncSubChild();
		}

		Bool RemoveChild(Int32 index = -1)
		{
			if (numChildren == 0)	//if we are a leaf
			{
				return true;		//then we are done recursing
			}
			else
			{
				if (index == -1)
				{
					for (Int32 i = 0; i < numChildren; i++)
					{
						if (child[i] && child[i]->RemoveChild())
						{
							delete child[i];
							child[i] = NULL;
						}
					}
				}
				else
				{
					if (child[index] && child[index]->RemoveChild())
					{
						delete child[index];
						child[index] = NULL;
					}
				}

				DecSubChild();

				return true;
			}
		}

		NodePtr operator [] (UInt32 index)
		{
			ASSERT(index < numChildren);
			return child[index];
		}
	};

	NodePtr		m_pRoot;
	UInt32		m_Size;

	NodePtr		m_pTemp;

public:

	CTree(void) : m_pRoot(NULL), m_Size(0)
	{
	}

	void CopyFromNode(NodePtr pNode)
	{
		m_pRoot = pNode;
		m_Size = pNode->numChildren + pNode->numSubChildren + 1 /*root*/;
	}

	void CopyFromNode(NodePtr pNode, Bool bTopLevel)
	{
		if (!pNode)
			return;

		if (bTopLevel)
		{
			//Clear();
			m_pRoot = NULL;
			m_Size = 0;

			//NodePtr copy = pNode;
			m_pTemp = NULL;
		}

		while (pNode)
		{
			m_pTemp = Add(pNode->item, m_pTemp);
			//m_pTemp = pNode;

			CopyFromNode(pNode->child[0], false);
			CopyFromNode(pNode->child[1], false);
		}
	}

	virtual ~CTree(void)
	{
		Clear();
	}

	///////////////////////////

	void AddNodeFromTree(NodePtr pNode)
	{
		Add(pNode, m_pTemp);
		m_pTemp = pNode;
	}

	///////////////////////////

	Bool IsRoot(NodePtr pNode)
	{
		return (pNode == m_pRoot);
	}

	Bool IsEmpty() const
	{
		return (!VALID_PTR(m_pRoot));
	}

	UInt32 Size() const
	{
		return m_Size;
	}

	NodePtr Add(_Type item, NodePtr parent)
	{
		NodePtr node = new Node(item);
		ASSERT(node);

		if (parent)
			parent->AddChild(node);
		else
		{
			if (m_pRoot)
				m_pRoot->AddChild(node);
			else
				m_pRoot = node;
		}

		m_Size++;

		return node;
	}

	NodePtr Add(NodePtr pNode, NodePtr parent)
	{
		if (parent)
			parent->AddChild(pNode);
		else
		{
			if (m_pRoot)
				m_pRoot->AddChild(pNode);
			else
				m_pRoot = pNode;
		}

		m_Size++;

		return pNode;
	}

	template <typename _FunctorType>
	void TraverseDepthFirst(NodePtr node, _FunctorType& functor)
	{
		if (!m_pRoot)
			return;

		if (!node)
			node = m_pRoot;

		if (node->numChildren > 0)
		{
			for (UInt32 c = 0; c < node->numChildren; c++)
			{
				TraverseDepthFirst(node->child[c], functor);
			}
		}

		functor(node);
	}

	template <typename _FunctorType>
	Int32 TraverseBreadthFirst(NodePtr node, _FunctorType& functor)
	{
		if (!m_pRoot)
			return 0;

		if (!node)
			node = m_pRoot;

		Int32 ret = functor(node);

		if (ret == 1)
			return ret;

		if (node->numChildren > 0)
		{
			for (UInt32 c = 0; c < node->numChildren; c++)
			{
				ret = TraverseBreadthFirst(node->child[c], functor);

				if (ret == 1)
					return ret;
			}
		}

		return ret;
	}

	template <typename _FunctorType>
	void TraverseMidFirst(NodePtr node, _FunctorType& functor)
	{
		ASSERT(MaxNumChildren == 2);

		if (!m_pRoot)
			return;

		if (!node)
			node = m_pRoot;

		if (node->child[0])
			TraverseMidFirst(node->child[0], functor);
			
		functor(node);
		
		if (node->child[1])
			TraverseMidFirst(node->child[1], functor);
	}

	Bool MoveNode(NodePtr nodeToMove, NodePtr nodeNewParent)
	{
		if (nodeToMove == nodeToMove->parent->child[0])
		{
			nodeToMove->parent->child[0] = nodeToMove->parent->child[0];
		}

		return true;
	}



	void Remove(NodePtr node)
	{
		ASSERT(node);
		node->RemoveChild();
		delete node;
	}

	void Clear()
	{
		if (m_pRoot)
		{
			Remove(m_pRoot);
			m_pRoot = NULL;
		}

		ASSERT(!m_pRoot);

		m_Size = 0;
	}
};
