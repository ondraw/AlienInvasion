//
//  CMutiplyProtocol.cpp
//  SongGL
//
//  Created by itsme on 2014. 7. 14..
//
//
#include <assert.h>
#include "CHWorld.h"
#include "sGL.h"
#include "CMutiplyProtocol.h"
#include "sGLUtils.h"
#include "CScenario.h"
#include "CSGLCore.h"
#include "CAniRuntimeBubble.h"
#include "CSGLCore.h"

#define BTN_NEXT            1006

extern void* MultiplayProtocolThread(void *data);
extern void* RcvMultiplayProtocolThread(void *data);
extern fSendMultiplayData  gSendMultiplayData;

CMutiplyProtocol::CMutiplyProtocol(IHWorld* pWorld)
{
    mpWorld = (CScenario*)pWorld;
    pthread_mutex_init(&mBufferMutex, NULL);
    pthread_mutex_init(&mRcvBufferMutex, NULL);
    
    pthread_mutex_init(&mRcvMutex, NULL);
    pthread_cond_init(&mRcvSyncCond, NULL);
    
    pthread_mutex_init(&mSndMutex, NULL);
    pthread_cond_init(&mSndSyncCond, NULL);
    
    srand((unsigned int)time(NULL));
    if( rand() % 2 )
    {
        mnTeamID = SGL_TEAM1;
        mnEnemyTeamID = SGL_TEAM4;
    }
    else
    {
        mnTeamID = SGL_TEAM4;
        mnEnemyTeamID = SGL_TEAM1;
    }

    mbStop = true;
    mRcvData = NULL;
    mRcvSize = 0;
    mAssignedSize = 0;
    mThreadMainID = 0;
    mThreadRcvMainID = 0;
    mbDelete = false;
    mbEnemyStartPlaying = false;
    mbMyStartPlaying = false;
    mnNeedCommand = 0;
//    mpSyncObjData = NULL;
    mnFindWaitTime = 0;
    mnMulEnemyMainTowerMaxID = 0;
    mnMulEnemyMainTowerStartIndex = 0;
    
    mbReauestingAliance = false;
    msCntAliance = 0;
    
    mState = SYNC_NONE;
}


CMutiplyProtocol::~CMutiplyProtocol()
{
    int nResult = 0;
    mbDelete = true;
    mbStop = true;
    if(mThreadMainID != 0)
	{
        //실행이벤트를 날려준다.
        pthread_cond_signal(&mSndSyncCond);
		pthread_join(mThreadMainID, (void **) &nResult);
        mThreadMainID = 0;
	}
    if(mThreadRcvMainID != 0)
    {
        //실행이벤트를 날려준다.
        pthread_cond_signal(&mRcvSyncCond);
        pthread_join(mThreadRcvMainID, (void **) &nResult);
        mThreadRcvMainID = 0;
    }
    
    
    pthread_mutex_destroy(&mBufferMutex);
    pthread_mutex_destroy(&mRcvBufferMutex);
    
    pthread_mutex_destroy(&mRcvMutex);
    pthread_cond_destroy(&mRcvSyncCond);
    
    pthread_mutex_destroy(&mSndMutex);
    pthread_cond_destroy(&mSndSyncCond);
    
//    ResetRcvSyncObjAll();
    
    if(mRcvData)
    {
        free(mRcvData);
        mRcvData = NULL;
    }
    ClearData();
    
}

void CMutiplyProtocol::Start()
{
    SetMultiplayState(SYNC_NONE);
    mbStop = false;
    if(mThreadMainID == 0)
    {
        int nThreadResult = pthread_create(&mThreadMainID, 0, (void* (*)(void*))MultiplayProtocolThread, (void*)this);
        if(nThreadResult != 0)
        {
            HLogE("Can not create Thread MultiplayProtocolThread , pthread_create\n");
            return ;
        }
    }
    
    if(mThreadRcvMainID == 0)
    {
        int nThreadResult = pthread_create(&mThreadRcvMainID, 0, (void* (*)(void*))RcvMultiplayProtocolThread, (void*)this);
        if(nThreadResult != 0)
        {
            HLogE("Can not create Thread RcvMultiplayProtocolThread , pthread_create\n");
            return ;
        }
    }
}

void CMutiplyProtocol::Stop()
{
    if(!mbStop)
    {
        mnNeedCommand = 0;
        mbEnemyStartPlaying = false;
        mbMyStartPlaying = false;
        mbStop = true;
        SetMultiplayState(SYNC_NONE);
        ClearData();
    }
    mnFindWaitTime = 0;
    mnMulEnemyMainTowerMaxID = 0;
    mnMulEnemyMainTowerStartIndex = 0;
    msAliasEnemy = "";
    ResetAliance();
    
    sleep(0);
    if(mRcvData)
    {
        free(mRcvData);
        mRcvData = NULL;
        mRcvSize = 0;
        mAssignedSize = 0;
    }

}

void CMutiplyProtocol::ClearData()
{
    mState = SYNC_NONE;
    vector<MutiplyProtocol*>::iterator b,e;
    pthread_mutex_lock(&mBufferMutex); //락
    b = mBuffer.begin();
    e = mBuffer.end();
    for (vector<MutiplyProtocol*>::iterator it = b; it != e; it++)
    {
        delete[] (*it)->pData;
        delete *it;
    }
    mBuffer.clear();
    pthread_mutex_unlock(&mBufferMutex); //언락
    
    
    
    pthread_mutex_lock(&mRcvBufferMutex); //락
    b = mRcvBuffer.begin();
    e = mRcvBuffer.end();
    for (vector<MutiplyProtocol*>::iterator it = b; it != e; it++)
    {
        delete[] (*it)->pData;
        delete *it;
    }
    mRcvBuffer.clear();
    pthread_mutex_unlock(&mRcvBufferMutex); //언락
}

//void CMutiplyProtocol::ResetRcvSyncObjAll()
//{
//    if(mpSyncObjData)
//    {
//        map<int, CArchive*>::iterator b = mpSyncObjData->begin();
//        for (map<int, CArchive*>::iterator it = b; it != mpSyncObjData->end(); it++)
//        {
//            if((*it).second) delete (*it).second;
//        }
//        delete mpSyncObjData;
//        mpSyncObjData = NULL;
//    }
//    
//}

void CMutiplyProtocol::SendSyncNoPlay()
{
    int nDataSize = 0;

    if( IsStop() || mState == SYNC_PLAYING) return;
    
    else if(mState == SYNC_NONE || mState == SYNC_PLYERT_STATE)
    {
        int           nMaxStartIndex;
        unsigned int  nMaxID;
        unsigned long nNow = GetGGTime();
        
        int nSubCnt = 6;
        int nMyLastState = (int)mpWorld->GetCurrentWorld()->GetWorldType();
        if(nMyLastState == WORLD_TYPE_H_WORLD) nSubCnt = 9;
            
        int nMapID = mpWorld->GetUserInfo()->GetLastSelectedMapID();
        int nRank = mpWorld->GetUserInfo()->GetRank();
        int nVersion = PROTOCOL_VERSION;
        
        MutiplyProtocol_Sub* pSub = (MutiplyProtocol_Sub*)new char[sizeof(MutiplyProtocol_Sub) * nSubCnt];
        
        nDataSize += SetMutiplyProtocol_Sub(&pSub[0],SUBCMD_VERSION,sizeof(int),(char*)&nVersion);
        nDataSize += SetMutiplyProtocol_Sub(&pSub[1],SUBCMD_PLYER_STATE,sizeof(int),(char*)&nMyLastState);
        nDataSize += SetMutiplyProtocol_Sub(&pSub[2],SUBCMD_START_CLOCK,sizeof(unsigned long),(char*)&nNow);
        nDataSize += SetMutiplyProtocol_Sub(&pSub[3],SUBCMD_PLYER_NAME,(int)msAlias.length() + 1,(char*)msAlias.c_str());
        nDataSize += SetMutiplyProtocol_Sub(&pSub[4],SUBCMD_PLYER_RANK,sizeof(int),(char*)&nRank);
        nDataSize += SetMutiplyProtocol_Sub(&pSub[5],SUBCMD_SEL_INDEX_MAP,sizeof(int),(char*)&nMapID);
       
        if(nMyLastState == WORLD_TYPE_H_WORLD) //현재 나의 상태가 플레이중에 존재 한다면. 나의 팀정보를 보내준다.
        {
            int nTeamID = 0;
            CHWorld* pWorld = (CHWorld*)mpWorld->GetCurrentWorld();
            CSprite* pActor = pWorld->GetActor();
            if(pActor && pActor->GetState() == SPRITE_RUN)
                nTeamID = pActor->GetTeamID();
            nDataSize += SetMutiplyProtocol_Sub(&pSub[6],SUBCMD_PLYER_TEAM,sizeof(int),(char*)&nTeamID);
            
            
            
            nMaxStartIndex = pWorld->GetCurrentETMainTowerStartPointIndex();
            nDataSize += SetMutiplyProtocol_Sub(&pSub[7],SUBCMD_ENMEY_MAINTOWER_START_INDEX,sizeof(int),(char*)&nMaxStartIndex);
            
            
            nMaxID = CSGLCore::mnEnemyMainTowerMaxID;
            nDataSize += SetMutiplyProtocol_Sub(&pSub[8],SUBCMD_ENMEY_MAINTOWER_MAXID,sizeof(int),(char*)&nMaxID);
        }
        
        SetMutiplyProtocol(new MutiplyProtocol,CMD_START_SYNC,nSubCnt,nDataSize,(char*)pSub);
        
        //평균값을 구하기위해..
        mfMyEscepAv = 0.f;
        mnMyLastTime = nNow;
        mnMyLastTimeEnd = nNow + 1000;
        SetMultiplayState(SYNC_START_CLOCK);
    }
    else if(mState == SYNC_START_CLOCK)
    {
        unsigned long nNow = GetGGTime();
        if(nNow < mnMyLastTimeEnd)
        {
            if(mfMyEscepAv == 0)
                mfMyEscepAv = (nNow - mnMyLastTime);
            else
                mfMyEscepAv = (mfMyEscepAv + (nNow - mnMyLastTime) ) / 2.f;
            mnMyLastTime = nNow;
        }
        else
        {
            SetMutiplyProtocol(new MutiplyProtocol,CMD_CLOCK_SPEED,0,sizeof(float),(char*)&mfMyEscepAv);
            SetMultiplayState(SYNC_READYPLAY);
        }
    }
}



