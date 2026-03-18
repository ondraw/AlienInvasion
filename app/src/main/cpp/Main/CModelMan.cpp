//
//  CModelMan.cpp
//  SongGL
//
//  Created by Song Hohak on 11. 11. 24..
//  Copyright (c) 2011 thinkm. All rights reserved.
//

#include <iostream>
#include "sGL.h"
#include "CModelMan.h"
#include "CMS3DLoader.h"
#include "CMS3DModelASCII.h"
#include "CTankModelASCII.h"

//map<string, CModelData*> CModelMan::models;
//CTextureMan* CModelMan::mpTextureMan = 0;

CModelMan::CModelMan(CTextureMan* pTextureMan)
{
    mpTextureMan = pTextureMan;
}

CModelMan::~CModelMan()
{
    deleteAllModelCoresf();
}

//주의 ppModel을 지우지 말자. Man에서 알아서 한다.
SGLResult CModelMan::getModelfAscii(const char* const filename, CMS3DASCII** ppModel,bool bLoadNormalVector)
{
	string	key(filename);
	SGLResult result = SGL_NO_ERROR;
    int nLen = strlen(filename);
    if(filename[nLen - 1] == 't' ||  filename[nLen - 1] == 'T') //TXT
    {
        string sp(filename);
        *ppModel = mapAscii[sp];
        if(*ppModel)
        {
            return result;
        }
        
        *ppModel = new CMS3DASCII(mpTextureMan);
        if((*ppModel)->Load(filename,bLoadNormalVector) != E_SUCCESS)
        {
            delete *ppModel;
            HLogE("CModelMan::getModelf::txt %s\n",filename);
            return SGL_MODEL_NOT_SUPPORTED_ERROR;
        }
        mapAscii[sp] = *ppModel;
        return result;
        
    }
    else
    {
        HLogE("여기로 오면 안뒤아 getModelf CMS3DASCII\n");

    }
	
	return result;
}

SGLResult CModelMan::getModelf(const char* const filename, CMyModel** ppModel,bool bLoadNormalVector)
{
	string	key(filename);
	CModelData*		modelCore;
	SGLResult result = SGL_NO_ERROR;

    int nLen = strlen(filename);
    if(filename[nLen - 1] == 't' ||  filename[nLen - 1] == 'T') //TXT
    {
        string sp(filename);
        CMS3DASCII* model = mapAscii[sp];
        if(model)
        {
            *ppModel = new CMS3DModelASCII(model);
            return result;
        }
        
        model = new CMS3DASCII(mpTextureMan);
        if(model->Load(filename,bLoadNormalVector) != E_SUCCESS)
        {
            delete model;
            HLogE("CModelMan::getModelf::txt %s\n",filename);
            return SGL_MODEL_NOT_SUPPORTED_ERROR;
        }
        mapAscii[sp] = model;
        *ppModel = new CMS3DModelASCII(model);
        return result;

    }
    else
    {
        *ppModel = new CMS3DModel();
        if (models.find(key) != models.end())
        {
            modelCore = models[key];
            ((CMS3DModel*)*ppModel)->initf(*modelCore);
            return SGL_NO_ERROR;
        }
        
        modelCore = new CModelData;
        
        if (!modelCore)
        {
            return SGL_OUT_OF_MEMORY_ERROR;
        }
        
        if ((result = CMS3DLoader::LoadMS3D(filename, *modelCore,mpTextureMan)) != SGL_NO_ERROR)
        {
            HLogE("[Error] Can not read model %s\n",filename);
            delete modelCore;
            modelCore = 0;
            
            return result;
        }
        
        ((CMS3DModel*)*ppModel )->initf(*modelCore);
        
        models[key] = modelCore;
    }
	
	return result;
}


SGLResult CModelMan::getModelfTankHeader(const char* const filename, CMyModel** ppModel,bool bLoadNormalVector)
{
	string	key(filename);
	SGLResult result = SGL_NO_ERROR;
    int nLen = strlen(filename);
    if(filename[nLen - 1] == 't' ||  filename[nLen - 1] == 'T') //TXT
    {
        string sp(filename);
        CMS3DASCII* model = mapAscii[sp];
        if(model)
        {
            *ppModel = new CTankModelASCII(model);
            return result;
        }
        
        model = new CMS3DASCII(mpTextureMan);
        if(model->Load(filename,bLoadNormalVector) != E_SUCCESS)
        {
            delete model;
            HLogE("CModelMan::getModelf::txt %s\n",filename);
            return SGL_MODEL_NOT_SUPPORTED_ERROR;
        }
        mapAscii[sp] = model;
        *ppModel = new CTankModelASCII(model);
        return result;
        
    }
    else
    {
        HLogE("여기로 오면 안뒤아\n");
    }
	
	return result;
}

GLvoid CModelMan::deleteModelCoref(const char* const filename)
{
    int nLen = strlen(filename);
	string	key(filename);
    if(filename[nLen - 1] == 't' ||  filename[nLen - 1] == 'T') //TXT
    {
        CMS3DASCII* currentModelCore = mapAscii[key];
        if (currentModelCore)
        {
            delete currentModelCore;
            mapAscii.erase(key);
        }
    }
    else
    {
        CModelData* currentModelCore = models[key];
        if (currentModelCore)
        {
            delete currentModelCore;
            models.erase(key);	
        }
    }
}

GLvoid CModelMan::deleteAllModelCoresf()
{
    map<string, CModelData*>::iterator i;
    for (i = models.begin(); i != models.end(); i++) 
    {
        delete i->second;
    }
	models.clear();
    
    
    map<string,CMS3DASCII*>::iterator b = mapAscii.begin();
    map<string,CMS3DASCII*>::iterator e = mapAscii.end();
    for (map<string,CMS3DASCII*>::iterator it = b; it != e; it++) {
        if(it->second)
            delete it->second;
    }
    mapAscii.clear();
}

#ifdef ANDROID
void CModelMan::ResetTexture()
{
    map<string, CModelData*>::iterator it;
    for (it = models.begin(); it != models.end(); it++)
    {
        for(int i = 0; i < it->second->numberMaterials; i++)
        {
            it->second->materials[i].textureName = mpTextureMan->GetTextureID(it->second->materials[i].textrueFileName.c_str());
        }
    }
    
    
    map<string,CMS3DASCII*>::iterator b = mapAscii.begin();
    map<string,CMS3DASCII*>::iterator e = mapAscii.end();
    for (map<string,CMS3DASCII*>::iterator it = b; it != e; it++) {
        
        if(it->second) it->second->ResetTexture();
    }
}
#endif