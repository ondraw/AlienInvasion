//
//  CParticleEmitterMan.cpp
//  SongGL
//
//  Created by 송 호학 on 12. 4. 22..
//  Copyright (c) 2012년 __MyCompanyName__. All rights reserved.
//

#include "CParticleEmitterMan.h"
//#include "CParticleEmitter.h"

CParticleEmitterMan::CParticleEmitterMan()
{
}

CParticleEmitterMan::~CParticleEmitterMan()
{
    list<CParticleEmitter*>::iterator b = mlstPartice.begin();
    list<CParticleEmitter*>::iterator e = mlstPartice.end();
    for (list<CParticleEmitter*>::iterator it = b; it != e; it++) {
        delete *it;
    }
}

int CParticleEmitterMan::Initialize(SPoint *pPosition,SVector *pvDirection)
{
    return E_SUCCESS;
}

int CParticleEmitterMan::RenderBegin(int nTime)
{
    list<CParticleEmitter*>::iterator b = mlstPartice.begin();
    list<CParticleEmitter*>::iterator e = mlstPartice.end();
    for (list<CParticleEmitter*>::iterator it = b; it != e; ) {
        CParticleEmitter* em = *it;
        em->RenderBegin(nTime);
        //수명이 다한 파티클모음은 지운다.
        if(em->mnTotalTime >= em->mnTotalLiveTime)
        {
            delete em;
            it = mlstPartice.erase(it); //지운다.
            continue;
        }
        it++;
    }
    return E_SUCCESS;
}

int CParticleEmitterMan::Render()
{
    glEnableClientState (GL_COLOR_ARRAY);
//    glEnable(GL_POINT_SMOOTH);
    
    list<CParticleEmitter*>::iterator b = mlstPartice.begin();
    list<CParticleEmitter*>::iterator e = mlstPartice.end();
    for (list<CParticleEmitter*>::iterator it = b; it != e; it++) {
        CParticleEmitter* em = *it;
        em->Render();
    }
    
    glDisableClientState (GL_COLOR_ARRAY);
//    glDisable(GL_POINT_SMOOTH);
    return E_SUCCESS;
}

int CParticleEmitterMan::RenderEnd()
{
    return E_SUCCESS;
}

int CParticleEmitterMan::OnEvent(SGLEvent *pEvent)
{
    return E_SUCCESS;
}

void CParticleEmitterMan::NewBomTail(SPoint* pPosition,SVector *pvDirection,float* StartColor)
{
    CParticleEmitter *pNewParticle = new CParticleEmitter();

//    float color[4];
//    color[0] = 0.7f;
//    color[1] = 0.7f;
//    color[2] = 0.7f;
//    color[3] = 0.5f;
    
    float colorV[4]; //파티클 변화값 랜덤값 (1~0) 사이값을 곱하기 때문에 더작아진다.
    colorV[0] = 0.1f;
    colorV[1] = 0.1f;
    colorV[2] = 0.1f;
    colorV[3] = 0.1f;
    
//    pNewParticle->Initialize(
//                             CParticleEmitter_Point, 
//                             pPosition, 
//                             pvDirection, 
//                             1000,          //1초 
//                             10,            //파티클 개수는 10개
//                             2.0f,          //파티클 사이즈
//                             300,           //하나의 파티클이 살아있는 시간 1초
//                             500,           //파티클이 살아 있는 시간의 변화값은 1초
//                             StartColor,         //파티클의 색
//                             colorV,        //색 변화량
//                             StartColor,         //색의 End ... (미적용)
//                             0.1,           //속도..
//                             0.01,          //속도 변화량
//                             15.0f,         //Roatate 변화량  
//                             0.3f           //시작시 파티클 생성될수 잇는 범위 (반지름)
//                             );
    pNewParticle->Initialize(
                             CParticleEmitter_Point,
                             pPosition,
                             pvDirection,
                             600,          //1초
                             5,            //파티클 개수는 10개
                             5.0f,          //파티클 사이즈
                             200,           //하나의 파티클이 살아있는 시간 1초
                             400,           //파티클이 살아 있는 시간의 변화값은 1초
                             StartColor,         //파티클의 색
                             colorV,        //색 변화량
                             StartColor,         //색의 End ... (미적용)
                             1.0,           //속도..
                             0.5,          //속도 변화량
                             0.0f,         //Roatate 변화량
                             0.7f           //시작시 파티클 생성될수 잇는 범위 (반지름)
                             );

    
    mlstPartice.push_back(pNewParticle);
}



