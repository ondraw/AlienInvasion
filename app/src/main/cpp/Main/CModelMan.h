//
//  CModelMan.h
//  SongGL
//
//  Created by Song Hohak on 11. 11. 24..
//  Copyright (c) 2011 thinkm. All rights reserved.
//

#ifndef SongGL_CModelMan_h
#define SongGL_CModelMan_h

#include <string>
#include <map>
#include "CMS3DModel.h"
#include "CTextureMan.h"
using namespace std;

class CModelData;
class CMS3DASCII;
class CModelMan
{
    
public:
    CModelMan(CTextureMan* pTextureMan);
    ~CModelMan();
    CTextureMan* GetTextureMan() { return mpTextureMan;}
    
#ifdef ANDROID
    void ResetTexture();
#endif
    
private:
    
	/**
	 * Map where the filename points to the given model.
	 */
	map<string, CModelData*> models;
    CTextureMan* mpTextureMan;
    map<string, CMS3DASCII*> mapAscii;
    
public:
    
	/**
	 * Retrieves the model of a given filename.
	 * 
	 * @param filename the filename of the model
	 * @param model variable where to store the model
	 * 
	 * @return error code
	 */
    
    //bLoadNormalVector = true , *.txt를 읽어올때 벡터를 읽어올때 가져온다.
    SGLResult getModelfAscii(const char* const filename, CMS3DASCII** ppModel,bool bLoadNormalVector = false);
	SGLResult getModelf(const char* const filename, CMyModel** ppModel,bool bLoadNormalVector = false);
    SGLResult getModelfTankHeader(const char* const filename, CMyModel** ppModel,bool bLoadNormalVector = false);
    
	/**
	 * Deletes the given core model. Associated models can not be used anymore.
	 * 
	 * @param filename the filename of the model
	 */
	GLvoid deleteModelCoref(const char* const filename);
	
	/**
	 * Deletes all core models. All models can not be used anymore.
	 */
	GLvoid deleteAllModelCoresf();
    
};

#endif
