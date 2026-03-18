#include "sGLTrasform.h"
#include <math.h>
#include <string.h>


void sglCopyVector(SVector *pS,SVector *pT)
{
	pT->x = pS->x;
	pT->y = pS->y;
	pT->z = pS->z;
}

void sglScale(SMatrix *result, GLfloat sx, GLfloat sy, GLfloat sz)
{
    result->m[0][0] *= sx;
    result->m[0][1] *= sx;
    result->m[0][2] *= sx;
    result->m[0][3] *= sx;
	
    result->m[1][0] *= sy;
    result->m[1][1] *= sy;
    result->m[1][2] *= sy;
    result->m[1][3] *= sy;
	
    result->m[2][0] *= sz;
    result->m[2][1] *= sz;
    result->m[2][2] *= sz;
    result->m[2][3] *= sz;
}

void sglTranslate(SMatrix *result, GLfloat tx, GLfloat ty, GLfloat tz)
{
    result->m[3][0] += (result->m[0][0] * tx + result->m[1][0] * ty + result->m[2][0] * tz);
    result->m[3][1] += (result->m[0][1] * tx + result->m[1][1] * ty + result->m[2][1] * tz);
    result->m[3][2] += (result->m[0][2] * tx + result->m[1][2] * ty + result->m[2][2] * tz);
    result->m[3][3] += (result->m[0][3] * tx + result->m[1][3] * ty + result->m[2][3] * tz);
}

void sglTranslateEx(float *pOutx,float *pOuty,float *pOutz,float Px,float Py,float Pz,SVector* vectorDir)
{
	*pOutx = Px + vectorDir->x;
	*pOuty = Py + vectorDir->y;
	*pOutz = Pz + vectorDir->z;
}

void sglRotate(SMatrix *result, GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
	GLfloat sinAngle, cosAngle;
	GLfloat mag = sqrtf(x * x + y * y + z * z);
	
	sinAngle = sinf ( angle * PI / 180.0f );
	cosAngle = cosf ( angle * PI / 180.0f );
	if ( mag > 0.0f )
	{
		GLfloat xx, yy, zz, xy, yz, zx, xs, ys, zs;
		GLfloat oneMinusCos;
		SMatrix rotMat;
		
		x /= mag;
		y /= mag;
		z /= mag;
		
		xx = x * x;
		yy = y * y;
		zz = z * z;
		xy = x * y;
		yz = y * z;
		zx = z * x;
		xs = x * sinAngle;
		ys = y * sinAngle;
		zs = z * sinAngle;
		oneMinusCos = 1.0f - cosAngle;
		
		rotMat.m[0][0] = (oneMinusCos * xx) + cosAngle;
		rotMat.m[0][1] = (oneMinusCos * xy) - zs;
		rotMat.m[0][2] = (oneMinusCos * zx) + ys;
		rotMat.m[0][3] = 0.0F; 
		
		rotMat.m[1][0] = (oneMinusCos * xy) + zs;
		rotMat.m[1][1] = (oneMinusCos * yy) + cosAngle;
		rotMat.m[1][2] = (oneMinusCos * yz) - xs;
		rotMat.m[1][3] = 0.0F;
		
		rotMat.m[2][0] = (oneMinusCos * zx) - ys;
		rotMat.m[2][1] = (oneMinusCos * yz) + xs;
		rotMat.m[2][2] = (oneMinusCos * zz) + cosAngle;
		rotMat.m[2][3] = 0.0F; 
		
		rotMat.m[3][0] = 0.0F;
		rotMat.m[3][1] = 0.0F;
		rotMat.m[3][2] = 0.0F;
		rotMat.m[3][3] = 1.0F;
		
		sglMatrixMultiply( result, &rotMat, result );
	}
}

void sglFrustum(SMatrix *result, float left, float right, float bottom, float top, float nearZ, float farZ)
{
    float       deltaX = right - left;
    float       deltaY = top - bottom;
    float       deltaZ = farZ - nearZ;
    SMatrix    frust;
	
    if ( (nearZ <= 0.0f) || (farZ <= 0.0f) ||
		(deltaX <= 0.0f) || (deltaY <= 0.0f) || (deltaZ <= 0.0f) )
		return;
	
    frust.m[0][0] = 2.0f * nearZ / deltaX;
    frust.m[0][1] = frust.m[0][2] = frust.m[0][3] = 0.0f;
	
    frust.m[1][1] = 2.0f * nearZ / deltaY;
    frust.m[1][0] = frust.m[1][2] = frust.m[1][3] = 0.0f;
	
    frust.m[2][0] = (right + left) / deltaX;
    frust.m[2][1] = (top + bottom) / deltaY;
    frust.m[2][2] = -(nearZ + farZ) / deltaZ;
    frust.m[2][3] = -1.0f;
	
    frust.m[3][2] = -2.0f * nearZ * farZ / deltaZ;
    frust.m[3][0] = frust.m[3][1] = frust.m[3][3] = 0.0f;
	
    sglMatrixMultiply(result, &frust, result);
}


void sglPerspective(SMatrix *result, float fovy, float aspect, float nearZ, float farZ)
{
	GLfloat frustumW, frustumH;
	
	frustumH = tanf( fovy / 360.0f * PI ) * nearZ;
	frustumW = frustumH * aspect;
	
	sglFrustum( result, -frustumW, frustumW, -frustumH, frustumH, nearZ, farZ );
}

void slgOrtho(SMatrix *result, float left, float right, float bottom, float top, float nearZ, float farZ)
{
    float       deltaX = right - left;
    float       deltaY = top - bottom;
    float       deltaZ = farZ - nearZ;
    SMatrix    ortho;
	
    if ( (deltaX == 0.0f) || (deltaY == 0.0f) || (deltaZ == 0.0f) )
        return;
	
    sglMatrixLoadIdentity(&ortho);
    ortho.m[0][0] = 2.0f / deltaX;
    ortho.m[3][0] = -(right + left) / deltaX;
    ortho.m[1][1] = 2.0f / deltaY;
    ortho.m[3][1] = -(top + bottom) / deltaY;
    ortho.m[2][2] = -2.0f / deltaZ;
    ortho.m[3][2] = -(nearZ + farZ) / deltaZ;
	
    sglMatrixMultiply(result, &ortho, result);
}


void sglMatrixMultiply(SMatrix *result, SMatrix *srcA, SMatrix *srcB)
{
    SMatrix    tmp;
    int         i;
	
	for (i=0; i<4; i++)
	{
		tmp.m[i][0] =	(srcA->m[i][0] * srcB->m[0][0]) +
		(srcA->m[i][1] * srcB->m[1][0]) +
		(srcA->m[i][2] * srcB->m[2][0]) +
		(srcA->m[i][3] * srcB->m[3][0]) ;
		
		tmp.m[i][1] =	(srcA->m[i][0] * srcB->m[0][1]) + 
		(srcA->m[i][1] * srcB->m[1][1]) +
		(srcA->m[i][2] * srcB->m[2][1]) +
		(srcA->m[i][3] * srcB->m[3][1]) ;
		
		tmp.m[i][2] =	(srcA->m[i][0] * srcB->m[0][2]) + 
		(srcA->m[i][1] * srcB->m[1][2]) +
		(srcA->m[i][2] * srcB->m[2][2]) +
		(srcA->m[i][3] * srcB->m[3][2]) ;
		
		tmp.m[i][3] =	(srcA->m[i][0] * srcB->m[0][3]) + 
		(srcA->m[i][1] * srcB->m[1][3]) +
		(srcA->m[i][2] * srcB->m[2][3]) +
		(srcA->m[i][3] * srcB->m[3][3]) ;
	}
    memcpy(result, &tmp, sizeof(SMatrix));
}


