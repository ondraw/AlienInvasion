//
//  CSThread.h
//  SongGL
//
//  Created by 호학 송 on 11. 4. 16..
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#ifndef _CSTHREAD_H
#define _CSTHREAD_H
#include <pthread.h>
#include <unistd.h> 

class CSThread
{
public:
    CSThread();
    ~CSThread();
    
    
    int CreateThread(void* pFunction,void* ObjectParam);
    void BeginPing();
    void EndPing();
    
    void BeginPong();
    void EndPong();
    
    bool IsStop() { return mbThreadMainStop;}
    void Stop() { mbThreadMainStop = true;};
    
protected:
    //메인쓰레드 아이디
	pthread_t				mThreadMainID;
	pthread_cond_t			mSyncCond;
	pthread_mutex_t			mSyncMutex;
	bool					mbThreadMainStop;
	bool					mbThreadPing;
	bool					mbThreadPong;
};

#endif //_CSTHREAD_H