//
//  CMainMenuWorld.h
//  SongGL
//  1.컨트롤을 만들때 주의사항.
//  컨트롤이 만들어주는 순서와 반대로 그려지게된다.
//  컨트롤이 만들어주는 순서대로 클릭이벤트를 받는다.
//
//  Created by 송 호학 on 11. 11. 30..
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.

#ifndef SongGL_CMainMenuWorld_h
#define SongGL_CMainMenuWorld_h

#include "CWorld.h"


class CTextureMan;
class CSoundMan;
class CControl;

#define BTN_MENU1_CON       1001 //Continue
#define BTN_MENU1_MUL       1002 //멀티플레이어
#define BTN_MENU1_SET       1003 //셋팅
#define BTN_MENU1_HLP       1004 //도움말.


#define BTN_MENU2_MAP 1005 //지도
#define BTN_MENU2_TAN 1006 //탱크
#define BTN_MENU2_BOM 1007 //폭탄
#define BTN_MENU2_ITE 1008 //아이템

#define BTN_MENU2_BOM_TABLE 1009 //폭탄 테이블
#define BTN_MENU2_MAP_TANK_ROLL  1010 //지도롤컨트롤
#define BTN_MENU2_TABLE_BOMB_NUM  1011 //Bol테이블의 셀을 클릭했을때
#define BTN_MENU2_ITEM_TABLE 1012 //폭탄 테이블
#define BTN_MENU2_TABLE_ITEM_NUM  1013 //Item테이블의 셀을 클릭했을때

#define BTN_MENU2_MAP_ROLL          1014 //
#define BTN_MENU2_TANK_ROLL         1015 //

#define BTN_MENU2_BUYBOMB           1016 //폭탄을 사자.
#define BTN_MENU2_BUYITEM           1017 //아이템을 사자

#define BTN_MENU2_TOP_GOLD_NUM        1018 //입력 -

#define BTN_MENU1_HELP_TABLE 1019 //폭탄 테이블
#define BTN_MENU1_SETTING_TABLE 1020 //Settings

#define BTN_MENU1_SOUNDFX 1021
#define BTN_MENU1_MUSIC 1022
#define BTN_MENU1_VIBRATION 1023

#if defined(APPSTOREKIT) || defined(ANDROIDAPPSTOREKIT)
#define BTN_MENU2_Shop       1024 //Shop
#endif //APPSTOREKIT or ANDROIDAPPSTOREKIT


#define BTN_MENU_NEXT 1100 //다음.
#define BTN_MENU_BACK 1101 //이전.
#define BTN_MENU_PLAY 1102 //Play화면으로 전환한다.




#define BTN_MENU_MAP        1001
#define BTN_MENU_TANK       1002
#define BTN_MENU_FACTORY    1003
#define BTN_MENU_BESTSCORE  1005
#define BTN_NEXT            1006
#define BTN_BOMBTAB         1007
#define BTN_ITEMTAB         1008

#define BTN_BOMBTABLE       1009
#define BTN_ITEMTABLE       1010
#define BTN_BOMBBUY         1011
#define BTN_ITEMBUY         1012

#define BTN_BOMBBUYOK           1013
#define BTN_ITEMBUYOK           1014
#define BTN_ITEMBOMBBUYCANCEL   1015
#define BTN_ITEMSLOTUPPAGE      1016
#define BTN_ITEMSLOTDOWNPAGE    1017
#define BTN_HELP_DLG            1018
#define BTN_SETTINGS_DLG        1019
#define BTN_SETTINGS_SAVE       1020
#define BTN_SETTINGS_CANCEL     1021
#define BTN_SETTINGS_BGM        1022
#define BTN_SETTINGS_FX         1023
#define BTN_SETTINGS_VIBRATION  1024
#define BTN_SELECT_TANK         1025
#define BTN_HELP_CANCEL         1026
#define BTN_ITEMSLOTSWITCH      1027
#define BTN_MENU_MULTIPLAY      1028
#define BTN_NEW_VERSION         1029






#define CTL_MAP_DESC_ICON       2001
#define CTL_TOP_GOLD            2002
#define CTL_TOP_GOLD2           2003 //음영

#define CTL_MAP_DESC_STAR1      2004
#define CTL_MAP_DESC_STAR2      2005
#define CTL_MAP_DESC_STAR3      2006
#define CTL_MAP_DESC_STAR4      2007
#define CTL_MAP_DESC_STAR5      2008
#define CTL_MAP_DESC_STAR6      2009
#define CTL_MAP_DESC_STAR7      2010
#define CTL_MAP_DESC_GRADE      2016

