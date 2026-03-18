//
//  CTextureMan.cpp
//  SongGL
//
//  Created by Songs on 11. 5. 13..
//  Copyright 2011 thinkm. All rights reserved.
//

extern "C" 
{
#include <png.h>
}
#include <stdio.h>
#include "CTextureMan.h"
#include "sGLUtils.h"
#include <stdlib.h>
#include "CLoader.h"
#include "sGLText.h"
#include "sGL.h"



#define ERROR_PNG -1;

void ClearsglTexture(sglTexture* pTexture)
{
    pTexture->nWidth = 0;
    pTexture->nHeight = 0;
    pTexture->nDepth = 0;
    pTexture->pImageBuffer = 0;
    pTexture->glTextureID = 0;
    
    pTexture->nSType = 0;
    pTexture->nTType = 0;
    //char            sTextureName[128];   //텍스쳐 맵 이름
    pTexture->sTextureName2 = "";
#ifdef ANDROID
    pTexture->bTextTexture = false;
#endif
}

CTextureMan::CTextureMan()
{
    
}

CTextureMan::~CTextureMan()
{
    
    map<string,sglTexture*>::iterator p;
    for (p = m_lstTextureMap.begin(); p != m_lstTextureMap.end(); ++p)
    {
        sglTexture *pTexture = p->second;
        if(pTexture)
        {
            if(pTexture->pImageBuffer) 
                //delete[] pTexture->pImageBuffer;
                free(pTexture->pImageBuffer);
            
            glDeleteTextures(1, &pTexture->glTextureID);
            delete pTexture; 
        }
    }
    
}

int CTextureMan::Clear()
{
    map<string,sglTexture*>::iterator p;
    for (p = m_lstTextureMap.begin(); p != m_lstTextureMap.end(); ++p)
    {
        sglTexture *pTexture = p->second;
        if(pTexture)
        {
            if(pTexture->pImageBuffer) 
            {
                free(pTexture->pImageBuffer);
                pTexture->pImageBuffer = NULL;
            }
            glDeleteTextures(1, &pTexture->glTextureID);
            delete pTexture; 
        }
    }
    m_lstTextureMap.clear();
    return 0;
}

#ifdef ANDROID
//For Android Texture
void CTextureMan::Reset()
{
    vector<string> lstTemp;
    vector<bool>   lstIsText;
    
    map<string,sglTexture*>::iterator p;
    for (p = m_lstTextureMap.begin(); p != m_lstTextureMap.end(); ++p)
    {
        if(p->second)
        {
            lstTemp.push_back(p->first);
            lstIsText.push_back(p->second->bTextTexture);
        }
        else
        {
            HLogE("Reset Texture p->first %s p->second is null",p->first.c_str());
        }
    }
    
    //현재 할당된 gl리소스를 해제한다.
    Clear();

    
    vector<string>::iterator b = lstTemp.begin();
    vector<bool>::iterator b2 = lstIsText.begin();
    vector<bool>::iterator it2 = b2;
    for (vector<string>::iterator it = b;
         it != lstTemp.end();
         it++,it2++)
    {
        //파일속성
        //if(strstr(it->c_str(), "."))
        if(*it2 == false)
            CTextureMan::GetTextureID(it->c_str());
        else //텍스쳐속성.
        {
            float f;
            float f2;
            CTextureMan::MakeText(it->c_str(),f,f2);
        }
    }
}
#endif //ANDROID

void CTextureMan::Remove(GLuint nTextureID)
{
    string sKey;
    map<string,sglTexture*>::iterator p;
    for (p = m_lstTextureMap.begin(); p != m_lstTextureMap.end(); ++p)
    {
        
        sglTexture *pTexture = p->second;
        if(pTexture)
        {
            if(pTexture->glTextureID == nTextureID)
            {
                sKey = pTexture->sTextureName2;
                break;
            }
        }
    }
    if(sKey.length() != 0)
    {
        Remove(sKey.c_str());
    }
}
void CTextureMan::Remove(const char* sKey)
{
    map<string,sglTexture*>::iterator p = m_lstTextureMap.find(sKey);
    if(p != m_lstTextureMap.end())
    {
        
        sglTexture *pTexture = p->second;
        if(pTexture)
        {
            if(pTexture->pImageBuffer) 
                //delete[] pTexture->pImageBuffer;
                free(pTexture->pImageBuffer);
            glDeleteTextures(1, &pTexture->glTextureID);
            delete pTexture;
        }
        
        m_lstTextureMap.erase(p);
    }
}

