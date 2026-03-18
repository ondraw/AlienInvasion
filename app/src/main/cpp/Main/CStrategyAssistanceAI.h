//
//  CStrategyExAI.h
//  SongGL
//
//  Created by itsme on 2014. 5. 12..
//
//

#ifndef __SongGL__CStrategyExAI__
#define __SongGL__CStrategyExAI__

#include "CFigtherStrategyAI.h"
class CStrategyAssistanceAI : public CFigtherStrategyAI
{
public:
    CStrategyAssistanceAI(CSprite *pSprite,AAI* pMoveAI,int nPoCount,bool bIsCun = false);
    virtual ~CStrategyAssistanceAI();
    
    virtual void RenderBeginCore(int nTime);
//    virtual bool GetDonComact(){return false;} //너무 많은 회피를 하면 다른 객체와 충돌을 무시하고 움직인다.
    //적탱크로 이동한다.
    virtual void Command(int nCommandID,void *pData);
    virtual CSprite* GetAttackTo();
    virtual CSprite* GetAttackTo(int nPoIndex) { return mpMovinAttack[nPoIndex];}
    virtual CSprite* GetAttackToByGun() { return mpEnemyGunSprite;}
    bool SetMakePosition(float fRadius,int nIndex,SPoint* ptCenter,SPoint* ptOutPoint,int nDepth = 0);
    
    void CheckActorAssist(SPoint* ptNow2);
//    virtual void SetAttackTo_Multiplay(int nIndex,CSprite* pSprite,float fDistance);
//    virtual void SetAttackToByGun_Multiplay(CSprite* pSprite);

protected:
    virtual void CmdAttackTo(CSprite *pSprite);
    
    
    long     mCheckNextTime;
    long     mCheckNextTime2;
    bool     mMovingByUser; //사용자가 직접움직이게 하였다.
    
    long     mInNextTime;
    CSprite* mpMainTower;
    bool     mMovingToMain;
    
    STRATEGY_TYPE mType;
    bool    mbNeedStop;
    unsigned long    mStopTime;
};

#endif /* defined(__SongGL__CStrategyExAI__) */
