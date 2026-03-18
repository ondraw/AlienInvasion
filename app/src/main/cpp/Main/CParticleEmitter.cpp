//
//  CParticleEmitter.cpp
//  SongGL
//
//  Created by 송 호학 on 12. 4. 20..
//  Copyright (c) 2012년 __MyCompanyName__. All rights reserved.
//

#include "CParticleEmitter.h"
#include <stdlib.h>
#include <math.h>

float CParticleEmitter::gfRandomTable[20];
int CParticleEmitter::gnRnadomIndex = 0;

CParticleEmitter::CParticleEmitter()
{
    mnArrCount = 0;
    mfArrVertex = NULL;
    mfArrColor  = NULL;
    mfArrAliveTime = NULL;
//    mfArrDirection = NULL;
    mfArrRotateAngle = NULL;
    mbInit = false;
    
    mnTotalTime = 0;
    mnTotalLiveTime = 0;
}

void CParticleEmitter::MakeRandomTable()
{
    #define ParticleRandom() (((float) (lrand48() % 100) / 50.0) - 1.0)
    for (int i = 0; i < MAX_RANDOM_TABLE; i++)
    {
        gfRandomTable[i] = ParticleRandom();
    }
    gnRnadomIndex = (int)(long)lrand48 % MAX_RANDOM_TABLE; //시작을 랜덤하게 하자.
    if(gnRnadomIndex < 0) gnRnadomIndex*=-1;
}

//파티클 랜덤함수 0.0 ~ 1.0의 랜던값... 
float CParticleEmitter::ParticleRanom()
{
    if(gnRnadomIndex >= MAX_RANDOM_TABLE)
        gnRnadomIndex = 0;
    return gfRandomTable[gnRnadomIndex++];
}

CParticleEmitter::~CParticleEmitter()
{
    if(mfArrVertex)
    {
        delete[] mfArrVertex;
        mfArrVertex = NULL;
    }
    
    if(mfArrColor)
    {
        delete [] mfArrColor;
        mfArrColor = NULL;
    }
    
    if(mfArrAliveTime)
    {
        delete [] mfArrAliveTime;
        mfArrAliveTime = NULL;
    }
    
//    if(mfArrDirection)
//    {
//        delete[] mfArrDirection;
//        mfArrDirection= NULL;
//    }
    
    if(mfArrRotateAngle)
    {
        delete[] mfArrRotateAngle;
        mfArrRotateAngle = NULL;
    }
}

int CParticleEmitter::Initialize(SPoint *pPosition,SVector *pvDirection)
{
    return E_SUCCESS;
}

int CParticleEmitter::Initialize(
                                 CParticleEmitter_Type Type,
                                 SPoint* pPos,
                                 SVector* pDir,
                                 int nTotalLiveTime,
                                 int nCount,
                                 float fSize,
                                 int nParticleLifespan,
                                 int nParticleLifespanVariance,
                                 float fColor[4],
                                 float fColorVariance[4],
                                 float fEndColor[4],
                                 float fVecility,
                                 float fVecilityVariance,
                                 float fPitchAndAzimuth,
                                 float fStartPosRadias)
{
    memcpy(&mptStartPos,pPos,sizeof(SPoint));
    memcpy(&mvDirection,pDir,sizeof(SVector));
    mnTotalLiveTime = nTotalLiveTime;
    mType = Type;
    mnParticleLifespan = nParticleLifespan;
    mnParticleLifespanVariance = nParticleLifespanVariance;
    memcpy(mfColor, fColor, sizeof(float)*4);
    memcpy(mfColorVariance, fColorVariance, sizeof(float)*4);
    mfVecility = fVecility;
    mfVecilityVariance = fVecilityVariance;
    mfPitchAndAzimuth = fPitchAndAzimuth;
    mfSize = fSize;
    mnArrCount = nCount;
    
    mfArrVertex = new GLfloat[3 * nCount];
    mfArrColor  = new GLfloat[4 * nCount];
    mfArrAliveTime = new GLint[nCount];
//    mfArrDirection = new GLfloat[3 * nCount];
    mfArrRotateAngle = new GLfloat[3 * nCount];

    mfStartPosRadias = fStartPosRadias;
    return E_SUCCESS;
}

