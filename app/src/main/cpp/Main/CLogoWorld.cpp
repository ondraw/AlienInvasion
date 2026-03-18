//
//  CLogoWorld.cpp
//  SongGL
//
//  Created by 송 호학 on 11. 11. 28..
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//
#include "sGL.h"
#include "CLogoWorld.h"
#include "CTextureMan.h"
#include "CSoundMan.h"
#include "sGLTrasform.h"
#include "sGLUtils.h"
#include "CControl.h"
#include "sGLText.h"

#ifdef ANDROID
#include "SongGLJNI.h"
#include <jni.h>
#endif //ANDROID

#ifdef ANDROID
bool gbUngizInit = false;
#endif

CLogoWorld::CLogoWorld(CScenario* pScenario) : CWorld(pScenario)
{
    
        
    mTextureLogoID = 0;
    m_pTextureMan = new CTextureMan;
    mnTotal = 0;
#ifdef ANDROID
    mNextLogo = false;
#endif
}

CLogoWorld::~CLogoWorld()
{
    if(m_pTextureMan)
    {
        delete m_pTextureMan;
        m_pTextureMan = 0;
    }
//    if(m_pModelMan)
//    {
//        delete m_pModelMan;
//        m_pModelMan = 0;
//    }
}


int CLogoWorld::Initialize(void* pResource)
{
	CWorld::Initialize(pResource);
    
    char sFilePath[256];//(char*)pResource;
	sprintf(sFilePath,"%s/",(const char*)pResource);
	CLoader::setSearchPath((const char*)sFilePath);	
    
	//-----------------------------------------------------
	mnStatus = 0x0001;
	Clear();
    
    
    
#ifdef ANDROID
    HLogD("CLogoWorld::Initialize\n");
    mNextLogo = false;
    gbUngizInit = false; //adnroid는 수시로 내려가기 때문에 다시 그려준다.
    
    //HLogD("-------------------%d,%d\n",gDisplayWidth,gDisplayHeight);
    int nDisplayScaleX = gDisplayWidth / 2;
    int nDisplayScaleY = gDisplayHeight / 2;
    
    //-------------------------------------------------------------
    float matrix[16];
    int nIndex = 0;
    sglLoadIdentityf(matrix);
    sglScaleMatrixf(matrix, nDisplayScaleX, nDisplayScaleY, 0.0f);
    for (int i = 0; i < 4; i++)
    {
        nIndex = i*3;
        sglMultMatrixVectorf(&mfWorldFloorVertex[nIndex], matrix,&gPanelVertexY[nIndex]);
        CControl::DtoGL(mfWorldFloorVertex[nIndex], mfWorldFloorVertex[nIndex+1], &mfWorldFloorVertex[nIndex],&mfWorldFloorVertex[nIndex+1]);
    }
    CControl::MakeWorldStickVertexGL(mfWorldFloorVertex,mfWorldFloorVertex,0.0f,0.0f);
    
    HLogD("CLogoWorld::GetTextureIDByJava\n");
    
    mTextureLogoID = GetTextureIDByJava(IMG_WAITTING);
    if(mTextureLogoID == 0)
    {
        HLogE("[Error] %s not ID",IMG_WAITTING);
    }
    HLogD("CLogoWorld::GetTextureIDByJava End\n");
    HLogD("CLogoWorld::Initialize End\n");
#else
    int nDisplayScaleX = gDisplayWidth / 4;
    int nDisplayScaleY = gDisplayHeight / 4;
    
    //-------------------------------------------------------------
    float matrix[16];
    int nIndex = 0;
    sglLoadIdentityf(matrix);
    sglScaleMatrixf(matrix, nDisplayScaleX, nDisplayScaleY, 0.0f);
    for (int i = 0; i < 4; i++)
    {
        nIndex = i*3;
        sglMultMatrixVectorf(&mfWorldFloorVertex[nIndex], matrix,&gPanelVertexY[nIndex]);
        CControl::DtoGL(mfWorldFloorVertex[nIndex], mfWorldFloorVertex[nIndex+1], &mfWorldFloorVertex[nIndex],&mfWorldFloorVertex[nIndex+1]);
    }
    CControl::MakeWorldStickVertexGL(mfWorldFloorVertex,mfWorldFloorVertex,0.0f,0.0f);
    
    mTextureLogoID = m_pTextureMan->GetTextureID(IMG_LOGO);
#endif
    
    //-----------------------------------
	this->ResetFrustum(true); //모든것이 초기화 되었기때문에 프리즘을 다시 설정한다.
	//-----------------------------------------------------    
	mnStatus |= 0x0002;
	return E_SUCCESS;	
}

int CLogoWorld::initProjection()
{
    
	// set projection
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
    
	SGLCAMERA *pCamera = GetCamera();
	sglPerspectivef(
                    pCamera->aperture, 
                    (GLfloat)pCamera->viewWidth / (GLfloat)pCamera->viewHeight, 
                    pCamera->ViewNear, FARVIEW);
    
   	return E_SUCCESS;
}

