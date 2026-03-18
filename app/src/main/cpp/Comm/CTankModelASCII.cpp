//
//  CTankModelASCII.cpp
//  SongGL
//
//  Created by itsme on 13. 9. 13..
//
//
#include "sGL.h"
#include "sGLTrasform.h"
#include "CTankModelASCII.h"

CTankModelASCII::CTankModelASCII(CMS3DASCII *pMS3DASCII) : CMS3DModelASCII(pMS3DASCII)
{
    
    mfPoUpAngle = 0.f;
    mptStartMissile = NULL;
    
    arrHide = new bool[pMS3DASCII->GetMeshSize()];
    memset(arrHide,0,pMS3DASCII->GetMeshSize() * sizeof(bool));
}

CTankModelASCII::~CTankModelASCII()
{
}


GLvoid	CTankModelASCII::updatef(GLuint time)
{
    int nIndex = 0;
    float matrix[16];
    float matrix2[16];
    GLfloat	rotationMatrix[16];
    GLfloat	quaternion0[4];
    
    int nSize = GetMeshSize();
    sglMesh *arrMesh = GetMesh();
    sglMesh *arrWroldMesh = GetWorldMesh();
    
    
    //---------------------------------해더몸통
    sglLoadIdentityf(matrix);
    sglTranslateMatrixf(matrix,position[0],position[1],position[2]); //회전전에 이동을 해야 한다.
    sglScaleMatrixf(matrix, scale[0], scale[1], scale[2]);
    sglAngleToQuaternionf(quaternion0, orientation[0], orientation[1], orientation[2]);
    sglQuaternionToMatrixf(rotationMatrix, quaternion0);
    sglMultMatrixf(matrix, matrix, rotationMatrix);
    
    if(pfinalmulMaxtix)
        sglMultMatrixf(matrix, matrix, pfinalmulMaxtix);
    //---------------------------------해더몸통
    
    if(mfPoUpAngle > MAXPOUPANGLE)        mfPoUpAngle = MAXPOUPANGLE;
    
    sglLoadIdentityf(matrix2);
    sglTranslateMatrixf(matrix2,position[0],position[1],position[2]); //회전전에 이동을 해야 한다.
    sglScaleMatrixf(matrix2, scale[0], scale[1], scale[2]);
    sglAngleToQuaternionf(quaternion0,
                          orientation[0],
                          orientation[1],
                          orientation[2]);
    sglQuaternionToMatrixf(rotationMatrix, quaternion0);
    sglMultMatrixf(matrix2, matrix2, rotationMatrix);
    if(pfinalmulMaxtix)
        sglMultMatrixf(matrix2, matrix2, pfinalmulMaxtix);
    
    
//    float *vtDown = &arrMesh[1].pVetexBuffer[169 * 3];
    if(mfPoUpAngle != 0.f) //CET8은 포신이 움직이지 않음.
    {
//        float* vtDown = &arrMesh[mptStartMissile[1][0]].pVetexBuffer[mptStartMissile[1][1] * 3];
        float *vtDown = &arrMesh[mptStartMissile[0][0]].pVetexBuffer[mptStartMissile[0][2] * 3];
        
        //z=0.f인이유는
        sglTranslateMatrixf(matrix2,vtDown[0],vtDown[1],0.f); //포가 원래 자리로 이동한다.
        sglRotateRzRyRxMatrixf(matrix2,
                               0.f,
                               0.f,
                               mfPoUpAngle);
        sglTranslateMatrixf(matrix2,-vtDown[0],-vtDown[1],0.f); //포밑이 원점이 되게 이동한다.
    }
    
    //---------------------------------해더포
    for(int j = 0; j < nSize; j++)
    {
        sglMesh* pMesh = &arrMesh[j];
        sglMesh* pWorldMesh = &arrWroldMesh[j];
        
        if(arrHide[j] == true) continue;
        for(int i = 0; i < pMesh->nVetexCnt; i++)
        {
            nIndex = i*3;
            if(j != 1)
                sglMultMatrixVectorf(&pWorldMesh->pVetexBuffer[nIndex],
                                     matrix,
                                     &pMesh->pVetexBuffer[nIndex]);
            else //포는 인덱스가 1이다.
                sglMultMatrixVectorf(&pWorldMesh->pVetexBuffer[nIndex],
                                     matrix2,
                                     &pMesh->pVetexBuffer[nIndex]);
        }
    }
}



GLvoid		CTankModelASCII::renderf() const
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
        if(arrHide[j] == true) continue;
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