//
//  CDTower.h
//  SongGL
//
//  Created by 호학 송 on 13. 7. 30..
//
//

#ifndef __SongGL__CDTower__
#define __SongGL__CDTower__

#include "CSprite.h"
#include "CMS3DASCII.h"

class CTextureMan;
class CDTower : public CSprite
{
public:
    CDTower(unsigned int nID,unsigned char cTeamID,int nModelID,CTextureMan *pTextureMan,IAction *pAction,IHWorld* pWorld,PROPERTY_TANK* pProperty,NETWORK_TEAMTYPE NetworkTeamType);
	virtual ~CDTower();
    virtual int     LoadResoure(CModelMan* pModelMan);
    virtual int     Initialize(SPoint *pPosition,SVector *pvDirection);
    virtual void    RenderBeginCore_Invisiable(int nTime);
    virtual void    RenderBeginCore(int nTime);
    virtual int     RenderBegin(int nTime);
	virtual int     Render();
    virtual int     RenderEnd();
    virtual int     OnEvent(SGLEvent *pEvent);
    virtual bool    CollidesBounding(CSprite* Sprite,bool bExact);
    virtual float   GetRadius() { return mfRadian;}
    virtual float   GetMaxDefendPower();// { return 400.0f;}      //방어력 100 발을 맞아야 한다.
    virtual float   GetMaxAttackPower();
    virtual bool    IsTabIn(CPicking* pPick,SPoint& nearPoint,SPoint& farPoint,int nPosX,int nPosY);
    
    virtual bool    AimAutoToEnemy(CSprite *pEnemy);
    virtual bool    AimAutoToEnemy(SPoint* ptEnemy);
    virtual bool    isMovingAutoHeader() { return ( mfRotationVector != 0.f || mfPo_UpDonwDir != 0.f);}
    virtual int     GetFirePerSec();
    virtual void    FireMissile();
    virtual bool    VisibleByCamera();
    void            FireMissileInVisible();
    void            FireOrgMissileInVisible();
    void            FireOrgMissile();
    virtual void            NewMissile(SPoint& ptNow,SVector& vtDir,SVector& vDirAngle,bool bNeedParticle = true);
    
    //자동 타겟으로 적을 향해 탱크의 해더를 회전한다.
	void  RenderBeginAutoTurnHeader(int nTime);
    
    //포가 자동으로 Up Down해야 하는 각도인지를 알아온다.
    void CheckAutoMove_PoUpDown();

    virtual bool IsMaking() { return mnMakingTime;}
    
    //어떤 행위가 필요할때... 사용하자.
    virtual void SetNeedExecuteCmd(long lCmd) { mExcuteCmd = lCmd;}
    virtual long GetNeedExecuteCmd() { return mExcuteCmd;}
    virtual void ExcuteCmd();
    virtual void ResetPower(bool bRunTimeUpgrade = false);
    virtual void GetInfomation(float* pfOutDLevel,float* pfOutALevel,float* pfOutDPower,float* pfOutAPower);
    
    //객체가 메인타워 안쪽에 있을때 버블 업그레이드가 되어 있으면 데미지를 적개 주어야 한다.
    virtual short GetMainTowerBubbleCntInRange() { return mshMainTowerBubbleCntInRange;}
    virtual void OnCompletedUpdated(RUNTIME_UPGRADE* prop,CArchive* pReadArc = NULL);
    virtual void ArchiveMultiplay(int nCmd,CArchive& ar,bool bWrite);
    
protected:
    float   CatRotation(float fNowAngle,float fToAngle);
    void    GetPoAngles(int nPoIndex,float* pOutWAngle,float* pOutHAngle,float* pOutUpPos,float* pOutDownPos,SVector *pvtDir);
    void    CampactAnimaion(int nTime);
    
protected:
    CMS3DASCII *mpModel;
    sglMesh    *mpWorldMesh;
    float      mfRadian;
    

    float       mfRotationAngle;
	float       mfRotationVector;
    float       mfRotationToAngle;		//이각도 방향으로 이동
    
    float   mfPoUpAngle;
    float   mfPoUpToAngle;
	float	mfPo_UpDonwDir; //-1아래 1위방향으로 이동
    
    int     mnPoUpgrade;

    
    //미사일 발사 위치 정보
    int mnPoStartCnt;
    int **mptStartMissile;  //미사일 시작위치.
    int mnPoIndex;
    
    
    //Compact
    long             mnAniGGTime;
    float           mfAniBottomAngle;
    float           mfAniBottomY;
    float           mfAniBottomZ;
    
    float           mfAniHeaderAngle;
    float           mfAniHeaderY;
    float           mfAniHeaderX;

    //포가 위아래로 움직이지 않은 미사일포일 경우 움직임이 없을때는 발사를 하지 않음. 그래서 이플래그가 true일때는 미사일을 발사하게 했음.
    bool            mbOnAmiedNeedFire;
    
    unsigned int    mnMakingTime; //not zero : making time , zero : runtime
    
    long            mExcuteCmd;
    unsigned long mnFireInterval;

    short         mshMainTowerBubbleCntInRange; //객체가 메인타워 안쪽에 있을때 버블 업그레이드가 되어 있으면 데미지를 적개 주어야 한다.
    
    int          mnConstMakingTime;
};


#endif /* defined(__SongGL__CDTower__) */
