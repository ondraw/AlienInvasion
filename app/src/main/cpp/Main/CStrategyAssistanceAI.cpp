//
//  CStrategyExAI.cpp
//  SongGL
//
//  Created by itsme on 2014. 5. 12..
//
//
#include "CSprite.h"
#include "CStrategyAssistanceAI.h"
#include "AAICore.h"
#include "CMoveAI.h"
#include "CHWorld.h"
#include "CSglTerrian.h"
#include "CMutiplyProtocol.h"

CStrategyAssistanceAI::CStrategyAssistanceAI(CSprite *pSprite,AAI* pMoveAI,int nPoCount,bool bIsCun) : CFigtherStrategyAI(pSprite,pMoveAI,nPoCount,bIsCun)
{
    mInNextTime = 0;
    mCheckNextTime = 0;
    mpMainTower = 0;
    mMovingToMain = false;
    mCheckNextTime2 = 0;
 
    mType = STRATEGY_NONE;
    mbNeedStop = false;
}

CStrategyAssistanceAI::~CStrategyAssistanceAI()
{
}


CSprite* CStrategyAssistanceAI::GetAttackTo()
{
    CSprite* orgSP;
    for (int i = 0; i < mnPoCnt; i++)
    {
        orgSP = mpMovinAttack[i];
        if(orgSP && orgSP->GetState() == SPRITE_RUN) return orgSP;
        if(mpSprite->GetModelID() != ACTORID_MISSILE) break;
    }
    return NULL;
}

bool CStrategyAssistanceAI::SetMakePosition(float fRadius,int nIndex,SPoint* ptCenter,SPoint* ptOutPoint,int nDepth)
{
    SPoint ptOut;
    //    float fRadius = mpSprite->GetRadius() * 4.f;
    float f[3];
    float matrix[16];
    
    f[0] = fRadius;
    f[1] = 0;
    f[2] = 0;
    
    if(nIndex > 8)    nIndex = 0;
    
    sglLoadIdentityf(matrix);
    sglRotateRzRyRxMatrixf(matrix,
                           0,
                           40 * nIndex,
                           0);
    
    
    //좌표를 이동한다.
    sglMultMatrixVectorf(f,matrix,f);
    
    memcpy(&ptOut, ptCenter, sizeof(SPoint));
    
    ptOut.x += f[0];
    ptOut.y = 0;
    ptOut.z += f[2];
    
    if(nDepth > 9) return false;
    
    if(CSglTerrian::IsObstacle(&ptOut))
    {
        return SetMakePosition(fRadius,nIndex + 1,ptCenter,ptOutPoint,nDepth + 1);
    }
    
    if(((CHWorld*)mpSprite->GetWorld())->GetPositionY(&ptOut) != E_SUCCESS)
    {
        return SetMakePosition(fRadius,nIndex + 1,ptCenter,ptOutPoint,nDepth + 1);
    }
    
    if(ptOutPoint) memcpy(ptOutPoint, &ptOut, sizeof(SPoint));
    return true;
}

void CStrategyAssistanceAI::CheckActorAssist(SPoint* ptNow2)
{
    SPoint ptNow;
    CSprite* pActor = mpSprite->GetWorld()->GetActor();
    if(pActor->GetState() == SPRITE_RUN)
    {
        pActor->GetPosition(&ptNow);
        //주인공 주변에 없다면.
        if((ptNow2->x - ptNow.x) * (ptNow2->x - ptNow.x) + (ptNow2->z - ptNow.z) * (ptNow2->z - ptNow.z) > pActor->GetDefendRadianBounds() / 10.f)
        {
            
            SPoint ptTo;
            int nRand = ((long)this + rand()) % 8;
            //---------
            //이동한다.
            //---------
            if(SetMakePosition(30,nRand,&ptNow,&ptTo))
            {
                ((CMoveAI*)mpMoveAI)->SetSimpleMove(true);
                ((CMoveAI*)mpMoveAI)->CmdMoveTo(&ptTo,true);
            }
            
        }
    }
}

