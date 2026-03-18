/*
 *  sGL.c
 *  SongGL
 *
 *  Created by 호학 송 on 10. 11. 9..
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#include <strings.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include "sGLUtils.h"
#include "sGL.h"
#include "IHWorld.h"
#include "sGLTrasform.h"


DISPLAYTYPE gnDisplayType = DISPLAY_IPHONE;

#if defined(APPSTOREKIT) || defined(ANDROIDAPPSTOREKIT)
extern fShowShopDlg gShowShopDlg;
#endif //APPSTOREKIT or ANDROIDAPPSTOREKIT

extern fShowBestScoreDlg gShowBestScoreDlg;

#define FIRESIZE_W 1.0f



//시계방향으로 버텍스가 정리되었다.
GLfloat gPanelVertexZ[]= 
{ 
    -FIRESIZE_W,  0.0f,   FIRESIZE_W,
    -FIRESIZE_W,  0.0f,   -FIRESIZE_W,
    FIRESIZE_W ,  0.0f,   -FIRESIZE_W,
    FIRESIZE_W ,  0.0f,   FIRESIZE_W
};


//반시계방향.
GLfloat gPanelVertexZR[]=
{ 
    FIRESIZE_W ,  0.0f,   FIRESIZE_W,
    FIRESIZE_W ,  0.0f,   -FIRESIZE_W,
    -FIRESIZE_W,  0.0f,   -FIRESIZE_W,
    -FIRESIZE_W,  0.0f,   FIRESIZE_W
};



GLfloat gPanelVertexY[]= 
{ 
    FIRESIZE_W ,  FIRESIZE_W, 0.0f,
    FIRESIZE_W ,  -FIRESIZE_W, 0.0f,
    -FIRESIZE_W,  -FIRESIZE_W, 0.0f,
    -FIRESIZE_W,  FIRESIZE_W, 0.0f
};

unsigned short gPanelIndices[] = {
    0, 1, 3, 2
};

GLfloat gPanelCoordTex[8] = {     
    0.0f, 1.0f,
    0.0f, 0.0f,		
    1.0f, 0.0f,  	
    1.0f, 1.0f
};

//똑 바로된 이미지가 된다. 하지만 글씨가 꺼꾸로 된다. 그래서 gPanelCoordTex를 사용하지만 
//GLfloat gPanelCoordTexX[8] = {
//    0.0f, 0.0f,
//    0.0f, 1.0f,
//    1.0f, 1.0f,
//    1.0f, 0.0f
//};



unsigned short gshPanelIndicesCnt = 4;

int gDisplayWidth = 0;
int gDisplayHeight = 0;
int gAperture = 0;




//--------------------------------------------------------------------------------------
//오픈지엘의 컨텍스트를 만든다.
//--------------------------------------------------------------------------------------
SGLContext* sglInitialize(fInitialize pFunction,void* pThis)
{
	
	SGLContext *pContext =  (SGLContext*)malloc(sizeof(SGLContext));
	memset(pContext,0,sizeof(SGLContext));	
    
//	sglResetCamera(pContext);	
//	pContext->pWorld = NULL;
	
	//컨텍스트를 초기화한다.
	if( pFunction != 0)
	{
		pContext->pViewContext = pThis;
		if( pFunction ( pThis,pContext) == 1) 
			return pContext;
	}
	
	//움직임 효과를 빠르게 해준다. (흔들림)을 꺼둔다.
	glDisable(GL_DITHER); 
	
	//드라이버에 원근보정을 가장 빠르게 요청한다. 하드웨어에 따라서 달라진다.
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_FASTEST);
	
	
	//컬러를 지운다. 
	glClearColor(0,0,0,0);
	
	//폴리곤을 추려난다. (어느방향으로 돌면서 앞면인지 ..)
	glEnable(GL_CULL_FACE);
	
	//시계방향으로 객체를 그린다.
	glFrontFace(GL_CCW);
	
	
	//glClearDepth(1.0f);
	
	//스무스하게 만든다.
	glShadeModel(GL_SMOOTH);
	
	//물체를 하나 그리고 그 앞쪽으로 물체를 하나 더 그리면 
	//처음에 그렸던 물체에 나중에 그린 물체가 가리는 현상이 생길 수 있는다.
	glEnable(GL_DEPTH_TEST); 
	
	return pContext;
}

void sglSetRootPath(const char* sRootPath)
{
    char sFilePath[256];//(char*)pResource;
	sprintf(sFilePath,"%s/",(const char*)sRootPath);
	CLoader::setSearchPath((const char*)sFilePath);
}

void sglResetCamera(SGLContext *sContext)
{
	
	IHWorld *pWorld = (IHWorld*)sContext->pWorld;
	if(pWorld == NULL) return;
	
	pWorld->ResetCamera();
}


////윈도우 크기에 맞게 뷰포트를 만들고,
////투영을 설정한다.
// return 1 : resized
// return 0 : 
int sglResize(SGLContext* sContext,int nWidth,int nHeight,int nAperture)
{
	IHWorld *pWorld = (IHWorld*)sContext->pWorld;
    if(pWorld == 0) return E_SUCCESS;
	SGLCAMERA *pCamera = pWorld->GetCamera();
    
	// ensure camera knows size changed
	if (!(gDisplayHeight == nHeight && gDisplayWidth == nWidth && gAperture == nAperture))
	{
        gDisplayWidth = nWidth;
        gDisplayHeight = nHeight;
        gAperture = nAperture;
        
		pCamera->viewHeight = nHeight;
		pCamera->viewWidth = nWidth;
        pCamera->aperture = nAperture;
        
		//pWorld->Resized(nWidth, nHeight);
        pWorld->InitializeByResized();
        pWorld->Resized(nWidth, nHeight);
		return 1;
	}
    else if (!(pCamera->viewHeight == nHeight && pCamera->viewWidth == nWidth && pCamera->aperture == nAperture))
    {
        pCamera->viewHeight = nHeight;
		pCamera->viewWidth = nWidth;
        pCamera->aperture = nAperture;
//        pWorld->Resized(nWidth, nHeight);       
    }
	return 0;
}


//투영을 설정한다.
int sglProjection(SGLContext* sContext,fProjection pFunction,void* pThis)
{
	IHWorld *pWorld = (IHWorld*)sContext->pWorld;
	pWorld->initProjection();
	if(pFunction) 
		pFunction(pThis,sContext);
	return E_SUCCESS;
}

#if defined ANDROID || defined IPHONE
//렌더링을 한다.
int sglRender(SGLContext* sContext,
			  fRenderBegin pRenderBegin,
			  fRendering   pRendering,
			  fRenderEnd   pRenderEnd,
			  void* pThis)
{
	int nResult = 0;

#if defined ANDROID //Added By Song 2014.06.26 아이폰은 부르는 곳에서 이미 정의 하였지만 안드로이드는 없다.
    IHWorld* pWorld = (IHWorld*)sContext->pWorld;
    if(pWorld && gDisplayWidth)
    {
        int nResultX = sglResize(sContext, gDisplayWidth, gDisplayHeight,pWorld->GetViewAperture());
        //내부적으로 사이즈가 변경될때만 뷰어의 초기화를 한다.
        if(nResultX == 1 || pWorld->getResetFrustum())
        {
            sglProjection(sContext,NULL,pThis);
            pWorld->ResetFrustum(false);
        }
    }
#endif //ANDROID
    

	nResult = sglRenderBegin(sContext,pRenderBegin,pThis);
	if(nResult != E_SUCCESS) return nResult;
	nResult = sglRendering(sContext,pRendering,pThis);
	if(nResult != E_SUCCESS) return nResult;	
	nResult = sglRenderEnd(sContext,pRenderEnd,pThis);
	if(nResult != E_SUCCESS) return nResult;
	
	return E_SUCCESS;
}

#else
//렌더링을 한다.
int sglRender(SGLContext* sContext,
			  int nViewWidth, 
			  int nViewHeight,
			  fProjection pProject,
			  fRenderBegin pRenderBegin,
			  fRendering   pRendering,
			  fRenderEnd   pRenderEnd,
			  void* pThis)
{
	int nResult = 0;
	IHWorld* pWorld = (IHWorld*)sContext->pWorld;
    if(pWorld == NULL) return E_SUCCESS;
	
	//내부적으로 사이즈가 변경될때만 뷰어의 초기화를 한다.
	if(sglResize(sContext,nViewWidth,nViewHeight,pWorld->GetViewAperture()) == 1 ||
	   (pWorld != NULL && pWorld->getResetFrustum()))
	{
		sglProjection(sContext,pProject,pThis);
		pWorld->ResetFrustum(false);
	}
	
	nResult = sglRenderBegin(sContext,pRenderBegin,pThis);
	if(nResult != E_SUCCESS) return nResult;
	nResult = sglRendering(sContext,pRendering,pThis);
	if(nResult != E_SUCCESS) return nResult;	
	nResult = sglRenderEnd(sContext,pRenderEnd,pThis);
	if(nResult != E_SUCCESS) return nResult;

	return E_SUCCESS;
}
#endif 

//렌더링을 초기화한다.
int sglRenderBegin(SGLContext* sContext,fRenderBegin pRenderBegin,void* pThis)
{
	int nResult = E_SUCCESS;
	IHWorld* pWorld = (IHWorld*)sContext->pWorld;
    if(pWorld == 0) return E_SUCCESS;
	pWorld->RenderBegin();
	if(pRenderBegin != 0) 
		nResult = pRenderBegin(pThis,sContext);
	return nResult;
}


//렌더링을 한다.
int sglRendering(SGLContext* sContext,fRendering pRendering,void* pThis)
{
	int nResult = E_SUCCESS;
	
	IHWorld* pWorld = (IHWorld*)sContext->pWorld;
    if(pWorld == 0) return E_SUCCESS;
	pWorld->Render();
	if(pRendering != 0) 
		nResult = pRendering(pThis,sContext);
	return nResult;
}

//렌더링이 종료될때 실행한다.
int sglRenderEnd(SGLContext* sContext,fRenderEnd pRenderEnd,void* pThis)
{
	int nResult = E_SUCCESS;
	IHWorld* pWorld = (IHWorld*)sContext->pWorld;
    if(pWorld == 0) return E_SUCCESS;
	pWorld->RenderEnd();
	if(pRenderEnd != 0) 
		nResult = pRenderEnd(pThis,sContext);
	return nResult;
}


//메모리를 해제한다.
void sglRelease(SGLContext* sContext)
{
	IHWorld *pWorld = 0;
	if(sContext != 0)
	{
		pWorld = (IHWorld*)sContext->pWorld;
		if(pWorld != 0) 
		{
			delete pWorld;
			pWorld = 0;
		}
		if(sContext != 0)
		{
			free((void*)sContext);
			sContext = 0;
		}
	}
}

void sglSerialize(SGLContext* sContext,bool bWrite)
{
    CArchive ar;
    IHWorld *pWorld = 0;
	if(sContext != 0)
	{
		pWorld = (IHWorld*)sContext->pWorld;
		pWorld->Serialize(ar, bWrite);
	}
}



extern "C" void ShowIDA(bool bShow);
void sglShowAD(bool bShow)
{
    ShowIDA(bShow);
}



bool sglIsFastModel()
{
    static short nType = -1;
    if (nType == -1)
    {
        const char* sModel = GetModel();
#if defined(ANDROID)
        //Android 2.3.x 단말기는 오래된 것.
        if(strstr(sModel,"2.3."))
            nType = 0;
        else
            nType = 1;
#elif defined(IPHONE)
        //iPhone3 or iPad1은 오래된 것이다.
        if(strstr(sModel,"iPhone3") || strstr(sModel,"iPad1"))
            nType = 0;
        else
            nType = 1;
#else
        nType = 1;
#endif //ANDROID
    }
    return nType == 1;
}

float gScale = 1.0f;
float sglGetDisplayScale()
{
    return gScale;
}
void sglSetDisplayScale(float fScale)
{
    gScale = fScale;
}
