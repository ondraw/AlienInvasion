//
//  CSThread.cpp
//  SongGL
//
//  Created by 호학 송 on 11. 4. 16..
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#include "sGLDefine.h"
#include "CSThread.h"

CSThread::CSThread()
{
    mThreadMainID = 0;
	mbThreadMainStop = false;
	mbThreadPing = true;
	mbThreadPong = false;
}

CSThread::~CSThread()
{
    if(mThreadMainID != 0)
	{
        //2016.06.10 stack corruption detected 안드로이드 롤리팝에서 발생한다. 반드시 void*로 받아야 한다.
        void* pResult = NULL;
		mbThreadMainStop = true;
    
        //Android 에서 파워를 종료할때 쓰레드 종료에서 멈춘다. 
        EndPong(); //Added By Song 2012.12.16
		pthread_join(mThreadMainID, &pResult);
		
		
		pthread_mutex_destroy(&mSyncMutex);
		pthread_cond_destroy(&mSyncCond);
	}
}

int CSThread::CreateThread(void* pFunction, void* ObjectParam)
{
    int nThreadResult = 0;
    pthread_mutex_init(&mSyncMutex, NULL);
	pthread_cond_init(&mSyncCond, NULL);
	
	nThreadResult = pthread_create(&mThreadMainID, 0, (void* (*)(void*))pFunction, (void*)ObjectParam);
	if(nThreadResult != 0)
	{
		mThreadMainID = 0;
		//sglLog("Error : Create Thread\n");
		return nThreadResult;
	}
    
    return nThreadResult;
}

void CSThread::BeginPing()
{
    pthread_mutex_lock(&mSyncMutex);
    while(mbThreadPing == false)
        pthread_cond_wait(&mSyncCond, &mSyncMutex);
}
void CSThread::EndPing()
{
    
    mbThreadPing = false;
    mbThreadPong = true;
    pthread_cond_signal(&mSyncCond);
    //4> 다른 프로세스에서 일을 하게끔 순서를 넘겨준다.
    pthread_mutex_unlock(&mSyncMutex);
}

void CSThread::BeginPong()
{
 	pthread_mutex_lock(&mSyncMutex);
	while(mbThreadPong == false)
		pthread_cond_wait(&mSyncCond, &mSyncMutex);   
}
void CSThread::EndPong()
{
    mbThreadPing = true;
	mbThreadPong = false;
	//무언가 동기화할 실행한다.	
	pthread_cond_signal(&mSyncCond);
	pthread_mutex_unlock(&mSyncMutex);
}