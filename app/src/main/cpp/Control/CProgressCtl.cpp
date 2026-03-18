//
//  CProgressCtl.cpp
//  SongGL
//
//  Created by 송 호학 on 11. 12. 28..
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include "CProgressCtl.h"
#include "sGLTrasform.h"

CProgressCtl::CProgressCtl(CControl* pParent) : CLabelCtl(pParent,NULL)
{
    mCurrentPos = 0;
}


CProgressCtl::~CProgressCtl()
{
    
}


int CProgressCtl::Initialize(int nControlID,float x, float y, float nWidth,float nHeight)
{
    vector<string> lstImage;
    
    lstImage.push_back("none");
    lstImage.push_back("none");
    
    
    int nIndex = 0;
    for (int i = 0; i < 4; i++) 
    {
        nIndex = i*4;
        
        //Red
        *(mfEnergeColor+nIndex) = 0.0;
        *(mfEnergeColor+nIndex+1) = 0x80 / 255.0f;
        *(mfEnergeColor+nIndex+2) = 0x80 / 255.0f;
        *(mfEnergeColor+nIndex+3) = 1.0f;
        
        //Blue
        *(mfRelealEnergeColor+nIndex) = 0.0f;
        *(mfRelealEnergeColor+nIndex+1) = 1.0f;
        *(mfRelealEnergeColor+nIndex+2) = 1.0f;
        *(mfRelealEnergeColor+nIndex+3) = 1.0f; 
    }
    
    
    CControl::Initialize(nControlID,x,y,nWidth,nHeight,lstImage,0.f,0.f,1.f,1.f);
    
    float matrix[16];
    float fHalfWidth = nWidth / 2.0f;
    float fHalfHeight = nHeight / 2.0f;
    
    
    sglLoadIdentityf(matrix);
    
    if(nControlID != -33)
    {
        x += fHalfWidth; 
        y += fHalfHeight;
    }
    else
    {
        x = fHalfWidth;
        y = fHalfHeight;
    }
    
    sglScaleMatrixf(matrix, fHalfWidth, fHalfHeight, 0.0f); 
    for (int i = 0; i < 4; i++) 
    {
        nIndex = i*3;
        sglMultMatrixVectorf(&mfWorldBackVertex[nIndex], matrix,&gPanelVertexY[nIndex]);
        CControl::DtoGL(mfWorldBackVertex[nIndex], mfWorldBackVertex[nIndex+1], &mfWorldBackVertex[nIndex],&mfWorldBackVertex[nIndex+1]);
    }

    CControl::MakeWorldStickVertexD(mfWorldBackVertex,mfWorldBackVertex,x,y);
    CControl::DtoGL(nWidth, &mGLWidth);
    memcpy(mfCurrentProVertex, mfWorldBackVertex, sizeof(mfCurrentProVertex));
    
    return E_SUCCESS;
}


void CProgressCtl::SetPosition(int nCurrentPos)
{
    if(nCurrentPos > 100) 
        nCurrentPos = 100;
    mCurrentPos = nCurrentPos;
}

void CProgressCtl::SetTranslate(float fAlpha)
{
    int nIndex = 0;
    for (int i = 0; i < 4; i++)
    {
        nIndex = i*4;
        
        //Red
        *(mfEnergeColor+nIndex+3) = fAlpha;
        
        //Blue
        *(mfRelealEnergeColor+nIndex+3) = fAlpha;
    }
}

int CProgressCtl::RenderBegin(int nTime)
{
    if(mbHide == false)
    {
        float fRate = mGLWidth * mCurrentPos / 100.0f;
        mfCurrentProVertex[6] = mfCurrentProVertex[0] + fRate;
        mfCurrentProVertex[9] = mfCurrentProVertex[3] + fRate;
    }
    return E_SUCCESS;
}

int CProgressCtl::Render()
{
    if(mbHide == false)
    {
        glDisable(GL_TEXTURE_2D);
        
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        
        glColorPointer(4, GL_FLOAT, 0, mfEnergeColor);
        glVertexPointer(3, GL_FLOAT, 0, mfWorldBackVertex);
        glDrawElements(GL_TRIANGLE_STRIP, gshPanelIndicesCnt, GL_UNSIGNED_SHORT, gPanelIndices);
        
        glVertexPointer(3, GL_FLOAT, 0, mfCurrentProVertex);
        glColorPointer(4, GL_FLOAT, 0, mfRelealEnergeColor);
        glDrawElements(GL_TRIANGLE_STRIP, gshPanelIndicesCnt, GL_UNSIGNED_SHORT, gPanelIndices);

        
        glDisableClientState(GL_COLOR_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glEnable(GL_TEXTURE_2D);
    }
    
    return E_SUCCESS;
}


int CProgressCtl::RenderEnd()
{
    return E_SUCCESS;
}