void sglMatrixLoadIdentity(SMatrix *result)
{
    memset(result, 0x0, sizeof(SMatrix));
    result->m[0][0] = 1.0f;
    result->m[1][1] = 1.0f;
    result->m[2][2] = 1.0f;
    result->m[3][3] = 1.0f;
}

void sglNormalize(SVector *result)
{
	float vectorLen = 0; 
	if(result->x == 0 && result->y == 0 && result->z == 0) return;
	vectorLen = sqrtf(result->x*result->x + result->y*result->y + result->z*result->z);
	result->x = result->x / vectorLen;
	result->y = result->y / vectorLen;
	result->z = result->z / vectorLen;
}

void sglAngleToVector(float fAngle[3],SVector* vtResult)
{
    float yaw = fAngle[2] * PI / 180.f;
    float pitch = fAngle[1] * PI / 180.f;
    float fyawcosf = cosf(yaw);
    vtResult->x = cosf(pitch) * fyawcosf;
    vtResult->y = sinf(yaw);
    vtResult->z = sinf(pitch) * fyawcosf;
    sglNormalize(vtResult);

}

// 법선벡터를 구해 온다.
void sglNormalVector(SVector *result,float fX1,float fY1,float fZ1,
								float fX2,float fY2,float fZ2,
								float fX3,float fY3,float fZ3)
{
	SVector A;
	SVector B;
	
	A.x = fX2 - fX1;
	A.y = fY2 - fY1;
	A.z = fZ2 - fZ1;
	
	B.x = fX1 - fX3;
	B.y = fY1 - fY3;
	B.z = fZ1 - fZ3;
	
	//A X B = A.y*B.z - A.z*B.y , A.x*B.z - A.z*B.x ,  A.x*B.y - A.y*B.x
	
	result->x =  A.y*B.z - A.z*B.y;
	result->y =  A.z*B.x - A.x*B.z;
	result->z =  A.x*B.y - A.y*B.x;
	sglNormalize(result);
}


// 명면방정식 구하는 식
// NormalVector @ 벡터 = |NormalVector|*|벡터|cos90 = 0
// 즉 법선벡터를 알고 평면의 한점 A(x0,y0,z0)를 알고 있다면
// 평면의 가상의 점 B(x,y,z) 의 변수로 방정식을 만든다.
// 벡터 = AB = <x-x0,y-y0,z-z0>
// NormalVector = <a,b,c>
// <a,b,c> @ <x-x0,y-y0,z-z0> = a(x-x0) + b(y-y0) + c(z-z0) = 0
// ax + by + cz - (ax0 + by0 + cz0) = 0
// 평면방정식은 ax+by+cz+d = 0 이다. 그래서 d = -(ax0 + by0 + cz0) = -(<a,b,c> @ <x0,y0,z0>)이다.
float sglGetYPlaneEquation(SVector* normalV,float x0,float y0, float z0,float x, float z)
{
	float d = -(normalV->x * x0 + normalV->y * y0 + normalV->z * z0);
	//normalV->x * x + normalV->y * y + normalV->z * z + d = 0
	return -(normalV->x * x + normalV->z * z + d) / normalV->y;
}

















/**
 * Returns true, if a row of a matrix is zero. Needed for the Gauss algorithm.
 * 
 * @param matrix the matrix to check
 * @param row the row to check
 * 
 * @return true, if all elements of the row are zero
 */
GLboolean sglIsRowZerof(const GLfloat matrix[16], GLint row)
{
    for (GLint column = 0; column < 4; column++)
    {
        if (matrix[column*4+row] != 0.0f)
            return GL_FALSE;
    }
    
    return GL_TRUE;
}

/**
 * Returns true, if a column of a matrix is zero. Needed for the Gauss algorithm.
 * 
 * @param matrix the matrix to check
 * @param column the column to check
 * 
 * @return true, if all elements of the column are zero
 */
GLboolean sglIsColumnZerof(const GLfloat matrix[16], GLint column)
{
    for (GLint row = 0; row < 4; row++)
    {
        if (matrix[column*4+row] != 0.0f)
            return GL_FALSE;
    }
    
    return GL_TRUE;
}

/**
 * Devides a given row of a matrix with a constant value. Needed for the Gauss algorithm.
 * 
 * @param result the matrix where to store the result
 * @param matrix the source matrix
 * @param row the row to divide
 * @param value	the value to devide with
 */
GLvoid sglDevideRowByf(GLfloat result[16], GLfloat matrix[16], GLint row, GLfloat value)
{
    for (GLint column = 0; column < 4; column++)
    {
        matrix[column*4+row] /= value;
        result[column*4+row] /= value;
    }
}

/**
 * Swaps to rows. Needed for the Gauss algorithm.
 * 
 * @param result the matrix where to store the result
 * @param matrix the source matrix
 * @param rowOne the first row
 * @param rowTwo the second row
 */
GLvoid sglSwapRowf(GLfloat result[16], GLfloat matrix[16], GLint rowOne, GLint rowTwo)
{
    GLfloat temp;
    
    for (GLint column = 0; column < 4; column++)
    {
        temp = matrix[column*4 + rowOne];
        matrix[column*4 + rowOne] = matrix[column*4 + rowTwo];
        matrix[column*4 + rowTwo] = temp;
        
        temp = result[column*4 + rowOne];
        result[column*4 + rowOne] = result[column*4 + rowTwo];
        result[column*4 + rowTwo] = temp;
    }	
}

/**
 * Adds a row to the other row. Row two is added to row one with a given factor.
 * Needed for the Gauss algorithm.
 * 
 * @param result the matrix where to store the result
 * @param matrix the source matrix
 * @param rowOne the first row
 * @param rowTwo the second row
 * @param factor the factor
 */
GLvoid sglAddRowf(GLfloat result[16], GLfloat matrix[16], GLint rowOne, GLint rowTwo, GLfloat factor)
{
//    for (GLint column = 0; column < 4; column++)
//    {
//        matrix[column*4+rowOne] += matrix[column*4+rowTwo]*factor;
//        result[column*4+rowOne] += result[column*4+rowTwo]*factor;
//    }

    int o,t;
    o = rowOne;
    t = rowTwo;
    *(matrix+o) = *(matrix+o) + *(matrix+t) * factor;
    *(result+o) = *(result+o) + *(result+t) * factor;
    
    o = rowOne + 4;
    t = rowTwo + 4;
    *(matrix+o) = *(matrix+o) + *(matrix+t) * factor;
    *(result+o) = *(result+o) + *(result+t) * factor;

    o = rowOne + 8;
    t = rowTwo + 8;
    *(matrix+o) = *(matrix+o) + *(matrix+t) * factor;
    *(result+o) = *(result+o) + *(result+t) * factor;

    
    o = rowOne + 12;
    t = rowTwo + 12;
    *(matrix+o) = *(matrix+o) + *(matrix+t) * factor;
    *(result+o) = *(result+o) + *(result+t) * factor;

}



