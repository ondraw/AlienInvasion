//
//  CFireParticle.h
//  SongGL
//
//  Created by Songs on 11. 4. 13..
//  Copyright 2011 thinkm. All rights reserved.
//
#ifndef _CFIREPARTICLE_H
#define _CFIREPARTICLE_H
#include "sGLDefine.h"
#include "AParticle.h"


class IHWorld;
class CHWorld;
class CFireParticle : public AParticle
{
#ifdef ANDROID
    friend class CHWorld;
#endif
    
public:
    CFireParticle(IHWorld *pWorld);
    virtual ~CFireParticle();
    
    static int FireParticleInitialize(const char *pFilePath);
    static void FireParticleUninitialize();

    virtual int Initialize(SPoint *pPosition,SVector *pvDirection);
    virtual void RenderBeginCore(int nTime);
    
	virtual int RenderBegin(int nTime);
	virtual int Render();
	virtual int RenderEnd();
    virtual int OnEvent(SGLEvent *pEvent) { return E_SUCCESS;};
    
    virtual SPRITE_STATE GetState() { return mState;}
    virtual void       SetState(SPRITE_STATE v) { mState = v;}
    virtual void GetPosition(SPoint* pOut) {
        memcpy(pOut,&mPosition,sizeof(mPosition));
    }
    
public:
    static GLfloat m_gFireParticleVertex[];
    static unsigned short m_gFireParticleIndices[];
    static GLfloat m_gFireParticleCoordTex[8];
    static GLuint g_FireTextureID;
    static unsigned short g_shFireIndicesCnt;
protected:
    
    GLfloat mWorldVertex[12];
    GLfloat mWorldMatrix[16];
    SPoint  mPosition;
    GLfloat mAlpha; //폭팔의 화면 투명도....
    
    SPRITE_STATE	mState;
    int             mnNowTime;
    IHWorld* mpWorld;
    
    
};


#endif //_CFIREPARTICLE_H