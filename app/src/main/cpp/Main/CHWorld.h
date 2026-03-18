/*
 *  untitled.h
 *  SongGL
 *
 *  Created by 호학 송 on 10. 11. 10..
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef _CHWORLD_H_
#define _CHWORLD_H_

#include <strings.h>
#include "ListNode.h"
#include "sGLDefine.h"
#include "CSprite.h"
#include "CBomb.h"
#include "IAction.h"
#include "CSThread.h"
#include "hQueue.h"
#include "ListNode.h"
#include "C2dRadaBoard.h"
#include "C2dStick.h"
#include "AParticle.h"
#include "CWorld.h"
#include "CPicking.h"
#include "CMissionEndCtl.h"

extern void* CoreThread(void *data);

#define BTN_PLAY_TOP_GOLD_NUM       1001
#define BTN_PLAY_HOME               1002
#define BTN_PLAY_SETTINGS           1003
#define BTN_PLAY_CAMERA             1004


#define BTN_PLAY_BOM_TABLE          1006
#define BTN_PLAY_BOM_SEL            1007
#define BTN_PLAY_BOM_SEL_CNT        1008

#define BTN_PLAY_CONFIRM_YES        1011
#define BTN_PLAY_CONFIRM_NO         1012
#define BTN_PLAY_CONFIRM_Retry      1013
#define BTN_PLAY_CONFIRM_Succ_YES   1014

#define BTN_PLAY_GoTo_Menu          1015 //play에서 메뉴화면으로 변경할때.



#define BTN_ITEMSLOTUPPAGE                      1016
#define BTN_ITEMSLOTDOWNPAGE                    1017
#define BTN_ITEMSLOTSWITCH                      1027
#define BTN_PLAY_PAUSE                          1028
#define BTN_PLAY_PAUSE_RESTART                  1029

//#define BTN_PLAY_BOM_SEL_MULTIPLAY              1030


#define CTL_TABLE_ITEM_SLOT_PAGE    2019
//#define CTL_TABLE_ITEM_SLOT1        2020 // 2026 까지 존재함.
//#define CTL_TABLE_ITEM_SLOT2        2021 // 2026 까지 존재함.
#define CTL_TABLE_ITEM_SLOT3        2022 // 2026 까지 존재함.
#define CTL_TABLE_ITEM_SLOT4        2023 // 2026 까지 존재함.
#define CTL_TABLE_ITEM_SLOT5        2024 // 2026 까지 존재함.
#define CTL_TABLE_ITEM_SLOT6        2025 // 2026 까지 존재함.


#define CTL_ENDISPLAY_NAME          2030  //적 디스플레이 네임
#define CTL_ENRANK_ICON             2032  //적 랭크


typedef struct _RuntimeRenderObjects 
{
	CListNode<CSprite> *plistSprite;
	CListNode<CBomb>   *plistBomb;
    CListNode<RadaBoardObject> *plistBoardObjects;
    CListNode<AParticle> *plistParticle;
}RuntimeRenderObjects;


class ARender;
class AMesh;
class CSGLCore;
class CTextureMan;
class CBackground;
class CNumberCtl;
class CButtonCtl;
class CTableCtl;
class CMessageCtl;
class CParticleEmitterMan;
class CImpactFlyParticle;
class CMissionEndCtl;
class CImgProgressCtl;
class CIndicatorCtl;
class CHWorld : public CWorld
{
	friend void* CoreThread(void *data);
    
public:
	CHWorld(CScenario* pScenario);
	virtual ~CHWorld();
	virtual int GetWorldType() { return WORLD_TYPE_H_WORLD;}
	//Function
public:
	virtual int Initialize(void* pResource); //리소스를 읽어온다.
    virtual int Initialize1(SGLEvent *pEvent);
    virtual int Initialize2(SGLEvent *pEvent);
    virtual int Initialize3(SGLEvent *pEvent);
    virtual void Init2DControl();
    virtual void ResetDisplayItemSlot();
    virtual void InitializeByResized();
    
	virtual void Clear();
    virtual int initProjection();
	virtual int RenderBegin();
	virtual int Render();
	virtual int RenderEnd();
	virtual int ActivateCamera();
    
	//주인공을 설정한다.
	virtual void SetActor(CSprite* pSprite);	
	virtual CSprite* GetActor(); 
	
	virtual int OnEvent(SGLEvent *pEvent);
	
	
    virtual CTextureMan* GetTextureMan();
    virtual CModelMan* GetModelMan();

    //화면 터치를 눌렀을 경우 좌우 스틱의 영역에 포함되는지를 체크 한다.
    virtual void BeginTouch(long lTouchID,float x, float y);
    virtual void MoveTouch(long lTouchID,float x, float y);
    virtual void EndTouch(long lTouchID,float x, float y);
    virtual CUserInfo* GetUserInfo() { return mpScenario->GetUserInfo();}
    virtual CSGLCore*               GetSGLCore() { return m_pSGLCore;}
    virtual void MakeActorAtStartPostion();
    virtual void SendMakeBlzMessage();
    
    CListNode<CSprite>*		GetSpriteList();
	
	//V : 1: 현재 액터가 바라보는 시점   2: 위에서 바라본다 (Debugging 형태)
	void SetCameraType(int v);
	int GetCameraType();	
   
    virtual unsigned int GetNewID() { return mMaxID++;}
    
    //미사일같은 것으로 움직일때..
    int GetPositionY(SPoint* pNow);
    
    //객체를 움직일때..
    virtual int GetMovePosiztionY(SPoint* pNow);


    //폭탄에 맞으면....흘들리는 현상
    void SetAnimationCameraAttacked();

    void RequestTry(CListNode<CSprite> *sp,CListNode<CBomb> *bm,CListNode<AParticle> *pa);
    
    CParticleEmitterMan* GetParticleEmitterMan() { return mpParticleMan;}
//    RuntimeRenderObjects	* GetCurrRuntimeRenderObjects() { return mpCurrRuntimeRenderObjects;}
    
    //지점에 가장가까운 적 팀의 객체를 찾는다.
    //nTeam : 자신의 팀,
    CSprite* GetTargetNearByPoint(unsigned char nTeam,float fx,float fz);
    
    //즉 아이템과 테이블 레이더를 제외한 곳에 클릭한것인지 확인한다.
    bool IsCenterClick(float fx,float fy);
    
    virtual void MakeRuntimeItem(SPoint* ptPostion);
    virtual void CheckAttectedByEnSpriteAtBomb(CListNode<CSprite> *pCoreListSprite,CBomb *pBomb);
    void CheckAttectedByEnSpriteAtRaser(CBomb* pBomb);
    
    void ShowMissionEndMessage(MissionEndCtlType type,int nScore,int nBonus,bool bTraining = false);
    virtual unsigned short GetMapLevel() { return mnMapLevel;}
    CPicking* GetPicking() { return &mPicking;}
    
    
    //동맹에 대한 설정.
    void InitAllians();
    void SetAllians(unsigned int nTeam,unsigned int nOtherTeam);
    void SetNoneAllians(unsigned int nTeam,unsigned int nOtherTeam);
    bool IsAllians(unsigned int nTeam,unsigned int nOtherTeam);
    
    virtual void SetBlzSlotCount(int nBlzID, bool bRemove,CSprite* pDeletedSprite);
    
    virtual void SetClickedSprite(CSprite* pClickedSprite);// { mpClickedSprite = pClickedSprite;}
    CSprite* GetClickedSprite() { return mpClickedSprite;}
    
    void TransferScoreToGold();
    void OnSelectedObjectShowDescBar(CSprite* pSprite,const char* sDesc = NULL);
    //레벨표시와 같이 보여지는 것이 아니라, 단순한 설명만 보여지게 하려면 false, 레벨과 같이 보여지게 하려면 true
    void OnSelectedObjectShowDescBar(const char* sDesc,bool bSpriteGroup = false);
    
    void RenderClickedSpriteSlot(CSprite* pSprite);
    void OnCompletedUpdated(CSprite* pSprite,int nID);
    void ResetClickedSpriteSlotFromMakingTime(CSprite* pSprite);
    
    void ResetDisplayTopGold();

    //터치로 뷰를 움직일때는 RenderEnd()에서 뷰를 변환하지 말자.
    bool IsViewRotationByTouch();
    //가장 가까운 메인타워를 찾는다. 쓰레드안쪽에서만 실행해야 한다.
    CSprite* GetMainTowerNearestInThread(SPoint* ptCenterSprite,int nTeamID);
    int SetMakePosition(float fRadius,int nIndex,CSprite *pTank,SPoint* ptCenter,SPoint* ptOutPoint = NULL);
    
    virtual CMutiplyProtocol* GetMutiplyProtocol() { return mpScenario->GetMutiplyProtocol();}
    void KilledActoerToMission(CSprite* pSprite,bool bRcv);
    int GetCurrentETMainTowerStartPointIndex() { return mnCurrentETMainTowerStartPointIndex;}
    bool IsFastBuild() { return mbFastBuild;}
    
    
#ifdef ACCELATOR
    virtual void SetAccelatorUpVector(float fv);
    //bool IsCarmeraRotationTartketPoint() { return mpCarmeraRotationTartketPoint? true : false;}
#endif //ACCELATOR

    
protected:
    void ChangeBombDisplay(USERINFO* userinfo); //현재 포탄 표시 컨트롤의 내용을 변경해준다.
    void ChangeBombDisplay(CSprite* pSprite,int nBombID);
    
#ifdef ANDROID
    void SetLight();
    virtual void ResetTexture();
#endif
    
#ifdef MAC
    virtual void RenderDesign() {};
#endif
    
    
    //Item Slot
    void InitItemSlot();
    void SetItemDescSlot(int nID);
    void SetBombDescSlot(int nID);
    void SetSpriteDescSlot(int nID);
    void SetUpgradeDescSlot(int nID);
    
    void SetRotationCarmeraToPoint(SPoint* pTartketPoint);
    void RotationCarmeraToPoint(int nTime);
    
    
    //Shop
    void ShowFrameTable();
    void InitFrameItemTable(CControl* pTableBack,float fX,float fY);
    void InitFrameBombTable(CControl* pTableBack,float fX,float fY);
    void InitBombCell(PROPERTY_BOMB* prop,CControl* pParent,float fOffsetX,float fOffsetY,int nType);
    void InitItemCell(PROPERTY_ITEM* prop,CControl* pParent,float fOffsetX,float fOffsetY,int nType);
    void ShowBuyDlg(int nID,int nType);
    bool OnPurchasedBomb(int nID);
    bool OnPurchasedProductItem(int nID);
    
    int  VisibleDisplayItemSlotPage(int nID);
    void AnimationDisplayItemSlot(int nID);
    bool OnPurchasedItem(int nID);
    void RemveItemTableRow(int nID);
    
    void InitSelectedObjectDescBar();
    void MakeBLZMainTower();
    
    
    void CompactedToBomb(CSprite* pSprite,CBomb* pBomb,SPoint* ptSprite,float fCompactGrade,int nActorTeamID,int nBombTeamID,float* pfModney);
    
    virtual void Serialize(CArchive& ar,bool bWrite);
    
    float GetGoldPerScore() { return mfGoldPerScore;}
    
    
    void InitEnemySprite();
    
    virtual float GetViewAperture();// { return VIEW_APERTURE_WORLD;}
    
//    void MakeAssTank(CSprite* pCenterSprite);
    
    void ShowEnemyTools(const char* sEndisplayName,int nEnRank,bool bAliens);
    void HideEnemyTools();
    
    
public:
    int                     mnArrEnSpriteCnt;
    int*                    mnArrEnSpriteTable;

protected:
    CControl                *mpDescCtl;
    CControl                *mpDescCtl2;
    CControl                *mpDescCtl3;
    
    CControl                *mpShopBtn;
    CControl                *mpGoldBar;
    CControl                *mpItemSlotBar;
    CButtonCtl              *mpItemSlotSwitchBtn;
    CControl                *mpRightTools;
    CControl                *mpLeftTools;
    CImgProgressCtl         *mpLeftToolsEnergyBar;
    CImgProgressCtl         *mpLeftToolsFireBar;
    CNumberCtl              *mpScoreNumCtl;
    CButtonCtl              *mpSelectedWeaponBtn; //현제 선택된 폭탄
    CNumberCtl              *mpAliveNumCtl;

    CControl                *mpSelectedObjectDescBar;
    long                     mnSelectedObjectDescBarVisibleTime;
    
    //------------------------------------------------------------
    //Item Factory Shop Bar
    CControl                      *mpFrameBar;
    CTableCtl                     *mpFrameBombTable;
    CTableCtl                     *mpFrameItembTable;
    CControl                      *mpFrameBombItemTab;
    CLabelCtl                     *mpBuyDlg;
    CTextureMan                   *mpFrameBarTexureMan;
    //------------------------------------------------------------
    CSGLCore                     *m_pSGLCore;
    

    C2dRadaBoard            *mpRadaBoard;
    C2dStick                *mpStick;

    //홈페이지로 갈지를 확인 하는 메세지.
    CMessageCtl             *mpConfirmMsgCtl;
    CMissionEndCtl          *mpMissionMsgCtl;
    CButtonCtl              *mpPauseMsgCtl;
    
    
    CBackground             *mBackground;

	//메인쓰레드 아이디
    CSThread                *mpThread;
	HQueue<int>				*mpThreadQueue;
	RuntimeRenderObjects	*mpCurrRuntimeRenderObjects;    
    unsigned int             mMaxID;
    
    //폭탄을 맞았을 경우 카메라가 흔들리는 효과를 준다.
    float                    mfAnimationCameraAttacked;
    //한번만 흔들자.. 왔다갔다. 1 = 왼쪽, 2 = 반대방향 3 = 멈춘다.
    int                      mnAnimationCameraAttackedCnt;
    
    int                     mRequestTry; //0:Not 1:Request 2:Try End
    
    CPicking                mPicking;
    CParticleEmitterMan     *mpParticleMan;
    CSprite                 *mpArrowMotionSprite;
    CImpactFlyParticle      *pImpactParticle;
    
    float                   mfScore;
    int                     mnBonus; //중간중간에 보너스가 나온다 그것을 먹으면 올라간다.
    unsigned short          mnMapLevel; //현재 맵의 레벨
    int                     mnMagnet; //아이템마그네팅을 가지고 있는지 없는지.
    unsigned int            mAllians[9]; //속도를 위해서 SGL_TEAM1~SGL_TEAM4 의 값을 어레이로 하였다.
    CControl                *mpBlzSelectedIcon;
    CSprite                 *mpClickedSprite; //DTower
    
    
    //특정위치로 화면을 움직여줄때
    SPoint                  *mpCarmeraRotationTartketPoint;
    
//    short                   mnDefaultAliveCnt; //1
    
    
    int                     mnNeedDefenceTowerUpgrade; //객체를 찾을때 쓰레드 밖에서 찾으면 오류가 날까봐 쓰레드 안쪽에서 찾게 하자.
    
#ifdef ANDROID
    bool                    mbNeedDelFrameBar;
#endif
    int                     mnCurrentETMainTowerStartPointIndex;
    bool                    mbIsDestroyEMainTower; //메인타워를 다부셨으면 true;
    bool                    mbIsDestoryEnemyActor; //적사용자탱크를 부셨으면 true;
    
    float                   mfGoldPerScore;
    vector<USERINFO*>*      mlstRemoveSlotInfo;
    vector<USERINFO*>*      mlstMainTowerSlotInfo;
    
    CIndicatorCtl           *mpWaitFindMultiplayCursor;
    CControl                *mpEnemyTools;
    
    bool                    mbFastBuild;
};

//extern CHWorld* g_pWorld;

#endif //_CHWORLD_H