/**
 * Calculates the amount of a vector.
 * 
 * @param vector the vector
 * 
 * @return the amount
 */
GLfloat sglAmountf(const GLfloat vector[3])
{
    return (GLfloat)sqrt(vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2]);
}

/**
 * Normalizes a vector.
 * 
 * @param vector the vector to normalize
 */
GLvoid sglNormalizef(GLfloat vector[3])
{
    GLfloat amount = sglAmountf(vector);
	
    if (amount == 0.0f) 
        return;
	
    //for (GLuint i = 0; i < 3; i++)
    vector[0] /= amount;
    vector[1] /= amount;
    vector[2] /= amount;
}

/**
 * Multiplies a vector by a scalar.
 * 
 * @param vector the vector to be multiplied
 * @param s the scalar
 */
GLvoid sglMultiplyf(GLfloat vector[3], GLfloat s)
{
    for (GLuint i = 0; i < 3; i++)	
        vector[i] *= s;
}

/**
 * Adds two vectors.
 * 
 * @param result the result
 * @param vector0 the first vector
 * @param vector1 the second vector
 */
GLvoid sglAddf(GLfloat result[3], const GLfloat vector0[3], const GLfloat vector1[3])
{
    result[0] = vector0[0] + vector1[0];
    result[1] = vector0[1] + vector1[1];
    result[2] = vector0[2] + vector1[2];
}

/**
 * Subtracts two vectors: vector0 - vector1
 * 
 * @param result the result
 * @param vector0 the first vector
 * @param vector1 the second vector
 */
GLvoid sglSubf(GLfloat result[3], const GLfloat vector0[3], const GLfloat vector1[3])
{
    result[0] = vector0[0] - vector1[0];
    result[1] = vector0[1] - vector1[1];
    result[2] = vector0[2] - vector1[2];
}

/**
 * Copys a vector
 * 
 * @param copy the copy
 * @param vector the source vector
 */
GLvoid sglCopyf(GLfloat copy[3], const GLfloat vector[3])
{
    copy[0] = vector[0];
    copy[1] = vector[1];
    copy[2] = vector[2];
}

/**
 * Calculates the dot product of two vectors.
 * 
 * @param vector0 the frist vector
 * @param vector1 the second vector
 * 
 * @return the result, the dot product
 */	
GLfloat sglDotProductf(const GLfloat vector0[3], const GLfloat vector1[3])
{
    return vector0[0]*vector1[0] + vector0[1]*vector1[1] + vector0[2]*vector1[2];
}

/**
 * Calculates the cross product of two vectors. result = vector0 x vector1
 * 
 * @param result the resulting vector
 * @param vector0 the first vector
 * @param vector1 the second vector 
 */
GLvoid sglCrossProductf(GLfloat result[3], const GLfloat vector0[3], const GLfloat vector1[3])
{
    GLfloat	temp[3];
    
    temp[0] = vector0[1]*vector1[2] - vector0[2]*vector1[1];
    temp[1] = vector0[2]*vector1[0] - vector0[0]*vector1[2];
    temp[2] = vector0[0]*vector1[1] - vector0[1]*vector1[0];
    
    for (GLuint i = 0; i < 3; i++)	
        result[i] = temp[i];
}

/**
 * Multiplies a vector with the given matrix.
 * 
 * @param result the resulting vector
 * @param matrix the matrix to multiply with
 * @param vector the source vector
 * @param w homogenous coordiante, when needed
 */
//GLvoid sglMultMatrixVectorf(GLfloat result[3], const GLfloat matrix[16], const GLfloat vector[3], const GLfloat w)
//{
////원본...
////    GLuint i;
////    
////    GLfloat temp[4];
////	
////    for (i = 0; i < 4; i++)
////        temp[i] = matrix[i]*vector[0] + matrix[4+i]*vector[1] + matrix[8+i]*vector[2] + matrix[12+i]*w;
////    
////    for (i = 0; i < 3; i++)	
////        result[i] = temp[i];
////    
////    if (temp[3] != 1.0f && temp[3] != 0.0f && w != 0.0f)
////    {
////        for (GLuint i = 0; i < 3; i++)	
////            result[i] /= temp[3];
////    }
//    
//    static int nSize = sizeof(GLfloat) * 3;
//    GLfloat temp[3];
//    GLfloat temp1;
//    
//    *temp =     *matrix*     *vector + *(matrix+4)* *(vector+1) + *(matrix+8)*  *(vector+2) + *(matrix+12)*w;
//    *(temp+1) = *(matrix+1)* *vector + *(matrix+5)* *(vector+1) + *(matrix+9)*  *(vector+2) + *(matrix+13)*w;
//    *(temp+2) = *(matrix+2)* *vector + *(matrix+6)* *(vector+1) + *(matrix+10)* *(vector+2) + *(matrix+14)*w;
//    temp1 = *(matrix+3)* *vector + *(matrix+7)* *(vector+1) + *(matrix+11)* *(vector+2) + *(matrix+15)*w;
//    
//    memcpy(result, temp, nSize);
//    
//    if (temp1 != 1.0f && temp1 != 0.0f && w != 0.0f)
//    {
//        *result = *result / temp1;
//        *(result+1) = *(result+1) / temp1;
//        *(result+2) = *(result+2) / temp1;
//    }
//}

/**
 * Multiplies a matrix with a scalar.
 * 
 * @param result the reulting matrix
 * @param matrix the source matrix
 * @param scalar the scalar to multiply with
 */
GLvoid sglMultMatrixScalarf(GLfloat result[16], const GLfloat matrix[16], GLfloat scalar)
{
    for (GLuint i = 0; i < 16; i++)
    {
        result[i] = matrix[i] * scalar;
    }
}

/**
 * Loads the identity matrix.
 * 
 * @param matrix the matrix where to load in the identity.
 */
GLvoid sglLoadIdentityf(GLfloat matrix[16])
{
    static GLfloat gm[] = {
        1.f,0.f,0.f,0.f,
        0.f,1.f,0.f,0.f,
        0.f,0.f,1.f,0.f,
        0.f,0.f,0.f,1.f};
    
    memcpy(matrix,gm,sizeof(gm));
        
//    matrix[0] = 1.0f;
//    matrix[1] = 0.0f;
//    matrix[2] = 0.0f;
//    matrix[3] = 0.0f;
//	
//    matrix[4] = 0.0f;
//    matrix[5] = 1.0f;
//    matrix[6] = 0.0f;
//    matrix[7] = 0.0f;
//	
//    matrix[8] = 0.0f;
//    matrix[9] = 0.0f;
//    matrix[10] = 1.0f;
//    matrix[11] = 0.0f;
//	
//    matrix[12] = 0.0f;
//    matrix[13] = 0.0f;
//    matrix[14] = 0.0f;
//    matrix[15] = 1.0f;
}

/**
 * Adds a matrix to another matrix.
 * 
 * @param result the resulting matrix
 * @param matrix0 the first matrix
 * @param matrix1 the second matrix
 */
