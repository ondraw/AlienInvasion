//
//  CBound.h
//  SongGL
//
//  Created by Song Hohak on 11. 11. 24..
//  Copyright (c) 2011 thinkm. All rights reserved.
//

#ifndef SongGL_CBoundingSpheref_h
#define SongGL_CBoundingSpheref_h

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

#include "sGLDefine.h"
#include "sGLTrasform.h"
#include "CModelData.h"

#include <vector>
#include "CMS3DModel.h"
using namespace std;

/**
 * Bounding sphere node.
 */
class CBound
{
    
	friend class CModelBound;
	friend class CMS3DModel;
    
private:
    
	/**
	 * Internal counter how many spheres are created.
	 */
	static GLuint	counter;
    
	/**
	 * Field to store, if a triangle has already been tested.
	 */
	static GLubyte* check;
    
	/**
	 * The size of the field.
	 */
	static GLuint	fieldSize;
    
	/**
	 * The limit of the radius, when no octree should be made.
	 * This value is calculated automatically.
	 */
	static GLfloat  radiusLimit;
    
	/**
	 * The minimum point of the octree
	 */
	GLfloat	min[3];
	
	/**
	 * The maximum point of the octree
	 */
	GLfloat	max[3];
    
	/**
	 * The local center of the octree
	 */
	GLfloat localCenter[3];
    
	/**
	 * The local radius (not world space)
	 */
	GLfloat localRadius;
    
	/**
	 * The model.
	 */
	CMS3DModel*	model;
	
	/**
	 * The joint this bound sphere belongs to.
	 */
	GLbyte	joint;
	
	/**
	 * The triangles in this sphere.
	 */
	vector<CModelData::Trianglef*>	triangles;
    
	/**
	 * The childs/octree of this bounding sphere
	 */
	CBound*	octree[8];
    
	/**
	 * Constructor.
	 * 
	 * @param model the model this sphere belongs to
	 * @param joint the joint this sphere belongs to
	 */
	CBound(CMS3DModel* model, GLbyte joint);
	
	/**
	 * Destructor
	 */
	virtual ~CBound();
	
	/**
	 * Sorts the octree regarding a given point.
	 * Closer spheres of the octree are traversed first.
	 * 
	 * @param p	the point
	 */
	inline GLvoid sortOctreef(const GLfloat p[3])
	{
		CBound* currentBoundingSphere = 0;
		GLfloat a[3], b[3];
        
		for (GLbyte i = 7; i >= 0; i--)
		{
			for (GLbyte k = 7; k >= 7-i+1; k--)
			{
				if (octree[k])
				{
					octree[k]->getCenterf(a);
                    
					if (octree[k-1])
					{
						octree[k-1]->getCenterf(b);
					}
					if (!octree[k-1] || sglSquareDistancePointPointf(p, a) < sglSquareDistancePointPointf(p, b))
					{
						currentBoundingSphere = octree[k-1];
						octree[k-1] = octree[k];
						octree[k] = currentBoundingSphere;
					}
				}
			}
		}
	}
    
	/**
	 * Creates the octree with the given level.
	 * 
	 * @param currentLevel the current level of the octree
	 * @param level the level where to stop
	 */	 
	GLvoid makeOctreef(GLbyte currentLevel, GLbyte level);
    
	/**
	 * Method for checking intersection.
	 * 
	 * @param other	the other sphere to check against
	 * @param exact flag, if the collision should occur exact
	 * @param currentLevel the level the algorithm is currently searching
	 * @param stopAtLevel the level where to stop, -1 if until a primitive is found, if exact is GL_TRUE, this is ignored
	 * 
	 * @return GL_TRUE if collided
	 */
	virtual GLboolean collidesf(CBound* const other, GLboolean exact, GLbyte currentLevel, GLbyte stopAtLevel);
    
	/**
	 * Method for checking ray intersection.
	 *
	 * @param hitPoint if a collision occurs, the hit point is stored here
	 * @param start the origin of the ray
	 * @param point any point of the ray
	 * @param exact flag, if the intersection should occur exact
	 * @param currentLevel the level the algorithm is currently searching
	 * @param stopAtLevel the level where to stop, -1 if until a primitive is found, if exact is GL_TRUE, this is ignored
	 * 
	 * @return GL_TRUE if collided
	 */
	virtual GLboolean intersectsf(GLfloat hitPoint[3], GLfloat start[3], GLfloat point[3], GLboolean exact, GLbyte currentLevel, GLbyte stopAtLevel);
	
public:
    
	/**
	 * If the amount of triangles is less than this value,
	 * no sub octree is made. 
	 */
	static GLuint  triangleLimit;
    
	/**
	 * Returns the radius of this sphere in world coordinates.
	 * 
	 * @return the radius
	 */
	inline virtual GLfloat getRadiusf() const
	{
		//
		// Maybe the model is scaled, so we use the x scale as reference
		//
		return model->scale[0] * localRadius;
	}
    
    //Added By Song 2012.08.26 간단하게 검사하기 위해서 사용한다.(속도를 위해)
    inline virtual GLfloat getSimpleRadiusf() const
	{
		//
		// Maybe the model is scaled, so we use the x scale as reference
		//
		return model->scale[0] * localRadius;
	}
	
	/**
	 * Retrieves the center in world coordinates of this sphere.
	 * 
	 * @param center the array where to store the center
	 */
	inline GLvoid getCenterf(GLfloat center[3]) const
	{
		//
		// the center has to be recalculated upon world coordiantes and animation
		//
		if (joint == -1 || joint == -2)
		{
            //원래 문자 finalMatrix를 충돌 체크시 사용하지 말자...
			//sglMultMatrixVectorf(center, model->finalMatrix, localCenter);
            center[0] = model->position[0];
            center[1] = model->position[1];
            center[2] = model->position[2];
		}
		else
		{
			sglMultMatrixVectorf(center, model->joints[joint].finalMatrix, localCenter);
		}
	}
    
};

#endif
