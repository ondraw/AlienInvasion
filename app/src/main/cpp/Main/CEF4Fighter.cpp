//
//  CEF4Fighter.cpp
//  SongGL
//
//  Created by itsme on 2014. 4. 25..
//
//
#include <math.h>
#include "CEF4Fighter.h"
#include "CBombRaser.h"
#include "CSprite.h"
#include "CSGLCore.h"
#include "CHWorld.h"
#include "CAICore.h"

//nModelID : 모델 아이디를 가지고 loadresource 에서 어떠한 밀크쉐이프 모델을 읽어오는지를 알아낸다.
CEF4Fighter::CEF4Fighter(unsigned int nID,unsigned char cTeamID,int nModelID,IAction *pAction,IHWorld* pWorld,PROPERTY_TANK* pProperty,NETWORK_TEAMTYPE NetworkTeamType) : CFighter(nID,cTeamID,nModelID,pAction,pWorld,pProperty,NetworkTeamType)
{
    
}

CEF4Fighter::~CEF4Fighter()
{
    
}

void CEF4Fighter::GetFirePoPos(int nIndex,float** pOutFront)
{
    sglMesh     *pWorldMesh = mpModel->GetWorldMesh();
    *pOutFront = &pWorldMesh[mptStartMissile[nIndex][0]].pVetexBuffer[mptStartMissile[nIndex][1] * 3];
}

float CEF4Fighter::GetMoveVelocity()
{
    if(GetState() != SPRITE_RUN) return mProperty.fMoveRate * mfVelocity;
    
    if(GetModelID() == ACTORID_EF5 && mfAttackAngle <= 50.f)
    {
        //공격하면 천천히 날고
        //공격하지 않으면 빠르게 날자.
        for (int i = 0; i < mnPoStartCnt; i++) {
            CSprite* pTargetSprite = mpAICore->GetAttackTo(i);
            if(pTargetSprite)
                return mProperty.fMoveRate * mfVelocity / 2.f; //속도 평상시 속도 /10
        }
    }
    return mProperty.fMoveRate * mfVelocity;
}

void  CEF4Fighter::FireOrgMissile(int nIndex)
{
    SVector vDirAngle,vtDir;
    SPoint ptNow;
    float* ptPos = NULL;
    GetFirePoPos(nIndex,&ptPos);

    ptNow.x = ptPos[0];
    ptNow.y = ptPos[1];
    ptNow.z = ptPos[2];
    
    
    NewMissile(nIndex,ptNow,vtDir,vDirAngle);
    
//    m_nCurrentPerTime -= GetFirePerSec();
//    if(m_nCurrentPerTime < 0)
//        m_bHodeFire = true;
}


bool CEF4Fighter::CanFireMissile()
{
    unsigned long lN = GetGGTime();
    if(lN < mnFireInterval)
        return false;
    if(mfAttackAngle > 50.f) return false; //각도가 너무 벗어나면
    
    return true;
}

void  CEF4Fighter::FireMissile()
{
    bool bIs = false;
    for(int i = 0; i < mnPoStartCnt; i++)
    {
        if(!CanFireMissile()) return;
        FireOrgMissile(i);
        bIs = true;
    }
    
    if(bIs)
        mnFireInterval = GetGGTime() + mBombProperty.nBombTailMaxAliveTime / mnPoStartCnt;
}

void CEF4Fighter::FireMissileInVisible()
{
    bool bIs = false;
    for(int i = 0; i < mnPoStartCnt; i++)
    {
        if(!CanFireMissile()) return;
        FireOrgMissileInVisible(i);
        bIs = true;
    }
    if(bIs)
        mnFireInterval = GetGGTime() + mBombProperty.nBombTailMaxAliveTime / mnPoStartCnt;

}


void  CEF4Fighter::FireOrgMissileInVisible(int nIndex)
{
    float fRadius = GetRadius();
    SVector vDirAngle;
    SPoint  ptNow;
    SVector vtDir;
    GetPosition(&ptNow);
    
    if(nIndex % 2)
        ptNow.x += fRadius;
    else
        ptNow.z += fRadius;

    NewMissile(nIndex,ptNow,vtDir,vDirAngle,false);
    
//    m_nCurrentPerTime -= GetFirePerSec();
//    if(m_nCurrentPerTime < 0)
//        m_bHodeFire = true;
}

