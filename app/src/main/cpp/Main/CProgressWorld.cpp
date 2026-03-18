//
//  CProgressWorld.cpp
//  SongGL
//
//  Created by Song Hohak on 11. 11. 29..
//  Copyright (c) 2011 thinkm. All rights reserved.
//

#include "sGL.h"
#include "CProgressWorld.h"
#include "CTextureMan.h"
#include "CSoundMan.h"
#include "sGLTrasform.h"
#include "sGLUtils.h"
#include "CImgProgressCtl.h"
#include "CScenario.h"
#include "CControl.h"

CProgressWorld::CProgressWorld(CScenario* pScenario) : CWorld(pScenario)
{
    mTextureLogoID = 0;
    m_pTextureMan = new CTextureMan;
    mProgressBack = new CLabelCtl(NULL,m_pTextureMan);
    mProgressCtl = NULL;
    mnTotal = 0;
}

CProgressWorld::~CProgressWorld()
{
    if(m_pTextureMan)
    {
        delete m_pTextureMan;
        m_pTextureMan = 0;
    }
    
    if(mProgressBack)
    {
        delete mProgressBack;
        mProgressBack = NULL;
    }
}

void CProgressWorld::InitializeByResized()
{
    if(mnStatus & 0x0002) //초기화가 되어 있다면 다시 초기화를 해준다.
    {
        int nPos = mProgressCtl->GetPosition();
        initProjection(); //xmax_3d,ymax_3d 재계한되어야 한다.
        Initialize(NULL);
        mProgressCtl->SetPosition(nPos);
    }
}



//0~100
void CProgressWorld::SetProgressPos(int nPos)
{
    CScenario::SendMessage(SGL_MESSAGE_PROGRESS_POS,nPos);
}

int CProgressWorld::Initialize(void* pResource)
{
	CWorld::Initialize(pResource);
    vector<string> lstimg;
    vector<float> layout;
    if(pResource)
    {
        char sFilePath[256];//(char*)pResource;
        sprintf(sFilePath,"%s/",(const char*)pResource);
        CLoader::setSearchPath((const char*)sFilePath);
    }
    
	//-----------------------------------------------------
	mnStatus = 0x0001;
    
    if(pResource)
        Clear();
    
    
    memcpy(mfWorldBackCoordTex, gPanelCoordTex, sizeof(mfWorldBackCoordTex));
    sglGet2DTextureCoord(0.f,0.f,1.0f,0.7490234375f,mfWorldBackCoordTex);
    
    //int nDisplayScaleX = gDisplayWidth / 4;
    //int nDisplayScaleY = gDisplayHeight / 4;
    
    //-------------------------------------------------------------
    float matrix[16];
    int nIndex = 0;
    sglLoadIdentityf(matrix);
    
    //화면에 꽉차게 조정한다.(정점의 순서가 좌우가 바뀌었다.) - xmax_3d
    sglScaleMatrixf(matrix,  - xmax_3d * CAMERA_YPOS_2D , ymax_3d * CAMERA_YPOS_2D, 0.0f ); 
    for (int i = 0; i < 4; i++) 
    {
        nIndex = i*3;
        sglMultMatrixVectorf(&mfWorldFloorVertex[nIndex], matrix,&gPanelVertexY[nIndex]);
    }
    
    mTextureLogoID = m_pTextureMan->GetTextureID(IMG_PROGRESS_BACK);
    
    layout.clear();
    PROPERTYI::GetPropertyFloatList("P_Back", layout);
    
    int nW = layout[2];
    int nXPos = (gDisplayWidth - nW) / 2;
    lstimg.clear();
    lstimg.push_back(IMG_PROGRESS_BACK);
    lstimg.push_back("none");
    mProgressBack->Initialize(0, nXPos, gDisplayHeight + layout[1], nW, layout[3],lstimg,
                              0.0126953125,0.7607421875,0.4091796875,0.8076171875);

    layout.clear();
    PROPERTYI::GetPropertyFloatList("P_BackCtl", layout);

    mProgressCtl = new CImgProgressCtl(mProgressBack,m_pTextureMan);
    mProgressCtl->Initialize(0, nXPos + layout[0], gDisplayHeight + layout[1], nW + layout[2], layout[3],lstimg,
                             0.0205078125,0.8154296875,0.3740234375,0.8466796875);
                             //0.0126953125,0.7607421875,0.4091796875,0.8076171875);
    mProgressBack->AddControl(mProgressCtl);
    
    //-----------------------------------
	this->ResetFrustum(true); //모든것이 초기화 되었기때문에 프리즘을 다시 설정한다.
	//-----------------------------------------------------    
	mnStatus |= 0x0002;
	return E_SUCCESS;	
}

int CProgressWorld::OnEvent(SGLEvent *pEvent)
{ 
    int nPos;
    switch (pEvent->nMsgID) 
    {
        case SGL_MESSAGE_PROGRESS_STEP:
            nPos = mProgressCtl->GetPosition();
            nPos += 10;
            mProgressCtl->SetPosition(nPos);
            break;
        case SGL_MESSAGE_PROGRESS_POS:
            mProgressCtl->SetPosition(pEvent->lParam);
            break;
        default:
            break;
    }
    return E_SUCCESS;
}

int CProgressWorld::initProjection()
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

void CProgressWorld::ResetCamera()
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

void CProgressWorld::Clear()
{
    m_pTextureMan->Clear();
}

int CProgressWorld::RenderBegin()
{ 
    int nTime = GetClockDelta();
    
    if( (mnStatus & 0x0002) != 0x0002) 
		return E_SUCCESS;
    
    mProgressCtl->RenderBegin(nTime);
    return E_SUCCESS;
}

int CProgressWorld::Render()
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
    glTexCoordPointer(2, GL_FLOAT, 0, mfWorldBackCoordTex);
    glVertexPointer(3, GL_FLOAT, 0, mfWorldFloorVertex);
    glDrawElements(GL_TRIANGLE_STRIP, gshPanelIndicesCnt, GL_UNSIGNED_SHORT, gPanelIndices);
    
    
    CControl::ActivateCamera();

    glDisable(GL_DEPTH_TEST);
    mProgressBack->Render();
    glEnable(GL_DEPTH_TEST);

    glDisable(GL_BLEND);    
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);   
    glDisable(GL_TEXTURE_2D);
    
    return E_SUCCESS;
}

float CProgressWorld::GetViewAperture()
{
    if(sglIsFastModel())
        return VIEW_APERTURE_WORLD;
    return VIEW_APERTURE;
}

int CProgressWorld::RenderEnd()
{ 
    //초기화가 안되었거나 초기화중이다.
	if( (mnStatus & 0x0002) != 0x0002) 
		return E_SUCCESS;	
    
    mnTotal += GetClockDeltaConst();
    return E_SUCCESS;
}
void CProgressWorld::SetActor(CSprite* pSprite) {};	
CSprite* CProgressWorld::GetActor() {return NULL;} 

CTextureMan* CProgressWorld::GetTextureMan(){ return m_pTextureMan;}
CModelMan* CProgressWorld::GetModelMan(){ return NULL;}
void CProgressWorld::BeginTouch(long lTouchID,float x, float y){}
void CProgressWorld::MoveTouch(long lTouchID,float x, float y){};
void CProgressWorld::EndTouch(long lTouchID,float x, float y){};
