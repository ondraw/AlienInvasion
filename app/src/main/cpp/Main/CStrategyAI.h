//
//  CStrategy.h
//  SongGL
//  전략전술 지능..
//  Created by 호학 송 on 11. 7. 24..
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#ifndef _CSTRATEGY_H
#define _CSTRATEGY_H
#include "sGL.h"
#include "AAICore.h"
#include "AAI.h"
#include "CSGLCore.h"

class CSprite;
class CMoveAI;
class CStrategyAI : public AAI
{
public:
    CStrategyAI(CSprite *pSprite,AAI* pMoveID,bool bIsCun = false);
    
    virtual ~CStrategyAI();
    virtual void RenderBeginCore(int nTime);
    virtual bool GetDonComact() { return false;}
    //현재의 상태를 모아온다.
    virtual void Command(int nCommandID,void *pData);
    
    static void Init_Static(SPoint* ptMainTowerPos,vector<STARTPOINT*>* plstStartPoint);
    static void Close_Static();
    virtual CSprite* GetAttackTo() { return mpMovinAttack;}
    virtual void ResetAttackTo();
    
    virtual CSprite* GetAttackToByGun() { return mpEnemyGunSprite;}
    
    virtual bool IsTrainningMode() { return false;}
    virtual void SetIsTrainningMode(bool v) {}
    virtual void ArchiveMultiplay(int nCmd,CArchive& ar,bool bWrite);
protected:
    virtual void CmdAttackTo(CSprite *pSprite);
    //CmdAttackTo로 공격을 하는데 계속해서 빗맞는다면. 다른 위치에서 공격을 하게 한다.
    void CmdAttackToDetail(CSprite* pSprite);
    //SPAI_STATE_NORMAL 상태일때 객체의 생명을 주자..
    //왔다갔다 움직이게 한다. (각 객체의 속성에 따라 어디로 움직일지 정의 한다.)
    bool GetReconPos(SPoint* ptOut);
    //정찰할 시작 지점의 정보로 Target Block을 가져온다.
    int GetRecon();
    
protected:
    CSprite *mpSprite;
    STRATEGY_TYPE mType;
    int mCntFire;
    CMoveAI* mvAI;
    static std::vector<std::vector<int>*> glstRecon;
    
    CSprite *mpMovinAttack;
    float   mfMovinAttackDistance;
    float mfGunDefendRadianBoundse; //총을 쏠수있는 최대거리.
    CSprite* mpEnemyGunSprite;
    
    bool    mbNeedStop;
    unsigned long    mStopTime;
    unsigned long    mRecTime; //STRATEGY_NONE에서 정찰할때까지의 기다리는 시간.
};

#endif //_CSTRATEGY_H