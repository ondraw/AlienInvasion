//
//  CAIBlzCore.cpp
//  SongGL
//
//  Created by Songs on 11. 7. 28..
//  Copyright 2011 thinkm. All rights reserved.
//
#include <algorithm>
#include <stdlib.h>
#include "CAIBlzCore.h"
#include "CSprite.h"
#include "CHWorld.h"
#include "CSGLCore.h"
#include "CET1.h"
#include "sGLTrasform.h"
#include "sGLText.h"
#include "CET2.h"
#include "sGLUtils.h"
#include "CScenario.h"
#include "CMutiplyProtocol.h"

CAIBlzCore::CAIBlzCore(CSprite *pSprite,int nCnt,int* nTable)
{
    mpSprite = pSprite; 
    mnState = 1;  //처음 부터 인구가 없기 때문에 만드는 것으로 초기화 했다.
    mnManufTimer = 0;
    mnPosIndex = 0;

    mpArrObjectTable = nTable;
    mnArrObjectTableCnt = nCnt;

    
    mnMissionTime = 0;
    mnMisssionStep = MISSIONSETP1;
    
}

CAIBlzCore::~CAIBlzCore()
{
}

void CAIBlzCore::RuleStepIntervalTime(unsigned long ggTime)
{
    long nMultiplay = 1l;
    if(mpSprite->GetWorld()->GetMutiplyProtocol()->IsStop()) //혼자하거나 끈기면,
        nMultiplay = 2l;
    
    if(mnMisssionStep == MISSIONSETP1) //8초에 2개
    {
        mnManufTimer = ggTime + 4000L * nMultiplay;
    }
    else if(mnMisssionStep == MISSIONSETP2) //2.5에 1개
    {
        mnManufTimer = ggTime + 1250L * nMultiplay;
    }
    else if(mnMisssionStep == MISSIONSETP3) //3초에 2개
    {
        mnManufTimer = ggTime + 1500L * nMultiplay;
    }
    else
    {
        mnManufTimer = ggTime + 2000L * nMultiplay;
    }
}

void CAIBlzCore::RuleNextStep(unsigned long ggTime)
{
    if(mnMissionTime == 0 && mnMisssionStep == MISSIONSETP1) //최소파워 객체 30초동안 8초간격으로
    {
        mnMissionTime = ggTime + 30000L;
    }
    else if(mnMisssionStep == MISSIONSETP1 && ggTime > mnMissionTime)
    {
        mnMisssionStep = MISSIONSETP2; //최소파워 30초동안 5초간격으로
        mnMissionTime = ggTime + 30000L;
    }
    else if(mnMisssionStep == MISSIONSETP2 && ggTime > mnMissionTime)
    {
        mnMisssionStep = MISSIONSETP3; //다음레벨 1분동안 8초간격으로 만들기. (마지막 단계까지...)
        mnMissionTime = ggTime + 60000L;
        
    }
    else if(mnMisssionStep == MISSIONSETP3 && ggTime > mnMissionTime)
    {
        mnMisssionStep = MISSIONSETP3; //다음레벨 1분동안 8초간격으로 만들기. (마지막 단계까지...)
        mnMissionTime = ggTime + 60000L;
    }
}

int  CAIBlzCore::RuleMakObjectIndex()
{
    return rand() % mnArrObjectTableCnt;
}

void CAIBlzCore::RenderBeginCore(int nTime)
{
    if(mnState == 0) return;
    
    unsigned long ggTime = GetGGTime();
    
    if(ggTime > mnManufTimer || mnManufTimer == 0)
    {   
        //--------------------------------------------
        //탱크 하나를 생성한다.
        //--------------------------------------------
        //초기화 할때 모델들을 미리 읽어서.. 정의 해둔다. 타워에서 모델을 만들때 텍스쳐가 없으면 로딩하기 때문에 
        //쓰레드에서 테스쳐를 바운딩할때 에러가 난다. 주의 해야한다.
        //Initialize2 에서 미리 만든당.
        
#if !PATH_TEST
        if(mnMisssionStep == MISSIONSETP1)
        {
            MakeTank();
            if(mnState == 0) return;
            MakeTank();
        }
        else if(mnMisssionStep == MISSIONSETP2)
        {
            MakeTank();
        }
        else if(mnMisssionStep == MISSIONSETP3)
        {
            MakeTank();
            if(mnState == 0) return;
            MakeTank();
        }
        else
            MakeTank();
            
#endif //PATH_TEST
        
        RuleStepIntervalTime(ggTime);
    }
    
    //------------------------------------------------------------------------------
    // 자신의 체력에 따라 위급하게 처리함.
    //------------------------------------------------------------------------------
    float fRate = mpSprite->GetCurrentDefendPower() / mpSprite->GetMaxDefendPower();
    if(mnMisssionStep <= MISSIONSETP2)
    {
        //터지기 일보 직전이라면 1초에 한개씩 만든다.
        if(fRate < 0.7)
            mnMisssionStep = MISSIONSETP3; //1초간격으로 만들기
    }
    else if(mnMisssionStep == MISSIONSETP4)
    {
        //터지기 일보 직전이라면 1초에 한개씩 만든다.
        if(fRate < 0.4)
            mnMisssionStep = MISSIONSETP4; //1초간격으로 만들기
    }

    //------------------------------------------------------------------------------
    RuleNextStep(ggTime);
}

