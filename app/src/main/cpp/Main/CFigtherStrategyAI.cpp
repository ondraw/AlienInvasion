//
//  CFigtherStrategyAI.cpp
//  SongGL
//
//  Created by itsme on 2013. 12. 18..
//
//
#include "CSprite.h"
#include "CFigtherStrategyAI.h"
#include "AAICore.h"
#include "CProperty.h"
#include "CMutiplyProtocol.h"
#include "CHWorld.h"
#include "CSGLCore.h"

CFigtherStrategyAI::CFigtherStrategyAI(CSprite *pSprite,AAI* pMoveAI,int nPoCount,bool bIsCun)
{
    mpSprite = pSprite;
    mnPoCnt = nPoCount;
    mpMoveAI = pMoveAI;
    mpMovinAttack = new CSprite*[mnPoCnt];
    mfMovinAttackDistance = new float[mnPoCnt];
    for (int i = 0; i < mnPoCnt; i++)
    {
        mpMovinAttack[i] = NULL;
        mfMovinAttackDistance[i] = pSprite->GetDefendRadianBounds();
    }
    if(bIsCun)
        mfGunDefendRadianBoundse = pSprite->GetDefendRadianBounds() / 2.0f;
    else
        mfGunDefendRadianBoundse = 0.0f;
    mpEnemyGunSprite = NULL;
}


CFigtherStrategyAI::~CFigtherStrategyAI()
{
    if(mfMovinAttackDistance)
        delete[] mfMovinAttackDistance;
    mfMovinAttackDistance = NULL;
    if(mpMovinAttack)
        delete[] mpMovinAttack;
    mpMovinAttack = NULL;
}

void CFigtherStrategyAI::RenderBeginCore(int nTime)
{
    CSprite* orgSP;
    CSprite* orgGunSP = mpEnemyGunSprite;
    
    for (int i = 0; i < mnPoCnt; i++)
    {
        orgSP = mpMovinAttack[i];
        if(orgSP && orgSP->GetState() != SPRITE_RUN)
        {
            mpMovinAttack[i] = NULL;
            mfMovinAttackDistance[i] = mpSprite->GetDefendRadianBounds();
        }
    }
    
    
    if(orgGunSP && orgGunSP->GetState() != SPRITE_RUN)
        mpEnemyGunSprite = NULL;
}


CSprite* CFigtherStrategyAI::GetAttackTo()
{
    CSprite* orgSP;
    for (int i = 0; i < mnPoCnt; i++)
    {
        orgSP = mpMovinAttack[i];
        if(orgSP && orgSP->GetState() == SPRITE_RUN) return orgSP;
    }
    return NULL;
}

void CFigtherStrategyAI::Command(int nCommandID,void *pData)
{
    switch (nCommandID)
    {
        case AICOMMAND_COMPACTTOSPRITE:
        {
            break;
        }
        case AICOMMAND_MOVEIN: //사정거리 안쪽으로 이동하면 미사일을 쏜다.
        {
            int nEnGunID = 0;
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
                        nEnGunID = orgGunSP->GetID();
                        
//                        //상대방에 데이터를 보내어 기본적으로 동기화가 되게 해준다.
//                        if(mpSprite->GetNetworkTeamType() == NETWORK_MYSIDE)
//                            mpSprite->GetWorld()->GetMutiplyProtocol()->SendSyncMoveInGunAI(mpSprite->GetID(),orgGunSP->GetID());
                    }
                }
            }
            
            
            CSprite* orgSP = NULL;
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
                
