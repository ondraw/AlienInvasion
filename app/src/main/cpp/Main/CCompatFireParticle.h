//
//  CFireParticle.h
//  SongGL
//
//  Created by Songs on 11. 4. 13..
//  Copyright 2011 thinkm. All rights reserved.
//
#ifndef _CCompatFireParticle_H
#define _CCompatFireParticle_H
#include "sGLDefine.h"
#include "AParticle.h"


class IHWorld;
class CMS3DModelASCII;
class CCompatFireParticle : public AParticle
{
public:
    CCompatFireParticle(IHWorld* pWorld,float fScale,const char* sFileName);
    virtual ~CCompatFireParticle();
    static int CompatFireParticleInitialize(const char *pFilePath);
    static void CompatFireParticleUninitialize();
    virtual int Initialize(SPoint *pPosition,SVector *pvDirection);
    virtual void RenderBeginCore(int nTime);
	virtual int RenderBegin(int nTime);
	virtual int Render();
	virtual int RenderEnd();
    virtual int OnEvent(SGLEvent *pEvent) { return E_SUCCESS;};
    virtual SPRITE_STATE GetState() { return mState;}
    virtual void       SetState(SPRITE_STATE v) { mState = v;}
    virtual void    GetPosition(SPoint* pOut) { 
        memcpy(pOut,&mPosition,sizeof(mPosition));
    }
    
protected:
    static GLfloat m_gCompatFireParticleVertex[];
    static unsigned short m_gCompatFireParticleIndices[];
    static GLfloat m_gCompatFireParticleCoordTex[8];
    static unsigned short g_shCompatFireIndicesCnt;
    
    
    GLfloat mWorldVertex[12];
    GLfloat mWorldMatrix[16];
    SPoint  mPosition;
    GLfloat mAlpha; //폭팔의 화면 투명도....
    
    SPRITE_STATE	mState;
    int             mnNowTime;
    IHWorld* mpWorld;
    float   mfScale;
    char* msFileName;
    GLuint mnCompatFireTextureID;
  
    CMS3DModelASCII* mpModel;
};


#endif //_CFIREPARTICLE_H