void  CEF4Fighter::NewMissile(int nIndex,SPoint& ptNow,SVector& vtDir,SVector& vDirAngle,bool bNeedParticle)
{
    CBombRaser *pNewBomb = NULL;
    CSGLCore* pCore = ((CHWorld*)mpWorld)->GetSGLCore();
    int nTargetResult = 0;
    SPoint ptTarget;
    float fLength = 0.f;
    
    CSprite* pTargetSprite = mpAICore->GetAttackTo(nIndex); //NULL검사는 하지 않음 앞에 CanFireGun에서 해버렸다.
    if(pTargetSprite == NULL) return ;
    pTargetSprite->GetPosition(&ptTarget);
    ptTarget.y += 5.f;
    
    nTargetResult = GetTargetPos(&ptNow, &ptTarget,&vtDir,&fLength);
    if(nTargetResult == -1)
    {
        delete pNewBomb;
        return;
    }
    
//    mnFireInterval = GetGGTime() + 200;
    pNewBomb = new CBombRaser(pTargetSprite,this,GetID(),GetTeamID(),mGunProperty.nID,mpAction,GetWorld(),&mGunProperty);
//    mnFireGunInterval = GetGGTime() + mGunProperty.nBombTailMaxAliveTime;
//    mnFireGunInterval = GetGGTime() + mGunProperty.nBombTailMaxAliveTime;
    pNewBomb->Initialize(NULL,NULL,NULL);
    pCore->AddBomb(pNewBomb);
    
    if(bNeedParticle)
    {
//레이져 소리가 없다.
//        float fIntervalToActor = GetIntervalToCamera();
//#ifdef ANDROID //JAVA의 터치시에 플래이가 JNI로 연결되어 잇어서 에러가 난다.
//        if(fIntervalToActor < 20000)
//            CScenario::SendMessage(SGL_PLAYSOUND,mBombProperty.nSoundFilreID,0);
//#else
//        if(fIntervalToActor < 20000)
//            pCore->PlaySystemSound(mBombProperty.nSoundFilreID);
//#endif
    }
}

//장애물에 막힌지점
int CEF4Fighter::GetTargetPos(SPoint *pPosition,SPoint *pToOutPosion,SVector* pOutDirection,float* pOutLength)
{
    float fx =  pToOutPosion->x - pPosition->x;
    float fy =  pToOutPosion->y - pPosition->y;
    float fz =  pToOutPosion->z - pPosition->z;
    
    SPoint pt,pt2;
    memcpy(&pt, pPosition, sizeof(SPoint));
    float fLen = sqrtf(fx * fx + fy * fy + fz *fz);
    if(fLen > sqrtf(GetDefendRadianBounds()))
    {
        //범위에서 너무 벗어 났다.
        return -1;
    }
    
    if(isnan(fLen))
    {
        HLogE("CFighter::GetTargetPos : fLen Nan\n");
        return -1;
    }
    
    *pOutLength = fLen;
    
    fx = fx/ fLen;
    fy = fy/ fLen;
    fz = fz/ fLen;
    
    pOutDirection->x = fx;
    pOutDirection->y = fy;
    pOutDirection->z = fz;
    
    float fUnit = 10.f; //길이 15단위로 나누어서 계산한다.
    float fLenTotal = 0.f;
    while (true)
    {
        if(fLenTotal >= fLen)
        {
            return 0; //명중
        }
        
        pt.x += fUnit * fx;
        pt.y += fUnit * fy;
        pt.z += fUnit * fz;
        
        memcpy(&pt2, &pt, sizeof(SPoint));
        if(((CHWorld*)mpWorld)->GetPositionY(&pt2) != E_SUCCESS)
        {
            break;
        }
        else if(pt.y < pt2.y) //땅밑으로 갔다면.
        {
            break;
        }
        fLenTotal += fUnit;
    }
    memcpy(pToOutPosion, &pt, sizeof(SPoint));
    fx =  pToOutPosion->x - pPosition->x;
    fy =  pToOutPosion->y - pPosition->y;
    fz =  pToOutPosion->z - pPosition->z;
    *pOutLength = sqrtf(fx * fx + fy * fy + fz *fz);
    return 1; //장애물에 막혔다. 즉 땅속이나 건물에 막혀있다.
}