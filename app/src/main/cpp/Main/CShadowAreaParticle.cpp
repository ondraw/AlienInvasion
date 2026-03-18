//
//  CShadowAreaParticle.cpp
//  SongGL
//
//  Created by itsme on 2013. 10. 25..
//
//

#include "CShadowAreaParticle.h"
#include "sGLTrasform.h"
#include "CMS3DASCII.h"
#include "CHWorld.h"
#include "CMS3DModelASCIIMan.h"
#include "CSGLCore.h"
#include "CSprite.h"

CShadowAreaParticle::CShadowAreaParticle(IHWorld* pWorld,CSprite* pOwner)
{
    mState = SPRITE_RUN;
    mpModel = NULL;
    mpWorld = (CHWorld*)pWorld;
    mpOwner = pOwner;
}

CShadowAreaParticle::~CShadowAreaParticle()
{
    if(mpModel)
    {
        delete mpModel;
        mpModel = NULL;
    }
}


int CShadowAreaParticle::Initialize(SPoint *pPosition,SVector *pvDirection,float fArea)
{
    SPoint ptNow;
    int nIndex = 0;
    mState = SPRITE_RUN;
    float matrix[16];
    
    


    mpModel = new CMS3DASCII(NULL);
    mpModel->Load("Circle.txt",false);
    sglMesh *arrMesh = mpModel->GetMesh();
    if(mpModel == NULL)
    {
        HLogE("[Error] %s can't load\n","Circle.txt");
        return -1;
    }
    
    
    sglLoadIdentityf(matrix);
    sglTranslateMatrixf(matrix,pPosition->x,pPosition->y,pPosition->z); //회전전에 이동을 해야 한다.
    sglScaleMatrixf(matrix, fArea, 1.0f, fArea);

    
    
    sglMesh* pMesh = &arrMesh[0];
    float fBefore = 0.f;
    
    for(int i = 0; i < pMesh->nVetexCnt; i++)
    {
        nIndex = i*3;
        
        //좌표를 이동한다.
        sglMultMatrixVectorf(&pMesh->pVetexBuffer[nIndex],
                             matrix,
                             &pMesh->pVetexBuffer[nIndex]);
        
        
        //지형의 높이로 변경해준다.
        ptNow.x = pMesh->pVetexBuffer[nIndex];
        ptNow.y = pMesh->pVetexBuffer[nIndex + 1];
        ptNow.z = pMesh->pVetexBuffer[nIndex + 2];
        if(mpWorld->GetSGLCore()->GetPositionY(&ptNow) < 0)
        {
            ptNow.y = fBefore; //벽위로 들뜨지 말게 하자.
        }

        
        pMesh->pVetexBuffer[nIndex] = ptNow.x;
        pMesh->pVetexBuffer[nIndex + 1] = ptNow.y + 2.f;
        pMesh->pVetexBuffer[nIndex + 2] = ptNow.z;
        
        fBefore = pMesh->pVetexBuffer[nIndex + 1];

    }
    return E_SUCCESS;
}


void CShadowAreaParticle::RenderBeginCore(int nTime)
{
    if(mpOwner == NULL || ( mpOwner && mpOwner->GetState() != SPRITE_RUN))
    {
        mState = SPRITE_READYDELETE;
        mpOwner = NULL;
    }
}

int CShadowAreaParticle::RenderBegin(int nTime)
{
    return E_SUCCESS;
}

int CShadowAreaParticle::Render()
{
    if(mpOwner == NULL || ( mpOwner && mpOwner->GetState() != SPRITE_RUN))
    {
        mState = SPRITE_READYDELETE;
        mpOwner = NULL;
        return E_SUCCESS;
    }
    
    if(mpOwner->IsVisibleRender() == false) return E_SUCCESS;
    sglMesh* pMesh = &mpModel->GetMesh()[0];

//    
    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    
    glColor4f(0.0, 0.4, 0.4, 0.2);//Render 50% black shadow color on top of whatever the floor appareance is
    
//    glVertexPointer(3, GL_FLOAT, 0, mfVertex);
//    glDrawElements(GL_TRIANGLE_STRIP, gshPanelIndicesCnt, GL_UNSIGNED_SHORT, gPanelIndices);

    glVertexPointer(3, GL_FLOAT, 0, pMesh[0].pVetexBuffer);
    glTexCoordPointer(2, GL_FLOAT, 0, pMesh->pCoordBuffer);
    glDrawElements(GL_TRIANGLES, pMesh->shIndicesCnt * 3, GL_UNSIGNED_SHORT, pMesh->pIndicesBuffer);
    
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    
    return E_SUCCESS;
}
