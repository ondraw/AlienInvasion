//
//  CParticleEmitterMan.h
//  SongGL
//
//  Created by 송 호학 on 12. 4. 22..
//  Copyright (c) 2012년 __MyCompanyName__. All rights reserved.
//

#ifndef SongGL_CParticleEmitterMan_h
#define SongGL_CParticleEmitterMan_h

#include "sGL.h"
#include "ARender.h"
#include <list>
#include "CParticleEmitter.h"

using namespace std;
//class CParticleEmitter;
class CParticleEmitterMan : public ARender
{
public:
    CParticleEmitterMan();
    ~CParticleEmitterMan();
    
    virtual int Initialize(SPoint *pPosition,SVector *pvDirection);
	virtual int RenderBegin(int nTime);
	virtual int Render();
	virtual int RenderEnd();	
    virtual int OnEvent(SGLEvent *pEvent);
    
    void    NewBomTail(SPoint* pPosition,SVector *pvDirection,float* StartColor);
    void    NewBackGas(SPoint* pPosition,SVector *pvDirection);
    void    NewFireGas(SPoint* pPosition,SVector *pvDirection);
    void    NewRollTail(SPoint* pPosition,SVector *pvDirection);
    
protected:
    list<CParticleEmitter*> mlstPartice;
    
};
#endif
