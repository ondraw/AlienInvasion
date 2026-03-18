//
//  CET2.h
//  SongGL
//
//  Created by 호학 송 on 12. 8. 31..
//
//

#ifndef SongGL_CET2_h
#define SongGL_CET2_h

#include "CET1.h"

class CET2AttackParticle;
class CTE1BoosterParticle;
class CET2 : public CET1
{
	
public:
	CET2(unsigned int nID,unsigned char cTeamID,int nModelID,IAction *pAction,IHWorld* pWorld,PROPERTY_TANK* pProperty,NETWORK_TEAMTYPE NetworkTeamType);
	virtual ~CET2();
	
//    virtual
    virtual int		LoadResoure(CModelMan* pModelMan);
    virtual int     Initialize(SPoint *pPosition,SVector *pvDirection);
//    virtual void    RenderBeginCore_Invisiable(int nTime);
    virtual int     RenderBegin(int nTime);
//    virtual void    RenderBeginCore(int nTime);
    virtual int     Render();
    
    virtual void    ArrangeHeaderSprite(int xHeader,int yHeader,int zHeader);
    virtual bool    CanFireMissile() { return true;} //CanFireMissile()가 true여야만 AimAutoToEnemy의 함수가 실행된다.
    virtual void    SetState(SPRITE_STATE v);
    virtual void    DontRotationHeader(float fAngle) { };
    virtual void    RenderBeginCore_Invisiable(int nTime);
    virtual void    RenderBeginCore(int nTime);
    bool IsAttactingToEn() { return mbAttackingToEn;}

    virtual void CampactAnimaion(int nTime);
    void CheckEnemyAttack();
    
    
    virtual bool    AimAutoToEnemy(SPoint* ptEnemy) {return false;}
    bool    AimAutoToEnemy(float fFar); //근접하면 바로 공격한다.
protected:
    
    float mfRenderRotation; //모델의 orientation을 조절하면 자동으로 움직일때 방향의 정보가 틀리기 때문에 그릴때만 잠깐 orientation을 변경하여 작업을 하자.

    SPoint mptEn; //매번 패스를 찾으면 퍼포먼스에 무리가 간다.
    
    int mbAttackingToEn; //공격을 하고 있음..
    
    CET2AttackParticle* mAttackParticle;

};

#endif
