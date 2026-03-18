//
//  CArmeredCar.cpp
//  SongGL
//
//  Created by itsme on 2014. 5. 21..
//
//
#include <math.h>
#include "CHWorld.h"
#include "CSGLCore.h"
#include "CBombGun.h"
#include "AAICore.h"
#include "CArmeredCar.h"
#include "CSpriteStatus.h"

CArmeredCar::CArmeredCar(unsigned int nID,unsigned char cTeamID,int nModelID,IAction *pAction,IHWorld* pWorld,PROPERTY_TANK* pProperty,NETWORK_TEAMTYPE NetworkTeamType) : CTireCar(nID,cTeamID,nModelID,pAction,pWorld,pProperty,NetworkTeamType)
{
}

CArmeredCar::~CArmeredCar()
{
    if(mpModelBody)
    {
        delete mpModelBody;
        mpModelBody = NULL;
    }
}
int CArmeredCar::LoadResoure(CModelMan* pModelMan)
{
    CTireCar::LoadResoure(pModelMan);
//    if(mpStatusBar == NULL)
//    {
//        mpStatusBar = new CSpriteStatus(this);
//        mpStatusBar->Initialize(NULL, NULL);
//    }
    return E_SUCCESS;
}
void CArmeredCar::FireMissile()
{
  
    if(!CanFireMissile()) return;
    for(int i = 0; i < mnPoStartCnt; i++)
    {
        FireOrgMissile();
        mnPoIndex++;
        if(mnPoIndex >= mnPoStartCnt)
            mnPoIndex = 0;
    }
    
    mnFireInterval = GetGGTime() + 200;

}

void CArmeredCar::FireMissileInVisible()
{
    if(!CanFireMissile()) return;
    for(int i = 0; i < mnPoStartCnt; i++)
    {
        
        FireOrgMissileInVisible();
        mnPoIndex++;
        if(mnPoIndex >= mnPoStartCnt)
            mnPoIndex = 0;
    }
    
    mnFireInterval = GetGGTime() + 200;
}


bool CArmeredCar::CanFireMissile()
{
//    if(CSprite::CanFireMissile() == false) return false;
    unsigned long lN = GetGGTime();
    if(lN < mnFireInterval || mpAICore->GetAttackTo() == NULL) return false;
    return true;
}

void CArmeredCar::NewMissile(SPoint& ptNow,SVector& vtDir,SVector& vDirAngle,bool bNeedParticle)
{
    CSprite* pTargetSprite = mpAICore->GetAttackTo();
    
    CSGLCore* pCore = ((CHWorld*)mpWorld)->GetSGLCore();
    CBombGun *pNewBomb = NULL;
    SPoint ptTarget;
    
    //유도기능이 존재한다.면, 타겟이 없으면 타겟을
    if(pTargetSprite == NULL) return ;
    
    pNewBomb = new CBombGun(pTargetSprite,this,GetID(),GetTeamID(),mBombProperty.nID,mpAction,GetWorld(),&mBombProperty);
    pTargetSprite->GetPosition(&ptTarget);
    
    if(pTargetSprite->GetType() != ACTORTYPE_FIGHTER)
        ptTarget.y += 5.f;
    
    if(GetTargetPos(&ptNow, &ptTarget,&vtDir) == -1)
    {
        delete pNewBomb;
        mpAICore->ResetAttackTo();
        return;
    }
    mnFireGunInterval = GetGGTime() + 200;
    
    vDirAngle.x = asinf(vtDir.y) * 180.0 / PI;
    vDirAngle.y = mfModelHeaderAngle;
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

bool CArmeredCar::AimAutoToEnemy(SPoint* ptEnemy)
{
    if(isMovingAutoHeader()) return false;
    
	SVector v;
	SPoint ptMine;
    //	if(isMovingAutoHeader()) return false;
    GetPosition(&ptMine);
	
    
    mfAutoTurnAHeader_ToAngle = 0.f;
	mfModelHeaderRotVector= 0.f;
#if OBJMilkShape
	mbAutoMovePo_UpDown = false;
#endif
	
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
    float fNowPoDir = AngleArrangeEx ( mpModel->orientation[1] + mfModelHeaderAngle );
    
	//--------------------------------
	// 포의 회전각 = 기본적으로 탱크의 방향(180 반대?) + 두점사이각
	//--------------------------------
    mfAutoTurnAHeader_ToAngle = AngleArrangeEx(atan2(v.z,v.x) * 180.0 / PI); //이 각도로 회전해야 한다.
    
    
    
    //+asinf(v.y) 높이/빞변 v.y = v.y/fLenT 평자가 아니라 산악일때
	float fHeightAngleRadian = asinf(v.y);
	mfAutoMovePo_ToUpDownAngle = 180 * fHeightAngleRadian / PI;
    
        float fRotDir = CatRotation(fNowPoDir,mfAutoTurnAHeader_ToAngle);
    if(fRotDir == 0.f)
    {
        CheckAutoMove_PoUpDown();
        mfModelHeaderRotVector = 0.f;
    }
    else
    {
        mfModelHeaderRotVector = fRotDir * mProperty.fHeaderTurnSize;
    }
    
    if(mfModelHeaderRotVector == 0.f && mfAutoMovePo_UpDonwDir == 0.f)
        mbOnAmiedNeedFire = true;
	return true;
    
    
}