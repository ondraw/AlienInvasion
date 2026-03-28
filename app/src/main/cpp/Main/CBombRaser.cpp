//
//  CBombRaser.cpp
//  SongGL
//
//  Created by itsme on 2014. 3. 11..
//
//
#include <math.h>
#include "CHWorld.h"
#include "CBombRaser.h"
#include "CSGLCore.h"
#include "CK9Sprite.h"
#include "CRaserParticle.h"
#include "CParticleEmitterMan.h"
#include "CElectricArcParticle.h"

namespace {
void SpawnElectricShell(CHWorld* pWorld,const SPoint& center,float radius)
{
    CElectricArcParticle *pShellMain = new CElectricArcParticle(pWorld);
    pShellMain->Initialize((SPoint*)&center, radius * 0.82f, 0.72f);
    pWorld->GetSGLCore()->AddParticle(pShellMain);

    CElectricArcParticle *pShellInner = new CElectricArcParticle(pWorld);
    pShellInner->Initialize((SPoint*)&center, radius * 0.72f, 0.60f);
    pWorld->GetSGLCore()->AddParticle(pShellInner);
}
}
CBombRaser::CBombRaser(CSprite* ptTarget,CSprite* pOwner,unsigned int nWhosBombID,unsigned char cTeamID,int nModelID,IAction *pAction,IHWorld* pWorld,PROPERTY_BOMB* pBombProperty):CBomb(ptTarget,pOwner,nWhosBombID,cTeamID,nModelID,pAction,pWorld,pBombProperty)
{
    mnAliveTime = GetGGTime() + pBombProperty->nBombTailMaxAliveTime; //1초후
    mnResult=0;
    mpTarget = ptTarget;
    mParticle = NULL;
    mnRecTargetPos = 0;
    mnLastImpactEffectTime = 0;
    mbCheckReset = true;
    mnCompactIntervalTime = 0;
}

CBombRaser::~CBombRaser()
{
    mParticle = NULL;
}

int CBombRaser::Initialize(SPoint *pPosition,SVector *pvDirAngle,SVector *pvtDir)
{
    string sPath = mBombProperty.sModelPath;
    sPath.append(".txt");
    mParticle = new CRaserParticle(mpWorld,this);
    mParticle->Initialize(mpWorld->GetModelMan(), sPath.c_str(),mBombProperty.sBombTailImgPath,mBombProperty.sBombBombImgPath);
    ((CHWorld*)mpWorld)->GetSGLCore()->AddParticle(mParticle);
    mnType = 0x10000000 | mBombProperty.nBombType; //폭탄은 0x10000000으로 시작한다.
    return E_SUCCESS;
}

void CBombRaser::RenderBeginCore(int nTime)
{
    int nLen;
    SVector vtDir;
    SVector vDirAngle;
    SPoint ptNow;
    SPoint ptTarget;
    SPoint ptEffect;
    
    if(mState == SPRITE_RUN && mpOwner)
    {
        if(mpOwner && mpOwner->GetState() != SPRITE_RUN)
        {
            mpOwner = NULL;
            mpTarget = NULL;
            mState = SPRITE_READYDELETE;
            return;
        }
        
        if (mpTarget && mpTarget->GetState() != SPRITE_RUN)
        {
            mpOwner = NULL;
            mpTarget = NULL;
            mState = SPRITE_READYDELETE;
            return;
        }
        
        //Alive Time이 존재하는지를 체크해준다. 넘으면 레이져를 지워준다.
        if(GetGGTime() >= mnAliveTime)
        {
            mpOwner = NULL;
//            mpTarget = NULL;
            mState = SPRITE_READYDELETE;
            return;
        }
        
        
        mpOwner->GetPosition(&ptNow);
        mpTarget->GetPosition(&ptTarget);
        mnResult = GetTargetPos(&ptNow, &ptTarget,&vtDir,&nLen);
        if(mnResult == -1) //거리가 짧거나 비정상일때...
        {
            SetCompactBomb();
            return;
        }
        
        if(mnResult == 0)
            mnCompactIntervalTime += nTime;
        
        vDirAngle.x = asinf(vtDir.y) * 180.0 / PI;
        vDirAngle.y = ((CMyModel*)mpOwner->GetModel())->orientation[1];
        vDirAngle.z = atan2(-vtDir.z,vtDir.x) * 180.0 / PI;
        
        //즉 지상의 타입은 몸에 가려지기 때문에 위쪽으로 해준다.
        if(mpTarget->GetType() != ACTORTYPE_FIGHTER)
            ptTarget.y += mpTarget->GetRadius();
        if(mpOwner->GetType() == ACTORTYPE_FIGHTER)
            ptNow.y += mpOwner->GetRadius() / 4.f;
        
        mParticle->Arrange(mBombProperty.fStartPosition, &ptNow, &ptTarget, &vDirAngle, &vtDir, nLen);

        if(strstr(mBombProperty.sBombBombImgPath, "ThunderBolt") != NULL)
        {
            long lNow = GetGGTime();
            if(lNow - mnLastImpactEffectTime >= 25)
            {
                if(mnResult == 0 && mpTarget)
                {
                    ptEffect = ptTarget;
                    if(mpTarget->GetType() != ACTORTYPE_FIGHTER)
                        ptEffect.y += mpTarget->GetRadius() * 0.25f;
                    SpawnElectricShell((CHWorld*)mpWorld, ptEffect, mpTarget->GetRadius() * 1.35f);
                }
                else
                {
                    ptEffect = ptTarget;
                    if(mnResult == 2)
                    {
                        SPoint ptGround = ptEffect;
                        if(((CHWorld*)mpWorld)->GetPositionY(&ptGround) == E_SUCCESS)
                            ptEffect.y = ptGround.y + 1.5f;
                    }
                    SpawnElectricShell((CHWorld*)mpWorld, ptEffect, 6.0f);
                }
                mnLastImpactEffectTime = lNow;
            }
        }
    }
}

