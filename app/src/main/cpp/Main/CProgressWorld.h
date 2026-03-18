//
//  CProgressWorld.h
//  SongGL
//
//  Created by Song Hohak on 11. 11. 29..
//  Copyright (c) 2011 thinkm. All rights reserved.
//

#ifndef SongGL_CProgressWorld_h
#define SongGL_CProgressWorld_h
#include "CWorld.h"




class CTextureMan;
class CSoundMan;
class CModelMan;
class CImgProgressCtl;
class CControl;
class CProgressWorld : public CWorld
{
public:
    CProgressWorld(CScenario* pScenario);
    virtual ~CProgressWorld();
    virtual int GetWorldType() { return WORLD_TYPE_PROGRESS;}
    virtual int Initialize(void* pResource); //리소스를 읽어온다.
    virtual void InitializeByResized();
	virtual void ResetCamera();
    virtual int initProjection();
    virtual void SetActor(CSprite* pSprite);	
    virtual CSprite* GetActor(); 
    virtual int OnEvent(SGLEvent *pEvent);
    virtual void Clear();
    virtual int RenderBegin();
    virtual int Render();
    virtual int RenderEnd();    
    virtual CTextureMan* GetTextureMan();
    virtual CModelMan* GetModelMan();
    virtual void BeginTouch(long lTouchID,float x, float y);
    virtual void MoveTouch(long lTouchID,float x, float y);
    virtual void EndTouch(long lTouchID,float x, float y);
    virtual CUserInfo* GetUserInfo() { return mpScenario->GetUserInfo();}
    
    //0~100
    void SetProgressPos(int nPos);
    //CHWorld에서 쓰레드로 2D컨트롤을 만들어주는 사이에 사이각이 프로그레스 월드로 대체하기 때문에 CHWorld와 같아야 한다.
    virtual float GetViewAperture();// { return VIEW_APERTURE_WORLD;}
    
protected:
    CTextureMan                 *m_pTextureMan;
    //    CModelMan                   *m_pModelMan;
    float                       mfWorldFloorVertex[12];
    GLuint                      mTextureLogoID;
    GLuint                      mnTotal;
    
    CControl                    *mProgressBack;
    CImgProgressCtl             *mProgressCtl;
    float                       mfWorldBackCoordTex[8];
};

#endif
