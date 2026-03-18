//
//  Header.h
//  SongGL
//
//  Created by 송 호학 on 12. 4. 20..
//  Copyright (c) 2012년 __MyCompanyName__. All rights reserved.
//

#ifndef SongGL_Header_h
#define SongGL_Header_h
#include "sGLDefine.h"
#include "ARender.h"

#define MAX_RANDOM_TABLE 20


enum CParticleEmitter_Type { CParticleEmitter_Point};
class CParticleEmitter : public ARender
{
public:
    CParticleEmitter();
    virtual ~CParticleEmitter();
    
    //랜덤 테이블을 만든다.
    static void MakeRandomTable();
    static float ParticleRanom();
    
	virtual int Initialize(SPoint *pPosition,SVector *pvDirection);
	virtual int RenderBegin(int nTime);
	virtual int Render();
	virtual int RenderEnd();	
    virtual int OnEvent(SGLEvent *pEvent);
    
    
    //Type : CParticleEmitter_Point 만지원
    //pPos  : 시작지점
    //pDir  : 방향
    //nCount : 파티클갯수
    //fSize  : 파티클 크기
    //nParticleLifespan : 파티클이 살아 있는 시간
    //nParticleLifespanVariance : 변화량
    //fColor : 파티클의 색
    //fColorVariance : 컬러 변화량
    //fEndColr : 파티클의 변화의 종료 색
    //fVecirity : 속도
    //fRotation : 파티클의 회전
    //fStartPosRadias : 파티클이 생성될때 시작점의 반경
    int Initialize(CParticleEmitter_Type Type,
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
                   float fStartPosRadias);
    
    void MakePartice(int nIndex);
    void NextPartice(int nIndex,int nTime);
    
public:
    unsigned int mnTotalTime;       //총 경과 시간.
    unsigned int mnTotalLiveTime;   //살아 있는 시간.
    
protected:
    CParticleEmitter_Type mType;
    SPoint mptStartPos;
    SVector mvDirection;
    
    int     mnParticleLifespan;
    int     mnParticleLifespanVariance;
    float   mfColor[4];
    float   mfColorVariance[4]; //시작시 컬러의 변화량에 대한 값 설정.
    float   mfEndColor[4];
    float   mfVecility;
    float   mfVecilityVariance;

    float   mfPitchAndAzimuth;
    float   mfSize;
    
    int      mnArrCount;        //파티클의 갯수
    float*   mfArrVertex;       //정점들 * 3
    float*   mfArrColor;        //칼라들 * 4
    int*     mfArrAliveTime;    //각파티클이 살아 있는 시간
//    float*   mfArrDirection;    //각파티클의 방량 * 3
    float*   mfArrRotateAngle;  //각파이클의 회전 * 3
    float    mfStartPosRadias;  //시작지점의 범위 반경
    bool     mbInit;        //초기화가 안되었을 경우 초기화를 한다.
    
  
    static float gfRandomTable[MAX_RANDOM_TABLE];
    static int gnRnadomIndex;
};


#endif
