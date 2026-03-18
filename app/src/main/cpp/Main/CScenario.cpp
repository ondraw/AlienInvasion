//
//  CScenario.cpp
//  SongGL
//
//  Created by 송 호학 on 11. 11. 28..
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//
#include <math.h>
#include "CScenario.h"
#include "CLogoWorld.h"
#include "CProgressWorld.h"
#include "CHWorld.h"
#include "CTrainingWorld.h"
#include "CMainMenuWorld.h"
#include "sGLText.h"
#include "CControl.h"
#include "CMutiplyProtocol.h"
#ifdef ANDROIDADMOB
#include "SongGLJNI.h"
#endif //ANDROIDADMOB
#if defined(IPHONE) || defined(ANDROID)
#include "AlienInvasionProtocolMan.h"
#endif

bool CScenario::gbNeedCompletedMap = false;

HQueue<SGLEvent>  CScenario::gEventQue;
pthread_mutex_t   CScenario::gEventQueMutex;
CScenario::CScenario()
{   
    mCurrentWorld = new CLogoWorld(this);
    mCurrentWorld->ResetCamera(); //시나리오에서 가장 처음 실행할때 (2D)는 max값을 구해와야한다. 그값은 프로젝션함수를 콜하는데 카메라정보를 사용한다.
    
    mNextWorld = NULL;
    mProgressWorld = NULL;
    SetStep(CSCENARIO_LOGO);
    mGoToStep = CSCENARIO_NONE;
    mStepOrg = CSCENARIO_NONE;
    mMultiplayProtocol = new CMutiplyProtocol(this);
#if defined(IPHONE) || defined(ANDROID)
    mpProtocolMan = NULL;
#endif
//    mUserInfo =  NULL;
    pthread_mutex_init(&gEventQueMutex, NULL);

    
}

CScenario::~CScenario()
{
    pthread_mutex_destroy(&gEventQueMutex);
    
    if(mMultiplayProtocol)
    {
        delete mMultiplayProtocol;
        mMultiplayProtocol = NULL;
    }
    
    if(mCurrentWorld && mProgressWorld != mCurrentWorld)
    {
        delete mCurrentWorld;
        mCurrentWorld = NULL;
    }
    
    if(mNextWorld)
    {
        delete mNextWorld;
        mNextWorld = NULL;
    }
    
    if(mProgressWorld)
    {
        delete mProgressWorld;
        mProgressWorld = NULL;
    }
    
//    if(mUserInfo)
//    {
//        delete mUserInfo;
//        mUserInfo = NULL;
//    }
    PROPERTYI::ReleaseProperty();
    
    //프라퍼티를 릴리즈해준다.
    //ReleaseProperty();
    
    
#if defined(IPHONE) || defined(ANDROID)
    if(mpProtocolMan)
    {
        delete mpProtocolMan;
        mpProtocolMan = NULL;
    }
#endif
    
    CScenario::ClearQue();
}

void CScenario::ClearQue()
{
    pthread_mutex_lock(&gEventQueMutex); //락
    int nSize = gEventQue.Size();
    for(int i = 0; i < nSize; i++)
    {
        SGLEvent* pEvent = gEventQue.GetAt(i);
        delete pEvent;
    }
    gEventQue.Clear();
    pthread_mutex_unlock(&gEventQueMutex); //언락
}

int CScenario::Initialize(void* pResource)
{
    msRootPath = (char*)pResource;

    mCurrentWorld->Initialize(pResource);
    
#if defined(IPHONE) || defined(ANDROID)
    if(mpProtocolMan == NULL)
        mpProtocolMan = new AlienInvasionProtocolMan(GetUserInfo());
#endif
    return E_SUCCESS;
}

void CScenario::InitializeByResized()
{
    mCurrentWorld->InitializeByResized();
}

//윈도우 사이즈가 변경되었을 때 발생한다.
int CScenario::Resized(int nWidth,int nHeight)
{
    if(mCurrentWorld)
        return mCurrentWorld->Resized(nWidth,nHeight);
    return E_SUCCESS;   
}

