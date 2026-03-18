//
//  DynamicBoundingSpheref.cpp
//  SongGL
//
//  Created by Song Hohak on 11. 11. 24..
//  Copyright (c) 2011 thinkm. All rights reserved.
//

#include <math.h>
#include "CModelBound.h"

CModelBound::CModelBound(CMS3DModel* model, GLbyte level) :
CBound(model, -2)
{
	this->level = level;
	
	if (model)
	{
		if(model->modelCore == 0) return ;
		numberJoints = model->modelCore->numberJoints;
        
		// Go through the model vertices and calculate bounding box
		for (GLushort index = 0; index < model->modelCore->numberVertices; index++)
		{
			// for the dynamic bounding sphere
            
			if (index == 0)
			{
				for (GLushort i = 0; i < 3; i++)
				{
					min[i] = model->modelCore->localVertices[index*3+i];
					max[i] = min[i];
				}				
			}
			else
			{
				for (GLushort i = 0; i < 3; i++)
				{
					if (model->modelCore->localVertices[index*3+i] < min[i])
						min[i] = model->modelCore->localVertices[index*3+i];
					if (model->modelCore->localVertices[index*3+i] > max[i])
						max[i] = model->modelCore->localVertices[index*3+i];
				}
			}
		}
        
		for (GLushort i = 0; i < 3; i++)
		{
			localCenter[i] = (min[i] + max[i]) / 2.0f;
		}
		
		localRadius = (GLfloat)sqrt((max[0]-localCenter[0])*(max[0]-localCenter[0]) + (max[1]-localCenter[1])*(max[1]-localCenter[1]) + (max[2]-localCenter[2])*(max[2]-localCenter[2]));
        
		//
		// Limit the creation of subtree
		//
		if (level > 0)
		{
			radiusLimit = localRadius / (1 << level);
		}
        
		//
		// Only create if level is larger -1
		//
		if (level > -1)
		{
			GLbyte joint;
            
			// Create the spheres upon the number of joints plus the default one
			for (joint = -1; joint < numberJoints; joint++)
			{	
				CBound* currentBoundingSphere;
				
				currentBoundingSphere = new CBound(model, joint);
				
				// add to the map of spheres
				spheres.put(joint, currentBoundingSphere);				
			}
			
			// go through all triangles
			for (GLushort i = 0; i < model->modelCore->numberTriangles; i++)
			{
				// add triangle (up to three) to associated bounding sphere
				for (GLushort k = 0; k < 3; k++)
				{
					CBound* currentBoundingSphere;
					
					GLbyte joint = model->modelCore->jointsOfVertices[model->modelCore->triangles[i].vertexIndices[k]];
					
					currentBoundingSphere = spheres.get(joint);
					
                    int xSize = (int)currentBoundingSphere->triangles.size();
                    bool bIsX = false;
                    for (int x = 0; x < xSize; x++) 
                    {
                        if(currentBoundingSphere->triangles[x] == &model->modelCore->triangles[i])
                        {
                            bIsX = true;
                            break;
                        }
                    }
					// add if not in the list
					if (!bIsX)
					{
						currentBoundingSphere->triangles.push_back(&model->modelCore->triangles[i]);
					}
				}
                
			}
            
			// calculate center and radius
			// Create the spheres upon the number of joints plus the default one
			for (joint = -1; joint < numberJoints; joint++)
			{	
				CBound* currentBoundingSphere = spheres.get(joint);
                
				//
				// Remove the spheres with no triangles
				//
				if (currentBoundingSphere->triangles.size() == 0)
				{
					delete currentBoundingSphere;
					
					spheres.remove(joint);
					
					continue;
				}
                
				//
				// Go through all triangles
				//
				for (GLushort index = 0; index < currentBoundingSphere->triangles.size(); index++)
				{
					CModelData::Trianglef* currentTriangle = currentBoundingSphere->triangles[index];
                    
					//
					// Go through all corners
					//				
					for (GLushort i = 0; i < 3; i++)
					{
						//
						// Calculate the min and max
						//
						if (index == 0 && i == 0)
						{
							for (GLushort k = 0; k < 3; k++)
							{
								currentBoundingSphere->min[k] = model->modelCore->localVertices[currentTriangle->vertexIndices[i]*3+k];
								currentBoundingSphere->max[k] = model->modelCore->localVertices[currentTriangle->vertexIndices[i]*3+k];
							}
						}
						else
						{
							for (GLushort k = 0; k < 3; k++)
							{
								if (model->modelCore->localVertices[currentTriangle->vertexIndices[i]*3+k] < currentBoundingSphere->min[k])
									currentBoundingSphere->min[k] = model->modelCore->localVertices[currentTriangle->vertexIndices[i]*3+k];
								if (model->modelCore->localVertices[currentTriangle->vertexIndices[i]*3+k] > currentBoundingSphere->max[k])
									currentBoundingSphere->max[k] = model->modelCore->localVertices[currentTriangle->vertexIndices[i]*3+k];
							}
						}				
					}
				}
				
				//
				// Calculate the center and ...
				//
				for (GLushort i = 0; i < 3; i++)
				{
					currentBoundingSphere->localCenter[i] = (currentBoundingSphere->min[i] + currentBoundingSphere->max[i]) / 2.0f;
				}
				
				// ... the radius
				currentBoundingSphere->localRadius = (GLfloat)sqrt((currentBoundingSphere->max[0]-currentBoundingSphere->localCenter[0])*(currentBoundingSphere->max[0]-currentBoundingSphere->localCenter[0]) + (currentBoundingSphere->max[1]-currentBoundingSphere->localCenter[1])*(currentBoundingSphere->max[1]-currentBoundingSphere->localCenter[1]) + (currentBoundingSphere->max[2]-currentBoundingSphere->localCenter[2])*(currentBoundingSphere->max[2]-currentBoundingSphere->localCenter[2]));
			}
            
			// for all bounding spheres
			for (joint = -1; joint < numberJoints; joint++)
			{	
				CBound* currentBoundingSphere = spheres.get(joint);
				
				if (currentBoundingSphere)
				{			
					currentBoundingSphere->makeOctreef(0, level);
				}
			} 
		}
	}									
	else
	{
		//
		// Invalid 
		//		
		localCenter[0] = 0.0f;
		localCenter[1] = 0.0f;
		localCenter[2] = 0.0f;
		
		localRadius = 0.0f;
	}
}

