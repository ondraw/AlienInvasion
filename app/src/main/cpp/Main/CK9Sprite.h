/*
 *  CK9Sprite.h
 *  SongGL
 *
 *  Created by 호학 송 on 11. 1. 30..
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "CSprite.h"


#ifndef _CK9SPRITE_H
#define _CK9SPRITE_H

//#define K9_ROTATION_RATE 0.1f //0~6단계 까지 있고 0.1 라디안으로 회전할수 있다. 그래서 0~0.6라디안까지 회전속도가 가능하다.
//#define K9_MOVE_RATE 0.015f //0~6단계 까지 있고 1클럭당 기준으로 0.01움직이면 보통 10클럭일때 0.1움직인다. 최다 0.6포인트 움직인다.
#include "CUserInfo.h"

#define TestPoStart_F 0 //포의 시작위치를 알아보려면 여기를 체크해준다.
class CTankModelASCII;
class CK9Sprite : public CSprite 
{
	
public:
	CK9Sprite(unsigned int nID,unsigned char cTeamID,int nModelID,IAction *pAction,IHWorld* pWorld,PROPERTY_TANK* pProperty,NETWORK_TEAMTYPE NetworkTeamType);
	virtual ~CK9Sprite();
	
public:
    //---------------------------------------------------------------------
    //Start CSprite Virtual
	virtual int		LoadResoure(CModelMan* pModelMan);
	virtual int		Initialize(SPoint *pPosition,SVector *pvDirection);
    virtual void    RenderBeginCore(int nTime);
    virtual void    RenderBeginCore_Invisiable(int nTime);
	virtual int		RenderBegin(int nTime);
	virtual int		Render();
	virtual int		RenderEnd();	
	virtual bool    SetPosition(SPoint* sp);
	virtual void    SetModelDirection(SVector* pvd);
	virtual bool	CollidesBounding(CSprite* Sprite,bool bExact);
	virtual int     OnEvent(SGLEvent *pEvent);
	virtual void	SetCameraType(int v);
    virtual void*   GetModel() { return (void*)mpModel;}; 
    virtual float*  GetOrientaion() { if(mpModel) return mpModel->orientation; return 0;};
    //공격하는 방향.. 탱크의 Attack의 방향은 GetOrientation()[2] + Header의 방향.
    virtual float GetOrientationToAttack() { return mpModel->orientation[1] + mfModelHeaderAngle;}
    virtual void    SetRotationVelocity(float v);// { mfRotation = v * K9_ROTATION_RATE;}
    virtual void    SetMoveVelocity(float v);// { mfMoveVelocity = v * K9_MOVE_RATE;}
    virtual float   GetRadius();
    virtual bool    AimAutoToEnemy(SPoint* ptEnemy);
    //자동으로 저방향으로 포탄을 발사한다.
	virtual bool    AimAutoToEnemy(CSprite *pEnemy);
    virtual void    ChangeCameraType();
    virtual int     GetFirePerSec();// { return 500;}    //폭탄을 시간당 쏠수있는 능력
    virtual float   GetMaxDefendPower();// { return 100;}      //방어력
    virtual float   GetMaxAttackPower();//공격력
    virtual bool    CanFireMissile();
    virtual CSprite* GetTargetBySensor();
    virtual void    PointOfImpact(SPoint* ptImpact,SVector& vtOutHeaderDir,bool byVecotr = false);
    //탱크가 로테이션될때 해더는 로테이션되지 않게 맞자.
    virtual void DontRotationHeader(float fAngle);
	//End CSprite Virtual
    //---------------------------------------------------------------------

	
    //bNeedParticle 안보이는데서 폭탄을 쏘았을 경우 파티클을 넣지 말자.
    virtual void  NewMissile(SPoint& ptNow,SVector& vtDir,SVector& vDirAngle,bool bNeedParticle = true);
    virtual void  NewDivideMissile(int nDivIndex,CSprite* pTarget,int nBombID,SPoint& ptNow,SVector& vtDir,SVector& vDirAngle);
    virtual void  FireMissile();
    virtual void  FireOrgMissile();
    
    virtual void TakeRunTimeBomb(int nItemID,int nCount);
    
    //포의 현재 위아래 각, 좌우각을 알아온다.
	virtual void GetPoAngles(int nInIndex,float* pOutWAngle,float* pOutHAngle,float* pOutUpPos,float* pOutDownPos,SVector *pvtDir);
    virtual void CampactAnimaion(int nTime);
    virtual void CampactAnimaionHeader(int nTime);
    virtual void CampactAnimaionBody(int nTime);
    virtual void OnCompletedUpdated(RUNTIME_UPGRADE* prop,CArchive* pReadArc = NULL);
    
    //Rada에서 해더 방향을 나타낸다.
    float GetBodyAngle() { return mpModel->orientation[1];};
    
    //Rada에서 해더의 포의 각도를 나타낸다.
	float GetHeaderPoAngle() { return mfModelHeaderAngle;}
   
    void SetCurrentSelMissaileInfo(USERINFO* v);
    void SetCurrentSelMissaileInfo(int nBombID);
    
    float* GetHeaderPosition();
    
#if TestPoStart_F
    void TestPoStart();
#endif
    
    virtual CSprite* GetAttackTo();
    
    virtual float GetCameraAngle();//현재 바라보는 카메라 각도를...
    virtual void SetCameraAngle(float fA); //현재 바라보는 카메라 각도를...
    virtual bool VisibleByCamera();
    float CatRotation(float fNowAngle,float fToAngle);
    
    virtual float GetInvisiblePoUpAngle() { return mfAutoMovePo_ToUpDownAngle;} //포신이 위아래 움직일수 있는 타입이면 0.f, CET8같이 고정이면

    
    //Gun
    int GetTargetPos(SPoint *pPosition,SPoint *pToOutPosion,SVector* pOutDirection);
    bool CanFireGun();
    void FireGun();
    void FireGunInvisible();
    virtual void NewGun(SPoint& ptNow,bool bNeedParticle);

    virtual void ParsMisailStartPos();
    virtual void GetFirePos(int nIndex,float** pOutFront,float** pOutBack );
protected:
    void  SetK9CameraType(int v);
    
    //Start Begin Render
    //포의 업다운을 그리기 위해서 계산을 한다.
    void  RenderBeginMovePo_UpDown();
    //파이어 하면 탱크머리가 뒤로 밀렸다가 앞으로 오게 한다.
    void  RenderBeginFireAnimation(int nTime,float fMaxMove);
    //자동 타겟으로 포를 적을 조준한다.
	void  RenderBeginAutoMovePo_UpDown(int nTime);
    //자동 타겟으로 적을 향해 탱크의 해더를 회전한다.
	void  RenderBeginAutoTurnHeader(int nTime);
    //End Begin Render
    
    
    //탱크의 방향과 위치가 변경될때 마다. 지형에 따라 각도와 탱크 해더의 위치가 달라진다.
	virtual int	ArrangeSprite();
    
	//탱크의 해더위치를 조정한다.
	virtual void ArrangeHeaderSprite(int xHeader,int yHeader,int zHeader);

    //fAngle 의 각도는 반드시 AngleArrange(fAngle)의 값이여야 한다.
    void  SetHeaderRotation(float fAngle);
    
    
	//좌의 바닥의 정점의 좌표를 array로 알아온다.
	void  GetMaxMinYPoint(SPoint *arrPoint,int nSize,int *pnIndexMin,int *pnIndexMax);
	
	/**
	 탱크의 좌우 바퀴의 가장자리의 지면의 점들을 계산한다.
	 */
	int CalculateBootomPoint(float fXHalf,float fZHalf,float angle,
							  SPoint* R0,
							  SPoint* R1,
							  SPoint* R2,
							  SPoint* R3,
							  SPoint* R4);	
	
	
    
    virtual void HandleCamera(float fUpDownVector,float fTurnVector);

