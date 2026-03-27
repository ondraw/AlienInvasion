/*
 *  CK9Sprite.cpp
 *  SongGL
 *
 *  Created by 호학 송 on 11. 1. 30..
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
#include <math.h>
#include <stdlib.h>
#include "CK9Sprite.h"
#include "CHWorld.h"
#include "sGLTrasform.h"
#include "CBomb.h"
#include "CParticleEmitterMan.h"
#include "CBombTailParticle.h"
#include "sGLUtils.h"
#include "CSGLCore.h"
#include "CAICore.h"
#include "CBombET1.h"
#include "CSpriteStatus.h"
#include "CShadowParticle.h"
#include "AAI.h"
#include "CMoveAI.h"
#include "CStrategyAI.h"
#include "CTankModelASCII.h"
#include "CAniRuntimeEnergyUP.h"
#include "CParticleEmitterMan.h"
#include "CBombGun.h"
#include "CAniRuntimeBubble.h"
#include "CCircleUpgradeMark.h"
#include "CMutiplyProtocol.h"
#include "CStrategyActorAI.h"
//#define K9_Half_X 63.0
//#define K9_Half_Y 90.0


//#define K9_HEADERX 0		//몸통이 0,0,0 위치에 회전이 없을때 탱크의 머리는 (0,48)의 위치에 있다.
//#define K9_HEADERY 48		//몸통이 0,0,0 위치에 회전이 없을때 탱크의 머리는 (0,48)의 위치에 있다.
//#define K9_HEADERZ 0

//#define K9_SCALE 0.06f

//#define K9_HEADERTRUNSIZE 0.6f
//#define K9_FIREANIMATION_ACC 0.25f   //탱크의 파이어시 해더의 움직임 속도 조절
//#define K9_FIREANIMATION_MAX 30.0f  //탱크의 파이어시 해더의 뒤로 제껴지는 맥스값.

CK9Sprite::CK9Sprite(unsigned int nID,unsigned char cTeamID,int nModelID,IAction *pAction,IHWorld* pWorld,PROPERTY_TANK* pProperty,NETWORK_TEAMTYPE NetworkTeamType) : CSprite(nID,cTeamID,nModelID,pAction,pWorld,pProperty,NetworkTeamType)
{
	mpModel = NULL;
	mpModelHeader = NULL;
	mfModelHeaderAngle = 0.0f;
	mfModelHeaderRotVector= 0.0f;	
#if OBJMilkShape
	mnPoAniStep = 10; //단계
    mbAutoMovePo_UpDown = false;
#endif
    mnPoUpDownState = 0;
	
	//--------------------------
	//자동화 타겟으로 ....
	//--------------------------
    mpCurrentSelMissaileInfo = NULL;
    
    mptLastPos.x = NONE_VALUE;
    mptStartMissile = NULL;
    mnPoStartCnt = 0;
    
    mnPoIndex = 0;
    mpBubbleParticle = NULL;
    mfAutoMovePo_ToUpDownAngle = 0.f;
    mfAutoMovePo_UpDonwDir = 0.f;
    
    mbOnAmiedNeedFire = false;
    memset(&mptCompactAni_Header_StartPos,0,sizeof(SPoint));
    mnCompactAni_EscapeTime = 0;
    memset(&mptCompactAni_Body_StartPos,0,sizeof(SPoint));
    mExcuteCmd = 0;
    mnFireInterval = 0;
    mnMapID = -1;
    
    mnGunStartCnt= 0;
    mptStartGun = NULL;
    mnFireGunInterval = 0;
    
    if(pProperty->nGunID != 0)
    {
        //폭탄정보를 가져온다.
        PROPERTY_BOMB::GetPropertyBomb(pProperty->nGunID, mGunProperty);
    }
    mlBirthTime = GetGGTime();
    

    mptUserClickForInvisible.x = -9999.f;
    mptUserClickForInvisible.y = 0.f;
    mptUserClickForInvisible.z = 0.f;
    
#ifdef ACCELATOR
    mfAccelatorViewDirY = 0.f;
    mfAccelatorViewDirVector = 0.f;
    mAccelatorViewDirBefor.x = 1.f;
    mAccelatorViewDirBefor.y = -0.40;
    mAccelatorViewDirBefor.z = 1.f;
#endif //ACCELATOR
}

CK9Sprite::~CK9Sprite()
{
	if(mpModel)
	{
		delete mpModel;
		mpModel = NULL;
	}
	
	if(mpModelHeader)
	{
		delete mpModel;
		mpModelHeader = NULL;
	}
    
    if(mptStartMissile)
    {
        int nCnt = mnPoStartCnt;
        for (int i = 0; i < nCnt; i++)
        {
            delete[] mptStartMissile[i];
        }
        delete[] mptStartMissile;
        mptStartMissile = NULL;
    }
    
    if(mptStartGun)
    {
        int nCnt = mnGunStartCnt;
        for (int i = 0; i < nCnt; i++)
        {
            delete[] mptStartGun[i];
        }
        delete[] mptStartGun;
        mptStartGun = NULL;
    }
}

//---------------------------------------------------------------------
//Start CSprite Virtual

//Data를 읽어온다.
int CK9Sprite::LoadResoure(CModelMan* pModelMan)
{
//	mpModel = new CMS3DModel;
    AAI* pMoveAI = new CMoveAI(this);
	mpAICore = new CAICore(this,pMoveAI,new CStrategyAI(this,pMoveAI));
    
    string sPath;
    if(mProperty.nType == 1) //탱크
        sPath = mProperty.sModelPath + string("Root.ms3d");
    else //날라다니는 것
        sPath = mProperty.sModelPath + string("Root.txt");
    
	pModelMan->getModelf(sPath.c_str(), &mpModel,true);
    
    sPath = mProperty.sModelPath + string("Root.ani");
    
    if(mpModel->GetType() == MODELTYPE_MS3D)
    {
        CAniLoader::loadAnimationf(sPath.c_str(), *(CMS3DModel*)mpModel);
    }
    
#if OBJMilkShape
	mpModelHeader = new CMS3DModel;
    sPath = mProperty.sModelPath + string("Header.ms3d");
	pModelMan->getModelf(sPath.c_str(), *mpModelHeader);
    sPath = mProperty.sModelPath + string("Header.ani");
	CAniLoader::loadAnimationf(sPath.c_str(), *mpModelHeader);
#else
    sPath = mProperty.sModelPath + string("Header.txt");
//    mpModelHeader = new CTankModelASCII(((CHWorld*)mpWorld)->GetSGLCore()->GetMS3DModelASCIIMan()->Load(sPath.c_str(),true));
    pModelMan->getModelfTankHeader(sPath.c_str(), (CMyModel**)&mpModelHeader,true);
    
#endif
    
    if(mpWorld->GetUserInfo())
        mnMapID = mpWorld->GetUserInfo()->GetLastSelectedMapID(); //달맵은 절벽이 없이 잘 움직이게 해주자.
    
	
	return E_SUCCESS;
}

void CK9Sprite::ParsMisailStartPos()
{
    if(mProperty.sMisailStartPos)
    {
        char* sTemp = new char[strlen(mProperty.sMisailStartPos) + 1];
        strcpy(sTemp, mProperty.sMisailStartPos);
        char* sStar = strstr(sTemp, "*");
        char* sStar2 = NULL;
        
        if(sStar) //미사일
        {
            *sStar = 0;
            //sTemp //부스터 위치
//            PosParse(sTemp,&mnBoosterCnt,mpBoosterPos);
            
            sStar2 = strstr(sStar + 1, "*"); //총위 위치
            if (sStar2)
            {
                *sStar2 = 0;
                //총의 위치
                sStar2+=1;
                PosParse(sStar2,&mnGunStartCnt,mptStartGun);
            }
            sStar += 1;
            //미사일 위치
            PosParse(sStar,&mnPoStartCnt,mptStartMissile);
            
        }
        else //부스터 위치
        {
//            PosParse(sTemp,&mnBoosterCnt,mpBoosterPos);
        }
        delete[] sTemp;
        
#if !OBJMilkShape
        mpModelHeader->SetPointStatrMissile(mptStartMissile);
#endif
    }
}

int CK9Sprite::Initialize(SPoint *pPosition,SVector *pvDirection)
{
    mpModel->pfinalmulMaxtix = new GLfloat[16];
	sglLoadIdentityf(mpModel->pfinalmulMaxtix);

    mpModelHeader->pfinalmulMaxtix = new GLfloat[16];
    sglLoadIdentityf(mpModelHeader->pfinalmulMaxtix);
    
#if OBJMilkShape
    mpModelHeader->setNextAnimationf(mnPoAniStep, GL_FALSE, GL_FALSE);
#endif
    
	mpModel->setNextAnimationf(2, GL_FALSE, GL_FALSE);
	
	
	mpModel->scale[0] = mProperty.fScale;
	mpModel->scale[1] = mProperty.fScale;
	mpModel->scale[2] = mProperty.fScale;
	
	mpModelHeader->scale[0] = mProperty.fScale;
	mpModelHeader->scale[1] = mProperty.fScale;
	mpModelHeader->scale[2] = mProperty.fScale;
    
    
    mHeaderPosition[0] = mProperty.fHeaderPosX;
    mHeaderPosition[1] = mProperty.fHeaderPosY;
    mHeaderPosition[2] = mProperty.fHeaderPosZ;
    
    mFireAnimationVector = 0.0f;

    
    mFireMovedHeader = 0.0f;
    
    SGL_PO_STEP = mProperty.nPoStep;
    SGL_PO_HALFSTEP = mProperty.nPoStep / 2;
    
    mfHalfX = mProperty.fHalfX;
    mfHalfZ = mProperty.fHalfZ;
    
    
    //그림자 파티클을 생성한다.
    mpShadowParticle = new CShadowParticle(this);
    mpShadowParticle->Initialize(pPosition, pvDirection);
    CSGLCore* pCore = ((CHWorld*)mpWorld)->GetSGLCore();
    pCore->AddParticle(mpShadowParticle);
    

    mpModel->position[0] = pPosition->x;
    mpModel->position[1] = pPosition->y;
    mpModel->position[2] = pPosition->z;

    mpModel->orientation[1] = atan2(pvDirection->z,pvDirection->x) * 180.0 / PI;

    //Added By Song 2012.03.06 모델을 초기화, 위치설정할때 update()를 해야 finalMatrix가 초기화된다. 
    //미사일을 발사하자마자 적이 터지는 현상은 생성되자마자 충돌검사를 할때 문제가 생긴다.
    mpModel->InitFinalMatrix();

    //PoPosition
    //-----------------------------------------------
    //CK9Sprite
    //-----------------------------------------------
    ParsMisailStartPos();
    
    mnType = mProperty.nType;
    
    memset(&mBeforeOwnerPos, 0, sizeof(SPoint));
    memset(&mBeforeTargetPos, 0, sizeof(SPoint));
    mBeforeLenght = -1;
    mBeforeResult = -1;
    
    InitRuntimeUpgrade();
    
    

    mpModel->createBoundingSpheref(0);
    
//    mpCircleUpgradeMark = new CCircleUpgradeMark(mpWorld,this,10.f);
//    mpCircleUpgradeMark->Initialize(pPosition, pvDirection);
//    pCore->AddParticle(mpCircleUpgradeMark);

	return CSprite::Initialize(pPosition, pvDirection);
}

void CK9Sprite::ExcuteCmd()
{
    mshMainTowerBubbleCntInRange = 0;
    if(mExcuteCmd & SGL_NEED_EXECUTE_IN_DMAINTOWER) //메인타워 안쪽으로 드러가서 에너지를 필할수 있게 하였다.
    {
        float fPower = GetMaxDefendPower() / 10.f;
        SetCurrentDefendPower(GetCurrentDefendPower() + fPower);
        CHWorld* pWorld = (CHWorld*)GetWorld();
        
        //Added By Song 2014.05.20 이전버전에서 죽었다. 쓰레드 아닐때만 화면에 셋해야 한다. 그렇지 않으면 Render()시에 죽어 버린다.
        if(IsVisibleRender())
        {
            if(GetIntervalToCamera() < 3500.f)
                pWorld->GetSGLCore()->PlaySystemSound(SOUND_ID_EnergyUP);
            
            //Fill Up~~~~을 할경우..
            SPoint pt2;
            GetPosition(&pt2);
            CAniRuntimeEnergyUP *pItem = new CAniRuntimeEnergyUP(mpWorld,this);
            pItem->Initialize(&pt2, NULL);
            CSGLCore *pCore = ((CHWorld*)mpWorld)->GetSGLCore();
            pCore->AddParticle(pItem);
        }
        mExcuteCmd &=~SGL_NEED_EXECUTE_IN_DMAINTOWER;
    }
    else if(mExcuteCmd & SGL_NEED_EXECUTE_CHGOLD_IN_DMAINTOWER)
    {
        //------------------------------------------------------
        //Added By Song 2014.05.20 이전버전에서 죽었다. 쓰레드 아닐때만 화면에 셋해야 한다. 그렇지 않으면 Render()시에 죽어 버린다.
        //Thread 밖에서 거시기해야한다. (로직을 추가하였으니 테스트를 해봐야한다.)
        //------------------------------------------------------
        if(mpAICore->IsActor()) //주인공이고 메인타워 안쪽에 드러왔다면, 돈을 올려준다.
        {
            if(IsVisibleRender())
            {
                CHWorld* pWorld = (CHWorld*)GetWorld();
                pWorld->TransferScoreToGold();
            }
            else
                CScenario::SendMessage(SGL_CHGOLD_IN_DMAINTOWER,0,0);
        }
        //------------------------------------------------------
        
        mExcuteCmd &= ~SGL_NEED_EXECUTE_CHGOLD_IN_DMAINTOWER;
        mshMainTowerBubbleCntInRange = mExcuteCmd;
        mExcuteCmd = 0;
    }
    
}

void CK9Sprite::RenderBeginCore_Invisiable(int nTime)
{

    if(mState == SPRITE_RUN)
    {
        CSprite::RenderBeginCore_Invisiable(nTime);
        float isMove = mfModelHeaderRotVector; //이전에는 이동이였는데 턴하고나니 멈추었는지를 확인한다.
        RuntimeUpgrading(nTime);
        
        //자동으로 적 탱크로 해더를 돌려보자. (가상으로 돌린다.)
        RenderBeginAutoTurnHeader(nTime * .3f);
        
        //보이지 않는 영역에서는 isMovingAutoHeader이면 적에게 공격한다.
        if(
           (isMove != 0.f && mfModelHeaderRotVector == 0.f) //사용자가 클릭했을 경우
           ||
           (mfModelHeaderRotVector == 0.f && mpAICore && mpAICore->GetAttackTo() //공격 대상이 존재 한다면.
            )
           )
        {
            mfAutoMovePo_UpDonwDir = 0.f;

            //리소스가 스레드 안쪽에서 생성되기 때문에 이미지가 할당 안된다.
            FireMissileInVisible();
        }
        
        FireGunInvisible();
        
    }
    else if(mState == BOMB_COMPACT || mState == BOMB_COMPACT_ANI)
    {
        SetState(SPRITE_READYDELETE);
    }
}

int CK9Sprite::RenderBegin(int nTime)
{
	//먼저 이동을 한ㄷ
	int nResult = E_SUCCESS;
	
    
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
        
        FireGun();
        
        RuntimeUpgrading(nTime);
        
        //파이어시 애니메이션을 해준다.
        RenderBeginFireAnimation(nTime,FireAnimationMax);
        mpModel->updatef(nTime);
        RenderBeginAutoTurnHeader(nTime * .3f);
        RenderBeginAutoMovePo_UpDown(nTime);
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

void CK9Sprite::RenderBeginCore(int nTime)
{
    //마지막 매트릭스를 구해온다.
    mpModel->calFinallyMatrix();
    CSprite::RenderBeginCore(nTime);
}


int CK9Sprite::Render()
{
	if(mpModel && mState == SPRITE_RUN)
	{

        if(mnReliveEffect == 0)
        {
            
//#ifndef ANDROID
//#if DEBUG && CGDisplayMode == 0
//            
//            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
//            glDisable(GL_TEXTURE_2D);
//            
//            DisplayBottomStatus();
//            
//            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
//            glEnable(GL_TEXTURE_2D);
//            
//            glColor4f(1.0f,1.0f,1.0f,1.0f);
//#endif
//#endif //ANDROID
            


            mpModel->renderf();
            mpModelHeader->renderf();
        }
        else //깜빡이게 하자.
        {
            bool bVisible = false;
            //CHWorld* pW = (CHWorld*)mpWorld;
            unsigned long nowGGT = GetGGTime();
            
            long lT = nowGGT % 1600l;
            if(lT < 400l)
                bVisible = true;
            else if(lT < 800l)
                bVisible = false;
            else if(lT < 1200l)
                bVisible = true;
            else
                bVisible = false;
            
            if(bVisible)
            {
                mpModel->renderf();
                mpModelHeader->renderf();
            }
            
            if(nowGGT - mnReliveEffect > 4000l)
            {
                mnReliveEffect = 0l;
                if(mpShadowParticle)
                    mpShadowParticle->SetReliveEffect(false);
            }
        }
	}
    else if(mState == BOMB_COMPACT || mState == BOMB_COMPACT_ANI)
    {
        mpModel->renderf();
        mpModelHeader->renderf();
    }
	return E_SUCCESS;
}

int CK9Sprite::RenderEnd()
{
#if VIEWROTATION_ANI
    if(mpWorld->GetActor() == this)
    {
        CHWorld* pWorld = (CHWorld*)mpWorld;
        if(mfCameraTurnVector != 0.0f && !pWorld->IsViewRotationByTouch())
        {
//            HLogD("dddd = %f,%f,%f\n",mfCameraTurnVector,GetClockDeltaConst() * 0.08f,0.01f * GetClockDeltaConst());
            //카메라를 움직여준다.
            if(mfCameraTurnVector < 0)
            {
                if(mfCameraTurnVector < -30.f)
                    mfCameraTurnVector -= mfCameraTurnVector * 0.5f;
                else if(mfCameraTurnVector < -3.f)
                    mfCameraTurnVector += 1.28f;    //GetClockDeltaConst() * 0.08f;
                else
                    mfCameraTurnVector += 0.16f;    //0.01f * GetClockDeltaConst();
                
                if(mfCameraTurnVector >= 0.f)
                    mfCameraTurnVector = 0.0f;
                HandleCamera(0.0f, mfCameraTurnVector);
            }
            else if(mfCameraTurnVector > 0)
            {
                if(mfCameraTurnVector > 30.f)
                    mfCameraTurnVector -= mfCameraTurnVector * 0.5f;
                else if(mfCameraTurnVector > 3.f)
                    mfCameraTurnVector -= 1.28f;    //GetClockDeltaConst() * 0.08f;
                else
                    mfCameraTurnVector -= 0.16f;    //0.01f * GetClockDeltaConst();
                
                if(mfCameraTurnVector <= 0.f)
                    mfCameraTurnVector = 0.0f;
                HandleCamera(0.0f, mfCameraTurnVector);
            }
#ifdef ACCELATOR
            else
                HandleAccelCamera();
#endif //ACCELATOR
            
        }
#ifdef ACCELATOR
        else
            HandleAccelCamera();
#endif //ACCELATOR

        

    }
#endif
	return E_SUCCESS;
}

void CK9Sprite::ResetPower(bool bRunTimeUpgrade)
{
    CSprite::ResetPower(bRunTimeUpgrade);
    //CGhostSprite ...
//    if(GetModelID() != -1) //고스트가 아닐때..
    if(!(GetType() & ACTORTYPE_TYPE_GHOSTANITEM)) //고스트가 아닐때..
    {
        if(bRunTimeUpgrade == false)
            ((CHWorld*)mpWorld)-> OnSelectedObjectShowDescBar(this);
        else
        {
            ((CHWorld*)mpWorld)-> OnSelectedObjectShowDescBar(this,SGLTextUnit::GetOnlyText("Completed Level Upgrade"));
            
            if(mpCircleUpgradeMark == NULL)
            {
                CSGLCore* pCore = ((CHWorld*)mpWorld)->GetSGLCore();
                mpCircleUpgradeMark = new CCircleUpgradeMark(mpWorld,this,10.f);
                mpCircleUpgradeMark->Initialize(NULL, NULL);
                pCore->AddParticle(mpCircleUpgradeMark);
            }
            else
            {
                mpCircleUpgradeMark->SetCircleCount(mpCircleUpgradeMark->GetCircleCount() + 1);
            }
        }
    }
}

bool CK9Sprite::SetPosition(SPoint* sp)
{
    SPoint ptBefore;
    int nResult = 0;
	
    //땅위에 0.9높혀서 지형의 높이가 1이하인것에 가리지 않게 한다.
    sp->y += SPRITE_GROUND_HEIGHT;
    
	if(mpModel)
	{
        ptBefore.x = mpModel->position[0];
        ptBefore.y = mpModel->position[1];
        ptBefore.z = mpModel->position[2];
        
        if(!CSprite::SetPosition(sp)) return false;
        
                
		mpModel->position[0] = sp->x;
		mpModel->position[1] = sp->y;
		mpModel->position[2] = sp->z;
        
        
        nResult = ArrangeSprite();
		if(nResult != E_SUCCESS)
        {
            if(mptLastPos.x != NONE_VALUE)
            {
                mpModel->position[0] = mptLastPos.x;
                mpModel->position[1] = mptLastPos.y;
                mpModel->position[2] = mptLastPos.z;
                CSprite::SetPosition(&mptLastPos);
                return false;
            }
            else 
            {
                //무한루프 돈다.
                HLogE("위치를 안전한 곳을 찾아서 옮겨주어야 한다.\n");
                return false;
            }
            
        }
        memcpy(&mptLastPos,sp,sizeof(SPoint));
	}
    
    return true;
}

CSprite* CK9Sprite::GetAttackTo(){ return mpAICore->GetAttackTo();}
void CK9Sprite::DontRotationHeader(float fAngle)
{
    
    SetHeaderRotation(AngleArrangeEx(mfModelHeaderAngle - fAngle));
}
void CK9Sprite::OnCompletedUpdated(RUNTIME_UPGRADE* prop,CArchive* pReadArc)
{
    if(prop->nID == RUNTIME_UPDATE_GUNMACHINE)
    {
        if(mptStartGun == NULL)
        {
         
            //*1,1,170*0,38,40
            mptStartGun = new int*[1];
            mptStartGun[0] = new int[3];
            mptStartGun[0][0] = 0;
            mptStartGun[0][1] = 38;
            mptStartGun[0][2] = 40;
        }
        return;
    }
    else
        CSprite::OnCompletedUpdated(prop,pReadArc);
}

bool CK9Sprite::AvoidDontGoArea()
{
    SPoint ptNow,ptTemp;
    float fRadius = GetRadius();;
    
    //fRadius = fRadius * fRadius;
    GetPosition(&ptNow);
    
    //맵은 동서남북 직선형으로 되어 있으서 x,y +-를 사용하면 될거 같다.
    memcpy(&ptTemp, &ptNow, sizeof(SPoint));
    ptTemp.x += fRadius;
    if(SetPosition(&ptTemp)) return true;
    
    memcpy(&ptTemp, &ptNow, sizeof(SPoint));
    ptTemp.x -= fRadius;
    if(SetPosition(&ptTemp)) return true;
    
    memcpy(&ptTemp, &ptNow, sizeof(SPoint));
    ptTemp.z += fRadius;
    if(SetPosition(&ptTemp)) return true;
    
    memcpy(&ptTemp, &ptNow, sizeof(SPoint));
    ptTemp.z -= fRadius;
    if(SetPosition(&ptTemp)) return true;
    
    HLogE("[Error] 회전시 갈수 있는 땅의 위치를 벗어났다\n");
    return false;
}

void CK9Sprite::SetModelDirection(SVector* pvd)
{
    float fBeforeDir;
    int nResult = 0;
    
	if(mpModel)
	{
        //이동하기전에 이전 위치를 저장하여 이동 못하는 위치이면 다시 롤백하여준다.
        fBeforeDir = mpModel->orientation[1];
        
		//	라디안 = 각도 * Math.PI / 180;
		//	각도 = 라디안 * 180 / Math.PI;
		//	Yaw:y Pich:x Roll:z 축회전
		//mpModel->orientation[0] = 0;										//Pich	= x
		//mpModel->orientation[1] = 0;										//Yaw	= y
		mpModel->orientation[1] = atan2(pvd->z,pvd->x) * 180.0 / PI;		//Roll	= z
        nResult = ArrangeSprite();
        
		if(nResult != E_SUCCESS)
        {
            if(AvoidDontGoArea() == false)
            {
                mpModel->orientation[1] = fBeforeDir;
                CSprite::SetModelDirection(pvd);
                return ;
            }
        }
	}
    
	CSprite::SetModelDirection(pvd);
}


//주의 모델의 처음 조인트에 정점을 할당해야 한다.
//bExact: true 는 버그가 있다. (위치에 따라. 충돌되었어도 검사를 못하는 경우가 있다.)
bool CK9Sprite::CollidesBounding(CSprite* Sprite,bool bExact)
{
    bool OnlyFalse = false;
    GLbyte m[1];
    GLbyte outm;
    GLbyte outm2;
    int nSize = 1;
    m[0] = 0;
    
    if(Sprite->GetType() == ACTORTYPE_FIGHTER)
    {
        Sprite->CollidesBounding(this,bExact);
        mfColidedDistance = Sprite->GetColidedDistance();
        return false; //비행기는 탱크와 부딛힐일이 없다.
    }
    
    //회피모드일때는 무조거 충돌이 안되게 하자꾸나.
    if(mpAICore && mpAICore->GetMoveAI()->GetDonComact())
    {
        OnlyFalse = true; //여기서 리턴을 해버리면 mfColidedDistance 계산을 하지 않음.
    }
    
    bool bCom;
    void *em = Sprite->GetModel();
    if(em == NULL)
    {
        bCom = Sprite->CollidesBounding(this,bExact);
        mfColidedDistance = Sprite->GetColidedDistance();
        return OnlyFalse ? false : bCom;
    }
    
    
    
	if(!bExact)
    {
		if(mpModel->collidesBoundingSpheref(0, 0, 0, *(CMyModel*)em, 0, 0, 0, false, -2))
        {
            mfColidedDistance = mpModel->GetCompactOtherSpriteInterval();
            Sprite->SetColidedDistance(mfColidedDistance); //상대도 같은 위치
            return OnlyFalse ? false : true;
        }
        mfColidedDistance = mpModel->GetCompactOtherSpriteInterval();
        Sprite->SetColidedDistance(mfColidedDistance); //상대도 같은 위치
        return false;
    }
    
    
    if(mpModel->collidesBoundingSpheref(m, nSize, &outm, *(CMyModel*)em, m, nSize, &outm2, true, 0))
    {
        mfColidedDistance = mpModel->GetCompactOtherSpriteInterval();
        Sprite->SetColidedDistance(mfColidedDistance); //상대도 같은 위치
        return OnlyFalse ? false : true;
    }

    mfColidedDistance = mpModel->GetCompactOtherSpriteInterval();
    Sprite->SetColidedDistance(mfColidedDistance); //상대도 같은 위치
    return false;
}


int CK9Sprite::OnEvent(SGLEvent *pEvent)
{
	int nMsgID = pEvent->nMsgID;
    
//    if(pEvent->nMsgID == GSL_VIEW_MOVEUP)
//    {
//        mfCameraUpDownVector = (float)(pEvent->lParam) / 1000.0f;
//        HandleCamera(mfCameraUpDownVector,mfCameraTurnVector);
//        return E_SUCCESS;
//    }
//    else if(pEvent->nMsgID == GSL_VIEW_MOVEDOWN)
//    {
//        mfCameraUpDownVector = -(float)(pEvent->lParam) / 1000.0f;
//        HandleCamera(mfCameraUpDownVector,mfCameraTurnVector);
//        return E_SUCCESS;
//    }
//    else if(pEvent->nMsgID == GSL_VIEW_MOVESTOP)
//    {
//        mfCameraUpDownVector = 0.0f;
//        return E_SUCCESS;
//    }
//	else
    if(nMsgID == GSL_THEADER_TRUNLEFT)
    {
        mfModelHeaderRotVector = pEvent->lParam / 1000.0f;
    }
	else if( nMsgID == GSL_THEADER_TRUNRIGHT)
    {
        mfModelHeaderRotVector = -pEvent->lParam / 1000.0f;
    }
	else if( nMsgID == GSL_THEADER_STOP)
	{
//		//자동으로 움직이고 있을때 종료하지 못하게 막는다.
//		if(!mbAutoMoveHeader_Trun)
//			mfModelHeaderRotVector = 0.0f;
        //@@ 확인 필요 : 주인공 탱크의 해더가 계속 돌아가는 현상
        mfModelHeaderRotVector = 0.0f;
        
	}
    else if( nMsgID == GSL_DONT_AUTO_FIRE)
    {
        CSprite* pOwner = (CSprite*)pEvent->lObject;
        if(pOwner && pOwner->GetAICore())
        {
            pOwner->GetAICore()->Command(AICOMMAND_DONT_AUTO_FIRE, NULL);
        }
        else if(mpAICore && mpAICore->IsActor())
            mpAICore->Command(AICOMMAND_DONT_AUTO_FIRE, NULL); //자동으로 발사 못하게 막자.
    }
	else if( nMsgID == GSL_BOMB_FIRE)
	{
#if !PATH_TEST
        FireMissile();
#endif //PATH_TEST
	}
	else if( nMsgID == GSL_PO_UP)
	{
#if OBJMilkShape
		//0~18
		if(mnPoAniStep > 0 && mnPoAniStep < SGL_PO_HALFSTEP)
		{
            mnPoUpDownState = 1; //위로 움직임
		}
		else if(mnPoAniStep >= SGL_PO_HALFSTEP && mnPoAniStep < SGL_PO_STEP)
		{
            mnPoUpDownState = 1; //위로 움직임
		}
#else
//        mfAutoMovePo_UpDonwDir = 1.0f; //위아래로 움직이고 멈추면 발사하기 때문에 ...제거함.
#endif
	}
	else if( nMsgID == GSL_PO_DOWN)
	{
#if OBJMilkShape
		//0~18
		if(mnPoAniStep < SGL_PO_HALFSTEP)
		{
            mnPoUpDownState = -1; //아래로 움직임
		}
		else if(mnPoAniStep > SGL_PO_HALFSTEP)//18이상일때 
		{
            mnPoUpDownState = -1; //아래로 움직임
		}
#else
//        mfAutoMovePo_UpDonwDir = -1.0f; //위아래로 움직이고 멈추면 발사하기 때문에 ...제거함.
#endif
		
	}
    else if( nMsgID == GSL_PO_STOP_UPDOWN)
    {
        mnPoUpDownState = 0;
    }
	else
    {
		return CSprite::OnEvent(pEvent);
    }
    return E_SUCCESS;
}

void CK9Sprite::ChangeCameraType()
{
    int nCameraType = GetCameraType();
    nCameraType++;
    if( nCameraType > CAMERT_TANKHEADER)
        nCameraType = CAMERT_BACK;
    SetCameraType(nCameraType);
}

void CK9Sprite::SetCameraType(int v) 
{ 
    SetK9CameraType(v);
}

CSprite* CK9Sprite::GetTargetBySensor()
{
    SPoint pt2;
    SPoint ptMe;
    float fA;
    float fx,fz;
    float flen2,fAngle;
    int mMyTeam = GetTeamID();
    float fMaxLen = RADAVISIBLE * RADAVISIBLE;
    CHWorld* pWorld = (CHWorld*)mpWorld;
    CSGLCore* pCore = pWorld->GetSGLCore();
    float fMin = 99999999.0f;
    CSprite* pMinSprite = NULL;
    GetPosition(&ptMe);
    CListNode<CSprite>* pList = pCore->GetSpriteList();
    ByNode<CSprite>* b = pList->begin();
    
    float fWAngle,fHAngle,vtPoUpPosition[3],vtPoDownPosition[3];
    SVector vtDir;
    GetPoAngles(0,&fWAngle,&fHAngle,vtPoUpPosition,vtPoDownPosition,&vtDir);
    
    
    //이공식의 역으로 계산하면... 거리를 구할 수가 있다.
    //fValue = fLen * 9.8f / (property.fVelocity * property.fVelocity);
	//float fHeightAngleRadian = asinf(fValue) / 2.0f;
    //---------------------------------------------------
    //    float fTargetLen = sinf(fHAngle * PI / 90.f) * (property.fVelocity * property.fVelocity) / 9.8f - 40.f;
    //    fTargetLen = 1.0f;
    //----------------------------------
    for(ByNode<CSprite>* it = b; it != pList->end(); )
    {
        CSprite* sp = it->GetObject();
        
        //CGhostSprite ...
        //아이템 스프라이트 이거나.
//        if(sp->GetState() != SPRITE_RUN || sp->GetModelID() == -1)
        if(sp->GetState() != SPRITE_RUN || sp->GetType() & ACTORTYPE_TYPE_GHOSTANITEM)
        {
            it = it->GetNextNode();
            continue;
        }
        
        
        if(pWorld->IsAllians(mMyTeam, sp->GetTeamID()) == false) //적일 경우
        {
            sp->GetPosition(&pt2);
            
            fx = pt2.x - ptMe.x;
            fz = -(pt2.z - ptMe.z);
            
            flen2 = fx * fx + fz * fz;// + fz * fz;
            if (flen2 <= fMaxLen)
            {
                fA =  atan2(fz,fx) * 180.0 / PI;
                fAngle = fA - fWAngle;   //GetOrientationToAttack();
                if(fAngle > -60.f && fAngle < 60.0f)
                {
                    //각도가 정확한것이 우선이다.
                    if(fAngle > -10.f && fAngle < 10.0f)
                    {
                        pMinSprite = sp;
                        break;
                    }
                    //그다음 가장가까운것이다.
                    else if(flen2 < fMin)
                    {
                        fMin = flen2;
                        pMinSprite = sp;
                    }
                }
            }
        }
        it = it->GetNextNode();
    }
    
    return pMinSprite;
}

//End CSprite Virtual
//---------------------------------------------------------------------



// --------------------------------------------------------------
//두점 사이의 거리를 계산한다.
//1>float fY = cosA * V * fTime; 
//  거리 = cosA * V * fTime
//  fTime = 거리 / (cosA * V)
//2>fZ = fVz0 * fTime - (9.8 * fTime * fTime) / 2;
//   0 = sinA * V * fTime - 4.9 * fTime*fTime
//   0 = fTime(sinA * V - 4.9 *fTime)
//   0 = sinA * V - 4.9 * fTime
//   fTime = sinA * V / 4.9
//3> 1 와 2 공식을 푼다.
//   거리 / (cosA * V) = sinA * V / 4.9
//   거리 = sinA * V / 4.9 * (conA * V)
//   거리 * 4.9 / V*V = sinA*conA
//   sin2A = 2sinAcosA => sinAcosA = sin2A / 2
//   거리 * 4.9 / V*V = sin2A / 2
//   거리 * 9.8 / V*V = sin2A
//   조건  -1 <= 거리 * 9.8 / V*V <= 1
//   asin(거리 * 9.8 / V * V) = 2A
//   A = asin(거리 * 9.8 / V * V) / 2
// ---------------------------------------------------------------
// 정리 : 두 물체의 거리와 초기 던지는 속도를 알면 던지는 각도를 구할 수가 있다.
// ---------------------------------------------------------------
bool CK9Sprite::AimAutoToEnemy(CSprite *pEnemy)
{
	SPoint ptEnemy;
    pEnemy->GetPosition(&ptEnemy);
	return AimAutoToEnemy(&ptEnemy);
}

float CK9Sprite::CatRotation(float fNowAngle,float fToAngle)
{
    float fDifAngle = AngleArrangeEx(fToAngle - fNowAngle);
    if(fDifAngle <= 1.f || (fDifAngle <= 365.f && fDifAngle >= 364.f)) //거의 정확한 방향으로 되어 있다면.
	{
        return 0.f; //정진
	}
	else
	{
		if(fDifAngle < 180)
			return 1.0f; //오른쪽 회전
		else
			return -1.0f; //왼쪽 회전
	}
    
}

bool CK9Sprite::AimAutoToEnemy(SPoint* ptEnemy)
{
	if(!CanFireMissile() || isMovingAutoHeader()) return false;
    
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
	
    float fLen = sqrtf(v.x*v.x + v.z*v.z) - 50.0f;
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
    
    

	// -------------------------------
	//두 점사이의 거리....
	// -------------------------------
	//포가 공중에 있는데 너무 멀리간다.(- 50.f) 정확하지 않지만 거리를 조정해준다.
	float fValue = fLen * 9.8f / (mBombProperty.fVelocity * mBombProperty.fVelocity);
	
	if(fValue >= 1.0 || fValue <= -1.0)
	{
        mbOnAmiedNeedFire = false;
		return false;
	}
    
    //+asinf(v.y) 높이/빞변 v.y = v.y/fLenT 평자가 아니라 산악일때
	float fHeightAngleRadian = asinf(fValue) / 2.0f + asinf(v.y);
	mfAutoMovePo_ToUpDownAngle = 180 * fHeightAngleRadian / PI;
    

    //---------------------------------------
    //Added By Song 2014.03.16 미사일 스타일
    if(mBombProperty.nBombType == 1) //미사일 스타일
    {
        if(mfAutoMovePo_ToUpDownAngle < 60)
            mfAutoMovePo_ToUpDownAngle = 60;
    }
    //---------------------------------------
    
    float fRotDir = CatRotation(fNowPoDir,mfAutoTurnAHeader_ToAngle);
    if(fRotDir == 0.f)
    {
        CheckAutoMove_PoUpDown();
        mfModelHeaderRotVector = 0.f;
    }
    else
    {
        mfModelHeaderRotVector = fRotDir * mProperty.fHeaderTurnSize;
        
        //TODO:: 사용자가 클릭한 위치를 기억해서 FireInvisible에서 GetAttackTo()가 없으면 이것으로 대체를 해준다.
        memcpy(&mptUserClickForInvisible, ptEnemy, sizeof(SPoint));
    }
    
    if(mfModelHeaderRotVector == 0.f && mfAutoMovePo_UpDonwDir == 0.f)
        mbOnAmiedNeedFire = true;
	return true;
}




void  CK9Sprite::NewDivideMissile(int nDivIndexAngle,CSprite* pTarget,int nBombID,SPoint& ptNow,SVector& vtDir,SVector& vDirAngle)
{
    
    CSGLCore* pCore = ((CHWorld*)mpWorld)->GetSGLCore();    
    CBomb *pNewBomb = NULL;
    CBombTailParticle *pNewBombTailParticle = NULL;
    
    PROPERTY_BOMB property;
    if(!PROPERTY_BOMB::GetPropertyBomb(nBombID, property))
    {
        HLogE("[Error] undefine missiletype %d\n",GetModelID());
        return ;
    }
    
// 폭탄이 터질때 내부에서 계산을 한다.
//    if(mpCurrentSelMissaileInfo)
//    {
//        property.nMaxAttackPower = property.nMaxAttackPower + 
//        (property.nMaxAttackPower * (mpWorld->GetUserInfo()->GetAttackUpgrade(GetModelID()) * 10.0f) / 100.0f);
//    }
    
    pNewBomb = new CBomb(pTarget,this,GetID(),GetTeamID(),property.nID,mpAction,GetWorld(),&property);
    
    nDivIndexAngle = nDivIndexAngle % 4;
    if(nDivIndexAngle == 1) nDivIndexAngle = 90;
    else if(nDivIndexAngle == 2) nDivIndexAngle = 180;
    else if(nDivIndexAngle == 3) nDivIndexAngle = 270;
    pNewBomb->SetRotationStartPosAngle(nDivIndexAngle);
    
    
    pNewBombTailParticle = new CBombTailParticle(mpWorld,pNewBomb);
    
    
    pNewBomb->Initialize( &ptNow, &vDirAngle,&vtDir);
    pCore->AddBomb(pNewBomb);
    
       //BombParticle을 추가한다.
    pNewBombTailParticle->Initialize(&ptNow, &vtDir);
    pCore->AddParticle(pNewBombTailParticle);
}


void  CK9Sprite::NewMissile(SPoint& ptNow,SVector& vtDir,SVector& vDirAngle,bool bNeedParticle)
{
    CSprite* pTargetSprite = mpAICore->GetAttackTo();
    
    CSGLCore* pCore = ((CHWorld*)mpWorld)->GetSGLCore();
//    CSprite* pActor = pCore->GetActor();
    CBomb *pNewBomb = NULL;
    CBombTailParticle *pNewBombTailParticle = NULL;
    
    
    //유도기능이 존재한다.면, 타겟이 없으면 타겟을
    if(pTargetSprite == NULL && mBombProperty.fAutoDectect != 0.0f)
    {
        //타겟을 멎추어준다.
        pTargetSprite = GetTargetBySensor();
    }
    
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
        
        pNewBombTailParticle = new CBombTailParticle(mpWorld,pNewBomb);
        
        if(mpCurrentSelMissaileInfo)
        {
            SPoint ptBombStart;
            SVector vtBombDir;
            pNewBomb->GetPosition(&ptBombStart);
            pNewBomb->GetModelDirection(&vtBombDir);
            ((CHWorld*)GetWorld())->GetParticleEmitterMan()->NewFireGas(&ptBombStart, &vtBombDir);
        }
        
        //BombParticle을 추가한다.
        pNewBombTailParticle->Initialize(&ptNow, &vtDir);
        pCore->AddParticle(pNewBombTailParticle);
    }
}


void CK9Sprite::PointOfImpact(SPoint* ptImpact,SVector& vtOutHeaderDir,bool byVecotr)
{
    
    float fWAngle,fHAnglePo,vtPoUpPosition[3],vtPoDownPosition[3];
    //float vDirAngle[3];
    //SVector vtDir;
    

    GetPoAngles(0,&fWAngle,&fHAnglePo,vtPoUpPosition,vtPoDownPosition,&vtOutHeaderDir);
    
    
    float fHAngle = fHAnglePo * PI / 180.0f; //지표면에 대한 각도로 발사
	float fHcos = cosf(fHAngle);
	float fHsin = sinf(fHAngle);
	float fVy0 = mBombProperty.fVelocity * fHsin;	//초기속도
	float fVx0 = mBombProperty.fVelocity * fHcos;	//초기속도
    
    //    //Y방향으로 운동
    //	float fY = fVy0 * fTime;
    //	//Z방향으로 운동
    //	float fZ = fVz0 * fTime - (9.8f * fTime * fTime) / 2;
    //0 = fTime * (fVz0 - 4.9f*fTime) //fZ= 0이면 떨어지는 시점 fVz0 - 4.9f*fTime = 0;
    float fTime = fVy0 / 4.9f;
    float ptModel[3],ptTemp[3];
	
	GLfloat matrix[16];
    ptTemp[0] = fVx0 * fTime + 50.0f;;
	ptTemp[1] = fVy0;
    ptTemp[2] = 0.f; //50.f를 더해야 비슷하게 나온다.이유는 나도 몰라.. 흠..왜일까..
    
    sglLoadIdentityf(matrix);
    sglRotateRzRyRxMatrixf(matrix,
                           0,
                           fWAngle,
                           0); //좌우의 방향..
    sglMultMatrixVectorf(ptModel, matrix, ptTemp);
    
	

    if(byVecotr)
    {
        ptImpact->x = ptModel[0];
        ptImpact->y = ptModel[1];
        ptImpact->z = ptModel[2];
    }
    else
    {
        ptImpact->x = ptModel[0] + vtPoUpPosition[0];
        ptImpact->y = ptModel[1] + vtPoUpPosition[1];
        ptImpact->z = ptModel[2] + vtPoUpPosition[2];

    }

}

bool CK9Sprite::CanFireMissile()
{    
    if(CSprite::CanFireMissile() == false) return false;
    
    if(mpCurrentSelMissaileInfo)
    {
        if(mpCurrentSelMissaileInfo->nType == 0 && mpCurrentSelMissaileInfo->nCnt <= 0) return false;
    }
    
    return true;
}

void CK9Sprite::FireMissile()
{ 
    unsigned long lN = GetGGTime();
    if(lN < mnFireInterval)
        return;
    mnFireInterval = lN + 200;
    for(int i = 0; i < mnPoStartCnt; i++)
    {
        if(!CanFireMissile()) return;
        
        FireOrgMissile();
        mnPoIndex++;
        if(mnPoIndex >= mnPoStartCnt)
            mnPoIndex = 0;
    }
}


void  CK9Sprite::FireOrgMissile()
{
    float fWAngle,fHAngle,vtPoUpPosition[3],vtPoDownPosition[3];
    SVector vDirAngle;
    SPoint  ptNow;
    SVector vtDir;

    if(mpCurrentSelMissaileInfo)
    {
        //영구사용이 아닌 것은 빼준다.
        if(mpCurrentSelMissaileInfo->nType == 0)
        {
            //미사일 카운트를 내려준다.
            mpCurrentSelMissaileInfo->nCnt--;
        }
        
//        //간혹 훈련병모드일때 죽을때 영원히 nType = -1일때가 존재한다.
//        if(mpCurrentSelMissaileInfo->nID == BOMB_ID_2SHOT && mpCurrentSelMissaileInfo->nType == -1)
//            mpCurrentSelMissaileInfo->nType = 0;
        
        if(mpCurrentSelMissaileInfo->nType == 0)
        {
            CScenario::SendMessage(SGL_MESSAGE_FIRE_MISSILE,mpCurrentSelMissaileInfo->nType,mpCurrentSelMissaileInfo->nCnt);
        }
    }
    //1,140,0;1,138,0;1,38,0;1,36,0;1,4,0;1,2,0;1,72,0;1,70,0;1,105,0;1,103,0
    
    GetPoAngles(mnPoIndex,&fWAngle,&fHAngle,vtPoUpPosition,vtPoDownPosition,&vtDir);
    
    ptNow.x = vtPoUpPosition[0];
    ptNow.y = vtPoUpPosition[1];
    ptNow.z = vtPoUpPosition[2];
    
    vDirAngle.x = fHAngle;
    vDirAngle.y = mpModelHeader->orientation[1];
    vDirAngle.z = fWAngle;//mpModelHeader->orientation[2] + mfModelHeaderAngle;
    
    
    //주인공은 클릭같은 것을 한다. 클릭을 했을때 미사일을 발사한다. 그러나 상대방에서 안보이는 영역일때는 발사하지 못한다. 즉 안보이는 영역은
    //가상으로 적을 타겟으로 정하여 발사하기 때문이다.
    //if (GetAICore()->IsActor()) //적군의 대빵도 액터이기때문에 이단말기의 액터만 발사한것을 보내준다.
    if(mpWorld->GetActor() == this)
    {
        if(((CStrategyActorAI*)GetAICore()->GetStategyAI())->isClicked()) //사용자가 직접 클릭을 하여 발사하는 폭탄은..
        {
            CHWorld* pWorld = (CHWorld*)mpWorld;
            pWorld->GetScenario()->GetMutiplyProtocol()->SendSyncClickedNewBomb(GetID(),&ptNow,&vtDir,&vDirAngle);
        }
    }
    
    NewMissile(ptNow,vtDir,vDirAngle);
    
    //Start FireAnimation
    mFireAnimationVector = mProperty.fFireAnimationAcc;
    m_nCurrentPerTime -= GetFirePerSec();
    
    if(m_nCurrentPerTime < 0)
        m_bHodeFire = true;
}

void CK9Sprite::FireMissileInVisible()
{
    unsigned long lN = GetGGTime();
    if(lN < mnFireInterval)
        return;
    mnFireInterval = lN + 200;
    
    int nCnt = mnPoStartCnt; //UP/Down 이기 때문에 나누기 2한다.
    for(int i = 0; i < nCnt; i++)
    {
        if(!CanFireMissile()) return;
        FireOrgMissileInVisible();
        mnPoIndex++;
        if(mnPoIndex >= nCnt)
            mnPoIndex = 0;
    }
}


void  CK9Sprite::FireOrgMissileInVisible()
{
    float fMaxLenDouble,fMaxLen,fDivy;
    //9999.f로 넘어온다 이러면 포신의 방향이 아래 마이너스각으로 변한다.
    //AICOMMAND_ATTACKEDBYEMSPRITE 로 인해 멀리 누군가가 공격해 온다면, 적군이 어디인지는 설정되어 있지만.
    //너무 멀리있어서 발사하지 못하게 막아야 한다.
    if(mfAutoMovePo_ToUpDownAngle == 9999.f) return;
#if OBJMilkShape
    float fUpAngle = mfAutoMovePo_ToUpDownAngle;
#else
    //float fUpAngle = GetInvisiblePoUpAngle() != 0.f ? mpModelHeader->GetmPoUpAngle() : GetInvisiblePoUpAngle();
    float fUpAngle = GetInvisiblePoUpAngle();
    
    //Added By Song 2014.03.16 미사일 스타일
    if(mBombProperty.nBombType == 1) //미사일 스타일
    {
        if(fUpAngle < 60)
            fUpAngle = 60;
    }
#endif
    
    float fRadius = GetRadius();
    float fWAngle,fHAngle;
    SVector vDirAngle;
    SPoint  ptNow,ptEnemy;
    SVector vtDir;
    float vectorLen;
    CSprite* pEn = mpAICore->GetAttackTo();
    if(pEn == NULL)
    {
        if(mptUserClickForInvisible.x != -9999.f)
        {
            memcpy(&ptEnemy, &mptUserClickForInvisible, sizeof(SPoint));
            mptUserClickForInvisible.x = -9999.f;
        }
        else
            return;
    }
    else
    {
        pEn->GetPosition(&ptEnemy);
    }
    
    ptEnemy.y += 5.f; //포신이 조금 위에 있자나. 그래서 좀 높혀서 비율에 맞추어봤다.
    GetPosition(&ptNow);
    ptNow.y += 8.f;
    
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

void CK9Sprite::SetAniRuntimeBubble(CAniRuntimeBubble* v)
{
    mpBubbleParticle = v;
}

CAniRuntimeBubble* CK9Sprite::GetAniRuntimeBubble() { return mpBubbleParticle;}

void CK9Sprite::RenderBeginMovePo_UpDown()
{
    
#if OBJMilkShape
    if(mbAutoMovePo_UpDown == false)
    {
        if(mnPoUpDownState == 1)
        {
            //0~18
            if(mnPoAniStep > 0 && mnPoAniStep < SGL_PO_HALFSTEP)
            {
                
                if(!mpModelHeader->isAnimationRunningf()) //제거하였다. 움직임이 부자연스럽다.
                {
                    int n = SGL_PO_STEP - --mnPoAniStep;
                    mpModelHeader->setNextAnimationf(n, GL_FALSE, GL_FALSE);
                }
            }
            else if(mnPoAniStep >= SGL_PO_HALFSTEP && mnPoAniStep < SGL_PO_STEP)
            {
                
                if(!mpModelHeader->isAnimationRunningf())
                {
                    mnPoAniStep++;
                    mpModelHeader->setNextAnimationf(mnPoAniStep, GL_FALSE, GL_FALSE);
                }
            }
            else
                mnPoUpDownState = 0; //중지
        }
        else if(mnPoUpDownState == -1)
        {
            //0~18
            if(mnPoAniStep < SGL_PO_HALFSTEP)
            {  
                if(!mpModelHeader->isAnimationRunningf())
                {
                    mnPoAniStep++;
                    mpModelHeader->setNextAnimationf(mnPoAniStep, GL_FALSE, GL_FALSE);
                }
            }
            else if(mnPoAniStep > SGL_PO_HALFSTEP)//18이상일때 
            {
                if(!mpModelHeader->isAnimationRunningf())
                {
                    int n = (SGL_PO_STEP - --mnPoAniStep);
                    mpModelHeader->setNextAnimationf(n, GL_FALSE, GL_FALSE);
                }
            }
            else
                mnPoUpDownState = 0; //중지
        }
    }
#endif
    
}

void CK9Sprite::RenderBeginFireAnimation(int nTime,float fMaxMove)
{
    float fMoveVector;
    float fInterval;
    float fHeaderAngleRadian;
    float fXVector , fZVector;
    
    if(mFireAnimationVector == 0.0f) return;
    
    //탱크의 머리를 뒤로 이동한다.
    if( mFireAnimationVector > 0.0f)
    {
        fHeaderAngleRadian = mfModelHeaderAngle * PI / 180;
        fXVector = cosf(fHeaderAngleRadian);
        fZVector = sinf(fHeaderAngleRadian);
        
        fMoveVector = mFireAnimationVector * (float)nTime * 2.0f;
        fInterval = mFireMovedHeader + fMoveVector;
        if(fInterval > fMaxMove)
        {
            
            mHeaderPosition[0] = mProperty.fHeaderPosX - fXVector * fMaxMove;
            mHeaderPosition[2] = mProperty.fHeaderPosZ + fZVector * fMaxMove;
            
            mFireAnimationVector = -mProperty.fFireAnimationAcc;
            mFireMovedHeader = fMaxMove;
        }
        else
        {
            mHeaderPosition[0] -= fXVector * fMoveVector;
            mHeaderPosition[2] += fZVector * fMoveVector;
            mFireMovedHeader = fInterval;
        }            
        //몸체 즉 mpModel의 방향을 기준으로 탱크의 해더의 위치를 재조정한다.
        ArrangeHeaderSprite(mHeaderPosition[0],mHeaderPosition[1],mHeaderPosition[2]);
    }
    //탱크의 머리를 앞으로 이동한다.
    else if( mFireAnimationVector < 0.0f)
    {
        fHeaderAngleRadian = mfModelHeaderAngle * PI / 180;
        fXVector = cosf(fHeaderAngleRadian); 
        fZVector = sinf(fHeaderAngleRadian);
        
        fMoveVector = mFireAnimationVector * (float)nTime;
        fInterval = mFireMovedHeader + fMoveVector;
        if(fInterval < 0)
        {
            mHeaderPosition[0] = mProperty.fHeaderPosX;
            mHeaderPosition[2] = mProperty.fHeaderPosZ;
            mFireAnimationVector = 0.0f; //정지상태로 변경한다.
            mFireMovedHeader = 0.0f;
        }
        else
        {
            mHeaderPosition[0] -= fXVector * fMoveVector;
            mHeaderPosition[2] += fZVector * fMoveVector;
            mFireMovedHeader = fInterval;
        }
        
        //몸체 즉 mpModel의 방향을 기준으로 탱크의 해더의 위치를 재조정한다.
        ArrangeHeaderSprite(mHeaderPosition[0],mHeaderPosition[1],mHeaderPosition[2]);
    }
    
}

void CK9Sprite::RenderBeginAutoMovePo_UpDown(int nTime)
{
#if OBJMilkShape
	float pW,pH;
	float fUpPos[3],fDownPos[3];
	SVector vtDir;

	if( mbAutoMovePo_UpDown && !mpModelHeader->isAnimationRunningf())
	{
		
		GetPoAngles(&pW,&pH,fUpPos,fDownPos,&vtDir);
		
		//아래방향
		if(mfAutoMovePo_UpDonwDir < 0.f)
		{
			// 위의 방향으로 이동한다.
			if(mfAutoMovePo_ToUpDownAngle  >= pH)
			{
				mbAutoMovePo_UpDown = false;
				mfAutoMovePo_UpDonwDir = 0.f; //위방향
				mpAction->OnAutoAimed(this);			
			}
			else
			{
				if(mnPoAniStep < SGL_PO_HALFSTEP)
				{
					mnPoAniStep++;
					mpModelHeader->setNextAnimationf(mnPoAniStep, GL_FALSE, GL_TRUE);
				}
				else if(mnPoAniStep > SGL_PO_HALFSTEP)//18이상일때 
				{
					int n = (SGL_PO_STEP - --mnPoAniStep);
					mpModelHeader->setNextAnimationf(n, GL_FALSE, GL_TRUE);
				}
				else 
				{
					mbAutoMovePo_UpDown = false;
					mfAutoMovePo_UpDonwDir = 0.f; //위방향
					mpAction->OnAutoAimed(this);
				}
				
			}
		}
		else if(mfAutoMovePo_UpDonwDir > 0.f)
		{
            // 위의 방향으로 이동한다.
			if(mfAutoMovePo_ToUpDownAngle  <= pH)
			{
				mbAutoMovePo_UpDown = false;
				mfAutoMovePo_UpDonwDir = 0.f; //위방향
				mpAction->OnAutoAimed(this);
			}
			else
			{
				//0~18
				if(mnPoAniStep > 0 && mnPoAniStep < SGL_PO_HALFSTEP)
				{
					int n = SGL_PO_STEP - --mnPoAniStep;
					mpModelHeader->setNextAnimationf(n, GL_FALSE, GL_TRUE);
				}
				else if(mnPoAniStep >= SGL_PO_HALFSTEP && mnPoAniStep < SGL_PO_STEP)
				{
					mnPoAniStep++;
					mpModelHeader->setNextAnimationf(mnPoAniStep, GL_FALSE, GL_TRUE);
				}
				else 
				{
					mbAutoMovePo_UpDown = false;
					mfAutoMovePo_UpDonwDir = 0.f; //위방향
					
					mpAction->OnAutoAimed(this);
				}
				
			}
		}
	}
#else
    if(mfAutoMovePo_UpDonwDir != 0.f)
    {
        float pW,pH;
        SVector vtDir;
        float fUpPos[3];
        
        //Modified By Song 2013.09.24 언덕일때 잘 안맞는 현상 수정함.
        //mfPoUpAngle += nTime/20.f* mfAutoMovePo_UpDonwDir;
        mpModelHeader->SetmPoUpAngle(mpModelHeader->GetmPoUpAngle() + nTime/20.f* mfAutoMovePo_UpDonwDir);
        GetPoAngles(0,&pW,&pH,fUpPos,fUpPos,&vtDir);

        if(pH < -30) //어떤이유에서인지 포가 빙빙돈다면 재조절해주자.
        {
            mfAutoMovePo_UpDonwDir = 0.f;
            mfAutoMovePo_ToUpDownAngle = 0.f;
            mpModelHeader->SetmPoUpAngle(0.0f);
            return;
        }

        
        if(mfAutoMovePo_UpDonwDir < 0.f && pH <= mfAutoMovePo_ToUpDownAngle)
        {
//            mpModelHeader->SetmPoUpAngle(pH);
            
            mfAutoMovePo_ToUpDownAngle = 0.f;
            mfAutoMovePo_UpDonwDir = 0.f;
            mpAction->OnAutoAimed(this);
        }
        else if(mfAutoMovePo_UpDonwDir > 0.f && (pH >= mfAutoMovePo_ToUpDownAngle || mpModelHeader->GetmPoUpAngle() >= MAXPOUPANGLE))
        {
//            mpModelHeader->SetmPoUpAngle(pH);
            mfAutoMovePo_ToUpDownAngle = 0.f;
            mfAutoMovePo_UpDonwDir = 0.f;
            mpAction->OnAutoAimed(this);
        }
        
    }
    else if(mbOnAmiedNeedFire && mpAICore->GetAttackTo())
    {
        mpAction->OnAutoAimed(this);
        mbOnAmiedNeedFire = false;
    }

#endif
}


void CK9Sprite::RenderBeginAutoTurnHeader(int nTime)
{
	if(mfModelHeaderRotVector != 0.0f)
	{
        float fNowAngle = AngleArrangeEx ( mpModel->orientation[1] + mfModelHeaderAngle );
        float fToPoAngle = fNowAngle + mfModelHeaderRotVector * (float)nTime;
        float fRotDir = CatRotation(fToPoAngle, mfAutoTurnAHeader_ToAngle);
        if(fRotDir == 0.f)
        {
            mfModelHeaderRotVector = 0.f;
            mfAutoTurnAHeader_ToAngle = 0.f;
            CheckAutoMove_PoUpDown();
            if(mfModelHeaderRotVector == 0.f && mfAutoMovePo_UpDonwDir == 0.f)
                mbOnAmiedNeedFire = true;

            return ;
        }
        else
        {
            float fHeaderAngle = fToPoAngle - mpModel->orientation[1];
            
            if((mfModelHeaderRotVector > 0 && fRotDir < 0) || ( mfModelHeaderRotVector < 0 && fRotDir > 0)) //방향이 지나쳤다.
            {
                fHeaderAngle = mfAutoTurnAHeader_ToAngle - mpModel->orientation[1];
                SetHeaderRotation(AngleArrangeEx(fHeaderAngle));
                
                mfModelHeaderRotVector = 0.f;
                mfAutoTurnAHeader_ToAngle = 0.f;
                CheckAutoMove_PoUpDown();
                if(mfModelHeaderRotVector == 0.f && mfAutoMovePo_UpDonwDir == 0.f)
                    mbOnAmiedNeedFire = true;

                return ;
            }
            SetHeaderRotation(AngleArrangeEx(fHeaderAngle));
        }
	}
}


//일반적으로 평면의 벡터를 구하고 그것의 x축/z축 벡터를 크로스곱을 하면 각이 나오기는 하지만.
//회전의 순서가 x방향을 기준으로 변경되지 않기 때문에??? 암튼 안된다.
//현재 모델을 재조정한다.
int CK9Sprite::ArrangeSprite()
{
    int nResult = 0;

	float fLP1[3],fLP2[3],fRP1[3],fRP2[3];
	
	int nRBP_Min = 0;
	int nRBP_Max = 0;
	
	int nLBP_Min = 0;
	int nLBP_Max = 0;
	
    //2014.07.24 보이지 않는 곳에서는 ArrangeSprite를 할 필요하가 없다.
    //if(IsVisibleRender() == false) return E_SUCCESS;
    
	float angle = mpModel->orientation[1] * PI / 180.0f;  
	
	float fHalfX = mfHalfX * mProperty.fScale;
	float fHalfZ = mfHalfZ * mProperty.fScale;
	int nPSRightBPSize = sizeof(mPSRightBP)/sizeof(SPoint);
    
    
	nResult = CalculateBootomPoint(fHalfX, -fHalfZ,angle,&mPSRightBP[0],&mPSRightBP[1],&mPSRightBP[2],&mPSRightBP[3],&mPSRightBP[4]);
    if(nResult != E_SUCCESS) 
    {
        //sglLog("충돌 오른쪽 %d\n",nResult);
        ArrangeHeaderSprite(mHeaderPosition[0],mHeaderPosition[1],mHeaderPosition[2]);
        return nResult*10; //오른쪽은 -100
    }
	nResult = CalculateBootomPoint(-fHalfX, -fHalfZ,angle,&mPSLeftBP[0],&mPSLeftBP[1],&mPSLeftBP[2],&mPSLeftBP[3],&mPSLeftBP[4]);
	if(nResult != E_SUCCESS) 
    {
        //sglLog("충돌 왼쪽 %d\n",nResult);
        ArrangeHeaderSprite(mHeaderPosition[0],mHeaderPosition[1],mHeaderPosition[2]);
        return nResult*5; //왼쪽은 -50
    }
	GetMaxMinYPoint(mPSRightBP,nPSRightBPSize,&nRBP_Min,&nRBP_Max);
	
	fRP1[0] = mPSRightBP[nRBP_Min].x;
	fRP1[1] = mPSRightBP[nRBP_Min].y;
	fRP1[2] = mPSRightBP[nRBP_Min].z;
	
	fRP2[0] = mPSRightBP[nRBP_Max].x;
	fRP2[1] = mPSRightBP[nRBP_Max].y;
	fRP2[2] = mPSRightBP[nRBP_Max].z;
	
	
	GetMaxMinYPoint(mPSLeftBP,nPSRightBPSize,&nLBP_Min,&nLBP_Max);
	
	fLP1[0] = mPSLeftBP[nLBP_Min].x;
	fLP1[1] = mPSLeftBP[nLBP_Min].y;
	fLP1[2] = mPSLeftBP[nLBP_Min].z;
	
	fLP2[0] = mPSLeftBP[nLBP_Max].x;
	fLP2[1] = mPSLeftBP[nLBP_Max].y;
	fLP2[2] = mPSLeftBP[nLBP_Max].z;

	float fvx = fLP1[0] - fLP2[0];
    float fvy = 0;
	float fvz = fLP1[2] - fLP2[2];
	
	
	//앞방향 뒤방향의 회전값의 방향이 틀려진다.
	if(nLBP_Min > nLBP_Max) 
	{
		fvy = fLP2[1] - fLP1[1];
//#if DEBUG
//		sglLog("앞이 올라감\n");
//#endif //DEBUG
	}
	else 
	{
		fvy = fLP1[1] - fLP2[1];
//#if DEBUG
//		sglLog("앞이 내려감\n");
//#endif //DEBUG
	}
	
	
	float vectorLen = sqrtf(fvx*fvx + fvy*fvy + fvz*fvz);
	float pitch = asinf(fvy/vectorLen) * 180.0 / PI;
	
	fvx = fRP1[0] - fLP2[0];
	fvz = fRP1[2] - fLP2[2];
//	fvz = fRP1[2] - fLP2[2];
	
	if(fRP2[1] > fLP2[1]) //왼쪽의 맥스값이 오른쪽의 맥스값 보다 작으면 오른쪽 맥스값을 기준으로 한다.
	{
		fvy =   fLP2[1] - fRP2[1];
	}
	else 
	{
//#if DEBUG
//		if(fRP1[2]/*오른쪽*/ < fLP2[2]/*왼쪽*/) 
//			sglLog("왼쪽이 높음\n");
//		else 
//			sglLog("오른쪽이 높음\n");
//#endif //DEBUG
        
		fvy =   fLP2[1] - fRP1[1];
	}
   
	vectorLen = sqrtf(fvx*fvx + fvy*fvy + fvz*fvz);
	float yaw =  asinf(fvy/vectorLen) * 180.0 / PI;
    
