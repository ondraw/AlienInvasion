//
//  CProgressCtl.cpp
//  SongGL
//
//  Created by 송 호학 on 11. 12. 28..
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include "CImgProgressCtl.h"
#include "sGLTrasform.h"

CImgProgressCtl::CImgProgressCtl(CControl* pParent,CTextureMan *pTextureMan) : CButtonCtl(pParent,pTextureMan)
{
    mButtonType = BUTTON_NORMAL;
    mCurrentPos = 0;
    mBeforeCurrentPos = -1;
    
}


CImgProgressCtl::~CImgProgressCtl()
{
    
}

CControl* CImgProgressCtl::Clone(CControl* pNew)
{
    if(pNew == NULL)
        pNew = new CImgProgressCtl(GetParent(),GetTextureMan());
    
    CImgProgressCtl* pNew2 = (CImgProgressCtl*)pNew;
    
    memcpy(pNew2->mfCurrentProVertex,mfCurrentProVertex,sizeof(mfCurrentProVertex));
    pNew2->mGLWidth = mGLWidth;
    pNew2->mCurrentPos = mCurrentPos;
    pNew2->mBeforeCurrentPos = mBeforeCurrentPos;
    return CButtonCtl::Clone(pNew2);
}

int CImgProgressCtl::Initialize(
                           int nControlID,
                           float x, float y, float nWidth,float nHeight,
                           vector<string>& lstImage,
                           float fsu,float fsv,float feu,float fev)
{
    CButtonCtl::Initialize(nControlID, x, y, nWidth, nHeight, lstImage, fsu, fsv, feu,fev);
    SetEnable(false);
    
    float matrix[16];
    float fHalfWidth = nWidth / 2.0f;
    float fHalfHeight = nHeight / 2.0f;
    int nIndex = 0;
    
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
    CControl::DtoGL(nWidth, &mGLWidth);
    return E_SUCCESS;

}


int CImgProgressCtl::Initialize(
                           int nControlID,
                           float x, float y, float nWidth,float nHeight,
                           vector<string>& lstImage,
                           float fsu,float fsv,float feu,float fev,
                           float fsu2,float fsv2,float feu2,float fev2
                           )
{
    CButtonCtl::Initialize(nControlID, x, y, nWidth, nHeight, lstImage, fsu, fsv, feu, fev,fsu2, fsv2, feu2, fev2);
    SetEnable(false);
    
    float matrix[16];
    float fHalfWidth = nWidth / 2.0f;
    float fHalfHeight = nHeight / 2.0f;
    int nIndex = 0;
    
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
    CControl::DtoGL(nWidth, &mGLWidth);
    return E_SUCCESS;
}



void CImgProgressCtl::SetPosition(int nCurrentPos)
{
    if(nCurrentPos > 100) 
        nCurrentPos = 100;
    mCurrentPos = nCurrentPos;
}

int CImgProgressCtl::RenderBegin(int nTime)
{
    //위치가 변경되거나 애니메이션중이라면....
    if(mBeforeCurrentPos != mCurrentPos || Animating() != CONTROLANI_NONE)
    {
        float fRate = mGLWidth * mCurrentPos / 100.0f;
        memcpy(mfCurrentProVertex, mfWorldBackVertex, sizeof(mfWorldBackVertex));
        mfCurrentProVertex[6] = mfCurrentProVertex[0] + fRate;
        mfCurrentProVertex[9] = mfCurrentProVertex[3] + fRate;
    }
    return E_SUCCESS;
}

int CImgProgressCtl::Render()
{
    glColor4f(mfBackColor[0], mfBackColor[1], mfBackColor[2], mfBackColor[3]);
    glBindTexture(GL_TEXTURE_2D, mnBackID);
    
    if(mbPushedTextured == true)
    {
        glVertexPointer(3, GL_FLOAT, 0, mfWorldBackVertex);
        glTexCoordPointer(2, GL_FLOAT, 0, mfWorldPushedBackCoordTex); //이미지를 바꾼다.
        glDrawElements(GL_TRIANGLE_STRIP, gshPanelIndicesCnt, GL_UNSIGNED_SHORT, gPanelIndices);
    }
    
    glVertexPointer(3, GL_FLOAT, 0, mfCurrentProVertex);
    glTexCoordPointer(2, GL_FLOAT, 0, mfWorldBackCoordTex);
    glDrawElements(GL_TRIANGLE_STRIP, gshPanelIndicesCnt, GL_UNSIGNED_SHORT, gPanelIndices);
    
    
    return E_SUCCESS;
}


int CImgProgressCtl::RenderEnd()
{
    return E_SUCCESS;
}
