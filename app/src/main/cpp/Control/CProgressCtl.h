//
//  CProgressCtl.h
//  SongGL
//
//  Created by 송 호학 on 11. 12. 28..
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef SongGL_CProgressCtl_h
#define SongGL_CProgressCtl_h
#include "CLabelCtl.h"
class CProgressCtl : public CLabelCtl
{
public:
    CProgressCtl(CControl* pParent);
    virtual ~CProgressCtl();
    
    
    // x ,y : Parent의 상대적 좌표이다.
    virtual int Initialize(int nControlID,float x, float y, float nWidth,float nHeight);
	virtual int RenderBegin(int nTime);
	virtual int Render();
	virtual int RenderEnd();	
    virtual void SetTranslate(float fAlpha);
    //0~100사이이다.
    void SetPosition(int nCurrentPos);
    int  GetPosition() { return mCurrentPos;}

protected:    
    float                       mfEnergeColor[16];
    float                       mfRelealEnergeColor[16];
    float                       mfCurrentProVertex[12];
    float                       mGLWidth;
    int                         mCurrentPos;

};


#endif
