//
//  CMS3DLoader.cpp
//  SongGL
//
//  Created by Song Hohak on 11. 11. 24..
//  Copyright (c) 2011 thinkm. All rights reserved.
//

#include "sGLTrasform.h"
#include "CMS3DLoader.h"
#include "CLoader.h"
#include "CTextureMan.h"
#include <string>
#include <ctype.h>

CMS3DLoader::Jointf*	CMS3DLoader::joints = 0;	

FILE*	CMS3DLoader::file = 0;

CModelData*	CMS3DLoader::modelCore = 0;

vector<string>	CMS3DLoader::listOfJoints;

GLvoid CMS3DLoader::readHeaderf()
{
	// seek through the header
 	fseek(file, 10 + 4, SEEK_CUR);
}

GLvoid CMS3DLoader::readNumberVerticesf()
{
 	fread(&modelCore->numberVertices, 2, 1, file);
	
	modelCore->localVertices = new GLfloat[3*modelCore->numberVertices];
	modelCore->jointsOfVertices = new GLbyte[modelCore->numberVertices];
}

GLvoid CMS3DLoader::readVerticesf()
{
	for (GLushort i = 0; i < modelCore->numberVertices; i++)
	{
	 	fseek(file, 1, SEEK_CUR);
        
		for (GLushort k = 0; k < 3; k++)
		{		
	 		fread(&modelCore->localVertices[i*3+k], 4, 1, file);
		}
	 	
	 	fread(&modelCore->jointsOfVertices[i], 1, 1, file);
        
	 	fseek(file, 1, SEEK_CUR);
	}
}

GLvoid CMS3DLoader::readNumberTrianglesf()
{
 	fread(&modelCore->numberTriangles, 2, 1, file);
    
	modelCore->triangles = new CModelData::Trianglef[modelCore->numberTriangles];	
}

GLvoid CMS3DLoader::readTrianglesf()
{
	for (GLushort i = 0; i < modelCore->numberTriangles; i++)
	{
		GLushort k;
        
	 	fseek(file, 2, SEEK_CUR);
        
		modelCore->triangles[i].index = i;
        
		for (k = 0; k < 3; k++)
		{
			fread(&modelCore->triangles[i].vertexIndices[k], 2, 1, file);
		}
        
      	for (k = 0; k < 3; k++)
      	{
	      	for (GLushort l = 0; l < 3; l++)
	      	{      		
				fread(&modelCore->triangles[i].normals[k*3+l], 4, 1, file);
	      	}
      	}
        
      	for (k = 0; k < 3; k++)
      	{
			fread(&modelCore->triangles[i].texCoord[k*2], 4, 1, file);
      	}
        
      	for (k = 0; k < 3; k++)
      	{
			fread(&modelCore->triangles[i].texCoord[k*2+1], 4, 1, file);
			modelCore->triangles[i].texCoord[k*2+1] = 1.0f - modelCore->triangles[i].texCoord[k*2+1];
      	}
        
	 	fseek(file, 1+1, SEEK_CUR);
	}	
}

GLvoid CMS3DLoader::readNumberGroupsf()
{
 	fread(&modelCore->numberGroups, 2, 1, file);
    
    modelCore->groups = new CModelData::Groupf[modelCore->numberGroups];
}

GLvoid CMS3DLoader::readGroupsf()
{
	for (GLushort i = 0; i < modelCore->numberGroups; i++)
	{
	 	fseek(file, 1+32, SEEK_CUR);
        
	 	fread(&modelCore->groups[i].numberTriangles, 2, 1, file);
        
		modelCore->groups[i].trianglesIndices = new GLushort[modelCore->groups[i].numberTriangles];
		
		for (GLushort k = 0; k < modelCore->groups[i].numberTriangles; k++)
		{
		 	fread(&modelCore->groups[i].trianglesIndices[k], 2, 1, file);
		}
	  	
	 	fread(&modelCore->groups[i].materialIndex, 1, 1, file);
	}
}

GLvoid CMS3DLoader::readNumberMaterialsf()
{
 	fread(&modelCore->numberMaterials, 2, 1, file);
    
    modelCore->materials = new CModelData::Materialf[modelCore->numberMaterials];    
}

