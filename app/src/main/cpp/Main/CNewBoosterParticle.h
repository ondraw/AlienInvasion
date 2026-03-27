//
//  CNewBoosterParticle.h
//  SongGL
//
//  Created by 호학 송 on 2014. 3. 6..
//
//

#ifndef __SongGL__CNewBoosterParticle__
#define __SongGL__CNewBoosterParticle__
#include "CSprite.h"
#include "AParticle.h"

class CNewBoosterParticle : public AParticle {
public:
    CNewBoosterParticle(){};
    virtual ~CNewBoosterParticle();
    CNewBoosterParticle(IHWorld* pWorld,
                      CSprite* pOwner);

    virtual int Initialize(SPoint *pPosition,SVector *pvDirection);
    virtual void RenderBeginCore(int nTime);
    
	virtual int RenderBegin(int nTime);
	virtual int Render();
	virtual int RenderEnd();
    virtual int OnEvent(SGLEvent *pEvent) { return E_SUCCESS;};
    
    virtual SPRITE_STATE GetState() { return mState;}
    virtual void       SetState(SPRITE_STATE v) { mState = v;}
    virtual void GetPosition(SPoint *pOut) { mpOwner->GetPosition(pOut);}

    virtual void    SetVisibleRender(bool v) { mbVisible = v;}
#ifdef ANDROID
    virtual void ResetTexture();
#endif
    
    
protected:
    int     mnBoosterCnt;
    GLfloat **mppWorldVertex;
    GLfloat **mppWorldVertex2;
    float   mCoreAlpha;
    float   mGlowAlpha;
    float   mCoreScale;
    float   mGlowScale;
    float   mYOffset;
    
    SPRITE_STATE	mState;
    CSprite         *mpOwner;
    IHWorld         *mpWorld;
    GLuint          mTailTextureID;
    bool            mbVisible;
};

#endif /* defined(__SongGL__CNewBoosterParticle__) */
