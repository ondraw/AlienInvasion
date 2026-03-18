//
//  CTextureMan.h
//  SongGL
//
//  Created by Songs on 11. 5. 13..
//  Copyright 2011 thinkm. All rights reserved.
//
#ifndef _CTextureMan_H
#define _CTextureMan_H
#include <map>
#include <string>
#include "sGLDefine.h"
using namespace std;

typedef struct _sglTexture
{
    int             nWidth;
    int             nHeight;
    int             nDepth;
    unsigned char*  pImageBuffer;
    GLuint          glTextureID;
    
    int             nSType;
    int             nTType;
    //char            sTextureName[128];   //텍스쳐 맵 이름
    string            sTextureName2;
#ifdef ANDROID
    //Added By Song 2013.09.16 문자형태의 텍스쳐이라면 안드로이드에서 사용한다. ResetTexture 에서 분할하여 읽어온다.
    bool            bTextTexture; //문자형태의 텍스쳐이라면 안드로이드에서 사용한다.
#endif
}sglTexture;


class CTextureMan
{
public:
    CTextureMan();
    ~CTextureMan();
    int Clear();
    
    //GetTextureID와의 차이점은 없으면 그냥리턴한다.
    GLuint GetTextTextureID(const char* sFileName);
    
    // if nUMode == 0 then GL_CLAMP_TO_EDGE
    // if nVMode == 0 then GL_CLAMP_TO_EDGE
    GLuint GetTextureID(const char* sFileName,int nUMode = 0,int nVMode = 0,bool bReverse = true);
    
    //Text의 이미지를 바인드하고 값을 가져온다.
    //bool bDeleteImage 안드로이드의 자바에서 이미지를 넣어줄때는 이미지를 지우지 말자.
    GLuint GetTextureID(const char* sKeyName,unsigned char* sImage,int nWidth,int nHeight,int nDepth,int nUMode = 0,int nVMode = 0,bool bDeleteImage = true);
    
    GLuint MakeText(const char* sTextKey,float& fOutURate,float& fOutVRate);
    
    GLuint MakeBack(const char* sTextKey);
    

    //--------------------------------------------------------------------------------------
    //sgl 파일을 만들때 사용한다.
    //--------------------------------------------------------------------------------------
    //GID로 m_lstTextureMapdml 인덱스를 알아온다.
    int GetTextureIDRef(GLuint gID);
    //GID로 m_lstTextureMapdml 인덱스를 알아온다. (저장할때만 사용하고 실제는 GetTextureID함수를 사용한다.)
    int GetTextureIDRef(const char* sFileName);
    //--------------------------------------------------------------------------------------
    
    //--------------------------------------------------------------------------------------
    //sgl 파일을 읽을때 사용한다.
    //--------------------------------------------------------------------------------------    
    //인덱스로 GID 값을 가져온다.
    GLuint GetTextureIndex(int nIndex);
    //--------------------------------------------------------------------------------------   
     
    
	GLuint operator [](GLint index)
	{
        return GetTextureIndex(index);
	}
    
    int Save(FILE* fFile);
    int Load(FILE* fFile);
    
    void Remove(GLuint nTextureID);
    void Remove(const char* sKey);
    
    //PNG 데이터는 위아래가 바뀌어서 나오기 때문에 원본이미지를 꺼꾸로 읽어 들인다.
    static GLint LoadPNG(const char* const filename, sglTexture& texture,bool bReverse = true);
    static GLint LoadBMP(const char* const filename, sglTexture& texture);
    static GLint LoadTGA(const char* const filename, sglTexture& texture);
    static GLint SavePNG(const char* filename, int width, int height, unsigned char *buffer, const char* title);

#ifdef ANDROID
    //For Android Texture
    void Reset();
#endif //ANDROID

    
private:
    sglTexture* LoadImage(const char* sFileName,int nSType,int nTType,bool bPngReverse = true);
    
    //Text의 이미지를 가지고 직접 바인드한다.
    sglTexture* BindByImageBuffer(const char* sKeyName,unsigned char* sImage,int nWidth,int nHeight,int nDepth,int nSType,int nTType,bool bDeleteImage = true);
    
    static GLvoid SwapColorChannel(sglTexture& texture);
    
    

                     
private:
    map<string, sglTexture*>     m_lstTextureMap;
};

#endif //CTextureMan