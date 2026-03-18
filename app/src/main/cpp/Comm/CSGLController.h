//
//  CSGLController.h
//  SongGL
//
//  Created by 호학 송 on 11. 7. 28..
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#ifndef _CSGLCONTROLLER_H
#define _CSGLCONTROLLER_H
#include "IAction.h"
#include <map>
#include <pthread.h>
#include <unistd.h>
using namespace std;

class CSprite;

typedef struct _TEAMINFO
{
    //model(type), count
    map<int,int> mapCnt;
    int nTotalCnt;
    CSprite *pMainBlz;
}TEAMINFO;



//각행동에 대한 이벤트가 발생할때 처리를 해준다.
//1. 객체가 추가될때 마다 맥스인원을 초과하였으면 컨틀럴타워에 생산중단 메세지를 보낸다.
//2. 객체가 지워질때 마다 맥스윈원 이내이면 컨트럴타워에 생산 메세지를 보낸다.
//3. 컨트롤 타워가 공격 당하고 있을때
//4. 컨트롤 타워가 공격 당하고 있지 않을때
// 4-1.
class IHWorld;
class CSGLController: public IAction
{
public:
    CSGLController(IHWorld* pWorld);
    virtual ~CSGLController();    
    
    virtual void OnAutoAimed(CSprite* pSrcObj);
    virtual void OnActotorDeleted(CSprite* pActor);
    
    //객체가 추가될 때 팀별로 TEAMINFO를 저장을 한다.
    //TEAMINFO는 각 모델별로 갯수와 총갯수를 가지고 있다.
    //각팀의 정원수가 모질라면 탱크를 만든다.
    virtual void OnAddedSprite(CSprite* pV);
    virtual void OnDeletedSprite(CSprite* pV);
    //virtual void OnSound(int nID);
    
    void Clear();
    
    CSprite* GetMainBlz(int nTeamID);
    IHWorld* GetWorld();

    //0:더이상 만들 탱크가 없다.(인구수 초과)
    //other : 만들수있는 객체의 갯수
    //arrResult : In:모델아이디 Out:(1:만들수있다, 0:만들수없다)
    virtual int OnMainTowerMakeAssistObject(int nTeamID,int nSize,int* arrResult);
//    map<int,TEAMINFO*>* GetTeamInfo() { return &mTeamInfo;}
protected:
    map<int,TEAMINFO*> mTeamInfo;
    IHWorld* mpWorld;
    
    pthread_mutex_t             mTeamInfoMutex;
};
#endif