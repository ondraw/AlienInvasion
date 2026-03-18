//
//  CBlzSprite.cpp
//  SongGL
//
//  Created by Songs on 11. 7. 26..
//  Copyright 2011 thinkm. All rights reserved.
//

#include "CEMainBlzSprite.h"
#include "CMS3DASCII.h"
#include "CTextureMan.h"
#include "CAIBlzCore.h"
#include "CPicking.h"
#include "CSpriteStatus.h"
#include "CWorld.h"
#include "CHWorld.h"
#include "CSGLCore.h"

CEMainBlzSprite::CEMainBlzSprite(unsigned int nID,unsigned char cTeamID,int nModelID,CTextureMan *pTextureMan,IAction *pAction,IHWorld* pWorld,NETWORK_TEAMTYPE NetworkTeamType): CSprite(nID,cTeamID,nModelID,pAction,pWorld,NULL,NetworkTeamType)

{
    mpModel = NULL;
    mpWorldMesh = NULL;
    mRotationAngle = 0;
//    HLogN("CEMainBlzSprite ID = %d",nID);
}


CEMainBlzSprite::~CEMainBlzSprite()
{
    if(mpWorldMesh)
    {
        int nSize = mpModel->GetMeshSize();
        for(int i = 0; i < nSize; i++)
        {
            delete[] mpWorldMesh[i].pVetexBuffer;
        }
        delete[] mpWorldMesh;
    }
    
}


int CEMainBlzSprite::LoadResoure(CModelMan* pModelMan)
{
    
    CHWorld* pWorld = (CHWorld*)mpWorld;
    mpWorld->GetModelMan()->getModelfAscii("EMainCenter.txt", &mpModel);
    
    if(mpModel == NULL)
    {
        HLogE("[Error] CEMainBlzSprite can't load\n");
        return -1;
    }
    
    mpAICore = new CAIBlzCore(this,pWorld->mnArrEnSpriteCnt,pWorld->mnArrEnSpriteTable);
    
    int nSize = mpModel->GetMeshSize();
    sglMesh *arrMesh = mpModel->GetMesh();
    mpWorldMesh = new sglMesh[nSize];
    
    
    for(int i = 0; i < nSize; i++)
    {
        mpWorldMesh[i].pVetexBuffer = new float[arrMesh[i].nVetexCnt * 3];
    }
    return E_SUCCESS;
}

int CEMainBlzSprite::Initialize(SPoint *pPosition,SVector *pvDirection)
{
    
    float matrix[16];
    int nIndex = 0;
    int nSize = mpModel->GetMeshSize();
    sglMesh *arrMesh = mpModel->GetMesh();
    sglMesh *arrWroldMesh = mpWorldMesh;
    
    sglLoadIdentityf(matrix);
    sglScaleMatrixf(matrix, 0.18f, 0.18f, 0.18f);
    
    
    float fMaxX = -9999.0f,fMinX = 9999.0f,fMaxY = -9999.0f,fMinY = 9999.0f,fMaxZ = -9999.0f,fMinZ = 9999.0f;
    float fRadianX,fRadianY,fRadianZ;
    
    for(int j = 0; j < nSize; j++)
    {
        sglMesh* pMesh = &arrMesh[j];
        sglMesh* pWorldMesh = &arrWroldMesh[j];
        
        for(int i = 0; i < pMesh->nVetexCnt; i++)
        {
            nIndex = i*3;
            
            
            pWorldMesh->pVetexBuffer[nIndex] = pMesh->pVetexBuffer[nIndex];
            pWorldMesh->pVetexBuffer[nIndex + 1] = pMesh->pVetexBuffer[nIndex + 1];
            pWorldMesh->pVetexBuffer[nIndex + 2] = pMesh->pVetexBuffer[nIndex + 2];
            
            sglMultMatrixVectorf(&pWorldMesh->pVetexBuffer[nIndex],
                                 matrix,
                                 &pWorldMesh->pVetexBuffer[nIndex]);
            
            pWorldMesh->pVetexBuffer[nIndex + 1] += 1; //공중에 떠있는 느낌...
            
            if(pWorldMesh->pVetexBuffer[nIndex] > fMaxX)
                fMaxX = pWorldMesh->pVetexBuffer[nIndex];
            if(pWorldMesh->pVetexBuffer[nIndex] < fMinX)
                fMinX = pWorldMesh->pVetexBuffer[nIndex];
            
            
            if(pWorldMesh->pVetexBuffer[nIndex+1] > fMaxY)
                fMaxY = pWorldMesh->pVetexBuffer[nIndex+1];
            if(pWorldMesh->pVetexBuffer[nIndex+1] < fMinY)
                fMinY = pWorldMesh->pVetexBuffer[nIndex+1];
            
            
            if(pWorldMesh->pVetexBuffer[nIndex+2] > fMaxZ)
                fMaxZ = pWorldMesh->pVetexBuffer[nIndex+2];
            if(pWorldMesh->pVetexBuffer[nIndex+2] < fMinZ)
                fMinZ = pWorldMesh->pVetexBuffer[nIndex+2];
        }
        
        pWorldMesh->nVetexCnt = pMesh->nVetexCnt;
    }
    
    fRadianX = (fMaxX - fMinX) / 2.0f;
    fRadianY = (fMaxY - fMinY) / 2.0f;
    fRadianZ = (fMaxZ - fMinZ) / 2.0f;
    
    mfRadian = fRadianX;
    
    //높이는 계산에서 제외한이유는 높이가 너무 높아서 이것을 충돌 라디안으로
    //정하면 너무 넓은 범위에 속한다.
    if(fRadianY > mfRadian)
        mfRadian = fRadianY;
    
    if(fRadianZ > mfRadian)
        mfRadian = fRadianZ;
    
    
    //각판의 파워를 더미로 조금씩 조절해준다.
    float fDummy[10] = {
        0,
        -300.f,             //해안
        0,                  //사막
        MainTowerDefend    ,//설원
        MainTowerDefend * 3,//산악
        MainTowerDefend * 4,//늪지
        MainTowerDefend * 5,//폐허
        MainTowerDefend * 6,//소도시
        MainTowerDefend * 7,//미로
        MainTowerDefend * 9};//달
    
        
    float fLevel = ((CWorld*)mpWorld)->GetMapLevel();
    float fMapRank = mpWorld->GetUserInfo() ? mpWorld->GetUserInfo()->GetCompletedRank() : 1.f;
    
    if(fLevel == 0.0f)
        mfMaxDefendPowerActor = MainTowerDefend + fDummy[(int)fLevel];
    else
    {
        if(fMapRank != 1.f)
            mfMaxDefendPowerActor =(MainTowerDefend * fLevel + fDummy[(int)fLevel]) + (MainTowerDefend * 10.f + fDummy[9]) * (fMapRank  - 1); //10개의 레벨이 존재 하기 때문에 그다음은
        else
            mfMaxDefendPowerActor =  MainTowerDefend * fLevel + fDummy[(int)fLevel]; //1은 약하게 2부터는 조금 강하게.
    }
    
    return CSprite::Initialize(pPosition, pvDirection);
}

