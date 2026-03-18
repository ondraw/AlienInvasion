//
//  CBombCell.h
//  SongGL
//
//  Created by 송 호학 on 12. 1. 5..
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef SongGL_CBombCell_h
#define SongGL_CBombCell_h
#include "CCellCtl.h"
#include "CUserInfo.h"

class CBombCell : public CCellCtl
{
public:
    CBombCell(CTextureMan *pTextureMan);
    virtual ~CBombCell();
    int Initialize(USERINFO* pBombInfo,int nWidth,int nHeight,vector<string>& lstImage,float fsu,float fsv,float feu,float fev);
    virtual int RenderBegin(int nTime);
	
protected:
    USERINFO* mpBombInfo;
    int       mnBeforeCnt;
};

#endif
