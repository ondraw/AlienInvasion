//
//  CInputCtl.cpp
//  SongGL
//
//  Created by 송 호학 on 11. 12. 20..
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include "CInputCtl.h"
#include "CButtonCtl.h"
#include "CMainMenuWorld.h"
#include "CTextureMan.h"
#include "CNumberCtl.h"
#include "sGLText.h"

CInputCtl::CInputCtl(CUserInfo* pUserInfo,CTextureMan *pTextureMan,int nItemID,CControl* pCountInCell,int nRow) : CPopupCtl(NULL,pTextureMan)
{
    mpUserInfo = pUserInfo;
    mnItemID = nItemID;
    mpCountInCell = pCountInCell;
    mnRow = nRow;
}

CInputCtl::~CInputCtl()
{
    
}


int CInputCtl::Initialize(int nControlID,PROPERTY_BOMB* Prop,int nWidth,int nHeight,vector<string>& lstImage)
{
    mType = CInputCtl_Bomb;
    vector<float> layout;
    int nResult = 0;
    nResult = CPopupCtl::Initialize(nControlID, nWidth, nHeight, lstImage,0.f,0.f,1.f,1.f);
    PROPERTYI::GetPropertyFloatList("M2_Table_Cell_BuyBtn", layout);
    CInputCtl::AddBombCell(mpTextureMan, mpUserInfo, Prop, this,mfDPosX,mfDPosY);
    CButtonCtl* pBuy = new CButtonCtl(this,mpTextureMan);
    lstImage.clear();
    lstImage.push_back(IMG_METAL_RECT_BACK);
    lstImage.push_back(IMGKEY_M1_Left_Grp_Buy);
    pBuy->Initialize(BTN_MENU2_BUYBOMB, mfDPosX+layout[0], mfDPosY+layout[1], layout[2], layout[3], lstImage,0.f,0.f,1.f,1.f);
    
    AddControl(pBuy);
    return nResult;
}


int CInputCtl::Initialize(int nControlID,PROPERTY_ITEM* Prop,int nWidth,int nHeight,vector<string>& lstImage)
{
    int nResult = 0;
    vector<float> layout;
    mType = CInputCtl_Item;
    nResult = CPopupCtl::Initialize(nControlID, nWidth, nHeight, lstImage,0.f,0.f,1.f,1.f);
    PROPERTYI::GetPropertyFloatList("M2_Table_Cell_BuyBtn", layout);
    CInputCtl::AddItemCell(mpTextureMan, mpUserInfo, Prop, this,mfDPosX,mfDPosY);
    CButtonCtl* pBuy = new CButtonCtl(this,mpTextureMan);
    lstImage.clear();
    lstImage.push_back(IMG_METAL_RECT_BACK);
    lstImage.push_back(IMGKEY_M1_Left_Grp_Buy);
    pBuy->Initialize(BTN_MENU2_BUYITEM, mfDPosX+layout[0], mfDPosY+layout[1], layout[2], layout[3], lstImage,0.f,0.f,1.f,1.f);
    AddControl(pBuy);
    
    
    if(Prop->nID == ID_ITEM_UPGRADE_ATTACK)
    {
        msAttackDesc = Prop->sDesc2;
        SetUpgrageData(true);
    }
    else if(Prop->nID == ID_ITEM_UPGRADE_DEPEND)
    {
        msDependDesc = Prop->sDesc2;
        SetUpgrageData(false);
    }
    
    return nResult;
}


//Item 3,4 Upgrade는 각 각 탱크와 각단계별로 돈과 %가 변하면서 올라간다.
void CInputCtl::SetUpgrageData(bool bAttack)
{
    char sData[1024];
    CUserInfo* pUserInfo = mpUserInfo;
    if(bAttack)
    {
        int grade = pUserInfo->GetAttackUpgrade() + 1;
        CNumberCtl* pGold = (CNumberCtl*)GetChild(3);
        int nGold = (int)pGold->GetLData();
        nGold = nGold * grade;
        pGold->SetNumber(nGold);
        
        
        sprintf(sData, GetText(msAttackDesc.c_str()), grade * 5);
        CControl* pDesc2 = GetChild(5);
        
        vector<string> simg;
        simg.push_back("none");
        simg.push_back(sData);
        pDesc2->SetImageData(simg);
    }
    else
    {
        int grade = pUserInfo->GetDependUpgrade() + 1;
        CNumberCtl* pGold = (CNumberCtl*)GetChild(3);
        int nGold = (int)pGold->GetLData();
        nGold = nGold * grade;
        pGold->SetNumber(nGold);
        
        
        sprintf(sData, GetText(msDependDesc.c_str()), grade * 5);
        CControl* pDesc2 = GetChild(5);
        
        vector<string> simg;
        simg.push_back("none");
        simg.push_back(sData);
        pDesc2->SetImageData(simg);
    }
}


