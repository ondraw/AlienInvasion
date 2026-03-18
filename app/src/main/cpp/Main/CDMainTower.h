//
//  CDMainTower.h
//  SongGL
//
//  Created by itsme on 13. 10. 25..
//
//

#ifndef __SongGL__CDMainTower__
#define __SongGL__CDMainTower__

#include "CSprite.h"
#include "CMS3DASCII.h"
#define MAXMAINSTATUS 100.f

class CAniRuntimeBubble;
class CMS3DModelASCIIMan;
class CDMainTower : public CSprite
{
public:
    CDMainTower(unsigned int nID,unsigned char cTeamID,int nModelID,CTextureMan *pTextureMan,IAction *pAction,IHWorld* pWorld,PROPERTY_TANK* pProperty,NETWORK_TEAMTYPE NetworkTeamType);
    virtual ~CDMainTower();
    
    
    virtual int     LoadResoure(CModelMan* pModelMan);
    virtual int     Initialize(SPoint *pPosition,SVector *pvDirection);
    
    virtual int     RenderBegin(int nTime);
	virtual int     Render();
    virtual int     RenderEnd();
    virtual int     OnEvent(SGLEvent *pEvent);
    virtual void    RenderBeginCore(int nTime);

    
    bool            CollidesBounding(CSprite* Sprite,bool bExact);
    virtual float   GetRadius() { return mfRadian;}
    float           GetArearRadus();
    virtual bool    VisibleByCamera();

    virtual void    RenderBeginCore_Invisiable(int nTime);
    
    virtual float   GetMaxDefendPower() { return mfMaxDefendPowerActor;}
    virtual float   GetCurrentDefendPowerEn() { return mfNowStatus;}
    virtual void    SetCurrentDefendPowerEn(float v);
    
    
    virtual bool    IsTabIn(CPicking* pPick,SPoint& nearPoint,SPoint& farPoint,int nPosX,int nPosY);

    virtual bool IsMaking() { return mnMakingTime == 0 ? false : true;}
    virtual void ResetPower(bool bRunTimeUpgrade = false);
    
    virtual void SetAniRuntimeBubble(CAniRuntimeBubble* v) {mpBubbleParticle = v;}
    virtual CAniRuntimeBubble* GetAniRuntimeBubble() { return mpBubbleParticle;}
    
    void CampactAnimaion(int nTime);
    virtual bool SetRuntimeUpgrade(int nID);
    virtual void       SetState(SPRITE_STATE v);
    virtual void OnCompletedUpdated(RUNTIME_UPGRADE* prop,CArchive* pReadArc = NULL);
    virtual void ArchiveMultiplay(int nCmd,CArchive& ar,bool bWrite);
    //return -1:더이상 만들수가 없다. 0:mpMakingObject[0] 1:mpMakingObject[1]
    int GetCanMakingObjectIndex();
protected:
    CMS3DASCII *mpModel;
    sglMesh    *mpWorldMesh;
    float       mfRadian;
    float       mfRotationAngle;
    float       mfNowStatus;
    unsigned long    mnMakingTime;
    
    
    unsigned long   mnAniGGTime;
    float           mfAniBottomAngle;
    float           mfAniBottomY;
    float           mfAniBottomZ;
    float           mfAniHeaderAngle;
    float           mfAniHeaderY;
    float           mfAniHeaderX;
    
    CAniRuntimeBubble* mpBubbleParticle;
    int            mnConstMakingTime;
    
    //현재 만드는 슬롯은 두개만 가능하다.
    CSprite*         mpMakingObject[2];
    int              mnMakePosIndex;
};

#endif /* defined(__SongGL__CDMainTower__) */
