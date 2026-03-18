//
//  CButtonCtl.cpp
//  SongGL
//
//  Created by 송 호학 on 11. 11. 30..
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include "CButtonCtl.h"
#include "CScenario.h"
#include "sGLUtils.h"

CButtonCtl::CButtonCtl(CControl* pParent,CTextureMan *pTextureMan) : CLabelCtl(pParent,pTextureMan)
{   
    mButtonType = BUTTON_NORMAL;
}

CButtonCtl::CButtonCtl(CControl* pParent,CTextureMan *pTextureMan,BUTTON_TYPE ButtonType) : CLabelCtl(pParent,pTextureMan)
{
    mButtonType = ButtonType;
}

CButtonCtl::~CButtonCtl()
{
    
}

CControl* CButtonCtl::Clone(CControl* pNew)
{
    CButtonCtl* pNew2;
    if(pNew == NULL)
        pNew = new CButtonCtl(GetParent(),GetTextureMan());
    pNew2 = (CButtonCtl*)pNew;
    
    pNew2->mButtonType = mButtonType;
    
    //BUTTON_GROUP_TOGGLE에서 클릭했을때 원래상태를 기억한다. 이미 클릭되어 있는 상태면 다시 클릭이벤트를 날리지 말자.
    pNew2->mbOrgPushed = mbOrgPushed;
    memcpy(pNew2->mfWorldPushedBackCoordTex, mfWorldPushedBackCoordTex, sizeof(mfWorldPushedBackCoordTex));
    pNew2->mbPushedTextured = mbPushedTextured;
    return CLabelCtl::Clone(pNew);
}
int CButtonCtl::Initialize(
                       int nControlID,
                       float x, float y, float nWidth,float nHeight,
                       vector<string>& lstImage,
                       float fsu,float fsv,float feu,float fev)
{
    mbPushedTextured = false;
    return CLabelCtl::Initialize(nControlID, x, y, nWidth, nHeight, lstImage, fsu, fsv, feu, fev);
}


int CButtonCtl::Initialize(
                       int nControlID,
                       float x, float y, float nWidth,float nHeight,
                       vector<string>& lstImage,
                       float fsu,float fsv,float feu,float fev,
                       float fsu2,float fsv2,float feu2,float fev2
                       )
{
    mbPushedTextured = true;
    sglGet2DTextureCoord(fsu2,fsv2,feu2,fev2,mfWorldPushedBackCoordTex);
    return CLabelCtl::Initialize(nControlID, x, y, nWidth, nHeight, lstImage, fsu, fsv, feu, fev);
}

void CButtonCtl::RenderBackgroundPushed()
{
    if(mbPushed == true && mbPushedTextured)
    {
        glColor4f(mfBackColor[0], mfBackColor[1], mfBackColor[2], mfBackColor[3]);
        glBindTexture(GL_TEXTURE_2D, mnBackID);
        glVertexPointer(3, GL_FLOAT, 0, mfWorldBackVertex);
        glTexCoordPointer(2, GL_FLOAT, 0, mfWorldPushedBackCoordTex); //이미지를 바꾼다.
        glDrawElements(GL_TRIANGLE_STRIP, gshPanelIndicesCnt, GL_UNSIGNED_SHORT, gPanelIndices);
    }
    else
    {
        glColor4f(0.9f, 0.9f, 0.9f, 0.5f); //약간 흐리게 한다.
        glBindTexture(GL_TEXTURE_2D, mnBackID);
        glVertexPointer(3, GL_FLOAT, 0, mfWorldBackVertex);
        glTexCoordPointer(2, GL_FLOAT, 0, mfWorldBackCoordTex);
        glDrawElements(GL_TRIANGLE_STRIP, gshPanelIndicesCnt, GL_UNSIGNED_SHORT, gPanelIndices);
    }
}

bool CButtonCtl::OnButtonDown()
{

    if(mButtonType == BUTTON_NORMAL)
    {
        mbPushed = true;
    }
    else if(mButtonType == BUTTON_TOGGLE)
    {
        mbPushed = !mbPushed;
    }    
    else if(mButtonType == BUTTON_GROUP_TOGGLE)
    {
        mbOrgPushed = mbPushed;
        mbPushed = true;
    }
    return true;
}


bool CButtonCtl::OnButtonUp(bool bInSide)
{
    if(mButtonType == BUTTON_NORMAL)
    {
        mbPushed = false;
        
        if(bInSide)
            CScenario::SendMessage(SGL_MESSAGE_CLICKED,mnID,mbPushed,0,(long)this);
       
    }
    else if(mButtonType == BUTTON_TOGGLE) //버튼안쪽이 아니면 다시 원래데로 해준다.
    {
        if(bInSide)
            CScenario::SendMessage(SGL_MESSAGE_CLICKED,mnID,mbPushed,0,(long)this);
        else
            mbPushed = !mbPushed;
    }
    else if(mButtonType == BUTTON_GROUP_TOGGLE)
    {
        if(bInSide && mbOrgPushed == false)
            CScenario::SendMessage(SGL_MESSAGE_CLICKED,mnID,mbPushed,0,(long)this);
        else
            mbPushed = mbOrgPushed;
    }
    return true;
}

void CButtonCtl::Toggle(bool bSendEvent)
{
    mbPushed = !mbPushed;
    if(bSendEvent)
        CScenario::SendMessage(SGL_MESSAGE_CLICKED,mnID,mbPushed,0,(long)this);
    
}


                        
void CButtonCtl::ArrangeToggleButton()
{
    if(GetButtonType() != BUTTON_GROUP_TOGGLE) return;
    CList<CControl*>* Childs = GetParent()->GetChilds();
    int nSize = Childs->size();
    for (int i = 0; i < nSize; i++)
    {
        CButtonCtl* pButton = dynamic_cast<CButtonCtl*>(Childs->get(i));
        if (pButton)
        {
            if(pButton != this && pButton->GetButtonType() == BUTTON_GROUP_TOGGLE && pButton != this)
                pButton->SetPushed(false);
        }
    }
}

void CButtonCtl::FlipVertical() //위아래 회전.
{
    
    float fsu = mfWorldPushedBackCoordTex[0];
    float fsv = mfWorldPushedBackCoordTex[1];
    float feu = mfWorldPushedBackCoordTex[4];
    float fev = mfWorldPushedBackCoordTex[5];

    mfWorldPushedBackCoordTex[0] = fsu;
    mfWorldPushedBackCoordTex[2] = fsu;
    
    mfWorldPushedBackCoordTex[4] = feu;
    mfWorldPushedBackCoordTex[6] = feu;
    
    mfWorldPushedBackCoordTex[3] = fsv;
    mfWorldPushedBackCoordTex[5] = fsv;
    
    mfWorldPushedBackCoordTex[1] = fev;
    mfWorldPushedBackCoordTex[7] = fev;
    CControl::FlipVertical();
}