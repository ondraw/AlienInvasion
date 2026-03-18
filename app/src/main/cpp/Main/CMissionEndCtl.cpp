//
//  CMissionEnd.cpp
//  SongGL
//
//  Created by 호학 송 on 12. 9. 5..
//
//

#include "CMissionEndCtl.h"
#include "CButtonCtl.h"
#include "CHWorld.h"
#include "sGL.h"
#include "CUserInfo.h"
#include "CNumberCtl.h"
#include "CScenario.h"


CMissionEndCtl::CMissionEndCtl(float fGoldPerScore,CTextureMan *pTextureMan,CUserInfo* pUserInfo) : CLabelCtl(NULL,pTextureMan)
{
   //mType = CMessageCtlType_GoToMenu;
    mpUserInfo = pUserInfo;

    mnTotalTime = 0;
    
    mfGoldPerScore = fGoldPerScore;
}

CMissionEndCtl::~CMissionEndCtl()
{
    
}

int CMissionEndCtl::Initialize()
{
    float fWhite[] = { 1.f,1.f,1.f,1.f};
    vector<string> lstImage;
    vector<float> layout;
    
//    int cMsgBoxWidth = 478;
//    int cMsgBoxHeight = 268;
    
    
    layout.clear();
    PROPERTYI::GetPropertyFloatList("WMS_Dlg", layout);
    
    float nStartX = (gDisplayWidth - layout[2]) / 2.f + layout[0];
    float nStartY = (gDisplayHeight - layout[3]) / 2.f + layout[1];

    lstImage.push_back("res_missionDialog.png");
    lstImage.push_back("none");
    
    //Display Size만큼... 크게 한다.
    CLabelCtl::Initialize(0, nStartX, nStartY, layout[2], layout[3], lstImage,
                          0.07421875,0.009765625,0.921875,0.490234375);
    SetHide(true);
    
  
    //----------------------------
    // 형태
    //----------------------------
    // 아이콘      미션 성공(실패)
    // Score        :           300
    // Bonus        :          5000
    // Gold         :           300
    // Total Gold   :          3802
    //----------------------------
    //ICon
    //----------------------------
    mpIcon = new CLabelCtl(this,mpTextureMan);
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    
    layout.clear();
    PROPERTYI::GetPropertyFloatList("WMS_Icon", layout);
    
    int nPoxX;// = nStartX + layout[0];
    int nPoxY;// = nStartY + layout[1];

    //Mission Title
    //----------------------------
    mpMissionTitle = new CLabelCtl(this,mpTextureMan);
    lstImage.clear();
    lstImage.push_back("none"); 
    lstImage.push_back("none");
    layout.clear();
    PROPERTYI::GetPropertyFloatList("WMS_MissionTitle", layout);
    nPoxX = nStartX + layout[0];
    nPoxY = nStartY + layout[1];
    mpMissionTitle->Initialize(0, nPoxX, nPoxY, layout[2], layout[3], lstImage,0.f,0.f,1.f,1.f);
    mpMissionTitle->SetTextColor(fWhite);
    AddControl(mpMissionTitle);
    
    //Score Label
    CControl* strLabel = new CLabelCtl(this,mpTextureMan);
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("MissBox_Score");
    layout.clear();
    PROPERTYI::GetPropertyFloatList("WMS_Label", layout);
    nPoxX = nStartX + layout[0];
    nPoxY = nStartY + layout[1];
    strLabel->Initialize(0, nPoxX, nPoxY, layout[2], layout[3], lstImage,0.f,0.f,1.f,1.f);
    strLabel->SetTextColor(fWhite);
    AddControl(strLabel);
    
    mpScore = new CNumberCtl(this,mpTextureMan);
    layout.clear();
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    layout.clear();
    PROPERTYI::GetPropertyFloatList("WMS_Score", layout);
    nPoxX = nStartX + layout[0];
    nPoxY = nStartY + layout[1];
    mpScore->Initialize(10, nPoxX, nPoxY, layout[2], layout[3], layout[4], layout[5], layout[6],lstImage,true);
    mpScore->SetTextColor(fWhite);
    mpScore->SetAlign(CNumberCtl::ALIGN_LEFT);
    mpScore->SetNumber(0);
    AddControl(mpScore);
    
    //Bonus
    strLabel = new CLabelCtl(this,mpTextureMan);
    layout.clear();
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("MissBox_Bonus");
    
    layout.clear();
    PROPERTYI::GetPropertyFloatList("WMS_Label2", layout);
    nPoxX = nStartX + layout[0];
    nPoxY = nStartY + layout[1];
    strLabel->Initialize(0, nPoxX, nPoxY, layout[2], layout[3], lstImage,0.f,0.f,1.f,1.f);
    strLabel->SetTextColor(fWhite);
    AddControl(strLabel);
    
    mpBonus = new CNumberCtl(this,mpTextureMan);
    layout.clear();
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    

    layout.clear();
    PROPERTYI::GetPropertyFloatList("WMS_Bonus", layout);
    nPoxX = nStartX + layout[0];
    nPoxY = nStartY + layout[1];
    mpBonus->Initialize(10, nPoxX, nPoxY, layout[2], layout[3], layout[4], layout[5], layout[6],lstImage,true);
    mpBonus->SetTextColor(fWhite);
    mpBonus->SetAlign(CNumberCtl::ALIGN_LEFT);
    mpBonus->SetNumber(0);
    AddControl(mpBonus);
    
    //Gold
    strLabel = new CLabelCtl(this,mpTextureMan);
    layout.clear();
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("MissBox_Gold");
    layout.clear();
    PROPERTYI::GetPropertyFloatList("WMS_Label3", layout);
    nPoxX = nStartX + layout[0];
    nPoxY = nStartY + layout[1];
    strLabel->Initialize(0, nPoxX, nPoxY, layout[2], layout[3], lstImage,0.f,0.f,1.f,1.f);
    strLabel->SetTextColor(fWhite);
    AddControl(strLabel);
    
    mpGold = new CNumberCtl(this,mpTextureMan);
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    layout.clear();
    PROPERTYI::GetPropertyFloatList("WMS_Gold", layout);
    nPoxX = nStartX + layout[0];
    nPoxY = nStartY + layout[1];
    mpGold->Initialize(10, nPoxX, nPoxY, layout[2], layout[3], layout[4], layout[5], layout[6],lstImage,true);
    mpGold->SetTextColor(fWhite);
    mpGold->SetAlign(CNumberCtl::ALIGN_LEFT);
    mpGold->SetNumber(0);
    AddControl(mpGold);

    //Total
    strLabel = new CLabelCtl(this,mpTextureMan);
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("MissBox_Total");
    layout.clear();
    PROPERTYI::GetPropertyFloatList("WMS_Label4", layout);
    nPoxX = nStartX + layout[0];
    nPoxY = nStartY + layout[1];
    strLabel->Initialize(0, nPoxX, nPoxY, layout[2], layout[3], lstImage,0.f,0.f,1.f,1.f);
    strLabel->SetTextColor(fWhite);
    AddControl(strLabel);


    mpTotal = new CNumberCtl(this,mpTextureMan);
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    layout.clear();
    PROPERTYI::GetPropertyFloatList("WMS_Total", layout);
    nPoxX = nStartX + layout[0];
    nPoxY = nStartY + layout[1];
    mpTotal->Initialize(10, nPoxX, nPoxY, layout[2], layout[3], layout[4], layout[5], layout[6],lstImage,true);
    mpTotal->SetTextColor(fWhite);
    mpTotal->SetAlign(CNumberCtl::ALIGN_LEFT);
    mpTotal->SetNumber(0);
    AddControl(mpTotal);
    
    //Close
    CControl* pCloseBtn = new CButtonCtl(this,mpTextureMan);
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("MissBox_OK");
    layout.clear();
    PROPERTYI::GetPropertyFloatList("WMS_CloseBtn", layout);
    nPoxX = nStartX + layout[0];
    nPoxY = nStartY + layout[1];
    pCloseBtn->Initialize(BTN_PLAY_GoTo_Menu, nPoxX, nPoxY, layout[2], layout[3], lstImage,0.f,0.f,1.f,1.f);
    AddControl(pCloseBtn);
 
    return E_SUCCESS;
}


