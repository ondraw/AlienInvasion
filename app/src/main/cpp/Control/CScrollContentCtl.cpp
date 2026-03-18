//
//  CScrollContentCtl.cpp
//  SongGL
//
//  Created by 송 호학 on 11. 12. 2..
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include "CScrollContentCtl.h"
#include "CScenario.h"
#include "sGLUtils.h"
#include "CScrollCtl.h"
extern unsigned int mEscapeT;
CScrollContentCtl::CScrollContentCtl(CControl* pParent,CTextureMan *pTextureMan) : CLabelCtl(pParent,pTextureMan)
{   
    mnTextID = 0;
    mbRowSelMode = true;
    SetCanDrag(true);

}

CScrollContentCtl::~CScrollContentCtl()
{
}

bool CScrollContentCtl::OnButtonMove(float fVectorX,float fVectorY)
{
    CScenario::SendMessage(SGL_MESSAGE_DRAG,(long)mParent,(long)fVectorX,(long)fVectorY,(long)mParent);
    return true;
}

int CScrollContentCtl::RenderBegin(int nTime)
{
    CControl::RenderBegin(nTime); //모션의 벡터를 시간으로 정리한다.
    
    
    CScrollCtl* pParent = (CScrollCtl*)mParent;
    if(mlTouchID == -1 && (mAccelatorVector[0] != 0 || mAccelatorVector[1] != 0))
        pParent->SetScrollVector(mAccelatorVector[0], mAccelatorVector[1]);
    
    //스크롤을 보이게 
    if(mbTouchMoving == true || (mAccelatorVector[0] != 0 || mAccelatorVector[1] != 0))
    {
        CList<CControl*>* childs = pParent->GetChilds();
        if(pParent->IsVScroll())
            childs->get(1)->SetHide(false);
        if(pParent->IsHScroll())
            childs->get(2)->SetHide(false);
            
    }
    //스크롤을 안보이게 한다.
    else
    {
        CList<CControl*>* childs = pParent->GetChilds();
        if(pParent->IsVScroll())
            childs->get(1)->SetHide(true);
        if(pParent->IsHScroll())
            childs->get(2)->SetHide(true);
        
    }
    return true;
}

int CScrollContentCtl::Render()
{
    //배경이 존재하면 배경을 바인드하여 먼저그려준다.
    if(mnBackID)
        glBindTexture(GL_TEXTURE_2D, mnBackID);
    
    glVertexPointer(3, GL_FLOAT, 0, mfWorldBackVertex);
    if(mnBackID)
    {

        glColor4f(mfBackColor[0], mfBackColor[1], mfBackColor[2], mfBackColor[3]);        
        glTexCoordPointer(2, GL_FLOAT, 0, gPanelCoordTex);
        glDrawElements(GL_TRIANGLE_STRIP, gshPanelIndicesCnt, GL_UNSIGNED_SHORT, gPanelIndices);
    }
    
    if(mnTextID)
    {
        glColor4f(1.0f, 1.0f, 1.0f, 0.3f);
        //글자를 바인드하여 그려준다.
        glTexCoordPointer(2, GL_FLOAT, 0, mfWorldTextCoordTex);
        glBindTexture(GL_TEXTURE_2D, mnTextID);
        glDrawElements(GL_TRIANGLE_STRIP, gshPanelIndicesCnt, GL_UNSIGNED_SHORT, gPanelIndices);
    }
    
    
    float fPosX,fPosY,fWidth,fHeight;
    float fParentPosX,fParentPosY,fParentWidth,fParentHeight;
    
    mParent->GetSize(fParentWidth, fParentHeight);
    mParent->GetPosition(fParentPosX, fParentPosY);
    float fTotalHeight = fParentPosY + fParentHeight;
    
    //안보이는 부분의 셀은 그리지 말자꾸나.
    int nSize = mlstChild.size();
    for (int i = 0; i < nSize; i++) 
    {
        CControl* ChildCon = mlstChild[i];
        
        ChildCon->GetPosition(fPosX, fPosY);
        ChildCon->GetSize(fWidth, fHeight);
        
        //퍼포먼스를 위해서 안그려도 되는 부분은 그리지 말자
        //단 가로의 계산은 하지 말고 세로의 계산만하자 꾸나.
        if(fPosY + fHeight < fParentPosY) continue;
        if(fPosY  > fTotalHeight) continue;
        
        ChildCon->Render();
    }
    
    return E_SUCCESS;

}


bool CScrollContentCtl::MoveTouch(long lTouchID,float x, float y)
{
    CScrollCtl* pParent = (CScrollCtl*)mParent;
    
    if(mlTouchID != -1 && mlTouchID == lTouchID)
    {   
        if(mbCanMove == true && mParent->IsTabIn(x, y))
        {
            float vectorx = x - ptBefore.x;
            if(pParent->IsHScroll() == false)
                vectorx = 0;
            
            float vectory = y - ptBefore.y;
            if(pParent->IsVScroll() == false)
                vectory = 0;
            
            if(vectorx != 0 || vectory != 0)
            {
                if(mbTouchMoving == false)
                {
                    
                    float asbVX = vectorx < 0.0f ? -vectorx : vectorx;
                    float asbVY = vectory < 0.0f ? -vectory : vectory;
                    
                    //3은 너무 움직임에 민감하면 단순 탭을 하기 어렵기때문에 3point가 움직이면 움직인 것으로 생각한다.
                    if(!(asbVX > 3 || asbVY > 3)) return true;
                }
                
                mAccelatorVector[0] += vectorx * mfTourchSensitivity*3 / mEscapeT;
                mAccelatorVector[1] += vectory * mfTourchSensitivity*3 / mEscapeT;
                
                OnButtonMove(vectorx,vectory);
                
                mbTouchMoving = true;
            }
            
            ptBefore.x = x;
            ptBefore.y = y;
            return false;
        }
    }
    
    if(mbTouchMoving) //더이상 이벤트를 날리지 않게 한다.
        return false;
    
    return true; //다른 하위뷰에 이벤트를 넘겨준다.
}





