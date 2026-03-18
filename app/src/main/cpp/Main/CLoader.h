//
//  CLoader.h
//  SongGL
//
//  Created by Song Hohak on 11. 11. 24..
//  Copyright (c) 2011 thinkm. All rights reserved.
//

#ifndef SongGL_CLoader_h
#define SongGL_CLoader_h

#if defined ANDROID
#include <GLES/gl.h>
#include <GLES/glext.h>
#elif defined IPHONE
#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#else //MAC
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <OpenGL/glu.h>
#endif

//경로값이 길어졌는지 확인해보자.
#define MAXLENGTH 356



/**
 * Loader for loading several animations belonging to a given model.
 */
class CLoader
{
    
public:
    
    static char* getSearchPath();
    
    /**
     * Deafualt search path
     */
    static char searchPath[MAXLENGTH];
    
    /**
     * Sets the default search path.
     * 
     * @param path the path where to look for data
     */	
    static GLvoid setSearchPath(const char* const path);
    
    
    /**
     * Returns the filename without any seperator
     *
     * @param filename the filename
     */
    static const char* getFilename(const char* const filename);
    
};

#endif