CModelBound::~CModelBound()
{
	// cleanup the bounding sphere list
	for (GLbyte joint = -1; joint < numberJoints; joint++)
	{	
		CBound* currentBoundingSphere;
		
		currentBoundingSphere = spheres.get(joint);
		
		if (currentBoundingSphere)
		{	
			delete currentBoundingSphere;
		}
	}
    
	spheres.clear();
}

GLboolean CModelBound::collidesf(CBound* const other, GLboolean exact, GLbyte currentLevel, GLbyte stopAtLevel)
{
    GLfloat fR;
	GLfloat a[3];
	GLfloat b[3];
	
    if(exact)
    {
        if (other == 0)
            return GL_FALSE;
        
        getCenterf(a);
        other->getCenterf(b);
        
        updateDistanceSprite = sglSquareDistancePointPointf(a, b);
       return updateDistanceSprite <= (getRadiusf() + other->getRadiusf()) * (getRadiusf() + other->getRadiusf());
    }
    else
    {
        getCenterf(a);
        other->getCenterf(b);
        
        updateDistanceSprite = sglSquareDistancePointPointf(a, b);
        fR = (getSimpleRadiusf() + other->getSimpleRadiusf()) / 2.0f; //2.0f를 한이유는 객체가 서로 부딛히는 비율을 조금 작게 하였다.
        return updateDistanceSprite <= (fR * fR);
    }
}

