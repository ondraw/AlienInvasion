//
//  CParticleEmitterMan.cpp
//  SongGL
//
//  Created by 송 호학 on 12. 4. 22..
//  Copyright (c) 2012년 __MyCompanyName__. All rights reserved.
//

#include "CParticleEmitterMan.h"
//#include "CParticleEmitter.h"

static float ClampExplosionScale(float fScale)
{
    if(fScale < 0.6f) return 0.6f;
    if(fScale > 3.0f) return 3.0f;
    return fScale;
}

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
    SVector denseDirection = *pvDirection;
    denseDirection.y += 0.10f;

    CParticleEmitter *pDenseSmoke = new CParticleEmitter();
    CParticleEmitter *pMidSmoke = new CParticleEmitter();
    CParticleEmitter *pLightSmoke = new CParticleEmitter();

    float denseStartColor[4] = {0.10f, 0.10f, 0.10f, 0.28f};
    float denseColorV[4] = {0.04f, 0.04f, 0.04f, 0.08f};

    pDenseSmoke->Initialize(
                            CParticleEmitter_Point,
                            pPosition,
                            &denseDirection,
                            420,
                            3,
                            7.5f,
                            210,
                            110,
                            denseStartColor,
                            denseColorV,
                            denseStartColor,
                            0.30f,
                            0.12f,
                            18.0f,
                            0.45f
                            );

    SVector midDirection = *pvDirection;
    midDirection.y += 0.20f;

    float midStartColor[4] = {0.18f, 0.18f, 0.18f, 0.18f};
    float midColorV[4] = {0.06f, 0.06f, 0.06f, 0.08f};

    pMidSmoke->Initialize(
                          CParticleEmitter_Point,
                          pPosition,
                          &midDirection,
                          700,
                          4,
                          11.5f,
                          320,
                          180,
                          midStartColor,
                          midColorV,
                          midStartColor,
                          0.18f,
                          0.10f,
                          30.0f,
                          1.05f
                          );

    SVector lightDirection = *pvDirection;
    lightDirection.y += 0.34f;

    float lightStartColor[4] = {0.28f, 0.28f, 0.28f, 0.10f};
    float lightColorV[4] = {0.08f, 0.08f, 0.08f, 0.06f};

    pLightSmoke->Initialize(
                            CParticleEmitter_Point,
                            pPosition,
                            &lightDirection,
                            900,
                            2,
                            15.0f,
                            480,
                            220,
                            lightStartColor,
                            lightColorV,
                            lightStartColor,
                            0.10f,
                            0.08f,
                            44.0f,
                            1.65f
                            );

    mlstPartice.push_back(pDenseSmoke);
    mlstPartice.push_back(pMidSmoke);
    mlstPartice.push_back(pLightSmoke);
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

