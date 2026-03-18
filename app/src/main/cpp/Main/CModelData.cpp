//
//  CModelData.cpp
//  SongGL
//
//  Created by Song Hohak on 11. 11. 24..
//  Copyright (c) 2011 thinkm. All rights reserved.
//

#include <iostream>
#include "CModelData.h"


CModelData::Groupf::Groupf() :
numberTriangles(0),
trianglesIndices(0),
materialIndex(-1)
{
}

CModelData::Groupf::~Groupf()
{
	if (trianglesIndices)
	{
		delete[] trianglesIndices;
	}
}

CModelData::Jointf::Jointf() : 
numberKeyframeRotations(0),
keyframeRotations(0),
numberKeyframeTranslations(0),
keyframeTranslations(0)
{
}

CModelData::Jointf::~Jointf()
{
	if (keyframeRotations)
	{
		delete[] keyframeRotations;
	}
	
	if (keyframeTranslations)
	{
		delete[] keyframeTranslations;
	}
}

CModelData::CModelData() :
numberVertices(0),
localVertices(0),
jointsOfVertices(0),	
numberTriangles(0),
triangles(0),
numberGroups(0),
groups(0),
numberMaterials(0),	 
materials(0),
numberJoints(0),
joints(0)
{
}

CModelData::~CModelData()
{
	resetf();
}

GLvoid CModelData::resetf()
{
	// free memory
	numberVertices = 0;
	
	if (localVertices)
	{
		delete[] localVertices;
		
		localVertices = 0;
	}
    
	if (jointsOfVertices)
	{
		delete[] jointsOfVertices;	
		
		jointsOfVertices = 0;	
	}
    
	if (triangles)
	{
		delete[] triangles;
		
		numberTriangles = 0;
		triangles = 0;
	}
    
	if (groups)
	{
		delete[] groups;
		
		numberGroups = 0;
		groups = 0;
	}
    
	if (materials)
	{
		delete[] materials;
		
		numberMaterials = 0;	 
		materials = 0;
	}
    
	if (joints)
	{
		delete[] joints;
		
		numberJoints = 0;
		joints = 0;
	}
    
}