//
//  CMS3DASCIIMan.cpp
//  SongGL
//
//  Created by 송 호학 on 12. 4. 2..
//  Copyright (c) 2012년 __MyCompanyName__. All rights reserved.
//

#include "CMS3DASCIIMan.h"
#include "CMS3DASCII.h"

CMS3DASCIIMan::CMS3DASCIIMan(CTextureMan *pTextureMan)
{
    mpTextureMan = pTextureMan;
}

CMS3DASCIIMan::~CMS3DASCIIMan()
{
    Clear();
}

void CMS3DASCIIMan::Clear()
{
    map<string,CMS3DASCII*>::iterator b = mapAscii.begin();
    map<string,CMS3DASCII*>::iterator e = mapAscii.end();
    for (map<string,CMS3DASCII*>::iterator it = b; it != e; it++) {
        if(it->second)
            delete it->second;
    }
    mapAscii.clear();
    
}

CMS3DASCII* CMS3DASCIIMan::Load(const char* sFilePath,TerrianMakeMeshInfo* pMeshInfo)
{
    string sp(sFilePath);
    CMS3DASCII* model = mapAscii[sp];
    if(model)
        return model->Clone(pMeshInfo);
    
    model = new CMS3DASCII(mpTextureMan);
    if(model->Load(sFilePath) != E_SUCCESS)
    {
        delete model;
        return NULL;
    }
    mapAscii[sp] = model;
    return model->Clone(pMeshInfo);
}