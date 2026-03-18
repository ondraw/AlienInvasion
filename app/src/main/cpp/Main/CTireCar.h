//
//  CArmoredCar.h
//  SongGL
//
//  Created by itsme on 2014. 5. 19..
//
//

#ifndef __SongGL__CArmoredCar__
#define __SongGL__CArmoredCar__
#include "CK9Sprite.h"
class CTireCar : public CK9Sprite
{
public:
    CTireCar(unsigned int nID,unsigned char cTeamID,int nModelID,IAction *pAction,IHWorld* pWorld,PROPERTY_TANK* pProperty,NETWORK_TEAMTYPE NetworkTeamType);
	virtual ~CTireCar();
    
    virtual int LoadResoure(CModelMan* pModelMan);
    virtual int	Initialize(SPoint *pPosition,SVector *pvDirection);
    
    virtual bool IsTabIn(CPicking* pPick,SPoint& nearPoint,SPoint& farPoint,int nPosX,int nPosY);
    virtual bool IsTabInZeroGroup(CPicking* pPick,int nPosX,int nPosY);
    
    virtual int Render();
    virtual int RenderBegin(int nTime);

    virtual bool IsAssistance() { return true;}

    virtual bool    SetPosition(SPoint* sp);
    virtual void    SetModelDirection(SVector* pvd);
    virtual void    ArchiveMultiplay(int nCmd,CArchive& ar,bool bWrite);
    //탱크의 방향과 위치가 변경될때 마다. 지형에 따라 각도와 탱크 해더의 위치가 달라진다.
	virtual int	ArrangeSprite();
public:
//    CMyModel*	mpModel; //타이어로 사용한다.
//    CTankModelASCII* mpModelHeader; //해더로 사용한다.
    CMyModel*	mpModelBody; //몸체
};

#endif /* defined(__SongGL__CArmoredCar__) */
