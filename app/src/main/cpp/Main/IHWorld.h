//
//  IHWorld.h
//  SongGL
//
//  Created by 송 호학 on 11. 11. 28..
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef SongGL_IHWorld_h
#define SongGL_IHWorld_h

#include <strings.h>
#include "ListNode.h"
#include "sGLDefine.h"
#include "CSprite.h"
#include "CArchive.h"

class CMutiplyProtocol;
class CUserInfo;
class IHWorld
{
public:
    virtual ~IHWorld() { }
    virtual int GetWorldType() = 0;
    virtual int Initialize(void* pResource) = 0; //리소스를 읽어온다.
    virtual void InitializeByResized() = 0; //리소스를 읽어온다. 광고가 뜰때 2D이미지의 좌표값이 틀려진다.
	virtual void Clear() = 0;
	
    virtual void ResetCamera() = 0;
	//윈도우 사이즈가 변경되었을 때 발생한다.
	virtual int Resized(int nWidth,int nHeight) = 0;
    virtual int getResetFrustum() = 0;
	virtual void ResetFrustum(int bFrustum) = 0;

    
	//윈도우즈사이즈가 변경되거나, 카메라의 원거리가 변경되거나 할때 프로젝션을 재설정한다.
	virtual int initProjection() = 0;
	virtual int RenderBegin() = 0;
	virtual int Render() = 0;
	virtual int RenderEnd() = 0;
	virtual void SetStatus(int v) = 0;
    virtual int GetStatus() = 0;
    
	//주인공을 설정한다.
	virtual void SetActor(CSprite* pSprite) = 0;	
	virtual CSprite* GetActor() = 0; 
	virtual int OnEvent(SGLEvent *pEvent) = 0;
    virtual void SetCamera(SGLCAMERA* pCam) = 0;
	virtual SGLCAMERA* GetCamera() = 0;
    virtual int ActivateCamera() = 0;
    virtual CTextureMan* GetTextureMan() = 0;
    virtual CModelMan* GetModelMan() = 0;
    
    //화면 터치를 눌렀을 경우 좌우 스틱의 영역에 포함되는지를 체크 한다.
    virtual void BeginTouch(long lTouchID,float x, float y) = 0;
    virtual void MoveTouch(long lTouchID,float x, float y) = 0;
    virtual void EndTouch(long lTouchID,float x, float y) =0;
  
    virtual CUserInfo* GetUserInfo() = 0;
    virtual unsigned int GetNewID() = 0;
    
#ifdef ANDROID
    virtual void ResetTexture() = 0;
#endif //ANDROID
    
    virtual void Serialize(CArchive& ar,bool bWrite) = 0;
    
    virtual float GetViewAperture() {return VIEW_APERTURE;}
    virtual CMutiplyProtocol* GetMutiplyProtocol() { return NULL;}
#ifdef ACCELATOR
    virtual void SetAccelatorUpVector(float fv) {}
#endif //ACCELATOR
//    virtual float GetViewAperture()
//    {
//        if(sglIsFastModel())
//            return VIEW_APERTURE_WORLD;
//        return VIEW_APERTURE;
//    }
};

#endif
