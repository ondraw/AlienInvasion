//
//  CFireParticle.cpp
//  SongGL
//
//  Created by Songs on 11. 4. 13..
//  Copyright 2011 thinkm. All rights reserved.
//
#include <math.h>
#include <stdlib.h>
#include "IHWorld.h"
#include "CFireParticle.h"
#include "sGLUtils.h"
#include "sGLTrasform.h"

#define  FIRESIZE  7.0f

GLfloat CFireParticle::m_gFireParticleVertex[]= 
{ 
    -FIRESIZE, FIRESIZE, 0.0f,       //left,top
    -FIRESIZE, -FIRESIZE,0.0f,      //left,bottom
    FIRESIZE,  -FIRESIZE,0.0f,      //right,bottom
    FIRESIZE,  FIRESIZE,0.0f             //right,top
};

unsigned short CFireParticle::m_gFireParticleIndices[] = {
    0, 1, 3, 2
};

GLfloat CFireParticle::m_gFireParticleCoordTex[8] = { 
    0.0f, 1.0f,
    0.0f, 0.0f,		
    1.0f, 0.0f,
  	1.0f, 1.0f
};

unsigned short CFireParticle::g_shFireIndicesCnt = 0;
GLuint CFireParticle::g_FireTextureID = 0;

//GLfloat CFireParticle::g_FireTextureID = 0;



CFireParticle::CFireParticle(IHWorld *pWorld)
{
    mpWorld = pWorld;
}


CFireParticle::~CFireParticle()
{
}

int CFireParticle::FireParticleInitialize(const char *pFilePath)
{
    int nWidth;
    int nHeight;
    int nDepth;
    unsigned char* pImage = 0;
    
    if(g_shFireIndicesCnt == 0)
        g_shFireIndicesCnt = sizeof(m_gFireParticleIndices) /sizeof(unsigned short);
    
    if(g_FireTextureID == 0)
    {
        //sprintf(sFile,"%s/%s",Loader::getSearchPath(),"Fire.tga");
        //g_FireTextureID = sglLoadTgaToBind(sFile);
        
        g_FireTextureID = sglLoadTgaToBind(pFilePath,GL_CLAMP_TO_EDGE,&nWidth,&nHeight,&nDepth,&pImage);
        if(g_FireTextureID == 0) return -1;
        if(pImage) free(pImage);
    }
    return E_SUCCESS;
}

void CFireParticle::FireParticleUninitialize()
{
    if(g_FireTextureID)
    {
        glDeleteTextures(1, &g_FireTextureID);
        g_FireTextureID = 0;
    }
}

int CFireParticle::Initialize(SPoint *pPosition,SVector *pvDirection)
{

    //char sFile[512];
    memcpy(&mPosition,pPosition,sizeof(SPoint));
    mAlpha = 1.0f;
    mState = SPRITE_RUN;
    mnNowTime = 0;
    return E_SUCCESS;
}


void CFireParticle::RenderBeginCore(int nTime)
{
    int nMaxAliveTime = 800;
    int nNextTime = 0;
    
    
    SGLCAMERA* camera = mpWorld->GetCamera();
    
    //카메라의 방향으로 설정을 한다.
    float fAngle = atan2(-camera->viewDir.z,camera->viewDir.x) * 180.0 / PI;
    
    sglLoadIdentityf(mWorldMatrix);
    
    if(mnNowTime == 0)
    {
        //이동을 먼저한후에
        sglTranslateMatrixf(mWorldMatrix,mPosition.x,mPosition.y,mPosition.z);
    }
    else
    {
        //카메라 방향으로 조금씩 움직인다. (연기가 바람에 흩허지는 효과)
        mPosition.x -= camera->viewDir.x * (GLfloat)mnNowTime / (GLfloat)1000;
        mPosition.z += camera->viewDir.z * (GLfloat)mnNowTime / (GLfloat)1000;
        //이동을 먼저한후에
        sglTranslateMatrixf(mWorldMatrix,mPosition.x,mPosition.y,mPosition.z);
    }
    
    //-90 : 면이 Y축 방향을 보고 있다. X축으로 보고 있다면... -90을 해준다.
    sglRotateRzRyRxMatrixf(mWorldMatrix,
                                     0,
                                     fAngle - 90.f,
                                     0);
    
    
    nNextTime = mnNowTime + nTime;
    
    if(nNextTime > nMaxAliveTime)
    {
        mState = SPRITE_READYDELETE; //다음 프레임에 제거 될 예정이다.
    }
    else if(mnNowTime != 0)
    {
        //1 : nMaxAliveTime = mAlpha : mnNowTime;
        mAlpha = 1.0f - (GLfloat)mnNowTime / (GLfloat)nMaxAliveTime;
    }
    
    mnNowTime = nNextTime;
}

int CFireParticle::RenderBegin(int nTime)
{
 
    for(int i = 0; i < g_shFireIndicesCnt; i++)
    {
        sglMultMatrixVectorf(mWorldVertex + i * 3, mWorldMatrix,m_gFireParticleVertex + i * 3);
    }
    return E_SUCCESS;
}



int CFireParticle::Render()
{
    if(mState == SPRITE_RUN)
    {
        //투명도로 서서히 없어지게 한다.
        glColor4f(1.0f,1.0f,1.0f,mAlpha);
        
        glBindTexture(GL_TEXTURE_2D, g_FireTextureID);
        
        glVertexPointer(3, GL_FLOAT, 0, mWorldVertex);
        
        glTexCoordPointer(2, GL_FLOAT, 0, m_gFireParticleCoordTex);
        
        glDrawElements(GL_TRIANGLE_STRIP, g_shFireIndicesCnt, GL_UNSIGNED_SHORT, m_gFireParticleIndices);
    
    }
    return E_SUCCESS;
}


int CFireParticle::RenderEnd()
{
    return E_SUCCESS;
}



