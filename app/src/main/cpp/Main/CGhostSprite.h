//
//  CGhostSprite.h
//  SongGL
//
//  Created by Songs on 11. 7. 25..
//  Copyright 2011 thinkm. All rights reserved.
//
#ifndef _CGHOSTSPRITE_H
#define _CGHOSTSPRITE_H
#include "CSprite.h"

class CGhostSprite : public CSprite
{
public:
    CGhostSprite(unsigned int nID,unsigned char cTeamID,int nModelID,IAction *pAction,IHWorld* pWorld);
    virtual ~CGhostSprite();    
    virtual int RenderBegin(int nTime);
	virtual int Render() { return E_SUCCESS;}
    virtual int RenderEnd() { return E_SUCCESS;}
    virtual int OnEvent(SGLEvent *pEvent) { return E_SUCCESS;}
    virtual bool CollidesBounding(CSprite* Sprite,bool bExact);
    virtual SPRITE_STATE GetState() { return SPRITE_RUN;} //항상살아 있다.
    virtual bool IsGhost() { return true;}
};

#endif //_CGHOSTSPRITE_H