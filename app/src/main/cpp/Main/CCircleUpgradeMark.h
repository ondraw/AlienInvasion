//
//  CCircleUpgradeMark.h
//  SongGL
//
//  Created by 호학 송 on 2014. 2. 19..
//
//

#ifndef __SongGL__CCircleUpgradeMark__
#define __SongGL__CCircleUpgradeMark__

#include "sGLDefine.h"
#include "AParticle.h"
#include "CSprite.h"


//선명하게.
//#define CIRCLEPOINTCNT 16
//#define CIRCLEDIV      64

#define CIRCLEPOINTCNT 8
#define CIRCLEDIV      32

#define CIRCLEMAX 10
class IHWorld;
class CCircleUpgradeMark : public AParticle
{
public:
    CCircleUpgradeMark(IHWorld* pWorld,
                      CSprite* pOwner,
                       float fRadius);
    virtual ~CCircleUpgradeMark();
    
    virtual int Initialize(SPoint *pPosition,SVector *pvDirection);
    virtual void RenderBeginCore(int nTime);
    
	virtual int RenderBegin(int nTime);
	virtual int Render();
	virtual int RenderEnd();
    virtual int OnEvent(SGLEvent *pEvent) { return E_SUCCESS;};
    
    virtual SPRITE_STATE GetState() { return mState;}
    virtual void       SetState(SPRITE_STATE v) { mState = v;}
    virtual void GetPosition(SPoint *pOut) { mpOwner->GetPosition(pOut);}
    
    void SetCircleCount(int nCnt);// { mCircleCount = nCnt; mbInit = false;}
    int GetCircleCount() { return mCircleCount;}
    void SetVisibleRender(bool v) {m_bIsVisibleRender = v;}
    
protected:
    SPRITE_STATE mState;
    CSprite* mpOwner;
    IHWorld* mpWorld;
    
    float *mpPoint;
    float mfRadius;
    int   mCircleCount;
    
    float *mpWorldPoint;
    bool  mbInit;
    bool m_bIsVisibleRender;
};

#endif /* defined(__SongGL__CCircleUpgradeMark__) */
