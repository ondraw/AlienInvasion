//
//  CTrainingWorld.cpp
//  SongGL
//
//  Created by 호학 송 on 13. 5. 27..
//
//

#include "CTrainingWorld.h"
#include "sGL.h"
#include "sGLUtils.h"
#include "CBackground.h"
#include "CSGLCore.h"
#include "CParticleEmitterMan.h"
#include "CSpriteStatus.h"
#include "CNumberCtl.h"
#include "CMessageCtl.h"
#include "CButtonCtl.h"
#include "sGLText.h"
#include "C2dStick.h"
#include "CK9Sprite.h"
#include "CArrowMotionSprite.h"
#include <math.h>
#include "sGLUtils.h"
#include "AAICore.h"
#include "CAniRuntimeBubble.h"
#include "CDTower.h"
#include "CAICore.h"
#include "CProgressCtl.h"
#include "CMoveAI.h"

#define DEFAULT_MAX_TIMEOUT 10000 //10초

#ifdef New_CTrainingWorld


#define INDEX_STARTPOS 0
#define INDEX_MOVEPOS  1
#define INDEX_ATTACKPOS 2
#define INDEX_ATTACKPOS2 3
#define INDEX_ATTACKPOS3 6
#define INDEX_ATTACKPOS4 4
#define INDEX_MAINTOWER5 5



CTrainingWorld::CTrainingWorld(CScenario* pScenario) : CHWorld(pScenario)
{
    mpMessageDlg = NULL;
    mpStepMessageDlg = NULL;
    mnNextTime = 0;
    mpStepMessageDes = NULL;
    mpStepMessageDes2 = NULL;
    mpStepMessageTitle = NULL;
    mpMoveTargetArrowSprite = NULL;
    mEnamySprite = NULL;
    mEnamySprite2= NULL;
    //    mpCarmeraRotationTartketPoint = NULL;
    mpRaidaFingerVertex = NULL;
    mpArrowBlz = NULL;
    
    mMoveMotionStep = 0;
    mnRenderMarkControlTime = 0;
    mbRenderMarkControlTime = true;
    mbUpgrading = false;
}

CTrainingWorld::~CTrainingWorld()
{
    mpStepMessageTitle = NULL;
    mpStepMessageDes = NULL;
    mpStepMessageDes2 = NULL;
    
    if(mpMessageDlg)
    {
        delete mpMessageDlg;
        mpMessageDlg = NULL;
    }
    
    if(mpStepMessageDlg)
    {
        delete mpStepMessageDlg;
        mpStepMessageDlg = NULL;
    }
    
    if(mpArrowBlz)
    {
        delete mpArrowBlz;
        mpArrowBlz = NULL;
    }
    
    SetRotationCarmeraToPoint(NULL);
    
    if(mpRaidaFingerVertex)
    {
        delete[] mpRaidaFingerVertex;
        mpRaidaFingerVertex= NULL;
    }
    
}

int CTrainingWorld::Initialize2(SGLEvent *pEvent)
{
    CHWorld::Initialize2(pEvent);
    mStep = TRAININGSTEP_NONE;
    return E_SUCCESS;
}

int CTrainingWorld::Initialize3(SGLEvent *pEvent)
{
    CHWorld::Initialize3(pEvent);
    mpItemSlotBar->SetHide(true);
    mpRadaBoard->SetHide(true);
    mpSelectedObjectDescBar->SetHide(true);
    mnFingerTexture = GetSGLCore()->GetTextureMan()->GetTextureID("Finger.tga");
    mfFingerUpDwonAction = 15.f;
    mfFingerUpDwonActionDirection = -1.f;
    
    return E_SUCCESS;
}


void CTrainingWorld::MakeActorAtStartPostion()
{
    
    int nv = INDEX_STARTPOS; //무조건 0부터 시작한다.
    CUserInfo* pUserInfo = 0;
    pUserInfo = mpScenario->GetUserInfo();
    STARTPOINT *pPoint = m_pSGLCore->mlstStartPoint[nv];
    
    CSprite* pSprite = CSGLCore::MakeSprite(GetNewID(),pPoint->nTeamID,pPoint->nModelID,GetTextureMan(),m_pSGLCore->GetAction(),this,NETWORK_MYSIDE);
    if(pSprite)
    {
        SVector vtDir;
        vtDir.x = pPoint->fDirX;
        vtDir.y = pPoint->fDirY;
        vtDir.z = pPoint->fDirZ;
        
        SPoint  ptPos;
        ptPos.x = pPoint->fPosX;
        ptPos.y = pPoint->fPosY;
        ptPos.z = pPoint->fPosZ;
        
        //GetPositionZ(&ptPos);
        pSprite->LoadResoure(GetModelMan());
        pSprite->Initialize(&ptPos, &vtDir);
        m_pSGLCore->AddSprite(pSprite);
        SetActor(pSprite); //주인공이다.
        pSprite->SetCurrentDefendPower(9999999.f);
        //주인공은 트레이닝 모드로 선언한다.
        GetActor()->GetAICore()->SetIsTrainningMode(true);
        pSprite->SetCameraType(CAMERT_BACK); //실제로 카메라 타입을 셋한다.
        
        vector<USERINFO*>*lstBomb = pUserInfo->GetListBomb();
        lstBomb->erase(lstBomb->begin()); //기본탄을 제거하고
        pUserInfo->AddListToBomb(6,0,-1); //분산타2을 추가해준다.
        int nBombSize = (int)lstBomb->size();
        
        if(nBombSize > 0)
        {
            CK9Sprite *pSP = dynamic_cast<CK9Sprite*>(pSprite);
            if(pSP)
                pSP->SetCurrentSelMissaileInfo((*lstBomb)[nBombSize - 1]);
        }
    }
}

void CTrainingWorld::SendMakeBlzMessage()
{
    //건물을 짖는다.
    float* fNew = new float[3];
    memcpy(fNew,mpStick->GetFlyPosition(),sizeof(float)*3);
    if(fNew[0] != -9999.f)
    {
        USERINFO* pInfo = (USERINFO*)mpBlzSelectedIcon->GetLData();
        if(pInfo->nCnt > 0)
            CScenario::SendMessage(SGL_MAKE_BLZ,pInfo->nID,GetActor()->GetTeamID(),0,(long)fNew);
    }
}

void CTrainingWorld::OnNextStep(enum TRAININGSTEP CurrentStep)
{
    unsigned long ggTime = GetGGTime();
    
    if(CurrentStep == TRAININGSTEP_NONE) //None이 끝나면 IntroDuce를 해준다.
    {
        mStep = TRAININGSTEP_INTRODUCE; //이동에 관련한 미션을 수행합니다.
        Setp1_IntroduceDlg();
        
//#ifdef MAC
//        mnNextTime = ggTime + 9000000.f;
//#else
        mnNextTime = ggTime + DEFAULT_MAX_TIMEOUT ;
//#endif
    }
    else
    {

        mnNextTime = ggTime + DEFAULT_MAX_TIMEOUT;

        
        CUserInfo* pUserInfo = mpScenario->GetUserInfo();
        if(CurrentStep == TRAININGSTEP_INTRODUCE)
        {
            Setp2_MoveDlg();
        }
        else if(CurrentStep == TRAININGSTEP_MOVE)
        {
            GetSGLCore()->PlaySystemSound(SOUND_ID_EnergyUP);
            if(!pUserInfo->IsCompletedMap(pUserInfo->GetLastSelectedMapID()))
                CScenario::SendMessage(SGL_MESSAGE_MAKE_SCORE,10000*1000);
            Setp3_Attack();
        }
        else if(CurrentStep == TRAININGSTEP_ATTACK)
        {
            GetSGLCore()->PlaySystemSound(SOUND_ID_EnergyUP);
            if(!pUserInfo->IsCompletedMap(pUserInfo->GetLastSelectedMapID()))
                CScenario::SendMessage(SGL_MESSAGE_MAKE_SCORE,10000*1000);
            Setp5_AttackRadia();
        }
        else if(CurrentStep == TRAININGSTEP_ATTACKRADIA)
        {
            Setp5_1_ChangeSlot();
            GetSGLCore()->PlaySystemSound(SOUND_ID_EnergyUP);
        }
        else if(CurrentStep == TRAININGSTEP_CHANGE_SLOT)
        {
            GetSGLCore()->PlaySystemSound(SOUND_ID_EnergyUP);
            Setp6_MakeTower();
        }
        else if(CurrentStep == TRAININGSTEP_MAKETOWER)
        {
            mnNextTime = ggTime + DEFAULT_MAX_TIMEOUT * 2;
            GetSGLCore()->PlaySystemSound(SOUND_ID_EnergyUP);
            Setp6_Upgrade();
        }
        else if(CurrentStep == TRAININGSTEP_UPGRADE)
        {
            mnNextTime = ggTime + DEFAULT_MAX_TIMEOUT * 2;
            GetSGLCore()->PlaySystemSound(SOUND_ID_EnergyUP);
            if(!pUserInfo->IsCompletedMap(pUserInfo->GetLastSelectedMapID()))
                CScenario::SendMessage(SGL_MESSAGE_MAKE_SCORE,8000*1000);
            Setp7_Mission(); //업그레이드를 추천한다.
        }
        else if(CurrentStep == TRAININGSTEP_MISSION)
        {
            GetSGLCore()->PlaySystemSound(SOUND_ID_EnergyUP);
            Setp8_End0();
        }
        else if(CurrentStep == TRAININGSTEP_END0)
        {
            Setp8_End();
        }
        
    }
}

int CTrainingWorld::RenderBegin()
{
    int nTime = 0;
    
	//초기화가 안되었거나 초기화중이다.
	if( (mnStatus & 0x0002) != 0x0002)
		return E_SUCCESS;
    
    mpThread->BeginPong();
    
    
	nTime = GetClockDeltaConst();
    
    
    mpArrowMotionSprite->RenderBegin(nTime);
    mpStick->RenderBegin(nTime);
    if(!IsPauseTime())
        m_pSGLCore->RenderBegin(nTime);
	mpCurrRuntimeRenderObjects = (RuntimeRenderObjects*)mpThreadQueue->Poll();
    if(mpCurrRuntimeRenderObjects == 0)
    {
        mpThread->EndPong();
        return E_SUCCESS;
    }
	
    if(!IsPauseTime())
    {
        //1> 폭탄을 그린다.
        ByNode<CBomb>* bb = mpCurrRuntimeRenderObjects->plistBomb->begin();
        for (ByNode<CBomb>* it = bb; it != mpCurrRuntimeRenderObjects->plistBomb->end(); )
        {
            CBomb* Object = it->GetObject();
            Object->RenderBegin(nTime);
            it = it->GetNextNode();
        }
        
        //2> 스프라이트를 그린다.
        ByNode<CSprite>* sb =  mpCurrRuntimeRenderObjects->plistSprite->begin();
        for(ByNode<CSprite>*it = sb; it != mpCurrRuntimeRenderObjects->plistSprite->end();)
        {
            CSprite* Object = it->GetObject();
            Object->RenderBegin(nTime);
            it = it->GetNextNode();
        }
        
        //3> 파티클
        ByNode<AParticle>* ab = mpCurrRuntimeRenderObjects->plistParticle->begin();
        for(ByNode<AParticle>* it = ab; it != mpCurrRuntimeRenderObjects->plistParticle->end();)
        {
            AParticle* Object = it->GetObject();
            Object->RenderBegin(nTime);
            it = it->GetNextNode();
        }
    }
    
    if(mBackground)
        mBackground->RenderBegin(nTime);
    
    
    if(mpRadaBoard && mpRadaBoard->GetHide() == false)
    {
        ByNode<RadaBoardObject>* rb = mpCurrRuntimeRenderObjects->plistBoardObjects->begin();
        for(ByNode<RadaBoardObject>* it = rb; it != mpCurrRuntimeRenderObjects->plistBoardObjects->end();)
        {
            RadaBoardObject* Object =  it->GetObject();
            
            if(Object && Object->pSprite && Object->pSprite->GetState() == SPRITE_RUN && Object->nBoardType == 1)
            {
                if(mpRaidaFingerVertex == NULL)
                    mpRaidaFingerVertex = new float[12];
                memcpy(mpRaidaFingerVertex, Object->pWorldVertex, sizeof(float) * 12);
                float fX = (mpRaidaFingerVertex[6] - mpRaidaFingerVertex[0]) / 2.f; //손가락을 탱크 중앙으로 옮겨준다.
                float fY = (mpRaidaFingerVertex[1] - mpRaidaFingerVertex[7]) / 2.f; //상단으로 옮겨준다.
                float matrix[16];
                sglLoadIdentityf(matrix);
                sglTranslateMatrixf(matrix, fX, fY, 0.0f);
                mfRaidaFingerMax = fY * 2;
                mfRaidaFingerMin = fY;
                
                sglMultMatrixVectorf(mpRaidaFingerVertex, matrix,mpRaidaFingerVertex);
                sglMultMatrixVectorf(mpRaidaFingerVertex+3, matrix,mpRaidaFingerVertex+3);
                sglMultMatrixVectorf(mpRaidaFingerVertex+6, matrix,mpRaidaFingerVertex+6);
                sglMultMatrixVectorf(mpRaidaFingerVertex+9, matrix,mpRaidaFingerVertex+9);
                
                break;
            }
            it = it->GetNextNode();
        }
        
        
        mpRadaBoard->SetObjects(mpCurrRuntimeRenderObjects->plistBoardObjects);
        mpRadaBoard->RenderBegin(nTime);
    }
    if(mStep == TRAININGSTEP_MAKETOWER)
    {
        
    }
    
    mpLeftTools->RenderBegin(nTime);
    mpItemSlotBar->RenderBegin(nTime);
    mpParticleMan->RenderBegin(nTime);
    
    
    
    //시간이 포즈가 되어도 실행되게끔 해주어야 한다.
    if(mpMissionMsgCtl)
        mpMissionMsgCtl->RenderBegin(50);
    
    //-----------------------------------------------------------------------------
    //CTrainingWorld 에서 필요한 로직
    //-----------------------------------------------------------------------------
    if(mpMessageDlg) mpMessageDlg->RenderBegin(nTime);
    if(mpStepMessageDlg) mpStepMessageDlg->RenderBegin(nTime);
    //-----------------------------------------------------------------------------
    
    
    if(mStep == TRAININGSTEP_MOVE)
    {
        STARTPOINT* pMoveTargetPostion = m_pSGLCore->mlstStartPoint[INDEX_MOVEPOS];
        
        float matrix[16];
        sglLoadIdentityf(matrix);
        sglTranslateMatrixf(matrix, pMoveTargetPostion->fPosX, pMoveTargetPostion->fPosY + 1.0f, pMoveTargetPostion->fPosZ);
        sglScaleMatrixf(matrix, 11.0f, 11.0f, 11.0f);
        
        sglMultMatrixVectorf(mMoveTargetGround, matrix,gPanelVertexZ);
        sglMultMatrixVectorf(mMoveTargetGround+3, matrix,gPanelVertexZ+3);
        sglMultMatrixVectorf(mMoveTargetGround+6, matrix,gPanelVertexZ+6);
        sglMultMatrixVectorf(mMoveTargetGround+9, matrix,gPanelVertexZ+9);
        
        if(mpMoveTargetArrowSprite)
        {
            if(GetActor()->CollidesBounding(mpMoveTargetArrowSprite, false)) //충돌되었다. 즉 원하는 위치까지 왔다면 다음스템으로 이동한다.
            {
                mpMoveTargetArrowSprite->SetState(SPRITE_READYDELETE);
                mpMoveTargetArrowSprite = NULL;
                //공격으로 변환한다.
                OnNextStep(mStep);
            }
        }
    }
    
    RotationCarmeraToPoint(nTime);
    
    mpSelectedObjectDescBar->SetHide(true);
    
    RenderFingerBegin(nTime);
    mpThread->EndPong();
    return E_SUCCESS;
}

void CTrainingWorld::RenderMarkControl(float* pVertex)
{
    //-----------------------------------------------------
    //깜빡깜빡거리게 한다.
    const float fDTime = 300.f;
    unsigned long nowTime = GetGGTime();
    if(nowTime > mnRenderMarkControlTime)
    {
        mbRenderMarkControlTime = !mbRenderMarkControlTime;
        if(mnRenderMarkControlTime == 0)
        {
            mnRenderMarkControlTime = nowTime + fDTime;
        }
        else
        {
            unsigned long nDif = nowTime - mnRenderMarkControlTime;
            if(nDif > fDTime)
                mnRenderMarkControlTime = nowTime + fDTime;
            else
                mnRenderMarkControlTime = nowTime + (fDTime - nDif);
        }
    }
    //-----------------------------------------------------
    
    glLineWidth(4.f);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);
//    float* fVerBuffer = pControl->GetWorldVertex();
    if(mbRenderMarkControlTime)
        sglColorf(1.0f, 0.0f, 0.0f, 1.0f);
    else
        sglColorf(0.0f, 0.0f, 1.0f, 1.0f);
    sglLinef(pVertex, pVertex+3);
    sglLinef(pVertex+3, pVertex+6);
    sglLinef(pVertex+6, pVertex+9);
    sglLinef(pVertex+9, pVertex);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnable(GL_TEXTURE_2D);
    glLineWidth(1.f);
}

int CTrainingWorld::Render()
{
    //버퍼를 클리어 해준다.
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_ALPHA_TEST); //속도가 높아진다는 소리가 있어서 한번해봤다.
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
    
#ifdef ANDROID //Android에서는 매번 셋팅해주어야 한다.
    //방향벡터의 normalize를 하지 않음. 그래서 gl에서 해주어야 샤이닝이 이러난다.
    //모델의 빛 반사를 정의 하려면 GL_NORMALIZE화 해주어야 한다.
    glEnable(GL_NORMALIZE);
#endif
    glColor4f(1.0f,1.0f,1.0f,1.0f);	 //glColor4f(1.0f,1.0f,1.0f,alpha);
	
	int nCameraType = 0;
	//초기화가 안되었거나 초기화중이다.
	if( (mnStatus & 0x0002) != 0x0002 || mpCurrRuntimeRenderObjects == 0) return E_SUCCESS;
    
    
    nCameraType = GetCameraType();
    
    //---------------------------------------------------
    //안개
    if(m_pSGLCore->m_fFogDentity != 0.0f)
    {
        //        GLfloat fcolor[4] = {0.7f, 0.7f, 0.7f, 1.0f};
        glEnable(GL_FOG);
#ifdef MAC
        glFogi(GL_FOG_MODE, GL_LINEAR);
#else
        glFogx(GL_FOG_MODE, GL_LINEAR);
#endif
        glFogf(GL_FOG_START, m_pSGLCore->m_fFogStart);//150.f);
        glFogf(GL_FOG_END, m_pSGLCore->m_fFogEnd);//350.f);
        glFogf(GL_FOG_DENSITY,m_pSGLCore->m_fFogDentity);//2.f);
        glFogfv(GL_FOG_COLOR, m_pSGLCore->m_fFogColor);
    }
    //---------------------------------------------------
    
    //하늘에서 내려다 보는 것 아닌 것은 하늘을 그린다.
	if(nCameraType < CAMERT_UP_VISIBLE)
    {
        if(mBackground)
        {
            mBackground->ActivateCamera(GetCamera());
            mBackground->Render();
        }
    }
    
	ActivateCamera();
    
    //1>그리는 순서를 캐릭터를 먼져그리고, 지형을 나중에 그린다.
    //그렇게 하면 나무같은 곳에 숨겨지기때문에 현실감이 있다.
    //스프라이트를 그린다.
#ifdef ANDROID
    SetLight();
#endif
    
    //    //---------------------------------------------------
    //    //안개
    //    if(m_pSGLCore->m_fFogDentity != 0.0f)
    //    {
    //        //        GLfloat fcolor[4] = {0.7f, 0.7f, 0.7f, 1.0f};
    //        glEnable(GL_FOG);
    //#ifdef MAC
    //        glFogi(GL_FOG_MODE, GL_LINEAR);
    //#else
    //        glFogx(GL_FOG_MODE, GL_LINEAR);
    //#endif
    //        glFogf(GL_FOG_START, m_pSGLCore->m_fFogStart);//150.f);
    //        glFogf(GL_FOG_END, m_pSGLCore->m_fFogEnd);//350.f);
    //        glFogf(GL_FOG_DENSITY,m_pSGLCore->m_fFogDentity);//2.f);
    //        glFogfv(GL_FOG_COLOR, m_pSGLCore->m_fFogColor);
    //    }
    //    //---------------------------------------------------
    
    ByNode<CSprite>* sb =  mpCurrRuntimeRenderObjects->plistSprite->begin();
    for(ByNode<CSprite>*it = sb; it != mpCurrRuntimeRenderObjects->plistSprite->end();)
	{
        CSprite* Object = it->GetObject();
		Object->Render();
        it = it->GetNextNode();
	}
    
    mpArrowMotionSprite->Render();
    
    //2>지형을 그랜다.
    m_pSGLCore->Render();
    
    
    //----------------------------------------
    //3> 폭탄을 그린다. 지형에 숨겨지지 않게 그리면 폭탄의 계적이 보여서 좋다.
    //순서의 반대로 그려준다.
    ByNode<CBomb>* be = mpCurrRuntimeRenderObjects->plistBomb->end()->GetPrevNode();
	for (ByNode<CBomb>* it = be; it != &mpCurrRuntimeRenderObjects->plistBomb->GetHead(); )
    {
		CBomb* Object = it->GetObject();
		Object->Render();
        it = it->GetPrevNode();
	}
	
    
    //-------------------------------
    glEnableClientState(GL_VERTEX_ARRAY);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glDepthMask(GL_FALSE);
    
    //Bomb Tracking..
    mpParticleMan->Render();
    
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnable(GL_TEXTURE_2D);
    
    
    if(mStep == TRAININGSTEP_MOVE)
    {
        glBindTexture(GL_TEXTURE_2D, m_pSGLCore->GetTextureMan()->GetTextureID(IMG_PARKING));
        glTexCoordPointer(2, GL_FLOAT, 0, gPanelCoordTex);
        glVertexPointer(3, GL_FLOAT, 0, mMoveTargetGround);
        glDrawElements(GL_TRIANGLE_STRIP, gshPanelIndicesCnt, GL_UNSIGNED_SHORT, gPanelIndices);
    }
    
    mpStick->RenderMoveingFly();
    
    
    
    //3> 파티클
    ByNode<AParticle>* ab = mpCurrRuntimeRenderObjects->plistParticle->begin();
	for(ByNode<AParticle>* it = ab; it != mpCurrRuntimeRenderObjects->plistParticle->end();)
	{
		AParticle* Object = it->GetObject();
		Object->Render();
        it = it->GetNextNode();
	}
    
    RenderFinger();
    
    if(m_pSGLCore->m_fFogDentity != 0.0f)
        glDisable(GL_FOG);
    
    //2D는 뎁스를 끄고 그려준다.
    glDisable(GL_DEPTH_TEST);
    
    
    //-----------------------------------------------------------------------------
    //4> 스프라이트 에너지 상태를 나타낸다...
    glDisable(GL_TEXTURE_2D);
    glEnableClientState(GL_COLOR_ARRAY);
    for(ByNode<CSprite>*it = sb; it != mpCurrRuntimeRenderObjects->plistSprite->end();)
	{
        CSprite* Object = it->GetObject();
		if(Object->GetState() == SPRITE_RUN && Object->GetSpriteStatus())
            Object->GetSpriteStatus()->Render();
        it = it->GetNextNode();
	}
    glDisableClientState(GL_COLOR_ARRAY);
    glEnable(GL_TEXTURE_2D);
    //-----------------------------------------------------------------------------
    
    
    CControl::ActivateCamera();
    mpStick->Render();
    mpRadaBoard->Render();
    mpLeftTools->Render();
    mpItemSlotBar->Render();

    //-----------------------------------------------------
    if(mStep == TRAININGSTEP_CHANGE_SLOT && mpItemSlotSwitchBtn)
        RenderMarkControl(mpItemSlotSwitchBtn->GetWorldVertex());
    else if(mStep == TRAININGSTEP_ATTACKRADIA && mpRadaBoard)
        RenderMarkControl(mpRadaBoard->GetBoardVertex());
    else if(mStep == TRAININGSTEP_MAKETOWER && mpItemSlotBar)
    {
        //건물이 존재 하지 않을때만 깜빡이자.
        bool bIs = false;
        for(ByNode<CSprite>*it = sb; it != mpCurrRuntimeRenderObjects->plistSprite->end();)
        {
            CSprite* Object = it->GetObject();
            if(Object->GetState() == SPRITE_RUN && Object->GetModelID() == ACTORID_BLZ_DMISSILETOWER)
            {
                bIs = true;
                break;
            }
            it = it->GetNextNode();
        }
        if(bIs == false)
        {
            CControl *pBtn = mpItemSlotBar->FindControl(CTL_TABLE_ITEM_SLOT3);
            RenderMarkControl(pBtn->GetWorldVertex());
        }
    }
    else if(mStep == TRAININGSTEP_UPGRADE && mpClickedSprite && mbUpgrading == false)
    {
        CControl *pBtn = mpItemSlotBar->FindControl(CTL_TABLE_ITEM_SLOT3);
        RenderMarkControl(pBtn->GetWorldVertex());
    }
    
    //-----------------------------------------------------
    
    if(mpArrowBlz)
        mpArrowBlz->Render();
    
    if(mpRaidaFingerVertex)
    {
        glColor4f(1.0f,1.0f,1.0f,1.0f);
        glBindTexture(GL_TEXTURE_2D, mnFingerTexture);
        glTexCoordPointer(2, GL_FLOAT, 0, gPanelCoordTex);
        glVertexPointer(3, GL_FLOAT, 0, mRaidaFingerWorldVertex);
        glDrawElements(GL_TRIANGLE_STRIP, gshPanelIndicesCnt, GL_UNSIGNED_SHORT, gPanelIndices);
    }
    
    
    mpScoreNumCtl->Render();
    mpRightTools->Render();
    
    
    if(mpMessageDlg) mpMessageDlg->Render();
    if(mpStepMessageDlg) mpStepMessageDlg->Render();
    if(mpConfirmMsgCtl) mpConfirmMsgCtl->Render();
    if(mpPauseMsgCtl) mpPauseMsgCtl->Render();
    if(mpMissionMsgCtl) mpMissionMsgCtl->Render();
    
#ifdef MAC
    RenderDesign();
#endif
    
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    
    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    //-------------------------------
    
	return E_SUCCESS;
}

int CTrainingWorld::RenderEnd()
{
    //초기화가 안되었거나 초기화중이다.
	if( (mnStatus & 0x0002) != 0x0002 || mpCurrRuntimeRenderObjects == 0)
		return E_SUCCESS;
    
    if(mpStepMessageDlg)
    {
        mpStepMessageDlg->RenderEnd();
    }
    
    if(mpMessageDlg)
    {
        mpMessageDlg->RenderEnd();
        if(mpMessageDlg->Animating() == CONTROLANI_ZOOM_IN_HIDE_END)
        {
            delete mpMessageDlg;
            mpMessageDlg = NULL;
        }
    }
    
    unsigned long ggTime = GetGGTime();
    if(mnNextTime && ggTime > mnNextTime)
    {
        if (mStep == TRAININGSTEP_NONE || mStep == TRAININGSTEP_INTRODUCE)
            OnNextStep(mStep);
        else if(mStep == TRAININGSTEP_MOVE)
        {
            STARTPOINT* pMoveTargetPostion = m_pSGLCore->mlstStartPoint[INDEX_MOVEPOS];
            SPoint ptT;
            ptT.x = pMoveTargetPostion->fPosX;
            ptT.y = pMoveTargetPostion->fPosY;
            ptT.z = pMoveTargetPostion->fPosZ;
            ((CMoveAI*)GetActor()->GetAICore()->GetMoveAI())->CmdMoveTo(&ptT,true);
            mnNextTime = GetGGTime() + DEFAULT_MAX_TIMEOUT;
        }
        else if(mStep == TRAININGSTEP_ATTACKRADIA)
        {
            if(mEnamySprite && mEnamySprite->GetState() == SPRITE_RUN)
            {
                GetActor()->AimAutoToEnemy(mEnamySprite);
            }
        }
        else if(mStep == TRAININGSTEP_CHANGE_SLOT)
        {
            mpItemSlotBar->SetHide(true);
            if(mpRaidaFingerVertex)
            {
                delete[] mpRaidaFingerVertex;
                mpRaidaFingerVertex = NULL;
            }
            OnNextStep(mStep);
        }
        else if(mStep == TRAININGSTEP_MAKETOWER)
        {
            
            bool isBlz = false;
            CListNode<CSprite>* pList = GetSGLCore()->GetSpriteList();
            ByNode<CSprite>* b = pList->begin();
            for(ByNode<CSprite>* it = b; it != pList->end(); )
            {
                CSprite* sp = it->GetObject();
                if(sp->GetState() == SPRITE_RUN && sp->GetModelID() == ACTORID_BLZ_DMISSILETOWER)
                {
                    isBlz = true;
                    break;
                }
                it = it->GetNextNode();
            }

            mnNextTime = GetGGTime() + DEFAULT_MAX_TIMEOUT;
            
            if(isBlz == false)
            {
                if(mpArrowBlz)
                {
                    delete mpArrowBlz;
                    mpArrowBlz = NULL;
                }
                
                if(mpRaidaFingerVertex)
                {
                    delete[] mpRaidaFingerVertex;
                    mpRaidaFingerVertex = NULL;
                }
                
                SVector vDir;
                SPoint ptNow;
                
                PROPERTY_TANK property;
                //Tank2
                mEnamySprite2 = CSGLCore::MakeSprite(GetNewID(), GetActor()->GetTeamID(), ACTORID_BLZ_DMISSILETOWER,m_pSGLCore->GetTextureMan(),m_pSGLCore->GetAction(), this,NETWORK_MYSIDE);
                mEnamySprite2->LoadResoure(m_pSGLCore->GetModelMan());
                
                vDir.x = -1;
                vDir.y = 0;
                vDir.z = -1;
                sglNormalize(&vDir);
//                STARTPOINT* pPos = m_pSGLCore->mlstStartPoint[INDEX_ATTACKPOS2];
//                ptNow.x = pPos->fPosX;
//                ptNow.y = pPos->fPosY;
//                ptNow.z = pPos->fPosZ;
                
                GetActor()->GetPosition(&ptNow);
                SPoint ptPos;
                mEnamySprite2->Initialize(&ptNow, &vDir);
                SetMakePosition(15.f * 5.f,0,mEnamySprite2,&ptNow,&ptPos);
                
                
                
                
                m_pSGLCore->AddSprite(mEnamySprite2);
                SetRotationCarmeraToPoint(&ptPos);
                OnNextStep(mStep); //다음 스템으로 변경한다.
            }
            
        }
        else if(mStep == TRAININGSTEP_UPGRADE)
        {
            if(mEnamySprite2 && mEnamySprite2->GetState() == SPRITE_RUN)
            {
                vector<USERINFO*>* plist= mEnamySprite2->GetRuntimeUpgradList();
                for (int i = 0; i < plist->size(); i++)
                {
                    USERINFO *userinfo = plist->at(i);
                    if(userinfo->nID == RUNTIME_UPDATE_UPGRADE)
                    {
                        ((RUNTIME_UPGRADE*)userinfo->pProp)->nUpgradeCnt = 5;
                        mEnamySprite2->OnCompletedUpdated((RUNTIME_UPGRADE*)userinfo->pProp); //업그레이드가 많이 되어서 비행기를 한방에
                        break;
                    }
                }
            }
            if(mpRaidaFingerVertex)
            {
                delete[] mpRaidaFingerVertex;
                mpRaidaFingerVertex = NULL;
            }
            //-------------------------------------------
            // 슬롯을 제거한다.
            mpClickedSprite = NULL;
            ResetDisplayItemSlot();
            //-------------------------------------------
            OnNextStep(mStep);
        }
        else if(mStep == TRAININGSTEP_END0)
            OnNextStep(mStep);
        else if(mStep == TRAININGSTEP_END)
        {
            //프로그램을 종료해준다.
            CScenario::SendMessage(SGL_MESSAGE_SUCCESS_MISSION,0);
            mnNextTime = 0;
        }
        else
        {
            //시간을 상단에 나타나게 하여. 그시간에 안나오면 미션 종료해야한다.
        }
    }
    if(mStep == TRAININGSTEP_NONE)
    {
        //2초 후에 인트로 메세지를 보여준다.
        if(mnNextTime == 0)
            mnNextTime = ggTime + 700.f;
    }
    
	return CHWorld::RenderEnd();
}


