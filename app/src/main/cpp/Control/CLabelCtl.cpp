//
//  CLabelCtl.cpp
//  SongGL
//
//  Created by 송 호학 on 11. 11. 30..
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include "CLabelCtl.h"
#include "sGL.h"
#include "sGLTrasform.h"


CLabelCtl::CLabelCtl(CControl* pParent,CTextureMan *pTextureMan) : CControl(pParent,pTextureMan)
{   
    mnTextID = 0;
    mnPushedTextID = 0;
    mfWorldPushedTextCoordTex = NULL;
}

CLabelCtl::~CLabelCtl()
{
    if(mfWorldPushedTextCoordTex)
    {
        delete [] mfWorldPushedTextCoordTex;
        mfWorldPushedTextCoordTex = NULL;
    }
    
}

CControl* CLabelCtl::Clone(CControl* pNew)
{
    if(pNew == NULL)
        pNew = new CLabelCtl(GetParent(),GetTextureMan());
    
    CLabelCtl* pNew2 = (CLabelCtl*)pNew;
    memcpy(pNew2->mfWorldTextCoordTex,mfWorldTextCoordTex,sizeof(mfWorldTextCoordTex));
    if(mfWorldPushedTextCoordTex)
    {
        pNew2->mfWorldPushedTextCoordTex = new float[8];
        memcpy(pNew2->mfWorldPushedTextCoordTex,mfWorldPushedTextCoordTex,sizeof(float)*8);
    }
    pNew2->mnTextID = mnTextID;
    pNew2->mnPushedTextID = mnPushedTextID;
    return CControl::Clone(pNew2);
}
//listImage : 0: 백그라운드 이미지
//            1: 글자 이미지.
//listModel : 0: 읽어올 모델.
int CLabelCtl::Initialize(int nControlID,float x, float y, float nWidth,float nHeight,vector<string>& lstImage,float fsu,float fsv,float feu,float fev)
{
    CControl::Initialize(nControlID,x,y,nWidth,nHeight,lstImage,fsu,fsv,feu,fev);
    
    float matrix[16];
    int nIndex = 0;
    float fHalfWidth = nWidth / 2.0f;
    float fHalfHeight = nHeight / 2.0f;
    
    
    sglLoadIdentityf(matrix);
    
    x += fHalfWidth;
    y += fHalfHeight;

    
    sglScaleMatrixf(matrix, fHalfWidth, fHalfHeight, 0.0f);
    
    for (int i = 0; i < 4; i++)
    {
        nIndex = i*3;
        sglMultMatrixVectorf(&mfWorldBackVertex[nIndex], matrix,&gPanelVertexY[nIndex]);
    
        CControl::DtoGL(mfWorldBackVertex[nIndex], mfWorldBackVertex[nIndex+1], &mfWorldBackVertex[nIndex],&mfWorldBackVertex[nIndex+1]);
    }
    
    CControl::MakeWorldStickVertexD(mfWorldBackVertex,mfWorldBackVertex,x,y);
    
    return E_SUCCESS;
}



int CLabelCtl::RenderBegin(int nTime)
{
    if (mbHide) return E_SUCCESS; //그리지 말자...
    
    return CControl::RenderBegin(nTime);
}

void CLabelCtl::RenderBackgroundPushed()
{
    glColor4f(0.9f, 0.9f, 0.9f, 0.5f); //약간 흐리게 한다.
    glBindTexture(GL_TEXTURE_2D, mnBackID);
    //glVertexPointer(3, GL_FLOAT, 0, mfWorldBackVertex);
    glTexCoordPointer(2, GL_FLOAT, 0, mfWorldBackCoordTex);
    glDrawElements(GL_TRIANGLE_STRIP, gshPanelIndicesCnt, GL_UNSIGNED_SHORT, gPanelIndices);
}


