//
//  File.cpp
//  SongGL
//
//  Created by Songs on 11. 6. 29..
//  Copyright 2011 thinkm. All rights reserved.
//

#include <math.h>
#include "CTE1BoosterParticle.h"
#include "sGL.h"
#include "IHWorld.h"
#include "sGLUtils.h"
#include "sGLTrasform.h"
#include "CTextureMan.h"

namespace {
float BoosterPulse(int nTime, float speed, float phase)
{
    return 0.5f + 0.5f * sinf((float)nTime * speed + phase);
}

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
}


//부스터
#define  BOOSTERSIZE_W  0.5f
#define  BOOSTERSIZE_H  1.3f

//부스터 바닥
GLfloat CTE1BoosterParticle::m_gBoosterParticleVertex2[]=
{ 
    -BOOSTERSIZE_H    ,   0.0f,  BOOSTERSIZE_H,     //left,top
    -BOOSTERSIZE_H    ,   0.0f, -BOOSTERSIZE_H,     //left,bottom
    BOOSTERSIZE_H   ,   0.0f, -BOOSTERSIZE_H,     //right,bottom
    BOOSTERSIZE_H   ,   0.0f,  BOOSTERSIZE_H      //right,top
};


//부스터
GLfloat CTE1BoosterParticle::m_gBoosterParticleVertex[] = 
{ 
    -BOOSTERSIZE_W    ,  BOOSTERSIZE_H,   0.0f,     //left,top
    -BOOSTERSIZE_W    , -BOOSTERSIZE_H,   0.0f,     //left,bottom
    BOOSTERSIZE_W   , -BOOSTERSIZE_H ,   0.0f,     //right,bottom
    BOOSTERSIZE_W   ,  BOOSTERSIZE_H ,   0.0f      //right,top
};



unsigned short CTE1BoosterParticle::m_gBoosterParticleIndices[] = {
    0, 1, 3, 2
};

GLfloat CTE1BoosterParticle::m_gBoosterParticleCoordTex[8] = 
{ 
    0.0f, 1.0f,
    0.0f, 0.0f,		
    1.0f, 0.0f,
    1.0f, 1.0f
};

unsigned short CTE1BoosterParticle::g_shBoosterIndicesCnt = 0;
GLuint CTE1BoosterParticle::g_BoosterTextureID1 = 0;
GLuint CTE1BoosterParticle::g_BoosterTextureID2 = 0;
GLuint CTE1BoosterParticle::g_BoosterTextureID3 = 0;

CTE1BoosterParticle::CTE1BoosterParticle()
{
    m_bIsVisibleRender = true;
    mCoreAlpha = 0.9f;
    mGlowAlpha = 0.35f;
    mCoreScale = 1.0f;
    mGlowScale = 1.0f;
    mYOffset = -0.55f;
}

CTE1BoosterParticle::~CTE1BoosterParticle()
{
    
}

int CTE1BoosterParticle::Initialize_ToCTE1Booster(IHWorld* pWorld)
{
    CTextureMan *pMan = pWorld->GetTextureMan();
    g_BoosterTextureID1 = pMan->GetTextureID(SGL_3D_FILENAME_ET1BOOSTER1);
    g_BoosterTextureID2 = pMan->GetTextureID(SGL_3D_FILENAME_ET1BOOSTER2);
    g_BoosterTextureID3 = pMan->GetTextureID(SGL_3D_FILENAME_ET1BOOSTER3);
    g_shBoosterIndicesCnt = sizeof(m_gBoosterParticleIndices) /sizeof(unsigned short);
    return 0;
}

int CTE1BoosterParticle::Initialize(SPoint *pPosition,SVector *pvDirection) 
{
    
    mState = SPRITE_RUN;
    mWorldTextureID = g_BoosterTextureID3;
    return E_SUCCESS;
}


