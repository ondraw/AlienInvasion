//
//  CScrollCtl.cpp
//  SongGL
//
//  Created by 송 호학 on 11. 12. 2..
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//
#include "CScrollCtl.h"
#include "sGL.h"
#include "sGLTrasform.h"
#include "CScrollContentCtl.h"
#include "CScrollBarCtl.h"

CScrollCtl::CScrollCtl(CControl* pParent,CTextureMan *pTextureMan) : CControl(pParent,pTextureMan)
{   
    mnBackID = 0;
    
    mbHasVerScroll = false;
    mbHasHorScroll = false;
    
    CControl *Content = new CScrollContentCtl(this,mpTextureMan);
    
    mlstChild.add(Content);
    
    CScrollBarCtl *VSc = new CScrollBarCtl(this,mpTextureMan);
    mlstChild.add(VSc);

    
    CScrollBarCtl *HSc = new CScrollBarCtl(this,mpTextureMan);
    mlstChild.add(HSc);
    
    mfVScrollPos = 0.0f;
    mfHScrollPos = 0.0f;
}

CScrollCtl::~CScrollCtl()
{
    
}

//listImage : 0: 백그라운드 이미지
//            1: 컨텍츠 백그라운드 이미지.
//listModel : 0: 읽어올 모델.
int CScrollCtl::Initialize(int nControlID,float x, float y, float nWidth,float nHeight,vector<string>& lstImage,float fsu,float fsv,float feu,float fev)
{
    CControl::Initialize(nControlID,x,y,nWidth,nHeight,lstImage,fsu,fsv,feu,fev);
    
    float matrix[16];
    int nIndex = 0;
    float fHalfWidth = nWidth / 2.0f;
    float fHalfHeight = nHeight / 2.0f;
    sglLoadIdentityf(matrix);
    
    x += fHalfWidth; 
    y += fHalfHeight;
    
    //sglRotateRzRyRxMatrixf(matrix, 0.0f, 0.0f, 180.0f);
    sglScaleMatrixf(matrix, fHalfWidth, fHalfHeight, 0.0f); 
    
    for (int i = 0; i < 4; i++) 
    {
        nIndex = i*3;
        sglMultMatrixVectorf(&mfWorldBackVertex[nIndex], matrix,&gPanelVertexY[nIndex]);
        CControl::DtoGL(mfWorldBackVertex[nIndex], mfWorldBackVertex[nIndex+1], &mfWorldBackVertex[nIndex],&mfWorldBackVertex[nIndex+1]);
    }
    
    CControl::MakeWorldStickVertexD(mfWorldBackVertex,mfWorldBackVertex,x,y);
    
    
    //lstImage,lstModel를 재정의해야한다.
    lstImage.clear();
    lstImage.push_back("scrollbar.png");
    lstImage.push_back("none");
    ((CScrollBarCtl*)mlstChild[1])->Initialize(SCROLLBAR_VER, mlstChild[0], mfDPosX + nWidth - SCROLL_WIDTH, mfDPosY, lstImage,0.f,0.f,1.f,1.f);
    ((CScrollBarCtl*)mlstChild[2])->Initialize(SCROLLBAR_HOR, mlstChild[0], mfDPosX, mfDPosY + nHeight - SCROLL_WIDTH, lstImage,0.f,0.f,1.f,1.f);
    return E_SUCCESS;
}


void CScrollCtl::SetImageData(vector<string>& lstImage)
{
    if(lstImage.size() < 2) 
    {
        HLog("[Error] CScrollCtl");
    }
    
    if(strcmp(lstImage[0].c_str(), "none") != 0)
        mnBackID = mpTextureMan->GetTextureID(lstImage[0].c_str());
    
    if (strcmp(lstImage[1].c_str(),"none") != 0) 
    {
        mfContentsBackgroundImg = lstImage[1];
    }
    
#if defined(ANDROID) || defined(MAC)
    CControl::SetImageData(lstImage);
#endif
}

void CScrollCtl::AddControl(CControl* pControl)
{
    return ; //지원을 하지 않음.
}

void CScrollCtl::InsertControl(int nIndex,CControl* pControl)
{
    return ; //지원을 하지 않음.
}


int CScrollCtl::RenderBegin(int nTime)
{
    if (mbHide) return E_SUCCESS; //그리지 말자...
    
    return CControl::RenderBegin(nTime);
}

int CScrollCtl::Render()
{
    if (mbHide) return E_SUCCESS; //그리지 말자...

    //다른 스텐실을 사용했다면 스텐실 버퍼를 클리어한다.
    glClear(GL_STENCIL_BUFFER_BIT);

    if(mnBackID)
    {        
        //1>어떠한 색깔도 그리지 말게 하자.
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        glDepthMask(GL_FALSE);
        
        //2>스텐실을 활성화한다
        glEnable(GL_STENCIL_TEST);
        
        //3>실패나 zbuffer실패일경우는 그대로 두고 성공하였을 경우에만 1로 변경한다.
        glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);     
        
        //4>모든것을 1로 설정되어있기때문에 인제 부터 그리는 부분만은 버퍼에 1로 채워진다.
        glStencilFunc(GL_ALWAYS, 1, 0xffffff00); 
        
        //-------------------------------------------------
        //스텐실 버퍼에 마루를 그린다.
        //-------------------------------------------------
        glBindTexture(GL_TEXTURE_2D, mnBackID);
        glVertexPointer(3, GL_FLOAT, 0, mfWorldBackVertex);
        glTexCoordPointer(2, GL_FLOAT, 0, gPanelCoordTex);
        glDrawElements(GL_TRIANGLE_STRIP, gshPanelIndicesCnt, GL_UNSIGNED_SHORT, gPanelIndices);
        //-------------------------------------------------
        
        //4>지금부터 그리는 것은 Color Buffer에 그린다.
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); 
        glDepthMask(GL_TRUE);
        
        //5>
        glStencilFunc(GL_EQUAL, 1, 0xffffffff);  
        //6>이미 저장되어 있는 값은 변경하지 말자...
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    }
    
