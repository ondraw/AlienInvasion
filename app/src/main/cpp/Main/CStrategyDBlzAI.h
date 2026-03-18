//
//  CStrategyDBlzAI.h
//  SongGL
//
//  Created by 호학 송 on 13. 10. 28..
//
//

#ifndef __SongGL__CStrategyDBlzAI__
#define __SongGL__CStrategyDBlzAI__

#include "sGLDefine.h"
#include "AAICore.h"
#include "sGL.h"

//#include <list>
//using namespace std;

class CDMainTower;
class AAI;
class CStrategyDBlzAI : public AAICore
{
public:
    CStrategyDBlzAI(CSprite *pSprite);
    virtual ~CStrategyDBlzAI();
    virtual void RenderBeginCore(int nTime);
    //현재의 상태를 모아온다.
    virtual void Command(AICORE_AICOMMANDD nCommandID,void *pData);
 
    virtual void SetActor() {};
    virtual AAI* GetMoveAI() {return 0;}
    virtual AAI* GetStategyAI() { return NULL;}
    virtual CSprite* GetAttackTo() {return 0;}
    virtual CSprite* GetAttackTo(int nIndex) { return 0;}
    virtual bool IsActor() {return false;}

    
protected:
    CDMainTower* mpOwner;
    long     mNextTime;
    long     mNextMakeTankTime;
};

#endif /* defined(__SongGL__CStrategyDBlzAI__) */