void CParticleEmitterMan::NewGroundExplosion(SPoint* pPosition,float fScale)
{
    float scale = ClampExplosionScale(fScale);

    SVector upDir = {0.0f, 1.0f, 0.0f};
    SVector diagonalDir = {0.0f, 0.55f, 0.45f};
    SVector smokeDir = {0.0f, 0.85f, 0.0f};

    CParticleEmitter *pFlash = new CParticleEmitter();
    CParticleEmitter *pFire = new CParticleEmitter();
    CParticleEmitter *pSmoke = new CParticleEmitter();
    CParticleEmitter *pDebris = new CParticleEmitter();

    float flashColor[4] = {1.0f, 0.95f, 0.70f, 0.92f};
    float flashVar[4] = {0.04f, 0.05f, 0.08f, 0.08f};
    pFlash->Initialize(
                       CParticleEmitter_Point,
                       pPosition,
                       &upDir,
                       (int)(220 * scale),
                       (int)(110 * scale),
                       10.5f * scale,
                       90,
                       70,
                       flashColor,
                       flashVar,
                       flashColor,
                       2.5f * scale,
                       1.2f * scale,
                       85.0f,
                       1.2f * scale
                       );

    float fireColor[4] = {1.0f, 0.45f, 0.10f, 0.82f};
    float fireVar[4] = {0.10f, 0.16f, 0.08f, 0.12f};
    pFire->Initialize(
                      CParticleEmitter_Point,
                      pPosition,
                      &diagonalDir,
                      (int)(560 * scale),
                      (int)(180 * scale),
                      12.0f * scale,
                      160,
                      120,
                      fireColor,
                      fireVar,
                      fireColor,
                      2.0f * scale,
                      1.15f * scale,
                      135.0f,
                      2.0f * scale
                      );

    float smokeColor[4] = {0.18f, 0.18f, 0.18f, 0.32f};
    float smokeVar[4] = {0.08f, 0.08f, 0.08f, 0.10f};
    pSmoke->Initialize(
                       CParticleEmitter_Point,
                       pPosition,
                       &smokeDir,
                       (int)(1200 * scale),
                       (int)(130 * scale),
                       16.0f * scale,
                       420,
                       260,
                       smokeColor,
                       smokeVar,
                       smokeColor,
                       0.75f * scale,
                       0.35f * scale,
                       42.0f,
                       2.4f * scale
                       );

    float debrisColor[4] = {0.62f, 0.52f, 0.34f, 0.70f};
    float debrisVar[4] = {0.18f, 0.14f, 0.10f, 0.10f};
    pDebris->Initialize(
                        CParticleEmitter_Point,
                        pPosition,
                        &diagonalDir,
                        (int)(650 * scale),
                        (int)(120 * scale),
                        5.8f * scale,
                        210,
                        130,
                        debrisColor,
                        debrisVar,
                        debrisColor,
                        2.4f * scale,
                        1.4f * scale,
                        175.0f,
                        1.6f * scale
                        );

    mlstPartice.push_back(pFlash);
    mlstPartice.push_back(pFire);
    mlstPartice.push_back(pSmoke);
    mlstPartice.push_back(pDebris);

    SVector dustDirs[4] = {
        { 1.0f, 0.10f, 0.0f},
        {-1.0f, 0.10f, 0.0f},
        { 0.0f, 0.10f, 1.0f},
        { 0.0f, 0.10f,-1.0f}
    };

    float dustColor[4] = {0.42f, 0.34f, 0.22f, 0.52f};
    float dustVar[4] = {0.10f, 0.08f, 0.06f, 0.10f};

    for(int i = 0; i < 4; i++)
    {
        CParticleEmitter *pDust = new CParticleEmitter();
        pDust->Initialize(
                          CParticleEmitter_Point,
                          pPosition,
                          &dustDirs[i],
                          (int)(920 * scale),
                          (int)(85 * scale),
                          14.0f * scale,
                          250,
                          180,
                          dustColor,
                          dustVar,
                          dustColor,
                          1.95f * scale,
                          1.0f * scale,
                          35.0f,
                          2.6f * scale
                          );
        mlstPartice.push_back(pDust);
    }
}

