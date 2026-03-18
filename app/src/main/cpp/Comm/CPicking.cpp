//
//  CPicking.cpp
//  SongGL
//
//  Created by Song Hohak on 11. 10. 26..
//  Copyright (c) 2011 thinkm. All rights reserved.
//

#include <iostream>
#include <math.h>
#include "sGL.h"
#include "sGLDefine.h"
#include "CPicking.h"

float CPicking::projection[16];
GLint CPicking::viewport[4];

CPicking::CPicking()
{
    
}

CPicking::~CPicking()
{
    
}

void CPicking::ResetPickingViewPort()
{
    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetFloatv(GL_PROJECTION_MATRIX, projection);
    
    glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
}


//void CPicking::Initialize()
//{
//   
//    glGetIntegerv(GL_VIEWPORT, viewport);
//    glGetFloatv(GL_PROJECTION_MATRIX, projection);
//}



//Start Picking  ------------------------
void transform_point(GLfloat out[4], const GLfloat m[16], const GLfloat in[4])
{
#define M(row,col)  m[col*4+row]
    out[0] =
    M(0, 0) * in[0] + M(0, 1) * in[1] + M(0, 2) * in[2] + M(0, 3) * in[3];
    out[1] =
    M(1, 0) * in[0] + M(1, 1) * in[1] + M(1, 2) * in[2] + M(1, 3) * in[3];
    out[2] =
    M(2, 0) * in[0] + M(2, 1) * in[1] + M(2, 2) * in[2] + M(2, 3) * in[3];
    out[3] =
    M(3, 0) * in[0] + M(3, 1) * in[1] + M(3, 2) * in[2] + M(3, 3) * in[3];
#undef M
}

void matmul(GLfloat * product, const GLfloat * a, const GLfloat * b)
{
    /* This matmul was contributed by Thomas Malik */
    GLfloat temp[16];
    GLint i;
    
#define A(row,col)  a[(col<<2)+row]
#define B(row,col)  b[(col<<2)+row]
#define T(row,col)  temp[(col<<2)+row]
    
    /* i-te Zeile */
    for (i = 0; i < 4; i++) {
        T(i, 0) =
        A(i, 0) * B(0, 0) + A(i, 1) * B(1, 0) + A(i, 2) * B(2, 0) + A(i,
                                                                      3) *
        B(3, 0);
        T(i, 1) =
        A(i, 0) * B(0, 1) + A(i, 1) * B(1, 1) + A(i, 2) * B(2, 1) + A(i,
                                                                      3) *
        B(3, 1);
        T(i, 2) =
        A(i, 0) * B(0, 2) + A(i, 1) * B(1, 2) + A(i, 2) * B(2, 2) + A(i,
                                                                      3) *
        B(3, 2);
        T(i, 3) =
        A(i, 0) * B(0, 3) + A(i, 1) * B(1, 3) + A(i, 2) * B(2, 3) + A(i,
                                                                      3) *
        B(3, 3);
    }
    
#undef A
#undef B
#undef T
    memcpy(product, temp, 16 * sizeof(GLfloat));
}

