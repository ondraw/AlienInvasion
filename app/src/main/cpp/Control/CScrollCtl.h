//
//  CScrollCtl.h
//  SongGL
//
//  Created by 송 호학 on 11. 12. 2..
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef SongGL_CScrollCtl_h
#define SongGL_CScrollCtl_h

#include "CControl.h"
class CTextureMan;
class CScrollCtl : public CControl
{
public:
    CScrollCtl(CControl* pParent,CTextureMan *pTextureMan);
    virtual ~CScrollCtl();
    virtual void AddControl(CControl* pControl);
    virtual void InsertControl(int nIndex,CControl* pControl);
    
    virtual int RenderBegin(int nTime);
	virtual int Render();
	virtual int RenderEnd(); 
    
    
    void SetContentsBounds(int nWidth,int nHeight);
    
    virtual bool OnButtonDown();
    virtual bool OnButtonUp(bool bInSide);
    
    //// x ,y : Parent의 상대적 좌표이다.
    //nWidth,nHeight 는 디스플레이 단위의 위치이다.
    virtual int Initialize(int nControlID,float x, float y, float nWidth,float nHeight,vector<string>& lstImage,float fsu,float fsv,float feu,float fev);
    virtual void SetImageData(vector<string>& lstImage);
    
    virtual bool OnDragInvalidate(float fDVectorX,float fDVectorY);
    
    //수직,수평 스크롤이 존재하는지 계산한다.
    void ResetScroll();
    void SetScrollVector(float fDVectorX,float fDVectorY);
    
    bool IsVScroll() { return mbHasVerScroll;}
    bool IsHScroll() { return mbHasHorScroll;}
    
    void SetScrollVPosition(float fPos);
    void SetScrollHPosition(float fPos);
    void SetScrollHVPosition(float fPosX,float fPosY);
    
    float GetScrollVPosition() { return -mfVScrollPos;} //반대방향이다.
    float GetScrollHPosition() { return -mfHScrollPos;} //반대방향이다.
    
    float GetMaxScrollVPosition();
    float GetMaxScrollHPosition();
    
    
public:

    bool                        mbHasVerScroll;
    bool                        mbHasHorScroll;
    string                      mfContentsBackgroundImg;
    
    
    float                       mfVScrollPos;
    float                       mfHScrollPos;
};

#endif
