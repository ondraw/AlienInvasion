/*
 *  CK9Sprite.h
 *  SongGL
 *
 *  Created by 호학 송 on 11. 1. 30..
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "CK9Sprite.h"


#ifndef _CET1_H
#define _CET1_H
 
class CTE1BoosterParticle;
class CET1 : public CK9Sprite 
{
	
public:
	CET1(unsigned int nID,unsigned char cTeamID,int nModelID,IAction *pAction,IHWorld* pWorld,PROPERTY_TANK* pProperty,NETWORK_TEAMTYPE NetworkTeamType);
	virtual ~CET1();
	
public:
    //---------------------------------------------------------------------
    //Start CSprite Virtual
	virtual int		LoadResoure(CModelMan* pModelMan);
	virtual int		Initialize(SPoint *pPosition,SVector *pvDirection);
    virtual bool    SetPosition(SPoint* sp);
    virtual int     Render();
    virtual void    SetState(SPRITE_STATE v);
    virtual void    SetVisibleRender(bool v); 
	//End CSprite Virtual
    //---------------------------------------------------------------------
    virtual int     ArrangeSprite();

protected:   
    CTE1BoosterParticle *mBoosterParticle;
};


#endif //_CET1_H
