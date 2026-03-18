//
//  CPopupCtl.h
//  SongGL
//
//  Created by 송 호학 on 12. 2. 9..
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef SongGL_CPopupCtl_h
#define SongGL_CPopupCtl_h
#include "CLabelCtl.h"

class CPopupCtl : public CLabelCtl 
{
public:
    CPopupCtl(CControl* pParent,CTextureMan *pTextureMan);
    virtual ~CPopupCtl();
    virtual int Initialize(int nControlID,float nWidth,float nHeight,vector<string>& lstImage,float fsu,float fsv,float feu,float fev);    
    void Show();
    void Hide();
    
protected:
    virtual void SetHide(bool bHide);

};

#endif