void CInputCtl::AddBombCell(CTextureMan *pTextureMan,CUserInfo* pUserInfo,PROPERTY_BOMB* prop,CControl* pCell,int x, int y)
{
//    float fRightGab = -8.0f;
//    if(gnDisplayType == DISPLAY_IPAD)
//    fRightGab *= 2.0f;
    float fRightGab  = 0.0f;
    
    float fRedColor[] = {1.0f,0.0f,0.0f,1.0f};
    float fBlueColor[] = {0.0f,0.0f,1.0f,1.0f};
    int nNum = 0;
    string sFile;
    vector<string>  lstImage;
    vector<float> layout;
    
    
    //이름.
    lstImage.clear();
    layout.clear();
    lstImage.push_back("none");
    lstImage.push_back(prop->sName);
    PROPERTYI::GetPropertyFloatList("M2_Table_Cell_Name", layout);
    CLabelCtl* pName = new CLabelCtl(pCell,pTextureMan);
    pName->Initialize(prop->nID, x + layout[0], y + layout[1], layout[2], layout[3], lstImage,0.f,0.f,1.f,1.f);
    pCell->AddControl(pName);
    
    //폭탄 이미지.
    lstImage.clear();
    layout.clear();
    sFile = prop->sBombTableImgPath;
    lstImage.push_back(sFile);
    lstImage.push_back("none");
    PROPERTYI::GetPropertyFloatList("M2_Table_Cell_Image", layout);
    CLabelCtl* pCSSS = new CButtonCtl(pCell,pTextureMan);
    pCSSS->Initialize(prop->nID, x + layout[0], y + layout[1], layout[2], layout[3], lstImage,0.f,0.f,1.f,1.f);
    pCell->AddControl(pCSSS);
    
    //--
    //돈 라벨.
    layout.clear();
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("TLabelGold");
    PROPERTYI::GetPropertyFloatList("M2_Table_Cell_MoneyLabel", layout);
    CLabelCtl* pMoneyIcon = new CLabelCtl(pCell,pTextureMan);
    pMoneyIcon->Initialize(prop->nID, x + layout[0], y + layout[1], layout[2], layout[3], lstImage,0.f,0.f,1.f,1.f);
    pCell->AddControl(pMoneyIcon);
    
    
    //돈
    layout.clear();
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    PROPERTYI::GetPropertyFloatList("M2_Table_Cell_Money", layout);
    CNumberCtl* pGoldNumberLabel = new CNumberCtl(pCell,pTextureMan);
    pGoldNumberLabel->Initialize(prop->nID, x + layout[0], y + layout[1], layout[2], layout[3], layout[3],0,0,lstImage);
    pGoldNumberLabel->SetAlign(CNumberCtl::ALIGN_RIGHT);
    pGoldNumberLabel->SetNumber(prop->nPrice); //돈
    pCell->AddControl(pGoldNumberLabel);
    
    //--
    //파워 라벨
    layout.clear();
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("TLabelAttack");
    PROPERTYI::GetPropertyFloatList("M2_Table_Cell_PowerLabel", layout);
    CLabelCtl* pPowerIcon = new CLabelCtl(pCell,pTextureMan);
    pPowerIcon->Initialize(prop->nID, x + layout[0], y + layout[1], layout[2], layout[3], lstImage,0.f,0.f,1.f,1.f);
    pCell->AddControl(pPowerIcon);
    
    
    //파워
    layout.clear();
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    PROPERTYI::GetPropertyFloatList("M2_Table_Cell_Power", layout);
    CNumberCtl* pPowerNumberLabel = new CNumberCtl(pCell,pTextureMan);
    pPowerNumberLabel->Initialize(prop->nID, x + layout[0], y + layout[1], layout[2], layout[3], layout[3],0,0,lstImage);
    pPowerNumberLabel->SetAlign(CNumberCtl::ALIGN_RIGHT);
    pPowerNumberLabel->SetNumber((int)prop->fMaxAttackPower); //파워
    pCell->AddControl(pPowerNumberLabel);
    
    //파워 tnt 단위
    layout.clear();
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("TLabelUpTNT");
    PROPERTYI::GetPropertyFloatList("M2_Table_Cell_PowerPer", layout);
    CLabelCtl* pPowerPersent = new CLabelCtl(pCell,pTextureMan);
    pPowerPersent->Initialize(prop->nID, x + layout[0], y + layout[1], layout[2], layout[3], lstImage,0.f,0.f,1.f,1.f);
    pCell->AddControl(pPowerPersent);
    
    //--
    //반경 파워
    layout.clear();
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("TLabelRange");
    PROPERTYI::GetPropertyFloatList("M2_Table_Cell_RangeLabel", layout);
    CLabelCtl* pPowerRangeIcon = new CLabelCtl(pCell,pTextureMan);
    pPowerRangeIcon->Initialize(prop->nID, x + layout[0], y + layout[1], layout[2], layout[3], lstImage,0.f,0.f,1.f,1.f);
    pCell->AddControl(pPowerRangeIcon);
    
    //반경
    layout.clear();
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    PROPERTYI::GetPropertyFloatList("M2_Table_Cell_Range", layout);
    CNumberCtl* pPowerRangeNumberLabel = new CNumberCtl(pCell,pTextureMan);
    pPowerRangeNumberLabel->Initialize(prop->nID, x + layout[0] + fRightGab, y + layout[1], layout[2], layout[3], layout[3],0,0,lstImage);
    pPowerRangeNumberLabel->SetAlign(CNumberCtl::ALIGN_RIGHT);
    pPowerRangeNumberLabel->SetNumber(prop->fMaxRadianDetect); //파워
    pCell->AddControl(pPowerRangeNumberLabel);
    
    //반경 m 미터단위.
    layout.clear();
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("TLabelUpM");
    PROPERTYI::GetPropertyFloatList("M2_Table_Cell_RangePer", layout);
    CLabelCtl* pPowerRangePersent = new CLabelCtl(pCell,pTextureMan);
    pPowerRangePersent->Initialize(prop->nID, x + layout[0] + fRightGab, y + layout[1], layout[2], layout[3], lstImage,0.f,0.f,1.f,1.f);
    pCell->AddControl(pPowerRangePersent);
    
    
    //--
    //밤의 특색...
    layout.clear();
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back(prop->sDesc);
    PROPERTYI::GetPropertyFloatList("M2_Table_Cell_Desc", layout);
    CLabelCtl* pBombDesc = new CLabelCtl(pCell,pTextureMan);
    pBombDesc->Initialize(prop->nID, x + layout[0], y + layout[1], layout[2], layout[3], lstImage,0.f,0.f,1.f,1.f);
    pCell->AddControl(pBombDesc);
    
    
    //--
    //한번에 살수 있는 단위
    layout.clear();
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    PROPERTYI::GetPropertyFloatList("M2_Table_Cell_BuyUnit", layout);
    CNumberCtl* pPackCount = new CNumberCtl(pCell,pTextureMan);
    pPackCount->SetTextColor(fBlueColor);
    pPackCount->Initialize(prop->nID, x + layout[0], y + layout[1], layout[2], layout[3], layout[3],0,0,lstImage);
    pPackCount->SetAlign(CNumberCtl::ALIGN_RIGHT);
    pPackCount->SetNumber(prop->nPackCount,CNumberCtlType_Plus); //파워
    pCell->AddControl(pPackCount);
    
    //한번에 살수 있는 단위 라벨.
    layout.clear();
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("TLabelPack");
    PROPERTYI::GetPropertyFloatList("M2_Table_Cell_BuyUnitLabel", layout);
    CLabelCtl* pPackCountLabel = new CLabelCtl(pCell,pTextureMan);
    //pPackCountLabel->SetTextColor(fGreenColor);
    pPackCountLabel->Initialize(prop->nID, x + layout[0], y + layout[1], layout[2], layout[3], lstImage,0.f,0.f,1.f,1.f);
    pCell->AddControl(pPackCountLabel);
    
    //--
    //수량
    layout.clear();
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    PROPERTYI::GetPropertyFloatList("M2_Table_Cell_Count", layout);
    CNumberCtl* pCountNumber = new CNumberCtl(pCell,pTextureMan);
    pCountNumber->SetTextColor(fRedColor);
    pCountNumber->Initialize(BTN_MENU2_TABLE_BOMB_NUM, x + layout[0], y + layout[1], layout[2], layout[3], layout[3],0,0,lstImage);
    pCountNumber->SetAlign(CNumberCtl::ALIGN_RIGHT);
    nNum = pUserInfo?pUserInfo->GetCntByIDWithBomb(prop->nID):0;
    pCountNumber->SetNumber(nNum == -999 ? 1 : nNum);
    pCell->AddControl(pCountNumber);
}