GLvoid sglAddMatrixf(GLfloat result[16], const GLfloat matrix0[16], const GLfloat matrix1[16])
{
    for (GLuint i = 0; i < 16; i++)
    {
        result[i] = matrix0[i] + matrix1[i];
    }
}

/**
 * Subtracts a matrix from another matrix.
 * 
 * @param result the resulting matrix
 * @param matrix0 the first matrix
 * @param matrix1 the second matrix
 */
GLvoid sglSubMatrixf(GLfloat result[16], const GLfloat matrix0[16], const GLfloat matrix1[16])
{
    for (GLuint i = 0; i < 16; i++)
    {
        result[i] = matrix0[i] - matrix1[i];
    }
}

/**
 * Multiplies a matrix with another. result = matrix0 * matrix1
 * 
 * @param result the resulting matrix
 * @param matrix0 the first matrix
 * @param matrix1 the second matrix
 */
GLvoid sglMultMatrixf(GLfloat result[16], const GLfloat matrix0[16], const GLfloat matrix1[16])
{
//원본
//    GLuint i;
//    
//    GLfloat temp[16];
//	
//    for (i = 0; i < 16; i++)
//    {
//        temp[i] = 0.0f;
//        
//        for(GLuint k = 0; k < 4; k++)
//        {
//            //			  		row   column   		   row column
//            temp[i] += matrix0[(i%4)+(k*4)] * matrix1[(k)+((i/4)*4)];
//        }
//    }
//    
//    memcpy(result, temp, sizeof(temp));

    
//포인터
//    GLuint i,a,b;
//    
//    GLfloat temp[16];
//	
//    for (i = 0; i < 16; i++)
//    {
//        //temp[i] = 0.0f;
//        a = i % 4;
//        b = i / 4;
//        
//        *(temp + i) = *(matrix0+a) * *(matrix1+(b*4)) + *(matrix0+(a+4)) * *(matrix1+(1+(b*4))) + *(matrix0+(a+8)) * *(matrix1+(2+(b*4))) + *(matrix0+(a+12)) * *(matrix1+(3+(b*4)));
//    }
//    
//    memcpy(result, temp, sizeof(temp));
    

    
    
    
//    *(temp + i) =
//    *(matrix0+a) * *(matrix1+(b*4)) +
//    *(matrix0+(a+4)) * *(matrix1+(1+(b*4))) +
//    *(matrix0+(a+8)) * *(matrix1+(2+(b*4))) +
//    *(matrix0+(a+12)) * *(matrix1+(3+(b*4)));
    
    GLfloat temp[16];
    //------------------------------------
    //0
    *temp =
    *matrix0 * *matrix1 +
    *(matrix0+4) * *(matrix1+1) +
    *(matrix0+8) * *(matrix1+2) +
    *(matrix0+12) * *(matrix1+3);
    
    //1
    *(temp + 1) =
    *(matrix0+1) * *matrix1 +
    *(matrix0+5) * *(matrix1+1) +
    *(matrix0+9) * *(matrix1+2) +
    *(matrix0+13) * *(matrix1+3);

    
    //2
    *(temp + 2) =
    *(matrix0+2) * *matrix1+ +
    *(matrix0+6) * *(matrix1+1) +
    *(matrix0+10) * *(matrix1+2) +
    *(matrix0+14) * *(matrix1+3);

    
    //3
    *(temp + 3) =
    *(matrix0+3) * *matrix1 +
    *(matrix0+7) * *(matrix1+1) +
    *(matrix0+11) * *(matrix1+2) +
    *(matrix0+15) * *(matrix1+3);
    
    
    //------------------------------------
    //4
    *(temp + 4) =
    *matrix0 * *(matrix1+4) +
    *(matrix0+4) * *(matrix1+5) +
    *(matrix0+8) * *(matrix1+6) +
    *(matrix0+12) * *(matrix1+7);
    
    //5
    *(temp + 5) =
    *(matrix0+1) * *(matrix1+4) +
    *(matrix0+5) * *(matrix1+5) +
    *(matrix0+9) * *(matrix1+6) +
    *(matrix0+13) * *(matrix1+7);
    
    
    //6
    *(temp + 6) =
    *(matrix0+2) * *(matrix1+4) +
    *(matrix0+6) * *(matrix1+5) +
    *(matrix0+10) * *(matrix1+6) +
    *(matrix0+14) * *(matrix1+7);
    
    
    //7
    *(temp + 7) =
    *(matrix0+3) * *(matrix1+4) +
    *(matrix0+7) * *(matrix1+5) +
    *(matrix0+11) * *(matrix1+6) +
    *(matrix0+15) * *(matrix1+7);


    
    //------------------------------------
    //8
    *(temp + 8) =
    *matrix0 * *(matrix1+8) +
    *(matrix0+4) * *(matrix1+9) +
    *(matrix0+8) * *(matrix1+10) +
    *(matrix0+12) * *(matrix1+11);
    
    //9
    *(temp + 9) =
    *(matrix0+1) * *(matrix1+8) +
    *(matrix0+5) * *(matrix1+9) +
    *(matrix0+9) * *(matrix1+10) +
    *(matrix0+13) * *(matrix1+11);
    
    
    //10
    *(temp + 10) =
    *(matrix0+2) * *(matrix1+8) +
    *(matrix0+6) * *(matrix1+9) +
    *(matrix0+10) * *(matrix1+10) +
    *(matrix0+14) * *(matrix1+11);
    
    
    //11
    *(temp + 11) =
    *(matrix0+3) * *(matrix1+8) +
    *(matrix0+7) * *(matrix1+9) +
    *(matrix0+11) * *(matrix1+10) +
    *(matrix0+15) * *(matrix1+11);
    
    
    
    //------------------------------------
    //12
    *(temp + 12) =
    *matrix0 * *(matrix1+12) +
    *(matrix0+4) * *(matrix1+13) +
    *(matrix0+8) * *(matrix1+14) +
    *(matrix0+12) * *(matrix1+15);
    
    //13
    *(temp + 13) =
    *(matrix0+1) * *(matrix1+12) +
    *(matrix0+5) * *(matrix1+13) +
    *(matrix0+9) * *(matrix1+14) +
    *(matrix0+13) * *(matrix1+15);
    
    
    //14
    *(temp + 14) =
    *(matrix0+2) * *(matrix1+12) +
    *(matrix0+6) * *(matrix1+13) +
    *(matrix0+10) * *(matrix1+14) +
    *(matrix0+14) * *(matrix1+15);
    
    
    //15
    *(temp + 15) =
    *(matrix0+3) * *(matrix1+12) +
    *(matrix0+7) * *(matrix1+13) +
    *(matrix0+11) * *(matrix1+14) +
    *(matrix0+15) * *(matrix1+15);


    memcpy(result, temp, sizeof(temp));
}

/**
 * Translates a matrix.
 * 
 * @param result the resulting matrix
 * @param x the x coordinate.
 * @param y the y coordinate.
 * @param z the z coordinate.
 */
GLvoid sglTranslateMatrixf(GLfloat result[16], GLfloat x, GLfloat y, GLfloat z)
{
    GLfloat matrix[16];
	
    sglLoadIdentityf(matrix);
	
    matrix[12] = x;
    matrix[13] = y;
    matrix[14] = z;
    
    sglMultMatrixf(result, result, matrix);
}

