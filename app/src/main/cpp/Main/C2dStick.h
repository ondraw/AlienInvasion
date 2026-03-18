//
//  C2dStick.h
//  SongGL
//
//  Created by 호학 송 on 11. 4. 4..
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#ifndef _C2DSTICK_H
#define _C2DSTICK_H
#include "sGLDefine.h"
#include "CControl.h"
#include "ListNode.h"
#include <time.h>
#define  MAX_SINGLE_CLICK_ESCAPE_TIME 250  //0.25초 안쪽으로 클릭이벤트가 날라오면 더블클릭이라고 판단한다.

typedef enum _STICKTYPE { STIKCKTYPE_NONE, STIKCKTYPE_ACTORMOVE = 1,STIKCKTYPE_ROTATIONVIEW = 2, STIKCKTYPE_ZOOM = 4}STICKTYPE;
class IHWorld;
class CSprite;
class C2dStick : public CControl
{
public:
    C2dStick(IHWorld* pWorld);
    virtual ~C2dStick();
    
    virtual int Initialize(SPoint *pPosition,SVector *pvDirection);
	virtual int RenderBegin(int nTime);
    virtual int Render();
	virtual int RenderEnd();
	virtual int OnEvent(SGLEvent *pEvent);

    //이동 깃발 밑에 파킹을 실세계좌표에 그려준다.
    int RenderMoveingFly();

    //lTouchID 멀티터치는 하나의 터치를 눌룰때마다 아이디가 있다.
    bool BeginTouch(long lTouchID,float x, float y);
    bool MoveTouch(long lTouchID,float x, float y);
    bool EndTouch(long lTouchID,float x, float y);

//    bool IsPoUpDown() { return mbPoUpDown;}
//    void SetPoUpDown(bool v) { mbPoUpDown = v;}
    void OnClick(float fx,float fy);
    bool IsCenterCenter(float x, float y);

    void ClearTouch() { mlTouch1 = -1; mlTouch2 = -1; mnStickType = (int)STIKCKTYPE_NONE;}
#ifdef ANDROID
    virtual void ResetTexture();
#endif
    int GetStickType() { return mnStickType;}
    
    //건물을 드래그 할때 그위치의 땅에 그려준다.
    void ShowFly(bool bv,float x,float y);
    float* GetFlyPosition() { return mbMovingFlyCenter;}
    CSprite* IsTabInZeroGroup(float x,float y);
protected:
    
   
    long    mlTouch1;       //ID : 터치 1
    long    mlTouch2;       //ID : 터치 2

    GLfloat mptBefore1[2];  //터치 1의 이전 좌표
    GLfloat mptBefore2[2];  //터치 2의 이전 좌표
    GLfloat mLenBefore;     //핀치줌의 이전 거리

    CControl      *mpMovingFly;       //파킹 컬트롤 탭시 mpParkingFly숨기고 이것을 보여준다.
    int           mnStickType;
    CSprite       *mpActorMove;
    

    GLfloat mbMovingFlyCenter[3];                       //실재 위치. (즉 삼각형의 무게중심)
    GLfloat mfMovingFlyVertex[12];                      //파킹시 실셰계의 깃발 밑에 파킹을 그려준다.
    GLuint  mfMovingFlyTextureID;                       //깃발 밑에 파킹의 텍스쳐 아이디.

    IHWorld* mpWorld;
    
    
    unsigned long mClickEscape;
    float   mfStartRotX;

    
};
#endif //_C2DSTICK_H