//
//  CLabelCtl.h
//  SongGL
//
//  Created by 송 호학 on 11. 11. 30..
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef SongGL_CLabelCtl_h
#define SongGL_CLabelCtl_h
#include "CControl.h"

class CTextureMan;
class CLabelCtl : public CControl
{
public:
    CLabelCtl(CControl* pParent,CTextureMan *pTextureMan);
    virtual ~CLabelCtl();
    
    virtual CControl* Clone(CControl* pNew = NULL);
    
    //// x ,y : Parent의 상대적 좌표이다.
    //nWidth,nHeight 는 디스플레이 단위의 위치이다.
    virtual int Initialize(
                           int nControlID,
                           float x, float y, float nWidth,float nHeight,
                           vector<string>& lstImage,
                           float fsu,float fsv,float feu,float fev);
   
    
 	virtual int  RenderBegin(int nTime);
	virtual int  Render();
    virtual void RenderBackgroundPushed();
    virtual void RenderBackgroundNormal();
	virtual int  RenderEnd();   
    

    virtual bool OnButtonDown();
    virtual bool OnButtonUp(bool bInSide);
    
    virtual void SetImageData(vector<string>& lstImage);
    virtual void SetContentsData(const char* sContentsImage);
    virtual float* GetBounds() {return mfWorldBackVertex;}
    
protected:
    float                       mfWorldTextCoordTex[8];
    float                       *mfWorldPushedTextCoordTex;;
//    float                       mfWorldBackCoordTex[8];
    GLuint                      mnTextID;
    GLuint                      mnPushedTextID;
};

#endif
