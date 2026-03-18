//
//  CMyModel.h
//  SongGL
//
//  Created by 호학 송 on 2014. 1. 28..
//
//

#ifndef __SongGL__CMyModel__
#define __SongGL__CMyModel__
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
typedef enum _MODELTYPE {
    MODELTYPE_MS3D,MODELTYPE_MS3D_ASSCII
}MODELTYPE;

class CPicking;
class CMyModel
{
public:
    CMyModel();
    virtual ~CMyModel();
   
    virtual MODELTYPE GetType() = 0;
    virtual void GetCenter(GLfloat *fOutCenter) = 0;
    virtual bool    IsPicking(CPicking* pPick,int nWinPosX,int nWinPosY) = 0;
    virtual bool    IsPicking(CPicking* pPick,SPoint& nearPoint,SPoint& farPoint,int nWinPosX,int nWinPosY) = 0;
    //주인공은 버텍스 갯수가 너무 많아서 그룹 0만 검색하자.
    virtual bool IsPickingZeroGroup(CPicking* pPick,int nWinPosX,int nWinPosY) = 0;
    
	
    /**
             * Creates the dynamic bounding sphere with its bounding sphere hierarchy.
             *
             * @param level deepness of the hierarchy tree
             */
	virtual GLvoid		createBoundingSpheref(GLbyte level) = 0;
    virtual GLvoid		setNextAnimationf(GLshort next, GLboolean loop, GLboolean wait) = 0;
    virtual GLboolean	collidesBoundingSpheref(GLbyte* search, GLubyte searchSize, GLbyte* hitJoint, CMyModel& model, GLbyte* searchModel, GLubyte searchSizeModel, GLbyte* hitJointModel, GLboolean exact, GLbyte stopAtLevel) = 0;
    virtual void InitFinalMatrix() = 0;
    virtual GLvoid		updatef(GLuint time) = 0;
    virtual  GLvoid		renderf() const = 0;
    virtual bool      calFinallyMatrix() = 0;
    virtual float GetCompactOtherSpriteInterval() = 0;
    virtual GLboolean	isAnimationRunningf() = 0;
    virtual float GetRadius() = 0;
    virtual GLshort		getCurrentAnimationf() = 0;

public:
    /**
	 * Position of the model in world space.
	 */
	GLfloat		position[3];
    
	/**
	 * Orientation of the model in world space.
	 */
	GLfloat		orientation[3];
	
	
	//Added By Song 2011.02.21 탱크의 머리부분을 프로그램적으로 회전할때 또는 이동할때 등등을
	//사용한다.
	GLfloat		*pfinalmulMaxtix;
	
	
	/**
	 * Scaling of the model.
	 */
	GLfloat		scale[3];

};

#endif /* defined(__SongGL__CMyModel__) */
