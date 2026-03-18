//
//  CScrollBarCtl.cpp
//  SongGL
//
//  Created by 송 호학 on 11. 12. 5..
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include "CScrollBarCtl.h"
#include "CScrollCtl.h"

CScrollBarCtl::CScrollBarCtl(CControl* pParent,CTextureMan *pTextureMan) : CLabelCtl(pParent,pTextureMan)
{
    mType = SCROLLBAR_VER;
    mpContentCtl = NULL;

}


CScrollBarCtl::~CScrollBarCtl()
{
    
}

void CScrollBarCtl::SetPos(float fVectorX,float fVectorY)
{

    float mDisplayHeight,mDisplayWidth;
    float mConHeight,mConWidth;

    if(mType == SCROLLBAR_VER)
    {
        mParent->GetSize(mDisplayWidth, mDisplayHeight);
        mpContentCtl->GetSize(mConWidth, mConHeight);
        
        //(mConHeight - mDisplayHeight) : mDisplayHeight = fCurHeight : y
        float yHeight = ((mDisplayHeight  - SCROLL_HEIGHT) * fVectorY) / (mConHeight - mDisplayHeight);
        MoveVector(0.0f, -yHeight);
    }
    else
    {
        mParent->GetSize(mDisplayWidth, mDisplayHeight);
        mpContentCtl->GetSize(mConWidth, mConHeight);
        float yWidth = ((mDisplayWidth  - SCROLL_HEIGHT) * fVectorX) / (mConWidth - mDisplayWidth);
        MoveVector(-yWidth, 0.0f);
    }
}

int CScrollBarCtl::Initialize(SCROLLBAR_TYPE type,CControl* pContentCtl,float x, float y, vector<string>& lstImage,float fsu,float fsv,float feu,float fev)
{
    int nTemp = 0;
    int nWidth = SCROLL_WIDTH;
    int nHeight = SCROLL_HEIGHT;
    mType = type;
    if(mType == SCROLLBAR_HOR)
    {
        nTemp = nWidth;
        nWidth = nHeight;
        nHeight = nTemp;
    }
    mpContentCtl = pContentCtl;
    return Initialize((int)(long)this, x, y, nWidth, nHeight, lstImage,fsu,fsv,feu,fev);
}

int CScrollBarCtl::Initialize(int nControlID,float x, float y, float nWidth,float nHeight,vector<string>& lstImage,float fsu,float fsv,float feu,float fev)
{
    return CLabelCtl::Initialize(nControlID, x, y, nWidth, nHeight, lstImage,fsu,fsv,feu,fev);
}


int CScrollBarCtl::RenderBegin(int nTime)
{
    if(mType == SCROLLBAR_VER && ((CScrollCtl*)mParent)->IsVScroll())
        return CLabelCtl::RenderBegin(nTime);
    else if(mType == SCROLLBAR_HOR && ((CScrollCtl*)mParent)->IsHScroll())
        return CLabelCtl::RenderBegin(nTime);
    return E_SUCCESS;   
    
}
int CScrollBarCtl::Render()
{
    if(mType == SCROLLBAR_VER && ((CScrollCtl*)mParent)->IsVScroll())
        return CLabelCtl::Render();
    else if(mType == SCROLLBAR_HOR && ((CScrollCtl*)mParent)->IsHScroll())
        return CLabelCtl::Render();
    return E_SUCCESS;
    
}

int CScrollBarCtl::RenderEnd()
{
    if(mType == SCROLLBAR_VER && ((CScrollCtl*)mParent)->IsVScroll())
        return CLabelCtl::RenderEnd();
    else if(mType == SCROLLBAR_HOR && ((CScrollCtl*)mParent)->IsHScroll())
        return CLabelCtl::RenderEnd();
    return E_SUCCESS;

}