int CTextureMan::Load(FILE* fFile)
{
    char sText[20];
    
    int nSize;
    fread(&nSize, 1, sizeof( int), fFile); 
    for(int i = 0; i < nSize; i++)
    {
        sglTexture *pTexture = new sglTexture;
//        memset(pTexture,0,sizeof(sglTexture));
        ClearsglTexture(pTexture);
        
        fread(&pTexture->nWidth, 1, sizeof( int), fFile);  
        fread(&pTexture->nHeight, 1, sizeof( int), fFile);  
        fread(&pTexture->nDepth, 1, sizeof( int), fFile);  
        
        int nSize = pTexture->nWidth * pTexture->nHeight * pTexture->nDepth / 8;
        //pTexture->pImageBuffer = new unsigned char[nSize];
        pTexture->pImageBuffer = (unsigned char*)malloc(nSize);
        fread(pTexture->pImageBuffer, 1, nSize, fFile);  
        
        fread(&pTexture->nSType, 1, sizeof( int), fFile);  
        fread(&pTexture->nTType, 1, sizeof( int), fFile);  
//        fread(&pTexture->sTextureName, 1, sizeof( char) * 20, fFile);
        memset(sText, 0, 20);
        fread(sText, 1, sizeof( char) * 20, fFile);
        sText[19] = 0;
        pTexture->sTextureName2 = sText;
        
        
        
        glGenTextures(1, &pTexture->glTextureID);
        glBindTexture(GL_TEXTURE_2D, pTexture->glTextureID);
        
        //축소시 가장가까운 픽셀을 참조하여 축소를 한다.
        //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        //GL_NEAREST 이미지가 고르게 보이지 않고 가장자리가 거칠어서 GL_LINEAR로 하니 부드럽게 변경되었다.
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        
        //확대시 선형으로 확대를 하여 고르게 한다.
        glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,pTexture->nSType);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,pTexture->nTType);   

        if(pTexture->nDepth == 24)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 
                         pTexture->nWidth, pTexture->nHeight, 
                         0, GL_RGB, GL_UNSIGNED_BYTE, pTexture->pImageBuffer);
        }
        else if(pTexture->nDepth == 32)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 
                         pTexture->nWidth, pTexture->nHeight, 
                         0, GL_RGBA, GL_UNSIGNED_BYTE, pTexture->pImageBuffer);
        }
        else //24 비트와 32비트만 지원한다.
        {
            glDeleteTextures(1, &pTexture->glTextureID);
            //delete[] pTexture->pImageBuffer;
            free(pTexture->pImageBuffer);
            pTexture->pImageBuffer = NULL;
            return 0;
        }
        
        //---------------------------------
        //버퍼를 지운다.
        //delete[] pTexture->pImageBuffer;
        free(pTexture->pImageBuffer);
        pTexture->pImageBuffer = 0;
        //---------------------------------
        
        string sKey = pTexture->sTextureName2;
        m_lstTextureMap[sKey] = pTexture;
    }
    return E_SUCCESS;
}

int CTextureMan::Save(FILE* fFile)
{
    //fwrite(&m_cGType, 1, sizeof( unsigned char), fFile);

    int nSize = (int)m_lstTextureMap.size();
    
    
    
    fwrite(&nSize, 1, sizeof( int), fFile); 
    
    map<string,sglTexture*>::iterator p;
    char sT[20];
    for (p = m_lstTextureMap.begin(); p != m_lstTextureMap.end(); ++p)
    {
        sglTexture *pTexture = p->second;
        
        fwrite(&pTexture->nWidth, 1, sizeof( int), fFile);  
        fwrite(&pTexture->nHeight, 1, sizeof( int), fFile);  
        fwrite(&pTexture->nDepth, 1, sizeof( int), fFile);  
        
        int nSize = pTexture->nWidth * pTexture->nHeight * pTexture->nDepth / 8;
        fwrite(pTexture->pImageBuffer, 1, nSize, fFile);  
        
        fwrite(&pTexture->nSType, 1, sizeof( int), fFile);  
        fwrite(&pTexture->nTType, 1, sizeof( int), fFile);
        if(pTexture->sTextureName2.length() > 20)
        {
            HLogE("Error Length is 20 over , %s\n",pTexture->sTextureName2.c_str());
        }
        strcpy(sT,pTexture->sTextureName2.c_str());
        fwrite(sT, 1, sizeof( char) * 20, fFile);
    }
    
    
    return E_SUCCESS;
}

int CTextureMan::GetTextureIDRef(GLuint gID)
{
    //fwrite(&m_cGType, 1, sizeof( unsigned char), fFile);
    
//    int nSize = lst.size();
//    for(int i = 0; i < nSize; i++)
//    {
//        sglTexture *pTexture = lst[i];
//        if(pTexture->glTextureID == gID)
//        {
//            return i;
//        }
//    }
    
    int i = 0;
    map<string,sglTexture*>::iterator p;
    for (p = m_lstTextureMap.begin(); p != m_lstTextureMap.end(); ++p)
    {
        sglTexture *pTexture = p->second;
        if(pTexture->glTextureID == gID)
        {
            return i;
        }
        i++;
    }
    
    return -1;
}

int CTextureMan::GetTextureIDRef(const char* sFileName)
{
    sglTexture *pTexture = NULL;
    string sKey = sFileName;
    
    
    if(m_lstTextureMap.find(sKey) != m_lstTextureMap.end())
    {
        pTexture = m_lstTextureMap[sKey];
        return GetTextureIDRef(pTexture->glTextureID);
    }    
    return -1;
}


GLuint CTextureMan::GetTextureIndex(int nIndex)
{
    int i = 0;
    map<string,sglTexture*>::iterator p;
    for (p = m_lstTextureMap.begin(); p != m_lstTextureMap.end(); ++p)
    {
        sglTexture *pTexture = p->second;
        if(i == nIndex)
        {
            return pTexture->glTextureID;
        }
        i++;
    }
    return -1;
}

