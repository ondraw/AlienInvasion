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

CNewBoosterParticle::CNewBoosterParticle(IHWorld* pWorld,CSprite* pOwner)
{
    mpOwner = pOwner;
    mpWorld = pWorld;
    mTailTextureID = 0;
    mbVisible = true;
    mppWorldVertex = 0;
    mppWorldVertex2 = 0;
    mnBoosterCnt = 0;
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
    
    int nB = rand() % 3;
    int nIndex = 0;
    int nIndexj = 0;
    
    SVector vtDir;
    mpOwner->GetModelDirection(&vtDir);
    //SVector *pDir = GetModelDirection();
    float fAngle = atan2(vtDir.z,vtDir.x) * 180.0 / PI;
    
    for(int z = 0; z < mnBoosterCnt; z++)
    {
        for(int j = 0; j < 3; j++)
        {
            sglLoadIdentityf(mWorldMatrix);
            sglLoadIdentityf(mWorldMatrix2);
            
            
            mpOwner->GetBoosterPos(z,&fUp,&fDown);
            //sglLog("x=%f,y=%f,z=%f\n",mPSRightBP[0].x, mPSRightBP[0].y, mPSRightBP[0].z);
            //이동을 먼저한후에
            
            
            if(nB == 1)      sglTranslateMatrixf(mWorldMatrix,fDown[0],fDown[1] - 0.5f,fDown[2]);
            else if(nB == 2) sglTranslateMatrixf(mWorldMatrix,fDown[0],fDown[1] - 0.7f,fDown[2]);
            else        sglTranslateMatrixf(mWorldMatrix,fDown[0],fDown[1] - 0.8f,fDown[2]);

            
            
            //이동을 먼저한후에
            sglTranslateMatrixf(mWorldMatrix2,fDown[0],fDown[1] - ET1_GROUND_HEIGHT,fDown[2]);
            
            
            //회전을 해야 한다.
            sglRotateRzRyRxMatrixf(mWorldMatrix,
                                   0,
                                   fAngle + j * 60,
                                   0);
            
            if(j == 0)
            {
                //회전을 해야 한다.
                sglRotateRzRyRxMatrixf(mWorldMatrix2,
                                       0,
                                       fAngle,
                                       0);
                if(nB == 2) sglScaleMatrixf(mWorldMatrix2, 0.5f, 1.0f, 0.5f);
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
            
        }
    }
    
    
    return E_SUCCESS;
}


int CNewBoosterParticle::Render()
{
    if(mbVisible == false || mState != SPRITE_RUN || mpOwner->GetType() == ACTORTYPE_TYPE_GHOST) return E_SUCCESS;
    
    int nB = rand() % 3;

    if(nB == 1)      glColor4f(1.0f,1.0f,1.0f,1.0f);
    else if(nB == 2) glColor4f(1.0f,1.0f,1.0f,.5f);
    else        glColor4f(1.0f,1.0f,1.0f,0.1f);
    
    
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
    
    if(nB == 1)      glColor4f(1.0f,1.0f,1.0f,0.27f);
    else if(nB == 2) glColor4f(1.0f,1.0f,1.0f,0.4f);
    else        glColor4f(1.0f,1.0f,1.0f,0.24f);

    
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
