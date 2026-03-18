//
//  CCellCtl.h
//  SongGL
//
//  Created by 송 호학 on 11. 12. 4..
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef SongGL_CCellCtl_h
#define SongGL_CCellCtl_h
#include "CButtonCtl.h"

class CCellCtl : public CButtonCtl
{
public:
    CCellCtl(CTextureMan *pTextureMan);
    ~CCellCtl();
    virtual CControl* Clone(CControl* pNew = NULL);
    
    virtual int Initialize(float nWidth,float nHeight,vector<string>& lstImage,float fsu,float fsv,float feu,float fev);
    virtual bool BeginTouch(long lTouchID,float x, float y);
    virtual bool EndTouch(long lTouchID,float x, float y);
    virtual void RenderBackgroundPushed();
    virtual bool OnButtonUp(bool bInSide);
    
protected:
    virtual int Initialize(int nControlID,float x, float y, float nWidth,float nHeight,vector<string>& lstImage,float fsu,float fsv,float feu,float fev);
    
#ifdef MAC
    bool SubLastSelControl(CControl* pCtl,float x, float y);
#endif
    
};

#endif
