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
        emitter->NewBomTail(&ptNow, &dvDir,color);
        
        
        sglLoadIdentityf(mWorldMatrix);
        
        //이동을 먼저한후에
        sglTranslateMatrixf(mWorldMatrix,ptNow.x,ptNow.y,ptNow.z);
        
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
        
        //sglScaleMatrixf(mWorldMatrix, mBombProperty.fBombTailScale, mBombProperty.fBombTailScale, mBombProperty.fBombTailScale);
        
        for(int i = 0; i < g_shBombTailIndicesCnt; i++)
        {
            nIndex = i * 3;
            
            sglMultMatrixVectorf(mWorldVertex[0] + nIndex, mWorldMatrix,m_gBombTailParticleVertex + nIndex);
            sglMultMatrixVectorf(mWorldVertex[1] + nIndex, mWorldMatrix1,m_gBombTailParticleVertex + nIndex);
            
        }
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