void CMutiplyProtocol::RcvSync()
{
    vector<MutiplyProtocol*> tmpList;
    vector<MutiplyProtocol*>::iterator b,e;
    
    if (IsStop()) return;
    //----------------------------------
    pthread_mutex_lock(&mRcvBufferMutex); //락
    if(mRcvBuffer.size() == 0)
    {
        pthread_mutex_unlock(&mRcvBufferMutex); //언락
        return;
    }
    b = mRcvBuffer.begin();
    e = mRcvBuffer.end();
    for (vector<MutiplyProtocol*>::iterator it = b; it != e; it++)
    {
        tmpList.push_back(*it);
    }
    mRcvBuffer.clear();
    pthread_mutex_unlock(&mRcvBufferMutex); //언락
    //----------------------------------
    
    b = tmpList.begin();
    e = tmpList.end();
    for (vector<MutiplyProtocol*>::iterator it = b; it != e; it++)
    {
        MutiplyProtocol* pro = *it;
        if (IsStop()) return;
        switch (pro->cCommand) {
            case CMD_START_SYNC: //상대방의 플레이 상태 (메인메뉴일때인지, 프로그레스바상태인지, ....)
            {
                int nEnemyLastMapID = 0;
                int nEnemyState = 0;
                int nEnemyTeamID = 0;
                int nVersion = 0;
                
                MutiplyProtocol_Sub* pSub = (MutiplyProtocol_Sub*)pro->pData;
                
                //1> 상대 플래이 상태
                assert(pSub[0].cSubCommand == SUBCMD_VERSION);
                memcpy(&nVersion, pSub[0].pData, pSub[0].nDataSize);
                
                if(nVersion != PROTOCOL_VERSION) //상대방의 버전이 틀리면 거시기 한다.
                {
                    HLogE("버전이 틀리다.");
                    if(gPlayOut) gPlayOut();
                    return;
                }

                
                //1> 상대 플래이 상태
                assert(pSub[1].cSubCommand == SUBCMD_PLYER_STATE);
                memcpy(&nEnemyState, pSub[1].pData, pSub[1].nDataSize);
                
                //2> 상대 시작 클럭
                assert(pSub[2].cSubCommand == SUBCMD_START_CLOCK);
                memcpy(&mnEnemyLastTime, pSub[2].pData, pSub[2].nDataSize);
                
                //3> 상대 Alias
                assert(pSub[3].cSubCommand == SUBCMD_PLYER_NAME);
                msAliasEnemy = pSub[3].pData;
                
                assert(pSub[4].cSubCommand == SUBCMD_PLYER_RANK);
                memcpy(&mnEnemyRank, pSub[4].pData, pSub[4].nDataSize);
                

                assert(pSub[5].cSubCommand == SUBCMD_SEL_INDEX_MAP);
                memcpy(&nEnemyLastMapID, pSub[5].pData, pSub[5].nDataSize);
                
                if(nEnemyState == WORLD_TYPE_H_WORLD)
                {
                    assert(pSub[6].cSubCommand == SUBCMD_PLYER_TEAM);
                    memcpy(&nEnemyTeamID, pSub[6].pData, pSub[6].nDataSize);

                    assert(pSub[7].cSubCommand == SUBCMD_ENMEY_MAINTOWER_START_INDEX);
                    memcpy(&mnMulEnemyMainTowerStartIndex, pSub[7].pData, pSub[7].nDataSize);

                    assert(pSub[8].cSubCommand == SUBCMD_ENMEY_MAINTOWER_MAXID);
                    memcpy(&mnMulEnemyMainTowerMaxID, pSub[8].pData, pSub[8].nDataSize);
                }
                
                
                if(mpWorld->GetCurrentWorld()->GetWorldType() == WORLD_TYPE_MENU && nEnemyState == WORLD_TYPE_MENU)
                {
                    //-----------------------------------------------------
                    //OK Match
                    HLogN("Start Play를 해준다.");
                    //-----------------------------------------------------
                    int nDif = strcmp(msAliasEnemy.c_str(), msAlias.c_str());
                    if(nDif == 0) nDif = (int)(mnEnemyLastTime - mnMyLastTime);
                    if(nDif > 0) //1팀
                    {
                        mnTeamID = SGL_TEAM1; //상대 SGL_TEAM4
                        mnEnemyTeamID = SGL_TEAM4;
                    }
                    else if(nDif < 0)//2팀
                    {
                        mnTeamID = SGL_TEAM4; //상대 SGL_TEAM1
                        mnEnemyTeamID = SGL_TEAM1;
                        //-----------------------------------------------------------------------------
                        //현재 실행 맵을 맞추어서 실행한다.
                        //-----------------------------------------------------------------------------
                        int nEnemyLastMapIndex = mpWorld->GetUserInfo()->GetMIDToIndex(nEnemyLastMapID);
                        mpWorld->GetUserInfo()->SetLastSelectedMapID(nEnemyLastMapID);
                        mpWorld->GetUserInfo()->SetLastSelectedMapIndex(nEnemyLastMapIndex);
                        //-----------------------------------------------------------------------------
                    }
                    else
                    {
                        HLogE("팀을 구분할 수가 없다");
                    }
                    
                    //플레이한다.
                    CScenario::SendMessage(SGL_MESSAGE_CLICKED,BTN_NEXT,0xFF);
                }
                //3. 현재 내가 플레이중에 있다면, 상대방은 메뉴중에 있다면, (O)
                else if(mpWorld->GetCurrentWorld()->GetWorldType() == WORLD_TYPE_H_WORLD && nEnemyState == WORLD_TYPE_MENU)
                {
                    SendSyncStartPlay(); //이미 실행된 상태이기 때문에 START_PLAY를 보내준다.
                }
                //4. 현재 내가 메뉴화면에 있다면, 상대방은 플레이중에 있다면 (서로같은맵으로 통일해준다.)
                else if(mpWorld->GetCurrentWorld()->GetWorldType() ==  WORLD_TYPE_MENU && nEnemyState == WORLD_TYPE_H_WORLD)
                {
                    if(nEnemyTeamID)
                    {
                        if(nEnemyTeamID == SGL_TEAM4)
                            mnTeamID = SGL_TEAM1;
                        else
                            mnTeamID = SGL_TEAM4;
                        mnEnemyTeamID = nEnemyTeamID;
                        
                        
                        int nEnemyLastMapIndex = mpWorld->GetUserInfo()->GetMIDToIndex(nEnemyLastMapID);
                        mpWorld->GetUserInfo()->SetLastSelectedMapID(nEnemyLastMapID);
                        mpWorld->GetUserInfo()->SetLastSelectedMapIndex(nEnemyLastMapIndex);
                        
                        //플레이한다.
                        CScenario::SendMessage(SGL_MESSAGE_CLICKED,BTN_NEXT,0xFF);

                    }
                    else
                    {
                        HLogE("상대팀아이디가 0이다. 멀티플레이를 끊고 다시 검색해야 한다.");
                        //멀티플레이를 끊고 다시 검색해야 한다.
                        if(gFindPlyer) gFindPlyer(20,0,GetFindMatchingGroup());
                    }
                }
                else if(mpWorld->GetCurrentWorld()->GetWorldType() ==  WORLD_TYPE_H_WORLD && nEnemyState == WORLD_TYPE_H_WORLD)
                {

                    //이미 모든 자리는 잡았기 때문에 더이상 할것이 없다.
                    HLogN("WORLD_TYPE_H_WORLD TeamID = %d",mpWorld->GetMutiplyProtocol()->GetMyDeviceActorTeamID());
                    
                    
                    CHWorld* pWorld = (CHWorld*)mpWorld->GetCurrentWorld();
                    if(mnMulEnemyMainTowerStartIndex > pWorld->GetCurrentETMainTowerStartPointIndex())
                    {
                        //$$ 적이 좀더 게임을 진행하였으면, 적메인타워를 동기화 해주어야 한다. 즉 메인타위가 두번째이면 다른 애도 두번째로 실행해야 한다.
                        HLogE("적이 좀더 게임을 진행하였으면, 적메인타워를 동기화 해주어야 한다.");
                    }
                    
                    //서로가 플레이중에 하기때문에 상대방은 플레이중이라고 미리 마킹하자.. 동일한 시간대로 오면 안보내는 경우가 생긴다.
                    //mpWorld->GetMutiplyProtocol()->SetEnemyStartPlaying(true);
                    SendSyncStartPlay(); //이미 실행된 상태이기 때문에 START_PLAY를 보내준다.
                    CScenario::SendMessage(SGL_SHOW_ENEMYTOOLS,1);
                }
                else
                {
                    HLogE("해야할일이 있다");
                    assert(0);
                }
                
                break;
            }
            case CMD_CLOCK_SPEED: //상대방의 시작 클럭
            {
                float fEnemyEscepAv = 0.f;
                assert(sizeof(float) == pro->nDataSize);
                memcpy(&fEnemyEscepAv, pro->pData, pro->nDataSize);
                mfDifSpeed = fEnemyEscepAv - mfMyEscepAv;
                break;
            }
            case CMD_START_PLAY:
            {
                //플레이 관련 메세지를 보낸다.
                mbEnemyStartPlaying = true;
                
                //나도 플레이 중이라면 나의 모든 정보를 보낸다.
                if(mbMyStartPlaying)
                {
                    //CHWORLD의 쓰레드 안쪽에서 나의 탱크 정보를 추출해서 보내준다.
                    SetNeedCommand(GetNeedCommand() | CHWORLD_INTHREAD_MULTIPLAY_NEEDALLTANKDATA);
                }
                break;
            }
            case CMD_SYNC:
            {
                CScenario::SendMessage(SGL_SYNC_MULTIPLAY,0,0,0,(long)pro);
                return ;
            }
            case CMD_OBJ_MOVETO:
            {
                bool bSimpleMode;
                bool bFront;
                CArchive ar(pro->pData,pro->nDataSize);
                int nID;
                SPoint ptTo;
                ar >> nID;
                ar >> bSimpleMode;
                ar >> ptTo;
                ar >> bFront;
                
                if(mpWorld->GetCurrentWorld()->GetWorldType() == WORLD_TYPE_H_WORLD) //Initialize를 할때 현재 월드가 메인메뉴월드이다.
                {
                    CHWorld* pWorld = (CHWorld*)mpWorld->GetCurrentWorld();
                    CSprite* pSprite = pWorld->GetSGLCore()->FindSprite(nID);
                    if(pSprite && pSprite->GetState() == SPRITE_RUN)
                    {
                        SPoint* pT = new SPoint;
                        memcpy(pT, &ptTo, sizeof(SPoint));
                        CScenario::SendMessage(SGL_MOVE_TO_MULTIPLAY,(long)pT,bSimpleMode,bFront,(long)pSprite);
                    }
                }
            }
                break;
            case CMD_OBJ_MOVETO_FIGHTER:
            {  
                if(mpWorld->GetCurrentWorld()->GetWorldType() == WORLD_TYPE_H_WORLD) //Initialize를 할때 현재 월드가 메인메뉴월드이다.
                {
                    int nID;
                    int nToID;
                    memcpy(&nID, pro->pData, sizeof(int));
                    memcpy(&nToID, pro->pData + sizeof(int), sizeof(int));
                    
                    CHWorld* pWorld = (CHWorld*)mpWorld->GetCurrentWorld();
                    CSprite* pSprite = pWorld->GetSGLCore()->FindSprite(nID);
                    if(pSprite && pSprite->GetState() == SPRITE_RUN)
                    {
                        CSprite* pSpriteTo = pWorld->GetSGLCore()->FindSprite(nToID);
                        if(pSpriteTo && pSpriteTo->GetState() == SPRITE_RUN)
                        {
                            CScenario::SendMessage(SGL_MOVE_TO_FIGHTER_MULTIPLAY,(long)pSpriteTo,0,0,(long)pSprite);
                        }
                    }
                }
            }
                break;
            case CMD_CHANGE_BOMB:
            {
                int nID;
                int nBombID;
                memcpy(&nID, pro->pData, sizeof(int));
                memcpy(&nBombID, pro->pData + sizeof(int), sizeof(int));
                CHWorld* pWorld = (CHWorld*)mpWorld->GetCurrentWorld();
                if(mpWorld->GetCurrentWorld()->GetWorldType() == WORLD_TYPE_H_WORLD)
                {
                    CSprite* pSprite = pWorld->GetSGLCore()->FindSprite(nID);
                    if(pSprite && pSprite->GetState() == SPRITE_RUN)
                    {
                        CScenario::SendMessage(SGL_CHANGE_BOM_SEL_MULTIPLAY,nBombID,0,0,(long)pSprite);
                    }
                }
                break;
            }
            case CMD_OBJ_CLICK_BYSTICK:
            {
                if(mpWorld->GetCurrentWorld()->GetWorldType() == WORLD_TYPE_H_WORLD)
                {
                    CHWorld* pWorld = (CHWorld*)mpWorld->GetCurrentWorld();
                    
                    int nOwenerID;
                    SPoint* pT = new SPoint;
                    memcpy(&nOwenerID, pro->pData, sizeof(int));
                    memcpy(pT, pro->pData + sizeof(int), sizeof(SPoint));
                    CSprite* pOwnerSprite = pWorld->GetSGLCore()->FindSprite(nOwenerID);
                    
                    
                    if(pOwnerSprite && pOwnerSprite->GetState() == SPRITE_RUN && pOwnerSprite->IsVisibleRender())
                        CScenario::SendMessage(SGL_CLICK_TO_POINT_MULTIPLAY,(long)pT,0,0,(long)pOwnerSprite);
                }
            }
                break;
            case CMD_OBJ_CLICK_SPRITE_BYSTICK:
            {
                if(mpWorld->GetCurrentWorld()->GetWorldType() == WORLD_TYPE_H_WORLD)
                {
                    CHWorld* pWorld = (CHWorld*)mpWorld->GetCurrentWorld();
                    int nOwenerID;
                    int nID;
                    int nSizeInt = sizeof(int);
                    memcpy(&nOwenerID, pro->pData, nSizeInt);
                    memcpy(&nID, pro->pData + nSizeInt, nSizeInt);
                    CSprite* pOwnerSprite = pWorld->GetSGLCore()->FindSprite(nOwenerID);
                    CSprite* pSprite = pWorld->GetSGLCore()->FindSprite(nID);
                    if(pSprite && pSprite->GetState() == SPRITE_RUN && pOwnerSprite && pOwnerSprite->GetState() == SPRITE_RUN && pOwnerSprite->IsVisibleRender())
                        CScenario::SendMessage(SGL_CLICK_TO_SPRITE_MULTIPLAY,(long)pSprite,0,0,(long)pOwnerSprite);
                }
            }
                break;
            case CMD_OBJ_CLICKED_NEW_BOMB:
                if(mpWorld->GetCurrentWorld()->GetWorldType() == WORLD_TYPE_H_WORLD)
                {
                    int nOwnerID;
                    CHWorld* pWorld = (CHWorld*)mpWorld->GetCurrentWorld();
                    memcpy(&nOwnerID, pro->pData, sizeof(int));
                    CSprite* pOwnerSprite = pWorld->GetSGLCore()->FindSprite(nOwnerID);
                    if(pOwnerSprite && pOwnerSprite->GetState() == SPRITE_RUN && !pOwnerSprite->IsVisibleRender())
                    {
                        CScenario::SendMessage(SGL_CLICKED_NEW_BOMB_MULTIPLAY,(long)pro,0,0,(long)pOwnerSprite);
                        return; //내부에서 pro를 사용하기 때문에 메모리는 나중에 지운다.
                    }
                }
                break;
            case CMD_COMPLETED_RUNTIMEUPGRADE:
                if(mpWorld->GetCurrentWorld()->GetWorldType() == WORLD_TYPE_H_WORLD)
                {
                    int nRuntimeItemID;
                    int nOwnerID;
                    CHWorld* pWorld = (CHWorld*)mpWorld->GetCurrentWorld();
                    MutiplyProtocol_Sub* pSub = (MutiplyProtocol_Sub*)pro->pData;
                    for (int i = 0; i < pro->cSubCnt; i++)
                    {
                        CArchive* pNewArc = new CArchive(pSub[i].pData,pSub[i].nDataSize);
                        *pNewArc >> nOwnerID;
                        *pNewArc >> nRuntimeItemID;
                        
                        
                        pNewArc->Begin(); //포지션을 0으로 다시조정해준다.
                        CSprite* pOwnerSprite = pWorld->GetSGLCore()->FindSprite(nOwnerID);
                        bool bIs = false;
                        if(pOwnerSprite && pOwnerSprite->GetState() == SPRITE_RUN)
                        {
                            vector<USERINFO*>* pList = pOwnerSprite->GetRuntimeUpgradList();
                            
                            for (vector<USERINFO*>::iterator it = pList->begin(); it != pList->end();it++)
                            {
                                RUNTIME_UPGRADE* prop = (RUNTIME_UPGRADE*)(*it)->pProp;
                                if(prop->nID == nRuntimeItemID)
                                {
                                    CScenario::SendMessage(SGL_COMPLETED_RUNTIME_UPGRADE_MULTIPLAY,(long)pOwnerSprite,(long)pNewArc,NULL,(long)prop);
                                    bIs = true;
                                    break;
                                }
                            }
                        }
                        
                        if(bIs == false)
                            delete pNewArc;
                    }
                }
                break;
            case CMD_STOP_MOVE:
            {
                int nOwnerID;
                if(mpWorld->GetCurrentWorld()->GetWorldType() == WORLD_TYPE_H_WORLD)
                {
                    CHWorld* pWorld = (CHWorld*)mpWorld->GetCurrentWorld();
                    memcpy(&nOwnerID, pro->pData, sizeof(int));
                    CSprite* pOwnerSprite = pWorld->GetSGLCore()->FindSprite(nOwnerID);
                    if(pOwnerSprite && pOwnerSprite->GetState() == SPRITE_RUN)
                    {
                        SGLEvent Event;
                        Event.lParam = 0;
                        Event.lParam2 = 0;
                        Event.lObject = (long)pOwnerSprite;
                        Event.nMsgID = GSL_MOVE_STOP|GSL_TRUN_STOP;
                        pWorld->OnEvent(&Event);
                    }
                }
                break;
            }
            case CMD_MAKE_SPRITE:
            {
                if(mpWorld->GetCurrentWorld()->GetWorldType() == WORLD_TYPE_H_WORLD)
                {
                    CArchive *pArc = new CArchive(pro->pData, pro->nDataSize);
                    CScenario::SendMessage(SGL_MAKE_SPRITE_MULTIPLAY,0,0,0,(long)pArc);
                }
                break;
            }
            case CMD_MOVEIN_TANK:
            {
                if(mpWorld->GetCurrentWorld()->GetWorldType() == WORLD_TYPE_H_WORLD)
                {
                    int nIndex;
                    int nOwnerID;
                    int nEnID;
                    float fDistance;
                    int nSizeInt = sizeof(int);
                    CHWorld* pWorld = (CHWorld*)mpWorld->GetCurrentWorld();
                    
                    memcpy(&nOwnerID, pro->pData, nSizeInt);
                    memcpy(&nIndex, pro->pData + nSizeInt, nSizeInt);
                    memcpy(&nEnID, pro->pData + nSizeInt * 2, nSizeInt);
                    memcpy(&fDistance, pro->pData + nSizeInt * 3, sizeof(float));
                    
                    CSprite* pOwnerSprite = pWorld->GetSGLCore()->FindSprite(nOwnerID);
                    if(pOwnerSprite && pOwnerSprite->GetState() == SPRITE_RUN)
                    {
                        CSprite* pEnSprite = pWorld->GetSGLCore()->FindSprite(nEnID);
                        if(pEnSprite && pEnSprite->GetState() == SPRITE_RUN)
                        {
                            pOwnerSprite->SetAttackTo_Multiplay(nIndex,pEnSprite, fDistance);
                        }
                    }
                }
                break;
            }
            case CMD_MOVEIN_TANK_BYGUN:
            {
                if(mpWorld->GetCurrentWorld()->GetWorldType() == WORLD_TYPE_H_WORLD)
                {
                    int nOwnerID;
                    int nEnID;
                    int nSizeInt = sizeof(int);
                    CHWorld* pWorld = (CHWorld*)mpWorld->GetCurrentWorld();
                    
                    memcpy(&nOwnerID, pro->pData, nSizeInt);
                    memcpy(&nEnID, pro->pData + nSizeInt, nSizeInt);
                    
                    CSprite* pOwnerSprite = pWorld->GetSGLCore()->FindSprite(nOwnerID);
                    if(pOwnerSprite && pOwnerSprite->GetState() == SPRITE_RUN)
                    {
                        CSprite* pEnSprite = pWorld->GetSGLCore()->FindSprite(nEnID);
                        if(pEnSprite && pEnSprite->GetState() == SPRITE_RUN)
                        {
                            pOwnerSprite->SetAttackToByGun_Multiplay(pEnSprite);
                        }
                    }
                }
                break;
            }
            case CMD_DELETE_OBJECT:
                if(mpWorld->GetCurrentWorld()->GetWorldType() == WORLD_TYPE_H_WORLD)
                {
                    int nID;
                    int nWhosKilledID;
                    bool bVisible;
                    int nSizeInt = sizeof(int);
                    CHWorld* pWorld = (CHWorld*)mpWorld->GetCurrentWorld();
                    
                    memcpy(&nID, pro->pData, nSizeInt);
                    memcpy(&nWhosKilledID, pro->pData + nSizeInt, nSizeInt);
                    memcpy(&bVisible, pro->pData + nSizeInt * 2, sizeof(bool));
                    
                    CSprite* pSprite = pWorld->GetSGLCore()->FindSprite(nID);
                    if(pSprite && pSprite->GetState() == SPRITE_RUN)
                    {
                        pSprite->SetState(BOMB_COMPACT);
                        pWorld->KilledActoerToMission(pSprite, true);
                    }
                }
                break;
            case CMD_FIGHTER_SPRITE_SYNC:
                if(mpWorld->GetCurrentWorld()->GetWorldType() == WORLD_TYPE_H_WORLD)
                {
                    int nID;
                    CHWorld* pWorld = (CHWorld*)mpWorld->GetCurrentWorld();
                    CArchive ar(pro->pData, pro->nDataSize);
                    ar >> nID;
                    ar.Begin();
                    CSprite* pSprite = pWorld->GetSGLCore()->FindSprite(nID);
                    if(pSprite && pSprite->GetState() == SPRITE_RUN)// && !pSprite->IsVisibleRender())
                        pSprite->ArchiveMultiplay(SUBCMD_OBJ_ALL_INFO, ar, false);
                }

                break;
            case CMD_BUBBLE_SYNC:
            {
                if(mpWorld->GetCurrentWorld()->GetWorldType() == WORLD_TYPE_H_WORLD)
                {

                    CSprite* pSprite;
                    CHWorld* pWorld = (CHWorld*)mpWorld->GetCurrentWorld();
                    int nID,nInterval,nUpgradeCount;
                    float fScale;
                    float* fColor;
                    CArchive ar(pro->pData, pro->nDataSize);
                    ar >> nID;
                    pSprite = pWorld->GetSGLCore()->FindSprite(nID);
                    if(pSprite && pSprite->GetState() == SPRITE_RUN)
                    {
                        ar >> nInterval;
                        ar >> nUpgradeCount; //UpgradeCount
                        ar >> fScale;
                        CAniRuntimeBubble* pBubble = pSprite->GetAniRuntimeBubble();
                        if(pBubble && pBubble->GetState() == SPRITE_RUN)
                        {
                            fColor = pBubble->GetColor();
                            ar >> fColor[0];
                            ar >> fColor[1];
                            ar >> fColor[2];
                            
                            if(nInterval)
                                pBubble->ResetBubble(nInterval);
                            pBubble->SetUpgradeCount(nUpgradeCount);
                        }
                        else
                        {
                            SPoint ptNow;
                            pSprite->GetPosition(&ptNow);
                            pBubble = new CAniRuntimeBubble(pWorld,pSprite,nInterval,nUpgradeCount);
                            pBubble->Initialize(&ptNow, NULL);
                            pBubble->SetScale(fScale, fScale, fScale);
                            
                            fColor = pBubble->GetColor();
                            ar >> fColor[0];
                            ar >> fColor[1];
                            ar >> fColor[2];
                            pBubble->SetColor(fColor[0], fColor[1], fColor[2]);
                            
                            pSprite->SetAniRuntimeBubble(pBubble);
                            CSGLCore *pCore = pWorld->GetSGLCore();
                            pCore->AddParticle(pBubble);
                        }
                    }
                }
            }
                break;
            case CMD_CURRENT_ENERGY_SYNC:
            {
                if(mpWorld->GetCurrentWorld()->GetWorldType() == WORLD_TYPE_H_WORLD)
                {

                    int nID;
                    float fCurrentEn;
                    int nSizeInt = sizeof(int);
                    CHWorld* pWorld = (CHWorld*)mpWorld->GetCurrentWorld();
                    
                    memcpy(&nID, pro->pData, nSizeInt);
                    memcpy(&fCurrentEn, pro->pData + nSizeInt, sizeof(float));

                    CSprite* pSprite = pWorld->GetSGLCore()->FindSprite(nID);
                    if(pSprite && pSprite->GetState() == SPRITE_RUN)
                        pSprite->SetCurrentDefendPower(fCurrentEn);
                }
            }
                break;
            case CMD_REQUEST_ALIANCE:
            {
                if(mpWorld->GetCurrentWorld()->GetWorldType() == WORLD_TYPE_H_WORLD)
                {
                    bool bAliance;
                    int nSizeBool = sizeof(bool);
                    memcpy(&bAliance, pro->pData, nSizeBool);
                    CScenario::SendMessage(SGL_ALIANCE_MULTIPLAY,bAliance,0,0,0);
                }
            }
                break;
            case CMD_RESPONSE_ALIANCE:
            {
                if(mpWorld->GetCurrentWorld()->GetWorldType() == WORLD_TYPE_H_WORLD)
                {
                    bool bYes;
                    int nSizeBool = sizeof(bool);
                    memcpy(&bYes, pro->pData, nSizeBool);
                    CScenario::SendMessage(SGL_ALIANCE_MULTIPLAY,bYes,1,1,1);
                    mbReauestingAliance = false; //동맹에 응답을 하였다.
                    
                    
                }
            }
                break;

            default:
                break;
        }
        delete [] pro->pData;
        delete pro;
    }
}



