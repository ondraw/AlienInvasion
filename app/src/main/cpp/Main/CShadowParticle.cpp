//
//  CShadowParticle.cpp
//  SongGL
//
//  Created by 송 호학 on 12. 1. 10..
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//
#include "CHWorld.h"
#include "CShadowParticle.h"
#include "sGLTrasform.h"
#include "CSprite.h"
#include "CMS3DModelASCII.h"
#include "CSGLCore.h"
#include "CModelMan.h"
CShadowParticle::CShadowParticle(CSprite* pOwner)
{
    m_bIsVisibleRender = true;
    mState = SPRITE_RUN;
    m_bReliveEffect = false;
    mpOwner = pOwner;
    mpModel = NULL;
}

CShadowParticle::~CShadowParticle()
{
    if(mpModel)
    {
        delete mpModel;
        mpModel = NULL;
    }
}



int CShadowParticle::Initialize(SPoint *pPosition,SVector *pvDirection)
{
    mState = SPRITE_RUN;
    string sModelPath;
    PROPERTY_TANK* pProperty = mpOwner->GetPropertyTank();
    sModelPath = pProperty->sModelPath;
    sModelPath.append("Shadow.txt");
    ((CHWorld*)mpOwner->GetWorld())->GetSGLCore()->GetModelMan()->getModelf(sModelPath.c_str(), &mpModel);
    
    mpModel->scale[0] = mpOwner->GetPropertyTank()->fScale;
    mpModel->scale[2] = mpOwner->GetPropertyTank()->fScale;
    return E_SUCCESS;
}

void CShadowParticle::RenderBeginCore(int nTime)
{
    if(mpOwner->GetState() != SPRITE_RUN)
    {
        SetState(SPRITE_READYDELETE);
    }
}


int CShadowParticle::RenderBegin(int nTime)
{
    if(mState != SPRITE_RUN) return E_SUCCESS;
    if(m_bIsVisibleRender == false || m_bReliveEffect == true) return E_SUCCESS;
    
    SPoint ptNow;
    float* fOrientation;
    mpOwner->GetPosition(&ptNow);
    fOrientation = mpOwner->GetOrientaion();
    
    
    
    if(mpOwner->GetType() == 5) //비행기이면
    {
        ((CHWorld*)mpOwner->GetWorld())->GetPositionY(&ptNow);
        mpModel->orientation[1] = fOrientation[1];
    }
    else
        memcpy(mpModel->orientation,fOrientation,sizeof(mpModel->orientation));
    
    mpModel->position[0] = ptNow.x + 1.5f;
    mpModel->position[1] = ptNow.y + 0.5f;
    mpModel->position[2] = ptNow.z + 1.5f;

    mpModel->updatef(nTime);
    return E_SUCCESS;
}


int CShadowParticle::Render()
{
    if(mState != SPRITE_RUN) return E_SUCCESS;
    if(m_bIsVisibleRender == false || m_bReliveEffect == true || mpOwner->GetType() == ACTORTYPE_TYPE_GHOST) return E_SUCCESS;
    
    CMS3DModelASCII* pModel = (CMS3DModelASCII*)mpModel;
    
    int nSize = pModel->GetMeshSize();
    sglMesh *arrWroldMesh = pModel->GetWorldMesh();
    sglMesh *arrMesh = pModel->GetMesh();
    
    glDisable(GL_TEXTURE_2D);
//    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glColor4f(0.0, 0.0, 0.0, 0.4);//Render 50% black shadow color on top of whatever the floor appareance is
    
    for(int j = 0; j < nSize; j++)
    {
        sglMesh* pMesh = &arrMesh[j];
        glVertexPointer(3, GL_FLOAT, 0, arrWroldMesh[j].pVetexBuffer);
        glTexCoordPointer(2, GL_FLOAT, 0, pMesh->pCoordBuffer);
        glDrawElements(GL_TRIANGLES, pMesh->shIndicesCnt * 3, GL_UNSIGNED_SHORT, pMesh->pIndicesBuffer);
    }
    glEnable(GL_TEXTURE_2D);
//    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    return E_SUCCESS;
}

//void CShadowParticle::CaculateFighter(float fAngle[3],float* pMatrix,SPoint& ptPos,float fScale)
//{
//    float matrix[16];
//    GLfloat gPanel[]=
//    {
//        -1, 0, -1,
//        -1, 0,  1,
//         1, 0,  0
//    };
//
//    
//    sglLoadIdentityf(matrix);
//    //+ SPRITE_GROUND_HEIGHT을 안하는 이유는 이미 상단에서 해버렸다.
//    sglTranslateMatrixf(matrix, ptPos.x + 1.f, ptPos.y, ptPos.z + 1.f);
//    sglRotateRzRyRxMatrixf(matrix,
//                           fAngle[0],
//                           fAngle[1],
//                           fAngle[2]);
//    if(pMatrix)
//        sglMultMatrixf(matrix, matrix, pMatrix);
//    sglScaleMatrixf(matrix, fScale, 1.0f, fScale); //고도에 따라서 그려진다.
//    
//    sglMultMatrixVectorf(mfVertex, matrix,gPanel);
//    sglMultMatrixVectorf(mfVertex+3, matrix,gPanel+3);
//    sglMultMatrixVectorf(mfVertex+6, matrix,gPanel+6);
////    sglMultMatrixVectorf(mfVertex+9, matrix,gPanel+9);
//}
//
//void CShadowParticle::Caculate(float fAngle[3],float* pMatrix,float fPos[3])
//{
//    float matrix[16];
//    
//    sglLoadIdentityf(matrix);
//    //+ SPRITE_GROUND_HEIGHT을 안하는 이유는 이미 상단에서 해버렸다.
//    sglTranslateMatrixf(matrix, *fPos + 1.f, *(fPos+1), *(fPos+2) + 1.f);
//    sglRotateRzRyRxMatrixf(matrix,
//                           fAngle[0],
//                           fAngle[1],
//                           fAngle[2]);
//    
//    sglMultMatrixf(matrix, matrix, pMatrix);
//    sglScaleMatrixf(matrix, 6.5f, 1.0f, 4.0f);
//    
//    sglMultMatrixVectorf(mfVertex, matrix,gPanelVertexZ);
//    sglMultMatrixVectorf(mfVertex+3, matrix,gPanelVertexZ+3);
//    sglMultMatrixVectorf(mfVertex+6, matrix,gPanelVertexZ+6);
//    sglMultMatrixVectorf(mfVertex+9, matrix,gPanelVertexZ+9);
//}
