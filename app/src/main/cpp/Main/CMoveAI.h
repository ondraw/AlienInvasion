//
//  CMoveAI.h
//  SongGL
//
//  Created by Songs on 11. 7. 20..
//  Copyright 2011 thinkm. All rights reserved.
//
#ifndef _CMOVEAI_H
#define _CMOVEAI_H
#include <vector>
#include "sGL.h"
#include "AAI.h"



//0:일반 1:이동 2:장애물이 아니지형으로 이동
enum MOVEAI_STATE { MOVEAI_NONE,MOVEAI_AUTOTURN,MOVEAI_AUTOMOVE};
enum AVOID_STATE {
    AVOID_NONE,
    
    //처음 충돌시 +- 30도 회전후 약간 뒤(300)로 이동한다. 
    //=> 상대가 움직이고 있을 경우 :AVOID_FORNT_COMPACT2
    //=> 상대가 움직이지 않고 있을 경우 : AVOID_FORNT_COMPACT3 (좀더 회전하고, 앞으로 이동한다.)
    AVOID_FORNT_COMPACT, 
    AVOID_FORNT_COMPACT2, //다시 전진 한다. (400) 전진 한다.
    AVOID_FORNT_COMPACT3  //좀더 회전한후에 앞으로 이동후  원래 타겟으로 간다.
};

typedef struct AvoidData_
{
    AVOID_STATE     AvoidState;
    float fRotAngleDir;
    float fMoveDir;
}AvoidData;


class CSprite;
class CMoveAI : public AAI
{
public:
    CMoveAI(CSprite *pSprite);
    virtual ~CMoveAI();
    virtual void RenderBeginCore(int nTime);
    virtual bool GetDonComact(); //너무 많은 회피를 하면 다른 객체와 충돌을 무시하고 움직인다.
    
    //적탱크로 이동한다.
    virtual void Command(int nCommandID,void *pData);
    virtual void ArchiveMultiplay(int nCmd,CArchive& ar,bool bWrite);
    
    //bFront : true (가기전) , false : 정확하게 그지점
    void CmdMoveTo(CSprite *pSprite,bool bFront);
    //bFront : true (가기전) , false : 정확하게 그지점
    void CmdMoveTo(SPoint *ptPos,bool bFront);
    bool IsAutoMovingAndTurn() { return mState != MOVEAI_NONE;}
    
    
    //액터의 이동은 심플모드로 이동하게 한다.
    void SetSimpleMove(bool v)
    {
        if(v)
        {
            if(mPathList)
            {
                delete mPathList;
                mPathList = 0;
            }
            mbActorAutoAvoid = false;
            mAvoidState.AvoidState = AVOID_NONE;
        }
        mbSimpleMove = v;
    }
    
    void StopMove();
    //void SetRotaionAngle(float fAngle);
    
    void    SetRotationVelocity(float v);
    void    SetToRotationAngle(float v);
    float   GetRotationVelocity();
    float   GetToRotationAngle() { return mfToRotationAngle;}
    void    SetMoveDirection(SVector* v) { memcpy(&mvMoveDirection, v, sizeof(SVector));}
    void    GetMoveDirection(SVector* v) {memcpy(v, &mvMoveDirection, sizeof(SVector));}
    void    SetMoveVelocity(float v) { mfMoveVelocity = v;}
    float   GetMoveVelocity() { return mfMoveVelocity;}
    void    TurnAndMove(int nTime);
    bool    isMoving() {return mfMoveVelocity == 0.0f ? false : true;}
    bool    isTurning() { return mfRotationVelocity == 0.0f ? false : true;}

    
    void SetAutoTurnAndMoveToTarget(SPoint* ptPathPos,SPoint* ptLastTarget,bool bOnlyFront = false);
    void AvoidCompactSprite(CSprite* pCompactSP = NULL);
    
    
    void NextAvoid();
    void AvoidCOMACT_F(bool bLeft);
    
    void SetAvoidState(AVOID_STATE st,float fRotAngleDir,float fMoveDir);
    bool GetCenterPointAtNextPath(SPoint* ptNow);
    
    void DontAvoidState();
    AVOID_STATE GetAvoidState();
    
    int  GetLastPathPos();
    
    virtual CSprite* GetAttackTo() { return NULL;}
//    int GetAvoidCount() { return mAvoidCnt;}
protected:
    
	//스프라이터의 움직임을 정의 한다.
	//이동
	//mvMove.x, mvMove.y 0 이 아니면 움직임으로 생각한다.
    //mvMove.z 는 실제 속도를 나타낸다.
	SVector         mvMoveDirection;    //  움직이고자하는 방향 
    float           mfMoveVelocity;     //
    float           mfEndTrunAndMoveDirection; //탱크는 회전후에 앞으로 이동할지 뒤로 이동할지를 틀리다.
	
	//회전
	float           mfRotationVelocity;
    float           mfToRotationAngle;  //  -9999.0이면 스틱으로 움직이는 것이고 아니면 자동으로 움직이는 것이라 이 각도까지 와서 멈춘다.(사이각)
    
    SPoint          mptLastTarget;      //  현재 움직여야할 위치의 최종 목적지 (Path의 최종목적지 아님)
    SPoint          mptNowTarget;       //  현재 움직여야할 위치
    
    SPoint          mptLastTargetAtPath;//  Path의 최종목적지. 
    
    bool            mbSimpleMove;       //  이동시 회피나 검색없이 직선거리로 이동 (Actor의 단순이동)
    double          mdMigrationLength;  //  거리
    
    CSprite*        mbAvoidingToComactObject; //어떤 객체와 부디쳐서 회피모드이다.
    AvoidData       mAvoidState;
    
    int             mAvoidCnt; //회피를 너무 자주하면 움직임을 종료를 하던 그밖의 작업을 하자.
    MOVEAI_STATE      mState;
    std::vector<int>* mPathList;

    CSprite          *mpSprite;
    bool             mbActorAutoAvoid; //혹시 너무 무한 루푸를 돌지 못하게 하려고 한다.
    
    unsigned long    mDonAvoidCompactTime;
};


#endif //_CMOVEAI_H