//---------------------------------------------------
// 절벽인지아니면 벽인지를 체크한다.
//---------------------------------------------------
    if(mnMapID != 9)
    {
        if(pitch > 35.0f || pitch < -35.0f)
        {
            return -100;
        }
        
        if(yaw > 35.0f || yaw < -35.0f) 
        {
            return -101;
        }
    }
//---------------------------------------------------

    //음 짐벌락 현상이 난다.
    //	mpModel->orientation[0] = yaw;
    //	mpModel->orientation[2] = pitch;
	sglLoadIdentityf(mpModel->pfinalmulMaxtix);
	sglRotateRzRyRxMatrixf(mpModel->pfinalmulMaxtix, yaw, 0, pitch);
    
	//몸체 즉 mpModel의 방향을 기준으로 탱크의 해더의 위치를 재조정한다.
	ArrangeHeaderSprite(mHeaderPosition[0],mHeaderPosition[1],mHeaderPosition[2]);

//    //위치를 재조정해준다.
//    if(mpShadowParticle)
//        mpShadowParticle->Caculate(mpModel->orientation, mpModel->pfinalmulMaxtix,mpModel->position);

    
    return nResult;
}


//탱크이 방향과 위치가 변경될 때 마다. 
//탱크의 머리의 위치가 변경된다.
void CK9Sprite::ArrangeHeaderSprite(int xHeader,int yHeader,int zHeader)
{
	GLfloat ptHeaderPos[3];
	GLfloat matrix[16];
	GLfloat pos[3];
    
	pos[0] = xHeader * mProperty.fScale;
	pos[1] = yHeader * mProperty.fScale;
	pos[2] = zHeader * mProperty.fScale;
	
	memset(ptHeaderPos,0,sizeof(GLfloat) * 3);
	
	sglLoadIdentityf(matrix);
	

    GLfloat	quaternion0[4];
    sglAngleToQuaternionf(quaternion0, mpModel->orientation[0], mpModel->orientation[1], mpModel->orientation[2]);
    sglQuaternionToMatrixf(matrix, quaternion0);
    
    
    
//짐벌락 현상.
//    sglRotateRzRyRxMatrixf(matrix,
//									 mpModel->orientation[0],
//									 mpModel->orientation[1], 
//									 mpModel->orientation[2]);
    
    sglMultMatrixf(matrix, matrix, mpModel->pfinalmulMaxtix);
    sglMultMatrixVectorf(ptHeaderPos, matrix,pos);
    
	
	//탱크의 머리는 몸체의 방향과 동일하다.
	mpModelHeader->orientation[0] = mpModel->orientation[0];
	mpModelHeader->orientation[1] = mpModel->orientation[1];
	mpModelHeader->orientation[2] = mpModel->orientation[2];
	
	mpModelHeader->position[0] =  mpModel->position[0] + ptHeaderPos[0];
	mpModelHeader->position[1] =  mpModel->position[1] + ptHeaderPos[1];
	mpModelHeader->position[2] =  mpModel->position[2] + ptHeaderPos[2];
    
    
    //탱크가 움직일때마다 연산을 하지 않게 하기 위함.
    SetHeaderRotation(mfModelHeaderAngle);
}