void CTrainingWorld::Setp1_IntroduceDlg()
{
    if(mpMessageDlg) return;
    string sText;
    vector<float> layout;
    vector<string>  lstImage;
    layout.clear();
    float fYellow[] = { 0.0f,0.0f,0.0f,1.0f};
    
    PROPERTYI::GetPropertyFloatList("Trn_Introduce_Msg", layout);
    
    float fX = (gDisplayWidth - layout[2]) / 2.f + layout[0];
    float fY = (gDisplayHeight - layout[3]) / 2.f + layout[1];
    
    //CellBack
    lstImage.clear();
    lstImage.push_back("greenpanel.png");
    lstImage.push_back("none");
    mpMessageDlg =  new CLabelCtl(NULL,GetTextureMan());
    mpMessageDlg->Initialize(0, fX, fY, layout[2], layout[3], lstImage, 0.f, 0.f, 1.f, 1.f);
    
    
    const char* ArrT[] = {"Trn_Introduce_Msg_T1","Trn_Introduce_Msg_T2","Trn_Introduce_Msg_T3","Trn_Introduce_Msg_T4"};
    int nCnt = sizeof(ArrT) / sizeof(const char*);
    for (int i = 0; i < nCnt; i++)
    {
        
        layout.clear();
        lstImage.clear();
        
        PROPERTYI::GetPropertyFloatList(ArrT[i], layout);
        sText = SGLTextUnit::GetTextAddGLData(ArrT[i],"Trn_Introduce_Msg");
        lstImage.push_back("none");
        lstImage.push_back(sText);
        CLabelCtl *pText = new CLabelCtl(mpMessageDlg,GetTextureMan());
        pText->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.0, 0.0, 0.0, 0.0);
        pText->SetTextColor(fYellow);
        mpMessageDlg->AddControl(pText);
    }
    
    layout.clear();
    PROPERTYI::GetPropertyFloatList("Trn_Introduce_Msg_GoBtn", layout);
    lstImage.clear();
    lstImage.push_back("res_Dialog.png");
    lstImage.push_back("M_IB_YES");
    CButtonCtl* btOK =  new CButtonCtl(mpMessageDlg,GetTextureMan());
    btOK->Initialize(BTN_TRAINING_STEP1_OK, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.09375,0.3828125,0.33203125,0.45703125);
    mpMessageDlg->AddControl(btOK);
    
    
    mpMessageDlg->SetTranslate(0.8f);
    mpMessageDlg->SetAni(CONTROLANI_LIN,1.6f);
    
#if defined(IPHONE) || defined(ANDROID)
    mpScenario->SendTrainningStep(1);
#endif
}

