//
//  BoundingSpheref.cpp
//  SongGL
//
//  Created by Song Hohak on 11. 11. 24..
//  Copyright (c) 2011 thinkm. All rights reserved.
//

#include <math.h>

#include "CBound.h"

GLuint	CBound::counter = 0;

GLubyte* CBound::check = 0;

GLuint CBound::fieldSize = 0;

GLuint  CBound::triangleLimit = 1;

GLfloat CBound::radiusLimit = 0.0f;

CBound::CBound(CMS3DModel* model, GLbyte joint)
{
	this->model = model;
	this->joint = joint;
	
	for (GLubyte i = 0; i < 8; i++)
	{
		octree[i] = 0;
	}
	
	localCenter[0] = 0.0f;
	localCenter[1] = 0.0f;
	localCenter[2] = 0.0f;
	
	localRadius = 0.0f;
	
	counter++;
}

CBound::~CBound()
{
	for (GLubyte i = 0; i < 8; i++)
	{
		if (octree[i])
		{
			delete octree[i];
			octree[i] = 0;
		}
	}
    
	counter--;
	
	if (counter == 0 && check)
	{
		delete[] check;
		check = 0;
        
        
        fieldSize = 0;
        triangleLimit = 1;
        radiusLimit = 0.0f;
        
	}
}

GLvoid CBound::makeOctreef(GLbyte currentLevel, GLbyte level)
{
	//
	// If we have this granularity, we can quit
	//
	if (triangles.size() <= triangleLimit)
		return;
	
	if (localRadius <= radiusLimit)
		return;
	
	if (currentLevel < level)
	{
		GLubyte i;
        
		// create 8 new bounding spheres	
		for (i = 0; i < 8; i++)
		{
			GLubyte	x = 0;
			GLubyte	y = 0;
			GLubyte	z = 0;
			
			octree[i] = new CBound(model ,joint);
            
			// calculate the new radius ...
			octree[i]->localRadius = localRadius / 2.0f;
            
			switch (i)
			{
				case 0:
					x = 0;
					y = 0;
					z = 0;
                    break;
				case 1:
					x = 1;
					y = 0;
					z = 0;
                    break;
				case 2:
					x = 0;
					y = 1;
					z = 0;
                    break;
				case 3:
					x = 1;
					y = 1;
					z = 0;
                    break;
				case 4:
					x = 0;
					y = 0;
					z = 1;
                    break;
				case 5:
					x = 1;
					y = 0;
					z = 1;
                    break;
				case 6:
					x = 0;
					y = 1;
					z = 1;
                    break;
				case 7:
					x = 1;
					y = 1;
					z = 1;
                    break;
			}
            
			if (x == 0)
			{
				octree[i]->localCenter[0] = (localCenter[0] + min[0]) / 2.0f;
				octree[i]->min[0] = min[0];
				octree[i]->max[0] = localCenter[0];
			}
			else
			{
				octree[i]->localCenter[0] = (localCenter[0] + max[0]) / 2.0f;
				octree[i]->min[0] = localCenter[0];
				octree[i]->max[0] = max[0];
			}
			if (y == 0)
			{
				octree[i]->localCenter[1] = (localCenter[1] + min[1]) / 2.0f;
				octree[i]->min[1] = min[1];
				octree[i]->max[1] = localCenter[1];
			}
			else
			{
				octree[i]->localCenter[1] = (localCenter[1] + max[1]) / 2.0f;
				octree[i]->min[1] = localCenter[1];
				octree[i]->max[1] = max[1];
			}
			if (z == 0)
			{
				octree[i]->localCenter[2] = (localCenter[2] + min[2]) / 2.0f;
				octree[i]->min[2] = min[2];
				octree[i]->max[2] = localCenter[2];
			}
			else
			{
				octree[i]->localCenter[2] = (localCenter[2] + max[2]) / 2.0f;
				octree[i]->min[2] = localCenter[2];
				octree[i]->max[2] = max[2];
			}
			
			// add the triangles
			for (GLushort k = 0; k < triangles.size(); k++)
			{
				GLfloat t[3*3];
				GLfloat closestPoint[3];
				GLfloat distance;
				
				for (GLubyte l = 0; l < 3; l++)
				{
					for (GLubyte m = 0; m < 3; m++)
					{
						t[l*3+m] = model->modelCore->localVertices[triangles[k]->vertexIndices[l]*3+m];
					}
				}
				
				sglClosestPointFromTriangleToPointf(closestPoint, &t[0], &t[3], &t[6], octree[i]->localCenter);
				
				distance = (GLfloat)sqrt((closestPoint[0]-octree[i]->localCenter[0])*(closestPoint[0]-octree[i]->localCenter[0]) + (closestPoint[1]-octree[i]->localCenter[1])*(closestPoint[1]-octree[i]->localCenter[1]) + (closestPoint[2]-octree[i]->localCenter[2])*(closestPoint[2]-octree[i]->localCenter[2]));
				if (distance <= octree[i]->localRadius)
				{
					octree[i]->triangles.push_back(triangles[k]);
				}
			}
			
			// if no triangles, remove node
			if (octree[i]->triangles.size() == 0)
			{
				delete octree[i];
				octree[i] = 0;
			}
			else
			{
				//if (currentLevel+1 == level)
				//	printf("Has %d triangles.\n", octree[i]->triangles.size());
			}
		}
        
		// make one step deeper
		for (i = 0; i < 8; i++)
		{
			if (octree[i])
			{
				octree[i]->makeOctreef(currentLevel+1, level);
			}
		}
        
		// remove triangles as they are now in the children
		triangles.clear();
		
	}
}

