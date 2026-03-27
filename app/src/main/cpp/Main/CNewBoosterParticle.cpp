//
//  CNewBoosterParticle.cpp
//  SongGL
//
//  Created by 호학 송 on 2014. 3. 6..
//
//

#include "CNewBoosterParticle.h"
#include <math.h>
#include "IHWorld.h"
#include "sGLUtils.h"
#include "CTextureMan.h"
#include "CBombTailParticle.h"
#include "sGLTrasform.h"
#include "CSGLCore.h"
#include "CHWorld.h"
#include "CParticleEmitterMan.h"
#include "CTE1BoosterParticle.h"

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

void DistortBoosterQuad(float* pQuadVertex,
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

    pQuadVertex[topLeft + 0] += sideX * (wobbleSide * 0.35f);
    pQuadVertex[topLeft + 1] += wobbleLift * 0.2f;
    pQuadVertex[topLeft + 2] += sideZ * (wobbleSide * 0.35f);

    pQuadVertex[topRight + 0] -= sideX * (wobbleSide * 0.35f);
    pQuadVertex[topRight + 1] += wobbleLift * 0.2f;
    pQuadVertex[topRight + 2] -= sideZ * (wobbleSide * 0.35f);

    pQuadVertex[bottomLeft + 0] += sideX * wobbleSide - dirX * wobbleBack;
    pQuadVertex[bottomLeft + 1] -= wobbleLift;
    pQuadVertex[bottomLeft + 2] += sideZ * wobbleSide - dirZ * wobbleBack;

    pQuadVertex[bottomRight + 0] -= sideX * wobbleSide - dirX * wobbleBack;
    pQuadVertex[bottomRight + 1] -= wobbleLift;
    pQuadVertex[bottomRight + 2] -= sideZ * wobbleSide - dirZ * wobbleBack;
}
}

CNewBoosterParticle::CNewBoosterParticle(IHWorld* pWorld,CSprite* pOwner)
{
    mpOwner = pOwner;
    mpWorld = pWorld;
    mTailTextureID = 0;
    mbVisible = true;
    mppWorldVertex = 0;
    mppWorldVertex2 = 0;
    mnBoosterCnt = 0;
    mCoreAlpha = 0.9f;
    mGlowAlpha = 0.35f;
    mCoreScale = 1.0f;
    mGlowScale = 1.0f;
    mYOffset = -0.65f;
}


CNewBoosterParticle::~CNewBoosterParticle()
{
    if(mppWorldVertex)
    {
        int nCnt = mnBoosterCnt;
        for (int i = 0; i < nCnt; i++)
        {
            delete[] mppWorldVertex[i];
        }
        delete[] mppWorldVertex;
        mppWorldVertex = NULL;
    }
    
    
    if(mppWorldVertex2)
    {
        int nCnt = mnBoosterCnt;
        for (int i = 0; i < nCnt; i++)
        {
            delete[] mppWorldVertex2[i];
        }
        delete[] mppWorldVertex2;
        mppWorldVertex2 = NULL;
    }
}


int CNewBoosterParticle::Initialize(SPoint *pPosition,SVector *pvDirection)
{
    //    memcpy(&mPosition,pPosition,sizeof(SPoint));
    mState = SPRITE_RUN;
    mTailTextureID = mpWorld->GetTextureMan()->GetTextureID((const char*)"NormalBombTail.tga");
    mnBoosterCnt = mpOwner->GetBoosterCnt();
    mppWorldVertex = new float*[mnBoosterCnt];
    for (int i = 0; i < mnBoosterCnt; i++)
    {
        mppWorldVertex[i] = new float[36];
    }
    
    mppWorldVertex2 = new float*[mnBoosterCnt];
    for (int i = 0; i < mnBoosterCnt; i++)
    {
        mppWorldVertex2[i] = new float[12];
    }
    
    if(CTE1BoosterParticle::g_shBoosterIndicesCnt == 0)
        CTE1BoosterParticle::Initialize_ToCTE1Booster(mpWorld);
    return E_SUCCESS;
}

#ifdef ANDROID
void CNewBoosterParticle::ResetTexture()
{
    mTailTextureID = mpWorld->GetTextureMan()->GetTextureID((const char*)"NormalBombTail.tga");
}
#endif


void CNewBoosterParticle::RenderBeginCore(int nTime)
{
    if(mpOwner->GetState() != SPRITE_RUN)
    {
        SetState(SPRITE_READYDELETE);
        return;
    }
}

