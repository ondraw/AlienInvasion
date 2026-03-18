//
//  CLoader.cpp
//  SongGL
//
//  Created by Song Hohak on 11. 11. 24..
//  Copyright (c) 2011 thinkm. All rights reserved.
//

#include <iostream>
#include "CLoader.h"

char CLoader::searchPath[MAXLENGTH] = {0};

char* CLoader::getSearchPath()
{
    return searchPath;
}

GLvoid CLoader::setSearchPath(const char* const path)
{
	if (path == 0)
		return;
    
	if (strlen(path)+1 > MAXLENGTH)
		return;
    
	strcpy(searchPath, path);
}

const char* CLoader::getFilename(const char* const filename)
{
	const char* start;
    
	if (filename == 0)
		return 0;
    
	start = strrchr(filename, '/');
    
	if (start != 0)
		return ++start;
    
	start = strrchr(filename, '\\');
    
	if (start != 0)
		return ++start;
    
	return filename;
}

