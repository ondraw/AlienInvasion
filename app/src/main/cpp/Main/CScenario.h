//
//  CScenario.h
//  SongGL
//  theK의 시나리오를 구성하여 
//  1> 로고를 만들어서 화면에 뿌려준후
//  2> 프로그래스바를 만들어서 메뉴 화면 구성하는데 필요한.. 거시기를 만든다.
//  Created by 송 호학 on 11. 11. 28..
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef SongGL_CScenario_h
#define SongGL_CScenario_h
#include <string>
#include "IHWorld.h"
#include "hQueue.h"
#include "CUserInfo.h"


enum CSCENARIO_STEP { 
    CSCENARIO_NONE,//아무런 작업을 하지 않는 초기상태
    CSCENARIO_LOGO,//로고 화면 => 프로그래스(CSCENARIO_FROM_LOGO_TO_MENU1_PROGRESS) => 메뉴1
    CSCENARIO_PROGRESS,
    CSCENARIO_MAINMENU,
    CSCENARIO_SINGLEGAME,
    CSCENARIO_TRAININGCENTER
};

class CMutiplyProtocol;
extern const char* GetRankPath(int nRank);
using namespace std;
#if defined(IPHONE) || defined(ANDROID)
class AlienInvasionProtocolMan;
#endif
class CUserInfo;
class CScenario : public IHWorld
{
public:
    CScenario();
    virtual ~CScenario();
    virtual int GetWorldType() { return WORLD_TYPE_CScenario;}
    
    virtual int Initialize(void* pResource); //리소스를 읽어온다.
    virtual void InitializeByResized();
	
    virtual void ResetCamera();
	//윈도우 사이즈가 변경되었을 때 발생한다.
	virtual int Resized(int nWidth,int nHeight);
    virtual int getResetFrustum();
	virtual void ResetFrustum(int bFrustum);
	//윈도우즈사이즈가 변경되거나, 카메라의 원거리가 변경되거나 할때 프로젝션을 재설정한다.
	virtual int initProjection();
	
    virtual unsigned int GetNewID() { return 1;}
    
    virtual void SetCamera(SGLCAMERA* pCam);
	virtual SGLCAMERA* GetCamera();
    virtual int ActivateCamera();
    
    //주인공을 설정한다.
    virtual void SetActor(CSprite* pSprite);	
    virtual CSprite* GetActor(); 
    virtual int OnEvent(SGLEvent *pEvent);
    virtual void Clear();
    virtual int RenderBegin();
    virtual int Render();
    virtual int RenderEnd();    
    virtual CTextureMan* GetTextureMan();
    virtual CModelMan* GetModelMan();
    virtual void BeginTouch(long lTouchID,float x, float y);
    virtual void MoveTouch(long lTouchID,float x, float y);
    virtual void EndTouch(long lTouchID,float x, float y);
    
    virtual void SetStatus(int v);
    virtual int GetStatus();
    virtual void GoToWorld(IHWorld *pWorld,CSCENARIO_STEP goScen);
    
    CSCENARIO_STEP GetStep() { return mStep;}
    void SetStep(CSCENARIO_STEP v) { mStep = v;}
    
    void OnEventProgress(SGLEvent* pEvent);
    
    static void SendMessage(int nMsgID,long lParam = 0,long lParam2 = 0,long lParam3 = 0,long lObject = 0);
    static void ClearQue();
    
    virtual CUserInfo* GetUserInfo() { return CUserInfo::GetDefaultUserInfo();}

#ifdef ANDROID
    virtual void ResetTexture();
    GLuint SetCurrentTexture(const char* sKey,unsigned char* sPixels,int nW,int nH,int nDepth);
#endif
    
    int IsPromotionRank(); // not 0 : 지급그함
    const char* GetRankPath();
//    const char* GetRankPath(int nRank);
    
    
    static bool gbNeedCompletedMap; 
    
#if defined(IPHONE) || defined(ANDROID)
    void SendTrainningStep(int nStep);
#endif
    
    
#ifdef ACCELATOR
    virtual void SetAccelatorUpVector(float fv);
#endif //ACCELATOR

    virtual float GetViewAperture();
    
    virtual void Serialize(CArchive& ar,bool bWrite);
    virtual CMutiplyProtocol* GetMutiplyProtocol() { return mMultiplayProtocol;}
    
    IHWorld* GetCurrentWorld() { return mCurrentWorld;}
    IHWorld* GetNextWorld() { return mNextWorld;}
protected:
    // 0x0000 초기화가 안됨, 0x0001 초기화중, 0x0002 초기화가 됨
//	int mnStatus;
    string msRootPath;

    
    IHWorld* mCurrentWorld;
    IHWorld* mNextWorld;
    IHWorld* mProgressWorld;
    
    CSCENARIO_STEP mStepOrg; //화면 시점에 어디서 넘어왔는 지를 알아야 한다.
    CSCENARIO_STEP mStep;
    CSCENARIO_STEP mGoToStep;
//    CUserInfo*     mUserInfox;

    
    static HQueue<SGLEvent>  gEventQue;
    static pthread_mutex_t   gEventQueMutex;
    
#if defined(IPHONE) || defined(ANDROID)
    AlienInvasionProtocolMan* mpProtocolMan;
#endif
    CMutiplyProtocol* mMultiplayProtocol;
    

};


#endif
