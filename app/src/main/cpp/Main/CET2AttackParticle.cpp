//
//  CET2Attack.cpp
//  SongGL
//
//  Created by 호학 송 on 12. 11. 10..
//
//
#include <stdlib.h>
#include "CET2AttackParticle.h"
#include <math.h>
#include "IHWorld.h"
#include "sGLUtils.h"
#include "sGLTrasform.h"
#include "CFireParticle.h"
#include "CHWorld.h"

GLuint CET2AttackParticle::g_CET2TextureID = 0;

CET2AttackParticle::CET2AttackParticle(IHWorld *pWorld,CSprite* pSprite)
{
    mpWorld = pWorld;
    mpOwner = pSprite;
    mAlpha = 0.5f;
    mState = SPRITE_RUN;
    mbHide = true;
}


CET2AttackParticle::~CET2AttackParticle()
{
}

int CET2AttackParticle::ET2AttackParticleInitialize(const char *pFilePath)
{

    int nWidth;
    int nHeight;
    int nDepth;
    unsigned char* pImage = 0;
    
    if(g_CET2TextureID == 0)
    {
        g_CET2TextureID = sglLoadTgaToBind(pFilePath,GL_CLAMP_TO_EDGE,&nWidth,&nHeight,&nDepth,&pImage);
        if(g_CET2TextureID == 0) return -1;
        if(pImage) free(pImage);
    }
    
    return E_SUCCESS;
}

void CET2AttackParticle::ET2AttackParticleUninitialize()
{
    if(g_CET2TextureID)
    {
       
        glDeleteTextures(1, &g_CET2TextureID);
        g_CET2TextureID = 0;
    }
}


int CET2AttackParticle::Initialize(SPoint *pPosition,SVector *pvDirection)
{
    return E_SUCCESS;
}

void CET2AttackParticle::GetPosition(SPoint* pOut)
{
    if(mState == SPRITE_RUN)
        mpOwner->GetPosition(pOut);
}

void CET2AttackParticle::RenderBeginCore(int nTime)
{
    if(mpOwner->GetState() != SPRITE_RUN)
    {
        SetState(SPRITE_READYDELETE);
        return;
    }
}

int CET2AttackParticle::RenderBegin(int nTime)
{
    SPoint ptNow;
    float matrix[16];
    if(mpOwner->GetState() != SPRITE_RUN)
    {
        SetState(SPRITE_READYDELETE);
        return E_SUCCESS;
    }
    
    if(mbHide) return E_SUCCESS;
    
    mpOwner->GetPosition(&ptNow);
    sglLoadIdentityf(matrix);
    sglTranslateMatrixf(matrix,ptNow.x,ptNow.y + 3.5f,ptNow.z);
    sglRotateRzRyRxMatrixf(matrix,
                           90.f,
                           GetGGTime(),
                           0);
    sglScaleMatrixf(matrix, 2.f, 2.f, 1.f);
    
    
    for(int i = 0; i < CFireParticle::g_shFireIndicesCnt; i++)
    {
        sglMultMatrixVectorf(mWorldVertex + i * 3, matrix,CFireParticle::m_gFireParticleVertex + i * 3);
    }
    
    return E_SUCCESS;
}



int CET2AttackParticle::Render()
{
    if(mbHide) return E_SUCCESS;
    
    if(mState == SPRITE_RUN)
    {
        //투명도로 서서히 없어지게 한다.
        glColor4f(1.0f,1.0f,1.0f,mAlpha);
        
        glBindTexture(GL_TEXTURE_2D, g_CET2TextureID);
        
        glVertexPointer(3, GL_FLOAT, 0, mWorldVertex);
        
        glTexCoordPointer(2, GL_FLOAT, 0, CFireParticle::m_gFireParticleCoordTex);
        
        glDrawElements(GL_TRIANGLE_STRIP, CFireParticle::g_shFireIndicesCnt, GL_UNSIGNED_SHORT, CFireParticle::m_gFireParticleIndices);
    }
    
    return E_SUCCESS;
}


int CET2AttackParticle::RenderEnd()
{
    return E_SUCCESS;
}



