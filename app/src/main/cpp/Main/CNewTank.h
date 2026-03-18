//
//  CNewTank.h
//  SongGL
//
//  Created by 호학 송 on 2014. 3. 6..
//
//

#ifndef __SongGL__CNewTank__
#define __SongGL__CNewTank__
#include "CK9Sprite.h"

class CNewBoosterParticle;
class CNewTank : public CK9Sprite {
public:
	CNewTank(unsigned int nID,unsigned char cTeamID,int nModelID,IAction *pAction,IHWorld* pWorld,PROPERTY_TANK* pProperty,NETWORK_TEAMTYPE NetworkTeamType);
	virtual ~CNewTank();
	
public:
    //---------------------------------------------------------------------
    //Start CSprite Virtual
    virtual bool    SetPosition(SPoint* sp);
    virtual int     Render();
    virtual void    SetVisibleRender(bool v);
    virtual void    ParsMisailStartPos();
	//End CSprite Virtual
    //---------------------------------------------------------------------


    virtual int	 Initialize(SPoint *pPosition,SVector *pvDirection);
    virtual int  GetBoosterCnt() { return mnBoosterCnt;}
    virtual void GetBoosterPos(int nIndex,float** pOutFront,float** pOutBack );
    virtual void GetFirePos(int nIndex,float** pOutFront,float** pOutBack );
    virtual void GetFireGunPos(float** pOutFront);

    virtual void NewGun(SPoint& ptNow,bool bNeedParticle);
    virtual void SetState(SPRITE_STATE v);
    
    virtual void OnCompletedUpdated(RUNTIME_UPGRADE* prop,CArchive* pReadArc = NULL);
protected:
    int              mnBoosterCnt;
    int              **mpBoosterPos;
    
    CNewBoosterParticle *mpTailParticle;
};

#endif /* defined(__SongGL__CNewTank__) */