//                if(mpSprite->GetNetworkTeamType() == NETWORK_MYSIDE)
//                {
//                    mpSprite->GetWorld()->GetMutiplyProtocol()->SendSyncMoveInAI(mpSprite->GetID(),nNulIndex,orgSP->GetID(),mfMovinAttackDistance[nNulIndex]);
//                }
            }
            
            if(mpMoveAI->IsMoveTo() == false && mpSprite->GetNetworkTeamType() == NETWORK_MYSIDE)
            {
                mpMoveAI->SetMoveTo((CSprite*)pData);
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
                    break;
                }
            }
            break;
        }
        case AICOMMAND_HITTHEMARK: //명중하였다. (공격 카운터를 주어서 공격의 정확도를 높혀보자)
            break;
        case AICOMMAND_BOMBED: //내가 쏜 폭탄이 터졌을때의 역할을 한다.
            break;
        case AICOMMAND_ATTACKEDBYEMSPRITE: //적군한테 공격당하였다면 그넘한테 이동하여 공격한다.
            if(pData && mpMoveAI->IsMoveTo() == false && mpSprite->GetNetworkTeamType() == NETWORK_MYSIDE)
            {
                mpMoveAI->SetMoveTo((CSprite*)pData);
            }
            break;
        case AICOMMAND_MOVE_SIMPLE_POS:
            break;
        case AICOMMAND_MOVE_MULTIPLAY:
        {
//            SPoint* ptTo;
//            SGLEvent* pEvent = (SGLEvent*)pData;
//            ptTo = (SPoint*)pEvent->lParam;
//            mpMoveAI->SetMoveTo(CSprite *v)
        }
            break;
        case AICOMMAND_MOVE_FIGHTER_MULTIPLAY:
        {
            SGLEvent* pEvent = (SGLEvent*)pData;
            CSprite* pToSprite = (CSprite*)pEvent->lParam;
            if(pToSprite && pToSprite->GetState() == SPRITE_RUN)
                mpMoveAI->SetMoveTo(pToSprite);
            
        }
        default:
            break;
    }
}


void CFigtherStrategyAI::SetAttackTo_Multiplay(int nIndex,CSprite* pSprite,float fDistance)
{
    if(mpMovinAttack[nIndex] != pSprite)
    {
        mpMovinAttack[nIndex] = pSprite;
        mfMovinAttackDistance[nIndex] = fDistance;
    }
}

void CFigtherStrategyAI::SetAttackToByGun_Multiplay(CSprite* pSprite)
{
    if(mpEnemyGunSprite != pSprite)
    {
        mpEnemyGunSprite = pSprite;
    }
    
}

void CFigtherStrategyAI::ArchiveMultiplay(int nCmd,CArchive& ar,bool bWrite)
{
    int nToID = 0;
    if(nCmd == SUBCMD_OBJ_ALL_INFO)
    {
        if(bWrite)
        {
            ar << mnPoCnt;
            if(mnPoCnt)
            {
                for (int i = 0; i < mnPoCnt; i++)
                {
                    if(mpMovinAttack[i])
                        nToID =  mpMovinAttack[i]->GetID();
                    else
                        nToID = 0;
                    ar << nToID;
                }
            }
            
            nToID = 0;
            if(mpEnemyGunSprite)
                nToID = mpEnemyGunSprite->GetID();
            ar << nToID;
        }
        else
        {
            int nCnt;
            ar >> nCnt;
            for (int i = 0; i < nCnt; i++)
            {
                ar >> nToID;
                if(nToID)
                {
                    mpMovinAttack[i] = ((CHWorld*)mpSprite->GetWorld())->GetSGLCore()->FindSprite(nToID);
                    if( mpMovinAttack[i] &&  mpMovinAttack[i]->GetState() != SPRITE_RUN)
                        mpMovinAttack[i] = NULL;
                }
                else
                    mpMovinAttack[i] = NULL;
            }
            ar >> nToID;
            if(nToID)
            {
                mpEnemyGunSprite= ((CHWorld*)mpSprite->GetWorld())->GetSGLCore()->FindSprite(nToID);
                if( mpEnemyGunSprite &&  mpEnemyGunSprite->GetState() != SPRITE_RUN)
                    mpEnemyGunSprite = NULL;
            }
        }
        
    }
}