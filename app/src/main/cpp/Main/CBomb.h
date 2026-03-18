/*
 *  CBomb.h
 *  SongGL
 *
 *  Created by 호학 송 on 11. 3. 2..
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

/*
 *  CTestActor.h
 *  SongGL
 *
 *  Created by 호학 송 on 10. 11. 14..
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _CBOMB_H
#define _CBOMB_H

//#define MilkShape

//#include "CHWorld.h"
#include "CSprite.h"
#if OBJMilkShape
#include "CMS3DModel.h"
#else
#include "CMS3DModelASCII.h"
#endif
#include "CProperty.h"

#define BOMBMAXRNG  200.f
#define CHECKCOMPACTMINHEIGHT 20.f  //폭탄이 나라갈때 객체들이 폭탄에 맞았는지 확인을 하는데 너무 높은곳에 잇으면 그런것을 할 필요가 없다.
//#define K9_BOMB_SCALE 0.05f
//일반 폭탄의 초기 속도
//#define BOMB_VELOCITY 60.0f

class CBomb : public CSprite 
{
	
public:
    //lWhosBombID 어떤 객체가 폭탄을 쌋을까나... 알수 있는 아이디.
    //유도장치가 존재한다면 타겟 위치를 정의 한다.
    CBomb(CSprite* ptTarget,CSprite* pOwner,unsigned int nWhosBombID,unsigned char cTeamID,int nModelID,IAction *pAction,IHWorld* pWorld,PROPERTY_BOMB* pBombProperty);
    
	virtual ~CBomb();
	
    void SetRotationStartPosAngle(int nV) { mnRotationStartPosAngle = nV;}
    virtual void SetCompactBomb();
    
    //폭탄이 터졌을때 영향을 받는 범위안에 있으면 1(거의중앙),2,3  범위 밖에 있으면 0
    float InsideRangeCompact(float fSPAndBombDistance,CSprite* Sprite);
    int GetBombType() { return mBombProperty.nBombType;}
    
//    CSprite* GetTargetBySensor();
    
public:
    
	virtual int LoadResoure(CModelMan* pModelMan);
	virtual int Initialize(SPoint *pPosition,SVector *pvDirAngle,SVector *pvtDir);
	virtual int RenderBegin(int nTime);
	virtual int Render();
	virtual int RenderEnd();	
	virtual bool SetPosition(SPoint* sp);
    virtual bool CollidesBounding(CSprite* Sprite,bool bExact);
    virtual CSprite *GetOwner() { return mpOwner;}
    virtual bool VisibleByCamera();
    
    
    virtual void RenderBeginCore(int nTime);
    
	
    //폭탄의 다음 상태 지면 또는 범위 밖으로 나가는지를 체크
    virtual void CheckCollidesToTerrian();
	GLfloat GetVelocity() { return mBombProperty.fVelocity;};
    virtual void* GetModel() { return mpModel;};
	virtual float* GetOrientaion() { if(mpModel) return mpModel->orientation; return 0;};
    virtual float GetRadius();
    virtual float GetMaxAttackPower();//공격력
    CSprite* GetTarget() { return mpTarget;}
    const PROPERTY_BOMB& GetBombProperty()  { return mBombProperty;}
    
    bool GetTargetPosition(SPoint* ptTarget);
    void DivMissile();
    
    //주인공이 죽을 때 타겟과 오너의 값이 메모리에서 지워졌기 때문에.. 죽는 것을 방지 하자.
    void ResetAtKilledActor() { mpTarget= NULL;mpOwner=NULL;}
    
    //폭탄이 너무 높이 있으면 객체들이 폭탄에 맞았는지 체크할 필요가 없다.
    bool CheckCompact() { return mbCheckCompact;}
    
    
    virtual int GetResultRaser() { return -1;} //레이져는 발사시점부터 타겟이 명중할 것인지 아닌지를 알고 있다. 0:명중 not:비명중
protected:
	virtual void ChainCourse(int nTime);
    void ChainCourseMissaile(int nTime);
	virtual int CollidesToTerrian();
    void DivTargetNearBySprite(int nCnt,CSprite* pSprite,int nBombID,SPoint& ptNow,SVector& vtDir,SVector& vDirAngle,float fMaxLen = 3200);
	
protected:
	GLfloat         mArrAngle[3];           //초기 발사 각도
	SPoint          mptStart;               //발사 시작점
	GLuint          muiElapsedTime;         //경과 시간.
	SPoint          mptBeforePos;           //이전위치

	//이전폭탄의 위치와 현재 폭탄의 위치 atan((mfBeforeZ - nowZ) / (mfBeforeY - nowY)) 포사체의 현재 각도
	GLfloat         mfBeforeY;              //이전의 폭탄의 위치
	GLfloat         mfBeforeX;              //이전의 폭탄의 위치
    
#if OBJMilkShape
	CMS3DModel*     mpModel;
#else
    CMyModel*       mpModel;
#endif
    
	
    CSprite         *mpOwner;
    PROPERTY_BOMB   mBombProperty;
    
    int             mOutSide;               //장외로 나간 시간..
    CSprite         *mpTarget;
    
    float           mfRoationAngle;         //회전각
    float           mfRoationDif;           //점점 벌어지는
    int             mnRotationStartPosAngle;//회전의 시작지점 0,90,180,270 도.
    
    float           mfDivideAngle;          //각도의 변화가 + => - 변경되면 분산탄으로 변한다.
    
    float           mfGuideDirectionX;      //각도가 왼쪽오른쪽 한번에 너무바껴버리면 흔들린다. 0 : 0 -1:왼쪽 1:오른쪽
    float           mfGuideDirectionY;
    
    bool            mbRenderByCamera;       //카메라에서 비추어지면 그리고 안비추어지면 그리지말자.
    bool            mbCheckCompact;         //폭탄이 너무 높이 있으면 객체들이 폭탄에 맞았는지 체크할 필요가 없다.
};


#endif //_CBOMB_H