//fAngle 의 각도는 반드시 AngleArrange(fAngle)의 값이여야 한다.
void CK9Sprite::SetHeaderRotation(float fAngle)
{
#if OBJMilkShape
	//fAngle = AngleArrange(fAngle);
	sglLoadIdentityf(mpModelHeader->pfinalmulMaxtix);
    //해더의 기울기를 곱한다.
    sglMultMatrixf(mpModelHeader->pfinalmulMaxtix, mpModelHeader->pfinalmulMaxtix, mpModel->pfinalmulMaxtix);
	sglRotateRzRyRxMatrixf(mpModelHeader->pfinalmulMaxtix, 0, fAngle, 0);
#else
    //fAngle = AngleArrange(fAngle);
	sglLoadIdentityf(mpModelHeader->pfinalmulMaxtix);
    //해더의 기울기를 곱한다.
    sglMultMatrixf(mpModelHeader->pfinalmulMaxtix, mpModelHeader->pfinalmulMaxtix, mpModel->pfinalmulMaxtix);
	sglRotateRzRyRxMatrixf(mpModelHeader->pfinalmulMaxtix, 0, fAngle, 0);
#endif
	mfModelHeaderAngle = fAngle;
	
	if(mpWorld->GetActor() == this && GetCameraType() == CAMERT_TANKHEADER)
	{
		SetCameraType(CAMERT_TANKHEADER); //카메라의 위치를 이동시켜준다.
	}
}

