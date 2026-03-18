//
//  CStrategyET2AI.cpp
//  SongGL
//
//  Created by itsme on 2014. 3. 18..
//
//

#include "CStrategyET2AI.h"
#include "CAICore.h"
#include "CMoveAI.h"
#include "CHWorld.h"
#include "CMutiplyProtocol.h"
#include "CET2.h"

CStrategyET2AI::CStrategyET2AI(CSprite *pSprite,AAI* pMoveID,bool bIsCun) : CStrategyAI(pSprite,pMoveID,bIsCun)
{
}
CStrategyET2AI::~CStrategyET2AI()
{

}


CSprite* CStrategyET2AI::GetAttackTo()
{
    if( mfMovinAttackDistance < 1000.f)
        return mpMovinAttack;
    return NULL;
}

void CStrategyET2AI::RenderBeginCore(int nTime)
{
    unsigned long lNow = GetGGTime();
    CSprite* orgSP = mpMovinAttack;
    if(orgSP && orgSP->GetState() != SPRITE_RUN)
    {
        mpMovinAttack = NULL;
        mfMovinAttackDistance = mpSprite->GetDefendRadianBounds();
        mType = STRATEGY_NONE;
        mRecTime = GetGGTime() + 10000;
    }
    else if(orgSP)
    {
        if(!mvAI->IsAutoMovingAndTurn() && mfMovinAttackDistance > 1000.f)
        {
            mType = STRATEGY_ATTACK_MOVE;
            mvAI->CmdMoveTo(orgSP, false);
        }
    }
    
    if( (mType == STRATEGY_NONE || (mType == STRATEGY_RECON && !mvAI->IsAutoMovingAndTurn())) &&
       lNow > mRecTime &&
       mpSprite->GetNetworkTeamType() == NETWORK_MYSIDE)
    {
        SPoint ptRec;
        if(GetReconPos(&ptRec))
        {
            mvAI->CmdMoveTo(&ptRec,true);
            mType = STRATEGY_RECON;
        }
        mRecTime = lNow + 3000;
    }
    ((CET2*)mpSprite)->AimAutoToEnemy(mfMovinAttackDistance);
}

void CStrategyET2AI::Command(int nCommandID,void *pData)
{
    switch (nCommandID)
    {
        case AICOMMAND_COMPACTTOSPRITE:
        {
            CSprite* pSP = (CSprite*)pData;
            //객체랑 충돌을 하였다.
            mvAI->AvoidCompactSprite(pSP);
            
            if(pSP)
            {
                CHWorld* pHWorld = (CHWorld*)mpSprite->GetWorld();
                if(!pHWorld->IsAllians( pSP->GetTeamID(),mpSprite->GetTeamID()))
                {
                    mType = STRATEGY_ATTACK;
                    mpMovinAttack = pSP;
                    mfMovinAttackDistance = 1;
                }
            }
            return;
        }
        case AICOMMAND_MOVEIN: //사정거리 안쪽으로 이동하면 미사일을 쏜다.
        {
            CSprite* orgSP = mpMovinAttack;
            CSprite* inSP = (CSprite*)pData;
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
                    if(mType == STRATEGY_ATTACKED_MOVGIN)
                    {
                        mType = STRATEGY_ATTACK;
                        mvAI->StopMove();
                    }
                    mpMovinAttack = orgSP;
//                    if(mpSprite->GetNetworkTeamType() == NETWORK_MYSIDE)
//                        mpSprite->GetWorld()->GetMutiplyProtocol()->SendSyncMoveInAI(mpSprite->GetID(),-1,orgSP->GetID(),mfMovinAttackDistance);
                }
            }
            return;
        }
        case AICOMMAND_MOVEOUT:
        {
            CSprite* orgSP = mpMovinAttack;
            if(orgSP == pData && mType != STRATEGY_ATTACKED_MOVGIN)
            {
                mfMovinAttackDistance = orgSP->GetDefendRadianBounds();
                mpMovinAttack = NULL;
                mType = STRATEGY_NONE;
                mRecTime = GetGGTime() + 10000;
            }
            return;
        }
        case AICOMMAND_ATTACKEDBYEMSPRITE: //적군한테 공격당하였다면 그넘한테 이동하여 공격한다.
        {
            if(pData == NULL)  break;
            CSprite* inSP = (CSprite*)pData;
            CSprite* orgSP = mpMovinAttack;
            if(inSP->GetType() == ACTORTYPE_FIGHTER && !mpSprite->IsCurrentBombMissileType())  return;
            if(orgSP == NULL) //이동한다.
            {
                SPoint ptNow,ptEnNow;
                mpSprite->GetPosition(&ptNow);
                inSP->GetPosition(&ptEnNow);
                mfMovinAttackDistance = sglSquareDistanceSPointSPointf(&ptNow, &ptEnNow);
                mvAI->CmdMoveTo(&ptEnNow,false); //이동한다.
                mpMovinAttack = inSP;
                mType = STRATEGY_ATTACKED_MOVGIN;
            }
            return;
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
            mType = STRATEGY_ATTACK_MOVE;
            return;
        }
    }
    CStrategyAI::Command(nCommandID, pData);
}

////단순하게 조절해서 맞추는 것이 아니라 멀면, 가까이 이동하여 공격해야한다.
//void CStrategyET2AI::CmdAttackTo(CSprite* pAttackTo)
//{
//    mpSprite->AimAutoToEnemy(pAttackTo);
//}
