//
//  CEF4Fighter.h
//  SongGL
//
//  Created by itsme on 2014. 4. 25..
//
//

#ifndef __SongGL__CEF4Fighter__
#define __SongGL__CEF4Fighter__

#include "CFighter.h"

//특징 : 포를 레이져로 사용한다. 즉 레이져포가 2개를 사용하기 위해서 만들어졌다.
//건을 이용하면 되지만 건은 1개만 사용할수 있기 때문에 이렇게 하였다.
class CEF4Fighter : public CFighter
{
public:
    //nModelID : 모델 아이디를 가지고 loadresource 에서 어떠한 밀크쉐이프 모델을 읽어오는지를 알아낸다.
	CEF4Fighter(unsigned int nID,unsigned char cTeamID,int nModelID,IAction *pAction,IHWorld* pWorld,PROPERTY_TANK* pProperty,NETWORK_TEAMTYPE NetworkTeamType);
    virtual ~CEF4Fighter();

    virtual bool  CanFireMissile();
    virtual void  NewMissile(int nIndex,SPoint& ptNow,SVector& vtDir,SVector& vDirAngle,bool bNeedParticle = true);
    virtual void  FireOrgMissile(int nIndex);
    virtual void  FireMissile();
    virtual void  FireMissileInVisible();
    virtual void  FireOrgMissileInVisible(int nIndex);
    virtual float GetMoveVelocity();
    virtual int  GetTargetPos(SPoint *pPosition,SPoint *pToOutPosion,SVector* pOutDirection,float* pOutLength); //레이져 총의 거리와 방향을 구해온다.
    
    void GetFirePoPos(int nIndex,float** pOutFront);
    
};


#endif /* defined(__SongGL__CEF4Fighter__) */