int CTE1BoosterParticle::RenderBegin(int nTime)
{
    float pulse = BoosterPulse(nTime, 0.02f, 0.0f);
    float shimmer = BoosterPulse(nTime, 0.033f, 1.3f);
    mCoreAlpha = 0.55f + pulse * 0.35f;
    mGlowAlpha = 0.16f + shimmer * 0.18f;
    mCoreScale = 0.88f + pulse * 0.44f;
    mGlowScale = 0.85f + shimmer * 0.35f;
    mYOffset = -0.45f - pulse * 0.35f;

    if(mWorldTextureID == g_BoosterTextureID3)
        mWorldTextureID = g_BoosterTextureID2;
    else if(mWorldTextureID == g_BoosterTextureID2)
        mWorldTextureID = g_BoosterTextureID1;
    else
        mWorldTextureID = g_BoosterTextureID3;    
    return E_SUCCESS;
}

int CTE1BoosterParticle::Render()
{
    
    if(mState != SPRITE_RUN) return E_SUCCESS;
    if(m_bIsVisibleRender == false) return E_SUCCESS;
    glColor4f(1.0f,1.0f,1.0f,mCoreAlpha);
    
//    glColor4f(1.0f,1.0f,1.0f,0.5f);
    glBindTexture(GL_TEXTURE_2D, mWorldTextureID);
    glTexCoordPointer(2, GL_FLOAT, 0, m_gBoosterParticleCoordTex);
    
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            glVertexPointer(3, GL_FLOAT, 0, &mWorldVertex[i][j*12]);
            glDrawElements(GL_TRIANGLE_STRIP, g_shBoosterIndicesCnt, GL_UNSIGNED_SHORT, m_gBoosterParticleIndices);
            
        }
    }
    
//    glColor4f(1.0f,1.0f,1.0f,0.08f);
    glColor4f(1.0f,0.95f,0.9f,mGlowAlpha);

    for(int i = 0; i < 4; i++)
    {
        glVertexPointer(3, GL_FLOAT, 0, &mWorldVertex2[i][0]);
        glDrawElements(GL_TRIANGLE_STRIP, g_shBoosterIndicesCnt, GL_UNSIGNED_SHORT, m_gBoosterParticleIndices);
    }
	return E_SUCCESS;
}