GLuint CTextureMan::GetTextTextureID(const char* sFileName)
{
    if (m_lstTextureMap.find(sFileName) != m_lstTextureMap.end()) 
    {
        sglTexture *pTexture = m_lstTextureMap[sFileName];
#ifdef DEBUG
        if(pTexture->glTextureID == 0)
        {
            HLogE("Not Bounding Image %s",sFileName);
        }
#endif
        if(pTexture)
            return pTexture->glTextureID;
    }
    return 0;
}



GLuint CTextureMan::GetTextureID(const char* sFileName,int nUMode,int nVMode,bool bReverse)
{ 
    int nSType = 0,nTType = 0;
    
    sglTexture *pTexture = NULL;
    string sKey = sFileName;
    
    //일반 모드와 UV모드는 틀리다.
    if(nUMode != 0 || nVMode != 0)
    {
        sKey += "UV";
    }


    pTexture = m_lstTextureMap[sKey];
    
    if(pTexture)
    {
        return pTexture->glTextureID;
    }
    if(nUMode == 0)
        nSType = GL_CLAMP_TO_EDGE; //경계선을 고르게 한다.
    else
    {
//#ifdef MAC
//        nSType = GL_MIRRORED_REPEAT;
//#else
//        nSType = GL_MIRRORED_REPEAT_OES;
//#endif
        nSType = GL_REPEAT;
    }
    
    if(nVMode == 0)
        nTType = GL_CLAMP_TO_EDGE;
    else
    {
//#ifdef MAC
//        nTType = GL_MIRRORED_REPEAT;
//#else
//        nTType = GL_MIRRORED_REPEAT_OES;
//#endif
        nSType = GL_REPEAT;
    }
    
    pTexture = LoadImage(sFileName,nSType,nTType,bReverse);
    if(pTexture == NULL) return 0;
    
    m_lstTextureMap[sKey] = pTexture;
    return pTexture->glTextureID;
}


GLuint CTextureMan::GetTextureID(const char* sKeyName,unsigned char* sImage,int nWidth,int nHeight,int nDepth,int nUMode,int nVMode,bool bDeleteImage)
{
    int nSType = 0,nTType = 0;
    
    sglTexture *pTexture = NULL;
    string sKey = sKeyName;
    
    pTexture = m_lstTextureMap[sKey];
    if(pTexture)
    {
        return pTexture->glTextureID;
    }
    
    if(nUMode == 0)
        nSType = GL_CLAMP_TO_EDGE; //경계선을 고르게 한다.
    else
    {
        nSType = GL_REPEAT;
    }
    
    if(nVMode == 0)
        nTType = GL_CLAMP_TO_EDGE;
    else
    {
        nSType = GL_REPEAT;
    }
    
    pTexture = BindByImageBuffer(sKeyName,sImage,nWidth,nHeight,nDepth,nSType,nTType,bDeleteImage);
    if(pTexture == NULL) return 0;
    m_lstTextureMap[sKey] = pTexture;
    return pTexture->glTextureID;
}

// 텍스트#텍트쳐(U1.0)#텍스쳐V(1.0)
GLuint CTextureMan::MakeBack(const char* sTextKey)
{
    return GetTextureID(sTextKey);
    
//    GLuint nResult = 0;
//    fOutURate = 1.0f;
//    fOutVRate = 1.0f;
//    char * pTemp;
//    char * pOrgData = NULL;
//    //char * pS = new char[strlen(lstImage[0].c_str()) + 1];
//    char * pS = new char[strlen(sTextKey) + 1];
//    strcpy(pS, sTextKey);
//    pOrgData = pS;
//    pTemp = strtok(pS, "#");
//    if(pTemp)
//    {
//        
//        nResult = GetTextureID(pTemp);
//        
//        pTemp = strtok(NULL, "#");
//        if(pTemp)
//        {
//            fOutURate = atof(pTemp);
//            pTemp = strtok(NULL, "#");
//            if(pTemp)
//            {
//                fOutVRate = atof(pTemp);
//            }                
//        }
//    }
//    delete[] pOrgData;
//    return nResult;

}