int CMutiplyProtocol::SetMutiplyProtocol_Sub(MutiplyProtocol_Sub* pSub,unsigned char cCmd,int nDataSize,char* pData)
{
    int nRead = 0;
    pSub->cSubCommand = cCmd;
    nRead += sizeof(unsigned char); //1
    pSub->nDataSize = nDataSize;
    nRead += sizeof(unsigned short); //2
    pSub->pData = new char[nDataSize];
    memcpy(pSub->pData, pData, nDataSize);
    nRead += nDataSize;           //sizeof(int)
    return nRead;
}

void CMutiplyProtocol::SetMutiplyProtocol(MutiplyProtocol* pPro,unsigned char cCmd,int nSubCnt,int nDataSize,char* pData) //,bool bReliable) UDP를 사용하지 말자. 받을때 로직이 잘못되었다.
{
    pPro->cCommand = cCmd;
    pPro->cSubCnt = nSubCnt;
    pPro->nDataSize = nDataSize;
    if(nSubCnt == 0)
    {
        pPro->pData = new char[nDataSize];
        memcpy(pPro->pData, pData, nDataSize);
    }
    else
        pPro->pData = pData;
//    pPro->bReliable = bReliable;
    pPro->bReliable = true;
    PushData(pPro);
    SndSyncEvent();
}

