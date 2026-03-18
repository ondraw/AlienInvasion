//
//  DynamicBoundingSpheref.h
//  SongGL
//
//  Created by Song Hohak on 11. 11. 24..
//  Copyright (c) 2011 thinkm. All rights reserved.
//

#ifndef SongGL_DynamicBoundingSpheref_h
#define SongGL_DynamicBoundingSpheref_h
#include <map>
#include "CBound.h"
#include "CListMap.h"


/**
 * Dynamic bounding sphere, which can resize during runtime.
 */
class CModelBound : private CBound
{
	friend class CMS3DModel;
private:
    
	/**
	 * Hierarchy level of bounding spheres.
	 */
	GLbyte level;
    
	/**
	 * Number of joints.
	 */
	GLushort numberJoints;
    
	/**
	 * List of bounding spheres by joint.
	 */
	CListMap<GLbyte, CBound*>	spheres;
    
	/**
	 * Sorts the joints/spheres against a given point.
	 * Closest spheres are used first.
	 * 
	 * @param p the point to sort against
	 */
	inline GLvoid sortJointsf(const GLfloat p[3])
	{
		GLbyte	currentJoint;
		CBound* currentBoundingSphere = 0;	
		GLfloat a[3], b[3];
        
		for (GLbyte i = (GLint)spheres.size()-1; i >= 0; i--)
		{
			for (GLbyte k = (GLint)spheres.size()-1; k >= (GLint)spheres.size()-i+1; k--)
			{
				spheres.getValues()[k]->getCenterf(a);
                
				spheres.getValues()[k-1]->getCenterf(b);
				
				if (sglSquareDistancePointPointf(p, a) < sglSquareDistancePointPointf(p, b))
				{
					currentJoint = spheres.getKeys()[k-1];
					currentBoundingSphere = spheres.getValues()[k-1];
					
					spheres.getKeys()[k-1] = spheres.getKeys()[k];
					spheres.getValues()[k-1] = spheres.getValues()[k];
					
					spheres.getKeys()[k] = currentJoint;
					spheres.getValues()[k] = currentBoundingSphere;
				}
			}
		}
	}
    
protected:
    
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
    
    inline virtual GLfloat getUpdateDistanceSprite() { return updateDistanceSprite;}
    
    /**
     * Added By Song 객체의 충돌때 상대와의 거리를 알아온다.
     */
    GLfloat     updateDistanceSprite; //충돌 체크할때 상대와의 거리.
    
	/**
	 * Flag, if the radius should be updated.
	 */
	GLboolean	updateRadius;
    
	/**
	 * Constructor
	 * 
	 * @param model the model from where to create the dynamic bounding sphere.
	 * @param level the level how deep the sphere hierarchy should be
	 */	
	CModelBound(CMS3DModel* model, GLbyte level);
	
	/**
	 * Destructor
	 */
	virtual ~CModelBound();
	
	/**
	 * Returns the radius of the dynamic bounding sphere.
	 * By default, recalculation is off.
	 * 
	 * @return the radius
	 */	
	inline virtual GLfloat getRadiusf()
	{
		// if wanted update local radius upon animation
		if (updateRadius)
		{
			for (GLushort i = 0; i < spheres.size(); i++)
			{
				if (i == 0)
				{
					CBound::localRadius = sglDistancePointPointf(CBound::localCenter, spheres.getValues()[i]->localCenter) + spheres.getValues()[i]->localRadius;
				}
				else
				{
					GLfloat distance = sglDistancePointPointf(CBound::localCenter, spheres.getValues()[i]->localCenter);
					
					if (distance + spheres.getValues()[i]->localRadius > CBound::localRadius)
					{
						CBound::localRadius = distance + spheres.getValues()[i]->localRadius;
					}	
				}
			}
		}
		
		return CBound::getRadiusf();
	}
    
    inline virtual GLfloat getSimpleRadiusf() const { return CBound::getSimpleRadiusf();}
    
    
	/**
	 * Intersection method using bounding spheres between this and another model.
	 * 
	 * @param search array of joints which should be considered in this search
	 * @param searchSize size of this array
	 * @param hitJoint if a collision occurs, the hit joint is stored here
	 * @param other the sphere to check against
	 * @param searchOther array of joints which should be considered in this search from the other sphere
	 * @param searchSizeOther size of this array
	 * @param hitJointOther if a collision occurs, the hit hoint of the other sphere is stored here
	 * @param exact flag, if intersection should be exact (triangle-triangle) or not (sphere-sphere)
	 * @param stopAtLevel level of the hierarchy to stop the search, if -1 one all spheres are considered; ignored, when exact is true, -2 only the dynamic bounding sphere is considered
	 * 
	 * @return true, if a collision did occur
	 */	
	GLboolean collidesBoundingSpheref(GLbyte* search, GLubyte searchSize, GLbyte* hitJoint, CModelBound* other, GLbyte* searchOther, GLubyte searchSizeOther, GLbyte* hitJointOther, GLboolean exact, GLbyte stopAtLevel);
    
	/**
	 * Intersection of a ray with a model.
	 * 
	 * @param search array of joints which should be considered in this search
	 * @param searchSize size of this array
	 * @param hitJoint if a collision occurs, the hit joint is stored here
	 * @param hitPoint if a collision occurs, the hit point is stored here
	 * @param start the origin of the ray
	 * @param point any point of the ray
	 * @param exact flag, if intersection should be exact (triangle-triangle) or not (sphere-sphere)
	 * @param stopAtLevel level of the hierarchy to stop the search, if -1 one all spheres are considered; ignored, when exact is true, -2 only the dynamic bounding sphere is considered
	 * 
	 * @return true, if a intersection did occur
	 */	
	GLboolean intersectsBoundingSpheref(GLbyte* search, GLubyte searchSize, GLbyte* hitJoint, GLfloat hitPoint[3], GLfloat start[3], GLfloat point[3], GLboolean exact, GLbyte stopAtLevel);
    
    
};


#endif
