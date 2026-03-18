//
//  CStrategyAmbulanceAI.h
//  SongGL
//
//  Created by itsme on 2014. 5. 22..
//
//

#ifndef __SongGL__CStrategyAmbulanceAI__
#define __SongGL__CStrategyAmbulanceAI__

#include "CStrategyAI.h"


//mpMovinAttack 은 도와주어야 할 같은틈의 탱크 
class CStrategyAmbulanceAI : public CStrategyAI
{
public:
    CStrategyAmbulanceAI(CSprite *pSprite,AAI* pMoveAI);
    virtual ~CStrategyAmbulanceAI();
        //적탱크로 이동한다.
    virtual void Command(int nCommandID,void *pData);
    virtual CSprite* GetAttackTo() { return NULL;}
    virtual CSprite* GetAttackTo(int nPoIndex) { return NULL;}
    virtual CSprite* GetAttackToByGun() { return mpEnemyGunSprite;}
    virtual void RenderBeginCore(int nTime);
    
    
protected:
    long     mInNextTime;
    long     mOutNextTime; //너무자주 체크하면 느려지지 않을까 걱정이 된다. 그래서 2초에 한번씩만 체크를 한다.
    bool     mMovingByUser; //사용자가 직접움직이게 하였다.

    
    CSprite* mpMainTower;
    long     mfMainTowerFar;
    bool     mpMainIn;
};


#endif /* defined(__SongGL__CStrategyAmbulanceAI__) */
