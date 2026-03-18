//
//  CDGunTower.cpp
//  SongGL
//
//  Created by itsme on 2013. 12. 11..
//
//
#include <math.h>
#include "CDGunTower.h"
#include "CHWorld.h"
#include "CSGLCore.h"
#include "CAICore.h"
#include "CBombGun.h"

CDGunTower::CDGunTower(unsigned int nID,unsigned char cTeamID,int nModelID,CTextureMan *pTextureMan,IAction *pAction,IHWorld* pWorld,PROPERTY_TANK* pProperty,NETWORK_TEAMTYPE NetworkTeamType) :
CDTower(nID,cTeamID,nModelID,pTextureMan,pAction,pWorld,pProperty,NetworkTeamType)
{
    
}

CDGunTower::~CDGunTower()
{
    
}

int     CDGunTower::Initialize(SPoint *pPosition,SVector *pvDirection)
{
    CDTower::Initialize(pPosition, pvDirection);
    memset(&mBeforeOwnerPos, 0, sizeof(SPoint));
    memset(&mBeforeTargetPos, 0, sizeof(SPoint));
    mBeforeLenght = -1;
    mBeforeResult = -1;
    return E_SUCCESS;
}

//장애물에 막힌지점
int CDGunTower::GetTargetPos(SPoint *pPosition,SPoint *pToOutPosion,SVector* pOutDirection)
{
    float fx =  pToOutPosion->x - pPosition->x;
    float fy =  pToOutPosion->y - pPosition->y;
    float fz =  pToOutPosion->z - pPosition->z;
    CSprite* pTargetSprite = mpAICore->GetAttackTo();
    SPoint pt,pt2;
    memcpy(&pt, pPosition, sizeof(SPoint));
    float fLen = sqrtf(fx * fx + fy * fy + fz *fz);
    if(fLen > sqrtf(GetDefendRadianBounds()))
    {
        mBeforeLenght = -1;
        mBeforeResult = -1;
        return mBeforeResult;
    }
    
    if(isnan(fLen))
    {
        HLogE("CDGunTower::GetTargetPos : fLen Nan\n");
        return -1;
    }
    
    fx = fx/ fLen;
    fy = fy/ fLen;
    fz = fz/ fLen;
    
    pOutDirection->x = fx;
    pOutDirection->y = fy;
    pOutDirection->z = fz;
    
    if(mBeforeLenght == -1)
    {
        bool bNeedReCat = false;
        float fx1 = pPosition->x - mBeforeOwnerPos.x;
        float fy1 = pPosition->y - mBeforeOwnerPos.y;
        float fz1 = pPosition->z - mBeforeOwnerPos.z;
        float fLenO = fx1 * fx1 + fy1 * fy1 + fz1 *fz1;
        if(fLenO > 4.f) //거리 2보다 크면 많이 움직였다.
        {
            memcpy(&mBeforeOwnerPos, pPosition, sizeof(SPoint));
            bNeedReCat = true;
        }
        
        fx1 = pToOutPosion->x - mBeforeTargetPos.x;
        fy1 = pToOutPosion->y - mBeforeTargetPos.y;
        fz1 = pToOutPosion->z - mBeforeTargetPos.z;
        fLenO = fx1 * fx1 + fy1 * fy1 + fz1 *fz1;
        if(fLenO > 4.f) //거리 2보다 크면 많이 움직였다.
        {
            memcpy(&mBeforeTargetPos, &pToOutPosion, sizeof(SPoint));
            bNeedReCat = true;
        }
        
        if(bNeedReCat == false)
        {
            memcpy(pToOutPosion, &mBeforeTargetPos, sizeof(SPoint));
            return mBeforeResult;
        }
    }
    
    float fUnit = 10.f; //길이 15단위로 나누어서 계산한다.
    float fLenTotal = 0.f;
    while (true)
    {
        if(fLenTotal >= fLen)
        {
            mBeforeResult = 0;
            mBeforeLenght = fLen;
            return mBeforeResult;
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
            if(pTargetSprite) //땅속에서 타겟의 반경을 빼면
            {
                if( (fLen - fLenTotal) < pTargetSprite->GetRadius() )
                    return 0;
            }
            break;
        }
        fLenTotal += fUnit;
    }
    memcpy(pToOutPosion, &pt, sizeof(SPoint));
    
    
    fx =  pToOutPosion->x - pPosition->x;
    fy =  pToOutPosion->y - pPosition->y;
    fz =  pToOutPosion->z - pPosition->z;
    fLen = sqrtf(fx * fx + fy * fy + fz *fz);
    mBeforeLenght = fLen;
    
    mBeforeResult = 1;
    return mBeforeResult;
}

