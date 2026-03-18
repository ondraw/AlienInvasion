//
//  CStrategyAmbulanceAI.cpp
//  SongGL
//
//  Created by itsme on 2014. 5. 22..
//
//

#include "CStrategyAmbulanceAI.h"
#include "CSprite.h"
#include "AAICore.h"
#include "CMoveAI.h"
#include "CAmbulance.h"

CStrategyAmbulanceAI::CStrategyAmbulanceAI(CSprite *pSprite,AAI* pMoveAI) : CStrategyAI(pSprite,pMoveAI,false)
{
    mInNextTime = 0;
    mOutNextTime = 0;
    mfMainTowerFar = 0;
    mpMainIn = false;
    mMovingByUser = false;
    mpMainTower = NULL;
}

CStrategyAmbulanceAI::~CStrategyAmbulanceAI()
{
    
}



void CStrategyAmbulanceAI::RenderBeginCore(int nTime)
{
    if(mpSprite->GetType() == ACTORTYPE_TYPE_GHOST) return;
    
    //fInterLen <= ATTACRADIAN
    if(mpMovinAttack && mpMovinAttack->GetState() == SPRITE_RUN && mpSprite->GetNetworkTeamType() == NETWORK_MYSIDE)
    {
        if(!mvAI->isMoving() && !mvAI->isTurning()) //도착했는데 이동한다면 다시 이동해준다.
        {
            SPoint ptTo;
            mpMovinAttack->GetPosition(&ptTo);
            mvAI->SetSimpleMove(true);
            mvAI->CmdMoveTo(&ptTo,false);
        }
    }
    else if(mpMovinAttack)
        mpMovinAttack = NULL;
    
    if(!mvAI->isMoving() && !mvAI->isTurning())
        mMovingByUser = false;
    
    if(mpMainTower && mpMainTower->GetState() != SPRITE_RUN)
    {
        mpMainIn = false;
        mpMainTower = NULL;
        mfMainTowerFar = 0.f;
    }
}