void CMutiplyProtocol::SetRcvMutiplyProtocol(MutiplyProtocol* pro,unsigned char cCmd,int nSubSize,int nDataSize,char* pData)
{
    pro->cCommand = cCmd;
    pro->cSubCnt = nSubSize;
    pro->nDataSize = nDataSize;
//-------------------------------------------------------------------------------------------------
#ifdef DEBUG
    if(pro->cCommand == CMD_START_SYNC) HLogN("Rcv Cmd = %s","CMD_START_SYNC");
    else if(pro->cCommand == CMD_CLOCK_SPEED) HLogN("Rcv Cmd = %s","CMD_CLOCK_SPEED");
    else if(pro->cCommand == CMD_START_PLAY) HLogN("Rcv Cmd = %s","CMD_START_PLAY");
    else if(pro->cCommand == CMD_SYNC) HLogN("Rcv Cmd = %s","CMD_SYNC");
    else if(pro->cCommand == CMD_OBJ_MOVETO) HLogN("Rcv Cmd = %s","CMD_OBJ_MOVETO");
    else if(pro->cCommand == CMD_OBJ_CLICK_BYSTICK) HLogN("Rcv Cmd = %s","CMD_OBJ_CLICK_BYSTICK");
    else if(pro->cCommand == CMD_OBJ_CLICK_SPRITE_BYSTICK) HLogN("Rcv Cmd = %s","CMD_OBJ_CLICK_SPRITE_BYSTICK");
    else if(pro->cCommand == CMD_OBJ_CLICKED_NEW_BOMB) HLogN("Rcv Cmd = %s","CMD_OBJ_CLICKED_NEW_BOMB");
    else if(pro->cCommand == CMD_CHANGE_BOMB) HLogN("Rcv Cmd = %s","CMD_CHANGE_BOMB");
    else if(pro->cCommand == CMD_COMPLETED_RUNTIMEUPGRADE) HLogN("Rcv Cmd = %s","CMD_COMPLETED_RUNTIMEUPGRADE");
    else if(pro->cCommand == CMD_STOP_MOVE) HLogN("Rcv Cmd = %s","CMD_STOP_MOVE");
    else if(pro->cCommand == CMD_MAKE_SPRITE) HLogN("Rcv Cmd = %s","CMD_MAKE_SPRITE");
    else if(pro->cCommand == CMD_MOVEIN_TANK) HLogN("Rcv Cmd = %s","CMD_MOVEIN_TANK");
    else if(pro->cCommand == CMD_MOVEIN_TANK_BYGUN) HLogN("Rcv Cmd = %s","CMD_MOVEIN_TANK_BYGUN");
    else if(pro->cCommand == CMD_DELETE_OBJECT) HLogN("Rcv Cmd = %s","CMD_DELETE_OBJECT");
    else if(pro->cCommand == CMD_OBJ_MOVETO_FIGHTER) HLogN("Rcv Cmd = %s","CMD_OBJ_MOVETO_FIGHTER");
    else if(pro->cCommand == CMD_FIGHTER_SPRITE_SYNC) HLogN("Rcv Cmd = %s","CMD_FIGHTER_SPRITE_SYNC");
    else if(pro->cCommand == CMD_BUBBLE_SYNC) HLogN("Rcv Cmd = %s","CMD_BUBBLE_SYNC");
    else if(pro->cCommand == CMD_CURRENT_ENERGY_SYNC) HLogN("Rcv Cmd = %s","CMD_CURRENT_ENERGY_SYNC");
    
    
    
#endif
//-------------------------------------------------------------------------------------------------
    if(nSubSize == 0)
    {
        pro->pData = new char[nDataSize];
        memcpy(pro->pData, pData, nDataSize);
    }
    else //써브가 존재하면
    {
        const unsigned int nConstCmdSize = sizeof(unsigned char);
        const unsigned int nConstDataSize = sizeof(unsigned short);
        int nRead = 0;
        unsigned char cSubCommand;
        unsigned short nDataSize;
        
        MutiplyProtocol_Sub* pSub = (MutiplyProtocol_Sub*)new char[sizeof(MutiplyProtocol_Sub) * nSubSize];
        for (int i = 0; i < nSubSize; i++)
        {
            memcpy(&cSubCommand, pData + nRead, nConstCmdSize);
            nRead += nConstCmdSize;
            memcpy(&nDataSize, pData + nRead, nConstDataSize);
            nRead += nConstDataSize;
            SetRcvMutiplyProtocol_Sub(&pSub[i],cSubCommand,nDataSize,pData + nRead);
            nRead += nDataSize;
        }
        pro->pData = (char*)pSub;
    }
    
    PushRcvData(pro);
}

