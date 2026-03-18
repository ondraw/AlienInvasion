//
//  CDMainTowerStatus.h
//  SongGL
//
//  Created by 호학 송 on 13. 10. 29..
//
//

#ifndef __SongGL__CDMainTowerStatus__
#define __SongGL__CDMainTowerStatus__

#include "CSpriteStatus.h"
class CDMainTowerStatus : public CSpriteStatus
{
public:
    CDMainTowerStatus(CSprite* pSprite);
    virtual ~CDMainTowerStatus();
    virtual int Initialize(SPoint *pPosition,SVector *pvDirection);
	virtual int RenderBegin(int nTime);
    virtual int Render();
    virtual void RenderBeginCore(int nTime);
};

#endif /* defined(__SongGL__CDMainTowerStatus__) */