int CEMainBlzSprite::RenderBegin(int nTime)
{
    
    SPoint ptNow;
    float matrix[16];
    float matrix2[16];
    
    int nIndex = 0;
    int nSize = mpModel->GetMeshSize();
    sglMesh *arrMesh = mpModel->GetMesh();
    sglMesh *arrWroldMesh = mpWorldMesh;
    
    mRotationAngle += nTime / 20.0f;
    
    if (mRotationAngle > 360)
    {
        mRotationAngle = 0;
    }
    
    sglLoadIdentityf(matrix);
    
    GetPosition(&ptNow);
    sglTranslateMatrixf(matrix,ptNow.x,ptNow.y + 1.f,ptNow.z); //회전전에 이동을 해야 한다.
    sglRotateRzRyRxMatrixf(matrix,
                           0,
                           mRotationAngle,
                           0);
    sglScaleMatrixf(matrix, 0.18f, 0.18f, 0.18f);
    
    //알들은 반대 방향으로 회전한다.
    sglLoadIdentityf(matrix2);
    sglTranslateMatrixf(matrix2,ptNow.x,ptNow.y + 1.f,ptNow.z); //회전전에 이동을 해야 한다.
    sglRotateRzRyRxMatrixf(matrix2,
                           0,
                           -mRotationAngle,
                           0);
    sglScaleMatrixf(matrix2, 0.18f, 0.18f, 0.18f);
    
    
    for(int j = 0; j < nSize; j++)
    {
        sglMesh* pMesh = &arrMesh[j];
        sglMesh* pWorldMesh = &arrWroldMesh[j];
        
        for(int i = 0; i < pMesh->nVetexCnt; i++)
        {
            nIndex = i*3;
            if(j == 0)
            {
                //좌표를 이동한다.
                sglMultMatrixVectorf(&pWorldMesh->pVetexBuffer[nIndex],
                                     matrix,
                                     &pMesh->pVetexBuffer[nIndex]);
            }
            else
            {
                //좌표를 이동한다.
                sglMultMatrixVectorf(&pWorldMesh->pVetexBuffer[nIndex],
                                     matrix2,
                                     &pMesh->pVetexBuffer[nIndex]);
                
            }
        }
    }
    
    mpStatusBar->RenderBegin(nTime);
    return E_SUCCESS;
}

void CEMainBlzSprite::RenderBeginCore(int nTime)
{
    CSprite::RenderBeginCore(nTime);
    if(mState == BOMB_COMPACT || mState == BOMB_COMPACT_ANI)
    {
        SetState(SPRITE_READYDELETE);
    }
}

void CEMainBlzSprite::RenderBeginCore_Invisiable(int nTime)
{
    CSprite::RenderBeginCore_Invisiable(nTime);
    if(mState == BOMB_COMPACT || mState == BOMB_COMPACT_ANI)
    {
        SetState(SPRITE_READYDELETE);
    }
}


