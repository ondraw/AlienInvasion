//
//  CBombGun.h
//  SongGL
//
//  Created by itsme on 2013. 12. 4..
//
//

#ifndef __SongGL__CBombRaser__
#define __SongGL__CBombRaser__


#include "CBomb.h"
class IHWorld;
class CSprite;
class CHWorld;
class CBombGun : public CBomb
{
	
public:
    //lWhosBombID 어떤 객체가 폭탄을 쌋을까나... 알수 있는 아이디.
    //유도장치가 존재한다면 타겟 위치를 정의 한다.
    CBombGun(CSprite* ptTarget,CSprite* pOwner,unsigned int nWhosBombID,unsigned char cTeamID,int nModelID,IAction *pAction,IHWorld* pWorld,PROPERTY_BOMB* pBombProperty);
	virtual ~CBombGun();
    
    int Initialize(SPoint *pPosition,SPoint* ptTargetPos,SVector *pvDirAngle,SVector *pvtDir,float fTotalLength,int nResult);
    virtual void SetCompactBomb();
    virtual int GetResultRaser() { return mnResult;} //레이져는 발사시점부터 타겟이 명중할 것인지 아닌지를 알고 있다. 0:명중 not:비명중
protected:
	virtual void ChainCourse(int nTime);
    virtual int CollidesToTerrian();
    
protected:
    float mfTotalLength; //총거리
    float mfNowLenght; //현재 간거리
    SPoint mptTargetPos; //최정 목표
    int    mnResult; // 0: 명중 Not:비명중
};

#endif /* defined(__SongGL__CBombRaser__) */
