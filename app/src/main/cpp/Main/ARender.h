/*
 *  IActor.h
 *  SongGL
 *
 *  Created by 호학 송 on 10. 11. 10..
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _ASPRITE_H
#define _ASPRITE_H
#include <string.h>
#include "sGLDefine.h"
enum SPRITE_STATE { SPRITE_RUN = 0, SPRITE_BOMB_NEXT_COMPACT, BOMB_COMPACT, BOMB_COMPACT_ANI,SPRITE_READYDELETE,SPRITE_DELETED };

class AMesh;
class ARender
{
	
public:
	virtual ~ARender() {};
	virtual int Initialize(SPoint *pPosition,SVector *pvDirection) = 0;
	virtual int RenderBegin(int nTime) = 0;
	virtual int Render() = 0;
	virtual int RenderEnd() = 0;	
    virtual int OnEvent(SGLEvent *pEvent) = 0;
    
#ifdef ANDROID
    virtual void ResetTexture() {}
#endif

	
};
#endif //_ASPRITE_H