void CK9Sprite::GetMaxMinYPoint(SPoint *arrPoint,int nSize,int *pnIndexMin,int *pnIndexMax)
{
	
	float nMax,nMin;
	nMax = nMin= arrPoint[0].y;
	*pnIndexMin = *pnIndexMax = 0;
	
	for (int i = 1; i < nSize; i++) 
	{
		if(arrPoint[i].y < nMin)
			*pnIndexMin = i;
		else if(arrPoint[i].y > nMax)
			*pnIndexMax = i;
	}
	
	if(*pnIndexMin == *pnIndexMax) 
		*pnIndexMax = nSize - 1;
}


float CK9Sprite::GetRadius() 
{ 
    return mpModel->GetRadius();
}

int CK9Sprite::CalculateBootomPoint(
                                    float fXHalf,
                                    float fZHalf,
                                    float angle,
                                    SPoint* R0,
                                    SPoint* R1,
                                    SPoint* R2,
                                    SPoint* R3,
                                    SPoint* R4)
{
	

	int nResult = E_SUCCESS;
    
	float fcos = sinf(angle);
	float fsin = cosf(angle);
	
	float fX = 0.0f;
	float fZ = 0.0f;
	
	
	//0
	R0->x = fXHalf;
    R0->y = 0;
	R0->z = fZHalf;
    
	
	fX = R0->x * fcos - R0->z * fsin;
	fZ = R0->x * fsin + R0->z * fcos;
	
	R0->x = fX;
	R0->z = fZ;
	
	
	//1
	R1->x = fXHalf;
    R1->y = 0;
	R1->z = fZHalf / 2.0f;
    
	
	fX = R1->x * fcos - R1->z * fsin;
	fZ = R1->x * fsin + R1->z * fcos;
	
	R1->x = fX;
	R1->z = fZ;
	
	//2
	R2->x = fXHalf;
	R2->y = 0;
    R2->z = 0;
	
	fX =   R2->x * fcos;
	fZ =   R2->x * fsin;
	R2->x = fX;
	R2->z = fZ;
	
	
	//3
	R3->x = fXHalf;
    R3->y = 0;
	R3->z = - fZHalf / 2.0f;
    
	
	fX = R3->x * fcos - R3->z * fsin;
	fZ = R3->x * fsin + R3->z * fcos;
	
	R3->x = fX;
	R3->z = fZ;
	
	
	
	//4
	R4->x = fXHalf;
	R4->z = - fZHalf;
    R4->y = 0;
	
	fX = R4->x * fcos - R4->z * fsin;
	fZ = R4->x * fsin + R4->z * fcos;
	
	R4->x = fX;
	R4->z = fZ;
    
	
	
	R0->x += mpModel->position[0];
	R0->z += mpModel->position[2];
	
    
	//nResult = g_pWorld->GetPositionY(R0, GetModelDirection());
    nResult = ((CWorld*)mpWorld)->GetMovePosiztionY(R0);
	if(nResult != E_SUCCESS) 
	{
        R0->y = mpModel->position[1];
        
        return -10;
        //if(nRes == E_SUCCESS) nRes = -10;
	}
	
	
	R1->x += mpModel->position[0];
	R1->z += mpModel->position[2];
	
	nResult = ((CWorld*)mpWorld)->GetMovePosiztionY(R1);
	if(nResult != E_SUCCESS) 
	{
        R1->y = mpModel->position[1];
        return -11;
//        if(nRes == E_SUCCESS) nRes = -11;
        
	}
	
	
	R2->x += mpModel->position[0];
	R2->z += mpModel->position[2];
	
	nResult = ((CWorld*)mpWorld)->GetMovePosiztionY(R2);
	if(nResult != E_SUCCESS) 
	{
        R2->y = mpModel->position[1];
        return -12;
//        if(nRes == E_SUCCESS) nRes = -12;
        
	}
	
	
	R3->x += mpModel->position[0];
	R3->z += mpModel->position[2];
	
	nResult = ((CWorld*)mpWorld)->GetMovePosiztionY(R3);
	if(nResult != E_SUCCESS) 
	{
        R3->y = mpModel->position[1];
        return -13;
//        if(nRes == E_SUCCESS) nRes = -13;
        
	}
	
	R4->x += mpModel->position[0];
	R4->z += mpModel->position[2];
	
	nResult = ((CWorld*)mpWorld)->GetMovePosiztionY(R4);
	if(nResult != E_SUCCESS) 
	{
        R4->y = mpModel->position[1];
        return -14;
//        if(nRes == E_SUCCESS) nRes = -14;
        
	}
	return nResult;
}

