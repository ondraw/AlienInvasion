//
//  CFighterMoveAI.h
//  SongGL
//
//  Created by itsme on 2013. 12. 17..
//
//

#ifndef __SongGL__CFighterMoveAI__
#define __SongGL__CFighterMoveAI__
#include "sGLDefine.h"
#include "AAI.h"

class CSprite;
class CFighterMoveAI : public AAI {
public:
    CFighterMoveAI(CSprite *pSprite);
    virtual ~CFighterMoveAI();
    
public:
    virtual void RenderBeginCore(int nTime);
    virtual bool GetDonComact();
    //적탱크로 이동한다.
    virtual void Command(int nCommandID,void *pData);
    virtual CSprite* GetAttackTo() { return 0;}
    
    
    //
    virtual bool IsMoveTo();// { return mpMoveToSprite? true : false;}
    virtual void SetMoveTo(CSprite* v);// { mpMoveToSprite = v;}
    void ChainCourse(int nTime);
    void ArchiveMultiplay(int nCmd,CArchive& ar,bool bWrite);
    
protected:
    CSprite* mpSprite;
    CSprite* mpMoveToSprite;
    
    float mfGuideDirectionX;
    float mfGuideDirectionY;
    
    bool  mBeforeUp;
};

#endif /* defined(__SongGL__CFighterMoveAI__) */
