//
//  CSGLController.cpp
//  SongGL
//
//  Created by 호학 송 on 11. 7. 28..
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "CSGLController.h"
#include "CSprite.h"
#include "CGhostSprite.h"
#include "CSGLCore.h"
#include "CHWorld.h"
#include "AAICore.h"
#include "CDMainTower.h"

CSGLController::CSGLController(IHWorld* pWorld)
{
    mpWorld = pWorld;
    
    pthread_mutex_init(&mTeamInfoMutex, NULL);
}


CSGLController::~CSGLController()
{   
    Clear();
    
    pthread_mutex_destroy(&mTeamInfoMutex);
}


void CSGLController::Clear()
{
    pthread_mutex_lock(&mTeamInfoMutex); //락

    
    map<int,TEAMINFO*>::iterator p;
    for (p = mTeamInfo.begin(); p != mTeamInfo.end(); ++p)
    {
        delete p->second;
    }
    
    pthread_mutex_unlock(&mTeamInfoMutex); //언락
}

IHWorld* CSGLController::GetWorld()
{
    return mpWorld;
}

CSprite* CSGLController::GetMainBlz(int nTeamID)
{
    pthread_mutex_lock(&mTeamInfoMutex); //락
    map<int,TEAMINFO*>::const_iterator p = mTeamInfo.find(nTeamID);
    if (p == mTeamInfo.end())
    {
        return 0;
    }
    else
    {
        return p->second->pMainBlz;
    }
    pthread_mutex_unlock(&mTeamInfoMutex); //언락
}

//IAction Event Start

void CSGLController::OnAutoAimed(CSprite* pSrcObj)
{
	//이벤트를 보낸다 -- 파이어하라고 
	SGLEvent event;
	event.nMsgID = GSL_BOMB_FIRE;
	pSrcObj->OnEvent(&event);
	
}

void CSGLController::OnActotorDeleted(CSprite* pActor)
{
    pthread_mutex_lock(&mTeamInfoMutex); //락
    
    CHWorld* pWorld = (CHWorld*)mpWorld;
    CSGLCore* pSGLCore = pWorld->GetSGLCore();
    SPoint ptNow;
    SVector vDir;
    CSprite* pActorNew = new CGhostSprite(pWorld->GetNewID(),SGL_TEAMALL,-1,NULL,mpWorld);
    
    pActor->GetPosition(&ptNow);
    ptNow.y += 5.f; //고스트는 탱크의 카메라 타입이 없다. 그래서 서 거시기하다.
    pActor->GetModelDirection(&vDir);
    pActorNew->Initialize(&ptNow, &vDir);
    pSGLCore->AddSprite(pActorNew);
    mpWorld->SetActor(pActorNew);
    
    pthread_mutex_unlock(&mTeamInfoMutex); //언락
}

void CSGLController::OnAddedSprite(CSprite* pV)
{
    int nTeamID =   pV->GetTeamID();
    int nModelID =  pV->GetModelID();
    
//    //Runtime Item일때는 무관한다.
//    if(nTeamID == 0 && nModelID == -1)
//        return;
 
    pthread_mutex_lock(&mTeamInfoMutex); //락
    TEAMINFO* pTeamInfo = 0;
    
    map<int,TEAMINFO*>::const_iterator p = mTeamInfo.find(nTeamID);
    
    if (p == mTeamInfo.end())
    {
        pTeamInfo = new TEAMINFO;
        pTeamInfo->mapCnt[nModelID] = 1;
        pTeamInfo->nTotalCnt = 1;
        
        if(nModelID == ACTORID_BLZ_EMAIN)
            pTeamInfo->pMainBlz = pV;
        else
            pTeamInfo->pMainBlz = 0;
            
        mTeamInfo[nTeamID] = pTeamInfo;
    }
    else
    {
     
        pTeamInfo = p->second;
        map<int,int>::const_iterator p2 = pTeamInfo->mapCnt.find(nModelID);
        if (p2 == pTeamInfo->mapCnt.end())
        {
            pTeamInfo->mapCnt[nModelID] = 1;
        }
        else
        {
            pTeamInfo->mapCnt[nModelID] = p2->second + 1;
        }
        
        
        pTeamInfo->nTotalCnt++;
        if(nModelID == ACTORID_BLZ_EMAIN)
        {
            pTeamInfo->pMainBlz = pV;
//            pTeamInfo->nTotalCnt = 1;
        }
    }
    
    //총 인구수를 정하여... 탱크를 계속 만들 것인지 아닌지를 정한다.
    //건물이 아니라 객체가 만들어진다면.
    int nV = 0;
    if(pTeamInfo->nTotalCnt >= SGL_MAX_POPULATION)   nV = 0;
    else        nV = 1;
    
    //이팀의 메인 컨트롤 타워에 더이상 만들지 말라는 명령어를 보낸다.
    if(pTeamInfo->pMainBlz)
        pTeamInfo->pMainBlz->Command((int)AICOMMAND_MANUFACTURE,&nV);

    pthread_mutex_unlock(&mTeamInfoMutex); //언락
    
}

