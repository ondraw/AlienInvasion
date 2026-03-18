//
//  ARollCtl.h
//  SongGL
//
//  Created by 송 호학 on 11. 12. 11..
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef SongGL_ARollCtl_h
#define SongGL_ARollCtl_h
#include "CControl.h"
#include "sGLDefine.h"
#include "CControl.h"
#include "CPicking.h"
#include <vector>
using namespace std;
#include "CProperty.h"
#define ROLL_FACE_COUNT 10




typedef struct _RollItem 
{
    int    nID;
    GLuint nTextureID;
    //Some thing
    ARender*  pRender;
    PROPERTYI*     pRollProperty;
    
#ifdef ANDROID
    string sFileName;
#endif
    
}RollItem;


class ARollCtl : public CControl
{
public:
    ARollCtl(CControl* pParent,
             CTextureMan *pTextureMan,
             bool bRevolvingType = true, //자동문처럼 회전
             float fFar = 4.0f,          //화면에 꽉차게 = 0.0f 화면에서 멀리하면 컨트롤이 작아진다. 700.0f
             float fTourchSesitivity = 0.1f, //fFar = 0.0f이면 0.1f로 하면 되고 700.0f으로 하면 0.4정도가 적당한다.
             int nRollFaceCount = ROLL_FACE_COUNT); //면의 갯수 현재 면의 갯수는 10개이다.
    
    virtual ~ARollCtl();
    
    
    virtual void OnStopMotion();
    virtual void OnSelected();
    virtual int GetCurrentPos();
    virtual void SetCurrentPos(int nIndex);
    virtual int LoadResource();
    virtual int CloseResource();
    virtual void SetLight();
    
    
    
    virtual int Initialize(int nControlID,SPoint *pPosition,SVector *pvDirection);
    //ARender
    
	virtual int RenderBegin(int nTime);
	virtual int Render();
	virtual int RenderEnd();	
    virtual int OnEvent(SGLEvent *pEvent);
    virtual bool IsTabIn(int nIndex,float x,float y);
    
    virtual bool BeginTouch(long lTouchID,float x, float y);
    virtual bool MoveTouch(long lTouchID,float x, float y);
    virtual bool EndTouch(long lTouchID,float x, float y);
    
    RollItem* GetItem(int nPos) { return mlstData[nPos];}
    void RotMotion(int nTime);
    bool isLeftRoll(bool bTouch = false);
    bool isRightRoll();
    int ActivateCamera();
    float GetCurrentAngle() { return mfCurrentAngle;}
    
    void SetAniZoomIn(float fAccel);
    void SetAniZoomOut(float fAccel);
    
#ifdef ANDROID
    virtual void ResetTexture();
#endif
//    virtual int Animation() { return mAniType;}
protected:
    virtual int Initialize(SPoint *pPosition,SVector *pvDirection);
    
    
    
protected:

    std::vector<RollItem*>      mlstData;
    float         mfCurrentAngle;
    int           mFaceCount;
    int           mViewHeight;
    float         mViewZ;   
    bool          mbRevolvingType; //계속해서 회전할수 있는 타임.
    float         mfFarOrg;//

    
    //GetCurrentPos에서 연산을 하지 않고 각도가 같으면 기억해둔 인덱스를 리턴한다.
    int mnBeforeIndex;
    float mnBeforeAngle;
    long mlTouchID;     
    int mnTouchedIndex; 
    int mnTouchStartPos;
    float mfBeforeX;
    float mfTouchStartAngle;
    //bool mbTouchMoving;
    int mnRotDirection;
    float mAccelator;    
    float mfTourceSensitivity; //터치의 감도를 
    float mfFar;//컨트롤을 작게 보기위해서 설정한다.  
    
    float mfSelectedAngle; //이전에 선택하였던것을 기억한다.
};


#endif
