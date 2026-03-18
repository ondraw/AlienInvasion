//
//  CStrategyActorAI.cpp
//  SongGL
//
//  Created by 호학 송 on 13. 9. 10..
//
//

#include "CStrategyActorAI.h"
#include "CMoveAI.h"
#include "CHWorld.h"
#include "CMutiplyProtocol.h"

CStrategyActorAI::CStrategyActorAI(CSprite *pSprite,AAI* pMoveID,bool bIsCun) : CStrategyAI(pSprite,pMoveID,bIsCun)
{
    mClickFireTime = 0;
    mbTrainningMode = false;

}

CStrategyActorAI::~CStrategyActorAI()
{
}

void CStrategyActorAI::RenderBeginCore(int nTime)
{
    CSprite* orgSP = mpMovinAttack;
    CSprite* orgGunSP = mpEnemyGunSprite;
    
    if(orgSP && orgSP->GetState() != SPRITE_RUN)
    {
        mpMovinAttack = NULL;
        mfMovinAttackDistance = mpSprite->GetDefendRadianBounds();
    }
    else if(orgSP)
        CmdAttackTo(orgSP);
    
    if(orgGunSP && orgGunSP->GetState() != SPRITE_RUN)
    {
        mpEnemyGunSprite = NULL;
    }
    
}

void CStrategyActorAI::SetAttackTo_Multiplay(int nIndex,CSprite* pSprite,float fDistance)
{
    if(mpMovinAttack != pSprite)
    {
        mpMovinAttack = pSprite;
        mfMovinAttackDistance = fDistance;
    }
}

void CStrategyActorAI::SetAttackToByGun_Multiplay(CSprite* pSprite)
{
    if(mpEnemyGunSprite != pSprite)
    {
        mpEnemyGunSprite = pSprite;
    }

}

