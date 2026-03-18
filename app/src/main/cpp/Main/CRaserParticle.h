//
//  CRaserParticle.h
//  SongGL
//
//  Created by itsme on 2014. 3. 12..
//
//

#ifndef __SongGL__CRaserParticle__
#define __SongGL__CRaserParticle__
#include "sGLDefine.h"
#include "AParticle.h"

class IHWorld;
class CMS3DModelASCII;
class CBombRaser;
class CModelMan;
class CRaserParticle : public AParticle
{
public:
    CRaserParticle(IHWorld* pWorld,CBombRaser* pSprite);
    virtual ~CRaserParticle();
    virtual int Initialize(CModelMan* pModelMan,const char* sModelPath,const char* sTexturePath,const char* sModelStartEndPath);
    
    virtual void RenderBeginCore(int nTime);
	virtual int RenderBegin(int nTime);
	virtual int Render();
	virtual int RenderEnd();
    
    virtual SPRITE_STATE GetState() { return mState;}
    virtual void       SetState(SPRITE_STATE v) { mState = v;}
    int Arrange(float BombStartPosition,SPoint *pPosition,SPoint* ptTargetPos,SVector *pvDirAngle,SVector *pvtDir,float fTotalLength);
    void SetVisibleRender(bool v) {m_bIsVisibleRender = v;}
protected:
    virtual void GetPosition(SPoint* pOut){};
    virtual int OnEvent(SGLEvent *pEvent) { return E_SUCCESS;};
    virtual int Initialize(SPoint *pPosition,SVector *pvDirection){return E_SUCCESS;}
    
protected:
    CBombRaser* mpSprite;
    SPRITE_STATE	mState;
    IHWorld* mpWorld;
    CMS3DModelASCII* mpModel;
    CMS3DModelASCII* mpModelStart;
    CMS3DModelASCII* mpModelEnd;
    
    bool m_bIsVisibleRender;
};

#endif /* defined(__SongGL__CRaserParticle__) */
