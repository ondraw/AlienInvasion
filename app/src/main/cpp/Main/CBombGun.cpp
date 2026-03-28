//
//  CBombGun.cpp
//  SongGL
//
//  Created by itsme on 2013. 12. 4..
//
//
#include "CBomb.h"
#include <math.h>
#include "CBombGun.h"
#include "CHWorld.h"

CBombGun::CBombGun(CSprite* ptTarget,CSprite* pOwner,unsigned int nWhosBombID,unsigned char cTeamID,int nModelID,IAction *pAction,IHWorld* pWorld,PROPERTY_BOMB* pBombProperty):CBomb(ptTarget,pOwner,nWhosBombID,cTeamID,nModelID,pAction,pWorld,pBombProperty)
{
    mfTotalLength = 0.f;
    mfNowLenght = 0.f;
    memset(&mptTargetPos,0,sizeof(SPoint));
    mbCheckCompact = false; //폭탄이 주위와 맞는 것인지 확인 하지 말자. 이미 맞을 놈은 정해져 있다.
    mpTarget = ptTarget; //CBomb에서는 유도기능이 가능해야만 타겟이 정해진다.
}

CBombGun::~CBombGun()
{
}


int CBombGun::CollidesToTerrian()
{
    if(mfNowLenght > mfTotalLength)
    {
        return -1;
    }
    return E_SUCCESS;
}

void CBombGun::ChainCourse(int nTime)
{
    
    SPoint ptNow;
    SVector vtDir;
    GetModelDirection(&vtDir);
    GetPosition(&ptNow);
    float fVol = mBombProperty.fVelocity * (float)nTime * .001f;
    
    mfNowLenght += fVol;
    
    if(mfNowLenght > mfTotalLength)
    {
        //터진다. 거리보다 많이 갔다.
        SetPosition(&mptTargetPos);
        return ;
    }
    
    ptNow.x += vtDir.x * fVol;
    ptNow.y += vtDir.y * fVol;
    ptNow.z += vtDir.z * fVol;
    
    SetPosition(&ptNow);
}

int CBombGun::Initialize(SPoint *pPosition,SPoint* ptTargetPos,SVector *pvDirAngle,SVector *pvtDir,float fTotalLength,int nResult)
{
    //초기발사속도
    //방향
    //각도
    //등등의 초기값을 사용한다.
    
    LoadResoure(mpWorld->GetModelMan());
    
    memcpy(&mptStart,pPosition,sizeof(SPoint));
    
    mptStart.x += pvtDir->x * mBombProperty.fStartPosition;
    mptStart.y += pvtDir->y * mBombProperty.fStartPosition;
    mptStart.z += pvtDir->z * mBombProperty.fStartPosition;
    
    //파티클 에미터에서 사용함.
    SetModelDirection(pvtDir);
    
    SetPosition(&mptStart);
    
    mArrAngle[0] = pvDirAngle->x; //높이
    mArrAngle[1] = pvDirAngle->y; //해덩의 방향
    mArrAngle[2] = pvDirAngle->z; //포의 방향
    
    
    mpModel->orientation[1] = pvDirAngle->z; //포의 방향
    mpModel->orientation[2] = pvDirAngle->x; //상하각
    
    mpModel->scale[0] = mBombProperty.fScale;
    mpModel->scale[1] = mBombProperty.fScale;
    mpModel->scale[2] = mBombProperty.fScale;
    
    
    mpModel->position[0] = mptStart.x;
    mpModel->position[1] = mptStart.y;
    mpModel->position[2] = mptStart.z;
    
    
    
    mfTotalLength = fTotalLength;
    memcpy(&mptTargetPos,ptTargetPos,sizeof(SPoint));
    
    mnResult = nResult;
    mnType = 0x10000000 | mBombProperty.nBombType; //폭탄은 0x10000000으로 시작한다.
    return E_SUCCESS;
}



void CBombGun::SetCompactBomb()
{
    CBomb::SetCompactBomb();
}
