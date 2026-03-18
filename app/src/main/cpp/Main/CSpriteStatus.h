//
//  CSpriteStatus.h
//  SongGL
//
//  Created by 송 호학 on 11. 12. 23..
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef SongGL_CSpriteStatus_h
#define SongGL_CSpriteStatus_h
#include "sGL.h"
#include "ARender.h"


class CSprite;
class CSpriteStatus: public ARender
{
public:
    CSpriteStatus(CSprite* pSprite);
    virtual ~CSpriteStatus();
    virtual int Initialize(SPoint *pPosition,SVector *pvDirection);
	virtual int RenderBegin(int nTime);
	virtual int Render();
	virtual int RenderEnd();	
    virtual int OnEvent(SGLEvent *pEvent);
    virtual void RenderBeginCore(int nTime);

    //에너지바 말고 서브의 바
    virtual void SetSubBarRate(float fRate);
    virtual void SetIsSubBar(bool v) { mbIsSub = v;}

protected:
    float                       mfEnergeVertex[12];
    float                       mfOrgVertex[12];
    float                       mfRelealEnergeVertex[12];
    float                       mfEnergeColor[16];
    float                       mfRelealEnergeColor[16];
    float                       mfBarWidth;
    float                       mfBombIntervalColor[16];
    float                       mfBombIntervalVertex[12];
    
    CSprite*                    mpSprite;
    float                       mMatrix[16];
    float                       mMatrix2[16];
    float                       mMatrix3[16];
    
    float                       mRate2;
    float                       mRate3;

    bool                        mbIsSub;
};


#endif
