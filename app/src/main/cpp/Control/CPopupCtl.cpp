//
//  CPopupCtl.cpp
//  SongGL
//
//  Created by 송 호학 on 12. 2. 9..
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "CPopupCtl.h"
#include "sGL.h"


CPopupCtl::CPopupCtl(CControl* pParent,CTextureMan *pTextureMan) : CLabelCtl(pParent,pTextureMan)
{   
}

CPopupCtl::~CPopupCtl()
{
}


int CPopupCtl::Initialize(int nControlID,float nWidth,float nHeight,vector<string>& lstImage,float fsu,float fsv,float feu,float fev)
{
    int x = (gDisplayWidth - nWidth) / 2.0f;
    int y = (gDisplayHeight - nHeight) / 2.0f;
    
    int nResult = CLabelCtl::Initialize(nControlID, x, y, nWidth, nHeight, lstImage,fsu,fsv,feu,fev);
    SetHide(true);
    return nResult;
}

void CPopupCtl::Show()
{
    if(Animating()) return;
    SetAni(CONTROLANI_ZOOM_IN);
}

void CPopupCtl::Hide()
{
    if(Animating()) return;
    SetAni(CONTROLANI_ZOOM_OUT);
}

void CPopupCtl::SetHide(bool bHide)
{
    CLabelCtl::SetHide(bHide);
}