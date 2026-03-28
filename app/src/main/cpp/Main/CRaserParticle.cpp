//
//  CRaserParticle.cpp
//  SongGL
//
//  Created by itsme on 2014. 3. 12..
//
//

#include "CRaserParticle.h"
#include "CMS3DModelASCII.h"
#include "CHWorld.h"
#include "CSGLCore.h"
#include "CMS3DASCIIMan.h"
#include "CBombRaser.h"
#include "CModelMan.h"
#include "CTextureMan.h"
CRaserParticle::CRaserParticle(IHWorld* pWorld,CBombRaser* pSprite)
{
    mState = SPRITE_RUN;
    mpWorld = pWorld;
    mpSprite = pSprite;
    mpModel = NULL;
    mpModelStart = NULL;
    mpModelEnd = NULL;
    m_bIsVisibleRender = true;
    mbElectricStyle = false;
    
}

CRaserParticle::~CRaserParticle()
{
    if(mpModel)
    {
        delete mpModel;
        mpModel = NULL;
    }
    if(mpModelStart)
    {
        delete mpModelStart;
        mpModelStart = NULL;
    }
    if(mpModelEnd)
    {
        delete mpModelEnd;
        mpModelEnd = NULL;
    }
}

int CRaserParticle::Initialize(CModelMan* pModelMan,const char* sModelPath,const char* sTexturePath,const char* sModelStartEndPath)
{
    if(strstr(sModelStartEndPath, "ThunderBolt") != NULL || strstr(sTexturePath, "ThunderBolt") != NULL)
        mbElectricStyle = true;
    
    pModelMan->getModelf(sModelPath, (CMyModel**)&mpModel,false);
    mpModel->pfinalmulMaxtix = new GLfloat[16];
    
    
    //텍스쳐르 설정.
    int nSize = mpModel->GetMeshSize();
//    sglMesh *arrMesh = mpModel->GetMesh();
    sglMesh *arrWroldMesh = mpModel->GetWorldMesh();
    for(int j = 0; j < nSize; j++)
    {
//        sglMesh* pMesh = &arrMesh[j];
        arrWroldMesh[j].nglTexutre = mpWorld->GetTextureMan()->GetTextureID(sTexturePath,0,1);;
    }
    
    string sPath;
    sPath = sModelStartEndPath;
    sPath.append("Start.txt");
    pModelMan->getModelf(sPath.c_str(), (CMyModel**)&mpModelStart,false);
    
    
    sPath = sModelStartEndPath;
    sPath.append("End.txt");
    pModelMan->getModelf(sPath.c_str(), (CMyModel**)&mpModelEnd,false);

    
    mpModel->scale[1] = 1.0f;
    mpModel->scale[2] = 1.0f;
    mpModelStart->scale[0] = .8f; // 10은 원본의 크기 -1은 자신의 길이이다.
    mpModelStart->scale[1] = .8f;
    mpModelStart->scale[2] = .8f;
    mpModelEnd->scale[0] = .8f; // 10은 원본의 크기 -1은 자신의 길이이다.
    mpModelEnd->scale[1] = .8f;
    mpModelEnd->scale[2] = .8f;
    
    return E_SUCCESS;
}

void CRaserParticle::RenderBeginCore(int nTime)
{
    if(mpSprite && mpSprite->GetState() != SPRITE_RUN)
    {
        SetState(SPRITE_READYDELETE);
        mpSprite = NULL;
    }
}


int CRaserParticle::RenderBegin(int nTime)
{
    if(mState != SPRITE_RUN || m_bIsVisibleRender == false) return false;
    sglLoadIdentityf(mpModel->pfinalmulMaxtix);
    
    int nR = rand() % 3;
    if(nR == 0)
    {
        sglTranslateMatrixf(mpModel->pfinalmulMaxtix, 0.1f, 0.0f, 0.0f);
        
        mpModelEnd->scale[0] = .5f; // 10은 원본의 크기 -1은 자신의 길이이다.
        mpModelEnd->scale[1] = .5f;
        mpModelEnd->scale[2] = .5f;
        mpModelEnd->orientation[1] = 45.f;

    }
    else if(nR == 1)
    {
        sglTranslateMatrixf(mpModel->pfinalmulMaxtix, 0.2f, 0.0f, 0.0f);
        mpModelEnd->scale[0] = .6f; // 10은 원본의 크기 -1은 자신의 길이이다.
        mpModelEnd->scale[1] = .6f;
        mpModelEnd->scale[2] = .6f;
        mpModelEnd->orientation[1] = 95.f;

    }
    else
    {
        mpModelEnd->scale[0] = .8f; // 10은 원본의 크기 -1은 자신의 길이이다.
        mpModelEnd->scale[1] = .8f;
        mpModelEnd->scale[2] = .8f;
        mpModelEnd->orientation[1] = 10.f;
    }
    
    
    
    
    sglRotateRzRyRxMatrixf(mpModel->pfinalmulMaxtix, GetGGTime() * 0.1f, 0.f, 0.f);
    
    

    
    mpModel->updatef(nTime);
    mpModelStart->updatef(nTime);
    mpModelEnd->updatef(nTime);
    return E_SUCCESS;
}