//    glDisable(GL_DEPTH_TEST); //disable depth testing of the
    CControl::Render();
//    glEnable(GL_DEPTH_TEST);

    //7>스텐실을 종료한다.
    glDisable(GL_STENCIL_TEST);
    return true;
}

int CScrollCtl::RenderEnd()
{
    if (mbHide) return E_SUCCESS; //그리지 말자...
    
    return CControl::RenderEnd();
}

bool CScrollCtl::OnButtonDown()
{
    //어떠한 이벤트도 날리지 않음.
    return true;
}

bool CScrollCtl::OnButtonUp(bool bInSide)
{
    //어떠한 이벤트도 날리지 않음.
    return true;
}

bool CScrollCtl::OnDragInvalidate(float fDVectorX, float fDVectorY)
{
    SetScrollVector(fDVectorX, fDVectorY);
    return true;
}

void CScrollCtl::SetContentsBounds(int nWidth,int nHeight)
{
    vector<string> lstImage;
    vector<string> lstModel;
    
    if(mfContentsBackgroundImg.length() != 0)
        lstImage.push_back(mfContentsBackgroundImg);
    else
        lstImage.push_back("none");
    
    lstImage.push_back("none"); //글자.
    mlstChild[0]->Initialize((long)this, mfDPosX, mfDPosY, nWidth, nHeight, lstImage,0.f,0.f,1.f,1.f);
    ResetScroll();
}


//수직,수평 스크롤이 존재하는지 계산한다.
void CScrollCtl::ResetScroll()
{
    mbHasVerScroll = false;
    mbHasHorScroll = false;

    float fContentWidth,fContentHeight;
    mlstChild[0]->GetSize(fContentWidth, fContentHeight);
    if(fContentHeight > mfDHeight) 
        mbHasVerScroll = true;
    if(fContentWidth > mfDWidth) 
        mbHasHorScroll = true;
}


void CScrollCtl::SetScrollVector(float fDVectorX,float fDVectorY)
{
    float fContentX,fContentY;
    float fContentWidth,fContentHeight;
    mlstChild[0]->GetPosition(fContentX, fContentY);
    mlstChild[0]->GetSize(fContentWidth, fContentHeight);
    
    //부모위치보다 이상로 이동못한다.
    if((fContentX + fDVectorX) > mfDPosX)
        fDVectorX = mfDPosX - fContentX;
    
    //부모위치보다 이상로 이동못한다.
    if((fContentY + fDVectorY) > mfDPosY)
        fDVectorY = mfDPosY - fContentY;
    
    float fContentRight = fContentX + fContentWidth;
    float fDisplayRight = mfDPosX + mfDWidth;
    if((fContentRight + fDVectorX) < fDisplayRight)
        fDVectorX = fDisplayRight - fContentRight;

    
    float fContentBottom = fContentY + fContentHeight;
    float fDisplayBottom = mfDPosY + mfDHeight;
    if((fContentBottom + fDVectorY) < fDisplayBottom)
        fDVectorY = fDisplayBottom - fContentBottom;
    
    if(fDVectorX == 0.0f && fDVectorY == 0.0f) return;

    mlstChild[0]->MoveVector(fDVectorX, fDVectorY);    
    ((CScrollBarCtl*)mlstChild[1])->SetPos(fDVectorX,fDVectorY);
    ((CScrollBarCtl*)mlstChild[2])->SetPos(fDVectorX,fDVectorY);
    
    mfHScrollPos += fDVectorX;
    mfVScrollPos += fDVectorY;
}



void CScrollCtl::SetScrollVPosition(float fPos)
{
    if(fPos > GetMaxScrollVPosition())
        fPos = GetMaxScrollVPosition();
    
    float fVector = -fPos - mfVScrollPos;   //-fPos반대 항향이다.
    if(fVector == 0.0f) return;
    SetScrollVector(0.0f,fVector);
}

void CScrollCtl::SetScrollHPosition(float fPos)
{
    if(fPos > GetMaxScrollHPosition())
        fPos = GetMaxScrollHPosition();
    
    float fVector = -fPos - mfHScrollPos; //-fPos반대 항향이다.
    if(fVector == 0.0f) return;
    SetScrollVector(fVector,0.0f);
}

void CScrollCtl::SetScrollHVPosition(float fPosX,float fPosY)
{
    float fVectorX = fPosX - mfHScrollPos;
    float fVectorY = fPosY - mfVScrollPos;
    if(fVectorX == 0.0f && fVectorY == 0.0f) return;
    SetScrollVector(fVectorX,fVectorY);
}

float CScrollCtl::GetMaxScrollVPosition()
{
    float fContentWidth,fContentHeight;
    mlstChild[0]->GetSize(fContentWidth, fContentHeight);
    return fContentHeight - mfDHeight;
}

float CScrollCtl::GetMaxScrollHPosition()
{
    float fContentWidth,fContentHeight;
    mlstChild[0]->GetSize(fContentWidth, fContentHeight);

    return fContentWidth - mfDWidth;
}