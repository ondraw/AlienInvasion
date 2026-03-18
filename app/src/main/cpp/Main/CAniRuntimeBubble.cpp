//
//  CBubbleParticle.cpp
//  SongGL
//
//  Created by itsme on 12. 10. 30..
//
//

#include "CAniRuntimeBubble.h"
#include "CMS3DModelASCII.h"
#include "CHWorld.h"
#include "CSGLCore.h"
#include "CMS3DASCIIMan.h"
#include "sGLUtils.h"


CAniRuntimeBubble::CAniRuntimeBubble(IHWorld* pWorld,CSprite* pSprite,int nMaxSec,int nUpgradeCount)
{
    mpWorld = pWorld;
    mpSprite = pSprite;
    mpModel = NULL;
    mAlpha = 0.0f;
    mnMaxAttacted = 0;
    
    mState = SPRITE_RUN;
    
    if(nUpgradeCount == 0)
    {
        mnUpgradeCount = 3;
        mnMaxTime = GetGGTime() + nMaxSec * 1000;
    }
    else
    {
        mnUpgradeCount = nUpgradeCount;
        mnMaxTime = 0;
    }
}


CAniRuntimeBubble::~CAniRuntimeBubble()
{
    if(mpModel)
    {
        delete mpModel;
        mpModel = NULL;
    }
}

void CAniRuntimeBubble::SetScale(float fx,float fy,float fz)
{
    mpModel->scale[0] = fx;
    mpModel->scale[1] = fy;
    mpModel->scale[2] = fz;
}
float CAniRuntimeBubble::GetScale()
{
    return mpModel->scale[0];
}

int CAniRuntimeBubble::Initialize(SPoint *pPosition,SVector *pvDirection)
{
    memcpy(&mPosition, pPosition, sizeof(SPoint));
    ((CHWorld*)mpWorld)->GetSGLCore()->GetModelMan()->getModelf(ANINAME_BUBBLE, (CMyModel**)&mpModel);
    
    mpModel->position[0] = pPosition->x;
    mpModel->position[1] = pPosition->y;
    mpModel->position[2] = pPosition->z;
    
    mpModel->orientation[0] = 0;
    SetColor(1.0f,1.0f,1.0f);
    SetScale(0.07f, 0.07f, 0.07f);
    return E_SUCCESS;
}

void CAniRuntimeBubble::Attacted() //공격당하면 조금 불투명하게 하자.
{
    mnMaxAttacted = GetGGTime() + 800l; //0.8초동안 불투명하게.
}

void CAniRuntimeBubble::ResetBubble(int nMaxSec)
{
    //현재 시간 보다 좀더 오래하게??
    mnMaxTime = mnMaxTime + (nMaxSec * 1000);
}

void CAniRuntimeBubble::RenderBeginCore(int nTime)
{
    unsigned long ggtime = GetGGTime();
    if(mnUpgradeCount >= 3)
    {
        if(mnMaxAttacted == 0)
            mAlpha = 0.4f; //점점점 선명하게 0~0.5f 값을 가지게 하자. 너무 뚜렷하면 디자인이 탈로난다.
        else if( ggtime <= mnMaxAttacted )
            mAlpha = 0.9f;
        else
        {
            mAlpha = 0.4f;
            mnMaxAttacted = 0;
        }
    }
    else if(mnUpgradeCount == 2)
    {
        if(mnMaxAttacted == 0)
            mAlpha = 0.25f; //점점점 선명하게 0~0.5f 값을 가지게 하자. 너무 뚜렷하면 디자인이 탈로난다.
        else if( ggtime <= mnMaxAttacted )
            mAlpha = 0.7f;
        else
        {
            mAlpha = 0.25f;
            mnMaxAttacted = 0;
        }
    }
    else if(mnUpgradeCount == 1)
    {
        if(mnMaxAttacted == 0)
            mAlpha = 0.15f; //점점점 선명하게 0~0.5f 값을 가지게 하자. 너무 뚜렷하면 디자인이 탈로난다.
        else if( ggtime <= mnMaxAttacted )
            mAlpha = 0.5f;
        else
        {
            mAlpha = 0.15f;
            mnMaxAttacted = 0;
        }
    }
    
    if (mnMaxTime && ggtime >= mnMaxTime)
    {
        if(mpSprite && mpSprite->GetState() == SPRITE_RUN)
        {
            mpSprite->SetAniRuntimeBubble(NULL);
            mpSprite = NULL;
            SetState(SPRITE_READYDELETE);
        }
    }
    
    //버블이 죽는 경우가 있다.
    if(mpSprite && mpSprite->GetState() != SPRITE_RUN)
    {
        mpSprite->SetAniRuntimeBubble(NULL);
        mpSprite = NULL;
        SetState(SPRITE_READYDELETE);
    }
}

int CAniRuntimeBubble::RenderBegin(int nTime)
{
    if(mpSprite && mpSprite->GetState() == SPRITE_RUN && mpSprite->IsVisibleRender())
    {
        SPoint st;
        mpSprite->GetPosition(&st);
        mpModel->position[0] = st.x;
        mpModel->position[1] = st.y;
        mpModel->position[2] = st.z;
        
        mpModel->updatef(nTime);
    }
    return E_SUCCESS;
}
void CAniRuntimeBubble::SetColor(float r,float g, float b)
{
    mfColor[0] = r;
    mfColor[1] = g;
    mfColor[2] = b;
}

int CAniRuntimeBubble::Render()
{
    if(mpSprite && mpSprite->GetState() == SPRITE_RUN && mpSprite->IsVisibleRender())
    {
        //이것을 그냥 사용해도 되지만. glEnableClientState을 재설정하지 않게 하기 위해
        //그리고 에너지업일때 glDisableClientState(GL_VERTEX_ARRAY); 을 하면 다른 컨트롤이 사라지는 현상이 있다.
        //mpModel->renderf(mAlpha);
        
        int nSize = mpModel->GetMeshSize();
        sglMesh *arrMesh = mpModel->GetMesh();
        sglMesh *arrWroldMesh = mpModel->GetWorldMesh();
        
        //glColor4f(0.22745098039216f,0.41960784313725f,0.6078431372549f,mAlpha);
        glColor4f(mfColor[0],mfColor[1],mfColor[2],mAlpha);
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

int CAniRuntimeBubble::RenderEnd()
{
    return E_SUCCESS;
}