int CScenario::getResetFrustum()
{
    if(mCurrentWorld)
        return mCurrentWorld->getResetFrustum();
    return E_SUCCESS;
}

void CScenario::ResetFrustum(int bFrustum)
{
    if(mCurrentWorld)
        mCurrentWorld->ResetFrustum(bFrustum);
}

//윈도우즈사이즈가 변경되거나, 카메라의 원거리가 변경되거나 할때 프로젝션을 재설정한다.
int CScenario::initProjection()
{
    if(mCurrentWorld)
        return mCurrentWorld->initProjection();
    return E_SUCCESS;
}

void CScenario::ResetCamera()
{
    if(mCurrentWorld)
        mCurrentWorld->ResetCamera();
}

void CScenario::Clear()
{
    if(mCurrentWorld)
        mCurrentWorld->Clear();
}


void CScenario::SetCamera(SGLCAMERA* pCam)
{
    if(mCurrentWorld)
        mCurrentWorld->SetCamera(pCam);
}

SGLCAMERA* CScenario::GetCamera()
{
    if(mCurrentWorld)
        return mCurrentWorld->GetCamera();
    return NULL;
}

int CScenario::ActivateCamera()
{
    if(mCurrentWorld)
        return mCurrentWorld->ActivateCamera();
    return E_SUCCESS;
}


int CScenario::RenderBegin()
{ 
    if(mCurrentWorld)
        mCurrentWorld->RenderBegin();
    
    unsigned long nFindWaitTime = mMultiplayProtocol->GetFindWaitTime();
    unsigned long nNow = GetGGTime();
    if(nFindWaitTime && nNow > nFindWaitTime)
    {
        mMultiplayProtocol->SetFindWaitTime(0l);
        if(mCurrentWorld->GetWorldType() == WORLD_TYPE_MENU)
        {
            ((CMainMenuWorld*)mCurrentWorld)->StartPlayNoMatching();
        }
        else
        {
            if(mMultiplayProtocol->GetMultiplayState() != SYNC_FINDPLAYER) //Finding진행중일때는
            {
                if(gFindPlyer)
                    gFindPlyer(20,0,mMultiplayProtocol->GetFindMatchingGroup());
            }
            else
            {
                mMultiplayProtocol->SetFindWaitTime(nNow + 20000l); //20초 다시 확인
            }
        }
    }
    
    
    return E_SUCCESS;
}

int CScenario::Render()
{ 
    if(mCurrentWorld)
        return mCurrentWorld->Render();
    
    return E_SUCCESS;
}

int CScenario::RenderEnd()
{ 
    if(mCurrentWorld)
    {
        mCurrentWorld->RenderEnd();
        
        //보내는 싱크에 대한 로직을 처리한다.
        mMultiplayProtocol->SendSyncNoPlay();
        mMultiplayProtocol->RcvSyncEvent();
        
        while(true)
        {
            pthread_mutex_lock(&gEventQueMutex); //락
            if(gEventQue.Size() == 0)
            {
                pthread_mutex_unlock(&gEventQueMutex); //언락
                break;
            }
            SGLEvent* pEvent = gEventQue.GetAt(0);// 드래그는 여러개가 쌓인다.
            pthread_mutex_unlock(&gEventQueMutex); //언락
            
            if(pEvent->nMsgID == SGL_MESSAGE_DRAG)
            {
                CControl *pCtl = dynamic_cast<CControl*>((CControl*)pEvent->lParam);
                if(pCtl)
                    pCtl->OnDragInvalidate(pEvent->lParam2,pEvent->lParam3);
            }
            else
                OnEventProgress(pEvent);

            pthread_mutex_lock(&gEventQueMutex); //락
            gEventQue.Remove(0);
            pthread_mutex_unlock(&gEventQueMutex); //언락
            delete pEvent;
//            break; //메세지큐가 많이 쌓이게 된다. 한꺼번에 처리를 해준다.
        }

    }
    return E_SUCCESS;
}


