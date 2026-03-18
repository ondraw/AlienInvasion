//
//  CAIBlzCore.h
//  SongGL
//
//  Created by Songs on 11. 7. 28..
//  Copyright 2011 thinkm. All rights reserved.
//
#ifndef _CAIBLZCORE_H
#define _CAIBLZCORE_H
#include "AAICore.h"
#include "CSGLController.h"
#include "sGLDefine.h"

enum MISSIONSETP {
    MISSIONSETP1,   //1분 8초     2개
    MISSIONSETP2,   //1분 2.5초   1개
    MISSIONSETP3,   //1분 3초     2개
    MISSIONSETP4    //1분 1초     1개
};

class CSprite;
class CAIBlzCore : public AAICore
{
    
public:
    CAIBlzCore(CSprite *pSprite,int nCnt,int* nTable);
    virtual ~CAIBlzCore();
    virtual void RenderBeginCore(int nTime);
    virtual void Command(AICORE_AICOMMANDD nCommandID,void *pData);
    virtual void SetActor() {};
    void MakeTank();
    virtual AAI* GetMoveAI() { return NULL;}
    virtual AAI* GetStategyAI() { return NULL;}
    virtual CSprite* GetAttackTo() { return NULL;}
    virtual CSprite* GetAttackTo(int nIndex) { return NULL;}
    virtual bool IsActor() {return false;}
    
protected:
    void RuleNextStep(unsigned long ggTime);
    void RuleStepIntervalTime(unsigned long ggTime);
    int  RuleMakObjectIndex();
    void SetMakePosition(CSprite *pTank,SPoint* ptCenter);
    
    
protected:
    CSprite*        mpSprite;
    int             mnState; //0:생산중지 1:생산시작
    unsigned long   mnManufTimer;
    int             mnPosIndex;

    int             *mpArrObjectTable;
    int             mnArrObjectTableCnt;
    
    
    unsigned long   mnMissionTime;
    MISSIONSETP     mnMisssionStep;
};

#endif //_CAIBLZCORE_H