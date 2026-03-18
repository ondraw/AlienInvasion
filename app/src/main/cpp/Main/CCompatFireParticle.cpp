
//
//  CFireParticle.cpp
//  SongGL
//
//  Created by Songs on 11. 4. 13..
//  Copyright 2011 thinkm. All rights reserved.
//
#include <math.h>
#include "IHWorld.h"
#include "CCompatFireParticle.h"
#include "sGLUtils.h"
#include "sGLTrasform.h"
#include "CHWorld.h"
#include "CTextureMan.h"
#include "CMS3DModelASCII.h"
#include "CSGLCore.h"

#define  FIRESIZE  1.0f

GLfloat CCompatFireParticle::m_gCompatFireParticleVertex[]=
{ 
    -FIRESIZE, FIRESIZE,0.0f,     //left,top
    -FIRESIZE, -FIRESIZE,0.0f,     //left,bottom
    FIRESIZE,  -FIRESIZE,0.0f,     //right,bottom
    FIRESIZE,  FIRESIZE,0.0f     //right,top
};

unsigned short CCompatFireParticle::m_gCompatFireParticleIndices[] = {
    0, 1, 3, 2
};

GLfloat CCompatFireParticle::m_gCompatFireParticleCoordTex[8] = { 
    
    0.0f, 1.0f,
    0.0f, 0.0f,		
    1.0f, 0.0f,
  	1.0f, 1.0f
};


unsigned short CCompatFireParticle::g_shCompatFireIndicesCnt = 0;
//GLuint CCompatFireParticle::g_CompatFireTextureID = 0;

//GLfloat CFireParticle::g_FireTextureID = 0;



CCompatFireParticle::CCompatFireParticle(IHWorld* pWorld,float fScale,const char* sFileName)
{
    mpWorld = pWorld;
    mfScale = fScale;
    msFileName = new char[strlen(sFileName) + 1];
    strcpy(msFileName, sFileName);
//    mpModel = new CMS3DModelASCII(((CHWorld*)mpWorld)->GetSGLCore()->GetMS3DModelASCIIMan()->Load("FireFrame.txt"));
    ((CHWorld*)mpWorld)->GetSGLCore()->GetModelMan()->getModelf("FireFrame.txt", (CMyModel**)&mpModel);
}


CCompatFireParticle::~CCompatFireParticle()
{
    if(msFileName)
        delete[] msFileName;
    msFileName = 0;
    
    if(mpModel)
        delete mpModel;
}

int CCompatFireParticle::CompatFireParticleInitialize(const char *pFilePath)
{
//    int nWidth;
//    int nHeight;
//    int nDepth;
//    unsigned char* pImage = 0;

    
    if(g_shCompatFireIndicesCnt == 0)
        g_shCompatFireIndicesCnt = sizeof(m_gCompatFireParticleIndices) /sizeof(unsigned short);
    
//    if(g_CompatFireTextureID == 0)
//    {
//        g_CompatFireTextureID = sglLoadTgaToBind(pFilePath,GL_CLAMP_TO_EDGE,&nWidth,&nHeight,&nDepth,&pImage);
//        if(g_CompatFireTextureID == 0) return -1;
//        if(pImage) free(pImage);
//    }
    return E_SUCCESS;
}


void CCompatFireParticle::CompatFireParticleUninitialize()
{
}


int CCompatFireParticle::Initialize(SPoint *pPosition,SVector *pvDirection)
{
    
    //char sFile[512];
    memcpy(&mPosition,pPosition,sizeof(SPoint));
    mAlpha = 1.0f;
    mState = SPRITE_RUN;
    mnNowTime = 0;
    
    mnCompatFireTextureID = ((CHWorld*)mpWorld)->GetTextureMan()->GetTextureID(msFileName);
    
    mpModel->orientation[0] = 0;
    mpModel->position[0] = mPosition.x;
    mpModel->position[1] = mPosition.y;
    mpModel->position[2] = mPosition.z;

    mpModel->scale[0] = mfScale;
    mpModel->scale[1] = mfScale;
    mpModel->scale[2] = mfScale;
    return E_SUCCESS;
}


