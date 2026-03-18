//
//  CStrategy.cpp
//  SongGL
//
//  Created by 호학 송 on 11. 7. 24..
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#include <stdlib.h>
#include "CSGLCore.h"
#include "CStrategyAI.h"
#include "CSprite.h"
#include "CSGLController.h"
#include "CSglTerrian.h"
#include "CAICore.h"
#include "CMoveAI.h"
#include "CHWorld.h"
#include "CMutiplyProtocol.h"

std::vector<std::vector<int>*> CStrategyAI::glstRecon;
CStrategyAI::CStrategyAI(CSprite *pSprite,AAI* pMoveID,bool bIsCun)
{
    mpSprite = pSprite;
    mType = STRATEGY_NONE;
    mCntFire = 0;
    mpMovinAttack = NULL;

    mvAI = (CMoveAI*)pMoveID;
    if(bIsCun)
        mfGunDefendRadianBoundse = pSprite->GetDefendRadianBounds() / 2.0f;
    else
        mfGunDefendRadianBoundse = 0.0f;
    mpEnemyGunSprite = NULL;
    mfMovinAttackDistance = pSprite->GetDefendRadianBounds();
    mbNeedStop = false;
    mRecTime = GetGGTime() + 10000;
    mStopTime = 0;
}

CStrategyAI::~CStrategyAI()
{
}

void CStrategyAI::ResetAttackTo()
{
    mpMovinAttack = NULL;
    mpEnemyGunSprite = NULL;
    mCntFire = 0;
    mRecTime = GetGGTime() + 10000;

}

void CStrategyAI::RenderBeginCore(int nTime)
{
    unsigned long lNow = GetGGTime();
    CSprite* orgSP = mpMovinAttack;
    CSprite* orgGunSP = mpEnemyGunSprite;
    
    if(mbNeedStop && lNow > mStopTime)
    {
        mvAI->StopMove();
        mbNeedStop = false;
    }
    
    if(orgSP && orgSP->GetState() != SPRITE_RUN)
    {
        mpMovinAttack = NULL;
        mfMovinAttackDistance = mpSprite->GetDefendRadianBounds();
        
        if(mType == STRATEGY_ATTACK_MOVE)
            mvAI->StopMove();
        
        
        mCntFire = 0;
        mType = STRATEGY_NONE;
        mRecTime = lNow + 10000;
    }
    else if (orgSP)
        CmdAttackTo(orgSP);
    
    if(orgGunSP && orgGunSP->GetState() != SPRITE_RUN)
    {
        mpEnemyGunSprite = NULL;
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
        
}

void CStrategyAI::Init_Static(SPoint* ptMainTowerPos,vector<STARTPOINT*>* plstStartPoint)
{
    int nCnt = (int)plstStartPoint->size();
    for (int i = 0; i < nCnt; i++) 
    {
        STARTPOINT* pStart = plstStartPoint->at(i);
    
        SPoint ptTarget;
        ptTarget.x = pStart->fPosX;
        ptTarget.y = pStart->fPosY;
        ptTarget.z = pStart->fPosZ;
        
        std::vector<int>* pResult = 0;
        pResult = CSglTerrian::FindPath(NULL,ptMainTowerPos,&ptTarget);
        glstRecon.push_back(pResult);
    }
}

void CStrategyAI::Close_Static()
{
    int nCnt = (int)glstRecon.size();
    for (int i = 0; i < nCnt; i++) 
    {
        delete glstRecon.at(i);
    }
    glstRecon.clear();
}


void CStrategyAI::Command(int nCommandID,void *pData)
{
    switch (nCommandID) 
    {
        case AICOMMAND_COMPACTTOSPRITE:
        {
            //객체랑 충돌을 하였다.
            mvAI->AvoidCompactSprite((CSprite*)pData);
            break;
        }   
        case AICOMMAND_MOVEIN: //사정거리 안쪽으로 이동하면 미사일을 쏜다.
        {
            if(pData == NULL) break;
            
            CSprite* orgSP = mpMovinAttack;
            CSprite* inSP = (CSprite*)pData;
            CSprite* orgGunSP = mpEnemyGunSprite;
            
            if(mType == STRATEGY_ATTACKED_MOVGIN) //누군가 공격해서 이동중에 적이 사정거리 안쪽으로 드러오면 멈춘다.
            {
                //너무 갑작이 멈추지 말고 좀 시간을 딜레이 한후에 멈추자.
                mbNeedStop = true;
                mStopTime = GetGGTime() + 2000l;
                mType = STRATEGY_ATTACK;
            }
            
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
                    mCntFire = 0;
                    if(mType == STRATEGY_ATTACK_MOVE)
                        mvAI->StopMove();
                    
                    mType = STRATEGY_ATTACK;
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
                mType = STRATEGY_NONE;
                mRecTime = GetGGTime() + 10000;
                
            }
            break;
        }
        case AICOMMAND_HITTHEMARK: //명중하였다. (공격 카운터를 주어서 공격의 정확도를 높혀보자)
            mCntFire = 0;
            if(mType == STRATEGY_ATTACK_MOVE && mpSprite->GetNetworkTeamType() == NETWORK_MYSIDE)
                mvAI->StopMove();
            break;
            
        case AICOMMAND_BOMBED:
            mCntFire++;
            if(mpMovinAttack &&
               mCntFire > AICORE_MAX_FIRE_DETAILATTACK &&
               mpSprite->GetNetworkTeamType() == NETWORK_MYSIDE)
            {
                CmdAttackToDetail(mpMovinAttack);
                mCntFire = 0;
                
                //잘맞지를 않아서 그쪽으로 가까이가서 공격하자.
                mType = STRATEGY_ATTACK_MOVE;
            }
            break;
            //적군한테 공격당하였다면 그넘한테 이동하여 공격한다.
        case AICOMMAND_ATTACKEDBYEMSPRITE:
            
            if(pData && mpSprite->GetNetworkTeamType() == NETWORK_MYSIDE)
            {
                //멈추는 현상이 있다 다시 이동하게 하자.
                if(mType == STRATEGY_ATTACKED_MOVGIN && mvAI->IsAutoMovingAndTurn() == false && mpSprite->GetAttackTo() == NULL)
                {
                    SPoint ptMove;
                    CSprite* pSP = (CSprite*)pData;
                    pSP->GetPosition(&ptMove);
                    mvAI->CmdMoveTo(&ptMove,true);
                }
                else if(mType != STRATEGY_ATTACKED_MOVGIN && //누군가 공격해서 이동중일때 누가 또 공격하면 무시한다.
                   (mpSprite->GetAttackTo() == NULL || mType == STRATEGY_RECON))
                {
                    SPoint ptMove;
                    CSprite* pSP = (CSprite*)pData;
                    pSP->GetPosition(&ptMove);
                    mvAI->CmdMoveTo(&ptMove,true);
                    
                    //누가 공격해서 이동중이다.
                    mType = STRATEGY_ATTACKED_MOVGIN;
                }
            }
            break;
        case AICOMMAND_MOVE_SIMPLE_POS:
        {
            mvAI->SetSimpleMove(true);
            mvAI->CmdMoveTo((SPoint*)pData,false);
        }
            break;
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
            //        case AICOMMAND_CHANGE_ATTACK_TYPE:
            //            mType = (STRATEGY_TYPE)(long)(pData);
            //            break;
        default:
            break;
    }
    
}


