//
//  CET8.cpp
//  SongGL
//
//  Created by itsme on 13. 9. 9..
//
//

#include "CET8.h"
#include "IAction.h"
#include "CTankModelASCII.h"
CET8::CET8(unsigned int nID,unsigned char cTeamID,int nModelID,IAction *pAction,IHWorld* pWorld,PROPERTY_TANK* pProperty,NETWORK_TEAMTYPE NetworkTeamType) :
CET1(nID,cTeamID,nModelID,pAction,pWorld,pProperty,NetworkTeamType)
{
}

CET8::~CET8()
{
}



bool CET8::AimAutoToEnemy(SPoint* ptEnemy)
{
    if(!CanFireMissile()) return false;
    return false;
}


int CET8::RenderBegin(int nTime)
{
	//먼저 이동을 한ㄷ
	int nResult = E_SUCCESS;
	
    //프레임이 너무느리면 제껴져 버린다.
#if OBJMilkShape
    bool bIS = false;
#endif
    if(mState == SPRITE_RUN)
    {
        CSprite::RenderBegin(nTime);
        
        //파이어시 애니메이션을 해준다.
        RenderBeginFireAnimation(nTime,FireAnimationMax);
//        RenderBeginMovePo_UpDown();
        
        mpModel->updatef(nTime);
        
        //프레임이 너무느리면 제껴져 버린다.
        if(nTime > 150)
        {
            nTime = 150;
#if OBJMilkShape
            bIS = true;
#endif
        }
        
        mpModelHeader->SetmPoUpAngle(0.f);
        //프레임이 너무느리면 제껴져 버린다. *0.6f
        mpModelHeader->updatef(nTime * 0.6f);
        
        if(CanFireMissile() && GetAttackTo())
            mpAction->OnAutoAimed(this);	
        
#if OBJMilkShape
        //프레임이 너무느리면 제껴져 버린다.
        if(bIS)
            mpModelHeader->setElapsedTimeByFramef(3);
#endif
    }
    else if(mState == BOMB_COMPACT || mState == BOMB_COMPACT_ANI)
    {
        CampactAnimaion(nTime);
        mpModel->updatef(nTime);
        mpModelHeader->updatef(nTime);
        
    }
	return nResult;
}

void CET8::RenderBeginCore_Invisiable(int nTime)
{
    //    bool bNon = true;
    if(mState == SPRITE_RUN)
    {
        
        CSprite::RenderBeginCore_Invisiable(nTime);
        
        //자동으로 적 탱크로 해더를 돌려보자. (가상으로 돌린다.)
        RenderBeginAutoTurnHeader(nTime * .3f);
        
        //리소스가 스레드 안쪽에서 생성되기 때문에 이미지가 할당 안된다.
        FireMissileInVisible();
        
        
    }
    else if(mState == BOMB_COMPACT || mState == BOMB_COMPACT_ANI)
    {
        SetState(SPRITE_READYDELETE);
    }
}