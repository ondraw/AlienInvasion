//
//  CArrowMotionSprite.h
//  SongGL
//
//  Created by 호학 송 on 12. 8. 16..
//
//

#ifndef SongGL_CArrowMotionSprite_h
#define SongGL_CArrowMotionSprite_h

#include "CSprite.h"
class CMyModel;
class CArrowMotionSprite : public CSprite
{
public:
    CArrowMotionSprite(unsigned int nID,unsigned char cTeamID,int nModelID,IAction *pAction,IHWorld* pWorld);
    virtual ~CArrowMotionSprite();
    virtual int Initialize(SPoint *pPosition,SVector *pvDirection);
    virtual int LoadResoure(CModelMan* pModelMan);
    virtual int RenderBegin(int nTime);
	virtual int Render();
    virtual int RenderEnd() { return E_SUCCESS;}
    virtual int OnEvent(SGLEvent *pEvent) { return E_SUCCESS;}
    virtual bool CollidesBounding(CSprite* Sprite,bool bExact);
    virtual SPRITE_STATE GetState();// { return SPRITE_RUN;} //항상살아 있다.
    virtual bool IsGhost() { return true;}
    virtual void SetModelDirection(SVector* pvd);
    virtual bool SetPosition(SPoint* sp);
    virtual void GetPosition(SPoint* pOut) { memcpy(pOut, &mptTab, sizeof(SPoint));}
    
    void Reset();

    
protected:
    CMyModel*     mpModel;
    SPoint mptTab;
    int     mVsibleTime;
    
    //클릭할때 이전 위치에서 보이고 클릭한 위치에 보이게 되는 잔상이 생긴다. 그래서 beginrender를 먼저하고 render를 할수 있게 순서를 정했다.
    bool    mbRender;
    
    float   mbAniDir; //화살표가 위아래로 움직인다.

};


#endif
