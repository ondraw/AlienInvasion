//
//  CArchive.cpp
//  SongGL
//
//  Created by 호학 송 on 2014. 2. 2..
//
//
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "sGLDefine.h"
#include "CArchive.h"

CArchive::CArchive(int nSize)
{
    mpBuffer = (unsigned char*)malloc(nSize);
    mnBufferSize = nSize;
    mnPos = 0;
    mnSize = 0;
}

CArchive::CArchive(char* sBuffer,int nSize)
{
    mpBuffer = (unsigned char*)malloc(nSize);
    memcpy(mpBuffer, sBuffer, nSize);
    mnPos = 0;
    mnSize = nSize;
    mnBufferSize = nSize;
}

CArchive::CArchive(const CArchive& arSrc)
{

}


CArchive::~CArchive()
{
    if(mpBuffer)
    {
        free(mpBuffer);
        mpBuffer = NULL;
    }
    mnBufferSize = 0;
    mnPos = 0;
    mnSize = 0;
}


void CArchive::operator=(const CArchive& arSrc)
{
}

bool CArchive::Write(const char* sPath)
{
    int nWLen = 0;
    FILE* hFile;
    hFile = fopen(sPath, "wb");
    if(hFile == NULL)
    {
        HLogE("Can't Write fopen %s Archive\n",sPath);
        return false;
    }
    
    //총 길이..
    fwrite(&mnPos, sizeof(int), 1, hFile);
    
    //데이터...
    nWLen = fwrite(mpBuffer, mnPos, 1, hFile);
    if(nWLen != mnPos)
    {
        HLogE("Can't Write %s Archive\n",sPath);
        fclose(hFile);
        return false;
    }
    
    fclose(hFile);
    return true;
}



bool CArchive::Write(unsigned char* sData,int nLen)
{
    if(mnPos + nLen > mnBufferSize)
    {
        mnBufferSize += ARCHIVE_DEFAULT_SIZE;
        mpBuffer = (unsigned char*)realloc(mpBuffer, mnBufferSize); //싸이즈를 변경하자.
    }
    
    memcpy(mpBuffer + mnPos,sData,nLen);
    mnPos += nLen;
    mnSize += nLen;
    return true;
}

bool CArchive::Read(unsigned char* sOutData,int nLen)
{
    if(mnPos + nLen > mnBufferSize)
    {
        assert(0);
        return false;
    }
    memcpy(sOutData,mpBuffer + mnPos,nLen);
    mnPos += nLen;
    return true;
}

CArchive& CArchive::operator<<(int& i)
{
    Write((unsigned char*)&i, sizeof(int));
    return *this;
}

CArchive& CArchive::operator<<(unsigned int& i)
{
    Write((unsigned char*)&i, sizeof(unsigned int));
    return *this;
}

CArchive& CArchive::operator<<(float& f)
{
    Write((unsigned char*)&f, sizeof(float));
    return *this;
}

CArchive& CArchive::operator<<(char& c)
{
    Write((unsigned char*)&c, sizeof(char));
    return *this;
}

CArchive& CArchive::operator<<(unsigned char& c)
{
    Write((unsigned char*)&c, sizeof(unsigned char));
    return *this;
}


CArchive& CArchive::operator<<(bool& b)
{
    Write((unsigned char*)&b, sizeof(bool));
    return *this;
}

CArchive& CArchive::operator<<(SPoint& p)
{
    Write((unsigned char*)&p.x, sizeof(float));
    Write((unsigned char*)&p.y, sizeof(float));
    Write((unsigned char*)&p.z, sizeof(float));
    return *this;
}
CArchive& CArchive::operator<<(SVector& v)
{
    Write((unsigned char*)&v.x, sizeof(float));
    Write((unsigned char*)&v.y, sizeof(float));
    Write((unsigned char*)&v.z, sizeof(float));
    return *this;
}


//Read
CArchive& CArchive::operator>>(int& i)
{
    //Write((unsigned char*)&c, sizeof(unsigned char));
    Read((unsigned char*)&i, sizeof(int));
    return *this;
}

CArchive& CArchive::operator>>(unsigned int& i)
{
    Read((unsigned char*)&i, sizeof(unsigned int));
    return *this;
}

CArchive& CArchive::operator>>(float& f)
{
    Read((unsigned char*)&f, sizeof(float));
    return *this;
}

CArchive& CArchive::operator>>(char& c)
{
    Read((unsigned char*)&c, sizeof(char));
    return *this;
}

CArchive& CArchive::operator>>(unsigned char& c)
{
    Read((unsigned char*)&c, sizeof(unsigned char));
    return *this;
}


CArchive& CArchive::operator>>(bool& b)
{
    Read((unsigned char*)&b, sizeof(bool));
    return *this;
}

CArchive& CArchive::operator>>(SPoint& p)
{
    Read((unsigned char*)&p.x, sizeof(float));
    Read((unsigned char*)&p.y, sizeof(float));
    Read((unsigned char*)&p.z, sizeof(float));
    return *this;
}

CArchive& CArchive::operator>>(SVector& v)
{
    Read((unsigned char*)&v.x, sizeof(float));
    Read((unsigned char*)&v.y, sizeof(float));
    Read((unsigned char*)&v.z, sizeof(float));
    return *this;
}