void CParticleEmitterMan::NewAirExplosion(SPoint* pPosition,float fScale)
{
    float scale = ClampExplosionScale(fScale);

    SVector burstDir = {0.0f, 0.15f, 1.0f};
    SVector sparkDir = {0.0f, 0.05f, 1.0f};
    SVector smokeDir = {0.0f, 0.45f, 0.0f};

    CParticleEmitter *pFlash = new CParticleEmitter();
    CParticleEmitter *pFire = new CParticleEmitter();
    CParticleEmitter *pSpark = new CParticleEmitter();
    CParticleEmitter *pSmoke = new CParticleEmitter();

    float flashColor[4] = {1.0f, 0.96f, 0.75f, 0.88f};
    float flashVar[4] = {0.05f, 0.04f, 0.10f, 0.10f};
    pFlash->Initialize(
                       CParticleEmitter_Point,
                       pPosition,
                       &burstDir,
                       (int)(190 * scale),
                       (int)(120 * scale),
                       10.0f * scale,
                       85,
                       60,
                       flashColor,
                       flashVar,
                       flashColor,
                       2.9f * scale,
                       1.4f * scale,
                       180.0f,
                       0.9f * scale
                       );

    float fireColor[4] = {1.0f, 0.48f, 0.12f, 0.72f};
    float fireVar[4] = {0.12f, 0.16f, 0.10f, 0.12f};
    pFire->Initialize(
                      CParticleEmitter_Point,
                      pPosition,
                      &burstDir,
                      (int)(540 * scale),
                      (int)(190 * scale),
                      10.5f * scale,
                      150,
                      120,
                      fireColor,
                      fireVar,
                      fireColor,
                      2.7f * scale,
                      1.4f * scale,
                      220.0f,
                      1.7f * scale
                      );

    float sparkColor[4] = {1.0f, 0.78f, 0.22f, 0.74f};
    float sparkVar[4] = {0.06f, 0.08f, 0.08f, 0.12f};
    pSpark->Initialize(
                       CParticleEmitter_Point,
                       pPosition,
                       &sparkDir,
                       (int)(620 * scale),
                       (int)(130 * scale),
                       4.8f * scale,
                       170,
                       130,
                       sparkColor,
                       sparkVar,
                       sparkColor,
                       3.5f * scale,
                       1.9f * scale,
                       235.0f,
                       1.2f * scale
                       );

    float smokeColor[4] = {0.22f, 0.22f, 0.22f, 0.24f};
    float smokeVar[4] = {0.08f, 0.08f, 0.08f, 0.08f};
    pSmoke->Initialize(
                       CParticleEmitter_Point,
                       pPosition,
                       &smokeDir,
                       (int)(980 * scale),
                       (int)(120 * scale),
                       13.0f * scale,
                       360,
                       220,
                       smokeColor,
                       smokeVar,
                       smokeColor,
                       0.95f * scale,
                       0.38f * scale,
                       135.0f,
                       1.9f * scale
                       );

    mlstPartice.push_back(pFlash);
    mlstPartice.push_back(pFire);
    mlstPartice.push_back(pSpark);
    mlstPartice.push_back(pSmoke);
}

void CParticleEmitterMan::NewElectricGroundExplosion(SPoint* pPosition,float fScale)
{
    float scale = ClampExplosionScale(fScale);
    NewGroundExplosion(pPosition, scale * 1.35f);

    SVector arcDirs[6] = {
        { 1.0f, 0.35f, 0.0f},
        {-1.0f, 0.35f, 0.0f},
        { 0.0f, 0.35f, 1.0f},
        { 0.0f, 0.35f,-1.0f},
        { 0.7f, 0.55f, 0.7f},
        {-0.7f, 0.55f, 0.7f}
    };

    float arcColor[4] = {0.62f, 0.95f, 1.0f, 0.88f};
    float arcVar[4] = {0.12f, 0.05f, 0.08f, 0.10f};
    float coreColor[4] = {0.92f, 1.0f, 1.0f, 0.96f};
    float coreVar[4] = {0.05f, 0.02f, 0.02f, 0.08f};

    CParticleEmitter *pCore = new CParticleEmitter();
    SVector upDir = {0.0f, 0.9f, 0.0f};
    pCore->Initialize(
                      CParticleEmitter_Point,
                      pPosition,
                      &upDir,
                      (int)(340 * scale),
                      (int)(150 * scale),
                      10.5f * scale,
                      130,
                      90,
                      coreColor,
                      coreVar,
                      coreColor,
                      3.2f * scale,
                      1.6f * scale,
                      150.0f,
                      0.8f * scale
                      );
    mlstPartice.push_back(pCore);

    for(int i = 0; i < 6; i++)
    {
        CParticleEmitter *pArc = new CParticleEmitter();
        pArc->Initialize(
                         CParticleEmitter_Point,
                         pPosition,
                         &arcDirs[i],
                         (int)(520 * scale),
                         (int)(135 * scale),
                         7.0f * scale,
                         150,
                         100,
                         arcColor,
                         arcVar,
                         arcColor,
                         5.4f * scale,
                         2.8f * scale,
                         46.0f,
                         2.3f * scale
                         );
        mlstPartice.push_back(pArc);
    }
}