int invert_matrix(const GLfloat * m, GLfloat * out)
{
    /* NB. OpenGL Matrices are COLUMN major. */
#define SWAP_ROWS(a, b) { GLfloat *_tmp = a; (a)=(b); (b)=_tmp; }
#define MAT(m,r,c) (m)[(c)*4+(r)]
    
    GLfloat wtmp[4][8];
    GLfloat m0, m1, m2, m3, s;
    GLfloat *r0, *r1, *r2, *r3;
    
    r0 = wtmp[0], r1 = wtmp[1], r2 = wtmp[2], r3 = wtmp[3];
    
    r0[0] = MAT(m, 0, 0), r0[1] = MAT(m, 0, 1),
    r0[2] = MAT(m, 0, 2), r0[3] = MAT(m, 0, 3),
    r0[4] = 1.f, r0[5] = r0[6] = r0[7] = 0.f,
    r1[0] = MAT(m, 1, 0), r1[1] = MAT(m, 1, 1),
    r1[2] = MAT(m, 1, 2), r1[3] = MAT(m, 1, 3),
    r1[5] = 1.f, r1[4] = r1[6] = r1[7] = 0.f,
    r2[0] = MAT(m, 2, 0), r2[1] = MAT(m, 2, 1),
    r2[2] = MAT(m, 2, 2), r2[3] = MAT(m, 2, 3),
    r2[6] = 1.f, r2[4] = r2[5] = r2[7] = 0.f,
    r3[0] = MAT(m, 3, 0), r3[1] = MAT(m, 3, 1),
    r3[2] = MAT(m, 3, 2), r3[3] = MAT(m, 3, 3),
    r3[7] = 1.f, r3[4] = r3[5] = r3[6] = 0.f;
    
    /* choose pivot - or die */
    if (fabsf(r3[0]) > fabsf(r2[0]))
        SWAP_ROWS(r3, r2);
    if (fabsf(r2[0]) > fabsf(r1[0]))
        SWAP_ROWS(r2, r1);
    if (fabsf(r1[0]) > fabsf(r0[0]))
        SWAP_ROWS(r1, r0);
    if (0.f == r0[0])
        return GL_FALSE;
    
    /* eliminate first variable     */
    m1 = r1[0] / r0[0];
    m2 = r2[0] / r0[0];
    m3 = r3[0] / r0[0];
    s = r0[1];
    r1[1] -= m1 * s;
    r2[1] -= m2 * s;
    r3[1] -= m3 * s;
    s = r0[2];
    r1[2] -= m1 * s;
    r2[2] -= m2 * s;
    r3[2] -= m3 * s;
    s = r0[3];
    r1[3] -= m1 * s;
    r2[3] -= m2 * s;
    r3[3] -= m3 * s;
    s = r0[4];
    if (s != 0.f) {
        r1[4] -= m1 * s;
        r2[4] -= m2 * s;
        r3[4] -= m3 * s;
    }
    s = r0[5];
    if (s != 0.f) {
        r1[5] -= m1 * s;
        r2[5] -= m2 * s;
        r3[5] -= m3 * s;
    }
    s = r0[6];
    if (s != 0.f) {
        r1[6] -= m1 * s;
        r2[6] -= m2 * s;
        r3[6] -= m3 * s;
    }
    s = r0[7];
    if (s != 0.f) {
        r1[7] -= m1 * s;
        r2[7] -= m2 * s;
        r3[7] -= m3 * s;
    }
    
    /* choose pivot - or die */
    if (fabsf(r3[1]) > fabsf(r2[1]))
        SWAP_ROWS(r3, r2);
    if (fabsf(r2[1]) > fabsf(r1[1]))
        SWAP_ROWS(r2, r1);
    if (0.f == r1[1])
        return GL_FALSE;
    
    /* eliminate second variable */
    m2 = r2[1] / r1[1];
    m3 = r3[1] / r1[1];
    r2[2] -= m2 * r1[2];
    r3[2] -= m3 * r1[2];
    r2[3] -= m2 * r1[3];
    r3[3] -= m3 * r1[3];
    s = r1[4];
    if (0.f != s) {
        r2[4] -= m2 * s;
        r3[4] -= m3 * s;
    }
    s = r1[5];
    if (0.f != s) {
        r2[5] -= m2 * s;
        r3[5] -= m3 * s;
    }
    s = r1[6];
    if (0.f != s) {
        r2[6] -= m2 * s;
        r3[6] -= m3 * s;
    }
    s = r1[7];
    if (0.f != s) {
        r2[7] -= m2 * s;
        r3[7] -= m3 * s;
    }
    
    /* choose pivot - or die */
    if (fabs(r3[2]) > fabs(r2[2]))
        SWAP_ROWS(r3, r2);
    if (0.f == r2[2])
        return GL_FALSE;
    
    /* eliminate third variable */
    m3 = r3[2] / r2[2];
    r3[3] -= m3 * r2[3], r3[4] -= m3 * r2[4],
    r3[5] -= m3 * r2[5], r3[6] -= m3 * r2[6], r3[7] -= m3 * r2[7];
    
    /* last check */
    if (0.f == r3[3])
        return GL_FALSE;
    
    s = 1.f / r3[3];        /* now back substitute row 3 */
    r3[4] *= s;
    r3[5] *= s;
    r3[6] *= s;
    r3[7] *= s;
    
    m2 = r2[3];         /* now back substitute row 2 */
    s = 1.f / r2[2];
    r2[4] = s * (r2[4] - r3[4] * m2), r2[5] = s * (r2[5] - r3[5] * m2),
    r2[6] = s * (r2[6] - r3[6] * m2), r2[7] = s * (r2[7] - r3[7] * m2);
    m1 = r1[3];
    r1[4] -= r3[4] * m1, r1[5] -= r3[5] * m1,
    r1[6] -= r3[6] * m1, r1[7] -= r3[7] * m1;
    m0 = r0[3];
    r0[4] -= r3[4] * m0, r0[5] -= r3[5] * m0,
    r0[6] -= r3[6] * m0, r0[7] -= r3[7] * m0;
    
    m1 = r1[2];         /* now back substitute row 1 */
    s = 1.f / r1[1];
    r1[4] = s * (r1[4] - r2[4] * m1), r1[5] = s * (r1[5] - r2[5] * m1),
    r1[6] = s * (r1[6] - r2[6] * m1), r1[7] = s * (r1[7] - r2[7] * m1);
    m0 = r0[2];
    r0[4] -= r2[4] * m0, r0[5] -= r2[5] * m0,
    r0[6] -= r2[6] * m0, r0[7] -= r2[7] * m0;
    
    m0 = r0[1];         /* now back substitute row 0 */
    s = 1.f / r0[0];
    r0[4] = s * (r0[4] - r1[4] * m0), r0[5] = s * (r0[5] - r1[5] * m0),
    r0[6] = s * (r0[6] - r1[6] * m0), r0[7] = s * (r0[7] - r1[7] * m0);
    
    MAT(out, 0, 0) = r0[4];
    MAT(out, 0, 1) = r0[5], MAT(out, 0, 2) = r0[6];
    MAT(out, 0, 3) = r0[7], MAT(out, 1, 0) = r1[4];
    MAT(out, 1, 1) = r1[5], MAT(out, 1, 2) = r1[6];
    MAT(out, 1, 3) = r1[7], MAT(out, 2, 0) = r2[4];
    MAT(out, 2, 1) = r2[5], MAT(out, 2, 2) = r2[6];
    MAT(out, 2, 3) = r2[7], MAT(out, 3, 0) = r3[4];
    MAT(out, 3, 1) = r3[5], MAT(out, 3, 2) = r3[6];
    MAT(out, 3, 3) = r3[7];
    
    return GL_TRUE;
    
#undef MAT
#undef SWAP_ROWS
}



