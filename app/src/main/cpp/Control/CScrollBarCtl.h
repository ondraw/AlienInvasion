//
//  CScrollBarCtl.h
//  SongGL
//
//  Created by 송 호학 on 11. 12. 5..
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef SongGL_CScrollBarCtl_h
#define SongGL_CScrollBarCtl_h
#include "CLabelCtl.h"

#define SCROLL_WIDTH 5
#define SCROLL_HEIGHT 80

enum SCROLLBAR_TYPE{ SCROLLBAR_HOR,SCROLLBAR_VER};

class CScrollBarCtl : public CLabelCtl
{
public:
    CScrollBarCtl(CControl* pParent,CTextureMan *pTextureMan);
    virtual ~CScrollBarCtl();
    
    virtual int RenderBegin(int nTime);
	virtual int Render();
	virtual int RenderEnd();   
    
    void SetBarType(SCROLLBAR_TYPE type) { mType = type;}
    int Initialize(SCROLLBAR_TYPE type,CControl* pContentCtl,float x, float y, vector<string>& lstImage,float fsu,float fsv,float feu,float fev);
    void SetPos(float fVectorX,float fVectorY);
    
protected:
    virtual int Initialize(int nControlID,float x, float y, float nWidth,float nHeight,vector<string>& lstImage,float fsu,float fsv,float feu,float fev);
protected:
    SCROLLBAR_TYPE mType;
    CControl* mpContentCtl;

    
};


#endif