//메인 타워 기준으로 각 속성에 맞게 정찰 임무를 구성한다.
int CStrategyAI::GetRecon()
{
    int nV = 0,nV2 = 0;
    int nRand = rand();
    
    
    int nCnt = (int)glstRecon.size();
    if (nCnt == 0) return -1;
    
    nV = nRand % nCnt;
    
    std::vector<int>* path = glstRecon.at(nV);
    
    int nCnt2 = (int)path->size();
    if (nCnt2 == 0) return -1;
    nV2 = nRand % nCnt2;
    
    return path->at(nV2);
}


//단순하게 조절해서 맞추는 것이 아니라 멀면, 가까이 이동하여 공격해야한다.
void CStrategyAI::CmdAttackTo(CSprite *pSprite)
{
    if(mpSprite->CanFireMissile() == false || mpSprite->isMovingAutoHeader())
        return;
    mpSprite->AimAutoToEnemy(pSprite);
}


//자기자신은 이동할 돔이고 pSprite이쪽으로 이동해야한다
//블럭이 다른 블럭속에 잇다면 (이동명령어가 존재하는데ㅖ)
//Info 같은블럭 안에 있을때는 이동을 할수가 없다. 좀더 정밀한 타격을 주고자 한다면 여기를 수정한다.
void CStrategyAI::CmdAttackToDetail(CSprite* pSprite)
{
    if(pSprite->isMoving() == true || pSprite->isTurning() == true) return;

    mvAI->CmdMoveTo(pSprite,false);
}

bool CStrategyAI::GetReconPos(SPoint* ptOut)
{
    if(mType == STRATEGY_NONE || mType == STRATEGY_RECON) //아무일도 하지 않는다면, 정찰을 진행한다.
    {
        int nIndex = GetRecon();
        if(nIndex != -1)
        {
            CSglTerrian::GetIndexToCenterPoint(nIndex , ptOut);
            return true;
        }
    }
    return false;
}

void CStrategyAI::ArchiveMultiplay(int nCmd,CArchive& ar,bool bWrite)
{
    int nID = 0,nGunID = 0;
    if(nCmd == SUBCMD_OBJ_ALL_INFO)
    {
        if(bWrite)
        {
            CSprite* pSprite = mpMovinAttack;
            if(pSprite)
                nID = pSprite->GetID();
            
            
            pSprite = mpEnemyGunSprite;
            if(pSprite)
                nGunID = pSprite->GetID();
            
            ar << nID;
            ar << nGunID;
        }
        else
        {
            ar >> nID;
            ar >> nGunID;
            if(nID)
            {
                mpMovinAttack= ((CHWorld*)mpSprite->GetWorld())->GetSGLCore()->FindSprite(nID);
                if( mpMovinAttack &&  mpMovinAttack->GetState() != SPRITE_RUN)
                    mpMovinAttack = NULL;
            }
            if(nGunID)
            {
                mpEnemyGunSprite= ((CHWorld*)mpSprite->GetWorld())->GetSGLCore()->FindSprite(nGunID);
                if( mpEnemyGunSprite &&  mpEnemyGunSprite->GetState() != SPRITE_RUN)
                    mpEnemyGunSprite = NULL;
            }
        }
    }
}