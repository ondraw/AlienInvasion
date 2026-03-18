//
//  CAmbulance.h
//  SongGL
//
//  Created by itsme on 2014. 5. 22..
//
//

#ifndef __SongGL__CAmbulance__
#define __SongGL__CAmbulance__
#include "CTireCar.h"

class CAmbulance : public CTireCar
{
public:
    CAmbulance(unsigned int nID,unsigned char cTeamID,int nModelID,IAction *pAction,IHWorld* pWorld,PROPERTY_TANK* pProperty,NETWORK_TEAMTYPE NetworkTeamType);
	virtual ~CAmbulance();
    virtual int LoadResoure(CModelMan* pModelMan);
    virtual int	 Initialize(SPoint *pPosition,SVector *pvDirection);
    virtual void    RenderBeginCore(int nTime);
    virtual void    RenderBeginCore_Invisiable(int nTime);

    
    virtual int RenderBegin(int nTime);
    virtual bool CanFireMissile() { return false;}
    
    
    
    virtual void    ResetPower(bool bRunTimeUpgrade = false);
    virtual float   GetMaxDefendPower() { return mfMaxDefendPowerActor;}
    virtual float   GetCurrentDefendPowerEn() { return mfNowStatus;}
    virtual void    SetCurrentDefendPowerEn(float v);
    
protected:
    float       mfNowStatus;
};

#endif /* defined(__SongGL__CAmbulance__) */