/**
 * Scaling matrix.
 * 
 * @param result the resulting matrix
 * @param x the x scaling factor
 * @param y the y scaling factor
 * @param z the z scaling factor
 */
GLvoid sglScaleMatrixf(GLfloat result[16], GLfloat x, GLfloat y, GLfloat z)
{
    GLfloat matrix[16];
	
    sglLoadIdentityf(matrix);
	
    matrix[0] = x;
    matrix[5] = y;
    matrix[10] = z;
    
    sglMultMatrixf(result, result, matrix);
}

/**
 * Rotates a matrix around the x, y and z axis.
 * 
 * @param result the resulting matrix.
 * @param anglex the x angle
 * @param angley the y angle
 * @param anglez the z angle
 */
GLvoid sglRotateRzRyRxMatrixf(GLfloat result[16], GLfloat anglex, GLfloat angley, GLfloat anglez)
{
    GLfloat matrix[16];
    GLfloat rx = PIf*anglex/180.f;
    GLfloat ry = PIf*angley/180.f;
    GLfloat rz = PIf*anglez/180.f;
    GLfloat sx = (GLfloat)sinf(rx);
    GLfloat cx = (GLfloat)cosf(rx);
    GLfloat sy = (GLfloat)sinf(ry);
    GLfloat cy = (GLfloat)cosf(ry);
    GLfloat sz = (GLfloat)sinf(rz);
    GLfloat cz = (GLfloat)cosf(rz);
    
    sglLoadIdentityf(matrix);
	
    matrix[0] = cy*cz;
    matrix[1] = cy*sz;
    matrix[2] = -sy;
	
    matrix[4] = cz*sx*sy-cx*sz;
    matrix[5] = cx*cz+sx*sy*sz;
    matrix[6] = cy*sx;
	
    matrix[8] = cx*cz*sy+sx*sz;
    matrix[9] = -cz*sx+cx*sy*sz;
    matrix[10] = cx*cy;
	
    sglMultMatrixf(result, result, matrix);
}

/**
 * Transposes a matrix.
 * 
 * @param result the resulting matrix
 * @param matrix the source matrix
 */	
GLvoid sglTransposeMatrixf(GLfloat result[16], const GLfloat matrix[16])
{
    GLfloat temp[16];
    
    for (GLuint i = 0; i < 16; i++)
    {
        temp[i] = matrix[i];
    }
	
    for (GLuint column = 0; column < 4; column++)
    {
        for (GLuint row = 0; row < 4; row++)
        {
            result[row*4+column] = temp[column*4+row];
        }		
    }
}

/**
 * Calculates the invers of a matrix.
 * 
 * @param result the resulting matrix
 * @param matrix the source matrix
 * 
 * @return true, if a inverse matrix could be found
 */	
GLboolean sglInverseMatrixf(GLfloat result[16], const GLfloat matrix[16])
{
    GLint column;
    
    GLfloat copy[16];
    
    sglLoadIdentityf(result);
    
    //
    // Copy the original matrix as we want to manipulate it
    //
    for (GLint i = 0; i < 16; i++)
    {
        copy[i] = matrix[i];
    }
    
    //
    // Make triangle form
    //
    for (column = 0; column < 4; column++)
    {
        GLint row;
        
        for (row = column; row < 4; row++)
        {
            //
            // Is row all zero, then return false			
            //
            if (sglIsRowZerof(copy, row))
                return GL_FALSE;	
            
            //
            // Divide, if not zero, by copy[column*4+row]
            //
            if (copy[column*4+row] != 0.0f)
            {
                sglDevideRowByf(result, copy, row, copy[column*4+row]);
            }
        }
        
        //
        // Is column all zero, then return false
        //		
        if (sglIsColumnZerof(copy, column))
            return GL_FALSE;
        
        //
        // Is pivot [column*4+column] = 0.0f
        //
        for (row = column+1; row < 4; row++)
        {
            if (copy[column*4+row] == 1.0f)
            {
                //
                // Swap with pivot row = column
                //
                sglSwapRowf(result, copy, column, row);
                
                break;
            }
        }
        
        for (row = column+1; row < 4; row++)
        {
            //
            // Subtract, [column*4+row] not zero, current row minus pivot row = column
            //
            if (copy[column*4+row] != 0.0f)
            {
                sglAddRowf(result, copy, row, column, -1.0f);
            }
        }
    }
    
    //
    // Make diagonal form
    //
    for (column = 3; column >= 0; column--)
    {
        for (GLint row = column-1; row >= 0; row--)
        {
            //
            // Subtract, if [column*4+row] not zero, current row minus pivot row = column with factor [column*4+row]
            //
            if (copy[column*4+row] != 0.0f)
            {
                sglAddRowf(result, copy, row, column, -copy[column*4+row]);
            }
        }
    }
	
    return GL_TRUE;
}





/**
 * Converts Euler Angles to a quaternion rotation.
 * 
 * @param result the resulting quaternion
 * @param anglex the x angle
 * @param angley the y angle
 * @param anglez the z angle
 */
GLvoid sglAngleToQuaternionf(GLfloat result[4], GLfloat anglex, GLfloat angley, GLfloat anglez)
{
    //
    // Half-angle, so not 2*M_PI
    //
    GLfloat cosAngleZ = (GLfloat)cosf(PIf*anglez/360.0f);
    GLfloat sinAngleZ = (GLfloat)sinf(PIf*anglez/360.0f);
    GLfloat cosAngleY = (GLfloat)cosf(PIf*angley/360.0f);
    GLfloat sinAngleY = (GLfloat)sinf(PIf*angley/360.0f);
    GLfloat cosAngleX = (GLfloat)cosf(PIf*anglex/360.0f);
    GLfloat sinAngleX = (GLfloat)sinf(PIf*anglex/360.0f);
//짐벌락 현상이 나온다. (내 생각에는 잘못되었음)
//    result[0] = cosAngleZ*cosAngleY*cosAngleX + sinAngleZ*sinAngleY*sinAngleX;
//    result[1] = cosAngleZ*cosAngleY*sinAngleX - sinAngleZ*sinAngleY*cosAngleX;
//    result[2] = cosAngleZ*sinAngleY*cosAngleX + sinAngleZ*cosAngleY*sinAngleX;
//    result[3] = sinAngleZ*cosAngleY*cosAngleX - cosAngleZ*sinAngleY*sinAngleX;
    //수정하면 짐벌락 현상이 나오지 않음. 참조 : http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
    result[0] = cosAngleZ*cosAngleY*cosAngleX - sinAngleZ*sinAngleY*sinAngleX;
    result[1] = cosAngleZ*cosAngleY*sinAngleX + sinAngleZ*sinAngleY*cosAngleX;
    result[2] = cosAngleZ*sinAngleY*cosAngleX - sinAngleZ*cosAngleY*sinAngleX;
    result[3] = sinAngleZ*cosAngleY*cosAngleX + cosAngleZ*sinAngleY*sinAngleX;
}

