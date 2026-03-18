/*
 *  sGLUtils.h
 *  SongGL
 *
 *  Created by 호학 송 on 10. 11. 19..
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ListNode.h"
#include "sGLDefine.h"
#include <sys/time.h>

#ifndef _SGLUTILS_H
#define _SGLUTILS_H


class AMesh;

//using namespace std;

#ifdef __cplusplus
extern "C" {
#endif
	
    //사용자 데이터를 저장할때 거시기 하자.
    int XorCrypto(unsigned char* key, int keyLength, unsigned char* in, int nBytes);
    
    //게임총시간.
    unsigned long GetGGTime();
    //Pause할때도 계속해서 시간의 흐름을 알고 싶다면 (즉 컨트롤 흔들어줄때 사용한다.)
    unsigned long GetGGTimeAtPause();
    //현재 클럭을 가져온다.
    timeval GetClock();
    //GLfloat GetClockF();
    
    //----------------------
    //Dif Time
#if VIEWROTATION_ANI
    void    BeginClock();
    GLfloat   EndClock();
#endif
    //----------------------
    
    //이전클럭과의 차이
    GLuint GetClockDelta();
    //이전클럭과의 차이를 새로 구하지 않고 기억한것을 가져온다.
    GLuint GetClockDeltaConst();
    
    //시간을 멈춘다.
    void SetPauseTime(bool v);
    bool IsPauseTime();

#if defined ANDROID || defined IPHONE
	void gluLookAt(GLfloat eyex, GLfloat eyey, GLfloat eyez,
				   GLfloat centerx, GLfloat centery, GLfloat centerz,
				   GLfloat upx, GLfloat upy, GLfloat upz);
#endif 
	
	//int SaveCut(char* sPath,int nGroupSize);
    //-360~360의 각으로 정리한다.
	float AngleArrange(float fAngle);
    //0~360의 각으로 정리한다.
    float AngleArrangeEx(float fAngle);
    GLuint sglLoadTgaToBind(const char* pFileName,int nBindType,int *pnOutWidth,int *pnOutHeight,int *pnOutDepth,unsigned char** ppOutImage);
    int QuadratiEcquation (float A,float B,float C , float &fV1, float &fV2);
    
    //포의 포지션을 파싱한다.
    void PosParse(char* sData,int* pCnt,int** &pStartPos);

    
    //그림자를 그리려면 매트릭스를 만들어야 한다.
    void sglShadowMatrix(GLfloat shadowMat[4][4],GLfloat groundplane[4],GLfloat lightpos[4]);
    //정점으로 평면방정식을 만든다.
    void sglFindPlane(GLfloat plane[4], GLfloat v0[3], GLfloat v1[3], GLfloat v2[3]);
    
    
    float sglNowToPosAngle(SPoint* ptNow,SPoint* ptTo);
    //+,- 오른쪽, 왼쪽으로 회전한 것인지를 알아온다.
    //bOnlyFront : (false=탱크는 앞뒤로 움직일수 있다.)
    //fMoveVector = 회전이 끝난후에 앞으로 이동할지 뒤로 이동할지 나타낸다.
    float sglGetRotationVelocity(bool bOnlyFront,float fNowDirAngle,float* fToDirAngle,float* fMoveVector); 
    //회전이 지나쳤는지 확인한다.
    bool sglCheckRotation(float fNowAngle,float fToAngle,float* fRotAngle);
    
    //퐁세이딩 컬러 계산.
    float sglPhongShadeColor(SVector* pNormal,float fTerrianShadow);
    
    void sglGet2DTextureCoord(float fsu,float fsv,float feu,float fev,float fOutText[8]);
    // 
    enum W_COMACT { 
        W_COMACT_LF,    //왼쪽 앞
        W_COMACT_LFL,   //왼쪽 앞옆
        W_COMACT_LB,    //왼쪽 뒤
        W_COMACT_LBL,   //왼쪽 뒤옆
        W_COMACT_RB,    //오른쪽 뒤
        W_COMACT_RBR,   //오른쪽 뒤옆
        W_COMACT_RF,    //오른쪽 앞
        W_COMACT_RFR    //오른쪽 앞옆
    };
    //객체가 어디를 부딕쳤는지를 알아온다.
    //pAvoidDir 회피 방향 , pToAvoidAngle 회피 각도 (to로 이동하길 원하면 사용하기 바람)
    W_COMACT sglWhichCompact(SPoint *ptNow,SVector *vtDir,float fDirAngle,SPoint *ptEnNow,float* pAvoidDir,float* pToAvoidAngle);
    
#ifdef __cplusplus
}
#endif

#endif //_SGLUTILS_H