#define CTL_TANK_DESC_ICON       2011
#define CTL_TANK_DESC_ATTACK     2012
#define CTL_TANK_DESC_DEFEND     2013
#define CTL_TANK_DESC_SPEED      2014
#define CTL_TANK_DESC_LEVEL      2015

#define CTL_TANK_DESC_GOLDICON      2016
#define CTL_TANK_DESC_GOLD      2017




//#define CTL_TABLE_BOMB_SLOT      2016
//#define CTL_TABLE_BOMB_SLOT_CNT  2017

//#define CTL_TABLE_ITEM_SLOT         2018
#define CTL_TABLE_ITEM_SLOT_PAGE    2019
//#define CTL_TABLE_ITEM_SLOT1        2020 // 2026 까지 존재함.
//#define CTL_TABLE_ITEM_SLOT2        2021 // 2026 까지 존재함.
#define CTL_TABLE_ITEM_SLOT3        2022 // 2026 까지 존재함.
#define CTL_TABLE_ITEM_SLOT4        2023 // 2026 까지 존재함.
#define CTL_TABLE_ITEM_SLOT5        2024 // 2026 까지 존재함.
#define CTL_TABLE_ITEM_SLOT6        2025 // 2026 까지 존재함.


#define CTL_TANK_DESC_RANK       2026
#define CTL_TANK_DESC_TANKNAME   2027
#define CTL_TANK_DESC_ATTACK_ICON 2028
#define CTL_TANK_DESC_DEFENSE_ICON 2029
#define CTL_MAP_DESC_NAME          2030
#define CTL_RANK_ICON               2031
#define CTL_MENU_WIFIICON           2032





//enum MENUSTEP {
//    MENUSTEP_1 = 0, //구매하기,멀티플레이어,설정,도움말 로 구성한다.
//    MENUSTEP_2      //무기선택...
//    };

enum MENUANI {
    MENUANI_NONE  = 0,
    MENUANI_WORLD_TO_MAP, //  X => 메뉴의 이동
    MENUANI_WORLD_TO_TANK,
    MENUANI_WORLD_TO_FACTORY,
    
    
    MENUANI_MAP_TO_WORLD, //  MENU => X
    MENUANI_TANK_TO_WORLD,
    MENUANI_FACTORY_TO_WORLD,
    
    
    MENUANI_MENU_TO_MAP,
    MENUANI_MENU_TO_TANK,
    MENUANI_MENU_TO_FACTORY,
    
    
    
    
//    MENUANI_WORLD_TO_SHOP, 현재는 없다. 앞으로는 있다.
    
    
//    MENUANI_LOGO_TO_MENU1,  //로고 => 메뉴로 이동
//    MENUANI_MENU1_NEXT_HIDE, //메뉴1단계에서 다음 버튼을 눌렀을 경우 메뉴1의 컨트롤을 하이드해준다.
//    MENUANI_MENU1_HELP,      //메뉴1(Help 포커스)
//    MENUANI_MENU1_SETTING,   //메뉴1(Settings 포커스)
//    MENUANI_MENU2_MAP,       //메뉴1(MENUANI_MENU1_NEXT_HIDE)=>메뉴2(맵 포커스)
//    MENUANI_MENU2_MAP_HIDE,  //메뉴2(맵 포커스) => 메뉴1
//    MENUANI_MENU2_TANK,      //메뉴1(MENUANI_MENU1_NEXT_HIDE)=>메뉴2(Tank 포커스)
//    MENUANI_MENU2_TANK_HIDE, //메뉴2(Tank 포커스) => 메뉴1
//    MENUANI_MENU2_BOMB,      //메뉴1(MENUANI_MENU1_NEXT_HIDE)=>메뉴2(Bomb 포커스)
//    MENUANI_MENU2_BOMB_HIDE, //메뉴2(BOMB 포커스) => 메뉴1
//    MENUANI_MENU2_ITEM,      //메뉴1(MENUANI_MENU1_NEXT_HIDE)=>메뉴2(Bomb 포커스)
//    MENUANI_MENU2_ITEM_HIDE, //메뉴2(ITEM 포커스) => 메뉴1
//    MENUANI_MENU2_PLAY_HIDE, //Menu2 => Play
//    
//    
//    MENUANI_MENU2_BOM_MAP,
//    MENUANI_MENU2_ITEM_MAP,
//    MENUANI_MENU2_BOM_TANK,
//    MENUANI_MENU2_ITEM_TANK,
//    
//    
//    MENUANI_MENU2_MAP_BOM,
//    MENUANI_MENU2_TANK_BOM,
//    MENUANI_MENU2_ITEM_BOM,
//    MENUANI_MENU2_MAP_ITEM,
//    MENUANI_MENU2_TANK_ITEM,
//    MENUANI_MENU2_BOMB_ITEM,
//    
//    
//    MENUANI_MENU1_HELP_SETTING,
//    MENUANI_MENU1_SETTING_HELP,
    
    
};

