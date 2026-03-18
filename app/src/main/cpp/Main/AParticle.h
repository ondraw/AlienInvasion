//
//  AParticle.h
//  SongGL
//
//  Created by Songs on 11. 4. 13..
//  Copyright 2011 thinkm. All rights reserved.
//
#ifndef _AParticle_H
#define _AParticle_H
#include "sGLDefine.h"
#include "ARender.h"

class AParticle : public ARender
{
public:
    virtual ~AParticle() {} ;
    virtual SPRITE_STATE GetState() = 0;
    
    virtual void        SetState(SPRITE_STATE v) = 0;
    virtual void        GetPosition(SPoint* pOut) = 0;
    virtual void        RenderBeginCore(int nTime) = 0;
    
    
    //Shadow
    virtual void SetVisibleRender(bool v) {}
    virtual void SetReliveEffect(bool v) {}
};



#endif //_AParticle_H