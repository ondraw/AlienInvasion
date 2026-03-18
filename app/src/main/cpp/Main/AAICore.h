//
//  AAICore.h
//  SongGL
//
//  Created by Songs on 11. 7. 28..
//  Copyright 2011 thinkm. All rights reserved.
//
#ifndef _AAICORE_H
#define _AAICORE_H

#include "AAI.h"
enum AICORE_AICOMMANDD {
    AICOMMAND_MANUFACTURE,
    AICOMMAND_MOVEIN,                   //사정거리 안쪽으로 이동함.,
    AICOMMAND_MOVEOUT,                  //사정거리 밖같쪽으로 이동함.
    AICOMMAND_COMPACTTOSPRITE,          //객체랑 충돌하였다.
    AICOMMAND_ATTACKEDBYEMSPRITE,       //적군한테 공격당하였다. (그러면 그놈한테 이동한다.)
    AICOMMAND_HITTHEMARK,               //명중했는지 안했는지
    AICOMMAND_BOMBED,                   //폭탄이 터졌다. ( 빗맞았는지를 체크하여 보다 정밀할 공격을 한다.)
    
    AICOMMAND_MOVE_SIMPLE_POS,          //간단하게 이동
    
    
    AICOMMAND_DONT_AUTO_FIRE,
//    AICOMMAND_CHANGE_ATTACK_TYPE,
    
    AICOMMAND_MAINKING,                 //메인타워에서 만들고 있는 상태
    AICOMMAND_COMPLETEDMAKE,             //메인타워에서 다만들었음.
    AICOMMAND_MOVE_MULTIPLAY,            //멀티플레이로 움지이게 해준다.
    AICOMMAND_MOVE_FIGHTER_MULTIPLAY
    
    
};


class AAICore
{
public:
    virtual ~AAICore() {};
    virtual void RenderBeginCore(int nTime) = 0;
    
//    //적탱크로 이동한다.
    virtual void Command(AICORE_AICOMMANDD nCommandID,void *pData) = 0; 
    virtual void SetActor() = 0;
    virtual AAI* GetMoveAI() = 0;
    virtual AAI* GetStategyAI() = 0;
    virtual CSprite* GetAttackTo() = 0;
    virtual CSprite* GetAttackTo(int nIndex) = 0;
    virtual CSprite* GetAttackToByGun(){ return 0;}
    virtual bool IsActor() = 0; //적군의대빵도 액터이다.
    virtual void ResetAttackTo(){};
    virtual void SetIsTrainningMode(bool v) {};
};


#endif //_AAICORE_H