//
//  CStrategyBlzAI.cpp
//  SongGL
//
//  Created by 호학 송 on 13. 8. 1..
//
//

#include "CStrategyBlzAI.h"
#include "CSprite.h"
#include "CMutiplyProtocol.h"

CStrategyBlzAI::CStrategyBlzAI(CSprite *pSprite) : CStrategyAI(pSprite,NULL)
{

}

CStrategyBlzAI::~CStrategyBlzAI()
{
}

void CStrategyBlzAI::RenderBeginCore(int nTime)
{
    CSprite* orgSP = mpMovinAttack;
    if(orgSP && orgSP->GetState() != SPRITE_RUN)
    {
        mpMovinAttack = NULL;
        mfMovinAttackDistance = mpSprite->GetDefendRadianBounds();
    }
    if(orgSP) //Attack이 우선한다.
    {
        //주인공 또한 자동으로 공격을 해야한다.
        CmdAttackTo(orgSP);
    }
}

void CStrategyBlzAI::SetAttackTo_Multiplay(int nIndex,CSprite* pSprite,float fDistance)
{
    if(mpMovinAttack != pSprite)
    {
        mpMovinAttack = pSprite;
        mfMovinAttackDistance = fDistance;
    }
}

void CStrategyBlzAI::SetAttackToByGun_Multiplay(CSprite* pSprite)
{
    if(mpEnemyGunSprite != pSprite)
    {
        mpEnemyGunSprite = pSprite;
    }
    
}

void CStrategyBlzAI::Command(int nCommandID,void *pData)
{
    switch (nCommandID)
    {
        case AICOMMAND_COMPACTTOSPRITE: //객체랑 충돌을 하였다.
        {
            break;
        }
        case AICOMMAND_MOVEIN: //사정거리 안쪽으로 이동하면 미사일을 쏜다.
        {
            
            if(pData == NULL) break;
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
                    mpMovinAttack = orgSP;
                    
//                    //                        //상대방에 데이터를 보내어 동기화를 하지 않아도. 거리순으로 소팅되어 있기 때문에 객체들이 알맞게 위치해있으면 동기화가 자동으로 된다.
//                    //                        //상대방에 데이터를 보내어 기본적으로 동기화가 되게 해준다.
//                    if(mpSprite->GetNetworkTeamType() == NETWORK_MYSIDE)
//                        mpSprite->GetWorld()->GetMutiplyProtocol()->SendSyncMoveInAI(mpSprite->GetID(),-1,orgSP->GetID(),mfMovinAttackDistance);
                }
            }
        
            break;
        }
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
        case AICOMMAND_HITTHEMARK:
            break;
            
        case AICOMMAND_BOMBED:
            break;
            
            
        case AICOMMAND_ATTACKEDBYEMSPRITE: //누군가가 공격하였다 하여도 무시한다. 사거리 안쪽에 것만 공격한다.
        {
            if(pData == NULL) break;
            
            CSprite* inSP = (CSprite*)pData;
            CSprite* orgSP = mpMovinAttack;
            if(inSP == NULL || (inSP->GetType() == ACTORTYPE_FIGHTER && !mpSprite->IsCurrentBombMissileType()))  break; //미사일타입일때만 비행기를 공격할 수가 있다.
            
            if(orgSP == NULL)
            {
                SPoint ptNow,ptEnNow;
                mpSprite->GetPosition(&ptNow);
                inSP->GetPosition(&ptEnNow);
                mfMovinAttackDistance = sglSquareDistanceSPointSPointf(&ptNow, &ptEnNow);
                mpMovinAttack = inSP;
            }
        }
            break;
        default:
            break;
    }
    
}
