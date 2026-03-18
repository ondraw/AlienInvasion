//
//  CMessageCtl.cpp
//  SongGL
//
//  Created by Song Hohak on 12. 1. 13..
//  Copyright (c) 2012 thinkm. All rights reserved.
//

#include "CMessageCtl.h"
#include "CButtonCtl.h"
#include "CHWorld.h"
#include "sGL.h"
#define ALLCLOSE 0

CMessageCtl::CMessageCtl(CTextureMan *pTextureMan,CMessageCtlType Type) : CLabelCtl(NULL,pTextureMan)
{
    mType = Type;
}

CMessageCtl::~CMessageCtl()
{
    
}

int CMessageCtl::Initialize()
{
    float fWhite[] = {1.f,1.f,1.f,1.f};
    
    vector<string> lstImage;
    vector<float> layout;
    lstImage.push_back("none");
    lstImage.push_back("none");
    
    //Display Size만큼... 크게 한다.
    CLabelCtl::Initialize(0, 0, 0, gDisplayWidth, gDisplayHeight, lstImage,0.f,0.f,1.f,1.f);
    SetHide(true);
    //------------------------------
  

    //GoToHome ---------------------------------
    CControl *pGoToHome = new CLabelCtl(this,mpTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("WM_GoToHome", layout);
    //컨펌메세지 홈화면으로 갈것인지...
    float cMsgWidth = layout[2];
    float cMsgHeight = layout[3];
    
    
    lstImage.clear();
    lstImage.push_back("res_Dialog.png");
    lstImage.push_back("none");
    
    //(359,298)
    float nPoxX = (gDisplayWidth - cMsgWidth) / 2.f;
    float nPoxY = (gDisplayHeight - cMsgHeight) / 2.f;
    pGoToHome->Initialize(0, nPoxX, nPoxY, cMsgWidth, cMsgHeight, lstImage,0.017578125,0.017578125,0.615234375,0.3515625);
    pGoToHome->SetHide(true);
    
    //ICon
    lstImage.clear();
    lstImage.push_back("res_Dialog.png");
    lstImage.push_back("none");
    CControl* pIcon = new CLabelCtl(pGoToHome,mpTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("WM_Icon", layout);
    pIcon->Initialize(111, nPoxX + layout[0], nPoxY + layout[1], layout[2], layout[3], lstImage,0.640625,0.017578125,0.73828125,0.138671875);
    pGoToHome->AddControl(pIcon);

    
    if(mType == CMessageCtlType_Request_Aliance)
    {
        string sN;
        const char* sFormat = SGLTextUnit::GetText("ExitBox_Msg");
        char* sFnd = (char*)strstr(sFormat, "#");
        sN = SGLTextUnit::GetOnlyText("AlianceBox_Msg");
        sN.append(sFnd);
        
        lstImage.clear();
        lstImage.push_back("none");
        lstImage.push_back(sN);
        CControl* pMsgCtl = new CLabelCtl(pGoToHome,mpTextureMan);
        layout.clear();
        PROPERTYI::GetPropertyFloatList("WM_MsgCtl", layout);
        pMsgCtl->Initialize(111, nPoxX + layout[0], nPoxY + layout[1] - (layout[3] / 2.f), layout[2], layout[3], lstImage,0.f,0.f,1.f,1.f);
        pMsgCtl->SetTextColor(fWhite);
        pGoToHome->AddControl(pMsgCtl);
    
        const char* sFormat2 = SGLTextUnit::GetText("ExitBox_Msg");
        char* sFnd2 = (char*)strstr(sFormat2, "#");
        sN = SGLTextUnit::GetOnlyText("AlianceBox_Msg2");
        sN.append(sFnd2);
        
        lstImage.clear();
        lstImage.push_back("none");
        lstImage.push_back(sN);
        pMsgCtl = new CLabelCtl(pGoToHome,mpTextureMan);
        layout.clear();
        PROPERTYI::GetPropertyFloatList("WM_MsgCtl", layout);
        pMsgCtl->Initialize(111, nPoxX + layout[0], nPoxY + layout[1] + (layout[3] / 2.f), layout[2], layout[3], lstImage,0.f,0.f,1.f,1.f);
        pMsgCtl->SetTextColor(fWhite);
        pGoToHome->AddControl(pMsgCtl);
        
    }
    else
    {
        lstImage.clear();
        lstImage.push_back("none");
        lstImage.push_back("ExitBox_Msg");
        CControl* pMsgCtl = new CLabelCtl(pGoToHome,mpTextureMan);
        layout.clear();
        PROPERTYI::GetPropertyFloatList("WM_MsgCtl", layout);
        pMsgCtl->Initialize(111, nPoxX + layout[0], nPoxY + layout[1], layout[2], layout[3], lstImage,0.f,0.f,1.f,1.f);
        pMsgCtl->SetTextColor(fWhite);
        pGoToHome->AddControl(pMsgCtl);
    }
    
    
    //확인
    lstImage.clear();
    lstImage.push_back("res_Dialog.png");
    lstImage.push_back("M_IB_YES");
    CControl* pMsgYesCtl = new CButtonCtl(pGoToHome,mpTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("WM_MsgYesCtl", layout);
    pMsgYesCtl->Initialize(BTN_PLAY_CONFIRM_YES, nPoxX + layout[0], nPoxY + layout[1], layout[2], layout[3], lstImage,0.09375,0.3828125,0.33203125,0.45703125);
    pMsgYesCtl->SetTextColor(fWhite);
    pGoToHome->AddControl(pMsgYesCtl);
    
    //취소.
    lstImage.clear();
    lstImage.push_back("res_Dialog.png");
    lstImage.push_back("M_IB_CANCEL");
    CControl* pMsgNoCtl = new CButtonCtl(pGoToHome,mpTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("WM_MsgNoCtl2", layout);
    pMsgNoCtl->Initialize(BTN_PLAY_CONFIRM_NO, nPoxX + layout[0], nPoxY + layout[1], layout[2], layout[3], lstImage,0.330078125,0.380859375,0.5625,0.451171875);
    pGoToHome->AddControl(pMsgNoCtl);
    pMsgNoCtl->SetTextColor(fWhite);
    this->AddControl(pGoToHome);
   
    
    return E_SUCCESS;
}

void CMessageCtl::SetHide(bool bHide)
{
    mbHide = bHide;
}

#ifndef ALLCLOSE
extern "C" void ShowCloseMissionView();
#endif

void CMessageCtl::Show()
{
#ifdef ALLCLOSE
    SetHide(false);
    
    CList<CControl*>* childes = GetChilds();
    
    if (mType == CMessageCtlType_GoToMenu || mType == CMessageCtlType_Request_Aliance)
    {
        childes->get(0)->SetHide(false);
    }
    else if(mType == CMessageCtlType_GoToMenu_Died) {
        childes->get(1)->SetHide(false);
    }
    else if(mType == CMessageCtlType_GoToMenu_Successed) {
        childes->get(2)->SetHide(false);
    }
#else
    mType = Type;
    ShowCloseMissionView();
#endif
}

void CMessageCtl::Hide()
{
    SetHide(true);
    CList<CControl*>* childes = GetChilds();
    if (mType == CMessageCtlType_GoToMenu) {
        childes->get(0)->SetHide(true);
    }
    else if(mType == CMessageCtlType_GoToMenu_Died) {
        childes->get(1)->SetHide(true);
    }
    else if(mType == CMessageCtlType_GoToMenu_Successed) {
        childes->get(2)->SetHide(true);
    }
}