#ifdef ACCELATOR
    virtual void SetAccelatorUpVector(float fv);
    virtual void HandleAccelCamera();
#endif //ACCELATOR
    //--------------------------
	//자동화 타겟으로 ....
    //자동으로 해더가 움직이고 있는지 확인한다.
#if OBJMilkShape
	virtual bool isMovingAutoHeader() { return ( mfModelHeaderRotVector != 0.f || mbAutoMovePo_UpDown);}
    
    //자동으로 포가 움직이는지를 셋한다.
	void SetAutoMovePo_UpDown(bool v) { mbAutoMovePo_UpDown = v;}
#else
    virtual bool isMovingAutoHeader() { return ( mfModelHeaderRotVector != 0.f || mfAutoMovePo_UpDonwDir != 0.f);}
#endif
    
#if DEBUG 
	void DisplayBottomStatus();
#endif
    //포가 자동으로 Up Down해야 하는 각도인지를 알아온다.
    virtual void CheckAutoMove_PoUpDown();
    //--------------------------
    

    virtual int GetCurrentAttackPerTime() { return m_nCurrentPerTime;}
    virtual bool IsTabIn(CPicking* pPick,SPoint& nearPoint,SPoint& farPoint,int nPosX,int nPosY);
    virtual bool IsTabInZeroGroup(CPicking* pPick,int nPosX,int nPosY);

    virtual bool AvoidDontGoArea();
    
    //보이지 않는 영역에서는 방향을 바로 정해서 쏜다.
    virtual void FireMissileInVisible();
    virtual void FireOrgMissileInVisible();
    
    virtual void SetAniRuntimeBubble(CAniRuntimeBubble* v);
    virtual CAniRuntimeBubble* GetAniRuntimeBubble();
    virtual float GetMaxScore();
    
    virtual void ResetPower(bool bRunTimeUpgrade = false);
    
    //어떤 행위가 필요할때... 사용하자.
    virtual void SetNeedExecuteCmd(long lCmd) { mExcuteCmd = lCmd;}
    virtual long GetNeedExecuteCmd() { return mExcuteCmd;}
    virtual void ExcuteCmd();
    virtual unsigned long GetBirthTime() { return mlBirthTime;}
    virtual void Serialize(CArchive& ar,bool bWrite);
    virtual void ArchiveMultiplay(int nCmd,CArchive& ar,bool bWrite);
    //객체가 메인타워 안쪽에 있을때 버블 업그레이드가 되어 있으면 데미지를 적개 주어야 한다.
    virtual short GetMainTowerBubbleCntInRange() { return mshMainTowerBubbleCntInRange;}
    

