//
//  CBombTailParticle.cpp
//  SongGL
//
//  Created by Songs on 11. 4. 13..
//  Copyright 2011 thinkm. All rights reserved.
//
#include <math.h>
#include "IHWorld.h"
#include "sGLUtils.h"
#include "CTextureMan.h"
#include "CBombTailParticle.h"
#include "sGLTrasform.h"
#include "CSGLCore.h"
#include "CHWorld.h"
#include "CParticleEmitterMan.h"

namespace {
void NormalizeHorizontalDirection(float x, float z, float* pOutX, float* pOutZ)
{
    float len = sqrtf(x * x + z * z);
    if(len < 0.0001f)
    {
        *pOutX = 1.0f;
        *pOutZ = 0.0f;
        return;
    }
    *pOutX = x / len;
    *pOutZ = z / len;
}

void DistortTrailQuad(float* pQuadVertex,
                      float dirX,
                      float dirZ,
                      float sideX,
                      float sideZ,
                      float wobbleSide,
                      float wobbleBack,
                      float wobbleLift)
{
    const int topLeft = 0;
    const int bottomLeft = 3;
    const int bottomRight = 6;
    const int topRight = 9;

    pQuadVertex[topLeft + 0] += sideX * (wobbleSide * 0.28f);
    pQuadVertex[topLeft + 1] += wobbleLift * 0.25f;
    pQuadVertex[topLeft + 2] += sideZ * (wobbleSide * 0.28f);

    pQuadVertex[topRight + 0] -= sideX * (wobbleSide * 0.28f);
    pQuadVertex[topRight + 1] += wobbleLift * 0.25f;
    pQuadVertex[topRight + 2] -= sideZ * (wobbleSide * 0.28f);

    pQuadVertex[bottomLeft + 0] += sideX * wobbleSide - dirX * wobbleBack;
    pQuadVertex[bottomLeft + 1] -= wobbleLift;
    pQuadVertex[bottomLeft + 2] += sideZ * wobbleSide - dirZ * wobbleBack;

    pQuadVertex[bottomRight + 0] -= sideX * wobbleSide - dirX * wobbleBack;
    pQuadVertex[bottomRight + 1] -= wobbleLift;
    pQuadVertex[bottomRight + 2] -= sideZ * wobbleSide - dirZ * wobbleBack;
}
}

#define FIRESIZE_W 1.0f
#define FIRESIZE_H 7.0f

#define FIRESIZEET1_W 0.8f
#define FIRESIZEET1_H 4.0f

//left,top
//left,bottom
//right,bottom
//right,top    
GLfloat CBombTailParticle::m_gBombTailParticleVertex[]= 
{ 
    -FIRESIZE_W,  0.0f,0.0f,     //left,top
    -FIRESIZE_W,  0.0f,-FIRESIZE_H,     //left,bottom
    FIRESIZE_W,   0.0f,-FIRESIZE_H,     //right,bottom
    FIRESIZE_W,   0,0,0.0f     //right,top
};

//GLfloat CBombTailParticle::m_gBombTailET1ParticleVertex[]= 
//{ 
//    -FIRESIZEET1_W,   0,0.0f,     //left,top
//    -FIRESIZEET1_W,  -FIRESIZEET1_H,0.0f,     //left,bottom
//    FIRESIZEET1_W,   -FIRESIZEET1_H,0.0f,     //right,bottom
//    FIRESIZEET1_W,    0,0,0.0f     //right,top
//};



unsigned short CBombTailParticle::m_gBombTailParticleIndices[] = {
    0, 1, 3, 2
};

GLfloat CBombTailParticle::m_gBombTailParticleCoordTex[8] = {     
    0.0f, 1.0f,
    0.0f, 0.0f,		
    1.0f, 0.0f,  	
    1.0f, 1.0f
};


unsigned short CBombTailParticle::g_shBombTailIndicesCnt = 0;
//GLuint CBombTailParticle::g_BombTailTextureID = 0;
//GLuint CBombTailParticle::g_BombTailET1TextureID = 0;

CBombTailParticle::CBombTailParticle(IHWorld* pWorld,CSprite* pBomb)
{
    mpBomb = pBomb;
    mBombProperty = ((CBomb*)pBomb)->GetBombProperty();
    mpWorld = pWorld;
    mTailTextureID = 0;
    mLayerCount = 4;
    for(int i = 0; i < 4; i++)
        mLayerAlpha[i] = 1.0f;
}


