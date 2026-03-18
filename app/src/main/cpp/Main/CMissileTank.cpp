//
//  CMissileTank.cpp
//  SongGL
//
//  Created by 호학 송 on 2014. 5. 11..
//
//
#include <math.h>
#include "CHWorld.h"
#include "CMissileTank.h"
#include "CParticleEmitterMan.h"
#include "CTankModelASCII.h"
#include "AAICore.h"
#include "CMoveAI.h"
#include "CStrategyAssistanceAI.h"
#include "CAICore.h"
#include "CBombTailParticle.h"
#include "CFireParticle.h"
#include "CSGLCore.h"
#include "CSpriteStatus.h"


CMissileTank::CMissileTank(unsigned int nID,unsigned char cTeamID,int nModelID,IAction *pAction,IHWorld* pWorld,PROPERTY_TANK* pProperty,NETWORK_TEAMTYPE NetworkTeamType) : CNewTank(nID,cTeamID,nModelID,pAction,pWorld,pProperty,NetworkTeamType)
{
    mNextPoHide1 = 0;
    mNextCanFireWate1 = 0;
    
    mNextPoHide2 = 0;
    mNextCanFireWate2 = 0;
    
    mNextPoHide3 = 0;
    mNextCanFireWate3 = 0;
}

CMissileTank::~CMissileTank()
{
}

//Data를 읽어온다.
int CMissileTank::LoadResoure(CModelMan* pModelMan)
{

    string sPath;
    if(mProperty.nType == 1) //탱크
        sPath = mProperty.sModelPath + string("Root.ms3d");
    else //날라다니는 것
        sPath = mProperty.sModelPath + string("Root.txt");
    
	pModelMan->getModelf(sPath.c_str(), &mpModel,true);
    sPath = mProperty.sModelPath + string("Root.ani");
    sPath = mProperty.sModelPath + string("Header.txt");
    //    mpModelHeader = new CTankModelASCII(((CHWorld*)mpWorld)->GetSGLCore()->GetMS3DModelASCIIMan()->Load(sPath.c_str(),true));
    pModelMan->getModelfTankHeader(sPath.c_str(), (CMyModel**)&mpModelHeader,true);
    
    if(mpWorld->GetUserInfo())
        mnMapID = mpWorld->GetUserInfo()->GetLastSelectedMapID(); //달맵은 절벽이 없이 잘 움직이게 해주자.
    
    if(mpStatusBar == NULL)
    {
        mpStatusBar = new CSpriteStatus(this);
        mpStatusBar->Initialize(NULL, NULL);
    }
	return E_SUCCESS;
}


int	 CMissileTank::Initialize(SPoint *pPosition,SVector *pvDirection)
{
    
    int nR = CK9Sprite::Initialize(pPosition, pvDirection);
    AAI* pMoveAI = new CMoveAI(this);
    mpAICore = new CAICore(this,pMoveAI,new CStrategyAssistanceAI(this,pMoveAI,mnPoStartCnt,mnGunStartCnt ? true : false));
    return nR;
}


bool CMissileTank::CanFireMissile()
{
    long nNow = GetGGTime();
    if(nNow > mNextCanFireWate1 || nNow > mNextCanFireWate2 || nNow > mNextCanFireWate3) return true;
    return false;
}


void CMissileTank::FireMissile()
{
    unsigned long nNow = GetGGTime();
    const int nWait = 3000.f;
    const int nHide  = 2000.f;
    
    if(nNow > mNextCanFireWate1 && mpAICore->GetAttackTo(0))
    {
        mnPoIndex = 0;
        CNewTank::AimAutoToEnemy(mpAICore->GetAttackTo(0));
        
        FireOrgMissile();
        
        mNextCanFireWate1 = nNow + nWait;
        mNextPoHide1 = nNow + nHide;
        mpModelHeader->SetHideMeshGroup(1, true);
    }
    
    if(nNow > mNextCanFireWate2  && mpAICore->GetAttackTo(1))
    {
        mnPoIndex = 1;
        //CNewTank::AimAutoToEnemy(mpAICore->GetAttackTo(1));
        FireOrgMissile();
        
        mNextCanFireWate2 = nNow + nWait;
        mNextPoHide2 = nNow + nHide;
        mpModelHeader->SetHideMeshGroup(2, true);
    }
    
    if(nNow > mNextCanFireWate3  && mpAICore->GetAttackTo(2))
    {
        mnPoIndex = 2;
        //CNewTank::AimAutoToEnemy(mpAICore->GetAttackTo(2));
        FireOrgMissile();
        
        mNextCanFireWate3 = nNow + nWait;
        mNextPoHide3 = nNow + nHide;
        mpModelHeader->SetHideMeshGroup(3, true);
    }
}