void CParticleEmitterMan::NewElectricAirExplosion(SPoint* pPosition,float fScale)
{
    float scale = ClampExplosionScale(fScale);
    NewAirExplosion(pPosition, scale * 1.28f);

    SVector arcDir = {0.0f, 0.22f, 1.0f};
    SVector coronaDir = {0.0f, 0.0f, 1.0f};

    float arcColor[4] = {0.58f, 0.92f, 1.0f, 0.84f};
    float arcVar[4] = {0.14f, 0.08f, 0.10f, 0.10f};
    float coronaColor[4] = {0.95f, 1.0f, 1.0f, 0.90f};
    float coronaVar[4] = {0.05f, 0.03f, 0.03f, 0.08f};

    CParticleEmitter *pCorona = new CParticleEmitter();
    pCorona->Initialize(
                         CParticleEmitter_Point,
                         pPosition,
                         &coronaDir,
                         (int)(320 * scale),
                         (int)(170 * scale),
                         8.8f * scale,
                         120,
                         80,
                         coronaColor,
                         coronaVar,
                         coronaColor,
                         4.0f * scale,
                         2.0f * scale,
                         180.0f,
                         0.9f * scale
                         );
    mlstPartice.push_back(pCorona);

    CParticleEmitter *pArcBurst = new CParticleEmitter();
    pArcBurst->Initialize(
                          CParticleEmitter_Point,
                          pPosition,
                          &arcDir,
                          (int)(620 * scale),
                          (int)(220 * scale),
                          6.2f * scale,
                          155,
                          110,
                          arcColor,
                          arcVar,
                          arcColor,
                          6.0f * scale,
                          3.4f * scale,
                          220.0f,
                          1.2f * scale
                          );
    mlstPartice.push_back(pArcBurst);
}

void CParticleEmitterMan::NewElectricBeamImpact(SPoint* pPosition,float fScale,bool bGrounded)
{
    float scale = ClampExplosionScale(fScale);

    SPoint strikeTop = *pPosition;
    strikeTop.y += 11.0f * scale;

    float coreColor[4] = {0.88f, 0.98f, 1.0f, 0.96f};
    float coreVar[4] = {0.04f, 0.03f, 0.02f, 0.06f};
    float boltColor[4] = {0.22f, 0.70f, 1.0f, 0.90f};
    float boltVar[4] = {0.10f, 0.10f, 0.06f, 0.08f};

    SVector downDir = {0.0f, -1.0f, 0.0f};
    CParticleEmitter *pMainBolt = new CParticleEmitter();
    pMainBolt->Initialize(
                          CParticleEmitter_Point,
                          &strikeTop,
                          &downDir,
                          (int)(180 * scale),
                          (int)(160 * scale),
                          6.4f * scale,
                          90,
                          55,
                          coreColor,
                          coreVar,
                          coreColor,
                          4.4f * scale,
                          1.5f * scale,
                          12.0f,
                          0.25f * scale
                          );
    mlstPartice.push_back(pMainBolt);

    SVector branchDirs[4] = {
        { 0.16f,-0.92f, 0.10f},
        {-0.16f,-0.92f, 0.10f},
        { 0.10f,-0.88f, 0.18f},
        {-0.10f,-0.88f, 0.18f}
    };

    for(int i = 0; i < 4; i++)
    {
        CParticleEmitter *pBranch = new CParticleEmitter();
        pBranch->Initialize(
                            CParticleEmitter_Point,
                            &strikeTop,
                            &branchDirs[i],
                            (int)(150 * scale),
                            (int)(80 * scale),
                            4.4f * scale,
                            80,
                            45,
                            boltColor,
                            boltVar,
                            boltColor,
                            3.5f * scale,
                            1.3f * scale,
                            18.0f,
                            0.55f * scale
                            );
        mlstPartice.push_back(pBranch);
    }

    SVector hitDir = {0.0f, 0.15f, 0.0f};
    CParticleEmitter *pHitFlash = new CParticleEmitter();
    pHitFlash->Initialize(
                          CParticleEmitter_Point,
                          pPosition,
                          &hitDir,
                          (int)(140 * scale),
                          (int)(90 * scale),
                          5.8f * scale,
                          65,
                          35,
                          coreColor,
                          coreVar,
                          coreColor,
                          1.6f * scale,
                          0.7f * scale,
                          180.0f,
                          0.18f * scale
                          );
    mlstPartice.push_back(pHitFlash);

    if(bGrounded)
    {
        SVector groundDirs[4] = {
            { 1.0f, 0.05f, 0.0f},
            {-1.0f, 0.05f, 0.0f},
            { 0.0f, 0.05f, 1.0f},
            { 0.0f, 0.05f,-1.0f}
        };

        float crawlColor[4] = {0.20f, 0.62f, 0.98f, 0.58f};
        float crawlVar[4] = {0.10f, 0.12f, 0.08f, 0.08f};
        for(int i = 0; i < 4; i++)
        {
            CParticleEmitter *pGroundArc = new CParticleEmitter();
            pGroundArc->Initialize(
                                   CParticleEmitter_Point,
                                   pPosition,
                                   &groundDirs[i],
                                   (int)(120 * scale),
                                   (int)(20 * scale),
                                   2.6f * scale,
                                   70,
                                   40,
                                   crawlColor,
                                   crawlVar,
                                   crawlColor,
                                   1.6f * scale,
                                   0.7f * scale,
                                   8.0f,
                                   0.45f * scale
                                   );
            mlstPartice.push_back(pGroundArc);
        }
    }
}

