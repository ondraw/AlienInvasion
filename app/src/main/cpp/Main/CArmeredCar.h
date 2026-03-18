//
//  CArmeredCar.h
//  SongGL
//
//  Created by itsme on 2014. 5. 21..
//
//

#ifndef __SongGL__CArmeredCar__
#define __SongGL__CArmeredCar__

#include "CTireCar.h"
class CArmeredCar : public CTireCar
{
public:
    CArmeredCar(unsigned int nID,unsigned char cTeamID,int nModelID,IAction *pAction,IHWorld* pWorld,PROPERTY_TANK* pProperty,NETWORK_TEAMTYPE NetworkTeamType);
	virtual ~CArmeredCar();
    virtual int LoadResoure(CModelMan* pModelMan);
    
    virtual void    NewMissile(SPoint& ptNow,SVector& vtDir,SVector& vDirAngle,bool bNeedParticle = true);
    virtual bool AimAutoToEnemy(SPoint* ptEnemy);
    virtual void  FireMissile();
    //보이지 않는 영역에서는 방향을 바로 정해서 쏜다.
    virtual void FireMissileInVisible();
    virtual bool CanFireMissile();
};

#endif /* defined(__SongGL__CArmeredCar__) */
