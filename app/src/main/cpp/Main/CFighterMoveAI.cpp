//
//  CFighterMoveAI.cpp
//  SongGL
//
//  Created by itsme on 2013. 12. 17..
//
//
#include "CHWorld.h"
#include <math.h>
#include "CFighterMoveAI.h"
#include "CSprite.h"
#include "sGLUtils.h"
#include "CMutiplyProtocol.h"
#include "CSGLCore.h"

CFighterMoveAI::CFighterMoveAI(CSprite* pSprite)
{
    mpSprite = pSprite;
    mpMoveToSprite = NULL;
    mfGuideDirectionX = 0.f;
    mfGuideDirectionY = 0.f;
    mBeforeUp = true;
}

CFighterMoveAI::~CFighterMoveAI()
{
    
}

void CFighterMoveAI::RenderBeginCore(int nTime)
{
    
    if(mpMoveToSprite && mpMoveToSprite->GetState() != SPRITE_RUN)
    {
        mpMoveToSprite = NULL;
    }
    else
    {
        ChainCourse(nTime);
    }
}

bool CFighterMoveAI::GetDonComact()
{
    return true;
}
//적탱크로 이동한다.
void CFighterMoveAI::Command(int nCommandID,void *pData)
{
}

bool CFighterMoveAI::IsMoveTo()
{
    if(mpMoveToSprite)
    {
        if(mpMoveToSprite->GetType() == ACTORTYPE_EMAINBLZ) return false;
        return true;
    }
    return false;
}