float* CK9Sprite::GetHeaderPosition()
{
    return mpModelHeader->position;
}

#if TestPoStart_F
void CK9Sprite::TestPoStart()
{

    
    //가운데
    //0.279366 84.822441 -53.164085 //y값이 높기 때문에 이것이 위 (G=0,T=257,V=1)
    //0.295627 83.820099 -51.433468 //y값이 낮기 때문에 이것이 아래 (G=0,T=256,V=2)
    
    //오른쪽
    //8.406083 61.866463 -58.961628
    //8.706850 62.530334 -60.824089
    
    //먼오른쪽
    //25.742157 66.906860 -55.172928
    //26.190445 67.855736 -56.875465
    
    //왼쪽
    //-7.906083 62.329002 -58.819145
    //-8.206849 62.828018 -60.732391
    
    //먼왼쪽
    //-25.992157 67.716362 -55.280453
    //-26.440445 68.513252 -57.059208
    
    float fff[3];
    int nG,nT,nV;
    
    fff[0] = 0.000000;
    fff[1] = 88.893051;
    fff[2] = -89.911789;
    if(!mpModelHeader->DevGetPoIndex(fff, nG, nT, nV))
    {
        HLog("ERROR not found \n");
        return ;
    }
    HLog("Right UP G=%d,T=%d,V=%d\n",nG,nT,nV);
    
    
    fff[0] = -0.000000;
    fff[1] = 60.647034;
    fff[2] = -2.979446;
    if(!mpModelHeader->DevGetPoIndex(fff, nG, nT, nV))
    {
        HLog("ERROR not found \n");
        return ;
    }
    HLog("Right DOWN G=%d,T=%d,V=%d\n",nG,nT,nV);
    
    
    
//    Right UP G=1,T=1,V=1
//    Right DOWN G=1,T=0,V=2
}
#endif

