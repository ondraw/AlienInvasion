//
//  AAI.h
//  SongGL
//
//  Created by Songs on 11. 7. 20..
//  Copyright 2011 thinkm. All rights reserved.
//
#ifndef _AAI_H
#define _AAI_H
#include "CArchive.h"
enum STRATEGY_TYPE {
    STRATEGY_NONE = 0,                  //현재 아무 일도 하지 않음.
    STRATEGY_RECON,                     //정찰중
    STRATEGY_ATTACKED_MOVGIN,           //누군가 공격을 하여 그쪽으로 이동중
    STRATEGY_ATTACK,                     //누군가를 공격중이다.
    STRATEGY_ATTACK_MOVE                //공격을 하고 있지만 잘맞지를 않아서 그쪽으로 이동한다.
};

//enum AI_TYPE { SEARCHPATH };
class CSprite;
class AAI
{
public:
    virtual ~AAI() {} ;
    virtual void RenderBeginCore(int nTime) = 0;
    virtual bool GetDonComact() = 0; //너무 많은 회피를 하면 다른 객체와 충돌을 무시하고 움직인다.
    //적탱크로 이동한다.
    virtual void Command(int nCommandID,void *pData) = 0;
    virtual CSprite* GetAttackTo() = 0;
    virtual CSprite* GetAttackTo(int nIndex){ return 0;}
    virtual void ResetAttackTo() {};
    virtual CSprite* GetAttackToByGun() { return 0;}

    //CStrategyAI
    virtual void SetAttackTo_Multiplay(int nIndex,CSprite* pSprite,float fDistance) {};
    virtual void SetAttackToByGun_Multiplay(CSprite* pSprite) {};
    
    //MoveAI
    virtual bool IsMoveTo() { return false;}
    virtual void SetMoveTo(CSprite* v) {}
    virtual void ArchiveMultiplay(int nCmd,CArchive& ar,bool bWrite) {};
};


#endif //_AAI_H