void CLabelCtl::RenderBackgroundNormal()
{
    if(mbEnable == false && mfBackDisalbeColor)
        glColor4f(mfBackDisalbeColor[0], mfBackDisalbeColor[1], mfBackDisalbeColor[2], mfBackDisalbeColor[3]);
    else
        glColor4f(mfBackColor[0], mfBackColor[1], mfBackColor[2], mfBackColor[3]);
    
    glBindTexture(GL_TEXTURE_2D, mnBackID);
    //glVertexPointer(3, GL_FLOAT, 0, mfWorldBackVertex);
    glTexCoordPointer(2, GL_FLOAT, 0, mfWorldBackCoordTex);
    glDrawElements(GL_TRIANGLE_STRIP, gshPanelIndicesCnt, GL_UNSIGNED_SHORT, gPanelIndices);
}



int CLabelCtl::Render()
{
    if (mbHide) return E_SUCCESS; //그리지 말자...
    
    //if(mfbRotationZ == 0.f && mfbRotationY == 0.f)
    if(mfbRotationZ == 0.f)
    {
        //한번만 하자..
        glVertexPointer(3, GL_FLOAT, 0, mfWorldBackVertex);
    }
    else
    {
        float fV[12];
        memcpy(fV, mfWorldBackVertex, sizeof(fV));
        //    1,2
        //    0,3
        float fX = (fV[0] + fV[9]) / 2.0f;
        float fY = (fV[1] + fV[4]) / 2.0f;
        
        fV[0] -= fX;
        fV[1] -= fY;
        
        fV[3] -= fX;
        fV[4] -= fY;
        
        fV[6] -= fX;
        fV[7] -= fY;
        
        fV[9] -= fX;
        fV[10] -= fY;
        
        //회전한다.
        //다시 원래 위치로 이동한다.
        int nIndex = 0;
        float   matrix[16];
        sglLoadIdentityf(matrix);
        sglTranslateMatrixf(matrix, fX, fY, 0.0f);
        if(mfbRotationZ != 0.f)
            sglRotateRzRyRxMatrixf(matrix, 0.f, 0.f, mfbRotationZ);
//        if(mfbRotationY != 0.f)
//            sglRotateRzRyRxMatrixf(matrix, 0.f, mfbRotationY, 0.f);
        
        for (int i = 0; i < 4; i++)
        {
            nIndex = i*3;
            sglMultMatrixVectorf(&fV[nIndex], matrix,&fV[nIndex]);
        }
        //한번만 하자..
        glVertexPointer(3, GL_FLOAT, 0, fV);
    }
    
    

    //배경이 존재하면 배경을 바인드하여 먼저그려준다.
    if(mnBackID)
    {
        if(mbPushed == false)
        {
            RenderBackgroundNormal();
        }
        else
        {
            RenderBackgroundPushed();
        }
    }
    
    if(mnContentsID)
    {

        if(mbEnable == false && mfBackDisalbeColor)
            glColor4f(mfBackDisalbeColor[0], mfBackDisalbeColor[1], mfBackDisalbeColor[2], mfBackDisalbeColor[3]);
        else
            glColor4f(mfBackColor[0], mfBackColor[1], mfBackColor[2], mfBackColor[3]);
            
        glBindTexture(GL_TEXTURE_2D, mnContentsID);
        //glVertexPointer(3, GL_FLOAT, 0, mfWorldBackVertex);
        glTexCoordPointer(2, GL_FLOAT, 0, mfWorldContentsCoordTex);
        glDrawElements(GL_TRIANGLE_STRIP, gshPanelIndicesCnt, GL_UNSIGNED_SHORT, gPanelIndices);
    }
    
    
    if(mnTextID && mnPushedTextID == 0)
    {
        glColor4f(mfTextColor[0], mfTextColor[1], mfTextColor[2], mfTextColor[3]);
        glBindTexture(GL_TEXTURE_2D, mnTextID);
        //glVertexPointer(3, GL_FLOAT, 0, mfWorldBackVertex);
        //글자를 바인드하여 그려준다.
        glTexCoordPointer(2, GL_FLOAT, 0, mfWorldTextCoordTex);
        glDrawElements(GL_TRIANGLE_STRIP, gshPanelIndicesCnt, GL_UNSIGNED_SHORT, gPanelIndices);
    }
    else if(mnTextID && mnPushedTextID)
    {
        if(mbPushed == false)
        {
            glColor4f(mfTextColor[0], mfTextColor[1], mfTextColor[2], mfTextColor[3]);
            glBindTexture(GL_TEXTURE_2D, mnTextID);
            //glVertexPointer(3, GL_FLOAT, 0, mfWorldBackVertex);
            //글자를 바인드하여 그려준다.
            glTexCoordPointer(2, GL_FLOAT, 0, mfWorldTextCoordTex);
            glDrawElements(GL_TRIANGLE_STRIP, gshPanelIndicesCnt, GL_UNSIGNED_SHORT, gPanelIndices);
        }
        else
        {
            glColor4f(mfTextColor[0], mfTextColor[1], mfTextColor[2], mfTextColor[3]);
            glBindTexture(GL_TEXTURE_2D, mnPushedTextID);
            //glVertexPointer(3, GL_FLOAT, 0, mfWorldBackVertex);
            //글자를 바인드하여 그려준다.
            glTexCoordPointer(2, GL_FLOAT, 0, mfWorldPushedTextCoordTex);
            glDrawElements(GL_TRIANGLE_STRIP, gshPanelIndicesCnt, GL_UNSIGNED_SHORT, gPanelIndices);
        }
    }
    
    return CControl::Render();
}