void CCompatFireParticle::RenderBeginCore(int nTime)
{
    int nMaxAliveTime = 800;
    int nNextTime = 0;
    float fScale = 1.0f;
    
    SGLCAMERA* camera = mpWorld->GetCamera();
    
    //카메라의 방향으로 설정을 한다.
    //-90 : 면이 Y축 방향을 보고 있다. X축으로 보고 있다면... -90을 해준다.
    float fAngle = atan2(-camera->viewDir.z,camera->viewDir.x) * 180.0f / PI - 90.f;
    
    sglLoadIdentityf(mWorldMatrix);

    //이동을 먼저한후에
    sglTranslateMatrixf(mWorldMatrix,mPosition.x,mPosition.y,mPosition.z);
    
    //회전을 해야 한다.
    sglRotateRzRyRxMatrixf(mWorldMatrix,
                                     0,
                                     fAngle,
                                     0);
	
    
    
    nNextTime = mnNowTime + nTime;
    
    if(nNextTime > nMaxAliveTime)
    {
        mState = SPRITE_READYDELETE; //다음 프레임에 제거 될 예정이다.
        return ;
    }
    else if(mnNowTime != 0)
    {
        float fR = (GLfloat)mnNowTime / (GLfloat)nMaxAliveTime;
        //1 : nMaxAliveTime = mAlpha : mnNowTime;
        mAlpha = 1.0f - fR;
        
        fScale = 3.0 + mfScale * fR; //점점 커지게 한다. 3.0 은 기본값으로 이값 보다 크게 보이게 한다.
    }
    
    sglScaleMatrixf(mWorldMatrix,  fScale, fScale, fScale);
    
    mnNowTime = nNextTime;
    mpModel->orientation[1] = fAngle;
}

int CCompatFireParticle::RenderBegin(int nTime)
{
    
    for(int i = 0; i < g_shCompatFireIndicesCnt; i++)
    {
        sglMultMatrixVectorf(mWorldVertex + i * 3, mWorldMatrix,m_gCompatFireParticleVertex + i * 3);
    }
    
    mpModel->updatef(nTime);
    return E_SUCCESS;
}



int CCompatFireParticle::Render()
{
    if(mState == SPRITE_RUN)
    {
        //투명도로 서서히 없어지게 한다.
        glColor4f(1.0f,1.0f,1.0f,mAlpha);
        
        //정면의 폭팔을 그린다.
        glBindTexture(GL_TEXTURE_2D, mnCompatFireTextureID);
        glVertexPointer(3, GL_FLOAT, 0, mWorldVertex);
        glTexCoordPointer(2, GL_FLOAT, 0, m_gCompatFireParticleCoordTex);
        glDrawElements(GL_TRIANGLE_STRIP, g_shCompatFireIndicesCnt, GL_UNSIGNED_SHORT, m_gCompatFireParticleIndices);
        
        
        //겹으로 그린다.
        int nSize = mpModel->GetMeshSize();
        sglMesh *arrMesh = mpModel->GetMesh();
        sglMesh *arrWroldMesh = mpModel->GetWorldMesh();
        glColor4f(1.0f,1.0f,1.0f,mAlpha/5.0f); //알파값을 연하게 주어야 곂치는 현상쳐러 안보인다.
        for(int j = 0; j < nSize; j++)
        {
            sglMesh* pMesh = &arrMesh[j];
            //같은이미지를 사용하면 바인드를 여러번 할필요가 없다.
            //glBindTexture(GL_TEXTURE_2D, mnCompatFireTextureID);
            glVertexPointer(3, GL_FLOAT, 0, arrWroldMesh[j].pVetexBuffer);
            glTexCoordPointer(2, GL_FLOAT, 0, pMesh->pCoordBuffer);
            glDrawElements(GL_TRIANGLES, pMesh->shIndicesCnt * 3, GL_UNSIGNED_SHORT, pMesh->pIndicesBuffer);
        }
    }
    return E_SUCCESS;
}


int CCompatFireParticle::RenderEnd()
{
    return E_SUCCESS;
}