GLint gluUnProject(GLfloat winx, GLfloat winy, GLfloat winz,
                   const GLfloat model[16], const GLfloat proj[16],
                   const GLint viewport[4],
                   GLfloat * objx, GLfloat * objy, GLfloat * objz)
{
    /* matrice de transformation */
    GLfloat m[16], A[16];
    GLfloat in[4], out[4];
    
    /* transformation coordonnees normalisees entre -1 et 1 */
    in[0] = (winx - viewport[0]) * 2 / viewport[2] - 1.f;
    in[1] = (winy - viewport[1]) * 2 / viewport[3] - 1.f;
    in[2] = 2 * winz - 1.f;
    in[3] = 1.f;
    
    /* calcul transformation inverse */
    matmul(A, proj, model);
    invert_matrix(A, m);
    
    /* d'ou les coordonnees objets */
    transform_point(out, m, in);
    if (out[3] == 0.f)
        return GL_FALSE;
    *objx = out[0] / out[3];
    *objy = out[1] / out[3];
    *objz = out[2] / out[3];
    return GL_TRUE;
}

GLvoid CrossVector(SVector* result, SVector* vector0, SVector* vector1)
{
    SVector temp;
    temp.x = vector0->y*vector1->z - vector0->z*vector1->y;
    temp.y = vector0->z*vector1->x - vector0->x*vector1->z;
    temp.z = vector0->x*vector1->y - vector0->y*vector1->x;
    memcpy(result, &temp, sizeof(SVector));
}