class CRollCtl;
class CInputCtl;
class CNumberCtl;
class CButtonCtl;
class CTableCtl;
class CSoundMan;
class CRollMapTankCtl;
class  CIndicatorCtl;
class CMainMenuWorld : public CWorld
{
public:
    CMainMenuWorld(CScenario* pScenario);
    virtual ~CMainMenuWorld();
    virtual int GetWorldType() { return WORLD_TYPE_MENU;}
    void DeleteObject();
    virtual int Initialize(void* pResource); //리소스를 읽어온다.
    virtual void InitializeByResized();
	virtual void ResetCamera();
    virtual int initProjection();
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
    virtual CUserInfo* GetUserInfo() { return mpScenario->GetUserInfo();}
    
    virtual CMutiplyProtocol* GetMutiplyProtocol() { return mpScenario->GetMutiplyProtocol();}
    //작은 단말기에서 광고가 보여질때 왼쪽 메뉴의 구릅값이 밑에 하단 버튼과 겹치는 현상이 발생한다.
//    void GetAdLeftMenuDif(int& nOutGroupDifY,int& nOutPrevDifY);
    
    int InitMenu2(int nGroupDifY,int nPrevDifY);
    int InitMenu2_BombTable();
    int InitMenu2_ItemTable();
    

//    void SetToggleMenuGroup(CControl* pGroupCtl,CControl* pButtonCtl);
    CControl* FindControl(vector<CControl*>& list,int nID);
    
    //Animating이 끝났는지를 체크해본다.
    bool CheckNonAnimation();
    MENUANI Animating() { return mAniStep;}
    void    SetAniStep(MENUANI v);

    MENUANI GetAniStepMenu2(CControl* pGroupCtl,int nNotIndex = -1);
    
    void ShowDialogPromotionRank(int nRank);
    //탱크선택시 선택 못하는 사유
    void ShowShotMessageBox(const char* sIConPath,float fsu,float fsv,float feu,float fev,const char* sTitle,const char* sMessage1, const char* sMessage2);
    //일반적인 메세지... (훈련병은 훈련소를 졸업해야합니다.) 파라미터는 none = null
    void ShowShotMessageBox(const char* sTitle,const char* sMessage1, const char* sMessage2);
//    void ShowDialogBomb(PROPERTY_BOMB *Prop,CControl* pCountInCell,int nRow);
//    void ShowDialogItem(PROPERTY_ITEM *Prop,CControl* pCountInCell,int nRow);
    
#ifdef ANDROID
    virtual void ResetTexture();
#endif

    //현재 메뉴상태
    MENUANI GetNowMenuStep();
    void CompletedMapDlg();
    void StartPlayNoMatching(); //FindMatch의 TimeOut이 걸렸을 경우.
protected:
    void SetUpgrageData(int nTankID,bool bAttack);
    void InitMenu2_TankDesc(float fDescX,float fDescY);
    void SetTankDesc(PROPERTY_TANK* property_tank);
    
    
    //배경을 추기화한다.
    void InitBackground();
    //상단 골드 타이틀
    void InitTop();
    //왼쪽 메뉴바
    void InitMenuBar();
    //Weapon Slot
//    void InitWeaponSlot();
//    void InitWeaponSlotRow(CTableCtl* pStotBombTable,vector<USERINFO*>*lstBomb);
    