// 텍스트#Font#이미지사이즈(200(200*200))#텍트쳐(U1.0)#텍스쳐V(1.0)#OffsetX#OffsetY
//"Menu_Select" = "Select#20.0#200#1.0#1.0#1.0#2.0"; 
GLuint CTextureMan::MakeText(const char* sTextKey,float& fOutURate,float& fOutVRate)
{
    GLuint nResult = 0;
    float fOffsetX = 0.f,fOffsetY = 0.f;
    int   nImageWidth = 32;
    int   nImageHeight = 0;
    float fFontSize = 10;
    char* sTextV = 0;
    char* sToken = 0;
    char* sOrgToken = 0;
    char* sTemp;
    int nIndex = 0;
    unsigned char *pImage = 0;
    bool bBold = false;
    float fScale = sglGetDisplayScale();
    
    const char* sText = GetText(sTextKey);
    sToken = new char[strlen(sText) + 1];
    strcpy(sToken, sText);
    sOrgToken = sToken;
    
    sTemp = strtok(sOrgToken, "#");
    while (sTemp) 
    {
        if(nIndex == 0) //Text
        {
            sTextV = new char[strlen(sTemp) + 1];
            strcpy(sTextV, sTemp);
        }
        else if(nIndex == 1) //FontSize
        {
            fFontSize = atof(sTemp) * fScale;
        }
        else if(nIndex == 2) //ImageSize (nWidth * nHeight)
        {
            nImageWidth = atoi(sTemp) * fScale;
        }
        else if(nIndex == 3) //Offset x
        {
            fOffsetX = atof(sTemp) * fScale;
        }
        else if(nIndex == 4) //Offset y
        {
            fOffsetY = atof(sTemp) * fScale;
            
        }
        else if(nIndex == 5) //U
        {
            fOutURate = atof(sTemp);
        }
        else if(nIndex == 6) //V
        {
            fOutVRate = atof(sTemp);
        }
        else if(nIndex == 7) //nHeight Added By Song 2012.12.21 확장했다. 그래서 뒤에 있는 것임.
        {
            nImageHeight = atoi(sTemp) * fScale;
            
        }
        else if(nIndex == 8) //nHeight Added By Song 2013.3.14 확장했다. 그래서 뒤에 있는 것임.
        {
            if(*sTemp == 'b' || *sTemp == 'B')
            {
                bBold = true;
            }
            
        }
        sTemp = strtok(0, "#");
        nIndex ++;
    }
    delete[] sToken;
    
    //if(nIndex != 6) return 0;
    
    
    if(fOutURate == -1.0)//자동계산.
        fOutURate = 1.0f;
    
    if(fOutVRate == -1.0) //자동계산
    {
#ifdef ANDROID
        float fSz = nImageWidth;
        if(nImageWidth == 32)
            fSz = nImageWidth;
        else if(nImageWidth == 64)
            fSz = fFontSize + 2;
        else
            fSz = fFontSize + 10;
#else
        float fSz = fFontSize - (fFontSize / 6.0f) + fOffsetY * 2.0f;
#endif //ANDROID
        if(nImageWidth >= fSz)
            fOutVRate = fSz / (float)nImageWidth;
    }
    
    nResult = GetTextTextureID(sTextKey);
    if(nResult != 0)   
    {
        if(sTextV)    delete[] sTextV;
        sTextV = 0;
        return nResult;
    }
    
    if(nImageHeight == 0)
        nImageHeight = nImageWidth;
    
#ifdef ANDROID
    //내부에서 텍스쳐를 대입한다.
    if(bBold == false)
        nResult = MakeStringToImageBuffer(this,sTextKey,sTextV,nImageWidth,nImageHeight,fFontSize,fOffsetX,fOffsetY,bBold);
    else
        nResult = MakeStringToImageBuffer(this,sTextKey,*sTextV != 'b' ? sTextV : sTextV + 1,nImageWidth,nImageHeight,fFontSize,fOffsetX,fOffsetY,bBold);
    if(sTextV)    delete[] sTextV;
#else
    
    if(bBold == false)
        pImage = MakeStringToImageBuffer(bBold == false ? sTextV : sTextV + 1,nImageWidth,nImageHeight,fFontSize,fOffsetX,fOffsetY,bBold);
    else
    {
        //볼드의데이터의 Number 값은 b0,b2 이렇게 되기 때문에 하나 줄인다.
        pImage = MakeStringToImageBuffer(*sTextV != 'b'? sTextV : sTextV + 1,nImageWidth,nImageHeight,fFontSize,fOffsetX,fOffsetY,bBold);
    }
    
    //    GetTextureID 안에서 메모리를 지운다.
    //    if(pImage)
    //    {
    //        free(pImage);
    //    }
   
    if(sTextV)    delete[] sTextV;
    if(pImage)
    {
        nResult = GetTextureID(sTextKey,pImage,nImageWidth,nImageHeight,32);
    }
#endif
    return nResult;
    
}

GLvoid CTextureMan::SwapColorChannel(sglTexture& texture)
{
	GLubyte temp;
	GLubyte bytesPerPixel = 3;
	
	if (texture.nDepth == 32)
		bytesPerPixel = 4;
	
	// swap the R and B values to get RGB since the bitmap color format is in BGR
	for (GLuint i = 0; i < texture.nWidth*texture.nHeight*bytesPerPixel; i+=bytesPerPixel)
	{
		temp = texture.pImageBuffer[i];
		texture.pImageBuffer[i] = texture.pImageBuffer[i+2];
		texture.pImageBuffer[i+2] = temp;
	}
}



//Taken from http://en.wikibooks.org/wiki/OpenGL_Programming/Intermediate/Textures
/** loadTexture
 *   loads a png file into an opengl texture object, using cstdio , libpng, and opengl.
 *
 *   \param filename : the png file to be loaded
 *   \param width : width of png, to be updated as a side effect of this function
 *   \param height : height of png, to be updated as a side effect of this function
 *
 *   \return GLuint : an opengl texture id.  Will be 0 if there is a major error,
 *           should be validated by the client of this function.
 *
 */