GLboolean CBound::collidesf(CBound* const other, GLboolean exact, GLbyte currentLevel, GLbyte stopAtLevel)
{
	GLfloat a[3];
	GLfloat b[3];
    
	if (other == 0)
		return GL_FALSE;
	
	getCenterf(a);
	other->getCenterf(b);
    
	if (sglSquareDistancePointPointf(a, b) <= (getRadiusf() + other->getRadiusf()) * (getRadiusf() + other->getRadiusf()))
	{
		if (stopAtLevel != -1 && !exact && currentLevel > stopAtLevel)
		{
			return GL_TRUE;
		}
		
		if (triangles.size() > 0 && other->triangles.size() > 0)
		{
			// do exact collision detection by checking triangle by triangle
			if (exact)
			{
				for (GLushort i = 0; i < triangles.size(); i++)
				{
					for (GLushort k = 0; k < other->triangles.size(); k++)
					{
						GLfloat t0[3*3];
						GLfloat t1[3*3];
                        
						GLuint index = (GLuint)triangles[i]->index + (GLuint)other->triangles[k]->index * model->modelCore->numberTriangles;
						
						GLuint arrayIndex = index / 8;
						GLubyte offset = index % 8;
						
						if (check[arrayIndex] & (1 << offset))
							continue;
						else
							check[arrayIndex] |= (1 << offset);
                        
						// go through all corners
						for (GLubyte l = 0; l < 3; l++)
						{
							GLushort indexTriangleOne = triangles[i]->vertexIndices[l];
							GLushort indexTriangleTwo = other->triangles[k]->vertexIndices[l];
                            
							// go through all coordinates
							for (GLubyte m = 0; m < 3; m++)
							{
								t0[l*3+m] = model->worldVertices[indexTriangleOne*3+m];
								
								t1[l*3+m] = other->model->worldVertices[indexTriangleTwo*3+m];
							}
						}
						
						if (sglTriangleTriangleIntersectionf(t0, t1))
						{
							return GL_TRUE;
						}
					}
					
				}
				
				return GL_FALSE;
			}
			
			return GL_TRUE;
		}
		else if (triangles.size() > 0)
		{
			other->sortOctreef(a);
            
			for (GLubyte i = 0; i < 8; i++)
			{
				if (other->octree[i])
				{
					GLfloat centerOther[3];
					GLfloat radiusOther;
					
					other->octree[i]->getCenterf(centerOther);
					radiusOther = other->octree[i]->getRadiusf();
                    
					if (sglSquareDistancePointPointf(a, centerOther) <= (getRadiusf()+radiusOther)*(getRadiusf()+radiusOther))
					{							
						if (collidesf(other->octree[i], exact, currentLevel+1, stopAtLevel))
						{
							return GL_TRUE;
						}
					}
				}
			}
		}
		else if (other->triangles.size() > 0)
		{
			sortOctreef(b);
            
			for (GLubyte i = 0; i < 8; i++)
			{
				if (octree[i])
				{
					GLfloat center[3];
					GLfloat radius;
					
					octree[i]->getCenterf(center);
					radius = octree[i]->getRadiusf();
                    
					if (sglSquareDistancePointPointf(center, b) <= (radius+other->getRadiusf())*(radius+other->getRadiusf()))
					{							
						if (other->collidesf(octree[i], exact, currentLevel+1, stopAtLevel))
						{
							return GL_TRUE;
						}
					}
				}
			}
		}
		else
		{
			sortOctreef(b);
			other->sortOctreef(a);
            
			for (GLubyte i = 0; i < 8; i++)
			{
				if (octree[i])
				{
					GLfloat center[3];
					GLfloat radius;
					
					octree[i]->getCenterf(center);
					radius = octree[i]->getRadiusf();
					
					for (GLubyte k = 0; k < 8; k++)
					{
						if (other->octree[k])
						{
							GLfloat centerOther[3];
							GLfloat radiusOther;
							
							other->octree[k]->getCenterf(centerOther);
							radiusOther = other->octree[k]->getRadiusf();
                            
							if (sglSquareDistancePointPointf(center, centerOther) <= (radius+radiusOther)*(radius+radiusOther))
							{							
								if (octree[i]->collidesf(other->octree[k], exact, currentLevel+1, stopAtLevel))
								{
									return GL_TRUE;
								}
							}
						}
					}
				}
			}
		}
	}
	
	return GL_FALSE;
}

