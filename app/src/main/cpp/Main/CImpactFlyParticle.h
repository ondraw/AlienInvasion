//
//  CImpactFlyParticle.h
//  SongGL
//
//  Created by itsme on 12. 8. 22..
//
//

#ifndef SongGL_CImpactFlyParticle_h
#define SongGL_CImpactFlyParticle_h

#include "sGLDefine.h"
#include "AParticle.h"


class IHWorld;
class CImpactFlyParticle : public AParticle
{
#ifdef ANDROID
    friend class CHWorld;
#endif
    
public:
    CImpactFlyParticle(IHWorld* pWorld);
    virtual ~CImpactFlyParticle();
    
    static int ImpactFlyParticleInitialize(const char *pFilePath);
    static void ImpactFlyParticleUninitialize();
    
    virtual int Initialize(SPoint *pPosition,SVector *pvDirection);
    virtual void RenderBeginCore(int nTime);
    
	virtual int RenderBegin(int nTime);
	virtual int Render();
	virtual int RenderEnd();
    virtual int OnEvent(SGLEvent *pEvent) { return E_SUCCESS;};
    
    virtual SPRITE_STATE GetState() { return mState;}
    virtual void       SetState(SPRITE_STATE v) { mState = v;}
    virtual void    GetPosition(SPoint* pOut)
    {
        memcpy(pOut,&mPosition,sizeof(mPosition));
    }
    void    SetPosition(SPoint* ptIn)
    {
        memcpy(&mPosition,ptIn,sizeof(mPosition));
    }
    

protected:
    static GLfloat m_gCompatFireParticleVertex[];
    static unsigned short m_gCompatFireParticleIndices[];
    static GLfloat m_gCompatFireParticleCoordTex[8];
    static GLuint g_CompatFireTextureID;
    static unsigned short g_shCompatFireIndicesCnt;
    
    
    GLfloat mWorldVertex[12];
    GLfloat mWorldMatrix[16];
    SPoint  mPosition;
    GLfloat mAlpha;
    
    SPRITE_STATE	mState;
    unsigned int             mnNowTime;
    IHWorld* mpWorld;

};


#endif
