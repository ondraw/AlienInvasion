//
//  CGhostSprite.cpp
//  SongGL
//
//  Created by Songs on 11. 7. 25..
//  Copyright 2011 thinkm. All rights reserved.
//

#include "CGhostSprite.h"


CGhostSprite::CGhostSprite(unsigned int nID,unsigned char cTeamID,int nModelID,IAction *pAction,IHWorld* pWorld) : CSprite(nID,cTeamID,nModelID,pAction,pWorld,NULL,NETWORK_NONE)
{
    mnType = ACTORTYPE_TYPE_GHOST;
}


CGhostSprite::~CGhostSprite()
{
    
}

int CGhostSprite::RenderBegin(int nTime)
{
    return E_SUCCESS;
}

//충돌값은 없다.
bool CGhostSprite::CollidesBounding(CSprite* Sprite,bool bExact)
{
    return false;
}