void CK9Sprite::GetFirePos(int nIndex,float** pOutFront,float** pOutBack )
{
    if(mpModelHeader->GetType() == MODELTYPE_MS3D_ASSCII)
    {

        sglMesh     *pWorldMesh = ((CMS3DModelASCII*)mpModelHeader)->GetWorldMesh();
        //nPoIndex 포인덱스를 사용하여 각도를 구해온다. //2,4,2
        *pOutFront = &pWorldMesh[mptStartMissile[nIndex][0]].pVetexBuffer[mptStartMissile[nIndex][1] * 3];
        *pOutBack = &pWorldMesh[mptStartMissile[nIndex][0]].pVetexBuffer[mptStartMissile[nIndex][2] * 3];
    }
}


void CK9Sprite::GetPoAngles(int nInIndex,float* pOutWAngle,float* pOutHAngle,float* pOutUpPos,float* pOutDownPos,SVector *pvtDir)
{
#if OBJMilkShape
    //1. 상단과 하단의 정점을 선택한다.
    //2. 정점 정보툴을 사용하여 상단,하단의 정점의 좌표가 보인다. (이것을 사용하지 않아도 됨)
    //3. 텍스트로 저장하여 정점의 처음 1이 선택인 것을 찾는다.
    //4. 정점 순서 인덱스의 위치와 그것의 정점의 위치를 표시한다.
    
	//포의 경의 2개의 그룹에 두번째 그룹은 포신이며 48개의 삼각형패널로 그성 되어 있다.
	//25(zero base is  24) 번째의  3(zero base is  2)번째의 점은 상단 꼭지점
//    GLfloat* vtUp = mpModelHeader->getWorldTriangle(1,0,1);
	
	//26(zero base is  25) 번째의  2(zero base is  1)번째의 점은 하단 꼭지점 으로 구성되어 있다.
//    GLfloat* vtDown = mpModelHeader->getWorldTriangle(1,138,0);
    
//    if(mnPoStartCnt == 0)
//    {
//        HLogE("ERROR GetPoAngles no count. check plist, 23 index\n");
//        return ;
//    }
//    
//    float fff[3];
//    int nG,nT,nV;
//
//    fff[0] = -0.000036;
//    fff[1] = 219.938232;
//    fff[2] = -131.236755;
//    if(!mpModelHeader->DevGetPoIndex(fff, nG, nT, nV))
//    {
//        HLog("ERROR not found \n");
//        return ;
//    }
//
//    HLog("Right UP G=%d,T=%d,V=%d\n",nG,nT,nV);
//    //0.000000 70.000000 -54.250000
//    fff[0] = -0.000000;
//    fff[1] = 70.000000;
//    fff[2] = -54.250000;
//    if(!mpModelHeader->DevGetPoIndex(fff, nG, nT, nV))
//    {
//        HLog("ERROR not found \n");
//        return ;
//    }
//    
//    HLog("Right UP G=%d,T=%d,V=%d\n",nG,nT,nV);
//    Right UP G=0,T=418,V=1
//    Right UP G=0,T=554,V=2
//    0,418,1;0,554,2
    
    float *vtUp,*vtDown;
//    if(mnPoIndex == -1)
//    {
//        vtUp = mpModelHeader->getWorldTriangle(mptStartMissile[0][0],mptStartMissile[0][1],mptStartMissile[0][2]);
//        vtDown = mpModelHeader->getWorldTriangle(mptStartMissile[1][0],mptStartMissile[1][1],mptStartMissile[1][2]);
//    }
//    else
//    {
        int nIndex = nInIndex *2;
        vtUp = mpModelHeader->getWorldTriangle(mptStartMissile[nIndex][0],mptStartMissile[nIndex][1],mptStartMissile[nIndex][2]);
        nIndex++;
        vtDown = mpModelHeader->getWorldTriangle(mptStartMissile[nIndex][0],mptStartMissile[nIndex][1],mptStartMissile[nIndex][2]);
//    }
	
	
	SPoint ptNow;
	ptNow.x = pOutUpPos[0] = vtUp[0];
	ptNow.y = pOutUpPos[1] = vtUp[1];
	ptNow.z = pOutUpPos[2] = vtUp[2];
	
	pOutDownPos[0] = vtDown[0];
	pOutDownPos[1] = vtDown[1];
	pOutDownPos[2] = vtDown[2];
	
	
	//SVector vtDir;
	pvtDir->x = vtUp[0] - vtDown[0];
	pvtDir->y = vtUp[1] - vtDown[1];
	pvtDir->z = -(vtUp[2] - vtDown[2]); //z.의 방향을 반대이다.
	
	float vectorLen = sqrtf(pvtDir->x*pvtDir->x + pvtDir->y*pvtDir->y + pvtDir->z*pvtDir->z);
    
    pvtDir->x /= vectorLen;
    pvtDir->y /= vectorLen;
    pvtDir->z /= vectorLen;
	
	*pOutWAngle = atan2(pvtDir->z,pvtDir->x) * 180.0 / PI;
    *pOutHAngle = asinf(pvtDir->y) * 180.0 / PI;
#else
    float *vtUp,*vtDown;
//    sglMesh *arrWroldMesh = mpModelHeader->GetWorldMesh();
    //nPoIndex 포인덱스를 사용하여 각도를 구해온다. //2,4,x;3,4,2;4,4,x
//    if(mnPoIndex == -1)
//    {
//        vtUp = &arrWroldMesh[mptStartMissile[0][0]].pVetexBuffer[mptStartMissile[0][1] * 3];
//        vtDown = &arrWroldMesh[mptStartMissile[1][0]].pVetexBuffer[mptStartMissile[1][1] * 3];
//    }
//    else
//    {
//        int nIndex = nInIndex *2;
//        vtUp = &arrWroldMesh[mptStartMissile[nIndex][0]].pVetexBuffer[mptStartMissile[nIndex][1] * 3];
//        nIndex++;
//        vtDown = &arrWroldMesh[mptStartMissile[nIndex][0]].pVetexBuffer[mptStartMissile[nIndex][1] * 3];
    GetFirePos(nInIndex,&vtUp,&vtDown);
//    }
    
    //1,1,0;1,169,0
    //1,1,0;1,170,0
//    vtUp = &arrWroldMesh[1].pVetexBuffer[1 * 3];
//    vtDown = &arrWroldMesh[1].pVetexBuffer[169 * 3];
    
    
	pOutUpPos[0] = vtUp[0];
	pOutUpPos[1] = vtUp[1];
	pOutUpPos[2] = vtUp[2];
	
    
	pOutDownPos[0] = vtDown[0];
	pOutDownPos[1] = vtDown[1];
	pOutDownPos[2] = vtDown[2];
	
	
	//SVector vtDir;
	pvtDir->x = vtUp[0] - vtDown[0];
	pvtDir->y = vtUp[1] - vtDown[1];
	pvtDir->z = -(vtUp[2] - vtDown[2]); //z.의 방향을 반대이다.
    
    float vectorLen = sqrtf(pvtDir->x*pvtDir->x + pvtDir->y*pvtDir->y + pvtDir->z*pvtDir->z);
    
    pvtDir->x /= vectorLen;
    pvtDir->y /= vectorLen;
    pvtDir->z /= vectorLen;
	
	*pOutWAngle = atan2(pvtDir->z,pvtDir->x) * 180.0 / PI;
    *pOutHAngle = asinf(pvtDir->y) * 180.0 / PI;

#endif
}




#if DEBUG 
void CK9Sprite::DisplayBottomStatus()
{
    int nResult = 0;
	SPoint  mPSRightBP[5];
	SPoint  mPSLeftBP[5];
	
	
	float angle = mpModel->orientation[1] * PI / 180.0f;
    
	nResult = CalculateBootomPoint(mfHalfX * mProperty.fScale, -mfHalfZ * mProperty.fScale,angle,&mPSRightBP[0],&mPSRightBP[1],&mPSRightBP[2],&mPSRightBP[3],&mPSRightBP[4]);
    if(nResult != E_SUCCESS)
    {
        HLog("오른쪽 %d\n",nResult);
    }
	nResult = CalculateBootomPoint(-mfHalfX * mProperty.fScale, -mfHalfZ * mProperty.fScale,angle,&mPSLeftBP[0],&mPSLeftBP[1],&mPSLeftBP[2],&mPSLeftBP[3],&mPSLeftBP[4]);
	
	if(nResult != E_SUCCESS)
    {
        HLog("왼쪽 %d\n",nResult);
    }
	float fLP1[3],fLP2[3],fRP1[3],fRP2[3];
	
	int nRBP_Min = 0;
	int nRBP_Max = 0;
	
	int nLBP_Min = 0;
	int nLBP_RMax = 0;
	GetMaxMinYPoint(mPSRightBP,sizeof(mPSRightBP)/sizeof(SPoint),&nRBP_Min,&nRBP_Max);
	
	fRP1[0] = mPSRightBP[nRBP_Min].x;
	fRP1[1] = mPSRightBP[nRBP_Min].y;
	fRP1[2] = mPSRightBP[nRBP_Min].z;
	
	fRP2[0] = mPSRightBP[nRBP_Max].x;
	fRP2[1] = mPSRightBP[nRBP_Max].y;
	fRP2[2] = mPSRightBP[nRBP_Max].z;
	
	GetMaxMinYPoint(mPSLeftBP,sizeof(mPSLeftBP)/sizeof(SPoint),&nLBP_Min,&nLBP_RMax);
	
	fLP1[0] = mPSLeftBP[nLBP_Min].x;
	fLP1[1] = mPSLeftBP[nLBP_Min].y;
	fLP1[2] = mPSLeftBP[nLBP_Min].z;
	
	fLP2[0] = mPSLeftBP[nLBP_RMax].x;
	fLP2[1] = mPSLeftBP[nLBP_RMax].y;
	fLP2[2] = mPSLeftBP[nLBP_RMax].z;
	
	
	sglColorf(0.0f, 1.0f, 0.0f, 1.0f);		
	sglLinef(fLP1,fLP2); //x축
	sglLinef(fLP2,fRP1); //y
	sglLinef(fRP1,fLP1); //z
	
	float ftX[3],ftdd[3];
	int nSize = sizeof(mPSRightBP) / sizeof(SPoint);
	for (int i = 0; i < nSize; i++) {
		
		ftX[0] = mPSRightBP[i].x;
		ftX[1] = mPSRightBP[i].y;
		ftX[2] = mPSRightBP[i].z;
		ftdd[0] = 0;
		ftdd[1] = 0;
		ftdd[2] = 45;
		sglCirclef(ftX,ftdd,1);
		
		
		ftX[0] = mPSLeftBP[i].x;
		ftX[1] = mPSLeftBP[i].y;
		ftX[2] = mPSLeftBP[i].z;
		ftdd[0] = 0;
		ftdd[1] = 0;
		ftdd[2] = 45;
		sglCirclef(ftX,ftdd,1);
	}
    
    
    
//    SPoint sp;
//    GetPosition(&sp);
//    
//    ftX[0] = sp.x;
//    ftX[1] = sp.y;
//    ftX[2] = sp.z+10.f;
//    ftdd[0] = 0;
//    ftdd[1] = 0.f;
//    ftdd[2] = 0;
//    float f = sqrt(200.f);
//    sglCirclef(ftX,ftdd,18.f);
    
    
    
    sglColorf(0.0f, 0.0f, 0.0f, 1.0f);
}
#endif




void CK9Sprite::CheckAutoMove_PoUpDown()
{
    if(m_bIsVisibleRender)
    {
        float pW,pH;
        float fUpPos[3],fDownPos[3];
        SVector vtDir;
        GetPoAngles(0,&pW,&pH,fUpPos,fDownPos,&vtDir);
        
        // 위의 방향으로 이동한다.
        if(mfAutoMovePo_ToUpDownAngle  > pH)
        {
#if OBJMilkShape
            mbAutoMovePo_UpDown = true;
#endif
            mfAutoMovePo_UpDonwDir = 1.f; //위방향
        }
        else if(mfAutoMovePo_ToUpDownAngle  < pH)
        {
#if OBJMilkShape
            mbAutoMovePo_UpDown = true;
#endif
            mfAutoMovePo_UpDonwDir = -1.f; //아래방향
        }
        else 
        {
#if OBJMilkShape
            mbAutoMovePo_UpDown = false;
#endif
            mfAutoMovePo_UpDonwDir = 0.0f;
        }
    }
    else
    {
#if OBJMilkShape
        mbAutoMovePo_UpDown = false;
#else
        mpModelHeader->SetmPoUpAngle(mfAutoMovePo_ToUpDownAngle);
#endif
        mfAutoMovePo_UpDonwDir = 0.0f;
    }
}

float CK9Sprite::GetCameraAngle()
{
    return mpModel->orientation[1] + mfCameraTurnPos;
}