void CLogoWorld::ResetCamera()
{
    SGLCAMERA Camera;
    
    Camera.aperture = GetViewAperture();
	
	Camera.rotPoint.x = 0;
	Camera.rotPoint.y = 0;
	Camera.rotPoint.z = 0;
	
	Camera.viewPos.x = 0.0;
	Camera.viewPos.y = 0.0;
	Camera.viewPos.z = CAMERA_YPOS_2D; 
	
	
	Camera.viewDir.x = -Camera.viewPos.x; 
	Camera.viewDir.y = -Camera.viewPos.y; 
	Camera.viewDir.z = -Camera.viewPos.z;

	Camera.viewUp.x = 0;  
	Camera.viewUp.y = 1; 
	Camera.viewUp.z = 0;
	
	Camera.ViewNear = 1.0f;	// Proejction에서 동적으로 구해진다.
	Camera.ViewFar = 32.0f; // 멀리볼수있는 한계 (초기화된후에 정해진다)
	Camera.ViewFarOrg = Camera.ViewFar; //처음 initialize에서 설정된 값.
    

    
    SetCamera(&Camera);
}

void CLogoWorld::Clear()
{
    m_pTextureMan->Clear();
}

int CLogoWorld::RenderBegin()
{ 
    GetClockDelta();

    if( (mnStatus & 0x0002) != 0x0002)
		return E_SUCCESS;
    
    
    return E_SUCCESS;
}

int CLogoWorld::Render()
{ 
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
    glColor4f(1.0f,1.0f,1.0f,1.0f);

    //초기화가 안되었거나 초기화중이다.
	if( (mnStatus & 0x0002) != 0x0002)
		return E_SUCCESS;

    ActivateCamera();

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnable(GL_TEXTURE_2D);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    glBindTexture(GL_TEXTURE_2D, mTextureLogoID);
    glTexCoordPointer(2, GL_FLOAT, 0, gPanelCoordTex);
    glVertexPointer(3, GL_FLOAT, 0, mfWorldFloorVertex);
    glDrawElements(GL_TRIANGLE_STRIP, gshPanelIndicesCnt, GL_UNSIGNED_SHORT, gPanelIndices);

    glDisable(GL_BLEND);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisable(GL_TEXTURE_2D);
    
    return E_SUCCESS;
}

int CLogoWorld::RenderEnd()
{ 
    //초기화가 안되었거나 초기화중이다.
	if( (mnStatus & 0x0002) != 0x0002) 
		return E_SUCCESS;	
    
    mnTotal += GetClockDeltaConst();
    
#ifdef ANDROID
    //static bool bInit = false;
    if(gbUngizInit == false)
    {
        //2019.02.12

        //Zip파일이 존재하면 zip을 storage파일에 저장한다.
//        HLogD("Java Call sglUnzip3DResource");
//        sglUnzip3DResource();
//        HLogD("Java Call sglUnzip3DResource End");
//        gbUngizInit = true;
    }


    if(mNextLogo)
    {
        if(mnTotal > 100)
        {
            //메뉴로 이동한다.
            mpScenario->GoToWorld(NULL, CSCENARIO_MAINMENU);
            gbUngizInit = false; //adnroid는 수시로 내려가기 때문에 다시 그려준다.
        }
    }

    
#else
    if(mnTotal > 100)
    {   
        //메뉴로 이동한다.
        mpScenario->GoToWorld(NULL, CSCENARIO_MAINMENU);
    }
#endif //ANDROID
    return E_SUCCESS;
}

#ifdef ANDROID
void CLogoWorld::ResetTexture()
{
    //Deleted By Song 2015.01.02 //자바쪽에서는 zip파일이 Thread안쪽에 있다 즉 모든 리소스를 풀고 나서 환경 변수들을 읽어온다. SongGLLib.sglTextUnitInitialize(SongGLLib.getPath()); 그러는 사이엔 로그인 액티비티가 생성되면 다시 RestTuexture이벤트가 날라온다.
    //그러면 내부적으로 리셋을 할때 객체가 만들어지지 않았기때문에 죽는다.
    //CWorld:ResetTexture();
}
#endif



void CLogoWorld::SetActor(CSprite* pSprite) {};
CSprite* CLogoWorld::GetActor() {return NULL;}

int CLogoWorld::OnEvent(SGLEvent *pEvent)
{
#ifdef ANDROID
    if(pEvent->nMsgID == SGL_WAIT_TO_LOGO_ANDROID)
    {
        int nDisplayScaleX = gDisplayWidth / 4;
        int nDisplayScaleY = gDisplayHeight / 4;

        //-------------------------------------------------------------
        float matrix[16];
        int nIndex = 0;
        sglLoadIdentityf(matrix);
        sglScaleMatrixf(matrix, nDisplayScaleX, nDisplayScaleY, 0.0f);
        for (int i = 0; i < 4; i++)
        {
            nIndex = i*3;
            sglMultMatrixVectorf(&mfWorldFloorVertex[nIndex], matrix,&gPanelVertexY[nIndex]);
            CControl::DtoGL(mfWorldFloorVertex[nIndex], mfWorldFloorVertex[nIndex+1], &mfWorldFloorVertex[nIndex],&mfWorldFloorVertex[nIndex+1]);
        }
        CControl::MakeWorldStickVertexGL(mfWorldFloorVertex,mfWorldFloorVertex,0.0f,0.0f);
        //mTextureLogoID = GetTextureIDByJava(IMG_LOGO);
        mTextureLogoID = m_pTextureMan->GetTextureID(IMG_LOGO);
        mNextLogo = true;
        mnTotal = 0;
    }
#endif
    return E_SUCCESS;
}

CTextureMan* CLogoWorld::GetTextureMan(){ return m_pTextureMan;}
CModelMan* CLogoWorld::GetModelMan(){ return NULL;}
void CLogoWorld::BeginTouch(long lTouchID,float x, float y){}
void CLogoWorld::MoveTouch(long lTouchID,float x, float y){};
void CLogoWorld::EndTouch(long lTouchID,float x, float y){};