void CMutiplyProtocol::SetRcvMutiplyProtocol_Sub(MutiplyProtocol_Sub* pSub,unsigned char cCmd,int nDataSize,char* pData)
{
    pSub->cSubCommand = cCmd;
    pSub->nDataSize = nDataSize;
    pSub->pData = new char[nDataSize];
    memcpy(pSub->pData, pData, nDataSize);
    
//-------------------------------------------------------------------------------------------------
#ifdef DEBUG
    if(pSub->cSubCommand == SUBCMD_PLYER_STATE)         HLogN("Rcv SubCmd = %s","SUBCMD_PLYER_STATE");
    else if(pSub->cSubCommand == SUBCMD_PLYER_NAME)     HLogN("Rcv SubCmd = %s","SUBCMD_PLYER_NAME");
    else if(pSub->cSubCommand == SUBCMD_START_CLOCK)    HLogN("Rcv SubCmd = %s","SUBCMD_START_CLOCK");
    else if(pSub->cSubCommand == SUBCMD_SEL_INDEX_MAP)  HLogN("Rcv SubCmd = %s","SUBCMD_SEL_INDEX_MAP");
    else if(pSub->cSubCommand == SUBCMD_OBJ_ALL_INFO)   HLogN("Rcv SubCmd = %s","SUBCMD_OBJ_ALL_INFO");
#endif
//-------------------------------------------------------------------------------------------------
}




void CMutiplyProtocol::PushData(MutiplyProtocol* pNew)
{
    static char StartP[] = { '!','@','#','$'};
    pthread_mutex_lock(&mBufferMutex); //락
    memcpy(pNew->sStart,StartP,sizeof(StartP));
    mBuffer.push_back(pNew);
    pthread_mutex_unlock(&mBufferMutex); //언락
    
}

void CMutiplyProtocol::PushRcvData(MutiplyProtocol* pNew)
{
    pthread_mutex_lock(&mRcvBufferMutex); //락
    mRcvBuffer.push_back(pNew);
    pthread_mutex_unlock(&mRcvBufferMutex); //언락
}