void CScenario::SetActor(CSprite* pSprite) 
{
    if(mCurrentWorld)
        return mCurrentWorld->SetActor(pSprite);
}

CSprite* CScenario::GetActor() 
{
    if(mCurrentWorld)
        return mCurrentWorld->GetActor();
    return NULL;
} 

void CScenario::SetStatus(int v)
{
    if(mCurrentWorld)
        mCurrentWorld->SetStatus(v);
}

int CScenario::GetStatus()
{
    if(mCurrentWorld)
        return mCurrentWorld->GetStatus();
    return 0;

}

int CScenario::OnEvent(SGLEvent *pEvent)
{ 
    if(mCurrentWorld)
        return mCurrentWorld->OnEvent(pEvent);
    return E_SUCCESS;
}


CTextureMan* CScenario::GetTextureMan()
{ 
    if(mCurrentWorld)
        return mCurrentWorld->GetTextureMan();
    return NULL;
}

CModelMan* CScenario::GetModelMan()
{ 
    if(mCurrentWorld)
        return mCurrentWorld->GetModelMan();
    return NULL;
}

void CScenario::BeginTouch(long lTouchID,float x, float y)
{
    if(mCurrentWorld)
        mCurrentWorld->BeginTouch(lTouchID,x,y);
}

void CScenario::MoveTouch(long lTouchID,float x, float y)
{
    if(mCurrentWorld)
        mCurrentWorld->MoveTouch(lTouchID,x,y);

}
void CScenario::EndTouch(long lTouchID,float x, float y)
{
    if(mCurrentWorld)
        mCurrentWorld->EndTouch(lTouchID,x,y);
}

void CScenario::SendMessage(int nMsgID,long lParam,long lParam2,long lParam3,long lObject)
{
    SGLEvent* pEvent = new SGLEvent;
    
    pEvent->nMsgID = nMsgID; //프로그래스를 시작한다.
    pEvent->lParam  =   lParam;
    pEvent->lParam2 =   lParam2;
    pEvent->lParam3 =   lParam3;
    pEvent->lObject =   lObject;
    pthread_mutex_lock(&gEventQueMutex); //락
    gEventQue.Offer(pEvent);
    pthread_mutex_unlock(&gEventQueMutex); //언락
}

//프로그래스 처리를 한다.
void CScenario::OnEventProgress(SGLEvent* pEvent)
{

    IHWorld *pTemp;
    switch (pEvent->nMsgID) {
        case GSL_PROGRESS_START: //프로그래스바를 시작한다.
            //일단 객체를 생성한다.
            mNextWorld->ResetCamera();
            SendMessage(SGL_PROGRESS_INITIALIZE);//다음화면을 초기화한다.
            break;
            
        case SGL_PROGRESS_INITIALIZE:
            mNextWorld->Initialize((char*)msRootPath.c_str());
            if(mNextWorld->GetStatus() & 0x0002)
            {
                //일단은 너무 빠르게 이동하면은... 허하니 프로그래스바를 완성시켜준다.
                ((CProgressWorld*)mProgressWorld)->SetProgressPos(50);
                ((CProgressWorld*)mProgressWorld)->SetProgressPos(100);
                SendMessage(SGL_PROGRESS_END); //화면전환을 하여 프로그래스를 종료한다.
            }
            break;
        case SGL_PROGRESS_INITIALIZE_1:
            ((CWorld*)mNextWorld)->Initialize1(pEvent);
            break;
        case SGL_PROGRESS_INITIALIZE_2:
            ((CWorld*)mNextWorld)->Initialize2(pEvent);
            break;
        case SGL_PROGRESS_INITIALIZE_3:
            ((CWorld*)mNextWorld)->Initialize3(pEvent);
            break;
        case SGL_PROGRESS_END:

            mCurrentWorld = mNextWorld;
            mNextWorld = NULL;
            pTemp = mProgressWorld;
            mProgressWorld = NULL;
            delete pTemp;
            SetStep(mGoToStep); //현재는 프로그래스 상태로 변경한다.
            mGoToStep = CSCENARIO_NONE;
            break;
        default:
            if(pEvent->nMsgID & 0x020000) //화면 이벤트
                mCurrentWorld->OnEvent(pEvent);
            break;
    }
  
}