int CBombRaser::RenderBegin(int nTime)
{
    return E_SUCCESS;
}

int CBombRaser::Render()
{
    return E_SUCCESS;
}

int CBombRaser::RenderEnd()
{
    return E_SUCCESS;
}

void CBombRaser::SetCompactBomb()
{
//    SPoint ptPos;
//    SVector vDir;
//    CSGLCore *pCore = ((CHWorld*)mpWorld)->GetSGLCore();
//    mState = SPRITE_READYDELETE;
}

bool CBombRaser::CollidesBounding(CSprite* Sprite,bool bExact)
{
    if(mnResult == 0 && mnCompactIntervalTime >= 200)
    {
        mnCompactIntervalTime -= 200;
        return true;
    }
    return false;
}

int CBombRaser::CollidesToTerrian()
{
    return 0; //비충돌
}

void CBombRaser::SetVisibleRender(bool v)
{
    if(mState == SPRITE_RUN)
    {
        
        if(v == false) //mParticle을 숨길때 자신의 자신이 안보인다면 , 상대방이 안보이는지를 체크해보자.
        {
            if(mpTarget->IsVisibleRender() == false) //타겟또한 안보이면 파티클이 보이지 않게 하자.
                mParticle->SetVisibleRender(false);
        }
        else
            mParticle->SetVisibleRender(true);
        CSprite::SetVisibleRender(v);
    }
}

//장애물에 막힌지점
int CBombRaser::GetTargetPos(SPoint *pPosition,SPoint *pToOutPosion,SVector* pOutDirection,int* pOutLen)
{
    int nResult = 0;
    float fx =  pToOutPosion->x - pPosition->x;
    float fy =  pToOutPosion->y - pPosition->y;
    float fz =  pToOutPosion->z - pPosition->z;
    
    SPoint pt,pt2;
    memcpy(&pt, pPosition, sizeof(SPoint));
    float fLen = sqrtf(fx * fx + fy * fy + fz *fz);
    if(fLen > sqrtf(mpOwner->GetDefendRadianBounds()))
        return -1;
    
    if(isnan(fLen))
    {
        HLogE("CK9Sprite::GetTargetPos : fLen Nan\n");
        return -1;
    }
    
    *pOutLen = fLen;
    
    fx = fx/ fLen;
    fy = fy/ fLen;
    fz = fz/ fLen;
    
    pOutDirection->x = fx;
    pOutDirection->y = fy;
    pOutDirection->z = fz;
    
    //-----------------------------------------------------------
    if(GetGGTime() <= mnRecTargetPos && mnResult == 0) //명중일때만 적용하자.
    {
        mbCheckReset = true;
        return mnResult;
    }
    if (mbCheckReset)
    {
        mnRecTargetPos = GetGGTime() + 500; //0.5초
        mbCheckReset = false;
    }
    //-----------------------------------------------------------
    
    float fUnit = 10.f; //길이 15단위로 나누어서 계산한다.
    float fLenTotal = 0.f;
    while (true)
    {
        if(fLenTotal >= fLen)
            return 0;
        
        
        pt.x += fUnit * fx;
        pt.y += fUnit * fy;
        pt.z += fUnit * fz;
        
        memcpy(&pt2, &pt, sizeof(SPoint));
        if(((CHWorld*)mpWorld)->GetPositionY(&pt2) != E_SUCCESS)
        {
            nResult = 1;
            break;
        }
        else if(pt.y < pt2.y) //땅밑으로 갔다면.
        {
            if(mpTarget) //땅속에서 타겟의 반경을 빼면
            {
                if( (fLen - fLenTotal) < mpTarget->GetRadius() )
                    return 0;
            }
            nResult = 2;
            break;
        }
        fLenTotal += fUnit;
    }
    memcpy(pToOutPosion, &pt, sizeof(SPoint));
    
    
    fx =  pToOutPosion->x - pPosition->x;
    fy =  pToOutPosion->y - pPosition->y;
    fz =  pToOutPosion->z - pPosition->z;
    *pOutLen = sqrtf(fx * fx + fy * fy + fz *fz);
    return nResult;
}