void CMutiplyProtocol::ReceiveData(const char* sPlayerID,int nSize,char* sData)
{
    const unsigned int nConstMarkSize = 4;
    const unsigned int nConstCmdSize = sizeof(unsigned char);
    const unsigned int nConstSubCntSize = sizeof(unsigned char);
    const unsigned int nConstDataSize = sizeof(unsigned short);
    
    if(IsStop()) return;
    
    if(mRcvData == NULL)
    {
        mAssignedSize = nSize + 200;
        mRcvData = (char*)malloc(mAssignedSize);
    }
    else
    {
        if(mRcvSize + nSize >= mAssignedSize)
        {
            mAssignedSize = mAssignedSize + nSize + 200;
            mRcvData = (char*)realloc(mRcvData,mAssignedSize);
        }
    }
    
    memcpy(mRcvData + mRcvSize, sData,nSize);
    mRcvSize += nSize;
    
    
    int nTotalRead = 0;
    unsigned char cCommand;
    unsigned char cSubCnt;
    unsigned short nDataSize;
    do
    {
        if(mbStop) break;
        
        if((mRcvSize - nTotalRead) < (nConstMarkSize + nConstCmdSize + nConstSubCntSize + nConstDataSize)) //기본적인 정보보다 커야 파싱할수가 있다.
            break;
        
        //1 시작 비트를 체크한다.
        if(!(*(mRcvData + nTotalRead) == '!' && *(mRcvData + nTotalRead + 1) == '@'))
        {
            //------------------------------------------------------------
            //UDP일 경우는 전혀 엉뚱한 패킷이 넘어왔다면..... 시작 비트를 찾는다.
            //------------------------------------------------------------
            HLogN("받은 데이터가 인벨리드한 데이터 이다. 받은데이터를 모두 지우자.");
            return;
        }
        
        
        memcpy(&nDataSize,mRcvData + (nTotalRead + nConstMarkSize + nConstCmdSize + nConstSubCntSize),nConstDataSize);
        if( (nDataSize + nTotalRead + nConstMarkSize + nConstCmdSize + nConstSubCntSize + nConstDataSize) > mRcvSize) //현재 받은 데이터 보다 많이 읽어야 한다면 데이터를 더 받아와야 한다.
        {
            HLogN("좀더 많이 패킷을 읽어 와야 한다.");
            break; //좀더 읽어와야 한다.
        }
        
        nTotalRead += nConstMarkSize;//위에서 체크를 했기 때문에, 해더를 읽었다 치자.
        cCommand = *(mRcvData + nTotalRead);
        nTotalRead += nConstCmdSize;
        memcpy(&cSubCnt, mRcvData + nTotalRead, nConstSubCntSize);
        nTotalRead += nConstSubCntSize;
        nTotalRead += nConstDataSize;//위에서 데이터 사이즈를 받았기 때문에 그냥 자릿수만 더해주자.
        SetRcvMutiplyProtocol(new MutiplyProtocol,cCommand,cSubCnt,nDataSize, mRcvData + nTotalRead);
        nTotalRead += nDataSize;
    }
    while (true);
    
    if(nTotalRead)
    {
        //남은 데이터는 처음으로 옮겨준다.
        int nNewSize = mRcvSize - nTotalRead; //데이터가 남은 사이즈.
        if(nNewSize > 0)
        {
            memcpy(mRcvData,mRcvData + nTotalRead,nNewSize);
            mRcvSize = nNewSize;
        }
        else
        {
            free(mRcvData);
            mRcvData = NULL;
            mRcvSize = 0;
            mAssignedSize = 0;
        }
    }
    
    RcvSyncEvent(); //데이터가 존재하면 이벤트를 날려준다.
}

void CMutiplyProtocol::RcvSyncEvent()
{
    //-----------------------------------------------------
    pthread_mutex_lock(&mRcvBufferMutex);
    if(mRcvBuffer.size() > 0)
    {
        //RcvMultiplayProtocolThread 실행이벤트를 날려준다.
        pthread_cond_signal(&mRcvSyncCond);
    }
    pthread_mutex_unlock(&mRcvBufferMutex);
    //-----------------------------------------------------
}


void CMutiplyProtocol::SndSyncEvent()
{
    //-----------------------------------------------------
    pthread_mutex_lock(&mBufferMutex);
    if(mBuffer.size() > 0)
    {
        //MultiplayProtocolThread 실행이벤트를 날려준다.
        pthread_cond_signal(&mSndSyncCond);
    }
    pthread_mutex_unlock(&mBufferMutex);
    //-----------------------------------------------------
}


void CMutiplyProtocol::SendData()
{
    int nReadSize = 0;
    int nReadUnreliableSize = 0;
    
    char* pBuffer = NULL;
    char* pUnreliableBuffer = NULL;
    
    unsigned int nSize = 0;
    unsigned int nUnreliableSize = 0;
    const unsigned int nConstMarkSize = 4;
    const unsigned int nConstCmdSize = sizeof(unsigned char);
    const unsigned int nConstSubCntSize = sizeof(unsigned char);
    const unsigned int nConstDataSize = sizeof(unsigned short);
    
    vector<MutiplyProtocol*>::iterator b,e;
    if(IsStop()) return;
    
    pthread_mutex_lock(&mBufferMutex); //락
    
    if(mBuffer.size() == 0)
    {
        pthread_mutex_unlock(&mBufferMutex); //언락
        return;
    }
    
    b = mBuffer.begin();
    e = mBuffer.end();
    for (vector<MutiplyProtocol*>::iterator it = b; it != e; it++)
    {
        if((*it)->bReliable)
            nSize += (nConstMarkSize + nConstCmdSize + nConstSubCntSize + nConstDataSize + (*it)->nDataSize);
        else
            nUnreliableSize += (nConstMarkSize + nConstCmdSize + nConstSubCntSize + nConstDataSize + (*it)->nDataSize);
            
    }
    
    if(nSize > 0)
        pBuffer = new char[nSize];
    if(nUnreliableSize > 0)
        pUnreliableBuffer = new char[nUnreliableSize];
    
    for (vector<MutiplyProtocol*>::iterator it = b; it != e; it++)
    {
        MutiplyProtocol* pro = *it;

        if(pro->bReliable)
        {
            memcpy(pBuffer + nReadSize, pro->sStart, nConstMarkSize);
            nReadSize += nConstMarkSize;
            
            memcpy(pBuffer + nReadSize, &pro->cCommand, nConstCmdSize);
            nReadSize += nConstCmdSize;
            
            memcpy(pBuffer + nReadSize, &pro->cSubCnt, nConstSubCntSize);
            nReadSize += nConstSubCntSize;
            
            memcpy(pBuffer + nReadSize, &pro->nDataSize, nConstDataSize);
            nReadSize += nConstDataSize;
            
            
            if(pro->cSubCnt > 0)
            {
                MutiplyProtocol_Sub* prosub = (MutiplyProtocol_Sub*)pro->pData;
                for (int i = 0; i < pro->cSubCnt; i++)
                {
                    memcpy(pBuffer + nReadSize, &prosub[i].cSubCommand, nConstCmdSize);
                    nReadSize += nConstCmdSize;
                    memcpy(pBuffer + nReadSize, &prosub[i].nDataSize, nConstDataSize);
                    nReadSize += nConstDataSize;
                    memcpy(pBuffer + nReadSize, prosub[i].pData, prosub[i].nDataSize);
                    nReadSize += prosub[i].nDataSize;
                }
            }
            else
            {
                memcpy(pBuffer + nReadSize, pro->pData, pro->nDataSize);
                nReadSize += pro->nDataSize;
            }
        }
        else
        {
            memcpy(pUnreliableBuffer + nReadUnreliableSize, pro->sStart, nConstMarkSize);
            nReadUnreliableSize += nConstMarkSize;
            
            memcpy(pUnreliableBuffer + nReadUnreliableSize, &pro->cCommand, nConstCmdSize);
            nReadUnreliableSize += nConstCmdSize;
            
            memcpy(pUnreliableBuffer + nReadUnreliableSize, &pro->cSubCnt, nConstSubCntSize);
            nReadUnreliableSize += nConstSubCntSize;
            
            memcpy(pUnreliableBuffer + nReadUnreliableSize, &pro->nDataSize, nConstDataSize);
            nReadUnreliableSize += nConstDataSize;
            
            
            if(pro->cSubCnt > 0)
            {
                MutiplyProtocol_Sub* prosub = (MutiplyProtocol_Sub*)pro->pData;
                for (int i = 0; i < pro->cSubCnt; i++)
                {
                    memcpy(pUnreliableBuffer + nReadUnreliableSize, &prosub[i].cSubCommand, nConstCmdSize);
                    nReadUnreliableSize += nConstCmdSize;
                    memcpy(pUnreliableBuffer + nReadUnreliableSize, &prosub[i].nDataSize, nConstDataSize);
                    nReadUnreliableSize += nConstDataSize;
                    memcpy(pUnreliableBuffer + nReadUnreliableSize, prosub[i].pData, prosub[i].nDataSize);
                    nReadUnreliableSize += prosub[i].nDataSize;
                }
            }
            else
            {
                memcpy(pUnreliableBuffer + nReadUnreliableSize, pro->pData, pro->nDataSize);
                nReadUnreliableSize += pro->nDataSize;
            }

        }
        
        
//-------------------------------------------------------------------------------------------------
#ifdef DEBUG
        if(pro->cCommand == CMD_START_SYNC) HLogN("Send Cmd = %s","CMD_START_SYNC");
        else if(pro->cCommand == CMD_CLOCK_SPEED) HLogN("Send Cmd = %s","CMD_CLOCK_SPEED");
        else if(pro->cCommand == CMD_START_PLAY) HLogN("Send Cmd = %s","CMD_START_PLAY");
        else if(pro->cCommand == CMD_SYNC) HLogN("Send Cmd = %s","CMD_SYNC");
        else if(pro->cCommand == CMD_OBJ_MOVETO) HLogN("Send Cmd = %s","CMD_OBJ_MOVETO");
        else if(pro->cCommand == CMD_OBJ_CLICK_BYSTICK) HLogN("Send Cmd = %s","CMD_OBJ_CLICK_BYSTICK");
        else if(pro->cCommand == CMD_OBJ_CLICK_SPRITE_BYSTICK) HLogN("Send Cmd = %s","CMD_OBJ_CLICK_SPRITE_BYSTICK");
        else if(pro->cCommand == CMD_OBJ_CLICKED_NEW_BOMB) HLogN("Send Cmd = %s","CMD_OBJ_CLICKED_NEW_BOMB");
        else if(pro->cCommand == CMD_CHANGE_BOMB) HLogN("Send Cmd = %s","CMD_CHANGE_BOMB");
        else if(pro->cCommand == CMD_COMPLETED_RUNTIMEUPGRADE) HLogN("Send Cmd = %s","CMD_COMPLETED_RUNTIMEUPGRADE");
        else if(pro->cCommand == CMD_STOP_MOVE) HLogN("Send Cmd = %s","CMD_STOP_MOVE");
        else if(pro->cCommand == CMD_MAKE_SPRITE) HLogN("Send Cmd = %s","CMD_MAKE_SPRITE");
        else if(pro->cCommand == CMD_MOVEIN_TANK) HLogN("Send Cmd = %s","CMD_MOVEIN_TANK");
        else if(pro->cCommand == CMD_MOVEIN_TANK_BYGUN) HLogN("Send Cmd = %s","CMD_MOVEIN_TANK_BYGUN");
        else if(pro->cCommand == CMD_DELETE_OBJECT) HLogN("Send Cmd = %s","CMD_DELETE_OBJECT");
        else if(pro->cCommand == CMD_OBJ_MOVETO_FIGHTER) HLogN("Send Cmd = %s","CMD_OBJ_MOVETO_FIGHTER");
        else if(pro->cCommand == CMD_FIGHTER_SPRITE_SYNC) HLogN("Send Cmd = %s","CMD_FIGHTER_SPRITE_SYNC");
        else if(pro->cCommand == CMD_BUBBLE_SYNC) HLogN("Send Cmd = %s","CMD_BUBBLE_SYNC");
        else if(pro->cCommand == CMD_CURRENT_ENERGY_SYNC) HLogN("Send Cmd = %s","CMD_CURRENT_ENERGY_SYNC");
#endif
//-------------------------------------------------------------------------------------------------
        
        if(pro->pData) delete[] pro->pData;
        delete pro;
    }
    mBuffer.clear();
    pthread_mutex_unlock(&mBufferMutex); //언락
    
    if(gSendMultiplayData)
    {
        if(pBuffer)
            gSendMultiplayData((void*)NULL,nSize,pBuffer,true);
        if(pUnreliableBuffer)
            gSendMultiplayData((void*)NULL,nSize,pUnreliableBuffer,false);
    }
//-----------------------------------------------------------------
}


