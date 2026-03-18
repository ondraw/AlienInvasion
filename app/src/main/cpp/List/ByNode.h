// ByNode.h: interface for the ByNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BYNODE_H__AB2B5BA1_220B_419A_A5FF_965860130287__INCLUDED_)
#define AFX_BYNODE_H__AB2B5BA1_220B_419A_A5FF_965860130287__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

template <class T>
class ByNode
{
	public:				
		ByNode()
		{
			m_pTData = 0;
		};		
		ByNode(T* pData)
		{
			m_pTData = pData;
		};
		
		~ByNode()
		{
			
			//Modiefied By Song 2010.11.24
			//데이터를 지우지 않고 밖에서 지우도록 해야 한다.
			//그이유는 리스트를 카피를 했을 경우 가피된 리스트에서 데이터를 지워버리면
			//원본에 영향을 주기 때문이다.
			m_pTData = 0;
//			if(m_pTData)
//			{
//				delete m_pTData;
//				m_pTData = 0;
//			}
		};
	
		T* GetObject()
		{
			return m_pTData;
		};
	
		void SetObject(T* pdata)
		{
			m_pTData = pdata;
		};
		
		void SetNextNode(ByNode* pNextNode)
		{
			m_lpNextNode = pNextNode;
		};
	
		void SetPrevNode(ByNode* pPrevNode)
		{
			m_lpPrevNode = pPrevNode;
		};

		ByNode<T>* GetNextNode()
		{
			return m_lpNextNode;
		};
	
		ByNode<T>* GetPrevNode()
		{
			return m_lpPrevNode;
		};
	
	private:
		ByNode * m_lpNextNode;
		ByNode * m_lpPrevNode;	
		T	   * m_pTData;  
};

#endif // !defined(AFX_BYNODE_H__AB2B5BA1_220B_419A_A5FF_965860130287__INCLUDED_)
