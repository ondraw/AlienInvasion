//
//  CItemSprite.h
//  SongGL
//
//  Created by itsme on 12. 9. 7..
//
//

#ifndef SongGL_CItemSprite_h
#define SongGL_CItemSprite_h
#include "CSprite.h"
class CMS3DModelASCII;
class CItemSprite : public CSprite
{
public:
    CItemSprite(PROPERTY_RUNTIMEITEM* pItem,IHWorld* pWorld,NETWORK_TEAMTYPE NetworkTeamType);
    virtual ~CItemSprite();
    virtual int Initialize(SPoint *pPosition,SVector *pvDirection);
    virtual int LoadResoure(CModelMan* pModelMan);
    virtual int RenderBegin(int nTime);
	virtual int Render();
    virtual int RenderEnd() { return E_SUCCESS;}
    virtual int OnEvent(SGLEvent *pEvent) { return E_SUCCESS;}
    virtual bool CollidesBounding(CSprite* Sprite,bool bExact);
    
    virtual bool IsGhost() { return true;}
    virtual void SetModelDirection(SVector* pvd);
    virtual bool SetPosition(SPoint* sp);

    PROPERTY_RUNTIMEITEM* GetRutimeItemInfo() { return mpItemInfo;}
    
    virtual void SetMoveVelocity(float v);
protected:
    CMyModel*        mpModel;
    unsigned long           mnEndTime;
    PROPERTY_RUNTIMEITEM*   mpItemInfo;
    bool                    mbMove;
//    //CollidesBounding을 여러번 실행하였을 경우. 여러번 아이템을 먹는 효과가 난타 그래서 이미 먹은것은 다시 먹지 못하게 막아야 한다.
//    bool                    mbAleardyUsed;
};



#endif
