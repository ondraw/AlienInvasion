//
//  CStrategyBlzAI.h
//  SongGL
//
//  Created by 호학 송 on 13. 8. 1..
//
//

#ifndef __SongGL__CStrategyBlzAI__
#define __SongGL__CStrategyBlzAI__

#include "CStrategyAI.h"

class CSprite;
class AAI;
class CStrategyBlzAI : public CStrategyAI
{
public:
    CStrategyBlzAI(CSprite *pSprite);
    virtual ~CStrategyBlzAI();
    virtual void RenderBeginCore(int nTime);
    //현재의 상태를 모아온다.
    virtual void Command(int nCommandID,void *pData);
    
    virtual void SetAttackTo_Multiplay(int nIndex,CSprite* pSprite,float fDistance);
    virtual void SetAttackToByGun_Multiplay(CSprite* pSprite);
    
protected:
};


#endif /* defined(__SongGL__CStrategyBlzAI__) */
