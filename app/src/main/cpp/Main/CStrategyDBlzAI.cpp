//
//  CStrategyDBlzAI.cpp
//  SongGL
//
//  Created by 호학 송 on 13. 10. 28..
//
//
#include <stdlib.h>
#include "CStrategyDBlzAI.h"
#include "CDMainTower.h"
#include "sGLUtils.h"
#include "CAniRuntimeBubble.h"
#include "CWorld.h"
#include "CUserInfo.h"
#include "CSGLCore.h"
#include "IAction.h"
CStrategyDBlzAI::CStrategyDBlzAI(CSprite *pSprite)
{
    mpOwner = (CDMainTower*)pSprite;
    mNextTime = 0;
    mNextMakeTankTime = 0;
    }

CStrategyDBlzAI::~CStrategyDBlzAI()
{
    
}

void CStrategyDBlzAI::RenderBeginCore(int nTime)
{
    long lNow = GetGGTime();
    if(!mpOwner->IsMaking())
    {
        if(lNow > mNextMakeTankTime && mpOwner->GetNetworkTeamType() == NETWORK_MYSIDE)
        {
            CScenario::SendMessage(SGL_MAKING_ASSISTOBJECTS,0,0,0,(long)mpOwner);
            mNextMakeTankTime = lNow + 5000; //3초에 한번씩 체크를 한다.
        }
    }
}

void CStrategyDBlzAI::Command(AICORE_AICOMMANDD nCommandID,void *pData)
{
    CSprite* pEn = (CSprite*)pData;
    if(pEn == NULL || pEn->GetTeamID() != mpOwner->GetTeamID()) return ;
    
    if(mpOwner->IsMaking()) return ;
    
    switch (nCommandID)
    {
        case AICOMMAND_MOVEIN: //구역안쪽으로 넘어오면 에너지를 필해준다.
        {
            long now = GetGGTime();
            float fEneryStatus = mpOwner->GetCurrentDefendPowerEn();
            if(now > mNextTime &&  fEneryStatus > 10.f)
            {
            
                float fX = pEn->GetMaxDefendPower() - pEn->GetMaxDefendPower()/10.f;
                if(pEn->GetCurrentDefendPower() < fX)
                {
                    pEn->SetNeedExecuteCmd(pEn->GetNeedExecuteCmd() | SGL_NEED_EXECUTE_IN_DMAINTOWER);
                    mNextTime = GetGGTime() + 1000; //1초 후에 적용해준다.
                    mpOwner->SetCurrentDefendPowerEn(fEneryStatus - 10.f);
                }
            }
            int nCnt = 0;
            CAniRuntimeBubble* pBubble = mpOwner->GetAniRuntimeBubble();
            if(pBubble) nCnt = pBubble->GetUpgradeCount();
            pEn->SetNeedExecuteCmd((pEn->GetNeedExecuteCmd() | SGL_NEED_EXECUTE_CHGOLD_IN_DMAINTOWER) + nCnt);
            break;
        }
        default:
            break;
    }
}