void png_zip_read(png_structp png_ptr, png_bytep data, png_size_t length) {
    
    png_size_t check = 0;
    png_voidp io_ptr;
    io_ptr = png_get_io_ptr(png_ptr);
    
    if (io_ptr != NULL)
    {
        //READFILE((png_FILE_p)io_ptr, data, length, check);   
        check=(png_size_t)fread(data, (png_size_t)1, length, (png_FILE_p)io_ptr);
    }
    
    if (check != length)
    {
        png_error(png_ptr, "Read Error!");
    }
    
    //zip_fread(file, data, length);
}

inline void setRGB(png_byte *ptr, float val)
{
	int v = (int)(val * 768);
	if (v < 0) v = 0;
	if (v > 768) v = 768;
	int offset = v % 256;
    
	if (v<256) {
		ptr[0] = 0; ptr[1] = 0; ptr[2] = offset;
	}
	else if (v<512) {
		ptr[0] = 0; ptr[1] = offset; ptr[2] = 255-offset;
	}
	else {
		ptr[0] = offset; ptr[1] = 255-offset; ptr[2] = 0;
	}
}

GLint CTextureMan::SavePNG(const char* filename, int width, int height, unsigned char *buffer, const char* title)
{
    int code = 0;
    FILE *fp;
    png_structp png_ptr;
    png_infop info_ptr;
    png_bytep row;
    int rowbytes = 0;
    
    // Open file for writing (binary mode)
    fp = fopen(filename, "wb");
    if (fp == NULL) {
        fprintf(stderr, "Could not open file %s for writing\n", filename);
        code = -1;
        goto finalise;
    }
    
    // Initialize write structure
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        fprintf(stderr, "Could not allocate write struct\n");
        code = -1;
        goto finalise;
    }
    
    // Initialize info structure
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        fprintf(stderr, "Could not allocate info struct\n");
        code = -1;
        goto finalise;
    }
    
    
    // Setup Exception handling
    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "Error during png creation\n");
        code = -1;
        goto finalise;
    }
    
    png_init_io(png_ptr, fp);
    
    // Write header (8 bit colour depth)
    png_set_IHDR(png_ptr, info_ptr, width, height,
                 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    
    // Set title
    if (title != NULL) {
        png_text title_text;
        title_text.compression = PNG_TEXT_COMPRESSION_NONE;
        title_text.key = new char[strlen("Title") + 1];
        strcpy(title_text.key, "Title");
        
        title_text.text = new char[strlen(title) + 1];
        strcpy(title_text.text, title);

        
        png_set_text(png_ptr, info_ptr, &title_text, 1);
        delete [] title_text.key;
        delete [] title_text.text;
    }
    
    png_write_info(png_ptr, info_ptr);
    
    // Allocate memory for one row (3 bytes per pixel - RGB)
    rowbytes = 3 * width * sizeof(png_byte);
    row = (png_bytep) malloc(rowbytes);
    
    // Write image data
//    int x, y;
//    for (y=0 ; y<height ; y++) {
//        for (x=0 ; x<width ; x++) {
//            setRGB(&(row[x*3]), buffer[y*width + x]);
//        }
//        
//        
//        png_write_row(png_ptr, row);
//    }
    
    for (int i = 0;  i < height;  ++i)
    {
        memcpy(row,buffer + i * rowbytes,rowbytes);
        png_write_row(png_ptr, row);
    }
    
    
    // End write
    png_write_end(png_ptr, NULL);
    
finalise:
    if (fp != NULL) fclose(fp);
    if (info_ptr != NULL) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
    if (png_ptr != NULL) png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
    if (row != NULL) free(row);
    
    return code;
    
}
    