void CParticleEmitter::MakePartice(int nIndex)
{

    int n3i = nIndex * 3;
    
    //Vertex 
    //2.0 = 방향으로 리즈너블하게 시작 위치의 범위를 넓혀준다.
    *(mfArrVertex + n3i) = mptStartPos.x + mfStartPosRadias * ParticleRanom();
    *(mfArrVertex + n3i + 1) = mptStartPos.y + mfStartPosRadias * ParticleRanom();
    *(mfArrVertex + n3i + 2) = mptStartPos.z + mfStartPosRadias * ParticleRanom();
    
   
    int n4i = nIndex * 4;
    
    //Color
    *(mfArrColor + n4i) = mfColor[0] + mfColorVariance[0] * ParticleRanom();
    *(mfArrColor + n4i + 1) = mfColor[1] + mfColorVariance[1] * ParticleRanom();
    *(mfArrColor + n4i + 2) = mfColor[2] + mfColorVariance[2] * ParticleRanom();
    *(mfArrColor + n4i + 3) = mfColor[3] + mfColorVariance[3] * ParticleRanom();
    
    

    //현재 시간 + 살아 있는 시간
    *(mfArrAliveTime + nIndex) = mnTotalTime + mnParticleLifespan + mnParticleLifespanVariance * ParticleRanom();
    
//    *(mfArrDirection + n3i) = mvDirection.x;
//    *(mfArrDirection + n3i + 1) = mvDirection.y;
//    *(mfArrDirection + n3i + 2) = mvDirection.z;
    
    //*(mfArrRotateAngle + n3i) = 
}


void RotateToDirection (GLfloat pitch, GLfloat azimuth, SVector *vector)
{
    #define DEGREES_TO_RADIANS(__ANGLE__) ((__ANGLE__) / 180.0 * PI)
	vector->x = -sinf(DEGREES_TO_RADIANS(azimuth)) * cosf(DEGREES_TO_RADIANS(pitch));
	vector->y = sin(DEGREES_TO_RADIANS(pitch));
	vector->z = cos(DEGREES_TO_RADIANS(pitch)) * cos(DEGREES_TO_RADIANS(azimuth));	
}


void CParticleEmitter::NextPartice(int nIndex,int nTime)
{
    
    int n3i = nIndex * 3;
    
    if(mnTotalTime >= *(mfArrAliveTime + nIndex)) //수명이 다하면 지운다.
    {
        *(mfArrVertex + n3i) = -9999.0f;
        return ;
    }
    
    if(mfPitchAndAzimuth != 0.0f)
    {
        GLfloat azimuth = mfPitchAndAzimuth* ParticleRanom();
        GLfloat pitch = (mfPitchAndAzimuth * ParticleRanom());
        RotateToDirection(pitch, azimuth, &mvDirection);
    }
    
    
    //Vertex 
    //2.0 = 방향으로 리즈너블하게 시작 위치의 범위를 넓혀준다.
    float fMoveS = mfVecility + mfVecilityVariance * ParticleRanom() * (nTime / 1000.0f);
    *(mfArrVertex + n3i) = *(mfArrVertex + n3i) + mvDirection.x * fMoveS;
    *(mfArrVertex + n3i + 1) = *(mfArrVertex + n3i + 1) + mvDirection.y * fMoveS;
    *(mfArrVertex + n3i + 2) = *(mfArrVertex + n3i + 2) - mvDirection.z * fMoveS;
}

int CParticleEmitter::RenderBegin(int nTime)
{
    int n3i = 0;
    
    if (nTime == 0) return E_SUCCESS;
        
    if(mbInit == false)
    {
        int nDiv = mnArrCount - (mnArrCount / 2);
        for (int i = 0; i < mnArrCount; i++) 
        {
            n3i = i * 3;
            if(i < nDiv)
                MakePartice(i);
            else 
                *(mfArrVertex + n3i) = -9999.0f; //먼거리의 정점은 죽은것으로 생각함.
        }
        mbInit = true;
    }
    else 
    {
        for (int i = 0; i < mnArrCount; i++) 
        {
            n3i = i * 3;
            if(*(mfArrVertex + n3i) == -9999.0f) //먼거리의 정점은 죽은것으로 생각함.
                MakePartice(i);
            else 
                NextPartice(i,nTime);
        }
    }
    
    mnTotalTime += nTime;
    return E_SUCCESS;
}

int CParticleEmitter::Render()
{

    glPointSize(mfSize);
    
    glColorPointer(4, GL_FLOAT, 0, mfArrColor);
    
    glVertexPointer(3, GL_FLOAT, 0, mfArrVertex);
    
    glDrawArrays(GL_POINTS, 0, mnArrCount);

    return E_SUCCESS;
}

int CParticleEmitter::RenderEnd()
{
    return E_SUCCESS;
}

int CParticleEmitter::OnEvent(SGLEvent *pEvent)
{
    return E_SUCCESS;
}
