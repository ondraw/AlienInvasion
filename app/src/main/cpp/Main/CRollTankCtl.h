//
//  CRollTank.h
//  SongGL
//
//  Created by 송 호학 on 11. 12. 11..
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef SongGL_CRollTank_h
#define SongGL_CRollTank_h
#include "ARollCtl.h"
#include "CPicking.h"
class CModelMan;
class CRollTankCtl : public ARollCtl 
{
public:
    CRollTankCtl(CControl* pParent,
              CTextureMan *pTextureMan,
              bool bRevolvingType = true, //자동문처럼 회전
              float fFar = 4.0f,          //화면에 꽉차게 = 0.0f 화면에서 멀리하면 컨트롤이 작아진다. 700.0f
              float fTourchSesitivity = 0.1f, //fFar = 0.0f이면 0.1f로 하면 되고 700.0f으로 하면 0.4정도가 적당한다.
              int nRollFaceCount = ROLL_FACE_COUNT);
    virtual ~CRollTankCtl();
    
    virtual int LoadResource();
    virtual int RenderBegin(int nTime);
	virtual int Render();
	virtual int RenderEnd();
    virtual void SetLight();
    
    virtual void OnStopMotion();
    
    
    virtual bool IsTabIn(int nIndex,float xWinPos,float yWinPos);
    int Initialize(int nControlID,SPoint *pPosition,SVector *pvDirection);
    int DrawRoll(bool bshadow);
    
#ifdef ANDROID
    virtual void ResetTexture();
#endif
    
protected:
    int Initialize(SPoint *pPosition,SVector *pvDirection);
public:  
    float mfPos[ROLL_FACE_COUNT][3];
    CModelMan* mpModelMan;
    CPicking    mPicking;
    
    int   mnBeforeSelectIndex;
};


#endif