int CLabelCtl::RenderEnd()
{
    if (mbHide) return E_SUCCESS; //그리지 말자...
    return CControl::RenderEnd();
}

bool CLabelCtl::OnButtonDown()
{
    //어떠한 이벤트도 날리지 않음.
    return true;
}

bool CLabelCtl::OnButtonUp(bool bInSide)
{
    //어떠한 이벤트도 날리지 않음.
    return true;
}


void CLabelCtl::SetImageData(vector<string>& lstImage)
{
    
    if(lstImage.size() < 2) 
    {
        HLogE("[Error] CLabelCtl");
        return;
    }
    
    mnBackID = 0;
    if(strcmp(lstImage[0].c_str(), "none") != 0)
    {
        mnBackID = mpTextureMan->MakeBack(lstImage[0].c_str());
    }

    
    mnTextID = 0;
    float fU = 0.0f;
    float fV = 0.0f;    
    if(strcmp(lstImage[1].c_str(), "none") != 0)
    {
        mnTextID = mpTextureMan->MakeText(lstImage[1].c_str(), fU, fV);
    }
    
    memcpy(mfWorldTextCoordTex, gPanelCoordTex, sizeof(gPanelCoordTex));
    for (int i = 0; i < 8; ) 
    {
        mfWorldTextCoordTex[i++] *= fU;
        mfWorldTextCoordTex[i] =  (1.0f - mfWorldTextCoordTex[i]) * fV;
        i++;
    }
    
    if(lstImage.size() > 2)
    {
        mnPushedTextID = 0;
        fU = 0.0f;
        fV = 0.0f;
        if(strcmp(lstImage[2].c_str(), "none") != 0)
        {
            mnPushedTextID = mpTextureMan->MakeText(lstImage[2].c_str(), fU, fV);
        }
        
        if(mfWorldPushedTextCoordTex) delete [] mfWorldPushedTextCoordTex;
        mfWorldPushedTextCoordTex = new float[8];
        memcpy(mfWorldPushedTextCoordTex, gPanelCoordTex, sizeof(gPanelCoordTex));
        for (int i = 0; i < 8; )
        {
            mfWorldPushedTextCoordTex[i++] *= fU;
            mfWorldPushedTextCoordTex[i] =  (1.0f - mfWorldPushedTextCoordTex[i]) * fV;
            i++;
        }
    }
    
#if defined(ANDROID) || defined(MAC)
    CControl::SetImageData(lstImage);
#endif
}



void CLabelCtl::SetContentsData(const char* sContentsImage)
{
    CControl::SetContentsData(sContentsImage);
    if(sContentsImage == NULL) return;
    mnContentsID = mpTextureMan->GetTextureID(sContentsImage);
}
