//
//  CButtonCtl.h
//  SongGL
//
//  Created by 송 호학 on 11. 11. 30..
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef SongGL_CButtonCtl_h
#define SongGL_CButtonCtl_h

#include "CLabelCtl.h"
enum BUTTON_TYPE { 
    BUTTON_NORMAL,      //일반 버튼.
    BUTTON_TOGGLE,      //톨글된다.
    BUTTON_GROUP_TOGGLE //한번 클릭되면 다시 클릭되지 않음.
};

class CButtonCtl : public CLabelCtl
{
public:
    CButtonCtl(CControl* pParent,CTextureMan *pTextureMan);
    CButtonCtl(CControl* pParent,CTextureMan *pTextureMan,BUTTON_TYPE ButtonType);
    virtual ~CButtonCtl();
    virtual CControl* Clone(CControl* pNew = NULL);
    virtual int Initialize(
                               int nControlID,
                               float x, float y, float nWidth,float nHeight,
                               vector<string>& lstImage,
                               float fsu,float fsv,float feu,float fev);
    
    virtual int Initialize(
                           int nControlID,
                           float x, float y, float nWidth,float nHeight,
                           vector<string>& lstImage,
                           float fsu,float fsv,float feu,float fev,
                           float fsu2,float fsv2,float feu2,float fev2
                           );
    virtual void RenderBackgroundPushed();
    
    
    virtual bool OnButtonDown();
    virtual bool OnButtonUp(bool bInSide);
    void Toggle(bool bSendEvent = false);
    
    BUTTON_TYPE GetButtonType() { return mButtonType;}
    void        SetButtonType(BUTTON_TYPE v) { mButtonType = v;}
    
    //BUTTON_GROUP_TOGGLE 의 버튼들을 정리한다.
    void ArrangeToggleButton();
    virtual void FlipVertical(); //위아래 회전.
    
protected:
    BUTTON_TYPE mButtonType;
    
    //BUTTON_GROUP_TOGGLE에서 클릭했을때 원래상태를 기억한다. 이미 클릭되어 있는 상태면 다시 클릭이벤트를 날리지 말자.
    bool        mbOrgPushed;
    float       mfWorldPushedBackCoordTex[8];
    bool        mbPushedTextured; //푸쉬된 버튼이미지가 존재하면 true 아니면 컬러를 사용하여 푸쉬를 보여준다.
};
#endif
