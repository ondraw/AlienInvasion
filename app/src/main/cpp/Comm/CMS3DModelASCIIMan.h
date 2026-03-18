//
//  CMS3DModelASCIIMan.h
//  SongGL
//
//  Created by itsme on 12. 8. 27..
//
//

#ifndef SongGL_CMS3DModelASCIIMan_h
#define SongGL_CMS3DModelASCIIMan_h


#include <map>
#include <string>
#include "TerrianMakeType.h"

using namespace std;

class CMS3DASCII;
class CTextureMan;

//디자인툴이나, 지형에서만 쓰인다. 즉 속도를 높이기 위해서 모든 캐릭터의 고유의 버텍스 메모리를 가지고 있다.
class CMS3DModelASCIIManX {
public:
    CMS3DModelASCIIManX(CTextureMan *pTextureMan);
    ~CMS3DModelASCIIManX();
    void Clear();
    CMS3DASCII* Load(const char* sFilePath,bool bLoadNormalVector = false);

#ifdef ANDROID
    virtual void ResetTexture();
#endif
    
protected:
    map<string, CMS3DASCII*> mapAscii;
    CTextureMan *mpTextureMan;
};

#endif
