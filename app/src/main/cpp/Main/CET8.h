//
//  CET8.h
//  SongGL
//
//  Created by itsme on 13. 9. 9..
//
//

#ifndef __SongGL__CET8__
#define __SongGL__CET8__

#include "CET1.h"
//특징 타겟이 필요없이 사정거리 안쪽으로 오면 바로 발사하는 객체이다.
class CET8 : public CET1
{
	
public:
	CET8(unsigned int nID,unsigned char cTeamID,int nModelID,IAction *pAction,IHWorld* pWorld,PROPERTY_TANK* pProperty,NETWORK_TEAMTYPE NetworkTeamType);
	virtual ~CET8();
    virtual bool AimAutoToEnemy(SPoint* ptEnemy);
    virtual int RenderBegin(int nTime);
    virtual void RenderBeginCore_Invisiable(int nTime);
    virtual float GetInvisiblePoUpAngle() { return 65.f;} //포신이 위아래 움직일수 있는 타입이면 true, CET8같이 고정이면
};

#endif /* defined(__SongGL__CET8__) */
