//
// Book:      OpenGL(R) ES 2.0 Programming Guide
// Authors:   Aaftab Munshi, Dan Ginsburg, Dave Shreiner
// ISBN-10:   0321502795
// ISBN-13:   9780321502797
// Publisher: Addison-Wesley Professional
// URLs:      http://safari.informit.com/9780321563835
//            http://www.opengles-book.com
//

//
/// \file ESUtil.h
/// \brief A utility library for OpenGL ES.  This library provides a
///        basic common framework for the example applications in the
///        OpenGL ES 2.0 Programming Guide.
//
#ifndef HTRANSFORM_H
#define HTRANSFORM_H

///
//  Includes
//
#include <string.h>
#include "sGLDefine.h"
#define PIf		3.1415927410125732f

#ifdef __cplusplus

extern "C" {
#endif
	
void sglCopyVector(SVector *pS,SVector *pT);
void sglScale(SMatrix *result, GLfloat sx, GLfloat sy, GLfloat sz);
void sglTranslate(SMatrix *result, GLfloat tx, GLfloat ty, GLfloat tz);
void sglRotate(SMatrix *result, GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
void sglFrustum(SMatrix *result, float left, float right, float bottom, float top, float nearZ, float farZ);
void sglPerspective(SMatrix *result, float fovy, float aspect, float nearZ, float farZ);
void sglOrtho(SMatrix *result, float left, float right, float bottom, float top, float nearZ, float farZ);
void sglMatrixMultiply(SMatrix *result, SMatrix *srcA, SMatrix *srcB);
void sglMatrixLoadIdentity(SMatrix *result);
void sglNormalVector(SVector *result,	float fX1,float fY1,float fZ1,
									float fX2,float fY2,float fZ2,
									float fX3,float fY3,float fZ3);

//노멀 벡터와 하나의 정점으로 현재 위치의 z값을 구해 온다.
float sglGetYPlaneEquation(SVector* normalV,float x0,float y0, float z0,float x, float z);
void sglNormalize(SVector *result);
void sglTranslateEx(float *pOutx,float *pOuty,float *pOutz,float Px,float Py,float Pz,SVector* vectorDir);
    

    
    
/**
 * Calculates the amount of a vector.
 * 
 * @param vector the vector
 * 
 * @return the amount
 */
GLfloat sglAmountf(const GLfloat vector[3]);

/**
 * Normalizes a vector.
 * 
 * @param vector the vector to normalize
 */
GLvoid sglNormalizef(GLfloat vector[3]);

/**
 * Multiplies a vector by a scalar.
 * 
 * @param vector the vector to be multiplied
 * @param s the scalar
 */
GLvoid sglMultiplyf(GLfloat vector[3], GLfloat s);

/**
 * Adds two vectors.
 * 
 * @param result the result
 * @param vector0 the first vector
 * @param vector1 the second vector
 */
GLvoid sglAddf(GLfloat result[3], const GLfloat vector0[3], const GLfloat vector1[3]);

/**
 * Subtracts two vectors: vector0 - vector1
 * 
 * @param result the result
 * @param vector0 the first vector
 * @param vector1 the second vector
 */
GLvoid sglSubf(GLfloat result[3], const GLfloat vector0[3], const GLfloat vector1[3]);

/**
 * Copys a vector
 * 
 * @param copy the copy
 * @param vector the source vector
 */
GLvoid sglCopyf(GLfloat copy[3], const GLfloat vector[3]);

/**
 * Calculates the dot product of two vectors.
 * 
 * @param vector0 the frist vector
 * @param vector1 the second vector
 * 
 * @return the result, the dot product
 */	
GLfloat sglDotProductf(const GLfloat vector0[3], const GLfloat vector1[3]);

/**
 * Calculates the cross product of two vectors. result = vector0 x vector1
 * 
 * @param result the resulting vector
 * @param vector0 the first vector
 * @param vector1 the second vector 
 */
GLvoid sglCrossProductf(GLfloat result[3], const GLfloat vector0[3], const GLfloat vector1[3]);

/**
 * Multiplies a vector with the given matrix.
 * 
 * @param result the resulting vector
 * @param matrix the matrix to multiply with
 * @param vector the source vector
 * @param w homogenous coordiante, when needed
 */
inline GLvoid sglMultMatrixVectorf(GLfloat result[3], const GLfloat matrix[16], const GLfloat vector[3], const GLfloat w = 1.0f)
{
    static int nSize = sizeof(GLfloat) * 3;
    GLfloat temp[3];
    GLfloat temp1;
    
    if(result != vector)
    {
        *result =     *matrix*     *vector + *(matrix+4)* *(vector+1) + *(matrix+8)*  *(vector+2) + *(matrix+12)*w;
        *(result+1) = *(matrix+1)* *vector + *(matrix+5)* *(vector+1) + *(matrix+9)*  *(vector+2) + *(matrix+13)*w;
        *(result+2) = *(matrix+2)* *vector + *(matrix+6)* *(vector+1) + *(matrix+10)* *(vector+2) + *(matrix+14)*w;
        
    }
    else
    {
        *temp =     *matrix*     *vector + *(matrix+4)* *(vector+1) + *(matrix+8)*  *(vector+2) + *(matrix+12)*w;
        *(temp+1) = *(matrix+1)* *vector + *(matrix+5)* *(vector+1) + *(matrix+9)*  *(vector+2) + *(matrix+13)*w;
        *(temp+2) = *(matrix+2)* *vector + *(matrix+6)* *(vector+1) + *(matrix+10)* *(vector+2) + *(matrix+14)*w;
        memcpy(result, temp, nSize);
    }
    temp1 = *(matrix+3)* *vector + *(matrix+7)* *(vector+1) + *(matrix+11)* *(vector+2) + *(matrix+15)*w;
    
    if (temp1 != 1.0f && temp1 != 0.0f && w != 0.0f)
    {
        *result = *result / temp1;
        *(result+1) = *(result+1) / temp1;
        *(result+2) = *(result+2) / temp1;
    }
}

/**
 * Multiplies a matrix with a scalar.
 * 
 * @param result the reulting matrix
 * @param matrix the source matrix
 * @param scalar the scalar to multiply with
 */
GLvoid sglMultMatrixScalarf(GLfloat result[16], const GLfloat matrix[16], GLfloat scalar);

/**
 * Loads the identity matrix.
 * 
 * @param matrix the matrix where to load in the identity.
 */
GLvoid sglLoadIdentityf(GLfloat matrix[16]);

/**
 * Adds a matrix to another matrix.
 * 
 * @param result the resulting matrix
 * @param matrix0 the first matrix
 * @param matrix1 the second matrix
 */
GLvoid sglAddMatrixf(GLfloat result[16], const GLfloat matrix0[16], const GLfloat matrix1[16]);

/**
 * Subtracts a matrix from another matrix.
 * 
 * @param result the resulting matrix
 * @param matrix0 the first matrix
 * @param matrix1 the second matrix
 */
GLvoid sglSubMatrixf(GLfloat result[16], const GLfloat matrix0[16], const GLfloat matrix1[16]);

/**
 * Multiplies a matrix with another. result = matrix0 * matrix1
 * 
 * @param result the resulting matrix
 * @param matrix0 the first matrix
 * @param matrix1 the second matrix
 */
GLvoid sglMultMatrixf(GLfloat result[16], const GLfloat matrix0[16], const GLfloat matrix1[16]);

/**
 * Translates a matrix.
 * 
 * @param result the resulting matrix
 * @param x the x coordinate.
 * @param y the y coordinate.
 * @param z the z coordinate.
 */
GLvoid sglTranslateMatrixf(GLfloat result[16], GLfloat x, GLfloat y, GLfloat z);

/**
 * Scaling matrix.
 * 
 * @param result the resulting matrix
 * @param x the x scaling factor
 * @param y the y scaling factor
 * @param z the z scaling factor
 */
GLvoid sglScaleMatrixf(GLfloat result[16], GLfloat x, GLfloat y, GLfloat z);

/**
 * Rotates a matrix around the x, y and z axis.
 * 
 * @param result the resulting matrix.
 * @param anglex the x angle
 * @param angley the y angle
 * @param anglez the z angle
 */
GLvoid sglRotateRzRyRxMatrixf(GLfloat result[16], GLfloat anglex, GLfloat angley, GLfloat anglez);

/**
 * Transposes a matrix.
 * 
 * @param result the resulting matrix
 * @param matrix the source matrix
 */	
GLvoid sglTransposeMatrixf(GLfloat result[16], const GLfloat matrix[16]);

/**
 * Calculates the invers of a matrix.
 * 
 * @param result the resulting matrix
 * @param matrix the source matrix
 * 
 * @return true, if a inverse matrix could be found
 */	
GLboolean sglInverseMatrixf(GLfloat result[16], const GLfloat matrix[16]);
    
    
    
/**
 * Converts Euler Angles to a quaternion rotation.
 * 
 * @param result the resulting quaternion
 * @param anglex the x angle
 * @param angley the y angle
 * @param anglez the z angle
 */
GLvoid sglAngleToQuaternionf(GLfloat result[4], GLfloat anglex, GLfloat angley, GLfloat anglez);

/**
 * Does a spherical interpolation (SLERP) with two quaternions.
 * 
 * @param result the resulting quaternion
 * @param quaternion0 the first quaternion
 * @param quaternion1 the second quaternion
 * @param t the ratio (between 0 and 1) to interpolate
 */
GLvoid sglSlerpf(GLfloat result[4], const GLfloat quaternion0[4], const GLfloat quaternion1[4], GLclampf t);

/**
 * Converts a quaternion to a rotation matrix.
 * 
 * @param result the resulting quaternion
 * @param quaternion the quaternion to convert
 */
GLvoid sglQuaternionToMatrixf(GLfloat result[16], GLfloat quaternion[4]);
	
    
    
/**
 * Distance between two points.
 * 
 * @param a first point
 * @param b second point
 * 
 * @return the distance
 */
GLfloat sglDistancePointPointf(const GLfloat a[3], const GLfloat b[3]);

/**
 * Square distance between two points.
 * 
 * @param a first point
 * @param b second point
 * 
 * @return the square distance
 */
GLfloat sglSquareDistancePointPointf(const GLfloat a[3], const GLfloat b[3]);
GLfloat sglSquareDistanceSPointSPointf(const SPoint* a, const SPoint* b);

/**
 * Calculates the closest point from a triangle to a given point.
 * 
 * @param result	the closest point
 * @param a			point a
 * @param b			point b
 * @param c			point c
 * @param p			the point to calculate the closest point to
 */
GLvoid sglClosestPointFromTriangleToPointf(GLfloat result[3], const GLfloat a[3], const GLfloat b[3], const GLfloat c[3], const GLfloat p[3]);
    
    
GLboolean sglLinePlaneIntersectionf(const GLfloat p[3], const GLfloat q[3], const GLfloat a[3], const GLfloat b[3], const GLfloat c[3], GLfloat& t, GLfloat& u, GLfloat& v, GLfloat& w);
GLboolean sglTriangleTriangleIntersectionf(const GLfloat t0[3*3], const GLfloat t1[3*3]);
    
    

/**
 * Added By Song 윈도우의 2D로 이미지를 그릴때 화면좌표,3d좌표와의 비율을 계산하기 위해서
 * perspectivef에 투영되는 좌표를 구해온다.
 * 즉 480 : 320의 아이폰 화면에
 * 투영의 맥스와 민이 0.5 : 0.8 이라면
 * 화면 1 pixel의 크기는 1 : x = 480 : 1 (2 * xmin_3d)
 * x = 1/480 (약0.002)의 크기이다.
 */
extern GLfloat xmin_3d, xmax_3d, ymin_3d, ymax_3d;

/**
 * Sets the view frustum.
 * 
 * @param fovy field of view in the y axis
 * @param aspect the aspect of width to height
 * @param zNear	the near clipping plane
 * @param zFar	the far clipping plane.
 */
GLvoid sglPerspectivef(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar);
    
/**
 * 미리 계산을 한다.
 */
GLvoid sglCalculatePerspectivef(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar);

/**
 * Defines the eye position with the point to look at plus the up vector.
 * 
 * @param eyeX x position of the eye
 * @param eyeY y position of the eye
 * @param eyeZ y position of the eye
 * @param centerX x position to look at
 * @param centerY y position to look at
 * @param centerZ z position to look at
 * @param upX up vector x
 * @param upY up vector y
 * @param upZ up vector z
 */
GLvoid sglLookAtf(GLfloat eyeX, GLfloat eyeY, GLfloat eyeZ, GLfloat centerX, GLfloat centerY, GLfloat centerZ, GLfloat upX, GLfloat upY, GLfloat upZ);
    
    
GLvoid sglColorf(GLclampf r, GLclampf g, GLclampf b, GLclampf a);
GLvoid sglLinef(const GLfloat s[3], const GLfloat e[3]);
GLvoid sglCirclef(const GLfloat p[3], const GLfloat o[3], GLfloat radius);

//Angle To Vector
void sglAngleToVector(float fAngle[3],SVector* vtResult);
    
#ifdef __cplusplus
}
#endif

#endif // HTRANSFORM_H