/**
 * Does a spherical interpolation (SLERP) with two quaternions.
 * 
 * @param result the resulting quaternion
 * @param quaternion0 the first quaternion
 * @param quaternion1 the second quaternion
 * @param t the ratio (between 0 and 1) to interpolate
 */
GLvoid sglSlerpf(GLfloat result[4], const GLfloat quaternion0[4], const GLfloat quaternion1[4], GLclampf t)
{
    GLfloat	factor0, factor1;
    GLfloat	target[4];
    
    GLfloat	angle;
    
    if (t < 0.0f)
        t = 0.0f;
    if (t > 1.0f)
        t = 1.0f;
    
    if (t == 0.0f)
    {
        result[0] = quaternion0[0];
        result[1] = quaternion0[1];
        result[2] = quaternion0[2];
        result[3] = quaternion0[3];
        
        return;
    }
    
    if (t == 1.0f)
    {
        result[0] = quaternion1[0];
        result[1] = quaternion1[1];
        result[2] = quaternion1[2];
        result[3] = quaternion1[3];
        
        return;
    }
    
    //
    // Quaternion has unit length of 1, so no division of amount necessary
    //
    angle = (GLfloat)acosf(quaternion0[0]*quaternion1[0] + 
                           quaternion0[1]*quaternion1[1] + 
                           quaternion0[2]*quaternion1[2] + 
                           quaternion0[3]*quaternion1[3]);
	
    if (2.0f*angle > PIf)
    {
        //
        // Note, these are half angles
        //
        angle -= PIf;
        target[0] = -quaternion1[0];
        target[1] = -quaternion1[1];
        target[2] = -quaternion1[2];
        target[3] = -quaternion1[3];
    }
    else
    {
        target[0] = quaternion1[0];
        target[1] = quaternion1[1];
        target[2] = quaternion1[2];
        target[3] = quaternion1[3];
    }
	
	
    //
    // If angle is less than one degree, we do linear interpolation.
    // Otherwise sin(angle) is almost zero and we get invalid results
    //
    if (2.0f*angle > 2.0f*PIf/360.0f || 2.0f*angle < -2.0f*PIf/360.0f)
    {	
        factor0 = (GLfloat)(sinf((1.0f-t)*angle)/sinf(angle));
        factor1 = (GLfloat)(sinf(t*angle)/sinf(angle));
    }
    else
    {
        factor0 = 1.0f - t;
        factor1 = t;
        
        //
        // Normally, the result should be normalized in this case.
        // But the gap is minimal.
        //
    }
    
    result[0] = quaternion0[0]*factor0 + target[0]*factor1;
    result[1] = quaternion0[1]*factor0 + target[1]*factor1;
    result[2] = quaternion0[2]*factor0 + target[2]*factor1;
    result[3] = quaternion0[3]*factor0 + target[3]*factor1;
}

/**
 * Converts a quaternion to a rotation matrix.
 * 
 * @param result the resulting quaternion
 * @param quaternion the quaternion to convert
 */
GLvoid sglQuaternionToMatrixf(GLfloat result[16], GLfloat quaternion[4])
{
    result[0] = 2.0f*quaternion[0]*quaternion[0] - 1.0f + 2.0f*quaternion[1]*quaternion[1];
    result[1] = 2.0f*quaternion[1]*quaternion[2] + 2.0f*quaternion[0]*quaternion[3];
    result[2] = 2.0f*quaternion[1]*quaternion[3] - 2.0f*quaternion[0]*quaternion[2];
    result[3] = 0.0f;
    result[4] = 2.0f*quaternion[1]*quaternion[2] - 2.0f*quaternion[0]*quaternion[3];
    result[5] = 2.0f*quaternion[0]*quaternion[0] - 1.0f + 2.0f*quaternion[2]*quaternion[2];
    result[6] = 2.0f*quaternion[2]*quaternion[3] + 2.0f*quaternion[0]*quaternion[1];
    result[7] = 0.0f;
    result[8] = 2.0f*quaternion[1]*quaternion[3] + 2.0f*quaternion[0]*quaternion[2];
    result[9] = 2.0f*quaternion[2]*quaternion[3] - 2.0f*quaternion[0]*quaternion[1];
    result[10] = 2.0f*quaternion[0]*quaternion[0] - 1.0f + 2.0f*quaternion[3]*quaternion[3];
    result[11] = 0.0f;
    result[12] = 0.0f;
    result[13] = 0.0f;
    result[14] = 0.0f;
    result[15] = 1.0f;
}



/**
 * Distance between two points.
 * 
 * @param a first point
 * @param b second point
 * 
 * @return the distance
 */
GLfloat sglDistancePointPointf(const GLfloat a[3], const GLfloat b[3])
{
    return (GLfloat)sqrt(sglSquareDistancePointPointf(a,b));
}

/**
 * Square distance between two points.
 * 
 * @param a first point
 * @param b second point
 * 
 * @return the square distance
 */
GLfloat sglSquareDistancePointPointf(const GLfloat a[3], const GLfloat b[3])
{
    return (a[0]-b[0])*(a[0]-b[0]) + (a[1]-b[1])*(a[1]-b[1]) + (a[2]-b[2])*(a[2]-b[2]);
}

GLfloat sglSquareDistanceSPointSPointf(const SPoint* a, const SPoint* b)
{
    return (a->x-b->x)*(a->x-b->x) + (a->y-b->y)*(a->y-b->y) + (a->z-b->z)*(a->z-b->z);
}


/**
 * Calculates the closest point from a triangle to a given point.
 * 
 * @param result	the closest point
 * @param a			point a
 * @param b			point b
 * @param c			point c
 * @param p			the point to calculate the closest point to
 */