int CNewBoosterParticle::RenderBegin(int nTime)
{
    GLfloat mWorldMatrix[16];
    GLfloat mWorldMatrix2[16];
    float *fUp,*fDown;
    if(mbVisible == false) return E_SUCCESS;

    float pulse = BoosterPulse(nTime, 0.02f, 0.6f);
    float shimmer = BoosterPulse(nTime, 0.034f, 1.8f);
    mCoreAlpha = 0.5f + pulse * 0.38f;
    mGlowAlpha = 0.15f + shimmer * 0.16f;
    mCoreScale = 0.82f + pulse * 0.55f;
    mGlowScale = 0.78f + shimmer * 0.42f;
    mYOffset = -0.45f - pulse * 0.4f;

    int nIndex = 0;
    int nIndexj = 0;
    
    SVector vtDir;
    mpOwner->GetModelDirection(&vtDir);
    //SVector *pDir = GetModelDirection();
    float fAngle = atan2(vtDir.z,vtDir.x) * 180.0 / PI;
    float dirX = 0.0f;
    float dirZ = 0.0f;
    NormalizeHorizontalDirection(vtDir.x, vtDir.z, &dirX, &dirZ);
    float sideX = -dirZ;
    float sideZ = dirX;
    
    for(int z = 0; z < mnBoosterCnt; z++)
    {
        for(int j = 0; j < 3; j++)
        {
            sglLoadIdentityf(mWorldMatrix);
            sglLoadIdentityf(mWorldMatrix2);
            float phase = (float)nTime * 0.03f + (float)z * 0.92f + (float)j * 1.41f;
            float sway = sinf(phase) * 0.09f;
            float lift = cosf(phase * 0.67f) * 0.05f;
            float backDrift = 0.14f + (0.18f * (0.5f + 0.5f * sinf(phase + 0.8f)));
            float localCoreScaleY = mCoreScale * (0.82f + 0.38f * (0.5f + 0.5f * sinf(phase + 0.5f)));
            float localCoreScaleX = 0.74f + 0.18f * (0.5f + 0.5f * cosf(phase * 1.15f));
            float localGlowScale = mGlowScale * (0.84f + 0.24f * (0.5f + 0.5f * cosf(phase + 0.35f)));
            float twist = sinf(phase * 0.75f) * 7.0f;
            
            
            mpOwner->GetBoosterPos(z,&fUp,&fDown);
            //sglLog("x=%f,y=%f,z=%f\n",mPSRightBP[0].x, mPSRightBP[0].y, mPSRightBP[0].z);
            //이동을 먼저한후에
            
            
            sglTranslateMatrixf(mWorldMatrix,
                                fDown[0] - dirX * backDrift + sideX * sway,
                                fDown[1] + mYOffset + lift,
                                fDown[2] - dirZ * backDrift + sideZ * sway);

            
            
            //이동을 먼저한후에
            sglTranslateMatrixf(mWorldMatrix2,
                                fDown[0] - dirX * (backDrift * 0.4f) + sideX * (sway * 0.45f),
                                fDown[1] - ET1_GROUND_HEIGHT + lift * 0.2f,
                                fDown[2] - dirZ * (backDrift * 0.4f) + sideZ * (sway * 0.45f));
            sglScaleMatrixf(mWorldMatrix, localCoreScaleX, localCoreScaleY, localCoreScaleX);
            
            
            //회전을 해야 한다.
            sglRotateRzRyRxMatrixf(mWorldMatrix,
                                   0,
                                   fAngle + j * 60 + twist,
                                   0);
            
            if(j == 0)
            {
                //회전을 해야 한다.
                sglRotateRzRyRxMatrixf(mWorldMatrix2,
                                       0,
                                       fAngle + twist * 0.4f,
                                       0);
                sglScaleMatrixf(mWorldMatrix2, localGlowScale, 1.0f, localGlowScale);
            }
            
            nIndexj = j * 12;
            
            for(int i = 0; i < CTE1BoosterParticle::g_shBoosterIndicesCnt; i++)
            {
                nIndex = i * 3;
                
                sglMultMatrixVectorf(&mppWorldVertex[z][nIndexj + nIndex],
                                     mWorldMatrix,
                                     CTE1BoosterParticle::m_gBoosterParticleVertex + nIndex);
                
                if(j == 0)
                sglMultMatrixVectorf(&mppWorldVertex2[z][nIndexj + nIndex],
                                     mWorldMatrix2,
                                     CTE1BoosterParticle::m_gBoosterParticleVertex2 + nIndex);
            }

            float* pQuadVertex = &mppWorldVertex[z][nIndexj];
            float tailPhase = phase + 0.45f;
            float wobbleSide = sinf(tailPhase) * 0.07f;
            float wobbleBack = 0.05f + (0.08f * (0.5f + 0.5f * cosf(tailPhase * 1.17f)));
            float wobbleLift = 0.015f + (0.035f * (0.5f + 0.5f * sinf(tailPhase * 0.83f)));
            DistortBoosterQuad(pQuadVertex, dirX, dirZ, sideX, sideZ, wobbleSide, wobbleBack, wobbleLift);
            
        }
    }
    
    
    return E_SUCCESS;
}


int CNewBoosterParticle::Render()
{
    if(mbVisible == false || mState != SPRITE_RUN || mpOwner->GetType() == ACTORTYPE_TYPE_GHOST) return E_SUCCESS;

    glColor4f(1.0f,1.0f,1.0f,mCoreAlpha);
    
    
    glBindTexture(GL_TEXTURE_2D, mTailTextureID);
    glTexCoordPointer(2, GL_FLOAT, 0, CTE1BoosterParticle::m_gBoosterParticleCoordTex);
    
    for(int i = 0; i < mnBoosterCnt; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            glVertexPointer(3, GL_FLOAT, 0, &mppWorldVertex[i][j*12]);
            glDrawElements(GL_TRIANGLE_STRIP, CTE1BoosterParticle::g_shBoosterIndicesCnt, GL_UNSIGNED_SHORT, CTE1BoosterParticle::m_gBoosterParticleIndices);
        }
    }
    
    glColor4f(1.0f,0.95f,0.9f,mGlowAlpha);

    
    for(int i = 0; i < mnBoosterCnt; i++)
    {
        glVertexPointer(3, GL_FLOAT, 0, &mppWorldVertex2[i][0]);
        glDrawElements(GL_TRIANGLE_STRIP, CTE1BoosterParticle::g_shBoosterIndicesCnt, GL_UNSIGNED_SHORT, CTE1BoosterParticle::m_gBoosterParticleIndices);
    }
	return E_SUCCESS;    return E_SUCCESS;
}


int CNewBoosterParticle::RenderEnd()
{
    return E_SUCCESS;
}