void CStrategyActorAI::Command(int nCommandID,void *pData)
{
    switch (nCommandID)
    {
        case AICOMMAND_DONT_AUTO_FIRE:
            mClickFireTime = GetGGTime() + 1000; //1초후에는 자동 모드
            mpMovinAttack = NULL;
            mfMovinAttackDistance = mpSprite->GetDefendRadianBounds();
            break;
            
        //적군한테 공격당하였다면 반격한다.
        case AICOMMAND_ATTACKEDBYEMSPRITE:
        {
            if(pData == NULL) break;
            CSprite* inSP = (CSprite*)pData;
            CSprite* orgSP = mpMovinAttack;
            
            if(GetGGTime() > mClickFireTime && IsTrainningMode() == false)
            {
                //미사일타입일때만 비행기를 공격할 수가 있다.
                if(inSP->GetType() == ACTORTYPE_FIGHTER && !mpSprite->IsCurrentBombMissileType())  break;
                if(orgSP == NULL)
                {
                    SPoint ptNow,ptEnNow;
                    mpSprite->GetPosition(&ptNow);
                    inSP->GetPosition(&ptEnNow);
                    mfMovinAttackDistance = sglSquareDistanceSPointSPointf(&ptNow, &ptEnNow);
                    mpMovinAttack = inSP;
                    
                    //----------------------------------------------------------
                    //상대방에 데이터를 보내주지 않아도 된다. 로직상 기본적으로 반응해주기 때문에
                    //----------------------------------------------------------
                }
            }
            break;
        }
            
        case AICOMMAND_MOVEIN:
            if(GetGGTime() > mClickFireTime && IsTrainningMode() == false)
            {
                if(pData == NULL) break;
                
                CSprite* orgSP = mpMovinAttack;
                CSprite* inSP = (CSprite*)pData;
                CSprite* orgGunSP = mpEnemyGunSprite;
                
                
                if(mfGunDefendRadianBoundse != 0.f)
                {
                    //건은 폭탄 사정거리 안쪽에 있기 때문에 벗어 나는 것은 AICOMMAND_MOVEOUT에 하면 안된다.
                    if(mpEnemyGunSprite == inSP && inSP->GetColidedDistance() > mfGunDefendRadianBoundse)
                    {
                        mpEnemyGunSprite = NULL;
                    }
                    else if(inSP->GetColidedDistance() < mfGunDefendRadianBoundse) //거리 안쪽으로 드러오면 공격해준다.
                    {
                        //처음 컨택한 넘만 죽을때 까정 계속해서 공격해준다.
                        if (orgGunSP == NULL)
                        {
                            orgGunSP = inSP;
                        }
                        
                        if(orgGunSP != mpEnemyGunSprite)
                        {
                            mpEnemyGunSprite = orgGunSP;
                            
//                            //상대방에 데이터를 보내어 기본적으로 동기화가 되게 해준다.
//                            if(mpSprite->GetNetworkTeamType() == NETWORK_MYSIDE)
//                                mpSprite->GetWorld()->GetMutiplyProtocol()->SendSyncMoveInGunAI(mpSprite->GetID(),orgGunSP->GetID());
                        }
                    }
                }
                
                //미사일타입일때만 비행기를 공격할 수가 있다.
                if(!(inSP->GetType() == ACTORTYPE_FIGHTER && !mpSprite->IsCurrentBombMissileType()))
                {
                    if (orgSP == NULL) //inSP->GetColidedDistance() < GetDefendRadianBounds()로 제한을 하지 않는 이유는 MoveIn으로 드러올때 이미 걸러서 드러온다.
                    {
                        orgSP = inSP;
                        mfMovinAttackDistance = inSP->GetColidedDistance();
                    }
                    else if(orgSP != inSP && inSP->GetColidedDistance() < mfMovinAttackDistance) //더가까운 넘을 공격한다.
                    {
                        orgSP = inSP;
                        mfMovinAttackDistance = inSP->GetColidedDistance();
                    }
                    else if(orgSP == inSP) //이미 가지고 있다 하더라도, 거리는 계속 갱신해주어야 한다.
                        mfMovinAttackDistance = inSP->GetColidedDistance();
                        
                    
                    if(orgSP != mpMovinAttack)
                    {
                        mpMovinAttack = orgSP;
                        
////                        //상대방에 데이터를 보내어 동기화를 하지 않아도. 거리순으로 소팅되어 있기 때문에 객체들이 알맞게 위치해있으면 동기화가 자동으로 된다.
////                        //상대방에 데이터를 보내어 기본적으로 동기화가 되게 해준다.
//                        if(mpSprite->GetNetworkTeamType() == NETWORK_MYSIDE)
//                            mpSprite->GetWorld()->GetMutiplyProtocol()->SendSyncMoveInAI(mpSprite->GetID(),-1,orgSP->GetID(),mfMovinAttackDistance);
                    }
                }
                
                
            }
            break;
        case AICOMMAND_MOVEOUT:
        {
            CSprite* orgSP = mpMovinAttack;
            if(orgSP == pData)
            {
                mfMovinAttackDistance = orgSP->GetDefendRadianBounds();
                mpMovinAttack = NULL;
            }
            break;
        }
        case AICOMMAND_COMPACTTOSPRITE:
        {
            mvAI->AvoidCompactSprite((CSprite*)pData);
        }
            break;
        case AICOMMAND_MOVE_SIMPLE_POS:
        {
            mvAI->SetSimpleMove(true);
            mvAI->CmdMoveTo((SPoint*)pData,false);
            break;
        }
        case AICOMMAND_MOVE_MULTIPLAY:
        {
            SPoint* ptTo;
            SGLEvent* pEvent = (SGLEvent*)pData;
            ptTo = (SPoint*)pEvent->lParam;
            bool bFront = (bool)pEvent->lParam3;
            bool bSmpleMode = (bool)pEvent->lParam2;
            if(bSmpleMode)
                mvAI->SetSimpleMove(true);
            else
                mvAI->SetSimpleMove(false);
            mvAI->CmdMoveTo(ptTo,bFront);
        }
            break;
        default:
            break;
    }
}