bool CInputCtl::AddItemCell(CTextureMan *pTextureMan,CUserInfo* pUserInfo,PROPERTY_ITEM* prop,CControl* pCell,int x, int y)
{
    float fRedColor[] = {1.0f,0.0f,0.0f,1.0f};
    int nNum = 0;
    vector<string>  lstImage;
    vector<float> layout;
    string sImagePath;
    
    //영구사용물을 이미 샀으면 리스트에 추가하지 말자.
    if(prop->nType == 1) 
    {
        if(pUserInfo)
        {
            vector<USERINFO*>* pUserItemList = pUserInfo->GetListItem();
            int nSize = (int)pUserItemList->size();
            for(int i = 0; i < nSize; i++)
            {
                USERINFO *pInfo = (*pUserItemList)[i];
                if(pInfo->nID == prop->nID) return false;
            }
        }
    }
    
    //이름.0
    layout.clear();
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back(prop->sName);
    PROPERTYI::GetPropertyFloatList("M2_Table_Cell_Name", layout);
    CLabelCtl* pName = new CLabelCtl(pCell,pTextureMan);
    pName->Initialize(prop->nID, x + layout[0], y + layout[1], layout[2], layout[3], lstImage,0.f,0.f,1.f,1.f);
    pCell->AddControl(pName);
    
    //아이템 이미지.1
    layout.clear();
    lstImage.clear();
    sImagePath = prop->sModelPath;
    sImagePath.append(".tga");
    lstImage.push_back(sImagePath);
    lstImage.push_back("none");
    PROPERTYI::GetPropertyFloatList("M2_Table_Cell_Image", layout);
    CLabelCtl* pCSSS = new CButtonCtl(pCell,pTextureMan);
    pCSSS->Initialize(prop->nID, x + layout[0], y + layout[1], layout[2], layout[3], lstImage,0.f,0.f,1.f,1.f);
    pCell->AddControl(pCSSS);
    
    //--
    //돈 라벨.2
    layout.clear();
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("TLabelGold");
    PROPERTYI::GetPropertyFloatList("M2_Table_Cell_MoneyLabel", layout);
    CLabelCtl* pMoneyIcon = new CLabelCtl(pCell,pTextureMan);
    pMoneyIcon->Initialize(prop->nID, x + layout[0], y + layout[1], layout[2], layout[3], lstImage,0.f,0.f,1.f,1.f);
    pCell->AddControl(pMoneyIcon);
    
    
    //돈3
    layout.clear();
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    PROPERTYI::GetPropertyFloatList("M2_Table_Cell_Money", layout);
    CNumberCtl* pGoldNumberLabel = new CNumberCtl(pCell,pTextureMan);
    pGoldNumberLabel->Initialize(prop->nID, x + layout[0], y + layout[1], layout[2], layout[3], layout[3],0,0,lstImage);
    pGoldNumberLabel->SetAlign(CNumberCtl::ALIGN_RIGHT);
    pGoldNumberLabel->SetNumber(prop->nPrice); //돈
    pGoldNumberLabel->SetLData(prop->nPrice);
    pCell->AddControl(pGoldNumberLabel);
    
    //--
    //Desc 4
    layout.clear();
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back(prop->sDesc);
    PROPERTYI::GetPropertyFloatList("M2_Table_Cell_ItemDesc", layout);
    CLabelCtl* pBombDesc = new CLabelCtl(pCell,pTextureMan);
    pBombDesc->Initialize(prop->nID, x + layout[0], y + layout[1], layout[2], layout[3], lstImage,0.f,0.f,1.f,1.f);
    pCell->AddControl(pBombDesc);
    
    //Desc2 5
    layout.clear();
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back(prop->sDesc2);
    PROPERTYI::GetPropertyFloatList("M2_Table_Cell_ItemDesc2", layout);
    CLabelCtl* pBombDesc2 = new CLabelCtl(pCell,pTextureMan);
    pBombDesc2->Initialize(prop->nID, x + layout[0], y + layout[1], layout[2], layout[3], lstImage,0.f,0.f,1.f,1.f);
    pCell->AddControl(pBombDesc2);
    
    //영구 사용...
    if(prop->nType == 0)
    {
        //--
        //수량
        layout.clear();
        lstImage.clear();
        lstImage.push_back("none");
        lstImage.push_back("none");
        PROPERTYI::GetPropertyFloatList("M2_Table_Cell_ItemCount", layout);
        CNumberCtl* pCountNumber = new CNumberCtl(pCell,pTextureMan);
        pCountNumber->SetTextColor(fRedColor);
        pCountNumber->Initialize(BTN_MENU2_TABLE_ITEM_NUM, x + layout[0], y + layout[1], layout[2], layout[3], layout[3],0,0,lstImage);
        pCountNumber->SetAlign(CNumberCtl::ALIGN_RIGHT);
        nNum = pUserInfo?pUserInfo->GetCntByIDWithItem(prop->nID):0;
        pCountNumber->SetNumber(nNum == -999 ? 1 : nNum);
        pCell->AddControl(pCountNumber);
    }
    
    return true;
}



#ifdef ANDROID
void CInputCtl::ResetTexture()
{
    if(mpCountInCell)
        mpCountInCell->ResetTexture();
    CPopupCtl::ResetTexture();
}
#endif