    //설명과 설정 메뉴바
    void InitHelpAndSettingsMenu();
    //맵 설명 바
    void InitMapDescBar();
    //탱크 설명 바
    void InitTankDescBar();
    void InitTankSelectedDescBar();
    //Rank Bar
    void InitRankBar();
    //Start Bar
    void InitStartBar();
    //Item Slot
    void InitItemSlot();
    //Frame Table
    void InitFrameTable();
    void InitFrameBombTable(CControl* pTableBack,float fX,float fY);
    //nType : 1 (Table) , 0 (PopupDlg) , 2 (Desc)
    void InitBombCell(PROPERTY_BOMB* prop,CControl* pParent,float fOffsetX,float fOffsetY,int nType);
    void InitFrameItemTable(CControl* pTableBack,float fX,float fY);
    //nType : 1 (Table) , 0 (PopupDlg) , 2 (Desc)
    void InitItemCell(PROPERTY_ITEM* prop,CControl* pParent,float fOffsetX,float fOffsetY,int nType);
    void InitMapATankRollCtl();
    int VisibleDisplayItemSlotPage(int nID);
    
    //nType : 0 (Bomb)
    //nType : 1 (Item)
    void ShowBuyDlg(int nID,int nType);
    void ShowSettingsDlg();
#ifdef HELPBTN
    void ShowHelpDlg();
#endif //HELPBTN
    void CloseDlg();
    
    void ResetDisplayTopGold();
//    void ResetDisplayBombSlot(int nID,bool bAdded); //테이블에 보여지고자 하는 아이디.
    void ResetDisplayItemSlot(); //테이블에 보여지고자 하는 아이디.
    void AnimationDisplayItemSlot(int nID);
    
    bool OnPurchasedBomb(int nID);
    bool OnPurchasedItem(int nID);
    bool OnPurchasedProductItem(int nID);
    
    void OnSelectedMap();
    void OnSelectedTank(int nTankID,bool bDesPopup);
    void OnClickedItemSlot(int nID);
    
    void ResetFrameItemTable();
    void RemveItemTableRow(int nID);
    void ResultReviews(); //평가를 하면 평가의 결과를 반영해준다.
    
    void SetPhoneStyleTankDescAni(int nTankID);
    bool CheckShowReview(int nCurrentMapIndex);
protected:
    CTextureMan                 *m_pTextureMan;
    //Setp 2의 메뉴컨틀롤의 집합니다.
    vector<CControl*>           mControlList;
    CIndicatorCtl                   *mpWaitFindMultiplayCursor;
    CControl                        *mpMultiplayButton;
    

    
    MENUANI                         mAniStep;
    //배경
    CControl                       *mpTopBack;
    CControl                       *mpBottomBack;
    CControl                       *mpTopLeftBack;
    CControl                       *mpBottomLeftBack;
    CControl                       *mpBottomRightBack;
    CControl                       *mpBottomItemBack;
    CControl                       *mpCompletedDlg;
    
    //상단 골드바.
    CControl                       *mpTobGoldBar;
    
    //메뉴
    CControl                       *mpMenuBar;
        

    //Help&Menu
    CControl                      *pmRightTopBar;
    
    //Map Desc Bar
    CControl                      *mpMapDescBar;
    
    //Tank Selected Desc Bar
    CControl                      *mpTankSelectedDescBar;
    //Tank Desc Bar
    CControl                      *mpTankDescBar;
    
    //Rank Bark
    CControl                      *mpRankPannel;
    
    //Start Bar
    CControl                      *mpStartPannel;
    
    //Item Slot Bar
    CControl                      *mpItemSlotBar;
    CButtonCtl                    *mpItemSlotSwitchBtn;

    CControl                      *mpNewViersion;
    
    //Item Factory Table Bar
    CControl                      *mpFrameBar;
    CTableCtl                     *mpFrameBombTable;
    CTableCtl                     *mpFrameItembTable;
    CControl                      *mpFrameBombItemTab;
    
    CSoundMan                     *mpSoundMan;
    
    CRollMapTankCtl               *mpRollCtl;
    
    CControl                      *mpBuyDlg;
#ifdef HELPBTN
    CControl                      *mpHelpDlg;
#endif //HELPBTN
    CControl                      *mpSettingsDlg;
    
    CControl                     *mpItemSlotDesc;
    long                          mnItemSlotAliveTime;
    float                         mfItemSlotAliveAlpha;
    
    CControl                     *mpPromotionRank;
    long                          mnPromotionRankTime;
    float                         mfPromotionRankAlpha;
    
    
    CControl                     *mpShotMessage;
    long                          mnShotMessage;
    float                         mfShotMessage;
    
    
    long                          mShowTimeCompletedDlg;

    
    long                          mnMulDlgMsgTime;
};


#endif
