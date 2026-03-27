//
//  File.h
//  SongGL
//
//  Created by Songs on 11. 6. 29..
//  Copyright 2011 thinkm. All rights reserved.
//
#ifndef _CTE1BOOSTER_H
#define _CTE1BOOSTER_H
#include "sGLDefine.h"
#include "AParticle.h"

class IHWorld;
class CTE1BoosterParticle : public AParticle
{
    
public:
    CTE1BoosterParticle();
    virtual ~CTE1BoosterParticle();
    
    static int Initialize_ToCTE1Booster(IHWorld* pWorld);
    
    virtual int Initialize(SPoint *pPosition,SVector *pvDirection);
    virtual void RenderBeginCore(int nTime) {};
    
	virtual int RenderBegin(int nTime);
	virtual int Render();
	virtual int RenderEnd(){return E_SUCCESS;};
    virtual int OnEvent(SGLEvent *pEvent) { return E_SUCCESS;};
    
    virtual SPRITE_STATE GetState() { return mState;}
    virtual void       SetState(SPRITE_STATE v) { mState = v;}
    void CalBooster(SVector* pModelDir,SPoint *mPSRightBP,SPoint* mPSLeftBP);
    virtual void     GetPosition(SPoint* pOut) {}
    
    void SetVisibleRender(bool v) {m_bIsVisibleRender = v;}

public:
    static GLfloat m_gBoosterParticleVertex[];
    static GLfloat m_gBoosterParticleVertex2[];
    static unsigned short m_gBoosterParticleIndices[];
    static GLfloat m_gBoosterParticleCoordTex[8];
    static GLuint g_BoosterTextureID1;
    static GLuint g_BoosterTextureID2;
    static GLuint g_BoosterTextureID3;
    static unsigned short g_shBoosterIndicesCnt;
    
protected:
    SPRITE_STATE	mState;
    bool            m_bIsVisibleRender;
    float           mCoreAlpha;
    float           mGlowAlpha;
    float           mCoreScale;
    float           mGlowScale;
    float           mYOffset;
    
    
    GLfloat mWorldVertex[4][36];
    GLfloat mWorldVertex2[4][12];
    GLfloat mWorldMatrix[16];
    GLfloat mWorldMatrix2[16];
    GLuint  mWorldTextureID;
    

    
};


#endif //_CTE1BOOSTER_H