void CMissileTank::FireMissileInVisible()
{
    const int nWait = 3000.f;
    const int nHide  = 2000.f;
    
    unsigned long nNow = GetGGTime();    
    if(nNow > mNextCanFireWate1  && mpAICore->GetAttackTo(0))
    {
        mnPoIndex = 0;
        FireOrgMissileInVisible();
        
        mNextCanFireWate1 = nNow + nWait;
        mNextPoHide1 = nNow + nHide;
        mpModelHeader->SetHideMeshGroup(1, true);
    }
    
    if(nNow > mNextCanFireWate2  && mpAICore->GetAttackTo(1))
    {
        mnPoIndex = 1;
        FireOrgMissileInVisible();
        
        mNextCanFireWate2 = nNow + nWait;
        mNextPoHide2 = nNow + nHide;
        mpModelHeader->SetHideMeshGroup(2, true);
    }
    
    if(nNow > mNextCanFireWate3  && mpAICore->GetAttackTo(2))
    {
        mnPoIndex = 2;
        FireOrgMissileInVisible();
        
        mNextCanFireWate3 = nNow + nWait;
        mNextPoHide3 = nNow + nHide;
        mpModelHeader->SetHideMeshGroup(3, true);
    }
}

void  CMissileTank::FireOrgMissileInVisible()
{
    float fMaxLenDouble,fMaxLen,fDivy;
    float fUpAngle = GetInvisiblePoUpAngle();
    
    //Added By Song 2014.03.16 미사일 스타일
    if(mBombProperty.nBombType == 1) //미사일 스타일
    {
        if(fUpAngle < 20)
            fUpAngle = 20;
    }
    
    float fRadius = GetRadius();
    float fWAngle,fHAngle;
    SVector vDirAngle;
    SPoint  ptNow,ptEnemy;
    SVector vtDir;
    float vectorLen;
    CSprite* pEn = mpAICore->GetAttackTo(mnPoIndex);
    if(pEn == NULL) return;
    pEn->GetPosition(&ptEnemy);
    
    ptEnemy.y += 8.f; //포신이 조금 위에 있자나. 그래서 좀 높혀서 비율에 맞추어봤다.
    GetPosition(&ptNow);
    ptNow.y += 8.f; //포신이 조금 위에 있자나. 그래서 좀 높혀서 비율에 맞추어봤다.
    
    //SVector vtDir;
	vtDir.x = ptEnemy.x - ptNow.x;
    vtDir.y = sinf(fUpAngle * PI / 180.f); //ptEnemy.y - ptNow.y;
	vtDir.z = -(ptEnemy.z - ptNow.z); //z.의 방향을 반대이다.
    
    fWAngle = atan2(vtDir.z,vtDir.x) * 180.0 / PI;
    fHAngle = fUpAngle;
    
	fMaxLenDouble = vtDir.x*vtDir.x + vtDir.z*vtDir.z;
    fMaxLen = sqrtf(fMaxLenDouble) - 50.0f;
    float fValue = fMaxLen * 9.8f / (mBombProperty.fVelocity * mBombProperty.fVelocity);
	if(fValue >= 1.0 || fValue <= -1.0)
	{
        //        mbOnAmiedNeedFire = false;
		return;
	}
    
    fDivy = ptEnemy.y - ptNow.y;
    vectorLen = sqrtf(fMaxLenDouble + fDivy*fDivy);
    
    vtDir.x /= vectorLen;
    vtDir.y /= vectorLen;
    vtDir.z /= vectorLen;
    
    ptNow.y += fRadius * .5f;
    
    vDirAngle.x = fHAngle;
    vDirAngle.y = mfModelHeaderAngle;
    vDirAngle.z = fWAngle;
    
    NewMissile(ptNow,vtDir,vDirAngle,false);
    
    m_nCurrentPerTime -= GetFirePerSec();
    if(m_nCurrentPerTime < 0)
        m_bHodeFire = true;
}

bool CMissileTank::AimAutoToEnemy(SPoint* ptEnemy)
{
	if(!CanFireMissile()) return false;
    
	SVector v;
	SPoint ptMine;
    GetPosition(&ptMine);
	
	if(isMovingAutoHeader()) return false;
	
    
    mfAutoTurnAHeader_ToAngle = 0.f;
	mfModelHeaderRotVector= 0.f;
#if OBJMilkShape
	mbAutoMovePo_UpDown = false;
#endif
	
	v.x = ptEnemy->x - ptMine.x;
	v.y = ptEnemy->y - ptMine.y;
	v.z = -(ptEnemy->z - ptMine.z); //z.의 방향이 반대이기 때문에 벡터의 방향을 반대로 설정해준다.
	

    //탱크의 각도 + 포의 각도가 실제 각도이다.
    float fNowPoDir = AngleArrangeEx ( mpModel->orientation[1] + mfModelHeaderAngle );
    
	//--------------------------------
	// 포의 회전각 = 기본적으로 탱크의 방향(180 반대?) + 두점사이각
	//--------------------------------
    mfAutoTurnAHeader_ToAngle = AngleArrangeEx(atan2(v.z,v.x) * 180.0 / PI); //이 각도로 회전해야 한다.

    
    float fRotDir = CatRotation(fNowPoDir,mfAutoTurnAHeader_ToAngle);
    if(fRotDir == 0.f)
    {
//        CheckAutoMove_PoUpDown(); 미사일탱크는 updown각이 없다.
        mfModelHeaderRotVector = 0.f;
        mfAutoMovePo_UpDonwDir = 0.f;
        mbOnAmiedNeedFire = true;
    }
    else
        mfModelHeaderRotVector = fRotDir * mProperty.fHeaderTurnSize;
    return true;
}


