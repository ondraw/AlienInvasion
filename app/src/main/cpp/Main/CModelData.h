//
//  CModelData.h
//  SongGL
//
//  Created by Song Hohak on 11. 11. 24..
//  Copyright (c) 2011 thinkm. All rights reserved.
//

#ifndef SongGL_CModelData_h
#define SongGL_CModelData_h

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

#include <string>

#ifdef ANDROID
class CModelMan;
#endif

class CModelData
{
    
	friend class CBound;	
	friend class CModelBound;	
	friend class CMS3DModel;	
	friend class CModelCoreLoader;	
	friend class CMS3DLoader;
	friend class CAniLoader;
    
#ifdef ANDROID
    friend class CModelMan;
#endif
private:
    
	/**
	 * Triangle
	 */
	class Trianglef
	{
		
	public:
        
		/**
		 * Index of the triangle
		 */
		GLushort	index;
        
		/**
		 * The three indices of the triangle.
		 */
	    GLushort    vertexIndices[3];                           
	    
	    /**
	     * The three normals of the triangle.
	     */
	    GLfloat   	normals[3*3];
	    
	    /**
	     * The texture coordinates.
	     */
	    GLfloat   	texCoord[2*3];
	};
    
	/**
	 * Group
	 */
	class Groupf
	{
		
	public:
        
		/**
		 * Number of triangles in this group.
		 */
		GLushort	numberTriangles;
        
		/**
		 * Array of triangle indices.
		 */
		GLushort*	trianglesIndices;
        
		/**
		 * The associated material of the group.
		 */
		GLbyte		materialIndex;
		
		/**
		 * Constructor
		 */
		Groupf();
		
		/**
		 * Destructor
		 */
		virtual ~Groupf();
		
	};
    
	/**
	 * Material
	 */
	class Materialf
	{
		
	public:
        
		/**
		 * Ambient color.
		 */
		GLfloat		ambient[4];
		
		/**
		 * Diffuse color.
		 */
		GLfloat		diffuse[4];
		
		/**
		 * Specular color.
		 */
		GLfloat		specular[4];
		
		/**
		 * Emission color.
		 */
		GLfloat		emission[4];
		
		/**
		 * Shininess.
		 */
		GLfloat		shininess;
		
		/**
		 * Flaf, if texture is available.
		 */
		GLboolean	textureAvailable;
		
		/**
		 * Array of texture names.
		 */
		GLuint		textureName;
        
#ifdef ANDROID
        std::string      textrueFileName;
#endif
	};	
    
	/**
	 * Joint
	 */
	class Jointf
	{
		
	public:
		
		/**
		 * Rotation of a keyframe.
		 */
		class KeyframeRotationf
		{
			
		public:
            
			/**
			 * The time in milliseconds.
			 */
			GLuint	time;
			
			/**
			 * The rotation
			 */
			GLfloat	rotation[3];
		};
		
		/**
		 * Translation of a keyframe.
		 */
		class KeyframeTranslationf
		{
			
		public:
			
			/**
			 * The time in milliseconds.
			 */
			GLuint	time;
            
			/**
			 * The translation.
			 */
			GLfloat	translation[3];
		};
        
		/**
		 * Index of the father joint. -1 if root.
		 */
		GLbyte	fatherJoint;
        
		/**
		 * The matrix of the joint.
		 */
		GLfloat	jointMatrix[16];
        
		/**
		 * The inverse matrix of the joint.
		 */
		GLfloat	inverseJointMatrix[16];
        
		/**
		 * Number of keyframe rotations.
		 */
		GLushort	numberKeyframeRotations;
		
		/**
		 * Array of keyframe rotations.
		 */
		KeyframeRotationf*	keyframeRotations;		
        
		/**
		 * Number of keyframe translations.
		 */
		GLushort	numberKeyframeTranslations;
        
		/**
		 * Array of keyframe translations.
		 */
		KeyframeTranslationf*	keyframeTranslations;
		
		/**
		 * Constructor
		 */
		Jointf();
		
		/**
		 * Destructor
		 */
		virtual ~Jointf();
		
	};
	
	/**
	 * Frames per second
	 */
	GLuint		framesPerSecond;
	
	/**
	 * Number of frames
	 */
	GLuint		lastFrame;
    
	/**
	 * Number of vertices
	 */
	GLushort	numberVertices;
	
	/**
	 * Dynamic array with 3 coordinates per vertex
	 */
	GLfloat*	localVertices;
    
	/**
	 * Dynamic array with 1 index per vertex
	 */
	GLbyte*		jointsOfVertices;	
    
	/**
	 * Number of triangles
	 */
	GLushort	numberTriangles;
	
	/**
	 * Dynamic array
	 */
	Trianglef*	triangles;
    
	/**
	 * Number of groups
	 */
	GLushort	numberGroups;
	
	/**
	 * Groups
	 */
	Groupf*		groups;
    
	/**
	 * Number of materials
	 */
	GLushort	numberMaterials;
    
	/**
	 * Materials
	 */
	Materialf*	materials;
    
	/**
	 * Number of joints
	 */
	GLushort	numberJoints;
    
	/**
	 * Joints
	 */
	Jointf*		joints;
	
public:
	
	/**
	 * Constructor.
	 */
	CModelData();
	
	/**
	 * Destructor.
	 */
	virtual ~CModelData();
	
	/**
	 * Resets and cleans up the model.
	 */
	GLvoid		resetf();
};

#endif
