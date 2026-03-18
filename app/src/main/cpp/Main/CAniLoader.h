//
//  CAniLoader.h
//  SongGL
//
//  Created by Song Hohak on 11. 11. 24..
//  Copyright (c) 2011 thinkm. All rights reserved.
//

#ifndef SongGL_CAniLoader_h
#define SongGL_CAniLoader_h
#include "CMS3DModel.h"
#include "CLoader.h"

#define BUFFERSIZE	16

class CAniLoader : public CLoader
{
    
private:
    
	/**
	 * Parses the start and the end of an animation from a given string.
	 * Format must be start-end e.g. 0-30.
	 * 
	 * @param data the string which contains the start and end
	 * @param start	variable to write back the start
	 * @param end variable to write back the end
	 * 
	 * @return error code
	 */
	static SGLResult	parseStartEndf(char* const data, GLuint& start, GLuint& end);
    
	/**
	 * Creates several animation keyframes from a given ES3AD file.
	 * Example:
	 * 2;
	 * MILLISECOND;
	 * 0-1000;
	 * AP;
	 * 16-24;
	 * This defines 2 animations. The first one contains the data in milliseconds.
	 * The second one as frame data. Please note, that MILLISECOND and FRAME are case sensitive
	 * and have to be in uppercase letters. Also these keywords are necessary before each entry.
	 * 
	 * @param filename the filename which contains the data
	 * @param model the model where the animation will associated to
	 * 
	 * @return error code
	 */
	static SGLResult loadAnimationESADf(const char* const filename, CMS3DModel& model);
	
public:
    
	/**
	 * Loads animation data from a given file depending on the file extension.
	 * 
	 * @param filename the filename which contains the data
	 * @param model the model where the animation will associated to
	 * 
	 * @return error code
	 */	
	static SGLResult loadAnimationf(const char* const filename, CMS3DModel& model);
    
};

#endif
