//
//  CMS3DModelASCIIMan.cpp
//  SongGL
//
//  Created by itsme on 12. 8. 27..
//
//

#include "CMS3DModelASCIIMan.h"
#include "CMS3DModelASCII.h"

CMS3DModelASCIIManX::CMS3DModelASCIIManX(CTextureMan *pTextureMan)
{
    mpTextureMan = pTextureMan;
}

CMS3DModelASCIIManX::~CMS3DModelASCIIManX()
{
    Clear();
}

void CMS3DModelASCIIManX::Clear()
{
    map<string,CMS3DASCII*>::iterator b = mapAscii.begin();
    map<string,CMS3DASCII*>::iterator e = mapAscii.end();
    for (map<string,CMS3DASCII*>::iterator it = b; it != e; it++) {
        if(it->second)
            delete it->second;
    }
    mapAscii.clear();
    
}

CMS3DASCII* CMS3DModelASCIIManX::Load(const char* sFilePath,bool bLoadNormalVector)
{
    string sp(sFilePath);
    CMS3DASCII* model = mapAscii[sp];
    if(model)
        return model;
    
    model = new CMS3DASCII(mpTextureMan);
    if(model->Load(sFilePath,bLoadNormalVector) != E_SUCCESS)
    {
        delete model;
        HLogE("CMS3DModelASCIIMan::Load %s\n",sFilePath);
        return NULL;
    }
    mapAscii[sp] = model;
    return model;
}

#ifdef ANDROID
void CMS3DModelASCIIManX::ResetTexture()
{
    map<string,CMS3DASCII*>::iterator b = mapAscii.begin();
    map<string,CMS3DASCII*>::iterator e = mapAscii.end();
    for (map<string,CMS3DASCII*>::iterator it = b; it != e; it++) {
     
        if(it->second) it->second->ResetTexture();
    }
}
#endif