void* MultiplayProtocolThread(void *data)
{
    int rc = 0;
    struct timeval now;
    struct timespec ts;
//    gettimeofday(&now, NULL);
//    ts.tv_sec = now.tv_sec + 5;
//    ts.tv_nsec = now.tv_usec * 1000;
//    unsigned long xnanosec = 0;
    
    static long lResult = 0;
    CMutiplyProtocol* pMan = (CMutiplyProtocol*)data;
    while (!pMan->IsDelete())
    {
        pthread_mutex_lock(&pMan->mSndMutex);
        gettimeofday(&now, NULL);
        ts.tv_sec = now.tv_sec;
        ts.tv_nsec = now.tv_usec * 1000;
        
        
        //0.5초 단위로
        if(ts.tv_nsec >= 500000000)
        {
            ts.tv_sec += 1;
            ts.tv_nsec-= 500000000;
        }
        else
            ts.tv_nsec+= 500000000;
        
        rc = pthread_cond_timedwait(&pMan->mSndSyncCond, &pMan->mSndMutex, &ts);
        if (rc == 0) //TimeOut
        {
        }
        if(pMan->IsStop() == false)
            pMan->SendData();
//        usleep(500000); //0.5초 마이크로초 단위
//        HLogN("xxxxxxx");
        pthread_mutex_unlock(&pMan->mSndMutex);
    }
    return (void*)&lResult;
}


void* RcvMultiplayProtocolThread(void *data)
{
    static long lResult = 0;
    CMutiplyProtocol* pMan = (CMutiplyProtocol*)data;
    while (!pMan->IsDelete())
    {
        pthread_mutex_lock(&pMan->mRcvMutex);
        pthread_cond_wait(&pMan->mRcvSyncCond, &pMan->mRcvMutex);
        if(pMan->IsStop() == false)
        {
            pMan->RcvSync();
        }
        pthread_mutex_unlock(&pMan->mRcvMutex);
    }
    return (void*)&lResult;
}


void CMutiplyProtocol::SendSyncStartPlay()
{
    int nT = 0;
    if(IsStop()) return; //보내지 말자.
    SetMutiplyProtocol(new MutiplyProtocol,CMD_START_PLAY,0,sizeof(int),(char*)&nT);
    mbMyStartPlaying = true;
    if(mbEnemyStartPlaying) //상대방이 플레이중이라면 나의 모든 정보를 보낸다.
    {
        //CHWORLD의 쓰레드 안쪽에서 나의 탱크 정보를 추출해서 보내준다.
        SetNeedCommand(CHWORLD_INTHREAD_MULTIPLAY_NEEDALLTANKDATA);
    }
}


void CMutiplyProtocol::SendSyncObjAll(CListNode<CArchive>* pList)
{
    int i = 0;
    int nDataSize = 0;
    int nSize = pList->Size();
    if(nSize == 0) return;
    
    MutiplyProtocol_Sub* pSub = (MutiplyProtocol_Sub*)new char[sizeof(MutiplyProtocol_Sub) * nSize];
    
    //데이터를 다지워준다.
    for (ByNode<CArchive>* pit = pList->end()->GetPrevNode(); pit != &pList->GetHead();)
    {
        CArchive* pArc = pit->GetObject();
        pit = pit->GetPrevNode();
        if(!IsStop())
            nDataSize += SetMutiplyProtocol_Sub(&pSub[i],SUBCMD_OBJ_ALL_INFO,(int)pArc->GetSize(),(char*)pArc->GetBuffer());
        i++;
        delete pArc;
    }
    
    if(!IsStop())
        SetMutiplyProtocol(new MutiplyProtocol,CMD_SYNC,nSize,nDataSize,(char*)pSub);
    
    pList->Clear();
}




void CMutiplyProtocol::SendSyncMove(int nID,bool bSimpleMode,SPoint* ptTo,bool bFront)
{
    if(IsStop() || !mbEnemyStartPlaying) return;
    CArchive ar;
    ar << nID;
    ar << bSimpleMode;
    ar << *ptTo;
    ar << bFront;
    SetMutiplyProtocol(new MutiplyProtocol,CMD_OBJ_MOVETO,0,ar.GetSize(),(char*)ar.GetBuffer());
    
}

void CMutiplyProtocol::SendSyncMoveFighter(int nID,int nToID)
{
    if(IsStop() || !mbEnemyStartPlaying) return;
    const int SPSize = sizeof(int) + sizeof(int);
    char* sP = new char[SPSize];
    
    memcpy(sP, &nID, sizeof(int));
    memcpy(sP + sizeof(int), &nToID, sizeof(int));
    SetMutiplyProtocol(new MutiplyProtocol,CMD_OBJ_MOVETO_FIGHTER,0,SPSize,sP); //UDP 로직을 사용하지 말자. 받을때 로직이 잘못되었다.
    
}



void CMutiplyProtocol::SendSyncClickToByStick(int pOwnerID,SPoint* ptTo)
{
    if(IsStop() || !mbEnemyStartPlaying) return;
    CArchive ar;
    const int SPSize = sizeof(int) + sizeof(SPoint);
    char* sP = new char[SPSize];
    
    memcpy(sP, &pOwnerID, sizeof(int));
    memcpy(sP + sizeof(int), ptTo, sizeof(SPoint));
    SetMutiplyProtocol(new MutiplyProtocol,CMD_OBJ_CLICK_BYSTICK,0,SPSize,sP);
    delete[] sP;
}


void CMutiplyProtocol::SendSyncClickSpriteToByStick(int pOwnerID,int nID)
{
    if(IsStop() || !mbEnemyStartPlaying) return;
    const int SPSize = sizeof(int) + sizeof(int);
    char* sP = new char[SPSize];
    memcpy(sP, &pOwnerID, sizeof(int));
    memcpy(sP + sizeof(int), &nID, sizeof(int));
    SetMutiplyProtocol(new MutiplyProtocol,CMD_OBJ_CLICK_SPRITE_BYSTICK,0,SPSize,sP);
    delete[] sP;
}