void CK9Sprite::SetCameraAngle(float fA)
{
    float fPosY = 0.0f;
    SPoint ptCameraPos;
    float fDiff;
    float fViewDirX = 0.0f;
    float fViewDirZ = 0.0f;
    

    mfCameraTurnPos = fA - mpModel->orientation[1];
    mfCameraTurnPos = AngleArrange(mfCameraTurnPos);

    GLfloat vViewDir[3];
    GLfloat matrix[16];
    GLfloat pos[3];
    
    pos[0] = -1;
    pos[1] = 0;
    pos[2] = 0;
    
    sglLoadIdentityf(matrix);
    sglRotateRzRyRxMatrixf(matrix,
                           0,
                           mpModel->orientation[1] + mfCameraTurnPos,//탱크 해더 방향을 기준으로 돌려면 + mfModelHeaderAngle);
                           0);
    sglMultMatrixVectorf(vViewDir, matrix,pos);
    
    fViewDirX = vViewDir[0];
    fViewDirZ = vViewDir[2];
 
    
    
    //(55 - 16) : (ZV - NormalViewMinZ) = X : (mfCameraUpDownPos - NormalViewMinZ)
    fDiff = 39.0f * (mfCameraUpDownPos - NormalViewMinY) / (NormalViewMaxY - NormalViewMinY) + 16;
    
    
    ptCameraPos.x = mpModelHeader->position[0] - fViewDirX * fDiff;
    ptCameraPos.y = mpModelHeader->position[1] + mfCameraUpDownPos;
    ptCameraPos.z = mpModelHeader->position[2] + fViewDirZ * fDiff;
    
    if(CameraComact(&ptCameraPos,&fPosY))
    {
//탱크가 안보인다.
//        ptCameraPos.x = mpModelHeader->position[0] - fViewDirX;
//        if(fPosY == -9999.f)
//            ptCameraPos.y = mpModelHeader->position[1] + mfCameraUpDownPos;
//        else
//            ptCameraPos.y = mpModelHeader->position[1] + fPosY;
//        ptCameraPos.z = mpModelHeader->position[2] + fViewDirZ;
        
        ptCameraPos.x = mpModelHeader->position[0] - fViewDirX * 10.f;
        ptCameraPos.y = mpModelHeader->position[1] + 9.6;
        ptCameraPos.z = mpModelHeader->position[2] + fViewDirZ * 10.f;
        
    }
    
    mpCamera->viewPos.x = ptCameraPos.x;
    mpCamera->viewPos.y = ptCameraPos.y;
    mpCamera->viewPos.z = ptCameraPos.z;
    
    
    
    mpCamera->viewDir.x = fViewDirX;
    mpCamera->viewDir.z = -fViewDirZ;
    
#if defined(ACCELATOR)
    mAccelatorViewDirBefor.x = mpCamera->viewDir.x;
    mAccelatorViewDirBefor.z = mpCamera->viewDir.z;

    mpCamera->viewDir.y = mAccelatorViewDirBefor.y;
#else
    //mpCamera->viewDir.z = -0.15f; //-4
    //-0.15 ~ 0.35 사이로 움직인다.
    //0.50 : (zV - zv)= X = (mfCameraUpDownPos - zv);
    //X = 0.50f * (mfCameraUpDownPos - zv) / (zV - zv) - 0.15
    mpCamera->viewDir.y =  -0.60 * (float)(mfCameraUpDownPos - NormalViewMinY) / (float)(NormalViewMaxY - NormalViewMinY) - 0.15f;
#endif //PINCHZOOM
    
    mpCamera->viewUp.x = 0;
    mpCamera->viewUp.y = 1;
    mpCamera->viewUp.z = 0;
    
    //단위벡터로 변경한다.
    sglNormalize(&mpCamera->viewDir);
    mpCamera->ViewFar = mpCamera->ViewFarOrg;
}


void CK9Sprite::HandleCamera(float fUpDownVector,float fTurnVector)
{
    static float fBeforeDirX = 0.0f;
    static float fBeforeDirZ = 0.0f;
    
    float fPosY = 0.0f;
    float fViewDirX = 0.0f;
    float fViewDirZ = 0.0f;
    float  fDiff = 16.0f;
    SPoint ptCameraPos;
    SPoint ptSpritePosition;
    SVector vSpriteDirection;
    
    GetPosition(&ptSpritePosition);
    GetModelDirection(&vSpriteDirection);


    int nMaxY = ptSpritePosition.y + NormalViewMaxY;
    int mMinY = ptSpritePosition.y + NormalViewMinY;
    
    
    //와우... 이동중에 카메라가 열나게 흔들렸는데 인제는 그렇게 하지 않아도 되것다.
    //지금 이동중이거나 회전중일때 카메라가 액터의 방향으로 돌아가지 않게 하자.
    if(isMoving() || isTurning())
    {
        //이전각도
        float mfBeforeAngle = atan2(fBeforeDirZ,fBeforeDirX) * 180.0 / PI;
        float mfNowAngle = atan2(vSpriteDirection.z,vSpriteDirection.x) * 180.0 / PI;
        
        mfCameraTurnPos -= mfNowAngle - mfBeforeAngle; //회전한 각도 만큼 빼준다.
        mfCameraTurnPos = AngleArrange(mfCameraTurnPos);
    }
    
    
    fViewDirX = vSpriteDirection.x;
    fViewDirZ = vSpriteDirection.z;
    
    fBeforeDirX = fViewDirX;
    fBeforeDirZ = fViewDirZ;
    
    if(fUpDownVector != 0.0f)
    {
//        int nTime = 80;//GetClockDeltaConst();
        mfCameraUpDownPos += fUpDownVector;
    
        if(ptSpritePosition.y  + mfCameraUpDownPos > nMaxY)
            mfCameraUpDownPos = nMaxY - ptSpritePosition.y; //카메라 줌인 아웃에 대한 흔들림 수정함. 2013.01.08
        
        if(ptSpritePosition.y  + mfCameraUpDownPos < mMinY)
            mfCameraUpDownPos = mMinY - ptSpritePosition.y; //카메라 줌인 아웃에 대한 흔들림 수정함. 2013.01.08
    }
    
    if(fTurnVector != 0.0f)
    {
        mfCameraTurnPos += fTurnVector;
        mfCameraTurnPos = AngleArrange(mfCameraTurnPos);
    }
    
    if(mfCameraTurnPos != 0.0f)
    {
        GLfloat vViewDir[3];
        GLfloat matrix[16];
        GLfloat pos[3];
        
        pos[0] = -1;
        pos[1] = 0;
        pos[2] = 0;
    
        sglLoadIdentityf(matrix);
        sglRotateRzRyRxMatrixf(matrix,
                               0,
                               mpModel->orientation[1] + mfCameraTurnPos,//탱크 해더 방향을 기준으로 돌려면 + mfModelHeaderAngle);
                               0);
        sglMultMatrixVectorf(vViewDir, matrix,pos);
        
        fViewDirX = vViewDir[0];
        fViewDirZ = vViewDir[2];
    }
    
    
    if(mfCameraUpDownPos > 32.f)
        mfCameraUpDownPos = 32.f; //32.f이상으로 옮겨지지 않게 하자.

    //(55 - 16) : (ZV - NormalViewMinZ) = X : (mfCameraUpDownPos - NormalViewMinZ)
    //fDiff = 39.0f * (mfCameraUpDownPos - NormalViewMinY) / (NormalViewMaxY - NormalViewMinY) + 16;
    fDiff = NormalViewMaxY * (mfCameraUpDownPos - NormalViewMinY) / (NormalViewMaxY - NormalViewMinY) + NormalViewMinY;


    
    ptCameraPos.x = mpModelHeader->position[0] - fViewDirX * fDiff;
    ptCameraPos.y = mpModelHeader->position[1] + mfCameraUpDownPos;
    ptCameraPos.z = mpModelHeader->position[2] + fViewDirZ * fDiff;
    
    if(CameraComact(&ptCameraPos,&fPosY))
    {
        ptCameraPos.x = mpModelHeader->position[0] - fViewDirX * 10.f;
        ptCameraPos.y = mpModelHeader->position[1] + 9.6;
        ptCameraPos.z = mpModelHeader->position[2] + fViewDirZ * 10.f;
        
    }
    
    mpCamera->viewPos.x = ptCameraPos.x;
    mpCamera->viewPos.y = ptCameraPos.y;
    mpCamera->viewPos.z = ptCameraPos.z;
    
    mpCamera->viewDir.x = fViewDirX;
    mpCamera->viewDir.z = -fViewDirZ;
    

    
#if defined(ACCELATOR)
    mAccelatorViewDirBefor.x = mpCamera->viewDir.x;
    mAccelatorViewDirBefor.z = mpCamera->viewDir.z;
    mpCamera->viewDir.y = mAccelatorViewDirBefor.y;
#else
    //-30.f ~ -49.5
    mpCamera->viewDir.y =  -0.60 * (float)(mfCameraUpDownPos - NormalViewMinY) / (float)(NormalViewMaxY - NormalViewMinY) - 0.15f;
#endif //ACCELATOR
    
    //-0.14285713
    mpCamera->viewUp.x = 0;
    mpCamera->viewUp.y = 1;
    mpCamera->viewUp.z = 0;
    
    //단위벡터로 변경한다.
    sglNormalize(&mpCamera->viewDir);
    mpCamera->ViewFar = mpCamera->ViewFarOrg;

}

void  CK9Sprite::SetK9CameraType(int v)
{
    mnCameraType = v;
	if(mpCamera == NULL) return ;
        
    if(v == CAMERT_BACK) //뒤에서 바라본다...
    {
        HandleCamera(0.0f,0.0f);
    }
    else 
    {
        CSprite::SetCameraType(v);
    }
}


void    CK9Sprite::SetRotationVelocity(float v)
{ 
    CSprite::SetRotationVelocity(v * mProperty.fRotationRate);
}

void    CK9Sprite::SetMoveVelocity(float v) 
{ 
    CSprite::SetMoveVelocity(v * mProperty.fMoveRate);
}

int CK9Sprite::GetFirePerSec() { return (float)mProperty.nFirePerSec / mBombProperty.fFirePerSec;}    //폭탄을 시간당 쏠수있는 능력

//방어력
float CK9Sprite::GetMaxDefendPower() 
{
    return mfMaxDefendPowerActor;
}      

float CK9Sprite::GetMaxScore()
{
    return mProperty.fMaxScore;
}

float CK9Sprite::GetMaxAttackPower()
{
    return m_fMaxAttackPower;
}

bool CK9Sprite::IsTabIn(CPicking* pPick,SPoint& nearPoint,SPoint& farPoint,int nPosX,int nPosY)
{
    return mpModel->IsPicking(pPick,nearPoint,farPoint,nPosX,nPosY);
}

bool CK9Sprite::IsTabInZeroGroup(CPicking* pPick,int nPosX,int nPosY)
{
    return mpModel->IsPickingZeroGroup(pPick,nPosX,nPosY);
}


void CK9Sprite::TakeRunTimeBomb(int nItemID,int nCount)
{
    CSprite::TakeRunTimeBomb(nItemID, nCount);
    
    if(mpCurrentSelMissaileInfo && mpCurrentSelMissaileInfo->nID == nItemID)
    {
        int nCnt = mpCurrentSelMissaileInfo->nCnt + nCount;
        CScenario::SendMessage(SGL_MESSAGE_FIRE_MISSILE,0,nCnt); //현재 선택한 버튼을 먹었으면 그 숫자값을 올려준다.
    }
    else
    {
        CHWorld* pWorld = (CHWorld*)mpWorld;
        pWorld->ResetDisplayItemSlot(); //슬롯을 화면에 반영해준다.
    }
}

void CK9Sprite::SetCurrentSelMissaileInfo(USERINFO* v)
{
    if(mpCurrentSelMissaileInfo == NULL || (mpCurrentSelMissaileInfo && mpCurrentSelMissaileInfo->nID != v->nID))
    {
        //Android 의 경우.. 화면의 탭을 클릭하였을 경우, main thread와 gl thread가 틀리기 때문에
        //Bomb 객체를 만들때 Texture Bound를 할때 에러가 난다.
        if(!PROPERTY_BOMB::GetPropertyBomb(v->nID, mBombProperty))
        {
            HLogE("[Error] undefine missile id %d\n",v->nID);
            return ;
        }
        string sPathModel = mBombProperty.sModelPath;
        mpWorld->GetTextureMan()->GetTextureID(mBombProperty.sBombTailImgPath);
    }
    
    mpCurrentSelMissaileInfo = v;
    

    //간혹 훈련병모드일때 죽을때 영원히 nType = -1일때가 존재한다.
    if(GetWorld()->GetUserInfo()->GetRank() != 0 && mpCurrentSelMissaileInfo->nID == BOMB_ID_2SHOT && mpCurrentSelMissaileInfo->nType == -1)
        mpCurrentSelMissaileInfo->nType = 0;
}

void CK9Sprite::SetCurrentSelMissaileInfo(int nBombID)
{
    if(!PROPERTY_BOMB::GetPropertyBomb(nBombID, mBombProperty))
    {
        HLogE("[Error] undefine missile id %d\n",nBombID);
        return ;
    }
    string sPathModel = mBombProperty.sModelPath;
    mpWorld->GetTextureMan()->GetTextureID(mBombProperty.sBombTailImgPath);
}



void CK9Sprite::CampactAnimaionHeader(int nTime)
{
    const float fVelocity = 20.0f;
    SPoint  ptNow;
    
	float fHAngle = mnCompactAni_Header_HAngle * PI / 180.0f; //지표면에 대한 각도로 발사
    
	float fHcos = cosf(fHAngle);
	float fHsin = sinf(fHAngle);
	float fVy0 = fVelocity * fHsin;	//높이 초기속도
    float fVx0 = fVelocity * fHcos;	//멀리 초기속도
    
    
    
    //폭탄 속도
    float fTime = (float)mnCompactAni_EscapeTime / 500.f;
	//멀리 방향으로 운동
	float fX = fVx0 * fTime;
	//높이 방향으로 운동
	float fY = fVy0 * fTime - 9.8f * fTime * fTime;
	
	float ptModel[3],ptTemp[3];
	
	GLfloat matrix[16];
	
	ptTemp[0] = fX;
	ptTemp[1] = fY;
    ptTemp[2] = 0;
    
	sglLoadIdentityf(matrix);
    
	sglRotateRzRyRxMatrixf(matrix,
                           0,
                           mnCompactAni_Header_WAngle * 30,
                           0); //좌우의 방향..
	
	sglMultMatrixVectorf(ptModel, matrix, ptTemp);
	
	ptNow.x = ptModel[0] + mptCompactAni_Header_StartPos.x;
	ptNow.y = ptModel[1] + mptCompactAni_Header_StartPos.y;
	ptNow.z = ptModel[2] + mptCompactAni_Header_StartPos.z;
	
	//현재 위 폭탄위치를 재설 정한다.
	mpModelHeader->position[0] = ptNow.x;
    mpModelHeader->position[1] = ptNow.y;
    mpModelHeader->position[2] = ptNow.z;
    
    mpModelHeader->orientation[0] += (float)nTime*0.1f;
    mpModelHeader->orientation[1] += (float)nTime*0.3f;
    mpModelHeader->orientation[2] += (float)nTime*0.4f;
}

