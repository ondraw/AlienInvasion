//
//  CFigtherStrategyAI.h
//  SongGL
//
//  Created by itsme on 2013. 12. 18..
//
//

#ifndef __SongGL__CFigtherStrategyAI__
#define __SongGL__CFigtherStrategyAI__
#include "AAI.h"

class AAI;
class CFigtherStrategyAI : public AAI
{
public:
    CFigtherStrategyAI(CSprite *pSprite,AAI* pMoveAI,int nPoCount,bool bIsCun = false);
    
    virtual ~CFigtherStrategyAI();
    virtual void RenderBeginCore(int nTime);
    virtual bool GetDonComact(){return false;} //너무 많은 회피를 하면 다른 객체와 충돌을 무시하고 움직인다.
    //적탱크로 이동한다.
    virtual void Command(int nCommandID,void *pData);
    virtual CSprite* GetAttackTo();
    virtual CSprite* GetAttackTo(int nPoIndex) { return mpMovinAttack[nPoIndex];}
    virtual CSprite* GetAttackToByGun() { return mpEnemyGunSprite;}
    virtual void SetAttackTo_Multiplay(int nIndex,CSprite* pSprite,float fDistance);
    virtual void SetAttackToByGun_Multiplay(CSprite* pSprite);
    virtual void ArchiveMultiplay(int nCmd,CArchive& ar,bool bWrite);
protected:
    int mnPoCnt;
    CSprite* mpSprite;
    CSprite** mpMovinAttack;
    
    CSprite* mpEnemyGunSprite;
    AAI* mpMoveAI;
    
    float mfGunDefendRadianBoundse;
    float* mfMovinAttackDistance;
};


#endif /* defined(__SongGL__CFigtherStrategyAI__) */