protected:
    
    //Body Model And Header Model
    CMyModel*	mpModel;
    
#if OBJMilkShape
	CMS3DModel* mpModelHeader;
#else
    CTankModelASCII* mpModelHeader;
#endif
    
    
	//탱크해더의 회전 벡터를 나타낸다. 180보다 크면 오른쪽 왼쪽으로 돌지 나타낸다.
	float   mfModelHeaderRotVector; 	
	float   mfModelHeaderAngle;


    int     mnPoUpDownState; //0 움직이지 않음, 1 : 위로 움직임, -1: 아래로 움직임
#if OBJMilkShape
	int		mnPoAniStep; //포신의 애니메이션 상태를 위에서 아래로 0~19, 아래에서위로 19~39
#endif


	//--------------------------
	//자동화 타겟으로 ....
	//--------------------------
	float	mfAutoTurnAHeader_ToAngle;		//이각도 방향으로 이동
	float   mfAutoMovePo_ToUpDownAngle;
#if OBJMilkShape
	bool    mbAutoMovePo_UpDown;
#endif
	float   mfAutoMovePo_UpDonwDir; //-1아래 1위방향으로 이동
	//-----------------------
    
    //-------------------------------------------
    //Fire Animation
    //폭탄을 발사하면 뒤로 밀렸다가 원위치로 돌아오는 효과
    //-------------------------------------------
    float mHeaderPosition[3]; 
    float mFireAnimationVector; // x < 0 (뒤의 방향), x > 0 (앞의 방향), x = 0 (정지상태)
    float mFireMovedHeader;
    
    //ArrangeSprite 
    SPoint  mPSRightBP[5];
	SPoint  mPSLeftBP[5];
    float   mfHalfX;
    float   mfHalfZ;
    
    int SGL_PO_STEP;             //36
    int SGL_PO_HALFSTEP;         //18
    
    
    //주인공일때 BOMB_K9
    USERINFO*     mpCurrentSelMissaileInfo; //현재 선택된 미사일 (폭탄)아이디 Default = 0;
    SPoint mptLastPos; //마지막으로 정상적인 위치를 기억한다.
    
    int mnPoStartCnt;
    int **mptStartMissile;  //미사일 시작위치.
    int mnPoIndex;
    
    int              mnGunStartCnt;
    int              **mptStartGun;
    
    CAniRuntimeBubble* mpBubbleParticle;
    
    
    //Compact
    float           mnCompactAni_Header_HAngle;
    float           mnCompactAni_Header_WAngle;
    SPoint          mptCompactAni_Header_StartPos;
    
    int             mnCompactAni_EscapeTime;
    float           mnCompactAni_Body_HAngle;
    float           mnCompactAni_Body_WAngle;
    SPoint          mptCompactAni_Body_StartPos;
    
    bool            mbOnAmiedNeedFire;
    
    long            mExcuteCmd;
    unsigned long   mnFireInterval; //너무 빠르게 발사하면 겹쳐보인다.
    int             mnMapID; //마지막 달맵은 절볔이 없이 그냥 갈수 있게 해주자.

    SPoint mBeforeOwnerPos;
    SPoint mBeforeTargetPos;
    float  mBeforeLenght;
    int    mBeforeResult;
    unsigned long             mnFireGunInterval; //너무 빠르게 발사하면 겹쳐보인다.
    PROPERTY_BOMB   mGunProperty;
    
    unsigned long mlBirthTime;
    
    short         mshMainTowerBubbleCntInRange; //객체가 메인타워 안쪽에 있을때 버블 업그레이드가 되어 있으면 데미지를 적개 주어야 한다.
    SPoint        mptUserClickForInvisible; //아보이는 영역에서 상대방 사용자가 클릭했을때 기억했다가 나중에 폭탄 발사시 이것을 사용하자.
    
#ifdef ACCELATOR
    //카메라를 sglNormalize을 해버리면 카메라값이 변경되기 때문에
    //아래의 것처렴 원래의 값을 대입하여 이것을 기준으로 하였다.
    SVector           mAccelatorViewDirBefor;
//    float           mfAccelatorViewDirBeforX;
//    float           mfAccelatorViewDirBeforZ;
//    float           mfAccelatorViewDirBeforY;
    
    float           mfAccelatorViewDirY;
    float           mfAccelatorViewDirVector;
#endif //ACCELATOR

};


#endif //_CK9SPRITE_H
