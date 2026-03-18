//
//  CAmbulance.cpp
//  SongGL
//
//  Created by itsme on 2014. 5. 22..
//
//
#include <math.h>
#include "CHWorld.h"
#include "CParticleEmitterMan.h"
#include "CAmbulance.h"
#include "CTankModelASCII.h"
#include "CDMainTowerStatus.h"
#include "CMoveAI.h"
#include "CAICore.h"
#include "CStrategyAmbulanceAI.h"

#define MAXMAINSTATUS 100.f

CAmbulance::CAmbulance(unsigned int nID,unsigned char cTeamID,int nModelID,IAction *pAction,IHWorld* pWorld,PROPERTY_TANK* pProperty,NETWORK_TEAMTYPE NetworkTeamType) : CTireCar(nID,cTeamID,nModelID,pAction,pWorld,pProperty,NetworkTeamType)
{
    
}
CAmbulance::~CAmbulance()
{
    
}
int CAmbulance::LoadResoure(CModelMan* pModelMan)
{
    int nResult = CTireCar::LoadResoure(pModelMan);
    if(mpStatusBar == NULL)
    {
        mpStatusBar = new CDMainTowerStatus(this);
        mpStatusBar->Initialize(NULL, NULL);
        SetCurrentDefendPowerEn(MAXMAINSTATUS);
    }
    return nResult;
}

int	 CAmbulance::Initialize(SPoint *pPosition,SVector *pvDirection)
{
    mpModelBody->pfinalmulMaxtix = new GLfloat[16];
    int nR = CK9Sprite::Initialize(pPosition, pvDirection);
    
    mpModelBody->scale[0] = mProperty.fScale;
	mpModelBody->scale[1] = mProperty.fScale;
	mpModelBody->scale[2] = mProperty.fScale;
    
    mpModelBody->position[0] = pPosition->x;
    mpModelBody->position[1] = pPosition->y;
    mpModelBody->position[2] = pPosition->z;
    
    mpModelBody->orientation[1] = atan2(pvDirection->z,pvDirection->x) * 180.0 / PI;
    
    mpModelBody->InitFinalMatrix();
    
    
    AAI* pMoveAI = new CMoveAI(this);
    mpAICore = new CAICore(this,pMoveAI,new CStrategyAmbulanceAI(this,pMoveAI));
    
    return nR;
}

void CAmbulance::SetCurrentDefendPowerEn(float v)
{
    mfNowStatus = v;
    mpStatusBar->SetSubBarRate(1.0f - (MAXMAINSTATUS - mfNowStatus) / MAXMAINSTATUS);
}

void CAmbulance::ResetPower(bool bRunTimeUpgrade)
{
    if(bRunTimeUpgrade == true)
    {
        float f = GetCurrentDefendPowerEn() + MAXMAINSTATUS / 2.0f;
        if(f > MAXMAINSTATUS) f = MAXMAINSTATUS;
        SetCurrentDefendPowerEn(f);
    }
    
    
    CTireCar::ResetPower(bRunTimeUpgrade);
}

void CAmbulance::RenderBeginCore(int nTime)
{
    if(mfNowStatus < MAXMAINSTATUS)
        SetCurrentDefendPowerEn(mfNowStatus + nTime / 2000.f); //2초에 1올라간다. 즉 10초에 5이 올라간다.
    else
        SetCurrentDefendPowerEn(MAXMAINSTATUS);
    
    CTireCar::RenderBeginCore(nTime);
}

void CAmbulance::RenderBeginCore_Invisiable(int nTime)
{
    CTireCar::RenderBeginCore(nTime);
    if(mState == SPRITE_RUN)
    {
        if(mfNowStatus < MAXMAINSTATUS)
            SetCurrentDefendPowerEn(mfNowStatus + nTime / 2000.f); //2초에 1올라간다. 즉 10초에 5이 올라간다.
        else
            SetCurrentDefendPowerEn(MAXMAINSTATUS);
    
    }
    else if(mState == BOMB_COMPACT || mState == BOMB_COMPACT_ANI)
    {
        SetState(SPRITE_READYDELETE);
    }
}


int CAmbulance::RenderBegin(int nTime)
{
	//먼저 이동을 한ㄷ
	int nResult = E_SUCCESS;
	
    if(GetType() == ACTORTYPE_TYPE_GHOST) return E_SUCCESS;
    
    if(mState == SPRITE_RUN)
    {
        CSprite::RenderBegin(nTime);
        if(mProperty.nType == 1 && isMoving())
        {
            SPoint ptP;
            SVector ptDir;
            GetPosition(&ptP);
            GetMoveDirection(&ptDir);
            
            ptDir.x *= -1; //반대
            ptDir.z *= -1; //반대
            
            ptP.x += (ptDir.x * 6.f);
            ptP.y += 2.5f;
            ptP.z -= (ptDir.z * 6.f);
            ((CHWorld*)GetWorld())->GetParticleEmitterMan()->NewBackGas(&ptP, &ptDir);
        }
        
        RuntimeUpgrading(nTime);
        
        //파이어시 애니메이션을 해준다.
        RenderBeginFireAnimation(nTime,FireAnimationMax);
        mpModel->updatef(nTime);
        mpModelBody->updatef(nTime);
        
//        RenderBeginAutoTurnHeader(nTime * .3f);
//        RenderBeginAutoMovePo_UpDown(nTime);
        
        
        mpModelHeader->updatef(nTime);
    }
    else if(mState == BOMB_COMPACT || mState == BOMB_COMPACT_ANI)
    {
        CampactAnimaion(nTime);
        mpModel->updatef(nTime);
        mpModelHeader->updatef(nTime);
        
    }
	return nResult;
}
