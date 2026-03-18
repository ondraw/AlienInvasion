//
//  CFighterTailParticle.h
//  SongGL
//
//  Created by itsme on 2013. 12. 17..
//
//

#ifndef __SongGL__CFighterTailParticle__
#define __SongGL__CFighterTailParticle__

#include "CBombTailParticle.h"

class CFighterTailParticle : public CBombTailParticle
{
public:
    CFighterTailParticle(IHWorld* pWorld,CSprite* pSprite,int nIndex);
    virtual ~CFighterTailParticle();
    virtual int Initialize(SPoint *pPosition,SVector *pvDirection);
    virtual void RenderBeginCore(int nTime);
    virtual int RenderBegin(int nTime);
    virtual void    SetVisibleRender(bool v) { mbVisible = v;}
    
#ifdef ANDROID
    virtual void ResetTexture();
#endif
    
    virtual int Render();
protected:
    int mIndex;
    bool mbVisible;
};

#endif /* defined(__SongGL__CFighterTailParticle__) */