GLfloat DotVector(SVector* vector0, SVector* vector1)
{
    return vector0->x*vector1->x + vector0->y*vector1->y + vector0->z*vector1->z;
}


//GLint viewport[4];
//float projection[16],modelview[16];



//Start Picking  ------------------------

//nType = 0 Near
//nType = 1 Far
void CPicking::GetUnProject(GLint winPos[2],SPoint& outPoint,int nType)
{
    //winPos[1] = viewport[3] - winPos[1];
    GLint nwin[2];
    nwin[0] = winPos[0];
    nwin[1] = viewport[3] - winPos[1];;
    gluUnProject(nwin[0], nwin[1] , nType, modelview, projection, viewport, &outPoint.x, &outPoint.y, &outPoint.z);
}

//출처...http://stackoverflow.com/questions/4752595/ray-triangle-intersection-picking-not-working
bool CPicking::sglIsPicking(float vertex0[3],float vertex1[3],float vertex2[3],SPoint& nearPoint,SPoint& farPoint)
{   
    
    //    GLint viewport[4];
    //    float projection[16],modelview[16];
    //    
    
    
//    SPoint nearPoint;
//    SPoint farPoint;

    //glGetIntegerv(GL_VIEWPORT, viewport);
    //winPos[1] = viewport[3] - winPos[1];
    
    //    glGetFloatv(GL_PROJECTION_MATRIX, projection);
    //    glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
    
    //Retreiving position projected on near plane
    //gluUnProject(winPos[0], winPos[1] , 0, modelview, projection, viewport, &nearPoint.x, &nearPoint.y, &nearPoint.z);
    
    //Retreiving position projected on far plane
    //gluUnProject(winPos[0], winPos[1],  1, modelview, projection, viewport, &farPoint.x, &farPoint.y, &farPoint.z);
    
    SVector near;
    near.x = nearPoint.x;near.y = nearPoint.y;near.z = nearPoint.z;
    SVector far;
    far.x = farPoint.x;far.y = farPoint.y;far.z = farPoint.z;
    SVector d; //subtract
    d.x = far.x - near.x;d.y = far.y - near.y;d.z = far.z - near.z;
    
    
    SVector v0;    
    SVector v1;
    SVector v2;
    
    SVector e1,e2;
    
    
    v0.x = vertex0[0];v0.y = vertex0[1];v0.z = vertex0[2];
    v1.x = vertex1[0];v1.y = vertex1[1];v1.z = vertex1[2];
    v2.x = vertex2[0];v2.y = vertex2[1];v2.z = vertex2[2];
    
    //subtract
    e1.x = v1.x - v0.x;e1.y = v1.y - v0.y;e1.z = v1.z - v0.z;
    e2.x = v2.x - v0.x;e2.y = v2.y - v0.y;e2.z = v2.z - v0.z;
    
    SVector p; 
    //memset(&p, 0, sizeof(SVector));
    
    CrossVector(&p,&d,&e2);
    
    float a = DotVector(&e1,&p);
    
    if (a > -.000001 && a < .000001)
    {
        return false;
    }
    
    float f = 1.0f/a;
    SVector s;
    s.x = near.x - v0.x;s.y = near.y - v0.y;s.z = near.z - v0.z;
    float u = f * DotVector(&s, &p);
    
    if (u<0 || u>1) {
        return false;
    }
    
    
    SVector q;
    //memset(&q, 0, sizeof(SVector));
    
    CrossVector(&q, &s, &e1);
    
    float v = f * DotVector(&d,&q);
    
    if (v<0 || (u+v)>1) 
    {
        return false;
    }
    
    return true;
} 