void CMissionEndCtl::Show(MissionEndCtlType Type,int nScore,int nBonus,bool bTraining)
{
    vector<string> lstImage;
    
    //SetHide(false);
    mType = Type;
    mnStep = 0;
    mbTraining = bTraining;
    
    mnTotalTime = 0;
    mfRenderScore = nScore;
    mfRenderBonus = nBonus;
    mfRenderGold = 0.f;
    mfRenderTotal = mpUserInfo->GetGold();
    
    if(mType == MissionEndCtlType_GoToMenu_Died)
    {
        lstImage.clear();
        lstImage.push_back("none");
        lstImage.push_back("MissBox_Title_E");
        SetBackCoordTex(0.07421875,0.009765625,0.921875,0.490234375);
        mpMissionTitle->SetImageData(lstImage);
        

    }
    else if(mType == MissionEndCtlType_GoToMenu_Successed)
    {   
        lstImage.clear();
        lstImage.push_back("none"); 
        lstImage.push_back("MissBox_Title_S");
        SetBackCoordTex(0.07421875,0.505859375,0.921875,0.982421875);
        mpMissionTitle->SetImageData(lstImage);
    }
    
    mpScore->SetNumber(mfRenderScore);
    mpBonus->SetNumber(mfRenderBonus);
    mpGold->SetNumber(0);
    mpTotal->SetNumber(0);
    
    if(mbTraining == false)
        mpUserInfo->AddTotalScore(nScore + nBonus);
    
    SetAni(CONTROLANI_ZOOM_IN);
    
    //OK을 눌러버리면 계산시간 전에 종료가 되기 때문에 미리 저장소에 반영해준다.
    if(mfRenderScore > 0.0f || mfRenderBonus > 0.0f)
    {
        float fDif = mfRenderScore+mfRenderBonus;
        
        fDif *= mfGoldPerScore;
//        fDif += (fDif * (float)mpUserInfo->GetRank() * 2.f / 100.f); //계급에 의해서 골드를 획득한다.
        
        mpUserInfo->SetGold(mfRenderTotal + fDif);
        if(mType == MissionEndCtlType_GoToMenu_Successed)
        {
            
            //미션 성공했다고 마크를 한다.
            mpUserInfo->AddListToCompletedMap(mpUserInfo->GetLastSelectedMapID());
            
            int nNoCompletedMapIndex = -1;
            int nNoCompletedMapID = mpUserInfo->GetNotCompletedMap(nNoCompletedMapIndex);
            if(nNoCompletedMapID != -1)
            {
                mpUserInfo->SetLastSelectedMapID(nNoCompletedMapID);
                mpUserInfo->SetLastSelectedMapIndex(nNoCompletedMapIndex);
            }
            else
            {
                
                mpUserInfo->UpgradeCompletedRank();
                CScenario::gbNeedCompletedMap = true; //맵을 다 깼으니까...
            }
        }
        mpUserInfo->Save();
    }
    
}