void CAIBlzCore::SetMakePosition(CSprite *pTank,SPoint* ptCenter)
{
    SPoint ptOut;
    float fRadius = mpSprite->GetRadius() * 4.f;
    float f[3];
    float matrix[16];
    
    f[0] = fRadius;
    f[1] = 0;
    f[2] = 0;
    
    if(mnPosIndex > 8)    mnPosIndex = 0;
    
    sglLoadIdentityf(matrix);
    sglRotateRzRyRxMatrixf(matrix,
                           0,
                           40 * mnPosIndex,
                           0);
    mnPosIndex++;
    
    //좌표를 이동한다.
    sglMultMatrixVectorf(f,matrix,f);
    
    memcpy(&ptOut, ptCenter, sizeof(SPoint));
    
    ptOut.x += f[0];
    ptOut.y = 0;
    ptOut.z += f[2];
    
    if(CSglTerrian::IsObstacle(&ptOut))
    {
        SetMakePosition(pTank,ptCenter);
        return;
    }
    
    ((CHWorld*)mpSprite->GetWorld())->GetPositionY(&ptOut);
    
    if(pTank->GetType() == ACTORTYPE_FIGHTER) //땅에 기어다니는 것은 장애물지역인지 아닌지를 재조정해준다.
        ptOut.y += FIGHTER_MIN_HEIGHT;
    
    if(!pTank->SetPosition(&ptOut))
    {
        SetMakePosition(pTank,ptCenter);
        return;
    }
}

void CAIBlzCore::MakeTank()
{
    
    SVector vDir;
    SPoint ptNow;
    CHWorld* pWorld = (CHWorld*)mpSprite->GetWorld();
    CSGLCore *pSGLCore = pWorld->GetSGLCore();
    mpSprite->GetPosition(&ptNow);
    
    CSprite *pTank = NULL;
    PROPERTY_TANK property;
    
    int nRandIndex = RuleMakObjectIndex();
    if (nRandIndex == -1)
    {
        HLogD("Can not make tanke RunMakObjectIndex\n");
        return ;
    }
    
//    pTank = CSGLCore::MakeSprite(pWorld->GetNewID(), mpSprite->GetTeamID(), 2817,pWorld->GetTextureMan(),pSGLCore->GetAction(), pWorld,NETWORK_MYSIDE);
//    pTank = CSGLCore::MakeSprite(pWorld->GetNewID(), mpSprite->GetTeamID(), ACTORID_ET2,pWorld->GetTextureMan(),pSGLCore->GetAction(), pWorld,NETWORK_MYSIDE);
    pTank = CSGLCore::MakeSprite(pWorld->GetNewID(), mpSprite->GetTeamID(), mpArrObjectTable[nRandIndex],pWorld->GetTextureMan(),pSGLCore->GetAction(), pWorld,NETWORK_MYSIDE);
    if(pTank == NULL)
    {
        HLogD("Can not make tanke ID %d\n",mpArrObjectTable[nRandIndex]);
        return;
    }
    pTank->LoadResoure(mpSprite->GetWorld()->GetModelMan());
    
//    if(mnMisssionStep >= MISSIONSETP3)
//        pTank->Command(AICOMMAND_CHANGE_ATTACK_TYPE, (void*)STRATEGY_TYPE_ATT); //항상공격형으로
    
    vDir.x = -1; 
    vDir.y = 0;
    vDir.z = -1;
    sglNormalize(&vDir);
    pTank->Initialize(&ptNow, &vDir);
    pTank->SetMainBlzTower(mpSprite);
    SetMakePosition(pTank,&ptNow);
    pSGLCore->AddSprite(pTank);
    mpSprite->GetWorld()->GetMutiplyProtocol()->SendSyncMakeSprite(pTank);
    
#if TestPoStart_F
    ((CET1*)pTank)->TestPoStart();
#endif
}

void CAIBlzCore::Command(AICORE_AICOMMANDD nCommandID,void *pData)
{
    switch (nCommandID) {
        case AICOMMAND_MANUFACTURE: //생산명령어를 받다. pData = bool
            mnState = *(int*)pData;
            break;
            
        default:
            break;
    }
}



