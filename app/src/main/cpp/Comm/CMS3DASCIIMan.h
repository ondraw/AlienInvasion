//
//  CMS3DASCIIMan.h
//  SongGL
//
//  Created by 송 호학 on 12. 4. 2..
//  Copyright (c) 2012년 __MyCompanyName__. All rights reserved.
//

#ifndef SongGL_CMS3DASCIIMan_h
#define SongGL_CMS3DASCIIMan_h

#include <map>
#include <string>
#include "TerrianMakeType.h"

using namespace std;

class CMS3DASCII;
class CTextureMan;

//디자인툴이나, 지형에서만 쓰인다. 즉 속도를 높이기 위해서 모든 캐릭터의 고유의 버텍스 메모리를 가지고 있다.
class CMS3DASCIIMan {
public:
    CMS3DASCIIMan(CTextureMan *pTextureMan);
    ~CMS3DASCIIMan();
    void Clear();
    CMS3DASCII* Load(const char* sFilePath,TerrianMakeMeshInfo* pMeshInfo = NULL);
protected:
    map<string, CMS3DASCII*> mapAscii;
    CTextureMan *mpTextureMan;
};

#endif
