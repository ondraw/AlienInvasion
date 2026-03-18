//
//  CShadowParticle.h
//  SongGL
//
//  Created by 송 호학 on 12. 1. 10..
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef SongGL_CShadowParticle_h
#define SongGL_CShadowParticle_h
#include "sGLDefine.h"
#include "AParticle.h"

class CSprite;
class CTextureMan;
class CShadowParticle : public AParticle
{
public:
    CShadowParticle(CSprite* pOwner);
    virtual ~CShadowParticle();
    virtual int Initialize(SPoint *pPosition,SVector *pvDirection);
    virtual void RenderBeginCore(int nTime);
    virtual int OnEvent(SGLEvent *pEvent) { return E_SUCCESS;};
	virtual int RenderBegin(int nTime);
	virtual int Render();
	virtual int RenderEnd(){return E_SUCCESS;};
    virtual SPRITE_STATE GetState() { return mState;}
    virtual void       SetState(SPRITE_STATE v) { mState = v;}
    virtual void        GetPosition(SPoint* pOut) {};
    
    //맵에서 보여지는지 안보여지는지를 설정하여 그려지는지 아닌지를 설정
    void SetVisibleRender(bool v) {m_bIsVisibleRender = v;}
    void SetReliveEffect(bool v) { m_bReliveEffect = v;}

protected:
    SPRITE_STATE	mState;
    bool            m_bIsVisibleRender;
    bool            m_bReliveEffect;
    CSprite*        mpOwner;
    CMyModel        *mpModel;
};

#endif
