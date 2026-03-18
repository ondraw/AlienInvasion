//
//  CBubbleParticle.h
//  SongGL
//
//  Created by itsme on 12. 10. 30..
//
//

#ifndef SongGL_CBubbleParticle_h
#define SongGL_CBubbleParticle_h
#include "sGLDefine.h"
#include "AParticle.h"

//초기화시에 미리 읽어들인다. 대부분 스레드에서 생성되기 때문에.. texture id를 미리 가져와야한다.
class IHWorld;
class CMS3DModelASCII;
class CSprite;
class CAniRuntimeBubble : public AParticle
{
public:
    CAniRuntimeBubble(IHWorld* pWorld,CSprite* pSprite,int nMaxSec,int nUpgradeCount = 0);

    
    virtual ~CAniRuntimeBubble();
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
    
    void ResetBubble(int nMaxSec);
    void Attacted(); //공격당하면 조금 불투명하게 하자.
    
    void SetColor(float r,float g, float b);
    void SetScale(float fx,float fy,float fz);
    float GetScale();
    float* GetColor() { return mfColor;}
    
    
    unsigned long GetMaxTime() { return mnMaxTime;}

    void SetUpgradeCount(int v) { mnUpgradeCount = v;}
    int  GetUpgradeCount() { return mnUpgradeCount;}
    
protected:
    SPoint  mPosition;
    GLfloat mAlpha; //폭팔의 화면 투명도....
    SPRITE_STATE	mState;
    IHWorld* mpWorld;
    CMS3DModelASCII* mpModel;
    CSprite* mpSprite;
    unsigned long       mnMaxAttacted;
    
    unsigned long   mnMaxTime;
    float           mfColor[3];
    int             mnUpgradeCount;
};

#endif
