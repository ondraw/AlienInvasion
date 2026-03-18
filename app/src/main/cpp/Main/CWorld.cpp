//
//  CWorld.cpp
//  SongGL
//
//  Created by 송 호학 on 11. 11. 28..
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//
#include "sGL.h"
#include "CWorld.h"
#include "sGLUtils.h"


CWorld::CWorld(CScenario* pScenario)
{
    mnStatus = 0x0000;
    mbResetFrustum = false;
    mpScenario = pScenario;
}

CWorld::~CWorld()
{
    
}


int CWorld::Initialize(void* pResource)
{
    ResetCamera();
    
	return E_SUCCESS;	
}

void CWorld::ResetCamera()
{
    SGLCAMERA Camera;
    
    Camera.aperture = GetViewAperture();
	
	Camera.rotPoint.x = 0;
	Camera.rotPoint.y = 0;
	Camera.rotPoint.z = 0;
	
	Camera.viewPos.x = 0.0;
	Camera.viewPos.y = 0.0;
	Camera.viewPos.z = 20; 
	
	
	Camera.viewDir.x = -Camera.viewPos.x; 
	Camera.viewDir.y = -Camera.viewPos.y; 
	Camera.viewDir.z = -Camera.viewPos.z;
	
	sglNormalize(&Camera.viewDir);
	
	Camera.viewUp.x = 0;  
	Camera.viewUp.y = 0; 
	Camera.viewUp.z = 1;
	
	Camera.ViewNear = 1.5f;	// Proejction에서 동적으로 구해진다.
	Camera.ViewFar = 32.0f; // 멀리볼수있는 한계 (초기화된후에 정해진다)
	Camera.ViewFarOrg = Camera.ViewFar; //처음 initialize에서 설정된 값.

    
    SetCamera(&Camera);
}


void CWorld::SetCamera(SGLCAMERA* pCam)
{
	mCamera.aperture = pCam->aperture;
	
	mCamera.rotPoint.x = pCam->rotPoint.x;
	mCamera.rotPoint.y = pCam->rotPoint.y;
	mCamera.rotPoint.z = pCam->rotPoint.z;
	
	mCamera.viewPos.x = pCam->viewPos.x;
	mCamera.viewPos.y = pCam->viewPos.y;
	mCamera.viewPos.z = pCam->viewPos.z;
	
	mCamera.viewDir.x = pCam->viewDir.x; 
	mCamera.viewDir.y = pCam->viewDir.y; 
	mCamera.viewDir.z = pCam->viewDir.z;
	
	mCamera.viewUp.x = pCam->viewUp.x ;  
	mCamera.viewUp.y = pCam->viewUp.y; 
	mCamera.viewUp.z = pCam->viewUp.z;
	
	
	mCamera.ViewNear = pCam->ViewNear;	// Proejction에서 동적으로 구해진다.
	mCamera.ViewFar = pCam->ViewFar; // 멀리볼수있는 한계 (초기화된후에 정해진다)
	mCamera.ViewFarOrg = pCam->ViewFarOrg; //처음 initialize에서 설정된 값.
	
}


//윈도우 사이즈가 변경되었을 때 발생한다.
int CWorld::Resized(int nWidth,int nHeight)
{
	glViewport (0, 0, nWidth, nHeight);
    return E_SUCCESS;
}


int CWorld::initProjection()
{
    
	//Light 조명 -- 조염이 너무 어둡다. 스프라이트는 자체 조명을 사용하기때문에 스프라이트이후에 그림을 그린후에 
    //glDisable(GL_LIGHTING)을 하기때문에 그려진다.
    //이부분은 그리고자하는 부분에서 glDiable되기때문에 문제가 있다.
    //	/////////////////////////////////////////////////////////////////////
    //광원은 전부 1로 해주고 재질을 통해서 반사율을 조절해준다.
    
	const GLfloat			lightAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
	const GLfloat			lightDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
	const GLfloat			lightPosition[] = {1.0f, 1.0f, 0.0f, 0.0f};
    const GLfloat			lightSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    
    
	//Configure OpenGL lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	
    
    //glEnable(GL_NORMALIZE);
	/////////////////////////////////////////////////////////////////////
	
	// set projection
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
    
	SGLCAMERA *pCamera = GetCamera();
    
      
    pCamera->ViewNear = 1.0f;
	sglPerspectivef(
                    pCamera->aperture, 
                    (GLfloat)pCamera->viewWidth / (GLfloat)pCamera->viewHeight, 
                    pCamera->ViewNear, FARVIEW);
   	return E_SUCCESS;
}

int CWorld::ActivateCamera()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt (mCamera.viewPos.x, 
			   mCamera.viewPos.y, 
			   mCamera.viewPos.z,
			   mCamera.viewPos.x + mCamera.viewDir.x,
			   mCamera.viewPos.y + mCamera.viewDir.y,
			   mCamera.viewPos.z + mCamera.viewDir.z,
			   mCamera.viewUp.x, 
			   mCamera.viewUp.y ,
			   mCamera.viewUp.z);	
	return E_SUCCESS;
}


#ifdef ANDROID
void CWorld::ResetTexture()
{
    CTextureMan *pMan = GetTextureMan();
    if(pMan)
        pMan->Reset();
}
#endif