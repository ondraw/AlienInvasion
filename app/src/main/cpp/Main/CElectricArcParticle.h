//
//  CElectricArcParticle.h
//  SongGL
//

#ifndef __SongGL__CElectricArcParticle__
#define __SongGL__CElectricArcParticle__

#include "AParticle.h"

class IHWorld;

class CElectricArcParticle : public AParticle
{
public:
    CElectricArcParticle(IHWorld* pWorld);
    virtual ~CElectricArcParticle();

    virtual int Initialize(SPoint *pPosition,SVector *pvDirection);
    int Initialize(SPoint *pPosition,float fRadius);
    int Initialize(SPoint *pPosition,float fRadius,float fThicknessScale);
    virtual void RenderBeginCore(int nTime);
    virtual int RenderBegin(int nTime);
    virtual int Render();
    virtual int RenderEnd();
    virtual int OnEvent(SGLEvent *pEvent) { return E_SUCCESS; }
    virtual SPRITE_STATE GetState() { return mState; }
    virtual void SetState(SPRITE_STATE v) { mState = v; }
    virtual void GetPosition(SPoint* pOut);

protected:
    void BuildArc(int nArcIndex,float fRadiusScale);

protected:
    IHWorld*        mpWorld;
    SPRITE_STATE    mState;
    SPoint          mPosition;
    float           mfRadius;
    float           mfThicknessScale;
    int             mnAliveTime;
    int             mnElapsedTime;
    float           mArrVertices[6 * 14 * 3];
    int             mnVertexCount[6];
};

#endif
