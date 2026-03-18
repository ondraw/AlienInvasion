//
//  CMS3DModelASCII.cpp
//  SongGL
//
//  Created by 호학 송 on 12. 8. 16..
//
//
#include <math.h>
#include "CMS3DModelASCII.h"
#include "CTextureMan.h"
#include "sGLTrasform.h"
#include "CPicking.h"

CMS3DModelASCII::CMS3DModelASCII(CMS3DASCII *pMS3DASCII) : CMyModel()
{
    mpWorldMesh = NULL;
    scale[0] = 1.0f;
    scale[1] = 1.0f;
    scale[2] = 1.0f;
    mpMS3DASCII = pMS3DASCII;
    init();
//    memset(mfCenter, 0, sizeof(mfCenter));
    mfColidedDistance = 9999.f;
}

CMS3DModelASCII::~CMS3DModelASCII()
{
//    if(pfinalmulMaxtix)
//    {
//        delete[] pfinalmulMaxtix;
//        pfinalmulMaxtix = NULL;
//    }
    if(mpWorldMesh)
    {
        int nSize = mpMS3DASCII->GetMeshSize();
        for(int i = 0; i < nSize; i++)
        {
            delete[] mpWorldMesh[i].pVetexBuffer;
#if CMS3DModelASCII_ORGNORMAL
            if(mpMS3DASCII->IsNormalVector())
                delete[] mpWorldMesh[i].pNormalBuffer;
#endif //FASTNORMAL
        }
        delete[] mpWorldMesh;
        mpWorldMesh = NULL;
    }
}


int CMS3DModelASCII::init()
{
    int nMeshCnt = mpMS3DASCII->GetMeshSize();
    
    mpWorldMesh = new sglMesh[nMeshCnt];
    for(int i = 0; i < nMeshCnt; i++)
    {
        mpWorldMesh[i].pVetexBuffer = new float[mpMS3DASCII->GetMesh()[i].nVetexCnt * 3];
#if CMS3DModelASCII_ORGNORMAL
        if(mpMS3DASCII->IsNormalVector())
            mpWorldMesh[i].pNormalBuffer = new float[mpMS3DASCII->GetMesh()[i].nNormalCnt * 3];
#endif //FASTNORMAL
    }
    return E_SUCCESS;
}

int CMS3DModelASCII::GetMeshSize()
{
    return mpMS3DASCII->GetMeshSize();
}

sglMesh* CMS3DModelASCII::GetMesh()
{
    return mpMS3DASCII->GetMesh();
}

GLvoid		CMS3DModelASCII::updatef(GLuint time)
{

    float matrix[16];
    int nIndex = 0;
    int nSize = mpMS3DASCII->GetMeshSize();
    sglMesh *arrMesh = mpMS3DASCII->GetMesh();
    sglMesh *arrWroldMesh = mpWorldMesh;
    
    sglLoadIdentityf(matrix);
    sglTranslateMatrixf(matrix,position[0],position[1],position[2]); //회전전에 이동을 해야 한다.
    

    //짐벌락현상 해결함.
    GLfloat	rotationMatrix[16];
    GLfloat	quaternion0[4];
    sglAngleToQuaternionf(quaternion0, orientation[0], orientation[1], orientation[2]);
    sglQuaternionToMatrixf(rotationMatrix, quaternion0);
    sglMultMatrixf(matrix, matrix, rotationMatrix);
    sglScaleMatrixf(matrix, scale[0], scale[1], scale[2]);
    if(pfinalmulMaxtix) sglMultMatrixf(matrix, matrix, pfinalmulMaxtix);
    
    for(int j = 0; j < nSize; j++)
    {
        sglMesh* pMesh = &arrMesh[j];
        sglMesh* pWorldMesh = &arrWroldMesh[j];
        for(int i = 0; i < pMesh->nVetexCnt; i++)
        {
            nIndex = i*3;
            //좌표를 이동한다.
            sglMultMatrixVectorf(&pWorldMesh->pVetexBuffer[nIndex],
                                 matrix,
                                 &pMesh->pVetexBuffer[nIndex]);
        }
#if CMS3DModelASCII_ORGNORMAL
        if(mpMS3DASCII->IsNormalVector())
        {
            for(int i = 0; i < pMesh->nNormalCnt; i++)
            {
                nIndex = i*3;
                //좌표를 이동한다.
                sglMultMatrixVectorf(&pWorldMesh->pNormalBuffer[nIndex],
                                     matrix,
                                     &pMesh->pNormalBuffer[nIndex]);
            }
        }
#endif
        
        
    }
}

