//
//  CFighter.h
//  SongGL
//
//  Created by itsme on 2013. 12. 17..
//
//

#ifndef __SongGL__CFighter__
#define __SongGL__CFighter__

#include "CSprite.h"
#include "CMS3DModelASCII.h"
class CFighterTailParticle;
class CFighter : public CSprite
{
public:
    //생성자
	//nModelID : 모델 아이디를 가지고 loadresource 에서 어떠한 밀크쉐이프 모델을 읽어오는지를 알아낸다.
	CFighter(unsigned int nID,unsigned char cTeamID,int nModelID,IAction *pAction,IHWorld* pWorld,PROPERTY_TANK* pProperty,NETWORK_TEAMTYPE NetworkTeamType);
	
	//소멸자
	virtual ~CFighter();
	
	//밀크쉐이프의 모델을 읽어 온다.
	virtual int LoadResoure(CModelMan* pModelMan);
	
	//모델이 만들어지면 모델의 초기화를 한다.
	//pPosition : 초기 위치
	//pvDirection : 모델의 방향
	virtual int Initialize(SPoint *pPosition,SVector *pvDirection);
	
    
    //CoreThread를 안에서 RenderBeginCore를 실행한다.
    virtual void RenderBeginCore(int nTime);
    
    virtual int Render();
    
    //보이지 않을때는 RenderBegin 가 실행되지 않아서 자동으로 위치이동 되는 명령어의 움직임이 적용이 안된다.
    //그래서 코어안에서 안보이는 객체들은 여기서 실행하여준다.
    virtual void RenderBeginCore_Invisiable(int nTime);
    
    
	//렌더링을 하기전에 먼져 수행해야 할 사항들을 기술한다.
	//즉 움직임, 회전 등의 연산을 하여 모델에 적용을 한다.
	//순수하게 데이터처리만 한다. 렌더링 처리는 RenderBegin에서 한다.
	virtual int RenderBegin(int nTime);
    
    virtual int OnEvent(SGLEvent *pEvent);
    virtual void ResetPower(bool bRunTimeUpgrade = false);
    
    virtual bool SetPosition(SPoint* sp);
    virtual void SetModelDirection(SVector* pvd);
    virtual bool CollidesBounding(CSprite* Sprite,bool bExact);
    
    virtual int RenderEnd() {return E_SUCCESS;}
    virtual void* GetModel() { return mpModel;}
    virtual int ArrangeSprite();
    virtual float* GetOrientaion() { return mpModel->orientation;}
    
    virtual void SetMoveVelocity(float v);
    virtual float GetMoveVelocity();
    
//    void PosParse(char* sData,int* pCnt,int** &pStartPos);
    virtual void GetBoosterPos(int nIndex,float** pOutFront,float** pOutBack );
    virtual void GetFirePos(int nIndex,float** pOutFront,float** pOutBack );
    virtual void GetFireGunPos(float** pOutFront);

    virtual void* GetSomethingValue(int nID); //1:mfGuideDirectionX 2:mfGuideDirectionY
    virtual void  SetSomethingValue(int nID,void* v); //각 하위 클래스의 어떤 정보들을 가져오려고 하는데 아이디로 확장을 하자.
    
    virtual void SetMainBlzTower(CSprite* pMainTower) {mpMainTower = pMainTower;} //적기지의 메인타워
    
    virtual float GetRadius();
    
    virtual float   GetMaxDefendPower();// { return 100;}      //방어력
    virtual float   GetMaxAttackPower();//공격력
    virtual void    CampactAnimaion(int nTime);
    virtual void    SetVisibleRender(bool v);
    
    
    
    virtual void  FireMissile();
    virtual void  FireOrgMissile(int nIndex);
    virtual void  FireMissileInVisible();
    virtual void  FireOrgMissileInVisible(int nIndex);
    virtual void  NewMissile(int nIndex,SPoint& ptNow,SVector& vtDir,SVector& vDirAngle,bool bNeedParticle = true);
    virtual int   GetFirePerSec();
    virtual bool  VisibleByCamera();
    
    virtual float GetMaxScore();
    
    void GetPoAngles(int nInIndex,float* pOutWAngle,float* pOutHAngle,float* pOutUpPos,float* pOutDownPos,SVector *pvtDir);
    
    virtual bool CanFireMissile();
    
    bool CanFireGun();
    void FireGun();
    void FireGunInvisible();
    virtual void NewGun(SPoint& ptNow,bool bNeedParticle);
    virtual int  GetTargetPos(SPoint *pPosition,SPoint *pToOutPosion,SVector* pOutDirection,float* pOutLength); //레이져 총의 거리와 방향을 구해온다.
    virtual void ArchiveMultiplay(int nCmd,CArchive& ar,bool bWrite);
protected:
    CMS3DModelASCII* mpModel;
    
    int              mnBoosterCnt;
    int              **mpBoosterPos;
    
    int              mnPoStartCnt;
    int              **mptStartMissile;
    
    int              mnGunStartCnt;
    int              **mptStartGun;
    
    float            mfVelocity;


    float            mXAngle;
    float            mYAngle;
    
    CSprite*         mpMainTower;

    long             mnCompactAniTime;

    unsigned long             mnFireInterval; //너무 빠르게 발사하면 겹쳐보인다.
    unsigned long             mnFireGunInterval; //너무 빠르게 발사하면 겹쳐보인다.
    
    CFighterTailParticle **mppTailParticle;
    
    float           mfAttackAngle;
    float           mfAttackFar;
    
    PROPERTY_BOMB   mGunProperty;
};

#endif /* defined(__SongGL__CFighter__) */
