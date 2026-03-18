//
//  CImpactFlyParticle.cpp
//  SongGL
//
//  Created by itsme on 12. 8. 22..
//
//

#include "CImpactFlyParticle.h"
#include <stdlib.h>
#include <math.h>
#include "IHWorld.h"
#include "sGLUtils.h"
#include "sGLTrasform.h"
#define  FIRESIZE  15.0f

GLfloat CImpactFlyParticle::m_gCompatFireParticleVertex[]=
{
    -FIRESIZE/3.f, FIRESIZE/1.5f,  0.0f,        //left,top
    -FIRESIZE/3.f, 0.0f, 0.0f,                  //left,bottom
    FIRESIZE/3.f,  0.0f, 0.0f,                  //right,bottom
    FIRESIZE/3.f,  FIRESIZE/1.5f,  0.0f         //right,top
};

unsigned short CImpactFlyParticle::m_gCompatFireParticleIndices[] = {
    0, 1, 3, 2
};

GLfloat CImpactFlyParticle::m_gCompatFireParticleCoordTex[8] = {
    
    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f,
  	1.0f, 1.0f
};


unsigned short CImpactFlyParticle::g_shCompatFireIndicesCnt = 0;
GLuint CImpactFlyParticle::g_CompatFireTextureID = 0;

CImpactFlyParticle::CImpactFlyParticle(IHWorld* pWorld)
{
    mpWorld = pWorld;
    
}


CImpactFlyParticle::~CImpactFlyParticle()
{
}

int CImpactFlyParticle::ImpactFlyParticleInitialize(const char *pFilePath)
{
    int nWidth;
    int nHeight;
    int nDepth;
    unsigned char* pImage = 0;
    
    
    if(g_shCompatFireIndicesCnt == 0)
        g_shCompatFireIndicesCnt = sizeof(m_gCompatFireParticleIndices) /sizeof(unsigned short);
    
    if(g_CompatFireTextureID == 0)
    {
        g_CompatFireTextureID = sglLoadTgaToBind(pFilePath,GL_CLAMP_TO_EDGE,&nWidth,&nHeight,&nDepth,&pImage);
        if(g_CompatFireTextureID == 0) return -1;
        if(pImage) free(pImage);
    }
    return E_SUCCESS;
}

void CImpactFlyParticle::ImpactFlyParticleUninitialize()
{
    if(g_CompatFireTextureID)
    {
        glDeleteTextures(1, &g_CompatFireTextureID);
        g_CompatFireTextureID = 0;
    }
}

int CImpactFlyParticle::Initialize(SPoint *pPosition,SVector *pvDirection)
{
    if(pPosition)
        memcpy(&mPosition,pPosition,sizeof(SPoint));
    
    mAlpha = 1.0f;
    mState = SPRITE_RUN;
    mnNowTime = 0;
    
    return E_SUCCESS;
}


void CImpactFlyParticle::RenderBeginCore(int nTime)
{

//    int nMaxAliveTime = 800;
    SGLCAMERA* camera = mpWorld->GetCamera();
    //카메라의 방향으로 설정을 한다. ( camera방향은 -로 설정해준다.)
    float fAngle = atan2(-camera->viewDir.z,camera->viewDir.x) * 180.0 / PI;
    sglLoadIdentityf(mWorldMatrix);
    //이동을 먼저한후에
    sglTranslateMatrixf(mWorldMatrix,mPosition.x,mPosition.y,mPosition.z);
    //회전을 해야 한다.
    //카메라의 방향으로 설정을 한다.
    //-90 : 면이 Y축 방향을 보고 있다. X축으로 보고 있다면... -90을 해준다.
    sglRotateRzRyRxMatrixf(mWorldMatrix,
                           0,
                           fAngle - 90.f,
                           0);
    
    if(mnNowTime > 1000000)
        mnNowTime = 0;
    
    int n = mnNowTime % 1000; //2초
    
    if(n < 500)
    {
        mAlpha = (float)n / 500.0f; //점점점 선명하게
    }
    else
    {
        n-= 500;
        mAlpha = 1 - (float)n/500.0f;//점점점 투명하게.
    }
    
    mnNowTime += nTime;
}

int CImpactFlyParticle::RenderBegin(int nTime)
{
    
    for(int i = 0; i < g_shCompatFireIndicesCnt; i++)
    {
        sglMultMatrixVectorf(mWorldVertex + i * 3, mWorldMatrix,m_gCompatFireParticleVertex + i * 3);
    }
    return E_SUCCESS;
}



int CImpactFlyParticle::Render()
{
    if(mState == SPRITE_RUN)
    {
        
        //투명도로 서서히 없어지게 한다. + 0.4f 투명하게 하지말자.. 타겟이 너무 안보인다.
        glColor4f(1.0f,1.0f,1.0f,mAlpha + 0.4f);
        
        glBindTexture(GL_TEXTURE_2D, g_CompatFireTextureID);
        
        glVertexPointer(3, GL_FLOAT, 0, mWorldVertex);
        
        glTexCoordPointer(2, GL_FLOAT, 0, m_gCompatFireParticleCoordTex);
        
        glDrawElements(GL_TRIANGLE_STRIP, g_shCompatFireIndicesCnt, GL_UNSIGNED_SHORT, m_gCompatFireParticleIndices);
        
    }
    return E_SUCCESS;
}


int CImpactFlyParticle::RenderEnd()
{
    return E_SUCCESS;
}