GLvoid sglClosestPointFromTriangleToPointf(GLfloat result[3], const GLfloat a[3], const GLfloat b[3], const GLfloat c[3], const GLfloat p[3])
{
    //
    // Check first region at a
    //
    
    GLfloat	ab[3];
    GLfloat ac[3];
	
    sglSubf(ab, b, a);
    sglSubf(ac, c, a);
	
    GLfloat ap[3];
    
    sglSubf(ap, p, a);
    
    GLfloat pDotab = sglDotProductf(ap, ab);
    GLfloat pDotac = sglDotProductf(ap, ac);
    
    //
    // Outside Voronoi region of a, then return a
    //
    if (pDotab <= 0.0f && pDotac <= 0.0f)
    {
        result[0] = a[0];
        result[1] = a[1];
        result[2] = a[2];
        return;
    }
    
    //
    // Check second region at b
    //
    
    GLfloat	ba[3];
    GLfloat	bc[3];
    
    sglSubf(ba, a, b);
    sglSubf(bc, c, b);
    
    GLfloat bp[3];
    
    sglSubf(bp, p, b);
	
    GLfloat pDotba = sglDotProductf(bp, ba);
    GLfloat pDotbc = sglDotProductf(bp, bc);
	
    //
    // Outside Voronoi region of b, then return b
    //
    if (pDotba <= 0.0f && pDotbc <= 0.0f)
    {
        result[0] = b[0];
        result[1] = b[1];
        result[2] = b[2];
        return;
    }
	
    //
    // Check third region at c
    //
    
    GLfloat	ca[3];
    GLfloat	cb[3];
    
    sglSubf(ca, a, c);
    sglSubf(cb, b, c);
    
    GLfloat cp[3];
    
    sglSubf(cp, p, c);
	
    GLfloat pDotca = sglDotProductf(cp, ca);
    GLfloat pDotcb = sglDotProductf(cp, cb);
	
    //
    // Outside Voronoi region of c, then return c
    //
    if (pDotca <= 0.0f && pDotcb <= 0.0f)
    {
        result[0] = c[0];
        result[1] = c[1];
        result[2] = c[2];
        return;
    }
    
    //
    // Normal vector can be used for all edges ...
    //
    GLfloat n[3];
    
    //
    // ... so calculate only once
    //
    sglCrossProductf(n, ab, ac);
    
    //
    // Now check if point lies within ab
    //
    GLfloat crosspapb[3];
    
    GLfloat	pa[3];
    GLfloat	pb[3];
    
    sglSubf(pa, a, p);
    sglSubf(pb, b, p);
    
    sglCrossProductf(crosspapb, pa, pb);
    
    GLfloat volumec = sglDotProductf(n, crosspapb);
    
    if (volumec <= 0.0f && pDotab >= 0.0f && pDotba >= 0.0f)
    {
        //
        // pDotab/(pDotab + pDotba) is the ration between a and b
        //
        result[0] = a[0] + pDotab/(pDotab + pDotba)*ab[0];
        result[1] = a[1] + pDotab/(pDotab + pDotba)*ab[1];
        result[2] = a[2] + pDotab/(pDotab + pDotba)*ab[2];
        return;
    }
	
    //
    // Now check if point lies within bc
    //
    GLfloat crosspbpc[3];
    
    GLfloat	pc[3];
    
    sglSubf(pc, c, p);
    
    sglCrossProductf(crosspbpc, pb, pc);
    
    GLfloat volumea = sglDotProductf(n, crosspbpc);
    
    if (volumea <= 0.0f && pDotbc >= 0.0f && pDotcb >= 0.0f)
    {
        result[0] = b[0] + pDotbc/(pDotbc + pDotcb)*bc[0];
        result[1] = b[1] + pDotbc/(pDotbc + pDotcb)*bc[1];
        result[2] = b[2] + pDotbc/(pDotbc + pDotcb)*bc[2];
        return;
    }
	
    //
    // Now check if point lies within ca
    //
    GLfloat crosspcpa[3];
    
    sglCrossProductf(crosspcpa, pc, pa);
    
    GLfloat volumeb = sglDotProductf(n, crosspcpa);
    
    if (volumeb <= 0.0f && pDotca >= 0.0f && pDotac >= 0.0f)
    {
        result[0] = c[0] + pDotca/(pDotca + pDotac)*ca[0];
        result[1] = c[1] + pDotca/(pDotca + pDotac)*ca[1];
        result[2] = c[2] + pDotca/(pDotca + pDotac)*ca[2];
        return;
    }
	
    //
    // If we did get so far, the point lies inside the triangle...	
    //	
	
    GLfloat u = volumea / (volumea + volumeb + volumec);
    GLfloat v = volumeb / (volumea + volumeb + volumec);
    GLfloat w = 1.0f - u - v;
    
    //
    // ... so we can use barycentric coordiantes
    //
    result[0] = u*a[0] + v*b[0] + w*c[0];
    result[1] = u*a[1] + v*b[1] + w*c[1];
    result[2] = u*a[2] + v*b[2] + w*c[2];
}












/**
 * Checks the intersection between a line and a plane.
 * 
 * @param p	starting point where the line goes through
 * @param q end point of the line
 * @param a first point of the plane
 * @param b second point of the plane
 * @param c third point of the plane
 * @param t variable to store t with P + (Q-P)*t
 * @param u barycentric coordinate u
 * @param v barycentric coordinate v
 * @param w barycentric coordinate w
 * 
 * @return true, if there is an intersection
 */
GLboolean sglLinePlaneIntersectionf(const GLfloat p[3], const GLfloat q[3], const GLfloat a[3], const GLfloat b[3], const GLfloat c[3], GLfloat& t, GLfloat& u, GLfloat& v, GLfloat& w)
{
    GLfloat qp[3];
    GLfloat ab[3];
    GLfloat ac[3];
    GLfloat ap[3];
    
    GLfloat denominator;
    
    GLfloat crossabac[3];
    GLfloat crossapac[3];
    GLfloat crossabap[3];
    
    //
    // we calculate the intersection point with Cramer's rule
    //
    
    for (GLubyte i = 0; i < 3; i++)
    {
        qp[i] = p[i] - q[i];
        ab[i] = b[i] - a[i];
        ac[i] = c[i] - a[i];
        ap[i] = p[i] - a[i];
    }
    
    sglCrossProductf(crossabac, ab, ac);
    denominator = sglDotProductf(qp, crossabac);
    
    if (denominator == 0.0f)
    {
        return GL_FALSE;
    }
	
    sglCrossProductf(crossapac, ap, ac);
    sglCrossProductf(crossabap, ab, ap);
	
    t = sglDotProductf(ap, crossabac) / denominator;
    v = sglDotProductf(qp, crossapac) / denominator;
    w = sglDotProductf(qp, crossabap) / denominator;
    u = 1.0f - v - w;
    
    return GL_TRUE;
}

/**
 * Triangle vs. Triangle intersection test.
 * 
 * @param t0 the first triangle
 * @param t1 the second triangle
 * 
 * @return true, if an intersection did occur
 */
GLboolean sglTriangleTriangleIntersectionf(const GLfloat t0[3*3], const GLfloat t1[3*3])
{
    GLubyte i;
    
    GLfloat t;
    GLfloat u;
    GLfloat v;
    GLfloat w;
    
    GLfloat	ab[3];
    GLfloat	ac[3];
    GLfloat normal[3];
    GLfloat	ap[3];
    GLbyte	sign = 0;
    
    sglSubf(ab, &t0[3*1], &t0[3*0]);
    sglSubf(ac, &t0[3*2], &t0[3*0]);
    sglCrossProductf(normal, ab, ac);
    
    // pre test
    for (i = 0; i < 3; i++)
    {
        GLfloat result;
        
        sglSubf(ap, &t1[3*i], &t0[3*0]);
        
        result = sglDotProductf(ap, normal);
        
        if (result == 0.0f)
        {
            // point lies on plane
            sign = 0;
            
            break;
        }
        
        if (i == 0)
        {
            if (result > 0.0f)
            {
                sign = 1;
            }
            else
            {
                sign = -1;
            }
        }
        else
        {
            if (result > 0.0f && sign == -1)
            {
                // point on opposite site
                sign = 0;
                
                break;
            }
            
            if (result < 0.0f && sign == 1)
            {
                // point on opposite site
                sign = 0;
                
                break;
            }			
        }		
    }
    
    if (sign)
    {
        // points are all on the other side
		
        return GL_FALSE;
    }
    
	
    sglSubf(ab, &t1[3*1], &t1[3*0]);
    sglSubf(ac, &t1[3*2], &t1[3*0]);
    sglCrossProductf(normal, ab, ac);
    
    // pre test
    for (i = 0; i < 3; i++)
    {
        GLfloat result;
        
        sglSubf(ap, &t0[3*i], &t1[3*0]);
        
        result = sglDotProductf(ap, normal);
        
        if (result == 0.0f)
        {
            // point lies on plane
            sign = 0;
            
            break;
        }
        
        if (i == 0)
        {
            if (result > 0.0f)
            {
                sign = 1;
            }
            else
            {
                sign = -1;
            }
        }
        else
        {
            if (result > 0.0f && sign == -1)
            {
                // point on opposite site
                sign = 0;
                
                break;
            }
            
            if (result < 0.0f && sign == 1)
            {
                // point on opposite site
                sign = 0;
                
                break;
            }			
        }		
    }
    
    if (sign)
    {
        // points are all on the other side
		
        return GL_FALSE;
    }
    
    //
    // Now we need to test segments against the triangle ...
    //
    
    // for every edge of triangle one
    for (i = 0; i < 3; i++)
    {
        if (sglLinePlaneIntersectionf(&t0[3*i], &t0[3*((i+1)%3)], &t1[0], &t1[3], &t1[6], t, u, v, w))
        {			
            if (t >= 0.0f && t <= 1.0f)
            {
                if (v >= 0 && w >= 0 && (v+w) <= 1.0f)
                {
                    return GL_TRUE;
                }
            }			
        }
    }
	
    // for every edge of triangle two
    for (i = 0; i < 3; i++)
    {
        if (sglLinePlaneIntersectionf(&t1[3*i], &t1[3*((i+1)%3)], &t0[0], &t0[3], &t0[6], t, u, v, w))
        {
            if (t >= 0.0f && t <= 1.0f)
            {
                if (v >= 0 && w >= 0 && (v+w) <= 1.0f)
                {
                    return GL_TRUE;
                }
            }			
        }
    }
    
    return GL_FALSE;
}