void CK9Sprite::CampactAnimaionBody(int nTime)
{
    const float fVelocity = 25.0f;
    SPoint  ptNow;
    
	float fHAngle = mnCompactAni_Body_HAngle * PI / 180.0f; //지표면에 대한 각도로 발사
    
	float fHcos = cosf(fHAngle);
	float fHsin = sinf(fHAngle);
	float fVy0 = fVelocity * fHsin;	//높이 초기속도
    float fVx0 = fVelocity * fHcos;	//멀리 초기속도
    
    //폭탄 속도
    float fTime = (float)mnCompactAni_EscapeTime / 500.f;
	//멀리 방향으로 운동
	float fX = fVx0 * fTime;
	//높이 방향으로 운동
	float fY = fVy0 * fTime - 9.8f * fTime * fTime;
	
	float ptModel[3],ptTemp[3];
	
	GLfloat matrix[16];
	
	ptTemp[0] = fX;
	ptTemp[1] = fY;
    ptTemp[2] = 0;
    
	sglLoadIdentityf(matrix);
    
	sglRotateRzRyRxMatrixf(matrix,
                           0,
                           mnCompactAni_Body_WAngle * 30,
                           0); //좌우의 방향..
	
	sglMultMatrixVectorf(ptModel, matrix, ptTemp);
	
	ptNow.x = ptModel[0] + mptCompactAni_Body_StartPos.x;
	ptNow.y = ptModel[1] + mptCompactAni_Body_StartPos.y;
	ptNow.z = ptModel[2] + mptCompactAni_Body_StartPos.z;
	
	//현재 위 mpModel 재설 정한다.
	mpModel->position[0] = ptNow.x;
    mpModel->position[1] = ptNow.y;
    mpModel->position[2] = ptNow.z;
    
    mpModel->orientation[0] += (float)nTime*0.3f;
    mpModel->orientation[1] += (float)nTime*0.2f;
    mpModel->orientation[2] += (float)nTime*0.1f;


    
    if( ptNow.y < mptCompactAni_Body_StartPos.y )
    {
        //지운다.
        SetState(SPRITE_READYDELETE);
    }

}
void CK9Sprite::CampactAnimaion(int nTime)
{
    static unsigned int nNom = 0;
    if(mnCompactAni_EscapeTime == 0)
    {
        srand((unsigned int)time(NULL) + nNom++);
        int nR = rand() % 10;
        mnCompactAni_Header_HAngle = nR + 60.f;
        mnCompactAni_Header_WAngle = nR * 30;
        mptCompactAni_Header_StartPos.x = mpModelHeader->position[0];
        mptCompactAni_Header_StartPos.y = mpModelHeader->position[1];
        mptCompactAni_Header_StartPos.z = mpModelHeader->position[2];
    
        mnCompactAni_Body_HAngle = nR + 60.f;
        mnCompactAni_Body_WAngle = mnCompactAni_Header_WAngle + 65.f;
        mptCompactAni_Body_StartPos.x = mpModel->position[0];
        mptCompactAni_Body_StartPos.y = mpModel->position[1];
        mptCompactAni_Body_StartPos.z = mpModel->position[2];
    }
    mnCompactAni_EscapeTime += nTime;
    
    CampactAnimaionHeader(nTime);
    CampactAnimaionBody(nTime);
}


bool CK9Sprite::VisibleByCamera()
{
    float fD1[3];
    float fD2[3];
    
    SGLCAMERA* pcam = mpWorld->GetCamera();
    fD1[0] = pcam->viewDir.x;
    fD1[1] = pcam->viewDir.y;
    fD1[2] = pcam->viewDir.z;
    
    fD2[0] = mpModel->position[0] - pcam->viewPos.x;
    fD2[1] = mpModel->position[1] - pcam->viewPos.y;
    fD2[2] = mpModel->position[2] - pcam->viewPos.z;
    sglNormalizef(fD2);
    
    //fDV > 0 90보다 작다
    //fDV < 0 90보다 크다.
    //fDV = 0 90
    float fDV = sglDotProductf(fD1, fD2);
    
    if(fDV > 0.6f) return true;
    
    return false;
    
}


//장애물에 막힌지점
int CK9Sprite::GetTargetPos(SPoint *pPosition,SPoint *pToOutPosion,SVector* pOutDirection)
{
    float fx =  pToOutPosion->x - pPosition->x;
    float fy =  pToOutPosion->y - pPosition->y;
    float fz =  pToOutPosion->z - pPosition->z;
    
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
        HLogE("CK9Sprite::GetTargetPos : fLen Nan\n");
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


bool CK9Sprite::CanFireGun()
{
    if(mpAICore == NULL) return false;
    CSprite* pEn = mpAICore->GetAttackToByGun();
    if(pEn == NULL) return false;
    unsigned long lN = GetGGTime();
    if(lN < mnFireGunInterval)
        return false;
    return true;
}

void  CK9Sprite::FireGun()
{
    SPoint ptNow;
    if(!CanFireGun() || mptStartGun == NULL) return;
    sglMesh *arrWroldMesh = mpModelHeader->GetWorldMesh();
    GLfloat *pPoPos = &arrWroldMesh[mptStartGun[0][0]].pVetexBuffer[mptStartGun[0][1] * 3];
    ptNow.x = pPoPos[0];
    ptNow.y = pPoPos[1];
    ptNow.z = pPoPos[2];
    NewGun(ptNow,true);
}


void  CK9Sprite::FireGunInvisible()
{
    SPoint ptNow;
    if(!CanFireGun() || mptStartGun == NULL) return;
    GetPosition(&ptNow);
    ptNow.y+= 5.f;
    NewGun(ptNow,false);
}


void CK9Sprite::NewGun(SPoint& ptNow,bool bNeedParticle)
{
    SVector vtDir;
    SVector vDirAngle;
    CSprite* pTargetSprite = mpAICore->GetAttackToByGun();
    
    CSGLCore* pCore = ((CHWorld*)mpWorld)->GetSGLCore();
    CBombGun *pNewBomb = NULL;
    SPoint ptTarget;
    
    if(pTargetSprite == NULL)        return ;

    pNewBomb = new CBombGun(pTargetSprite,this,GetID(),GetTeamID(),mGunProperty.nID,mpAction,GetWorld(),&mGunProperty);
    pTargetSprite->GetPosition(&ptTarget);
    
    if(pTargetSprite->GetType() != ACTORTYPE_FIGHTER)
        ptTarget.y += 5.f;
    
    if(GetTargetPos(&ptNow, &ptTarget,&vtDir) == -1)
    {
//        HLogE("Long Far CK9Sprite::NewGun %d\n",this);
        delete pNewBomb;
        mpAICore->ResetAttackTo();
        return;
    }
    
    mnFireGunInterval = GetGGTime() + 200;
    
    vDirAngle.x = asinf(vtDir.y) * 180.0 / PI;
    vDirAngle.y = mpModelHeader->orientation[1];
    vDirAngle.z = atan2(-vtDir.z,vtDir.x) * 180.0 / PI;
    
    pNewBomb->Initialize(&ptNow,&ptTarget, &vDirAngle,&vtDir,mBeforeLenght,mBeforeResult);
    pCore->AddBomb(pNewBomb);
    

    if(bNeedParticle)
    {
        float fIntervalToActor = GetIntervalToCamera();
#ifdef ANDROID //JAVA의 터치시에 플래이가 JNI로 연결되어 잇어서 에러가 난다.
        if(fIntervalToActor < 3500)
            CScenario::SendMessage(SGL_PLAYSOUND,mGunProperty.nSoundFilreID,0);
#else
        if(fIntervalToActor < 3500)
            pCore->PlaySystemSound(mGunProperty.nSoundFilreID);
#endif
    }

}


void CK9Sprite::Serialize(CArchive& ar,bool bWrite)
{
    CSprite::Serialize(ar, bWrite);
    if(bWrite)
    {
        int nTemp;
        //---------------------------------
        //CSprite
        
        ar << mnType; //1:    1:K9시리즈 2:ET1시리즈(날라다니는) 3:BlzItem 4:DMainTower 5:Fighter 6:GhostSprite
        ar << mnCameraType;
//        IAction*        mpAction;
        nTemp = (int)mState;
        ar <<           nTemp;

        ar <<           m_nCurrentPerTime;       //폭탄의 시간당 발사 수 조절한다.
        ar <<           m_fCurrentDefendPower;  //현재 생명게이지
        ar <<           m_fMaxAttackPower;
        ar <<           m_bHodeFire;
        //현재 주인공과의 거리 (가까운곳에서 폭탄이 터지면 터지는 효과음을 내기위함 등등)
        ar <<           m_fIntervalToActor;
        
//        CSpriteStatus*  mpStatusBar;
//        CShadowParticle* mpShadowParticle;
        ar << mProperty.nID;
        ar << mBombProperty.nID;
        
        
        ar <<           mfMaxDefendPowerActor;
        
        ar <<           mfCameraUpDownVector; //카메라 업다운.
        ar <<           mfCameraUpDownPos;
        
        ar <<           mfCameraTurnVector;
        ar <<           mfCameraTurnPos;
        
        
        ar <<           mfColidedDistance;//충돌되었을 경우 상대와의 거리.
        ar <<           mfDefendRadianBounds;
//        unsigned long   mnReliveEffect; //다시살아나는 효과를 표현한다.
        
        nTemp = 0;
        if(mpRuntimeUpgradeList)
        {
            nTemp = (int)mpRuntimeUpgradeList->size();
            ar << nTemp;
            for (int i = 0; mpRuntimeUpgradeList->size(); i++) {
                USERINFO* info = mpRuntimeUpgradeList->at(i);
                ar << info->nID;
            }
        }
        else
        {
            nTemp = 0;
            ar << nTemp;
        }
        
        
        nTemp = (int)mpRuntimeUpgradeingList.size();
        ar << nTemp;
        for (int i = 0; nTemp; i++) {
            USERINFO* info = mpRuntimeUpgradeingList.at(i);
            ar << info->nID;
        }
        //----------------------------------
        
        
        
        //탱크해더의 회전 벡터를 나타낸다. 180보다 크면 오른쪽 왼쪽으로 돌지 나타낸다.
        ar <<   mfModelHeaderRotVector;
        ar <<   mfModelHeaderAngle;
        ar <<   mnPoUpDownState; //0 움직이지 않음, 1 : 위로 움직임, -1: 아래로 움직임
        ar <<	mfAutoTurnAHeader_ToAngle;		//이각도 방향으로 이동
        ar <<   mfAutoMovePo_ToUpDownAngle;
        ar <<   mfAutoMovePo_UpDonwDir; //-1아래 1위방향으로 이동
        
        ar << mHeaderPosition[0];
        ar << mHeaderPosition[1];
        ar << mHeaderPosition[2];
        ar << mFireAnimationVector; // x < 0 (뒤의 방향), x > 0 (앞의 방향), x = 0 (정지상태)
        ar << mFireMovedHeader;
        
        for (int i = 0; i < 5; i++) {
            ar << mPSRightBP[i].x;
            ar << mPSRightBP[i].y;
            ar << mPSRightBP[i].z;
        }
        for (int i = 0; i < 5; i++) {
            ar << mPSLeftBP[i].x;
            ar << mPSLeftBP[i].y;
            ar << mPSLeftBP[i].z;
        }

        ar << mfHalfX;
        ar << mfHalfZ;
        
        ar << SGL_PO_STEP;             //36
        ar << SGL_PO_HALFSTEP;         //18
        
        if(mpCurrentSelMissaileInfo)
            ar << mpCurrentSelMissaileInfo->nID;
        
        ar << mptLastPos.x;
        ar << mptLastPos.y;
        ar << mptLastPos.z;
        
        ar << mnPoStartCnt;
        for (int i = 0; i < mnPoStartCnt; i++) {
            ar << mptStartMissile[i][0];
            ar << mptStartMissile[i][1];
            ar << mptStartMissile[i][2];
        }
        ar << mnPoIndex;
        
        ar << mnGunStartCnt;
        for (int i = 0; i < mnGunStartCnt; i++) {
            ar << mptStartGun[i][0];
            ar << mptStartGun[i][1];
            ar << mptStartGun[i][2];
        }
        
        
        ar << mnMapID;
        ar << mGunProperty.nID;
        
//        long nLiveTime  = 0;
//        if(mpBubbleParticle)
//            nLiveTime = mpBubbleParticle->GetMaxTime() -GetGGTime();
//        ar << nLiveTime;
    }
    else
    {
    }
}


void CK9Sprite::ArchiveMultiplay(int nCmd,CArchive& ar,bool bWrite)
{
    bool bActor = false;
    CSprite::ArchiveMultiplay(nCmd, ar, bWrite);
    if(nCmd == SUBCMD_OBJ_ALL_INFO)
    {
        if(bWrite)
        {
            bActor = mpAICore->IsActor();
            ar << bActor;
            ar << mpModel->orientation[0];
            ar << mpModel->orientation[1];
            ar << mpModel->orientation[2];
            ar << mpModel->position[0];
            ar << mpModel->position[1];
            ar << mpModel->position[2];
            
            ar <<           m_nCurrentPerTime;       //폭탄의 시간당 발사 수 조절한다.
            ar <<           mProperty.nID;
            ar <<           mBombProperty.nID;
            ar <<           mnMapID;
            ar <<           mGunProperty.nID;
            ar <<           mfDefendRadianBounds;
            
            //주인공의 폭탄....
            if(mpCurrentSelMissaileInfo)
                ar << mpCurrentSelMissaileInfo->nID;
            else
                ar << mBombProperty.nID;
        }
        else
        {
            ar >> bActor;
            ar >> mpModel->orientation[0];
            ar >> mpModel->orientation[1];
            ar >> mpModel->orientation[2];
            ar >> mpModel->position[0];
            ar >> mpModel->position[1];
            ar >> mpModel->position[2];


            ar >>           m_nCurrentPerTime;       //폭탄의 시간당 발사 수 조절한다.
            ar >>           mProperty.nID;
            ar >>           mBombProperty.nID;
            ar >>           mnMapID;
            ar >>           mGunProperty.nID;
            ar >>           mfDefendRadianBounds;
            ar >>           mBombProperty.nID;
            
            SetCurrentSelMissaileInfo(mBombProperty.nID);
            
            ArrangeSprite(); //모든 위치를 재정리한다.
            
            if(bActor)
                mpAICore->SetActor(); //AI를 거시기 해준다.
        }
        
        if(mpAICore->GetMoveAI())
            mpAICore->GetMoveAI()->ArchiveMultiplay(nCmd, ar, bWrite);
        if(mpAICore->GetStategyAI())
            mpAICore->GetStategyAI()->ArchiveMultiplay(nCmd, ar, bWrite);

    }
    
}


#ifdef ACCELATOR
#define fAccMin  0.4f
#define fAccMax  0.9f

#define fAccVdirMin -0.48f
#define fAccVdirMax -0.29f
#define fAccMoveFact 0.0002f
void CK9Sprite::SetAccelatorUpVector(float fv)
{
    static float fRate =   (fAccVdirMax - fAccVdirMin) / (fAccMax - fAccMin);
    mfAccelatorViewDirY = fRate   *  (fv - fAccMin) + fAccVdirMin;
    if(mpCamera == NULL) return;
    
    float fNowDirY = mAccelatorViewDirBefor.y;
    if(mfAccelatorViewDirY < fNowDirY)                  mfAccelatorViewDirVector = -1.f;
    else if(mfAccelatorViewDirY > fNowDirY)             mfAccelatorViewDirVector = +1.f;
    else mfAccelatorViewDirVector = 0.f;
}


void CK9Sprite::HandleAccelCamera()
{
    
    float fTime = (float)GetClockDeltaConst();
    if(mfAccelatorViewDirVector == 0.f) return;
    float fY = mAccelatorViewDirBefor.y + fTime * fAccMoveFact * mfAccelatorViewDirVector;
    
    if(mfAccelatorViewDirVector < 0 && fY < mfAccelatorViewDirY)
    {
        fY = mfAccelatorViewDirY;
        mfAccelatorViewDirVector = 0.f;
    }
    else if(mfAccelatorViewDirVector > 0 && fY > mfAccelatorViewDirY)
    {
        fY = mfAccelatorViewDirY;
        mfAccelatorViewDirVector = 0.f;
    }
    else if(fY < fAccVdirMin)
        fY = fAccVdirMin;
    else if(fY > fAccVdirMax)
        fY = fAccVdirMax;
    mAccelatorViewDirBefor.y = fY;

    mpCamera->viewDir.x = mAccelatorViewDirBefor.x;
    mpCamera->viewDir.z = mAccelatorViewDirBefor.z;
    mpCamera->viewDir.y = mAccelatorViewDirBefor.y;
    sglNormalize(&mpCamera->viewDir);
}
#endif //ACCELATOR