GLvoid		CMS3DModelASCII::renderf() const
{
    int nSize = mpMS3DASCII->GetMeshSize();
    sglMesh *arrMesh = mpMS3DASCII->GetMesh();
    sglMesh *arrWroldMesh = mpWorldMesh;

    
    glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    if(mpMS3DASCII->IsNormalVector())
    {
        glEnableClientState(GL_NORMAL_ARRAY);
        glEnable(GL_LIGHTING);
#ifdef ANDROID
        glEnable(GL_LIGHT0); //Android
#endif
    }
    
    glEnable(GL_TEXTURE_2D);
    glColor4f(1.0f,1.0f,1.0f,1.0f);
    for(int j = 0; j < nSize; j++)
    {
        sglMesh* pMesh = &arrMesh[j];
        //for(int i = 0; i < pMesh->nVetexCnt; i++)
        //{
        glBindTexture(GL_TEXTURE_2D, pMesh->nglTexutre);
        glVertexPointer(3, GL_FLOAT, 0, arrWroldMesh[j].pVetexBuffer);
#if CMS3DModelASCII_ORGNORMAL
        if(mpMS3DASCII->IsNormalVector())
            glNormalPointer(GL_FLOAT, 0, arrWroldMesh[j].pNormalBuffer);
#else
        if(mpMS3DASCII->IsNormalVector())
            glNormalPointer(GL_FLOAT, 0, pMesh->pNormalBuffer);
#endif //FASTNORMAL
        
        glTexCoordPointer(2, GL_FLOAT, 0, pMesh->pCoordBuffer);
        glDrawElements(GL_TRIANGLES, pMesh->shIndicesCnt * 3, GL_UNSIGNED_SHORT, pMesh->pIndicesBuffer);
        //}
    }
    glDisable(GL_TEXTURE_2D);
    if(mpMS3DASCII->IsNormalVector())
    {
#ifdef ANDROID
        glDisable(GL_LIGHT0); //Android
#endif
        glDisable(GL_LIGHTING);
        glDisableClientState(GL_NORMAL_ARRAY);
    }
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void CMS3DModelASCII::GetCenter(GLfloat *fOutCenter)
{
    memcpy(fOutCenter, position, sizeof(position));
}

bool    CMS3DModelASCII::IsPicking(CPicking* pPick,int nWinPosX,int nWinPosY)
{
    GLint nwin[2];
    nwin[0] = nWinPosX;
    nwin[1] = nWinPosY;
    
    SPoint nearPoint;
    SPoint farPoint;
    pPick->GetUnProject(nwin, nearPoint, 0);
    pPick->GetUnProject(nwin, farPoint, 1);
    return IsPicking(pPick,nearPoint,farPoint,nWinPosX,nWinPosY);
}

bool    CMS3DModelASCII::IsPicking(CPicking* pPick,SPoint& nearPoint,SPoint& farPoint,int nWinPosX,int nWinPosY)
{
    bool b1;
    int nVIndex1,nVIndex2,nVIndex3;
    sglMesh *arrMesh = GetMesh();
    int nSize = GetMeshSize();

    for (int i = 0; i < nSize; i++)
    {
        sglMesh *pWorldMesh = &mpWorldMesh[i];
        sglMesh *pMesh = &arrMesh[i];
        
        int nCntz = pMesh->shIndicesCnt * 3;
        for (int z = 0; z < nCntz; )
        {
            nVIndex1 = pMesh->pIndicesBuffer[z++]*3;
            nVIndex2 = pMesh->pIndicesBuffer[z++]*3;
            nVIndex3 = pMesh->pIndicesBuffer[z++]*3;
            b1 = pPick->sglIsPicking(&pWorldMesh->pVetexBuffer[nVIndex1],
                                     &pWorldMesh->pVetexBuffer[nVIndex2],
                                     &pWorldMesh->pVetexBuffer[nVIndex3],
                                     nearPoint,farPoint);
            if(b1)
            {
                nearPoint.x = pWorldMesh->pVetexBuffer[nVIndex1];
                nearPoint.y = pWorldMesh->pVetexBuffer[nVIndex1+1];
                nearPoint.z = pWorldMesh->pVetexBuffer[nVIndex1+2];
                
                farPoint.x = pWorldMesh->pVetexBuffer[nVIndex2];
                farPoint.y = pWorldMesh->pVetexBuffer[nVIndex2+1];
                farPoint.z = pWorldMesh->pVetexBuffer[nVIndex2+2];
                
                return true;
            }
        }
    }
    return false;
}

//주인공은 버텍스 갯수가 너무 많아서 그룹 0만 검색하자.
bool CMS3DModelASCII::IsPickingZeroGroup(CPicking* pPick,int nWinPosX,int nWinPosY)
{
    bool b1;
    int nVIndex1,nVIndex2,nVIndex3;
    GLint nwin[2];
    nwin[0] = nWinPosX;
    nwin[1] = nWinPosY;
    
    SPoint nearPoint;
    SPoint farPoint;
    pPick->GetUnProject(nwin, nearPoint, 0);
    pPick->GetUnProject(nwin, farPoint, 1);
    
    sglMesh *arrMesh = GetMesh();
    int i = 0; //zero group
    sglMesh *pWorldMesh = &mpWorldMesh[i];
    sglMesh *pMesh = &arrMesh[i];
    
    int nCntz = pMesh->shIndicesCnt * 3;
    for (int z = 0; z < nCntz; )
    {
        nVIndex1 = pMesh->pIndicesBuffer[z++]*3;
        nVIndex2 = pMesh->pIndicesBuffer[z++]*3;
        nVIndex3 = pMesh->pIndicesBuffer[z++]*3;
        b1 = pPick->sglIsPicking(&pWorldMesh->pVetexBuffer[nVIndex1],
                                 &pWorldMesh->pVetexBuffer[nVIndex2],
                                 &pWorldMesh->pVetexBuffer[nVIndex3],
                                 nearPoint,farPoint);
        if(b1)
        {
//            nearPoint.x = pWorldMesh->pVetexBuffer[nVIndex1];
//            nearPoint.y = pWorldMesh->pVetexBuffer[nVIndex1+1];
//            nearPoint.z = pWorldMesh->pVetexBuffer[nVIndex1+2];
//            
//            farPoint.x = pWorldMesh->pVetexBuffer[nVIndex2];
//            farPoint.y = pWorldMesh->pVetexBuffer[nVIndex2+1];
//            farPoint.z = pWorldMesh->pVetexBuffer[nVIndex2+2];
            
            return true;
        }
    }
    return false;
}

GLvoid		CMS3DModelASCII::createBoundingSpheref(GLbyte level)
{
    float fCenter[3];
    float min[3],max[3];
    int nIndex = 0;
    if(mpMS3DASCII->mfRadius == -1)
    {
        sglMesh *arrMesh = GetMesh();
        
        int nSize = GetMeshSize();
        for (int i = 0; i < nSize; i++)
        {
            sglMesh *pMesh = &arrMesh[i];
            for( int j = 0; j < pMesh->nVetexCnt; j++)
            {
                if(i == 0 && j == 0)
                {
                    
                    min[0] = pMesh->pVetexBuffer[0];
                    min[1] = pMesh->pVetexBuffer[1];
                    min[2] = pMesh->pVetexBuffer[2];
                    max[0] = min[0];
                    max[1] = min[1];
                    max[2] = min[2];
                }
                else
                {
                    nIndex = j * 3;
                    if (pMesh->pVetexBuffer[nIndex] < min[0])
                        min[0] = pMesh->pVetexBuffer[nIndex];
                    if(pMesh->pVetexBuffer[nIndex + 1] < min[1])
                        min[1] = pMesh->pVetexBuffer[nIndex + 1];
                    if(pMesh->pVetexBuffer[nIndex + 2] < min[2])
                        min[2] = pMesh->pVetexBuffer[nIndex + 2];
                    
                    
                    if (pMesh->pVetexBuffer[nIndex] > max[0])
                        max[0] = pMesh->pVetexBuffer[nIndex];
                    if(pMesh->pVetexBuffer[nIndex + 1] > max[1])
                        max[1] = pMesh->pVetexBuffer[nIndex + 1];
                    if(pMesh->pVetexBuffer[nIndex + 2] > max[2])
                        max[2] = pMesh->pVetexBuffer[nIndex + 2];
                }
            }
        }
    
        
        for (GLushort i = 0; i < 3; i++)
        {
            fCenter[i] = (min[i] + max[i]) / 2.0f;
        }
        
        mpMS3DASCII->mfRadius = (GLfloat)sqrt(
                                              (max[0]-fCenter[0])*(max[0]-fCenter[0]) +
                                              (max[1]-fCenter[1])*(max[1]-fCenter[1]) +
                                              (max[2]-fCenter[2])*(max[2]-fCenter[2])
                                              ) * scale[0];
    }
}

GLboolean	CMS3DModelASCII::collidesBoundingSpheref(GLbyte* search, GLubyte searchSize, GLbyte* hitJoint, CMyModel& model, GLbyte* searchModel, GLubyte searchSizeModel, GLbyte* hitJointModel, GLboolean exact, GLbyte stopAtLevel)
{

    float modelPT[3];
    float PT[3];
    
    model.GetCenter(modelPT);
    GetCenter(PT);
    
    
    float fRadius = GetRadius();
    float fRadiusSum = (fRadius + model.GetRadius()) / 2.0f; //2.0f를 한이유는 객체가 서로 부딛히는 비율을 조금 작게 하였다.
    
    //(a[0]-b[0])*(a[0]-b[0]) + (a[1]-b[1])*(a[1]-b[1]) + (a[2]-b[2])*(a[2]-b[2]);
    float fx = modelPT[0] - PT[0];
    float fy = modelPT[1] - PT[1];
    float fz = modelPT[2] - PT[2];
    
    mfColidedDistance = fx * fx + fy * fy + fz * fz;
    if( mfColidedDistance < (fRadiusSum * fRadiusSum))
        return true;
    
    return false;
}

float CMS3DModelASCII::GetCompactOtherSpriteInterval()
{
    return mfColidedDistance;
}

float CMS3DModelASCII::GetRadius()
{
    return mpMS3DASCII->mfRadius;
}