void CDGunTower::NewMissile(SPoint& ptNow,SVector& vtDir,SVector& vDirAngle,bool bNeedParticle)
{
    CSprite* pTargetSprite = mpAICore->GetAttackTo();
    
    CSGLCore* pCore = ((CHWorld*)mpWorld)->GetSGLCore();
    CBombGun *pNewBomb = NULL;
    SPoint ptTarget;
    
    //유도기능이 존재한다.면, 타겟이 없으면 타겟을
    if(pTargetSprite == NULL)
        return ;
    
    pNewBomb = new CBombGun(pTargetSprite,this,GetID(),GetTeamID(),mBombProperty.nID,mpAction,GetWorld(),&mBombProperty);
    pTargetSprite->GetPosition(&ptTarget);
    if(pTargetSprite->GetType() != ACTORTYPE_FIGHTER)
        ptTarget.y += 5.f;
    
    if(GetTargetPos(&ptNow, &ptTarget,&vtDir) == -1)
    {
//        HLogE("Long Far CDGunTower %d\n",this);
        delete pNewBomb;
        mpAICore->ResetAttackTo();
        return;
    }
    
    vDirAngle.x = asinf(vtDir.y) * 180.0 / PI;
    vDirAngle.y = mfRotationAngle;
    vDirAngle.z = atan2(-vtDir.z,vtDir.x) * 180.0 / PI;
    
    pNewBomb->Initialize(&ptNow,&ptTarget, &vDirAngle,&vtDir,mBeforeLenght,mBeforeResult);
    pCore->AddBomb(pNewBomb);
    
    if(bNeedParticle)
    {
        float fIntervalToActor = GetIntervalToCamera();
#ifdef ANDROID //JAVA의 터치시에 플래이가 JNI로 연결되어 잇어서 에러가 난다.
        if(fIntervalToActor < 5500)
            CScenario::SendMessage(SGL_PLAYSOUND,mBombProperty.nSoundFilreID,0);
#else
        if(fIntervalToActor < 5500)
            pCore->PlaySystemSound(mBombProperty.nSoundFilreID);
#endif
    }
}

bool CDGunTower::AimAutoToEnemy(SPoint* ptEnemy)
{

    if(isMovingAutoHeader()) return false;
    
	SVector v;
	SPoint ptMine;
    GetPosition(&ptMine);
    
    mfPo_UpDonwDir = 0.f;
	mfRotationVector= 0.f;
    //	mbAutoMovePo_UpDown = false;
	
	v.x = ptEnemy->x - ptMine.x;
	v.y = ptEnemy->y - ptMine.y;
	v.z = -(ptEnemy->z - ptMine.z); //z.의 방향이 반대이기 때문에 벡터의 방향을 반대로 설정해준다.
	
    float fLenT = sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
    v.x /= fLenT;
    v.y /= fLenT;
    v.z /= fLenT;
    
    if(fLenT < 0.0000001f) //거의 0일때.
        return false;
    
    //탱크의 각도 + 포의 각도가 실제 각도이다.
    float fNowPoDir = AngleArrangeEx ( mfRotationAngle );
    
	//--------------------------------
	// 포의 회전각 = 기본적으로 탱크의 방향(180 반대?) + 두점사이각
	//--------------------------------
    mfRotationToAngle = AngleArrangeEx(atan2(v.z,v.x) * 180.0 / PI); //이 각도로 회전해야 한다.
    
    float fHeightAngleRadian = asinf(v.y);
    mfPoUpToAngle = 180 * fHeightAngleRadian / PI;
    
    float fRotDir = CatRotation(fNowPoDir,mfRotationToAngle);
    if(fRotDir == 0.f)
    {
        CheckAutoMove_PoUpDown();
        mfRotationVector = 0.f;
    }
    else
    {
        mfRotationVector = fRotDir * mProperty.fHeaderTurnSize;
    }
    
    if(mfRotationVector == 0.f && mfPo_UpDonwDir == 0.f)
        mbOnAmiedNeedFire = true;
    return true;
}


