//
//  Modelf.h
//  SongGL
//
//  Created by Song Hohak on 11. 11. 24..
//  Copyright (c) 2011 thinkm. All rights reserved.
//

#ifndef SongGL_Modelf_h
#define SongGL_Modelf_h
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
#include "sGL.h"


typedef GLenum	SGLResult;

#define		SGL_NO_ERROR						0
#define		SGL_NULL_POINTER_ERROR				1
#define		SGL_FILE_ERROR						2
#define		SGL_OUT_OF_MEMORY_ERROR			3
#define		SGL_TEXTURE_NOT_SUPPORTED_ERROR	4
#define		SGL_MODEL_NOT_SUPPORTED_ERROR		5
#define		SGL_ANIMATION_NOT_SUPPORTED_ERROR	6
#define		SGL_DATA_CORRUPTED_ERROR			7
#define		SGL_MAP_NOT_SUPPORTED_ERROR		8
#define		SGL_NO_HEIGHTMAP					9
#define		SGL_NOT_ENOUGH_TEXTURE_UNITS		10
#define		SGL_GENERATED_KEY_IN_USE			11
#define		SGL_OUT_OF_BOUNDS_ERROR			12
#define		SGL_NOT_ENOUGH_TILES				13
#define		SGL_INVALID_VALUE					14

#define		SGL_NO_LIGHT						2000
#define		SGL_HARDWARE_LIGHT					2001
#define		SGL_SIMULATED_LIGHT				2002
#define		SGL_POSITIONAL_LIGHT				2003
#define		SGL_DIRECTIONAL_LIGHT				2004


//#include "Error.h"
#include "CModelData.h"
#include "CMyModel.h"
class CPicking;
class CModelBound;
class CMS3DModel : public CMyModel
{
    
	friend class CAniLoader;
	friend class CBound;
	friend class CModelBound;
    
private:
    
	/**
	 * Triangle
	 */
	class Trianglef
	{
		
	public:
        
	    /**
	     * The normals in world coordinates.
	     */
	    GLfloat		worldNormals[3*3];
	};
    
	/**
	 * Group
	 */
	class Groupf
	{
		
	public:
        
		/**
		 * The vertices.
		 */
		GLfloat*	vertices;
        
		/**
		 * The normals.
		 */
		GLfloat*	normals;
        
		/**
		 * Texture coordiantes
		 */
		GLfloat*	texCoord;
        
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
	 * Joint
	 */
	class Jointf
	{		
	public:
		/**
		 * The final matrix of the joint.
		 */
		GLfloat	finalMatrix[16];
	};
    
	/**
	 * Animation data.
	 */
	class Animationf
	{
		
	public:
        
		/**
		 * Start in milliseconds.
		 */		
		GLuint	start;
		
		/**
		 * End in milliseconds.
		 */
		GLuint	end;
        
		/**
		 * Flag, if the animation should be looped or not.
		 */
		GLboolean	loop;
	};
    
	/**
	 * Pointer to the core model data.
	 */
	CModelData*	modelCore;
    
	/**
	 * Triangles
	 */
	Trianglef*	triangles;
    
	/**
	 * Groups
	 */
	Groupf*	groups;
    
	/**
	 * Joints
	 */
	Jointf*		joints;
    
	/**
	 * Number animations
	 */
	GLushort	numberAnimations;
    
	/**
	 * Animations
	 */
	Animationf*	animations;
    
	/**
	 * Dynamic array with 3 coordinates per vertex - world coordinates
	 */
	GLfloat*	worldVertices;
	
	/**
	 * Flag, if the animation is still running. A looping
	 * animation is always running.
	 */	
	GLboolean	running;
    
	/**
	 * The current animation. -1 if none
	 */
	GLshort		currentAnimation;
	
	/**
	 * The next animation to play.
	 */
	GLshort		nextAnimation;
    
	/**
	 * Elapsed time
	 */
	GLuint		elapsedTime;		
	
	/**
	 * The final matrix. Local to world coordiantes are calculated
	 * with this matrix.
	 */
	GLfloat		finalMatrix[16];
    
    
    //Added By Song 2011.04.07
    //finalMatrix와 finalBeforeMatrix가 같으면 움직임 또는 애니메이션 작업이 없어서 
    //world정점을 구할 필요가 없다.
    GLfloat     finalBeforeMatrix[16];
	
    

public:
    /**
	 * Constructor
	 */
	CMS3DModel();
	
	/**
	 * Destructor
	 */
	virtual ~CMS3DModel();

    virtual MODELTYPE GetType() {return MODELTYPE_MS3D;}
    virtual void GetCenter(GLfloat *fOutCenter);
    virtual bool    IsPicking(CPicking* pPick,int nWinPosX,int nWinPosY);
    virtual bool    IsPicking(CPicking* pPick,SPoint& nearPoint,SPoint& farPoint,int nWinPosX,int nWinPosY);
    //주인공은 버텍스 갯수가 너무 많아서 그룹 0만 검색하자.
    virtual bool IsPickingZeroGroup(CPicking* pPick,int nWinPosX,int nWinPosY);
    
    
    
	   
    /**
	 * Creates the dynamic bounding sphere with its bounding sphere hierarchy.
	 *
	 * @param level deepness of the hierarchy tree
	 */
	virtual GLvoid		createBoundingSpheref(GLbyte level);
    
