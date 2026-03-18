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
    
    float color[4];
    
    //----------------------------------------------------    
    SPoint ptNow;
    GetPosition(&ptNow);
    
    CHWorld* pWorld = (CHWorld*)this->mpWorld;
    CParticleEmitterMan* emitter = pWorld->GetParticleEmitterMan();
    SVector dvDir;
    mpBomb->GetModelDirection(&dvDir);
    
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
    
    memcpy(mWorldMatrix1, mWorldMatrix, sizeof(mWorldMatrix));
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

    sglMultMatrixf(mWorldMatrix1, mWorldMatrix1, rotationMatrix);
    sglAngleToQuaternionf(quaternion0, 0.f, 90.f, 90.f);
    sglQuaternionToMatrixf(rotationMatrix, quaternion0);
    sglMultMatrixf(mWorldMatrix1, mWorldMatrix1, rotationMatrix);
   
    sglScaleMatrixf(mWorldMatrix, mBombProperty.fBombTailScale, mBombProperty.fBombTailScale, mBombProperty.fBombTailScale);
    
    for(int i = 0; i < g_shBombTailIndicesCnt; i++)
    {
        nIndex = i * 3;
        
        sglMultMatrixVectorf(mWorldVertex[0] + nIndex, mWorldMatrix,m_gBombTailParticleVertex + nIndex);
        sglMultMatrixVectorf(mWorldVertex[1] + nIndex, mWorldMatrix1,m_gBombTailParticleVertex + nIndex);
        
    }
    
    
    return E_SUCCESS;
}


int CBombTailParticle::Render()
{
    if(mState == SPRITE_RUN)
    {
        //투명도로 서서히 없어지게 한다.
        glColor4f(1.0f,1.0f,1.0f,1.0f);
        
        glBindTexture(GL_TEXTURE_2D, mTailTextureID);
        glTexCoordPointer(2, GL_FLOAT, 0, m_gBombTailParticleCoordTex);
        
        glVertexPointer(3, GL_FLOAT, 0, mWorldVertex[0]);
        glDrawElements(GL_TRIANGLE_STRIP, g_shBombTailIndicesCnt, GL_UNSIGNED_SHORT, m_gBombTailParticleIndices);
        
        glVertexPointer(3, GL_FLOAT, 0, mWorldVertex[1]);
        glDrawElements(GL_TRIANGLE_STRIP, g_shBombTailIndicesCnt, GL_UNSIGNED_SHORT, m_gBombTailParticleIndices);

        
    }
    return E_SUCCESS;
}


int CBombTailParticle::RenderEnd()
{
    return E_SUCCESS;
}