CBombTailParticle::~CBombTailParticle()
{
}


int CBombTailParticle::BombTailParticleInitialize(IHWorld* pWorld)
{
    if(g_shBombTailIndicesCnt == 0)
        g_shBombTailIndicesCnt = sizeof(m_gBombTailParticleIndices) /sizeof(unsigned short);
    return E_SUCCESS;
}



int CBombTailParticle::Initialize(SPoint *pPosition,SVector *pvDirection)
{
//    memcpy(&mPosition,pPosition,sizeof(SPoint));
    mState = SPRITE_RUN;
    mnNowTime = 0;
    mTailTextureID = mpWorld->GetTextureMan()->GetTextureID((const char*)mBombProperty.sBombTailImgPath);
    return E_SUCCESS;
}

#ifdef ANDROID
void CBombTailParticle::ResetTexture()
{
    mTailTextureID = mpWorld->GetTextureMan()->GetTextureID((const char*)mBombProperty.sBombTailImgPath);
}
#endif


void CBombTailParticle::RenderBeginCore(int nTime)
{
    int nMaxAliveTime = 1500;
    int nNextTime = 0;
    

    nMaxAliveTime = mBombProperty.nBombTailMaxAliveTime;
        
    
    if(mpBomb->GetState() != SPRITE_RUN) 
    {
        SetState(SPRITE_READYDELETE);
        return;
    }
    
    nNextTime = mnNowTime + nTime;
    if(nNextTime > nMaxAliveTime)
    {
        mState = SPRITE_READYDELETE; //다음 프레임에 제거 될 예정이다.
    }
    
    mnNowTime = nNextTime;
}