void CSGLController::OnDeletedSprite(CSprite* pV)
{
    int nTeamID = pV->GetTeamID();
    int nModelID = pV->GetModelID();
    pthread_mutex_lock(&mTeamInfoMutex); //락
    
    map<int,TEAMINFO*>::const_iterator p = mTeamInfo.find(nTeamID);
    if (p == mTeamInfo.end())
    {
        HLogE("[Error] Can not find teamid in teaminfo\n");
        pthread_mutex_unlock(&mTeamInfoMutex); //언락
        return;
    }
    TEAMINFO* pTeamInfo = p->second;
    map<int,int>::const_iterator p2 = pTeamInfo->mapCnt.find(nModelID);
    if (p2 == pTeamInfo->mapCnt.end())
    {
        HLogE("[Error] Can not find teamid in teaminfo of mapcnt\n");
        pthread_mutex_unlock(&mTeamInfoMutex); //언락
        return;
    }
    pTeamInfo->mapCnt[nModelID] = p2->second - 1;
    pTeamInfo->nTotalCnt--;

    
    //메인타워가 제거되면 ... 프로그램은 종료 된다.
    if(nModelID == ACTORID_BLZ_EMAIN)
    {
        if(pTeamInfo->pMainBlz && pTeamInfo->pMainBlz == pV) //먼저 새로운 메인타워가 Add되고 나서야 나중에 지워지기 때문에 새로운 메인타워 정보를 지우지 말아야 한다.
            pTeamInfo->pMainBlz = 0;
    }
    
    //총 인구수를 정하여... 탱크를 계속 만들 것인지 아닌지를 정한다.
    if(pTeamInfo->nTotalCnt < SGL_MAX_POPULATION)     
    {
        int nV = 1;
        //이팀의 메인 컨트롤 타워에  만들라는 명령어를 보낸다.
        if(pTeamInfo->pMainBlz)
            pTeamInfo->pMainBlz->Command((int)AICOMMAND_MANUFACTURE,&nV);
    }
    
    pthread_mutex_unlock(&mTeamInfoMutex); //언락
}


//0:더이상 만들 탱크가 없다.(인구수 초과)
//other : 만들수있는 객체의 갯수
//arrResult : In:모델아이디 Out:(x:만들수있다(현재 인구), -1:만들수없다)
int CSGLController::OnMainTowerMakeAssistObject(int nTeamID,int nSize,int* arrResult)
{
    int nResult = 0;

#define MAX_ASSIST_CNT 2 //각 종류별로 1개만 만들 수 있다.
    //--------------------------------------------
    pthread_mutex_lock(&mTeamInfoMutex); //락
    map<int,TEAMINFO*>::const_iterator p = mTeamInfo.find(nTeamID);
    if (p == mTeamInfo.end())
    {
        HLogE("[Error] OnMainTowerMakeAssistObject Can not find teamid in teaminfo\n");
        pthread_mutex_unlock(&mTeamInfoMutex); //언락
        return 0;
    }
    
    for (int i = 0; i < nSize; i++)
    {
        TEAMINFO* pTeamInfo = p->second;
        map<int,int>::const_iterator p2 = pTeamInfo->mapCnt.find(arrResult[i]);
        if (p2 == pTeamInfo->mapCnt.end())
        {
            arrResult[i] = 0;
            nResult++;
        }
        else
        {
            //The Model Count
            if(p2->second >= MAX_ASSIST_CNT)
                arrResult[i] = -1;
            else
            {
                arrResult[i] = p2->second;
                nResult++;
            }
        }
    }
    pthread_mutex_unlock(&mTeamInfoMutex); //언락
    //--------------------------------------------
    return nResult;
}



//IAction Event End