GLint CTextureMan::LoadPNG(const char* const filename, sglTexture& texture,bool bReverse)
{
    png_byte        pbSig[8];
    
    long width;
    long height;
    FILE* fp = fopen(filename, "rb");
    if (!fp) 
    {
        HLogE("[Error] opening %s from png\n", filename);
        return ERROR_PNG;
    }
    
    
    fread(pbSig, 1, 8, fp);
    if(!png_check_sig(pbSig, 8)) 
    {
        HLogE("[Error] Not PNG %s from png\n", filename);
        return ERROR_PNG;
    }
    
    //create png struct
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
                                                 NULL, NULL);
    if (!png_ptr) {
        fclose(fp);
        HLogE("[Error] Unable to create png struct : %s\n", filename);
        return ERROR_PNG;
    }
    
    //create png info struct
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
        HLogE("[Error] Unable to create png info : %s\n", filename);
        fclose(fp);
        return ERROR_PNG;
    }
    
    //create png info struct
    png_infop end_info = png_create_info_struct(png_ptr);
    if (!end_info) {
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
        HLogE("[Error] Unable to create png end info : %s\n", filename);
        fclose(fp);
        return ERROR_PNG;
    }
    
    //png error stuff, not sure libpng man suggests this.
    if (setjmp(png_jmpbuf(png_ptr))) {
        fclose(fp);
        HLogE("[Error] during setjmp : %s\n", filename);
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        return ERROR_PNG;
    }
    
    //init png reading
    png_init_io(png_ptr, fp);
    //png_set_read_fn(png_ptr, NULL, png_zip_read);
    
    //let libpng know you already read the first 8 bytes
    png_set_sig_bytes(png_ptr, 8);
    
    // read all the info up to the image data
    png_read_info(png_ptr, info_ptr);
    
    //variables to pass to get info
    int bit_depth, color_type;
    png_uint_32 twidth, theight;
    
    // get info about png
    png_get_IHDR(png_ptr, info_ptr, &twidth, &theight, &bit_depth, &color_type,
                 NULL, NULL, NULL);
    
    //update width and height based on png info
    width = twidth;
    height = theight;
    
    // Update the png info struct.
    png_read_update_info(png_ptr, info_ptr);
    
    // Row size in bytes.
    long rowbytes = png_get_rowbytes(png_ptr, info_ptr);
    
    // Allocate the image_data as a big block, to be given to opengl
    png_byte *image_data = (png_byte*)malloc(rowbytes * height);//new png_byte[rowbytes * height];
    if (!image_data) {
        //clean up memory and close stuff
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        HLogE("[Error] Unable to allocate image_data while loading %s\n", filename);
        fclose(fp);
        return ERROR_PNG;
    }
    
    //row_pointers is for pointing to image_data for reading the png with libpng
    png_bytep *row_pointers = new png_bytep[height];
    if (!row_pointers) {
        //clean up memory and close stuff
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        //delete[] image_data;
        free(image_data);
        HLogE("[Error] Unable to allocate row_pointer while loading %s\n", filename);
        fclose(fp);
        return ERROR_PNG;
    }
    
    if(bReverse)
    {
        // set the individual row_pointers to point at the correct offsets of image_data
        for (int i = 0; i < height; i++)
            row_pointers[height - 1 - i] = image_data + i * rowbytes;
    }
    else
    {
        for (int i = 0;  i < height;  ++i)
            row_pointers[i] = image_data + i * rowbytes;
    }
    
    
    //read the png into image_data through row_pointers
    png_read_image(png_ptr, row_pointers);
    
    texture.nDepth = info_ptr->pixel_depth;
    
    //clean up memory and close stuff
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    
    //free(image_data);
    texture.nWidth = (int)width;
    texture.nHeight = (int)height;
    texture.pImageBuffer = image_data;
    
    if(!(texture.nDepth == 32 || texture.nDepth == 24 || texture.nDepth == 16))
    {
        //clean up memory and close stuff
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        //delete[] image_data;
        free(image_data);
        HLogE("[Error] color bit invalide %s\n", filename);
        fclose(fp);
        return ERROR_PNG;
    }
    
    delete[] row_pointers;
    fclose(fp);
    return 0;
}


GLint CTextureMan::LoadTGA(const char* const filename, sglTexture& texture)
{
	FILE*				file;
	GLubyte				type;

//    memset(&texture, 0, sizeof(sglTexture));
    ClearsglTexture(&texture);
    
	// check, if we have a valid pointer
	if (!filename)
		return -1;
    
	// open filename in "read binary" mode
	file = fopen(filename, "rb");
	if (!file)
    {
        HLogE("[Error] loadTGA Not found %s\n",filename);
		return -2;
    }
    
 	// seek through the tga header, up to the type:
 	fseek(file, 2, SEEK_CUR);
    
 	// read the type
 	fread(&type, 1, 1, file);
    
	// check the type	
	if (type != 2 && type != 10 && type != 11)
	{
        HLogE("[Error] loadTGA Not found %s type=%d\n",filename,type);
		fclose(file);
		return -3;
	}
    
 	// seek through the tga header, up to the width/height:
 	fseek(file, 9, SEEK_CUR);
	
 	// read the width
 	fread(&texture.nWidth, 2, 1, file);
    
 	// read the height 
 	fread(&texture.nHeight, 2, 1, file);
    
    
	// read the bits per pixel
	fread(&texture.nDepth, 1, 1, file);
    
	// check the pixel depth
	if (texture.nDepth != 8 && texture.nDepth != 24 && texture.nDepth != 32)
	{
        HLogE("[Error] loadTGA Not found %s nDepth = %d\n",filename,texture.nDepth);
        fclose(file);
		return -4;
	}
    
    int nPicByte = texture.nDepth/8;
    
    	// move file pointer to beginning of targa  data
	fseek(file, 1, SEEK_CUR);
    
	// allocate enough memory for the targa  data
	texture.pImageBuffer = (unsigned char*)malloc(texture.nWidth*texture.nHeight*nPicByte);
    
	// verify memory allocation
	if (!texture.pImageBuffer)
	{
		HLogE("[Error] loadTGA Not found %s memory\n",filename);
		fclose(file);
		return -5;
	}
    
	if (type == 2)
	{
		// read in the raw data
		fread(texture.pImageBuffer, 1, texture.nWidth*texture.nHeight*nPicByte, file);
	}
	else
	{
		// RLE encoded
		GLuint pixelsRead = 0;
        int nSize = texture.nWidth*texture.nHeight;
        
        
		while (pixelsRead < nSize)
		{
			GLubyte amount;
            
			fread(&amount, 1, 1, file);
            
			if (amount & 0x80)
			{
				amount&=0x7F;
                
				amount++;
                
				// read in the rle data
				fread(&texture.pImageBuffer[pixelsRead*nPicByte], 1, nPicByte, file);
                
				for (GLuint i = 1; i < amount; i++)
				{
					for (GLuint k = 0; k < (GLuint)nPicByte; k++)
					{
						texture.pImageBuffer[(pixelsRead+i)*nPicByte+k] = texture.pImageBuffer[pixelsRead*nPicByte+k];
					}
				}
			}
			else
			{
				amount&=0x7F;
                
				amount++;
				
				// read in the raw data
				fread(&texture.pImageBuffer[pixelsRead*nPicByte], 1, (GLuint)amount*nPicByte, file);				
			}
            
			pixelsRead += amount;
		}
	}
    
	// swap the color if necessary
	if (texture.nDepth == 24 || texture.nDepth == 32)
	{
		SwapColorChannel(texture);
	}
    
	// close the file
	fclose(file);
	
	return 0;
}


