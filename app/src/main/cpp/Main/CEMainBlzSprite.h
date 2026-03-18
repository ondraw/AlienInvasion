//
//  CBlzSprite.h
//  SongGL
//
//  Created by Songs on 11. 7. 26..
//  Copyright 2011 thinkm. All rights reserved.
//
#ifndef _CEMAINBLZSPRITE_H
#define _CEMAINBLZSPRITE_H
#include "CSprite.h"
#include "CMS3DASCII.h"

//#define MainTowerDefend 170.f
#define MainTowerDefend 400.f
class CTextureMan;
class CEMainBlzSprite : public CSprite
{
public:
    CEMainBlzSprite(unsigned int nID,unsigned char cTeamID,int nModelID,CTextureMan *pTextureMan,IAction *pAction,IHWorld* pWorld,NETWORK_TEAMTYPE NetworkTeamType);
	virtual ~CEMainBlzSprite();
    virtual int     LoadResoure(CModelMan* pModelMan);
    virtual int     Initialize(SPoint *pPosition,SVector *pvDirection);
    virtual int     RenderBegin(int nTime);
	virtual int     Render();
    virtual int     RenderEnd();
    virtual int     OnEvent(SGLEvent *pEvent);
    virtual bool    CollidesBounding(CSprite* Sprite,bool bExact);
    virtual float   GetRadius() { return mfRadian;}
    virtual float   GetMaxDefendPower();// { return 400.0f;}      //방어력 100 발을 맞아야 한다.
    virtual float GetMaxScore() { return 20.f;}
    virtual bool    IsTabIn(CPicking* pPick,SPoint& nearPoint,SPoint& farPoint,int nPosX,int nPosY);

    virtual void RenderBeginCore_Invisiable(int nTime);
    virtual void RenderBeginCore(int nTime);

    virtual int GetType() { return ACTORTYPE_EMAINBLZ;}
    
    //동기화 대상이 아니다..
    void ArchiveMultiplay(int nCmd,CArchive& ar,bool bWrite) {};
    
protected:
    CMS3DASCII *mpModel;
    sglMesh    *mpWorldMesh;
    float      mRotationAngle;
    float      mfRadian;
};


#endif //_CBLZSPRITE_H