void  CMissileTank::NewMissile(SPoint& ptNow,SVector& vtDir,SVector& vDirAngle,bool bNeedParticle)
{
    CSprite* pTargetSprite = mpAICore->GetAttackTo(mnPoIndex);
    
    CSGLCore* pCore = ((CHWorld*)mpWorld)->GetSGLCore();
    //    CSprite* pActor = pCore->GetActor();
    CBomb *pNewBomb = NULL;
    CFireParticle *pNewFireParticle = NULL;
    CBombTailParticle *pNewBombTailParticle = NULL;
    
    pNewBomb = new CBomb(pTargetSprite,this,GetID(),GetTeamID(),mBombProperty.nID,mpAction,GetWorld(),&mBombProperty);
    
    
    pNewBomb->Initialize( &ptNow, &vDirAngle,&vtDir);
    pCore->AddBomb(pNewBomb);
    
    if(bNeedParticle)
    {
        float fIntervalToActor = GetIntervalToCamera();
#ifdef ANDROID //JAVA의 터치시에 플래이가 JNI로 연결되어 잇어서 에러가 난다.
        if(fIntervalToActor < 3500)
            CScenario::SendMessage(SGL_PLAYSOUND,mBombProperty.nSoundFilreID,0);
#else
        if(fIntervalToActor < 3500)
            pCore->PlaySystemSound(mBombProperty.nSoundFilreID);
#endif
        
        pNewFireParticle = new CFireParticle(GetWorld());
        pNewBombTailParticle = new CBombTailParticle(mpWorld,pNewBomb);
        
        if(mpCurrentSelMissaileInfo)
        {
            //파티클을 추가한다.
            if(pNewFireParticle)
            {
                pNewFireParticle->Initialize(&ptNow, &vtDir);
                pCore->AddParticle(pNewFireParticle);
            }
        }
        
        //BombParticle을 추가한다.
        pNewBombTailParticle->Initialize(&ptNow, &vtDir);
        pCore->AddParticle(pNewBombTailParticle);
    }
}

void CMissileTank::RenderBeginCore_Invisiable(int nTime)
{
    //    bool bNon = true;
    if(mState == SPRITE_RUN)
    {
        
        CSprite::RenderBeginCore_Invisiable(nTime);
        
        RuntimeUpgrading(nTime);
        
        //자동으로 적 탱크로 해더를 돌려보자. (가상으로 돌린다.)
        RenderBeginAutoTurnHeader(nTime * .3f);
        
        //리소스가 스레드 안쪽에서 생성되기 때문에 이미지가 할당 안된다.
        FireMissileInVisible();
        
    }
    else if(mState == BOMB_COMPACT || mState == BOMB_COMPACT_ANI)
    {
        SetState(SPRITE_READYDELETE);
    }
}

int CMissileTank::RenderBegin(int nTime)
{
	//먼저 이동을 한ㄷ
	int nResult = E_SUCCESS;
    unsigned long nNow = GetGGTime();
	
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
        RenderBeginAutoTurnHeader(nTime * .3f);
//        RenderBeginAutoMovePo_UpDown(nTime); 미사일탱크는 updown각이 없다.
        
        
        //---------------------------------------------------------
        // 이부분이 틀리다. (거치대에 미사일이 보이게 하는지 마는지를 설정한다)
        //---------------------------------------------------------
        if(nNow > mNextPoHide1)
            mpModelHeader->SetHideMeshGroup(1, false);
        if(nNow > mNextPoHide2)
            mpModelHeader->SetHideMeshGroup(2, false);
        if(nNow > mNextPoHide3)
            mpModelHeader->SetHideMeshGroup(3, false);
        //---------------------------------------------------------

        
        mpAction->OnAutoAimed(this);
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

int CMissileTank::Render()
{
    if(GetType() == ACTORTYPE_TYPE_GHOST) return E_SUCCESS;
    return CNewTank::Render();
}

bool CMissileTank::IsTabIn(CPicking* pPick,SPoint& nearPoint,SPoint& farPoint,int nPosX,int nPosY)
{
    if(GetType() == ACTORTYPE_TYPE_GHOST) return false;
    return CNewTank::IsTabIn(pPick, nearPoint, farPoint, nPosX, nPosY);
}

bool CMissileTank::IsTabInZeroGroup(CPicking* pPick,int nPosX,int nPosY)
{
    if(GetType() == ACTORTYPE_TYPE_GHOST) return false;
    
    return CNewTank::IsTabInZeroGroup(pPick, nPosX, nPosY);
}
