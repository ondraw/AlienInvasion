//
//  CET2.cpp
//  SongGL
//
//  Created by 호학 송 on 12. 8. 31..
//
//

#include <stdlib.h>
#include <math.h>
#include "CET2.h"
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
#include "AAICore.h"
#include "CMoveAI.h"
#include "CET2AttackParticle.h"
#include "CStrategyET2AI.h"
#include "CMoveAI.h"
#include "CMutiplyProtocol.h"

CET2::CET2(unsigned int nID,unsigned char cTeamID,int nModelID,IAction *pAction,IHWorld* pWorld,PROPERTY_TANK* pProperty,NETWORK_TEAMTYPE NetworkTeamType) :
CET1(nID,cTeamID,nModelID,pAction,pWorld,pProperty,NetworkTeamType)
{
    mfRenderRotation = 0.0f;
    mptEn.x = -999999.0;
    mbAttackingToEn = false;
    mAttackParticle = NULL;
}

CET2::~CET2()
{
    mAttackParticle = NULL;
}


int CET2::LoadResoure(CModelMan* pModelMan)
{
    AAI* pMoveAI = new CMoveAI(this);
	mpAICore = new CAICore(this,pMoveAI,new CStrategyET2AI(this,pMoveAI));
    string sPath;
    sPath = mProperty.sModelPath + string("Root.ms3d");
	pModelMan->getModelf(sPath.c_str(), &mpModel);
    sPath = mProperty.sModelPath + string("Root.ani");
    if(mpModel->GetType() == MODELTYPE_MS3D)
        CAniLoader::loadAnimationf(sPath.c_str(), *(CMS3DModel*)mpModel);
    return E_SUCCESS;
}


int CET2::Initialize(SPoint *pPosition,SVector *pvDirection)
{
    mpModel->pfinalmulMaxtix = new GLfloat[16];
	sglLoadIdentityf(mpModel->pfinalmulMaxtix);
    
	mpModel->setNextAnimationf(0, GL_FALSE, GL_FALSE);

	
	mpModel->scale[0] = mProperty.fScale;
	mpModel->scale[1] = mProperty.fScale;
	mpModel->scale[2] = mProperty.fScale;
	
    
    SGL_PO_STEP = mProperty.nPoStep;
    SGL_PO_HALFSTEP = mProperty.nPoStep / 2;
    
    mfHalfX = mProperty.fHalfX;
    mfHalfZ = mProperty.fHalfZ;
    
    
    //그림자 파티클을 생성한다.
    mpShadowParticle = new CShadowParticle(this);
    mpShadowParticle->Initialize(pPosition, pvDirection);
    CSGLCore* pCore = ((CHWorld*)mpWorld)->GetSGLCore();
    pCore->AddParticle(mpShadowParticle);
    
    
    //CSprite::Initialize(pPosition, pvDirection) 가 실패할경우.. 먼저 값을 셋팅하면 셋팅한 값으로 채워진다.
    mpModel->position[0] = pPosition->x;
    mpModel->position[1] = pPosition->y;
    mpModel->position[2] = pPosition->z;
    mpModel->orientation[0] = 0;
    mpModel->orientation[1] = atan2(pvDirection->z,pvDirection->x) * 180.0 / PI;
    
    //Added By Song 2012.03.06 모델을 초기화, 위치설정할때 update()를 해야 finalMatrix가 초기화된다.
    //미사일을 발사하자마자 적이 터지는 현상은 생성되자마자 충돌검사를 할때 문제가 생긴다.
    mpModel->InitFinalMatrix();
    mnType = mProperty.nType;
    
//    CSGLCore* pCore = ((CHWorld*)mpWorld)->GetSGLCore();
    

    
    //소멸을 파티클 리스트에서 해제된다.
    mBoosterParticle = new CTE1BoosterParticle;
    mBoosterParticle->Initialize(NULL, NULL);
    pCore->AddParticle(mBoosterParticle);
    
    
    mAttackParticle = new CET2AttackParticle(mpWorld,this);
    mAttackParticle->Initialize(NULL, NULL);
    pCore->AddParticle(mAttackParticle);

    
    mpModel->createBoundingSpheref(0);
	return CSprite::Initialize(pPosition, pvDirection);
}

void CET2::CheckEnemyAttack()
{
    CSprite* pET2Target = NULL;
    float fTargetPower;
    
    if(IsAttactingToEn())
    {
        //공격을 하고 있다면...
        pET2Target = GetAttackTo();
        if(pET2Target && pET2Target->GetState() == SPRITE_RUN) //존재한다면.
        {
            fTargetPower = pET2Target->GetCurrentDefendPower();
            fTargetPower -= GetClockDeltaConst() * 0.009f; //0.1f줄게 한다.
            pET2Target->SetCurrentDefendPower(fTargetPower);
            if( fTargetPower < 0.f)
            {
                if(pET2Target->CheckAlive()) return;
                mpWorld->GetMutiplyProtocol()->SendSyncDeleteObj(pET2Target->GetID(), 0, pET2Target->IsVisibleRender());
                pET2Target->SetState(BOMB_COMPACT);
                if(pET2Target->GetPropertyTank()->nType == ACTORTYPE_TYPE_BLZ) //Blz 타입이면
                {
                    ((CHWorld*)mpWorld)->SetBlzSlotCount(pET2Target->GetModelID(), true, pET2Target);
                }
                ((CHWorld*)mpWorld)->KilledActoerToMission(pET2Target,false);
            }
        }
    }
}
void CET2::RenderBeginCore_Invisiable(int nTime)
{
    if(mState == SPRITE_RUN)
    {
        CheckEnemyAttack();
        CSprite::RenderBeginCore_Invisiable(nTime);
    }

    else if(mState == BOMB_COMPACT || mState == BOMB_COMPACT_ANI)
    {
        SetState(SPRITE_READYDELETE);
    }
    
}

