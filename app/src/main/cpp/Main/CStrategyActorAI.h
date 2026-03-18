//
//  CStrategyActorAI.h
//  SongGL
//
//  Created by 호학 송 on 13. 9. 10..
//
//

#ifndef __SongGL__CStrategyActorAI__
#define __SongGL__CStrategyActorAI__
#include "CStrategyAI.h"
class CStrategyActorAI : public CStrategyAI
{
public:
    CStrategyActorAI(CSprite *pSprite,AAI* pMoveID,bool bIsCun = false);
    
    virtual ~CStrategyActorAI();
    virtual void RenderBeginCore(int nTime);
    
    virtual bool GetDonComact() { return false;}
    //현재의 상태를 모아온다.
    virtual void Command(int nCommandID,void *pData);
    virtual bool IsTrainningMode() { return mbTrainningMode;}
    virtual void SetIsTrainningMode(bool v) { mbTrainningMode = v;}
    bool isClicked() { return GetGGTime() < mClickFireTime;}

    virtual void SetAttackTo_Multiplay(int nIndex,CSprite* pSprite,float fDistance);
    virtual void SetAttackToByGun_Multiplay(CSprite* pSprite);
    
protected:
    unsigned long mClickFireTime;
    bool    mbTrainningMode;
};


#endif /* defined(__SongGL__CStrategyActorAI__) */