#define  MaxRank  19

int CScenario::IsPromotionRank()
{

    CUserInfo* pUserInfo = GetUserInfo();
    int nCurrentRank = pUserInfo->GetRank();
    
    const long nDummy[MaxRank] = {
        0,0,0,0,
        0,0,0,0,0,
        0,0,0,
        0,0,0,
        0,200000,400000,600000
    };
    
    if(nCurrentRank == 0) return 0; //훈련병은 진급을 못한다. 훈련코스가 완료되면 진급해준다.
    
//#ifdef DEBUG
//    const char fBaeS[][MaxRank] = {
//        "이병","일병","상병","병장",
//        "하사","중사","상사","원사","준위",
//        "소위","중위","대위",
//        "소령","중령","대령",
//        "소장","준장","중장","대장"
//    };
//    
//    
//    for (int i = 0; i < MaxRank;i++)
//    {
//        int R = i + 1;
//        long fNow = 400 * R * R + nDummy[i];
//        HLogD("%02d:%s = %12ld,\tAP=%f,\tDP=%3.3f\tLevel=%d\n",i+1,fBaeS[i],fNow, DAttackLevelToPower(DRankToLevel(R)), DDefenceLevelToPower(DRankToLevel(R)), (int)DRankToLevel(R));
//    }
//#endif

    
//    Rank (n:점수 R:랭크)
//    n = 200 * R * R
    int nTotalScore = pUserInfo->GetTotalScore();
    int nScoreRank = 1;
    long fScore = 0;
    for (int i = 0; i < MaxRank;i++)
    {
        int R = i + 1;
        fScore = 400 * R * R + nDummy[i];
        if(nTotalScore >= fScore)
        {
            nScoreRank = R;
        }
    }
    
    if(nScoreRank > nCurrentRank) return nScoreRank;
    return 0;
}

const char* GetRankPath(int nRank)
{
    static char sFileName[20];
//    const int nMaxLevel = 7;
    if(nRank > MaxRank)
    {
        nRank = MaxRank;
    }
    sprintf(sFileName, "rank_%02d.tga",nRank);
    return sFileName;
}

const char* CScenario::GetRankPath()
{
    CUserInfo* pUserInfo = GetUserInfo();
    return ::GetRankPath(pUserInfo->GetRank());
}



