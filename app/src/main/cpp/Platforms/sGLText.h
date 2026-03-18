//
//  sGLText.h
//  SongGL
//
//  Created by 호학 송 on 11. 8. 17..
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#ifndef SGLTEXT_H
#define SGLTEXT_H

#include "sGLDefine.h"
#include "CRollCtl.h"
#include <vector>
#include <string>
#include "CUserInfo.h"
using namespace std;

//스트링을 이미지로 변경한다.
#ifdef ANDROID
GLuint MakeStringToImageBuffer(void* pTextureMan,const char* sKey,const char* sData,int nImgWidth,int nImgHeight,float fFntSize, float fOffSetX,float fOffSetY,bool bBold = false);
#else
unsigned char* MakeStringToImageBuffer(char* sData,int nImgWidth,int nImgHeight,float fFntSize, float fOffSetX,float fOffSetY,bool bBold = false);
#endif
const char* GetText(const char* sKey);

#ifdef ANDROID
extern "C"
{
    GLuint GetTextureIDByJava(const char* sKey);
}
#endif

#endif //SGLTEXT_H