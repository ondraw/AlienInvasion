//
//  CAICore.cpp
//  SongGL
//
//  Created by Songs on 11. 7. 20..
//  Copyright 2011 thinkm. All rights reserved.
//

#include "CAICore.h"
#include "CMoveAI.h"
#include "CSprite.h"
#include "CStrategyAI.h"
#include "CSglTerrian.h"
#include "CStrategyActorAI.h"

CAICore::CAICore(CSprite *pSprite,AAI* pMoveAI,AAI* pStrategyAI)
{
    mpSprite = pSprite;
    mpMoveAI = pMoveAI;
    mpStrategyAI = pStrategyAI;
    mbActor = false;
}

CAICore::~CAICore()
{
    if(mpMoveAI)
    {
        delete mpMoveAI;
        mpMoveAI = NULL;
    }
    
    
    if(mpStrategyAI)
    {
        delete mpStrategyAI;
        mpStrategyAI = NULL;
    }
}
void CAICore::ResetAttackTo()
{
    if(mpStrategyAI)
        mpStrategyAI->ResetAttackTo();
}

void CAICore::SetActor()
{
    mbActor = true;
    
    //지우고 액터 거시기를 할당한다.
    if(mpStrategyAI)
    {
        delete mpStrategyAI;
        mpStrategyAI = NULL;
    }
    mpStrategyAI = new CStrategyActorAI(mpSprite,mpMoveAI,true);
}

void CAICore::RenderBeginCore(int nTime)
{
    if(mpStrategyAI)    mpStrategyAI->RenderBeginCore(nTime);
    if(mpMoveAI)        mpMoveAI->RenderBeginCore(nTime);
}


void CAICore::Command(AICORE_AICOMMANDD nCommandID,void *pData)
{
    if(mpStrategyAI)
        mpStrategyAI->Command(nCommandID, pData);
//    else if(mpMoveAI) //주인공은 mpStrategyAI가 없다.
//    {
//        mpMoveAI->Command(nCommandID, pData);
//    }
}

CSprite* CAICore::GetAttackTo()
{
    if(mpStrategyAI)
        return mpStrategyAI->GetAttackTo();
    return NULL;
}


CSprite* CAICore::GetAttackTo(int nIndex)
{
    if(mpStrategyAI)
        return mpStrategyAI->GetAttackTo(nIndex);
    return NULL;
}

CSprite* CAICore::GetAttackToByGun()
{
    if(mpStrategyAI)
        return mpStrategyAI->GetAttackToByGun();
    return NULL;
}

void CAICore::SetIsTrainningMode(bool v)
{
    if(mpStrategyAI) ((CStrategyAI*)mpStrategyAI)->SetIsTrainningMode(v);
}