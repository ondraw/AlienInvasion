//
//  CProgressCtl.h
//  SongGL
//
//  Created by 송 호학 on 11. 12. 28..
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef SongGL_CImgProgressCtl_h
#define SongGL_CImgProgressCtl_h
#include "CButtonCtl.h"
//이미지에서 텍스쳐를 가져온다. (객체의 파워,스피드를 나타낼때 사용하였음)
class CImgProgressCtl : public CButtonCtl
{
public:
    CImgProgressCtl(CControl* pParent,CTextureMan *pTextureMan);
    virtual ~CImgProgressCtl();
    virtual CControl* Clone(CControl* pNew = NULL);
    
    
    virtual int Initialize(
                           int nControlID,
                           float x, float y, float nWidth,float nHeight,
                           vector<string>& lstImage,
                           float fsu,float fsv,float feu,float fev,
                           float fsu2,float fsv2,float feu2,float fev2
                           );
    virtual int Initialize(
                                       int nControlID,
                                       float x, float y, float nWidth,float nHeight,
                                       vector<string>& lstImage,
                                       float fsu,float fsv,float feu,float fev);
    
    
	virtual int RenderBegin(int nTime);
	virtual int Render();
	virtual int RenderEnd();	
    
    //0~100사이이다.
    void SetPosition(int nCurrentPos);
    int  GetPosition() { return mCurrentPos;}

protected:    
    float                       mfCurrentProVertex[12];
    float                       mGLWidth;
    int                         mCurrentPos;
    int                         mBeforeCurrentPos;

};


#endif
