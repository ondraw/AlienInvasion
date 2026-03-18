//
//  CAICore.h
//  SongGL
//  각 생명체의 머리 즉 인지,공격,방어,행동 에 관련 사항들을 
//  처리하는 클래스이다.
//  Created by Songs on 11. 7. 20..
//  Copyright 2011 thinkm. All rights reserved.
//
#ifndef _CAICORE_H
#define _CAICORE_H
#include "AAICore.h"

#define AICORE_MAX_FIRE_DETAILATTACK 5

#define SPAI_STATE_NORMAL                   0x00
#define SPAI_STATE_MOVIN_FIGHTING           0x01

class CSprite;
class CMoveAI;
class CStrategyAI;
class CAICore : public AAICore
{
    
public:
    CAICore(CSprite *pSprite,AAI* pMoveAI,AAI* pStrategyAI);
    virtual ~CAICore();
    virtual void RenderBeginCore(int nTime);
    virtual void Command(AICORE_AICOMMANDD nCommandID,void *pData);
    virtual void SetActor();
    virtual AAI* GetMoveAI() { return mpMoveAI;}
    virtual CSprite* GetAttackTo();
    virtual CSprite* GetAttackTo(int nIndex);
    virtual bool IsActor() {return mbActor;} //적군의대빵도 액터이다.
    virtual void ResetAttackTo();
    virtual CSprite* GetAttackToByGun();
    virtual void SetIsTrainningMode(bool v);// { mbTrainningMode = v;}
    virtual AAI* GetStategyAI() { return mpStrategyAI;}
    
protected:
    //적탱크를 향해 공격을 한다.
    //void CmdAttackTo(CSprite *pSprite);
    
    
protected:
    AAI*            mpMoveAI;
    AAI*            mpStrategyAI;
    CSprite*        mpSprite;
    bool            mbActor;
};


#endif //_CAICORE_H