int CBombTailParticle::RenderBegin(int nTime)
{
    int nIndex = 0;
    GLfloat mWorldMatrix[16];
    GLfloat mWorldMatrix1[16];
    GLfloat mWorldMatrix2[16];
    GLfloat mWorldMatrix3[16];
    
    float color[4];
    
    //----------------------------------------------------    
    SPoint ptNow;
    GetPosition(&ptNow);
    
    CHWorld* pWorld = (CHWorld*)this->mpWorld;
    CParticleEmitterMan* emitter = pWorld->GetParticleEmitterMan();
    SVector dvDir;
    mpBomb->GetModelDirection(&dvDir);
    float dirX = 0.0f;
    float dirZ = 0.0f;
    NormalizeHorizontalDirection(-dvDir.x, -dvDir.z, &dirX, &dirZ);
    float sideX = -dirZ;
    float sideZ = dirX;
    
    //아군
    CSprite* pS = ((CBomb*)mpBomb)->GetOwner();
    if(pS && pS->GetState() == SPRITE_RUN)
    {
    
        if(pS->GetModelID() < ACTORID_ET1)
        {
            color[0] = 0.7f;
            color[1] = 0.7f;
            color[2] = 0.7f;
            color[3] = 0.5f;

        }
        else //적군
        {
            color[0] = 0.609f;
            color[1] = 0.777f;
            color[2] = 0.921f;
            color[3] = 0.5f;
        }
    }
    else
    {
        color[0] = 0.7f;
        color[1] = 0.7f;
        color[2] = 0.7f;
        color[3] = 0.5f;
    }
    
    //폭탄의 꼬리 방향은 폭탄방향의 반대이다.
    dvDir.x = -dvDir.x;
    dvDir.y = -dvDir.y;
    dvDir.z = -dvDir.z;
    emitter->NewBomTail(&ptNow, &dvDir,color);
    

    sglLoadIdentityf(mWorldMatrix);
    
    //이동을 먼저한후에
    sglTranslateMatrixf(mWorldMatrix,ptNow.x,ptNow.y,ptNow.z);
    
    //VectorMathf::translateMatrixf(mWorldMatrix1,ptNow->x,ptNow->y,ptNow->z);
    
#if OBJMilkShape
    CMS3DModel*	pModel = (CMS3DModel*)mpBomb->GetModel();
#else
    CMS3DModelASCII* pModel = (CMS3DModelASCII*)mpBomb->GetModel();
#endif

    
    //짐벌락현상 해결함.
    GLfloat	rotationMatrix[16];
    GLfloat	quaternion0[4];
    sglAngleToQuaternionf(quaternion0, pModel->orientation[0], pModel->orientation[1], pModel->orientation[2]);
    sglQuaternionToMatrixf(rotationMatrix, quaternion0);
    sglMultMatrixf(mWorldMatrix, mWorldMatrix, rotationMatrix);
    
    sglRotateRzRyRxMatrixf(mWorldMatrix,
                           0.f,
                           90.f,
                           0.f);

    memcpy(mWorldMatrix1, mWorldMatrix, sizeof(mWorldMatrix));
    memcpy(mWorldMatrix2, mWorldMatrix, sizeof(mWorldMatrix));
    memcpy(mWorldMatrix3, mWorldMatrix, sizeof(mWorldMatrix));
   
    float phase = (float)mnNowTime * 0.025f;
    float pulse = 0.5f + 0.5f * sinf(phase);
    float pulse2 = 0.5f + 0.5f * cosf(phase * 1.31f);
    float mainScale = mBombProperty.fBombTailScale * (0.9f + pulse * 0.35f);
    float wingScale = mBombProperty.fBombTailScale * (0.7f + pulse2 * 0.28f);
    float drift = 0.18f + pulse * 0.22f;
    float sway = sinf(phase * 1.7f) * 0.12f;

    sglScaleMatrixf(mWorldMatrix, mainScale, mainScale, mainScale);
    sglScaleMatrixf(mWorldMatrix1, wingScale, mainScale * 0.92f, wingScale);
    sglScaleMatrixf(mWorldMatrix2, wingScale * 0.88f, mainScale * 0.82f, wingScale * 0.88f);
    sglScaleMatrixf(mWorldMatrix3, wingScale * 0.88f, mainScale * 0.82f, wingScale * 0.88f);

    sglTranslateMatrixf(mWorldMatrix2, sideX * (0.18f + sway), 0.0f, sideZ * (0.18f + sway));
    sglTranslateMatrixf(mWorldMatrix3, -sideX * (0.18f - sway), 0.0f, -sideZ * (0.18f - sway));
    
    for(int i = 0; i < g_shBombTailIndicesCnt; i++)
    {
        nIndex = i * 3;
        
        sglMultMatrixVectorf(mWorldVertex[0] + nIndex, mWorldMatrix,m_gBombTailParticleVertex + nIndex);
        sglMultMatrixVectorf(mWorldVertex[1] + nIndex, mWorldMatrix1,m_gBombTailParticleVertex + nIndex);
        sglMultMatrixVectorf(mWorldVertex[2] + nIndex, mWorldMatrix2,m_gBombTailParticleVertex + nIndex);
        sglMultMatrixVectorf(mWorldVertex[3] + nIndex, mWorldMatrix3,m_gBombTailParticleVertex + nIndex);
        
    }

    DistortTrailQuad(mWorldVertex[0], dirX, dirZ, sideX, sideZ, sway * 0.35f, drift * 0.7f, 0.05f + pulse * 0.08f);
    DistortTrailQuad(mWorldVertex[1], dirX, dirZ, sideX, sideZ, -sway * 0.4f, drift * 0.55f, 0.03f + pulse2 * 0.06f);
    DistortTrailQuad(mWorldVertex[2], dirX, dirZ, sideX, sideZ, 0.10f + sway * 0.55f, drift, 0.05f + pulse2 * 0.07f);
    DistortTrailQuad(mWorldVertex[3], dirX, dirZ, sideX, sideZ, -0.10f + sway * 0.55f, drift, 0.05f + pulse * 0.07f);

    mLayerAlpha[0] = 0.95f;
    mLayerAlpha[1] = 0.52f + pulse2 * 0.12f;
    mLayerAlpha[2] = 0.28f + pulse * 0.08f;
    mLayerAlpha[3] = 0.28f + pulse2 * 0.08f;
    
    
    return E_SUCCESS;
}


int CBombTailParticle::Render()
{
    if(mState == SPRITE_RUN)
    {
        glBindTexture(GL_TEXTURE_2D, mTailTextureID);
        glTexCoordPointer(2, GL_FLOAT, 0, m_gBombTailParticleCoordTex);

        for(int i = 0; i < mLayerCount; i++)
        {
            glColor4f(1.0f,1.0f,1.0f,mLayerAlpha[i]);
            glVertexPointer(3, GL_FLOAT, 0, mWorldVertex[i]);
            glDrawElements(GL_TRIANGLE_STRIP, g_shBombTailIndicesCnt, GL_UNSIGNED_SHORT, m_gBombTailParticleIndices);
        }

        
    }
    return E_SUCCESS;
}


int CBombTailParticle::RenderEnd()
{
    return E_SUCCESS;
}
