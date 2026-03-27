//
//  CFighterTailParticle.cpp
//  SongGL
//
//  Created by itsme on 2013. 12. 17..
//
//
#include "CHWorld.h"
#include "CFighterTailParticle.h"
#include "CParticleEmitterMan.h"
#include "CFighter.h"

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

    pQuadVertex[topLeft + 0] += sideX * (wobbleSide * 0.25f);
    pQuadVertex[topLeft + 1] += wobbleLift * 0.2f;
    pQuadVertex[topLeft + 2] += sideZ * (wobbleSide * 0.25f);

    pQuadVertex[topRight + 0] -= sideX * (wobbleSide * 0.25f);
    pQuadVertex[topRight + 1] += wobbleLift * 0.2f;
    pQuadVertex[topRight + 2] -= sideZ * (wobbleSide * 0.25f);

    pQuadVertex[bottomLeft + 0] += sideX * wobbleSide - dirX * wobbleBack;
    pQuadVertex[bottomLeft + 1] -= wobbleLift;
    pQuadVertex[bottomLeft + 2] += sideZ * wobbleSide - dirZ * wobbleBack;

    pQuadVertex[bottomRight + 0] -= sideX * wobbleSide - dirX * wobbleBack;
    pQuadVertex[bottomRight + 1] -= wobbleLift;
    pQuadVertex[bottomRight + 2] -= sideZ * wobbleSide - dirZ * wobbleBack;
}
}

CFighterTailParticle::CFighterTailParticle(IHWorld* pWorld,CSprite* pSprite,int nIndex) : CBombTailParticle()
{
    mpBomb = pSprite;
    mpWorld = pWorld;
    mTailTextureID = 0;
    mIndex = nIndex;
    mbVisible = true;
}


CFighterTailParticle::~CFighterTailParticle()
{
}

int CFighterTailParticle::Initialize(SPoint *pPosition,SVector *pvDirection)
{
    mState = SPRITE_RUN;
    mnNowTime = 0;
    mTailTextureID = mpWorld->GetTextureMan()->GetTextureID((const char*)"NormalBombET1Tail.tga");
    mLayerCount = 4;
    for(int i = 0; i < 4; i++)
        mLayerAlpha[i] = 1.0f;
    return E_SUCCESS;
}

#ifdef ANDROID
void CFighterTailParticle::ResetTexture()
{
    mTailTextureID = mpWorld->GetTextureMan()->GetTextureID((const char*)"NormalBombET1Tail.tga");
    
}
#endif

void CFighterTailParticle::RenderBeginCore(int nTime)
{
    int nNextTime = 0;
//    nMaxAliveTime = mBombProperty.nBombTailMaxAliveTime;
    if(mpBomb->GetState() != SPRITE_RUN)
    {
        SetState(SPRITE_READYDELETE);
        return;
    }
    
    nNextTime = mnNowTime + nTime;
//    if(nNextTime > nMaxAliveTime)
//    {
//        mState = SPRITE_READYDELETE; //다음 프레임에 제거 될 예정이다.
//    }
    mnNowTime = nNextTime;
}