GLboolean CModelBound::collidesBoundingSpheref(GLbyte* search, GLubyte searchSize, GLbyte* hitJoint, CModelBound* other, GLbyte* searchOther, GLubyte searchSizeOther, GLbyte* hitJointOther, GLboolean exact, GLbyte stopAtLevel)
{
	GLfloat q[3];
	
    if (other == 0)
		return GL_FALSE;
    

    //정확하게 충돌검사시
    if(exact)
    {
        if (!collidesf(other, exact, 1, stopAtLevel))
            return GL_FALSE;
    }
    else //비정확하지만 속도를 위해 충돌검사시....
    {
        if (collidesf(other, exact, 1, stopAtLevel))
            return GL_TRUE;
        else
            return GL_FALSE;
    }
    
	if (spheres.size() == 0 || other->spheres.size() == 0)
		return GL_FALSE;
    
	if (stopAtLevel == -2 && !exact)
	{
		return GL_TRUE;
	}
    
	other->getCenterf(q);				
	sortJointsf(q);
    
	if (search == 0 || searchSize == 0)
	{
		for (GLubyte i = 0; i < spheres.size(); i++)
		{
			GLfloat p[3];
            
			CBound* currentBoundingSphere = spheres.getValues()[i];
			currentBoundingSphere->getCenterf(p);
			
			if (searchOther == 0 || searchSizeOther == 0)
			{
				
				other->sortJointsf(p);
				
				for (GLubyte k = 0; k < other->spheres.size(); k++)
				{	
					GLfloat	center[3];
					GLfloat radius;
					
					other->spheres.getValues()[k]->getCenterf(center);
					radius = other->spheres.getValues()[k]->getRadiusf();
					
					if (sglSquareDistancePointPointf(p, center) <= (currentBoundingSphere->getRadiusf() + radius)*(currentBoundingSphere->getRadiusf() + radius))
					{					
						if (currentBoundingSphere->collidesf(other->spheres.getValues()[k], exact, 1, stopAtLevel))
						{
							if (hitJoint)
							{
								*hitJoint = spheres.getKeys()[i];
							}
							if (hitJointOther)
							{
								*hitJointOther = other->spheres.getKeys()[k];
							}
							
							return GL_TRUE;
						}
					}
				}
			}
			else
			{
				for (GLubyte k = 0; k < searchSizeOther; k++)
				{
					GLfloat	center[3];
					GLfloat radius;
					
					other->spheres.get(searchOther[k])->getCenterf(center);
					radius = other->spheres.get(searchOther[k])->getRadiusf();
					
					if (sglSquareDistancePointPointf(p, center) <= (currentBoundingSphere->getRadiusf() + radius)*(currentBoundingSphere->getRadiusf() + radius))
					{					
						if (currentBoundingSphere->collidesf(other->spheres.get(searchOther[k]), exact, 1, stopAtLevel))
						{
							if (hitJoint)
							{
								*hitJoint = spheres.getKeys()[i];
							}
							if (hitJointOther)
							{
								*hitJointOther = searchOther[k];
							}
                            
							return GL_TRUE;
						}
					}
				}
			}
		}		
	}
	else
	{
		for (GLubyte i = 0; i < searchSize; i++)
		{
			GLfloat p[3];
			
			CBound* currentBoundingSphere = spheres.get(search[i]);
            if(currentBoundingSphere == 0) 
            {
                HLog("첫번째 조인트에 바운즈 정점을 할당해야한다.");
                continue;
            }
			currentBoundingSphere->getCenterf(p);
            
			if (searchOther == 0 || searchSizeOther == 0)
			{				
				other->sortJointsf(p);
                
				for (GLubyte k = 0; k < other->spheres.size(); k++)
				{
					GLfloat	center[3];
					GLfloat radius;
					
					other->spheres.getValues()[k]->getCenterf(center);
					radius = other->spheres.getValues()[k]->getRadiusf();
					
					if (sglSquareDistancePointPointf(p, center) <= (currentBoundingSphere->getRadiusf() + radius)*(currentBoundingSphere->getRadiusf() + radius))
					{					
						if (currentBoundingSphere->collidesf(other->spheres.getValues()[k], exact, 1, stopAtLevel))
						{
							if (hitJoint)
							{
								*hitJoint = search[i];
							}
							if (hitJointOther)
							{
								*hitJointOther = other->spheres.getKeys()[k];
							}
							return GL_TRUE;
						}
					}
				}
			}
			else
			{
				for (GLubyte k = 0; k < searchSizeOther; k++)
				{
					GLfloat	center[3];
					GLfloat radius;
					
					other->spheres.get(searchOther[k])->getCenterf(center);
					radius = other->spheres.get(searchOther[k])->getRadiusf();
					
					if (sglSquareDistancePointPointf(p, center) <= (currentBoundingSphere->getRadiusf() + radius)*(currentBoundingSphere->getRadiusf() + radius))
					{					
						if (currentBoundingSphere->collidesf(other->spheres.get(searchOther[k]), exact, 1, stopAtLevel))
						{
							if (hitJoint)
							{
								*hitJoint = search[i];
							}
							if (hitJointOther)
							{
								*hitJointOther = searchOther[k];
							}
							return GL_TRUE;
						}
					}
				}
			}
		}
	}
	
	return GL_FALSE;
}