int CRaserParticle::Render()
{
    if(mState != SPRITE_RUN || m_bIsVisibleRender == false) return false;
    
    int nSize;
    sglMesh *arrMesh;
    sglMesh *arrWroldMesh;
    
    if(mbElectricStyle)
        glColor4f(0.45f,0.82f,1.0f,0.78f);
    else
        glColor4f(1.0f,1.0f,1.0f,0.6f);
    
    nSize = mpModel->GetMeshSize();
    arrMesh = mpModel->GetMesh();
    arrWroldMesh = mpModel->GetWorldMesh();
    for(int j = 0; j < nSize; j++)
    {
        sglMesh* pMesh = &arrMesh[j];
        glBindTexture(GL_TEXTURE_2D, arrWroldMesh[j].nglTexutre);
        glVertexPointer(3, GL_FLOAT, 0, arrWroldMesh[j].pVetexBuffer);
        glTexCoordPointer(2, GL_FLOAT, 0, pMesh->pCoordBuffer);
        glDrawElements(GL_TRIANGLES, pMesh->shIndicesCnt * 3, GL_UNSIGNED_SHORT, pMesh->pIndicesBuffer);
    }
    
    
    //스타트
    nSize = mpModelStart->GetMeshSize();
    arrMesh = mpModelStart->GetMesh();
    arrWroldMesh = mpModelStart->GetWorldMesh();
    
    for(int j = 0; j < nSize; j++)
    {
        sglMesh* pMesh = &arrMesh[j];
        glBindTexture(GL_TEXTURE_2D, pMesh->nglTexutre);
        glVertexPointer(3, GL_FLOAT, 0, arrWroldMesh[j].pVetexBuffer);
        glTexCoordPointer(2, GL_FLOAT, 0, pMesh->pCoordBuffer);
        glDrawElements(GL_TRIANGLES, pMesh->shIndicesCnt * 3, GL_UNSIGNED_SHORT, pMesh->pIndicesBuffer);
    }
    
    
    if(mbElectricStyle)
        glColor4f(0.75f,0.95f,1.0f,0.65f);
    else
        glColor4f(1.0f,1.0f,1.0f,0.4f);
    //End
    nSize = mpModelEnd->GetMeshSize();
    arrMesh = mpModelEnd->GetMesh();
    arrWroldMesh = mpModelEnd->GetWorldMesh();
    
    for(int j = 0; j < nSize; j++)
    {
        sglMesh* pMesh = &arrMesh[j];
        glBindTexture(GL_TEXTURE_2D, pMesh->nglTexutre);
        glVertexPointer(3, GL_FLOAT, 0, arrWroldMesh[j].pVetexBuffer);
        glTexCoordPointer(2, GL_FLOAT, 0, pMesh->pCoordBuffer);
        glDrawElements(GL_TRIANGLES, pMesh->shIndicesCnt * 3, GL_UNSIGNED_SHORT, pMesh->pIndicesBuffer);
    }

    return E_SUCCESS;
}

int CRaserParticle::RenderEnd()
{
    if(mState != SPRITE_RUN) return false;
    return E_SUCCESS;
}

int CRaserParticle::Arrange(float BombStartPosition,SPoint *pPosition,SPoint* ptTargetPos,SVector *pvDirAngle,SVector *pvtDir,float fTotalLength)
{
    //초기발사속도
    //방향
    //각도
    //등등의 초기값을 사용한다.
    SPoint ptStart;
    memcpy(&ptStart,pPosition,sizeof(SPoint));
    
    ptStart.x += pvtDir->x * BombStartPosition;
    ptStart.y += pvtDir->y * BombStartPosition;
    ptStart.z += pvtDir->z * BombStartPosition;
    
    mpModel->orientation[1] = pvDirAngle->z; //포의 방향
    mpModel->orientation[2] = pvDirAngle->x; //상하각
    
    mpModel->scale[0] = fTotalLength / 10.f - 1.f; // 10은 원본의 크기 -1은 자신의 길이이다.
    
    mpModel->position[0] = ptStart.x;
    mpModel->position[1] = ptStart.y;
    mpModel->position[2] = ptStart.z;
    
    mpModelStart->orientation[1] = mpModel->orientation[1];
    mpModelStart->orientation[2] = mpModel->orientation[2];
    mpModelStart->position[0] = ptStart.x;
    mpModelStart->position[1] = ptStart.y;
    mpModelStart->position[2] = ptStart.z;
    
    mpModelEnd->orientation[1] = mpModel->orientation[1];
    mpModelEnd->orientation[2] = mpModel->orientation[2];
    
    mpModelEnd->position[0] = ptTargetPos->x;
    mpModelEnd->position[1] = ptTargetPos->y;
    mpModelEnd->position[2] = ptTargetPos->z;
    
    
    CMS3DASCII* pMS3D =  mpModel->GetMS3DASCII();
    sglMesh* pMesh = NULL;
    int nIndex = 0;
    int nSize = pMS3D->GetMeshSize();
    //기억한 텍스처를 사용하여 메쉬의 텍스처를 입힌다.
    for(int i = 0; i < nSize; i++)
    {
        pMesh = &pMS3D->GetMesh()[i];
        for (int j = 0; j < pMesh->shCoordCount; j++)
        {
            nIndex = j*2;
            //U만 곱해준다.

            if(pMesh->pCoordBuffer[nIndex] != 0.0f && mpModel->scale[0] != 0) //U 좌표만.
            {
                pMesh->pCoordBuffer[nIndex] = mpModel->scale[0]; //몇번반복하는지..
            }
        }
    }
    return E_SUCCESS;
}
