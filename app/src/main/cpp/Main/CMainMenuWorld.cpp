//
//  CMainMenuWorld.cpp
//  SongGL
//
//  Created by 송 호학 on 11. 11. 30..
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
#include "sGL.h"
#include "CMainMenuWorld.h"
#include "CTextureMan.h"
#include "CSoundMan.h"
#include "sGLTrasform.h"
#include "sGLUtils.h"
#include "CRollCtl.h"
#include "sGLUtils.h"
#include "CButtonCtl.h"
#include "CScrollCtl.h"
#include "CTableCtl.h"
#include "CCellCtl.h"
#include "sGLText.h"
#include "CNumberCtl.h"
#include "CRollMapTankCtl.h"
#include "CInputCtl.h"
#include "sGLSound.h"
#include "CSoundMan.h"
#include "CImgProgressCtl.h"
#include "CMutiplyProtocol.h"
#include "CIndicatorCtl.h"

#ifdef NOFREE //유료
#define ANDROID_PRODUCT_ID "kr.co.songs.alieninvasion.gold.1000"
#else //이전 무료버전
#define ANDROID_PRODUCT_ID "kr.co.songhh.alieninvasion.gold.1000"
#endif //NOFREE

#if defined(ANDROIDAPPSTOREKIT)
extern "C" const char* sglReadCacheProduct();
#endif

char* gsNewAppVersion = NULL;

CMainMenuWorld::CMainMenuWorld(CScenario* pScenario) : CWorld(pScenario)
{

    m_pTextureMan = new CTextureMan;
    mAniStep = MENUANI_NONE;
    mpItemSlotSwitchBtn = NULL;

    mpSoundMan = NULL;
    

    //배경
    mpTopBack = NULL;
    mpBottomBack = NULL;
    mpTopLeftBack = NULL;
    mpBottomLeftBack = NULL;
    mpBottomRightBack = NULL;
    mpBottomItemBack = NULL;
    
    //상단 골드바
    mpTobGoldBar = NULL;
    
    //매뉴
    mpMenuBar = NULL;

    //Help & Settings
    pmRightTopBar = NULL;
    
    //Map Desc Bar
    mpMapDescBar = NULL;
    
    //Tank Desc Bar
    mpTankDescBar = NULL;
    
    //Start Bar
    mpStartPannel = NULL;
    
    //Item Slot Bar
    mpItemSlotBar = NULL;
    
    //Factory Table Bar
    mpFrameBar = NULL;
    mpFrameBombTable = NULL;
    mpFrameItembTable = NULL;
    mpFrameBombItemTab = NULL;
    
    mpRollCtl = NULL;
    mpBuyDlg = NULL;
#ifdef HELPBTN
    mpHelpDlg = NULL;
#endif //HELPBTN
    mpSettingsDlg = NULL;
    
    mpItemSlotDesc = NULL;
    mpPromotionRank = NULL;
 
    mnItemSlotAliveTime= 0;
    mnPromotionRankTime = 0;
    
    mpShotMessage = NULL;
    mnShotMessage = 0;
    mfShotMessage = 0.f;
    
    mpCompletedDlg = NULL;
    mShowTimeCompletedDlg = 0;
    
    mpNewViersion = NULL;
}

CMainMenuWorld::~CMainMenuWorld()
{
    if(m_pTextureMan)
    {
        delete m_pTextureMan;
        m_pTextureMan = 0;
    }
    
    DeleteObject();
    
    if(mpSoundMan)
    {
        delete mpSoundMan;
        mpSoundMan = NULL;
    }
    
    StopBgSound();
}

void CMainMenuWorld::DeleteObject()
{
    int nSize = (int)mControlList.size();
    for (int i = 0; i < nSize; i++) {
        delete mControlList[i];
    }
    mControlList.clear();
    
//    nSize = mAniMationControl.size();
//    for (int i = 0; i < nSize; i++) {
//        delete mAniMationControl[i];
//    }
//    mAniMationControl.clear();
    
    
    if(mpBuyDlg)
    {
        delete mpBuyDlg;
        mpBuyDlg = NULL;
    }
#ifdef HELPBTN
    if(mpHelpDlg)
    {
        delete mpHelpDlg;
        mpHelpDlg = NULL;
    }
#endif //HELPBTN
    
    if(mpSettingsDlg)
    {
        delete mpSettingsDlg;
        mpSettingsDlg = NULL;
    }
    
    if(mpShotMessage)
    {
        delete mpShotMessage;
        mpShotMessage = NULL;
    }
    
    //배경
    if(mpTopBack) delete mpTopBack;
    mpTopBack = NULL;
    if(mpBottomBack) delete mpBottomBack;
    mpBottomBack = NULL;
    if(mpTopLeftBack) delete mpTopLeftBack;
    mpTopLeftBack = NULL;
    if(mpBottomLeftBack) delete mpBottomLeftBack;
    mpBottomLeftBack = NULL;
    if(mpBottomRightBack) delete mpBottomRightBack;
    mpBottomRightBack= NULL;
    if(mpBottomItemBack) delete mpBottomItemBack;
    mpBottomItemBack = NULL;
    
    //상단 골드 바
    mpTobGoldBar = NULL;
    
    //메뉴
    mpMenuBar = NULL;
    
    //Help & Settings
    pmRightTopBar = NULL;
    
    //Map Desc Bar
    mpMapDescBar = NULL;
    
    //Tank Desc Bar
    mpTankDescBar = NULL;
    
    //Rank Bar
    mpRankPannel = NULL;
    
    //Start Bar
    mpStartPannel = NULL;
    
    //Item Slot Bar
    mpItemSlotBar = NULL;
    
    //Factory Table Bar
    mpFrameBar = NULL;
    
    mpRollCtl = NULL;

    mpPromotionRank = NULL;
    mpItemSlotDesc = NULL;
}

