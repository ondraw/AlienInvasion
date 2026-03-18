//
//  CArchive.h
//  SongGL
//
//  Created by 호학 송 on 2014. 2. 2..
//
//

#ifndef __SongGL__CArchive__
#define __SongGL__CArchive__
#include <stdio.h>
#define ARCHIVE_DEFAULT_SIZE 128
#include "sGLDefine.h"

class CArchive
{

public:
    CArchive(int nSize = ARCHIVE_DEFAULT_SIZE); //쓰기
    CArchive(char* sBuffer,int nSize); //읽기
    virtual ~CArchive();
    CArchive(const CArchive& /* arSrc */);
    void operator=(const CArchive& /* arSrc */);
    bool Write(const char* sPath);
    
    int GetSize() { return mnSize;}
    unsigned char* GetBuffer() { return mpBuffer;}
    
    void Begin() { mnPos = 0;}
    //Write
    CArchive& operator<<(int& i);
    CArchive& operator<<(unsigned int& i);
//64bit를 넣지 말자. 32bit단말기와 64bit단말기의 차이가 존재한다.
//    CArchive& operator<<(long& i);
//    CArchive& operator<<(unsigned long& i);
    CArchive& operator<<(float& f);
    CArchive& operator<<(char& c);
    CArchive& operator<<(unsigned char& c);
    CArchive& operator<<(bool& b);
    CArchive& operator<<(SPoint& p);
    CArchive& operator<<(SVector& v);
    

    bool Read(unsigned char* sOutData,int nLen);
    //Read
    CArchive& operator>>(int& i);
    CArchive& operator>>(unsigned int& i);
    CArchive& operator>>(float& f);
    CArchive& operator>>(char& c);
    CArchive& operator>>(unsigned char& c);
    CArchive& operator>>(bool& b);
    CArchive& operator>>(SPoint& p);
    CArchive& operator>>(SVector& v);

    void SetPosition(int v) { mnPos = v;}

protected:
    bool Write(unsigned char* sData,int nLen);
protected:
    unsigned char*  mpBuffer;
    int             mnBufferSize;
    int             mnPos;
    int             mnSize;
};

#endif /* defined(__SongGL__CArchive__) */
