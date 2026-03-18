/*
 *  CActor.h
 *  SongGL
 *
 *  Created by 호학 송 on 10. 11. 17..
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _SPRITE_H
#define _SPRITE_H
#include <vector>
#include "sGLDefine.h"
#include "ARender.h"
#include "CMS3DModel.h"
#include "CAniLoader.h"
#include "CModelMan.h"
#include "CModelBound.h"
#include "CProperty.h"
#include "CUserInfo.h"
#include "CArchive.h"

//하단에 슬롯이 존재하면 슬롯에 좀 가린다.
//#define NormalViewMinY  8.0f
//#define NormalViewMaxY  60.0
//#define NormalViewDiv   18.4f // 8 + (60-8) / 5   => Min과 Max의 크기를 5등분한 값.

//하단에 슬롯이 존재하면 적당한 뷰이다.
#define NormalViewMinY  8.0f
//#define NormalViewMaxY  50.0f
#define NormalViewMaxY  50.0f
#define NormalViewDiv   16.4f // 8 + (60-8) / 5   => Min과 Max의 크기를 5등분한 값.


#define MAXFirePerSec 2000 //연속사격을 쏠때 4초

typedef enum {
    NETWORK_NONE,
    NETWORK_MYSIDE, //내팀
    NETWORK_OTHERSIDE, //상대방팀
}NETWORK_TEAMTYPE;

class CBomb;
class AAICore;
class IAction;
class CModelMan;
class IHWorld;
class CSpriteStatus; //에너지 및 기타 상태
class AParticle;
class CPicking;
class CAniRuntimeBubble;
class CUserInfo;
class CCircleUpgradeMark;
class CSprite : public ARender
{
public:
	//생성자 
	//nModelID : 모델 아이디를 가지고 loadresource 에서 어떠한 밀크쉐이프 모델을 읽어오는지를 알아낸다.
	CSprite(unsigned int nID,unsigned char cTeamID,int nModelID,IAction *pAction,IHWorld* pWorld,PROPERTY_TANK* pProperty,NETWORK_TEAMTYPE NetworkTeamType);
	
	//소멸자
	virtual ~CSprite();
	
	//밀크쉐이프의 모델을 읽어 온다.
	virtual int LoadResoure(CModelMan* pModelMan);
	
	//모델이 만들어지면 모델의 초기화를 한다. 
	//pPosition : 초기 위치
	//pvDirection : 모델의 방향
	virtual int Initialize(SPoint *pPosition,SVector *pvDirection);
	
    
    //CoreThread를 안에서 RenderBeginCore를 실행한다.
    virtual void RenderBeginCore(int nTime);
    
    //보이지 않을때는 RenderBegin 가 실행되지 않아서 자동으로 위치이동 되는 명령어의 움직임이 적용이 안된다.
    //그래서 코어안에서 안보이는 객체들은 여기서 실행하여준다.
    virtual void RenderBeginCore_Invisiable(int nTime); 

    
	//렌더링을 하기전에 먼져 수행해야 할 사항들을 기술한다.
	//즉 움직임, 회전 등의 연산을 하여 모델에 적용을 한다.
	//순수하게 데이터처리만 한다. 렌더링 처리는 RenderBegin에서 한다.
	virtual int RenderBegin(int nTime);
	
	//현재 액터에 맞게 카메라를 재정의 한다.
	//즉 액터의 능력치에 멀리 볼수 있는 한계를 정의 할수 있다.
	virtual void SetActor(void* pCamera);
	
	//현재 위치의 포인터를 얻어온다.
	virtual void GetPosition(SPoint* pOut) { memcpy(pOut, &mPosition, sizeof(SPoint));}
    

	//현제 위치를 재정의 한다.
	virtual bool SetPosition(SPoint* sp);
	
	//현재 모델의 방향의 포인터를 얻어온다.
	virtual void GetModelDirection(SVector* pOut) { memcpy(pOut,&mvViewDirection,sizeof(SVector));}
	
	//현재 모델의 방향의 재정의 한다.
	virtual void    SetModelDirection(SVector* pvd);
	
	//카메라 정보를 가져온다.
	virtual SGLCAMERA* GetCamera() { return mpCamera;}
	
	//카메라 타입을 가져온다. 정면에서 본다, 뒤에서 본다 등등의 타입
	virtual int GetCameraType();
	
	//카메라 타입 정보를 재정의 한다. 정명에서본다, 뒤에서 본다 등등의 타입
	virtual void SetCameraType(int v);
	
    
    //런타임시 폭탄 아이템을 얻었을 경우.
    virtual void TakeRunTimeBomb(int nItemID,int nCount);
    
    //탱크가 로테이션될때 해더는 로테이션되지 않게 맞자.
    virtual void DontRotationHeader(float fAngle) {};
    
    virtual void    SetRotationVelocity(float v);
    virtual void    SetToRotationAngle(float v);
    virtual float   GetRotationVelocity();
    virtual float   GetToRotationAngle();
    
    virtual void SetMoveDirection(SVector* v);
    virtual void GetMoveDirection(SVector* outv);
    virtual void SetMoveVelocity(float v); 
    virtual float GetMoveVelocity();
    virtual CSprite* GetAttackTo() { return NULL;}
	
	//모델의 아이디를 받아온다.
	virtual int GetModelID() { return mnModelID;}	
	
	//이번트 함수이다.
	virtual int OnEvent(SGLEvent *pEvent);
	
	//충돌을 검사한다.
	virtual bool CollidesBounding(CSprite* Sprite,bool bExact);
	
	//모델 포인터를 가져온다.
	virtual void* GetModel() { return 0;};
	
	virtual	IAction* GetAction() { return mpAction;}
	
    
    virtual SPRITE_STATE GetState() { return mState;}
    
    virtual void       SetState(SPRITE_STATE v);
    
    virtual void HandleCamera(float fUpDownVector,float fTurnVector);
    virtual float GetCameraAngle() { return 0.f;} //현재 바라보는 카메라 각도를...
    virtual void SetCameraAngle(float fA) {} //현재 바라보는 카메라 각도를...
    
    
    //virtual void BeginTurnAndMove(int nTime);
    
    virtual bool IsVisibleRender() { return m_bIsVisibleRender;}
    
    virtual void SetVisibleRender(bool v);
    
    virtual float* GetOrientaion() { return 0;};
    
    //탱크의 Attack의 방향은 GetOrientation()[2] + Header의 방향.
    virtual float GetOrientationToAttack() {
        float* f = GetOrientaion();
        return f ? f[2] : 0;
    }
    
    //AI함수
    //virtual void CmdMoveTo(CSprite *pSprite);
    //virtual void CmdAttackTo(CSprite *pSprite);
    virtual void Command(int nCommandID,void *pData);
    
    virtual bool IsGhost() { return false;}
    
    virtual bool  AimAutoToEnemy(SPoint* ptEnemy) { return true;}
    virtual bool AimAutoToEnemy(CSprite *pEnemy) { return true;}
    
    virtual bool isMovingAutoHeader() { return true;}
    
    virtual float GetMaxDefendPower() { return 0;} //방어력

    virtual float GetCurrentDefendPower() { return m_fCurrentDefendPower;} //방어력
    virtual void SetCurrentDefendPower(float f) { m_fCurrentDefendPower= f;}

    virtual int GetCurrentAttackPerTime() { return 0;}
    virtual float GetMaxAttackPower() { return 0;} //공격력
    virtual int GetFirePerSec() { return 0;} //폭탄을 시간당 쏠수있는 능력
    virtual int GetMaxAttackTime() { return MAXFirePerSec;} //4000 4초를 기준.
    virtual bool CanFireMissile();
    
    virtual void ChangeCameraType();
    virtual float GetColidedDistance() { return mfColidedDistance;}
    virtual void SetColidedDistance(float v) { mfColidedDistance = v;}
    virtual unsigned long GetBirthTime() { return 0l;}

    virtual float GetDefendRadianBounds();
    //현재 가장 공격하기 적합한 위치의 적을 찾는 센서로 타겟을 찾는다.
    virtual CSprite* GetTargetBySensor();
    
    virtual int GetType() { return mnType;}
    virtual void SetType(int v) { mnType = v;}
    
    virtual void PointOfImpact(SPoint* ptImpact,SVector& vtOutHeaderDir,bool byVecotr = false)
    {
        ptImpact->x = 0.0f;
        ptImpact->y = 0.0f;
        ptImpact->z = 0.0f;
        
    }
    
    virtual void CampactAnimaion(int nTime) {};
    
    //터졌을때 올라가는 Money수치...
    virtual float GetMaxScore() { return 10.f;}
    
    //현재 선택된 무기 아이디.
    virtual int GetCurrentWeaponID() { return mBombProperty.nID;}
    
    
    
    virtual float GetRadius() { return 0;}
    
    //폭탄에 명중하였을 경우 현재 죽느냐 사느냐를 알아온다.
    //1:정중앙에맞았다. 2:비껴맞았다.
    bool CompactToBomb(CBomb *pBomb,float fCompactGrade);
    
    float GetIntervalToCamera() { return m_fIntervalToActor;}
    void  SetIntervalToCamera(float v) { m_fIntervalToActor = v;}
    
    IHWorld* GetWorld() { return mpWorld;}
    
    void SetCameraUpDownPos(float v) { mfCameraUpDownPos= v;}
    
    virtual bool IsTabIn(CPicking* pPick,SPoint& nearPoint,SPoint& farPoint,int nPosX,int nPosY) { return false;}
    //주인공의 버텍스 크기가 너무커서 0그룹만 검사한다.
    virtual bool IsTabInZeroGroup(CPicking* pPick,int nPosX,int nPosY) { return false;}
    
    virtual void SetAutoTurnAndMoveToTarget(SPoint* ptPathPos,SPoint* ptLastTarget);
    
    AAICore*    GetAICore() { return mpAICore;}
    void DeleteAICore();
    PROPERTY_TANK* GetPropertyTank() { return &mProperty;}
    
    virtual void ResetPower(bool bRunTimeUpgrade = false);
    
    CSpriteStatus* GetSpriteStatus() { return mpStatusBar;}
    
    virtual void SetAniRuntimeBubble(CAniRuntimeBubble* v) {};
    virtual CAniRuntimeBubble* GetAniRuntimeBubble() { return NULL;}
    virtual bool VisibleByCamera() { return true;}
    
    //어떤 행위가 필요할때... 사용하자.
    virtual void SetNeedExecuteCmd(long lCmd) { };
    virtual long GetNeedExecuteCmd() { return 0;}
    virtual void ExcuteCmd() {}
    
    virtual void* GetSomethingValue(int nID)  {return NULL;} //각 하위 클래스의 어떤 정보들을 가져오려고 하는데 아이디로 확장을 하자.
    virtual void  SetSomethingValue(int nID,void* v)  {} //각 하위 클래스의 어떤 정보들을 가져오려고 하는데 아이디로 확장을 하자.
    
    virtual void SetMainBlzTower(CSprite* pMainTower) {}; //적기지의 메인타워
    
    
    virtual bool IsMaking() { return false;}
    virtual void InitRuntimeUpgrade(); //에 선언해 준다.
    virtual void RuntimeUpgrading(int nTime); //RenderBegin or RenderBeginCore_Invisiable 에서 콜해준다.
    
    
    virtual void GetInfomation(float* pfOutDLevel,float* pfOutALevel,float* pfOutDPower,float* pfOutAPower);
    
    virtual void Serialize(CArchive& ar,bool bWrite);
    virtual void ArchiveMultiplay(int nCmd,CArchive& ar,bool bWrite);
    
    bool IsCurrentBombMissileType() {
        if(mBombProperty.nBombType == 1 || mBombProperty.nBombType & 0x1000) return true; //미사일이나 레이져 총 같은 타입이면.
        return false;
    }
    
    virtual int GetBoosterCnt() { return 0;}
    virtual void GetBoosterPos(int nIndex,float** pOutFront,float** pOutBack ) {}
    virtual void GetFirePos(int nIndex,float** pOutFront,float** pOutBack ) {}
    virtual void GetFireGunPos(float** pOutFront){}
    //객체가 메인타워 안쪽에 있을때 버블 업그레이드가 되어 있으면 데미지를 적개 주어야 한다.
    virtual short GetMainTowerBubbleCntInRange() { return 0;}
    
//    vector<USERINFO*>* GetRuntimeUpgrade() { return mpRuntimeUpgradeList;}
    virtual bool SetRuntimeUpgrade(int nID);
    virtual void OnCompletedUpdated(RUNTIME_UPGRADE* prop,CArchive* pReadArc = NULL);
    
    bool IsReliveEffect() { return mnReliveEffect != 0? true : false;}
    void StartReliveEffect();

    bool CameraComact(SPoint *pPoint,float* pOutY);
    
    unsigned int    GetID()       { return mnID;}
    
    unsigned char   GetTeamID()   { return mcTeamID; }
    
    bool isMoving();
    bool isTurning();
    
    void SetAttackTo_Multiplay(int nIndex,CSprite* pSprite,float fDistance);
    void SetAttackToByGun_Multiplay(CSprite* pSprite);

    void SetMovePath(std::vector<int>* pv);
    vector<USERINFO*>* GetRuntimeUpgradList() { return mpRuntimeUpgradeList;}
    vector<USERINFO*>* GetRuntimeUpgradeingList() { return &mpRuntimeUpgradeingList;}
    
    virtual bool IsAssistance() { return false;}
    NETWORK_TEAMTYPE GetNetworkTeamType() { return mNetworkTeamType;} //멀티플레이어에서 사용된다.
    void SetNetworkTeamType(NETWORK_TEAMTYPE v) { mNetworkTeamType = v;} //멀티플레이어에서 사용된다.
    
    virtual bool CheckAlive();
    virtual int  GetAliveCount();
#ifdef ACCELATOR
    virtual void SetAccelatorUpVector(float fv){};
    virtual void HandleAccelCamera(){};
#endif //ACCELATOR
    
protected:
    //인공지능
    AAICore         *mpAICore; 
    
	SGLCAMERA       *mpCamera;

	//1:	mvViewDirection 방향으로 사물을 바라본다.   
	//2:	카메라를 위에서 자신을 바라본다.
	int             mnCameraType; 
    
	IAction*        mpAction;
    SPRITE_STATE	mState;
    
    bool            m_bIsVisibleRender;
    
    int             m_nCurrentPerTime;       //폭탄의 시간당 발사 수 조절한다.
    float           m_fCurrentDefendPower;  //현재 생명게이지
    float           m_fMaxAttackPower;
    bool            m_bHodeFire;
    //현재 주인공과의 거리 (가까운곳에서 폭탄이 터지면 터지는 효과음을 내기위함 등등)
    float           m_fIntervalToActor; 
    
    IHWorld*         mpWorld;
    
    CSpriteStatus*  mpStatusBar;
    

    AParticle*      mpShadowParticle;
    PROPERTY_TANK   mProperty;
    PROPERTY_BOMB   mBombProperty; //폭탄 Property를 계속해서 복사하지 않고 재사용하자는 취지임.
    
    
    float           mfMaxDefendPowerActor;
    
    float           mfCameraUpDownVector; //카메라 업다운.
    float           mfCameraUpDownPos;
    
    float           mfCameraTurnVector;
    float           mfCameraTurnPos;
    
    
    float           mfColidedDistance;//충돌되었을 경우 상대와의 거리.
    float           mfDefendRadianBounds;
    
    unsigned long   mnReliveEffect; //다시살아나는 효과를 표현한다.
    
    int mnType; //1:    1:K9시리즈 2:ET1시리즈(날라다니는) 3:빌딩  5:Fighter 0x1006:GhostSprite 0x1007:ItemSprite
    
    vector<USERINFO*> *mpRuntimeUpgradeList;
    vector<USERINFO*>  mpRuntimeUpgradeingList; //현재 업그레이드를 하고 있는 것은
    
    CCircleUpgradeMark* mpCircleUpgradeMark;
    
    NETWORK_TEAMTYPE      mNetworkTeamType; //멀티플레이어서 사용된다.
    unsigned char  mcAliveCount;
    
    
private:
	SVector         mvViewDirection;
	SPoint          mPosition;
	int             mnModelID; //밀크세이크의 어떤 모델인지
    unsigned int    mnID;       //객체의 시릴얼 아이디.
    unsigned char   mcTeamID;   //같은 수는 같은 팀..(1,2,4,8..... &연산을 하면 동맹)
};
#endif //_SPRITE_H