void CFighterMoveAI::ChainCourse(int nTime)
{
    bool bNeedSync = false;
    float fMaxAngle = (float)*(float*)mpSprite->GetSomethingValue(3);
    float fGuideAngleY = 0.f;
    float fGuideAngleX = 0.f;
    bool  bNeedGuideAngleY = true;
    SPoint ptTarget,ptNow,ptGround;
    SVector vtDir,vtDirTarget;
    float* pOrientation;
    
    CSprite *pMoveToSprite = mpMoveToSprite;
    if(pMoveToSprite == NULL)
    {
        CSprite* pMainTower = (CSprite*)mpSprite->GetSomethingValue(4);
        if((long)this % 2)
        {
            if(pMainTower && pMainTower->GetState() == SPRITE_RUN)
                SetMoveTo(pMainTower);
        }
        else
        {
            CSprite* pActor = mpSprite->GetWorld()->GetActor();
            if(pActor && pActor->GetState() == SPRITE_RUN)
                SetMoveTo(pActor);
        }
        return ;
    }
    
    mpSprite->GetModelDirection(&vtDir);
    pOrientation = mpSprite->GetOrientaion();
    
    //    float fTime = (float)nTime / 1000.f;
#if CGDisplayMode == 1
    float fTime = (float)nTime / 1000.f;
    if(gnCGDiplayMode == 2)
        fTime = (float)nTime / 10000000.f;
#else
    
    float fTime = (float)nTime / 1000.f;
#endif
    float vInter = mpSprite->GetMoveVelocity() * fTime;
    if(vInter == 0.f) return ;
    
    mpSprite->GetPosition(&ptNow);
    
    pMoveToSprite->GetPosition(&ptTarget);
    if(pMoveToSprite->GetType() != ACTORTYPE_FIGHTER)
        ptTarget.y += FIGHTER_MIN_HEIGHT;
    

    vtDirTarget.x = ptTarget.x - ptNow.x;
    vtDirTarget.y = ptTarget.y  - ptNow.y; //+5.f는 너무 땅이기 때문에 캐릭터가 존재하는 부분까지 ....
    vtDirTarget.z = - (ptTarget.z - ptNow.z);
    
    //타겟과의 거리를 계산하여 너무 가까 우면...
    float fV = vtDirTarget.x*vtDirTarget.x + vtDirTarget.y*vtDirTarget.y + vtDirTarget.z*vtDirTarget.z;
    float fDifAngleX = AngleArrangeEx(pOrientation[1] - atan2(vtDirTarget.z,vtDirTarget.x) * 180.0 / PI);
    
//    HLogD("fV=%f, fD = %f\n",fV,fDifAngleX);
    if(fDifAngleX <= 180.f) //Right
    {
        if(fDifAngleX > 90 && fV < 20000.f)
        {
            fGuideAngleX = 0.f; //앞으로만 전진한다.
            mfGuideDirectionX = 0.f;
            bNeedGuideAngleY = false;
        }
        else
        {
            if(mfGuideDirectionX > 0.f) //이전에 왼쪽이라면 급속하게 오른쪽으로 변경하지 말자
            {
                if(fDifAngleX > 5.f) //너무 많이 트러지면 다시 재조정해준다.
                {
                    mfGuideDirectionX = -fTime * 100.f;
                }
                fGuideAngleX = 0.f;
            }
            else
            {
                if(fDifAngleX > fMaxAngle)
                    fGuideAngleX = fMaxAngle;
                else
                {
                    fGuideAngleX = fDifAngleX;
                }
                mfGuideDirectionX = -fTime * 100.f;
            }

        }
        
    }
    else //Left
    {
        fDifAngleX = 360 - fDifAngleX;
        if(fDifAngleX > 90 && fV < 20000.f)
        {
            fGuideAngleX = 0.f; //앞으로만 전진한다.
            mfGuideDirectionX = 0.f;
            bNeedGuideAngleY = false;
        }
        else
        {
            if(mfGuideDirectionX < 0.f)
            {
                if(fDifAngleX > 5.f)
                {
                    mfGuideDirectionX = fTime * 100.f;
                }
                fGuideAngleX = 0.f;
            }
            else
            {
                if(fDifAngleX > fMaxAngle)
                    fGuideAngleX = fMaxAngle;
                else
                {
                    fGuideAngleX = fDifAngleX;
                }
                mfGuideDirectionX = fTime * 100.f;
            }
        }
    }
    
    //        local dx, dy, dz = object.getDirection(application.getCurrentUserActiveCamera, object.kGlobalSpace)
    //        local yaw = -math.atan2(dx, -dz)
    //        local pitch = math.atan2(dy, math.sqrt((dx * dx) + (dz * dz)))
    
    //float fDifAngleY = AngleArrangeEx(mpModel->orientation[2] - atan2(vtDirTarget.y,vtDirTarget.x) * 180.0 / PI);
    
    memcpy(&ptGround, &ptNow, sizeof(SPoint));
    ((CHWorld*)mpSprite->GetWorld())->GetPositionY(&ptGround);
    
    
    SPRITE_STATE st = mpSprite->GetState();
    
    if(ptNow.y < ptGround.y)
    {
        bNeedGuideAngleY = false;
        
        if(st == BOMB_COMPACT || st == BOMB_COMPACT_ANI)
        {
            mpSprite->SetState(SPRITE_READYDELETE);
            return ;
        }
    }
    
    float fDifAngleY = 0.f;
    if (bNeedGuideAngleY)
    {
        if(mBeforeUp == false)
        {
            bNeedSync = true;
            mBeforeUp = true;
        }
        
        fDifAngleY = AngleArrangeEx(pOrientation[2] -
                                          atan2(vtDirTarget.y,
                                                sqrtf(vtDirTarget.x * vtDirTarget.x + vtDirTarget.z *vtDirTarget.z)) * 180.0 / PI);
    }
    else
    {
        fDifAngleY = AngleArrangeEx(pOrientation[2] -
                                    atan2(vtDirTarget.y + FIGHTER_MAX_HEIGHT,
                                          sqrtf(vtDirTarget.x * vtDirTarget.x + vtDirTarget.z *vtDirTarget.z)) * 180.0 / PI);
        mBeforeUp = false;
    }
    
    if(st == BOMB_COMPACT || st == BOMB_COMPACT_ANI)
    {
        fDifAngleY = 40.f; //추락 시킨다.
    }

    if(fDifAngleY <= 180.f) //Down
    {
        if(mfGuideDirectionY > 0.f)
        {
            if(fDifAngleY > 5.f) //너무 많이 트러지면 다시 재조정해준다.
            {
                mfGuideDirectionY = -fTime * 100.f;
            }
            fGuideAngleY = 0.f;
        }
        else
        {
            if(fDifAngleY > fMaxAngle)
                fGuideAngleY = fMaxAngle;
            else
            {
                fGuideAngleY = fDifAngleY;
            }
            mfGuideDirectionY = -fTime * 100.f;
        }
    }
    else //Up
    {
        fDifAngleY = 360.f - fDifAngleY;
        if(mfGuideDirectionY < 0.f)
        {
            if(fDifAngleY > 5.f)
            {
                mfGuideDirectionY = fTime * 100.f;
            }
            fGuideAngleY = 0.f;
        }
        else
        {
            if(fDifAngleY > fMaxAngle)
                fGuideAngleY = fMaxAngle;
            else
            {
                fGuideAngleY = fDifAngleY;
            }
            
            mfGuideDirectionY = fTime * 100.f;
        }
        
    }
    
    mpSprite->SetSomethingValue(5, (void*)&fDifAngleX);
    mpSprite->SetSomethingValue(6, (void*)&fV);
    

    if(fGuideAngleX == 0.f && fGuideAngleY == 0.f)
    {
        vtDir.x *= vInter;
        vtDir.y *= vInter;
        vtDir.z *= vInter;
        
        ptNow.x += vtDir.x;
        ptNow.y += vtDir.y;
        ptNow.z -= vtDir.z;
        
        mpSprite->SetSomethingValue(1, (void*)&fGuideAngleX);
        mpSprite->SetSomethingValue(2, (void*)&fGuideAngleY);
        
        sglNormalize(&vtDir);
        mpSprite->SetModelDirection(&vtDir);
        mpSprite->SetPosition(&ptNow);
    }
    else
    {

        fGuideAngleX = fGuideAngleX * mfGuideDirectionX;
        fGuideAngleY = fGuideAngleY * mfGuideDirectionY;
                
        mpSprite->SetSomethingValue(1, (void*)&fGuideAngleX);
        mpSprite->SetSomethingValue(2, (void*)&fGuideAngleY);
        
        
        
        pOrientation[1] = pOrientation[1] + fGuideAngleX ;
        pOrientation[2] = pOrientation[2] + fGuideAngleY;
        
        //AngleToVector
        sglAngleToVector(pOrientation,&vtDir);
        
        ptNow.x += vtDir.x * vInter;
        ptNow.y += vtDir.y * vInter;
        ptNow.z -= vtDir.z * vInter;
        
        sglNormalize(&vtDir);
        mpSprite->SetModelDirection(&vtDir);
        mpSprite->SetPosition(&ptNow);
    }
    
    //비행기는 너무 빠르기 때문에 싱크를 시켜준다.
    if(bNeedSync && mpSprite->GetNetworkTeamType() == NETWORK_MYSIDE)
        mpSprite->GetWorld()->GetMutiplyProtocol()->SendSyncFighterSprite(mpSprite);


}

void CFighterMoveAI::ArchiveMultiplay(int nCmd,CArchive& ar,bool bWrite)
{
    int nToID = 0;
    
    if(nCmd == SUBCMD_OBJ_ALL_INFO)
    {
        if(bWrite)
        {
            
            if(mpMoveToSprite)
                nToID = mpMoveToSprite->GetID();
            
            ar << nToID;
            ar << mfGuideDirectionX;
            ar << mfGuideDirectionY;
        }
        else
        {
            ar << nToID;
            if(nToID)
                mpMoveToSprite = ((CHWorld*)mpSprite->GetWorld())->GetSGLCore()->FindSprite(nToID);
            ar << mfGuideDirectionX;
            ar << mfGuideDirectionY;
        }
        
    }

}


void CFighterMoveAI::SetMoveTo(CSprite* v)
{
    mpMoveToSprite = v;
    if(mpSprite->GetNetworkTeamType() == NETWORK_MYSIDE)
        mpSprite->GetWorld()->GetMutiplyProtocol()->SendSyncMoveFighter(mpSprite->GetID(), v->GetID());
}