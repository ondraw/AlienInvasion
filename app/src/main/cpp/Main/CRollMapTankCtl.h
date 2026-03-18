//
//  CRollMapTankCtl.h
//  SongGL
//
//  Created by 송 호학 on 11. 12. 11..
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef SongGL_CRollMapTankCtl_h
#define SongGL_CRollMapTankCtl_h

#include "CControl.h"
#include "ARollCtl.h"
class CRollMapCtl;
class CMS3DASCII;
class CRollTankCtl;
class CUserInfo;
class CRollMapTankCtl : public CControl
{
public:
    enum CRollMapTankCtl_Type { 
        CRollMapTankCtl_Map, //현재 상태가 맵을 선택할 수 있다.
        CRollMapTankCtl_Tank};//현재 상태가 탱크를 선택할수 있다.
    CRollMapTankCtl(CControl* pParent,
                    CUserInfo* pUserInfo,
                    CTextureMan *pTextureMan,
                    float fFar = 4.0f,          //화면에 꽉차게 = 0.0f 화면에서 멀리하면 컨트롤이 작아진다. 700.0f
                    float fTourchSesitivity = 0.1f, //fFar = 0.0f이면 0.1f로 하면 되고 700.0f으로 하면 0.4정도가 적당한다.
                    int nRollFaceCount = ROLL_FACE_COUNT);
    virtual ~CRollMapTankCtl();
    
    virtual void SetCurrentPos(int nPosMap,int nPosTank);
    virtual int  GetCurrentPos(CRollMapTankCtl_Type v);
    RollItem* GetItem(int nPos,CRollMapTankCtl_Type v);
    virtual void SetAni(CONTROLANI nAniType,float fAccel = 1.0f);
    
    virtual int Initialize(int nControlID);
    virtual int RenderBegin(int nTime);
	virtual int Render();
	virtual int RenderEnd();	

    
    virtual bool BeginTouch(long lTouchID,float x, float y);
    virtual bool MoveTouch(long lTouchID,float x, float y);
    virtual bool EndTouch(long lTouchID,float x, float y);
    
    
    virtual CONTROLANI Animating();
    
    void DrawFloor(bool bReflactor);
    void SetType(CRollMapTankCtl_Type v) { mType = v;}
    
#ifdef ANDROID
    virtual void ResetTexture();
#endif
    
 
    virtual bool GetHide();
    void ResetMapUpgradeRank();
    
    int  GetNowMapIndex();
protected:
    CRollMapTankCtl_Type    mType;
    CRollMapCtl             *mMapCtl;
    CRollTankCtl            *mTankCtl;
    CMS3DASCII              *mpModelBottom;
    CUserInfo               *mpUserInfo;
};

#endif