void CStrategyAmbulanceAI::Command(int nCommandID,void *pData)
{
    if(mpSprite->GetType() == ACTORTYPE_TYPE_GHOST)
    {
        if(nCommandID == AICOMMAND_COMPLETEDMAKE)
        {
            //---------------------------------------------------
            mInNextTime = GetGGTime() + 1000l; //8초에후에 이동같은 메세지를 보내야 상대의 느린단말기는 Sprite를 Add하기전에 무부를 보내기 때문에 받지를 못한다.
            //---------------------------------------------------
            PROPERTY_TANK* pTank = mpSprite->GetPropertyTank();
            mpSprite->SetType(pTank->nType);
        }
        return;
    }
    
    switch (nCommandID)
    {
        case AICOMMAND_COMPACTTOSPRITE:
        {
            if(pData != NULL && (mpMovinAttack == pData || mpMainTower == pData))
            {
                mpMovinAttack = NULL;
                mpMainTower = NULL;
                mfMainTowerFar = 0.f;
                mvAI->StopMove();
            }
            //객체랑 충돌을 하였다.
            mvAI->AvoidCompactSprite((CSprite*)pData);
            
            break;
        }
        case AICOMMAND_MOVEIN:
        {
            CAmbulance* pOwner = (CAmbulance*)mpSprite;
            CSprite* pEn = (CSprite*)pData;
            
            if(pEn->GetTeamID() != pOwner->GetTeamID()) break;
            
            if(mpMovinAttack == pEn)
            {
                mpMovinAttack = NULL; //도와주로 도착하였다면.
                mvAI->StopMove();
            }
            else if(mpMainTower == pEn)
                mpMainIn = true;
            
            if(pEn->GetState() == SPRITE_RUN)
            {
                long now = GetGGTime();
                float fEneryStatus = pOwner->GetCurrentDefendPowerEn();
                if(now > mInNextTime &&  fEneryStatus > 10.f)
                {
                    
                    float fX = pEn->GetMaxDefendPower() - pEn->GetMaxDefendPower() / 10.f;
                    if(pEn->GetCurrentDefendPower() < fX)
                    {
                        pEn->SetNeedExecuteCmd(pEn->GetNeedExecuteCmd() | SGL_NEED_EXECUTE_IN_DMAINTOWER);
                        mInNextTime = GetGGTime() + 1000; //1초 후에 적용해준다.
                        pOwner->SetCurrentDefendPowerEn(fEneryStatus - 10.f);
                    }
                }
            }
            break;
        }
        case AICOMMAND_MOVEOUT:
        {
            CSprite* pEn = (CSprite*)pData;
            long now = GetGGTime();
            //mpMovinAttack 은 도와주어야 할 같은틈의 탱크
            //now > mInNextTime 도와주는데 딴데로 이동하지는 말자.
            if(
               !mpMovinAttack &&
               now > mOutNextTime &&
               now > mInNextTime  &&
               pEn->GetState() == SPRITE_RUN &&
               mMovingByUser == false &&
               (!mvAI->isMoving() && !mvAI->isTurning()))
            {
                
                
                if(pEn->GetTeamID() == mpSprite->GetTeamID() )
                {
                    int nModelID = pEn->GetModelID();
                    
                    if( nModelID != ACTORID_BLZ_MAINTOWER)
                    {
                        float fM = pEn->GetMaxDefendPower();
                        if(pEn->GetCurrentDefendPower() < fM - fM / 4.f)
                        {
                            mOutNextTime = GetGGTime() + 2000; //2초 후에 다시 검사한다.
                            mpMovinAttack = pEn;

                            if(mpSprite->GetNetworkTeamType() == NETWORK_MYSIDE)
                            {
                                SPoint ptTo;
                                mpMovinAttack->GetPosition(&ptTo);
                                mvAI->SetSimpleMove(true);
                                mvAI->CmdMoveTo(&ptTo,false);
                            }

                        }
                    }
                    else
                    {
                        if(mpMainTower == pEn)
                            mpMainIn = false;
                        
                        if(mpMainTower == NULL || (pEn->GetColidedDistance() < mfMainTowerFar && mpMainTower != pEn))
                        {
                            mfMainTowerFar = pEn->GetColidedDistance();
                            mpMainTower = pEn;
                        }
                    }
                }
            }
            break;
        }
        case AICOMMAND_MOVE_SIMPLE_POS:
        {
            mvAI->SetSimpleMove(true);
            mvAI->CmdMoveTo((SPoint*)pData,false);
            mMovingByUser = true;
            mpMovinAttack = NULL;
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
        case AICOMMAND_ATTACKEDBYEMSPRITE:
            if( pData && mpSprite->GetNetworkTeamType() == NETWORK_MYSIDE &&
               (!mvAI->isMoving() && !mvAI->isTurning()) &&
               mpMainTower &&
               mpMovinAttack == NULL &&
               mMovingByUser == false &&
               mpMainIn == false) //타워쪽으로 이동해준다.
            {
                long now = GetGGTime();
                if(now > mInNextTime)
                {
                    SPoint ptTo;
                    mpMainTower->GetPosition(&ptTo);
                    ptTo.x += 5.f;
                    mvAI->SetSimpleMove(true);
                    mvAI->CmdMoveTo(&ptTo,false);
                }
            }
            break;
        case AICOMMAND_MAINKING:
        {
            mpSprite->SetType(ACTORTYPE_TYPE_GHOST);
        }
            break;
        case AICOMMAND_COMPLETEDMAKE:
        {
            mInNextTime = GetGGTime() + 1000;
            PROPERTY_TANK* pTank = mpSprite->GetPropertyTank();
            mpSprite->SetType(pTank->nType);
        }
            break;
        default:
//            CStrategyAI::Command(nCommandID,pData);
            break;
    }
}