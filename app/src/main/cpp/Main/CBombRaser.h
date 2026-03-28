//
//  CBombRaser.h
//  SongGL
//
//  Created by itsme on 2014. 3. 11..
//
//

#ifndef __SongGL__CBombRaser__
#define __SongGL__CBombRaser__

#include "CBomb.h"
class IHWorld;
class CSprite;
class CHWorld;
class CRaserParticle;
class CBombRaser : public CBomb
{
	
public:
    //lWhosBombID 어떤 객체가 폭탄을 쌋을까나... 알수 있는 아이디.
    //유도장치가 존재한다면 타겟 위치를 정의 한다.
    CBombRaser(CSprite* ptTarget,CSprite* pOwner,unsigned int nWhosBombID,unsigned char cTeamID,int nModelID,IAction *pAction,IHWorld* pWorld,PROPERTY_BOMB* pBombProperty);
	virtual ~CBombRaser();
    virtual int Initialize(SPoint *pPosition,SVector *pvDirAngle,SVector *pvtDir);
    virtual void SetCompactBomb();
    virtual void RenderBeginCore(int nTime);
	virtual int RenderBegin(int nTime);
    virtual int Render();
    virtual int RenderEnd();
    virtual bool CollidesBounding(CSprite* Sprite,bool bExact);
    virtual int  CollidesToTerrian();
    virtual int GetResultRaser() { return mnResult;} //레이져는 발사시점부터 타겟이 명중할 것인지 아닌지를 알고 있다. 0:명중 not:비명중
    int GetTargetPos(SPoint *pPosition,SPoint *pToOutPosion,SVector* pOutDirection,int *pOutLen);

    virtual void SetVisibleRender(bool v);
protected:
    long mnAliveTime;
    long mnRecTargetPos; //타겟을 매번 알아오면 느려질것 같아. 0.5초이후에 가져오자.
    long mnLastImpactEffectTime;
    int  mnResult; // 0: 명중 Not:비명중
    int  mnCompactIntervalTime; //CPU가 빠르면 CollidesBounding을 여러번 불러오기 때문에 100(0.1)초를 유지 할수 있게 해준다.

    CRaserParticle* mParticle;
    bool            mbCheckReset;
};


#endif /* defined(__SongGL__CBombRaser__) */