void CMainMenuWorld::InitBackground()
{
    vector<string>  lstImage;
    vector<float> layout;
    
    //배경
    lstImage.clear();
    lstImage.push_back(IMG_MAINMENU_BACK);
    lstImage.push_back("none");
    
    layout.clear();
    PROPERTYI::GetPropertyFloatList("Back_TopBack", layout);
    mpTopBack = new CLabelCtl(NULL,m_pTextureMan);
    mpTopBack->Initialize(0, layout[0], layout[1], gDisplayWidth, layout[3], lstImage, 0.f, 0.f, 0.849609375f, 0.2197265625f);
    
    
    //같은 이미지 상하 반전...
    lstImage.clear();
    lstImage.push_back(IMG_MAINMENU_BACK);
    lstImage.push_back("none");
    mpBottomBack = new CLabelCtl(NULL,m_pTextureMan);
    //-85는 Bottom의 ImpBottomItemBack의 높이이다.
    layout.clear();
    PROPERTYI::GetPropertyFloatList("Back_BottomBack", layout); //0,299,-1,225
    mpBottomBack->Initialize(0, layout[0], gDisplayHeight - layout[1], gDisplayWidth, layout[3], lstImage, 0.f, 0.f, 0.849609375f, 0.2197265625f);
//    //같은 이미지 상하 반전...
    mpBottomBack->FlipVertical();
 
    //배경
    lstImage.clear();
    lstImage.push_back(IMG_MAINMENU_BACK);
    lstImage.push_back("none");
    mpTopLeftBack = new CLabelCtl(NULL,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("Back_TopLeftBack", layout);
    mpTopLeftBack->Initialize(0, layout[0], layout[0], layout[2], layout[3], lstImage, 0.8583984375f, 0.02734375f, 0.97265625f, 0.234375f);
    
    lstImage.clear();
    lstImage.push_back(IMG_MAINMENU_BACK);
    lstImage.push_back("none");
    mpBottomLeftBack = new CLabelCtl(NULL,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("Back_BottomLeftBack", layout);
    mpBottomLeftBack->Initialize(0, layout[0], gDisplayHeight - layout[1], layout[2], layout[3], lstImage, 0.8583984375f, 0.279296875f, 0.97265625f, 0.443359375f);
    
    lstImage.clear();
    lstImage.push_back(IMG_MAINMENU_BACK);
    lstImage.push_back("none");
    layout.clear();
    PROPERTYI::GetPropertyFloatList("Back_BottomRightBack", layout);
    mpBottomRightBack = new CLabelCtl(NULL,m_pTextureMan);
    mpBottomRightBack->Initialize(0, gDisplayWidth - layout[0], gDisplayHeight - layout[1], layout[2], layout[3], lstImage, 0.8583984375f, 0.4765625f, 0.97265625f, 0.640625f);
    
    
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    mpBottomItemBack =  new CLabelCtl(NULL,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("Back_BottomItemBack", layout);
    mpBottomItemBack->Initialize(0, layout[0], gDisplayHeight - layout[1], gDisplayWidth - layout[0], layout[3], lstImage, 0.0, 0.677734375, 0.9970703125, 0.75390625);
    
    //아이패드는 아래쪽에 슬롯이 존재한다.
    if(gnDisplayType == DISPLAY_IPAD)
    {
        lstImage.clear();
        lstImage.push_back("res_slot.png");
        lstImage.push_back("none");
        CControl* pBottomLeftItemBack =  new CLabelCtl(mpBottomItemBack,m_pTextureMan);
        layout.clear();
        PROPERTYI::GetPropertyFloatList("Back_BottomLeftItemBack", layout);
        pBottomLeftItemBack->Initialize(0, layout[0], gDisplayHeight - layout[1], (gDisplayWidth - layout[0]) / 2.f, layout[3], lstImage, 0.0, 0.0f, 1.0f, 0.15234375);
        mpBottomItemBack->AddControl(pBottomLeftItemBack);
        
        lstImage.clear();
        lstImage.push_back("res_slot.png");
        lstImage.push_back("none");
        CControl* pBottomRightItemBack =  new CLabelCtl(mpBottomItemBack,m_pTextureMan);
        layout.clear();
        PROPERTYI::GetPropertyFloatList("Back_BottomRightItemBack", layout);
        pBottomRightItemBack->Initialize(0, layout[0] + (gDisplayWidth - layout[0])/2.f, gDisplayHeight - layout[1], (gDisplayWidth - layout[0])/2.f, layout[3], lstImage, 0.0, 0.16015625, 1.0f, 0.3125);
        mpBottomItemBack->AddControl(pBottomRightItemBack);
    }
    
    
//    lstImage.clear();
//    lstImage.push_back("none");
//    lstImage.push_back("none");
//    mpXXXX =  new CLabelCtl(NULL,m_pTextureMan);
    //layout.clear();
//    PROPERTYI::GetPropertyFloatList("Back_BottomItemBack", layout);
    //IPhone
    //14.000000#512#0.000000#4.000000#0.820312#0.625000#32#b
    //mpXXXX->Initialize(0, 14, 293, 420, 20, lstImage, 0.0, 0.677734375, 0.9970703125, 0.75390625);
    
    //IPad
    //mpXXXX->Initialize(0, 15, 633, 840, 40, lstImage, 0.0, 0.677734375, 0.9970703125, 0.75390625);
    //mControlList.push_back(mpXXXX);

}


void CMainMenuWorld::InitTop()
{
    vector<float> layout;
    float fWhite[] = { 1.0f,1.0f,1.0f,1.0f};
    CUserInfo* pUserInfo = mpScenario->GetUserInfo();
    vector<string>  lstImage;
    //상단 좌측 메뉴바
    lstImage.clear();
    lstImage.push_back(IMG_MAINMENU_BACK);
    lstImage.push_back("M_Menu");
    
    //멀티 플레이를 추가하면서 화면이 너무작아서 메뉴타이를 제거하고 그위치에 메뉴바를 넣었다.
    if(gnDisplayType == DISPLAY_IPAD)
    {
        CControl* pTobLeftMenuBar =  new CLabelCtl(NULL,m_pTextureMan);
        layout.clear();
        PROPERTYI::GetPropertyFloatList("Top_TobLeftMenuBar", layout);
        pTobLeftMenuBar->Initialize(0, layout[0] , layout[1], layout[2], layout[3], lstImage, 0.162109375, 0.9462890625, 0.2431640625, 0.9833984375);
        pTobLeftMenuBar->SetTextColor(fWhite);
        mControlList.push_back(pTobLeftMenuBar);
    }
    
    //상단 돈 바탕화면
    lstImage.clear();
    lstImage.push_back(IMG_MAINMENU_BACK);
    lstImage.push_back("none");
    mpTobGoldBar =  new CLabelCtl(NULL,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("Top_TobGoldBar", layout);
    //상단 가운데 정렬
    mpTobGoldBar->Initialize(0, (gDisplayWidth - layout[0]) / 2, layout[1], layout[2], layout[3], lstImage, 0.0048828125, 0.7578125, 0.4462890625, 0.81640625);
    mControlList.push_back(mpTobGoldBar);
    
    //골드...
//    virtual int Initialize(int nControlID,
//                           int x, int y, int nWidth,int nHeight,int nNumberWidth,
//                           int offsetx,int offsety,vector<string>& lstImage,bool bButton = false);
 
    layout.clear();
    PROPERTYI::GetPropertyFloatList("Top_TobGoldDefault", layout);
    
    float fX = (gDisplayWidth - layout[0]) / 2.0f;
    float fY = layout[1];
    float fW = layout[2];
    float fH = layout[3];
    float fFontW = layout[4];
    float fFontOffsetX = layout[5];
    
    float fMoreX = layout[6];
    float fMoreY = layout[6];
    
    //숫자 음영
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    CNumberCtl* pTopGold =  new CNumberCtl(mpTobGoldBar,m_pTextureMan,true);
    pTopGold->Initialize(CTL_TOP_GOLD2, fX + fMoreX, fY + fMoreY, fW, fH, fFontW, fFontOffsetX, 0,lstImage);
    float fTextColorB[] = { 0.3f,0.3f,0.3f,1.0f}; //Black
    pTopGold->SetTextColor(fTextColorB);
    pTopGold->SetAlign(CNumberCtl::ALIGN_CENTER);
    pTopGold->SetNumber(pUserInfo->GetGold());
    mpTobGoldBar->AddControl(pTopGold);

    
    //숫자.
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    pTopGold =  new CNumberCtl(mpTobGoldBar,m_pTextureMan,true);
    pTopGold->Initialize(CTL_TOP_GOLD, fX , fY, fW, fH, fFontW, fFontOffsetX, 0,lstImage);
    float fTextColor[] = { 1.0f,1.0f,0.0f,1.0f}; //노랑
    pTopGold->SetTextColor(fTextColor);
    pTopGold->SetAlign(CNumberCtl::ALIGN_CENTER);
    pTopGold->SetNumber(pUserInfo->GetGold());
    mpTobGoldBar->AddControl(pTopGold);
}


void CMainMenuWorld::InitHelpAndSettingsMenu()
{
    vector<float> layout;
    vector<string>  lstImage;
    
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    pmRightTopBar = new CLabelCtl(NULL,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("Help_RightTopBar", layout);
    
    float fX = gDisplayWidth - layout[2];
    float fY = layout[1];
    pmRightTopBar->Initialize(0, fX - layout[0], fY - layout[1], layout[2], layout[3], lstImage, 0, 0, 0, 0);
    
    //Help
    lstImage.clear();
    lstImage.push_back(IMG_MAINMENU_BACK);
    lstImage.push_back("none");
    CControl* pHelpButton =  new CButtonCtl(pmRightTopBar,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("Help_HelpButton", layout);
    pHelpButton->Initialize(BTN_HELP_DLG, fX - layout[0], fY - layout[1], layout[2], layout[3], lstImage, 0.3486328125, 0.6142578125, 0.3994140625, 0.6640625);
    pmRightTopBar->AddControl(pHelpButton);
    
    //설정
    lstImage.clear();
    lstImage.push_back(IMG_MAINMENU_BACK);
    lstImage.push_back("none");
    CControl* pSettingsButton =  new CButtonCtl(NULL,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("Help_SettingsButton", layout);
    pSettingsButton->Initialize(BTN_SETTINGS_DLG, fX + layout[2] - layout[0], fY - layout[1], layout[2], layout[3], lstImage, 0.4013671875, 0.6142578125, 0.4521484375, 0.6640625);
    pmRightTopBar->AddControl(pSettingsButton);
    
    mControlList.push_back(pmRightTopBar);
}

void CMainMenuWorld::InitMenuBar()
{
    float fWhite[] = { 1.f,1.f,1.f,1.f};
    vector<float> layout;
    vector<string>  lstImage;
    //왼쪽 메뉴
    //37,44 시작 메뉴의 높이는 49 , 넓이는 201
    //메뉴그룹은 (맵/탱크/Bomb&Item/상점) (201 , 196)
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    mpMenuBar = new CLabelCtl(NULL,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("Menu_MenuBar", layout);
    mpMenuBar->Initialize(0,layout[0], layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
    
    
    //맵
    lstImage.clear();
    lstImage.push_back(IMG_MAINMENU_BACK);
    lstImage.push_back("M_Menu_Map");
    CButtonCtl *pMapMenu = new CButtonCtl(mpMenuBar,m_pTextureMan,BUTTON_GROUP_TOGGLE);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("Menu_MapMenu", layout);
    pMapMenu->Initialize(BTN_MENU_MAP, layout[0], layout[1], layout[2], layout[3], lstImage,
                          0.162109375, 0.8876953125, 0.3583984375, 0.9326171875,
                          0.162109375, 0.833984375, 0.3583984375, 0.87890625
                          );
    pMapMenu->SetTextColor(fWhite);
    pMapMenu->SetPushed(true);
    mpMenuBar->AddControl(pMapMenu);
    
    //탱크
    lstImage.clear();
    lstImage.push_back(IMG_MAINMENU_BACK);
    lstImage.push_back("M_Menu_Tank");
    layout.clear();
    PROPERTYI::GetPropertyFloatList("Menu_TankMenu", layout);
    CButtonCtl *pTankMenu = new CButtonCtl(mpMenuBar,m_pTextureMan,BUTTON_GROUP_TOGGLE);
    pTankMenu->Initialize(BTN_MENU_TANK, layout[0], layout[1], layout[2], layout[3], lstImage,
                           0.162109375, 0.8876953125, 0.3583984375, 0.9326171875,
                           0.162109375, 0.833984375, 0.3583984375, 0.87890625
                           );
    pTankMenu->SetTextColor(fWhite);
    mpMenuBar->AddControl(pTankMenu);
    
    
    //Best Score
    lstImage.clear();
    lstImage.push_back(IMG_MAINMENU_BACK);
    lstImage.push_back("M_Menu_Shop");
    //lstImage.push_back("M_Menu_BombItem");
    CButtonCtl *pFactoryMenu = new CButtonCtl(mpMenuBar,m_pTextureMan,BUTTON_GROUP_TOGGLE);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("Menu_FactoryMenu", layout);
    pFactoryMenu->Initialize(BTN_MENU_FACTORY, layout[0], layout[1], layout[2], layout[3], lstImage,
                              0.162109375, 0.8876953125, 0.3583984375, 0.9326171875,
                              0.162109375, 0.833984375, 0.3583984375, 0.87890625
                              );
    pFactoryMenu->SetTextColor(fWhite);
    mpMenuBar->AddControl(pFactoryMenu);
    

    //상점
    lstImage.clear();
    lstImage.push_back(IMG_MAINMENU_BACK);
    lstImage.push_back("M_Menu_BombItem");
    CButtonCtl *pShopMenu = new CButtonCtl(mpMenuBar,m_pTextureMan,BUTTON_NORMAL);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("Menu_ShopMenu", layout);
    pShopMenu->Initialize(BTN_MENU_BESTSCORE, layout[0], layout[1], layout[2], layout[3], lstImage,
                           0.162109375, 0.8876953125, 0.3583984375, 0.9326171875,
                           0.162109375, 0.833984375, 0.3583984375, 0.87890625
                           );
    pShopMenu->SetTextColor(fWhite);
    mpMenuBar->AddControl(pShopMenu);
    
    
    
    //멀티플레이
    lstImage.clear();
    lstImage.push_back(IMG_MAINMENU_BACK);
    lstImage.push_back("M_Menu_Multi1");
    mpMultiplayButton = new CButtonCtl(mpMenuBar,m_pTextureMan,BUTTON_NORMAL);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("Menu_MultiplayMenu", layout);
    ((CButtonCtl*)mpMultiplayButton)->Initialize(BTN_MENU_MULTIPLAY, layout[0], layout[1], layout[2], layout[3], lstImage,
                          0.162109375, 0.8876953125, 0.3583984375, 0.9326171875,
                          0.162109375, 0.833984375, 0.3583984375, 0.87890625
                          );
    mpMultiplayButton->SetTextColor(fWhite);
    
    //멀티플레이
    lstImage.clear();
    lstImage.push_back("wifi.png");
    lstImage.push_back("none");
    CLabelCtl *pWifiIcon = new CLabelCtl(mpMultiplayButton,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("Menu_MultiplayMenuWIFI", layout);
    pWifiIcon->Initialize(CTL_MENU_WIFIICON, layout[0], layout[1], layout[2], layout[3], lstImage,
                          0.f, 0.f, 1.f, 1.f);
    pWifiIcon->SetEnable(false);
    pWifiIcon->SetHide(true);
    mpMultiplayButton->AddControl(pWifiIcon);
    
    mpMenuBar->AddControl(mpMultiplayButton);
    
    
    
    mControlList.push_back(mpMenuBar);
}

void CMainMenuWorld::InitMapDescBar()
{
    float fWhite[] = { 1.f,1.f,1.f,1.f};
    vector<float> layout;
    vector<string>  lstImage;
    
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    mpMapDescBar =  new CLabelCtl(NULL,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("Desc_MapDescBar", layout);
    mpMapDescBar->Initialize(0, gDisplayWidth - layout[0], layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
    if(gnDisplayType == DISPLAY_IPAD)
    {
        //오른쪽 맵 설명타이틀
        lstImage.clear();
        lstImage.push_back(IMG_MAINMENU_BACK);
        lstImage.push_back("none");

    
        CControl* pMapDescTitle =  new CLabelCtl(mpMapDescBar,m_pTextureMan);
        layout.clear();
        PROPERTYI::GetPropertyFloatList("Desc_MapDescTitle", layout);
        pMapDescTitle->Initialize(0, gDisplayWidth - layout[0], layout[1], layout[2], layout[3], lstImage, 0.7958984375, 0.76171875, 0.9912109375, 0.802734375);
        mpMapDescBar->AddControl(pMapDescTitle);
    
    
        //체크
        lstImage.clear();
        lstImage.push_back(IMG_MAINMENU_BACK);
        lstImage.push_back("none");
        CControl* pMapDescCheckTitle =  new CLabelCtl(pMapDescTitle,m_pTextureMan);
        layout.clear();
        PROPERTYI::GetPropertyFloatList("Desc_MapDescCheckTitle", layout);
        pMapDescCheckTitle->Initialize(0, gDisplayWidth - layout[0], layout[1], layout[2], layout[3], lstImage, 0.37109375, 0.880859375, 0.3984375, 0.9091796875);
        pMapDescTitle->AddControl(pMapDescCheckTitle);
        
        //체크옆 타이트
        lstImage.clear();
        lstImage.push_back("none");
        lstImage.push_back("M_Map_Desc_Title");
        CControl* pMapDescCheckName =  new CLabelCtl(pMapDescTitle,m_pTextureMan);
        layout.clear();
        PROPERTYI::GetPropertyFloatList("Desc_MapDescCheckName", layout);
        pMapDescCheckName->Initialize(0, gDisplayWidth - layout[0], layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
        pMapDescCheckName->SetTextColor(fWhite);
        pMapDescTitle->AddControl(pMapDescCheckName);
    }
    
    
    //맵설명패널
    lstImage.clear();
    lstImage.push_back(IMG_MAINMENU_BACK);
    lstImage.push_back("none");
    CControl* pMapDescPannel =  new CLabelCtl(mpMapDescBar,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("Desc_MapDescPannel", layout);
    pMapDescPannel->Initialize(0, gDisplayWidth - layout[0], layout[1], layout[2], layout[3], lstImage, 0.7958984375, 0.8115234375, 0.9912109375, 0.93359375);
    
    
    //맴이름
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    CControl* pMapDescName =  new CLabelCtl(pMapDescPannel,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("Desc_MapDescName", layout);
    pMapDescName->Initialize(CTL_MAP_DESC_NAME, gDisplayWidth - layout[0], layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
    pMapDescName->SetTextColor(fWhite);
    pMapDescPannel->AddControl(pMapDescName);
    
    //262 : 60 = 82 : xor
    
    //난위도... 등급 easy, normal, difficlt, expert
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("M_Map_Desc_Dificulty");
    CControl* pMapDescGrade =  new CLabelCtl(pMapDescPannel,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("Desc_MapDescGrade", layout);
    pMapDescGrade->Initialize(CTL_MAP_DESC_GRADE, gDisplayWidth - layout[0], layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
    pMapDescGrade->SetTextColor(fWhite);
    pMapDescPannel->AddControl(pMapDescGrade);

    
    //맵아이콘
    lstImage.clear();
    lstImage.push_back(IMG_MAINMENU_BACK);
    lstImage.push_back("none");
    CControl* pMapDescIcon =  new CLabelCtl(pMapDescPannel,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("Desc_MapDescIcon", layout);
    pMapDescIcon->Initialize(CTL_MAP_DESC_ICON, gDisplayWidth - layout[0], layout[1], layout[2], layout[3], lstImage, 0.6748046875, 0.896484375, 0.7607421875, 0.982421875);
    pMapDescPannel->AddControl(pMapDescIcon);
    
    
    //별..
    lstImage.clear();
    lstImage.push_back(IMG_MAINMENU_BACK);
    lstImage.push_back("none");
    CControl* pStar1 =  new CLabelCtl(pMapDescPannel,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("Desc_Star1", layout);
    pStar1->Initialize(CTL_MAP_DESC_STAR1, gDisplayWidth - layout[0], layout[1], layout[2], layout[3], lstImage, 0.3740234375, 0.84375, 0.39453125, 0.8642578125);
    pStar1->SetHide(true);
    pMapDescPannel->AddControl(pStar1);
    
    lstImage.clear();
    lstImage.push_back(IMG_MAINMENU_BACK);
    lstImage.push_back("none");
    CControl* pStar2 =  new CLabelCtl(pMapDescPannel,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("Desc_Star2", layout);
    pStar2->Initialize(CTL_MAP_DESC_STAR2, gDisplayWidth - layout[0], layout[1], layout[2], layout[3], lstImage, 0.3740234375, 0.84375, 0.39453125, 0.8642578125);
    pStar2->SetHide(true);
    pMapDescPannel->AddControl(pStar2);
    
    lstImage.clear();
    lstImage.push_back(IMG_MAINMENU_BACK);
    lstImage.push_back("none");
    CControl* pStar3 =  new CLabelCtl(pMapDescPannel,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("Desc_Star3", layout);
    pStar3->Initialize(CTL_MAP_DESC_STAR3, gDisplayWidth - layout[0], layout[1], layout[2], layout[3], lstImage, 0.3740234375, 0.84375, 0.39453125, 0.8642578125);
    pStar3->SetHide(true);
    pMapDescPannel->AddControl(pStar3);
    
    
    lstImage.clear();
    lstImage.push_back(IMG_MAINMENU_BACK);
    lstImage.push_back("none");
    CControl* pStar4 =  new CLabelCtl(pMapDescPannel,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("Desc_Star4", layout);
    pStar4->Initialize(CTL_MAP_DESC_STAR4, gDisplayWidth - layout[0], layout[1], layout[2], layout[3], lstImage, 0.3740234375, 0.84375, 0.39453125, 0.8642578125);
    pStar4->SetHide(true);
    pMapDescPannel->AddControl(pStar4);
    
    
    
    
    //별..
    lstImage.clear();
    lstImage.push_back(IMG_MAINMENU_BACK);
    lstImage.push_back("none");
    CControl* pStar5 =  new CLabelCtl(pMapDescPannel,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("Desc_Star5", layout);
    pStar5->Initialize(CTL_MAP_DESC_STAR5, gDisplayWidth - layout[0], layout[1], layout[2], layout[3], lstImage, 0.3740234375, 0.84375, 0.39453125, 0.8642578125);
    pStar5->SetHide(true);
    pMapDescPannel->AddControl(pStar5);
    
    
    
    //별..
    lstImage.clear();
    lstImage.push_back(IMG_MAINMENU_BACK);
    lstImage.push_back("none");
    CControl* pStar6 =  new CLabelCtl(pMapDescPannel,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("Desc_Star6", layout);
    pStar6->Initialize(CTL_MAP_DESC_STAR6, gDisplayWidth - layout[0], layout[1], layout[2], layout[3], lstImage, 0.3740234375, 0.84375, 0.39453125, 0.8642578125);
    pStar6->SetHide(true);
    pMapDescPannel->AddControl(pStar6);
    
    
    //별..
    lstImage.clear();
    lstImage.push_back(IMG_MAINMENU_BACK);
    lstImage.push_back("none");
    CControl* pStar7 =  new CLabelCtl(pMapDescPannel,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("Desc_Star7", layout);
    pStar7->Initialize(CTL_MAP_DESC_STAR7, gDisplayWidth - layout[0], layout[1], layout[2], layout[3], lstImage, 0.3740234375, 0.84375, 0.39453125, 0.8642578125);
    pStar7->SetHide(true);
    pMapDescPannel->AddControl(pStar7);

    
    
    
    mpMapDescBar->AddControl(pMapDescPannel);
    
    
    mControlList.push_back(mpMapDescBar);
}


void CMainMenuWorld::InitTankDescBar()
{
    float fWhite[] = {1.0f,1.0f,1.0f,1.0f};
    vector<float> layout;
    vector<string>  lstImage;
    
    
    
    lstImage.clear();
    lstImage.push_back(IMG_RES_TANK);
    lstImage.push_back("none");
    mpTankDescBar =  new CLabelCtl(NULL,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("DescT_TankDescBar", layout);
    
    float fX = (gDisplayWidth - layout[2]) / 2.0f + layout[0];//layout[0];
    float fY = (gDisplayHeight - layout[3]) / 2.0f + layout[1];//107.f;//layout[1];
    mpTankDescBar->Initialize(0, fX , fY , layout[2], layout[3], lstImage, 0.029296875, 0.021484375, 0.767578125, 0.513671875);
    
    //선택버튼.
    lstImage.clear();
    lstImage.push_back(IMG_RES_TANK);
    lstImage.push_back("M_Tank_Desc_Select");
    CControl* pSelectBtn =  new CButtonCtl(mpTankSelectedDescBar,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("DescT_SelectBtn", layout);
    pSelectBtn->Initialize(BTN_SELECT_TANK, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.02734375, 0.5390625, 0.57421875, 0.615234375);
    mpTankDescBar->AddControl(pSelectBtn);
    
    //RankTitle1 (Requested)
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("M_Tank_Desc_Requested");
    CControl* pRankTitle1 =  new CLabelCtl(mpTankSelectedDescBar,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("DescT_RankTitle1", layout);
    pRankTitle1->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.02734375, 0.638671875, 0.109375, 0.6640625);
    pRankTitle1->SetTextColor(fWhite);
    mpTankDescBar->AddControl(pRankTitle1);
    

    //RankTitle2 (Rank)
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("M_Tank_Desc_Rank");
    CControl* pRankTitle2 =  new CLabelCtl(mpTankSelectedDescBar,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("DescT_RankTitle2", layout);
    pRankTitle2->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.02734375, 0.638671875, 0.109375, 0.6640625);
    pRankTitle2->SetTextColor(fWhite);
    mpTankDescBar->AddControl(pRankTitle2);

    //RankBak
    lstImage.clear();
    lstImage.push_back(IMG_RES_TANK);
    lstImage.push_back("none");
    CControl* pLankBack =  new CLabelCtl(mpTankSelectedDescBar,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("DescT_LankBack", layout);
    pLankBack->Initialize(CTL_TANK_DESC_RANK, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.0f, 0.0f, 1.0f, 1.0f);
    mpTankDescBar->AddControl(pLankBack);

    //탱크 타이틀
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    CControl* pTankescName =  new CLabelCtl(mpTankDescBar,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("DescT_TankescName", layout);
    pTankescName->Initialize(CTL_TANK_DESC_TANKNAME, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
    pTankescName->SetTextColor(fWhite);
    mpTankDescBar->AddControl(pTankescName);
    

    
    //Defense Icon
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    CControl* pDefendICon =  new CLabelCtl(mpTankDescBar,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("DescT_DefendICon", layout);
    pDefendICon->Initialize(CTL_TANK_DESC_DEFENSE_ICON, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.0f, 0.0f, 0.96875f, 0.96875f);
    mpTankDescBar->AddControl(pDefendICon);
    
//    float fRed[] = {1.0f,0.0f,0.0f,1.0f};
    CNumberCtl* pItemBox4Num =  new CNumberCtl(pDefendICon,m_pTextureMan,true);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("DescT_ItemBox4Num", layout);
    pItemBox4Num->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3],layout[4],layout[5],layout[6],lstImage);
    pItemBox4Num->SetAlign(CNumberCtl::ALIGN_RIGHT);
    pItemBox4Num->SetTextColor(fWhite);
    pItemBox4Num->SetEnable(false);
    pDefendICon->AddControl(pItemBox4Num);
    
    
    //Attack Icon
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    CControl* pAttackICon =  new CLabelCtl(mpTankDescBar,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("DescT_AttackICon", layout);
    pAttackICon->Initialize(CTL_TANK_DESC_ATTACK_ICON, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.0f, 0.0f, 0.96875f, 0.96875f);
    mpTankDescBar->AddControl(pAttackICon);

    pItemBox4Num =  new CNumberCtl(pAttackICon,m_pTextureMan,true);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("DescT_ItemBox4Num2", layout);
    pItemBox4Num->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3],layout[4],layout[5],layout[6],lstImage);
    pItemBox4Num->SetAlign(CNumberCtl::ALIGN_RIGHT);
    pItemBox4Num->SetTextColor(fWhite);
    pItemBox4Num->SetEnable(false);
    pAttackICon->AddControl(pItemBox4Num);
    
    //탱크레벨타이틀
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("M_Tank_Desc_Level");
    CControl* pTankDescLevelTitle =  new CLabelCtl(mpTankDescBar,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("DescT_TankDescLevelTitle", layout);
    pTankDescLevelTitle->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
    pTankDescLevelTitle->SetTextColor(fWhite);
    mpTankDescBar->AddControl(pTankDescLevelTitle);

    //텡크레벨
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    CNumberCtl* pTankDescLevel =  new CNumberCtl(mpTankDescBar,m_pTextureMan,true);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("DescT_TankDescLevel", layout);
    pTankDescLevel->Initialize(CTL_TANK_DESC_LEVEL, fX + layout[0], fY + layout[1], layout[2], layout[3],layout[4],layout[5],layout[6], lstImage);
    pTankDescLevel->SetAlign(CNumberCtl::ALIGN_RIGHT);
    float fTextColor[] = { 1.0f,1.0f,0.0f,1.0f}; //노랑
    pTankDescLevel->SetTextColor(fTextColor);
    pTankDescLevel->SetNumber(0);
    mpTankDescBar->AddControl(pTankDescLevel);

    //탱크 Attack상태 라벨
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("M_Tank_Desc_Power");
    CLabelCtl* pTankPowerStateTitle = new CLabelCtl(mpTankDescBar,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("DescT_TankPowerStateTitle", layout);
    pTankPowerStateTitle->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
    pTankPowerStateTitle->SetTextColor(fWhite);
    mpTankDescBar->AddControl(pTankPowerStateTitle);
    
    //탱크 Attack상태
    lstImage.clear();
    lstImage.push_back(IMG_MAINMENU_BACK);
    lstImage.push_back("none");
    CImgProgressCtl* pTankPowerState = new CImgProgressCtl(mpTankDescBar,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("DescT_TankPowerState", layout);
    pTankPowerState->Initialize(CTL_TANK_DESC_ATTACK, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage,
                                0.44140625, 0.953125, 0.5166015625, 0.9580078125,
                                0.44140625, 0.9716796875, 0.5196015625, 0.9775625
                                );
    mpTankDescBar->AddControl(pTankPowerState);

    
    //탱크 Defence상태 라벨
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("M_Tank_Desc_Defense");
    CLabelCtl* pTankDefenceStateTitle = new CLabelCtl(mpTankDescBar,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("DescT_TankDefenceStateTitle", layout);
    pTankDefenceStateTitle->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
    pTankDefenceStateTitle->SetTextColor(fWhite);
    mpTankDescBar->AddControl(pTankDefenceStateTitle);
    
    //탱크 Defence상태
    lstImage.clear();
    lstImage.push_back(IMG_MAINMENU_BACK);
    lstImage.push_back("none");
    CImgProgressCtl* pTankDefenceState = new CImgProgressCtl(mpTankDescBar,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("DescT_TankDefenceState", layout);
    pTankDefenceState->Initialize(CTL_TANK_DESC_DEFEND, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage,
                                  0.44140625, 0.953125, 0.5166015625, 0.9580078125,
                                  0.44140625, 0.9716796875, 0.5196015625, 0.9775625
                                  );
    mpTankDescBar->AddControl(pTankDefenceState);

    
    //탱크 Speed상태 라벨
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("M_Tank_Desc_Seed");
    CLabelCtl* pTankSpeedStateTitle = new CLabelCtl(mpTankDescBar,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("DescT_TankSpeedStateTitle", layout);
    pTankSpeedStateTitle->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
    pTankSpeedStateTitle->SetTextColor(fWhite);
    mpTankDescBar->AddControl(pTankSpeedStateTitle);
    
    //탱크 Speed상태
    lstImage.clear();
    lstImage.push_back(IMG_MAINMENU_BACK);
    lstImage.push_back("none");
    CImgProgressCtl* pTankSpeedState = new CImgProgressCtl(mpTankDescBar,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("DescT_TankSpeedState", layout);
    pTankSpeedState->Initialize(CTL_TANK_DESC_SPEED, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage,
                                0.44140625, 0.953125, 0.5166015625, 0.9580078125,
                                0.44140625, 0.9716796875, 0.5196015625, 0.9775625
                                );
    mpTankDescBar->AddControl(pTankSpeedState);
    
    
    //골드아이콘
    lstImage.clear();
    lstImage.push_back("gold.png");
    lstImage.push_back("none");
    CLabelCtl* pGoldICon = new CLabelCtl(mpTankDescBar,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("DescT_TankGoldICon", layout);
    pGoldICon->Initialize(CTL_TANK_DESC_GOLDICON, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 1.f, 1.f);
    pGoldICon->SetHide(true);
    mpTankDescBar->AddControl(pGoldICon);

    
    
    //골드아이콘
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    CNumberCtl* pGold = new CNumberCtl(mpTankDescBar,m_pTextureMan,true);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("DescT_TankGoldNum", layout);

    float fYellow[] = { 1.f,1.f,0.f,1.f};
    pGold->Initialize(CTL_TANK_DESC_GOLD, fX + layout[0], fY + layout[1], layout[2], layout[3],layout[4],layout[5],layout[6],lstImage);
    pGold->SetAlign(CNumberCtl::ALIGN_RIGHT);
    pGold->SetTextColor(fYellow);
    pGold->SetEnable(false);
//    pGold->SetNumber(9500);
    pGold->SetHide(true);
    mpTankDescBar->AddControl(pGold);
    
    
    mpTankDescBar->SetHide(true);
    mControlList.push_back(mpTankDescBar);
}

void CMainMenuWorld::InitTankSelectedDescBar()
{
    vector<string>  lstImage;
    vector<float> layout;
    float fWhite[] = { 1.f,1.f,1.f,1.f};
    
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    mpTankSelectedDescBar =  new CLabelCtl(NULL,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("DescST_TankSelectedDescBar", layout);
    
    mpTankSelectedDescBar->Initialize(0, gDisplayWidth -layout[0], layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
    
    if(gnDisplayType == DISPLAY_IPAD)
    {
        //탱크 타이틀
        lstImage.clear();
        lstImage.push_back(IMG_MAINMENU_BACK);
        lstImage.push_back("none");
        CControl* pTankDescTitle =  new CLabelCtl(mpTankSelectedDescBar,m_pTextureMan);
        layout.clear();
        PROPERTYI::GetPropertyFloatList("DescST_TankDescTitle", layout);
        pTankDescTitle->Initialize(0, gDisplayWidth -layout[0], layout[1], layout[2], layout[3], lstImage, 0.7958984375, 0.76171875, 0.9912109375, 0.802734375);
        
        //체크
        lstImage.clear();
        lstImage.push_back(IMG_MAINMENU_BACK);
        lstImage.push_back("none");
        CControl* pTankDescCheckTitle =  new CLabelCtl(pTankDescTitle,m_pTextureMan);
        layout.clear();
        PROPERTYI::GetPropertyFloatList("DescST_TankDescCheckTitle", layout);
        pTankDescCheckTitle->Initialize(0, gDisplayWidth -layout[0], layout[1], layout[2], layout[3], lstImage, 0.37109375, 0.880859375, 0.3984375, 0.9091796875);
        pTankDescTitle->AddControl(pTankDescCheckTitle);
        mpTankSelectedDescBar->AddControl(pTankDescTitle);
        
        //체크옆 타이트
        lstImage.clear();
        lstImage.push_back("none");
        lstImage.push_back("M_Tank_Desc_Title");
        CControl* pTankDescCheckName =  new CLabelCtl(pTankDescTitle,m_pTextureMan);
        layout.clear();
        PROPERTYI::GetPropertyFloatList("DescST_TankDescCheckNamee", layout);
        pTankDescCheckName->Initialize(0, gDisplayWidth -layout[0], layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
        pTankDescCheckName->SetTextColor(fWhite);
        pTankDescTitle->AddControl(pTankDescCheckName);
    }
    
    
    //탱크이름
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    CControl* pTankescName =  new CLabelCtl(mpTankSelectedDescBar,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("DescST_TankescName", layout);
    pTankescName->Initialize(CTL_TANK_DESC_TANKNAME, gDisplayWidth -layout[0], layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
    pTankescName->SetTextColor(fWhite);
    mpTankSelectedDescBar->AddControl(pTankescName);
    
    
    //탱크레벨타이틀
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("M_Tank_Desc_Level");
    CControl* pTankDescLevelTitle =  new CLabelCtl(mpTankSelectedDescBar,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("DescST_TankDescLevelT", layout);
    pTankDescLevelTitle->Initialize(0, gDisplayWidth -layout[0], layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
    pTankDescLevelTitle->SetTextColor(fWhite);
    mpTankSelectedDescBar->AddControl(pTankDescLevelTitle);
    
    //텡크레벨
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    CNumberCtl* pTankDescLevel =  new CNumberCtl(mpTankSelectedDescBar,m_pTextureMan,true);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("DescST_TankDescLevel", layout);
    pTankDescLevel->Initialize(CTL_TANK_DESC_LEVEL, gDisplayWidth -layout[0], layout[1], layout[2], layout[3],layout[4], layout[5], layout[6],lstImage);
    pTankDescLevel->SetAlign(CNumberCtl::ALIGN_RIGHT);
    float fTextColor[] = { 1.0f,1.0f,0.0f,1.0f}; //노랑
    pTankDescLevel->SetTextColor(fTextColor);
    pTankDescLevel->SetNumber(0);
    mpTankSelectedDescBar->AddControl(pTankDescLevel);
    
    

    //탱크아이콘 배경
    lstImage.clear();
    lstImage.push_back(IMG_MAINMENU_BACK);
    lstImage.push_back("none");
    CControl* pTankDescIconBack =  new CLabelCtl(mpTankSelectedDescBar,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("DescST_TankDescIconBack", layout);
    pTankDescIconBack->Initialize(CTL_TANK_DESC_ICON, gDisplayWidth -layout[0], layout[1], layout[2], layout[3], lstImage, 0.6748046875, 0.896484375, 0.7607421875, 0.982421875);
    mpTankSelectedDescBar->AddControl(pTankDescIconBack);
    
    
    
    
    //탱크 Attack상태 라벨
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("M_Tank_Desc_Power");
    CLabelCtl* pTankPowerStateTitle = new CLabelCtl(mpTankSelectedDescBar,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("DescST_TankPowerStateTitle", layout);
    pTankPowerStateTitle->Initialize(0, gDisplayWidth -layout[0], layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
    pTankPowerStateTitle->SetTextColor(fWhite);
    mpTankSelectedDescBar->AddControl(pTankPowerStateTitle);
    
    //탱크 Attack상태
    lstImage.clear();
    lstImage.push_back(IMG_MAINMENU_BACK);
    lstImage.push_back("none");
    CImgProgressCtl* pTankPowerState = new CImgProgressCtl(mpTankSelectedDescBar,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("DescST_TankPowerState", layout);
    pTankPowerState->Initialize(CTL_TANK_DESC_ATTACK, gDisplayWidth -layout[0], layout[1], layout[2], layout[3], lstImage,
                                0.44140625, 0.953125, 0.5166015625, 0.9580078125,
                                0.44140625, 0.9716796875, 0.5196015625, 0.9775625
                                );
    mpTankSelectedDescBar->AddControl(pTankPowerState);
    
    
    //탱크 Defence상태 라벨
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("M_Tank_Desc_Defense");
    CLabelCtl* pTankDefenceStateTitle = new CLabelCtl(mpTankSelectedDescBar,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("DescST_TankDefenceStateT", layout);
    pTankDefenceStateTitle->Initialize(0, gDisplayWidth -layout[0], layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
    pTankDefenceStateTitle->SetTextColor(fWhite);
    mpTankSelectedDescBar->AddControl(pTankDefenceStateTitle);
    
    //탱크 Defence상태
    lstImage.clear();
    lstImage.push_back(IMG_MAINMENU_BACK);
    lstImage.push_back("none");
    CImgProgressCtl* pTankDefenceState = new CImgProgressCtl(mpTankSelectedDescBar,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("DescST_TankDefenceSt", layout);
    pTankDefenceState->Initialize(CTL_TANK_DESC_DEFEND, gDisplayWidth -layout[0], layout[1], layout[2], layout[3], lstImage,
                                  0.44140625, 0.953125, 0.5166015625, 0.9580078125,
                                  0.44140625, 0.9716796875, 0.5196015625, 0.9775625
                                  );
    mpTankSelectedDescBar->AddControl(pTankDefenceState);
    
    
    //탱크 Speed상태 라벨
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("M_Tank_Desc_Seed");
    CLabelCtl* pTankSpeedStateTitle = new CLabelCtl(mpTankSelectedDescBar,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("DescST_TankSpeedStateT", layout);
    pTankSpeedStateTitle->Initialize(0, gDisplayWidth -layout[0], layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
    pTankSpeedStateTitle->SetTextColor(fWhite);
    mpTankSelectedDescBar->AddControl(pTankSpeedStateTitle);
    
    //탱크 Speed상태
    lstImage.clear();
    lstImage.push_back(IMG_MAINMENU_BACK);
    lstImage.push_back("none");
    CImgProgressCtl* pTankSpeedState = new CImgProgressCtl(mpTankSelectedDescBar,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("DescST_TankSpeedState", layout);
    pTankSpeedState->Initialize(CTL_TANK_DESC_SPEED, gDisplayWidth -layout[0], layout[1], layout[2], layout[3], lstImage,
                                0.44140625, 0.953125, 0.5166015625, 0.9580078125,
                                0.44140625, 0.9716796875, 0.5196015625, 0.9775625
                                );
    
    mpTankSelectedDescBar->AddControl(pTankSpeedState);
    
    mpTankSelectedDescBar->SetEnable(false); //스타스타트 버튼이 가린다.
    
    mControlList.push_back(mpTankSelectedDescBar);
    

}

void CMainMenuWorld::InitRankBar()
{
    float fWhite[] = { 1.f,1.f,1.f,1.f};
    vector<string>  lstImage;
    vector<float> layout;
    //Start Pannel
    lstImage.clear();
    lstImage.push_back(IMG_TABLE_BANDI);
    lstImage.push_back("none");
    
    mpRankPannel =  new CLabelCtl(NULL,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("Rnk_RankPannel", layout);
    mpRankPannel->Initialize(0, layout[0], gDisplayHeight - layout[1], layout[2], layout[3], lstImage, 0.7529296875, 0.0078125, 0.9716796875, 0.2265625);
    
    
    lstImage.clear();
    lstImage.push_back(IMG_TABLE_BANDI);
    lstImage.push_back("none");
    CLabelCtl* pRing =  new CLabelCtl(mpRankPannel,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("Rnk_Ring", layout);
    pRing->Initialize(0, layout[0], gDisplayHeight - layout[1], layout[2], layout[3], lstImage, 0.8369140625, 0.24609375, 1, 0.408203125);
    mpRankPannel->AddControl(pRing);
    
    
    lstImage.clear();
    lstImage.push_back(mpScenario->GetRankPath());
    lstImage.push_back("none");
    CLabelCtl* pRankImg =  new CLabelCtl(mpRankPannel,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("Rnk_RankImg", layout);
    pRankImg->Initialize(CTL_RANK_ICON, layout[0], gDisplayHeight - layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 1.f, 1.f);
    mpRankPannel->AddControl(pRankImg);
    
//    lstImage.clear();
//    lstImage.push_back("none");
//    lstImage.push_back("M_Rank_Title");
//    CLabelCtl* pYouRankTitle =  new CLabelCtl(mpRankPannel,m_pTextureMan);
//    layout.clear();
//    PROPERTYI::GetPropertyFloatList("Rnk_YouRankTitle", layout);
//    pYouRankTitle->Initialize(0, layout[0], gDisplayHeight - layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
//    pYouRankTitle->SetTextColor(fWhite);
//    mpRankPannel->AddControl(pYouRankTitle);
    //Modified M_Rank_Title을 토탈스코어로 변경하였다.
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    layout.clear();
    PROPERTYI::GetPropertyFloatList("Rnk_YouRankTitle", layout);
    CNumberCtl* pYouRankTitle =  new CNumberCtl(mpRankPannel,m_pTextureMan,true);
    pYouRankTitle->Initialize(0, layout[0], gDisplayHeight - layout[1], layout[2], layout[3],layout[4],layout[5],layout[6],lstImage);
    
    pYouRankTitle->SetTextColor(fWhite);
    pYouRankTitle->SetAlign(CNumberCtl::ALIGN_CENTER);
    CUserInfo* pUserInfo = GetUserInfo();
    if(pUserInfo && pUserInfo->GetTotalScore() != 0)
        pYouRankTitle->SetNumber(pUserInfo->GetTotalScore());
    mpRankPannel->AddControl(pYouRankTitle);
    
    mControlList.push_back(mpRankPannel);
}



void CMainMenuWorld::InitStartBar()
{
    vector<string>  lstImage;
    vector<float> layout;
    
    //Start Pannel
    lstImage.clear();
    lstImage.push_back(IMG_MAINMENU_BACK);
    lstImage.push_back("none");
    
    mpStartPannel =  new CLabelCtl(NULL,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("Sta_StartPannel", layout);
    mpStartPannel->Initialize(0, gDisplayWidth - layout[0], gDisplayHeight - layout[1], layout[2], layout[3], lstImage, 0.4609375, 0.7646484375, 0.634765625, 0.9384765625);
    
    
    //Start Pannel
    lstImage.clear();
    lstImage.push_back(IMG_MAINMENU_BACK);
    lstImage.push_back("M_Start");
    //Start Button
    CButtonCtl* pStartButton = new CButtonCtl(mpStartPannel,m_pTextureMan,BUTTON_NORMAL);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("Sta_StartButton", layout);
    pStartButton->Initialize(BTN_NEXT, gDisplayWidth - layout[0], gDisplayHeight - layout[1], layout[2], layout[3], lstImage,
                             0.0048828125, 0.8203125, 0.1318359375, 0.9453125,
                             0.65625, 0.765625, 0.779296875, 0.890625
                             );
    mpStartPannel->AddControl(pStartButton);
    
    
    
    // New Version
    lstImage.clear();
    lstImage.push_back("newver.png");
    lstImage.push_back("none");
    mpNewViersion = new CButtonCtl(mpStartPannel,m_pTextureMan,BUTTON_NORMAL);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("Sta_StartPannel", layout);
    float fW = layout[2] / 1.5f;
    float fH = layout[3] / 2.f;
    float fX = gDisplayWidth - layout[0] + (layout[2] - fW) / 2.f;
    float fY = gDisplayHeight - layout[1] + layout[3] - fH;
    mpNewViersion->Initialize(BTN_NEW_VERSION, fX, fY, fW, fH, lstImage,0.f, 0.f, 1.f, 1.f);
    mpStartPannel->AddControl(mpNewViersion);
    mpNewViersion->SetHide(true); //처음에는 숨겨준다.
    
    
    
    //WaitCursor
    mpWaitFindMultiplayCursor =  new CIndicatorCtl(mpStartPannel,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("Sta_StartPannel", layout);
    mpWaitFindMultiplayCursor->Initialize(0, gDisplayWidth - layout[0], gDisplayHeight - layout[1], layout[2], layout[3]);
    mpStartPannel->AddControl(mpWaitFindMultiplayCursor);
    mControlList.push_back(mpStartPannel);
}

void CMainMenuWorld::InitItemSlot()
{
    CUserInfo* pUserInfo = mpScenario->GetUserInfo();
    float fDisableColor[] = {0.5f,0.5f,0.5f,0.9f};
    
    vector<string>  lstImage;
    vector<float> layout;

    
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    
    mpItemSlotBar =  new CLabelCtl(NULL,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SL_ItemSlotBar", layout);
    
    //246,-80
    float fX = layout[0];
    float fY = gDisplayHeight + layout[1];
    
    if(gnDisplayType == DISPLAY_IPHONE)
    {
        fX = gDisplayWidth + layout[0];
        fY = layout[1];
    }
    
    mpItemSlotBar->Initialize(0, fX, fY, layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
    
    lstImage.clear();
    lstImage.push_back("res_slot.png");
    lstImage.push_back("none");
    
    //아이템 페이지.
    CControl* pItemPageNumberBack =  new CLabelCtl(mpItemSlotBar,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SL_ItemPageNumberBack", layout);
    pItemPageNumberBack->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.43359375, 0.330078125, 0.525390625, 0.421875);
    mpItemSlotBar->AddControl(pItemPageNumberBack);
    
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    CNumberCtl* pItemPageNumber =  new CNumberCtl(pItemPageNumberBack,m_pTextureMan,true);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SL_ItemPageNumber", layout);
    pItemPageNumber->Initialize(CTL_TABLE_ITEM_SLOT_PAGE, fX + layout[0], fY + layout[1], layout[2], layout[3],layout[4],layout[5],layout[6],lstImage);
    float fWhite[] = { 1.f,1.f,1.f,1.f};
    pItemPageNumber->SetTextColor(fWhite);
    pItemPageNumber->SetAlign(CNumberCtl::ALIGN_CENTER);
    pItemPageNumberBack->AddControl(pItemPageNumber);
    
//    float fRed[] = { 1.0f,0.0f,0.0f,1.0f};    
    lstImage.clear();
    lstImage.push_back("res_slot.png");
    lstImage.push_back("M_Slot_Item");
    lstImage.push_back("M_Slot_Bomb");
    
    
    
    mpItemSlotSwitchBtn =  new CButtonCtl(mpItemSlotBar,m_pTextureMan,BUTTON_TOGGLE);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SL_ItemSlotSwitchBtn", layout);
    mpItemSlotSwitchBtn->Initialize(BTN_ITEMSLOTSWITCH, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage,
                                    0.025390625, 0.33203125, 0.26953125, 0.44921875,
                                    0.025390625, 0.33203125, 0.26953125, 0.44921875);
    mpItemSlotSwitchBtn->SetTextColor(fWhite);
    if(pUserInfo->GetLastBombItemSwitch() == 1)
        mpItemSlotSwitchBtn->SetPushed(true);
    else
        mpItemSlotSwitchBtn->SetPushed(false);
    mpItemSlotBar->AddControl(mpItemSlotSwitchBtn);

    //Slot3
    lstImage.clear();
    lstImage.push_back("res_slot.png");
    lstImage.push_back("none");
    CButtonCtl* pItemBox3Back =  new CButtonCtl(mpItemSlotBar,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SL_ItemBox3Back", layout);
    pItemBox3Back->Initialize(CTL_TABLE_ITEM_SLOT3, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.2890625, 0.328125, 0.416015625, 0.455078125,
                              0.654296875,0.328125,0.78125,0.45703125);
    mpItemSlotBar->AddControl(pItemBox3Back);
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    CControl* pItemBox3 =  new CLabelCtl(pItemBox3Back,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SL_ItemBox3", layout);
    pItemBox3->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 1.f, 1.f);
    pItemBox3->SetEnable(false);
    pItemBox3Back->AddControl(pItemBox3);

    
    CNumberCtl* pItemBox3Num =  new CNumberCtl(pItemBox3,m_pTextureMan,true);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SL_ItemBox3Num", layout);
    pItemBox3Num->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3],layout[4],layout[5],layout[6],lstImage);
    pItemBox3Num->SetAlign(CNumberCtl::ALIGN_RIGHT);
    pItemBox3Num->SetTextColor(fWhite);
    pItemBox3Num->SetEnable(false);
    pItemBox3->AddControl(pItemBox3Num);
    

    
    //Slot4
    lstImage.clear();
    lstImage.push_back("res_slot.png");
    lstImage.push_back("none");
    CButtonCtl* pItemBox4Back =  new CButtonCtl(mpItemSlotBar,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SL_ItemBox4Back", layout);
    pItemBox4Back->Initialize(CTL_TABLE_ITEM_SLOT4, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.2890625, 0.328125, 0.416015625, 0.4550781255,
                              0.654296875,0.328125,0.78125,0.45703125);
    mpItemSlotBar->AddControl(pItemBox4Back);
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    CControl* pItemBox4 =  new CLabelCtl(pItemBox4Back,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SL_ItemBox4", layout);
    pItemBox4->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 1.f, 1.f);
    pItemBox4->SetEnable(false);
    pItemBox4Back->AddControl(pItemBox4);
    
    CNumberCtl* pItemBox4Num =  new CNumberCtl(pItemBox4,m_pTextureMan,true);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SL_ItemBox4Num", layout);
    pItemBox4Num->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3],layout[4],layout[5],layout[6],lstImage);
    pItemBox4Num->SetAlign(CNumberCtl::ALIGN_RIGHT);
    pItemBox4Num->SetTextColor(fWhite);
    pItemBox4Num->SetEnable(false);
    pItemBox4->AddControl(pItemBox4Num);
    
    //Slot5
    lstImage.clear();
    lstImage.push_back("res_slot.png");
    lstImage.push_back("none");
    CButtonCtl* pItemBox5Back =  new CButtonCtl(mpItemSlotBar,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SL_ItemBox5Back", layout);
    pItemBox5Back->Initialize(CTL_TABLE_ITEM_SLOT5, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.2890625, 0.328125, 0.416015625, 0.4550781255,
                              0.654296875,0.328125,0.78125,0.45703125);
    mpItemSlotBar->AddControl(pItemBox5Back);
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    CControl* pItemBox5 =  new CLabelCtl(pItemBox5Back,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SL_ItemBox5", layout);
    pItemBox5->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 1.f, 1.f);
    pItemBox5->SetEnable(false);
    pItemBox5Back->AddControl(pItemBox5);

    CNumberCtl* pItemBox5Num =  new CNumberCtl(pItemBox5,m_pTextureMan,true);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SL_ItemBox5Num", layout);
    pItemBox5Num->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3],layout[4],layout[5],layout[6],lstImage);
    pItemBox5Num->SetAlign(CNumberCtl::ALIGN_RIGHT);
    pItemBox5Num->SetTextColor(fWhite);
    pItemBox5Num->SetEnable(false);
    pItemBox5->AddControl(pItemBox5Num);
        
    
    //Slot6
    lstImage.clear();
    lstImage.push_back("res_slot.png");
    lstImage.push_back("none");
    CButtonCtl* pItemBox6Back =  new CButtonCtl(mpItemSlotBar,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SL_ItemBox6Back", layout);
    pItemBox6Back->Initialize(CTL_TABLE_ITEM_SLOT6, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.2890625, 0.328125, 0.416015625, 0.4550781255,
                              0.654296875,0.328125,0.78125,0.45703125);
    mpItemSlotBar->AddControl(pItemBox6Back);
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    CControl* pItemBox6 =  new CLabelCtl(pItemBox6Back,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SL_ItemBox6", layout);
    pItemBox6->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 1.f, 1.f);
    pItemBox6->SetEnable(false);
    pItemBox6Back->AddControl(pItemBox6);

    CNumberCtl* pItemBox6Num =  new CNumberCtl(pItemBox6,m_pTextureMan,true);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SL_ItemBox6Num", layout);
    pItemBox6Num->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3],layout[4],layout[5],layout[6],lstImage);
    pItemBox6Num->SetAlign(CNumberCtl::ALIGN_RIGHT);
    pItemBox6Num->SetTextColor(fWhite);
    pItemBox6Num->SetEnable(false);
    pItemBox6->AddControl(pItemBox6Num);
    
    lstImage.clear();
    lstImage.push_back("res_slot.png");
    lstImage.push_back("none");
    
    //아이템페이지 updown
    CControl* pItemPageBack =  new CLabelCtl(mpItemSlotBar,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SL_ItemPageBack", layout);
    pItemPageBack->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage,
                              0.544921875, 0.330078125, 0.634765625, 0.46484375);
    mpItemSlotBar->AddControl(pItemPageBack);
//    
//    CControl* pItemPageUp =  new CButtonCtl(pItemPageBack,m_pTextureMan);
//    layout.clear();
//    PROPERTYI::GetPropertyFloatList("SL_ItemPageUp", layout);
//    pItemPageUp->Initialize(BTN_ITEMSLOTUPPAGE, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage,
//                            0.80078125, 0.337890625, 0.85546875, 0.392578125);
//    pItemPageBack->AddControl(pItemPageUp);
//    
//    CControl* pItemPageDown =  new CButtonCtl(pItemPageBack,m_pTextureMan);
//    layout.clear();
//    PROPERTYI::GetPropertyFloatList("SL_ItemPageDown", layout);
//    pItemPageDown->Initialize(BTN_ITEMSLOTDOWNPAGE, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage,
//                            0.80078125, 0.39453125, 0.85546875, 0.44921875);
//    pItemPageBack->AddControl(pItemPageDown);
    
//    CControl* pItemPageBack =  new CLabelCtl(mpItemSlotBar,m_pTextureMan);
//    layout.clear();
//    PROPERTYI::GetPropertyFloatList("SL_ItemPageBack", layout);
//    pItemPageBack->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage,
//                              0.3046875, 0.716796875, 0.52734375, 0.8125);
//    mpItemSlotBar->AddControl(pItemPageBack);
    
    CControl* pItemPageUp =  new CButtonCtl(pItemPageBack,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SL_ItemPageUp", layout);
    pItemPageUp->Initialize(BTN_ITEMSLOTUPPAGE, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage,
                            0.404296875, 0.607421875, 0.49609375, 0.69921875);
    pItemPageUp->SetDisableColor(fDisableColor);
    pItemPageBack->AddControl(pItemPageUp);
    
    CControl* pItemPageDown =  new CButtonCtl(pItemPageBack,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SL_ItemPageDown", layout);
    pItemPageDown->Initialize(BTN_ITEMSLOTDOWNPAGE, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage,
                              0.404296875, 0.83984375, 0.49609375, 0.931640625);
    pItemPageDown->SetDisableColor(fDisableColor);
    pItemPageBack->AddControl(pItemPageDown);

    
    //폰일때는 처음에 보여지게 하지 말자.
    if(gnDisplayType == DISPLAY_IPHONE)
        mpItemSlotBar->SetHide(true);
    
    mControlList.push_back(mpItemSlotBar);
}

//nType : 1 (Table) , 0 (PopupDlg) , 2 (Desc)
void CMainMenuWorld::InitBombCell(PROPERTY_BOMB* prop,CControl* pParent,float fOffsetX,float fOffsetY,int nType)
{
    float fWhite[] = {1.f,1.f,1.f,1.f};
    float fRed[] = {1.f,0.f,0.f,1.f};
    float fGreen[] = {0.f,1.0f,0.0f,1.0f};
    vector<string>  lstImage;
    vector<float> layout;
    
    //CellTitleBack
    lstImage.clear();
if(nType != 2)
    lstImage.push_back(IMG_TABLE_BANDI);
else
    lstImage.push_back("none");
    lstImage.push_back("none");
    CControl* pCellTitleBack =  new CLabelCtl(pParent,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("BCe_CellTitleBack", layout);
    pCellTitleBack->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3], lstImage, 0.466796875, 0.2978515625, 0.69921875, 0.3203125);
    pParent->AddControl(pCellTitleBack);
    
    
    //폰의 속성 : (375(56),168(11) = 56,15) 고폭탄/유도탄
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back(prop->sDesc);
    
    CControl* pBombType = new CLabelCtl(pCellTitleBack,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("BCe_BombType", layout);
    pBombType->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
    pBombType->SetTextColor(fWhite);
    pCellTitleBack->AddControl(pBombType);
    
    //폭탄이름
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back(prop->sName);
    CControl* pBombName = new CLabelCtl(pCellTitleBack,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("BCe_BombName", layout);
    pBombName->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
    pBombName->SetTextColor(fWhite);
    pCellTitleBack->AddControl(pBombName);
    
    //CellDescBack
    lstImage.clear();
    lstImage.push_back(IMG_TABLE_BANDI);
    lstImage.push_back("none");
    CControl* pCellDescBack =  new CLabelCtl(pParent,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("BCe_CellDescBack", layout);
    pCellDescBack->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3], lstImage, 0.466796875, 0.4326171875, 0.69921875, 0.501953125);
    pParent->AddControl(pCellDescBack);
    
    //폭탄이미지.
    lstImage.clear();
    lstImage.push_back(prop->sBombTableImgPath);
    lstImage.push_back("none");
    CControl* pBombIcon = new CLabelCtl(pCellDescBack,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("BCe_BombIcon", layout);
    pBombIcon->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3], lstImage, 0.20703125, 0.2265625, 0.79296875, 0.7734375);
    pCellDescBack->AddControl(pBombIcon);
    
    //Power Label.
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("M_IB_Power");
    CControl* pBombSPLabel = new CLabelCtl(pCellDescBack,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("BCe_BombSPLabel", layout);
    pBombSPLabel->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
    pBombSPLabel->SetTextColor(fWhite);
    pCellDescBack->AddControl(pBombSPLabel);
    //Power
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    CNumberCtl* pBombSP = new CNumberCtl(pCellDescBack,m_pTextureMan,true);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("BCe_BombSP", layout);
    pBombSP->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3],layout[4],layout[5],layout[6], lstImage);
    pBombSP->SetTextColor(fRed);
    pBombSP->SetNumber((int)prop->fMaxAttackPower);
    pBombSP->SetAlign(CNumberCtl::ALIGN_LEFT);
    pCellDescBack->AddControl(pBombSP);
    
    //Range Label.
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("M_IB_Range");
    CControl* pBombRngLabel = new CLabelCtl(pCellDescBack,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("BCe_BombRngLabel", layout);
    pBombRngLabel->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
    pBombRngLabel->SetTextColor(fWhite);
    pCellDescBack->AddControl(pBombRngLabel);
    
    
    //Range.
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    CNumberCtl* pBombRng = new CNumberCtl(pCellDescBack,m_pTextureMan,true);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("BCe_BombRng", layout);
    pBombRng->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3],layout[4],layout[5],layout[6], lstImage);
    pBombRng->SetTextColor(fGreen);
    pBombRng->SetNumber(prop->fMaxRadianDetect);
    pBombRng->SetAlign(CNumberCtl::ALIGN_LEFT);
    pCellDescBack->AddControl(pBombRng);
    

    //Ammo Label.
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("M_IB_Ammo");
    CControl* pBombAmmoLabel = new CLabelCtl(pCellDescBack,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("BCe_BombAmmoLabel", layout);
    pBombAmmoLabel->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
    pBombAmmoLabel->SetTextColor(fWhite);
    pCellDescBack->AddControl(pBombAmmoLabel);
    
    
    //Ammo.
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    CNumberCtl* pBombAmmo = new CNumberCtl(pCellDescBack,m_pTextureMan,true);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("BCe_BombAmmo", layout);
    pBombAmmo->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3],layout[4],layout[5],layout[6], lstImage);
    pBombAmmo->SetTextColor(fWhite);
    pBombAmmo->SetNumber(prop->nPackCount);
    pBombAmmo->SetAlign(CNumberCtl::ALIGN_LEFT);
    pCellDescBack->AddControl(pBombAmmo);
    
}

//nType : 1 (Table) , 0 (PopupDlg) , 2 (Desc)
void CMainMenuWorld::InitItemCell(PROPERTY_ITEM* prop,CControl* pParent,float fOffsetX,float fOffsetY,int nType)
{
    float fWhite[] = {1.0f,1.0f,1.0f,1.0f};
    vector<string>  lstImage;
    vector<float> layout;
    string sTemp;
    char sTempDesc2[128];
    const char* sFormat;

    CUserInfo* pUserInfo = mpScenario->GetUserInfo();
    string sDesc1 = prop->sDesc;
    string sDesc2 = prop->sDesc2;
    int grade = -1;
    
    
    //CellTitleBack
    lstImage.clear();
if(nType != 2) //Des가 아니면 타이틀 테두리가 필요하다.
    lstImage.push_back(IMG_TABLE_BANDI);
else
    lstImage.push_back("none");
    lstImage.push_back("none");
    CControl* pCellTitleBack =  new CLabelCtl(pParent,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("ICe_CellTitleBack", layout);
    pCellTitleBack->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3], lstImage, 0.466796875, 0.2978515625, 0.69921875, 0.3203125);
    pParent->AddControl(pCellTitleBack);
    
    //아이템이름
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back(prop->sName);
    CControl* pItemType = new CLabelCtl(pCellTitleBack,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("ICe_ItemType", layout);
    pItemType->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
    pItemType->SetTextColor(fWhite);
    pCellTitleBack->AddControl(pItemType);
    
    //아이템이미지백.
    lstImage.clear();
    lstImage.push_back(IMG_TABLE_BANDI);
    lstImage.push_back("none");
    CControl* pItemIconBack = new CLabelCtl(pParent,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("ICe_ItemIconBack", layout);
    pItemIconBack->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3], lstImage, 0.40234375, 0.685546875, 0.4697265625, 0.7529296875);
    pParent->AddControl(pItemIconBack);
    
    //아이템이미지백.
    sTemp = prop->sModelPath;
    sTemp.append(".tga");
    lstImage.clear();
    lstImage.push_back(sTemp);
    lstImage.push_back("none");
    CControl* pItemIcon = new CLabelCtl(pItemIconBack,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("ICe_ItemIcon", layout);
    pItemIcon->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3], lstImage, 0.0f, 0.0f, 0.96875, 0.96875);
    pItemIconBack->AddControl(pItemIcon);
    
    //아이템Desc백.
    lstImage.clear();
    lstImage.push_back(IMG_TABLE_BANDI);
    lstImage.push_back("none");
    CControl* pItemDescBack = new CLabelCtl(pParent,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("ICe_ItemDescBack", layout);
    pItemDescBack->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3], lstImage, 0.3994140625, 0.6103515625, 0.55859375, 0.6787109375);
    pParent->AddControl(pItemDescBack);
    
    if(sDesc1.length() > 0 && sDesc2.length() > 0)
    {
        //Desc2_1 (설명이 두줄 첫번째)
        lstImage.clear();
        lstImage.push_back("none");
        lstImage.push_back(prop->sDesc);
        CControl* pDesc21 = new CLabelCtl(pItemDescBack,m_pTextureMan);
        layout.clear();
        PROPERTYI::GetPropertyFloatList("ICe_Desc21", layout);
        pDesc21->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
        pDesc21->SetTextColor(fWhite);
        pItemDescBack->AddControl(pDesc21);
        
        
        
        //-------------------------------------------
        //중의 : 어의가 없는 에러 즉 이상하게 에러가나는데 이유가 없음
        // 한글 리소스 중에 "I_D-PWD Cap.Desc2" = "방어파워%d%%업그레이드";
        // 이렇게 하면 아주 심각한 에러가 종종발생한다. "I_D-PWD Cap.Desc2" = "방어파워 %d%% 업그레이드";
        // 뛰어쓰기를 해주면 에러가 안난다. 왜일까나...
        // 암튼 글자를 만들거나 할때 문제가 생기는 것같음.
        if(prop->nID == ID_ITEM_UPGRADE_ATTACK)
        {
            if(nType != 2)
                grade = (pUserInfo->GetAttackUpgrade() + 1) * 5.f;
            else
                grade = pUserInfo->GetAttackUpgrade() * 5.f;
                
            sFormat = SGLTextUnit::GetText(prop->sDesc2);
            sprintf(sTempDesc2, sFormat,grade);
            
        }
        else if(prop->nID == ID_ITEM_UPGRADE_DEPEND)
        {
            //grade = pUserInfo?pUserInfo->GetDependUpgrade(pUserInfo->GetLastSelectedTankID()) + 1: 1;
            if(nType != 2)
                grade = (pUserInfo->GetDependUpgrade() + 1) * 5.f;
            else
                grade = pUserInfo->GetDependUpgrade() * 5.f;
                
            sFormat = SGLTextUnit::GetText(prop->sDesc2);
            sprintf(sTempDesc2, sFormat,grade);
        }
        else if(prop->nID == ID_ITEM_UPGRADE_MISSILE_TOWER || prop->nID == ID_ITEM_UPGRADE_DEFENCE_TOWER)
        {
            //grade = pUserInfo?pUserInfo->GetDependUpgrade(pUserInfo->GetLastSelectedTankID()) + 1: 1;
            grade = (pUserInfo->GetCntByIDWithItem(prop->nID) + 1) * 5.f;
            sFormat = SGLTextUnit::GetText(prop->sDesc2);
            sprintf(sTempDesc2, sFormat,grade);
        }
        else
            strcpy(sTempDesc2, prop->sDesc2);
        //-------------------------------------------
        
        //Desc2_2 (설명이 두줄 두번째)
        lstImage.clear();
        lstImage.push_back("none");
        lstImage.push_back(sTempDesc2);
        //lstImage.push_back("none");
        CControl* pDesc22 = new CLabelCtl(pItemDescBack,m_pTextureMan);
        layout.clear();
        PROPERTYI::GetPropertyFloatList("ICe_Desc22", layout);
        pDesc22->Initialize(9999, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
        pDesc22->SetTextColor(fWhite);
        pItemDescBack->AddControl(pDesc22);
    }
    else
    {
        //Desc1 (설명이 한줄)
        lstImage.clear();
        lstImage.push_back("none");
        lstImage.push_back(prop->sDesc);
        CControl* pDesc11 = new CLabelCtl(pItemDescBack,m_pTextureMan);
        layout.clear();
        PROPERTYI::GetPropertyFloatList("ICe_Desc11", layout);
        pDesc11->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
        pDesc11->SetTextColor(fWhite);
        pItemDescBack->AddControl(pDesc11);
        
    }
}



void CMainMenuWorld::InitFrameBombTable(CControl* pTableBack,float fX,float fY)
{
    float fWhite[] = {1.f,1.f,1.f,1.f};
    vector<string>  lstImage;
    vector<float> layout;
    lstImage.clear();
    lstImage.push_back(IMG_TABLE_BANDI);
    lstImage.push_back("none");
    
    //BombTable
    //테이블
    mpFrameBombTable = new CTableCtl(pTableBack,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("TB_FrameBombTable", layout);
    mpFrameBombTable->Initialize(BTN_BOMBTABLE,fX + layout[0],fY + layout[1], layout[2], layout[3] , lstImage,0.0087890625, 0.6123046875, 0.392578125, 0.9453125);
    ((CScrollCtl*)mpFrameBombTable)->SetContentsBounds(layout[2], 0);
    
    if(gnDisplayType == DISPLAY_IPAD)
        mpFrameBombTable->SetRowSelMode(false); //cell단위로 이벤트를 받지 않음.
    else
        mpFrameBombTable->SetRowSelMode(true);
    
    
    vector<PROPERTY_BOMB*> list;
    PROPERTY_BOMB::GetPropertyBombList(list);
    int nCnt = (int)list.size();
    
    for (int i = 0; i < nCnt; i++) {
        
        PROPERTY_BOMB* prop = list[i];
        if(prop->nMenuVisible == 0) continue;  //기본 폭탄,적 폭탄 은 안보이게 한다.
        
       
        //사실상 CellBack 없이 Cell에서 이미지를 넣을수 잇지만.. 테두리를 만들기 위해서 하지 않음.
        lstImage.clear();
        lstImage.push_back("none");
        lstImage.push_back("none");
        CCellCtl* pCell = new CCellCtl(m_pTextureMan);
        layout.clear();
        PROPERTYI::GetPropertyFloatList("TB_Cell", layout);
        pCell->Initialize(layout[2], layout[3] , lstImage,0.0f,0.0f,1.0f,1.0f);
        
        lstImage.clear();
        lstImage.push_back(IMG_TABLE_BANDI);
        lstImage.push_back("none");
        CControl* pCellBack =  new CLabelCtl(pCell,m_pTextureMan);
        layout.clear();
        PROPERTYI::GetPropertyFloatList("TB_CellBack", layout);
        pCellBack->Initialize(0, layout[0], layout[1], layout[2], layout[3], lstImage, 0.466796875, 0.3232421875, 0.6982421875, 0.4296875);
        pCell->AddControl(pCellBack);
        pCell->SetLData(prop->nID);

        InitBombCell(prop,pCellBack,0,0,1);
        
        
        
        if(gnDisplayType == DISPLAY_IPAD)
        {
            //가격
            lstImage.clear();
            lstImage.push_back(IMG_TABLE_BANDI);
            lstImage.push_back("none");
            CControl* pCellValue =  new CLabelCtl(pCell,m_pTextureMan);
            layout.clear();
            PROPERTYI::GetPropertyFloatList("TB_CellValue", layout);
            pCellValue->Initialize(0, layout[0], layout[1], layout[2], layout[3], lstImage, 0.4716796875, 0.5107421875, 0.5888671875, 0.546875);
            pCell->AddControl(pCellValue);
            
            //GoldICon
            lstImage.clear();
            lstImage.push_back("gold.png");
            lstImage.push_back("none");
            CControl* pGoldIcon =  new CLabelCtl(pCellValue,m_pTextureMan);
            layout.clear();
            PROPERTYI::GetPropertyFloatList("TB_GoldIcon", layout);
            pGoldIcon->Initialize(0, layout[0], layout[1], layout[2], layout[3], lstImage, 0.0f, 0.0f, 1.0f, 1.0f);
            pCellValue->AddControl(pGoldIcon);

            lstImage.clear();
            lstImage.push_back("none");
            lstImage.push_back("none");
            CNumberCtl* pGold =  new CNumberCtl(pCellValue,m_pTextureMan,true);
            layout.clear();
            PROPERTYI::GetPropertyFloatList("TB_Gold", layout);
            pGold->Initialize(0, layout[0], layout[1], layout[2], layout[3],layout[4], layout[5], layout[6],lstImage);
            pGold->SetAlign(CNumberCtl::ALIGN_LEFT);
            pGold->SetTextColor(fWhite);
            pGold->SetNumber(prop->nPrice);
            pCellValue->AddControl(pGold);
            
            //Buy Button
            lstImage.clear();
            lstImage.push_back(IMG_TABLE_BANDI);
            lstImage.push_back("M_IB_BUY");
            CControl* pCellBuyButton =  new CButtonCtl(pCell,m_pTextureMan);
            layout.clear();
            PROPERTYI::GetPropertyFloatList("TB_CellBuyButton", layout);
            pCellBuyButton->Initialize(BTN_BOMBBUY, layout[0], layout[1], layout[2], layout[3], lstImage, 0.5908203125, 0.5107421875, 0.708984375, 0.546875);
            pCellBuyButton->SetLData(prop->nID);
            pCellBuyButton->SetTextColor(fWhite);
            pCell->AddControl(pCellBuyButton);
        }
        else //아이폰일 경우 Buy버튼이 필요가 없다.
        {
            //GoldICon
            lstImage.clear();
            lstImage.push_back("gold.png");
            lstImage.push_back("none");
            CControl* pGoldIcon =  new CLabelCtl(pCellBack,m_pTextureMan);
            layout.clear();
            PROPERTYI::GetPropertyFloatList("TB_GoldIcon", layout);
            pGoldIcon->Initialize(0, layout[0], layout[1], layout[2], layout[3], lstImage, 0.0f, 0.0f, 1.0f, 1.0f);
            pCellBack->AddControl(pGoldIcon);
            
            lstImage.clear();
            lstImage.push_back("none");
            lstImage.push_back("none");
            CNumberCtl* pGold =  new CNumberCtl(pCellBack,m_pTextureMan,true);
            layout.clear();
            PROPERTYI::GetPropertyFloatList("TB_Gold", layout);
            pGold->Initialize(0, layout[0], layout[1], layout[2], layout[3],layout[4], layout[5], layout[6],lstImage);
            pGold->SetAlign(CNumberCtl::ALIGN_LEFT);
            pGold->SetTextColor(fWhite);
            pGold->SetNumber(prop->nPrice);
            pCellBack->AddControl(pGold);
        }
        
        mpFrameBombTable->AddItem(pCell);
    }
    
    pTableBack->AddControl(mpFrameBombTable);
    PROPERTY_BOMB::ReleaseList((vector<PROPERTYI*>*)&list);
}

void CMainMenuWorld::ResetFrameItemTable()
{
    vector<float> layout;
    layout.clear();
    PROPERTYI::GetPropertyFloatList("FT_FrameBar", layout);
    float fX = (gDisplayWidth - layout[2] ) / 2.0f + layout[0]; //282;
    float fY = (gDisplayHeight - layout[3] ) / 2.4f+ layout[1];//106; //약간 상단에 위치하길 원한다.
    float fYPos = mpFrameItembTable->GetScrollVPosition();
    CControl* pTableBack = mpFrameItembTable->GetParent();
    pTableBack->Remove(mpFrameItembTable);
    InitFrameItemTable(pTableBack,fX,fY);
    
    
    CButtonCtl* pItemTab = (CButtonCtl*)mpFrameBombItemTab->FindControl(BTN_ITEMTAB);
    if(pItemTab->GetPushed())        mpFrameItembTable->SetHide(false);
    mpFrameItembTable->SetScrollVPosition(fYPos);
    
}

void CMainMenuWorld::InitFrameItemTable(CControl* pTableBack,float fX,float fY)
{
    float fWhite[] = {1.f,1.f,1.f,1.f};
    CUserInfo* pUserInfo = mpScenario->GetUserInfo();
    vector<string>  lstImage;
    vector<float> layout;
    int nItemCnt = 0;
    lstImage.clear();
    lstImage.push_back(IMG_TABLE_BANDI);
    lstImage.push_back("none");
    
    //if(mpFrameItembTable) delete[] mpFrameItembTable; mpFrameItembTable = NULL;
    
    //BombTable
    //테이블
    mpFrameItembTable = new CTableCtl(pTableBack,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("TB_FrameBombTable", layout);
    mpFrameItembTable->Initialize(BTN_ITEMTABLE,fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage,0.0087890625, 0.6123046875, 0.392578125, 0.9453125);
    ((CScrollCtl*)mpFrameItembTable)->SetContentsBounds(layout[2], 0);
    
    if(gnDisplayType == DISPLAY_IPAD)
        mpFrameItembTable->SetRowSelMode(false); //cell단위로 이벤트를 받지 않음.
    else
        mpFrameItembTable->SetRowSelMode(true);
        
    mpFrameItembTable->SetHide(true);

    
    vector<PROPERTY_ITEM*> list;
    PROPERTY_ITEM::GetPropertyItemList(list);
    int nCnt = (int)list.size();
    
    for (int i = 0; i < nCnt; i++)
    {
        PROPERTY_ITEM* prop = list[i];
        //if(prop->nType == 3 || prop->nType == 6) continue;
        if(prop->nType == 3) continue;
        else if(prop->nType == 1 || prop->nType == 5) //영구사용물을 이미 샀으면 리스트에 추가하지 말자.
        {
            if(pUserInfo)
            {
                bool bIs = false;
                vector<USERINFO*>* pUserItemList = pUserInfo->GetListItem();
                int nSize = (int)pUserItemList->size();
                for(int i = 0; i < nSize; i++)
                {
                    USERINFO *pInfo = (*pUserItemList)[i];
                    if(pInfo->nID == prop->nID)
                    {
                        bIs = true;
                        break;
                    }
                }
                if(bIs) continue;
            }
        }
        
        nItemCnt = pUserInfo->GetCntByIDWithItem(prop->nID);
        
//        if(prop->nID == ID_ITEM_UPGRADE_DEFENCE_TOWER || prop->nID == ID_ITEM_UPGRADE_MISSILE_TOWER)
//        {
//            if(nItemCnt >= 2) //2개이상 사면 리스트에서 사라진다.
//            {
//                continue;
//            }
//        }

        
        //사실상 CellBack 없이 Cell에서 이미지를 넣을수 잇지만.. 테두리를 만들기 위해서 하지 않음.
        lstImage.clear();
        lstImage.push_back("none");
        lstImage.push_back("none");
        CCellCtl* pCell = new CCellCtl(m_pTextureMan);
        layout.clear();
        PROPERTYI::GetPropertyFloatList("TB_Cell", layout);
        pCell->Initialize(layout[2], layout[3] , lstImage,0.0f,0.0f,1.0f,1.0f);
        pCell->SetLData(prop->nID);
        
        //CellBack
        lstImage.clear();
        lstImage.push_back(IMG_TABLE_BANDI);
        lstImage.push_back("none");
        CControl* pCellBack =  new CLabelCtl(pCell,m_pTextureMan);
        layout.clear();
        PROPERTYI::GetPropertyFloatList("TB_CellBack", layout);
        pCellBack->Initialize(0, layout[0], layout[1], layout[2], layout[3], lstImage, 0.466796875, 0.3232421875, 0.6982421875, 0.4296875);
        pCell->AddControl(pCellBack);
        pCell->SetLData(prop->nID);
        
        //1은 테이블에서 부름
        InitItemCell(prop,pCellBack,0,0,1);
        
        
        int nGold = prop->nPrice;
        if(prop->nID == ID_ITEM_UPGRADE_ATTACK)
        {
            int grade = pUserInfo?pUserInfo->GetAttackUpgrade() + 1: 1;
            nGold = prop->nPrice * grade;
        }
        else if(prop->nID == ID_ITEM_UPGRADE_DEPEND)
        {
            int grade = pUserInfo?pUserInfo->GetDependUpgrade() + 1: 1;
            nGold = prop->nPrice * grade;
        }
        else if(prop->nID == ID_ITEM_UPGRADE_DEFENCE_TOWER || prop->nID == ID_ITEM_UPGRADE_MISSILE_TOWER)
        {
            nGold = prop->nPrice * (nItemCnt + 1);
        }
        
        if(gnDisplayType == DISPLAY_IPAD)
        {
            //가격
            lstImage.clear();
            lstImage.push_back(IMG_TABLE_BANDI);
            lstImage.push_back("none");
            CControl* pCellValue =  new CLabelCtl(pCell,m_pTextureMan);
            layout.clear();
            PROPERTYI::GetPropertyFloatList("TB_CellValue", layout);
            pCellValue->Initialize(0, layout[0], layout[1], layout[2], layout[3], lstImage, 0.4716796875, 0.5107421875, 0.5888671875, 0.546875);
            pCell->AddControl(pCellValue);
            
            //GoldICon
            if(!(prop->nType == 4 || prop->nType == 5))
            {
                lstImage.clear();
                lstImage.push_back("gold.png");
                lstImage.push_back("none");
                CControl* pGoldIcon =  new CLabelCtl(pCellValue,m_pTextureMan);
                layout.clear();
                PROPERTYI::GetPropertyFloatList("TB_GoldIcon", layout);
                pGoldIcon->Initialize(0, layout[0], layout[1], layout[2], layout[3], lstImage, 0.0f, 0.0f, 1.0f, 1.0f);
                pCellValue->AddControl(pGoldIcon);
                
                lstImage.clear();
                lstImage.push_back("none");
                lstImage.push_back("none");
                CNumberCtl* pGold =  new CNumberCtl(pCellValue,m_pTextureMan,true);
                layout.clear();
                PROPERTYI::GetPropertyFloatList("TB_Gold", layout);
                pGold->Initialize(9998, layout[0], layout[1], layout[2], layout[3],layout[4], layout[5], layout[6],lstImage);
                pGold->SetAlign(CNumberCtl::ALIGN_LEFT);
                pGold->SetTextColor(fWhite);
                pGold->SetNumber(nGold);
                pCellValue->AddControl(pGold);

            }
            else
            {
                lstImage.clear();
                lstImage.push_back("dollar.png");
                lstImage.push_back("none");
                CControl* pGoldIcon =  new CLabelCtl(pCellValue,m_pTextureMan);
                layout.clear();
                PROPERTYI::GetPropertyFloatList("TB_GoldIcon", layout);
                pGoldIcon->Initialize(0, layout[0], layout[1], layout[2], layout[3], lstImage, 0.0f, 0.0f, 1.0f, 1.0f);
                pCellValue->AddControl(pGoldIcon);
                
                lstImage.clear();
                lstImage.push_back("none");
                lstImage.push_back("none");
                CNumberCtl* pGold =  new CNumberCtl(pCellValue,m_pTextureMan,true);
                layout.clear();
                PROPERTYI::GetPropertyFloatList("TB_Gold", layout);
                pGold->Initialize(9998, layout[0], layout[1], layout[2], layout[3],layout[4], layout[5], layout[6],lstImage);
                pGold->SetAlign(CNumberCtl::ALIGN_LEFT);
                pGold->SetTextColor(fWhite);
                pGold->SetNumberF(nGold / 1000.f);
                pCellValue->AddControl(pGold);

                
            }
            
            
            //Buy Button
            lstImage.clear();
            lstImage.push_back(IMG_TABLE_BANDI);
            lstImage.push_back("M_IB_BUY");
            CControl* pCellBuyButton =  new CButtonCtl(pCell,m_pTextureMan);
            layout.clear();
            PROPERTYI::GetPropertyFloatList("TB_CellBuyButton", layout);
            pCellBuyButton->Initialize(BTN_ITEMBUY, layout[0], layout[1], layout[2], layout[3], lstImage, 0.5908203125, 0.5107421875, 0.708984375, 0.546875);
            pCellBuyButton->SetLData(prop->nID);
            pCellBuyButton->SetTextColor(fWhite);
            pCell->AddControl(pCellBuyButton);
        }
        else
        {
            
            //GoldICon
            if(!(prop->nType == 4 || prop->nType == 5))
            {
                lstImage.clear();
                lstImage.push_back("gold.png");
                lstImage.push_back("none");
                CControl* pGoldIcon =  new CLabelCtl(pCellBack,m_pTextureMan);
                layout.clear();
                PROPERTYI::GetPropertyFloatList("TB_GoldIcon", layout);
                pGoldIcon->Initialize(0, layout[0], layout[1], layout[2], layout[3], lstImage, 0.0f, 0.0f, 1.0f, 1.0f);
                pCellBack->AddControl(pGoldIcon);
                
                lstImage.clear();
                lstImage.push_back("none");
                lstImage.push_back("none");
                CNumberCtl* pGold =  new CNumberCtl(pCellBack,m_pTextureMan,true);
                layout.clear();
                PROPERTYI::GetPropertyFloatList("TB_Gold", layout);
                pGold->Initialize(9998, layout[0], layout[1], layout[2], layout[3],layout[4], layout[5], layout[6],lstImage);
                pGold->SetAlign(CNumberCtl::ALIGN_LEFT);
                pGold->SetTextColor(fWhite);
                pGold->SetNumber(nGold);
                pCellBack->AddControl(pGold);
            }
            else
            {
                lstImage.clear();
                lstImage.push_back("dollar.png");
                lstImage.push_back("none");
                CControl* pGoldIcon =  new CLabelCtl(pCellBack,m_pTextureMan);
                layout.clear();
                PROPERTYI::GetPropertyFloatList("TB_GoldIcon", layout);
                pGoldIcon->Initialize(0, layout[0], layout[1], layout[2], layout[3], lstImage, 0.0f, 0.0f, 1.0f, 1.0f);
                pCellBack->AddControl(pGoldIcon);
                
                
                lstImage.clear();
                lstImage.push_back("none");
                lstImage.push_back("none");
                CNumberCtl* pGold =  new CNumberCtl(pCellBack,m_pTextureMan,true);
                layout.clear();
                PROPERTYI::GetPropertyFloatList("TB_Gold", layout);
                pGold->Initialize(9998, layout[0], layout[1], layout[2], layout[3],layout[4], layout[5], layout[6],lstImage);
                pGold->SetAlign(CNumberCtl::ALIGN_LEFT);
                pGold->SetTextColor(fWhite);
                pGold->SetNumberF(nGold / 1000.f);
                pCellBack->AddControl(pGold);
            }
            
            
        }
        
        
        mpFrameItembTable->AddItem(pCell);
    }
    pTableBack->AddControl(mpFrameItembTable);
    PROPERTY_ITEM::ReleaseList((vector<PROPERTYI*>*)&list);
}

void CMainMenuWorld::InitFrameTable()
{
    vector<string>  lstImage;
    vector<float> layout;
    vector<float> layout2;
    float fMoveY = 0.f;
    float fWhite[] = {1.0f,1.0f,1.0f,1.0f};
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    mpFrameBar =  new CLabelCtl(NULL,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("FT_FrameBar", layout);
    
    
    if(gnDisplayType == DISPLAY_IPHONE)
    {
        PROPERTYI::GetPropertyFloatList("FT_TableBombTab", layout2);
        fMoveY = layout2[3];
    }
    
    float fX = (gDisplayWidth - layout[2] ) / 2.0f + layout[0]; //282;
    float fY = (gDisplayHeight - layout[3] ) / 2.4f+ layout[1];//106; //약간 상단에 위치하길 원한다.

    mpFrameBar->Initialize(0, fX, fY  - fMoveY, layout[2], layout[3] + fMoveY, lstImage, 0.f, 0.f, 0.f, 0.f);
    mpFrameBar->SetHide(true); //일단 숨겨주자.
    
    
    lstImage.clear();
    lstImage.push_back(IMG_TABLE_BANDI);
    lstImage.push_back("none");
    
    //테이블 테두리
    CControl* pTableRecBack =  new CLabelCtl(mpFrameBar,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("FT_TableRecBack", layout);
    pTableRecBack->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.0087890625, 0.2119140625, 0.46484375, 0.6044921875);
    mpFrameBar->AddControl(pTableRecBack);
    
    
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    mpFrameBombItemTab = new CLabelCtl(mpFrameBar,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("FT_FrameBombItemTab", layout);
    mpFrameBombItemTab->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.0087890625, 0.2119140625, 0.46484375, 0.6044921875);
    
    //폭탄탭
    lstImage.clear();
    lstImage.push_back(IMG_TABLE_BANDI);
    lstImage.push_back("M_Tap_Bomb");
    CButtonCtl* pTableBombTab = new CButtonCtl(mpFrameBombItemTab,m_pTextureMan,BUTTON_GROUP_TOGGLE);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("FT_TableBombTab", layout);
    pTableBombTab->Initialize(BTN_BOMBTAB, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage,
                              0.5498046875, 0.21875, 0.6279296875, 0.2529296875,
                              0.466796875, 0.21875, 0.544921875, 0.2529296875
                              );
    pTableBombTab->SetPushed(true);
    pTableBombTab->SetTextColor(fWhite);
    mpFrameBombItemTab->AddControl(pTableBombTab);
    
    
    //아이템탭
    lstImage.clear();
    lstImage.push_back(IMG_TABLE_BANDI);
    lstImage.push_back("M_Tap_Item");
    CButtonCtl* pTableItemTab = new CButtonCtl(mpFrameBombItemTab,m_pTextureMan,BUTTON_GROUP_TOGGLE);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("FT_TableItemTab", layout);
    pTableItemTab->Initialize(BTN_ITEMTAB, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage,
                              0.5498046875, 0.21875, 0.6279296875, 0.2529296875,
                              0.466796875, 0.21875, 0.544921875, 0.2529296875
                              );
    pTableItemTab->SetTextColor(fWhite);
    mpFrameBombItemTab->AddControl(pTableItemTab);
    
    mpFrameBar->AddControl(mpFrameBombItemTab);
    

    //테이블 백그라운드
    CControl* pTableBack =  new CLabelCtl(pTableRecBack,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("FT_TableBack", layout);
    pTableBack->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.0087890625, 0.6123046875, 0.392578125, 0.9453125);
    pTableRecBack->AddControl(pTableBack);
    
    
    //282,106
    InitFrameBombTable(pTableBack,fX,fY);
    InitFrameItemTable(pTableBack,fX,fY);
    
    mControlList.push_back(mpFrameBar);
}

void CMainMenuWorld::InitMapATankRollCtl()
{
    CUserInfo* pUserInfo = mpScenario->GetUserInfo();
    //-----------------------------------------------------------------------
    if(gnDisplayType == DISPLAY_IPAD)
        mpRollCtl = new CRollMapTankCtl(NULL,mpScenario?mpScenario->GetUserInfo():NULL,GetTextureMan(),10,0.15f);
    else
        mpRollCtl = new CRollMapTankCtl(NULL,mpScenario?mpScenario->GetUserInfo():NULL,GetTextureMan(),10,0.3f);
    
    mpRollCtl->Initialize(BTN_MENU2_MAP_TANK_ROLL);
    mpRollCtl->SetCurrentPos(pUserInfo?pUserInfo->GetLastSelectedMapIndex():0, pUserInfo?pUserInfo->GetLastSelectedTankIndex():0);
    mControlList.push_back(mpRollCtl);
    mpRollCtl->SetHide(true);
    //-----------------------------------------------------------------------
}

void CMainMenuWorld::InitializeByResized()
{
    if(mnStatus & 0x0002) //초기화가 되어 있다면 다시 초기화를 해준다.
    {
        DeleteObject();
        initProjection(); //xmax_3d,ymax_3d 재계한되어야 한다.
        Initialize(NULL);
    }
}

int CMainMenuWorld::Initialize(void* pResource)
{
	CWorld::Initialize(pResource);
    if(pResource)
    {
        char sFilePath[256];
        sprintf(sFilePath,"%s/",(const char*)pResource);
        CLoader::setSearchPath((const char*)sFilePath);
    }
    
	
	mnStatus = 0x0001;
    
    if(pResource)
        Clear();
    
    //배경초기화
    InitBackground();
    //상단 골드 타이틀 초기화
    InitTop();
    //왼쪽 메뉴를 초기화 해준다.
    InitMenuBar();
    //Weapon Slot
//    InitWeaponSlot();
    //Help Settings Bar
    InitHelpAndSettingsMenu();
    //Map 설명
    InitMapDescBar();
    //Tank 설명
    InitTankSelectedDescBar();
    InitTankDescBar();
    //Bomb Item Table
    InitFrameTable();
    //Rank Bar
    InitRankBar();
    //Start Bar
    InitStartBar();
    //Item Slot
    InitItemSlot();

    //마지막으로 맵/탱크 컨트롤을 초기화 해준다.
    InitMapATankRollCtl();
    
    //화면에 적용해준다.
    CUserInfo* pUserInfo = mpScenario->GetUserInfo();
    OnSelectedTank(pUserInfo->GetLastSelectedTankID(),false);
    OnSelectedTank(pUserInfo->GetLastSelectedTankID(),true);
    
    if(mpSoundMan)
    {
        delete mpSoundMan;
        mpSoundMan = NULL;
    }
    
    
#ifndef MAC
    mpSoundMan = new CSoundMan;
    mpSoundMan->Initialize(0);
    PlayBgSound("MenuBak");
#endif
    
//저장되어 있다면. 적용해보자.
#if defined(ANDROIDAPPSTOREKIT)
    SGLEvent pEvent;
    pEvent.nMsgID = SGL_BUY_PRODUCT_GOLDTTYPE_FOR_ANDROID;
    OnEvent(&pEvent);
#endif //ANDROIDAPPSTOREKIT
    
    //-------------------------------------------------------
    //마지막 상태를 저장한후에 그상태로 복원을 해주어야 할것 같다.
    if(pResource)
        SetAniStep(MENUANI_WORLD_TO_MAP);
    else
        mpRollCtl->SetHide(false);
    //-------------------------------------------------------
    
    
    //-----------------------------------
	this->ResetFrustum(true); //모든것이 초기화 되었기때문에 프리즘을 다시 설정한다.
	//-----------------------------------------------------
	mnStatus |= 0x0002;
	return E_SUCCESS;	
}

#ifdef HELPBTN
void CMainMenuWorld::ShowHelpDlg()
{
    float fWhite[] = {1.0f,1.0f,1.0f,1.f};
    vector<string>  lstImage;
    vector<float> layout;
    
    lstImage.clear();
    lstImage.push_back(IMG_TABLE_BANDI);
    lstImage.push_back("none");
    if(mpHelpDlg) delete mpHelpDlg;
    mpHelpDlg = new CLabelCtl(NULL,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("HD_HelpDlg", layout);
    float fX = (gDisplayWidth - layout[2]) / 2.0f;
    float fY = (gDisplayHeight - layout[3]) / 2.0f;
    mpHelpDlg->Initialize(0, fX, fY, layout[2], layout[3], lstImage, 0.5703125, 0.611328125, 0.9541015625, 0.9443359375);
    //-----------------------------------------------------------------------------------------------------------
    
    //테이블
    lstImage.push_back(IMG_TABLE_BACK); //반드시 테이블 배경은 존재해야 한다. (영역버퍼를 만들기 때문이다.)
    lstImage.push_back("none");
    CTableCtl* pHelpTableCtl = new CTableCtl(mpHelpDlg,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("HD_HelpTableCtl", layout);
    pHelpTableCtl->Initialize(0,fX + layout[0], fY + layout[1], layout[2], layout[3] , lstImage,0.f,0.f,1.f,1.f);
    //Horizen
    ((CScrollCtl*)pHelpTableCtl)->SetContentsBounds(0, layout[3]);
    pHelpTableCtl->SetRowSelMode(true); //cell단위로 이벤트를 받지 않음.

    
    const char* sName[] = { "AHelp.png","BHelp.png","CHelp.png","DHelp.png","EHelp.png"};
    const char* sTitle[] = {
        "HMove",
        "HAttack",
        "HBomb",
        "HItem",
        "HMission"};
    
    const char* sHDesc[] = {
        "HMoveDesc",
        "HAttackDesc",
        "HBombDesc",
        "HItemDesc",
        "HMissionDesc"};
    
    const char* sHDesc2[] = {
        "",
        "",
        "",
        "",
        "HMissionDesc2"};
    
    const char* sHScenario[] = {
        "HDes_1",
        "HDes_2",
        "HDes_3",
        "HDes_4",
        ""};
    
    const char* sHScenario2[] = {
        "",
        "HDes_2_1",
        "",
        "",
        ""};
    
    
    int nCnt = sizeof(sName) / sizeof(const char*);
    //float fRed[] = { 1.f,0.f,0.f,1.f};
    //float fBlack[] = {0.f,0.f,0.f,1.f};
    float fYellow[] = { 1.f,1.f,0.f,1.f};
    for (int i = 0; i < nCnt; i++) {

        
        lstImage.clear();
        lstImage.push_back("none");
        lstImage.push_back("none");
        CCellCtl* pCell = new CCellCtl(m_pTextureMan);
        layout.clear();
        PROPERTYI::GetPropertyFloatList("HD_Cell", layout);
        pCell->Initialize(layout[2], layout[3] , lstImage,0.f,0.f,1.f,1.f);

        CLabelCtl* pTitleCtl = new CLabelCtl(pCell,m_pTextureMan);
        lstImage.clear();
        lstImage.push_back("none");
        lstImage.push_back(sTitle[i]);
        layout.clear();
        PROPERTYI::GetPropertyFloatList("HD_TitleCtl", layout);
        pTitleCtl->Initialize(0, layout[0], layout[1], layout[2], layout[3],  lstImage,0.f,0.f,1.f,1.f);
        pTitleCtl->SetTextColor(fWhite);
        pCell->AddControl(pTitleCtl);
        
        CLabelCtl* pBack = new CLabelCtl(pCell,m_pTextureMan);
        lstImage.clear();
        lstImage.push_back(sName[i]);
        lstImage.push_back("none");
        layout.clear();
        PROPERTYI::GetPropertyFloatList("HD_Back", layout);
        pBack->Initialize(0, layout[0], layout[1], layout[2], layout[3],  lstImage,0.f,0.f,1.f,1.f);//260,70,512,500
        pCell->AddControl(pBack);

        
        
        CLabelCtl* pHelpCtl = new CLabelCtl(pCell,m_pTextureMan);
        lstImage.clear();
        lstImage.push_back("none");
        lstImage.push_back(sHDesc[i]);
        layout.clear();
        PROPERTYI::GetPropertyFloatList("HD_HelpCtl", layout);
        pHelpCtl->Initialize(0, layout[0], layout[1], layout[2], layout[3],  lstImage,0.f,0.f,1.f,1.f);
        pHelpCtl->SetTextColor(fWhite);
        pCell->AddControl(pHelpCtl);

        
        if(*sHDesc2[i] != 0)
        {
            CLabelCtl* pHelpCtl2 = new CLabelCtl(pCell,m_pTextureMan);
            lstImage.clear();
            lstImage.push_back("none");
            lstImage.push_back(sHDesc2[i]);
            layout.clear();
            PROPERTYI::GetPropertyFloatList("HD_HelpCtl2", layout);
            pHelpCtl2->Initialize(0, layout[0], layout[1], layout[2], layout[3],  lstImage,0.f,0.f,1.f,1.f);
            pHelpCtl2->SetTextColor(fWhite);
            pCell->AddControl(pHelpCtl2);
        }

        
        if(*sHScenario[i] != 0)
        {
            CLabelCtl* pHelpCtl3 = new CLabelCtl(pCell,m_pTextureMan);
            lstImage.clear();
            lstImage.push_back("none");
            lstImage.push_back(sHScenario[i]);
            layout.clear();
            PROPERTYI::GetPropertyFloatList("HD_HelpCtl3", layout);
            pHelpCtl3->Initialize(0, layout[0], layout[1], layout[2], layout[3],  lstImage,0.f,0.f,1.f,1.f);
            pHelpCtl3->SetTextColor(fYellow);
            pCell->AddControl(pHelpCtl3);
        }

        if(*sHScenario2[i] != 0)
        {
            CLabelCtl* pHelpCtl4 = new CLabelCtl(pCell,m_pTextureMan);
            lstImage.clear();
            lstImage.push_back("none");
            lstImage.push_back(sHScenario2[i]);
            layout.clear();
            PROPERTYI::GetPropertyFloatList("HD_HelpCtl4", layout);
            pHelpCtl4->Initialize(0, layout[0], layout[1], layout[2], layout[3],  lstImage,0.f,0.f,1.f,1.f);
            pHelpCtl4->SetTextColor(fYellow);
            pCell->AddControl(pHelpCtl4);
        }
        
        pHelpTableCtl->AddItemHorizon(pCell);
        
    }
    
    //재조정
    pHelpTableCtl->ResetScroll();
    mpHelpDlg->AddControl(pHelpTableCtl);


}
#endif //HELPBTN

void CMainMenuWorld::ShowSettingsDlg()
{
    float fWhite[] = {1.0f,1.0f,1.0f,1.f};
    vector<float> layout;
    CUserInfo* pUserInfo = mpScenario->GetUserInfo();
    vector<string>  lstImage;
    lstImage.clear();
    lstImage.push_back(IMG_TABLE_BANDI);
    lstImage.push_back("none");
    if(mpSettingsDlg) delete mpSettingsDlg;
    
    mpSettingsDlg = new CLabelCtl(NULL,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SD_SettingsDlg", layout);
    float fX = (gDisplayWidth - layout[2]) / 2.0f + layout[0];
    float fY = (gDisplayHeight - layout[3]) / 2.0f + layout[1];
    layout.clear();
    mpSettingsDlg->Initialize(0, fX, fY, layout[2], layout[3], lstImage, 0.5703125, 0.611328125, 0.9541015625, 0.9443359375);
    
    
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("S_Title");
    CControl* pTitle = new CLabelCtl(mpSettingsDlg,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SD_Title", layout);
    pTitle->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
    pTitle->SetTextColor(fWhite);
    mpSettingsDlg->AddControl(pTitle);
    
    
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("S_Music");
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SD_BGMTitle", layout);
    CControl* pBGMTitle = new CLabelCtl(mpSettingsDlg,m_pTextureMan);
    pBGMTitle->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
    pBGMTitle->SetTextColor(fWhite);
    mpSettingsDlg->AddControl(pBGMTitle);
    
    lstImage.clear();
    lstImage.push_back(IMG_TABLE_BANDI);
    lstImage.push_back("none");
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SD_BGMBtn", layout);
    CButtonCtl* pBGMBtn = new CButtonCtl(mpSettingsDlg,m_pTextureMan,BUTTON_TOGGLE);
    pBGMBtn->Initialize(BTN_SETTINGS_BGM, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage,
                        0.4072265625, 0.814453125, 0.478515625, 0.8486328125,
                        0.4072265625, 0.7744140625, 0.478515625, 0.80859375);
    mpSettingsDlg->AddControl(pBGMBtn);
    if(pUserInfo->GetMusic() == 1)
        pBGMBtn->SetPushed(true);
    

    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("S_SoundFX");
    CControl* pFXTitle = new CLabelCtl(mpSettingsDlg,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SD_FXTitle", layout);
    pFXTitle->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
    pFXTitle->SetTextColor(fWhite);
    mpSettingsDlg->AddControl(pFXTitle);

    lstImage.clear();
    lstImage.push_back(IMG_TABLE_BANDI);
    lstImage.push_back("none");
    CButtonCtl* pFXBtn = new CButtonCtl(mpSettingsDlg,m_pTextureMan,BUTTON_TOGGLE);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SD_FXBtn", layout);
    pFXBtn->Initialize(BTN_SETTINGS_FX, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage,
                        0.4072265625, 0.814453125, 0.478515625, 0.8486328125,
                        0.4072265625, 0.7744140625, 0.478515625, 0.80859375);
    mpSettingsDlg->AddControl(pFXBtn);
    if(pUserInfo->GetSoundFX() == 1)
        pFXBtn->SetPushed(true);
    
    
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("S_Vibration");
    CControl* pVibrationTitle = new CLabelCtl(mpSettingsDlg,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SD_VibrationTitle", layout);
    pVibrationTitle->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
    pVibrationTitle->SetTextColor(fWhite);
    mpSettingsDlg->AddControl(pVibrationTitle);
    
    lstImage.clear();
    lstImage.push_back(IMG_TABLE_BANDI);
    lstImage.push_back("none");
    CButtonCtl* pVibrationBTN = new CButtonCtl(mpSettingsDlg,m_pTextureMan,BUTTON_TOGGLE);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SD_VibrationBTN", layout);
    pVibrationBTN->Initialize(BTN_SETTINGS_VIBRATION, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage,
                       0.4072265625, 0.814453125, 0.478515625, 0.8486328125,
                       0.4072265625, 0.7744140625, 0.478515625, 0.80859375);
    mpSettingsDlg->AddControl(pVibrationBTN);
    if(pUserInfo->GetVibration() == 1)
        pVibrationBTN->SetPushed(true);
    
    
    lstImage.clear();
    lstImage.push_back(IMG_TABLE_BANDI);
    lstImage.push_back("M_IB_SAVE");
    CControl* pSaveBtn = new CButtonCtl(mpSettingsDlg,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SD_SaveBtn", layout);
    pSaveBtn->Initialize(BTN_SETTINGS_SAVE, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.4169921875, 0.859375, 0.533203125, 0.896484375);
    mpSettingsDlg->AddControl(pSaveBtn);
    
    
    lstImage.clear();
    lstImage.push_back(IMG_TABLE_BANDI);
    lstImage.push_back("M_IB_CANCEL");
    CControl* pCancelBtn = new CButtonCtl(mpSettingsDlg,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SD_CancelBtn", layout);
    pCancelBtn->Initialize(BTN_SETTINGS_CANCEL, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.41796875, 0.8994140625, 0.5322265625, 0.93359375);
    pCancelBtn->SetTextColor(fWhite);
    mpSettingsDlg->AddControl(pCancelBtn);
    

}

void CMainMenuWorld::ShowBuyDlg(int nID,int nType)
{
    float fWhite[] = {1.f,1.f,1.f,1.f};
    if(mpBuyDlg) delete mpBuyDlg;
    
    vector<string>  lstImage;
    vector<float> layout;
    vector<float> layout0;
    lstImage.clear();
    lstImage.push_back(IMG_TABLE_BANDI);
    lstImage.push_back("none");
    
    
    //테이블 백그라운드 319, 157, 393, 341 의 테이블 백그라운드
    layout0.clear();
    PROPERTYI::GetPropertyFloatList("BD_BuyDlg0", layout0);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("BD_BuyDlg", layout);

    mpBuyDlg = new CLabelCtl(NULL,m_pTextureMan);
    float fX = (layout0[2] - layout[2]) / 2.0f + layout0[0];
    float fY = (layout0[3] - layout[3]) / 2.0f + layout0[1];
    
    if( gnDisplayType == DISPLAY_IPHONE)
    {
        fX = (gDisplayWidth - layout[2]) / 2.f + layout[0];
        fY = (gDisplayHeight - layout[3]) / 2.f+ layout[1];
    }
    

    mpBuyDlg->Initialize(nType, fX, fY, layout[2], layout[3], lstImage, 0.0068359375, 0.01171875, 0.3564453125, 0.20703125);

    
    mpBuyDlg->SetLData(nID);
    
    float fXT,fYT;
    
    if(nType == 0) //Bomb
    {
        PROPERTY_BOMB Prop;
        PROPERTY_BOMB::GetPropertyBomb(nID, Prop);
        layout.clear();
        PROPERTYI::GetPropertyFloatList("BD_BombCell", layout);
        fXT = fX + layout[2];
        fYT = fY + layout[3];
        
        InitBombCell(&Prop, mpBuyDlg, fXT, fYT, 0);
                
        lstImage.clear();
        lstImage.push_back("gold.png");
        lstImage.push_back("none"); //B_Dlg_Gold
        CControl* pGoldLabel = new CLabelCtl(mpBuyDlg,m_pTextureMan);
        layout.clear();
        PROPERTYI::GetPropertyFloatList("BD_GoldLabel", layout);
        fXT = fX + layout[0];
        fYT = fY + layout[1];
        pGoldLabel->Initialize(0, fXT, fYT, layout[2], layout[3], lstImage, 0.0f, 0.0f, 1.0f, 1.0f);
        pGoldLabel->SetTextColor(fWhite);
        mpBuyDlg->AddControl(pGoldLabel);
        
        
        lstImage.clear();
        lstImage.push_back("none");
        lstImage.push_back("none");
        CNumberCtl* pGold = new CNumberCtl(mpBuyDlg,m_pTextureMan,true);
        layout.clear();
        PROPERTYI::GetPropertyFloatList("BD_Gold", layout);
        fXT = fX + layout[0];
        fYT = fY + layout[1];
        pGold->Initialize(0, fXT, fYT, layout[2], layout[3],layout[4],layout[5],layout[6], lstImage);
        pGold->SetTextColor(fWhite);
        pGold->SetAlign(CNumberCtl::ALIGN_LEFT);
        pGold->SetNumber(Prop.nPrice);
        mpBuyDlg->AddControl(pGold);
        
        
        lstImage.clear();
        lstImage.push_back(IMG_TABLE_BANDI);
        lstImage.push_back("M_IB_OK");
        CControl* pBtnOK = new CButtonCtl(mpBuyDlg,m_pTextureMan);
        layout.clear();
        PROPERTYI::GetPropertyFloatList("BD_BtnOK", layout);
        fXT = fX + layout[0];
        fYT = fY + layout[1];
        pBtnOK->Initialize(BTN_BOMBBUYOK, fXT, fYT, layout[2], layout[3], lstImage, 0.3623046875, 0.0908203125, 0.4814453125, 0.1279296875);
        pBtnOK->SetLData(nID);
        pBtnOK->SetTextColor(fWhite);
        mpBuyDlg->AddControl(pBtnOK);
        
        
        lstImage.clear();
        lstImage.push_back(IMG_TABLE_BANDI);
        lstImage.push_back("M_IB_CANCEL");
        CControl* pBtnCancel = new CButtonCtl(mpBuyDlg,m_pTextureMan);
        layout.clear();
        PROPERTYI::GetPropertyFloatList("BD_BtnCancel", layout);
        fXT = fX + layout[0];
        fYT = fY + layout[1];
        pBtnCancel->Initialize(BTN_ITEMBOMBBUYCANCEL, fXT, fYT, layout[2], layout[3], lstImage, 0.3623046875, 0.1318359375, 0.478515625, 0.1767578125);
        pBtnCancel->SetTextColor(fWhite);
        mpBuyDlg->AddControl(pBtnCancel);
    }
    else if(nType == 1) //Item
    {
        CUserInfo* pUserInfo = mpScenario->GetUserInfo();
        PROPERTY_ITEM Prop;
        PROPERTY_ITEM::GetPropertyItem(nID, Prop);
        
        if(!(Prop.nType == 4 || Prop.nType == 5))
        {
    //        layout.clear();
    //        PROPERTYI::GetPropertyFloatList("BD_GoldLabel", layout);
    //        fXT = fX + layout[2];
    //        fYT = fY + layout[3];
            
            layout.clear();
            PROPERTYI::GetPropertyFloatList("BD_BombCell", layout);
            fXT = fX + layout[2];
            fYT = fY + layout[3];
            
            //0 은 popup dlg에서 부름
            InitItemCell(&Prop, mpBuyDlg, fXT, fYT, 0);
            
            lstImage.clear();
            lstImage.push_back("gold.png");
            lstImage.push_back("none"); //B_Dlg_Gold
            CControl* pGoldLabel = new CButtonCtl(mpBuyDlg,m_pTextureMan);
            layout.clear();
            PROPERTYI::GetPropertyFloatList("BD_GoldLabel", layout);
            fXT = fX + layout[0];
            fYT = fY + layout[1];
            pGoldLabel->Initialize(0, fXT, fYT, layout[2], layout[3], lstImage, 0.0f, 0.0f, 1.0f, 1.0f);
            pGoldLabel->SetTextColor(fWhite);
            mpBuyDlg->AddControl(pGoldLabel);
            
            
            int nGold = Prop.nPrice;
            if(Prop.nID == ID_ITEM_UPGRADE_ATTACK)
            {
                int grade = pUserInfo?pUserInfo->GetAttackUpgrade() + 1: 1;
                nGold = Prop.nPrice * grade;
            }
            else if(Prop.nID == ID_ITEM_UPGRADE_DEPEND)
            {
                int grade = pUserInfo?pUserInfo->GetDependUpgrade() + 1: 1;
                nGold = Prop.nPrice * grade;
            }
            else if(Prop.nID == ID_ITEM_UPGRADE_DEFENCE_TOWER || Prop.nID == ID_ITEM_UPGRADE_MISSILE_TOWER)
            {
                int grade = pUserInfo->GetCntByIDWithItem(Prop.nID) + 1;
                nGold = Prop.nPrice * grade;
            }

            
            lstImage.clear();
            lstImage.push_back("none");
            lstImage.push_back("none");
            CNumberCtl* pGold = new CNumberCtl(mpBuyDlg,m_pTextureMan,true);
            layout.clear();
            PROPERTYI::GetPropertyFloatList("BD_Gold", layout);
            fXT = fX + layout[0];
            fYT = fY + layout[1];
            pGold->Initialize(0, fXT, fYT, layout[2], layout[3],layout[4],layout[5],layout[6], lstImage);
            pGold->SetTextColor(fWhite);
            pGold->SetAlign(CNumberCtl::ALIGN_LEFT);
            pGold->SetNumber(nGold);
            mpBuyDlg->AddControl(pGold);
            
            
            lstImage.clear();
            lstImage.push_back(IMG_TABLE_BANDI);
            lstImage.push_back("M_IB_OK");
            CControl* pBtnOK = new CButtonCtl(mpBuyDlg,m_pTextureMan);
            layout.clear();
            PROPERTYI::GetPropertyFloatList("BD_BtnOK", layout);
            fXT = fX + layout[0];
            fYT = fY + layout[1];
            pBtnOK->Initialize(BTN_ITEMBUYOK, fXT, fYT, layout[2], layout[3], lstImage, 0.3623046875, 0.0908203125, 0.4814453125, 0.1279296875);
            pBtnOK->SetLData(nID);
            pBtnOK->SetTextColor(fWhite);
            mpBuyDlg->AddControl(pBtnOK);
            
            
            lstImage.clear();
            lstImage.push_back(IMG_TABLE_BANDI);
            lstImage.push_back("M_IB_CANCEL");
            CControl* pBtnCancel = new CButtonCtl(mpBuyDlg,m_pTextureMan);
            layout.clear();
            PROPERTYI::GetPropertyFloatList("BD_BtnCancel", layout);
            fXT = fX + layout[0];
            fYT = fY + layout[1];
            pBtnCancel->Initialize(BTN_ITEMBOMBBUYCANCEL, fXT, fYT, layout[2], layout[3], lstImage, 0.3623046875, 0.1318359375, 0.478515625, 0.1767578125);
            pBtnCancel->SetTextColor(fWhite);
            mpBuyDlg->AddControl(pBtnCancel);
        }
        else
        {
            if(mpBuyDlg)
            {
                delete mpBuyDlg;
                mpBuyDlg = NULL;
            }
            
#if defined(APPSTOREKIT) || defined(ANDROIDAPPSTOREKIT)
            string sID = GetBundleID();
            sID.append(".");
            sID.append(Prop.sID);
            gShowShopDlg(sID.c_str(),(void*)this,Prop.nType); //SongGL프로젝트는 NOFREE가 없다.
#endif //defined(APPSTOREKIT) || defined(ANDROIDAPPSTOREKIT)
            
        }
    }
  
}



void CMainMenuWorld::CloseDlg()
{
    if(mpBuyDlg)
    {
        delete mpBuyDlg;
        mpBuyDlg = NULL;
    }
    
#ifdef HELPBTN
    if(mpHelpDlg)
    {
        delete mpHelpDlg;
        mpHelpDlg = NULL;
    }
#endif //HELPBTN
    
    if(mpSettingsDlg)
    {
        delete mpSettingsDlg;
        mpSettingsDlg = NULL;
    }
    
}

void CMainMenuWorld::ResetDisplayTopGold()
{
    if(mpScenario == NULL) return;
    if(mpTobGoldBar == NULL) return;
    int nGold = (float)mpScenario->GetUserInfo()->GetGold();
    CNumberCtl* pNum1 = (CNumberCtl*)mpTobGoldBar->FindControl(CTL_TOP_GOLD);
    if(pNum1) pNum1->SetNumber(nGold);
    CNumberCtl* pNum2 = (CNumberCtl*)mpTobGoldBar->FindControl(CTL_TOP_GOLD2);
    if(pNum2) pNum2->SetNumber(nGold);
}

int CMainMenuWorld::VisibleDisplayItemSlotPage(int nID)
{
    vector<string>  lstImage;
    const int nMaxItem= MAXSLOTCNT;
    CUserInfo* pUserInfo = mpScenario->GetUserInfo();
    bool bItem = !mpItemSlotSwitchBtn->GetPushed();
    vector<USERINFO*>* lstItem = NULL;
    if(bItem)
        lstItem = pUserInfo->GetListItem();
    else
        lstItem = pUserInfo->GetListBomb();
    
    int nIndex = 0;
    for(int i = 0; i < lstItem->size(); i++)
    {
        if(lstItem->at(i)->nID == nID)
        {
            nIndex = i;
            break;
        }
    }
    return (nIndex / nMaxItem) + 1;
}

void CMainMenuWorld::AnimationDisplayItemSlot(int nID)
{
    for(int i = CTL_TABLE_ITEM_SLOT3; i <= CTL_TABLE_ITEM_SLOT6; i++)
    {
        CControl* pCtl = mpItemSlotBar->FindControl(i);
        if(pCtl->GetLData() == nID)
        {
            pCtl->SetAni(CONTROLANI_WIGWAG,0.5f);
            break;
        }
    }
}

void CMainMenuWorld::ResetDisplayItemSlot()
{
    vector<string>  lstImage;
    int nMaxPageCnt = 0;
    const int nMaxItem = MAXSLOTCNT;
    vector<CControl*> lstSlotCtl;
    CUserInfo* pUserInfo = mpScenario->GetUserInfo();
    vector<USERINFO*>* lstItem = NULL;
    
    
    bool bItem = !mpItemSlotSwitchBtn->GetPushed();
    
    if(bItem)
        lstItem = pUserInfo->GetListItem();
    else
        lstItem = pUserInfo->GetListBomb();
    
    CNumberCtl* pItemPageNumber = (CNumberCtl*)mpItemSlotBar->FindControl(CTL_TABLE_ITEM_SLOT_PAGE);
    
    int nSelIndex = 1;
    if(bItem)
        nSelIndex = pUserInfo->GetLastSelectedItemPageIndex();
    else
        nSelIndex = pUserInfo->GetLastSelectedBombPageIndex();
        
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
    
    for (int i = 0; i < nMaxItem; i++)
    {
        nIndex = (nSelIndex - 1) * nMaxItem + i;
        if(nIndex >= nMaxCnt || nMaxCnt == 0)
        {
            lstImage.clear();
            lstImage.push_back("none");
            lstImage.push_back("none");
            lstSlotCtl[i]->GetChild(0)->SetImageData(lstImage);
            
            lstSlotCtl[i]->GetChild(0)->GetChild(0)->SetHide(true);
            lstSlotCtl[i]->SetLData(-1);
        }
        else
        {
            if(bItem)
            {
                int nID = lstItem->at(nIndex)->nID;
                PROPERTY_ITEM property;
                if(PROPERTY_ITEM::GetPropertyItem(nID, property) == NULL)
                    continue;
                
                lstImage.clear();
                sPath = property.sModelPath;
                sPath.append(".tga");
                lstImage.push_back(sPath);
                lstImage.push_back("none");
                lstSlotCtl[i]->GetChild(0)->SetImageData(lstImage);
                lstSlotCtl[i]->GetChild(0)->SetBackCoordTex(0.0f, 0.0f, 0.96875f, 0.96875f);
                lstSlotCtl[i]->SetLData(nID);
                
                if(property.nType == 0 || property.nType == 6) //Count
                {
                    CNumberCtl* pNumCtl = (CNumberCtl*)lstSlotCtl[i]->GetChild(0)->GetChild(0);
                    pNumCtl->SetNumber(pUserInfo->GetCntByIDWithItem(nID));
                    lstSlotCtl[i]->GetChild(0)->GetChild(0)->SetHide(false);
                }
                else if(property.nType == 2) //Upgrade
                {
                    PROPERTY_TANK property;
                    PROPERTY_TANK::GetPropertyTank(pUserInfo->GetLastSelectedTankID(),property);
                    if(nID == ID_ITEM_UPGRADE_ATTACK)
                    {
                        CNumberCtl* pNumCtl = (CNumberCtl*)lstSlotCtl[i]->GetChild(0)->GetChild(0);
                        int nUpgrade = pUserInfo->GetAttackUpgrade() * 5.f; //100으로 처리하자.
                        pNumCtl->SetNumber(nUpgrade,CNumberCtlType_Percent);
                        lstSlotCtl[i]->GetChild(0)->GetChild(0)->SetHide(false);

                    }
                    else if(nID == ID_ITEM_UPGRADE_DEPEND)
                    {
                        CNumberCtl* pNumCtl = (CNumberCtl*)lstSlotCtl[i]->GetChild(0)->GetChild(0);
                        int nUpgrade = pUserInfo->GetDependUpgrade() * 5.f; //100으로 처리하자.
                        pNumCtl->SetNumber(nUpgrade,CNumberCtlType_Percent);
                        lstSlotCtl[i]->GetChild(0)->GetChild(0)->SetHide(false);

                    }
                    else if(nID == ID_ITEM_UPGRADE_DEFENCE_TOWER || nID == ID_ITEM_UPGRADE_MISSILE_TOWER)
                    {
                        CNumberCtl* pNumCtl = (CNumberCtl*)lstSlotCtl[i]->GetChild(0)->GetChild(0);
                        int nUpgrade = pUserInfo->GetCntByIDWithItem(nID) * 5; //100으로 처리하자.
                        pNumCtl->SetNumber(nUpgrade,CNumberCtlType_Percent);
                        lstSlotCtl[i]->GetChild(0)->GetChild(0)->SetHide(false);
                    }
                    else
                    {
                        lstSlotCtl[i]->GetChild(0)->GetChild(0)->SetHide(true);
                    }
                }
                else
                {
    //                pNumCtl->ClearNumber();
                    lstSlotCtl[i]->GetChild(0)->GetChild(0)->SetHide(true);
                }
            }
            else //Bomb
            {
                int nID = lstItem->at(nIndex)->nID;
                PROPERTY_BOMB property;
                PROPERTY_BOMB::GetPropertyBomb(nID, property);
                
                lstImage.clear();
                lstImage.push_back(property.sBombTableImgPath);
                lstImage.push_back("none");
                lstSlotCtl[i]->GetChild(0)->SetImageData(lstImage);
                //폭탄이미지는 0.0,0.0,1.0,1.0 이 아니다.
                //0.2265625 => 0.1265625 (비율때문)
                //0.7734375 => 0.8734375 (비율때문)
                lstSlotCtl[i]->GetChild(0)->SetBackCoordTex(0.20703125, 0.1265625, 0.79296875, 0.8734375);
                lstSlotCtl[i]->SetLData(nID);
                
                if(property.nType == 0) //Count
                {
                    CNumberCtl* pNumCtl = (CNumberCtl*)lstSlotCtl[i]->GetChild(0)->GetChild(0);
                    pNumCtl->SetNumber(pUserInfo->GetCntByIDWithBomb(nID));
                    lstSlotCtl[i]->GetChild(0)->GetChild(0)->SetHide(false);
                }
                else
                {
                    lstSlotCtl[i]->GetChild(0)->GetChild(0)->SetHide(true);
                }
            }
        }
    }
    if(bItem)
        pUserInfo->SetLastSelectedItemPageIndex(nSelIndex);
    else
        pUserInfo->SetLastSelectedBombPageIndex(nSelIndex);
}

int CMainMenuWorld::InitMenu2(int nGroupDifY,int nPrevDifY)
{
     return E_SUCCESS;
}



void CMainMenuWorld::SetAniStep(MENUANI v)
{
    mAniStep = v;
    switch (mAniStep)
    {
        case MENUANI_WORLD_TO_MAP:
        case MENUANI_WORLD_TO_TANK:
        case MENUANI_WORLD_TO_FACTORY:
//        case MENUANI_WORLD_TO_SHOP:
        {
            mpTobGoldBar->SetAni(CONTROLANI_TIN,0.1);
            mpMenuBar->SetAni(CONTROLANI_LIN,0.3);
            mpRankPannel->SetAni(CONTROLANI_BIN,0.3);
            mpStartPannel->SetAni(CONTROLANI_BIN,0.3);
            
            //아이폰일 경우 각각의 화면별로 따로 보여준다.
            if(gnDisplayType == DISPLAY_IPAD)
            {
                pmRightTopBar->SetAni(CONTROLANI_TIN,0.1);
                mpMapDescBar->SetAni(CONTROLANI_RIN,0.3);
                mpTankSelectedDescBar->SetAni(CONTROLANI_RIN,0.3);
                mpItemSlotBar->SetAni(CONTROLANI_BIN,0.1);
            }
         
            if(mAniStep == MENUANI_WORLD_TO_MAP)
            {
                mpRollCtl->SetAni(CONTROLANI_ZOOM_IN,0.05);
                mpRollCtl->SetType(CRollMapTankCtl::CRollMapTankCtl_Map);
                mpRollCtl->SetHide(false);
                
                if(gnDisplayType == DISPLAY_IPHONE)
                {
                    pmRightTopBar->SetAni(CONTROLANI_TIN,0.1);
                    mpMapDescBar->SetAni(CONTROLANI_RIN,0.3);
                    mpTankSelectedDescBar->SetAni(CONTROLANI_RIN,0.3);
                }
            }
            else if(mAniStep == MENUANI_WORLD_TO_TANK)
            {
                mpRollCtl->SetAni(CONTROLANI_ZOOM_IN,0.05);
                mpRollCtl->SetType(CRollMapTankCtl::CRollMapTankCtl_Tank);
                mpRollCtl->SetHide(false);
                mpTankDescBar->SetAni(CONTROLANI_TIN,2.5);
                
                if(gnDisplayType == DISPLAY_IPHONE)
                {
                    pmRightTopBar->SetAni(CONTROLANI_TIN,0.1);
                    mpMapDescBar->SetAni(CONTROLANI_RIN,0.3);
                    mpTankSelectedDescBar->SetAni(CONTROLANI_RIN,0.3);
                }
            }
            else if(mAniStep == MENUANI_WORLD_TO_FACTORY)
            {
                mpFrameBar->SetAni(CONTROLANI_BIN,1.7);
                
                if(gnDisplayType == DISPLAY_IPHONE)
                {
                    mpItemSlotBar->SetAni(CONTROLANI_RIN,0.5);
                }
            }
        }
            break;
        case MENUANI_MAP_TO_WORLD:
        case MENUANI_TANK_TO_WORLD:
        case MENUANI_FACTORY_TO_WORLD:
        {
            mpTobGoldBar->SetAni(CONTROLANI_TOUT,0.1);
            mpMenuBar->SetAni(CONTROLANI_LOUT,0.3);
            mpRankPannel->SetAni(CONTROLANI_BOUT,0.3);
            mpStartPannel->SetAni(CONTROLANI_BOUT,0.3);
            
            //아이폰일 경우 각각의 화면별로 따로 보여준다.
            if(gnDisplayType == DISPLAY_IPAD)
            {
                pmRightTopBar->SetAni(CONTROLANI_TOUT,0.1);
                mpMapDescBar->SetAni(CONTROLANI_ROUT,0.3);
                mpTankSelectedDescBar->SetAni(CONTROLANI_ROUT,0.3);
                mpItemSlotBar->SetAni(CONTROLANI_BOUT,0.1);
            }
            
            
            if(mAniStep == MENUANI_MAP_TO_WORLD)
            {
                mpRollCtl->SetAni(CONTROLANI_ZOOM_OUT,0.1);
                mpRollCtl->SetType(CRollMapTankCtl::CRollMapTankCtl_Map);
                
                if(gnDisplayType == DISPLAY_IPHONE)
                {
                    pmRightTopBar->SetAni(CONTROLANI_TOUT,0.1);
                    mpMapDescBar->SetAni(CONTROLANI_ROUT,0.3);
                    mpTankSelectedDescBar->SetAni(CONTROLANI_ROUT,0.3);
                }
            }
            else if(mAniStep == MENUANI_TANK_TO_WORLD)
            {
                mpRollCtl->SetAni(CONTROLANI_ZOOM_OUT,0.1);
                mpRollCtl->SetType(CRollMapTankCtl::CRollMapTankCtl_Tank);
                mpTankDescBar->SetAni(CONTROLANI_TOUT,2.5);
                
                if(gnDisplayType == DISPLAY_IPHONE)
                {
                    if(pmRightTopBar->GetHide() == false)
                    {
                        pmRightTopBar->SetAni(CONTROLANI_TOUT,0.1);
                        mpMapDescBar->SetAni(CONTROLANI_ROUT,0.3);
                        mpTankSelectedDescBar->SetAni(CONTROLANI_ROUT,0.3);
                    }
                    else
                    {
                        mpItemSlotBar->SetAni(CONTROLANI_ROUT,0.3f);
                    }
                }
            }
            else if(mAniStep == MENUANI_FACTORY_TO_WORLD)
            {
                mpFrameBar->SetAni(CONTROLANI_BOUT,1.7);
                
                if(gnDisplayType == DISPLAY_IPHONE)
                {
                    mpItemSlotBar->SetAni(CONTROLANI_ROUT,0.5);
                }
            }
        }
            break;
            
        
        case MENUANI_MENU_TO_FACTORY:
        {
            if(mpRollCtl->GetHide() == false) //롤이 보여지고 있다면 숨긴다.
                mpRollCtl->SetAni(CONTROLANI_ZOOM_OUT,0.1);
            if(mpFrameBar->GetHide() == true) //테이블이 숨겨져 있다면 보여지게 한다.
                mpFrameBar->SetAni(CONTROLANI_BIN,1.7);
            if(mpTankDescBar->GetHide() == false)
                mpTankDescBar->SetAni(CONTROLANI_TOUT,2.5);
            
            //아이폰일경우.
            if(gnDisplayType == DISPLAY_IPHONE)
            {
                if(mpItemSlotBar->GetHide() == true)
                    mpItemSlotBar->SetAni(CONTROLANI_RIN,0.5);
                if(mpTankSelectedDescBar->GetHide() == false)
                    mpTankSelectedDescBar->SetAni(CONTROLANI_ROUT,0.3); //숨긴다.
                if(mpMapDescBar->GetHide() == false)
                    mpMapDescBar->SetAni(CONTROLANI_ROUT,0.3); //숨긴다.
                if(pmRightTopBar->GetHide() == false) //숨긴다.
                    pmRightTopBar->SetAni(CONTROLANI_TOUT,0.1);
            }
        }
            break;
        case MENUANI_MENU_TO_MAP:
        {
            if(mpRollCtl->GetHide() == true)
                mpRollCtl->SetAni(CONTROLANI_ZOOM_IN,0.05);
            if(mpFrameBar->GetHide() == false)
            {
                mpFrameBar->SetAni(CONTROLANI_BOUT,1.7);
            }
            if(mpTankDescBar->GetHide() == false)
                mpTankDescBar->SetAni(CONTROLANI_TOUT,2.5);
            
            mpRollCtl->SetType(CRollMapTankCtl::CRollMapTankCtl_Map);
            mpRollCtl->SetHide(false);
            
            
            //아이폰일경우.
            if(gnDisplayType == DISPLAY_IPHONE)
            {
                if(mpItemSlotBar->GetHide() == false)
                    mpItemSlotBar->SetAni(CONTROLANI_ROUT,0.5); //숨긴다.
                if(mpTankSelectedDescBar->GetHide() == true)
                    mpTankSelectedDescBar->SetAni(CONTROLANI_RIN,0.3); //보이게한다.
                if(mpMapDescBar->GetHide() == true)
                    mpMapDescBar->SetAni(CONTROLANI_RIN,0.3); //보이게한다.
                if(pmRightTopBar->GetHide() == true) //보이게한다.
                    pmRightTopBar->SetAni(CONTROLANI_TIN,0.1);

            }
        }
            break;
        case MENUANI_MENU_TO_TANK:
        {
            if(mpRollCtl->GetHide() == true)
                mpRollCtl->SetAni(CONTROLANI_ZOOM_IN,0.05);
            if(mpFrameBar->GetHide() == false)
            {
                mpFrameBar->SetAni(CONTROLANI_BOUT,1.7);
            }
            mpRollCtl->SetType(CRollMapTankCtl::CRollMapTankCtl_Tank);
            mpRollCtl->SetHide(false);
            
            
            int nPos = mpRollCtl->GetCurrentPos(CRollMapTankCtl::CRollMapTankCtl_Tank);
            CUserInfo* pUserInfo = mpScenario->GetUserInfo();
            if(nPos != pUserInfo->GetLastSelectedTankIndex()) //선택된 텐크가 아니면 상태를 보여준다.
            {
                mpTankDescBar->SetAni(CONTROLANI_TIN,2.5);
            }
            
            //아이폰일경우.
            if(gnDisplayType == DISPLAY_IPHONE)
            {
//                if(mpItemSlotBar->GetHide() == false)
//                    mpItemSlotBar->SetAni(CONTROLANI_ROUT,0.5); //숨긴다.
//                if(mpTankSelectedDescBar->GetHide() == true)
//                    mpTankSelectedDescBar->SetAni(CONTROLANI_RIN,0.3); //보이게한다.
//                if(mpMapDescBar->GetHide() == true)
//                    mpMapDescBar->SetAni(CONTROLANI_RIN,0.3); //보이게한다.
//                if(pmRightTopBar->GetHide() == true) //보이게한다.
//                    pmRightTopBar->SetAni(CONTROLANI_TIN,0.1);
                
                int nPos = mpRollCtl->GetCurrentPos(CRollMapTankCtl::CRollMapTankCtl_Tank);
                if(nPos != -1)
                {
                    RollItem* pItem = mpRollCtl->GetItem(nPos, CRollMapTankCtl::CRollMapTankCtl_Tank);
                    SetPhoneStyleTankDescAni(pItem->nID);
                }
                else
                {
                    if(mpItemSlotBar->GetHide() == false)
                        mpItemSlotBar->SetAni(CONTROLANI_ROUT,0.5); //숨긴다.
                    if(mpTankSelectedDescBar->GetHide() == true)
                        mpTankSelectedDescBar->SetAni(CONTROLANI_RIN,0.3); //보이게한다.
                    if(mpMapDescBar->GetHide() == true)
                        mpMapDescBar->SetAni(CONTROLANI_RIN,0.3); //보이게한다.
                    if(pmRightTopBar->GetHide() == true) //보이게한다.
                        pmRightTopBar->SetAni(CONTROLANI_TIN,0.1);

                }
            }
        }
            break;
        default:
            break;
    }
}

int CMainMenuWorld::initProjection()
{
    
    //이부분은 그리고자하는 부분에서 glDiable되기때문에 문제가 있다.
    //	/////////////////////////////////////////////////////////////////////
	const GLfloat			lightAmbient[] = {1.0, 1.0, 1.0, 1.0};
	const GLfloat			lightDiffuse[] = {1.0, 1.0, 1.0, 1.0};
	const GLfloat			lightPosition[] = {1.0, 0.0, 1.0, 0.0}; 
    
	//Configure OpenGL lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	
    
#ifndef ANDROID //IOS에서는 한번만 셋팅해주면 정의가 된다.
    //방향벡터의 normalize를 하지 않음. 그래서 gl에서 해주어야 샤이닝이 이러난다.
    //모델의 빛 반사를 정의 하려면 GL_NORMALIZE화 해주어야 한다.
    glEnable(GL_NORMALIZE);
#endif
    
    //	/////////////////////////////////////////////////////////////////////
	
	// set projection
	glMatrixMode (GL_PROJECTION);
    
    
	glLoadIdentity ();
    
	SGLCAMERA *pCamera = GetCamera();
	sglPerspectivef(
                    pCamera->aperture, 
                    (GLfloat)pCamera->viewWidth / (GLfloat)pCamera->viewHeight, 
                    pCamera->ViewNear, FARVIEW);
    
    //픽킹정보는 반드시 glMatrixMode (GL_PROJECTION); 이후에 와야 한다.
//    CPicking::Initialize();
   	return E_SUCCESS;
}

void CMainMenuWorld::ResetCamera()
{
    SGLCAMERA Camera;
    
    Camera.aperture = GetViewAperture();
	
	Camera.rotPoint.x = 0;
	Camera.rotPoint.y = 0;
	Camera.rotPoint.z = 0;
	
	Camera.viewPos.x = 0.0;
	Camera.viewPos.y = 0.0;
	Camera.viewPos.z = CAMERA_YPOS_2D; 
	
	
	Camera.viewDir.x = -Camera.viewPos.x; 
	Camera.viewDir.y = -Camera.viewPos.y; 
	Camera.viewDir.z = -Camera.viewPos.z;
    
	Camera.viewUp.x = 0;  
	Camera.viewUp.y = 1; 
	Camera.viewUp.z = 0;
	
	Camera.ViewNear = 1.0f;	// Proejction에서 동적으로 구해진다.
	Camera.ViewFar = 32.0f; // 멀리볼수있는 한계 (초기화된후에 정해진다)
	Camera.ViewFarOrg = Camera.ViewFar; //처음 initialize에서 설정된 값.

    
    SetCamera(&Camera);
}

void CMainMenuWorld::Clear()
{
    m_pTextureMan->Clear();
}


bool CMainMenuWorld::CheckNonAnimation()
{
    if(mpTobGoldBar->Animating() == CONTROLANI_NONE &&
       mpMenuBar->Animating() == CONTROLANI_NONE &&
//       mpBombSlotBar->Animating() == CONTROLANI_NONE &&
       pmRightTopBar->Animating() == CONTROLANI_NONE &&
       mpMapDescBar->Animating() == CONTROLANI_NONE &&
       mpTankSelectedDescBar->Animating() == CONTROLANI_NONE &&
       mpRankPannel->Animating() == CONTROLANI_NONE &&
       mpStartPannel->Animating() == CONTROLANI_NONE &&
       mpItemSlotBar->Animating() == CONTROLANI_NONE &&
       mpFrameBar->Animating() == CONTROLANI_NONE &&
       mpRollCtl->Animating() == CONTROLANI_NONE &&
       (mpBuyDlg == NULL || (mpBuyDlg && mpBuyDlg->Animating() == CONTROLANI_NONE)) &&
#ifdef HELPBTN
       (mpHelpDlg == NULL || (mpHelpDlg && mpHelpDlg->Animating() == CONTROLANI_NONE)) &&
#endif //HELPBTN
       (mpSettingsDlg == NULL || (mpSettingsDlg && mpSettingsDlg->Animating() == CONTROLANI_NONE))) return true;
    return false;
}

bool CMainMenuWorld::OnPurchasedBomb(int nID)
{
    PROPERTY_BOMB Prop;
    PROPERTY_BOMB::GetPropertyBomb(nID, Prop);
    CUserInfo* pUserInfo = mpScenario->GetUserInfo();
    
    int nGold = pUserInfo->GetGold();
    int nPrice = Prop.nPrice;
    
    if(!mpItemSlotSwitchBtn->GetPushed())
        mpItemSlotSwitchBtn->SetPushed(true);
    
    

    if(nGold >= nPrice)
    {
        nGold -= nPrice;
        pUserInfo->SetGold(nGold);
        
        //화면에 정의 해준다.
        ResetDisplayTopGold();
        
        //무기 슬롯테이블에 넣어준다.
        pUserInfo->AddListToBomb(nID,Prop.nPackCount); //이미존재하면
        int nPage = VisibleDisplayItemSlotPage(Prop.nID);
        pUserInfo->SetLastSelectedBombPageIndex(nPage);
        ResetDisplayItemSlot();
        pUserInfo->Save();
        
        
        
        AnimationDisplayItemSlot(nID);
        
        if(mpSoundMan)
            mpSoundMan->PlaySystemSound(SOUND_ID_EnergyUP);
    }
    else
    {
        //돈이 부족하다.
        if(mpSoundMan)
            mpSoundMan->PlaySystemSound(SOUND_ID_Buzzer);

        mpBuyDlg->SetAni(CONTROLANI_WIGWAG,0.3);
        return false;
    }
    return true;
}

void CMainMenuWorld::ResultReviews()
{
    CUserInfo* pUserInfo = GetUserInfo();

    if(pUserInfo->GetReport() == -1)
    {
        int nRaida = pUserInfo->GetCntByIDWithItem(ID_ITEM_ATTACKBYRADA);
        if(nRaida == 0)
        {
            //Radar
            //무기 슬롯테이블에 넣어준다.
            pUserInfo->AddListToItem(ID_ITEM_ATTACKBYRADA);
            mpItemSlotSwitchBtn->SetPushed(false); //폭탄으로 설정되어 있으면 폭탄이 보여지기 때문에 아이템으로 변경해서 보여주어야 한다.
            OnClickedItemSlot(ID_ITEM_ATTACKBYRADA);
            RemveItemTableRow(ID_ITEM_ATTACKBYRADA);
            //스롯을 다시그려준다.
            ResetDisplayItemSlot();
        }
        else
        {
            //Gold
            pUserInfo->SetGold(pUserInfo->GetGold() + 2000);
            //화면에 정의 해준다.
            ResetDisplayTopGold();
            
        }
        pUserInfo->SetReport(-2);
        if(mpSoundMan)
            mpSoundMan->PlaySystemSound(SOUND_ID_EnergyUP);
        pUserInfo->Save(); //저장해준다.
    }
}

void CMainMenuWorld::RemveItemTableRow(int nID)
{
    int nIndex = -1;
    int nCnt = mpFrameItembTable->GetRowCnt();
    for(int i = 0; i < nCnt; i++)
    {
        if(mpFrameItembTable->GetRowCtl(i)->GetLData() == nID)
        {
            nIndex = i;
            break;
        }
    }
    if(nIndex != -1)
    {
        mpFrameItembTable->RemoveItem(nIndex);
    }
    //            mpItemTableCtl->RemoveItem(mpBuyPopupCtl->GetRow());
    //            mpBuyPopupCtl->SetAni(CONTROLANI_ZOOM_IN_HIDE,0.5); //다이얼로그 박스를 살아지게 한다.

}

bool CMainMenuWorld::OnPurchasedProductItem(int nID)
{
    PROPERTY_ITEM Prop;
    PROPERTY_ITEM::GetPropertyItem(nID, Prop);
    
    //Android 에서 살때.
    if(mpScenario == NULL) return false;
    if(mpItemSlotSwitchBtn == NULL) return false;

    CUserInfo* pUserInfo = mpScenario->GetUserInfo();
    if(mpItemSlotSwitchBtn->GetPushed())
        mpItemSlotSwitchBtn->SetPushed(false);
    
    pUserInfo->AddListToItem(nID,Prop.nType); //이미존재하면
    if(Prop.nType == 5) //영구이면 테이블의 로를 지운다.
        RemveItemTableRow(nID);
    
    int nPage = VisibleDisplayItemSlotPage(Prop.nID);
    pUserInfo->SetLastSelectedItemPageIndex(nPage);
    
    //스롯을 다시그려준다.
    ResetDisplayItemSlot();
    
    AnimationDisplayItemSlot(nID);
    
    //광고를 지워지게 하자.
    if(nID == ID_ITEM_REMOVE_AD)
        sglShowAD(false);
    else if(nID == ID_ITEM_UPGRADE_PACKAGE)
    {
        pUserInfo->AddAttackUpgrade();
        pUserInfo->AddAttackUpgrade();
        pUserInfo->AddAttackUpgrade();
        
        pUserInfo->AddDependUpgrade();
        pUserInfo->AddDependUpgrade();
        pUserInfo->AddDependUpgrade();
        
        //탱크설명을 다시그려준다.
        OnSelectedTank(pUserInfo->GetLastSelectedTankID(),false);
        int nPos =mpRollCtl->GetCurrentPos(CRollMapTankCtl::CRollMapTankCtl_Tank);
        if(nPos != -1)
        {
            RollItem* pRollItem = mpRollCtl->GetItem(nPos, CRollMapTankCtl::CRollMapTankCtl_Tank);
            
            //현재 컨트롤이 선택된것이 설정된 탱크이면 화면에 보여준다. 아니면 무시한다.
            if(pRollItem->nID == pUserInfo->GetLastSelectedTankID())
                OnSelectedTank(pUserInfo->GetLastSelectedTankID(),true);
        }
    }
    else if(nID == ID_TIEM_FASTGOLD)
        pUserInfo->AddListToItem(nID,1);
    else if(nID == ID_TIEM_FASTBUILD)
        pUserInfo->AddListToItem(nID,1);
    
    pUserInfo->Save();

    
//    if(mpSoundMan)
//        mpSoundMan->PlaySystemSound(SOUND_ID_EnergyUP);
    return true;

}

bool CMainMenuWorld::OnPurchasedItem(int nID)
{
    int grade = 1;
    PROPERTY_ITEM Prop;
    PROPERTY_ITEM::GetPropertyItem(nID, Prop);
    CUserInfo* pUserInfo = mpScenario->GetUserInfo();
    
    int nGold = pUserInfo->GetGold();
    int nPrice = Prop.nPrice;
    
    if(mpItemSlotSwitchBtn->GetPushed())
        mpItemSlotSwitchBtn->SetPushed(false);
        
    
    //Added By Song 2013.02.14 업그레이드 돈을 수정하였다.
    if(Prop.nType == 2) //Upgrade
    {
        if(Prop.nID == ID_ITEM_UPGRADE_ATTACK) //AttackUpgrade
           grade = pUserInfo->GetAttackUpgrade() + 1;
        else if(Prop.nID == ID_ITEM_UPGRADE_DEPEND) //AttackUpgrade
            grade = pUserInfo->GetDependUpgrade() + 1;
        else if(Prop.nID == ID_ITEM_UPGRADE_DEFENCE_TOWER || Prop.nID == ID_ITEM_UPGRADE_MISSILE_TOWER)
            grade = pUserInfo->GetCntByIDWithItem(Prop.nID) + 1;
        nPrice = Prop.nPrice * grade;
    }
    
    if(nGold >= nPrice)
    {
        nGold -= nPrice;
        pUserInfo->SetGold(nGold);
        
        //화면에 정의 해준다.
        ResetDisplayTopGold();
        
        //무기 슬롯테이블에 넣어준다.
        pUserInfo->AddListToItem(nID,Prop.nType); //이미존재하면
        if(Prop.nType == 1) //영구이면 테이블의 로를 지운다.
        {
            RemveItemTableRow(nID);
        }
        else if(Prop.nType == 2) //Upgrade 이면
        {
            if(Prop.nID == ID_ITEM_UPGRADE_ATTACK) //AttackUpgrade
                pUserInfo->AddAttackUpgrade();
            else if(Prop.nID == ID_ITEM_UPGRADE_DEPEND)
                pUserInfo->AddDependUpgrade();
//            else if(Prop.nID == ID_ITEM_UPGRADE_DEFENCE_TOWER || Prop.nID == ID_ITEM_UPGRADE_MISSILE_TOWER)
//            {
//                int nCnt = pUserInfo->GetCntByIDWithItem(Prop.nID); //Max 2단계 업그레이드 이면 리스트에서 지운다.
//                if(nCnt >= 2)
//                    RemveItemTableRow(nID);
//            }
        }
        
        int nPage = VisibleDisplayItemSlotPage(Prop.nID);
        pUserInfo->SetLastSelectedItemPageIndex(nPage);
        
        //스롯을 다시그려준다.
        ResetDisplayItemSlot();
        
        //아이템 테이블의 현재 리소스에 바로적용해준다.
        if(mpFrameItembTable && Prop.nType == 2) //Upgrade
        {
            char sTempDesc2[128];
            const char *sFormat;
            int nRowCnt = mpFrameItembTable->GetRowCnt();
            for (int i = 0; i < nRowCnt; i++)
            {
                int nID = (int)mpFrameItembTable->GetRowCtl(i)->GetLData();
                if(nID == Prop.nID)
                {
                    
                    CControl* pDesc = mpFrameItembTable->GetRowCtl(i)->FindControl(9999);
                    sFormat = SGLTextUnit::GetText(Prop.sDesc2);
                    grade = grade + 1;
//                    if(nID == ID_ITEM_UPGRADE_DEFENCE_TOWER || nID == ID_ITEM_UPGRADE_MISSILE_TOWER)
//                        sprintf(sTempDesc2, sFormat,grade * 50);
//                    else
                        sprintf(sTempDesc2, sFormat,grade * 5);
                    
                    vector<string> lstImage;
                    lstImage.push_back("none");
                    lstImage.push_back(sTempDesc2);
                    pDesc->SetImageData(lstImage);
                    
                    CNumberCtl* pGold = (CNumberCtl*)mpFrameItembTable->GetRowCtl(i)->FindControl(9998);
                    pGold->SetNumber(Prop.nPrice * grade);
                    break;
                }
            }
        }
        
        //탱크설명을 다시그려준다.
        OnSelectedTank(pUserInfo->GetLastSelectedTankID(),false);
        int nPos =mpRollCtl->GetCurrentPos(CRollMapTankCtl::CRollMapTankCtl_Tank);
        if(nPos != -1)
        {
            RollItem* pRollItem = mpRollCtl->GetItem(nPos, CRollMapTankCtl::CRollMapTankCtl_Tank);
            
            //현재 컨트롤이 선택된것이 설정된 탱크이면 화면에 보여준다. 아니면 무시한다.
            if(pRollItem->nID == pUserInfo->GetLastSelectedTankID())
                OnSelectedTank(pUserInfo->GetLastSelectedTankID(),true);
        }
        pUserInfo->Save();
        
        AnimationDisplayItemSlot(nID);
        
        if(mpSoundMan)
            mpSoundMan->PlaySystemSound(SOUND_ID_EnergyUP);
    }
    else
    {
        //돈이 부족하다.
        if(mpSoundMan)
            mpSoundMan->PlaySystemSound(SOUND_ID_Buzzer);
        
        mpBuyDlg->SetAni(CONTROLANI_WIGWAG,0.4);
        return false;
    }
    return true;
    
}

void CMainMenuWorld::ShowShotMessageBox(const char* sIConPath,float fsu,float fsv,float feu,float fev,const char* sTitle,const char* sMessage1, const char* sMessage2)
{
    char sIn[128],sOut[128];
    vector<float> layout;
    vector<string>  lstImage;
    
    
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SLD_Back", layout);
    
    float fOffsetX = (gDisplayWidth - layout[2]) / 2.f + layout[0];
    float fOffsetY = gDisplayHeight - layout[3] + layout[1];
    //CellBack
    lstImage.clear();
    lstImage.push_back("res_slot.png");
    lstImage.push_back("none");
    
    if(mpShotMessage) delete mpShotMessage;
    mpShotMessage =  new CLabelCtl(NULL,m_pTextureMan);
    mpShotMessage->Initialize(0, fOffsetX, fOffsetY, layout[2], layout[3], lstImage, 0.3046875, 0.716796875, 0.52734375, 0.8125);
    
    //기본적인 거시기는 완료하였다.
    float fWhite[] = {1.0f,1.0f,1.0f,1.0f};
    string sTemp;
        
    
    //CellTitleBack
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    CControl* pCellTitleBack =  new CLabelCtl(mpShotMessage,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("ICe_CellTitleBack", layout);
    pCellTitleBack->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3], lstImage, 0.466796875, 0.2978515625, 0.69921875, 0.3203125);
    mpShotMessage->AddControl(pCellTitleBack);
    
    if(strcmp(sTitle, "none") != 0)
    {
        
        const char* sTemp = GetText(sTitle);
        strcpy(sIn, sTemp);
        char* sFnd = strstr(sIn, "#");
        if(sFnd)
        {
            *sFnd = 0;
            const char* sFormat = GetText("Shot_Msg_Title");
            sprintf(sOut, sFormat,sIn);
            
            //타이틀
            lstImage.clear();
            lstImage.push_back("none");
            lstImage.push_back(sOut);
        }
        else
        {
            //타이틀
            lstImage.clear();
            lstImage.push_back("none");
            lstImage.push_back("none");
        }

    }
    else
    {
        //타이틀
        lstImage.clear();
        lstImage.push_back("none");
        lstImage.push_back("none");
    }
    
    CControl* pItemType = new CLabelCtl(pCellTitleBack,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("ICe_ItemType", layout);
    pItemType->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
    pItemType->SetTextColor(fWhite);
    pCellTitleBack->AddControl(pItemType);
    
    //이미지....
    lstImage.clear();
    lstImage.push_back(IMG_TABLE_BANDI);
    lstImage.push_back("none");
    CControl* pItemIconBack = new CLabelCtl(mpShotMessage,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("ICe_ItemIconBack", layout);
    pItemIconBack->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3], lstImage, 0.40234375, 0.685546875, 0.4697265625, 0.7529296875);
    mpShotMessage->AddControl(pItemIconBack);
    
    //아이콘....
    lstImage.clear();
    lstImage.push_back(sIConPath);
    lstImage.push_back("none");
    CControl* pItemIcon = new CLabelCtl(pItemIconBack,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("ICe_ItemIcon", layout);
    pItemIcon->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3], lstImage, fsu, fsv, feu, fev);
    pItemIconBack->AddControl(pItemIcon);
    
    //아이템Desc백.
    lstImage.clear();
    lstImage.push_back(IMG_TABLE_BANDI);
    lstImage.push_back("none");
    CControl* pItemDescBack = new CLabelCtl(mpShotMessage,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("ICe_ItemDescBack", layout);
    pItemDescBack->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3], lstImage, 0.3994140625, 0.6103515625, 0.55859375, 0.6787109375);
    mpShotMessage->AddControl(pItemDescBack);
    
    if(sMessage1 && sMessage2 &&
       strlen(sMessage1) > 0 && strlen(sMessage2) > 0 &&
       strcmp(sMessage1, "none") != 0 && strcmp(sMessage2, "none") != 0
       )
    {
        

        const char* sTemp = GetText(sMessage1);
        strcpy(sIn, sTemp);
        char* sFnd = strstr(sIn, "#");
        if(sFnd)
        {
            *sFnd = 0;
            const char* sFormat = GetText("Shot_Msg_Desc");
            sprintf(sOut, sFormat,sIn);
            
            //Desc2_1 (설명이 두줄 첫번째)
            lstImage.clear();
            lstImage.push_back("none");
            lstImage.push_back(sOut);
        }
        else
        {
            //Desc2_1 (설명이 두줄 첫번째)
            lstImage.clear();
            lstImage.push_back("none");
            lstImage.push_back("none");
        }
        
        
//        lstImage.clear();
//        lstImage.push_back("none");
//        lstImage.push_back(sMessage1);
        CControl* pDesc21 = new CLabelCtl(pItemDescBack,m_pTextureMan);
        layout.clear();
        PROPERTYI::GetPropertyFloatList("ICe_Desc21", layout);
        pDesc21->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
        pDesc21->SetTextColor(fWhite);
        pItemDescBack->AddControl(pDesc21);
        
        

        sTemp = GetText(sMessage2);
        strcpy(sIn, sTemp);
        sFnd = strstr(sIn, "#");
        if(sFnd)
        {
            *sFnd = 0;
            const char* sFormat = GetText("Shot_Msg_Desc");
            sprintf(sOut, sFormat,sIn);
            
            //Desc2_1 (설명이 두줄 첫번째)
            lstImage.clear();
            lstImage.push_back("none");
            lstImage.push_back(sOut);
        }
        else
        {
            //Desc2_1 (설명이 두줄 첫번째)
            lstImage.clear();
            lstImage.push_back("none");
            lstImage.push_back("none");
        }
        
        //Desc2_2 (설명이 두줄 두번째)
//        lstImage.clear();
//        lstImage.push_back("none");
//        lstImage.push_back(sMessage2);
        CControl* pDesc22 = new CLabelCtl(pItemDescBack,m_pTextureMan);
        layout.clear();
        PROPERTYI::GetPropertyFloatList("ICe_Desc22", layout);
        pDesc22->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
        pDesc22->SetTextColor(fWhite);
        pItemDescBack->AddControl(pDesc22);
    }
    else
    {
        const char* sTemp = GetText(sMessage1);
        strcpy(sIn, sTemp);
        char* sFnd = strstr(sIn, "#");
        if(sFnd)
        {
            *sFnd = 0;
            const char* sFormat = GetText("Shot_Msg_Desc");
            sprintf(sOut, sFormat,sIn);
            
            //Desc2_1 (설명이 두줄 첫번째)
            lstImage.clear();
            lstImage.push_back("none");
            lstImage.push_back(sOut);
        }
        else
        {
            //Desc2_1 (설명이 두줄 첫번째)
            lstImage.clear();
            lstImage.push_back("none");
            lstImage.push_back("none");
        }
        
        CControl* pDesc11 = new CLabelCtl(pItemDescBack,m_pTextureMan);
        layout.clear();
        PROPERTYI::GetPropertyFloatList("ICe_Desc11", layout);
        pDesc11->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
        pDesc11->SetTextColor(fWhite);
        pItemDescBack->AddControl(pDesc11);
        
    }
    
    mfShotMessage = 1.0f;
    mnShotMessage = GetGGTime() + 1500;
}


//일반적인 메세지... (훈련병은 훈련소를 졸업해야합니다.)
void CMainMenuWorld::ShowShotMessageBox(const char* sTitle,const char* sMessage1, const char* sMessage2)
{
    
//    string sT = GetText("Tran_Shot_Msg_T");
//    string sD = GetText("Tran_Shot_Msg_Des1");
//    string sD2= GetText("Tran_Shot_Msg_Des2");

    
//    char sIn[128],sOut[128];
    vector<float> layout;
    vector<string>  lstImage;
    
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SLD_Back", layout);
    
    float fOffsetX = (gDisplayWidth - layout[2]) / 2.f + layout[0];
    float fOffsetY = gDisplayHeight - layout[3] + layout[1];
    //CellBack
    lstImage.clear();
    lstImage.push_back("res_slot.png");
    lstImage.push_back("none");
    
    if(mpShotMessage) delete mpShotMessage;
    mpShotMessage =  new CLabelCtl(NULL,m_pTextureMan);
    mpShotMessage->Initialize(0, fOffsetX, fOffsetY, layout[2], layout[3], lstImage, 0.3046875, 0.716796875, 0.52734375, 0.8125);
    
    //기본적인 거시기는 완료하였다.
    float fWhite[] = {1.0f,1.0f,1.0f,1.0f};
    float fYellow[] = {1.0f,1.0f,0.0f,1.0f};
    string sTemp;
    
    
    //CellTitleBack
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    CControl* pCellTitleBack =  new CLabelCtl(mpShotMessage,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("ICe_CellTitleBack", layout);
    pCellTitleBack->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3], lstImage, 0.466796875, 0.2978515625, 0.69921875, 0.3203125);
    mpShotMessage->AddControl(pCellTitleBack);
    
    if(sTitle)
    {
        lstImage.clear();
        lstImage.push_back("none");
        lstImage.push_back(SGLTextUnit::GetTextAddGLData(sTitle,"Shot_Msg_Title"));
    }
    
    CControl* pTitle = new CLabelCtl(pCellTitleBack,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("ICe_ItemType", layout);
    pTitle->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
    pTitle->SetTextColor(fYellow);
    pCellTitleBack->AddControl(pTitle);
    
    //아이템Desc백.
    lstImage.clear();
    lstImage.push_back(IMG_TABLE_BANDI);
    lstImage.push_back("none");
    CControl* pItemDescBack = new CLabelCtl(mpShotMessage,m_pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("ICe_ItemDescBackX", layout);
    pItemDescBack->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3], lstImage, 0.3994140625, 0.6103515625, 0.55859375, 0.6787109375);
    mpShotMessage->AddControl(pItemDescBack);
    
    //두줄의 메세지가 존재한다면.
    if(sMessage1 && sMessage2)
    {
        
        lstImage.clear();
        lstImage.push_back("none");
        lstImage.push_back(SGLTextUnit::GetTextAddGLData(sMessage1,"Shot_Msg_Desc"));
    
        CControl* pDesc21 = new CLabelCtl(pItemDescBack,m_pTextureMan);
        layout.clear();
        PROPERTYI::GetPropertyFloatList("ICe_Desc21X", layout);
        pDesc21->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
        pDesc21->SetTextColor(fWhite);
        pItemDescBack->AddControl(pDesc21);
        
        
        lstImage.clear();
        lstImage.push_back("none");
        lstImage.push_back(SGLTextUnit::GetTextAddGLData(sMessage2,"Shot_Msg_Desc"));
        
        CControl* pDesc22 = new CLabelCtl(pItemDescBack,m_pTextureMan);
        layout.clear();
        PROPERTYI::GetPropertyFloatList("ICe_Desc22X", layout);
        pDesc22->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
        pDesc22->SetTextColor(fWhite);
        pItemDescBack->AddControl(pDesc22);
    }
    else
    {
        lstImage.clear();
        lstImage.push_back("none");
        lstImage.push_back(SGLTextUnit::GetTextAddGLData(sMessage1,"Shot_Msg_Desc"));
        
        CControl* pDesc11 = new CLabelCtl(pItemDescBack,m_pTextureMan);
        layout.clear();
        PROPERTYI::GetPropertyFloatList("ICe_Desc11X", layout);
        pDesc11->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
        pDesc11->SetTextColor(fWhite);
        pItemDescBack->AddControl(pDesc11);
        
    }
    
    mfShotMessage = 1.0f;
    mnShotMessage = GetGGTime() + 1500;
}

void CMainMenuWorld::OnClickedItemSlot(int nID)
{
    if(nID == -1) return;
    vector<float> layout;
    vector<string>  lstImage;
    
    
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SLD_Back", layout);
    
    float fX = (gDisplayWidth - layout[2]) / 2.f + layout[0];
    float fY = gDisplayHeight - layout[3] + layout[1];
    
    
    //    CControl* pItemPageBack =  new CLabelCtl(mpItemSlotBar,m_pTextureMan);
    //    layout.clear();
    //    PROPERTYI::GetPropertyFloatList("SL_ItemPageBack", layout);
    //    pItemPageBack->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage,
    //                              0.3046875, 0.716796875, 0.52734375, 0.8125);
    //    mpItemSlotBar->AddControl(pItemPageBack);
    
    if(mpItemSlotDesc == NULL)
    {
        //CellBack
        lstImage.clear();
        lstImage.push_back("res_slot.png");
        lstImage.push_back("none");
        
        mpItemSlotDesc =  new CLabelCtl(NULL,m_pTextureMan);
        mpItemSlotDesc->Initialize(0, fX, fY, layout[2], layout[3], lstImage, 0.3046875, 0.716796875, 0.52734375, 0.8125);
        mControlList.push_back(mpItemSlotDesc);
    }
    
    
    mpItemSlotDesc->SetHide(false);
    
    
    //---------------------------------------------
    //투명처리된 컨트롤들을 다시 복원해준다.
    mfItemSlotAliveAlpha = 1.0f;
    mpItemSlotDesc->SetTranslate(mfItemSlotAliveAlpha);
    
    mnItemSlotAliveTime = GetGGTime() + 1500;
    
    //-----------------------------------------------------
    //밑의 아이템을 전부 지운다. 비효율적이지만 확실하다.
    mpItemSlotDesc->RemoveAll();
    //-----------------------------------------------------
    bool bItem = !mpItemSlotSwitchBtn->GetPushed();
    if(bItem)
    {
        PROPERTY_ITEM prop;
        PROPERTY_ITEM::GetPropertyItem(nID, prop);
        //2 : desc
        InitItemCell(&prop,mpItemSlotDesc,fX,fY,2);
    }
    else
    {
        PROPERTY_BOMB prop;
        PROPERTY_BOMB::GetPropertyBomb(nID, prop);
        //2 : desc
        InitBombCell(&prop,mpItemSlotDesc,fX,fY,2);
    }
    
}

void CMainMenuWorld::OnSelectedMap()
{
    vector<string>  lstImage;
    CUserInfo* pUserInfo = mpScenario->GetUserInfo();
    PROPERTY_MAP property;
    
    
    if(!PROPERTY_MAP::GetPropertyMap(pUserInfo->GetLastSelectedMapID(),property))
    {
        HLogE("Can not find map id %d\n",pUserInfo->GetLastSelectedMapID());
        return ;
    }
    
    string sModelImgPath = property.sModelPath;
    sModelImgPath.append(".png");
    
    CControl* pIconCtl = mpMapDescBar->FindControl(CTL_MAP_DESC_ICON);
    if(pIconCtl)
        pIconCtl->SetContentsData(sModelImgPath.c_str());
    
    
    CControl* pName = mpMapDescBar->FindControl(CTL_MAP_DESC_NAME);
    if(pName)
    {
        lstImage.clear();
        lstImage.push_back("none");
        lstImage.push_back(property.sDesc);
        pName->SetImageData(lstImage);
    }
    
    //스타를 모두 숨긴다.
    for (int i = CTL_MAP_DESC_STAR1; i <= CTL_MAP_DESC_STAR7; i++)
    {
        CControl* pStarCtl = mpMapDescBar->FindControl(i);
        pStarCtl->SetHide(true);
    }

    
    //총 8단계의 맵레벨이 있는데 그중에 4단계 어려움 정도로 나타나게 하기 위해 2로 나누었다.
    int nStarMax = CTL_MAP_DESC_STAR1 + property.nMinLevel - 1;
    if(nStarMax > CTL_MAP_DESC_STAR7)
    {
//        HLogE("Invalide Map Level %d\n",property.nMinLevel);
        nStarMax = CTL_MAP_DESC_STAR7;
    }
    
    int nCntRank = pUserInfo->GetCompletedRank();
    //설정만큼만 보이게 한다.
    for (int i = CTL_MAP_DESC_STAR1; i <= nStarMax; i++)
    {
        CControl* pStarCtl = mpMapDescBar->FindControl(i);
        pStarCtl->SetHide(false);
        
        if(nCntRank == 1)
        {
            float f[] = {0.0f,1.f,1.f,1.0f};
            pStarCtl->SetBackColor(f);
        }
        else if(nCntRank == 2)
        {
            float f[] = {1.0f,0.f,1.f,1.0f};
            pStarCtl->SetBackColor(f);
        }
        else  if(nCntRank > 2)
        {
            float f[] = {1.0f,0.f,0.f,1.0f};
            pStarCtl->SetBackColor(f);
        }
        
    }
    HLog("Level %d\n",property.nMinLevel);
}


void CMainMenuWorld::OnSelectedTank(int nTankID,bool bDesPopup)
{
    vector<string>  lstImage;
    CControl* pBar = NULL;
    if(bDesPopup)
        pBar = mpTankDescBar;
    else
        pBar = mpTankSelectedDescBar;

    CUserInfo* pUserInfo = mpScenario->GetUserInfo();
    PROPERTY_TANK property;
    
    if(bDesPopup)
    {
        //팝업은 메뉴일때만 보였다 말았다 한다.
        CButtonCtl* pMenuBtn = (CButtonCtl*) mpMenuBar->FindControl(BTN_MENU_TANK);
        if(pMenuBtn->GetPushed())
        {
            if(nTankID == pUserInfo->GetLastSelectedTankID())
            {
                pBar->SetHide(true);
            }
            else
            {
                pBar->SetHide(false);
                pBar->SetTranslate(0.8f);
            }
        }
        else
            pBar->SetHide(true);
    }
    
    if(!PROPERTY_TANK::GetPropertyTank(nTankID,property))
    {
        HLogE("Can not find tank id %d\n",nTankID);
        return ;
    }
    
    string sModelImgPath = property.sModelPath;
    sModelImgPath.append("Icon.png");
    
    CControl* pIconCtl = pBar->FindControl(CTL_TANK_DESC_ICON);
    if(pIconCtl)
    {
        pIconCtl->SetContentsData(sModelImgPath.c_str());
    }
    
    
    CControl* pControl = pBar->FindControl(BTN_SELECT_TANK);
    //-------------------------------------------------------------
    if(pControl)
    {
        //구매.......
        if(property.nGold == 0)
        {
            //선택버튼.
            lstImage.clear();
            lstImage.push_back(IMG_RES_TANK);
            lstImage.push_back("M_Tank_Desc_Select");
            pControl->SetImageData(lstImage);
            
        }
        else
        {
            //구매버튼.
            lstImage.clear();
            lstImage.push_back(IMG_RES_TANK);
            lstImage.push_back("M_Tank_Desc_Buy");
            pControl->SetImageData(lstImage);
        }
    }
    CControl* pGoldIcon = pBar->FindControl(CTL_TANK_DESC_GOLDICON);
    if(pGoldIcon)
    {
        CNumberCtl* pGoldNum = (CNumberCtl*)pBar->FindControl(CTL_TANK_DESC_GOLD);
        if(property.nGold == 0)
        {
            pGoldIcon->SetHide(true);
            pGoldNum->SetHide(true);
        }
        else
        {
            pGoldIcon->SetHide(false);
            pGoldNum->SetHide(false);
            pGoldNum->SetNumber(property.nGold);
        }
    }
    //-------------------------------------------------------------
    

    //업그레이드 처리
    float fAttackLevel = DUnitLevelToTotalLevel(property.nMaxAttackPower,pUserInfo->GetRank(),pUserInfo->GetAttackUpgrade() * 2.f);
    CImgProgressCtl* pAttackCtl = (CImgProgressCtl*)pBar->FindControl(CTL_TANK_DESC_ATTACK);
    pAttackCtl->SetPosition(fAttackLevel);
    
    //Defend 상태바 (맥스는 300)
    float fDefenceLevel = DUnitLevelToTotalLevel(property.nMaxDefendPower,pUserInfo->GetRank(),pUserInfo->GetDependUpgrade() * 2.f);
    CImgProgressCtl* pDefendCtl = (CImgProgressCtl*)pBar->FindControl(CTL_TANK_DESC_DEFEND);
    pDefendCtl->SetPosition(fDefenceLevel);
    
    //속도 (0.015~0.03)
    int nSpeedG = (property.fMoveRate * 100.f) * 100.f / 3.f;
    CImgProgressCtl* pSpeedCtl = (CImgProgressCtl*)pBar->FindControl(CTL_TANK_DESC_SPEED);
    pSpeedCtl->SetPosition(nSpeedG);    
    
    
    int nLevel = (fAttackLevel + fDefenceLevel) / 2.f + 0.5f;
    CNumberCtl* pLevelCtl = (CNumberCtl*)pBar->FindControl(CTL_TANK_DESC_LEVEL);
    pLevelCtl->SetNumber(nLevel);
    
    CControl* pTankName = pBar->FindControl(CTL_TANK_DESC_TANKNAME);
    if(bDesPopup)
    {
        lstImage.clear();
        lstImage.push_back("none");
        lstImage.push_back(property.sName);
        pTankName->SetImageData(lstImage);
        
        CControl* pRank = pBar->FindControl(CTL_TANK_DESC_RANK);
        lstImage.clear();
        lstImage.push_back(::GetRankPath(property.nMinLevel));
        lstImage.push_back("none");
        pRank->SetImageData(lstImage);
        
        float fWhite[] = {1.0f,1.0f,1.0f,1.0f};
        
        int nAUpgrade = pUserInfo->GetAttackUpgrade() * 5; //100으로 처리하자.
        int nDUpgrade = pUserInfo->GetDependUpgrade() * 5; //100으로 처리하자.
        
        if(property.nGold != 0) //돈주고 사야하는 것은 업그레이드 아이콘을 숨겨서 골드숫자와 겹치지 않게 하자.
        {
            nAUpgrade = 0;
            nDUpgrade = 0;
        }
        
        if(nAUpgrade > 0 && nDUpgrade > 0)
        {
            lstImage.clear();
            lstImage.push_back("ItemDefence.tga");
            lstImage.push_back("none");
            CControl* pDef = pBar->FindControl(CTL_TANK_DESC_DEFENSE_ICON);
            pDef->SetHide(false);
            pDef->SetImageData(lstImage);
            CNumberCtl* pNumCtl = (CNumberCtl*)pDef->GetChild(0);
            pNumCtl->SetTextColor(fWhite);
            pNumCtl->SetNumber(nAUpgrade,CNumberCtlType_Percent);
            

            
            
            lstImage.clear();
            lstImage.push_back("ItemAttack.tga");
            lstImage.push_back("none");
            CControl* pAtt = pBar->FindControl(CTL_TANK_DESC_ATTACK_ICON);
            pAtt->SetHide(false);
            pAtt->SetImageData(lstImage);
            pNumCtl = (CNumberCtl*)pAtt->GetChild(0);
            pNumCtl->SetTextColor(fWhite);
            pNumCtl->SetNumber(nDUpgrade,CNumberCtlType_Percent);
        }
        else if(nAUpgrade > 0 && nDUpgrade == 0)
        {
            lstImage.clear();
            lstImage.push_back("ItemDefence.tga");
            lstImage.push_back("none");
            CControl* pDef = pBar->FindControl(CTL_TANK_DESC_ATTACK_ICON);
            pDef->SetHide(false);
            pDef->SetImageData(lstImage);
            CNumberCtl* pNumCtl = (CNumberCtl*)pDef->GetChild(0);
            pNumCtl->SetTextColor(fWhite);
            pNumCtl->SetNumber(nAUpgrade,CNumberCtlType_Percent);
            
            CControl* pAtt = pBar->FindControl(CTL_TANK_DESC_DEFENSE_ICON);
            pAtt->SetHide(true);
            
        }
        else if(nAUpgrade == 0 && nDUpgrade > 0)
        {
            lstImage.clear();
            lstImage.push_back("ItemAttack.tga");
            lstImage.push_back("none");
            CControl* pDef = pBar->FindControl(CTL_TANK_DESC_ATTACK_ICON);
            pDef->SetHide(false);
            pDef->SetImageData(lstImage);
            CNumberCtl* pNumCtl = (CNumberCtl*)pDef->GetChild(0);
            pNumCtl->SetTextColor(fWhite);
            pNumCtl->SetNumber(nDUpgrade,CNumberCtlType_Percent);
            
            CControl* pAtt = pBar->FindControl(CTL_TANK_DESC_DEFENSE_ICON);
            pAtt->SetHide(true);
            
        }
        else
        {
            CControl* pDef = pBar->FindControl(CTL_TANK_DESC_ATTACK_ICON);
            pDef->SetHide(true);
            CControl* pAtt = pBar->FindControl(CTL_TANK_DESC_DEFENSE_ICON);
            pAtt->SetHide(true);
        }
    }
    else
    {
        lstImage.clear();
        lstImage.push_back("none");
        lstImage.push_back(property.sDesc);
        pTankName->SetImageData(lstImage);
        
        ResetDisplayItemSlot();
    }
    
}


int CMainMenuWorld::RenderBegin()
{
    if( (mnStatus & 0x0002) != 0x0002)
		return E_SUCCESS;
    
    int nTime = GetClockDelta();
    
    
    if(mpWaitFindMultiplayCursor)
    {
        CMutiplyProtocol* pPro = GetMutiplyProtocol();
        if(pPro->GetMultiplayState() == SYNC_NONE || pPro->GetMultiplayState() == SYNC_MATCHINGERROR)
        {
            mpWaitFindMultiplayCursor->Stop();
            mnMulDlgMsgTime = 0;
        }
        else
        {
            
            if(GetGGTime() > mnMulDlgMsgTime)
            {
                char sMessage[256],sMessage2[256];
                mnMulDlgMsgTime = GetGGTime() + 4000;
                int nSec = (int)((pPro->GetFindWaitTime() - GetGGTime()) / 1000.f);
                
                if( nSec < 1000)
                {
                    strcpy(sMessage, SGLTextUnit::GetOnlyText("MulPlay_Start"));
                    sprintf(sMessage2,SGLTextUnit::GetOnlyText("MulPlay_Start2"),nSec);
                    ShowShotMessageBox("",sMessage,sMessage2);
                }
            }
            mpWaitFindMultiplayCursor->Start();
            
            // New Version
            if(mpNewViersion)
                mpNewViersion->SetEnable(false); //클릭을 못하게 막는다.
        }
        
    }
    
    // New Version
    if(gsNewAppVersion && mpNewViersion && mpNewViersion->GetHide() == true)
        mpNewViersion->SetHide(false);
    
    if(CScenario::gbNeedCompletedMap)
    {
        CompletedMapDlg();
        CScenario::gbNeedCompletedMap = false;
        mShowTimeCompletedDlg = GetGGTime() + 1000 * 10; //5초동안 보여준다.
    }
    
    
    //스텐실 버퍼도 클리어해준다.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    
    
    //-----------------------------------------------------------------------------------------------------------------------
    CUserInfo* pUserInfo = GetUserInfo();
    int nGS = pUserInfo->GetGameCenterAuthStatus();
    if(nGS == 0) //로그인이고 로그인 중에 있다면 싱글플레이로 해준다.
    {
        vector<string>  lstImage;
        lstImage.push_back(IMG_MAINMENU_BACK);
        lstImage.push_back("M_Menu_Multi4");
        mpMultiplayButton->SetImageData(lstImage);
        
        CControl* pIcon = mpMultiplayButton->FindControl(CTL_MENU_WIFIICON);
        pIcon->SetHide(true);
    }
    else if(nGS == 1)
    {
        vector<string>  lstImage;
        lstImage.push_back(IMG_MAINMENU_BACK);
        lstImage.push_back("M_Menu_Multi2");
        mpMultiplayButton->SetImageData(lstImage);
        CControl* pIcon = mpMultiplayButton->FindControl(CTL_MENU_WIFIICON);
        pIcon->SetHide(true);
    }
    else if(nGS == 2) //로그인 됨.
    {
        //메뉴
        //싱글플레이 멀티 플레이.
        if(pUserInfo->GetMultiplayOn())
        {
            vector<string>  lstImage;
            lstImage.push_back(IMG_MAINMENU_BACK);
            lstImage.push_back("M_Menu_Multi3");
            mpMultiplayButton->SetImageData(lstImage);
            CControl* pIcon = mpMultiplayButton->FindControl(CTL_MENU_WIFIICON);
            pIcon->SetHide(false);
        }
        else
        {
            vector<string>  lstImage;
            lstImage.push_back(IMG_MAINMENU_BACK);
            lstImage.push_back("M_Menu_Multi4");
            mpMultiplayButton->SetImageData(lstImage);

            CControl* pIcon = mpMultiplayButton->FindControl(CTL_MENU_WIFIICON);
            pIcon->SetHide(true);
        }
    }
    //-----------------------------------------------------------------------------------------------------------------------
    
    
    
    //Animation 중이면 애니매니션으 끝날을때 다음 액션을 해준다.
    if(mAniStep != MENUANI_NONE)
    {
        bool bStopedAnimation = CheckNonAnimation();
        if(bStopedAnimation)
        {
            
            //플레이를 한다.
            if(mAniStep == MENUANI_MAP_TO_WORLD || mAniStep == MENUANI_TANK_TO_WORLD || mAniStep == MENUANI_FACTORY_TO_WORLD)
            {
                //현재 User정보를 저장한다.
                
                if(pUserInfo)
                {
                    PROPERTY_TANK prop;
                    PROPERTY_TANK::GetPropertyTank(pUserInfo->GetLastSelectedTankID(), prop);
                    pUserInfo->SetDefaultBomb(prop.nDefaultBombID);
                    pUserInfo->Save();
                }
                CScenario::SendMessage(SGL_MESSAGE_CLICKED,BTN_MENU_PLAY,0,0,0);
            }
            else if(MENUANI_WORLD_TO_MAP ||
                    MENUANI_WORLD_TO_TANK ||
                    MENUANI_WORLD_TO_FACTORY )
            {
                int nRank = mpScenario->IsPromotionRank();
                if(nRank != 0)
                {
                    //----------------------------------------------------
                    //진급 메세지 박스를 보여준다.
                    //----------------------------------------------------
                    CScenario::SendMessage(SGL_MESSAGE_PROMOTION_RANK,nRank,0,0,0);
                }
                else
                {
                    //훈련병에서 이등병으로 진급해준다.
                    CUserInfo* pUserInfo = mpScenario->GetUserInfo();
                    if(pUserInfo->IsPromotionSloder())
                    {
                        pUserInfo->SetPromotionSloder(false);
                        CScenario::SendMessage(SGL_MESSAGE_PROMOTION_RANK,1,0,0,0);
                    }
                }
            }
            mAniStep = MENUANI_NONE;
        }
    }
    
    if(mpItemSlotDesc && mnItemSlotAliveTime != 0)
    {
        if(GetGGTime() > mnItemSlotAliveTime)
        {
            if(mfItemSlotAliveAlpha < 0.1)
            {
                mfItemSlotAliveAlpha = 1.0f;
                mnItemSlotAliveTime = 0;
                mpItemSlotDesc->SetHide(true);
            }
            else
            {
                mfItemSlotAliveAlpha -= (float)nTime/1000.0f;
                mpItemSlotDesc->SetTranslate(mfItemSlotAliveAlpha);
            }
            
        }
    }
    
    

    //진급 프로 모션
    if(mpPromotionRank && mnPromotionRankTime != 0)
    {

        if(GetGGTime() > mnPromotionRankTime)
        {

            if(mfPromotionRankAlpha < 0.1)
            {
                mfPromotionRankAlpha = 1.0f;
                mnPromotionRankTime = 0;
                mpPromotionRank->SetHide(true);
            }
            else
            {
                mfPromotionRankAlpha -= (float)nTime / 1000.0f;
                mpPromotionRank->SetTranslate(mfPromotionRankAlpha);
            }
            
        }
    }
    
    if(mpBuyDlg) mpBuyDlg->RenderBegin(nTime);
#ifdef HELPBTN
    if(mpHelpDlg) mpHelpDlg->RenderBegin(nTime);
#endif //HELPBTN
    if(mpSettingsDlg) mpSettingsDlg->RenderBegin(nTime);
    if(mpCompletedDlg)
    {
        if(GetGGTime() > mShowTimeCompletedDlg)
        {
            delete mpCompletedDlg;
            mpCompletedDlg = NULL;
        }
        else
            mpCompletedDlg->RenderBegin(nTime);
    }
    
    //---------------------------------------------
    //shot 메세지를 보여준다.
    if(mpShotMessage && mnShotMessage != 0)
    {
        if(GetGGTime() > mnShotMessage)
        {
            if(mfShotMessage < 0.1)
            {
                mfShotMessage = 1.0f;
                mnShotMessage = 0;
                delete mpShotMessage;
                mpShotMessage = NULL;
            }
            else
            {
                mfShotMessage -= (float)nTime/1000.0f;
                mpShotMessage->SetTranslate(mfShotMessage);
            }
        }
        if(mpShotMessage) mpShotMessage->RenderBegin(nTime);
    }
    //---------------------------------------------

    
    int nCnt = (int)mControlList.size();
    for (int i = 0; i < nCnt; i++)
    {
        mControlList[i]->RenderBegin(nTime);
    }

    return E_SUCCESS;
}

void CMainMenuWorld::StartPlayNoMatching()
{
    CUserInfo* pUserInfo = GetUserInfo();
    
    //잠김맵을 실행하면 잠기지 않는 맵을 찾아서 실행해야 한다.
    if(!pUserInfo->IsCompletedMap(pUserInfo->GetLastSelectedMapID()))
    {
        int nMapID = 0;
        pUserInfo->GetNotCompletedMap(nMapID);
        if(nMapID != -1)
        {
            pUserInfo->SetLastSelectedMapID(nMapID);
            int nIndex = pUserInfo->GetMIDToIndex(nMapID);
            pUserInfo->SetLastSelectedMapIndex(nIndex);
        }
        
        int nTank = mpRollCtl->GetCurrentPos(CRollMapTankCtl::CRollMapTankCtl_Tank);
        mpRollCtl->SetCurrentPos(mpRollCtl->GetNowMapIndex(), nTank);
    }
    
    //-------------------------------------------------------
    //플레이한다.
    CScenario::SendMessage(SGL_MESSAGE_CLICKED,BTN_NEXT,0xFF);
    //-------------------------------------------------------
    if(gPlayOut) gPlayOut(); //프로그램 준비시간에 랙이 걸려 IPhone3는 매칭유지가 잘 안된다. 그래서 시작 시점에 다시 하자.
    HLogN("Start Play on No Matching");

}
void CMainMenuWorld::SetPhoneStyleTankDescAni(int nTankID)
{
    //---------------------------------------------------------------------------------------
    //탱크 선택이 어시스트인지 아닌지에 따라서 보여지는 것을 조절해준다.
    if(gnDisplayType == DISPLAY_IPHONE)
    {
        PROPERTY_TANK property;
        if(!PROPERTY_TANK::GetPropertyTank(nTankID,property))
        {
            HLogE("PROPERTY_TANK::GetPropertyTank Can not find tank id %d\n",nTankID);
            return;
        }
        
        if(property.nGold == 0) //선택형 탱크
        {
            if(mpTankSelectedDescBar->GetHide())
            {
                mpTankSelectedDescBar->SetAni(CONTROLANI_RIN,0.8);
                mpMapDescBar->SetAni(CONTROLANI_RIN,0.8);
                pmRightTopBar->SetAni(CONTROLANI_RIN,0.8);
                mpItemSlotBar->SetAni(CONTROLANI_ROUT,0.8);
                
            }
            
        }
        else
        {
            if(mpItemSlotBar->GetHide())
            {
                mpTankSelectedDescBar->SetAni(CONTROLANI_ROUT,0.8);
                mpMapDescBar->SetAni(CONTROLANI_ROUT,0.8);
                pmRightTopBar->SetAni(CONTROLANI_ROUT,0.8);
                mpItemSlotBar->SetAni(CONTROLANI_RIN,0.8);
                
            }
        }
    }
}
//---------------------------------------------------------------------------------------
bool CMainMenuWorld::CheckShowReview(int nCurrentMapIndex)
{
    CUserInfo* pUserInfo = GetUserInfo();
    int nRep = pUserInfo->GetReport();
    int nPlayTime = pUserInfo->GetTotalPalyTime();
    vector<int>* mapcnt = pUserInfo->GetListCompletedMap();
    
    //60 * 10 = 600 10분정도 사용하 사람은 권유해보자.
    //맵을 한번깻거나 10분사용했다면
    if(pUserInfo->GetCompletedRank() == 1 && nCurrentMapIndex != 0 && nRep == 0 && (nPlayTime > 600 || mapcnt->size() >= 2))
    {
        if(gShowReviewDlg) gShowReviewDlg((void*)pUserInfo);
        return true;
    }
    else if(nCurrentMapIndex != 0 && nRep > 0 && pUserInfo->GetRank() >= nRep && pUserInfo->GetGold() < 1500.f)
    {
        if(gShowReviewDlg) gShowReviewDlg((void*)pUserInfo);
        return true;
    }
    else if(nRep == -1) //평점을 한후에 앱이 죽었으면 스타트할때 결과를 적용해주자.
    {
        ResultReviews();
    }
    else if(nRep > 0)
    {
        int nRadar = pUserInfo->GetCntByIDWithItem(ID_ITEM_ATTACKBYRADA);
        if(nRadar == 0) //존재 하지 않으면
        {
            pUserInfo->AddListToItem(ID_ITEM_ATTACKBYRADA);
            pUserInfo->Save(); //저장해준다.
            if(mpSoundMan)
                mpSoundMan->PlaySystemSound(SOUND_ID_EnergyUP);
        }
    }
    return false;
}
int CMainMenuWorld::Render()
{
    
	if( (mnStatus & 0x0002) != 0x0002)
		return E_SUCCESS;

    
    mpRollCtl->Render();
    
    glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
    
#ifdef ANDROID //Android에서는 매번 셋팅해주어야 한다.
    //방향벡터의 normalize를 하지 않음. 그래서 gl에서 해주어야 샤이닝이 이러난다.
    //모델의 빛 반사를 정의 하려면 GL_NORMALIZE화 해주어야 한다.
    glEnable(GL_NORMALIZE);
#endif

    
    ActivateCamera();
    
    glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnable(GL_TEXTURE_2D);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    mpTopBack->Render();
    mpBottomBack->Render();
    mpTopLeftBack->Render();
    
    //투명도가 조절이 안되면 빛을 Disable하고 glDepthFunc을 GL_LEQUAL로 하여 투명하게 처를 한다.
    glDepthFunc(GL_LEQUAL);
    
    glDisable(GL_DEPTH_TEST);
    mpBottomItemBack->Render(); //Reflaction 위에 그려야 한다.
    mpBottomLeftBack->Render();
    mpBottomRightBack->Render();
    

    
    //Roll컨트롤은 상단에서 이미 돌았다.
    int nSize = (int)mControlList.size();
    for (int i = 0; i < nSize; i++)
    {
        CControl* pCtl =mControlList[i];
        if(pCtl != mpRollCtl)
            mControlList[i]->Render();
    }
    
//    nSize = mAniMationControl.size();
//    for (int i = 0; i < nSize; i++)
//    {
//        mAniMationControl[i]->Render();
//    }

    if(mpBuyDlg) mpBuyDlg->Render();
#ifdef HELPBTN
    if(mpHelpDlg) mpHelpDlg->Render();
#endif //HELPBTN
    if(mpSettingsDlg) mpSettingsDlg->Render();
    if(mpShotMessage) mpShotMessage->Render();
    if(mpCompletedDlg) mpCompletedDlg->Render();
   
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);    
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);   
    glDisable(GL_TEXTURE_2D);
    
    return E_SUCCESS;
}

int CMainMenuWorld::RenderEnd()
{ 
    //초기화가 안되었거나 초기화중이다.
	if( (mnStatus & 0x0002) != 0x0002) 
		return E_SUCCESS;
    
   
    int nCnt = (int)mControlList.size();
    for (int i = 0; i < nCnt; i++)
    {
        mControlList[i]->RenderEnd();
    }
    
//    nCnt = mAniMationControl.size();
//    for (int i = 0; i < nCnt; i++)
//    {
//        mAniMationControl[i]->RenderEnd();
//    }
    
    if(mpBuyDlg) mpBuyDlg->RenderEnd();
#ifdef HELPBTN
    if(mpHelpDlg) mpHelpDlg->RenderEnd();
#endif //HELPBTN
    if(mpSettingsDlg) mpSettingsDlg->RenderEnd();
    if(mpShotMessage) mpShotMessage->RenderEnd();
    if(mpCompletedDlg) mpCompletedDlg->RenderEnd();
    
    return E_SUCCESS;
}


void CMainMenuWorld::SetActor(CSprite* pSprite) {};	
CSprite* CMainMenuWorld::GetActor() {return NULL;} 

int CMainMenuWorld::OnEvent(SGLEvent *pEvent)
{
    switch (pEvent->nMsgID)
    {
        case SGL_MESSAGE_CLICKED:
        {
            
            switch (pEvent->lParam) {
                case BTN_MENU_MAP:
                {
                    CButtonCtl* pCtl = (CButtonCtl*)pEvent->lObject;
                    pCtl->ArrangeToggleButton();
                    SetAniStep(MENUANI_MENU_TO_MAP);
                    if(mpSoundMan) mpSoundMan->PlaySystemSound(SOUND_ID_Click);
                }
                    break;
                case BTN_MENU_TANK:
                {
                    CButtonCtl* pCtl = (CButtonCtl*)pEvent->lObject;
                    pCtl->ArrangeToggleButton();
                    SetAniStep(MENUANI_MENU_TO_TANK);
                    if(mpSoundMan) mpSoundMan->PlaySystemSound(SOUND_ID_Click);
                }
                    break;
                case BTN_MENU_FACTORY:
                {
                    CButtonCtl* pCtl = (CButtonCtl*)pEvent->lObject;
                    pCtl->ArrangeToggleButton();
                    SetAniStep(MENUANI_MENU_TO_FACTORY);
                    if(mpSoundMan) mpSoundMan->PlaySystemSound(SOUND_ID_Click);
                }
                    break;

                case BTN_MENU_BESTSCORE:
                {
                    
                    if(gShowBestScoreDlg) gShowBestScoreDlg((void*)GetUserInfo());
                    
                    if(mpSoundMan) mpSoundMan->PlaySystemSound(SOUND_ID_Click);
                }
                    break;
                case BTN_MENU_MULTIPLAY:
                {
                    CUserInfo* pUserInfo =  GetUserInfo();
                    int nGS = pUserInfo->GetGameCenterAuthStatus();
                    if(nGS == 2) //로그인 되어 있다면
                    {
                        if(pUserInfo->GetMultiplayOn()) //멀티플레이에서 => 싱글플레이로
                        {
                            vector<string>  lstImage;
                            lstImage.push_back(IMG_MAINMENU_BACK);
                            lstImage.push_back("M_Menu_Multi4");
                            mpMultiplayButton->SetImageData(lstImage);
                            CControl* pIcon = mpMultiplayButton->FindControl(CTL_MENU_WIFIICON);
                            pIcon->SetHide(true);
                            pUserInfo->SetMultiplayOn(false);
                            pUserInfo->Save();
                        }
                        else //싱글플레이로 => 멀티플레이에서
                        {
                            vector<string>  lstImage;
                            lstImage.push_back(IMG_MAINMENU_BACK);
                            lstImage.push_back("M_Menu_Multi3");
                            mpMultiplayButton->SetImageData(lstImage);
                            CControl* pIcon = mpMultiplayButton->FindControl(CTL_MENU_WIFIICON);
                            pIcon->SetHide(false);
                            pUserInfo->SetMultiplayOn(true);
                            pUserInfo->Save();
                        }
                        
                    }
                    else if(nGS == 1) //로그인 중입니다. 잠시만 기다려주세요.
                    {
                        vector<string>  lstImage;
                        lstImage.push_back(IMG_MAINMENU_BACK);
                        lstImage.push_back("M_Menu_Multi2");
                        mpMultiplayButton->SetImageData(lstImage);
                        CControl* pIcon = mpMultiplayButton->FindControl(CTL_MENU_WIFIICON);
                        pIcon->SetHide(true);
                        
                        //언어 해주어야 한다. NoGold_Shot_Msg_Des1
                        ShowShotMessageBox("Game Center","Waiting ...","...");
                    }
                    else
                    {
                        //언어 해주어야 한다. NoGold_Shot_Msg_Des1
                        ShowShotMessageBox("Multi_MLoginT","Multi_MLoginDes1","Multi_MLoginDes2");
                    }
                    
                    if(mpSoundMan) mpSoundMan->PlaySystemSound(SOUND_ID_Click);
                }
                    break;


                case BTN_NEXT:
                {
                    

///////////////////////////////////////////////////////////////////////
// 평점을 권유 해본다.
///////////////////////////////////////////////////////////////////////
#ifndef MAC
                    CUserInfo* pUserInfo = GetUserInfo();
                    //-----------------------------------------------------------------------------
                    //CMissionEndCtl에서 이미처리를 했지만 혹시 몰라서 다시 한번 해본다. 진행이 안되면 안되잖오.
                    int nNoCompletedMapIndex = -1;
                    int nNoCompletedMapID = pUserInfo->GetNotCompletedMap(nNoCompletedMapIndex);
                    if(nNoCompletedMapID == -1)
                    {
                        pUserInfo->UpgradeCompletedRank();
                        mpRollCtl->ResetMapUpgradeRank();
                        CScenario::gbNeedCompletedMap = true; //맵을 다 깼으니까...
                        break;
                    }
                    //-----------------------------------------------------------------------------

                    //Mofided 2014.11.20 그냥 맵전체로 검색하자 사용자가 없으니 매칭하기가 힘들다. 싱글일때만 맵에대해 락을 건다.
                    //잠금이 되어 있는 상태이고 열쇠가 없는 화면이라면 메세지를 보이고 움직이게 하지말자.
                    if((pUserInfo->GetMultiplayOn() == false || pUserInfo->GetGameCenterAuthStatus() != 2) &&
                       !pUserInfo->IsCompletedMap(pUserInfo->GetLastSelectedMapID()) && mpRollCtl->GetNowMapIndex() != pUserInfo->GetLastSelectedMapIndex())
                    {
                        int nTank = mpRollCtl->GetCurrentPos(CRollMapTankCtl::CRollMapTankCtl_Tank);
                        mpRollCtl->SetCurrentPos(mpRollCtl->GetNowMapIndex(), nTank);
                        ShowShotMessageBox("Tran_Shot_Msg_T","Tran_Com_Msg_Des1","Tran_Com_Msg_Des2");
                        break;
                    }
                    int nCurrentMapIndex = pUserInfo->GetLastSelectedMapIndex();
                    
                    //평점을 요구한다.
                    if(CheckShowReview(nCurrentMapIndex))
                        break;
                    
                    //멀티 플레이이면
                    if(pUserInfo->GetMultiplayOn() && pUserInfo->GetGameCenterAuthStatus() == 2 && nCurrentMapIndex != 0)
                    {
                        //매칭으로 시작하지 않으면 친구찾기를 한다.
                        if(pEvent->lParam2 != 0xFF)
                        {
                            //훈련소맵이지만 아직 깨지를 못하였다면
                            if(!pUserInfo->IsCompletedMap(0) && nCurrentMapIndex != 0)
                            {
                                pUserInfo->SetLastSelectedMapID(0);
                                pUserInfo->SetLastSelectedMapIndex(0);
                                MENUANI step = GetNowMenuStep();
                                if(step == MENUANI_WORLD_TO_MAP)
                                    SetAniStep(MENUANI_MAP_TO_WORLD);
                                else if(step == MENUANI_WORLD_TO_TANK)
                                    SetAniStep(MENUANI_TANK_TO_WORLD);
                                else if(step == MENUANI_WORLD_TO_FACTORY)
                                    SetAniStep(MENUANI_FACTORY_TO_WORLD);

                                break;
                            }
                               
                            if(!(GetMutiplyProtocol()->GetMultiplayState() == SYNC_NONE || GetMutiplyProtocol()->GetMultiplayState() == SYNC_MATCHINGERROR))
                            {
                                //찾는중에 다시 시작 버튼을 누루면 바로 시작해 버린다.
                                StartPlayNoMatching();
                            }
                            else
                            {
                                if(gFindPlyer)
                                {
                                    //Added 2014.11.22 플래그가 꼬인다. 그래서 클리어 한후에 파인드를 하자.
                                    gInitFindPlayer();
                                    
                                    gFindPlyer(20,MUL_FILTER_ALL,GetMutiplyProtocol()->GetFindMatchingGroup());
                                }
                            }
                        }
                        else //매칭으로 시작하면 플레이 한다.
                        {
                            MENUANI step = GetNowMenuStep();
                            if(step == MENUANI_WORLD_TO_MAP)
                                SetAniStep(MENUANI_MAP_TO_WORLD);
                            else if(step == MENUANI_WORLD_TO_TANK)
                                SetAniStep(MENUANI_TANK_TO_WORLD);
                            else if(step == MENUANI_WORLD_TO_FACTORY)
                                SetAniStep(MENUANI_FACTORY_TO_WORLD);
                            
                            //------------------------------------------------------
                            //플레이 준비중에 있다
                            CMutiplyProtocol* pro = mpScenario->GetMutiplyProtocol();
                            if(!pro->IsStop())
                                pro->SetMultiplayState(SYNC_READYPLAY);
                            //------------------------------------------------------
                        }
                        
                    }
                    else
                    {
                        MENUANI step = GetNowMenuStep();
                        if(step == MENUANI_WORLD_TO_MAP)
                            SetAniStep(MENUANI_MAP_TO_WORLD);
                        else if(step == MENUANI_WORLD_TO_TANK)
                            SetAniStep(MENUANI_TANK_TO_WORLD);
                        else if(step == MENUANI_WORLD_TO_FACTORY)
                            SetAniStep(MENUANI_FACTORY_TO_WORLD);
                    }
#else
                    CUserInfo* pUserInfo = GetUserInfo();
                    //-----------------------------------------------------------------------------
                    //CMissionEndCtl에서 이미처리를 했지만 혹시 몰라서 다시 한번 해본다. 진행이 안되면 안되잖오.
                    int nNoCompletedMapIndex = -1;
                    int nNoCompletedMapID = pUserInfo->GetNotCompletedMap(nNoCompletedMapIndex);
                    if(nNoCompletedMapID == -1)
                    {
                        pUserInfo->UpgradeCompletedRank();
                        mpRollCtl->ResetMapUpgradeRank();
                        CScenario::gbNeedCompletedMap = true; //맵을 다 깼으니까...
                        break;
                    }
                    //-----------------------------------------------------------------------------
                    
                    
                    //잠금이 되어 있는 상태이고 열쇠가 없는 화면이라면 메세지를 보이고 움직이게 하지말자.
                    if(!pUserInfo->IsCompletedMap(pUserInfo->GetLastSelectedMapID()) && mpRollCtl->GetNowMapIndex() != pUserInfo->GetLastSelectedMapIndex())
                    {
                        int nTank = mpRollCtl->GetCurrentPos(CRollMapTankCtl::CRollMapTankCtl_Tank);
                        mpRollCtl->SetCurrentPos(mpRollCtl->GetNowMapIndex(), nTank);
                        ShowShotMessageBox("Tran_Shot_Msg_T","Tran_Com_Msg_Des1","Tran_Com_Msg_Des2");
                        break;
                    }

                    
                    MENUANI step = GetNowMenuStep();
                    if(step == MENUANI_WORLD_TO_MAP)
                        SetAniStep(MENUANI_MAP_TO_WORLD);
                    else if(step == MENUANI_WORLD_TO_TANK)
                        SetAniStep(MENUANI_TANK_TO_WORLD);
                    else if(step == MENUANI_WORLD_TO_FACTORY)
                        SetAniStep(MENUANI_FACTORY_TO_WORLD);
#endif
///////////////////////////////////////////////////////////////////////
                    
                    
                }
                    break;
                case BTN_MENU_PLAY:
                {
                    CUserInfo* pUserInfo = GetUserInfo();
                    if(pUserInfo->GetLastSelectedMapID() != 0)
                        mpScenario->GoToWorld(NULL, CSCENARIO_SINGLEGAME);
                    else if(pUserInfo->GetLastSelectedMapID() == 0)
                        mpScenario->GoToWorld(NULL, CSCENARIO_TRAININGCENTER);
                        
                }
                    break;
                case BTN_ITEMTAB:
                {
                    CButtonCtl* pCtl = (CButtonCtl*)pEvent->lObject;
                    pCtl->ArrangeToggleButton();

                    mpFrameBombTable->SetHide(true);
                    mpFrameItembTable->SetHide(false);
                    if(mpSoundMan) mpSoundMan->PlaySystemSound(SOUND_ID_Click);
                }
                    break;
                case BTN_BOMBTAB:
                {
                    CButtonCtl* pCtl = (CButtonCtl*)pEvent->lObject;
                    pCtl->ArrangeToggleButton();
                    
                    mpFrameBombTable->SetHide(false);
                    mpFrameItembTable->SetHide(true);
                    if(mpSoundMan) mpSoundMan->PlaySystemSound(SOUND_ID_Click);
                    break;
                }
                case BTN_ITEMTABLE: //아이폰 폭탄 다이얼로그
                case BTN_BOMBTABLE:
                {
                    int nType = pEvent->lParam - BTN_BOMBTABLE;
                    CControl* pCtn = (CControl*)pEvent->lObject;
                    ShowBuyDlg(pCtn->GetLData(), nType);
                    if(mpSoundMan) mpSoundMan->PlaySystemSound(SOUND_ID_Click);
                }
                    break;
                case BTN_BOMBBUY: //아이패드 폭탄 다이얼로그
                case BTN_ITEMBUY: //아이템 다이얼로그
                {
                    int nType = pEvent->lParam - BTN_BOMBBUY;
                    CControl* pCtn = (CControl*)pEvent->lObject;
                    ShowBuyDlg(pCtn->GetLData(), nType);
                    if(mpSoundMan) mpSoundMan->PlaySystemSound(SOUND_ID_Click);
                }
                    break;
                case BTN_BOMBBUYOK: //폭탄을 사다.
                {
                    CControl* pCtn = (CControl*)pEvent->lObject;
                    if(OnPurchasedBomb(pCtn->GetLData()))
                        CloseDlg();
                    //if(mpSoundMan) mpSoundMan->PlaySystemSound(SOUND_ID_Click);
                }
                    break;
                case BTN_ITEMBUYOK: //아이템을 사다.
                {
                    CControl* pCtn = (CControl*)pEvent->lObject;
                    if(OnPurchasedItem(pCtn->GetLData()))
                        CloseDlg();
                    //if(mpSoundMan) mpSoundMan->PlaySystemSound(SOUND_ID_Click);
                }
                    break;
                case BTN_ITEMBOMBBUYCANCEL:
                {
                    CloseDlg();
                    if(mpSoundMan) mpSoundMan->PlaySystemSound(SOUND_ID_Click);
                }
                    break;
                case BTN_ITEMSLOTUPPAGE: //아이템 슬롯 다음 페이지
                {
                    int nMaxPageCnt = 0;
                    const int nMaxItem = MAXSLOTCNT;
                    vector<CControl*> lstSlotCtl;
                    CUserInfo* pUserInfo = mpScenario->GetUserInfo();
                    
                    bool bItem = !mpItemSlotSwitchBtn->GetPushed();
                    vector<USERINFO*>* lstItem = NULL;
                    if(bItem)
                        lstItem = pUserInfo->GetListItem();
                    else
                        lstItem = pUserInfo->GetListBomb();
                    
                    int nSelIndex = 1;
                    if(bItem)
                        nSelIndex = pUserInfo->GetLastSelectedItemPageIndex() + 1;
                    else
                        nSelIndex = pUserInfo->GetLastSelectedBombPageIndex() + 1;
                        
                    nMaxPageCnt = (int)lstItem->size() / nMaxItem;
                    if((lstItem->size() % nMaxItem) != 0)
                        nMaxPageCnt += 1;
                    
                    if(nSelIndex > nMaxPageCnt) break;
                    
                    if(bItem)
                        pUserInfo->SetLastSelectedItemPageIndex(nSelIndex);
                    else
                        pUserInfo->SetLastSelectedBombPageIndex(nSelIndex);
                        
                    ResetDisplayItemSlot();
                    pUserInfo->Save();
                    if(mpSoundMan) mpSoundMan->PlaySystemSound(SOUND_ID_Click);
                }
                    break;
                case BTN_ITEMSLOTDOWNPAGE: //아이템 슬롯 이전 페이지.
                {
                    vector<CControl*> lstSlotCtl;
                    CUserInfo* pUserInfo = mpScenario->GetUserInfo();
                    bool bItem = !mpItemSlotSwitchBtn->GetPushed();
                    int nSelIndex = 1;
                    if(bItem)
                        nSelIndex = pUserInfo->GetLastSelectedItemPageIndex() - 1;
                    else
                        nSelIndex = pUserInfo->GetLastSelectedBombPageIndex() - 1;
                        
                    if( nSelIndex < 1) break;
                    
                    if(bItem)
                        pUserInfo->SetLastSelectedItemPageIndex(nSelIndex);
                    else
                        pUserInfo->SetLastSelectedBombPageIndex(nSelIndex);
                    ResetDisplayItemSlot();
                    pUserInfo->Save();
                    if(mpSoundMan) mpSoundMan->PlaySystemSound(SOUND_ID_Click);
                }
                    break;
                case BTN_HELP_DLG:
                {
#ifdef HELPBTN
                    ShowHelpDlg();
#else
                    if(gShowSNSDlg)
                        gShowSNSDlg();
#endif //HELPBTN
                    
                    if(mpSoundMan) mpSoundMan->PlaySystemSound(SOUND_ID_Click);
                }
                    break;
                case BTN_SETTINGS_DLG:
                {
                    ShowSettingsDlg();
                    if(mpSoundMan) mpSoundMan->PlaySystemSound(SOUND_ID_Click);
                }
                    break;
                case BTN_SETTINGS_SAVE:
                {
                    CUserInfo* pUserInfo = mpScenario->GetUserInfo();
                    CButtonCtl* pBGM = (CButtonCtl*)mpSettingsDlg->FindControl(BTN_SETTINGS_BGM);
                    if(pBGM->GetPushed())
                    {
                        if(pUserInfo->GetMusic() == 0) //이전에 꺼져있으면 음악을 켜준다.
                        {
                            pUserInfo->SetMusic(1);
                            PlayBgSound("MenuBak");
                        }
                        else
                            pUserInfo->SetMusic(1);
                    }
                    else
                    {
                        if(pUserInfo->GetMusic() == 1) //이전에 켜져있으면 음악을 끈다.
                            StopBgSound();
                        pUserInfo->SetMusic(0);
                    }
                    CButtonCtl* pFx = (CButtonCtl*)mpSettingsDlg->FindControl(BTN_SETTINGS_FX);
                    if(pFx->GetPushed())
                        pUserInfo->SetSoundFX(1);
                    else
                        pUserInfo->SetSoundFX(0);
                    CButtonCtl* pVibration = (CButtonCtl*)mpSettingsDlg->FindControl(BTN_SETTINGS_VIBRATION);
                    if(pVibration->GetPushed())
                        pUserInfo->SetVibration(1);
                    else
                        pUserInfo->SetVibration(0);
                    pUserInfo->Save(); // 저장.
                    if(mpSoundMan) mpSoundMan->PlaySystemSound(SOUND_ID_Click);
                }
                case BTN_SETTINGS_CANCEL:
                case BTN_HELP_CANCEL:
                {
                    CloseDlg();
                    if(mpSoundMan) mpSoundMan->PlaySystemSound(SOUND_ID_Click);
                }
                    break;
                case BTN_SELECT_TANK: //탱크를 선택한다.
                {
                    
                    int nPos = mpRollCtl->GetCurrentPos(CRollMapTankCtl::CRollMapTankCtl_Tank);
                    if(nPos != -1)
                    {
                        CUserInfo* pUserInfo = mpScenario->GetUserInfo();
                        
                        RollItem* pItem = (RollItem*)mpRollCtl->GetItem(nPos, CRollMapTankCtl::CRollMapTankCtl_Tank);
                        
                        PROPERTY_TANK property;
//                        pUserInfo->GetRank();
                        if(!PROPERTY_TANK::GetPropertyTank(pItem->nID,property))
                        {
                            HLogE("PROPERTY_TANK::GetPropertyTank Can not find tank id %d\n",pItem->nID);
                            break;
                        }
                        if(pUserInfo->GetRank() >= property.nMinLevel)
                        {
                            
                            if(property.nGold == 0) //선택.
                            {
                                if(pItem)
                                {
                                    pUserInfo->SetLastSelectedTankIndex(nPos);
                                    pUserInfo->SetLastSelectedTankID(pItem->nID);
                                    OnSelectedTank(pItem->nID,false);
                                }
                                pUserInfo->Save();                            
        //                        mpTankDescBar->SetHide(true); //선택하면 안보여준다.
                                mpTankDescBar->SetAni(CONTROLANI_TOUT,2.5);
                                if(mpSoundMan)
                                    mpSoundMan->PlaySystemSound(SOUND_ID_Click);
                                
                                ResetFrameItemTable();
                            }
                            else
                            {
                                if(pUserInfo->GetGold() >= property.nGold)
                                {
                                    
                                    PROPERTY_ITEM Prop;
                                    PROPERTY_ITEM::GetPropertyItem(property.nID, Prop);
                                    if(mpItemSlotSwitchBtn->GetPushed())
                                        mpItemSlotSwitchBtn->SetPushed(false);
                                    OnClickedItemSlot(property.nID);
                                    pUserInfo->AddListToItem(property.nID,Prop.nType); //이미존재하면
                                    
                                    
                                    int nPage = VisibleDisplayItemSlotPage(Prop.nID);
                                    pUserInfo->SetLastSelectedItemPageIndex(nPage);
                                    pUserInfo->SetGold(pUserInfo->GetGold() - property.nGold);
                                    
                                    //스롯을 다시그려준다.
                                    ResetDisplayItemSlot();
                                    ResetDisplayTopGold();
                                    
//                                    //폰일때는 슬롯을 보여지지 않잖아. 그래서 계속해서 안보여지게 하자.
//                                    if(gnDisplayType == DISPLAY_IPHONE)
//                                        mpItemSlotBar->SetHide(false);
//                                    if(gnDisplayType == DISPLAY_IPAD)
                                        AnimationDisplayItemSlot(property.nID);
                                   
                                    
                                    pUserInfo->Save();
                                    if(mpSoundMan)
                                        mpSoundMan->PlaySystemSound(SOUND_ID_EnergyUP);
                                    
                                }
                                else
                                {
                                    if(mpSoundMan)
                                        mpSoundMan->PlaySystemSound(SOUND_ID_Buzzer);
                                    
                                    //창을 흔든다.
                                    mpTankDescBar->SetAni(CONTROLANI_WIGWAG,0.3f);
                                    
                                    //언어 해주어야 한다. NoGold_Shot_Msg_Des1
                                    ShowShotMessageBox("Tran_Shot_Msg_T","NoGold_Shot_Msg_Des1","NoGold_Shot_Msg_Des2");
                                }
                            }
                        }
                        else
                        {
                            if(mpSoundMan)
                                mpSoundMan->PlaySystemSound(SOUND_ID_Buzzer);
                            
                            //창을 흔든다.
                            mpTankDescBar->SetAni(CONTROLANI_WIGWAG,0.3f);
                            //선택못하는 이유에 대하여 메세지를 보여준다.
                            ShowShotMessageBox(GetRankPath(property.nMinLevel), 0.15f, 0.15f, 0.85f, 0.85f, "none","M_Tank_Desc_Requested", "M_Tank_Desc_Rank");
                            
                        }
                    }
                    //mAniMationControl.push_back(mpTankDescBar->Clone());
                        
                }
                    break;
//                case CTL_TABLE_ITEM_SLOT1:
//                case CTL_TABLE_ITEM_SLOT2:
                case CTL_TABLE_ITEM_SLOT3:
                case CTL_TABLE_ITEM_SLOT4:
                case CTL_TABLE_ITEM_SLOT5:
                case CTL_TABLE_ITEM_SLOT6:
                {
                    CControl* pCtn = (CControl*)pEvent->lObject;
                    OnClickedItemSlot(pCtn->GetLData());
                }
                    break;
                case BTN_ITEMSLOTSWITCH:
                {
                    CUserInfo* pUserInfo = mpScenario->GetUserInfo();
                    if(mpItemSlotSwitchBtn->GetPushed())
                        pUserInfo->SetLastBombItemSwitch(1);
                    else
                        pUserInfo->SetLastBombItemSwitch(0);
                    ResetDisplayItemSlot();
                    if(mpSoundMan) mpSoundMan->PlaySystemSound(SOUND_ID_Click);
                }
                    break;
                case BTN_NEW_VERSION:
                {
                    //------------------------------
                    // New Version 새로운 버전이 존재 하여 다운로드를 한다.
                    //------------------------------
                    if(gShowNewVersionDlg)
                        gShowNewVersionDlg(gsNewAppVersion);
                }
                    break;
                default:
                    break;
            }
        }
        case SGL_MESSAGE_ROLLSTOP:
        {
            if(pEvent->lParam == BTN_MENU2_MAP_ROLL)
            {
                CUserInfo* pUserInfo = mpScenario->GetUserInfo();
                RollItem* pItem = (RollItem*)pEvent->lParam3;
                if(pItem)
                {
                    pUserInfo->SetLastSelectedMapIndex(pEvent->lParam2);
                    pUserInfo->SetLastSelectedMapID(pItem->nID);
                    
                    OnSelectedMap(); //맵이 선택되면 왼쪽 맵 정보를 변경해준다.
                }
            }
            else if(pEvent->lParam == BTN_MENU2_TANK_ROLL)
            {
                RollItem* pItem = (RollItem*)pEvent->lParam3;
                if(pItem)
                {
                    OnSelectedTank(pItem->nID,true);
                    SetPhoneStyleTankDescAni(pItem->nID);
                }
            }
        }
            break;
        case SGL_SHOP_PRODUCT_GOLDTTYPE: //Gold Type의 Product를 샀다.
        {
            CUserInfo* pUserInfo = mpScenario->GetUserInfo();
            

            //---------------------------------------
            //Product을 적용해준다.
            //---------------------------------------
            if(pEvent->lParam == ID_ITEM_GOLD_4000 || pEvent->lParam == ID_ITEM_GOLD_9000) //골드타임
            {
                
                float fG = 4000.f;
                if(pEvent->lParam == ID_ITEM_GOLD_9000)
                    fG = 9000.f;
                else if(pEvent->lParam == ID_ITEM_GOLD_4000)
                    fG = 4000.f;
                pUserInfo->SetGold(fG + pUserInfo->GetGold());
                ResetDisplayTopGold();
            }
            else if(pEvent->lParam == ID_ITEM_ATTACKBYRADA || pEvent->lParam == ID_ITEM_REMOVE_AD || pEvent->lParam == ID_ITEM_UPGRADE_PACKAGE || pEvent->lParam == ID_TIEM_FASTBUILD || pEvent->lParam == ID_TIEM_FASTGOLD)
            {
                OnPurchasedProductItem(pEvent->lParam);
            }
            
            pUserInfo->SetBuy();
            pUserInfo->Save();
        
            if(mpSoundMan)
                mpSoundMan->PlaySystemSound(SOUND_ID_EnergyUP);

            HLogD("Producted SGL_SHOP_PRODUCT_GOLDTTYPE ID %d\n",pEvent->lParam);
        }
            break;
        case SGL_BUY_PRODUCT_GOLDTTYPE_FOR_ANDROID:
        {
#if defined(ANDROIDAPPSTOREKIT)
            extern char* strtolower(const char *s);
            
            HLogD("sglReadCacheProduct before\n");
            const char* sData = sglReadCacheProduct();
            char sBundle[100];
            char* slogBundle;
            char* sTData;
            char* sTDataX;

            HLogD("sglReadCacheProduct after\n");
            if(sData == NULL || strlen(sData) == 0) break;

            HLogD("sglReadCacheProduct %s\n",sData);
            sTDataX = sTData = new char[strlen(sData)+1];
            strcpy(sTData, sData);
            strcpy(sBundle, GetBundleID());
            slogBundle = strtolower(sBundle);
            
            char* sTok = strtok(sTData,"\n");
            while (sTok)
            {
                const char* sID = strstr(sTok, slogBundle);
                if(sID)
                {
                    sID = sID + strlen(slogBundle) + 1;
                    PROPERTY_ITEM item;
                    PROPERTY_ITEM::GetPropertyItem(sID, item); //대문자 소문자 구분하지 않고 찾아옴
                    CScenario::SendMessage(SGL_SHOP_PRODUCT_GOLDTTYPE,item.nID,item.nType,0,0);
                }
                sTok = strtok(NULL,"\n");
            }
            delete[] slogBundle;
            delete[] sTDataX;
            
#endif //ANDROIDAPPSTOREKIT
        }
            break;
        case SGL_MESSAGE_PROMOTION_RANK: //진급 메세지 박스를 보여준다.
        {
            ShowDialogPromotionRank(pEvent->lParam);
            //탱크설명을 다시그려준다.
            OnSelectedTank(GetUserInfo()->GetLastSelectedTankID(),false);
        }
            break;
        case SGL_RESULT_REVIEW:
        {
            ResultReviews();
            break;
        }
        case SGL_CLICKED_ADMOB_ANDROID:
            GetUserInfo()->SetClickedProp();
            break;
        case SGL_MATCHING_MSG:
        {
            string sT = SGLTextUnit::GetOnlyText("Multi_MMatchT");
            string sM = SGLTextUnit::GetOnlyText("Multi_MMatchDes1");
            string sM2 = SGLTextUnit::GetOnlyText("Multi_MMatchDes2");
            
            ShowShotMessageBox(sT.c_str(), sM.c_str(), sM2.c_str());
            break;
        }
        default:
            break;
    }
    
    return E_SUCCESS;
}

void CMainMenuWorld::ShowDialogPromotionRank(int nRank)
{
    
    vector<string> lstImage;
    CUserInfo* pUserInfo = GetUserInfo();
    pUserInfo->SetRank(nRank);
    pUserInfo->Save(); //진급을 저장해 준다.
    
    if(mpPromotionRank == NULL)
    {
        float fWhite[] = {1.0f,1.0f,1.0f,1.f};
        //CellBack
        lstImage.clear();
        lstImage.push_back(IMG_TABLE_BANDI);
        lstImage.push_back("none");
        
        mpPromotionRank =  new CLabelCtl(NULL,m_pTextureMan);
        
        
        int cMsgWidth = 306;
        int cMsgHeight = 171;
        
        
        lstImage.clear();
        lstImage.push_back("res_Dialog.png");
        lstImage.push_back("none");
        
        int nPoxX = (gDisplayWidth - cMsgWidth) / 2;
        int nPoxY = (gDisplayHeight - cMsgHeight) / 2;
        mpPromotionRank->Initialize(0, nPoxX, nPoxY, cMsgWidth, cMsgHeight, lstImage,0.017578125,0.017578125,0.615234375,0.3515625);
    
        
        
        lstImage.clear();
        lstImage.push_back("none");
        lstImage.push_back("Prom_Title");
        //진급을 축하합니다.
        CControl* pMsgCtl = new CLabelCtl(mpPromotionRank,m_pTextureMan);
        pMsgCtl->Initialize(111, nPoxX + 32, nPoxY + 34, 250, 45, lstImage,0.f,0.f,1.f,1.f);
        pMsgCtl->SetTextColor(fWhite);
        mpPromotionRank->AddControl(pMsgCtl);
        
        
        //계급아이콘
        float fYInter = 8.f;
        lstImage.clear();
        lstImage.push_back("res_slot.png");
        lstImage.push_back("none");
        CButtonCtl* pRankImg =  new CButtonCtl(mpPromotionRank,m_pTextureMan);
        pRankImg->Initialize(111, nPoxX + 44, nPoxY + 84, 65, 65, lstImage, 0.2890625, 0.328125, 0.416015625, 0.4550781255,
                             0.654296875,0.328125,0.78125,0.45703125);
        mpPromotionRank->AddControl(pRankImg);
        
        lstImage.clear();
        lstImage.push_back(mpScenario->GetRankPath());
        lstImage.push_back("none");
        CControl* pIcon =  new CLabelCtl(pRankImg,m_pTextureMan);
        pIcon->Initialize(111, nPoxX + 44 + fYInter + 1, nPoxY + 84 + fYInter + 1, 65 - fYInter*2 - 1, 65 - fYInter*2 - 2, lstImage, 0.f, 0.f, 1.f, 1.f);
        pIcon->SetEnable(false);
        pRankImg->AddControl(pIcon);
        
        lstImage.clear();
        lstImage.push_back("none");
        lstImage.push_back("Prom_Msg1");
        //방어력,공격력,Gold
        CControl* pMsg1 = new CLabelCtl(mpPromotionRank,m_pTextureMan);
        pMsg1->Initialize(111, nPoxX + 102, nPoxY + 84, 180, 30, lstImage,0.f,0.f,1.f,1.f);
        pMsg1->SetTextColor(fWhite);
        mpPromotionRank->AddControl(pMsg1);

        const char* sMsgF = SGLTextUnit::GetText("Prom_Msg2");
        char sMsg[128];
        sprintf(sMsg, sMsgF,nRank * 2);
        lstImage.clear();
        lstImage.push_back("none");
        lstImage.push_back(sMsg);
        
        CControl* pMsg2 = new CLabelCtl(mpPromotionRank,m_pTextureMan);
        pMsg2->Initialize(111, nPoxX + 102, nPoxY + 119, 180, 30, lstImage,0.f,0.f,1.f,1.f);
        pMsg2->SetTextColor(fWhite);
        mpPromotionRank->AddControl(pMsg2);
        
        mControlList.push_back(mpPromotionRank);
    }
    mpPromotionRank->SetHide(false);
    
    //---------------------------------------------
    //투명처리된 컨트롤들을 다시 복원해준다.
    mfPromotionRankAlpha = 1.0f;
    mpPromotionRank->SetTranslate(mfPromotionRankAlpha);
    mnPromotionRankTime = GetGGTime() + 5000;
    
    //랭크바에 적용해준다.
    lstImage.clear();
    lstImage.push_back(mpScenario->GetRankPath());
    lstImage.push_back("none");
    mpRankPannel->FindControl(CTL_RANK_ICON)->SetImageData(lstImage);
    
    if(mpSoundMan) mpSoundMan->PlaySystemSound(SOUND_ID_EnergyUP);
    
}


CTextureMan* CMainMenuWorld::GetTextureMan(){ return m_pTextureMan;}
CModelMan* CMainMenuWorld::GetModelMan(){ return NULL;}

void CMainMenuWorld::BeginTouch(long lTouchID,float x, float y)
{
    //애니메이팅하고 있다면... 터치를 무시한다.
    if(Animating()) return;
    
    //터치를 막자.
    if(mpCompletedDlg) return;
    
    if(mpBuyDlg)
    {
        mpBuyDlg->BeginTouch(lTouchID, x, y);
        return;
    }
#ifdef HELPBTN
    if(mpHelpDlg)
    {
        if(mpHelpDlg->BeginTouch(lTouchID, x, y) == true)
        {
            CloseDlg();
        }
        return;
    }
#endif //
    if(mpSettingsDlg)
    {
        mpSettingsDlg->BeginTouch(lTouchID, x, y);
        return;
    }
    
    //1>시작 버튼으로 매칭중에 있다면
    if(!(GetMutiplyProtocol()->GetMultiplayState() == SYNC_NONE || GetMutiplyProtocol()->GetMultiplayState() == SYNC_MATCHINGERROR))
    {
        mpStartPannel->BeginTouch(lTouchID, x, y);
        return ;
    }
    
    int nSize = (int)mControlList.size();
    for (int i = 0; i < nSize; i++) {
        if(mControlList[i]->BeginTouch(lTouchID,x, y) == false)
            break;
    }    
}

void CMainMenuWorld::MoveTouch(long lTouchID,float x, float y)
{
    if(Animating()) return;
    if(mpBuyDlg)
    {
        mpBuyDlg->MoveTouch(lTouchID, x, y);
        return;
    }
#ifdef HELPBTN
    if(mpHelpDlg)
    {
        mpHelpDlg->MoveTouch(lTouchID, x, y);
        return;
    }
#endif //HELPBTN
    if(mpSettingsDlg)
    {
        mpSettingsDlg->MoveTouch(lTouchID, x, y);
        return;
    }

    
    int nSize = (int)mControlList.size();
    for (int i = 0; i < nSize; i++) {
        if(mControlList[i]->MoveTouch(lTouchID,x, y) == false)
            break;
    }
}

void CMainMenuWorld::EndTouch(long lTouchID,float x, float y)
{
    if(Animating()) return;
    
    if(mpBuyDlg)
    {
        mpBuyDlg->EndTouch(lTouchID, x, y);
        return;
    }
#ifdef HELPBTN
    if(mpHelpDlg)
    {
        mpHelpDlg->EndTouch(lTouchID, x, y);
        return;
    }
#endif //HELPBTN

    if(mpSettingsDlg)
    {
        mpSettingsDlg->EndTouch(lTouchID, x, y);
        return;
    }

    
    int nSize = mControlList.size();
    for (int i = 0; i < nSize; i++) {
        if(mControlList[i]->EndTouch(lTouchID,x, y) == false)
            break;
    }
}

MENUANI CMainMenuWorld::GetAniStepMenu2(CControl* pGroupCtl,int nNotIndex)
{
    return MENUANI_NONE;
}

MENUANI CMainMenuWorld::GetNowMenuStep()
{
    CList<CControl*>* Childs = mpMenuBar->GetChilds();
    int nSize = Childs->size();
    for (int i = 0; i < nSize; i++)
    {
        CButtonCtl* pButton = dynamic_cast<CButtonCtl*>(Childs->get(i));
        if (pButton)
        {
            if(pButton->GetButtonType() == BUTTON_GROUP_TOGGLE && pButton->GetPushed())
            {
                if(i == 0)
                    return MENUANI_WORLD_TO_MAP;
                else if( i == 1)
                    return MENUANI_WORLD_TO_TANK;
                else if( i == 2)
                    return MENUANI_WORLD_TO_FACTORY;
            }
        }
    }
    return MENUANI_NONE;
}


CControl* CMainMenuWorld::FindControl(vector<CControl*>& list,int nID)
{
    int nCnt = (int)list.size();
    for (int i = 0; i < nCnt; i++) {
        if(list[i]->GetID() == nID)
            return list[i];
    }
    return NULL;
    
}


#ifdef ANDROID
void CMainMenuWorld::ResetTexture()
{
    HLogD("---------ResetTexture");
    
    CWorld::ResetTexture();

    
    int nSize = mControlList.size();
    for(int i = 0; i < nSize; i++)
    {
        mControlList[i]->ResetTexture();
    }

    
//    nSize = mListStep2Ctl.size();
//    for(int i = 0; i < nSize; i++)
//    {
//        mListStep2Ctl[i]->ResetTexture();
//    }

    if(mpBuyDlg)
        mpBuyDlg->ResetTexture();
#ifdef HELPBTN
    if(mpHelpDlg)
        mpHelpDlg->ResetTexture();
#endif //HELPBTN

    
    
    if(mpSettingsDlg)
        mpSettingsDlg->ResetTexture();
    
    HLogD("---------ResetTexture End");
    
    //배경
    if(mpTopBack) mpTopBack->ResetTexture();
    if(mpBottomBack) mpBottomBack->ResetTexture();
    if(mpTopLeftBack) mpTopLeftBack->ResetTexture();
    if(mpBottomLeftBack) mpBottomLeftBack->ResetTexture();
    if(mpBottomRightBack) mpBottomRightBack->ResetTexture();
    if(mpBottomItemBack) mpBottomItemBack->ResetTexture();
    
}
#endif


//이부분을 정리해주어야 한다.
void CMainMenuWorld::CompletedMapDlg()
{
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
    lstImage.push_back("CompletedMapPanel.png");
    lstImage.push_back("none");
    
    mpCompletedDlg =  new CLabelCtl(NULL,GetTextureMan());
    mpCompletedDlg->Initialize(0, fX, fY, layout[2], layout[3], lstImage, 0.f, 0.f, 1.f, 1.f);
    
    
    const char* ArrLayOut[] = {"Trn_Introduce_Msg_T1","Trn_Introduce_Msg_T2","Trn_Introduce_Msg_T3","Trn_Introduce_Msg_T4"};
    const char* ArrT[] = {"Menu_Comp_Msg_T1","Menu_Comp_Msg_T2","Menu_Comp_Msg_T3","Menu_Comp_Msg_T4"};
    int nCnt = sizeof(ArrT) / sizeof(const char*);
    for (int i = 0; i < nCnt; i++)
    {
        
        layout.clear();
        lstImage.clear();
        
        PROPERTYI::GetPropertyFloatList(ArrLayOut[i], layout);
        sText = SGLTextUnit::GetTextAddGLData(ArrT[i],"Trn_Introduce_Msg");
        lstImage.push_back("none");
        lstImage.push_back(sText);
        CLabelCtl *pText = new CLabelCtl(mpCompletedDlg,GetTextureMan());
        pText->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.0, 0.0, 0.0, 0.0);
        pText->SetTextColor(fYellow);
        mpCompletedDlg->AddControl(pText);
    }
    
    mpCompletedDlg->SetTranslate(0.9f);
    mpCompletedDlg->SetAni(CONTROLANI_LIN,1.6f);
    
}





