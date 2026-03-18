/*
 *  CK9Sprite.cpp
 *  SongGL
 *
 *  Created by 호학 송 on 11. 1. 30..
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
#include <math.h>
#include "CET1.h"
#include "CHWorld.h"
#include "sGLTrasform.h"
#include "CBombET1.h"
#include "CFireParticle.h"
#include "CBombTailParticle.h"
#include "sGLUtils.h"
#include "CTextureMan.h"
#include "CTE1BoosterParticle.h"
#include "CAICore.h"
#include "CSGLCore.h"
#include "CShadowParticle.h"
#include "CTankModelASCII.h"



CET1::CET1(unsigned int nID,unsigned char cTeamID,int nModelID,IAction *pAction,IHWorld* pWorld,PROPERTY_TANK* pProperty,NETWORK_TEAMTYPE NetworkTeamType) : CK9Sprite(nID,cTeamID,nModelID,pAction,pWorld,pProperty,NetworkTeamType)
{
	mfModelHeaderAngle = 0.0f;
	mfModelHeaderRotVector= 0.0f;
    mnPoUpDownState = 0;
	
	//--------------------------
	//자동화 타겟으로 ....
	//--------------------------
	mfAutoMovePo_UpDonwDir = 0.0f;
    
    mBoosterParticle = NULL;
    mpCurrentSelMissaileInfo = NULL;
}

CET1::~CET1()
{
}

//---------------------------------------------------------------------
//Start CSprite Virtual

//Data를 읽어온다.
int CET1::LoadResoure(CModelMan* pModelMan)
{
//    CSGLCore* pCore = ((CHWorld*)mpWorld)->GetSGLCore();
    CK9Sprite::LoadResoure(pModelMan);
    
//    //소멸을 파티클 리스트에서 해제된다.
//    mBoosterParticle = new CTE1BoosterParticle;
//    mBoosterParticle->Initialize(NULL, NULL);
//    pCore->AddParticle(mBoosterParticle);
    return E_SUCCESS;
}


int CET1::Initialize(SPoint *pPosition,SVector *pvDirection)
{
    CSGLCore* pCore = ((CHWorld*)mpWorld)->GetSGLCore();
    //소멸을 파티클 리스트에서 해제된다.
    mBoosterParticle = new CTE1BoosterParticle;
    mBoosterParticle->Initialize(NULL, NULL);
    pCore->AddParticle(mBoosterParticle);

    return CK9Sprite::Initialize(pPosition, pvDirection);
}



bool CET1::SetPosition(SPoint* sp)
{

    if(CK9Sprite::SetPosition(sp))
    {
        //공중부양
        mpModel->position[1] += ET1_GROUND_HEIGHT;
        if(mpModelHeader)
            mpModelHeader->position[1] += ET1_GROUND_HEIGHT;
        return true;
    }
    return false;
}


int CET1::Render()
{
    return CK9Sprite::Render();
}

void CET1::SetVisibleRender(bool v)
{
    CK9Sprite::SetVisibleRender(v);
    if(mState == SPRITE_RUN)
    {
        if(mBoosterParticle) mBoosterParticle->SetVisibleRender(v);
    }

}

//End CSprite Virtual
//---------------------------------------------------------------------


int  CET1::ArrangeSprite()
{
    SVector vDir;
    int nResult = CK9Sprite::ArrangeSprite();
    if(nResult == E_SUCCESS)
    {
        //부스터를 계산한다.
        if(mBoosterParticle) 
        {
            GetModelDirection(&vDir);
            mBoosterParticle->CalBooster(&vDir,mPSRightBP,mPSLeftBP);        
        }
    }
    return nResult;
}



void CET1::SetState(SPRITE_STATE v) 
{ 
    CK9Sprite::SetState(v);
    if(mBoosterParticle)
    {
        mBoosterParticle->SetState(v);
        if(v != SPRITE_RUN)
            mBoosterParticle = NULL;
    }
}








