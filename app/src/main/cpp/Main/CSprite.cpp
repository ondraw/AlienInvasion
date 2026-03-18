/*
 *  CActor.cpp
 *  SongGL
 *
 *  Created by 호학 송 on 10. 11. 17..
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#include <strings.h>
#include <math.h>
#include <assert.h>
#include "sGL.h"
#include "CSprite.h"
#include "AMesh.h"
#include "sGLTrasform.h"
#include "IAction.h"
#include "CSGLCore.h"
#include "CHWorld.h"
#include "AAICore.h"
#include "CSpriteStatus.h"
#include "CShadowParticle.h"
#include "sGLText.h"
#include "sGLUtils.h"
#include "CMoveAI.h"
#include "CCircleUpgradeMark.h"
#include "CAniRuntimeBubble.h"
#include "CMutiplyProtocol.h"

CSprite::CSprite(unsigned int nID,unsigned char cTeamID,int nModelID,IAction *pAction,IHWorld* pWorld,PROPERTY_TANK* pProperty,NETWORK_TEAMTYPE NetworkTeamType)
{
	memset(&mvViewDirection,0,sizeof(SVector));
	memset(&mPosition,0,sizeof(SPoint));

    m_bHodeFire = false;
    
    mpAICore = NULL;
	
	mpCamera = NULL;
	mnModelID = nModelID;
	mnCameraType = CAMERT_NORMAL; 

	mpAction = pAction;    
    mState = SPRITE_RUN;
    
    mnID = nID;
    mcTeamID = cTeamID;
    m_bIsVisibleRender = true;
    
    m_nCurrentPerTime = MAXFirePerSec;
    m_fCurrentDefendPower = 0.0f;
    
    m_fIntervalToActor = 99999999.f;
    mpWorld = pWorld;

    mpShadowParticle = NULL;
    
    mfCameraUpDownVector = 0.0f;
    mfCameraUpDownPos = 32.f;     //NormalViewMaxY / 2.0f;
    
    mfCameraTurnVector = 0.0f;
    mfCameraTurnPos = 0.0f;
    
    mfDefendRadianBounds= 0.f;
    
    mpRuntimeUpgradeList = NULL;

    
    mNetworkTeamType = NetworkTeamType;
    
    if(pProperty)
    {
        mProperty = *pProperty;
        ResetPower();
        //폭탄정보를 가져온다.
        PROPERTY_BOMB::GetPropertyBomb(mProperty.nDefaultBombID, mBombProperty);
        
        mfDefendRadianBounds = mProperty.fDefendRadianBounds;
    }

    mpStatusBar = NULL;
    //모든 스프라이트가 보이게 하려면
//    mpStatusBar = new CSpriteStatus(this);
//    mpStatusBar->Initialize(NULL, NULL);
    //메인타워와 주인공만 보이게 하자. (주인공은..  SetActor할때 생성한다.)
    if(nModelID == ACTORID_BLZ_EMAIN || (pProperty && pProperty->nType == 3))
    {
        mpStatusBar = new CSpriteStatus(this);
        mpStatusBar->Initialize(NULL, NULL);
    }
        
    
    mnReliveEffect = 0;
    mfColidedDistance = 99999999.f;
    
    mnType = 0;
    mpCircleUpgradeMark = NULL;
    
    
    mcAliveCount = 0;

}


CSprite::~CSprite()
{	
    if (mpAICore) 
    {
        delete mpAICore;
        mpAICore = 0;
    }
    if(mpStatusBar)
    {
        delete mpStatusBar;
        mpStatusBar = NULL;
    }
    if(mpRuntimeUpgradeList)
    {
        int nCnt = (int)mpRuntimeUpgradeList->size();
        for (int i = 0; i < nCnt; i++)
        {
            delete mpRuntimeUpgradeList->at(i);
        }
        delete mpRuntimeUpgradeList;
        mpRuntimeUpgradeList = NULL;
    }
}


int  CSprite::GetAliveCount()
{
    if (mpWorld->GetActor() == this)
        return mcAliveCount + mpWorld->GetUserInfo()->GetCntByIDWithItem(ID_ITEM_FILL_POWER);
    return mcAliveCount;
}

bool CSprite::CheckAlive()
{
    bool bIs = false;
    CHWorld* pWorld = (CHWorld*)mpWorld;
    CSprite* pActor = pWorld->GetActor();
    if(pActor == this) //주인공일 경우
    {
        if(mcAliveCount > 0)
        {
            mcAliveCount -= 1;
            bIs = true;
        }
        else if(pWorld->GetUserInfo()->CanAlive())
        {
            int nItemCnt;
            pWorld->GetUserInfo()->MinusFillItem(nItemCnt);
            bIs = true;
        }
        if(bIs)
        {
            //-------------------------------------
            //파워를 다시 채워준다.
            //-------------------------------------
            SetState(SPRITE_RUN);
            ResetPower();
            StartReliveEffect(); //깜빡 깜빡이는 효과를 준다.
            pWorld->GetSGLCore()->PlaySystemSound(SOUND_ID_Relive);
            CScenario::SendMessage(SGL_REALIVE_RESETSLOT);
        }
    }
    else //상대팀
    {
        if(mcAliveCount > 0)
        {
            mcAliveCount -= 1;
            bIs = true;
            //-------------------------------------
            //파워를 다시 채워준다.
            //-------------------------------------
            SetState(SPRITE_RUN);
            ResetPower();
            StartReliveEffect(); //깜빡 깜빡이는 효과를 준다.
        }
    }
    
    
    return bIs;
}


void CSprite::DeleteAICore()
{
    if(mpAICore)
    {
        delete mpAICore;
        mpAICore = NULL;
    }
}


void CSprite::InitRuntimeUpgrade()
{
    RUNTIME_UPGRADES property;
    if(RUNTIME_UPGRADES::GetPropertyRunUpgradesItem(GetModelID(), property))
    {
        mpRuntimeUpgradeList = new vector<USERINFO*>;
        
        for (int i = 0; i < property.nCount; i++)
        {
            USERINFO *userinfo = new USERINFO;
            RUNTIME_UPGRADE* pUp = new RUNTIME_UPGRADE;
            RUNTIME_UPGRADE::GetPropertyRunUpgradeItem(property.nUpgradeIDs[i], *pUp);
            userinfo->nID = pUp->nID;
            userinfo->nType = pUp->nTime;
            userinfo->nCnt = pUp->nGold;
            userinfo->pProp = pUp;
            mpRuntimeUpgradeList->push_back(userinfo);
        }
    }
}
void CSprite::SetAttackTo_Multiplay(int nIndex,CSprite* pSprite,float fDistance)
{
    if(mpAICore && mpAICore->GetStategyAI())
    {
        mpAICore->GetStategyAI()->SetAttackTo_Multiplay(nIndex,pSprite, fDistance);
    }
}

void CSprite::SetAttackToByGun_Multiplay(CSprite* pSprite)
{
    if(mpAICore && mpAICore->GetStategyAI())
    {
       mpAICore->GetStategyAI()->SetAttackToByGun_Multiplay(pSprite);
    }
}

float CSprite::GetDefendRadianBounds()
{
    return mfDefendRadianBounds;
}

bool CSprite::SetRuntimeUpgrade(int nID)
{
    CUserInfo* pUserInfo = GetWorld()->GetUserInfo();
    CHWorld* pHWorld = (CHWorld*)GetWorld();
    RUNTIME_UPGRADE *prop = 0;
    for (vector<USERINFO*>::iterator it = mpRuntimeUpgradeList->begin(); it != mpRuntimeUpgradeList->end(); it++)
    {
        if((*it)->nID == nID)
        {
            prop = (RUNTIME_UPGRADE*)(*it)->pProp;
            if(pUserInfo->GetGold() < prop->nGold || prop->nUpgradeCnt >= prop->nUpgradeMaxCnt) return false;
            prop->nUpgreadTime = 0;
            mpRuntimeUpgradeingList.push_back(*it);
            
            pUserInfo->SetGold(pUserInfo->GetGold() - prop->nGold);
            pHWorld->ResetDisplayTopGold();
            return true;
        }
    }
    return false;
}


void CSprite::RuntimeUpgrading(int nTime)
{
    CHWorld* pHWorld = (CHWorld*)GetWorld();
    RUNTIME_UPGRADE *prop = 0;
    int nFastBuild = 1;
    if(mpRuntimeUpgradeingList.size() == 0) return;
    if(pHWorld->IsFastBuild()) nFastBuild = 2;
    for (vector<USERINFO*>::iterator it = mpRuntimeUpgradeingList.begin(); it != mpRuntimeUpgradeingList.end();)
    {
        prop = (RUNTIME_UPGRADE*)(*it)->pProp;
        if(prop->nUpgreadTime > prop->nTime)
        {
            prop->nUpgradeCnt += 1;
            prop->nUpgreadTime = -1;
            
            pHWorld->OnCompletedUpdated(this,prop->nID);

            //OnCompletedUpdated(prop);
            CScenario::SendMessage(SGL_COMPLETED_RUNTIME_UPGRADE,(long)this,NULL,NULL,(long)prop);
            it = mpRuntimeUpgradeingList.erase(it);
            continue;
        }
        else
        {
            prop->nUpgreadTime += (nTime * nFastBuild);
            pHWorld->RenderClickedSpriteSlot(this);
        }
        it++;
    }
}



void CSprite::OnCompletedUpdated(RUNTIME_UPGRADE* prop,CArchive* pReadArc)
{

    int nID = GetID();
    if(prop->nID == RUNTIME_UPDATE_UPGRADE)
    {
        if(pReadArc == NULL)
        {
            float fCurrentRate = GetCurrentDefendPower() / mfMaxDefendPowerActor;
            ResetPower(true);
            SetCurrentDefendPower(fCurrentRate * mfMaxDefendPowerActor); //업그레이드가되어도 에너지의 비율을 맞추어주자.
            
            CHWorld* pWorld = (CHWorld*)GetWorld();
            if(!pWorld->GetMutiplyProtocol()->IsStop())
            {
                float fD = GetMaxDefendPower();
                float fA = GetMaxAttackPower();
                CArchive arc;
                arc << nID;
                arc << prop->nID;
                arc << prop->nUpgradeCnt;
                arc << fD;
                arc << fA;
                pWorld->GetMutiplyProtocol()->SendSyncCompletedRuntimeUpgrade(&arc);
            }
        }
        else
        {
            float fCurrentRate = GetCurrentDefendPower() / mfMaxDefendPowerActor;
            *pReadArc >> nID;
            *pReadArc >> prop->nID;
            *pReadArc >> prop->nUpgradeCnt;
            *pReadArc << mfMaxDefendPowerActor;
            *pReadArc << m_fMaxAttackPower;
            
            SetCurrentDefendPower(fCurrentRate * mfMaxDefendPowerActor); //업그레이드가되어도 에너지의 비율을 맞추어주자.
            delete pReadArc;
            
            //화면에 반영해준다.
            if(mpCircleUpgradeMark == NULL)
            {
                CSGLCore* pCore = ((CHWorld*)mpWorld)->GetSGLCore();
                mpCircleUpgradeMark = new CCircleUpgradeMark(mpWorld,this,10.f);
                mpCircleUpgradeMark->Initialize(NULL, NULL);
                mpCircleUpgradeMark->SetCircleCount(prop->nUpgradeCnt);
                pCore->AddParticle(mpCircleUpgradeMark);
            }
            else
            {
                mpCircleUpgradeMark->SetCircleCount(prop->nUpgradeCnt);
            }

        }
    }
    else if(prop->nID == RUNTIME_UPDATE_RNGUPGRADE)
    {
        if(pReadArc == NULL)
        {
            mfDefendRadianBounds = mProperty.fDefendRadianBounds + prop->nUpgradeCnt * 3.f / 10.f * mProperty.fDefendRadianBounds; //30/100 30퍼센트 멀리.
            string sDesc;
            CHWorld* pWorld = (CHWorld*)GetWorld();
            sDesc = SGLTextUnit::GetOnlyText(prop->sName);
            sDesc.append(" ");
            sDesc.append(SGLTextUnit::GetOnlyText("Completed Rng Upgrade"));
            pWorld->OnSelectedObjectShowDescBar(this,sDesc.c_str());
        
            if(!pWorld->GetMutiplyProtocol()->IsStop())
            {
                CArchive arc;
                arc << nID;
                arc << prop->nID;
                arc << prop->nUpgradeCnt;
                arc << mfDefendRadianBounds;
                pWorld->GetMutiplyProtocol()->SendSyncCompletedRuntimeUpgrade(&arc);
            }
        }
        else
        {
            *pReadArc >> nID;
            *pReadArc >> prop->nID;
            *pReadArc >> prop->nUpgradeCnt;
            *pReadArc >> mfDefendRadianBounds;
            
            delete pReadArc;
        }
    }
    else if(prop->nID == RUNTIME_UPDATE_REPAIRE)
    {
        if(pReadArc == NULL)
        {
            SetCurrentDefendPower(GetMaxDefendPower());
            string sDesc;
            CHWorld* pWorld = (CHWorld*)GetWorld();
            sDesc = SGLTextUnit::GetOnlyText(prop->sName);
            sDesc.append(" ");
            sDesc.append(SGLTextUnit::GetOnlyText("Completed Repare"));
            pWorld->OnSelectedObjectShowDescBar(this,sDesc.c_str());
            
            if(!pWorld->GetMutiplyProtocol()->IsStop())
            {
                CArchive arc;
                arc << nID;
                arc << prop->nID;
                arc << prop->nUpgradeCnt;
                pWorld->GetMutiplyProtocol()->SendSyncCompletedRuntimeUpgrade(&arc);
            }
        }
        else
        {
            *pReadArc >> nID;
            *pReadArc >> prop->nID;
            *pReadArc >> prop->nUpgradeCnt;
            SetCurrentDefendPower(GetMaxDefendPower());
            
            delete pReadArc;
        }
    }
    else if(prop->nID == RUNTIME_UPDATE_BUBBLE)
    {
        //-------------------------------------------------
        if(pReadArc == NULL)
        {
            CHWorld* pWorld = (CHWorld*)GetWorld();
            if(!pWorld->GetMutiplyProtocol()->IsStop())
            {
                CArchive arc;
                arc << nID;
                arc << prop->nID;
                arc << prop->nUpgradeCnt;
                pWorld->GetMutiplyProtocol()->SendSyncCompletedRuntimeUpgrade(&arc);
            }
        }
        else
        {
            *pReadArc >> nID;
            *pReadArc >> prop->nID;
            *pReadArc >> prop->nUpgradeCnt;
            
            delete pReadArc;
        }
        CAniRuntimeBubble* pBubble = GetAniRuntimeBubble();
        if(pBubble)
            pBubble->SetUpgradeCount(prop->nUpgradeCnt);
        else
        {
            float fR = GetRadius() * 0.035;
            SPoint pt2;
            GetPosition(&pt2);
            pBubble = new CAniRuntimeBubble(GetWorld(),this,0,prop->nUpgradeCnt);
            SetAniRuntimeBubble(pBubble);
            pBubble->Initialize(&pt2, NULL);
            pBubble->SetScale(fR, fR, fR);
            ((CHWorld*)GetWorld())->GetSGLCore()->AddParticle(pBubble);
        }
        //-------------------------------------------------
    }
}



void CSprite::SetAutoTurnAndMoveToTarget(SPoint* ptPathPos,SPoint* ptLastTarget)
{
    CMoveAI *mv = (CMoveAI*)mpAICore->GetMoveAI();
    if(mv)
        mv->SetAutoTurnAndMoveToTarget(ptPathPos,ptLastTarget);
}

void CSprite::SetRotationVelocity(float v)
{
    CMoveAI *mv = (CMoveAI*)mpAICore->GetMoveAI();
    mv->SetRotationVelocity(v);
}

void CSprite::SetToRotationAngle(float v)
{
    CMoveAI *mv = (CMoveAI*)mpAICore->GetMoveAI();
    mv->SetToRotationAngle(v);
}

float   CSprite::GetRotationVelocity()
{
    CMoveAI *mv = (CMoveAI*)mpAICore->GetMoveAI();
    return mv->GetRotationVelocity();
}

float   CSprite::GetToRotationAngle()
{
    CMoveAI *mv = (CMoveAI*)mpAICore->GetMoveAI();
    return mv->GetToRotationAngle();
}


void CSprite::SetMoveDirection(SVector* v)
{
    CMoveAI *mv = (CMoveAI*)mpAICore->GetMoveAI();
    mv->SetMoveDirection(v);
}
void CSprite::GetMoveDirection(SVector* outv)
{
    CMoveAI *mv = (CMoveAI*)mpAICore->GetMoveAI();
    if(mv) mv->GetMoveDirection(outv);
}

void CSprite::SetMoveVelocity(float v)
{
    if(mpAICore)
    {
        ((CMoveAI*)mpAICore->GetMoveAI())->SetMoveVelocity(v);
    }
}

float CSprite::GetMoveVelocity()
{
    CMoveAI *mv = (CMoveAI*)mpAICore->GetMoveAI();
    if(mv == 0) return 0.f;
    return mv->GetMoveVelocity();
}

bool CSprite::isMoving() 
{
    if(mpAICore == 0) return false;
    CMoveAI *mv = (CMoveAI*)mpAICore->GetMoveAI();
    if(mv == 0) return false;
    return mv->isMoving();
}

bool CSprite::isTurning() 
{ 
    if(mpAICore == 0) return false;
    CMoveAI *mv = (CMoveAI*)mpAICore->GetMoveAI();
    if(mv == 0) return false;
    return mv->isTurning();
}



int CSprite::LoadResoure(CModelMan* pModelMan)
{
	return E_SUCCESS;
}



int CSprite::Initialize(SPoint *pPosition,SVector *pvDirection)
{
    int nResult = 0;
	m_fCurrentDefendPower = GetMaxDefendPower();
	SetModelDirection(pvDirection);
	nResult = SetPosition(pPosition);
    return nResult;
}



void CSprite::SetVisibleRender(bool v) 
{
    if(mState == SPRITE_RUN)
    {
        m_bIsVisibleRender = v;
        if(mpShadowParticle)
            mpShadowParticle->SetVisibleRender(v);
        if(mpCircleUpgradeMark)
            mpCircleUpgradeMark->SetVisibleRender(v);
        
//        CAniRuntimeBubble* pB = GetAniRuntimeBubble();
//        if(pB)
//        {
//            pB->SetVisibleRender()
        
    }
    
        
}

void CSprite::SetState(SPRITE_STATE v) 
{ 
    mState = v;
    if(mpShadowParticle)
    {
        mpShadowParticle->SetState(v);
        if(v != SPRITE_RUN)
        {
            mpShadowParticle = NULL;
        }
    }
    
    if(mpCircleUpgradeMark)
    {
        mpCircleUpgradeMark->SetState(v);
        if(v != SPRITE_RUN)
        {
            mpCircleUpgradeMark = NULL;
        }
    }

    if(GetAniRuntimeBubble())
    {
        if(v != SPRITE_RUN)
            SetAniRuntimeBubble(NULL);
    }
}


void CSprite::RenderBeginCore(int nTime)
{
    m_nCurrentPerTime += nTime/2; //초당으로 계산해야하는데 쏘는 속도보다 올라가는 속도가 더빨라서 /2해주었다.
    if(m_nCurrentPerTime > GetMaxAttackTime())
    {
        m_nCurrentPerTime = GetMaxAttackTime();
        m_bHodeFire = false;
    }

    if(mpAICore)
        mpAICore->RenderBeginCore(nTime);
    
    if(mpStatusBar) mpStatusBar->RenderBeginCore(nTime);
}

bool CSprite::CanFireMissile()
{
    if(m_bHodeFire && m_nCurrentPerTime < (GetMaxAttackTime() / 4))
        return false;
    if(m_nCurrentPerTime < 0.f) return false;
    m_bHodeFire = false; //이부분을 코멘트 치면 최대 발사속도 초과 이후부터 에너지가 반이상 차지 않으면 연발로 쏠수가 없다.
    return true;
}

void CSprite::RenderBeginCore_Invisiable(int nTime)
{
    m_nCurrentPerTime += nTime/2;
    if(m_nCurrentPerTime > GetMaxAttackTime())
    {
        m_nCurrentPerTime = GetMaxAttackTime();
        m_bHodeFire = false;
    }
    
    if(mpAICore) 
        mpAICore->RenderBeginCore(nTime);
    
    ExcuteCmd();
}

int CSprite::RenderBegin(int nTime)
{
    
    CSprite *pActor = mpWorld->GetActor();
    if(pActor && pActor == this)
    {
        CMoveAI* mv = (CMoveAI*)mpAICore->GetMoveAI();
        if(mv)
            mv->TurnAndMove(nTime); //Actor만 RenderBegin에서 실행된다.
    }
    if(mpStatusBar) mpStatusBar->RenderBegin(nTime);
    
    ExcuteCmd();
	return E_SUCCESS;
}

void CSprite::GetInfomation(float* pfOutDLevel,float* pfOutALevel,float* pfOutDPower,float* pfOutAPower)
{
    //Added By Song 2013.10.30 Map Rank
    CUserInfo* pUserInfo = mpWorld->GetUserInfo();
    CSprite* pActor = GetWorld()->GetActor();
    float fMapRank = pUserInfo ? pUserInfo->GetCompletedRank() : 1.f;
    float fMapRankAValue = (fMapRank - 1) * POWWER_PER_AMAPRANK;
    float fMapRankDValue = (fMapRank - 1) * POWWER_PER_DMAPRANK;
    float fDUp = 0.f;
    float fDAt = 0.f;
    float fRunTimeUp = 0.f;
    if(pActor && pActor->GetTeamID() == GetTeamID())
    {
        if(pActor == this)
        {
            //주인공은 랭크만,업그레이드,를 적용해준다.
            fMapRankAValue = pUserInfo->GetRank();
            if(fMapRankAValue == 0.f) fMapRankAValue = 5.0f; //훈련소모드일때 너무 약하기 때문에 랭크 1로 해준다.
            fMapRankDValue = fMapRankAValue;
            fDUp = pUserInfo->GetDependUpgrade() * 2.5f;
            fDAt = pUserInfo->GetAttackUpgrade() * 2.f;
        }
        else
        {
            fMapRankAValue = pUserInfo->GetRank();
            fMapRankDValue = fMapRankAValue;
            fDUp = (float)pUserInfo->GetDependUpgrade() * 2.5f;
            fDAt = (float)pUserInfo->GetAttackUpgrade() * 2.f;
        }
        
        if(mpRuntimeUpgradeList)
        {
            RUNTIME_UPGRADE *prop = 0;
            for (vector<USERINFO*>::iterator it = mpRuntimeUpgradeList->begin(); it != mpRuntimeUpgradeList->end();)
            {
                prop = (RUNTIME_UPGRADE*)(*it)->pProp;
                if(prop && prop->nID == RUNTIME_UPDATE_UPGRADE)
                {
                    fRunTimeUp = prop->nUpgradeCnt;
                    break;
                }
                it++;
            }
        }
    }
    
    *pfOutDLevel = DUnitLevelToTotalLevel(mProperty.nMaxDefendPower,fMapRankDValue ,(fDUp + fRunTimeUp * 3.0f));
    *pfOutDPower = DDefenceLevelToPower(*pfOutDLevel);
    *pfOutALevel = DUnitLevelToTotalLevel(mProperty.nMaxAttackPower,fMapRankAValue ,(fDAt + fRunTimeUp * 3.0f));
    *pfOutAPower = DAttackLevelToPower(*pfOutALevel);
}

void CSprite::ResetPower(bool bRunTimeUpgrade)
{
    if(mnModelID != -1) //고스트가 아닐때..
    {
        float fDLevel,fALevel;
        
        float fCurrentRate = 1.0f;
        //비율을 거시기 해준다.
        if(bRunTimeUpgrade)
        {
            fCurrentRate = GetCurrentDefendPower() / mfMaxDefendPowerActor;
        }
        
        //상대팀은 mfMaxDefendPowerActor,m_fMaxAttackPower가 넘오온다.
        if(GetNetworkTeamType() == NETWORK_MYSIDE)
            GetInfomation(&fDLevel,&fALevel,&mfMaxDefendPowerActor,&m_fMaxAttackPower);
        
        if(bRunTimeUpgrade == false)
            SetCurrentDefendPower(mfMaxDefendPowerActor);
        else
            SetCurrentDefendPower(fCurrentRate * mfMaxDefendPowerActor); //업그레이드가 되면 될수록 현재 에너지가 너무작게 해주면 거시기 하자나.
        
        

        if(GetWorld()->GetActor() == this)
        {
            CUserInfo* pUserInfo = mpWorld->GetUserInfo();
            if (bRunTimeUpgrade)
                pUserInfo->mnRunTimeUpgrade += 1;
            pUserInfo->mnDLevel = fDLevel;
            pUserInfo->mnALevel = fALevel;
        }
    }
}

//카메라를 재정의 한다.
void CSprite::SetActor(void* pCamera)
{
    if(mpAICore)  mpAICore->SetActor();
    ResetPower();
	mpCamera = (SGLCAMERA*)pCamera;
	if(mpCamera->ViewFar != mpCamera->ViewFarOrg)
		mpCamera->ViewFar = mpCamera->ViewFarOrg;

    //첫번째판은 쉽게 깰수 있게 하였다.
    if(mpWorld->GetUserInfo() && mpWorld->GetUserInfo()->GetCompletedRank() <= 1)
        mcAliveCount = 1; //좀쉽게 생명을 디폴트로 하다 더 주자.

}


bool CSprite::SetPosition(SPoint* sp)
{	
//    if(sp->x == mPosition.x && sp->y == mPosition.y && sp->z == mPosition.z)
//    {
//        //sglLog("동일한위치를 셋하여 무시하였다.\n");
//        return false;
//    }
	memcpy(&mPosition,sp,sizeof(SPoint));
    return true;
}

//--------------------------------------------------------------
//주의 모든 리스트 어레이의 메모리를 검색하기 때문에 쓰레드에 안전하지 않다.
//그래서 EndRender()에 사용하는 것이 적합하다.
//너무 자주 사용하면 포퍼먼스에 무리가 간단다...
//--------------------------------------------------------------
CSprite* CSprite::GetTargetBySensor()
{
    SPoint pt2;
    float fA;
    float fx,fy,fz;
    float flen2,fAngle;
    
    int mMyTeam = GetTeamID();
    float fMaxLen = RADAVISIBLE * RADAVISIBLE;
    CHWorld* pWorld = (CHWorld*)mpWorld;
    CSGLCore* pCore = pWorld->GetSGLCore();
    float fMin = 99999999.0f;
    CSprite* pMinSprite = NULL;
    
    CListNode<CSprite>* pList = pCore->GetSpriteList();
    ByNode<CSprite>* b = pList->begin();
    for(ByNode<CSprite>* it = b; it != pList->end(); )
    {
        CSprite* sp = it->GetObject();
        //CGhostSprite ...
//        if(sp->GetState() != SPRITE_RUN || sp->GetModelID() == -1)
        if(sp->GetState() != SPRITE_RUN || sp->GetType() & ACTORTYPE_TYPE_GHOSTANITEM)
        {
            it = it->GetNextNode();
            continue;
        }
        if(pWorld->IsAllians(mMyTeam, sp->GetTeamID()) == false) //적일 경우
        {
            sp->GetPosition(&pt2);
            
            fx = pt2.x - mPosition.x;
            fy = pt2.y - mPosition.y;
            fz = -(pt2.z - mPosition.z);
            
            flen2 = fx * fx + fy * fy + fz * fz;
            if (flen2 <= fMaxLen)
            {
                fA =  atan2(fz,fx) * 180.0 / PI;
                fAngle = fA - GetOrientationToAttack();
                if(fAngle > -60.f && fAngle < 60.0f)
                {
                    //정확한 타겟이면 거리에 상관없다.
                    if(fAngle > -10.f && fAngle < 10.0f)
                    {
                        pMinSprite = sp;
                        break;
                    }
                    
                    if(flen2 < fMin)
                    {
                        fMin = flen2;
                        pMinSprite = sp;
                    }
                }
            }
        }
        it = it->GetNextNode();
    }
    return pMinSprite;
}


bool CSprite::CompactToBomb(CBomb *pBomb,float fCompactGrade)
{
    float fRatePowerDumy = GetWorld()->GetUserInfo()->GetLastSelectedMapIndex() == 0 ? 1.f : 0.5f;
    
    short shBubleCnt = 0;
    float fBombPower = pBomb->GetMaxAttackPower();
    
    CSprite *pEnamy = pBomb->GetOwner();
    if(pEnamy && pEnamy->GetState() == SPRITE_RUN)
    {
        float fM = pEnamy->GetMaxAttackPower();
        if(fM != 0.f)
        {
            //업그레이드 처리 (공격의 궁국적인 사항....)
            fBombPower = fBombPower + fBombPower * fM;
        }
    }
    
    if(fCompactGrade == 0.0f) return false;
    float fAttack = fBombPower * fRatePowerDumy / fCompactGrade;
    
    CAniRuntimeBubble* pBubble = GetAniRuntimeBubble();
    if(pBubble)
        shBubleCnt = (short)pBubble->GetUpgradeCount();
    else
        shBubleCnt = GetMainTowerBubbleCntInRange();
    
    //Bubble이 있으면 타격을 주지 말자.
    if(shBubleCnt)
    {
        if(shBubleCnt == 1)            fAttack = fAttack * 0.7f;
        else if(shBubleCnt == 2)       fAttack = fAttack * 0.4f;
        else                           fAttack = fAttack * 0.2f;
    }
    
    float fC = GetCurrentDefendPower();
    fC -= fAttack;
    if(fC <= 0)
    {
        m_fCurrentDefendPower = 0;
        return true;
    }
    SetCurrentDefendPower(fC);
    return false;
}

void CSprite::SetModelDirection(SVector* pvd)
{
	sglNormalize(pvd);
	memcpy(&mvViewDirection,pvd,sizeof(SVector));
}

int CSprite::GetCameraType() 
{ 
	return mnCameraType;
}



void CSprite::HandleCamera(float fUpDownVector,float fTurnVector)
{
    SPoint ptSpritePosition;
    SVector vSpriteDirection;
    
    GetPosition(&ptSpritePosition);
    GetModelDirection(&vSpriteDirection);
    

    if(fUpDownVector != 0.0f && GetCameraType() == CAMERT_NORMAL) //Actor의 기본적으로 +8
    {
        int nMaxY = ptSpritePosition.y + NormalViewMaxY;
        int mMinY = ptSpritePosition.y + NormalViewMinY;
        int nTime = GetClockDeltaConst();
        mfCameraUpDownPos += fUpDownVector * (float)nTime;
        
        if(ptSpritePosition.y  + mfCameraUpDownPos > nMaxY)
            mfCameraUpDownPos = NormalViewMaxY;
        if(ptSpritePosition.y  + mfCameraUpDownPos < mMinY)
            mfCameraUpDownPos = NormalViewMinY;
    }
    
    mpCamera->viewPos.x = ptSpritePosition.x;
    mpCamera->viewPos.y = ptSpritePosition.y + mfCameraUpDownPos;
    mpCamera->viewPos.z = ptSpritePosition.z;
    
    
    mpCamera->viewDir.x = vSpriteDirection.x;
    mpCamera->viewDir.z = vSpriteDirection.z;
    
    
//    //mfAccelatorUpAngle 0.4~0.9 = 0.5
//    //-30.f ~ -50.f = -20
//    float fAccMin = 0.4;
//    float fAccMax = 0.9;
//    
//    float fAccRate = fAccMax - fAccMin;
//    
//    float fVdirMin = -30.f;
//    float fVdirMax = -49.f;
//    
//    float fVdirRate = fVdirMax - fVdirMin;
//    
//    float fNowAcc = 0.8f;
//    float fNowVDir = fAccRate / fVdirRate * (fAccMin + fNowAcc);
    
    
#ifdef PINCHZOOM
    mpCamera->viewDir.y = vSpriteDirection.y - (float)(mfCameraUpDownPos - NormalViewMinY) / (float)(NormalViewMaxY - NormalViewMinY);
#endif
    
    mpCamera->viewUp.x = 0;  
    mpCamera->viewUp.y = 1;
    mpCamera->viewUp.z = 0;
    
    //단위벡터로 변경한다.
    sglNormalize(&mpCamera->viewDir);
    mpCamera->ViewFar = mpCamera->ViewFarOrg;
}

void CSprite::SetCameraType(int v) 
{ 
	
    
	mnCameraType = v;
	if(mpCamera == NULL) return ;
	if(v == CAMERT_NORMAL)
	{
        HandleCamera(0.0f,0.0f);
	}
    else if(v == CAMERT_BACK) //뒤에서 바라본다...
	{
        SPoint ptSpritePosition;
        SVector vSpriteDirection;
        
        GetPosition(&ptSpritePosition);
        GetModelDirection(&vSpriteDirection);

        
        float xBack = vSpriteDirection.x * 20;
        float yBack = ptSpritePosition.y + 15;
        float zBack = vSpriteDirection.z * 20;
		
		//위에서 아래로 본다.
		mpCamera->viewPos.x = (ptSpritePosition.x - xBack);
		mpCamera->viewPos.y = yBack;
		mpCamera->viewPos.z = (ptSpritePosition.z - zBack);
		
		mpCamera->viewDir.x = vSpriteDirection.x;
		mpCamera->viewDir.y = -0.4f;
		mpCamera->viewDir.z = vSpriteDirection.z;
		
		mpCamera->viewUp.x = 0;  
		mpCamera->viewUp.y = 1;
		mpCamera->viewUp.z = 0;
		
		//단위벡터로 변경한다.
		sglNormalize(&mpCamera->viewDir);
		mpCamera->ViewFar = mpCamera->ViewFarOrg;
	}
    else if(v == CAMERT_BACK_UP) //뒤에서 바라본다...
	{
        
        SPoint ptSpritePosition;
        SVector vSpriteDirection;
        
        GetPosition(&ptSpritePosition);
        GetModelDirection(&vSpriteDirection);

        
        float xBack = vSpriteDirection.x * 35;
        float yBack = ptSpritePosition.y + 30;
        float zBack = vSpriteDirection.z * 35;
		
		//위에서 아래로 본다.
		mpCamera->viewPos.x = (ptSpritePosition.x - xBack);
		mpCamera->viewPos.y = yBack;
		mpCamera->viewPos.z = (ptSpritePosition.y - zBack);
		
		mpCamera->viewDir.x = vSpriteDirection.x;
		mpCamera->viewDir.y = 0.0f;
		mpCamera->viewDir.z = vSpriteDirection.z;
        sglNormalize(&mpCamera->viewDir);
        mpCamera->viewDir.y = -0.4f;
		
		mpCamera->viewUp.x = 0;  
		mpCamera->viewUp.y = 1;
		mpCamera->viewUp.z = 0;
		
		//단위벡터로 변경한다.
		sglNormalize(&mpCamera->viewDir);
		mpCamera->ViewFar = mpCamera->ViewFarOrg;
	}
	else if(v == CAMERT_UP_VISIBLE)
	{
        SPoint ptSpritePosition;
        SVector vSpriteDirection;
        
        GetPosition(&ptSpritePosition);
        GetModelDirection(&vSpriteDirection);

		
		mpCamera->ViewFar = 1000; //위에서 보자
			
		//위에서 아래로 본다.
		mpCamera->viewPos.x = ptSpritePosition.x;
		mpCamera->viewPos.y = 400;
		mpCamera->viewPos.z = ptSpritePosition.z;
		
		mpCamera->viewDir.x = 0;
		mpCamera->viewDir.y = -400;
		mpCamera->viewDir.z = 0;
		
		//단위벡터로 변경한다.
		sglNormalize(&mpCamera->viewDir);
		
		mpCamera->viewUp.x = 0;  
		mpCamera->viewUp.y = 0;
		mpCamera->viewUp.z = 1;
		
	}
	else if(v == CAMERT_UP)
	{
        SPoint ptSpritePosition;
        SVector vSpriteDirection;
        
        GetPosition(&ptSpritePosition);
        GetModelDirection(&vSpriteDirection);

        
		//테스트로 위에서 바라보게 조정을 했다.
		mpCamera->ViewFar = 10000; //멀리보자.
		
		//위에서 아래로 본다.
		mpCamera->viewPos.x = 0;
		mpCamera->viewPos.y = 1000;
		mpCamera->viewPos.z = -1300;
		
		mpCamera->viewDir.x = 0;
		mpCamera->viewDir.y = -1100;
		mpCamera->viewDir.z = 1300;
		//단위벡터로 변경한다.
		sglNormalize(&mpCamera->viewDir);
		
		mpCamera->viewUp.x = 0;  
		mpCamera->viewUp.y = 0;
		mpCamera->viewUp.z = 1;
        
	}	
    else
    {
        HLogE("[Error] Not Found CameraType %d\n",v);
    }
	
}

bool CSprite::CollidesBounding(CSprite* Sprite,bool bExact)
{
	return false;
}



int CSprite::OnEvent(SGLEvent *pEvent)
{
	//int nResult = 0;
	//현재위치에서 움직인다.
	//SPoint* ptNow = GetPosition();
	int nMsgID = pEvent->nMsgID;
    
	//mvMove.x = 0, mvMove.y = 0, mvMove.z = 0를 해서 움직임을 멈춘다.
    if(pEvent->nMsgID == GSL_VIEW_MOVEUP)
    {
        mfCameraUpDownVector = -(float)(sqrtf(-pEvent->lParam)) / (float)gDisplayWidth * 16.f;
        HandleCamera(mfCameraUpDownVector,0.0f);
        return E_SUCCESS;
    }
    else if(pEvent->nMsgID == GSL_VIEW_MOVEDOWN)
    {
        //거리/시간;
        mfCameraUpDownVector = +(float)(sqrtf(pEvent->lParam)) / (float)gDisplayWidth * 16.f;
        HandleCamera(mfCameraUpDownVector,0.0f);
        return E_SUCCESS;
    }
    else if(pEvent->nMsgID == GSL_VIEW_MOVESTOP)
    {
        mfCameraUpDownVector = 0.0f;
        return E_SUCCESS;
    }
    else if(pEvent->nMsgID == GSL_VIEW_TURNLEFT)
    {
        mfCameraTurnVector = (float)(pEvent->lParam) / (float)gDisplayWidth * 360.f;
        HandleCamera(mfCameraUpDownVector,mfCameraTurnVector);
        return E_SUCCESS;
    }
    else if(pEvent->nMsgID == GSL_VIEW_TURNRIGHT)
    {
        mfCameraTurnVector = (float)(-pEvent->lParam) / (float)gDisplayWidth * 360.f;
        HandleCamera(mfCameraUpDownVector,mfCameraTurnVector);
        return E_SUCCESS;
    }
    else if(pEvent->nMsgID == GSL_VIEW_TURNSTOP)
    {

#if VIEWROTATION_ANI
            if(pEvent->lParam2 != 0)
            {
                pEvent->lParam2 = -pEvent->lParam2;
                if(gnDisplayType == DISPLAY_IPAD)
                    mfCameraTurnVector = (float)(pEvent->lParam2) / (float)gDisplayWidth * 540.f;
                else
                    mfCameraTurnVector = (float)(pEvent->lParam2) / (float)gDisplayWidth * 320.f;
                    
            }
            
            float fTime = (float)pEvent->lParam / 10000.f; //1000.f를 좀더 조절해서 10000.f로 했다.
            mfCameraTurnVector = mfCameraTurnVector / fTime;
            HandleCamera(mfCameraUpDownVector,mfCameraTurnVector);

#else
        mfCameraTurnVector = 0.0f;
        //굉장히 빠르게 돌린다.
        HandleCamera(mfCameraUpDownVector,mfCameraTurnVector);
#endif
        
        return E_SUCCESS;
    }
	else if(nMsgID & GSL_MOVE_STOP)
    {
        SetMoveVelocity(0.0f);
    }
    else
    {
        SVector vUD;
        int nSize = sizeof(SVector);
        SVector vViewDir;
        GetModelDirection(&vViewDir);
        memset(&vUD,0,nSize);
        
        //memset(&mvMove,0,nSize);
        
        if(nMsgID & GSL_MOVE_UP)
        {	
            vUD.x = vViewDir.x;
            vUD.y = 0;
            vUD.z = -vViewDir.z;
            sglNormalize(&vUD);		
        }
        else if(nMsgID & GSL_MOVE_DOWN)
        {
            
            vUD.x = -vViewDir.x;
            vUD.y = 0;
            vUD.z = vViewDir.z;
            sglNormalize(&vUD);
        }
        
        if(nMsgID & GSL_MOVE_UP || nMsgID & GSL_MOVE_DOWN )
        {
            SVector mv;
            mv.x = vUD.x;
            mv.y = 0;
            mv.z = vUD.z;
            SetMoveDirection(&mv);
            SetMoveVelocity((float)pEvent->lParam2 / 1000.0f);
        }
    }
	
	
    if(nMsgID & GSL_TRUN_STOP)
	{
		SetRotationVelocity(0.0f);
	}
    else
    {
        if(nMsgID & GSL_TRUN_RIGHT)
        {
            if(pEvent->lParam == 0)
                SetRotationVelocity(0.0f);
            else
                SetRotationVelocity((float)pEvent->lParam / 1000.0f); //고정소수점을 부동소수점으로 변환한다.
            
        }
        else if(nMsgID & GSL_TRUN_LEFT)
        {
            if(pEvent->lParam == 0)
                SetRotationVelocity(0.0f);
            else
                SetRotationVelocity((float)-pEvent->lParam / 1000.0f);//고정소수점을 부동소수점으로 변환한다.
        }
        
    }
    
    if(nMsgID & GSL_CHANGE_VIEWTYPE)
        ChangeCameraType();
    

    return E_SUCCESS;
	
	
}

void CSprite::ChangeCameraType()
{
    int nCameraType = GetCameraType();
    nCameraType++;
    if( nCameraType > CAMERT_BACK)
        nCameraType = CAMERT_NORMAL;
    SetCameraType(nCameraType);
}

//카메라가 벽에 걸치거나 어디에 내부를 그리지 않게 라디안이 1로 하여 360도 회전하여
//각 위치에 대한 충돌 검사를 한다.
bool CSprite::CameraComact(SPoint *pPoint,float* pOutY)
{
    int nResult;
    GLfloat fY = 0.f;
    SPoint ptNow;
	const GLubyte LINES = 4;
    const GLfloat radius = 3.0f;
	GLfloat points[3*LINES];
	GLfloat worldpoints[3];
	GLfloat matrix[16];
    
    int nIndex = 0;
    *pOutY = -9999.f;
    
	for (GLubyte i = 0; i < LINES; i++)
    {
        nIndex = i * 3;
        points[nIndex] = radius*(GLfloat)cosf(2.0f*PIf*i/LINES);		
        points[nIndex+1] = 0.0f;
        points[nIndex+2] = radius*(GLfloat)sinf(2.0f*PIf*i/LINES);
    }
    
	sglLoadIdentityf(matrix);
	sglTranslateMatrixf(matrix, pPoint->x,pPoint->y,pPoint->z);
	for (GLubyte i = 0; i < LINES; i++)
	{
		sglMultMatrixVectorf(&worldpoints[0], matrix, &points[i*3]);
        ptNow.x = worldpoints[0];
//        ptNow.y = worldpoints[1];
        fY = worldpoints[1];
        ptNow.z = worldpoints[2];
        

        nResult = ((CHWorld*)mpWorld)->GetPositionY(&ptNow);
        if(nResult != E_SUCCESS && nResult != E_POS_COMPACT_WALL)
        {
            return true;
        }
        else if(nResult == E_POS_COMPACT_WALL)
        {
            if(pPoint->y < ptNow.y) //건물의 높이보다 카메라의 높이가 낮으면 충돌되었다고 보자.
                return true;
        }
        else if(fY < ptNow.y) //카메라가 지형밑에 있다면.
        {
            *pOutY = ptNow.y;
            return true;
        }
	}
	return false;
}


void CSprite::Command(int nCommandID,void *pData)
{
    if(mpAICore) 
        mpAICore->Command((AICORE_AICOMMANDD)nCommandID,pData);
}

//런타임시 폭탄 아이템을 얻었을 경우.
void CSprite::TakeRunTimeBomb(int nItemID,int nCount)
{
    //폭탄의
    mpWorld->GetUserInfo()->AddListToBomb(nItemID,nCount);
}

void CSprite::StartReliveEffect()
{
    mnReliveEffect = GetGGTime();
//-------------------------------------------------------
//이전 것은 지워질것이다.
    if(mpShadowParticle)
    {
        mpShadowParticle->SetReliveEffect(true);
        mpShadowParticle->SetState(SPRITE_READYDELETE);
    }
//-------------------------------------------------------
    SPoint spNow;
    GetPosition(&spNow);
    SVector vtDir;
    vtDir.x = 0;
    vtDir.y = 0;
    vtDir.z = 1;
    
    mpShadowParticle = new CShadowParticle(this);
    mpShadowParticle->Initialize(&spNow, &vtDir);
    mpShadowParticle->SetReliveEffect(true);
    ((CHWorld*)mpWorld)->GetSGLCore()->AddParticle(mpShadowParticle);
    
}

void CSprite::Serialize(CArchive& ar,bool bWrite)
{
    if(bWrite)
    {
        ar << mnID;
        ar << mnModelID;
        ar << mcTeamID;

        ar << mPosition.x;
        ar << mPosition.y;
        ar << mPosition.z;
        
        ar << mvViewDirection.x;
        ar << mvViewDirection.y;
        ar << mvViewDirection.z;
    }
}


void CSprite::ArchiveMultiplay(int nCmd,CArchive& ar,bool bWrite)
{
    int nInterval = 0;
    char cNull = 0;
    int nTemp = 0;
    float fScale = 0.f;
    float* fColor;
    
    if(bWrite)
    {
        ar << mnID;
        ar << mnModelID;
        ar << mcTeamID;

    }
    else
    {
        ar >> mnID;
        ar >> mnModelID;
        ar >> mcTeamID;
        
    }
    
//    명령어에 따라 위치와 방향을 읽고 쓴다.
    if(nCmd == SUBCMD_OBJ_ALL_INFO)
    {
        unsigned char cAliveCount;
        if(bWrite)
        {
            ar << mnType;
            
            ar << mPosition.x;
            ar << mPosition.y;
            ar << mPosition.z;
            
            ar << mvViewDirection.x;
            ar << mvViewDirection.y;
            ar << mvViewDirection.z;
            
            
            ar << m_fMaxAttackPower;
            ar << mfMaxDefendPowerActor;
            ar << m_fCurrentDefendPower;  //현재 생명게이지
            cAliveCount = (unsigned char)GetAliveCount();
            ar << cAliveCount;
            
            //RunTimeList 정보
            int RunTimeListCnt = 0;
            if(mpRuntimeUpgradeList)
            {
                vector<USERINFO*> tempList;
                int nCnt = (int)mpRuntimeUpgradeList->size();
                for (int i = 0; i < nCnt; i++)
                {
                    USERINFO* pUserInfo = mpRuntimeUpgradeList->at(i);
                    RUNTIME_UPGRADE* prop = (RUNTIME_UPGRADE*)pUserInfo->pProp;
                    if(prop->nUpgradeCnt > 0)
                    {
                        tempList.push_back(pUserInfo);
                    }
                }
                
                RunTimeListCnt = (int)tempList.size();
                ar << RunTimeListCnt;
                for (int i = 0; i < RunTimeListCnt; i++)
                {
                    USERINFO* pUserInfo = tempList.at(i);
                    RUNTIME_UPGRADE* prop = (RUNTIME_UPGRADE*)pUserInfo->pProp;
                    ar << prop->nID;
                    ar << prop->nUpgradeCnt;
                }
            }
            else
                ar << RunTimeListCnt;
            
            
            //Bubble ----------------------
            CAniRuntimeBubble* pBubble = GetAniRuntimeBubble();
            if(pBubble && pBubble->GetState() == SPRITE_RUN)
            {
                cNull = 1;
                ar << cNull; //버블이 존재
                nTemp = 0;
                
                if(pBubble->GetMaxTime())
                {
                    nInterval = pBubble->GetMaxTime() - GetGGTime();
                    ar << nInterval;
                }
                else
                    ar << nTemp;
                
                nTemp = pBubble->GetUpgradeCount();
                ar << nTemp;
                fScale = pBubble->GetScale();
                ar << fScale;
                fColor = pBubble->GetColor();
                ar << fColor[0];
                ar << fColor[1];
                ar << fColor[2];
            }
            else
                ar << cNull;
            //Bubble ----------------------

            
        }
        else
        {
            ar >> mnType;
            
            ar >> mPosition.x;
            ar >> mPosition.y;
            ar >> mPosition.z;
            
            ar >> mvViewDirection.x;
            ar >> mvViewDirection.y;
            ar >> mvViewDirection.z;
            
            ar >> m_fMaxAttackPower;
            ar >> mfMaxDefendPowerActor;
            ar >> m_fCurrentDefendPower;  //현재 생명게이지
            ar >> mcAliveCount;
            //RunTimeList 정보
            int RunTimeListCnt = 0;
            int nID,nCnt;
            ar >> RunTimeListCnt;
            for (int i = 0; i < RunTimeListCnt; i++)
            {
                ar >> nID;
                ar >> nCnt;
                
                if(mpRuntimeUpgradeList)
                {
                    for (int j = 0; j < mpRuntimeUpgradeList->size(); j++)
                    {
                        USERINFO* pUserInfo = mpRuntimeUpgradeList->at(j);
                        RUNTIME_UPGRADE* prop = (RUNTIME_UPGRADE*)pUserInfo->pProp;
                        nID = prop->nID;
                        if(nID == prop->nID)
                        {
                            prop->nUpgradeCnt = nCnt;
                            //CScenario::SendMessage(SGL_COMPLETED_RUNTIME_UPGRADE_MULTIPLAY,(long)this,NULL,NULL,(long)prop);
                        }
                    }
                }
            }
            
            //Bubble ----------------------
            ar >> cNull;
            if(cNull)
            {
                ar >> nInterval;
                ar >> nTemp; //UpgradeCount
                ar >> fScale;
                CAniRuntimeBubble* pBubble = GetAniRuntimeBubble();
                if(pBubble && pBubble->GetState() == SPRITE_RUN)
                {
                    fColor = pBubble->GetColor();
                    ar >> fColor[0];
                    ar >> fColor[1];
                    ar >> fColor[2];
                    
                    if(nInterval)
                        pBubble->ResetBubble(nInterval);
                    pBubble->SetUpgradeCount(nTemp);
                }
                else
                {
                    SPoint ptNow;
                    GetPosition(&ptNow);
                    pBubble = new CAniRuntimeBubble(GetWorld(),this,nInterval,nTemp);
                    pBubble->Initialize(&ptNow, NULL);
                    pBubble->SetScale(fScale, fScale, fScale);
                    
                    fColor = pBubble->GetColor();
                    ar >> fColor[0];
                    ar >> fColor[1];
                    ar >> fColor[2];
                    pBubble->SetColor(fColor[0], fColor[1], fColor[2]);
                    
                    SetAniRuntimeBubble(pBubble);
                    CSGLCore *pCore = ((CHWorld*)mpWorld)->GetSGLCore();
                    pCore->AddParticle(pBubble);
                }
            }
            //Bubble ----------------------
        }
    }
}