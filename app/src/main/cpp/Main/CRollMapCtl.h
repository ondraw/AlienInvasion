//
//  CRollMapCtl.h
//  SongGL
//
//  Created by 송 호학 on 11. 12. 13..
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef SongGL_CRollMapCtl_h
#define SongGL_CRollMapCtl_h


#include "ARollCtl.h"
#include <vector>
using namespace std;
typedef struct _MAPTITLE{
    GLuint nTextureID;
    float fU;
    float fV;
    bool  bCompleted;
    int   nCompletedCnt;
}MAPTITLE;

class CUserInfo;
class CMS3DASCII;
class CRollMapCtl : public ARollCtl
{
public:
    CRollMapCtl(CControl* pParent,
             CTextureMan *pTextureMan,
             CUserInfo   *pUserInfo,
             bool bRevolvingType = true, //자동문처럼 회전
             float fFar = 4.0f,          //화면에 꽉차게 = 0.0f 화면에서 멀리하면 컨트롤이 작아진다. 700.0f
             float fTourchSesitivity = 0.1f, //fFar = 0.0f이면 0.1f로 하면 되고 700.0f으로 하면 0.4정도가 적당한다.
             int nRollFaceCount = ROLL_FACE_COUNT); //면의 갯수 현재 면의 갯수는 10개이다.
    
    virtual ~CRollMapCtl();
    virtual int LoadResource();
    virtual int Initialize(int nControlID,SPoint *pPosition,SVector *pvDirection);
    virtual int CloseResource();
    virtual bool IsTabIn(int nIndex,float xWinPos,float yWinPos);
    
    //ARender
	virtual int RenderBegin(int nTime);
	virtual int Render();
	virtual int RenderEnd();	
    virtual int OnEvent(SGLEvent *pEvent);
    
    int DrawRoll(bool bshadow);
    void ResetMapUpgradeRank();

#ifdef ANDROID
    virtual void ResetTexture();
#endif
    int GetNowMapIndex() { return mnNowMapIndex;}
    
protected:
    virtual int Initialize(SPoint *pPosition,SVector *pvDirection);

protected:
    CMS3DASCII  *mpModel;
    CPicking    mPicking;

    GLuint      mMapFrameTexture;
    GLuint      mMapCompleteTexture;
    GLuint      mMapCompleteKeyTexture;
    GLuint      mMapCompleteCountTexture;
    float       mfWorldTextCoordTex[8];
    
    vector<MAPTITLE*> mLstMapTitle;
    CUserInfo   *mpUserInfo;
    int         mnNowMapIndex;
};
#endif
