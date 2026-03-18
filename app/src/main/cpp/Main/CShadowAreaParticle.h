//
//  CShadowAreaParticle.h
//  SongGL
//
//  Created by itsme on 2013. 10. 25..
//
//

#ifndef __SongGL__CShadowAreaParticle__
#define __SongGL__CShadowAreaParticle__
#include "sGLDefine.h"
#include "AParticle.h"

class CMS3DASCII;
class CHWorld;
class IHWorld;
class CSprite;
class CShadowAreaParticle : public AParticle
{
public:
    CShadowAreaParticle(IHWorld* pWorld,CSprite* pOwner);
    virtual ~CShadowAreaParticle();
    virtual int Initialize(SPoint *pPosition,SVector *pvDirection) { return E_SUCCESS;}
    virtual int Initialize(SPoint *pPosition,SVector *pvDirection,float fArea);
    virtual void RenderBeginCore(int nTime);
    virtual int OnEvent(SGLEvent *pEvent) { return E_SUCCESS;};
	virtual int RenderBegin(int nTime);
	virtual int Render();
	virtual int RenderEnd(){return E_SUCCESS;};
    virtual SPRITE_STATE GetState() { return mState;}
    virtual void       SetState(SPRITE_STATE v) { mState = v;}
    virtual void        GetPosition(SPoint* pOut) {};

        
protected:
    SPRITE_STATE	mState;
    CMS3DASCII      *mpModel;
    CHWorld         *mpWorld;
    CSprite        *mpOwner;
//    bool            m_bIsVisibleRender;
//    float           mfVertex[12];
//    CTextureMan*    mpTextureMan;
//    GLuint          mlTextureID;
//    bool            m_bReliveEffect;
};

#endif /* defined(__SongGL__CShadowAreaParticle__) */