    /**
	 * Sets the next animation.
	 *
	 * @param next index of the next animation
	 * @param loop flag, if animation should be looped
	 * @param wait flag, if next animation should wait until current animation is finished
	 */
	virtual GLvoid		setNextAnimationf(GLshort next, GLboolean loop, GLboolean wait);
    
    
     	/**
                 * Intersection method using bounding spheres between this and another model.
                 *
                 * @param search array of joints which should be considered in this search
                 * @param searchSize size of this array
                 * @param hitJoint if a collision occurs, the hit hoint is stored here
                 * @param model the model to check against
                 * @param searchModel array of joints which should be considered in this search from the other model
                 * @param searchSizeModel size of this array
                 * @param hitJointModel if a collision occurs, the hit hoint of the other model is stored here
                 * @param exact flag, if intersection should be exact (triangle-triangle) or not (sphere-sphere)
                 * @param stopAtLevel level of the hierarchy to stop the search, if -1 one all spheres are considered; ignored, when exact is true
                 *
                 * @return true, if a collision did occur
                 */
	virtual GLboolean	collidesBoundingSpheref(GLbyte* search, GLubyte searchSize, GLbyte* hitJoint, CMyModel& model, GLbyte* searchModel, GLubyte searchSizeModel, GLbyte* hitJointModel, GLboolean exact, GLbyte stopAtLevel);
    
    //Added By Song 2012.03.06 모델을 초기화, 위치설정할때 update()를 해야 finalMatrix가 초기화된다.
    //미사일을 발사하자마자 적이 터지는 현상은 생성되자마자 충돌검사를 할때 문제가 생긴다.
    virtual void InitFinalMatrix();


    /**
	 * Updates the given model by time.
	 *
	 * @param	time the elapsed time by one frame
	 */
	virtual GLvoid		updatef(GLuint time);
    
	/**
	 * Renders the given model.
	 */
	virtual  GLvoid		renderf() const;
    
    
    //Added By Song 2011.07.07 finally매트릭스를 계산한다.(안보이는 모델은 finallyMaxtrix라도 구해야 충돌의 계산을 할 수가 있다.
    virtual bool      calFinallyMatrix();

    virtual float GetCompactOtherSpriteInterval();
    virtual float GetRadius();
    
    
    /**
	 * Returns if an animation is current running.
	 *
	 * @return true, if the animation is running
	 */
	virtual GLboolean	isAnimationRunningf();
    
    /**
	 * Returns the current animation.
	 *
	 * @return current animation index. -1 if none
	 */
	virtual GLshort		getCurrentAnimationf();
    
    /**
	 * Init the given model with the core model.
	 *
	 * @param currentModelCore	the core model
	 *
	 * @return error code
	 */
	SGLResult	initf(CModelData& currentModelCore);

protected:
	/**
	 * Flag, if quaternions should be used. By default on.
	 */
	GLboolean	useQuaternion;
    
	/**
	 * The dynamic bounding sphere of tis model.
	 */
	CModelBound*	dynamicBoundingSphere;
    
	/**
	 * Creates the default animation if one exists.
	 */
	GLvoid		createDefaultAnimationf();
    

	/**
	 * Returns the absolute elapsed time.
	 * 
	 * @return the elapsed time
	 */
	GLuint		getElapsedTimef();
    
	/**
	 * Sets the elapsed time. Time is clamped between the current animation time.
	 * 
	 * @param time the new elapsed time
	 */
	GLvoid		setElapsedTimef(GLuint time);
    
	/**
	 * Sets the elapsed time by frame.
	 * 
	 * @param frame the frame
	 */
	GLvoid		setElapsedTimeByFramef(GLuint frame);
    
	/**
	 * Resets and cleans up the model.
	 */
	GLvoid		resetf();
	
	
	
    //포의 정점으로 인덱스를 가져온다. (개발할때 사용한다.)
    bool    DevGetPoIndex(float *fV,int &nGroupIndex,int &nTriangleIndex,int &nVerIndex);
    
    //Added By Song 포의 경우 현재 포의 위치와 방향을 알아올려면
	//계산된 정점을 가져와서 특정 정점의 위치를 알아본다.
	//nGroup = 그룹의 인덱스
	//nTrangleIndex = 가져오려는 삼각형의 인덱스
	//return : 총 9 개의 어레이를 가져오는데 삼각형은 세개의 정점 * 세개의 좌표(x,y,z)로 구성되어 있다.
	GLfloat*    getWorldTriangle(int nGroup,int nTrangleIndex,int nVertexIndex);

    
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
    
protected:    
    //Added By Song 2011.04.07
    //finalMatrix와 finalBeforeMatrix가 같으면 움직임 또는 애니메이션 작업이 없어서 정점이 변경된 것이 없으면 매트릭스를 다시 구할 필요가 없다.
    GLfloat mbeforePosition[3];
    GLfloat mbeforeOreination[3];
    GLfloat mbeforeMatrix[16];
    //calFinallyMatrix CoreThread에서 실행되면 BeginUpdate에서는 아래의 로직을 타야한다. 그래야 화면에 그려지는데 
    //CoreBeginUpdate에서 위치를 연산해버리면 BeginUpdate에서는 밑의 연산을 타지 않기때문에 화면에 그려질 데이터가 없단다.
    bool      mbMoreOpre;
    
    
    
};

#endif
