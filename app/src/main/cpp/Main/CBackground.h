//
//  CBackground.h
//  SongGL
//
//  Created by Song Hohak on 11. 10. 7..
//  Copyright 2011 thinkm. All rights reserved.
//

#ifndef SongGL_CBackground_h
#define SongGL_CBackground_h
#include "CSprite.h"
#include "CMS3DASCII.h"


class CTextureMan;
class CBackground : public CSprite
{
public:
    CBackground(unsigned int nID,unsigned char cTeamID,int nModelID,CTextureMan *pTextureMan,IAction *pAction,IHWorld* pWorld);
	virtual ~CBackground();
    virtual int     LoadResoure(CModelMan* pModelMan);
    virtual int     Initialize(SPoint *pPosition,SVector *pvDirection);
    virtual int     RenderBegin(int nTime);
	virtual int     Render();
    virtual int     RenderEnd();
    virtual int     OnEvent(SGLEvent *pEvent);
    
    
    int             ActivateCamera(SGLCAMERA* pCamera);
    void            SetBackgroundTexture(const char* sFleName,float* pPosition,float* pRotation);
    void            SetArrangeVertex(float* pPosition,float* pRotation);
    
    float    mfPosY; // 높은 곳으로 이동하여 지상의 캐릭터와 결치지 않게 한다.
    
#ifdef ANDROID
    virtual void ResetTexture();
#endif
    
protected:
    CMS3DASCII *mpModel;
    sglMesh    *mpWorldMesh;
    
    float      mfPosition[3];
    float      mfRotation[3];
    
};

#endif