void CTE1BoosterParticle::CalBooster(SVector* pModelDir,SPoint *mPSRightBP,SPoint* mPSLeftBP)
{
    int nIndex = 0;
    int nIndexj = 0;
    if(m_bIsVisibleRender == false) return ;
    
    //SVector *pDir = GetModelDirection();
    float fAngle = atan2(pModelDir->z,pModelDir->x) * 180.0 / PI;
    float dirX = 0.0f;
    float dirZ = 0.0f;
    NormalizeHorizontalDirection(pModelDir->x, pModelDir->z, &dirX, &dirZ);
    float sideX = -dirZ;
    float sideZ = dirX;

    for(int z = 0; z < 4; z++)
    {
        for(int j = 0; j < 3; j++)
        {
            sglLoadIdentityf(mWorldMatrix);
            sglLoadIdentityf(mWorldMatrix2);

            float phase = (float)GetClockDeltaConst() * 0.03f + (float)z * 0.85f + (float)j * 1.35f;
            float sway = sinf(phase) * 0.08f;
            float lift = cosf(phase * 0.73f) * 0.05f;
            float backDrift = 0.12f + (0.12f * (0.5f + 0.5f * sinf(phase + 0.9f)));
            float localCoreScaleY = mCoreScale * (0.86f + 0.32f * (0.5f + 0.5f * sinf(phase + 0.4f)));
            float localCoreScaleX = 0.82f + 0.16f * (0.5f + 0.5f * cosf(phase * 1.2f));
            float localGlowScale = mGlowScale * (0.88f + 0.18f * (0.5f + 0.5f * cosf(phase + 0.5f)));
            float twist = sinf(phase * 0.7f) * 6.0f;
            
            if(z == 0)
            {
                //sglLog("x=%f,y=%f,z=%f\n",mPSRightBP[0].x, mPSRightBP[0].y, mPSRightBP[0].z);
                //이동을 먼저한후에
                sglTranslateMatrixf(mWorldMatrix,
                                    mPSRightBP[0].x - dirX * backDrift + sideX * sway,
                                    mPSRightBP[0].y + ET1_GROUND_HEIGHT - 0.3f + mYOffset + lift,
                                    mPSRightBP[0].z - dirZ * backDrift + sideZ * sway );
                
                //이동을 먼저한후에
                if(j == 0) sglTranslateMatrixf(mWorldMatrix2,
                                               mPSRightBP[0].x - dirX * (backDrift * 0.4f) + sideX * (sway * 0.4f),
                                               mPSRightBP[0].y + 0.3f + lift * 0.3f,
                                               mPSRightBP[0].z - dirZ * (backDrift * 0.4f) + sideZ * (sway * 0.4f));
            }
            else if(z == 1)
            {
                sglTranslateMatrixf(mWorldMatrix,
                                    mPSRightBP[4].x - dirX * backDrift + sideX * sway,
                                    mPSRightBP[4].y + ET1_GROUND_HEIGHT - 0.3f + mYOffset + lift,
                                    mPSRightBP[4].z - dirZ * backDrift + sideZ * sway);
                
                if(j == 0) sglTranslateMatrixf(mWorldMatrix2,
                                               mPSRightBP[4].x - dirX * (backDrift * 0.4f) + sideX * (sway * 0.4f),
                                               mPSRightBP[4].y + 0.3f + lift * 0.3f,
                                               mPSRightBP[4].z - dirZ * (backDrift * 0.4f) + sideZ * (sway * 0.4f));
            }
            else if(z == 2)
            {
                //이동을 먼저한후에
                sglTranslateMatrixf(mWorldMatrix,
                                    mPSLeftBP[0].x - dirX * backDrift + sideX * sway,
                                    mPSLeftBP[0].y + ET1_GROUND_HEIGHT - 0.3f + mYOffset + lift,
                                    mPSLeftBP[0].z - dirZ * backDrift + sideZ * sway);
                
                if(j == 0) sglTranslateMatrixf(mWorldMatrix2,
                                               mPSLeftBP[0].x - dirX * (backDrift * 0.4f) + sideX * (sway * 0.4f),
                                               mPSLeftBP[0].y + 0.3f + lift * 0.3f,
                                               mPSLeftBP[0].z - dirZ * (backDrift * 0.4f) + sideZ * (sway * 0.4f));
            }
            else if(z == 3)
            {
                sglTranslateMatrixf(mWorldMatrix,
                                    mPSLeftBP[4].x - dirX * backDrift + sideX * sway,
                                    mPSLeftBP[4].y + ET1_GROUND_HEIGHT - 0.3f + mYOffset + lift,
                                    mPSLeftBP[4].z - dirZ * backDrift + sideZ * sway);
                
                if(j == 0) sglTranslateMatrixf(mWorldMatrix2,
                                               mPSLeftBP[4].x - dirX * (backDrift * 0.4f) + sideX * (sway * 0.4f),
                                               mPSLeftBP[4].y + 0.3f + lift * 0.3f,
                                               mPSLeftBP[4].z - dirZ * (backDrift * 0.4f) + sideZ * (sway * 0.4f));
            }
            
            sglScaleMatrixf(mWorldMatrix, localCoreScaleX, localCoreScaleY, localCoreScaleX);
            
            //회전을 해야 한다.
            sglRotateRzRyRxMatrixf(mWorldMatrix,
                                             0,
                                             fAngle + j * 60 + twist,
                                             0);
            
            //회전을 해야 한다.
            if(j == 0) sglRotateRzRyRxMatrixf(mWorldMatrix2,
                                             0,
                                             fAngle + twist * 0.35f,
                                             0);
            if(j == 0) sglScaleMatrixf(mWorldMatrix2, localGlowScale, 1.0f, localGlowScale);
            
            nIndexj = j * 12;
            
            for(int i = 0; i < g_shBoosterIndicesCnt; i++)
            {
                nIndex = i * 3;
                
                sglMultMatrixVectorf(&mWorldVertex[z][nIndexj + nIndex], 
                                               mWorldMatrix,
                                               m_gBoosterParticleVertex + nIndex);
                
                if(j == 0) sglMultMatrixVectorf(&mWorldVertex2[z][nIndexj + nIndex], 
                                               mWorldMatrix2,
                                               m_gBoosterParticleVertex2 + nIndex);
            }
            
        }
    }
}