GLboolean CBound::intersectsf(GLfloat hitPoint[3], GLfloat start[3], GLfloat point[3], GLboolean exact, GLbyte currentLevel, GLbyte stopAtLevel)
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
    
	if (stopAtLevel != -1 && !exact && currentLevel > stopAtLevel)
	{
		return GL_TRUE;
	}
	
	if (triangles.size() > 0)
	{
		// do exact intersection
		if (exact)
		{
			GLboolean result = GL_FALSE;
			GLfloat smallest = 0.0f;
            
			for (GLushort i = 0; i < triangles.size(); i++)
			{
				GLfloat triangle[3*3];
				GLfloat t, u, v, w;
                
				// go through all corners
				for (GLubyte l = 0; l < 3; l++)
				{
					GLushort indexTriangle = triangles[i]->vertexIndices[l];
                    
					// go through all coordinates
					for (GLubyte m = 0; m < 3; m++)
					{
						triangle[l*3+m] = model->worldVertices[indexTriangle*3+m];
					}
				}
                
				if (sglLinePlaneIntersectionf(start, point, &triangle[0], &triangle[3], &triangle[6], t, u, v, w))
				{								
					// Ray intersects plane
					if (t >= 0.0f)
					{
						// Ray intersects triangle
						if (v >= 0.0f && w >= 0.0f && (v+w) <= 1.0f)
						{
							// a closer intersection point
							if (!result || t < smallest)
							{
								smallest = t;
                                
								if (hitPoint)
								{
									GLfloat temp[3];
                                    
									sglCopyf(hitPoint, &triangle[0]);
									sglMultiplyf(hitPoint, u);
                                    
									sglCopyf(temp, &triangle[3]);
									sglMultiplyf(temp, v);
									sglAddf(hitPoint, hitPoint, temp);
                                    
									sglCopyf(temp, &triangle[6]);
									sglMultiplyf(temp, w);
									sglAddf(hitPoint, hitPoint, temp);
								}
                                
								result = GL_TRUE;
							}
						}
					}
				}
				
			}
            
			return result;
		}
		
		return GL_TRUE;
	}
	else
	{
		// go deeper
		sortOctreef(start);
        
		for (GLubyte i = 0; i < 8; i++)
		{
			if (octree[i])
			{
				if (octree[i]->intersectsf(hitPoint, start, point, exact, currentLevel+1, stopAtLevel))
				{
					return GL_TRUE;
				}
			}
		}
	}
	
	return GL_FALSE;
}