void CTrainingWorld::Setp2_MoveDlg()
{
    //이동 관련 메세지를 보여준다.
    mStep = TRAININGSTEP_MOVE; //이동에 관련한 미션을 수행합니다.
    
    if(mpStepMessageDlg) return;
    
    vector<float> layout;
    vector<string>  lstImage;
    
    if(mpMessageDlg) mpMessageDlg->SetAni(CONTROLANI_ZOOM_IN_HIDE,0.2f);
    
    
    layout.clear();
    PROPERTYI::GetPropertyFloatList("Trn_Step_Msg", layout);
    float fX = (gDisplayWidth - layout[2]) / 2.f + layout[0];
    float fY = layout[1];
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    mpStepMessageDlg =  new CLabelCtl(NULL,GetTextureMan());
    mpStepMessageDlg->Initialize(0, fX, fY, layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
    
    
    float fRed[] = {1.0f,0.0f,0.0f,1.0f};
    float fYellow[] = {1.0f,1.0f,0.0f,1.0f};
    //Title
    layout.clear();
    PROPERTYI::GetPropertyFloatList("Trn_Step_Msg_Title", layout);
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("Tran_Step1_MoveT");
    CLabelCtl* pTitle =  new CLabelCtl(mpStepMessageDlg,GetTextureMan());
    pTitle->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
    pTitle->SetTextColor(fRed);
    mpStepMessageDlg->AddControl(pTitle);
    mpStepMessageTitle = pTitle;
    
    //Description
    layout.clear();
    PROPERTYI::GetPropertyFloatList("Trn_Step_Msg_Desc", layout);
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("Tran_Step1_MoveDes1");
    CLabelCtl* pDescription =  new CLabelCtl(mpStepMessageDlg,GetTextureMan());
    pDescription->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
    pDescription->SetTextColor(fYellow);
    mpStepMessageDlg->AddControl(pDescription);
    mpStepMessageDes = pDescription;
    
    
    float fHe = layout[3] + 4;
    
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    CLabelCtl* pDescription2 =  new CLabelCtl(mpStepMessageDlg,GetTextureMan());
    pDescription2->Initialize(0, fX + layout[0], fY + layout[1] + fHe, layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
    pDescription2->SetTextColor(fYellow);
    mpStepMessageDlg->AddControl(pDescription2);
    mpStepMessageDes2 = pDescription2;
    
    
    
    mpStepMessageDlg->SetTranslate(0.8f);
    mpStepMessageDlg->SetAni(CONTROLANI_TIN,0.8f);
    
    
    //    CSprite* pActor = GetActor();
    //Added Target Arrow
    mpMoveTargetArrowSprite = new CArrowMotionSprite(GetNewID(),SGL_TEAMALL,ACTORID_ARROWMOTION_TRAIN,(IAction*)this,this);
    mpMoveTargetArrowSprite->LoadResoure(m_pSGLCore->GetModelMan());
    mpMoveTargetArrowSprite->Initialize(NULL,NULL);
    STARTPOINT* pMoveTargetPostion = m_pSGLCore->mlstStartPoint[INDEX_MOVEPOS];
    SPoint ptT;
    ptT.x = pMoveTargetPostion->fPosX;
    ptT.y = pMoveTargetPostion->fPosY;
    ptT.z = pMoveTargetPostion->fPosZ;
    mpMoveTargetArrowSprite->SetPosition(&ptT);
    m_pSGLCore->AddSprite(mpMoveTargetArrowSprite);
    
    SetRotationCarmeraToPoint(&ptT);
#if defined(IPHONE) || defined(ANDROID)
    mpScenario->SendTrainningStep(2);
#endif
    
}

void CTrainingWorld::Setp3_Attack()
{
    mStep = TRAININGSTEP_ATTACK;
    SetStepMessage("Tran_Step2_AttackT","Tran_Step2_AttackDes1");
    
    //움직임을 멈춘다.
    GetActor()->SetMoveVelocity(0.0f);
    GetActor()->SetRotationVelocity(0.0f);
    
    //주인공은 트레이닝 모드를 풀어준다.
    GetActor()->GetAICore()->SetIsTrainningMode(false);
    
    SVector vDir;
    SPoint ptNow;
    
    //-------------------------------------------------------------------------------------------------------------------------------
    //아군을 만들어준다.
    CSprite* pSP1 = CSGLCore::MakeSprite(GetNewID(), GetActor()->GetTeamID(), ACTORID_MISSILE,m_pSGLCore->GetTextureMan(),m_pSGLCore->GetAction(), this,NETWORK_MYSIDE);
    pSP1->LoadResoure(m_pSGLCore->GetModelMan());
    vDir.x = -1;
    vDir.y = 0;
    vDir.z = -1;
    sglNormalize(&vDir);
    GetActor()->GetPosition(&ptNow);
    pSP1->Initialize(&ptNow, &vDir);
    float fRadias = 15.f * 2.f;
    SetMakePosition(fRadias,6,pSP1,&ptNow);
    m_pSGLCore->AddSprite(pSP1);
    
    pSP1 = CSGLCore::MakeSprite(GetNewID(), GetActor()->GetTeamID(), ACTORID_ARMEREDCAR,m_pSGLCore->GetTextureMan(),m_pSGLCore->GetAction(), this,NETWORK_MYSIDE);
    pSP1->LoadResoure(m_pSGLCore->GetModelMan());
    vDir.x = -1;
    vDir.y = 0;
    vDir.z = -1;
    sglNormalize(&vDir);
    GetActor()->GetPosition(&ptNow);
    pSP1->Initialize(&ptNow, &vDir);
    fRadias = 15.f * 2.f;
    SetMakePosition(fRadias,5,pSP1,&ptNow);
    m_pSGLCore->AddSprite(pSP1);
    //-------------------------------------------------------------------------------------------------------------------------------

    
    
    PROPERTY_TANK property;
    //Tank2
    mEnamySprite2 = CSGLCore::MakeSprite(GetNewID(), SGL_TEAM2, ACTORID_ET1,m_pSGLCore->GetTextureMan(),m_pSGLCore->GetAction(), this,NETWORK_MYSIDE);
    mEnamySprite2->LoadResoure(m_pSGLCore->GetModelMan());
    
    vDir.x = -1;
    vDir.y = 0;
    vDir.z = -1;
    sglNormalize(&vDir);
    
    STARTPOINT* pPos = m_pSGLCore->mlstStartPoint[INDEX_ATTACKPOS2];
    ptNow.x = pPos->fPosX;
    ptNow.y = pPos->fPosY;
    ptNow.z = pPos->fPosZ;
    
    mEnamySprite2->Initialize(&ptNow, &vDir);
//    mEnamySprite2->DeleteAICore();
//    mEnamySprite2->SetCurrentDefendPower(0.f);
    m_pSGLCore->AddSprite(mEnamySprite2);
    
    
    //Tank1
    mEnamySprite = CSGLCore::MakeSprite(GetNewID(), SGL_TEAM2, ACTORID_ET3,m_pSGLCore->GetTextureMan(),m_pSGLCore->GetAction(), this,NETWORK_MYSIDE);
    mEnamySprite->LoadResoure(m_pSGLCore->GetModelMan());
    
    vDir.x = 1;
    vDir.y = 0;
    vDir.z = 1;
    sglNormalize(&vDir);
    
    //    pPos = m_pSGLCore->mlstStartPoint[INDEX_ATTACKPOS]; 사용하지 않고 INDEX_ATTACKPOS2 을 기준으로 그린다.
    ptNow.x = pPos->fPosX  - 80.f;
    ptNow.y = pPos->fPosY;
    ptNow.z = pPos->fPosZ - 80.f;
    GetPositionY(&ptNow);
    
    mEnamySprite->Initialize(&ptNow, &vDir);
//    mEnamySprite->SetCurrentDefendPower(0.f);
//    mEnamySprite->DeleteAICore();
    m_pSGLCore->AddSprite(mEnamySprite);
    
//    
//    //Tank2
//    CSprite* pOtherSprite = CSGLCore::MakeSprite(GetNewID(), SGL_TEAM2, ACTORID_ET1,m_pSGLCore->GetTextureMan(),m_pSGLCore->GetAction(), this);
//    pOtherSprite->LoadResoure(m_pSGLCore->GetModelMan());
//    
//    vDir.x = 1;
//    vDir.y = 0;
//    vDir.z = 1;
//    sglNormalize(&vDir);
//    
//    //    pPos = m_pSGLCore->mlstStartPoint[INDEX_ATTACKPOS]; 사용하지 않고 INDEX_ATTACKPOS2 을 기준으로 그린다.
//    ptNow.x = pPos->fPosX + 12.f;
//    ptNow.y = pPos->fPosY;
//    ptNow.z = pPos->fPosZ + 30.f;
//    GetPositionY(&ptNow);
//    
//    pOtherSprite->Initialize(&ptNow, &vDir);
//    //    mEnamySprite->SetCurrentDefendPower(0.f);
//    //    mEnamySprite->DeleteAICore();
//    m_pSGLCore->AddSprite(pOtherSprite);

    SetRotationCarmeraToPoint(&ptNow);
#if defined(IPHONE) || defined(ANDROID)
    mpScenario->SendTrainningStep(3);
#endif
}


void CTrainingWorld::Setp5_AttackRadia()
{
    mEnamySprite = NULL;
    mEnamySprite2 = NULL;
    mStep = TRAININGSTEP_ATTACKRADIA;
    
    //주인공은 트레이닝 모드로 돌려준다.
    GetActor()->GetAICore()->SetIsTrainningMode(true);
    
    SetStepMessage(NULL,"Tran_Step2_AttackDes2");
    
    SVector vDir;    SPoint ptNow;
    PROPERTY_TANK property;
    
    mEnamySprite = CSGLCore::MakeSprite(GetNewID(), SGL_TEAM2, ACTORID_ET1,m_pSGLCore->GetTextureMan(),m_pSGLCore->GetAction(), this,NETWORK_MYSIDE);
    mEnamySprite->LoadResoure(m_pSGLCore->GetModelMan());
    
    
    vDir.x = -1;
    vDir.y = 0;
    vDir.z = -1;
    sglNormalize(&vDir);
    
    STARTPOINT* pPos = m_pSGLCore->mlstStartPoint[INDEX_ATTACKPOS3];
    ptNow.x = pPos->fPosX;
    ptNow.y = pPos->fPosY;
    ptNow.z = pPos->fPosZ;
    
    mEnamySprite->Initialize(&ptNow, &vDir);
    mEnamySprite->DeleteAICore();
    mEnamySprite->SetCurrentDefendPower(0.f);
    m_pSGLCore->AddSprite(mEnamySprite);
    
    mpRadaBoard->SetHide(false);
    
    SetRotationCarmeraToPoint(&ptNow);
#if defined(IPHONE) || defined(ANDROID)
    mpScenario->SendTrainningStep(4);
#endif
    
}

void CTrainingWorld::Setp5_1_ChangeSlot()
{
    mStep = TRAININGSTEP_CHANGE_SLOT;
    SetStepMessage("Tran_Step2_ChSlotT","Tran_Step2_ChSlotTDes1");
    
    //움직임을 멈춘다.
    GetActor()->SetMoveVelocity(0.0f);
    GetActor()->SetRotationVelocity(0.0f);
    
    GetUserInfo()->SetLastBombItemSwitch(2);
    
    //슬롯을 보여준다.
    ResetDisplayItemSlot();
    
    CControl* ctl = mpItemSlotSwitchBtn;
    ctl->SetAni(CONTROLANI_WIGWAG,0.3f);
    
    
    float fPX,fPY,szW,szH;
    ctl->GetPosition(fPX, fPY);
    ctl->GetSize(szW, szH);
    szH *= 0.5f;
    vector<string> lstImage;
    lstImage.push_back("none");
    lstImage.push_back("none");
    CLabelCtl tmpCtl(NULL,GetTextureMan());
    tmpCtl.Initialize(0, fPX + szW/2.f, fPY - szH/2.f, szH, szH*2.f, lstImage, 0.f, 0.f, 0.f, 0.f);
    
    if(mpRaidaFingerVertex == NULL)
        mpRaidaFingerVertex = new float[12];
    memcpy(mpRaidaFingerVertex, tmpCtl.GetWorldVertex(), sizeof(float) * 12);
    float fY = (tmpCtl.GetWorldVertex()[1] - tmpCtl.GetWorldVertex()[7]) / 2.f; //상단으로 옮겨준다.
    mfRaidaFingerMax = fY * 2.f;
    mfRaidaFingerMin = fY;
    
#if defined(IPHONE) || defined(ANDROID)
    mpScenario->SendTrainningStep(5);
#endif
}






void CTrainingWorld::Setp6_MakeTower()
{
    mStep = TRAININGSTEP_MAKETOWER;
    mEnamySprite = NULL;
    mEnamySprite2 = NULL;
    
    //메세지 정의해야 한다.
    SetStepMessage("Tran_Step6_MakeTowerT","Tran_Step6_MakeTowerDes1");
    SPoint ptNow;
    PROPERTY_TANK property;
    
    STARTPOINT* pPos = m_pSGLCore->mlstStartPoint[INDEX_ATTACKPOS2];
    ptNow.x = pPos->fPosX;
    ptNow.y = pPos->fPosY;
    ptNow.z = pPos->fPosZ;
    SetRotationCarmeraToPoint(&ptNow);
    
    ResetDisplayItemSlot();
    
    CControl* ctl = mpItemSlotBar->FindControl(CTL_TABLE_ITEM_SLOT3);
    ctl->SetAni(CONTROLANI_WIGWAG,0.3f);
    
    CUserInfo* pUserInfo = mpScenario->GetUserInfo();
    pUserInfo->SetLastBombItemSwitch(2);
    
    
    
    //손가락
    float fPX,fPY,szW,szH;
    ctl->GetPosition(fPX, fPY);
    ctl->GetSize(szW, szH);
    szH *= 0.5f;
    vector<string> lstImage;
    lstImage.push_back("none");
    lstImage.push_back("none");
    CLabelCtl tmpCtl(NULL,GetTextureMan());
    tmpCtl.Initialize(0, fPX + szW/2.f, fPY - szH/2.f, szH, szH*2.f, lstImage, 0.f, 0.f, 0.f, 0.f);
    
    if(mpRaidaFingerVertex == NULL)
        mpRaidaFingerVertex = new float[12];
    memcpy(mpRaidaFingerVertex, tmpCtl.GetWorldVertex(), sizeof(float) * 12);
    float fY = (tmpCtl.GetWorldVertex()[1] - tmpCtl.GetWorldVertex()[7]) / 2.f; //상단으로 옮겨준다.
    
    
    
    //방향
    lstImage.clear();
    if(gnDisplayType == DISPLAY_IPHONE)
        lstImage.push_back("TrainingArrow1.tga");
    else
        lstImage.push_back("TrainingArrow.tga");
    lstImage.push_back("none");
    
    vector<float> layout;
    PROPERTYI::GetPropertyFloatList("Trn_ArrowBlz", layout);
    mpArrowBlz = new CLabelCtl(mpLeftTools,GetTextureMan());
    szW = layout[2];
    szH = layout[3];
    
    if(gnDisplayType == DISPLAY_IPAD)
    {
        mfRaidaFingerMax = fY * 11.f;
        mfRaidaFingerMin = fY;
        mfFingerUpDwonActionDirection = 1.f;
    }
    else
    {
        mfRaidaFingerMax = fY;
        mfRaidaFingerMin = fY - szW;
        mfFingerUpDwonActionDirection = -1.f;
    }
    
    if(gnDisplayType == DISPLAY_IPHONE)
        mpArrowBlz->Initialize(0, fPX - szW/2.f, fPY + szH/2.f, szW, szH, lstImage, 0.f, 0.f, 1.f, 1.f);
    else
        mpArrowBlz->Initialize(0, fPX + szW/2.f, fPY - (szH * 6)/2.f - szH * 2, szW, szH * 6, lstImage, 0.f, 0.f, 1.f, 1.f);
    mpArrowBlz->SetEnable(false);
    
    
    SVector vDir;
    //Tank2
    mEnamySprite = CSGLCore::MakeSprite(GetNewID(), SGL_TEAM2, ACTORID_EF1,m_pSGLCore->GetTextureMan(),m_pSGLCore->GetAction(), this,NETWORK_MYSIDE);
    mEnamySprite->LoadResoure(m_pSGLCore->GetModelMan());
    
    vDir.x = -1;
    vDir.y = 0;
    vDir.z = -1;
    sglNormalize(&vDir);
    
    pPos = m_pSGLCore->mlstStartPoint[INDEX_ATTACKPOS2];
    ptNow.x = pPos->fPosX;
    ptNow.y = pPos->fPosY;
    ptNow.z = pPos->fPosZ;
    
    mEnamySprite->Initialize(&ptNow, &vDir);
    //    mEnamySprite->DeleteAICore();
    //    mEnamySprite->SetCurrentDefendPower(0.f);
    m_pSGLCore->AddSprite(mEnamySprite);
    
    
#if defined(IPHONE) || defined(ANDROID)
    mpScenario->SendTrainningStep(6);
#endif
}

void CTrainingWorld::Setp6_Upgrade()
{
    mStep = TRAININGSTEP_UPGRADE;
    SetStepMessage("Tran_Step6_Upgrade","Tran_Step6_UpgradeDes1");
#if defined(IPHONE) || defined(ANDROID)
    
    
    ResetDisplayItemSlot();
    mpScenario->SendTrainningStep(7);
#endif
}

void CTrainingWorld::Setp7_Mission()
{
    mStep = TRAININGSTEP_MISSION;
    mEnamySprite = NULL;
    mEnamySprite2= NULL;
    
    SetStepMessage("Tran_Step3_MissionT","Tran_Step3_MissionDes1");
    SVector vDir;
    SPoint ptNow;
    PROPERTY_TANK property;
    
    mEnamySprite = CSGLCore::MakeSprite(GetNewID(), SGL_TEAM2, ACTORID_BLZ_EMAIN,m_pSGLCore->GetTextureMan(),m_pSGLCore->GetAction(), this,NETWORK_MYSIDE);
    mEnamySprite->LoadResoure(m_pSGLCore->GetModelMan());
    //공격하게 해준다.
    GetActor()->GetAICore()->SetIsTrainningMode(false);
    
    vDir.x = -1;
    vDir.y = 0;
    vDir.z = -1;
    sglNormalize(&vDir);
    
    STARTPOINT* pPos = m_pSGLCore->mlstStartPoint[INDEX_ATTACKPOS4];
    ptNow.x = pPos->fPosX;
    ptNow.y = pPos->fPosY;
    ptNow.z = pPos->fPosZ;
    
    mEnamySprite->Initialize(&ptNow, &vDir);
    mEnamySprite->DeleteAICore();
    m_pSGLCore->AddSprite(mEnamySprite);
    
    SetRotationCarmeraToPoint(&ptNow);
    
    
#if defined(IPHONE) || defined(ANDROID)
    mpScenario->SendTrainningStep(8);
#endif
    
    
}





void CTrainingWorld::Setp8_End0()
{
    mStep = TRAININGSTEP_END0;
    mEnamySprite = NULL;
    mEnamySprite2= NULL;
    
    SetStepMessage("Tran_Step3_END","Tran_Step3_ENDDes1");
    SVector vDir;
    SPoint ptNow;
    PROPERTY_TANK property;
    
    
    STARTPOINT* pPos = NULL;
    float fRadias = 15.f * 2.f;
    
    //가드타워를 만든다.
    pPos = m_pSGLCore->mlstStartPoint[INDEX_ATTACKPOS];
    ptNow.x = pPos->fPosX;
    ptNow.y = pPos->fPosY;
    ptNow.z = pPos->fPosZ;
    
    SetRotationCarmeraToPoint(&ptNow);
    
    //아군 메인타워를 건설한다.
    mEnamySprite = CSGLCore::MakeSprite(GetNewID(), SGL_TEAM1, ACTORID_BLZ_MAINTOWER,m_pSGLCore->GetTextureMan(),m_pSGLCore->GetAction(), this,NETWORK_MYSIDE);
    mEnamySprite->LoadResoure(m_pSGLCore->GetModelMan());
    mEnamySprite->Initialize(&ptNow, &vDir);
    m_pSGLCore->AddSprite(mEnamySprite);
    
    
    mEnamySprite = CSGLCore::MakeSprite(GetNewID(), SGL_TEAM1, ACTORID_BLZ_DGUNTOWER,m_pSGLCore->GetTextureMan(),m_pSGLCore->GetAction(), this,NETWORK_MYSIDE);
    mEnamySprite->LoadResoure(m_pSGLCore->GetModelMan());
    mEnamySprite->Initialize(&ptNow, &vDir);
    SetMakePosition(fRadias,0,mEnamySprite,&ptNow);
    m_pSGLCore->AddSprite(mEnamySprite);
    
    mEnamySprite = CSGLCore::MakeSprite(GetNewID(), SGL_TEAM1, ACTORID_BLZ_DMISSILETOWER,m_pSGLCore->GetTextureMan(),m_pSGLCore->GetAction(), this,NETWORK_MYSIDE);
    mEnamySprite->LoadResoure(m_pSGLCore->GetModelMan());
    mEnamySprite->Initialize(&ptNow, &vDir);
    SetMakePosition(fRadias,2,mEnamySprite,&ptNow);
    m_pSGLCore->AddSprite(mEnamySprite);
    
    
    mEnamySprite = CSGLCore::MakeSprite(GetNewID(), SGL_TEAM1, ACTORID_BLZ_DMISSILETOWER,m_pSGLCore->GetTextureMan(),m_pSGLCore->GetAction(), this,NETWORK_MYSIDE);
    mEnamySprite->LoadResoure(m_pSGLCore->GetModelMan());
    mEnamySprite->Initialize(&ptNow, &vDir);
    SetMakePosition(fRadias,3,mEnamySprite,&ptNow);
    m_pSGLCore->AddSprite(mEnamySprite);
    
    mEnamySprite = CSGLCore::MakeSprite(GetNewID(), SGL_TEAM1, ACTORID_BLZ_DTOWER,m_pSGLCore->GetTextureMan(),m_pSGLCore->GetAction(), this,NETWORK_MYSIDE);
    mEnamySprite->LoadResoure(m_pSGLCore->GetModelMan());
    mEnamySprite->Initialize(&ptNow, &vDir);
    SetMakePosition(fRadias,4,mEnamySprite,&ptNow);
    m_pSGLCore->AddSprite(mEnamySprite);
    
    GetActor()->GetPosition(&ptNow);
    mEnamySprite = CSGLCore::MakeSprite(GetNewID(), SGL_TEAM1, ACTORID_BLZ_DMISSILETOWER,m_pSGLCore->GetTextureMan(),m_pSGLCore->GetAction(), this,NETWORK_MYSIDE);
    mEnamySprite->LoadResoure(m_pSGLCore->GetModelMan());
    mEnamySprite->Initialize(&ptNow, &vDir);
    SetMakePosition(fRadias,5,mEnamySprite,&ptNow);
    m_pSGLCore->AddSprite(mEnamySprite);
    //
    //    mEnamySprite = CSGLCore::MakeSprite(GetNewID(), SGL_TEAM1, ACTORID_BLZ_DGUNTOWER,m_pSGLCore->GetTextureMan(),m_pSGLCore->GetAction(), this);
    //    mEnamySprite->LoadResoure(m_pSGLCore->GetModelMan());
    //    mEnamySprite->Initialize(&ptNow, &vDir);
    //    SetMakePosition(fRadias,6,mEnamySprite,&ptNow);
    //    m_pSGLCore->AddSprite(mEnamySprite);
    //
    //    //아군 메인타워를 건설한다.
    //    mEnamySprite = CSGLCore::MakeSprite(GetNewID(), SGL_TEAM1, ACTORID_BLZ_MAINTOWER,m_pSGLCore->GetTextureMan(),m_pSGLCore->GetAction(), this);
    //    mEnamySprite->LoadResoure(m_pSGLCore->GetModelMan());
    //    mEnamySprite->Initialize(&ptNow, &vDir);
    //    SetMakePosition(fRadias,7,mEnamySprite,&ptNow);
    //    m_pSGLCore->AddSprite(mEnamySprite);
    
    mnNextTime = GetGGTime() + 1000 * 4; //5초후에 적을 만든다.
}


void CTrainingWorld::Setp8_End()
{
    mStep = TRAININGSTEP_END;
    mEnamySprite = NULL;
    mEnamySprite2= NULL;
    
    SetStepMessage("Tran_Step3_END","Tran_Step3_ENDDes1");
    SVector vDir;
    SPoint ptNow;
    PROPERTY_TANK property;
    
    
    //------------------------------------------------------------------------------------------------------------------------------------
    //Main Tower를 만든다.
    mEnamySprite = CSGLCore::MakeSprite(GetNewID(), SGL_TEAM2, ACTORID_BLZ_EMAIN,m_pSGLCore->GetTextureMan(),m_pSGLCore->GetAction(), this,NETWORK_MYSIDE);
    mEnamySprite->LoadResoure(m_pSGLCore->GetModelMan());
    
    vDir.x = -1;
    vDir.y = 0;
    vDir.z = -1;
    sglNormalize(&vDir);
    
    STARTPOINT* pPos = m_pSGLCore->mlstStartPoint[INDEX_ATTACKPOS4];
    ptNow.x = pPos->fPosX;
    ptNow.y = pPos->fPosY;
    ptNow.z = pPos->fPosZ;
    SetRotationCarmeraToPoint(&ptNow);
    
    mEnamySprite->Initialize(&ptNow, &vDir);
    mEnamySprite->DeleteAICore();
    m_pSGLCore->AddSprite(mEnamySprite);
    SetRotationCarmeraToPoint(&ptNow);
    //------------------------------------------------------------------------------------------------------------------------------------
    
    float fRadias = mEnamySprite->GetRadius() * 2.f;
    //------------------------------------------------------------------------------------------------------------------------------------
    //메인타워 주위에 비행기,탱크,레이져등등을 배치해준다.
    //------------------------------------------------------------------------------------------------------------------------------------
    //비행기를 만든다.
    mEnamySprite = CSGLCore::MakeSprite(GetNewID(), SGL_TEAM2, ACTORID_EF1,m_pSGLCore->GetTextureMan(),m_pSGLCore->GetAction(), this,NETWORK_MYSIDE);
    mEnamySprite->LoadResoure(m_pSGLCore->GetModelMan());
    mEnamySprite->Initialize(&ptNow, &vDir);
    SetMakePosition(fRadias * 3.f,0,mEnamySprite,&ptNow);
    m_pSGLCore->AddSprite(mEnamySprite);
    
    
    //비행기를 만든다.
    mEnamySprite = CSGLCore::MakeSprite(GetNewID(), SGL_TEAM2, ACTORID_EF2,m_pSGLCore->GetTextureMan(),m_pSGLCore->GetAction(), this,NETWORK_MYSIDE);
    mEnamySprite->LoadResoure(m_pSGLCore->GetModelMan());
    mEnamySprite->Initialize(&ptNow, &vDir);
    SetMakePosition(fRadias * 2.f,1,mEnamySprite,&ptNow);
    m_pSGLCore->AddSprite(mEnamySprite);
    
    
    mEnamySprite = CSGLCore::MakeSprite(GetNewID(), SGL_TEAM2, ACTORID_ET2,m_pSGLCore->GetTextureMan(),m_pSGLCore->GetAction(), this,NETWORK_MYSIDE);
    mEnamySprite->LoadResoure(m_pSGLCore->GetModelMan());
    mEnamySprite->Initialize(&ptNow, &vDir);
    SetMakePosition(fRadias * 5.f,3,mEnamySprite,&ptNow);
    m_pSGLCore->AddSprite(mEnamySprite);
    
    mEnamySprite = CSGLCore::MakeSprite(GetNewID(), SGL_TEAM2, ACTORID_ET5,m_pSGLCore->GetTextureMan(),m_pSGLCore->GetAction(), this,NETWORK_MYSIDE);
    mEnamySprite->LoadResoure(m_pSGLCore->GetModelMan());
    mEnamySprite->Initialize(&ptNow, &vDir);
    SetMakePosition(fRadias * 2.f,5,mEnamySprite,&ptNow);
    m_pSGLCore->AddSprite(mEnamySprite);
    
    //8
    mEnamySprite = CSGLCore::MakeSprite(GetNewID(), SGL_TEAM2, ACTORID_ET7,m_pSGLCore->GetTextureMan(),m_pSGLCore->GetAction(), this,NETWORK_MYSIDE);
    mEnamySprite->LoadResoure(m_pSGLCore->GetModelMan());
    mEnamySprite->Initialize(&ptNow, &vDir);
    SetMakePosition(fRadias,6,mEnamySprite,&ptNow);
    m_pSGLCore->AddSprite(mEnamySprite);
    
    
#if defined(IPHONE) || defined(ANDROID)
    mpScenario->SendTrainningStep(9);
#endif
    
    mnNextTime = GetGGTime() + 1000 * 6; //5초후에 종료한다.
}

void CTrainingWorld::SetBlzSlotCount(int nBlzID, bool bAdd,CSprite* pDeletedSprite)
{
    int nCnt = 0;
    CUserInfo* pUserInfo = GetUserInfo();
    
    vector<USERINFO*>* arrTank = pUserInfo->GetListBlz();
    for (vector<USERINFO*>::iterator it = arrTank->begin(); it != arrTank->end(); it++)
    {
        USERINFO* info = *it;
        if(info->nID == nBlzID)
        {
            if(bAdd)
                nCnt = ++info->nCnt;
            else
                nCnt = --info->nCnt;
            break;
        }
    }
    
    if(mpClickedSprite == NULL)
    {
        if(pUserInfo->GetLastBombItemSwitch() != 2) return ;
        CList<CControl*>*pList = mpItemSlotBar->GetChilds();
        for (int i = 0; i < pList->size(); i++)
        {
            CControl* control = pList->get(i);
            
            if ((long)control->GetLData() == -1) continue; //빈슬롯이면.... 건너띤다.
            
            USERINFO* info = (USERINFO*)control->GetLData();
            if(info && info->nID == nBlzID)
            {
                CNumberCtl* pNum = (CNumberCtl*) control->GetChild(0)->GetChild(0);
                pNum->SetNumber(nCnt);
                break;
            }
        }
    }
    else if(bAdd && mpClickedSprite == pDeletedSprite)
    {
        SetClickedSprite(NULL);
    }
}

void CTrainingWorld::SetStepMessage(const char *sTitle,const char *sDesc)
{
    vector<string>  lstImage;
    
    if(sTitle && mpStepMessageTitle)
    {
        lstImage.clear();
        lstImage.push_back("none");
        lstImage.push_back(SGLTextUnit::GetTextAddGLData(sTitle,"Tran_Step1_MoveT"));
        
        mpStepMessageTitle->SetImageData(lstImage);
    }
    
    if(sDesc && mpStepMessageDes)
    {
        string sDescX = SGLTextUnit::GetOnlyText(sDesc);
        
        char* fnd = (char*)strstr(sDescX.c_str(), "^");
        if(fnd)
        {
            *fnd = 0;
            lstImage.clear();
            lstImage.push_back("none");
            //Trn_Introduce_Msg
            lstImage.push_back(SGLTextUnit::GetTextAddGLData(fnd+1,"Trn_Introduce_Msg_Step"));
            mpStepMessageDes2->SetImageData(lstImage);
            mpStepMessageDes2->SetHide(false);
            
        }
        else
            mpStepMessageDes2->SetHide(true );
        
        lstImage.clear();
        lstImage.push_back("none");
        //Trn_Introduce_Msg
        lstImage.push_back(SGLTextUnit::GetTextAddGLData(sDescX.c_str(),"Trn_Introduce_Msg_Step"));
        mpStepMessageDes->SetImageData(lstImage);
        
        
        
    }
    
}

int CTrainingWorld::OnEvent(SGLEvent *pEvent)
{
    if(pEvent->nMsgID == SGL_MESSAGE_CLICKED)
    {
        if(pEvent->lParam == BTN_TRAINING_STEP1_OK)
        {
            //이동 훈련을 합니다.
            Setp2_MoveDlg();
        }
        else if(pEvent->lParam == BTN_PLAY_CONFIRM_YES)
        {
            if(mpConfirmMsgCtl)
            {
                delete mpConfirmMsgCtl;
                mpConfirmMsgCtl = NULL;
            }
            CScenario::SendMessage(SGL_MESSAGE_SUCCESS_MISSION,0);
//            //            mpConfirmMsgCtl->Hide();
//            mfScore = 0;
//            mnBonus = 0;
//            ShowMissionEndMessage(MissionEndCtlType_GoToMenu_Successed,mfScore,mnBonus,true);
//            SetPauseTime(true); //시간을 멈춘다.
//            if(GetUserInfo()->GetRank() == 0)
//            {
//                GetUserInfo()->SetPromotionSloder(true);
//            }
////            ShowMissionEndMessage(MissionEndCtlType_GoToMenu_Died,mfScore,mnBonus,true);
//            GetUserInfo()->GetListBlz()->clear();
//            GetUserInfo()->GetListBomb()->clear();
            return E_SUCCESS;
        }
        else if(pEvent->lParam == BTN_ITEMSLOTSWITCH)
        {
            CUserInfo* pUserInfo = mpScenario->GetUserInfo();
            
            int nType = pUserInfo->GetLastBombItemSwitch();
            if(nType == 0)
            {
                pUserInfo->SetLastBombItemSwitch(1);
                SetStepMessage(NULL,"Tran_Step2_ChSlotTDes3");
            }
            else if(nType == 1)
            {
                pUserInfo->SetLastBombItemSwitch(2);
            }
            else if(nType == 2)
            {
                pUserInfo->SetLastBombItemSwitch(0);
                SetStepMessage(NULL,"Tran_Step2_ChSlotTDes2");
            }
            
            ResetDisplayItemSlot();
            
            nType = pUserInfo->GetLastBombItemSwitch();
            if(nType == 2)
            {
                mpItemSlotBar->SetHide(true);
                if(mpRaidaFingerVertex)
                {
                    delete[] mpRaidaFingerVertex;
                    mpRaidaFingerVertex = NULL;
                }
                OnNextStep(mStep);
            }
            return E_SUCCESS;
        }
        else if(pEvent->lParam == BTN_PLAY_GoTo_Menu) //추가해주었다. 종료하면 무조건 훈련소 모드를 통과한것을 ..
        {
            CUserInfo* pUserInfo = mpScenario->GetUserInfo();
            if(!pUserInfo->IsCompletedMap(0))
                pUserInfo->AddListToCompletedMap(0);
            
//            return E_SUCCESS;
        }
        else if(pEvent->lParam == CTL_TABLE_ITEM_SLOT3 ||
                pEvent->lParam == CTL_TABLE_ITEM_SLOT4 ||
                pEvent->lParam == CTL_TABLE_ITEM_SLOT5 ||
                pEvent->lParam ==CTL_TABLE_ITEM_SLOT6 )
        {
            CButtonCtl* pCtl = (CButtonCtl*)pEvent->lObject;
            USERINFO* userinfo = (USERINFO*)pCtl->GetLData();
            if(pCtl->GetLData() == -1) return E_SUCCESS;
            if(userinfo->nID == RUNTIME_UPDATE_REMOVE)
            {
                if(mpClickedSprite && mpClickedSprite->GetState() == SPRITE_RUN)
                {
                    mpClickedSprite->SetState(BOMB_COMPACT_ANI);
                    SetBlzSlotCount(mpClickedSprite->GetModelID(),true,mpClickedSprite);
                }
                
                RUNTIME_UPGRADE prop;
                RUNTIME_UPGRADE::GetPropertyRunUpgradeItem(userinfo->nID , prop);
                OnSelectedObjectShowDescBar(prop.sDesc);
                
            }
            else if(userinfo->nID > RUNTIME_UPDATE_START &&
                    userinfo->nID < RUNTIME_UPDATE_END )
            {
                if(mpClickedSprite && mpClickedSprite->GetState() == SPRITE_RUN)
                {
                    RUNTIME_UPGRADE *prop = 0;
                    vector<USERINFO*>* pList = mpClickedSprite->GetRuntimeUpgradList();
                    for (vector<USERINFO*>::iterator it = pList->begin(); it != pList->end(); it++)
                    {
                        if((*it)->nID == userinfo->nID)
                        {
                            prop = (RUNTIME_UPGRADE*)(*it)->pProp;
                            prop->nUpgreadTime = 0;
                            prop->nTime = 2000;
                            mpClickedSprite->GetRuntimeUpgradeingList()->push_back(*it);
                            pCtl->SetEnable(false); //다시 클릭되지 않게 막아준다.
                            pCtl->GetChild(1)->SetHide(false); //
                            mbUpgrading = true;
                            break;
                        }
                    }
                }
                SetUpgradeDescSlot(userinfo->nID);
            }
            else
            {
                CUserInfo* pUserInfo = mpScenario->GetUserInfo();
                int nType = pUserInfo->GetLastBombItemSwitch();
                if(nType == 0) //0:Bomb 1:Item 2:Blz
                {
                    mpSelectedWeaponBtn = pCtl;
                    pCtl->ArrangeToggleButton();
                    ChangeBombDisplay(userinfo);
                    SetBombDescSlot(userinfo->nID);
                }
                else if(nType == 1)
                    SetItemDescSlot(userinfo->nID);
                else if(nType == 2)
                    SetSpriteDescSlot(userinfo->nID);
            }
            return E_SUCCESS;
        }
    }
    else if(pEvent->nMsgID == SGL_COMPLETED_RUNTIME_UPGRADE)
    {
        CSprite* pUpSp = (CSprite*)pEvent->lParam;
        if(pUpSp && pUpSp->GetState() == SPRITE_RUN)
        {
            ((RUNTIME_UPGRADE*)pEvent->lObject)->nUpgradeCnt = 5; //업그레이드가 많이 되어서 비행기를 한방에
            pUpSp->OnCompletedUpdated((RUNTIME_UPGRADE*)pEvent->lObject);
        }
        //-------------------------------------------
        // 슬롯을 제거한다.
        mpClickedSprite = NULL;
        ResetDisplayItemSlot();
        //-------------------------------------------
        OnNextStep(mStep); //다음 스텝으로 변경한다.
        return E_SUCCESS;
    }
    else if(pEvent->nMsgID == SGL_TRAINING_NEXT_END)
    {
        OnNextStep(mStep);
    }
    else if(pEvent->nMsgID == SGL_MESSAGE_SUCCESS_MISSION)
    {
        //성공했을때 보너스를 추가해주지 말자.
        CUserInfo* pUserInfo = 0;
        pUserInfo = mpScenario->GetUserInfo();
        
        
        if(!pUserInfo->IsCompletedMap(pUserInfo->GetLastSelectedMapID()))
            mnBonus = 4000;
        else
            mnBonus = 0;
        if(pUserInfo->GetRank() == 0)
            ShowMissionEndMessage(MissionEndCtlType_GoToMenu_Successed,mfScore,mnBonus,true);
        else
        {
            mnBonus = 0;
            mfScore = 0;
            ShowMissionEndMessage(MissionEndCtlType_GoToMenu_Successed,mfScore,mnBonus,true);
        }
        
        SetPauseTime(true); //시간을 멈춘다.
    
        //현재 훈련병일때 이등병으로 진급해준다.
        if(pUserInfo->GetRank() == 0)
        {
            GetUserInfo()->GetListBomb()->clear();
            GetUserInfo()->GetListBlz()->clear();
            pUserInfo->SetPromotionSloder(true);
            
            // 발사속도가 빠른
            pUserInfo->AddListToBomb(5,50,0);
            // 4분산탄 100개 2분산탄 2개
            pUserInfo->AddListToBomb(8,200,0);
        }
        
        return E_SUCCESS;
    }
    else if(pEvent->nMsgID == SGL_MAKE_BLZ)
    {
        float* fpos = (float*)pEvent->lObject;
        SVector vDir;
        vDir.x = -1;
        vDir.y = 0;
        vDir.z = -1;
        sglNormalize(&vDir);
        CSprite* pBlz = CSGLCore::MakeSprite(GetNewID(), pEvent->lParam2, (int)pEvent->lParam, GetTextureMan(),m_pSGLCore->GetAction(), this,NETWORK_MYSIDE);
        pBlz->LoadResoure(GetModelMan());
        SPoint ptNow;
        ptNow.x = fpos[0];
        ptNow.y = fpos[1];
        ptNow.z = fpos[2];
        pBlz->Initialize(&ptNow, &vDir);
        m_pSGLCore->AddSprite(pBlz);
        if(fpos) delete[] fpos;
        SetBlzSlotCount((int)pEvent->lParam, false, NULL); //--카운터해준다.
        
        return E_SUCCESS;
    }
    
    return CHWorld::OnEvent(pEvent);
}

void CTrainingWorld::BeginTouch(long lTouchID,float x, float y)
{
    //카메라가 회전할때 터치를 못하게 막는다.
    if(mpCarmeraRotationTartketPoint) return;
    
    if((mpConfirmMsgCtl == NULL && mpMissionMsgCtl == NULL && mpPauseMsgCtl == NULL) &&
       mpMessageDlg && mpMessageDlg->BeginTouch(lTouchID, x, y) == false) return;
#ifdef MAC
    if(mpStepMessageDlg) mpStepMessageDlg->BeginTouch(lTouchID, x, y);
#endif
    CHWorld::BeginTouch(lTouchID, x, y);
}

void CTrainingWorld::MoveTouch(long lTouchID,float x, float y)
{
    if((mpConfirmMsgCtl == NULL && mpMissionMsgCtl == NULL && mpPauseMsgCtl == NULL) &&
       mpMessageDlg && mpMessageDlg->MoveTouch(lTouchID, x, y) == false) return;
#ifdef MAC
    if(mpStepMessageDlg) mpStepMessageDlg->MoveTouch(lTouchID, x, y);
#endif
    CHWorld::MoveTouch(lTouchID, x, y);
    
}

void CTrainingWorld::EndTouch(long lTouchID,float x, float y)
{
    
    if((mpConfirmMsgCtl == NULL && mpMissionMsgCtl == NULL && mpPauseMsgCtl == NULL) &&
       mpMessageDlg && mpMessageDlg->EndTouch(lTouchID, x, y) == false) return;
#ifdef MAC
    if(mpStepMessageDlg) mpStepMessageDlg->EndTouch(lTouchID, x, y);
#endif
    CHWorld::EndTouch(lTouchID, x, y);
}


void CTrainingWorld::RenderFingerBegin(int nTime)
{
    
    const float fMin = 8.f;
    const float fMax = 15.f;
    
    float   matrix[16];
    
    CK9Sprite* pActor = (CK9Sprite*)GetActor();
    if(mStep == TRAININGSTEP_MOVE && !(pActor->isMoving() || pActor->isTurning()))
    {
        SPoint ptNow;
        pActor->GetPosition(&ptNow);
        
        SGLCAMERA* camera = GetCamera();
        //카메라의 방향으로 설정을 한다. ( camera방향은 -로 설정해준다.)
        float fAngle = atan2(-camera->viewDir.z,camera->viewDir.x) * 180.0 / PI;
        
        if(mMoveMotionStep == 0)
        {
            mfFingerUpDwonAction = mfFingerUpDwonAction + mfFingerUpDwonActionDirection * (float)nTime / 80.f;
            
            if(mfFingerUpDwonAction <= fMin)
            {
                
                mMoveMotionStep = 1;
                //                mfFingerUpDwonAction = fMin;
                //                mfFingerUpDwonActionDirection = 1.f;
            }
            else
            {
                SetStepMessage(NULL,"Tran_Step1_MoveDes1");
                mvtFingerMove.x = 0; //초기화.
                
                sglLoadIdentityf(matrix);
                
                //15 ~ 8 로 움직인다.
                sglTranslateMatrixf(matrix, ptNow.x, ptNow.y + mfFingerUpDwonAction, ptNow.z);
                
                sglRotateRzRyRxMatrixf(matrix,
                                       0,
                                       fAngle - 90.f,
                                       0);
                
                sglScaleMatrixf(matrix, 2.f, 4.f, 1.f);
                
                
                
                sglMultMatrixVectorf(&mFingerVertex[0], matrix,&gPanelVertexY[0]);
                sglMultMatrixVectorf(&mFingerVertex[3], matrix,&gPanelVertexY[3]);
                sglMultMatrixVectorf(&mFingerVertex[6], matrix,&gPanelVertexY[6]);
                sglMultMatrixVectorf(&mFingerVertex[9], matrix,&gPanelVertexY[9]);
            }
        }
        else
        {
            
            STARTPOINT* pMoveTargetPostion = m_pSGLCore->mlstStartPoint[INDEX_MOVEPOS];
            
            SetStepMessage(NULL,"Tran_Step1_MoveDes2");
            
            if(mvtFingerMove.x == 0)
            {
                mvtFingerMove.x = pMoveTargetPostion->fPosX - ptNow.x;
                mvtFingerMove.y = 0;
                mvtFingerMove.z = pMoveTargetPostion->fPosZ - ptNow.z;
                
                sglNormalize(&mvtFingerMove);
                memcpy(&mptFingerMove, &ptNow, sizeof(SPoint));
                
                mfFingerUpDwonAction = fMin;
                mfFingerUpDwonActionDirection = 1.f;
            }
            
            //----------------------------------------------------
            //이동하게 한다.
            //----------------------------------------------------
            mptFingerMove.x = mptFingerMove.x + mvtFingerMove.x * (float)nTime / 20.f;
            mptFingerMove.y = 0;
            mptFingerMove.z = mptFingerMove.z + mvtFingerMove.z * (float)nTime / 20.f;
            GetPositionY(&mptFingerMove);
            
            
            //목표지점에 도달했으면 이동을 중지 한다.
            float fxDir = pMoveTargetPostion->fPosX - mptFingerMove.x;
            float fzDir = pMoveTargetPostion->fPosZ - mptFingerMove.z;
            if((mvtFingerMove.x >= 0 && fxDir < 0) || (mvtFingerMove.x <= 0 && fxDir > 0) || (mvtFingerMove.z >= 0 && fzDir < 0) || (mvtFingerMove.z <= 0 && fzDir > 0))
            {
                //목표지점에 도착하면 다시 시작 한다.
                mMoveMotionStep = 0;
                mfFingerUpDwonAction = fMin;
                mfFingerUpDwonActionDirection = -1.f;
                mfFingerUpDwonAction = 15.f;
                
            }
            else
            {
                
                
                sglLoadIdentityf(matrix);
                
                
                //15 ~ 8 로 움직인다.
                sglTranslateMatrixf(matrix, mptFingerMove.x, mptFingerMove.y + mfFingerUpDwonAction, mptFingerMove.z);
                
                sglRotateRzRyRxMatrixf(matrix,
                                       0,
                                       fAngle - 90.f,
                                       0);
                
                sglScaleMatrixf(matrix, 2.f, 4.f, 1.f);
                
                
                
                sglMultMatrixVectorf(&mFingerVertex[0], matrix,&gPanelVertexY[0]);
                sglMultMatrixVectorf(&mFingerVertex[3], matrix,&gPanelVertexY[3]);
                sglMultMatrixVectorf(&mFingerVertex[6], matrix,&gPanelVertexY[6]);
                sglMultMatrixVectorf(&mFingerVertex[9], matrix,&gPanelVertexY[9]);
            }
            
        }
        
        
    }
    else if(mStep == TRAININGSTEP_MOVE)
    {
        SetStepMessage(NULL,"Tran_Step1_MoveDes3");
    }
    else if(mStep == TRAININGSTEP_ATTACK)
    {
        
        bool IsEnamy = false;
        
        if(mEnamySprite && mEnamySprite->GetState() == SPRITE_RUN)
        {
            SPoint ptNow;
            mEnamySprite->GetPosition(&ptNow);
            
            SGLCAMERA* camera = GetCamera();
            //카메라의 방향으로 설정을 한다. ( camera방향은 -로 설정해준다.)
            float fAngle = atan2(-camera->viewDir.z,camera->viewDir.x) * 180.0 / PI;
            
            mfFingerUpDwonAction = mfFingerUpDwonAction + mfFingerUpDwonActionDirection * (float)nTime / 80.f;
            
            if(mfFingerUpDwonAction <= fMin)
            {
                mfFingerUpDwonAction = fMin;
                mfFingerUpDwonActionDirection = 1.f;
            }
            if(mfFingerUpDwonAction >= fMax)
            {
                mfFingerUpDwonAction = fMax;
                mfFingerUpDwonActionDirection = -1.f;
            }
            
            
            //SetStepMessage(NULL,"Tran_Step1_MoveDes1");
            mvtFingerMove.x = 0; //초기화.
            
            sglLoadIdentityf(matrix);
            
            //15 ~ 8 로 움직인다.
            sglTranslateMatrixf(matrix, ptNow.x, ptNow.y + mfFingerUpDwonAction, ptNow.z);
            
            sglRotateRzRyRxMatrixf(matrix,
                                   0,
                                   fAngle - 90.f,
                                   0);
            
            sglScaleMatrixf(matrix, 2.f, 4.f, 1.f);
            
            
            
            sglMultMatrixVectorf(&mFingerVertex[0], matrix,&gPanelVertexY[0]);
            sglMultMatrixVectorf(&mFingerVertex[3], matrix,&gPanelVertexY[3]);
            sglMultMatrixVectorf(&mFingerVertex[6], matrix,&gPanelVertexY[6]);
            sglMultMatrixVectorf(&mFingerVertex[9], matrix,&gPanelVertexY[9]);
            IsEnamy = true;
        }
        else if(mEnamySprite && mEnamySprite->GetState() != SPRITE_RUN)
        {
            mEnamySprite = NULL;
        }
        
        
        if(IsEnamy == false && mEnamySprite2 && mEnamySprite2->GetState() == SPRITE_RUN)
        {
            SPoint ptNow;
            mEnamySprite2->GetPosition(&ptNow);
            
            SGLCAMERA* camera = GetCamera();
            //카메라의 방향으로 설정을 한다. ( camera방향은 -로 설정해준다.)
            float fAngle = atan2(-camera->viewDir.z,camera->viewDir.x) * 180.0 / PI;
            
            mfFingerUpDwonAction = mfFingerUpDwonAction + mfFingerUpDwonActionDirection * (float)nTime / 80.f;
            
            if(mfFingerUpDwonAction <= fMin)
            {
                mfFingerUpDwonAction = fMin;
                mfFingerUpDwonActionDirection = 1.f;
            }
            if(mfFingerUpDwonAction >= fMax)
            {
                mfFingerUpDwonAction = fMax;
                mfFingerUpDwonActionDirection = -1.f;
            }
            
            
            //SetStepMessage(NULL,"Tran_Step1_MoveDes1");
            mvtFingerMove.x = 0; //초기화.
            
            sglLoadIdentityf(matrix);
            
            //15 ~ 8 로 움직인다.
            sglTranslateMatrixf(matrix, ptNow.x, ptNow.y + mfFingerUpDwonAction, ptNow.z);
            
            sglRotateRzRyRxMatrixf(matrix,
                                   0,
                                   fAngle - 90.f,
                                   0);
            
            sglScaleMatrixf(matrix, 2.f, 4.f, 1.f);
            
            
            
            sglMultMatrixVectorf(&mFingerVertex[0], matrix,&gPanelVertexY[0]);
            sglMultMatrixVectorf(&mFingerVertex[3], matrix,&gPanelVertexY[3]);
            sglMultMatrixVectorf(&mFingerVertex[6], matrix,&gPanelVertexY[6]);
            sglMultMatrixVectorf(&mFingerVertex[9], matrix,&gPanelVertexY[9]);
            IsEnamy = true;
        }
        else if(mEnamySprite2 && mEnamySprite2->GetState() != SPRITE_RUN)
        {
            mEnamySprite2 = NULL;
        }
        
        if(IsEnamy == false)
        {
            mEnamySprite = NULL;
            OnNextStep(mStep);
        }
    }
    else if(mStep == TRAININGSTEP_ATTACKRADIA)
    {
        if(!(mEnamySprite && mEnamySprite->GetState() == SPRITE_RUN))
        {
            mEnamySprite = NULL;
            mpRadaBoard->SetHide(true);
            if(mpRaidaFingerVertex)
            {
                delete[] mpRaidaFingerVertex;
                mpRaidaFingerVertex = NULL;
            }
            OnNextStep(mStep);
        }
    }
    else if(mStep == TRAININGSTEP_MAKETOWER)
    {
        CListNode<CSprite>* pList = GetSGLCore()->GetSpriteList();
        ByNode<CSprite>* b = pList->begin();
        for(ByNode<CSprite>* it = b; it != pList->end(); )
        {
            CSprite* sp = it->GetObject();
            if(sp->GetState() == SPRITE_RUN && sp->GetModelID() == ACTORID_BLZ_DMISSILETOWER)
            {
                //타워는 절대 터지지 않게 유지한다.
                sp->SetCurrentDefendPower(sp->GetMaxDefendPower());
                
                if(((CDTower*)sp)->IsMaking())
                {
                    SetStepMessage(NULL,"Tran_Step6_MakeTowerDes2");
                    
                    if(mpArrowBlz)
                    {
                        delete mpArrowBlz;
                        mpArrowBlz = NULL;
                    }
                    
                    if(mpRaidaFingerVertex)
                    {
                        delete[] mpRaidaFingerVertex;
                        mpRaidaFingerVertex = NULL;
                    }
                    
                
                    mEnamySprite2 = NULL;
                }
                else //건물을 다지었다.
                {
                    SPoint ptNow;
                    mEnamySprite2 = sp;
                    mEnamySprite2->GetPosition(&ptNow);
                    SetRotationCarmeraToPoint(&ptNow);
                    OnNextStep(mStep); //다음 스템으로 변경한다.
                }
                break;
            }
            it = it->GetNextNode();
        }
    }
    else if(mStep == TRAININGSTEP_UPGRADE)
    {
        //타워는 절대 터지지 않게 유지한다.
        mEnamySprite2->SetCurrentDefendPower(mEnamySprite2->GetMaxDefendPower());

        if(mbUpgrading == false)
        {
            //비행기도 절대 터지지 않게 한다.
            mEnamySprite->SetCurrentDefendPower(mEnamySprite->GetMaxDefendPower());
            
            if(mpClickedSprite == NULL) //건물이 선택되어 있지 않으면 건물을 선택하게 해준다.
            {
                SetStepMessage(NULL,"Tran_Step6_UpgradeDes1");
                if(mpRaidaFingerVertex)
                {
                    delete[] mpRaidaFingerVertex;
                    mpRaidaFingerVertex = NULL;
                }
                
                if(mEnamySprite2 && mEnamySprite2->GetState() == SPRITE_RUN)
                {
                    SPoint ptNow;
                    
                    mEnamySprite2->GetPosition(&ptNow);
                    
                    SGLCAMERA* camera = GetCamera();
                    //카메라의 방향으로 설정을 한다. ( camera방향은 -로 설정해준다.)
                    float fAngle = atan2(-camera->viewDir.z,camera->viewDir.x) * 180.0 / PI;
                    
                    mfFingerUpDwonAction = mfFingerUpDwonAction + mfFingerUpDwonActionDirection * (float)nTime / 80.f;
                    
                    if(mfFingerUpDwonAction <= fMin)
                    {
                        mfFingerUpDwonAction = fMin;
                        mfFingerUpDwonActionDirection = 1.f;
                    }
                    if(mfFingerUpDwonAction >= fMax)
                    {
                        mfFingerUpDwonAction = fMax;
                        mfFingerUpDwonActionDirection = -1.f;
                    }
                    
                    mvtFingerMove.x = 0; //초기화.
                    sglLoadIdentityf(matrix);
                    
                    //15 ~ 8 로 움직인다.
                    sglTranslateMatrixf(matrix, ptNow.x, ptNow.y + 10.0f + mfFingerUpDwonAction, ptNow.z);
                    
                    sglRotateRzRyRxMatrixf(matrix,
                                           0,
                                           fAngle - 90.f,
                                           0);
                    
                    sglScaleMatrixf(matrix, 2.f, 4.f, 1.f);
                    
                    
                    
                    sglMultMatrixVectorf(&mFingerVertex[0], matrix,&gPanelVertexY[0]);
                    sglMultMatrixVectorf(&mFingerVertex[3], matrix,&gPanelVertexY[3]);
                    sglMultMatrixVectorf(&mFingerVertex[6], matrix,&gPanelVertexY[6]);
                    sglMultMatrixVectorf(&mFingerVertex[9], matrix,&gPanelVertexY[9]);
                }
                else
                    mEnamySprite2 = NULL;
            }
            else
            {
                SetStepMessage(NULL,"Tran_Step6_UpgradeDes2");
                if(mpRaidaFingerVertex == NULL)
                {
                    //손가락
                    CControl* ctl = mpItemSlotBar->FindControl(CTL_TABLE_ITEM_SLOT3);
                    ctl->SetAni(CONTROLANI_WIGWAG,0.3f);
                    
                    float fPX,fPY,szW,szH;
                    ctl->GetPosition(fPX, fPY);
                    ctl->GetSize(szW, szH);
                    szH *= 0.5f;
                    vector<string> lstImage;
                    lstImage.push_back("none");
                    lstImage.push_back("none");
                    CLabelCtl tmpCtl(NULL,GetTextureMan());
                    tmpCtl.Initialize(0, fPX + szW/2.f, fPY - szH/2.f, szH, szH*2.f, lstImage, 0.f, 0.f, 0.f, 0.f);
                    
                    
                    mpRaidaFingerVertex = new float[12];
                    memcpy(mpRaidaFingerVertex, tmpCtl.GetWorldVertex(), sizeof(float) * 12);
                    float fY = (tmpCtl.GetWorldVertex()[1] - tmpCtl.GetWorldVertex()[7]) / 2.f; //상단으로 옮겨준다.
                    mfRaidaFingerMax = fY * 2;
                    mfRaidaFingerMin = fY;
                }
                
                
            }
        }
        else
        {
            SetStepMessage(NULL,"Tran_Step6_UpgradeDes1");
            
            if(mpRaidaFingerVertex)
            {
                delete[] mpRaidaFingerVertex;
                mpRaidaFingerVertex = NULL;
            }
        }
    }
    else if(mStep == TRAININGSTEP_MISSION)
    {
        SPoint ptNow;
        
        if(mEnamySprite && mEnamySprite->GetState() == SPRITE_RUN)
        {
            mEnamySprite->GetPosition(&ptNow);
            
            SGLCAMERA* camera = GetCamera();
            //카메라의 방향으로 설정을 한다. ( camera방향은 -로 설정해준다.)
            float fAngle = atan2(-camera->viewDir.z,camera->viewDir.x) * 180.0 / PI;
            
            mfFingerUpDwonAction = mfFingerUpDwonAction + mfFingerUpDwonActionDirection * (float)nTime / 80.f;
            
            if(mfFingerUpDwonAction <= fMin)
            {
                mfFingerUpDwonAction = fMin;
                mfFingerUpDwonActionDirection = 1.f;
            }
            if(mfFingerUpDwonAction >= fMax)
            {
                mfFingerUpDwonAction = fMax;
                mfFingerUpDwonActionDirection = -1.f;
            }
            
            
            //SetStepMessage(NULL,"Tran_Step1_MoveDes1");
            mvtFingerMove.x = 0; //초기화.
            
            sglLoadIdentityf(matrix);
            
            //15 ~ 8 로 움직인다.
            sglTranslateMatrixf(matrix, ptNow.x, ptNow.y + 10.0f + mfFingerUpDwonAction, ptNow.z);
            
            sglRotateRzRyRxMatrixf(matrix,
                                   0,
                                   fAngle - 90.f,
                                   0);
            
            sglScaleMatrixf(matrix, 2.f, 4.f, 1.f);
            
            
            
            sglMultMatrixVectorf(&mFingerVertex[0], matrix,&gPanelVertexY[0]);
            sglMultMatrixVectorf(&mFingerVertex[3], matrix,&gPanelVertexY[3]);
            sglMultMatrixVectorf(&mFingerVertex[6], matrix,&gPanelVertexY[6]);
            sglMultMatrixVectorf(&mFingerVertex[9], matrix,&gPanelVertexY[9]);
        }
        else
            mEnamySprite = NULL;
    }
    
    if(mpRaidaFingerVertex)
    {
        sglLoadIdentityf(matrix);
        if(mStep == TRAININGSTEP_MAKETOWER)
        {
            if(gnDisplayType == DISPLAY_IPAD)
            {
                mfFingerUpDwonAction = mfFingerUpDwonAction + mfFingerUpDwonActionDirection * (float)nTime / 20.f;
                if(mfFingerUpDwonAction <= mfRaidaFingerMin)
                {
                    mfFingerUpDwonAction = mfRaidaFingerMin;
                    mfFingerUpDwonActionDirection = 1.f;
                }
                if(mfFingerUpDwonAction >= mfRaidaFingerMax)
                {
                    mfFingerUpDwonAction = mfRaidaFingerMin;
                }
                //15 ~ 8 로 움직인다.
                sglTranslateMatrixf(matrix, 0.f, mfFingerUpDwonAction, 0.f);
            }
            else
            {
                mfFingerUpDwonAction = mfFingerUpDwonAction + mfFingerUpDwonActionDirection * (float)nTime / 30.f;
                if(mfFingerUpDwonAction <= mfRaidaFingerMin)
                {
                    mfFingerUpDwonAction = mfRaidaFingerMax;
                }
                if(mfFingerUpDwonAction >= mfRaidaFingerMax)
                {
                    mfFingerUpDwonAction = mfRaidaFingerMax;
                    mfFingerUpDwonActionDirection = -1.f;
                }
                //15 ~ 8 로 움직인다.
                sglTranslateMatrixf(matrix, 0.f, 0.f, mfFingerUpDwonAction);
            }
        }
        else
        {
            mfFingerUpDwonAction = mfFingerUpDwonAction + mfFingerUpDwonActionDirection * (float)nTime / 200.f;
            if(mfFingerUpDwonAction <= mfRaidaFingerMin)
            {
                mfFingerUpDwonAction = mfRaidaFingerMin;
                mfFingerUpDwonActionDirection = 1.f;
            }
            if(mfFingerUpDwonAction >= mfRaidaFingerMax)
            {
                mfFingerUpDwonAction = mfRaidaFingerMax;
                mfFingerUpDwonActionDirection = -1.f;
            }
            
            //15 ~ 8 로 움직인다.
            sglTranslateMatrixf(matrix, 0.f, mfFingerUpDwonAction, 0.f);
            
        }
        sglMultMatrixVectorf(&mRaidaFingerWorldVertex[0], matrix,&mpRaidaFingerVertex[0]);
        sglMultMatrixVectorf(&mRaidaFingerWorldVertex[3], matrix,&mpRaidaFingerVertex[3]);
        sglMultMatrixVectorf(&mRaidaFingerWorldVertex[6], matrix,&mpRaidaFingerVertex[6]);
        sglMultMatrixVectorf(&mRaidaFingerWorldVertex[9], matrix,&mpRaidaFingerVertex[9]);
        
    }
}

void CTrainingWorld::RenderFinger()
{
    CK9Sprite* pActor = (CK9Sprite*)GetActor();
    if(
       (mStep == TRAININGSTEP_MOVE && !(pActor->isMoving() || pActor->isTurning())) ||
       ((mStep == TRAININGSTEP_ATTACK || mStep == TRAININGSTEP_MISSION ) && mEnamySprite) ||
       (mStep == TRAININGSTEP_UPGRADE && mpClickedSprite == NULL && mbUpgrading == false)
       )
    {
        glColor4f(1.0f,1.0f,1.0f,1.0f);
        glBindTexture(GL_TEXTURE_2D, mnFingerTexture);
        glTexCoordPointer(2, GL_FLOAT, 0, gPanelCoordTex);
        
        glVertexPointer(3, GL_FLOAT, 0, mFingerVertex);
        glDrawElements(GL_TRIANGLE_STRIP, gshPanelIndicesCnt, GL_UNSIGNED_SHORT, gPanelIndices);
    }
}

void CTrainingWorld::SetClickedSprite(CSprite* pClickedSprite)
{
//Added By Song 2014.03.27 캐릭터 클릭이벤트를 전부 무시하자.
    
//    if(mpClickedSprite == NULL && pClickedSprite == NULL) return;
//    if(GetActor() == pClickedSprite) return ; //Added By Song 2014.02.25 5단계에서 탱크를 클릭하면 더이상 진행할 수가 없다.
//    mpClickedSprite = pClickedSprite;
//    if(pClickedSprite) OnSelectedObjectShowDescBar(pClickedSprite);
    
    if(mStep == TRAININGSTEP_UPGRADE)
    {
        if(GetActor() == pClickedSprite) return ;
        mpClickedSprite = pClickedSprite;
        ResetDisplayItemSlot();
        if(pClickedSprite) OnSelectedObjectShowDescBar(pClickedSprite);
    }
}



//확장한 이유는 적이 터졌을때 점수를 올려주지 말자.
void CTrainingWorld::CheckAttectedByEnSpriteAtBomb(CListNode<CSprite> *pCoreListSprite,CBomb *pBomb)
{
    float fSPAndBombDistance;
    CSprite *pSprite;
    CSprite *pActor = GetActor();
    CSprite *pOwnerBomb = pBomb->GetOwner();
    int spState;
    SPoint ptSp,ptBomb;
//    CSGLCore *pSGLCore = GetSGLCore();
    
//    float subX;
//    float subY;
    
    float fCompactGrade = 0;
    //    float fAttackInter = 0.0f;
    
    int nActorTeamID = pActor->GetTeamID();
    int nSpriteTeamID = 0;
    int nBombTeamID = 0;
    
    pBomb->GetPosition(&ptBomb);
    for(ByNode<CSprite>* it = pCoreListSprite->end()->GetPrevNode(); it != &pCoreListSprite->GetHead();)
    {
        pSprite = it->GetObject();
        it = it->GetPrevNode();
        
        spState = pSprite->GetState();
        
        
        if( spState != SPRITE_RUN) continue;
        
        nSpriteTeamID = pSprite->GetTeamID();
        nBombTeamID = pBomb->GetTeamID();
        
        if(pSprite->GetType() & ACTORTYPE_TYPE_GHOSTANITEM || IsAllians(nSpriteTeamID, nBombTeamID)) continue;
        
        pSprite->GetPosition(&ptSp);
        fSPAndBombDistance = sglSquareDistanceSPointSPointf(&ptSp, &ptBomb);
        
        if(fSPAndBombDistance < 90000.f && pBomb->GetOwner())
        {
            pSprite->Command(AICOMMAND_ATTACKEDBYEMSPRITE, pBomb->GetOwner());
        }
        
        if(fSPAndBombDistance < 2000.f)
        {
            if( pSprite->GetState() != SPRITE_DELETED)
            {
                fCompactGrade = pBomb->InsideRangeCompact(fSPAndBombDistance,pSprite);
                if(fCompactGrade != 0.f)
                {
                    //SPRITE_READYDELETE하면 다음단계에서 캐릭터는 SPRITE_READYDELETE => SPRITE_DELETED 로 해버린다. 그럴경운 그다음 단계에 CET1의 부스터의 단계는 SPRITE_DELETED로 변경되어 에러가 발생한다.
                    pOwnerBomb = pBomb->GetOwner();
                    if(pSprite->CompactToBomb(pBomb,fCompactGrade)) //명중하여 .....
                    {
                        
                        pSprite->SetState(BOMB_COMPACT);
                        
                        if(mStep == TRAININGSTEP_END)
                        {
                            //맞아서 죽으면 기준치값을 받아온다.
                            if(nActorTeamID == pBomb->GetTeamID())
                            {
                                if(pSprite->GetModelID() == ACTORID_BLZ_EMAIN)
                                    CScenario::SendMessage(SGL_MESSAGE_SUCCESS_MISSION,0);
                            }
                            else if(IsAllians(nActorTeamID, nBombTeamID)) //동맹군이 주이거나 메인타워를 터트리면 아이템을 만들거나, 게임을 종려한다.
                            {
                                if(pSprite->GetModelID() == ACTORID_BLZ_EMAIN)
                                    CScenario::SendMessage(SGL_MESSAGE_SUCCESS_MISSION,0);
                            }
                        }
                        else if(mStep == TRAININGSTEP_MISSION && pSprite->GetModelID() == ACTORID_BLZ_EMAIN)
                        {
                            mEnamySprite = NULL;
                            mEnamySprite2 = NULL;
                            CScenario::SendMessage(SGL_TRAINING_NEXT_END,0);
                        }
                        
                    }
                    else
                    {
                        
                        if(pSprite == pActor) //버블이 있으면 흔들지 말자.
                        {
                            CAniRuntimeBubble* pBubble = pSprite->GetAniRuntimeBubble();
                            if(pBubble)
                            {
                                pBubble->Attacted();
                            }
                            else
                            {
                                //버블이 있을 경우 그리고 폭탄이 레이져일 경우 메인타워 버블안쪽에 있는 경우.
                                if(!(pBomb->GetBombType() & BOMB_FASET_BOMB_TYPE) && pSprite->GetMainTowerBubbleCntInRange() == 0)
                                {
                                    SetAnimationCameraAttacked();
                                    GetSGLCore()->PlayVibrate();
                                }

                            }
                        }
                    }
                    
                    if(pOwnerBomb)
                        pOwnerBomb->Command(AICOMMAND_HITTHEMARK, (void*)1); //명중했다라고 ....
                    
                }
            }
        }
    }
}


void CTrainingWorld::ResetDisplayItemSlot()
{
    if(mStep == TRAININGSTEP_CHANGE_SLOT)
    {
        CSprite* pActor = GetActor();
        vector<string>  lstImage;
        int nMaxPageCnt = 0;
        const int nMaxItem = MAXSLOTCNT;
        vector<CControl*> lstSlotCtl;
        CUserInfo* pUserInfo = mpScenario->GetUserInfo();
        vector<USERINFO*>  lstType3Org;
        vector<USERINFO*>* lstItem = NULL;
        int nType = 0;
        int nBombID = 0;
        
        if(mpClickedSprite == NULL)
        {
            nType = pUserInfo->GetLastBombItemSwitch(); // 0 : Bomb 1:Item 2:Blz 3:ClickedTower
            if(nType == 1)
            {
                lstItem = pUserInfo->GetListItem();
            }
            else if(nType == 0)
            {
                lstItem = pUserInfo->GetListBomb();
                if(pActor && pActor->GetState() == SPRITE_RUN)
                    nBombID = pActor->GetCurrentWeaponID();
            }
            else //2 : Blz
            {
                lstItem = pUserInfo->GetListBlz();
            }
            mpItemSlotSwitchBtn->SetEnable(true);
        }
        else
        {
            nType = 3;
            
            //------------------------------------------------
            //TowerStatus Info List
            USERINFO* pNew = new USERINFO;
            pNew->nID = RUNTIME_UPDATE_REMOVE;
            pNew->nType = 1; //영구 사용
            pNew->nCnt = 0; //포스텝으로 카운트를 나타낸다.
            lstType3Org.push_back(pNew);
            lstItem = &lstType3Org;
            //------------------------------------------------
            
            mpItemSlotSwitchBtn->SetEnable(false);
        }
        
        //        //내용을 다 지우자.
        //        lstItem->clear();
        
        mpSelectedWeaponBtn = NULL;
        
        CNumberCtl* pItemPageNumber = (CNumberCtl*)mpItemSlotBar->FindControl(CTL_TABLE_ITEM_SLOT_PAGE);
        
        int nSelIndex = 1;
        if(nType == 1)
            nSelIndex = pUserInfo->GetLastSelectedItemPageIndex();
        else if(nType == 0)
            nSelIndex = pUserInfo->GetLastSelectedBombPageIndex();
        else if(nType == 2)
            nSelIndex = pUserInfo->GetLastSelectedBlzPageIndex();
        
        
        
        lstImage.clear();
        lstImage.push_back("res_slot.png");
        
        if(mpClickedSprite == NULL)
        {
            if(pUserInfo->GetLastBombItemSwitch() == 0)
                lstImage.push_back("M_Slot_Bomb");
            else if(pUserInfo->GetLastBombItemSwitch() == 1)
                lstImage.push_back("M_Slot_Item");
            else if(pUserInfo->GetLastBombItemSwitch() == 2)
                lstImage.push_back("M_Slot_BLZ");
        }
        else
        {
            lstImage.push_back("타워");
        }
        mpItemSlotSwitchBtn->SetImageData(lstImage);
        
        nMaxPageCnt = (int)lstItem->size() / nMaxItem;
        if((lstItem->size() % nMaxItem) != 0)
            nMaxPageCnt += 1;
        
        if(nSelIndex > nMaxPageCnt)
            nSelIndex = nMaxPageCnt;
        else if( nSelIndex < 1)
            nSelIndex = 1;
        
        pItemPageNumber->SetNumber(nSelIndex);
        
        
        CControl* pUp = mpItemSlotBar->FindControl(BTN_ITEMSLOTUPPAGE);
        CControl* pDown= mpItemSlotBar->FindControl(BTN_ITEMSLOTDOWNPAGE);
        
        if(nSelIndex == 1)
            pDown->SetEnable(false);
        else
            pDown->SetEnable(true);
        
        if(nSelIndex == nMaxPageCnt)
            pUp->SetEnable(false);
        else
            pUp->SetEnable(true);
        
        
        for(int i = CTL_TABLE_ITEM_SLOT3; i <= CTL_TABLE_ITEM_SLOT6; i++)
        {
            lstSlotCtl.push_back(mpItemSlotBar->FindControl(i));
        }
        
        int nIndex = 0;
        int nMaxCnt = (int)lstItem->size();
        string sPath;
        CButtonCtl* pBtnSlot = NULL;
        for (int i = 0; i < nMaxItem; i++)
        {
            nIndex = (nSelIndex - 1) * nMaxItem + i;
            pBtnSlot = (CButtonCtl*)lstSlotCtl[i];
            
            if(nIndex >= nMaxCnt || nMaxCnt == 0)
            {
                lstImage.clear();
                lstImage.push_back("none");
                lstImage.push_back("none");
                pBtnSlot->GetChild(0)->SetImageData(lstImage);
                pBtnSlot->GetChild(0)->GetChild(0)->SetHide(true);
                pBtnSlot->GetChild(0)->GetChild(1)->SetHide(true);
                pBtnSlot->SetLData(-1);
                
                pBtnSlot->SetButtonType(BUTTON_NORMAL);
                pBtnSlot->SetPushed(false);
                pBtnSlot->SetEnable(false);
            }
            else
            {
                pBtnSlot->SetEnable(false);
                pBtnSlot->GetChild(0)->GetChild(1)->SetHide(true);
                if(nType == 1)
                {
                    int nID = lstItem->at(nIndex)->nID;
                    PROPERTY_ITEM property;
                    if(PROPERTY_ITEM::GetPropertyItem(nID, property) == NULL) continue;
                    
                    lstImage.clear();
                    sPath = property.sModelPath;
                    sPath.append(".tga");
                    lstImage.push_back(sPath);
                    lstImage.push_back("none");
                    
                    pBtnSlot->GetChild(0)->SetImageData(lstImage);
                    pBtnSlot->GetChild(0)->SetBackCoordTex(0.0f, 0.0f, 0.96875f, 0.96875f);
                    pBtnSlot->SetLData((long)lstItem->at(nIndex));
                    pBtnSlot->SetPushed(false);
                    
                    pBtnSlot->SetButtonType(BUTTON_NORMAL);
                    
                    if(property.nType == 0) //Count
                    {
                        CNumberCtl* pNumCtl = (CNumberCtl*)lstSlotCtl[i]->GetChild(0)->GetChild(0);
                        pNumCtl->SetNumber(pUserInfo->GetCntByIDWithItem(nID));
                        pBtnSlot->GetChild(0)->GetChild(0)->SetHide(false);
                    }
                    else if(property.nType == 2) //Upgrade
                    {
                        PROPERTY_TANK property;
                        PROPERTY_TANK::GetPropertyTank(pUserInfo->GetLastSelectedTankID(),property);
                        if(nID == ID_ITEM_UPGRADE_ATTACK)
                        {
                            CNumberCtl* pNumCtl = (CNumberCtl*)pBtnSlot->GetChild(0)->GetChild(0);
                            int nUpgrade = pUserInfo->GetAttackUpgrade() * 5; //100으로 처리하자.
                            pNumCtl->SetNumber(nUpgrade,CNumberCtlType_Percent);
                            pBtnSlot->GetChild(0)->GetChild(0)->SetHide(false);
                            
                        }
                        else if(nID == ID_ITEM_UPGRADE_DEPEND)
                        {
                            CNumberCtl* pNumCtl = (CNumberCtl*)pBtnSlot->GetChild(0)->GetChild(0);
                            int nUpgrade = pUserInfo->GetDependUpgrade() * 5; //100으로 처리하자.
                            pNumCtl->SetNumber(nUpgrade,CNumberCtlType_Percent);
                            pBtnSlot->GetChild(0)->GetChild(0)->SetHide(false);
                            
                        }
                        else if(nID == ID_ITEM_UPGRADE_DEFENCE_TOWER || nID == ID_ITEM_UPGRADE_MISSILE_TOWER)
                        {
                            CNumberCtl* pNumCtl = (CNumberCtl*)lstSlotCtl[i]->GetChild(0)->GetChild(0);
                            int nUpgrade = pUserInfo->GetCntByIDWithItem(nID) * 50; //100으로 처리하자.
                            pNumCtl->SetNumber(nUpgrade,CNumberCtlType_Percent);
                            lstSlotCtl[i]->GetChild(0)->GetChild(0)->SetHide(false);
                        }
                        else
                        {
                            //pNumCtl->ClearNumber();
                            pBtnSlot->GetChild(0)->GetChild(0)->SetHide(true);
                        }
                    }
                    else
                    {
                        //                pNumCtl->ClearNumber();
                        pBtnSlot->GetChild(0)->GetChild(0)->SetHide(true);
                    }
                }
                else if(nType == 0) //Bomb
                {
                    int nID = lstItem->at(nIndex)->nID;
                    PROPERTY_BOMB property;
                    PROPERTY_BOMB::GetPropertyBomb(nID, property);
                    
                    lstImage.clear();
                    lstImage.push_back(property.sBombTableImgPath);
                    lstImage.push_back("none");
                    pBtnSlot->GetChild(0)->SetImageData(lstImage);
                    //폭탄이미지는 0.0,0.0,1.0,1.0 이 아니다.
                    //0.2265625 => 0.1265625 (비율때문)
                    //0.7734375 => 0.8734375 (비율때문)
                    pBtnSlot->GetChild(0)->SetBackCoordTex(0.20703125, 0.1265625, 0.79296875, 0.8734375);
                    pBtnSlot->SetLData((long)lstItem->at(nIndex));
                    pBtnSlot->SetButtonType(BUTTON_GROUP_TOGGLE);
                    
                    pBtnSlot->SetLData2(property.nType); //카운터.. 비카운터
                    if(nBombID == nID)
                    {
                        pBtnSlot->SetPushed(true);
                        mpSelectedWeaponBtn = pBtnSlot;
                    }
                    else
                        pBtnSlot->SetPushed(false);
                    
                    
                    if(property.nType == 0) //Count
                    {
                        CNumberCtl* pNumCtl = (CNumberCtl*)lstSlotCtl[i]->GetChild(0)->GetChild(0);
                        pNumCtl->SetNumber(pUserInfo->GetCntByIDWithBomb(nID));
                        pBtnSlot->GetChild(0)->GetChild(0)->SetHide(false);
                    }
                    else
                    {
                        pBtnSlot->GetChild(0)->GetChild(0)->SetHide(true);
                    }
                }
                else if(nType == 2)
                {
                    USERINFO* pInfo = lstItem->at(nIndex);
                    int nID = pInfo->nID;
                    PROPERTY_TANK property;
                    PROPERTY_TANK::GetPropertyTank(nID, property);
                    
                    lstImage.clear();
                    sPath = property.sModelPath;
                    sPath.append(".tga");
                    lstImage.push_back(sPath);
                    lstImage.push_back("none");
                    
                    pBtnSlot->GetChild(0)->SetImageData(lstImage);
                    pBtnSlot->GetChild(0)->SetBackCoordTex(0.0f, 0.0f, 0.96875f, 0.96875f);
                    pBtnSlot->GetChild(0)->GetChild(1)->SetHide(false);
                    pBtnSlot->SetLData((long)pInfo);
                    pBtnSlot->SetPushed(false);
                    
                    pBtnSlot->SetButtonType(BUTTON_NORMAL);
                    
                    if(pInfo->nType == 0) //Count
                    {
                        CNumberCtl* pNumCtl = (CNumberCtl*)lstSlotCtl[i]->GetChild(0)->GetChild(0);
                        pNumCtl->SetNumber(pInfo->nCnt);
                        pBtnSlot->GetChild(0)->GetChild(0)->SetHide(false);
                    }
                    else
                    {
                        pBtnSlot->GetChild(0)->GetChild(0)->SetHide(true);
                    }
                }
                else if(nType == 3)
                {
                    USERINFO* pInfo = lstItem->at(nIndex);
                    int nID = pInfo->nID;
                    if(nID == RUNTIME_UPDATE_REMOVE) //현재 건물을 제거한다.
                    {
                        lstImage.clear();
                        sPath = "RevTower"; //RUNTIME_UPDATE_REMOVE 아이콘 패스
                        sPath.append(".tga");
                        lstImage.push_back(sPath);
                        lstImage.push_back("none");
                        
                        pBtnSlot->GetChild(0)->SetImageData(lstImage);
                        pBtnSlot->GetChild(0)->SetBackCoordTex(0.0f, 0.0f, 0.96875f, 0.96875f);
                        pBtnSlot->SetLData((long)pInfo);
                        pBtnSlot->SetPushed(false);
                        
                        pBtnSlot->GetChild(0)->GetChild(0)->SetHide(true);
                        pBtnSlot->SetButtonType(BUTTON_NORMAL);
                    }
                    
                }
            }
        }
        
        if(nType == 1)
            pUserInfo->SetLastSelectedItemPageIndex(nSelIndex);
        else if(nType == 0)
            pUserInfo->SetLastSelectedBombPageIndex(nSelIndex);
        else if(nType == 2)
            pUserInfo->SetLastSelectedBlzPageIndex(nSelIndex);
        
        mpItemSlotBar->SetHide(false);
    }
    else if(mStep == TRAININGSTEP_MAKETOWER)
    {
        vector<string>  lstImage;
        int nMaxPageCnt = 0;
        const int nMaxItem = MAXSLOTCNT;
        vector<CControl*> lstSlotCtl;
        CUserInfo* pUserInfo = mpScenario->GetUserInfo();
        vector<USERINFO*>  lstType3Org;
        vector<USERINFO*>* lstItem = NULL;
        int nType = 2;
        
        
        //스롯의 내용을 하나만 정의 하자.
        lstItem = pUserInfo->GetListBlz();
        
        
        for(vector<USERINFO*>::iterator it = lstItem->begin(); it != lstItem->end();)
        {
            if((*it)->nID != ACTORID_BLZ_DMISSILETOWER )
                it = lstItem->erase(it);
            else if((*it)->nID == ACTORID_BLZ_DMISSILETOWER)
            {
                (*it)->nCnt = 1;
                it++;
            }
            else
                it++;
        }
        //        lstItem->erase(lstItem->begin());
        
        mpItemSlotSwitchBtn->SetEnable(false);
        
        CNumberCtl* pItemPageNumber = (CNumberCtl*)mpItemSlotBar->FindControl(CTL_TABLE_ITEM_SLOT_PAGE);
        
        int nSelIndex = 1;
        nSelIndex = pUserInfo->GetLastSelectedBlzPageIndex();
        
        lstImage.clear();
        lstImage.push_back("res_slot.png");
        
        if(nType == 2)
            lstImage.push_back("M_Slot_BLZ");
        
        mpItemSlotSwitchBtn->SetImageData(lstImage);
        
        nMaxPageCnt = (int)lstItem->size() / nMaxItem;
        if((lstItem->size() % nMaxItem) != 0)
            nMaxPageCnt += 1;
        
        if(nSelIndex > nMaxPageCnt)
            nSelIndex = nMaxPageCnt;
        else if( nSelIndex < 1)
            nSelIndex = 1;
        
        pItemPageNumber->SetNumber(nSelIndex);
        
        
        CControl* pUp = mpItemSlotBar->FindControl(BTN_ITEMSLOTUPPAGE);
        CControl* pDown= mpItemSlotBar->FindControl(BTN_ITEMSLOTDOWNPAGE);
        
        if(nSelIndex == 1)
            pDown->SetEnable(false);
        else
            pDown->SetEnable(true);
        
        if(nSelIndex == nMaxPageCnt)
            pUp->SetEnable(false);
        else
            pUp->SetEnable(true);
        
        
        for(int i = CTL_TABLE_ITEM_SLOT3; i <= CTL_TABLE_ITEM_SLOT6; i++)
        {
            lstSlotCtl.push_back(mpItemSlotBar->FindControl(i));
        }
        
        int nIndex = 0;
        int nMaxCnt = (int)lstItem->size();
        string sPath;
        CButtonCtl* pBtnSlot = NULL;
        for (int i = 0; i < nMaxItem; i++)
        {
            nIndex = (nSelIndex - 1) * nMaxItem + i;
            pBtnSlot = (CButtonCtl*)lstSlotCtl[i];
            
            if(nIndex >= nMaxCnt || nMaxCnt == 0)
            {
                lstImage.clear();
                lstImage.push_back("none");
                lstImage.push_back("none");
                pBtnSlot->GetChild(0)->SetImageData(lstImage);
                pBtnSlot->GetChild(0)->GetChild(0)->SetHide(true);
                pBtnSlot->GetChild(0)->GetChild(1)->SetHide(true);
                pBtnSlot->SetLData(-1);
                
                pBtnSlot->SetButtonType(BUTTON_NORMAL);
                pBtnSlot->SetPushed(false);
                pBtnSlot->SetEnable(false);
            }
            else
            {
                pBtnSlot->SetEnable(true);
                pBtnSlot->GetChild(0)->GetChild(1)->SetHide(true);
                if(nType == 2)
                {
                    USERINFO* pInfo = lstItem->at(nIndex);
                    int nID = pInfo->nID;
                    PROPERTY_TANK property;
                    PROPERTY_TANK::GetPropertyTank(nID, property);
                    
                    lstImage.clear();
                    sPath = property.sModelPath;
                    sPath.append(".tga");
                    lstImage.push_back(sPath);
                    lstImage.push_back("none");
                    
                    pBtnSlot->GetChild(0)->SetImageData(lstImage);
                    pBtnSlot->GetChild(0)->SetBackCoordTex(0.0f, 0.0f, 0.96875f, 0.96875f);
                    pBtnSlot->SetLData((long)pInfo);
                    pBtnSlot->SetPushed(false);
                    
                    pBtnSlot->SetButtonType(BUTTON_NORMAL);
                    
                    if(pInfo->nType == 0) //Count
                    {
                        CNumberCtl* pNumCtl = (CNumberCtl*)lstSlotCtl[i]->GetChild(0)->GetChild(0);
                        pNumCtl->SetNumber(pInfo->nCnt);
                        pBtnSlot->GetChild(0)->GetChild(0)->SetHide(false);
                    }
                    else
                    {
                        pBtnSlot->GetChild(0)->GetChild(0)->SetHide(true);
                    }
                }
            }
        }
        //pUserInfo->SetLastSelectedBlzPageIndex(nSelIndex)
        
        mpItemSlotBar->SetHide(false);
    }
    else if(mStep == TRAININGSTEP_UPGRADE)
    {
        vector<string>  lstImage;
        int nMaxPageCnt = 0;
        const int nMaxItem = MAXSLOTCNT;
        vector<CControl*> lstSlotCtl;
        CUserInfo* pUserInfo = mpScenario->GetUserInfo();
        vector<USERINFO*>  lstType3Org;
        vector<USERINFO*>* lstItem = NULL;
        vector<USERINFO*> lstItemEmpty;
        int nType = 0;
        int nBombID = 0;
        
        if(mpClickedSprite == NULL)
        {
            nType = 1;
            mpItemSlotSwitchBtn->SetEnable(false);
            lstItem = &lstItemEmpty;
        }
        else
        {
            nType = 3;
            if(mpClickedSprite)
            {
                
                lstItem =  mpClickedSprite->GetRuntimeUpgradList();
                if(lstItem == NULL) return;
            }
            mpItemSlotSwitchBtn->SetEnable(false);
        }
        
        mpSelectedWeaponBtn = NULL;
        
        CNumberCtl* pItemPageNumber = (CNumberCtl*)mpItemSlotBar->FindControl(CTL_TABLE_ITEM_SLOT_PAGE);
        
        int nSelIndex = 1;
        if(nType == 1)
            nSelIndex = pUserInfo->GetLastSelectedItemPageIndex();
        else if(nType == 0)
            nSelIndex = pUserInfo->GetLastSelectedBombPageIndex();
        else if(nType == 2)
            nSelIndex = pUserInfo->GetLastSelectedBlzPageIndex();
        
        
        
        lstImage.clear();
        lstImage.push_back("res_slot.png");
        
        if(mpClickedSprite == NULL)
        {
            if(pUserInfo->GetLastBombItemSwitch() == 0)
                lstImage.push_back("M_Slot_Bomb");
            else if(pUserInfo->GetLastBombItemSwitch() == 1)
                lstImage.push_back("M_Slot_Item");
            else if(pUserInfo->GetLastBombItemSwitch() == 2)
                lstImage.push_back("M_Slot_BLZ");
        }
        else
        {
            lstImage.push_back("none");
        }
        mpItemSlotSwitchBtn->SetImageData(lstImage);
        
        int nListSize = (int)lstItem->size();
        if(mpClickedSprite)
        {
            nListSize = 1; //아이템한개만 보이게 하자.
        }
        nMaxPageCnt = (int)nListSize / nMaxItem;
        if((nListSize % nMaxItem) != 0)
            nMaxPageCnt += 1;
        
        if(nSelIndex > nMaxPageCnt)
            nSelIndex = nMaxPageCnt;
        else if( nSelIndex < 1)
            nSelIndex = 1;
        
        pItemPageNumber->SetNumber(nSelIndex);
        
        
        CControl* pUp = mpItemSlotBar->FindControl(BTN_ITEMSLOTUPPAGE);
        CControl* pDown= mpItemSlotBar->FindControl(BTN_ITEMSLOTDOWNPAGE);
        
        
        pDown->SetEnable(false);
        pUp->SetEnable(false);
        
        CButtonCtl* pBtnSlot = NULL;
        
        for(int i = CTL_TABLE_ITEM_SLOT3; i <= CTL_TABLE_ITEM_SLOT6; i++)
        {
            lstSlotCtl.push_back(mpItemSlotBar->FindControl(i));
        }
        
        
        if(nType == 3 && mpClickedSprite->IsMaking()) //지금 짖고 있다면 제거 아이템만 보이게 한다.
        {
            pBtnSlot = (CButtonCtl*)lstSlotCtl[0];
            lstImage.clear();
            lstImage.push_back("RevTower.tga");
            lstImage.push_back("none");
            
            pBtnSlot->GetChild(0)->SetImageData(lstImage);
            pBtnSlot->GetChild(0)->SetBackCoordTex(0.0f, 0.0f, 0.96875f, 0.96875f);
            pBtnSlot->SetLData((long)lstItem->at(0));
            pBtnSlot->SetPushed(false);
            
            pBtnSlot->GetChild(0)->GetChild(1)->SetHide(true); //골드아이콘
            pBtnSlot->GetChild(0)->GetChild(0)->SetHide(true); //골드
            pBtnSlot->GetChild(1)->SetHide(true); //프로그래스
            pBtnSlot->SetButtonType(BUTTON_NORMAL);
            
            for (int i = 1; i < nMaxItem; i++)
            {
                pBtnSlot = (CButtonCtl*)lstSlotCtl[i];
                lstImage.clear();
                lstImage.push_back("none");
                lstImage.push_back("none");
                pBtnSlot->GetChild(0)->SetImageData(lstImage);
                pBtnSlot->GetChild(0)->GetChild(0)->SetHide(true); //골드
                pBtnSlot->GetChild(0)->GetChild(1)->SetHide(true); //골드아이콘
                pBtnSlot->GetChild(1)->SetHide(true); //프로그래스
                pBtnSlot->SetLData(-1);
                
                pBtnSlot->SetButtonType(BUTTON_NORMAL);
                pBtnSlot->SetPushed(false);
                pBtnSlot->SetEnable(false);
            }
            return ;
        }
        
        int nIndex = 0;
        //int nMaxCnt = (int)nListSize;
        string sPath;
        
        for (int i = 0; i < nMaxItem; i++)
        {
            nIndex = (nSelIndex - 1) * nMaxItem + i;
            pBtnSlot = (CButtonCtl*)lstSlotCtl[i];
            
            if(nIndex >= nListSize || nListSize == 0)
            {
                lstImage.clear();
                lstImage.push_back("none");
                lstImage.push_back("none");
                pBtnSlot->GetChild(0)->SetImageData(lstImage);
                pBtnSlot->GetChild(0)->GetChild(0)->SetHide(true); //골드
                pBtnSlot->GetChild(0)->GetChild(1)->SetHide(true); //골드아이콘
                pBtnSlot->GetChild(1)->SetHide(true); //프로그래스
                pBtnSlot->SetLData(-1);
                
                pBtnSlot->SetButtonType(BUTTON_NORMAL);
                pBtnSlot->SetPushed(false);
                pBtnSlot->SetEnable(false);
            }
            else
            {
                pBtnSlot->SetEnable(true);
                if(nType == 1)
                {
                    int nID = lstItem->at(nIndex)->nID;
                    PROPERTY_ITEM property;
                    if(PROPERTY_ITEM::GetPropertyItem(nID, property) == NULL) continue;
                    
                    lstImage.clear();
                    sPath = property.sModelPath;
                    sPath.append(".tga");
                    lstImage.push_back(sPath);
                    lstImage.push_back("none");
                    
                    pBtnSlot->GetChild(0)->SetImageData(lstImage);
                    pBtnSlot->GetChild(0)->SetBackCoordTex(0.0f, 0.0f, 0.96875f, 0.96875f);
                    pBtnSlot->GetChild(0)->GetChild(1)->SetHide(true);
                    pBtnSlot->GetChild(1)->SetHide(true); //프로그래스
                    
                    pBtnSlot->SetLData((long)lstItem->at(nIndex));
                    pBtnSlot->SetPushed(false);
                    
                    pBtnSlot->SetButtonType(BUTTON_NORMAL);
                    
                    if(property.nType == 0) //Count
                    {
                        CNumberCtl* pNumCtl = (CNumberCtl*)lstSlotCtl[i]->GetChild(0)->GetChild(0);
                        pNumCtl->SetNumber(pUserInfo->GetCntByIDWithItem(nID));
                        pBtnSlot->GetChild(0)->GetChild(0)->SetHide(false);
                    }
                    else if(property.nType == 2) //Upgrade
                    {
                        PROPERTY_TANK property;
                        PROPERTY_TANK::GetPropertyTank(pUserInfo->GetLastSelectedTankID(),property);
                        if(nID == ID_ITEM_UPGRADE_ATTACK)
                        {
                            CNumberCtl* pNumCtl = (CNumberCtl*)pBtnSlot->GetChild(0)->GetChild(0);
                            int nUpgrade = pUserInfo->GetAttackUpgrade() * 5.f; //100으로 처리하자.
                            pNumCtl->SetNumber(nUpgrade,CNumberCtlType_Percent);
                            pBtnSlot->GetChild(0)->GetChild(0)->SetHide(false);
                            
                        }
                        else if(nID == ID_ITEM_UPGRADE_DEPEND)
                        {
                            CNumberCtl* pNumCtl = (CNumberCtl*)pBtnSlot->GetChild(0)->GetChild(0);
                            int nUpgrade = pUserInfo->GetDependUpgrade() * 5.f; //100으로 처리하자.
                            pNumCtl->SetNumber(nUpgrade,CNumberCtlType_Percent);
                            pBtnSlot->GetChild(0)->GetChild(0)->SetHide(false);
                            
                        }
                        else if(nID == ID_ITEM_UPGRADE_DEFENCE_TOWER || nID == ID_ITEM_UPGRADE_MISSILE_TOWER)
                        {
                            CNumberCtl* pNumCtl = (CNumberCtl*)lstSlotCtl[i]->GetChild(0)->GetChild(0);
                            int nUpgrade = pUserInfo->GetCntByIDWithItem(nID) * 50; //100으로 처리하자.
                            pNumCtl->SetNumber(nUpgrade,CNumberCtlType_Percent);
                            lstSlotCtl[i]->GetChild(0)->GetChild(0)->SetHide(false);
                        }
                        else
                        {
                            //pNumCtl->ClearNumber();
                            pBtnSlot->GetChild(0)->GetChild(0)->SetHide(true);
                        }
                    }
                    else
                    {
                        //                pNumCtl->ClearNumber();
                        pBtnSlot->GetChild(0)->GetChild(0)->SetHide(true);
                    }
                }
                else if(nType == 0) //Bomb
                {
                    int nID = lstItem->at(nIndex)->nID;
                    PROPERTY_BOMB property;
                    PROPERTY_BOMB::GetPropertyBomb(nID, property);
                    
                    lstImage.clear();
                    lstImage.push_back(property.sBombTableImgPath);
                    lstImage.push_back("none");
                    pBtnSlot->GetChild(0)->SetImageData(lstImage);
                    //폭탄이미지는 0.0,0.0,1.0,1.0 이 아니다.
                    //0.2265625 => 0.1265625 (비율때문)
                    //0.7734375 => 0.8734375 (비율때문)
                    pBtnSlot->GetChild(0)->SetBackCoordTex(0.20703125, 0.1265625, 0.79296875, 0.8734375);
                    pBtnSlot->GetChild(0)->GetChild(1)->SetHide(true);
                    pBtnSlot->GetChild(1)->SetHide(true); //프로그래스
                    
                    pBtnSlot->SetLData((long)lstItem->at(nIndex));
                    pBtnSlot->SetButtonType(BUTTON_GROUP_TOGGLE);
                    
                    pBtnSlot->SetLData2(property.nType); //카운터.. 비카운터
                    if(nBombID == nID)
                    {
                        pBtnSlot->SetPushed(true);
                        mpSelectedWeaponBtn = pBtnSlot;
                    }
                    else
                        pBtnSlot->SetPushed(false);
                    
                    
                    if(property.nType == 0) //Count
                    {
                        CNumberCtl* pNumCtl = (CNumberCtl*)lstSlotCtl[i]->GetChild(0)->GetChild(0);
                        pNumCtl->SetNumber(pUserInfo->GetCntByIDWithBomb(nID));
                        pBtnSlot->GetChild(0)->GetChild(0)->SetHide(false);
                    }
                    else
                    {
                        pBtnSlot->GetChild(0)->GetChild(0)->SetHide(true);
                    }
                }
                else if(nType == 2) //건물
                {
                    USERINFO* pInfo = lstItem->at(nIndex);
                    int nID = pInfo->nID;
                    PROPERTY_TANK property;
                    PROPERTY_TANK::GetPropertyTank(nID, property);
                    
                    lstImage.clear();
                    sPath = property.sModelPath;
                    sPath.append(".tga");
                    lstImage.push_back(sPath);
                    lstImage.push_back("none");
                    
                    pBtnSlot->GetChild(0)->SetImageData(lstImage);
                    pBtnSlot->GetChild(0)->SetBackCoordTex(0.0f, 0.0f, 0.96875f, 0.96875f);
                    pBtnSlot->SetLData((long)pInfo);
                    pBtnSlot->SetPushed(false);
                    
                    pBtnSlot->SetButtonType(BUTTON_NORMAL);
                    
                    if(pInfo->nType == 0) //Count
                    {
                        CNumberCtl* pNumCtl = (CNumberCtl*)lstSlotCtl[i]->GetChild(0)->GetChild(0);
                        pNumCtl->SetNumber(pInfo->nCnt);
                        pBtnSlot->GetChild(0)->GetChild(0)->SetHide(false);
                    }
                    else
                    {
                        pBtnSlot->GetChild(0)->GetChild(0)->SetHide(true);
                    }
                    
                    pBtnSlot->GetChild(0)->GetChild(1)->SetHide(false); //골드아이콘
                    pBtnSlot->GetChild(1)->SetHide(true); //프로그래스
                    
                }
                else if(nType == 3)
                {
                    USERINFO* pInfo = lstItem->at(nIndex);
                    RUNTIME_UPGRADE* pProperty = (RUNTIME_UPGRADE*)pInfo->pProp;
                    lstImage.clear();
                    sPath = pProperty->sPath;
                    lstImage.push_back(pProperty->sPath);
                    lstImage.push_back("none");
                    
                    pBtnSlot->GetChild(0)->SetImageData(lstImage);
                    pBtnSlot->GetChild(0)->SetBackCoordTex(0.0f, 0.0f, 0.96875f, 0.96875f);
                    pBtnSlot->SetLData((long)pInfo);
                    pBtnSlot->SetPushed(false);
                    
                    pBtnSlot->SetButtonType(BUTTON_NORMAL);
                    
                    if(pInfo->nCnt != 0)
                    {
                        CNumberCtl* pNumCtl = (CNumberCtl*)lstSlotCtl[i]->GetChild(0)->GetChild(0);
                        pNumCtl->SetNumber(pInfo->nCnt);
                        pBtnSlot->GetChild(0)->GetChild(0)->SetHide(false); //Gold Number
                        pBtnSlot->GetChild(0)->GetChild(1)->SetHide(false); //Gold Icon
                        
                        if(pProperty->nUpgreadTime == -1)
                        {
                            pBtnSlot->GetChild(1)->SetHide(true); //프로그래스 완성되어 있다면 보여지지 말자.
                            
                            
                            RUNTIME_UPGRADE* pRunTime = (RUNTIME_UPGRADE*)pInfo->pProp;
                            if(pRunTime)
                            {
                                if(pRunTime->nUpgradeCnt >= pRunTime->nUpgradeMaxCnt)
                                {
                                    //안보이게 한다.
                                    pBtnSlot->GetChild(0)->GetChild(0)->SetHide(true); //돈
                                    pBtnSlot->GetChild(0)->GetChild(1)->SetHide(true); //골드아이콘
                                }
                            }
                            
                        }
                        else
                        {
                            CProgressCtl* pro = (CProgressCtl*)pBtnSlot->GetChild(1);
                            pro->SetPosition((float)pProperty->nUpgreadTime / (float)pProperty->nTime * 100.f);
                            pBtnSlot->GetChild(1)->SetHide(false); //프로그래스
                            pBtnSlot->SetEnable(false); //다시클릭되지 않게 막아준다
                        }
                    }
                    else
                    {
                        pBtnSlot->GetChild(0)->GetChild(0)->SetHide(true);
                        pBtnSlot->GetChild(0)->GetChild(1)->SetHide(true);
                        pBtnSlot->GetChild(1)->SetHide(true); //프로그래스
                    }
                }
            }
        }
        
        if(nType == 1)
            pUserInfo->SetLastSelectedItemPageIndex(nSelIndex);
        else if(nType == 0)
            pUserInfo->SetLastSelectedBombPageIndex(nSelIndex);
        else if(nType == 2)
            pUserInfo->SetLastSelectedBlzPageIndex(nSelIndex);
    }
}

#ifdef ANDROID
void CTrainingWorld::ResetTexture()
{
    CHWorld::ResetTexture();
    mnFingerTexture = GetSGLCore()->GetTextureMan()->GetTextureID("Finger.tga");
    
    if(mpMessageDlg) mpMessageDlg->ResetTexture();
    if(mpStepMessageDlg) mpStepMessageDlg->ResetTexture();
}
#endif



#else
//
//
//
//
//
//#define INDEX_STARTPOS 0
//#define INDEX_MOVEPOS  1
//#define INDEX_ATTACKPOS 2
//#define INDEX_ATTACKPOS2 3
//#define INDEX_ATTACKPOS3 6
//#define INDEX_ATTACKPOS4 4
//#define INDEX_MAINTOWER5 5
//
//
//
//CTrainingWorld::CTrainingWorld(CScenario* pScenario) : CHWorld(pScenario)
//{
//    mpMessageDlg = NULL;
//    mpStepMessageDlg = NULL;
//    mnNextTime = 0;
//    mpStepMessageDes = NULL;
//    mpStepMessageDes2 = NULL;
//    mpStepMessageTitle = NULL;
//    mpMoveTargetArrowSprite = NULL;
//    mEnamySprite = NULL;
//    mEnamySprite2= NULL;
////    mpCarmeraRotationTartketPoint = NULL;
//    mpRaidaFingerVertex = NULL;
//    mpArrowBlz = NULL;
//    
//    mMoveMotionStep = 0;
//}
//
//CTrainingWorld::~CTrainingWorld()
//{
//    mpStepMessageTitle = NULL;
//    mpStepMessageDes = NULL;
//    mpStepMessageDes2 = NULL;
//    
//    if(mpMessageDlg)
//    {
//        delete mpMessageDlg;
//        mpMessageDlg = NULL;
//    }
//    
//    if(mpStepMessageDlg)
//    {
//        delete mpStepMessageDlg;
//        mpStepMessageDlg = NULL;
//    }
//    
//    if(mpArrowBlz)
//    {
//        delete mpArrowBlz;
//        mpArrowBlz = NULL;
//    }
//    
//    SetRotationCarmeraToPoint(NULL);
//
//    if(mpRaidaFingerVertex)
//    {
//        delete[] mpRaidaFingerVertex;
//        mpRaidaFingerVertex= NULL;
//    }
//    
//}
//
//int CTrainingWorld::Initialize2(SGLEvent *pEvent)
//{
//    CHWorld::Initialize2(pEvent);
//    mStep = TRAININGSTEP_NONE;
//    return E_SUCCESS;
//}
//
//int CTrainingWorld::Initialize3(SGLEvent *pEvent)
//{
//    CHWorld::Initialize3(pEvent);
//    mpItemSlotBar->SetHide(true);
//    mpRadaBoard->SetHide(true);
//    mpSelectedObjectDescBar->SetHide(true);
//    mnFingerTexture = GetSGLCore()->GetTextureMan()->GetTextureID("Finger.tga");
//    mfFingerUpDwonAction = 15.f;
//    mfFingerUpDwonActionDirection = -1.f;
//    
//    return E_SUCCESS;
//}
//
//
//void CTrainingWorld::MakeActorAtStartPostion()
//{
//
//    int nv = INDEX_STARTPOS; //무조건 0부터 시작한다.
//    CUserInfo* pUserInfo = 0;
//    pUserInfo = mpScenario->GetUserInfo();
//    STARTPOINT *pPoint = m_pSGLCore->mlstStartPoint[nv];
//    
//    CSprite* pSprite = CSGLCore::MakeSprite(GetNewID(),pPoint->nTeamID,pPoint->nModelID,GetTextureMan(),m_pSGLCore->GetAction(),this);
//    if(pSprite)
//    {
//        SVector vtDir;
//        vtDir.x = pPoint->fDirX;
//        vtDir.y = pPoint->fDirY;
//        vtDir.z = pPoint->fDirZ;
//        
//        SPoint  ptPos;
//        ptPos.x = pPoint->fPosX;
//        ptPos.y = pPoint->fPosY;
//        ptPos.z = pPoint->fPosZ;
//        
//        //GetPositionZ(&ptPos);
//        pSprite->LoadResoure(GetModelMan());
//        pSprite->Initialize(&ptPos, &vtDir);
//        m_pSGLCore->AddSprite(pSprite);
//        SetActor(pSprite); //주인공이다.
//        pSprite->SetCameraType(CAMERT_BACK); //실제로 카메라 타입을 셋한다.
//        
//        vector<USERINFO*>*lstBomb = pUserInfo->GetListBomb();
//        lstBomb->erase(lstBomb->begin()); //기본탄을 제거하고
//        pUserInfo->AddListToBomb(6,0,-1); //분산타2을 추가해준다.
//        int nBombSize = (int)lstBomb->size();
//        
//        if(nBombSize > 0)
//        {
//            CK9Sprite *pSP = dynamic_cast<CK9Sprite*>(pSprite);
//            if(pSP)
//                pSP->SetCurrentSelMissaileInfo((*lstBomb)[nBombSize - 1]);
//        }
//    }
//}
//
//void CTrainingWorld::SendMakeBlzMessage()
//{
//    //건물을 짖는다.
//    float* fNew = new float[3];
//    memcpy(fNew,mpStick->GetFlyPosition(),sizeof(float)*3);
//    if(fNew[0] != -9999.f)
//    {
//        USERINFO* pInfo = (USERINFO*)mpBlzSelectedIcon->GetLData();
//        if(pInfo->nCnt > 0)
//            CScenario::SendMessage(SGL_MAKE_BLZ,pInfo->nID,GetActor()->GetTeamID(),0,(long)fNew);
//    }
//}
//
//void CTrainingWorld::OnNextStep(enum TRAININGSTEP CurrentStep)
//{
//    unsigned long ggTime = GetGGTime();
//    
//    if(CurrentStep == TRAININGSTEP_NONE) //None이 끝나면 IntroDuce를 해준다.
//    {
//        mStep = TRAININGSTEP_INTRODUCE; //이동에 관련한 미션을 수행합니다.
//        Setp1_IntroduceDlg();
//        
//#ifdef MAC
//        mnNextTime = ggTime + 9000000.f;
//#else
//        mnNextTime = ggTime + 30000.f;
//#endif
//    }
//    else
//    {
//#ifdef MAC
//        mnNextTime = ggTime + 600000.f;
//#else
//        mnNextTime = ggTime + 600000.f;
//#endif
//        CUserInfo* pUserInfo = mpScenario->GetUserInfo();
//
//        
//        
//        if(CurrentStep == TRAININGSTEP_INTRODUCE)
//        {
//            Setp2_MoveDlg();
//        }
//        else if(CurrentStep == TRAININGSTEP_MOVE)
//        {
//            GetSGLCore()->PlaySystemSound(SOUND_ID_EnergyUP);
//            if(!pUserInfo->IsCompletedMap(pUserInfo->GetLastSelectedMapID()))
//                CScenario::SendMessage(SGL_MESSAGE_MAKE_SCORE,10000*1000);
//            Setp3_Attack();
//        }
//        else if(CurrentStep == TRAININGSTEP_ATTACK)
//        {
//            GetSGLCore()->PlaySystemSound(SOUND_ID_EnergyUP);
//            if(!pUserInfo->IsCompletedMap(pUserInfo->GetLastSelectedMapID()))
//                CScenario::SendMessage(SGL_MESSAGE_MAKE_SCORE,10000*1000);
//            Setp5_AttackRadia();
//        }
//        else if(CurrentStep == TRAININGSTEP_ATTACKRADIA)
//        {
//            GetSGLCore()->PlaySystemSound(SOUND_ID_EnergyUP);
//            Setp5_1_ChangeSlot();
//        }
//        else if(CurrentStep == TRAININGSTEP_CHANGE_SLOT)
//        {
//            Setp6_MakeTower();
//            GetSGLCore()->PlaySystemSound(SOUND_ID_EnergyUP);
//        }
//        else if(CurrentStep == TRAININGSTEP_MAKETOWER)
//        {            
//            GetSGLCore()->PlaySystemSound(SOUND_ID_EnergyUP);
//            if(!pUserInfo->IsCompletedMap(pUserInfo->GetLastSelectedMapID()))
//                CScenario::SendMessage(SGL_MESSAGE_MAKE_SCORE,8000*1000);
//            Setp7_Mission();
//        }
//        else if(CurrentStep == TRAININGSTEP_MISSION)
//        {
//            GetSGLCore()->PlaySystemSound(SOUND_ID_EnergyUP);
//            Setp8_End0();
//        }
//        else if(CurrentStep == TRAININGSTEP_END0)
//        {
//            Setp8_End();
//        }
//
//    }
//}
//
//int CTrainingWorld::RenderBegin()
//{
//    int nTime = 0;
//    
//	//초기화가 안되었거나 초기화중이다.
//	if( (mnStatus & 0x0002) != 0x0002)
//		return E_SUCCESS;
//    
//    mpThread->BeginPong();
//    
//    
//	nTime = GetClockDeltaConst();
//    
//    mpArrowMotionSprite->RenderBegin(nTime);
//    mpStick->RenderBegin(nTime);
//    if(!IsPauseTime())
//        m_pSGLCore->RenderBegin(nTime);
//	mpCurrRuntimeRenderObjects = (RuntimeRenderObjects*)mpThreadQueue->Poll();
//    if(mpCurrRuntimeRenderObjects == 0)
//    {
//        mpThread->EndPong();
//        return E_SUCCESS;
//    }
//	
//    if(!IsPauseTime())
//    {
//        //1> 폭탄을 그린다.
//        ByNode<CBomb>* bb = mpCurrRuntimeRenderObjects->plistBomb->begin();
//        for (ByNode<CBomb>* it = bb; it != mpCurrRuntimeRenderObjects->plistBomb->end(); )
//        {
//            CBomb* Object = it->GetObject();
//            Object->RenderBegin(nTime);
//            it = it->GetNextNode();
//        }
//        
//        //2> 스프라이트를 그린다.
//        ByNode<CSprite>* sb =  mpCurrRuntimeRenderObjects->plistSprite->begin();
//        for(ByNode<CSprite>*it = sb; it != mpCurrRuntimeRenderObjects->plistSprite->end();)
//        {
//            CSprite* Object = it->GetObject();
//            Object->RenderBegin(nTime);
//            it = it->GetNextNode();
//        }
//        
//        //3> 파티클
//        ByNode<AParticle>* ab = mpCurrRuntimeRenderObjects->plistParticle->begin();
//        for(ByNode<AParticle>* it = ab; it != mpCurrRuntimeRenderObjects->plistParticle->end();)
//        {
//            AParticle* Object = it->GetObject();
//            Object->RenderBegin(nTime);
//            it = it->GetNextNode();
//        }
//    }
//    
//    if(mBackground)
//        mBackground->RenderBegin(nTime);
//    
//
//    if(mpRadaBoard && mpRadaBoard->GetHide() == false)
//    {
//        ByNode<RadaBoardObject>* rb = mpCurrRuntimeRenderObjects->plistBoardObjects->begin();
//        for(ByNode<RadaBoardObject>* it = rb; it != mpCurrRuntimeRenderObjects->plistBoardObjects->end();)
//        {
//            RadaBoardObject* Object =  it->GetObject();
//            
//            if(Object && Object->pSprite && Object->pSprite->GetState() == SPRITE_RUN && Object->nBoardType == 1)
//            {
//                if(mpRaidaFingerVertex == NULL)
//                    mpRaidaFingerVertex = new float[12];
//                memcpy(mpRaidaFingerVertex, Object->pWorldVertex, sizeof(float) * 12);
//                float fX = (mpRaidaFingerVertex[6] - mpRaidaFingerVertex[0]) / 2.f; //손가락을 탱크 중앙으로 옮겨준다.
//                float fY = (mpRaidaFingerVertex[1] - mpRaidaFingerVertex[7]) / 2.f; //상단으로 옮겨준다.
//                float matrix[16];
//                sglLoadIdentityf(matrix);
//                sglTranslateMatrixf(matrix, fX, fY, 0.0f);
//                mfRaidaFingerMax = fY * 2;
//                mfRaidaFingerMin = fY;
//                
//                sglMultMatrixVectorf(mpRaidaFingerVertex, matrix,mpRaidaFingerVertex);
//                sglMultMatrixVectorf(mpRaidaFingerVertex+3, matrix,mpRaidaFingerVertex+3);
//                sglMultMatrixVectorf(mpRaidaFingerVertex+6, matrix,mpRaidaFingerVertex+6);
//                sglMultMatrixVectorf(mpRaidaFingerVertex+9, matrix,mpRaidaFingerVertex+9);
//                
//                break;
//            }
//            it = it->GetNextNode();
//        }
//        
//        
//        mpRadaBoard->SetObjects(mpCurrRuntimeRenderObjects->plistBoardObjects);
//        mpRadaBoard->RenderBegin(nTime);
//    }
//    if(mStep == TRAININGSTEP_MAKETOWER)
//    {
// 
//    }
//    
//    mpLeftTools->RenderBegin(nTime);
//    mpItemSlotBar->RenderBegin(nTime);
//    mpParticleMan->RenderBegin(nTime);
//    
//    
//    
//    //시간이 포즈가 되어도 실행되게끔 해주어야 한다.
//    if(mpMissionMsgCtl)
//        mpMissionMsgCtl->RenderBegin(50);
//    
//  //-----------------------------------------------------------------------------
//  //CTrainingWorld 에서 필요한 로직
//  //-----------------------------------------------------------------------------
//    if(mpMessageDlg) mpMessageDlg->RenderBegin(nTime);
//    if(mpStepMessageDlg) mpStepMessageDlg->RenderBegin(nTime);
//   //-----------------------------------------------------------------------------
//    
//    
//    if(mStep == TRAININGSTEP_MOVE)
//    {
//        STARTPOINT* pMoveTargetPostion = m_pSGLCore->mlstStartPoint[INDEX_MOVEPOS];
//        
//        float matrix[16];
//        sglLoadIdentityf(matrix);
//        sglTranslateMatrixf(matrix, pMoveTargetPostion->fPosX, pMoveTargetPostion->fPosY + 1.0f, pMoveTargetPostion->fPosZ);
//        sglScaleMatrixf(matrix, 11.0f, 11.0f, 11.0f);
//        
//        sglMultMatrixVectorf(mMoveTargetGround, matrix,gPanelVertexZ);
//        sglMultMatrixVectorf(mMoveTargetGround+3, matrix,gPanelVertexZ+3);
//        sglMultMatrixVectorf(mMoveTargetGround+6, matrix,gPanelVertexZ+6);
//        sglMultMatrixVectorf(mMoveTargetGround+9, matrix,gPanelVertexZ+9);
//
//        if(mpMoveTargetArrowSprite)
//        {
//            if(GetActor()->CollidesBounding(mpMoveTargetArrowSprite, false)) //충돌되었다. 즉 원하는 위치까지 왔다면 다음스템으로 이동한다.
//            {
//                mpMoveTargetArrowSprite->SetState(SPRITE_READYDELETE);
//                mpMoveTargetArrowSprite = NULL;
//                //공격으로 변환한다.
//                OnNextStep(mStep);
//            }
//        }
//    }
//    
//    RotationCarmeraToPoint(nTime);
//    
//    mpSelectedObjectDescBar->SetHide(true);
//    
//    RenderFingerBegin(nTime);
//    mpThread->EndPong();
//    return E_SUCCESS;
//}
//
//
//int CTrainingWorld::Render()
//{
//    //버퍼를 클리어 해준다.
//	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    glDisable(GL_ALPHA_TEST); //속도가 높아진다는 소리가 있어서 한번해봤다.
//	glFrontFace(GL_CCW);
//	glEnable(GL_CULL_FACE);
//    
//#ifdef ANDROID //Android에서는 매번 셋팅해주어야 한다.
//    //방향벡터의 normalize를 하지 않음. 그래서 gl에서 해주어야 샤이닝이 이러난다.
//    //모델의 빛 반사를 정의 하려면 GL_NORMALIZE화 해주어야 한다.
//    glEnable(GL_NORMALIZE);
//#endif
//    glColor4f(1.0f,1.0f,1.0f,1.0f);	 //glColor4f(1.0f,1.0f,1.0f,alpha);
//	
//	int nCameraType = 0;
//	//초기화가 안되었거나 초기화중이다.
//	if( (mnStatus & 0x0002) != 0x0002 || mpCurrRuntimeRenderObjects == 0) return E_SUCCESS;
//    
//    
//    nCameraType = GetCameraType();
//    
//    //---------------------------------------------------
//    //안개
//    if(m_pSGLCore->m_fFogDentity != 0.0f)
//    {
//        //        GLfloat fcolor[4] = {0.7f, 0.7f, 0.7f, 1.0f};
//        glEnable(GL_FOG);
//#ifdef MAC
//        glFogi(GL_FOG_MODE, GL_LINEAR);
//#else
//        glFogx(GL_FOG_MODE, GL_LINEAR);
//#endif
//        glFogf(GL_FOG_START, m_pSGLCore->m_fFogStart);//150.f);
//        glFogf(GL_FOG_END, m_pSGLCore->m_fFogEnd);//350.f);
//        glFogf(GL_FOG_DENSITY,m_pSGLCore->m_fFogDentity);//2.f);
//        glFogfv(GL_FOG_COLOR, m_pSGLCore->m_fFogColor);
//    }
//    //---------------------------------------------------
//    
//    //하늘에서 내려다 보는 것 아닌 것은 하늘을 그린다.
//	if(nCameraType < CAMERT_UP_VISIBLE)
//    {
//        if(mBackground)
//        {
//            mBackground->ActivateCamera(GetCamera());
//            mBackground->Render();
//        }
//    }
//    
//	ActivateCamera();
//    
//    //1>그리는 순서를 캐릭터를 먼져그리고, 지형을 나중에 그린다.
//    //그렇게 하면 나무같은 곳에 숨겨지기때문에 현실감이 있다.
//    //스프라이트를 그린다.
//#ifdef ANDROID
//    SetLight();
//#endif
//    
////    //---------------------------------------------------
////    //안개
////    if(m_pSGLCore->m_fFogDentity != 0.0f)
////    {
////        //        GLfloat fcolor[4] = {0.7f, 0.7f, 0.7f, 1.0f};
////        glEnable(GL_FOG);
////#ifdef MAC
////        glFogi(GL_FOG_MODE, GL_LINEAR);
////#else
////        glFogx(GL_FOG_MODE, GL_LINEAR);
////#endif
////        glFogf(GL_FOG_START, m_pSGLCore->m_fFogStart);//150.f);
////        glFogf(GL_FOG_END, m_pSGLCore->m_fFogEnd);//350.f);
////        glFogf(GL_FOG_DENSITY,m_pSGLCore->m_fFogDentity);//2.f);
////        glFogfv(GL_FOG_COLOR, m_pSGLCore->m_fFogColor);
////    }
////    //---------------------------------------------------
//    
//    ByNode<CSprite>* sb =  mpCurrRuntimeRenderObjects->plistSprite->begin();
//    for(ByNode<CSprite>*it = sb; it != mpCurrRuntimeRenderObjects->plistSprite->end();)
//	{
//        CSprite* Object = it->GetObject();
//		Object->Render();
//        it = it->GetNextNode();
//	}
//    
//    mpArrowMotionSprite->Render();
//    
//    //2>지형을 그랜다.
//    m_pSGLCore->Render();
//    
//    
//    //----------------------------------------
//    //3> 폭탄을 그린다. 지형에 숨겨지지 않게 그리면 폭탄의 계적이 보여서 좋다.
//    //순서의 반대로 그려준다.
//    ByNode<CBomb>* be = mpCurrRuntimeRenderObjects->plistBomb->end()->GetPrevNode();
//	for (ByNode<CBomb>* it = be; it != &mpCurrRuntimeRenderObjects->plistBomb->GetHead(); )
//    {
//		CBomb* Object = it->GetObject();
//		Object->Render();
//        it = it->GetPrevNode();
//	}
//	
//    
//    //-------------------------------
//    glEnableClientState(GL_VERTEX_ARRAY);
//    
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    glEnable(GL_BLEND);
//    glDisable(GL_CULL_FACE);
//    glDepthMask(GL_FALSE);
//    
//    //Bomb Tracking..
//    mpParticleMan->Render();
//    
//    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
//    glEnable(GL_TEXTURE_2D);
//    
//    
//    if(mStep == TRAININGSTEP_MOVE)
//    {
//        glBindTexture(GL_TEXTURE_2D, m_pSGLCore->GetTextureMan()->GetTextureID(IMG_PARKING));
//        glTexCoordPointer(2, GL_FLOAT, 0, gPanelCoordTex);
//        glVertexPointer(3, GL_FLOAT, 0, mMoveTargetGround);
//        glDrawElements(GL_TRIANGLE_STRIP, gshPanelIndicesCnt, GL_UNSIGNED_SHORT, gPanelIndices);
//    }
//    
//    mpStick->RenderMoveingFly();
//    
//    
//    
//    //3> 파티클
//    ByNode<AParticle>* ab = mpCurrRuntimeRenderObjects->plistParticle->begin();
//	for(ByNode<AParticle>* it = ab; it != mpCurrRuntimeRenderObjects->plistParticle->end();)
//	{
//		AParticle* Object = it->GetObject();
//		Object->Render();
//        it = it->GetNextNode();
//	}
//    
//    RenderFinger();
//    
//    if(m_pSGLCore->m_fFogDentity != 0.0f)
//        glDisable(GL_FOG);
//    
//    //2D는 뎁스를 끄고 그려준다.
//    glDisable(GL_DEPTH_TEST);
//    
//    
//    //-----------------------------------------------------------------------------
//    //4> 스프라이트 에너지 상태를 나타낸다...
//    glDisable(GL_TEXTURE_2D);
//    glEnableClientState(GL_COLOR_ARRAY);
//    for(ByNode<CSprite>*it = sb; it != mpCurrRuntimeRenderObjects->plistSprite->end();)
//	{
//        CSprite* Object = it->GetObject();
//		if(Object->GetState() == SPRITE_RUN && Object->GetSpriteStatus())
//            Object->GetSpriteStatus()->Render();
//        it = it->GetNextNode();
//	}
//    glDisableClientState(GL_COLOR_ARRAY);
//    glEnable(GL_TEXTURE_2D);
//    //-----------------------------------------------------------------------------
//    
//    
//    CControl::ActivateCamera();
//    mpStick->Render();
//    mpRadaBoard->Render();
//    
//    mpLeftTools->Render();
//    
//
//    
//    mpItemSlotBar->Render();
//    
//    if(mpArrowBlz)
//        mpArrowBlz->Render();
//    
//    if(mpRaidaFingerVertex)
//    {
//        glColor4f(1.0f,1.0f,1.0f,1.0f);
//        glBindTexture(GL_TEXTURE_2D, mnFingerTexture);
//        glTexCoordPointer(2, GL_FLOAT, 0, gPanelCoordTex);
//        glVertexPointer(3, GL_FLOAT, 0, mRaidaFingerWorldVertex);
//        glDrawElements(GL_TRIANGLE_STRIP, gshPanelIndicesCnt, GL_UNSIGNED_SHORT, gPanelIndices);
//    }
//    
//
//    mpScoreNumCtl->Render();
//    mpRightTools->Render();
//
//    
//    if(mpMessageDlg) mpMessageDlg->Render();
//    if(mpStepMessageDlg) mpStepMessageDlg->Render();
//    if(mpConfirmMsgCtl) mpConfirmMsgCtl->Render();
//    if(mpPauseMsgCtl) mpPauseMsgCtl->Render();
//    if(mpMissionMsgCtl) mpMissionMsgCtl->Render();
//
//#ifdef MAC
//    RenderDesign();
//#endif
//    
//    glEnable(GL_DEPTH_TEST);
//    glDepthMask(GL_TRUE);
//    glEnable(GL_CULL_FACE);
//    glDisable(GL_BLEND);
//    
//    glDisable(GL_TEXTURE_2D);
//    glDisableClientState(GL_VERTEX_ARRAY);
//    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
//    //-------------------------------
//    
//	return E_SUCCESS;
//}
//
//int CTrainingWorld::RenderEnd()
//{
//    //초기화가 안되었거나 초기화중이다.
//	if( (mnStatus & 0x0002) != 0x0002 || mpCurrRuntimeRenderObjects == 0)
//		return E_SUCCESS;
//    
//    if(mpStepMessageDlg)
//    {
//        mpStepMessageDlg->RenderEnd();
//    }
//    
//    if(mpMessageDlg)
//    {
//        mpMessageDlg->RenderEnd();
//        if(mpMessageDlg->Animating() == CONTROLANI_ZOOM_IN_HIDE_END)
//        {
//            delete mpMessageDlg;
//            mpMessageDlg = NULL;
//        }
//    }
//    
//    unsigned long ggTime = GetGGTime();
//    if(mnNextTime && ggTime > mnNextTime)
//    {
//        if (mStep == TRAININGSTEP_NONE || mStep == TRAININGSTEP_INTRODUCE)
//            OnNextStep(mStep);
//        else if(mStep == TRAININGSTEP_END0)
//            OnNextStep(mStep);
//        else if(mStep == TRAININGSTEP_END)
//        {
//            //프로그램을 종료해준다.
//            CScenario::SendMessage(SGL_MESSAGE_SUCCESS_MISSION,0);
//            mnNextTime = 0;
//        }
//        else
//        {
//            //시간을 상단에 나타나게 하여. 그시간에 안나오면 미션 종료해야한다.
//        }
//    }
//    if(mStep == TRAININGSTEP_NONE)
//    {
//        //2초 후에 인트로 메세지를 보여준다.
//        if(mnNextTime == 0)
//            mnNextTime = ggTime + 700.f;
//    }
//
//	return CHWorld::RenderEnd();
//}
//
//
//void CTrainingWorld::Setp1_IntroduceDlg()
//{
//    if(mpMessageDlg) return;
//    string sText;
//    vector<float> layout;
//    vector<string>  lstImage;
//    layout.clear();
//    float fYellow[] = { 0.0f,0.0f,0.0f,1.0f};
//    
//    PROPERTYI::GetPropertyFloatList("Trn_Introduce_Msg", layout);
//    
//    float fX = (gDisplayWidth - layout[2]) / 2.f + layout[0];
//    float fY = (gDisplayHeight - layout[3]) / 2.f + layout[1];
//    
//    //CellBack
//    lstImage.clear();
//    lstImage.push_back("greenpanel.png");
//    lstImage.push_back("none");
//    mpMessageDlg =  new CLabelCtl(NULL,GetTextureMan());
//    mpMessageDlg->Initialize(0, fX, fY, layout[2], layout[3], lstImage, 0.f, 0.f, 1.f, 1.f);
//
//
//    const char* ArrT[] = {"Trn_Introduce_Msg_T1","Trn_Introduce_Msg_T2","Trn_Introduce_Msg_T3","Trn_Introduce_Msg_T4"};
//    int nCnt = sizeof(ArrT) / sizeof(const char*);
//    for (int i = 0; i < nCnt; i++)
//    {
//        
//        layout.clear();
//        lstImage.clear();
//        
//        PROPERTYI::GetPropertyFloatList(ArrT[i], layout);
//        sText = SGLTextUnit::GetTextAddGLData(ArrT[i],"Trn_Introduce_Msg");
//        lstImage.push_back("none");
//        lstImage.push_back(sText);
//        CLabelCtl *pText = new CLabelCtl(mpMessageDlg,GetTextureMan());
//        pText->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.0, 0.0, 0.0, 0.0);
//        pText->SetTextColor(fYellow);
//        mpMessageDlg->AddControl(pText);
//    }
//    
//    layout.clear();
//    PROPERTYI::GetPropertyFloatList("Trn_Introduce_Msg_GoBtn", layout);
//    lstImage.clear();
//    lstImage.push_back("res_Dialog.png");
//    lstImage.push_back("M_IB_YES");
//    CButtonCtl* btOK =  new CButtonCtl(mpMessageDlg,GetTextureMan());
//    btOK->Initialize(BTN_TRAINING_STEP1_OK, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.09375,0.3828125,0.33203125,0.45703125);
//    mpMessageDlg->AddControl(btOK);
//    
//    
//    mpMessageDlg->SetTranslate(0.8f);
//    mpMessageDlg->SetAni(CONTROLANI_LIN,1.6f);
//    
//#if defined(IPHONE) || defined(ANDROID)
//    mpScenario->SendTrainningStep(1);
//#endif
//}
//
//void CTrainingWorld::Setp2_MoveDlg()
//{    
//    //이동 관련 메세지를 보여준다.
//    mStep = TRAININGSTEP_MOVE; //이동에 관련한 미션을 수행합니다.
//    
//    if(mpStepMessageDlg) return;
//    
//    vector<float> layout;
//    vector<string>  lstImage;
//    
//    if(mpMessageDlg) mpMessageDlg->SetAni(CONTROLANI_ZOOM_IN_HIDE,0.2f);
//    
//    
//    layout.clear();
//    PROPERTYI::GetPropertyFloatList("Trn_Step_Msg", layout);
//    float fX = (gDisplayWidth - layout[2]) / 2.f + layout[0];
//    float fY = layout[1];
//    lstImage.clear();
//    lstImage.push_back("none");
//    lstImage.push_back("none");
//    mpStepMessageDlg =  new CLabelCtl(NULL,GetTextureMan());
//    mpStepMessageDlg->Initialize(0, fX, fY, layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
//    
//    
//    float fRed[] = {1.0f,0.0f,0.0f,1.0f};
//    float fYellow[] = {1.0f,0.0f,1.0f,1.0f};
//    //Title
//    layout.clear();
//    PROPERTYI::GetPropertyFloatList("Trn_Step_Msg_Title", layout);
//    lstImage.clear();
//    lstImage.push_back("none");
//    lstImage.push_back("Tran_Step1_MoveT");
//    CLabelCtl* pTitle =  new CLabelCtl(mpStepMessageDlg,GetTextureMan());
//    pTitle->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
//    pTitle->SetTextColor(fRed);
//    mpStepMessageDlg->AddControl(pTitle);
//    mpStepMessageTitle = pTitle;
//
//    //Description
//    layout.clear();
//    PROPERTYI::GetPropertyFloatList("Trn_Step_Msg_Desc", layout);
//    lstImage.clear();
//    lstImage.push_back("none");
//    lstImage.push_back("Tran_Step1_MoveDes1");
//    CLabelCtl* pDescription =  new CLabelCtl(mpStepMessageDlg,GetTextureMan());
//    pDescription->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
//    pDescription->SetTextColor(fYellow);
//    mpStepMessageDlg->AddControl(pDescription);
//    mpStepMessageDes = pDescription;
//    
//    
//    float fHe = layout[3] + 4;
//    
//    lstImage.clear();
//    lstImage.push_back("none");
//    lstImage.push_back("none");
//    CLabelCtl* pDescription2 =  new CLabelCtl(mpStepMessageDlg,GetTextureMan());
//    pDescription2->Initialize(0, fX + layout[0], fY + layout[1] + fHe, layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
//    pDescription2->SetTextColor(fYellow);
//    mpStepMessageDlg->AddControl(pDescription2);
//    mpStepMessageDes2 = pDescription2;
//
//    
//    
//    mpStepMessageDlg->SetTranslate(0.8f);
//    mpStepMessageDlg->SetAni(CONTROLANI_TIN,0.8f);
//    
//    
////    CSprite* pActor = GetActor();
//    //Added Target Arrow
//    mpMoveTargetArrowSprite = new CArrowMotionSprite(GetNewID(),SGL_TEAMALL,ACTORID_ARROWMOTION_TRAIN,(IAction*)this,this);
//    mpMoveTargetArrowSprite->LoadResoure(m_pSGLCore->GetModelMan());
//    mpMoveTargetArrowSprite->Initialize(NULL,NULL);
//    STARTPOINT* pMoveTargetPostion = m_pSGLCore->mlstStartPoint[INDEX_MOVEPOS];
//    SPoint ptT;
//    ptT.x = pMoveTargetPostion->fPosX;
//    ptT.y = pMoveTargetPostion->fPosY;
//    ptT.z = pMoveTargetPostion->fPosZ;
//    mpMoveTargetArrowSprite->SetPosition(&ptT);
//    m_pSGLCore->AddSprite(mpMoveTargetArrowSprite);
//    
//    SetRotationCarmeraToPoint(&ptT);
//#if defined(IPHONE) || defined(ANDROID)
//    mpScenario->SendTrainningStep(2);
//#endif
//
//}
//
//void CTrainingWorld::Setp3_Attack()
//{
//    mStep = TRAININGSTEP_ATTACK;
//    SetStepMessage("Tran_Step2_AttackT","Tran_Step2_AttackDes1");
//    
//    //움직임을 멈춘다.
//    GetActor()->SetMoveVelocity(0.0f);
//    GetActor()->SetRotationVelocity(0.0f);
//    
//    
//    SVector vDir;
//    SPoint ptNow;
//    
//    PROPERTY_TANK property;
//    //Tank2
//    mEnamySprite2 = CSGLCore::MakeSprite(GetNewID(), SGL_TEAM2, ACTORID_ET1,m_pSGLCore->GetTextureMan(),m_pSGLCore->GetAction(), this);
//    mEnamySprite2->LoadResoure(m_pSGLCore->GetModelMan());
//    
//    vDir.x = -1;
//    vDir.y = 0;
//    vDir.z = -1;
//    sglNormalize(&vDir);
//    
//    STARTPOINT* pPos = m_pSGLCore->mlstStartPoint[INDEX_ATTACKPOS2];
//    ptNow.x = pPos->fPosX;
//    ptNow.y = pPos->fPosY;
//    ptNow.z = pPos->fPosZ;
//    
//    mEnamySprite2->Initialize(&ptNow, &vDir);
//    mEnamySprite2->DeleteAICore();
//    mEnamySprite2->SetCurrentDefendPower(0.f);
//    m_pSGLCore->AddSprite(mEnamySprite2);
//    
//
//    //Tank1
//    mEnamySprite = CSGLCore::MakeSprite(GetNewID(), SGL_TEAM2, ACTORID_ET1,m_pSGLCore->GetTextureMan(),m_pSGLCore->GetAction(), this);
//    mEnamySprite->LoadResoure(m_pSGLCore->GetModelMan());
//    
//    vDir.x = 1;
//    vDir.y = 0;
//    vDir.z = 1;
//    sglNormalize(&vDir);
//    
////    pPos = m_pSGLCore->mlstStartPoint[INDEX_ATTACKPOS]; 사용하지 않고 INDEX_ATTACKPOS2 을 기준으로 그린다.
//    ptNow.x = pPos->fPosX + 12.f;
//    ptNow.y = pPos->fPosY;
//    ptNow.z = pPos->fPosZ + 12.f;
//    GetPositionY(&ptNow);
//    
//    mEnamySprite->Initialize(&ptNow, &vDir);
//    mEnamySprite->SetCurrentDefendPower(0.f);
//    mEnamySprite->DeleteAICore();
//    m_pSGLCore->AddSprite(mEnamySprite);
//
//    
//    
////    //Tank1
////    mEnamySprite = CSGLCore::MakeSprite(GetNewID(), SGL_TEAM2, ACTORID_ET1,m_pSGLCore->GetTextureMan(),m_pSGLCore->GetAction(), this);
////    mEnamySprite->LoadResoure(m_pSGLCore->GetModelMan());
////    
////    vDir.x = -1;
////    vDir.y = 0;
////    vDir.z = -1;
////    sglNormalize(&vDir);
////    
////    pPos = m_pSGLCore->mlstStartPoint[INDEX_ATTACKPOS];
////    ptNow.x = pPos->fPosX;
////    ptNow.y = pPos->fPosY;
////    ptNow.z = pPos->fPosZ;
////    
////    mEnamySprite->Initialize(&ptNow, &vDir);
////    mEnamySprite->DeleteAICore();
////    m_pSGLCore->AddSprite(mEnamySprite);
//    
//    
//    
//    
//    
//    SetRotationCarmeraToPoint(&ptNow);
//#if defined(IPHONE) || defined(ANDROID)
//    mpScenario->SendTrainningStep(3);
//#endif
//}
//
//
//void CTrainingWorld::Setp5_AttackRadia()
//{
//    mEnamySprite = NULL;
//    mEnamySprite2 = NULL;
//    mStep = TRAININGSTEP_ATTACKRADIA;
//    
//    SetStepMessage(NULL,"Tran_Step2_AttackDes2");
//    
//    SVector vDir;    SPoint ptNow;
//    
//    
//    PROPERTY_TANK property;
//    
//    mEnamySprite = CSGLCore::MakeSprite(GetNewID(), SGL_TEAM2, ACTORID_ET1,m_pSGLCore->GetTextureMan(),m_pSGLCore->GetAction(), this);
//    mEnamySprite->LoadResoure(m_pSGLCore->GetModelMan());
//    
//    
//    vDir.x = -1;
//    vDir.y = 0;
//    vDir.z = -1;
//    sglNormalize(&vDir);
//    
//    STARTPOINT* pPos = m_pSGLCore->mlstStartPoint[INDEX_ATTACKPOS3];
//    ptNow.x = pPos->fPosX;
//    ptNow.y = pPos->fPosY;
//    ptNow.z = pPos->fPosZ;
//    
//    mEnamySprite->Initialize(&ptNow, &vDir);
//    mEnamySprite->DeleteAICore();
//    mEnamySprite->SetCurrentDefendPower(0.f);
//    m_pSGLCore->AddSprite(mEnamySprite);
//    
//    mpRadaBoard->SetHide(false);
//    
//    SetRotationCarmeraToPoint(&ptNow);
//#if defined(IPHONE) || defined(ANDROID)
//    mpScenario->SendTrainningStep(4);
//#endif
//    
//}
//
//void CTrainingWorld::Setp5_1_ChangeSlot()
//{
//    mStep = TRAININGSTEP_CHANGE_SLOT;
//    SetStepMessage("Tran_Step2_ChSlotT","Tran_Step2_ChSlotTDes1");
//    
//    //움직임을 멈춘다.
//    GetActor()->SetMoveVelocity(0.0f);
//    GetActor()->SetRotationVelocity(0.0f);
//    
//    GetUserInfo()->SetLastBombItemSwitch(2);
//    
//    //슬롯을 보여준다.
//    ResetDisplayItemSlot();
//    
//    CControl* ctl = mpItemSlotSwitchBtn;
//    ctl->SetAni(CONTROLANI_WIGWAG,0.3f);
//    
//
//    float fPX,fPY,szW,szH;
//    ctl->GetPosition(fPX, fPY);
//    ctl->GetSize(szW, szH);
//    szH *= 0.5f;
//    vector<string> lstImage;
//    lstImage.push_back("none");
//    lstImage.push_back("none");
//    CLabelCtl tmpCtl(NULL,GetTextureMan());
//    tmpCtl.Initialize(0, fPX + szW/2.f, fPY - szH/2.f, szH, szH*2.f, lstImage, 0.f, 0.f, 0.f, 0.f);
//   
//    if(mpRaidaFingerVertex == NULL)
//        mpRaidaFingerVertex = new float[12];
//    memcpy(mpRaidaFingerVertex, tmpCtl.GetWorldVertex(), sizeof(float) * 12);
//    float fY = (tmpCtl.GetWorldVertex()[1] - tmpCtl.GetWorldVertex()[7]) / 2.f; //상단으로 옮겨준다.
//    mfRaidaFingerMax = fY * 2;
//    mfRaidaFingerMin = fY;
//    
//#if defined(IPHONE) || defined(ANDROID)
//    mpScenario->SendTrainningStep(5);
//#endif
//}
//
//
//
//
//
//
//void CTrainingWorld::Setp6_MakeTower()
//{
//    mStep = TRAININGSTEP_MAKETOWER;
//    mEnamySprite = NULL;
//    mEnamySprite2 = NULL;
//    
//    //메세지 정의해야 한다.
//    SetStepMessage("Tran_Step6_MakeTowerT","Tran_Step6_MakeTowerDes1");
//    SPoint ptNow;
//    PROPERTY_TANK property;
//    
//    STARTPOINT* pPos = m_pSGLCore->mlstStartPoint[INDEX_ATTACKPOS2];
//    ptNow.x = pPos->fPosX;
//    ptNow.y = pPos->fPosY;
//    ptNow.z = pPos->fPosZ;
//    SetRotationCarmeraToPoint(&ptNow);
// 
//    ResetDisplayItemSlot();
//    
//    CControl* ctl = mpItemSlotBar->FindControl(CTL_TABLE_ITEM_SLOT3);
//    ctl->SetAni(CONTROLANI_WIGWAG,0.3f);
//    
//    CUserInfo* pUserInfo = mpScenario->GetUserInfo();
//    pUserInfo->SetLastBombItemSwitch(2);
//    
//    
//    
//    //손가락
//    float fPX,fPY,szW,szH;
//    ctl->GetPosition(fPX, fPY);
//    ctl->GetSize(szW, szH);
//    szH *= 0.5f;
//    vector<string> lstImage;
//    lstImage.push_back("none");
//    lstImage.push_back("none");
//    CLabelCtl tmpCtl(NULL,GetTextureMan());
//    tmpCtl.Initialize(0, fPX + szW/2.f, fPY - szH/2.f, szH, szH*2.f, lstImage, 0.f, 0.f, 0.f, 0.f);
//    
//    if(mpRaidaFingerVertex == NULL)
//        mpRaidaFingerVertex = new float[12];
//    memcpy(mpRaidaFingerVertex, tmpCtl.GetWorldVertex(), sizeof(float) * 12);
//    float fY = (tmpCtl.GetWorldVertex()[1] - tmpCtl.GetWorldVertex()[7]) / 2.f; //상단으로 옮겨준다.
//    mfRaidaFingerMax = fY * 2;
//    mfRaidaFingerMin = fY;
//    
//    
//    
//    //방향
//    lstImage.clear();
//    if(gnDisplayType == DISPLAY_IPHONE)
//        lstImage.push_back("TrainingArrow1.tga");
//    else
//        lstImage.push_back("TrainingArrow.tga");
//    lstImage.push_back("none");
//    
//    vector<float> layout;
//    PROPERTYI::GetPropertyFloatList("Trn_ArrowBlz", layout);
//    mpArrowBlz = new CLabelCtl(mpLeftTools,GetTextureMan());
//    szW = layout[2];
//    szH = layout[3];
//    
//    if(gnDisplayType == DISPLAY_IPHONE)
//        mpArrowBlz->Initialize(0, fPX - szW/2.f, fPY + szH/2.f, szW, szH, lstImage, 0.f, 0.f, 1.f, 1.f);
//    else
//        mpArrowBlz->Initialize(0, fPX + szW/2.f, fPY - szH/2.f, szW, szH, lstImage, 0.f, 0.f, 1.f, 1.f);
//    mpArrowBlz->SetEnable(false);
//    
//    
//    
//#if defined(IPHONE) || defined(ANDROID)
//    mpScenario->SendTrainningStep(6);
//#endif
//}
//
//void CTrainingWorld::Setp7_Mission()
//{
//    mStep = TRAININGSTEP_MISSION;
//    mEnamySprite = NULL;
//    mEnamySprite2= NULL;
//    
//    SetStepMessage("Tran_Step3_MissionT","Tran_Step3_MissionDes1");
//    SVector vDir;
//    SPoint ptNow;
//    PROPERTY_TANK property;
//    
//    mEnamySprite = CSGLCore::MakeSprite(GetNewID(), SGL_TEAM2, ACTORID_BLZ_EMAIN,m_pSGLCore->GetTextureMan(),m_pSGLCore->GetAction(), this);
//    mEnamySprite->LoadResoure(m_pSGLCore->GetModelMan());
//    
//    
//    vDir.x = -1;
//    vDir.y = 0;
//    vDir.z = -1;
//    sglNormalize(&vDir);
//    
//    STARTPOINT* pPos = m_pSGLCore->mlstStartPoint[INDEX_ATTACKPOS4];
//    ptNow.x = pPos->fPosX;
//    ptNow.y = pPos->fPosY;
//    ptNow.z = pPos->fPosZ;
//    
//    mEnamySprite->Initialize(&ptNow, &vDir);
//    mEnamySprite->DeleteAICore();
//    m_pSGLCore->AddSprite(mEnamySprite);
//    
//    SetRotationCarmeraToPoint(&ptNow);
//#if defined(IPHONE) || defined(ANDROID)
//    mpScenario->SendTrainningStep(7);
//#endif
//}
//
//
//
//
//
//void CTrainingWorld::Setp8_End0()
//{
//    mStep = TRAININGSTEP_END0;
//    mEnamySprite = NULL;
//    mEnamySprite2= NULL;
//    
//    SetStepMessage("Tran_Step3_END","Tran_Step3_ENDDes1");
//    SVector vDir;
//    SPoint ptNow;
//    PROPERTY_TANK property;
//    
//    
//    STARTPOINT* pPos = NULL;
//    float fRadias = 15.f * 2.f;
//
//    //가드타워를 만든다.
//    pPos = m_pSGLCore->mlstStartPoint[INDEX_ATTACKPOS];
//    ptNow.x = pPos->fPosX;
//    ptNow.y = pPos->fPosY;
//    ptNow.z = pPos->fPosZ;
//    
//    SetRotationCarmeraToPoint(&ptNow);
//    
//    //아군 메인타워를 건설한다.
//    mEnamySprite = CSGLCore::MakeSprite(GetNewID(), SGL_TEAM1, ACTORID_BLZ_MAINTOWER,m_pSGLCore->GetTextureMan(),m_pSGLCore->GetAction(), this);
//    mEnamySprite->LoadResoure(m_pSGLCore->GetModelMan());
//    mEnamySprite->Initialize(&ptNow, &vDir);
//    m_pSGLCore->AddSprite(mEnamySprite);
//    
//    
//    mEnamySprite = CSGLCore::MakeSprite(GetNewID(), SGL_TEAM1, ACTORID_BLZ_DGUNTOWER,m_pSGLCore->GetTextureMan(),m_pSGLCore->GetAction(), this);
//    mEnamySprite->LoadResoure(m_pSGLCore->GetModelMan());
//    mEnamySprite->Initialize(&ptNow, &vDir);
//    SetMakePosition(fRadias,0,mEnamySprite,&ptNow);
//    m_pSGLCore->AddSprite(mEnamySprite);
//    
//    mEnamySprite = CSGLCore::MakeSprite(GetNewID(), SGL_TEAM1, ACTORID_BLZ_DMISSILETOWER,m_pSGLCore->GetTextureMan(),m_pSGLCore->GetAction(), this);
//    mEnamySprite->LoadResoure(m_pSGLCore->GetModelMan());
//    mEnamySprite->Initialize(&ptNow, &vDir);
//    SetMakePosition(fRadias,2,mEnamySprite,&ptNow);
//    m_pSGLCore->AddSprite(mEnamySprite);
//    
//    
//    mEnamySprite = CSGLCore::MakeSprite(GetNewID(), SGL_TEAM1, ACTORID_BLZ_DMISSILETOWER,m_pSGLCore->GetTextureMan(),m_pSGLCore->GetAction(), this);
//    mEnamySprite->LoadResoure(m_pSGLCore->GetModelMan());
//    mEnamySprite->Initialize(&ptNow, &vDir);
//    SetMakePosition(fRadias,3,mEnamySprite,&ptNow);
//    m_pSGLCore->AddSprite(mEnamySprite);
//    
//    mEnamySprite = CSGLCore::MakeSprite(GetNewID(), SGL_TEAM1, ACTORID_BLZ_DTOWER,m_pSGLCore->GetTextureMan(),m_pSGLCore->GetAction(), this);
//    mEnamySprite->LoadResoure(m_pSGLCore->GetModelMan());
//    mEnamySprite->Initialize(&ptNow, &vDir);
//    SetMakePosition(fRadias,4,mEnamySprite,&ptNow);
//    m_pSGLCore->AddSprite(mEnamySprite);
//    
//    GetActor()->GetPosition(&ptNow);
//    mEnamySprite = CSGLCore::MakeSprite(GetNewID(), SGL_TEAM1, ACTORID_BLZ_DMISSILETOWER,m_pSGLCore->GetTextureMan(),m_pSGLCore->GetAction(), this);
//    mEnamySprite->LoadResoure(m_pSGLCore->GetModelMan());
//    mEnamySprite->Initialize(&ptNow, &vDir);
//    SetMakePosition(fRadias,5,mEnamySprite,&ptNow);
//    m_pSGLCore->AddSprite(mEnamySprite);
////    
////    mEnamySprite = CSGLCore::MakeSprite(GetNewID(), SGL_TEAM1, ACTORID_BLZ_DGUNTOWER,m_pSGLCore->GetTextureMan(),m_pSGLCore->GetAction(), this);
////    mEnamySprite->LoadResoure(m_pSGLCore->GetModelMan());
////    mEnamySprite->Initialize(&ptNow, &vDir);
////    SetMakePosition(fRadias,6,mEnamySprite,&ptNow);
////    m_pSGLCore->AddSprite(mEnamySprite);
////    
////    //아군 메인타워를 건설한다.
////    mEnamySprite = CSGLCore::MakeSprite(GetNewID(), SGL_TEAM1, ACTORID_BLZ_MAINTOWER,m_pSGLCore->GetTextureMan(),m_pSGLCore->GetAction(), this);
////    mEnamySprite->LoadResoure(m_pSGLCore->GetModelMan());
////    mEnamySprite->Initialize(&ptNow, &vDir);
////    SetMakePosition(fRadias,7,mEnamySprite,&ptNow);
////    m_pSGLCore->AddSprite(mEnamySprite);
//
//    mnNextTime = GetGGTime() + 1000 * 4; //5초후에 적을 만든다.
//}
//
//
//void CTrainingWorld::Setp8_End()
//{
//    mStep = TRAININGSTEP_END;
//    mEnamySprite = NULL;
//    mEnamySprite2= NULL;
//    
//    SetStepMessage("Tran_Step3_END","Tran_Step3_ENDDes1");
//    SVector vDir;
//    SPoint ptNow;
//    PROPERTY_TANK property;
//    
//
//    //------------------------------------------------------------------------------------------------------------------------------------
//    //Main Tower를 만든다.
//    mEnamySprite = CSGLCore::MakeSprite(GetNewID(), SGL_TEAM2, ACTORID_BLZ_EMAIN,m_pSGLCore->GetTextureMan(),m_pSGLCore->GetAction(), this);
//    mEnamySprite->LoadResoure(m_pSGLCore->GetModelMan());
//    
//    vDir.x = -1;
//    vDir.y = 0;
//    vDir.z = -1;
//    sglNormalize(&vDir);
//    
//    STARTPOINT* pPos = m_pSGLCore->mlstStartPoint[INDEX_ATTACKPOS4];
//    ptNow.x = pPos->fPosX;
//    ptNow.y = pPos->fPosY;
//    ptNow.z = pPos->fPosZ;
//    SetRotationCarmeraToPoint(&ptNow);
//    
//    mEnamySprite->Initialize(&ptNow, &vDir);
//    mEnamySprite->DeleteAICore();
//    m_pSGLCore->AddSprite(mEnamySprite);
//    SetRotationCarmeraToPoint(&ptNow);
//    //------------------------------------------------------------------------------------------------------------------------------------
//    
//    float fRadias = mEnamySprite->GetRadius() * 2.f;
//    //------------------------------------------------------------------------------------------------------------------------------------
//    //메인타워 주위에 비행기,탱크,레이져등등을 배치해준다.
//    //------------------------------------------------------------------------------------------------------------------------------------
//    //비행기를 만든다.
//    mEnamySprite = CSGLCore::MakeSprite(GetNewID(), SGL_TEAM2, ACTORID_EF1,m_pSGLCore->GetTextureMan(),m_pSGLCore->GetAction(), this);
//    mEnamySprite->LoadResoure(m_pSGLCore->GetModelMan());
//    mEnamySprite->Initialize(&ptNow, &vDir);
//    SetMakePosition(fRadias * 3.f,0,mEnamySprite,&ptNow);
//    m_pSGLCore->AddSprite(mEnamySprite);
//    
//
//    //비행기를 만든다.
//    mEnamySprite = CSGLCore::MakeSprite(GetNewID(), SGL_TEAM2, ACTORID_EF2,m_pSGLCore->GetTextureMan(),m_pSGLCore->GetAction(), this);
//    mEnamySprite->LoadResoure(m_pSGLCore->GetModelMan());
//    mEnamySprite->Initialize(&ptNow, &vDir);
//    SetMakePosition(fRadias * 2.f,1,mEnamySprite,&ptNow);
//    m_pSGLCore->AddSprite(mEnamySprite);
//    
//    
//    mEnamySprite = CSGLCore::MakeSprite(GetNewID(), SGL_TEAM2, ACTORID_ET2,m_pSGLCore->GetTextureMan(),m_pSGLCore->GetAction(), this);
//    mEnamySprite->LoadResoure(m_pSGLCore->GetModelMan());
//    mEnamySprite->Initialize(&ptNow, &vDir);
//    SetMakePosition(fRadias * 5.f,3,mEnamySprite,&ptNow);
//    m_pSGLCore->AddSprite(mEnamySprite);
//    
//    mEnamySprite = CSGLCore::MakeSprite(GetNewID(), SGL_TEAM2, ACTORID_ET5,m_pSGLCore->GetTextureMan(),m_pSGLCore->GetAction(), this);
//    mEnamySprite->LoadResoure(m_pSGLCore->GetModelMan());
//    mEnamySprite->Initialize(&ptNow, &vDir);
//    SetMakePosition(fRadias * 2.f,5,mEnamySprite,&ptNow);
//    m_pSGLCore->AddSprite(mEnamySprite);
//    
//    //8
//    mEnamySprite = CSGLCore::MakeSprite(GetNewID(), SGL_TEAM2, ACTORID_ET7,m_pSGLCore->GetTextureMan(),m_pSGLCore->GetAction(), this);
//    mEnamySprite->LoadResoure(m_pSGLCore->GetModelMan());
//    mEnamySprite->Initialize(&ptNow, &vDir);
//    SetMakePosition(fRadias,6,mEnamySprite,&ptNow);
//    m_pSGLCore->AddSprite(mEnamySprite);
//    
//
//
//#if defined(IPHONE) || defined(ANDROID)
//    mpScenario->SendTrainningStep(8);
//#endif
//    
//    mnNextTime = GetGGTime() + 1000 * 6; //5초후에 종료한다.
//}
//
//void CTrainingWorld::SetBlzSlotCount(int nBlzID, bool bAdd,CSprite* pDeletedSprite)
//{
//    int nCnt = 0;
//    CUserInfo* pUserInfo = GetUserInfo();
//    
//    vector<USERINFO*>* arrTank = pUserInfo->GetListBlz();
//    for (vector<USERINFO*>::iterator it = arrTank->begin(); it != arrTank->end(); it++)
//    {
//        USERINFO* info = *it;
//        if(info->nID == nBlzID)
//        {
//            if(bAdd)
//                nCnt = ++info->nCnt;
//            else
//                nCnt = --info->nCnt;
//            break;
//        }
//    }
//    
//    if(mpClickedSprite == NULL)
//    {
//        if(pUserInfo->GetLastBombItemSwitch() != 2) return ;
//        CList<CControl*>*pList = mpItemSlotBar->GetChilds();
//        for (int i = 0; i < pList->size(); i++)
//        {
//            CControl* control = pList->get(i);
//            
//            if ((long)control->GetLData() == -1) continue; //빈슬롯이면.... 건너띤다.
//            
//            USERINFO* info = (USERINFO*)control->GetLData();
//            if(info && info->nID == nBlzID)
//            {
//                CNumberCtl* pNum = (CNumberCtl*) control->GetChild(0)->GetChild(0);
//                pNum->SetNumber(nCnt);
//                break;
//            }
//        }
//    }
//    else if(bAdd && mpClickedSprite == pDeletedSprite)
//    {
//        SetClickedSprite(NULL);
//    }
//}
//
//void CTrainingWorld::SetStepMessage(const char *sTitle,const char *sDesc)
//{
//    vector<string>  lstImage;
//    
//    if(sTitle && mpStepMessageTitle)
//    {
//        lstImage.clear();
//        lstImage.push_back("none");
//        lstImage.push_back(SGLTextUnit::GetTextAddGLData(sTitle,"Tran_Step1_MoveT"));
//        
//        mpStepMessageTitle->SetImageData(lstImage);
//    }
//    
//    if(sDesc && mpStepMessageDes)
//    {
//        string sDescX = SGLTextUnit::GetOnlyText(sDesc);
//        
//        char* fnd = strstr(sDescX.c_str(), "^");
//        if(fnd)
//        {
//            *fnd = 0;
//            lstImage.clear();
//            lstImage.push_back("none");
//            //Trn_Introduce_Msg
//            lstImage.push_back(SGLTextUnit::GetTextAddGLData(fnd+1,"Trn_Introduce_Msg_Step"));
//            mpStepMessageDes2->SetImageData(lstImage);
//            mpStepMessageDes2->SetHide(false);
//
//        }
//        else
//            mpStepMessageDes2->SetHide(true );
//        
//        lstImage.clear();
//        lstImage.push_back("none");
//        //Trn_Introduce_Msg
//        lstImage.push_back(SGLTextUnit::GetTextAddGLData(sDescX.c_str(),"Trn_Introduce_Msg_Step"));
//        mpStepMessageDes->SetImageData(lstImage);
//        
//        
//
//    }
//    
//}
//
//int CTrainingWorld::OnEvent(SGLEvent *pEvent)
//{
//    if(pEvent->nMsgID == SGL_MESSAGE_CLICKED)
//    {
//        if(pEvent->lParam == BTN_TRAINING_STEP1_OK)
//        {
//            //이동 훈련을 합니다.
//            Setp2_MoveDlg();
//        }
//        else if(pEvent->lParam == BTN_PLAY_CONFIRM_YES)
//        {
//            if(mpConfirmMsgCtl)
//            {
//                delete mpConfirmMsgCtl;
//                mpConfirmMsgCtl = NULL;
//            }
//            //            mpConfirmMsgCtl->Hide();
//            mfScore = 0;
//            mnBonus = 0;
//            ShowMissionEndMessage(MissionEndCtlType_GoToMenu_Died,mfScore,mnBonus,true);
//            GetUserInfo()->GetListBlz()->clear();
//            GetUserInfo()->GetListBomb()->clear();
//            return E_SUCCESS;
//        }
//        else if(pEvent->lParam == BTN_ITEMSLOTSWITCH)
//        {
//            CUserInfo* pUserInfo = mpScenario->GetUserInfo();
//            
//            int nType = pUserInfo->GetLastBombItemSwitch();
//            if(nType == 0)
//            {
//                pUserInfo->SetLastBombItemSwitch(1);
//                SetStepMessage(NULL,"Tran_Step2_ChSlotTDes3");
//            }
//            else if(nType == 1)
//            {
//                pUserInfo->SetLastBombItemSwitch(2);
//            }
//            else if(nType == 2)
//            {
//                pUserInfo->SetLastBombItemSwitch(0);
//                SetStepMessage(NULL,"Tran_Step2_ChSlotTDes2");
//            }
//            
//            
//            ResetDisplayItemSlot();
//            
//            nType = pUserInfo->GetLastBombItemSwitch();
//            if(nType == 2)
//            {
//                mpItemSlotBar->SetHide(true);
//                if(mpRaidaFingerVertex)
//                {
//                    delete[] mpRaidaFingerVertex;
//                    mpRaidaFingerVertex = NULL;
//                }
//                OnNextStep(mStep);
//            }
//            return E_SUCCESS;
//        }
//    }
//    else if(pEvent->nMsgID == SGL_TRAINING_NEXT_END)
//    {
//        OnNextStep(mStep);
//    }
//    else if(pEvent->nMsgID == SGL_MESSAGE_SUCCESS_MISSION)
//    {
//        //성공했을때 보너스를 추가해주지 말자.
//        CUserInfo* pUserInfo = 0;
//        pUserInfo = mpScenario->GetUserInfo();
//        if(!pUserInfo->IsCompletedMap(pUserInfo->GetLastSelectedMapID()))
//            mnBonus = 4000;
//        else
//            mnBonus = 0;
//        ShowMissionEndMessage(MissionEndCtlType_GoToMenu_Successed,mfScore,mnBonus,true);
//        SetPauseTime(true); //시간을 멈춘다.
//        
//        
//        GetUserInfo()->GetListBomb()->clear();
//        //현재 훈련병일때 이등병으로 진급해준다.
//        if(pUserInfo->GetRank() == 0)
//        {
//            pUserInfo->SetPromotionSloder(true);
//            
//            // 발사속도가 빠른
//            pUserInfo->AddListToBomb(1,1000,0);
//            // 4분산탄 100개 2분산탄 2개
//            pUserInfo->AddListToBomb(8,200,0);
//        }
//        
//        GetUserInfo()->GetListBlz()->clear();
//        
//        return E_SUCCESS;
//    }
//    else if(pEvent->nMsgID == SGL_MAKE_BLZ)
//    {
//        float* fpos = (float*)pEvent->lObject;
//        SVector vDir;
//        vDir.x = -1;
//        vDir.y = 0;
//        vDir.z = -1;
//        sglNormalize(&vDir);
//        CSprite* pBlz = CSGLCore::MakeSprite(GetNewID(), pEvent->lParam2, (int)pEvent->lParam, GetTextureMan(),m_pSGLCore->GetAction(), this);
//        pBlz->LoadResoure(GetModelMan());
//        SPoint ptNow;
//        ptNow.x = fpos[0];
//        ptNow.y = fpos[1];
//        ptNow.z = fpos[2];
//        pBlz->Initialize(&ptNow, &vDir);
//        m_pSGLCore->AddSprite(pBlz);
//        if(fpos) delete[] fpos;
//        SetBlzSlotCount((int)pEvent->lParam, false, NULL); //--카운터해준다.
//        
//        return E_SUCCESS;
//    }
//    
//    
//    return CHWorld::OnEvent(pEvent);
//}
//
//void CTrainingWorld::BeginTouch(long lTouchID,float x, float y)
//{
//    //카메라가 회전할때 터치를 못하게 막는다.
//    if(mpCarmeraRotationTartketPoint) return;
//    
//    if((mpConfirmMsgCtl == NULL && mpMissionMsgCtl == NULL && mpPauseMsgCtl == NULL) &&
//       mpMessageDlg && mpMessageDlg->BeginTouch(lTouchID, x, y) == false) return;
//#ifdef MAC
//    if(mpStepMessageDlg) mpStepMessageDlg->BeginTouch(lTouchID, x, y);
//#endif
//    CHWorld::BeginTouch(lTouchID, x, y);
//}
//
//void CTrainingWorld::MoveTouch(long lTouchID,float x, float y)
//{
//    if((mpConfirmMsgCtl == NULL && mpMissionMsgCtl == NULL && mpPauseMsgCtl == NULL) &&
//       mpMessageDlg && mpMessageDlg->MoveTouch(lTouchID, x, y) == false) return;
//#ifdef MAC
//    if(mpStepMessageDlg) mpStepMessageDlg->MoveTouch(lTouchID, x, y);
//#endif
//    CHWorld::MoveTouch(lTouchID, x, y);
//    
//}
//
//void CTrainingWorld::EndTouch(long lTouchID,float x, float y)
//{
//    
//    if((mpConfirmMsgCtl == NULL && mpMissionMsgCtl == NULL && mpPauseMsgCtl == NULL) &&
//       mpMessageDlg && mpMessageDlg->EndTouch(lTouchID, x, y) == false) return;
//#ifdef MAC
//    if(mpStepMessageDlg) mpStepMessageDlg->EndTouch(lTouchID, x, y);
//#endif
//    CHWorld::EndTouch(lTouchID, x, y);
//}
//
//
//void CTrainingWorld::RenderFingerBegin(int nTime)
//{
//    
//    const float fMin = 8.f;
//    const float fMax = 15.f;
//    
//    float   matrix[16];
//    
//    CK9Sprite* pActor = (CK9Sprite*)GetActor();
//    if(mStep == TRAININGSTEP_MOVE && !(pActor->isMoving() || pActor->isTurning()))
//    {
//        SPoint ptNow;
//        pActor->GetPosition(&ptNow);
//        
//        SGLCAMERA* camera = GetCamera();
//        //카메라의 방향으로 설정을 한다. ( camera방향은 -로 설정해준다.)
//        float fAngle = atan2(-camera->viewDir.z,camera->viewDir.x) * 180.0 / PI;
//        
//        if(mMoveMotionStep == 0)
//        {
//            mfFingerUpDwonAction = mfFingerUpDwonAction + mfFingerUpDwonActionDirection * (float)nTime / 80.f;
//            
//            if(mfFingerUpDwonAction <= fMin)
//            {
//                
//                mMoveMotionStep = 1;
////                mfFingerUpDwonAction = fMin;
////                mfFingerUpDwonActionDirection = 1.f;
//            }
//            else
//            {
//                SetStepMessage(NULL,"Tran_Step1_MoveDes1");
//                mvtFingerMove.x = 0; //초기화.
//                
//                sglLoadIdentityf(matrix);
//                
//                //15 ~ 8 로 움직인다.
//                sglTranslateMatrixf(matrix, ptNow.x, ptNow.y + mfFingerUpDwonAction, ptNow.z);
//                
//                sglRotateRzRyRxMatrixf(matrix,
//                                       0,
//                                       fAngle - 90.f,
//                                       0);
//                
//                sglScaleMatrixf(matrix, 2.f, 4.f, 1.f);
//                
//                
//                
//                sglMultMatrixVectorf(&mFingerVertex[0], matrix,&gPanelVertexY[0]);
//                sglMultMatrixVectorf(&mFingerVertex[3], matrix,&gPanelVertexY[3]);
//                sglMultMatrixVectorf(&mFingerVertex[6], matrix,&gPanelVertexY[6]);
//                sglMultMatrixVectorf(&mFingerVertex[9], matrix,&gPanelVertexY[9]);
//            }
//        }
//        else
//        {
//            
//            STARTPOINT* pMoveTargetPostion = m_pSGLCore->mlstStartPoint[INDEX_MOVEPOS];
//            
//            SetStepMessage(NULL,"Tran_Step1_MoveDes2");
//            
//            if(mvtFingerMove.x == 0)
//            {
//                mvtFingerMove.x = pMoveTargetPostion->fPosX - ptNow.x;
//                mvtFingerMove.y = 0;
//                mvtFingerMove.z = pMoveTargetPostion->fPosZ - ptNow.z;
//                
//                sglNormalize(&mvtFingerMove);
//                memcpy(&mptFingerMove, &ptNow, sizeof(SPoint));
//                
//                mfFingerUpDwonAction = fMin;
//                mfFingerUpDwonActionDirection = 1.f;
//            }
//            
//            //----------------------------------------------------
//            //이동하게 한다.
//            //----------------------------------------------------
//            mptFingerMove.x = mptFingerMove.x + mvtFingerMove.x * (float)nTime / 20.f;
//            mptFingerMove.y = 0;
//            mptFingerMove.z = mptFingerMove.z + mvtFingerMove.z * (float)nTime / 20.f;
//            GetPositionY(&mptFingerMove);
//            
//            
//            //목표지점에 도달했으면 이동을 중지 한다.
//            float fxDir = pMoveTargetPostion->fPosX - mptFingerMove.x;
//            float fzDir = pMoveTargetPostion->fPosZ - mptFingerMove.z;
//            if((mvtFingerMove.x >= 0 && fxDir < 0) || (mvtFingerMove.x <= 0 && fxDir > 0) || (mvtFingerMove.z >= 0 && fzDir < 0) || (mvtFingerMove.z <= 0 && fzDir > 0))
//            {
//                //목표지점에 도착하면 다시 시작 한다.
//                mMoveMotionStep = 0;
//                mfFingerUpDwonAction = fMin;
//                mfFingerUpDwonActionDirection = -1.f;
//                mfFingerUpDwonAction = 15.f;
//                
//            }
//            else
//            {
//            
//            
//                sglLoadIdentityf(matrix);
//                
//                
//                //15 ~ 8 로 움직인다.
//                sglTranslateMatrixf(matrix, mptFingerMove.x, mptFingerMove.y + mfFingerUpDwonAction, mptFingerMove.z);
//                
//                sglRotateRzRyRxMatrixf(matrix,
//                                       0,
//                                       fAngle - 90.f,
//                                       0);
//                
//                sglScaleMatrixf(matrix, 2.f, 4.f, 1.f);
//                
//                
//                
//                sglMultMatrixVectorf(&mFingerVertex[0], matrix,&gPanelVertexY[0]);
//                sglMultMatrixVectorf(&mFingerVertex[3], matrix,&gPanelVertexY[3]);
//                sglMultMatrixVectorf(&mFingerVertex[6], matrix,&gPanelVertexY[6]);
//                sglMultMatrixVectorf(&mFingerVertex[9], matrix,&gPanelVertexY[9]);
//            }
//            
//        }
//        
//        
////    if(mStep == TRAININGSTEP_MOVE && !(pActor->isMoving() || pActor->isTurning()))
////    {
////        SPoint ptNow;
////        pActor->GetPosition(&ptNow);
////        
////        SGLCAMERA* camera = GetCamera();
////        //카메라의 방향으로 설정을 한다. ( camera방향은 -로 설정해준다.)
////        float fAngle = atan2(-camera->viewDir.z,camera->viewDir.x) * 180.0 / PI;
////        
////        if(!mpStick->IsActorMovingMotion())
////        {
////            mfFingerUpDwonAction = mfFingerUpDwonAction + mfFingerUpDwonActionDirection * (float)nTime / 80.f;
////            
////            if(mfFingerUpDwonAction <= fMin)
////            {
////                mfFingerUpDwonAction = fMin;
////                mfFingerUpDwonActionDirection = 1.f;
////            }
////            if(mfFingerUpDwonAction >= fMax)
////            {
////                mfFingerUpDwonAction = fMax;
////                mfFingerUpDwonActionDirection = -1.f;
////            }
////            
////
////            SetStepMessage(NULL,"Tran_Step1_MoveDes1");
////            mvtFingerMove.x = 0; //초기화.
////            
////            sglLoadIdentityf(matrix);
////            
////            //15 ~ 8 로 움직인다.
////            sglTranslateMatrixf(matrix, ptNow.x, ptNow.y + mfFingerUpDwonAction, ptNow.z);
////            
////            sglRotateRzRyRxMatrixf(matrix,
////                                   0,
////                                   fAngle - 90.f,
////                                   0);
////            
////            sglScaleMatrixf(matrix, 2.f, 4.f, 1.f);
////            
////            
////            
////            sglMultMatrixVectorf(&mFingerVertex[0], matrix,&gPanelVertexY[0]);
////            sglMultMatrixVectorf(&mFingerVertex[3], matrix,&gPanelVertexY[3]);
////            sglMultMatrixVectorf(&mFingerVertex[6], matrix,&gPanelVertexY[6]);
////            sglMultMatrixVectorf(&mFingerVertex[9], matrix,&gPanelVertexY[9]);
////        }
////        else
////        {
////            
////            STARTPOINT* pMoveTargetPostion = m_pSGLCore->mlstStartPoint[INDEX_MOVEPOS];
////            
////            SetStepMessage(NULL,"Tran_Step1_MoveDes2");
////            
////            if(mvtFingerMove.x == 0)
////            {
////                mvtFingerMove.x = pMoveTargetPostion->fPosX - ptNow.x;
////                mvtFingerMove.y = 0;
////                mvtFingerMove.z = pMoveTargetPostion->fPosZ - ptNow.z;
////                
////                sglNormalize(&mvtFingerMove);
////                memcpy(&mptFingerMove, &ptNow, sizeof(SPoint));
////
////                mfFingerUpDwonAction = fMin;
////                mfFingerUpDwonActionDirection = 1.f;
////            }
////            
////            //----------------------------------------------------
////            //이동하게 한다.
////            //----------------------------------------------------
////            mptFingerMove.x = mptFingerMove.x + mvtFingerMove.x * (float)nTime / 20.f;
////            mptFingerMove.y = 0;
////            mptFingerMove.z = mptFingerMove.z + mvtFingerMove.z * (float)nTime / 20.f;
////            GetPositionY(&mptFingerMove);
////            
////            
////            //목표지점에 도달했으면 이동을 중지 한다.
////            float fxDir = pMoveTargetPostion->fPosX - mptFingerMove.x;
////            float fzDir = pMoveTargetPostion->fPosZ - mptFingerMove.z;
////            if((mvtFingerMove.x >= 0 && fxDir < 0) || (mvtFingerMove.x <= 0 && fxDir > 0) || (mvtFingerMove.z >= 0 && fzDir < 0) || (mvtFingerMove.z <= 0 && fzDir > 0))
////                return;
////
////            
////            sglLoadIdentityf(matrix);
////            
////            
////            //15 ~ 8 로 움직인다.
////            sglTranslateMatrixf(matrix, mptFingerMove.x, mptFingerMove.y + mfFingerUpDwonAction, mptFingerMove.z);
////            
////            sglRotateRzRyRxMatrixf(matrix,
////                                   0,
////                                   fAngle - 90.f,
////                                   0);
////            
////            sglScaleMatrixf(matrix, 2.f, 4.f, 1.f);
////            
////            
////            
////            sglMultMatrixVectorf(&mFingerVertex[0], matrix,&gPanelVertexY[0]);
////            sglMultMatrixVectorf(&mFingerVertex[3], matrix,&gPanelVertexY[3]);
////            sglMultMatrixVectorf(&mFingerVertex[6], matrix,&gPanelVertexY[6]);
////            sglMultMatrixVectorf(&mFingerVertex[9], matrix,&gPanelVertexY[9]);
////            
////        }
//    
//        
//    }
//    else if(mStep == TRAININGSTEP_MOVE)
//    {
//        SetStepMessage(NULL,"Tran_Step1_MoveDes3");
//    }
//    else if(mStep == TRAININGSTEP_ATTACK)
//    {
//     
//        bool IsEnamy = false;
//        
//        if(mEnamySprite && mEnamySprite->GetState() == SPRITE_RUN)
//        {
//            SPoint ptNow;
//            mEnamySprite->GetPosition(&ptNow);
//            
//            SGLCAMERA* camera = GetCamera();
//            //카메라의 방향으로 설정을 한다. ( camera방향은 -로 설정해준다.)
//            float fAngle = atan2(-camera->viewDir.z,camera->viewDir.x) * 180.0 / PI;
//            
//            mfFingerUpDwonAction = mfFingerUpDwonAction + mfFingerUpDwonActionDirection * (float)nTime / 80.f;
//            
//            if(mfFingerUpDwonAction <= fMin)
//            {
//                mfFingerUpDwonAction = fMin;
//                mfFingerUpDwonActionDirection = 1.f;
//            }
//            if(mfFingerUpDwonAction >= fMax)
//            {
//                mfFingerUpDwonAction = fMax;
//                mfFingerUpDwonActionDirection = -1.f;
//            }
//            
//            
//            //SetStepMessage(NULL,"Tran_Step1_MoveDes1");
//            mvtFingerMove.x = 0; //초기화.
//            
//            sglLoadIdentityf(matrix);
//            
//            //15 ~ 8 로 움직인다.
//            sglTranslateMatrixf(matrix, ptNow.x, ptNow.y + mfFingerUpDwonAction, ptNow.z);
//            
//            sglRotateRzRyRxMatrixf(matrix,
//                                   0,
//                                   fAngle - 90.f,
//                                   0);
//            
//            sglScaleMatrixf(matrix, 2.f, 4.f, 1.f);
//            
//            
//            
//            sglMultMatrixVectorf(&mFingerVertex[0], matrix,&gPanelVertexY[0]);
//            sglMultMatrixVectorf(&mFingerVertex[3], matrix,&gPanelVertexY[3]);
//            sglMultMatrixVectorf(&mFingerVertex[6], matrix,&gPanelVertexY[6]);
//            sglMultMatrixVectorf(&mFingerVertex[9], matrix,&gPanelVertexY[9]);
//            IsEnamy = true;
//        }
//        else if(mEnamySprite && mEnamySprite->GetState() != SPRITE_RUN)
//        {
//            mEnamySprite = NULL;
//        }
//        
//        
//        if(IsEnamy == false && mEnamySprite2 && mEnamySprite2->GetState() == SPRITE_RUN)
//        {
//            SPoint ptNow;
//            mEnamySprite2->GetPosition(&ptNow);
//            
//            SGLCAMERA* camera = GetCamera();
//            //카메라의 방향으로 설정을 한다. ( camera방향은 -로 설정해준다.)
//            float fAngle = atan2(-camera->viewDir.z,camera->viewDir.x) * 180.0 / PI;
//            
//            mfFingerUpDwonAction = mfFingerUpDwonAction + mfFingerUpDwonActionDirection * (float)nTime / 80.f;
//            
//            if(mfFingerUpDwonAction <= fMin)
//            {
//                mfFingerUpDwonAction = fMin;
//                mfFingerUpDwonActionDirection = 1.f;
//            }
//            if(mfFingerUpDwonAction >= fMax)
//            {
//                mfFingerUpDwonAction = fMax;
//                mfFingerUpDwonActionDirection = -1.f;
//            }
//            
//            
//            //SetStepMessage(NULL,"Tran_Step1_MoveDes1");
//            mvtFingerMove.x = 0; //초기화.
//            
//            sglLoadIdentityf(matrix);
//            
//            //15 ~ 8 로 움직인다.
//            sglTranslateMatrixf(matrix, ptNow.x, ptNow.y + mfFingerUpDwonAction, ptNow.z);
//            
//            sglRotateRzRyRxMatrixf(matrix,
//                                   0,
//                                   fAngle - 90.f,
//                                   0);
//            
//            sglScaleMatrixf(matrix, 2.f, 4.f, 1.f);
//            
//            
//            
//            sglMultMatrixVectorf(&mFingerVertex[0], matrix,&gPanelVertexY[0]);
//            sglMultMatrixVectorf(&mFingerVertex[3], matrix,&gPanelVertexY[3]);
//            sglMultMatrixVectorf(&mFingerVertex[6], matrix,&gPanelVertexY[6]);
//            sglMultMatrixVectorf(&mFingerVertex[9], matrix,&gPanelVertexY[9]);
//            IsEnamy = true;
//        }
//        else if(mEnamySprite2 && mEnamySprite2->GetState() != SPRITE_RUN)
//        {
//            mEnamySprite2 = NULL;
//        }
//        
//        if(IsEnamy == false)
//        {
//            mEnamySprite = NULL;
//            OnNextStep(mStep);
//        }
//    }
//    else if(mStep == TRAININGSTEP_ATTACKRADIA)
//    {
//        if(!(mEnamySprite && mEnamySprite->GetState() == SPRITE_RUN))
//        {
//            mEnamySprite = NULL;
//            mpRadaBoard->SetHide(true);
//            if(mpRaidaFingerVertex)
//            {
//                delete[] mpRaidaFingerVertex;
//                mpRaidaFingerVertex = NULL;
//            }
//            OnNextStep(mStep);
//        }
//    }
//    else if(mStep == TRAININGSTEP_MAKETOWER)
//    {
//        CListNode<CSprite>* pList = GetSGLCore()->GetSpriteList();
//        ByNode<CSprite>* b = pList->begin();
//        for(ByNode<CSprite>* it = b; it != pList->end(); )
//        {
//            CSprite* sp = it->GetObject();
//            if(sp->GetState() == SPRITE_RUN && sp->GetModelID() == ACTORID_BLZ_DTOWER)
//            {
//                if(((CDTower*)sp)->IsMaking())
//                {
//                    SetStepMessage(NULL,"Tran_Step6_MakeTowerDes2");
//                    
//                    if(mpArrowBlz)
//                    {
//                        delete mpArrowBlz;
//                        mpArrowBlz = NULL;
//                    }
//                    
//                    if(mpRaidaFingerVertex)
//                    {
//                        delete[] mpRaidaFingerVertex;
//                        mpRaidaFingerVertex = NULL;
//                    }
//                }
//                else
//                {
//                    OnNextStep(mStep); //다음 스템으로 변경한다.
//                }
//                break;
//            }
//            it = it->GetNextNode();
//        }
//    }
//    else if(mStep == TRAININGSTEP_MISSION)
//    {
//        SPoint ptNow;
//        
//        if(mEnamySprite && mEnamySprite->GetState() == SPRITE_RUN)
//        {
//            mEnamySprite->GetPosition(&ptNow);
//            
//            SGLCAMERA* camera = GetCamera();
//            //카메라의 방향으로 설정을 한다. ( camera방향은 -로 설정해준다.)
//            float fAngle = atan2(-camera->viewDir.z,camera->viewDir.x) * 180.0 / PI;
//            
//            mfFingerUpDwonAction = mfFingerUpDwonAction + mfFingerUpDwonActionDirection * (float)nTime / 80.f;
//            
//            if(mfFingerUpDwonAction <= fMin)
//            {
//                mfFingerUpDwonAction = fMin;
//                mfFingerUpDwonActionDirection = 1.f;
//            }
//            if(mfFingerUpDwonAction >= fMax)
//            {
//                mfFingerUpDwonAction = fMax;
//                mfFingerUpDwonActionDirection = -1.f;
//            }
//            
//            
//            //SetStepMessage(NULL,"Tran_Step1_MoveDes1");
//            mvtFingerMove.x = 0; //초기화.
//            
//            sglLoadIdentityf(matrix);
//            
//            //15 ~ 8 로 움직인다.
//            sglTranslateMatrixf(matrix, ptNow.x, ptNow.y + 10.0f + mfFingerUpDwonAction, ptNow.z);
//            
//            sglRotateRzRyRxMatrixf(matrix,
//                                   0,
//                                   fAngle - 90.f,
//                                   0);
//            
//            sglScaleMatrixf(matrix, 2.f, 4.f, 1.f);
//            
//            
//            
//            sglMultMatrixVectorf(&mFingerVertex[0], matrix,&gPanelVertexY[0]);
//            sglMultMatrixVectorf(&mFingerVertex[3], matrix,&gPanelVertexY[3]);
//            sglMultMatrixVectorf(&mFingerVertex[6], matrix,&gPanelVertexY[6]);
//            sglMultMatrixVectorf(&mFingerVertex[9], matrix,&gPanelVertexY[9]);
//        }
//        else
//            mEnamySprite = NULL;
//    }
//    
//    if(mpRaidaFingerVertex)
//    {
//        mfFingerUpDwonAction = mfFingerUpDwonAction + mfFingerUpDwonActionDirection * (float)nTime / 200.f;
//        if(mfFingerUpDwonAction <= mfRaidaFingerMin)
//        {
//            mfFingerUpDwonAction = mfRaidaFingerMin;
//            mfFingerUpDwonActionDirection = 1.f;
//        }
//        if(mfFingerUpDwonAction >= mfRaidaFingerMax)
//        {
//            mfFingerUpDwonAction = mfRaidaFingerMax;
//            mfFingerUpDwonActionDirection = -1.f;
//        }
//        sglLoadIdentityf(matrix);
//        //15 ~ 8 로 움직인다.
//        sglTranslateMatrixf(matrix, 0.f, mfFingerUpDwonAction, 0.f);
//        sglMultMatrixVectorf(&mRaidaFingerWorldVertex[0], matrix,&mpRaidaFingerVertex[0]);
//        sglMultMatrixVectorf(&mRaidaFingerWorldVertex[3], matrix,&mpRaidaFingerVertex[3]);
//        sglMultMatrixVectorf(&mRaidaFingerWorldVertex[6], matrix,&mpRaidaFingerVertex[6]);
//        sglMultMatrixVectorf(&mRaidaFingerWorldVertex[9], matrix,&mpRaidaFingerVertex[9]);
//    }
//}
//
//void CTrainingWorld::RenderFinger()
//{
//    CK9Sprite* pActor = (CK9Sprite*)GetActor();
//    if(
//       (mStep == TRAININGSTEP_MOVE && !(pActor->isMoving() || pActor->isTurning())) ||
//       ((mStep == TRAININGSTEP_ATTACK || mStep == TRAININGSTEP_MISSION) && mEnamySprite)
//       )
//    {
//        glColor4f(1.0f,1.0f,1.0f,1.0f);
//        glBindTexture(GL_TEXTURE_2D, mnFingerTexture);
//        glTexCoordPointer(2, GL_FLOAT, 0, gPanelCoordTex);
//
//        glVertexPointer(3, GL_FLOAT, 0, mFingerVertex);
//        glDrawElements(GL_TRIANGLE_STRIP, gshPanelIndicesCnt, GL_UNSIGNED_SHORT, gPanelIndices);
//    }
//}
//
//void CTrainingWorld::SetClickedSprite(CSprite* pClickedSprite)
//{
//    if(mpClickedSprite == NULL && pClickedSprite == NULL) return;
//    if(GetActor() == pClickedSprite) return ; //Added By Song 2014.02.25 5단계에서 탱크를 클릭하면 더이상 진행할 수가 없다.
//    mpClickedSprite = pClickedSprite;
//    if(pClickedSprite) OnSelectedObjectShowDescBar(pClickedSprite);
//}
//
//
//
////확장한 이유는 적이 터졌을때 점수를 올려주지 말자.
//void CTrainingWorld::CheckAttectedByEnSpriteAtBomb(CListNode<CSprite> *pCoreListSprite,CBomb *pBomb)
//{
//    CSprite *pSprite;
//    CSprite *pActor = GetActor();
//    CSprite *pOwnerBomb = pBomb->GetOwner();
//    int spState;
//    SPoint ptSp,ptBomb;
//    CSGLCore *pSGLCore = GetSGLCore();
//    
//    float subX;
//    float subY;
//    
//    float fCompactGrade = 0;
////    float fAttackInter = 0.0f;
//    
//    int nActorTeamID = pActor->GetTeamID();
//    int nSpriteTeamID = 0;
//    int nBombTeamID = 0;
//    
//    pBomb->GetPosition(&ptBomb);
//    for(ByNode<CSprite>* it = pCoreListSprite->end()->GetPrevNode(); it != &pCoreListSprite->GetHead();)
//    {
//        pSprite = it->GetObject();
//        it = it->GetPrevNode();
//        
//        spState = pSprite->GetState();
//        
//        
//        if( spState != SPRITE_RUN) continue;
//        
//        nSpriteTeamID = pSprite->GetTeamID();
//        nBombTeamID = pBomb->GetTeamID();
//
//        if(IsAllians(nSpriteTeamID, nBombTeamID)) continue;
//        pSprite->GetPosition(&ptSp);
//        
//        subX = ptSp.x - ptBomb.x;
//        subX = fabs(subX);
//        subY = ptSp.y - ptBomb.y;
//        subY = fabs(subY);
//        
//        if(subX < 300.f && subY < 300.f)
//        {
//            pSprite->Command(AICOMMAND_ATTACKEDBYEMSPRITE, pBomb->GetOwner());
//        }
//        
//        if(subX < 3000.f && subY < 3000.f)
//        {
//            if( pSprite->GetState() != SPRITE_DELETED)
//            {
//                fCompactGrade = pBomb->InsideRangeCompact(pSprite);
//                if(fCompactGrade != 0.f)
//                {
//                    //SPRITE_READYDELETE하면 다음단계에서 캐릭터는 SPRITE_READYDELETE => SPRITE_DELETED 로 해버린다. 그럴경운 그다음 단계에 CET1의 부스터의 단계는 SPRITE_DELETED로 변경되어 에러가 발생한다.
//                        pOwnerBomb = pBomb->GetOwner();
//                        if(pSprite->CompactToBomb(pBomb,fCompactGrade)) //명중하여 .....
//                        {
//                            
//                            pSprite->SetState(BOMB_COMPACT);
//                            
//                            if(mStep == TRAININGSTEP_END)
//                            {
//                                //맞아서 죽으면 기준치값을 받아온다.
//                                if(nActorTeamID == pBomb->GetTeamID())
//                                {
//                                    if(pSprite->GetModelID() == ACTORID_BLZ_EMAIN)
//                                        CScenario::SendMessage(SGL_MESSAGE_SUCCESS_MISSION,0);
//                                }
//                                else if(IsAllians(nActorTeamID, nBombTeamID)) //동맹군이 주이거나 메인타워를 터트리면 아이템을 만들거나, 게임을 종려한다.
//                                {
//                                    if(pSprite->GetModelID() == ACTORID_BLZ_EMAIN)
//                                        CScenario::SendMessage(SGL_MESSAGE_SUCCESS_MISSION,0);
//                                }
//                            }
//                            else if(mStep == TRAININGSTEP_MISSION)
//                            {
//                                mEnamySprite = NULL;
//                                mEnamySprite2 = NULL;
//                                CScenario::SendMessage(SGL_TRAINING_NEXT_END,0);
//                            }
//                                
//                        }
//                        else
//                        {
//                            
//                            if(pSprite == pActor) //버블이 있으면 흔들지 말자.
//                            {
//                                CAniRuntimeBubble* pBubble = pSprite->GetAniRuntimeBubble();
//                                if(pBubble)
//                                {
//                                    pBubble->Attacted();
//                                }
//                                else
//                                {
//                                    SetAnimationCameraAttacked();
//                                    pSGLCore->PlayVibrate();
//                                }
//                            }
//                        }
//                        
//                        if(pOwnerBomb)
//                            pOwnerBomb->Command(AICOMMAND_HITTHEMARK, (void*)1); //명중했다라고 ....
//                    
//                }
//            }
//        }
//    }
//}
//
//
//void CTrainingWorld::ResetDisplayItemSlot()
//{
//    if(mStep == TRAININGSTEP_CHANGE_SLOT)
//    {
//        CSprite* pActor = GetActor();
//        vector<string>  lstImage;
//        int nMaxPageCnt = 0;
//        const int nMaxItem = MAXSLOTCNT;
//        vector<CControl*> lstSlotCtl;
//        CUserInfo* pUserInfo = mpScenario->GetUserInfo();
//        vector<USERINFO*>  lstType3Org;
//        vector<USERINFO*>* lstItem = NULL;
//        int nType = 0;
//        int nBombID = 0;
//        
//        if(mpClickedSprite == NULL)
//        {
//            nType = pUserInfo->GetLastBombItemSwitch(); // 0 : Bomb 1:Item 2:Blz 3:ClickedTower
//            if(nType == 1)
//            {
//                lstItem = pUserInfo->GetListItem();
//            }
//            else if(nType == 0)
//            {
//                lstItem = pUserInfo->GetListBomb();
//                if(pActor && pActor->GetState() == SPRITE_RUN)
//                    nBombID = pActor->GetCurrentWeaponID();
//            }
//            else //2 : Blz
//            {
//                lstItem = pUserInfo->GetListBlz();
//            }
//            mpItemSlotSwitchBtn->SetEnable(true);
//        }
//        else
//        {
//            nType = 3;
//            
//            //------------------------------------------------
//            //TowerStatus Info List
//            USERINFO* pNew = new USERINFO;
//            pNew->nID = RUNTIME_UPDATE_REMOVE;
//            pNew->nType = 1; //영구 사용
//            pNew->nCnt = 0; //포스텝으로 카운트를 나타낸다.
//            lstType3Org.push_back(pNew);
//            lstItem = &lstType3Org;
//            //------------------------------------------------
//            
//            mpItemSlotSwitchBtn->SetEnable(false);
//        }
//        
////        //내용을 다 지우자.
////        lstItem->clear();
//        
//        mpSelectedWeaponBtn = NULL;
//        
//        CNumberCtl* pItemPageNumber = (CNumberCtl*)mpItemSlotBar->FindControl(CTL_TABLE_ITEM_SLOT_PAGE);
//        
//        int nSelIndex = 1;
//        if(nType == 1)
//            nSelIndex = pUserInfo->GetLastSelectedItemPageIndex();
//        else if(nType == 0)
//            nSelIndex = pUserInfo->GetLastSelectedBombPageIndex();
//        else if(nType == 2)
//            nSelIndex = pUserInfo->GetLastSelectedBlzPageIndex();
//        
//        
//        
//        lstImage.clear();
//        lstImage.push_back("res_slot.png");
//        
//        if(mpClickedSprite == NULL)
//        {
//            if(pUserInfo->GetLastBombItemSwitch() == 0)
//                lstImage.push_back("M_Slot_Bomb");
//            else if(pUserInfo->GetLastBombItemSwitch() == 1)
//                lstImage.push_back("M_Slot_Item");
//            else if(pUserInfo->GetLastBombItemSwitch() == 2)
//                lstImage.push_back("M_Slot_BLZ");
//        }
//        else
//        {
//            lstImage.push_back("타워");
//        }
//        mpItemSlotSwitchBtn->SetImageData(lstImage);
//        
//        nMaxPageCnt = (int)lstItem->size() / nMaxItem;
//        if((lstItem->size() % nMaxItem) != 0)
//            nMaxPageCnt += 1;
//        
//        if(nSelIndex > nMaxPageCnt)
//            nSelIndex = nMaxPageCnt;
//        else if( nSelIndex < 1)
//            nSelIndex = 1;
//        
//        pItemPageNumber->SetNumber(nSelIndex);
//        
//        
//        CControl* pUp = mpItemSlotBar->FindControl(BTN_ITEMSLOTUPPAGE);
//        CControl* pDown= mpItemSlotBar->FindControl(BTN_ITEMSLOTDOWNPAGE);
//        
//        if(nSelIndex == 1)
//            pDown->SetEnable(false);
//        else
//            pDown->SetEnable(true);
//        
//        if(nSelIndex == nMaxPageCnt)
//            pUp->SetEnable(false);
//        else
//            pUp->SetEnable(true);
//        
//        
//        for(int i = CTL_TABLE_ITEM_SLOT3; i <= CTL_TABLE_ITEM_SLOT6; i++)
//        {
//            lstSlotCtl.push_back(mpItemSlotBar->FindControl(i));
//        }
//        
//        int nIndex = 0;
//        int nMaxCnt = (int)lstItem->size();
//        string sPath;
//        CButtonCtl* pBtnSlot = NULL;
//        for (int i = 0; i < nMaxItem; i++)
//        {
//            nIndex = (nSelIndex - 1) * nMaxItem + i;
//            pBtnSlot = (CButtonCtl*)lstSlotCtl[i];
//            
//            if(nIndex >= nMaxCnt || nMaxCnt == 0)
//            {
//                lstImage.clear();
//                lstImage.push_back("none");
//                lstImage.push_back("none");
//                pBtnSlot->GetChild(0)->SetImageData(lstImage);
//                pBtnSlot->GetChild(0)->GetChild(0)->SetHide(true);
//                pBtnSlot->GetChild(0)->GetChild(1)->SetHide(true);
//                pBtnSlot->SetLData(-1);
//                
//                pBtnSlot->SetButtonType(BUTTON_NORMAL);
//                pBtnSlot->SetPushed(false);
//                pBtnSlot->SetEnable(false);
//            }
//            else
//            {
//                pBtnSlot->SetEnable(false);
//                pBtnSlot->GetChild(0)->GetChild(1)->SetHide(true);
//                if(nType == 1)
//                {
//                    int nID = lstItem->at(nIndex)->nID;
//                    PROPERTY_ITEM property;
//                    if(PROPERTY_ITEM::GetPropertyItem(nID, property) == NULL) continue;
//                    
//                    lstImage.clear();
//                    sPath = property.sModelPath;
//                    sPath.append(".tga");
//                    lstImage.push_back(sPath);
//                    lstImage.push_back("none");
//                    
//                    pBtnSlot->GetChild(0)->SetImageData(lstImage);
//                    pBtnSlot->GetChild(0)->SetBackCoordTex(0.0f, 0.0f, 0.96875f, 0.96875f);
//                    pBtnSlot->SetLData((long)lstItem->at(nIndex));
//                    pBtnSlot->SetPushed(false);
//                    
//                    pBtnSlot->SetButtonType(BUTTON_NORMAL);
//                    
//                    if(property.nType == 0) //Count
//                    {
//                        CNumberCtl* pNumCtl = (CNumberCtl*)lstSlotCtl[i]->GetChild(0)->GetChild(0);
//                        pNumCtl->SetNumber(pUserInfo->GetCntByIDWithItem(nID));
//                        pBtnSlot->GetChild(0)->GetChild(0)->SetHide(false);
//                    }
//                    else if(property.nType == 2) //Upgrade
//                    {
//                        PROPERTY_TANK property;
//                        PROPERTY_TANK::GetPropertyTank(pUserInfo->GetLastSelectedTankID(),property);
//                        if(nID == ID_ITEM_UPGRADE_ATTACK)
//                        {
//                            CNumberCtl* pNumCtl = (CNumberCtl*)pBtnSlot->GetChild(0)->GetChild(0);
//                            int nUpgrade = pUserInfo->GetAttackUpgrade() * 5; //100으로 처리하자.
//                            pNumCtl->SetNumber(nUpgrade,CNumberCtlType_Percent);
//                            pBtnSlot->GetChild(0)->GetChild(0)->SetHide(false);
//                            
//                        }
//                        else if(nID == ID_ITEM_UPGRADE_DEPEND)
//                        {
//                            CNumberCtl* pNumCtl = (CNumberCtl*)pBtnSlot->GetChild(0)->GetChild(0);
//                            int nUpgrade = pUserInfo->GetDependUpgrade() * 5; //100으로 처리하자.
//                            pNumCtl->SetNumber(nUpgrade,CNumberCtlType_Percent);
//                            pBtnSlot->GetChild(0)->GetChild(0)->SetHide(false);
//                            
//                        }
//                        else if(nID == ID_ITEM_UPGRADE_DEFENCE_TOWER || nID == ID_ITEM_UPGRADE_MISSILE_TOWER)
//                        {
//                            CNumberCtl* pNumCtl = (CNumberCtl*)lstSlotCtl[i]->GetChild(0)->GetChild(0);
//                            int nUpgrade = pUserInfo->GetCntByIDWithItem(nID) * 50; //100으로 처리하자.
//                            pNumCtl->SetNumber(nUpgrade,CNumberCtlType_Percent);
//                            lstSlotCtl[i]->GetChild(0)->GetChild(0)->SetHide(false);
//                        }
//                        else
//                        {
//                            //pNumCtl->ClearNumber();
//                            pBtnSlot->GetChild(0)->GetChild(0)->SetHide(true);
//                        }
//                    }
//                    else
//                    {
//                        //                pNumCtl->ClearNumber();
//                        pBtnSlot->GetChild(0)->GetChild(0)->SetHide(true);
//                    }
//                }
//                else if(nType == 0) //Bomb
//                {
//                    int nID = lstItem->at(nIndex)->nID;
//                    PROPERTY_BOMB property;
//                    PROPERTY_BOMB::GetPropertyBomb(nID, property);
//                    
//                    lstImage.clear();
//                    lstImage.push_back(property.sBombTableImgPath);
//                    lstImage.push_back("none");
//                    pBtnSlot->GetChild(0)->SetImageData(lstImage);
//                    //폭탄이미지는 0.0,0.0,1.0,1.0 이 아니다.
//                    //0.2265625 => 0.1265625 (비율때문)
//                    //0.7734375 => 0.8734375 (비율때문)
//                    pBtnSlot->GetChild(0)->SetBackCoordTex(0.20703125, 0.1265625, 0.79296875, 0.8734375);
//                    pBtnSlot->SetLData((long)lstItem->at(nIndex));
//                    pBtnSlot->SetButtonType(BUTTON_GROUP_TOGGLE);
//                    
//                    pBtnSlot->SetLData2(property.nType); //카운터.. 비카운터
//                    if(nBombID == nID)
//                    {
//                        pBtnSlot->SetPushed(true);
//                        mpSelectedWeaponBtn = pBtnSlot;
//                    }
//                    else
//                        pBtnSlot->SetPushed(false);
//                    
//                    
//                    if(property.nType == 0) //Count
//                    {
//                        CNumberCtl* pNumCtl = (CNumberCtl*)lstSlotCtl[i]->GetChild(0)->GetChild(0);
//                        pNumCtl->SetNumber(pUserInfo->GetCntByIDWithBomb(nID));
//                        pBtnSlot->GetChild(0)->GetChild(0)->SetHide(false);
//                    }
//                    else
//                    {
//                        pBtnSlot->GetChild(0)->GetChild(0)->SetHide(true);
//                    }
//                }
//                else if(nType == 2)
//                {
//                    USERINFO* pInfo = lstItem->at(nIndex);
//                    int nID = pInfo->nID;
//                    PROPERTY_TANK property;
//                    PROPERTY_TANK::GetPropertyTank(nID, property);
//                    
//                    lstImage.clear();
//                    sPath = property.sModelPath;
//                    sPath.append(".tga");
//                    lstImage.push_back(sPath);
//                    lstImage.push_back("none");
//                    
//                    pBtnSlot->GetChild(0)->SetImageData(lstImage);
//                    pBtnSlot->GetChild(0)->SetBackCoordTex(0.0f, 0.0f, 0.96875f, 0.96875f);
//                    pBtnSlot->GetChild(0)->GetChild(1)->SetHide(false);
//                    pBtnSlot->SetLData((long)pInfo);
//                    pBtnSlot->SetPushed(false);
//                    
//                    pBtnSlot->SetButtonType(BUTTON_NORMAL);
//                    
//                    if(pInfo->nType == 0) //Count
//                    {
//                        CNumberCtl* pNumCtl = (CNumberCtl*)lstSlotCtl[i]->GetChild(0)->GetChild(0);
//                        pNumCtl->SetNumber(pInfo->nCnt);
//                        pBtnSlot->GetChild(0)->GetChild(0)->SetHide(false);
//                    }
//                    else
//                    {
//                        pBtnSlot->GetChild(0)->GetChild(0)->SetHide(true);
//                    }
//                }
//                else if(nType == 3)
//                {
//                    USERINFO* pInfo = lstItem->at(nIndex);
//                    int nID = pInfo->nID;
//                    if(nID == RUNTIME_UPDATE_REMOVE) //현재 건물을 제거한다.
//                    {
//                        lstImage.clear();
//                        sPath = "RevTower"; //RUNTIME_UPDATE_REMOVE 아이콘 패스
//                        sPath.append(".tga");
//                        lstImage.push_back(sPath);
//                        lstImage.push_back("none");
//                        
//                        pBtnSlot->GetChild(0)->SetImageData(lstImage);
//                        pBtnSlot->GetChild(0)->SetBackCoordTex(0.0f, 0.0f, 0.96875f, 0.96875f);
//                        pBtnSlot->SetLData((long)pInfo);
//                        pBtnSlot->SetPushed(false);
//                        
//                        pBtnSlot->GetChild(0)->GetChild(0)->SetHide(true);
//                        pBtnSlot->SetButtonType(BUTTON_NORMAL);
//                    }
//                    
//                }
//            }
//        }
//        
//        if(nType == 1)
//            pUserInfo->SetLastSelectedItemPageIndex(nSelIndex);
//        else if(nType == 0)
//            pUserInfo->SetLastSelectedBombPageIndex(nSelIndex);
//        else if(nType == 2)
//            pUserInfo->SetLastSelectedBlzPageIndex(nSelIndex);
//        
//        mpItemSlotBar->SetHide(false);
//    }
//    else if(mStep == TRAININGSTEP_MAKETOWER)
//    { 
//        vector<string>  lstImage;
//        int nMaxPageCnt = 0;
//        const int nMaxItem = MAXSLOTCNT;
//        vector<CControl*> lstSlotCtl;
//        CUserInfo* pUserInfo = mpScenario->GetUserInfo();
//        vector<USERINFO*>  lstType3Org;
//        vector<USERINFO*>* lstItem = NULL;
//        int nType = 2;
//        
//        
//        //스롯의 내용을 하나만 정의 하자.
//        lstItem = pUserInfo->GetListBlz();
//        
//        
//        for(vector<USERINFO*>::iterator it = lstItem->begin(); it != lstItem->end();)
//        {
//            if((*it)->nID != ACTORID_BLZ_DTOWER )
//                it = lstItem->erase(it);
//            else if((*it)->nID == ACTORID_BLZ_DTOWER)
//            {
//                (*it)->nCnt = 1;
//                it++;
//            }
//            else
//                it++;
//        }
////        lstItem->erase(lstItem->begin());
//        
//        mpItemSlotSwitchBtn->SetEnable(false);
//        
//        CNumberCtl* pItemPageNumber = (CNumberCtl*)mpItemSlotBar->FindControl(CTL_TABLE_ITEM_SLOT_PAGE);
//        
//        int nSelIndex = 1;
//        nSelIndex = pUserInfo->GetLastSelectedBlzPageIndex();
//        
//        lstImage.clear();
//        lstImage.push_back("res_slot.png");
//        
//        if(nType == 2)
//            lstImage.push_back("M_Slot_BLZ");
//        
//        mpItemSlotSwitchBtn->SetImageData(lstImage);
//        
//        nMaxPageCnt = (int)lstItem->size() / nMaxItem;
//        if((lstItem->size() % nMaxItem) != 0)
//            nMaxPageCnt += 1;
//        
//        if(nSelIndex > nMaxPageCnt)
//            nSelIndex = nMaxPageCnt;
//        else if( nSelIndex < 1)
//            nSelIndex = 1;
//        
//        pItemPageNumber->SetNumber(nSelIndex);
//        
//        
//        CControl* pUp = mpItemSlotBar->FindControl(BTN_ITEMSLOTUPPAGE);
//        CControl* pDown= mpItemSlotBar->FindControl(BTN_ITEMSLOTDOWNPAGE);
//        
//        if(nSelIndex == 1)
//            pDown->SetEnable(false);
//        else
//            pDown->SetEnable(true);
//        
//        if(nSelIndex == nMaxPageCnt)
//            pUp->SetEnable(false);
//        else
//            pUp->SetEnable(true);
//        
//        
//        for(int i = CTL_TABLE_ITEM_SLOT3; i <= CTL_TABLE_ITEM_SLOT6; i++)
//        {
//            lstSlotCtl.push_back(mpItemSlotBar->FindControl(i));
//        }
//        
//        int nIndex = 0;
//        int nMaxCnt = (int)lstItem->size();
//        string sPath;
//        CButtonCtl* pBtnSlot = NULL;
//        for (int i = 0; i < nMaxItem; i++)
//        {
//            nIndex = (nSelIndex - 1) * nMaxItem + i;
//            pBtnSlot = (CButtonCtl*)lstSlotCtl[i];
//            
//            if(nIndex >= nMaxCnt || nMaxCnt == 0)
//            {
//                lstImage.clear();
//                lstImage.push_back("none");
//                lstImage.push_back("none");
//                pBtnSlot->GetChild(0)->SetImageData(lstImage);
//                pBtnSlot->GetChild(0)->GetChild(0)->SetHide(true);
//                pBtnSlot->GetChild(0)->GetChild(1)->SetHide(true);
//                pBtnSlot->SetLData(-1);
//                
//                pBtnSlot->SetButtonType(BUTTON_NORMAL);
//                pBtnSlot->SetPushed(false);
//                pBtnSlot->SetEnable(false);
//            }
//            else
//            {
//                pBtnSlot->SetEnable(true);
//                pBtnSlot->GetChild(0)->GetChild(1)->SetHide(true);
//                if(nType == 2)
//                {
//                    USERINFO* pInfo = lstItem->at(nIndex);
//                    int nID = pInfo->nID;
//                    PROPERTY_TANK property;
//                    PROPERTY_TANK::GetPropertyTank(nID, property);
//                    
//                    lstImage.clear();
//                    sPath = property.sModelPath;
//                    sPath.append(".tga");
//                    lstImage.push_back(sPath);
//                    lstImage.push_back("none");
//                    
//                    pBtnSlot->GetChild(0)->SetImageData(lstImage);
//                    pBtnSlot->GetChild(0)->SetBackCoordTex(0.0f, 0.0f, 0.96875f, 0.96875f);
//                    pBtnSlot->SetLData((long)pInfo);
//                    pBtnSlot->SetPushed(false);
//                    
//                    pBtnSlot->SetButtonType(BUTTON_NORMAL);
//                    
//                    if(pInfo->nType == 0) //Count
//                    {
//                        CNumberCtl* pNumCtl = (CNumberCtl*)lstSlotCtl[i]->GetChild(0)->GetChild(0);
//                        pNumCtl->SetNumber(pInfo->nCnt);
//                        pBtnSlot->GetChild(0)->GetChild(0)->SetHide(false);
//                    }
//                    else
//                    {
//                        pBtnSlot->GetChild(0)->GetChild(0)->SetHide(true);
//                    }
//                }
//            }
//        }
//        //pUserInfo->SetLastSelectedBlzPageIndex(nSelIndex)
//        
//        mpItemSlotBar->SetHide(false);
//    }
//}
//
//#ifdef ANDROID
//void CTrainingWorld::ResetTexture()
//{
//    CHWorld::ResetTexture();
//    mnFingerTexture = GetSGLCore()->GetTextureMan()->GetTextureID("Finger.tga");
//    
//    if(mpMessageDlg) mpMessageDlg->ResetTexture();
//    if(mpStepMessageDlg) mpStepMessageDlg->ResetTexture();
//}
//#endif

#endif //New_CTrainingWorld