void CParticleEmitterMan::NewBackGas(SPoint* pPosition,SVector *pvDirection)
{
    CParticleEmitter *pNewParticle = new CParticleEmitter();
    
    float StartColor[4];
    StartColor[0] = 0.7f;
    StartColor[1] = 0.7f;
    StartColor[2] = 0.7f;
    StartColor[3] = 0.5f;

    
    float colorV[4]; //파티클 변화값 랜덤값 (1~0) 사이값을 곱하기 때문에 더작아진다.
    colorV[0] = 0.1f;
    colorV[1] = 0.1f;
    colorV[2] = 0.1f;
    colorV[3] = 0.1f;

    pNewParticle->Initialize(
                             CParticleEmitter_Point,
                             pPosition,
                             pvDirection,
                             600,          //1초
                             30,            //파티클 개수는 10개
                             2.0f,          //파티클 사이즈
                             300,           //하나의 파티클이 살아있는 시간 1초
                             400,           //파티클이 살아 있는 시간의 변화값은 1초
                             StartColor,         //파티클의 색
                             colorV,        //색 변화량
                             StartColor,         //색의 End ... (미적용)
                             0.05,           //속도..
                             0.05,          //속도 변화량
                             0.0f,         //Roatate 변화량
                             1.0f           //시작시 파티클 생성될수 잇는 범위 (반지름)
                             );
    
    
    mlstPartice.push_back(pNewParticle);
}

void CParticleEmitterMan::NewFireGas(SPoint* pPosition,SVector *pvDirection)
{
    SVector fireDirection = *pvDirection;

    CParticleEmitter *pFireCoreParticle = new CParticleEmitter();
    CParticleEmitter *pFireParticle = new CParticleEmitter();
    CParticleEmitter *pSmokeParticle = new CParticleEmitter();

    float fireCoreStartColor[4] = {1.0f, 0.82f, 0.35f, 0.92f};
    float fireCoreColorV[4] = {0.06f, 0.08f, 0.10f, 0.08f};

    pFireCoreParticle->Initialize(
                             CParticleEmitter_Point,
                             pPosition,
                             &fireDirection,
                             320,
                             110,
                             7.0f,
                             170,
                             120,
                             fireCoreStartColor,
                             fireCoreColorV,
                             fireCoreStartColor,
                             1.45f,
                             0.55f,
                             0.0f,
                             0.55f
                             );

    float fireStartColor[4] = {0.98f, 0.48f, 0.08f, 0.78f};
    float fireColorV[4] = {0.12f, 0.16f, 0.10f, 0.14f};

    pFireParticle->Initialize(
                             CParticleEmitter_Point,
                             pPosition,
                             &fireDirection,
                             520,
                             150,
                             8.5f,
                             240,
                             180,
                             fireStartColor,
                             fireColorV,
                             fireStartColor,
                             1.15f,
                             0.70f,
                             0.0f,
                             1.05f
                             );

    SVector smokeDirection = fireDirection;

    float smokeStartColor[4] = {0.26f, 0.26f, 0.26f, 0.32f};
    float smokeColorV[4] = {0.10f, 0.10f, 0.10f, 0.08f};

    pSmokeParticle->Initialize(
                              CParticleEmitter_Point,
                              pPosition,
                              &smokeDirection,
                              760,
                              110,
                              9.5f,
                              340,
                              240,
                              smokeStartColor,
                              smokeColorV,
                              smokeStartColor,
                              0.28f,
                              0.14f,
                              0.0f,
                              1.20f
                              );

    mlstPartice.push_back(pFireCoreParticle);
    mlstPartice.push_back(pFireParticle);
    mlstPartice.push_back(pSmokeParticle);
}


void CParticleEmitterMan::NewRollTail(SPoint* pPosition,SVector *pvDirection)
{
    CParticleEmitter *pNewParticle = new CParticleEmitter();
    
    float StartColor[4];
    StartColor[0] = 0.1f;
    StartColor[1] = 0.1f;
    StartColor[2] = 0.1f;
    StartColor[3] = 0.5f;
    
    
    float colorV[4]; //파티클 변화값 랜덤값 (1~0) 사이값을 곱하기 때문에 더작아진다.
    colorV[0] = 0.1f;
    colorV[1] = 0.2f;
    colorV[2] = 0.2f;
    colorV[3] = 0.1f;
    
    pNewParticle->Initialize(
                             CParticleEmitter_Point,
                             pPosition,
                             pvDirection,
                             600,          //1초
                             5,            //파티클 개수는 10개
                             5.0f,          //파티클 사이즈
                             200,           //하나의 파티클이 살아있는 시간 1초
                             400,           //파티클이 살아 있는 시간의 변화값은 1초
                             StartColor,         //파티클의 색
                             colorV,        //색 변화량
                             StartColor,         //색의 End ... (미적용)
                             0.1,           //속도..
                             0.3,          //속도 변화량
                             0.0f,         //Roatate 변화량
                             1.7f           //시작시 파티클 생성될수 잇는 범위 (반지름)
                             );
    
    
    mlstPartice.push_back(pNewParticle);
}