void CMutiplyProtocol::SendSyncClickedNewBomb(int pOwnerID,SPoint* ptNow,SVector* vtDir,SVector* pDirAngle)
{
    if(IsStop() || !mbEnemyStartPlaying) return;
    CArchive arc;
    arc << pOwnerID;
    arc << *ptNow;
    arc << *vtDir;
    arc << *pDirAngle;
    SetMutiplyProtocol(new MutiplyProtocol,CMD_OBJ_CLICKED_NEW_BOMB,0,arc.GetSize(),(char*)arc.GetBuffer());
    
}

void CMutiplyProtocol::SendSyncChandeBomb(int OwnerID,int BombID)
{
    if(IsStop() || !mbEnemyStartPlaying) return;
    const int SPSize = sizeof(int) + sizeof(int);
    char* sP = new char[SPSize];
    memcpy(sP, &OwnerID, sizeof(int));
    memcpy(sP + sizeof(int), &BombID, sizeof(int));
    SetMutiplyProtocol(new MutiplyProtocol,CMD_CHANGE_BOMB,0,SPSize,(char*)sP);
}

void CMutiplyProtocol::SendSyncCompletedRuntimeUpgrade(CArchive* pArc)
{
    if(IsStop() || !mbEnemyStartPlaying) return;
    int nDataSize = 0;
    MutiplyProtocol_Sub* pSub = (MutiplyProtocol_Sub*)new char[sizeof(MutiplyProtocol_Sub)];
    nDataSize += SetMutiplyProtocol_Sub(pSub,0,pArc->GetSize(),(char*)pArc->GetBuffer());
    SetMutiplyProtocol(new MutiplyProtocol,CMD_COMPLETED_RUNTIMEUPGRADE,1,nDataSize,(char*)pSub);
}

void CMutiplyProtocol::SendSyncStopMove(int OwnerID)
{

    if(IsStop() || !mbEnemyStartPlaying) return;
    const int SPSize = sizeof(int);
    char* sP = new char[SPSize];
    memcpy(sP, &OwnerID, sizeof(int));
    
    SetMutiplyProtocol(new MutiplyProtocol,CMD_STOP_MOVE,0,SPSize,(char*)sP);
}



void CMutiplyProtocol::SendSyncMakeSprite(CSprite* pNewSprite)
{
    if(IsStop() || !mbEnemyStartPlaying) return;
    CArchive ar;
    pNewSprite->ArchiveMultiplay(SUBCMD_OBJ_ALL_INFO, ar, true);
    SetMutiplyProtocol(new MutiplyProtocol,CMD_MAKE_SPRITE,0,ar.GetSize(),(char*)ar.GetBuffer());
}

void CMutiplyProtocol::SendSyncMoveInAI(int nOwnerID,int nIndex,int nEnID,float fDistance)
{
    if(IsStop()) return;
    int SPSize = sizeof(int) + sizeof(int) + sizeof(int) + sizeof(float);
    char* sP = new char[SPSize];
    memcpy(sP, &nOwnerID, sizeof(int));
    memcpy(sP + sizeof(int), &nIndex, sizeof(int));
    memcpy(sP + sizeof(int) * 2, &nEnID, sizeof(int));
    memcpy(sP + sizeof(int) * 3, &fDistance, sizeof(float));
    
    SetMutiplyProtocol(new MutiplyProtocol,CMD_MOVEIN_TANK,0,SPSize,(char*)sP);
}

void CMutiplyProtocol::SendSyncMoveInGunAI(int nOwnerID,int nEnGunID)
{
    char* sP;
    int SPSize;
    if(IsStop()) return;
    SPSize = sizeof(int) + sizeof(int);
    sP = new char[SPSize];
    memcpy(sP, &nOwnerID, sizeof(int));
    memcpy(sP + sizeof(int), &nEnGunID, sizeof(int));
    SetMutiplyProtocol(new MutiplyProtocol,CMD_MOVEIN_TANK_BYGUN,0,SPSize,(char*)sP);
}


void CMutiplyProtocol::SendSyncDeleteObj(int nID,int WhosKilledID,bool bVisible)
{
    if(IsStop() || !mbEnemyStartPlaying) return;
    char* sP;
    int SPSize;
    SPSize = sizeof(int) * 2 + sizeof(bool);
    sP = new char[SPSize];
    memcpy(sP, &nID, sizeof(int));
    memcpy(sP + sizeof(int), &WhosKilledID, sizeof(int));
    memcpy(sP + sizeof(int)* 2, &bVisible, sizeof(bool));
    SetMutiplyProtocol(new MutiplyProtocol,CMD_DELETE_OBJECT,0,SPSize,(char*)sP);
    
}

void CMutiplyProtocol::SendSyncFighterSprite(CSprite* pSprite)
{
    if(IsStop() || !mbEnemyStartPlaying) return;
    CArchive ar;
    pSprite->ArchiveMultiplay(SUBCMD_OBJ_ALL_INFO, ar, true);
    SetMutiplyProtocol(new MutiplyProtocol,CMD_FIGHTER_SPRITE_SYNC,0,ar.GetSize(),(char*)ar.GetBuffer());
}

void CMutiplyProtocol::SendSyncBubble(CSprite* pSprite)
{
    
    if(IsStop() || !mbEnemyStartPlaying) return;
    int nInterval = 0;
    int nUpCount;
    float fScale;
    float* fColor;
    int nID = pSprite->GetID();
    CAniRuntimeBubble* pBubble = pSprite->GetAniRuntimeBubble();
    if(pBubble)
    {
    
        CArchive ar;
 
        ar << nID;

        if(pBubble->GetMaxTime())
        {
            nInterval = pBubble->GetMaxTime() - GetGGTime();
            ar << nInterval;
        }
        else
            ar << nInterval;
        
        nUpCount = pBubble->GetUpgradeCount();
        ar << nUpCount;
        fScale = pBubble->GetScale();
        ar << fScale;
        fColor = pBubble->GetColor();
        ar << fColor[0];
        ar << fColor[1];
        ar << fColor[2];
        SetMutiplyProtocol(new MutiplyProtocol,CMD_BUBBLE_SYNC,0,ar.GetSize(),(char*)ar.GetBuffer());
    }
}


void CMutiplyProtocol::SendSyncCurrentEnergy(CSprite* pSprite)
{
    if(IsStop() || !mbEnemyStartPlaying) return;
    char* sP;
    int SPSize;
    int nID = pSprite->GetID();
    float fCurr = pSprite->GetCurrentDefendPower();
    SPSize = sizeof(int) + sizeof(float);
    sP = new char[SPSize];
    
    memcpy(sP, &nID, sizeof(int));
    memcpy(sP + sizeof(int), &fCurr, sizeof(float));
    
    SetMutiplyProtocol(new MutiplyProtocol,CMD_CURRENT_ENERGY_SYNC,0,SPSize,(char*)sP);
}


unsigned int CMutiplyProtocol::GetFindMatchingGroup()
{
    //------------------------------------------------------------------------------
    //(버전 + 컴플리티드링크)<<16 + 맵아이디
    //------------------------------------------------------------------------------
    CUserInfo* pUserInfo = mpWorld->GetUserInfo();
    int nComRank = pUserInfo->GetCompletedRank();
    int nHiGroup = (PROTOCOL_VERSION + nComRank * 100) << 16;
    
    //맵을 전체 깬 유저가 아니면 사용자들이 많을 것으로 예상하여 현재 맵에 대해서만 매칭을 하지만
    //매을 전체 깬 유저는 사용자들이 적기 때문에 모든 맵에 대해서 매칭하게 하였다.
    //int HLowGroup = (nComRank == 1 ? mpWorld->GetUserInfo()->GetLastSelectedMapID() : 0);
    //return nHiGroup + HLowGroup;
    
    //Mofided 2014.11.20 그냥 맵전체로 검색하자 사용자가 없으니 매칭하기가 힘들다.
    return nHiGroup;
}


bool CMutiplyProtocol::SendRequestAliance(bool bAliance) //동맹요청/ 비동맹요청
{
    if(IsStop() || mbReauestingAliance) return false;
    if(bAliance)
    {
        msCntAliance ++;
        if( msCntAliance > 2) return false;
        
        mbReauestingAliance = true; //동맹 요청중
    }
    char* sP;
    int SPSize = sizeof(bool);
    sP = new char[SPSize];
    memcpy(sP, &bAliance, SPSize);
    SetMutiplyProtocol(new MutiplyProtocol,CMD_REQUEST_ALIANCE,0,SPSize,(char*)sP);
    return true;
}


void CMutiplyProtocol::SendResponseAliance(bool bAlianceYESNO) //응답 동맹요청에 대한 Yes/No
{
    if(IsStop()) return;
    char* sP;
    int SPSize = sizeof(bool);
    sP = new char[SPSize];
    memcpy(sP, &bAlianceYESNO, SPSize);
    SetMutiplyProtocol(new MutiplyProtocol,CMD_RESPONSE_ALIANCE,0,SPSize,(char*)sP);
}


void CMutiplyProtocol::ResetAliance() //새플레이면 현제 동맹관련 내용을 리셋해준다.
{
    msCntAliance = 0;
    mbReauestingAliance = false;
}










