//
//  CItemCell.h
//  SongGL
//
//  Created by 송 호학 on 12. 1. 10..
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef SongGL_CItemCell_h
#define SongGL_CItemCell_h
#include "CCellCtl.h"
#include "CUserInfo.h"


class CItemCell : public CCellCtl
{
public:
    CItemCell(CTextureMan *pTextureMan);
    virtual ~CItemCell();
    int Initialize(USERINFO* pBombInfo,int nWidth,int nHeight,vector<string>& lstImage,float fsu,float fsv,float feu,float fev);
    virtual int RenderBegin(int nTime);
    

protected:
    USERINFO* mpBombInfo;
    int       mnBeforeCnt;

};

#endif