int CEMainBlzSprite::Render()
{
    int nSize = mpModel->GetMeshSize();
    sglMesh *arrMesh = mpModel->GetMesh();
    sglMesh *arrWroldMesh = mpWorldMesh;
    
    
    glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnable(GL_TEXTURE_2D);
    glColor4f(1.0f,1.0f,1.0f,1.0f);
    for(int j = 0; j < nSize; j++)
    {
        sglMesh* pMesh = &arrMesh[j];
        //for(int i = 0; i < pMesh->nVetexCnt; i++)
        //{
        glBindTexture(GL_TEXTURE_2D, pMesh->nglTexutre);
        glVertexPointer(3, GL_FLOAT, 0, arrWroldMesh[j].pVetexBuffer);
        glTexCoordPointer(2, GL_FLOAT, 0, pMesh->pCoordBuffer);
        glDrawElements(GL_TRIANGLES, pMesh->shIndicesCnt * 3, GL_UNSIGNED_SHORT, pMesh->pIndicesBuffer);
        //}
    }
    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    return E_SUCCESS;
}

int CEMainBlzSprite::RenderEnd()
{
    return E_SUCCESS;
}

int CEMainBlzSprite::OnEvent(SGLEvent *pEvent)
{
    return CSprite::OnEvent(pEvent);
}

bool CEMainBlzSprite::CollidesBounding(CSprite* Sprite,bool bExact)
{
    
    if(GetState() != SPRITE_RUN) return false;
    
    SPoint pt1,pt2;
    GetPosition(&pt1);
    Sprite->GetPosition(&pt2);
    
    float fRadius = Sprite->GetRadius();
    float fRadiusSum = fRadius + mfRadian;
    
    //(a[0]-b[0])*(a[0]-b[0]) + (a[1]-b[1])*(a[1]-b[1]) + (a[2]-b[2])*(a[2]-b[2]);
    float fx = pt1.x - pt2.x;
    float fy = pt1.y - pt2.y;
    float fz = pt1.z - pt2.z;
    
    float fd = (fx * fx + fy * fy + fz * fz);
    mfColidedDistance = fd;
    Sprite->SetColidedDistance(mfColidedDistance);
    if( fd < (fRadiusSum * fRadiusSum))
    {
        return false; //충돌이 되지 않게 막자.
    }

    return false;
}


bool CEMainBlzSprite::IsTabIn(CPicking* pPick,SPoint& nearPoint,SPoint& farPoint,int nPosX,int nPosY)
{
    bool b1;
    int nVIndex1,nVIndex2,nVIndex3;
    //    float fOutPoint[9];
    sglMesh *arrMesh = mpModel->GetMesh();
    
    int nSize = mpModel->GetMeshSize();
    for (int i = 0; i < nSize; i++)
    {
        sglMesh *pWorldMesh = &mpWorldMesh[i];
        sglMesh *pMesh = &arrMesh[i];
        
        int nCntz = pMesh->shIndicesCnt * 3;
        for (int z = 0; z < nCntz; )
        {
            nVIndex1 = pMesh->pIndicesBuffer[z++]*3;
            //            fOutPoint[0] = pWorldMesh->pVetexBuffer[nVIndex];
            //            fOutPoint[1] = pWorldMesh->pVetexBuffer[nVIndex + 1];
            //            fOutPoint[2] = pWorldMesh->pVetexBuffer[nVIndex + 2];
            
            nVIndex2 = pMesh->pIndicesBuffer[z++]*3;
            //            fOutPoint[3] = pWorldMesh->pVetexBuffer[nVIndex];
            //            fOutPoint[4] = pWorldMesh->pVetexBuffer[nVIndex + 1];
            //            fOutPoint[5] = pWorldMesh->pVetexBuffer[nVIndex + 2];
            
            nVIndex3 = pMesh->pIndicesBuffer[z++]*3;
            //            fOutPoint[6] = pWorldMesh->pVetexBuffer[nVIndex];
            //            fOutPoint[7] = pWorldMesh->pVetexBuffer[nVIndex + 1];
            //            fOutPoint[8] = pWorldMesh->pVetexBuffer[nVIndex + 2];
            
            b1 = pPick->sglIsPicking(&pWorldMesh->pVetexBuffer[nVIndex1],
                                     &pWorldMesh->pVetexBuffer[nVIndex2],
                                     &pWorldMesh->pVetexBuffer[nVIndex3],
                                     nearPoint,farPoint);
            if(b1)
            {
                nearPoint.x = pWorldMesh->pVetexBuffer[nVIndex1];
                nearPoint.y = pWorldMesh->pVetexBuffer[nVIndex1+1];
                nearPoint.z = pWorldMesh->pVetexBuffer[nVIndex1+2];
                
                farPoint.x = pWorldMesh->pVetexBuffer[nVIndex2];
                farPoint.y = pWorldMesh->pVetexBuffer[nVIndex2+1];
                farPoint.z = pWorldMesh->pVetexBuffer[nVIndex2+2];
                
                return true;
            }
#if HALF_PICK
            z+=3;
#endif //HALF_PICK
        }
        
    }
    return false;
}

//방어력 50.f 발을 맞아야 한다.
float     CEMainBlzSprite::GetMaxDefendPower()
{
    return mfMaxDefendPowerActor;
}