GLboolean CModelBound::intersectsf(GLfloat hitPoint[3], GLfloat start[3], GLfloat point[3], GLboolean exact, GLbyte currentLevel, GLbyte stopAtLevel)
{
	GLfloat c[3];
	GLfloat closest[3];
	GLfloat ab[3];
	GLfloat ac[3];
	GLfloat dot;
    
	// Calculate closest point and check distance. Return if not in range
	getCenterf(c);
    
	sglSubf(ab, point, start);
	sglSubf(ac, c, start);
    
	sglNormalizef(ab);
    
	dot = sglDotProductf(ab, ac);
    
	sglCopyf(closest, ab);
    
	sglMultiplyf(closest, dot);
	sglAddf(closest, closest, start);
    
	// Ray is not intersecting this sphere
	if (sglSquareDistancePointPointf(closest, c) > getRadiusf() * getRadiusf())
	{
		return GL_FALSE;
	}
	else if (dot < 0.0f)
	{
		// Ray starts after the sphere
		if (sglSquareDistancePointPointf(start, c) > getRadiusf() * getRadiusf())
		{
			return GL_FALSE;
		}
	}
    
	return GL_TRUE;
}

GLboolean CModelBound::intersectsBoundingSpheref(GLbyte* search, GLubyte searchSize, GLbyte* hitJoint, GLfloat hitPoint[3], GLfloat start[3], GLfloat point[3], GLboolean exact, GLbyte stopAtLevel)
{
	if (!intersectsf(hitPoint, start, point, exact, 1, stopAtLevel))
		return GL_FALSE;
    
	if (spheres.size() == 0)
		return GL_FALSE;
    
	if (stopAtLevel == -2 && !exact)
	{
		return GL_TRUE;
	}
    
	sortJointsf(start);
    
	if (search == 0 || searchSize == 0)
	{
		for (GLubyte i = 0; i < spheres.size(); i++)
		{
			CBound* currentBoundingSphere = spheres.getValues()[i];
            
			if (currentBoundingSphere->intersectsf(hitPoint, start, point, exact, 1, stopAtLevel))
			{
				if (hitJoint)
				{
					*hitJoint = spheres.getKeys()[i];
				}
				
				return GL_TRUE;
			}
		}		
	}
	else
	{
		for (GLubyte i = 0; i < searchSize; i++)
		{
			CBound* currentBoundingSphere = spheres.get(search[i]);
            
			if (currentBoundingSphere->intersectsf(hitPoint, start, point, exact, 1, stopAtLevel))
			{
				if (hitJoint)
				{
					*hitJoint = spheres.getKeys()[i];
				}
				
				return GL_TRUE;
			}
		}
	}
	
	return GL_FALSE;
    
}