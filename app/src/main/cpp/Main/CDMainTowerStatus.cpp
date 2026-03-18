//
//  CDMainTowerStatus.cpp
//  SongGL
//
//  Created by 호학 송 on 13. 10. 29..
//
//
#include <math.h>
#include "CDMainTowerStatus.h"
#include "sGLDefine.h"
#include "IHWorld.h"
#include "IAction.h"
#include "CSprite.h"

CDMainTowerStatus::CDMainTowerStatus(CSprite* pSprite) : CSpriteStatus(pSprite)
{
}
CDMainTowerStatus::~CDMainTowerStatus()
{
}

int CDMainTowerStatus::Initialize(SPoint *pPosition,SVector *pvDirection)
{
#define FIRESIZE_W 1.0f
    GLfloat pPanelVertexY[]=
    {
        -FIRESIZE_W,   FIRESIZE_W, 0.0f,
        -FIRESIZE_W,  -FIRESIZE_W, 0.0f,
        FIRESIZE_W ,  -FIRESIZE_W, 0.0f,
        FIRESIZE_W ,  FIRESIZE_W,  0.0f
    };
    
    int nIndex = 0;
    for (int i = 0; i < 4; i++)
    {
        nIndex = i*4;
        
        //Red
        *(mfEnergeColor+nIndex) = 1.0f;
        *(mfEnergeColor+nIndex+1) = 0.0f;
        *(mfEnergeColor+nIndex+2) = 0.0f;
        *(mfEnergeColor+nIndex+3) = 1.0f;
        
        //Blue
        *(mfRelealEnergeColor+nIndex) = 0.0f;
        *(mfRelealEnergeColor+nIndex+1) = 1.0f;
        *(mfRelealEnergeColor+nIndex+2) = 1.0f;
        *(mfRelealEnergeColor+nIndex+3) = 1.0f;
    }
    
    float matrix[16];
    sglLoadIdentityf(matrix);
    sglScaleMatrixf(matrix, 4.f, .7f, 1.0f);
    
    //속도를 높이기 위해서 for 문장을 제거했다. 그리고 배열을 사용하지 않고 포인터를 사용하였다.
    sglMultMatrixVectorf(mfOrgVertex, matrix,pPanelVertexY);
    sglMultMatrixVectorf(mfOrgVertex+3, matrix,pPanelVertexY+3);
    sglMultMatrixVectorf(mfOrgVertex+6, matrix,pPanelVertexY+6);
    sglMultMatrixVectorf(mfOrgVertex+9, matrix,pPanelVertexY+9);
    
    
    mfBarWidth= *(mfOrgVertex+6) - *(mfOrgVertex);
    mRate3 = 1.f;
    return E_SUCCESS;
}

void CDMainTowerStatus::RenderBeginCore(int nTime)
{
    if(mpSprite->GetState() != SPRITE_RUN) return ;
    
    SGLCAMERA* dirCarmera = mpSprite->GetWorld()->GetCamera();
    SPoint ptPosition;
    mpSprite->GetPosition(&ptPosition);
    ptPosition.y += 11.f;//mpSprite->GetRadius();
    
    //카메라의 방향으로 설정을 한다.
    float fAngle = atan2(-dirCarmera->viewDir.z,dirCarmera->viewDir.x) * 180.0 / PI - 90.f;
    
    
    sglLoadIdentityf(mMatrix);
    sglTranslateMatrixf(mMatrix, ptPosition.x, ptPosition.y, ptPosition.z);
    sglRotateRzRyRxMatrixf(mMatrix,
                           0,
                           fAngle,
                           0);
    
    sglLoadIdentityf(mMatrix2);
    sglTranslateMatrixf(mMatrix2, ptPosition.x, ptPosition.y, ptPosition.z);
    sglRotateRzRyRxMatrixf(mMatrix2,
                           0,
                           fAngle,
                           0);
}

int CDMainTowerStatus::RenderBegin(int nTime)
{
    
    if(mpSprite->GetState() != SPRITE_RUN) return E_SUCCESS;
    
    sglMultMatrixVectorf(mfEnergeVertex, mMatrix,mfOrgVertex);
    sglMultMatrixVectorf(mfEnergeVertex+3, mMatrix,mfOrgVertex+3);
    sglMultMatrixVectorf(mfEnergeVertex+6, mMatrix,mfOrgVertex+6);
    sglMultMatrixVectorf(mfEnergeVertex+9, mMatrix,mfOrgVertex+9);
    
    
    memcpy(mfRelealEnergeVertex,mfOrgVertex,12*sizeof(float));
    *(mfRelealEnergeVertex+6) = *(mfRelealEnergeVertex) + mRate3;
    *(mfRelealEnergeVertex+9) = *(mfRelealEnergeVertex+3) +  mRate3;
    
    sglMultMatrixVectorf(mfRelealEnergeVertex, mMatrix2,mfRelealEnergeVertex);
    sglMultMatrixVectorf(mfRelealEnergeVertex+3, mMatrix2,mfRelealEnergeVertex+3);
    sglMultMatrixVectorf(mfRelealEnergeVertex+6, mMatrix2,mfRelealEnergeVertex+6);
    sglMultMatrixVectorf(mfRelealEnergeVertex+9, mMatrix2,mfRelealEnergeVertex+9);
    
    return E_SUCCESS;
}


int CDMainTowerStatus::Render()
{
    if(mpSprite->GetState() != SPRITE_RUN || mpSprite->GetType() == ACTORTYPE_TYPE_GHOST) return E_SUCCESS;
    
    glVertexPointer(3, GL_FLOAT, 0, mfEnergeVertex);
    glColorPointer(4, GL_FLOAT, 0, mfEnergeColor);
    glDrawElements(GL_TRIANGLE_STRIP, gshPanelIndicesCnt, GL_UNSIGNED_SHORT, gPanelIndices);
    
    glVertexPointer(3, GL_FLOAT, 0, mfRelealEnergeVertex);
    glColorPointer(4, GL_FLOAT, 0, mfRelealEnergeColor);
    glDrawElements(GL_TRIANGLE_STRIP, gshPanelIndicesCnt, GL_UNSIGNED_SHORT, gPanelIndices);
  
    return E_SUCCESS;
}