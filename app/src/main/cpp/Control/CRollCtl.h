//
//  CRollCtl.h
//  SongGL
//
//  Created by Song Hohak on 11. 10. 15..
//  Copyright 2011 thinkm. All rights reserved.
//

#ifndef SongGL_CRollCtl_h
#define SongGL_CRollCtl_h
#include "ARollCtl.h"


class CMS3DASCII;
class CRollCtl : public ARollCtl
{
public:
    CRollCtl(CControl* pParent,
             CTextureMan *pTextureMan,
             bool bRevolvingType = true, //자동문처럼 회전
             float fFar = 4.0f,          //화면에 꽉차게 = 0.0f 화면에서 멀리하면 컨트롤이 작아진다. 700.0f
             float fTourchSesitivity = 0.1f, //fFar = 0.0f이면 0.1f로 하면 되고 700.0f으로 하면 0.4정도가 적당한다.
             int nRollFaceCount = ROLL_FACE_COUNT); //면의 갯수 현재 면의 갯수는 10개이다.
    
    virtual ~CRollCtl();
    int LoadResource();
    int Initialize(int nControlID,SPoint *pPosition,SVector *pvDirection);
    
//ARender
	virtual int RenderBegin(int nTime);
	virtual int Render();
	virtual int RenderEnd();	
    virtual int OnEvent(SGLEvent *pEvent);
    virtual int CloseResource();
    virtual bool IsTabIn(int nIndex,float xWinPos,float yWinPos);
    
protected:
    virtual int Initialize(SPoint *pPosition,SVector *pvDirection);
    int DrawRoll(bool bshadow);
    void DrawFloor(bool bReflactor);
   
   
protected:
    CMS3DASCII  *mpModel;
    CMS3DASCII  *mpModelBottom;
    CPicking    mPicking;
};

#endif