GLfloat xmin_3d = 0;
GLfloat xmax_3d = 0;
GLfloat ymin_3d = 0;
GLfloat ymax_3d = 0;

GLvoid sglPerspectivef(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar)
{
    //GLfloat xmin, xmax, ymin, ymax;
    
    ymax_3d = zNear * (GLfloat)tan(fovy * PIf / 360.0f);
    ymin_3d = -ymax_3d;
    xmin_3d = ymin_3d * aspect;
    xmax_3d = ymax_3d * aspect;
    
#if defined ANDROID || defined IPHONE
	glFrustumf(xmin_3d, xmax_3d, ymin_3d, ymax_3d, zNear, zFar);
#else //MAC
	glFrustum(xmin_3d, xmax_3d, ymin_3d, ymax_3d, zNear, zFar);
#endif
	
}

GLvoid sglCalculatePerspectivef(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar)
{
    //GLfloat xmin, xmax, ymin, ymax;
    
    ymax_3d = zNear * (GLfloat)tan(fovy * PIf / 360.0f);
    ymin_3d = -ymax_3d;
    xmin_3d = ymin_3d * aspect;
    xmax_3d = ymax_3d * aspect;
}

GLvoid sglLookAtf(GLfloat eyeX, GLfloat eyeY, GLfloat eyeZ, GLfloat centerX, GLfloat centerY, GLfloat centerZ, GLfloat upX, GLfloat upY, GLfloat upZ )
{
    GLfloat forward[3], side[3], up[3];
    GLfloat matrix[16];
    
    forward[0] = centerX - eyeX;
    forward[1] = centerY - eyeY;
    forward[2] = centerZ - eyeZ;
	sglNormalizef(forward);
    
    up[0] = upX;
    up[1] = upY;
    up[2] = upZ;
    
	sglCrossProductf(side, forward, up);
	sglNormalizef(side);
	sglCrossProductf(up, side, forward);
    
	matrix[0] = side[0];
	matrix[1] = up[0];
	matrix[2] = -forward[0];
	matrix[3] = 0.0f;
	matrix[4] = side[1];
	matrix[5] = up[1];
	matrix[6] = -forward[1];
	matrix[7] = 0.0f;
	matrix[8] = side[2];
	matrix[9] = up[2];
	matrix[10] = -forward[2];
	matrix[11] = 0.0f;
	matrix[12] = 0.0f;
	matrix[13] = 0.0f;
	matrix[14] = 0.0f;
	matrix[15] = 1.0f;
    
    glMultMatrixf(matrix);
    glTranslatef(-eyeX, -eyeY, -eyeZ);
}


GLclampf sglRed = 1.0f;

GLclampf sglGreen = 1.0f;

GLclampf sglBlue = 1.0f;

GLclampf sglAlpha = 1.0f;

GLvoid sglColorf(GLclampf r, GLclampf g, GLclampf b, GLclampf a)
{
	if (r < 0.0f)
		sglRed = 0.0f;
	else if (r > 1.0f)
		sglRed = 1.0f;
	else
		sglRed = r;
    
	if (g < 0.0f)
		sglGreen = 0.0f;
	else if (g > 1.0f)
		sglGreen = 1.0f;
	else
		sglGreen = g;
    
	if (b < 0.0f)
		sglBlue = 0.0f;
	else if (b > 1.0f)
		sglBlue = 1.0f;
	else
		sglBlue = b;
    
	if (a < 0.0f)
		sglAlpha = 0.0f;
	else if (a > 1.0f)
		sglAlpha = 1.0f;
	else
		sglAlpha = a;
}

GLvoid sglLinef(const GLfloat s[3], const GLfloat e[3])
{
	static GLfloat points[3*2];
    
	points[0] = s[0];
	points[1] = s[1];
	points[2] = s[2];
    
	points[3] = e[0];
	points[4] = e[1];
	points[5] = e[2];
    
	glColor4f(sglRed, sglGreen, sglBlue, sglAlpha);
    
	glVertexPointer(3, GL_FLOAT, 0, points);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDrawArrays(GL_LINES, 0, 2);	
	glDisableClientState(GL_VERTEX_ARRAY);
}

GLvoid sglCirclef(const GLfloat p[3], const GLfloat o[3], GLfloat radius)
{
	static const GLubyte LINES = 32;
	static GLfloat points[3*LINES];
	static GLfloat worldpoints[3*LINES];
	static GLfloat matrix[16];
	static GLboolean init = GL_TRUE;
    
	if (init)
	{
		for (GLubyte i = 0; i < LINES; i++)
		{
			points[i*3] = radius*(GLfloat)cosf(2.0f*PIf*i/LINES);		
			points[i*3+1] = radius*(GLfloat)sinf(2.0f*PIf*i/LINES);		
			points[i*3+2] = 0.0f;
		}
        
		init = GL_FALSE;
	}
    
	sglLoadIdentityf(matrix);
	sglTranslateMatrixf(matrix, p[0], p[1], p[2]);
	sglRotateRzRyRxMatrixf(matrix, o[0], o[1], o[2]);
    
	for (GLubyte i = 0; i < LINES; i++)
	{
		sglMultMatrixVectorf(&worldpoints[i*3], matrix, &points[i*3]);
	}
    
	glColor4f(sglRed, sglGreen, sglBlue, sglAlpha);
    
	glVertexPointer(3, GL_FLOAT, 0, worldpoints);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDrawArrays(GL_LINE_LOOP, 0, LINES);	
	glDisableClientState(GL_VERTEX_ARRAY);
}






