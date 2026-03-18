//
//  CLogoWorld.h
//  SongGL
//
//  Created by 송 호학 on 11. 11. 28..
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef SongGL_CLogoWorld_h
#define SongGL_CLogoWorld_h
#include "CWorld.h"

class CUserInfo;
class CTextureMan;
class CSoundMan;
//class CModelMan;
class CLogoWorld : public CWorld
{
public:
    CLogoWorld(CScenario* pScenario);
    virtual ~CLogoWorld();
    virtual int GetWorldType() { return WORLD_TYPE_LOGO;}
    
    virtual int Initialize(void* pResource); //리소스를 읽어온다.
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
    
#ifdef ANDROID
    virtual void ResetTexture();
#endif
    
protected:
    CTextureMan                 *m_pTextureMan;
//    CModelMan                   *m_pModelMan;
    float                       mfWorldFloorVertex[12];
    GLuint                      mTextureLogoID;
    GLuint                      mnTotal;

#ifdef ANDROID
    bool mNextLogo;
#endif
};

#endif
