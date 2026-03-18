//
//  CET9.h
//  SongGL
//
//  Created by itsme on 2013. 12. 4..
//
//

#ifndef __SongGL__CET9__
#define __SongGL__CET9__

#include "CET1.h"
#include "CBombGun.h"
//특징 타겟이 필요없이 사정거리 안쪽으로 오면 바로 발사하는 객체이다.
class CET9 : public CET1
{
	
public:
	CET9(unsigned int nID,unsigned char cTeamID,int nModelID,IAction *pAction,IHWorld* pWorld,PROPERTY_TANK* pProperty,NETWORK_TEAMTYPE NetworkTeamType);
	virtual ~CET9();
    virtual int	 Initialize(SPoint *pPosition,SVector *pvDirection);
    virtual void NewMissile(SPoint& ptNow,SVector& vtDir,SVector& vDirAngle,bool bNeedParticle = true);
    virtual bool AimAutoToEnemy(SPoint* ptEnemy);
    int GetTargetPos(SPoint *pPosition,SPoint *pToOutPosion,SVector* pOutDirection);
    
protected:
    SPoint mBeforeOwnerPos;
    SPoint mBeforeTargetPos;
    float  mBeforeLenght;
    int    mBeforeResult; // 0: 명중 1:아님
};

#endif /* defined(__SongGL__CET9__) */