void CStrategyAssistanceAI::RenderBeginCore(int nTime)
{
    if(mpSprite->GetType() == ACTORTYPE_TYPE_GHOST) return;
    unsigned long lNow = GetGGTime();
    
    CMoveAI* pMoveAI = (CMoveAI*)mpMoveAI;
    if(!pMoveAI->isMoving() && !pMoveAI->isTurning())
        mMovingByUser = false;
    
    
    if(mbNeedStop && lNow > mStopTime)
    {
         ((CMoveAI*)mpMoveAI)->StopMove();
        mbNeedStop = false;
    }
    
    CSprite* AttackSprite = NULL;
    CSprite* orgSP;
    CSprite* orgGunSP = mpEnemyGunSprite;
    for (int i = 0; i < mnPoCnt; i++)
    {
        orgSP = mpMovinAttack[i];
        if(orgSP && orgSP->GetState() != SPRITE_RUN)
        {
            mpMovinAttack[i] = NULL;
            mfMovinAttackDistance[i] = mpSprite->GetDefendRadianBounds();
            if(i == 0) mType = STRATEGY_NONE;
            
        }
        else
            AttackSprite = orgSP;
    }
    if(orgGunSP && orgGunSP->GetState() != SPRITE_RUN)
        mpEnemyGunSprite = NULL;
    
    
    
    if(AttackSprite)
        CmdAttackTo(AttackSprite);

    if(mpMainTower && mpMainTower->GetState() != SPRITE_RUN)
    {
        mpMainTower = NULL;
    }

    
    
    if(mMovingByUser == false && lNow > mInNextTime && mpSprite->GetNetworkTeamType() == NETWORK_MYSIDE)
    {
        //---------------------------------------------------
        mInNextTime = lNow + 8000l; //8초에 한번씩 체크를 한다.
        //---------------------------------------------------
        
        SPoint ptNow,ptNow2;
        
        mpSprite->GetPosition(&ptNow2);
        
        if(mpMainTower == NULL)
            mpMainTower = ((CHWorld*)mpSprite->GetWorld())->GetMainTowerNearestInThread(&ptNow2,mpSprite->GetTeamID());
        else
        {
            if(lNow > mCheckNextTime2)
            {
                mCheckNextTime2 = lNow + 30000l; //30초.
                mpMainTower = ((CHWorld*)mpSprite->GetWorld())->GetMainTowerNearestInThread(&ptNow2,mpSprite->GetTeamID());
            }
        }
        
        bool bNotEnergy = mpSprite->GetCurrentDefendPower() < mpSprite->GetMaxDefendPower() / 2.0f;
        mpMainTower->GetPosition(&ptNow);
        if((ptNow2.x - ptNow.x) * (ptNow2.x - ptNow.x) + (ptNow2.z - ptNow.z) * (ptNow2.z - ptNow.z) < mpMainTower->GetDefendRadianBounds())
        {
            ((CMoveAI*)mpMoveAI)->StopMove();
            mMovingToMain = false;
            
            //에너지가 어느정도 찼으면 검사한다.
            if(mpSprite->GetCurrentDefendPower() > (mpSprite->GetMaxDefendPower() - mpSprite->GetMaxDefendPower() / 5.0f))
                CheckActorAssist(&ptNow2);
        }
        else if(bNotEnergy && (mMovingToMain == false || (!pMoveAI->isMoving() && !pMoveAI->isTurning()))) //범위밖에 있고 에너지가 반정도 있으면
        {
            mMovingToMain = true;
            pMoveAI->SetSimpleMove(true);
            ptNow.x += 10.f;
            pMoveAI->CmdMoveTo(&ptNow,true);
        }
        else if(!bNotEnergy && (!pMoveAI->isMoving() && !pMoveAI->isTurning()) && mMovingByUser == false)
        {
            CheckActorAssist(&ptNow2);
        }
        
    }
}