int CFighterTailParticle::RenderBegin(int nTime)
{
    int nIndex = 0;
    GLfloat mWorldMatrix[16];
    GLfloat mWorldMatrix1[16];
    GLfloat mWorldMatrix2[16];
    GLfloat mWorldMatrix3[16];
    float color[4];
    float *fFront=NULL,*fBack=NULL;
    SPoint ptNow;
    
    CHWorld* pWorld = (CHWorld*)this->mpWorld;
    
    
    if(mbVisible == false) return E_SUCCESS;
    
    CParticleEmitterMan* emitter = pWorld->GetParticleEmitterMan();
    SVector dvDir;
    
    //아군
    if(mpBomb && mpBomb->GetState() == SPRITE_RUN)
    {
        mpBomb->GetModelDirection(&dvDir);
        mpBomb->GetBoosterPos(mIndex,&fFront,&fBack);
        ptNow.x = fBack[0];
        ptNow.y = fBack[1];
        ptNow.z = fBack[2];
        
        if(mpBomb->GetModelID() < ACTORID_ET1)
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
        
        
        
        //폭탄의 꼬리 방향은 폭탄방향의 반대이다.
        dvDir.x = -dvDir.x;
        dvDir.y = -dvDir.y;
        dvDir.z = -dvDir.z;
        float dirX = 0.0f;
        float dirZ = 0.0f;
        NormalizeHorizontalDirection(dvDir.x, dvDir.z, &dirX, &dirZ);
        float sideX = -dirZ;
        float sideZ = dirX;
        emitter->NewBomTail(&ptNow, &dvDir,color);
        
        
        sglLoadIdentityf(mWorldMatrix);
        
        //이동을 먼저한후에
        sglTranslateMatrixf(mWorldMatrix,ptNow.x,ptNow.y,ptNow.z);
        
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

        float phase = (float)mnNowTime * 0.028f + (float)mIndex * 0.9f;
        float pulse = 0.5f + 0.5f * sinf(phase);
        float pulse2 = 0.5f + 0.5f * cosf(phase * 1.21f);
        float mainScale = 0.75f + pulse * 0.28f;
        float wingScale = 0.6f + pulse2 * 0.22f;
        float sway = sinf(phase * 1.6f) * 0.1f;
        float drift = 0.14f + pulse * 0.18f;

        sglScaleMatrixf(mWorldMatrix, mainScale, mainScale, mainScale);
        sglScaleMatrixf(mWorldMatrix1, wingScale, mainScale * 0.95f, wingScale);
        sglScaleMatrixf(mWorldMatrix2, wingScale * 0.88f, mainScale * 0.82f, wingScale * 0.88f);
        sglScaleMatrixf(mWorldMatrix3, wingScale * 0.88f, mainScale * 0.82f, wingScale * 0.88f);

        sglTranslateMatrixf(mWorldMatrix2, sideX * (0.16f + sway), 0.0f, sideZ * (0.16f + sway));
        sglTranslateMatrixf(mWorldMatrix3, -sideX * (0.16f - sway), 0.0f, -sideZ * (0.16f - sway));
        
        //sglScaleMatrixf(mWorldMatrix, mBombProperty.fBombTailScale, mBombProperty.fBombTailScale, mBombProperty.fBombTailScale);
        
        for(int i = 0; i < g_shBombTailIndicesCnt; i++)
        {
            nIndex = i * 3;
            
            sglMultMatrixVectorf(mWorldVertex[0] + nIndex, mWorldMatrix,m_gBombTailParticleVertex + nIndex);
            sglMultMatrixVectorf(mWorldVertex[1] + nIndex, mWorldMatrix1,m_gBombTailParticleVertex + nIndex);
            sglMultMatrixVectorf(mWorldVertex[2] + nIndex, mWorldMatrix2,m_gBombTailParticleVertex + nIndex);
            sglMultMatrixVectorf(mWorldVertex[3] + nIndex, mWorldMatrix3,m_gBombTailParticleVertex + nIndex);
            
        }

        DistortTrailQuad(mWorldVertex[0], dirX, dirZ, sideX, sideZ, sway * 0.3f, drift * 0.6f, 0.04f + pulse * 0.06f);
        DistortTrailQuad(mWorldVertex[1], dirX, dirZ, sideX, sideZ, -sway * 0.35f, drift * 0.48f, 0.03f + pulse2 * 0.05f);
        DistortTrailQuad(mWorldVertex[2], dirX, dirZ, sideX, sideZ, 0.08f + sway * 0.45f, drift * 0.8f, 0.05f + pulse2 * 0.05f);
        DistortTrailQuad(mWorldVertex[3], dirX, dirZ, sideX, sideZ, -0.08f + sway * 0.45f, drift * 0.8f, 0.05f + pulse * 0.05f);

        mLayerAlpha[0] = 0.95f;
        mLayerAlpha[1] = 0.5f + pulse2 * 0.1f;
        mLayerAlpha[2] = 0.25f + pulse * 0.08f;
        mLayerAlpha[3] = 0.25f + pulse2 * 0.08f;
    }
    else
        return E_SUCCESS;
    
    

    return E_SUCCESS;
}

int CFighterTailParticle::Render()
{
    if(mbVisible == false) return E_SUCCESS;
    return CBombTailParticle::Render();
}
