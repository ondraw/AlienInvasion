//
//  CAniRuntimeEnergyUP.h
//  SongGL
//
//  Created by itsme on 12. 10. 15..
//
//

#ifndef SongGL_CAniRuntimeEnergyUP_h
#define SongGL_CAniRuntimeEnergyUP_h
#include "sGLDefine.h"
#include "AParticle.h"

//초기화시에 미리 읽어들인다. 대부분 스레드에서 생성되기 때문에.. texture id를 미리 가져와야한다.
class IHWorld;
class CMS3DModelASCII;
class CSprite;
class CAniRuntimeEnergyUP : public AParticle
{
public:
    CAniRuntimeEnergyUP(IHWorld* pWorld,CSprite* pSprite);
    virtual ~CAniRuntimeEnergyUP();
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
    
protected:
    SPoint  mPosition;
    GLfloat mAlpha; //폭팔의 화면 투명도....
    SPRITE_STATE	mState;
    IHWorld* mpWorld;
    CMS3DModelASCII* mpModel;
    CSprite* mpSprite;
    int       mnNowTime;
};
#endif