GLvoid CMS3DLoader::readMaterialsf(CTextureMan* pTextureMan)
{
	char buffer[128];
	const char* filename;
	
    for (GLushort i = 0; i < modelCore->numberMaterials; i++)
    {
		GLushort k;
        
    	fseek(file, 32, SEEK_CUR);
        
      	for (k = 0; k < 4; k++)
      	{
			fread(&modelCore->materials[i].ambient[k], 4, 1, file);
      	}
        
      	for (k = 0; k < 4; k++)
      	{
			fread(&modelCore->materials[i].diffuse[k], 4, 1, file);
      	}
        
      	for (k = 0; k < 4; k++)
      	{
			fread(&modelCore->materials[i].specular[k], 4, 1, file);
      	}
        
      	for (k = 0; k < 4; k++)
      	{
			fread(&modelCore->materials[i].emission[k], 4, 1, file);
      	}
		
		fread(&modelCore->materials[i].shininess, 4, 1, file);
		
		fseek(file, 4 + 1, SEEK_CUR);
        
		fread(buffer, 1, 128, file);
        
        
		filename = CLoader::getFilename(buffer);
        
        modelCore->materials[i].textureName = pTextureMan->GetTextureID(filename);
        if (modelCore->materials[i].textureName != 0)
		{
			modelCore->materials[i].textureAvailable = GL_TRUE;
            
#ifdef ANDROID
            modelCore->materials[i].textrueFileName = filename;
#endif
		}
		else
		{
			modelCore->materials[i].textureAvailable = GL_FALSE;
			modelCore->materials[i].textureName = 0;
		}
        
		// no alpha map currently used
		fseek(file, 128, SEEK_CUR);
	}
}

GLvoid CMS3DLoader::readKeyFrameDataf()
{
	GLfloat framesPerSecond;
    
	fread(&framesPerSecond, 4, 1, file);
	
	modelCore->framesPerSecond = (GLuint)framesPerSecond;
	
	fseek(file, 4, SEEK_CUR);
    
	fread(&modelCore->lastFrame, 4, 1, file);
}

GLvoid CMS3DLoader::readNumberJointsf()
{
	fread(&modelCore->numberJoints, 2, 1, file);
	
	modelCore->joints = new CModelData::Jointf[modelCore->numberJoints];
	joints = new CMS3DLoader::Jointf[modelCore->numberJoints];
}

GLvoid CMS3DLoader::readJointsf()
{
	char joint[32];
	char father[32];
	
    for (GLushort i = 0; i < modelCore->numberJoints; i++)
    {
		GLushort k;
        
		fseek(file, 1, SEEK_CUR);
		
		fread(&joint, 1, 32, file);
		
		listOfJoints.push_back(string(joint));
		
		fread(&father, 1, 32, file);

		
        
        
        
        /////////////////////////////////////////////////
        //modelCore->joints[i].fatherJoint = listOfJoints.indexOf(SimpleString(father))
        string sX(father);
        GLint SizeX = listOfJoints.size();
        GLint IsContine = -1;
        for (GLint X = 0; X < SizeX; X++)
		{
			if (sX == listOfJoints[X])
            {
                IsContine = X;
            }
		}
        modelCore->joints[i].fatherJoint = IsContine;
        /////////////////////////////////////////////////
		
		for (k = 0; k < 3; k++)
		{
			fread(&joints[i].rotation[k], 4, 1, file);
			
			joints[i].rotation[k] = 360.0f*joints[i].rotation[k]/(2.0f*PIf);
		}
        
		for (k = 0; k < 3; k++)
		{
			fread(&joints[i].translation[k], 4, 1, file);
		}
        
		fread(&modelCore->joints[i].numberKeyframeRotations, 2, 1, file);
		modelCore->joints[i].keyframeRotations = new CModelData::Jointf::KeyframeRotationf[modelCore->joints[i].numberKeyframeRotations];
        
		fread(&modelCore->joints[i].numberKeyframeTranslations, 2, 1, file);
		modelCore->joints[i].keyframeTranslations = new CModelData::Jointf::KeyframeTranslationf[modelCore->joints[i].numberKeyframeTranslations];
        
		for (k = 0; k < modelCore->joints[i].numberKeyframeRotations; k++)
		{
			GLfloat	time;			
			fread(&time, 4, 1, file);
			
			modelCore->joints[i].keyframeRotations[k].time = (GLuint)(1000*time);
            
			for (GLushort l = 0; l < 3; l++)
			{
				fread(&modelCore->joints[i].keyframeRotations[k].rotation[l], 4, 1, file);
				
				modelCore->joints[i].keyframeRotations[k].rotation[l] = 360.0f*modelCore->joints[i].keyframeRotations[k].rotation[l]/(2.0f*PIf);
			}
		}
        
		for (k = 0; k < modelCore->joints[i].numberKeyframeTranslations; k++)
		{
			GLfloat	time;			
			fread(&time, 4, 1, file);
			
			modelCore->joints[i].keyframeTranslations[k].time = (GLuint)(1000*time);
            
			for (GLushort l = 0; l < 3; l++)
			{
				fread(&modelCore->joints[i].keyframeTranslations[k].translation[l], 4, 1, file);
			}
		}
    }
}

