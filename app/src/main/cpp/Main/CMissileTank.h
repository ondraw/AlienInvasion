//
//  CMissileTank.h
//  SongGL
//
//  Created by 호학 송 on 2014. 5. 11..
//
//

#ifndef __SongGL__CMissileTank__
#define __SongGL__CMissileTank__

#include "CNewTank.h"
class CModelMan;
class CMissileTank : public CNewTank
{
public:
	CMissileTank(unsigned int nID,unsigned char cTeamID,int nModelID,IAction *pAction,IHWorld* pWorld,PROPERTY_TANK* pProperty,NETWORK_TEAMTYPE NetworkTeamType);
	virtual ~CMissileTank();
    virtual int LoadResoure(CModelMan* pModelMan);
    virtual int	 Initialize(SPoint *pPosition,SVector *pvDirection);
    virtual int	 RenderBegin(int nTime);
    virtual int  Render();
    virtual void RenderBeginCore_Invisiable(int nTime);

    virtual void FireMissile();
    virtual void FireMissileInVisible();;
    virtual void FireOrgMissileInVisible();
    
    virtual bool CanFireMissile();
    
    virtual bool    AimAutoToEnemy(SPoint* ptEnemy);
    
    virtual void  NewMissile(SPoint& ptNow,SVector& vtDir,SVector& vDirAngle,bool bNeedParticle = true);
    virtual void CheckAutoMove_PoUpDown() {}; //포업다운은 무시해버리장...
    
    virtual bool IsAssistance() { return true;}
    virtual bool IsTabIn(CPicking* pPick,SPoint& nearPoint,SPoint& farPoint,int nPosX,int nPosY);
    virtual bool IsTabInZeroGroup(CPicking* pPick,int nPosX,int nPosY);

protected:
    
    
    //포신이 3개있다. 최대발사 거시기를 세개로 분리해서 나타낸다.
    long mNextPoHide1;
    long mNextCanFireWate1;
    
    long mNextPoHide2;
    long mNextCanFireWate2;
    
    long mNextPoHide3;
    long mNextCanFireWate3;
    
};


#endif /* defined(__SongGL__CMissileTank__) */