int CMissionEndCtl::RenderBegin(int nTime)
{
    float fDif = 0;
    if (mbHide) return E_SUCCESS;
    CLabelCtl::RenderBegin(nTime);
    mnTotalTime += nTime;
    if(mnStep == 0 && mnTotalTime > 2000) //0.5 초부터 점수를 돈으로 환산한다.
    {
        if(mfRenderScore > 0.f)
        {
            fDif = (float)nTime;
            
            if((mfRenderScore - fDif) <= 0)
                fDif = mfRenderScore;
            
            mfRenderScore -= fDif;
            mpScore->SetNumber(mfRenderScore);
        }
        else if(mfRenderBonus > 0.f)
        {
            fDif = (float)nTime;
            
            if((mfRenderBonus - fDif) <= 0)
                fDif = mfRenderBonus;
            
            mfRenderBonus -= fDif;
            mpBonus->SetNumber(mfRenderBonus);
        }
        else
        {
            mnStep = 1; //종료 단계
            mnTotalTime = 0;
            return E_SUCCESS;
        }
          
        
        fDif *= mfGoldPerScore;
//        fDif += (fDif * (float)mpUserInfo->GetRank() * 2.f / 100.f); //계급에의해서 골드를 획득한다.
        mfRenderGold += fDif;
        mpGold->SetNumber(mfRenderGold);
        
        mfRenderTotal += fDif;
        
        if(mbTraining == false)
            mpTotal->SetNumber(mfRenderTotal);
        
    }
    else if(mnStep == 1 && mnTotalTime > 30000) //30초후에는 자동으로 종료한다.
    {
        mbHide = true;
        CScenario::SendMessage(SGL_MESSAGE_CLICKED,BTN_PLAY_GoTo_Menu);

    }
    else if(mfRenderScore == 0.0f && mfRenderBonus == 0.0f)
    {
        mnStep = 1; //종료 단계
    }
    
    return E_SUCCESS;
}