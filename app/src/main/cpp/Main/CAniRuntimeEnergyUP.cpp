//
//  CAniRuntimeEnergyUP.cpp
//  SongGL
//
//  Created by itsme on 12. 10. 15..
//
//

#include "CAniRuntimeEnergyUP.h"
#include "CMS3DModelASCII.h"
#include "CHWorld.h"
#include "CSGLCore.h"
#include "CMS3DASCIIMan.h"


CAniRuntimeEnergyUP::CAniRuntimeEnergyUP(IHWorld* pWorld,CSprite* pSprite)
{
    mpWorld = pWorld;
    mpSprite = pSprite;
    mpModel = NULL;
    mAlpha = 0.0f;
    mnNowTime = 0;
}

CAniRuntimeEnergyUP::~CAniRuntimeEnergyUP()
{
    if(mpModel)
    {
        delete mpModel;
        mpModel = NULL;
    }
}

int CAniRuntimeEnergyUP::Initialize(SPoint *pPosition,SVector *pvDirection)
{
    
    memcpy(&mPosition, pPosition, sizeof(SPoint));
//    //AniEnergyUp.txt
//    mpModel = new CMS3DModelASCII(
//                                  ((CHWorld*)mpWorld)->GetSGLCore()->
//                                  GetMS3DModelASCIIMan()->Load(ANINAME_ANIENERGYUP)
//                                  );
    
    ((CHWorld*)mpWorld)->GetSGLCore()->GetModelMan()->getModelf(ANINAME_ANIENERGYUP, (CMyModel**)&mpModel);
    
    mpModel->scale[0] = 0.06f;
    mpModel->scale[1] = 0.06f;
    mpModel->scale[2] = 0.06f;
    
    mpModel->position[0] = pPosition->x;
    mpModel->position[1] = pPosition->y;
    mpModel->position[2] = pPosition->z;
    
    mpModel->orientation[0] = 0;
    
    return E_SUCCESS;
}

void CAniRuntimeEnergyUP::RenderBeginCore(int nTime)
{
    int n = mnNowTime % 1000; //1초
    mAlpha = (float)n / 2000; //점점점 선명하게 0~0.5f 값을 가지게 하자. 너무 뚜렷하면 디자인이 탈로난다.
    
    if(mnNowTime > 1000)
    {
        SetState(SPRITE_READYDELETE);
    }
    mnNowTime += nTime;
}

int CAniRuntimeEnergyUP::RenderBegin(int nTime)
{
    if(mpSprite && mpSprite->GetState() == SPRITE_RUN)
    {
        SPoint st;
        mpSprite->GetPosition(&st);
        mpModel->position[0] = st.x;
        mpModel->position[1] = st.y;
        mpModel->position[2] = st.z;
        
        mpModel->updatef(nTime);
    }
    else
    {
        mpSprite = NULL; //더이상 그리지 말자.
        SetState(SPRITE_READYDELETE);
    }
    
    return E_SUCCESS;
}

int CAniRuntimeEnergyUP::Render()
{
    if(mpSprite && mpSprite->GetState() == SPRITE_RUN)
    {
        //이것을 그냥 사용해도 되지만. glEnableClientState을 재설정하지 않게 하기 위해
        //그리고 에너지업일때 glDisableClientState(GL_VERTEX_ARRAY); 을 하면 다른 컨트롤이 사라지는 현상이 있다.
        //mpModel->renderf(mAlpha);
        
        int nSize = mpModel->GetMeshSize();
        sglMesh *arrMesh = mpModel->GetMesh();
        sglMesh *arrWroldMesh = mpModel->GetWorldMesh();
        
        glColor4f(1.0f,1.0f,1.0f,mAlpha);
        for(int j = 0; j < nSize; j++)
        {
            sglMesh* pMesh = &arrMesh[j];
            glBindTexture(GL_TEXTURE_2D, pMesh->nglTexutre);
            glVertexPointer(3, GL_FLOAT, 0, arrWroldMesh[j].pVetexBuffer);
            glTexCoordPointer(2, GL_FLOAT, 0, pMesh->pCoordBuffer);
            glDrawElements(GL_TRIANGLES, pMesh->shIndicesCnt * 3, GL_UNSIGNED_SHORT, pMesh->pIndicesBuffer);
        }
    }
    return E_SUCCESS;
}

int CAniRuntimeEnergyUP::RenderEnd()
{
    return E_SUCCESS;
}