void CET2::RenderBeginCore(int nTime)
{

    if(mState == SPRITE_RUN)
    {
        CheckEnemyAttack();
        CSprite::RenderBeginCore(nTime);
    }
    
}

int CET2::RenderBegin(int nTime)
{
	//먼저 이동을 한ㄷ
	int nResult = E_SUCCESS;
    if(mState == SPRITE_RUN)
    {
        if(GetIntervalToCamera() < 1000.f) //타격울 준다....
        {
            ((CHWorld*)mpWorld)->GetSGLCore()->PlaySystemSound(SOUND_ID_SAW);
        }
        
        //모델을의 방향을 변경해버리면, 자동으로 길 찾아갈때 방향을 못찾는다. 그래서 renderEnd에 복원해주자.
        float fOrgOrientation = mpModel->orientation[1];
        if(mbAttackingToEn) //공격일때는 좀더 빠르게 회전
            mfRenderRotation += nTime * 0.3f;
        else
            mfRenderRotation += nTime * 0.1f;
            
        mpModel->orientation[1] = AngleArrangeEx(mfRenderRotation);
        CSprite::RenderBegin(nTime);
        mpModel->updatef(nTime);
        mpModel->orientation[1] = fOrgOrientation; //원래값으로 복원해준다.
    }
    else if(mState == BOMB_COMPACT || mState == BOMB_COMPACT_ANI)
    {
        CampactAnimaion(nTime);
        mpModel->updatef(nTime);
    }
	return nResult;
}


int CET2::Render()
{
	if(mpModel && mState == SPRITE_RUN)
	{
        
//#ifndef ANDROID
//#if DEBUG && CGDisplayMode == 0
//        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
//        glDisable(GL_TEXTURE_2D);
//        
//		DisplayBottomStatus();
//        
//        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
//        glEnable(GL_TEXTURE_2D);
//        
//        glColor4f(1.0f,1.0f,1.0f,1.0f);
//#endif
//#endif
		mpModel->renderf();
	}
    else if(mState == BOMB_COMPACT || mState == BOMB_COMPACT_ANI)
    {
        mpModel->renderf();
    }
	return E_SUCCESS;
}


//CE2는 적에 접근하는 톱니형 탱크다.
bool CET2::AimAutoToEnemy(float fFar)
{
    if(mpAICore->GetAttackTo() == NULL)
    {
        mbAttackingToEn = false;
        if(mpModel->isAnimationRunningf())
            mpModel->setNextAnimationf(0, GL_FALSE, GL_FALSE);
        if(mAttackParticle) mAttackParticle->SetHide(true);
        return false;
    }
    
    if(fFar < 1000.f) //타격울 준다....
    {
        mbAttackingToEn = true;
        if(mAttackParticle) mAttackParticle->SetHide(false);
    }
    else if(fFar < 2000.f)
    {
        if(mpModel->isAnimationRunningf() == false)
        {
            mpModel->setNextAnimationf(1, GL_TRUE, GL_FALSE);
        }
        mbAttackingToEn = false;
        
        if(mAttackParticle) mAttackParticle->SetHide(true);
    }
    else
    {
        if(mpModel->isAnimationRunningf())
        {
            mpModel->setNextAnimationf(0, GL_FALSE, GL_FALSE);
        }
        if(mAttackParticle) mAttackParticle->SetHide(true);
        mbAttackingToEn = false;
    }
	return true;
}


void CET2::ArrangeHeaderSprite(int xHeader,int yHeader,int zHeader)
{
    //해더가 없기 때문에 정리할 것이 없다.
}


void CET2::SetState(SPRITE_STATE v)
{
    CET1::SetState(v);
    if(mAttackParticle)
    {
        mAttackParticle->SetState(v);
        if(v != SPRITE_RUN)
            mAttackParticle = NULL;
    }
}



void CET2::CampactAnimaion(int nTime)
{
    if(mnCompactAni_EscapeTime == 0)
    {
        srand(time(NULL));
        int nR = rand() % 10;
        mnCompactAni_Body_HAngle = nR + 60.f;
        mnCompactAni_Body_WAngle = nR * 30 + 65.f;
        mptCompactAni_Body_StartPos.x = mpModel->position[0];
        mptCompactAni_Body_StartPos.y = mpModel->position[1];
        mptCompactAni_Body_StartPos.z = mpModel->position[2];
    }
    mnCompactAni_EscapeTime += nTime;
    CampactAnimaionBody(nTime);
}