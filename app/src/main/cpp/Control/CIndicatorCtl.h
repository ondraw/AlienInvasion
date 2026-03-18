//
//  CIndicator.h
//  SongGL
//
//  Created by itsme on 2014. 8. 11..
//
//

#ifndef __SongGL__CIndicatorCtl__
#define __SongGL__CIndicatorCtl__
#include "CLabelCtl.h"

class CIndicatorCtl : public CLabelCtl {
public:
    CIndicatorCtl(CControl* pParent,CTextureMan *pTextureMan);
    virtual ~CIndicatorCtl();
    virtual int  RenderBegin(int nTime);
    int Initialize(int nControlID,float x, float y, float nWidth,float nHeight,bool bSmallIcon = false);//,vector<string>& lstImage,float fsu,float fsv,float feu,float fev);
    void Start();
    void Stop();
    
public:
    
    bool mbStop;
    float mfAngle;
    
};

#endif /* defined(__SongGL__CIndicator__) */