GLint CTextureMan::LoadBMP(const char* const filename, sglTexture& texture)
{
	FILE*				file;
	GLushort			type;
	GLuint				offset;		
	GLuint				compression;
//    memset(&texture, 0, sizeof(sglTexture));
    ClearsglTexture(&texture);
    
	// check, if we have a valid pointer
	if (!filename)
		return -2;
    
    
	// open filename in "read binary" mode
	file = fopen(filename, "rb");
	if (!file)
    {
        HLogE("[Error] loadBMP Not found %s\n",filename);
		return -1;
    }
    
 	// read the type
 	fread(&type, 2, 1, file);
    
	// check the type	
	if (type != 0x4D42)
	{
		fclose(file);
        HLogE("[Error] loadBMP Not found %s type=%d\n",filename,type);
		return -2;
	}
    
 	// seek through the bmp header, up to the width/height:
 	fseek(file, 8, SEEK_CUR);
    
 	// read the offset
 	fread(&offset, 4, 1, file);
    
 	// seek through the bmp header, up to the width/height:
 	fseek(file, 4, SEEK_CUR);
	
 	// read the width
 	fread(&texture.nWidth, 4, 1, file);
    
 	// read the height 
 	fread(&texture.nHeight, 4, 1, file);
    
	// seek through the bmp header, up to the color depth
	fseek(file, 2, SEEK_CUR);
    
	// read the bits per pixel
	fread(&texture.nDepth, 2, 1, file);
    
	// check the pixel depth
	if (texture.nDepth != 8 && texture.nDepth != 24)
	{

		fclose(file);
        HLogE("[Error] loadBMP Not found %s nDepth=%d\n",filename,texture.nDepth);
		return -3;
	}
	
    
	// read the compression
	fread(&compression, 4, 1, file);
    
	// check the compression
	if (compression != 0)
	{
		
		fclose(file);
        HLogE("[Error] loadBMP Not found %s\n compressed",filename);
		return -5;
	}
    
	// move file pointer to beginning of bitmap data
	fseek(file, offset, SEEK_SET);
    
	// allocate enough memory for the bitmap  data
	texture.pImageBuffer = (unsigned char*)malloc(texture.nWidth*texture.nHeight*texture.nDepth/8);
    
	// verify memory allocation
	if (!texture.pImageBuffer)
	{
		fclose(file);
        HLogE("[Error] loadBMP Not found %s\n memory",filename);
		return -6;
	}
    
	// read in the bitmap  data
	fread(texture.pImageBuffer, 1, texture.nWidth*texture.nHeight*texture.nDepth/8, file);
    
	// swap the color if necessary
	if (texture.nDepth == 24)
	{
		SwapColorChannel(texture);
	}
    
	// close the file
	fclose(file);
	
	return 0;
}


