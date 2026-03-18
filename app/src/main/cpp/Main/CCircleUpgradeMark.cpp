//
//  CCircleUpgradeMark.cpp
//  SongGL
//
//  Created by 호학 송 on 2014. 2. 19..
//
//
#include <math.h>
#include "CCircleUpgradeMark.h"


CCircleUpgradeMark::CCircleUpgradeMark(IHWorld* pWorld,
                   CSprite* pOwner,float fRadius)
{
    mpWorld = pWorld;
    mpOwner = pOwner;
    mfRadius = fRadius;
    mpPoint = 0;
    mCircleCount = 1;
    mbInit = false;
    m_bIsVisibleRender = false;
    mState = SPRITE_RUN;
    mpWorldPoint = NULL;
}

CCircleUpgradeMark::~CCircleUpgradeMark()
{
    if(mpPoint)
    {
        delete[] mpPoint;
        mpPoint= 0;
    }
    
    if(mpWorldPoint)
    {
        delete[] mpWorldPoint;
        mpWorldPoint = 0;
    }
}

void CCircleUpgradeMark::SetCircleCount(int nCnt)
{
    if(nCnt == mCircleCount || nCnt > CIRCLEMAX) return; //Max 는 10개
    mCircleCount = nCnt;
    mbInit = false;
}

int CCircleUpgradeMark::Initialize(SPoint *pPosition,SVector *pvDirection)
{
	
    mpPoint = new float[3 * CIRCLEPOINTCNT];
    
    //		for (GLubyte i = 0; i < LINES; i++)
    for (GLubyte i = 0; i < CIRCLEPOINTCNT; i++)
    {
        mpPoint[i*3] = mfRadius*(GLfloat)cosf(2.0f*PIf*i/CIRCLEDIV);
        mpPoint[i*3+1] = mfRadius*(GLfloat)sinf(2.0f*PIf*i/CIRCLEDIV);
        mpPoint[i*3+2] = 0.0f;
    }
    
    
    return E_SUCCESS;
}

void CCircleUpgradeMark::RenderBeginCore(int nTime)
{
    if(mpOwner && mpOwner->GetState() != SPRITE_RUN)
    {
        mState = SPRITE_READYDELETE;
        mpOwner = NULL;
        return ;
    }
    
    if(mbInit == false)
    {
        if(mpWorldPoint) delete[] mpWorldPoint;
        mpWorldPoint = new float[3 * CIRCLEPOINTCNT * mCircleCount];
        mbInit = true;
    }
    
    float matrix[16];
    GLfloat	rotationMatrix[16];
    float fGGTime = (float)GetGGTime() + (float)((int)(long)this % 1000);
    SPoint ptNow;
    
    mpOwner->GetPosition(&ptNow);
    for (int j = 0; j < mCircleCount; j++)
    {
        sglLoadIdentityf(matrix);
        sglTranslateMatrixf(matrix, ptNow.x, ptNow.y, ptNow.z);
        sglLoadIdentityf(rotationMatrix);
        sglRotateRzRyRxMatrixf(matrix, 90.f + j * 20, 0.f, 0.f);
        sglRotateRzRyRxMatrixf(matrix, 0.f, 0.f, fGGTime * 0.8f + 60.f * j);
        sglMultMatrixf(matrix, matrix, rotationMatrix);
        for (GLubyte i = 0; i < CIRCLEPOINTCNT; i++)
        {
            sglMultMatrixVectorf(&mpWorldPoint[i*3+(3 * CIRCLEPOINTCNT*j)], matrix, &mpPoint[i*3]);
        }
    }
}

int CCircleUpgradeMark::RenderBegin(int nTime)
{
    
    if(mpOwner == NULL || mState != SPRITE_RUN || m_bIsVisibleRender == false) return E_SUCCESS;
    return E_SUCCESS;
}

int CCircleUpgradeMark::Render()
{
    if(mpOwner == NULL || mState != SPRITE_RUN || m_bIsVisibleRender == false || mpWorldPoint == NULL) return E_SUCCESS;
    
    float fW1 = 16.f;
    float fW3 = 4.f;
    
    if(gnDisplayType == DISPLAY_IPAD)
    {
        fW1 = 32.f;
        fW3 = 8.f;
    }
    
    glDisable(GL_TEXTURE_2D);
    
//    glEnableClientState(GL_VERTEX_ARRAY);
    for (int i = 0; i < mCircleCount; i++)
    {
        glColor4f(0.953125, 0.7265625, 1.0f, 0.2f);
        glLineWidth(fW1);
        glVertexPointer(3, GL_FLOAT, 0, mpWorldPoint + (3 * CIRCLEPOINTCNT * i));
        glDrawArrays(GL_LINE_STRIP, 0, CIRCLEPOINTCNT);
        
        glColor4f(0.953125, 0.7265625, 1.0f, 0.5f);
        glLineWidth(fW3);
        glDrawArrays(GL_LINE_STRIP, 0, CIRCLEPOINTCNT);
    }
//    glDisableClientState(GL_VERTEX_ARRAY);
    glLineWidth(1.0f);
    glEnable(GL_TEXTURE_2D);
    return E_SUCCESS;
}

int CCircleUpgradeMark::RenderEnd()
{
    return E_SUCCESS;
}
