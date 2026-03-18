//
//  CIndicator.cpp
//  SongGL
//
//  Created by itsme on 2014. 8. 11..
//
//

#include "CIndicatorCtl.h"



CIndicatorCtl::CIndicatorCtl(CControl* pParent,CTextureMan *pTextureMan): CLabelCtl(pParent,pTextureMan)
{
    mbStop = true;
    mfAngle = 0.f;
}


CIndicatorCtl::~CIndicatorCtl()
{
    
}

int CIndicatorCtl::Initialize(int nControlID,float x, float y, float nWidth,float nHeight,bool bSmallIcon)
{
    vector<string> lstImage;
    lstImage.clear();
    if(bSmallIcon == false)
        lstImage.push_back("WaitL.tga");
    else
        lstImage.push_back("WaitS.tga");
        
    lstImage.push_back("none");
    int nResult = CLabelCtl::Initialize(nControlID, x, y, nWidth, nHeight, lstImage, 0.f, 0.f, 1.f, 1.f);
    SetHide(true);
    SetEnable(false);
    SetTranslate(0.7);
    return nResult;
    
}

void CIndicatorCtl::Start()
{
    mbStop = false;
    SetHide(false);
}


void CIndicatorCtl::Stop()
{
    mbStop = true;
    SetHide(true);
}


int  CIndicatorCtl::RenderBegin(int nTime)
{
    CLabelCtl::RenderBegin(nTime);
    if(!mbStop)
    {
        mfAngle += nTime * 0.36f;
        
        //30단위로 돌려야 이상하게 도라가지 않음 (이미지가 30단위로 나누어져있다.)
        
        int nDiv = (int) (mfAngle / 30.f);
        float fAngle = 30.f * (float)nDiv;
        if(nDiv > 0)
            SetRotationZ(-fAngle);
        
        if(mfAngle > 360.f)
            mfAngle -= 360.f;
    }
    return E_SUCCESS;
}