sglTexture* CTextureMan::LoadImage(const char* sFileName,int nSType,int nTType,bool bPngReverse)
{
    sglTexture *pTexture = NULL;
    //Fire이미지를 제너레이터 한다.
	int nResult = 0;
	///Users/hohak/Project/88.MyDream/theK/Image
	//char sFile[1024];
    //sprintf(sFile,"%s/%s",Loader::getSearchPath(),pFileName);
	//int nWidth,nHeight;
	//unsigned char* pImage = 0;
    char sPath[MAXLENGTH];
    sprintf(sPath,"%s%s",CLoader::searchPath,sFileName);
    
    
    
    pTexture = new sglTexture;
#ifdef ANDROID
    
    //다시 텍스쳐를 읽어올때 UV가 존재 한다.
    int nLen = (int)strlen(sPath);
    if(nLen > 2)
    {
        if(sPath[nLen - 2] == 'U' && sPath[nLen - 1] == 'V')
        {
            sPath[nLen - 2] = 0;
            nSType = GL_REPEAT;
            nTType = GL_REPEAT;
        }
    }

    
    //Added By Song 2013.09.16 문자형태의 텍스쳐이라면 안드로이드에서 사용한다. ResetTexture 에서 분할하여 읽어온다.
    pTexture->bTextTexture = false;
#endif
    
    if(strstr(sPath, ".png"))
    {
        nResult = LoadPNG((char*)sPath,*pTexture,bPngReverse);
        if(nResult != 0)
        {
            //sglLog("Can't load tga %s",sFileName);
            delete pTexture;
            return 0;
        }
    }
    else if(strstr(sPath, ".bmp"))
    {
        nResult = LoadBMP((char*)sPath,*pTexture);
        if(nResult != 0)
        {
            //sglLog("Can't load tga %s",sFileName);
            delete pTexture;
            return 0;
        }
    }
    else
    {
        nResult = LoadTGA((char*)sPath,*pTexture);
        if(nResult != 0)
        {
            //sglLog("Can't load tga %s",sFileName);
            delete pTexture;
            return 0;
        }
    }
    
    pTexture->sTextureName2 = sFileName;
//    strcpy(pTexture->sTextureName,sFileName);
    
    glGenTextures(1, &pTexture->glTextureID);
    glBindTexture(GL_TEXTURE_2D, pTexture->glTextureID);
    
    //축소시 가장가까운 픽셀을 참조하여 축소를 한다.
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    //GL_NEAREST 이미지가 고르게 보이지 않고 가장자리가 거칠어서 GL_LINEAR로 하니 부드럽게 변경되었다.
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    
    //확대시 선형으로 확대를 하여 고르게 한다.
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,nSType);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,nTType);
    
    pTexture->nSType = nSType;
    pTexture->nTType = nTType;

    //GL_REPLACE 텍스처를 물체에 입힌다.
    //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_REPLACE);
    //glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    
    if(pTexture->nDepth == 24)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 
                     pTexture->nWidth, pTexture->nHeight, 
                     0, GL_RGB, GL_UNSIGNED_BYTE, pTexture->pImageBuffer);
    }
    else if(pTexture->nDepth == 32)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 
                     pTexture->nWidth, pTexture->nHeight, 
                     0, GL_RGBA, GL_UNSIGNED_BYTE, pTexture->pImageBuffer);
    }
    else if(pTexture->nDepth == 16)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 
                     pTexture->nWidth, pTexture->nHeight, 
                     0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, pTexture->pImageBuffer);
    }
    else //24 비트와 32비트만 지원한다.
    {
        glDeleteTextures(1, &pTexture->glTextureID);
        //delete[] pTexture->pImageBuffer;
        free(pTexture->pImageBuffer);
        pTexture->pImageBuffer = NULL;
        return 0;
    }
    
    //---------------------------------
    //Info버퍼를 지운다. 저장시에 에러가 난다...
    free(pTexture->pImageBuffer);
    pTexture->pImageBuffer = NULL;
    //---------------------------------
    
    return pTexture;
}


//bDeleteImage=false 는 안드로이드에서 java리소스를 할당할때 사용한다.
sglTexture* CTextureMan::BindByImageBuffer(const char* sKeyName,unsigned char* sImage,int nWidth,int nHeight,int nDepth,int nSType,int nTType,bool bDeleteImage)
{
    sglTexture *pTexture = NULL;
    //Fire이미지를 제너레이터 한다.
	//int nResult = 0;
    
    pTexture = new sglTexture;
    pTexture->nWidth = nWidth;
    pTexture->nHeight = nHeight;
    pTexture->nDepth = nDepth;
    pTexture->pImageBuffer = sImage;
#ifdef ANDROID
    //Added By Song 2013.09.16 문자형태의 텍스쳐이라면 안드로이드에서 사용한다. ResetTexture 에서 분할하여 읽어온다.
    pTexture->bTextTexture = true;
#endif
    
#ifdef DEBUG
//    if(strlen(sKeyName) >= sizeof(pTexture->sTextureName))
    if(strlen(sKeyName) > 128)
    {
        HLogE("[Error] Maxsize %s %d\n",sKeyName,strlen(sKeyName));
    }
#endif
//    strcpy(pTexture->sTextureName,sKeyName);
    pTexture->sTextureName2 = sKeyName;
    
    glGenTextures(1, &pTexture->glTextureID);
    glBindTexture(GL_TEXTURE_2D, pTexture->glTextureID);
    
    //축소시 가장가까운 픽셀을 참조하여 축소를 한다.
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);

    //글자의 경우 고르게 확대 축소를 해야 깨져 보이지 않음.
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    //확대시 선형으로 확대를 하여 고르게 한다.
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,nSType);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,nTType);   
    
    pTexture->nSType = nSType;
    pTexture->nTType = nTType;
    
    if(pTexture->nDepth == 24)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 
                     pTexture->nWidth, pTexture->nHeight,
                     0, GL_RGB, GL_UNSIGNED_BYTE, pTexture->pImageBuffer);
    }
    else if(pTexture->nDepth == 32)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 
                     pTexture->nWidth, pTexture->nHeight, 
                     0, GL_RGBA, GL_UNSIGNED_BYTE, pTexture->pImageBuffer);
    }
    else //24 비트와 32비트만 지원한다.
    {
        glDeleteTextures(1, &pTexture->glTextureID);
        free(pTexture->pImageBuffer);
        pTexture->pImageBuffer = NULL;
        return 0;
    }
    
    if(bDeleteImage)
    {
        //---------------------------------
        //Info버퍼를 지운다.
        free(pTexture->pImageBuffer);
        pTexture->pImageBuffer = NULL;
        //---------------------------------
    }
    else
    {
        pTexture->pImageBuffer = NULL;
    }
    return pTexture;
}