void CParticleEmitterMan::NewElectricBodyShock(SPoint* pPosition,float fScale,float fRadius)
{
    float scale = ClampExplosionScale(fScale);
    float bodyRadius = fRadius * 0.42f;
    if(bodyRadius < 1.6f) bodyRadius = 1.6f;
    if(bodyRadius > 7.5f) bodyRadius = 7.5f;

    float coreColor[4] = {0.90f, 0.98f, 1.0f, 0.92f};
    float coreVar[4] = {0.04f, 0.03f, 0.02f, 0.06f};
    float sparkColor[4] = {0.18f, 0.68f, 1.0f, 0.82f};
    float sparkVar[4] = {0.10f, 0.10f, 0.08f, 0.08f};

    SVector wrapDirs[6] = {
        { 1.0f, 0.25f, 0.0f},
        {-1.0f, 0.25f, 0.0f},
        { 0.0f, 0.25f, 1.0f},
        { 0.0f, 0.25f,-1.0f},
        { 0.7f,-0.20f, 0.7f},
        {-0.7f,-0.20f, 0.7f}
    };

    CParticleEmitter *pCore = new CParticleEmitter();
    SVector upDir = {0.0f, 0.4f, 0.0f};
    pCore->Initialize(
                      CParticleEmitter_Point,
                      pPosition,
                      &upDir,
                      (int)(135 * scale),
                      (int)(88 * scale),
                      5.4f * scale,
                      85,
                      50,
                      coreColor,
                      coreVar,
                      coreColor,
                      1.2f * scale,
                      0.45f * scale,
                      180.0f,
                      bodyRadius * 0.55f
                      );
    mlstPartice.push_back(pCore);

    for(int i = 0; i < 6; i++)
    {
        CParticleEmitter *pWrap = new CParticleEmitter();
        pWrap->Initialize(
                          CParticleEmitter_Point,
                          pPosition,
                          &wrapDirs[i],
                          (int)(260 * scale),
                          (int)(95 * scale),
                          5.2f * scale,
                          110,
                          60,
                          sparkColor,
                          sparkVar,
                          sparkColor,
                          3.4f * scale,
                          1.5f * scale,
                          70.0f,
                          bodyRadius
                          );
        mlstPartice.push_back(pWrap);
    }
}
