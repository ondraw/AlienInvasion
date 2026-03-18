//
//  CMS3DLoader.h
//  SongGL
//
//  Created by Song Hohak on 11. 11. 24..
//  Copyright (c) 2011 thinkm. All rights reserved.
//

#ifndef SongGL_CMS3DLoader_h
#define SongGL_CMS3DLoader_h

#include "sGLDefine.h"
#include "CMS3DModel.h"
#include <vector>
#include <string>
using namespace std;

class CTextureMan;
class CMS3DLoader
{
	
private:
    
	/**
	 * Class for temporary storage of data.
	 */
	class Jointf
	{
		
	public:
		
		/**
		 * Local rotation of the joint.
		 */
		GLfloat	rotation[3];
		
		/**
		 * Local translation of the joint.
		 */
		GLfloat	translation[3];
	};
    
	/**
	 * Array of temporary joints.
	 */
	static Jointf*	joints;
    
	/**
	 * Pointer to the current opened file.
	 */
	static FILE*	file;
    
	/**
	 * Pointer to the model where the data will be loaded.
	 */
	static CModelData*	modelCore;
	
	/**
	 * List of joint names. Used temporary.
	 */
	static vector<string>	listOfJoints;
    
	/**
	 * Reads the header of the MS3D model.
	 */
	static GLvoid readHeaderf();
    
	/**
	 * Reads the number of vertices of the MS3D model.
	 */
    static GLvoid readNumberVerticesf();
    
    /**
     * Reads the vertices of the MS3D model.
     */
	static GLvoid readVerticesf();
    
	/**
	 * Reads the number of triangles of the MS3D model.
	 */
	static GLvoid readNumberTrianglesf();
	
	/**
	 * Reads the triangles of the MS3D model.
	 */
	static GLvoid readTrianglesf();
    
	/**
	 * Reads the number of groups of the MS3D model.
	 */
	static GLvoid readNumberGroupsf();
	
	/**
	 * Reads the groups of the MS3D model.
	 */
	static GLvoid readGroupsf();
    
	/**
	 * Reads the number of materials of the MS3D model.
	 */
	static GLvoid readNumberMaterialsf();
	
	/*
	 * Reads the materials of the MS3D model.
	 */
	static GLvoid readMaterialsf(CTextureMan* pTextureMan);
    
	/**
	 * Reads the keyframe data of the MS3D model.
	 */
	static GLvoid readKeyFrameDataf();
    
	/**
	 * Read number of joints of the MS3D model.
	 */
	static GLvoid readNumberJointsf();
	
	/**
	 * Reads the joints of the MS3D model.
	 */
	static GLvoid readJointsf();
	
	/**
	 * Calculates the joints. Matrix and inverse matrix are generated.
	 */
	static GLvoid calculateJointsf();
	
public:
    
	/**
	 * Loads the MS3D model.
	 * 
	 * @param filename the filename of the model
	 * @param currentModelCore	the model where the 3D object is created
	 * 
	 * @return error code
	 */
	static SGLResult loadModelCoref(const char* const filename, CModelData& currentModelCore,CTextureMan* pTextureMan);
    static SGLResult LoadMS3D(const char* const filename, CModelData& modelCore,CTextureMan* pTextureMan);
	
};

#endif
