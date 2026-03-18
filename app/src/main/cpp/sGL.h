/*
 *  sGL.h
 *  SongGL
 *
 *  Created by 호학 송 on 10. 11. 9..
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#include "sGLDefine.h"

#ifndef _SGL_H
#define _SGL_H




//-----------------------------------------------------------------------
// fInitialize SGL을 초기화 한후에 불러오는 OS단의 함수를 실행하기위한 함수 포인터
// 실행단의 클리스, 생성된 pSContext를 가지고 있다.
typedef int (*fInitialize)(void* pThis,SGLContext* pSContext); 
typedef int (*fProjection)(void* pThis,SGLContext* pSContext);

typedef int (*fRenderBegin)(void* pThis,SGLContext* pSContext);
typedef int (*fRendering)(void* pThis,SGLContext* pSContext);
typedef int (*fRenderEnd)(void* pThis,SGLContext* pSContext);
//-----------------------------------------------------------------------

extern int gDisplayWidth;
extern int gDisplayHeight;

typedef void (*fShowSNSDlg)();
extern fShowSNSDlg gShowSNSDlg;
#if defined(APPSTOREKIT) || defined(ANDROIDAPPSTOREKIT)
typedef bool (*fShowShopDlg)(const char* sProductID,void* pWorld,int nType);
extern fShowShopDlg gShowShopDlg;
#endif //APPSTOREKIT or ANDROIDAPPSTOREKIT

typedef bool (*fShowBestScoreDlg)(void* pUserInfo);
extern fShowBestScoreDlg gShowBestScoreDlg;

typedef bool (*fShowReviewDlg)(void* pUserInfo);
extern fShowReviewDlg gShowReviewDlg;

typedef void (*fSendUserInfo)(void* pUserInfo);
extern fSendUserInfo gSendUserInfo;

typedef void (*fSendMultiplayData)(void* pObject,int nSize, char* sData,bool bReliable);
extern fSendMultiplayData gSendMultiplayData;

typedef void (*fShowNewVersionDlg)(const char* sURL);
extern fShowNewVersionDlg gShowNewVersionDlg;

typedef void(*fInitFindPlayer)();
extern fInitFindPlayer gInitFindPlayer;
typedef void (*fFindPlyer)(int nResetFindTime,unsigned int nMatchFilter,unsigned int nMatchGroup);
extern fFindPlyer gFindPlyer;
typedef void (*fPlayOut)();
extern fPlayOut   gPlayOut;
typedef void(*fHWorldEnd)();
extern fHWorldEnd gHWorldEnd;
typedef void(*fLoginGC)();
extern fLoginGC gLoginGC;

typedef bool(*fIsHacking)();
extern fIsHacking gIsHacking;


#ifdef __cplusplus
extern "C" {
#endif
	// 로그를 쌓을때 사용한다.
	// 메세지의 크기는 BUFSIZ=1024이다.
	void  sglLog ( const char *formatStr, ... );
    void  sglLogNet ( const char *formatStr, ... );
    void  sglSetRootPath(const char* sRootPath);
	
	//sglInitialize 를 초기화 한다.
	//pFunction : OS단에서 실행해야 할 함수가 존재하면 함수 포인터를 넣어준다.
	SGLContext* sglInitialize(fInitialize pFunction,void* pThis);
	
	//카메라를 리셋한다.
	void sglResetCamera(SGLContext *sContext);
	
	// 윈도우 사이즈가 변경되었을 경우 컨텍스트와 투영,카메라를 초기화한다.
	int sglResize(SGLContext* sContext,int nWidth,int nHeight,int nAperture);
	
	
	// 투영을 설정한다.
	int sglProjection(SGLContext* sContext,fProjection pFunction,void* pThis);
	
	
	void sglRelease(SGLContext* sContext);
	
#if defined ANDROID || defined IPHONE
	int sglRender(SGLContext* sContext,
				  fRenderBegin pRenderBegin,
				  fRendering   pRendering,
				  fRenderEnd   pRenderEnd,
				  void* pThis);
#else
	int sglRender(SGLContext* sContext,
				  int nViewWidth, 
				  int nViewHeight,
				  fProjection pProject,
				  fRenderBegin pRenderBegin,
				  fRendering   pRendering,
				  fRenderEnd   pRenderEnd,
				  void* pThis);
#endif 
	
	
	int sglRenderBegin(SGLContext* sContext,fRenderBegin pRenderBegin,void* pThis);
	int sglRendering(SGLContext* sContext,fRendering pRendering,void* pThis);
	int sglRenderEnd(SGLContext* sContext,fRenderEnd pRenderEnd,void* pThis);
	
	
    void sglSerialize(SGLContext* sContext,bool bWrite);
    
    void sglShowAD(bool bShow);
    
    bool sglIsFastModel();
    bool sglNoFree();
    float sglGetDisplayScale();
    void sglSetDisplayScale(float fScale);
#ifdef __cplusplus
}
#endif


#endif //_SGL_H