//단순하게 조절해서 맞추는 것이 아니라 멀면, 가까이 이동하여 공격해야한다.
void CStrategyAssistanceAI::CmdAttackTo(CSprite *pSprite)
{
    if(mpSprite->CanFireMissile() == false || mpSprite->isMovingAutoHeader())
        return;
    mpSprite->AimAutoToEnemy(pSprite);
}



void CStrategyAssistanceAI::Command(int nCommandID,void *pData)
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
            if(pData && mpMainTower == pData)
            {
                ((CMoveAI*)mpMoveAI)->StopMove();
                mMovingToMain = false;
            }
            //객체랑 충돌을 하였다.
            ((CMoveAI*)mpMoveAI)->AvoidCompactSprite((CSprite*)pData);
            break;

        }
        case AICOMMAND_MOVEIN: //사정거리 안쪽으로 이동하면 미사일을 쏜다.
        {
            CSprite* inSP = (CSprite*)pData;
            CSprite* orgGunSP = mpEnemyGunSprite;
            int nModelID = mpSprite->GetModelID();
            
            //xtank는 비행기를 공격하지 못한다.
            if(nModelID == ACTORID_XTANK && inSP->GetType() == ACTORTYPE_FIGHTER)
                break;

            if(mType == STRATEGY_ATTACKED_MOVGIN) //누군가 공격해서 이동중에 적이 사정거리 안쪽으로 드러오면 멈춘다.
            {
                //너무 갑작이 멈추지 말고 좀 시간을 딜레이 한후에 멈추자.
                mbNeedStop = true;
                mStopTime = GetGGTime() + 2000l;
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
//                        //상대방에 데이터를 보내어 기본적으로 동기화가 되게 해준다.
//                        if(mpSprite->GetNetworkTeamType() == NETWORK_MYSIDE)
//                            mpSprite->GetWorld()->GetMutiplyProtocol()->SendSyncMoveInGunAI(mpSprite->GetID(),orgGunSP->GetID());
                    }
                }
            }
            
            
            CSprite* orgSP = NULL;
            if(nModelID == ACTORID_MISSILE)
            {
                int nNulIndex = -1;
                for (int i = mnPoCnt - 1; i >= 0; i--)
                {
                    orgSP = mpMovinAttack[i];
                    if(orgSP == inSP) //이미존재 한다면 스킵
                        return;
                    else if(orgSP == NULL)
                        nNulIndex = i;
                }
                
                if(nNulIndex != -1)
                {
                    orgSP = inSP;
                    mpMovinAttack[nNulIndex] = orgSP;
//                    if(mpSprite->GetNetworkTeamType() == NETWORK_MYSIDE)
//                        mpSprite->GetWorld()->GetMutiplyProtocol()->SendSyncMoveInAI(mpSprite->GetID(),nNulIndex,orgSP->GetID(),mfMovinAttackDistance[nNulIndex]);
                    
                    mType = STRATEGY_ATTACK;
                }
            }
            else //XTank는 포가 하나이다 그래서
            {
                if(mnPoCnt > 0)
                {
                    orgSP = mpMovinAttack[0];
                    //드러온넘이 존재 하면 스킵
                    if(orgSP == inSP) //이미 가지고 있다 하더라도, 거리는 계속 갱신해주어야 한다.
                        mfMovinAttackDistance[0] = inSP->GetColidedDistance();
                    else if (orgSP == NULL)
                    {
                        orgSP = inSP;
                        mfMovinAttackDistance[0] = inSP->GetColidedDistance();
                    }
                    //가까운것이 우선
                    else if(orgSP != inSP && inSP->GetColidedDistance() < mfMovinAttackDistance[0])
                    {
                        orgSP = inSP;
                        mfMovinAttackDistance[0] = inSP->GetColidedDistance();
                    }
                    if(orgSP != mpMovinAttack[0])
                    {
                        mType = STRATEGY_ATTACK;
                        mpMovinAttack[0] = orgSP;
//                        if(mpSprite->GetNetworkTeamType() == NETWORK_MYSIDE)
//                            mpSprite->GetWorld()->GetMutiplyProtocol()->SendSyncMoveInAI(mpSprite->GetID(),0,orgSP->GetID(),mfMovinAttackDistance[0]);
                        break;
                    }
                }
            }
            break;
        }
        case AICOMMAND_MOVEOUT:
        {
            for (int i = 0; i < mnPoCnt; i++)
            {
                CSprite* orgSP = mpMovinAttack[i];
                if(orgSP == pData)
                {
                    mfMovinAttackDistance[i] = orgSP->GetDefendRadianBounds();
                    mpMovinAttack[i] = NULL;
                    if(i == 0) mType = STRATEGY_NONE;
                    break;
                }
                
                //미사일은 mpMovinAttack를 사용하고 미사일아니면 0인덱스만 사용한다.
                if(mpSprite->GetModelID() != ACTORID_MISSILE)
                    break;
            }
            break;
        }
        case AICOMMAND_HITTHEMARK: //명중하였다. (공격 카운터를 주어서 공격의 정확도를 높혀보자)
            break;
        case AICOMMAND_BOMBED: //내가 쏜 폭탄이 터졌을때의 역할을 한다.
            break;
        case AICOMMAND_ATTACKEDBYEMSPRITE: 
        {
            if(pData && mType != STRATEGY_ATTACKED_MOVGIN && //누군가 공격해서 이동중일때 누가 또 공격하면 무시한다.
               (mpSprite->GetAttackTo() == NULL || mType == STRATEGY_RECON) &&
               (mpSprite->GetCurrentDefendPower() / mpSprite->GetMaxDefendPower() > 0.3f) &&
               mpSprite->GetNetworkTeamType() == NETWORK_MYSIDE)
            {
                if(mpSprite->GetCurrentDefendPower() / mpSprite->GetMaxDefendPower() < 0.3f) break;
                
                CMoveAI* pMoveAI = (CMoveAI*)mpMoveAI;
                SPoint ptMove;
                CSprite* pSP = (CSprite*)pData;
                pSP->GetPosition(&ptMove);
                
                
                pMoveAI->SetSimpleMove(true);
                pMoveAI->CmdMoveTo(&ptMove,true);
                
                //누가 공격해서 이동중이다.
                mType = STRATEGY_ATTACKED_MOVGIN;
            }
            break;
        }
        case AICOMMAND_MOVE_SIMPLE_POS:
        {
            CMoveAI* pMoveAI = (CMoveAI*)mpMoveAI;
            pMoveAI->SetSimpleMove(true);
            pMoveAI->CmdMoveTo((SPoint*)pData,false);
            mMovingByUser = true;
        }
            break;
        case AICOMMAND_MOVE_MULTIPLAY:
        {
            CMoveAI* pMoveAI = (CMoveAI*)mpMoveAI;
            SPoint* ptTo;
            SGLEvent* pEvent = (SGLEvent*)pData;
            ptTo = (SPoint*)pEvent->lParam;
            bool bFront = (bool)pEvent->lParam3;
            bool bSmpleMode = (bool)pEvent->lParam2;
            if(bSmpleMode)
                pMoveAI->SetSimpleMove(true);
            else
                pMoveAI->SetSimpleMove(false);
            pMoveAI->CmdMoveTo(ptTo,bFront);
        }
            break;
        case AICOMMAND_MAINKING:
        {
            mpSprite->SetType(ACTORTYPE_TYPE_GHOST);
        }
            break;
        case AICOMMAND_COMPLETEDMAKE:
        {
            //---------------------------------------------------
            mInNextTime = GetGGTime() + 1000l; //8초에후에 이동같은 메세지를 보내야 상대의 느린단말기는 Sprite를 Add하기전에 무부를 보내기 때문에 받지를 못한다.
            //---------------------------------------------------
            PROPERTY_TANK* pTank = mpSprite->GetPropertyTank();
            mpSprite->SetType(pTank->nType);
        }
            break;
        default:
            break;
    }
}