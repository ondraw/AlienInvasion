/*
 *  hQueue.h
 *  SongGL
 *
 *  Created by 호학 송 on 11. 3. 18..
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef HQUEUE_H
#define HQUEUE_H
#include "ListNode.h"
#include <pthread.h>

/**
 큐의 값은 쓰레드에 안전 하다.
 */
template <class T>
class HQueue
{
public:
	HQueue() { 
		 pthread_mutex_init(&mMutex, NULL);
	};
	~HQueue() { };
	
public:
	bool IsQueue()
	{
		if(mList.Size() > 0) 
			return true;
		return false;
	}
	
	int Size() 
	{ 
		return mList.Size();
	}
	
	void Offer(T* v) 
	{
		//pthread_mutex_lock(&mMutex);
		mList.AddTTail(v);
		//pthread_mutex_unlock(&mMutex);
	}
	
	T* Poll() { 
		T* v = 0;
		pthread_mutex_lock(&mMutex);
		int nCnt = mList.Size();
		if(nCnt > 0) 
		{
			v = mList.GetAt(nCnt -1);
			mList.Delete(nCnt - 1);
		}
		pthread_mutex_unlock(&mMutex);
		return v;
	}
	
	T* Peek() 
	{
		T* v = 0;
		pthread_mutex_lock(&mMutex);
		int nCnt = mList.Size();
		if(nCnt > 0) 
		{
			v = mList.GetAt(nCnt -1);
		}
		pthread_mutex_unlock(&mMutex);
	}

	T* GetAt(int i) 
	{
		T* v = 0;
		pthread_mutex_lock(&mMutex);
        int nCnt = mList.Size();
        if( i < nCnt)
            v = mList.GetAt(i);
		pthread_mutex_unlock(&mMutex);
		return v;
	}
    
    void Remove(int i) 
	{
		pthread_mutex_lock(&mMutex);
        int nCnt = mList.Size();
        if( i < nCnt)
            mList.Delete(i);
		pthread_mutex_unlock(&mMutex);
	}

    
    void Clear()
    {
        pthread_mutex_lock(&mMutex);
		mList.Clear();
		pthread_mutex_unlock(&mMutex);
    }
	
	
private:
	CListNode<T> mList;
	pthread_mutex_t mMutex;
};

#endif//