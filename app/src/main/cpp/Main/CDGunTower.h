//
//  CDGunTower.h
//  SongGL
//
//  Created by itsme on 2013. 12. 11..
//
//

#ifndef __SongGL__CDGunTower__
#define __SongGL__CDGunTower__

#include "CDTower.h"
class CDGunTower : public CDTower
{
public:
    CDGunTower(unsigned int nID,unsigned char cTeamID,int nModelID,CTextureMan *pTextureMan,IAction *pAction,IHWorld* pWorld,PROPERTY_TANK* pProperty,NETWORK_TEAMTYPE NetworkTeamType);
    virtual ~CDGunTower();
    virtual int     Initialize(SPoint *pPosition,SVector *pvDirection);
    
    virtual void    NewMissile(SPoint& ptNow,SVector& vtDir,SVector& vDirAngle,bool bNeedParticle = true);
    virtual bool    AimAutoToEnemy(SPoint* ptEnemy);
    int GetTargetPos(SPoint *pPosition,SPoint *pToOutPosion,SVector* pOutDirection);
protected:
    SPoint mBeforeOwnerPos;
    SPoint mBeforeTargetPos;
    float  mBeforeLenght;
    int    mBeforeResult;
};

#endif /* defined(__SongGL__CDGunTower__) */
