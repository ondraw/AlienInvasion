//
//  sGLText.m
//  SongGL
//
//  Created by 호학 송 on 11. 8. 17..
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#include "sGLText.h"
#include <stdlib.h>
#include "sGL.h"
#include "SongGLJNI.h"
#include "CTextureMan.h"

#include <jni.h>
#include <android/bitmap.h>
#include "CLoader.h"

#include <string>
#include <map>
#include "CProperty.h"

using namespace std;

extern JNIEnv * gEnv;
extern jclass  gjNativesCls;
extern jmethodID  gjMDID;
extern JavaVM *gVM;
extern jlong glGLContext;
extern string gsLocalDir; //다중언어지원함.

bool LanguageTable(std::map<string,string>* pTable)
{
    string sPath = CLoader::getSearchPath();
    sPath.append(gsLocalDir);
    sPath.append("/Localizable.strings");
    char* sData = NULL;
    FILE* fp = fopen(sPath.c_str(), "rb");
    if (!fp)
    {
        HLogE("[Error] opening %s from LanguageTable\n", sPath.c_str());
        return NULL;
    }
    
    fseek(fp, 0L, SEEK_END);
    long sz = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    
    sData = new char[sz+1];
    fread(sData, sz, 1, fp);
    sData[sz] = 0;
    fclose(fp);
    
    //"Yes"               = "Yes#20.0#128#7.0#7.0#0.85#-1.0";
    char* sTemp = sData;
    char* sTok = NULL;
    sTok = strtok(sTemp, ";");
    while(sTok)
    {
        // " Find
        char* sFind = strstr(sTok,"\"");
        if(sFind == NULL)
        {
            sTok = strtok(NULL, "=");
            continue;
        }
        char* sKey = sFind + 1;
        sFind = strstr(sKey,"\"");
        *sFind = 0;
        sFind = strstr(sFind + 1,"\"");
        char* sValue = sFind + 1;
        sFind = strstr(sValue, "\"");
        *sFind = 0;
        (*pTable)[sKey] = sValue;
        //HLogD("%s=%s",sKey,sValue);
        sTok = strtok(NULL, ";");
    }
    
    delete[] sData;
    return true;
}


const char* GetText(const char* sKey)
{
//    static string s;
//    static map<string,string> LanTable;
//    
//    if(LanTable.size() == 0)
//    {
//        LanguageTable(&LanTable);
//    }
//    
//    s = LanTable[sKey];
//    
//    //Added By Song 2012.12.15 키값에 대한 데이터가 없으면 그대로 리턴한다.
//    if(s.length() == 0)
//    {
//        s = sKey;
//        return s.c_str();
//    }
//    
//    s.append("#");
//    s.append(SGLTextUnit::GetText(sKey));
// 
////    HLogD("GetText %s\n",s.c_str());
//    return s.c_str();
    return SGLTextUnit::GetText(sKey);
}

GLuint MakeStringToImageBuffer(void* pTextureMan,const char* sTextKey,const char* sData,int nImgWidth,int nImgHeight,float fFntSize, float fOffSetX,float fOffSetY,bool bBold)
{
    GLuint nResult = 0;
    jobject pBitmap = NULL;
    pBitmap = sglMakeBitmapWithText(sData,fFntSize,nImgWidth,nImgHeight,fOffSetX,fOffSetY,bBold);
    if(pBitmap)
    {
        unsigned char*              pixels;
        AndroidBitmapInfo  info;
        
        if ((nResult = AndroidBitmap_getInfo(gEnv, pBitmap, &info)) < 0) {
            HLogE("MakeStringToImageBuffer AndroidBitmap_getInfo() failed ! error=%d", nResult);
            sglClearBitmapJava(pBitmap);
            return 0;
        }
        if ((nResult = AndroidBitmap_lockPixels(gEnv, pBitmap, (void**)&pixels)) < 0) {
            HLogE("MakeStringToImageBuffer AndroidBitmap_lockPixels() failed ! error=%d", nResult);
            sglClearBitmapJava(pBitmap);
            return 0;
        }
        
        nResult = ((CTextureMan*)pTextureMan)->GetTextureID(sTextKey,pixels,info.width,info.height,32,0,0,false);

        AndroidBitmap_unlockPixels(gEnv, pBitmap);
        sglClearBitmapJava(pBitmap);
    }
    return nResult;
}


GLuint GetTextureIDByJava(const char* sKey)
{
    int nResult = 0;
    jobject pBitmap = NULL;
    pBitmap = sglGetTextureBitmap(sKey);
    if(pBitmap)
    {
        nResult = sglGetTextureSub(gEnv,glGLContext,sKey,pBitmap);
        sglClearBitmapJava(pBitmap);
    }
    return nResult;
}