void CScenario::GoToWorld(IHWorld *pWorld,CSCENARIO_STEP goScen)
{
    IHWorld *pCurr = mCurrentWorld;
    CUserInfo* pUserInfo = GetUserInfo();
    CSCENARIO_STEP CurrStep = GetStep();
    
    mStepOrg = CurrStep; //종료시점에 어디서 넘어왔는 지를 알아야 한다.
    
    //이미 화면 변환작업을 하고 있기때문에 다른 명령어는 취소한다.
    if(mNextWorld) return;
    
    if(mProgressWorld == NULL)
    {
        //프로그래스 클래스를 만든다.
        mProgressWorld = new CProgressWorld(this);
        mProgressWorld->ResetCamera();
        mProgressWorld->Initialize((void*)msRootPath.c_str());
    }
    
    mGoToStep = goScen;
    mCurrentWorld = mProgressWorld; //프로그래스화면으로 전환한다.
    SetStep(CSCENARIO_PROGRESS); //현재는 프로그래스 상태로 변경한다.
    delete pCurr; //현재 화면의 메모리를 해제한다.
    
    switch (goScen)
    {
        case CSCENARIO_MAINMENU:  //메뉴 => 플레이
            mNextWorld = new CMainMenuWorld(this);
            if(pUserInfo->GetCntByIDWithItem(ID_ITEM_REMOVE_AD) == 0)
            {
                sglShowAD(true);
            }
            else
            {
                sglShowAD(false);
            }
#if defined(IPHONE) || defined(ANDROID)
            if(GetUserInfo()->mnBGetherTime)
                GetUserInfo()->SetTotalPlayTime((GetGGTime() - GetUserInfo()->mnBGetherTime) / 1000);
            if(mpProtocolMan) mpProtocolMan->SendUserInfo();
#endif

            SendMessage(GSL_PROGRESS_START);
            break;
        case CSCENARIO_SINGLEGAME: //플레이 => 메뉴
#ifdef MAC
            CControl::gLastSelControl = NULL;
#endif
            mNextWorld = new CHWorld(this);
#if ALWAYSAD == 0
            if(pUserInfo->GetCntByIDWithItem(ID_ITEM_REMOVE_AD) == 0)
            {
                sglShowAD(false);
            }
#endif //ALWAYSAD
            SendMessage(GSL_PROGRESS_START); //프로그래스를 시작한다.
            break;
        case CSCENARIO_TRAININGCENTER: //플레이 => 메뉴
#ifdef MAC
            CControl::gLastSelControl = NULL;
#endif
            mNextWorld = new CTrainingWorld(this);
            if(pUserInfo->GetCntByIDWithItem(ID_ITEM_REMOVE_AD) == 0)
            {
                sglShowAD(false);
            }
            SendMessage(GSL_PROGRESS_START); //프로그래스를 시작한다.
            break;
        default:
            break;
    }
}

void CScenario::Serialize(CArchive& ar,bool bWrite)
{
    IHWorld *pCurr = (IHWorld*)mCurrentWorld;
    if(pCurr)
    {
        pCurr->Serialize(ar,bWrite);
    }
}

#ifdef ANDROID
void CScenario::ResetTexture()
{
    IHWorld *pCurr = (IHWorld*)mCurrentWorld;
    if(pCurr)
    {
        pCurr->ResetTexture();
    }
}

GLuint CScenario::SetCurrentTexture(const char* sKey,unsigned char* sPixels,int nW,int nH,int nDepth)
{
    IHWorld *pCurr = (IHWorld*)mCurrentWorld;
    if(pCurr)
    {
        enum AndroidBitmapFormat {
            ANDROID_BITMAP_FORMAT_NONE      = 0,
            ANDROID_BITMAP_FORMAT_RGBA_8888 = 1,
            ANDROID_BITMAP_FORMAT_RGB_565   = 4,
            ANDROID_BITMAP_FORMAT_RGBA_4444 = 7,
            ANDROID_BITMAP_FORMAT_A_8       = 8,
        };
        if(nDepth == ANDROID_BITMAP_FORMAT_RGBA_8888)
            nDepth = 32;
        else
        {
            HLogE("[ERROR] Not Found Android Format %d",nDepth);
            nDepth = 24;
        }
        
        CTextureMan* pMan = pCurr->GetTextureMan();
        return pMan->GetTextureID(sKey,sPixels,nW,nH,nDepth,0,0,false);
    }
    return 0;
}
#endif

#if defined(IPHONE) || defined(ANDROID)
void CScenario::SendTrainningStep(int nStep)
{
    if(mpProtocolMan) mpProtocolMan->SendTrainningStep(nStep);
}
#endif

float CScenario::GetViewAperture()
{
    IHWorld* pCurr = mCurrentWorld;
    if(pCurr)
        return pCurr->GetViewAperture();
    return GetViewAperture();
}

#ifdef ACCELATOR
void CScenario::SetAccelatorUpVector(float fv)
{
    IHWorld* pCurr = mCurrentWorld;
    if(pCurr)
        pCurr->SetAccelatorUpVector(fv);
}
#endif //ACCELATOR
