//
//  CWorld.h
//  SongGL
//
//  Created by 송 호학 on 11. 11. 28..
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef SongGL_CWorld_h
#define SongGL_CWorld_h

#include "IHWorld.h"
#include "CScenario.h"
class CSGLCore;
class CWorld : public IHWorld
{
public:
    CWorld(CScenario* pScenario);
    virtual ~CWorld();
    virtual int Initialize(void* pResource); //리소스를 읽어온다.
    virtual void InitializeByResized() {} //리소스를 읽어온다. 광고가 뜰때 2D이미지의 좌표값이 틀려진다.
    virtual int Initialize1(SGLEvent *pEvent) { return E_SUCCESS;}
    virtual int Initialize2(SGLEvent *pEvent) { return E_SUCCESS;}
    virtual int Initialize3(SGLEvent *pEvent) { return E_SUCCESS;}
    virtual void Clear() = 0;
	virtual void ResetCamera();
	//윈도우 사이즈가 변경되었을 때 발생한다.
	virtual int Resized(int nWidth,int nHeight);
    virtual int getResetFrustum() { return mbResetFrustum;}
	virtual void ResetFrustum(int bFrustum) { mbResetFrustum = bFrustum;}
    
    
	//윈도우즈사이즈가 변경되거나, 카메라의 원거리가 변경되거나 할때 프로젝션을 재설정한다.
	virtual int initProjection();
	virtual int RenderBegin() = 0;
    virtual int Render() = 0;
    virtual int RenderEnd() = 0;    
    virtual void SetStatus(int v) { mnStatus = v;}
    virtual int GetStatus() { return mnStatus;}
    
    virtual void SetActor(CSprite* pSprite) = 0;	
    virtual CSprite* GetActor() = 0; 
    virtual int OnEvent(SGLEvent *pEvent) = 0;
    virtual void SetCamera(SGLCAMERA* pCam);
	virtual SGLCAMERA* GetCamera() { return &mCamera;}
    virtual int ActivateCamera();
    virtual CTextureMan* GetTextureMan() = 0;
    virtual CModelMan* GetModelMan() = 0;
    
    virtual void BeginTouch(long lTouchID,float x, float y) = 0;
    virtual void MoveTouch(long lTouchID,float x, float y) = 0;
    virtual void EndTouch(long lTouchID,float x, float y) = 0;
    virtual CUserInfo* GetUserInfo() { return NULL;}
    
    virtual CSGLCore*               GetSGLCore() { return 0;}
    virtual unsigned int GetNewID()                  { return 1;}
//    virtual unsigned long GetGGTime() { return 0;}
    virtual unsigned short GetMapLevel() { return 1;}
    
#ifdef ANDROID
    virtual void ResetTexture();
    virtual GLuint SetCurrentTexture(const char* sKey,unsigned char* sPixels,int nW,int nH,int nDepth) { return 0;}
#endif
    
    virtual int GetMovePosiztionY(SPoint* pNow) {return E_SUCCESS;}
    
    virtual void Serialize(CArchive& ar,bool bWrite) {};
    

    
    CScenario* GetScenario() { return mpScenario;}
protected:
    SGLCAMERA			mCamera;
	int					mbResetFrustum;
    // 0x0000 초기화가 안됨, 0x0001 초기화중, 0x0002 초기화가 됨
	int mnStatus;
    CScenario* mpScenario;
};
#endif
