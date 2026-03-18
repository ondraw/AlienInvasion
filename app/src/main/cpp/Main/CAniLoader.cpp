//
//  CAniLoader.cpp
//  SongGL
//
//  Created by Song Hohak on 11. 11. 24..
//  Copyright (c) 2011 thinkm. All rights reserved.
//

#include <iostream>
#include <string>
#include <ctype.h>
#include "CAniLoader.h"
using namespace std;

SGLResult CAniLoader::parseStartEndf(char* const data, GLuint& start, GLuint& end)
{
	char*	minus;
    
	if (data == 0)
	{
		return SGL_NULL_POINTER_ERROR;
	}
	
	minus = strchr(data, '-');
	if (minus)
	{
		*minus = 0;
		minus++;
		
		start = strtoul(data, 0, 0);
		end = strtoul(minus, 0, 0);
		
		if (end < start)
		{
			return SGL_DATA_CORRUPTED_ERROR;
		}
	}
	else
	{
		return SGL_DATA_CORRUPTED_ERROR;						
	}
	
	return SGL_NO_ERROR;
}

SGLResult CAniLoader::loadAnimationESADf(const char* const filename, CMS3DModel& model)
{
	FILE* file;
	char  c;
	GLbyte	index = -1; 
	char  buffer[BUFFERSIZE];
	GLubyte	state = 0;	
	GLubyte	counter = 0;
	string fullFilename(searchPath);
    
	if (model.animations)
	{
		delete[] model.animations;
		model.animations = 0;
		model.numberAnimations = 0;
	}
	
	// check, if we have a valid pointer
	if (!filename)
		return SGL_NULL_POINTER_ERROR;
    
	fullFilename = fullFilename + filename;
    
	// open filename in "read" mode
	file = fopen(fullFilename.c_str(), "r");
	if (!file)
		return SGL_FILE_ERROR;
    
	while ((c = fgetc(file)) != (char)EOF)
	{
		if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
			continue;
		
		index++;
		
		if (index == BUFFERSIZE)
		{
			fclose(file);
			return SGL_DATA_CORRUPTED_ERROR;
		}
        
		if (c != ';')
		{				
			buffer[index] = c;
		}
		else
		{
			buffer[index] = 0;
			index = -1;
            
			switch (state)
			{
                    // Expecting amount of animations
				case 0:
					model.numberAnimations = (GLushort)strtoul(buffer, 0, 0);
					if (model.numberAnimations > 0)
					{
						model.animations = new CMS3DModel::Animationf[model.numberAnimations];
						state = 1;
					}
					else
					{
						fclose(file);
						return SGL_DATA_CORRUPTED_ERROR;						
					}
                    break;
                    
                    // Expecting type of data
				case 1:
					if (strcmp(buffer, "AP") == 0)
					{
						state = 2;
					}
					//else if (strcmp(buffer, "MILLISECONDS") == 0)
                    else if (strcmp(buffer, "MILLISEC") == 0)
					{
						state = 3;
					}
					else
					{
						model.numberAnimations = 0;
						delete[] model.animations;
						model.animations = 0;
						
						fclose(file);
						return SGL_DATA_CORRUPTED_ERROR;						
					}					
                    break;
                    
                    // Expecting frame data
				case 2:
					if (parseStartEndf(buffer, model.animations[counter].start, model.animations[counter].end) == SGL_NO_ERROR)
					{
						model.animations[counter].start = model.animations[counter].start * 1000/model.modelCore->framesPerSecond;
						
						model.animations[counter].end = model.animations[counter].end * 1000/model.modelCore->framesPerSecond;
						
						state = 1;
						
						counter++;
						
						if (counter == model.numberAnimations)
						{
							break;
						}
					}
					else
					{
						model.numberAnimations = 0;
						delete[] model.animations;
						model.animations = 0;
						
						fclose(file);
						return SGL_DATA_CORRUPTED_ERROR;						
					}
                    break;
                    
                    // Expecting milliseconds data
				case 3:
					if (parseStartEndf(buffer, model.animations[counter].start, model.animations[counter].end) == SGL_NO_ERROR)
					{
						state = 1;
                        
						counter++;
						
						if (counter == model.numberAnimations)
						{
							break;
						}
					}
					else
					{
						model.numberAnimations = 0;
						delete[] model.animations;
						model.animations = 0;
						
						fclose(file);
						return SGL_DATA_CORRUPTED_ERROR;						
					}				
                    break;
			}			
		}		
	}
	
	fclose(file);
	
	return SGL_NO_ERROR;
}

SGLResult CAniLoader::loadAnimationf(const char* const filename, CMS3DModel& model)
{
	const char* extension = 0;
	char* copyOfExtension = 0;
	GLuint	index = 0;
	SGLResult	result;
    
	// check, if we have a valid pointer
	if (!filename)
		return SGL_NULL_POINTER_ERROR;
	
	// find the file extension
	extension = strrchr(filename, '.');
    
	if (!extension)
		return SGL_ANIMATION_NOT_SUPPORTED_ERROR;
	
	// prepare a copy of the file extension
	copyOfExtension = new char[strlen(extension)+1];
    
	if (!copyOfExtension)
		return SGL_OUT_OF_MEMORY_ERROR;
    
	// make a lowercase copy of the file extension
	do
	{
		copyOfExtension[index] = tolower(extension[index]);
	} while (extension[index++]);
    
	// load the texture depending on the file extension
	if (strcmp(copyOfExtension, ".ani") == 0)
		result = loadAnimationESADf(filename, model);
	else
		result = SGL_ANIMATION_NOT_SUPPORTED_ERROR;
    
	delete[] copyOfExtension;	
	return result;
	
}