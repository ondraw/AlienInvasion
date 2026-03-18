// ListNode.h: interface for the ListNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LISTNODE_H__342355F2_971B_4CFE_BC76_3A8D930D809C__INCLUDED_)
#define AFX_LISTNODE_H__342355F2_971B_4CFE_BC76_3A8D930D809C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ByNode.h"
template <class T>
class CListNode  
{
public:
	CListNode()
	{
		InitNode();
	};	

	virtual ~CListNode() 
	{
		Reset();
	};	


	virtual bool AddTHead(T* pData)
	{	
		ByNode<T> * pNewNode=0;
		pNewNode = new ByNode<T>(pData);
		return AddTHead(pNewNode);		
	};

	virtual bool AddTTail(T* pData)
	{	
		ByNode<T> * pNewNode=0;
		pNewNode = new ByNode<T>(pData);
		return AddTTail(pNewNode);		
	};
	
	virtual int  Size() const { return m_iCnt;};
	T* operator [] (int offset)
	{
		ByNode<T>* tempNode;
		tempNode = m_Head.GetNextNode();
		
		//마지막 노드 까지 찾는다.	
		for( int i = 0; i <= offset && tempNode != &m_Tail ; i++) 
		{	
			if(i == offset)
				return tempNode->GetObject();
			tempNode = tempNode->GetNextNode();
		}	
		return 0;
	};	
	
	T* GetAt(int offset)
	{
		ByNode<T>* tempNode;
		tempNode = m_Head.GetNextNode();	
		//마지막 노드 까지 찾는다.	
		for( int i = 0; i <= offset && tempNode != &m_Tail ; i++) 
		{	
			if(i == offset)
				return tempNode->GetObject();
			tempNode = tempNode->GetNextNode();
		}	
		return 0;
	};

	virtual bool Delete(T* pData)
	{
		ByNode<T>* tempNode;
		tempNode = m_Head.GetNextNode();
		
		//마지막 노드 까지 찾는다.	
		for( int i = 0; tempNode != &m_Tail ; i++) 
		{			
			if(tempNode->GetObject() == pData)
				return Delete(tempNode);
			tempNode = tempNode->GetNextNode();
		}	
		return false;
	};

	virtual bool Delete(int offset)
	{
		ByNode<T>* tempNode;
		tempNode = m_Head.GetNextNode();
		
		//마지막 노드 까지 찾는다.	
		for( int i = 0; i <= offset && tempNode != &m_Tail ; i++) 
		{	
			if(i == offset)
				return Delete(tempNode);
			tempNode = tempNode->GetNextNode();
		}	
		return false;
	};

	virtual void Clear()
	{
		Reset();
	};
    
    virtual bool Delete(ByNode<T>* pOldNode)
	{
		if(!pOldNode)			return false;
		pOldNode->GetNextNode()->SetPrevNode(pOldNode->GetPrevNode());
		pOldNode->GetPrevNode()->SetNextNode(pOldNode->GetNextNode());
		delete pOldNode;
		pOldNode = 0;
		m_iCnt--;
		return true;
	};

	virtual ByNode<T>& GetHead() { return m_Head; };
	virtual ByNode<T>& GetTail() { return m_Tail; };
    
    //-----------------------------------------------
    ByNode<T>* begin() { return m_Head.GetNextNode();}
    ByNode<T>* end() { return &m_Tail;}
    //-----------------------------------------------
    
protected:
	virtual bool AddTHead(ByNode<T>* pNewNode)
	{	
		
		pNewNode -> SetNextNode(m_Head.GetNextNode());
		pNewNode -> SetPrevNode(&m_Head);		

		m_Head.GetNextNode()->SetPrevNode(pNewNode);
		m_Head.SetNextNode(pNewNode);
		m_iCnt++;
		return true;
	};

	virtual bool AddTTail(ByNode<T>* pNewNode)
	{	
		

		//자신의 같은 메모리를 쓰고 있는 노드가 있다면 false를 리턴한다.
		//더 빠른 연산을 하기 위해서 코멘트 로 막아 놓겠다.
		//ByNode<T> *pTempNode;
		//pTempNode = FindNode(pNewNode);
		//if(pTempNode != 0)
		//	return false;
		
		pNewNode -> SetPrevNode(m_Tail.GetPrevNode());
		pNewNode -> SetNextNode(&m_Tail);
		m_Tail.GetPrevNode()->SetNextNode(pNewNode);
		m_Tail.SetPrevNode(pNewNode);
		m_iCnt++;

		return true;
	};

	virtual bool AddTNodeNext(ByNode<T>* pNode,ByNode<T>* pNewNode)
	{	
		

		//자신의 같은 메모리를 쓰고 있는 노드가 있다면 false를 리턴한다.
		//더 빠른 연산을 하기 위해서 코멘트 로 막아 놓겠다.
		/*
		ByNode<T> *pTempNode;
		pTempNode = FindNode(pNewNode);
		if(pTempNode != 0)
			return false;
		*/
		
		if(pNode != &m_Tail) //Tail뒤에 첨가 할수 없다.
			return false;

		pNewNode -> SetNextNode(pNode->GetNextNode());
		pNewNode -> SetPrevNode(pNode);	
		pNode->GetNextNode()->SetPrevNode(pNewNode);
		pNode->SetNextNode(pNewNode);
		m_iCnt++;

		return true;
	};

	virtual void InitNode()
	{
		m_Head.SetNextNode(&m_Tail);
		m_Head.SetPrevNode(0);

		m_Tail.SetNextNode(0);
		m_Tail.SetPrevNode(&m_Head);	
		m_iCnt = 0;
	};

	virtual ByNode<T>* FindNode(ByNode<T>* node)
	{
		ByNode<T>* tempNode;
		tempNode = m_Head.GetNextNode();

		while( tempNode != &m_Tail) //마지막 노드 까지 찾는다.
		{
			if(tempNode == node)
				return tempNode;
			tempNode = tempNode->GetNextNode();
		}
		return 0; //찾는 노드가 없으면 NULL을 리턴한다.
	};

		
	
	virtual void Reset() 
	{
		
		ByNode<T> *pTempNode=0;
		ByNode<T> *pDeleteNode = 0;
		pTempNode = m_Head.GetNextNode();
		
		while( pTempNode != &m_Tail)
		{		
			pTempNode = pTempNode->GetNextNode();
			pDeleteNode = pTempNode->GetPrevNode();
			
			
			delete pDeleteNode;		
			
			pTempNode->SetPrevNode(0);
		}
		
		InitNode();
	};
	
protected:
	ByNode<T>  m_Head;
	ByNode<T>  m_Tail;
	int        m_iCnt;
};

#endif // !defined(AFX_LISTNODE_H__342355F2_971B_4CFE_BC76_3A8D930D809C__INCLUDED_)
