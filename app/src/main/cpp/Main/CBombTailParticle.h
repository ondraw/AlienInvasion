//
//  CBombTailParticle.h
//  SongGL
//
//  Created by Songs on 11. 4. 13..
//  Copyright 2011 thinkm. All rights reserved.
//
#ifndef _CBombTailParticle_H
#define _CBombTailParticle_H

#include "sGLDefine.h"
#include "AParticle.h"
#include "CBomb.h"


class IHWorld;
class CBombTailParticle : public AParticle
{
public:
    CBombTailParticle() {};
    CBombTailParticle(IHWorld* pWorld,
                      CSprite* pBomb);
    virtual ~CBombTailParticle();
    
    static int BombTailParticleInitialize(IHWorld* pWorld);
    
    virtual int Initialize(SPoint *pPosition,SVector *pvDirection);
    virtual void RenderBeginCore(int nTime);
    
	virtual int RenderBegin(int nTime);
	virtual int Render();
	virtual int RenderEnd();
    virtual int OnEvent(SGLEvent *pEvent) { return E_SUCCESS;};
    
    virtual SPRITE_STATE GetState() { return mState;}
    virtual void       SetState(SPRITE_STATE v) { mState = v;}
    virtual void GetPosition(SPoint *pOut) { mpBomb->GetPosition(pOut);}
#ifdef ANDROID
    virtual void ResetTexture();
#endif


    static GLfloat m_gBombTailParticleVertex[];
    static unsigned short m_gBombTailParticleIndices[];
    static GLfloat m_gBombTailParticleCoordTex[8];
    static unsigned short g_shBombTailIndicesCnt;
    
protected:
    GLfloat mWorldVertex[2][12];
//    SPoint  mPosition;
    SPRITE_STATE	mState;
    int             mnNowTime;
    CSprite   *mpBomb;
    IHWorld *mpWorld;
    PROPERTY_BOMB mBombProperty;
    GLuint        mTailTextureID;
    
    
};

#endif