GLvoid CMS3DLoader::calculateJointsf()
{
	GLfloat	rotationMatrix[16];
	GLfloat	quaternion[4];
    
	for (GLushort i = 0; i < modelCore->numberJoints; i++)
	{
		sglLoadIdentityf(modelCore->joints[i].jointMatrix);
		
		if (modelCore->joints[i].fatherJoint != -1)
		{
			// calculate in the father matrix
			sglMultMatrixf(modelCore->joints[i].jointMatrix, modelCore->joints[modelCore->joints[i].fatherJoint].jointMatrix, modelCore->joints[i].jointMatrix);
		}
        
		sglTranslateMatrixf(modelCore->joints[i].jointMatrix, joints[i].translation[0], joints[i].translation[1], joints[i].translation[2]);
		
		//
		// Use of quaternions
		//
		sglAngleToQuaternionf(quaternion, joints[i].rotation[0], joints[i].rotation[1], joints[i].rotation[2]);
		sglQuaternionToMatrixf(rotationMatrix, quaternion);
		
		sglMultMatrixf(modelCore->joints[i].jointMatrix, modelCore->joints[i].jointMatrix, rotationMatrix);
        
		//
		// We use Quaternions instead
		//
		//VectorMathf::rotateRzRyRxMatrixf(modelCore->joints[i].jointMatrix, joints[i].rotation[0], joints[i].rotation[1], joints[i].rotation[2]);
        
		sglInverseMatrixf(modelCore->joints[i].inverseJointMatrix, modelCore->joints[i].jointMatrix);
	}
}

SGLResult CMS3DLoader::loadModelCoref(const char* const filename, CModelData& currentModelCore,CTextureMan* pTextureMan)
{
	// reset the current modelCore
	currentModelCore.resetf();
    
	// check, if we have a valid pointer
	if (!filename)
		return SGL_NULL_POINTER_ERROR;
    
	// open filename in "read binary" mode
	file = fopen(filename, "rb");
	if (!file)
		return SGL_FILE_ERROR;
    
	modelCore = &currentModelCore;
    
	readHeaderf();
    
	readNumberVerticesf();
	readVerticesf();
    
	readNumberTrianglesf();
	readTrianglesf();
    
	readNumberGroupsf();
	readGroupsf();
    
	readNumberMaterialsf();
	readMaterialsf(pTextureMan);
    
	readKeyFrameDataf();
    
	readNumberJointsf();
	readJointsf();
    
	calculateJointsf();
    
	listOfJoints.clear();
	
	if (joints)
	{
		delete[] joints;
		joints = 0;
	}
	
	fclose(file);
	
	return SGL_NO_ERROR;		
}



SGLResult CMS3DLoader::LoadMS3D(const char* const filename, CModelData& modelCore,CTextureMan* pTextureMan)
{
	const char* extension = 0;
	char* copyOfExtension = 0;
	GLuint	index = 0;
	SGLResult	result;
	string fullFilename(CLoader::searchPath);
    
	// check, if we have a valid pointer
	if (!filename)
		return SGL_NULL_POINTER_ERROR;
    
	fullFilename = fullFilename + filename;
	
	// find the file extension
	extension = strrchr(filename, '.');
    
	if (!extension)
		return SGL_MODEL_NOT_SUPPORTED_ERROR;
	
	// prepare a copy of the file extension
	copyOfExtension = new char[strlen(extension)+1];
    
	if (!copyOfExtension)
		return SGL_OUT_OF_MEMORY_ERROR;
    
	// make a lowercase copy of the file extension
	do
	{
		copyOfExtension[index] = tolower(extension[index]);
	} while (extension[index++]);
    
	// load the model depending on the file extension
	if (strcmp(copyOfExtension, ".ms3d") == 0)
		result = CMS3DLoader::loadModelCoref(fullFilename.c_str(), modelCore,pTextureMan);
	else
		result = SGL_MODEL_NOT_SUPPORTED_ERROR;
    
	delete[] copyOfExtension;	
	return result;
}