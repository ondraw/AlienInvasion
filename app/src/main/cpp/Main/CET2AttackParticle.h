//
//  CET2Attack.h
//  SongGL
//
//  Created by 호학 송 on 12. 11. 10..
//
//

#ifndef SongGL_CET2AttackParticle_h
#define SongGL_CET2AttackParticle_h
#include "sGLDefine.h"
#include "AParticle.h"

class CSprite;
class IHWorld;
class CET2AttackParticle : public AParticle
{
#ifdef ANDROID
    friend class CHWorld;
#endif
    
public:
    CET2AttackParticle(IHWorld *pWorld,CSprite* pSprite);
    virtual ~CET2AttackParticle();
    
    static  int ET2AttackParticleInitialize(const char *pFilePath);
    static  void ET2AttackParticleUninitialize();
    virtual int Initialize(SPoint *pPosition,SVector *pvDirection);
    virtual void RenderBeginCore(int nTime);
	virtual int RenderBegin(int nTime);
	virtual int Render();
	virtual int RenderEnd();
    virtual int OnEvent(SGLEvent *pEvent) { return E_SUCCESS;};
    virtual SPRITE_STATE GetState() { return mState;}
    virtual void       SetState(SPRITE_STATE v) { mState = v;}
    virtual void        GetPosition(SPoint* pOut);
    void SetHide(bool v) { mbHide = v;}
protected:
    static GLuint   g_CET2TextureID;
    GLfloat         mWorldVertex[12];
    GLfloat         mAlpha;
    SPRITE_STATE	mState;
    IHWorld*        mpWorld;
    CSprite*        mpOwner;
    bool            mbHide;
};

#endif
