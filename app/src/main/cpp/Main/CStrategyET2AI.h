//
//  CStrategyET2AI.h
//  SongGL
//
//  Created by itsme on 2014. 3. 18..
//
//

#ifndef __SongGL__CStrategyET2AI__
#define __SongGL__CStrategyET2AI__
#include "CStrategyAI.h"

class CStrategyET2AI : public CStrategyAI
{
public:
    CStrategyET2AI(CSprite *pSprite,AAI* pMoveID,bool bIsCun = false);
    virtual ~CStrategyET2AI();
    virtual void RenderBeginCore(int nTime);
    virtual void Command(int nCommandID,void *pData);
    virtual CSprite* GetAttackTo();
protected:
//    void CmdAttackTo(CSprite* pAttackTo);
    
//    float mfLastEnDistance;
//    CSprite *mpMovinAttacked; //누가 나를 공격하고 있는지 알아온다.
};;

#endif /* defined(__SongGL__CStrategyET2AI__) */
