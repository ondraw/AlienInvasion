#include "CBackground.h"
#include "sGLDefine.h"
#include "sGLUtils.h"
#include "CMS3DASCII.h"
#include "CTextureMan.h"
#include "CAIBlzCore.h"
#include "IHWorld.h"
#include "CWorld.h"
#include "CSGLCore.h"

//float    CBackground::mfPosY = 1500.0f;
CBackground::CBackground(unsigned int nID,unsigned char cTeamID,int nModelID,CTextureMan *pTextureMan,IAction *pAction,IHWorld* pWorld): CSprite(nID,cTeamID,nModelID,pAction,pWorld,NULL,NETWORK_NONE)

{
    mpModel = new CMS3DASCII(pTextureMan);
    mpWorldMesh = NULL;
    mfPosY = 1500.0f;
    memset(mfPosition, 0, sizeof(mfPosition));
    memset(mfRotation, 0, sizeof(mfRotation));
    
    
}


CBackground::~CBackground()
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
    
    if(mpModel)
    {
        delete mpModel;
        mpModel = 0;
    }
}

int CBackground::LoadResoure(CModelMan* pModelMan)
{
    if(mpModel->Load("Bak1.txt") < 0)
    {
        HLogE("[Error]Bak1 can't load\n");
        return -1;
    }
    
    int nCnt = mpModel->GetMeshSize();
    mpWorldMesh = new sglMesh[nCnt];
    for(int i = 0; i < nCnt; i++)
    {
        mpWorldMesh[i].pVetexBuffer = new float[mpModel->GetMesh()[i].nVetexCnt * 3];
    }
 
    //MakeTerrianInfo에서
    CWorld* pWorld = (CWorld*)this->GetWorld();
    CSGLCore* pCore = (CSGLCore*)pWorld->GetSGLCore();
    TerrianMakeInfo* pMakeInfo = pCore->GetMakeTerrinInfo();
    if(pMakeInfo && pMakeInfo->sBackgroundTexture)
    {
        SetBackgroundTexture(pMakeInfo->sBackgroundTexture,pMakeInfo->fBackgroundPosition,pMakeInfo->fBackgroundRotation);
    }
    else
        return -1;
    
    return E_SUCCESS;
}


void CBackground::SetBackgroundTexture(const char* sFleName,float* pPosition,float* pRotation)
{
    if(sFleName == NULL)     return;
    
    GLuint nID = 0;
    nID = mpModel->GetTextureMan()->GetTextureID(sFleName, 0.f,0.f,true);
    int nCnt = mpModel->GetMeshSize();
    sglMesh* arrMesh = mpModel->GetMesh();
    for(int i = 0; i < nCnt; i++)
    {
        
        sglMesh* pMesh = &arrMesh[i];
        pMesh->nglTexutre = nID;
        //이미지를 뒤집어서 읽어온다.
        strcpy(pMesh->sTextureName, sFleName);
    }
    memcpy(mfPosition, pPosition,sizeof(mfPosition));
    memcpy(mfRotation, pRotation,sizeof(mfRotation));
    
    SetArrangeVertex(mfPosition,mfRotation);
}

void  CBackground::SetArrangeVertex(float* pPosition,float* pRotation)
{
    float matrix[16];
    int nIndex = 0;
    int nSize = mpModel->GetMeshSize();
    sglMesh *arrMesh = mpModel->GetMesh();
    
    sglLoadIdentityf(matrix);
    sglTranslateMatrixf(matrix,pPosition[0],mfPosY - 80 + pPosition[1],pPosition[2]);
    sglRotateRzRyRxMatrixf(matrix, pRotation[0], pRotation[1], pRotation[2]);
    sglScaleMatrixf(matrix, 55.18f, 35.0f, 55.18f); //x,y(높이),z
    
    for(int j = 0; j < nSize; j++)
    {
        sglMesh* pMesh = &arrMesh[j];
        sglMesh* pMesh2 = &mpWorldMesh[j];
        for(int i = 0; i < pMesh->nVetexCnt; i++)
        {
            nIndex = i*3;
            
            sglMultMatrixVectorf(&pMesh2->pVetexBuffer[nIndex],
                                 matrix,
                                 &pMesh->pVetexBuffer[nIndex]);
        }
    }
}

int CBackground::ActivateCamera(SGLCAMERA* pCamera)
{
    
     SPoint ptCamera;
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    
    ptCamera.x = 0.0f;
    ptCamera.y = mfPosY + 100.0f;
    ptCamera.z = 0.0f;
    

    gluLookAt (ptCamera.x, 
               ptCamera.y, 
               ptCamera.z,
               
               ptCamera.x - pCamera->viewDir.x, //방향이 돌아가는 반대 방향으로 돌아야 하기 때문에 -로 해야한다.
               ptCamera.y - 0.2,
               ptCamera.z - pCamera->viewDir.z, 
               
               0.0f, 
               1.0f , //y축 상향.
               0.0f);
    
	return E_SUCCESS;
}

int CBackground::Initialize(SPoint *pPosition,SVector *pvDirection)
{
    
    return E_SUCCESS;
}

int CBackground::RenderBegin(int nTime)
{
    return E_SUCCESS;
}


int CBackground::Render() 
{ 
    int nSize = mpModel->GetMeshSize();
    sglMesh *arrMesh = mpModel->GetMesh();

    glEnableClientState(GL_VERTEX_ARRAY); 
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnable(GL_TEXTURE_2D);

    for(int j = 0; j < nSize; j++)
    {
        sglMesh* pMesh = &arrMesh[j];
        glBindTexture(GL_TEXTURE_2D, pMesh->nglTexutre);
        glVertexPointer(3, GL_FLOAT, 0, mpWorldMesh[j].pVetexBuffer);
        glTexCoordPointer(2, GL_FLOAT, 0, pMesh->pCoordBuffer);
        glDrawElements(GL_TRIANGLES, pMesh->shIndicesCnt * 3, GL_UNSIGNED_SHORT, pMesh->pIndicesBuffer);

    }
    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY); 

    return E_SUCCESS;
}

int CBackground::RenderEnd() 
{ 
    return E_SUCCESS;
}

int CBackground::OnEvent(SGLEvent *pEvent) 
{ 
    return E_SUCCESS;
}

#ifdef ANDROID
void CBackground::ResetTexture()
{
    mpModel->ResetTexture();
}
#endif