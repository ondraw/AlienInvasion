//
//  CRollCtl.cpp
//  SongGL
//
//  Created by Song Hohak on 11. 10. 15..
//  Copyright 2011 thinkm. All rights reserved.
//

#include <iostream>
#include "CRollCtl.h"
#include "CMS3DASCII.h"
#include "CTextureMan.h"
#include "IAction.h"
#include "sGLUtils.h"
#include "sGLTrasform.h"
#include "sGL.h"

#if defined ANDROID
#elif defined IPHONE
#else //MAC
#include <GLUT/GLUT.h>
#endif


CRollCtl::CRollCtl(CControl* pParent,CTextureMan *pTextureMan,bool bRevolvingType,float fFar,float fTourchSesitivity,int nRollFaceCount) : ARollCtl(pParent,pTextureMan,bRevolvingType,fFar,fTourchSesitivity,nRollFaceCount)
{
    mpModel = new CMS3DASCII(pTextureMan);
    mpModelBottom = new CMS3DASCII(pTextureMan);
}


CRollCtl::~CRollCtl()
{
   
}


int CRollCtl::LoadResource()
{
    if(mpModel->Load(MODEL_ROLL,true) < 0)
    {
        HLogE("[Error] CRollCtl can't load\n");
        return -1;
    }
    
    if(mpModelBottom->Load(MODEL_ROOL_BOTTOM,true) < 0)
    {
        HLogE("[Error] CRollCtl_Bottom can't load\n");
        return -1;
    }
    
    //Sample test 데이터를 만들자.
    char sName[10];
    for (int i = 0; i < 3; i++) 
    {
        sprintf(sName,"%d", i);
        RollItem *pItem = new RollItem;
        pItem->nID = 0;
        pItem->pRender = 0;
        pItem->pRollProperty = 0;
        pItem->nTextureID = mpTextureMan->GetTextureID(sName);
#ifdef ANDROID
        pItem->sFileName = sName;
#endif
        mlstData.push_back(pItem);
    }    
    
    return ARollCtl::LoadResource();
}


int CRollCtl::CloseResource()
{
    //리소스를 지운다.
    if(mpModel)
    {
        delete mpModel;
        mpModel = NULL;
    }
    if(mpModelBottom)
    {
        delete mpModelBottom;
        mpModelBottom = NULL;
    }
    return ARollCtl::CloseResource();
}

int CRollCtl::Initialize(int nControlID,SPoint *pPosition,SVector *pvDirection)
{
    mnID = nControlID;
    return Initialize(pPosition,pvDirection);
}

//Start ARender ------------------------------------------------

int CRollCtl::Initialize(SPoint *pPosition,SVector *pvDirection)
{
    float matrix[16];
    int nIndex = 0;
    int nSize = mpModel->GetMeshSize();
    sglMesh *arrMesh = mpModel->GetMesh();
    
    sglLoadIdentityf(matrix);
    
    sglScaleMatrixf(matrix, 1.0f, 1.0f, 1.0f); //x,y(높이),z
    
    float fMaxY = 0.0f;
    float fMinY = 0.0f;
    
    float fMaxX = 0.0f;
    float fMinX = 0.0f;
    
    for(int j = 0; j < nSize; j++)
    {
        sglMesh* pMesh = &arrMesh[j];
        for(int i = 0; i < pMesh->nVetexCnt; i++)
        {
            nIndex = i*3;
            
            sglMultMatrixVectorf(&pMesh->pVetexBuffer[nIndex], 
                                                 matrix,
                                                 &pMesh->pVetexBuffer[nIndex]);
            
            
            if(pMesh->pVetexBuffer[nIndex] < fMinX) fMinX = pMesh->pVetexBuffer[nIndex];
            if(pMesh->pVetexBuffer[nIndex] > fMaxX) fMaxX = pMesh->pVetexBuffer[nIndex];
            
            
            if(pMesh->pVetexBuffer[nIndex + 1] < fMinY) fMinY = pMesh->pVetexBuffer[nIndex + 1];
            if(pMesh->pVetexBuffer[nIndex + 1] > fMaxY) fMaxY = pMesh->pVetexBuffer[nIndex + 1];
            
        }
    } 
    
    mViewHeight = (fMaxY - fMinY) / 2.0f; //화면의 정중앙에 오게 한다.
    mViewZ      = (fMaxX - fMinX) / 2.0f; //150.0f는 적당하게 확인해본값.
    
    
    
    //-------------------------------------------------------------
    //바닥
    sglLoadIdentityf(matrix);
    sglTranslateMatrixf(matrix, 0.0f, -1.2, 0.0f); //바닥을 아래로 내린다.
    sglScaleMatrixf(matrix, 1.6, 0.1f, 1.6f); 
    //sglScaleMatrixf(matrix, 1.0f, 1.0f, 1.0f); //x,y(높이),z
    
    nSize = mpModelBottom->GetMeshSize();
    arrMesh = mpModelBottom->GetMesh();
    for(int j = 0; j < nSize; j++)
    {
        sglMesh* pMesh = &arrMesh[j];
        for(int i = 0; i < pMesh->nVetexCnt; i++)
        {
            nIndex = i*3;
            
            sglMultMatrixVectorf(&pMesh->pVetexBuffer[nIndex], 
                                 matrix,
                                 &pMesh->pVetexBuffer[nIndex]);            
        }
    } 
    //-------------------------------------------------------------
    return E_SUCCESS;
}

int CRollCtl::RenderBegin(int nTime)
{
    if(mbHide) return E_SUCCESS;
    
    ARollCtl::RenderBegin(nTime);
    
    //More ....
    
    return E_SUCCESS;
}

void CRollCtl::DrawFloor(bool bReflactor)
{
    sglMesh* pMesh = 0;
    sglMesh *arrMesh = mpModelBottom->GetMesh();
    
       
    GLfloat ambient[4] = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat diffuse[4] = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat specular[4] = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat emission[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    GLfloat shininess = 6; //0(가장 반짝이게)~128(반짝이지 않게)값이된다.
    
    if(bReflactor == false)
        glEnable(GL_TEXTURE_2D);
    
    glEnableClientState(GL_VERTEX_ARRAY); 
    glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    
    
    int nSize = mpModelBottom->GetMeshSize();
    
    for(int j = nSize - 1; j >= 0; j--)
    {
        pMesh = &arrMesh[j];
        
        if(bReflactor == false)
        {
            //OpenGL Es 의 플래그는 GL_FRONT_AND_BACK이여야 반응을 한다. (GL_FRONT)는 안된다.
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
            glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission);
            glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
        }
        
        glBindTexture(GL_TEXTURE_2D, pMesh->nglTexutre);
        glVertexPointer(3, GL_FLOAT, 0, pMesh->pVetexBuffer);
        glNormalPointer(GL_FLOAT, 0, pMesh->pNormalBuffer);
        glTexCoordPointer(2, GL_FLOAT, 0, pMesh->pCoordBuffer);
        glDrawElements(GL_TRIANGLES, pMesh->shIndicesCnt * 3, GL_UNSIGNED_SHORT, pMesh->pIndicesBuffer);
        
        if(bReflactor) break; //1메쉬만 리플렉터이다.
    }
    
    
    if(bReflactor == false)
        glDisable(GL_TEXTURE_2D);

    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY); 
    
    
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);

    
}

int CRollCtl::Render()
{
    
    if(mbHide) return E_SUCCESS;
    
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    
    //1>어떠한 색깔도 그리지 말게 하자.
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDepthMask(GL_FALSE);
    //2>스텐실을 활성화한다
    glEnable(GL_STENCIL_TEST);
    //3>실패나 zbuffer실패일경우는 그대로 두고 성공하였을 경우에만 1로 변경한다.
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);     
    //4>모든것을 1로 설정되어있기때문에 인제 부터 그리는 부분만은 버퍼에 1로 채워진다.
    glStencilFunc(GL_ALWAYS, 1, 0xffffffff); 
    
    //-------------------------------------------------
    //스텐실 버퍼에 마루를 그린다.
    //-------------------------------------------------
    DrawFloor(true);
    //-------------------------------------------------
    
    //4>지금부터 그리는 것은 Color Buffer에 그린다.
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); 
    glDepthMask(GL_TRUE);
    
    //5>
    glStencilFunc(GL_EQUAL, 1, 0xffffffff);  
    //6>이미 저장되어 있는 값은 변경하지 말자...
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    
    
    //-------------------------------------------------
    //Reflrection을 한다.
    //-------------------------------------------------
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0); 
    glDisable(GL_DEPTH_TEST); //disable depth testing of the
    glPushMatrix();
    glRotatef(mfCurrentAngle, 0.0f, 1.0f, 0.0f);
    glTranslatef(0, -0.5, 0);
    glScalef(1, -1, 1); //면이 뒤집히고 위아래가 변경된다.
    glCullFace(GL_FRONT); //면이 뒤집혔기 때문에... 앞면추려내기를 한다.
    glColor4f(1.0, 1.0, 1.0, 1.0);
    DrawRoll(true);
    glCullFace(GL_BACK); //다시 뒷면 추려내기로 변경해준다.
    glPopMatrix();
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0); 
    //-------------------------------------------------
    
    //7>스텐실을 종료한다.
    glDisable(GL_STENCIL_TEST);

    
    //-------------------------------------------------
    // 바닥을 그려준다.
    //-------------------------------------------------
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0f, 1.0f, 1.0f, 0.4f);    //0.4 투명값으로 마루를 그린다.
    DrawFloor(false);
    glDisable(GL_BLEND);
    //-------------------------------------------------
    
    
    //-------------------------------------------------
    // 롤을 그려준다.
    //-------------------------------------------------
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);   
    glPushMatrix();
    glRotatef(mfCurrentAngle, 0.0f, 1.0f, 0.0f);
    mPicking.ResetPickingViewPort(); //회전한 값에 대한 모델값을 가져와야한다.
    DrawRoll(false);
    glPopMatrix();
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);  
    //-------------------------------------------------
    
    
    
    //    //그림자를 그린다.
    //    GLfloat fShadowMatrix[4][4];
    //    GLfloat fFloorPlan[4];
    //    GLfloat fLightPos[4];
    //    fLightPos[0] = 0.0f;
    //    fLightPos[1] = mViewZ + mfFar;
    //    fLightPos[2] = -(mViewZ + mfFar);
    //    fLightPos[3] = 0.0f;//방향만 가지고 있다.
    //    
    //    
    //    sglFindPlane(fFloorPlan, &m_gVertex[0], &m_gVertex[1*3], &m_gVertex[2*3]);
    //    sglShadowMatrix(fShadowMatrix, fFloorPlan, fLightPos);
    //    
    //    
    //    
    //    glDisable(GL_LIGHTING);
    //    glEnable(GL_BLEND);
    //    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //    glColor4f(0.0, 0.0, 0.0, 0.5);//Render 50% black shadow color on top of whatever the floor appareance is
    //    glCullFace(GL_FRONT);
    //    glPushMatrix();
    //    /* Project the shadow. */
    //    glMultMatrixf((GLfloat *) fShadowMatrix);
    //    DrawRoll(false);
    //    glPopMatrix();
    //    glCullFace(GL_BACK);
    //    glDisable(GL_BLEND);
    
    
    
//    glDepthMask(GL_TRUE);
//    glEnable(GL_CULL_FACE);
//    glDisable(GL_BLEND);
//    
//    glDisable(GL_TEXTURE_2D);
//    glDisableClientState(GL_VERTEX_ARRAY);
//    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    return E_SUCCESS;
}


int CRollCtl::DrawRoll(bool bshadow)
{
    sglMesh* pMesh = 0;
    sglMesh *arrMesh = mpModel->GetMesh();
    
    //36 = 360 / 10(10개의 원통형)
    int nPosIndex = GetCurrentPos();
    int nS,nE;
    int nIndex = 0;
    
    GLfloat ambient[4] = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat diffuse[4] = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat specular[4] = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat emission[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    GLfloat shininess = 6; //0(가장 반짝이게)~128(반짝이지 않게)값이된다.
    
    
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_VERTEX_ARRAY); 
    glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    
    if(bshadow == false)
    {
        nS = nPosIndex - 2;
        nE = nPosIndex + 2; //다음꺼는 보여야 한다.
    }
    else
    {
        nS = nPosIndex - 4;
        nE = nPosIndex + 5; //다음꺼는 보여야 한다.
    }
    int nDataMax = mlstData.size();
    
    for(int j = nS; j <= nE; j++)
    {
        nIndex = j;
        
        if( j < 0)                      nIndex= mFaceCount + j;
        else if(j >= mFaceCount)        nIndex = j - mFaceCount;
        
        pMesh = &arrMesh[nIndex];
        
        
        //OpenGL Es 의 플래그는 GL_FRONT_AND_BACK이여야 반응을 한다. (GL_FRONT)는 안된다.
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
        
        if( nIndex >= nDataMax)
            glBindTexture(GL_TEXTURE_2D, 1); //Info다른 데이터를 넣어주어야 한다.
        else
            glBindTexture(GL_TEXTURE_2D, mlstData[nIndex]->nTextureID);
        
        glVertexPointer(3, GL_FLOAT, 0, pMesh->pVetexBuffer);
        glNormalPointer(GL_FLOAT, 0, pMesh->pNormalBuffer);
        glTexCoordPointer(2, GL_FLOAT, 0, pMesh->pCoordBuffer);
        glDrawElements(GL_TRIANGLES, pMesh->shIndicesCnt * 3, GL_UNSIGNED_SHORT, pMesh->pIndicesBuffer);
    }
    
    //    glPopMatrix();
    
    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY); 
    
    
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
    
    return E_SUCCESS;
}

int CRollCtl::RenderEnd()
{
    if(mbHide) return E_SUCCESS;
    
    return E_SUCCESS;
}


int CRollCtl::OnEvent(SGLEvent *pEvent)
{
    if(mbHide) return E_SUCCESS;
    
    return E_SUCCESS;
}

bool CRollCtl::IsTabIn(int nIndex,float xWinPos,float yWinPos)
{  
    
    GLint nwin[2];
    sglMesh *arrMesh = mpModel->GetMesh();
    sglMesh* pMesh = &arrMesh[nIndex];    
    
    nwin[0] = xWinPos;
    nwin[1] = yWinPos;
    
    SPoint nearPoint;
    SPoint farPoint;
    mPicking.GetUnProject(nwin, nearPoint, 0);
    mPicking.GetUnProject(nwin, farPoint, 1);
    
    
    //정점. 0,1,2
    bool b1 = mPicking.sglIsPicking(&pMesh->pVetexBuffer[0], &pMesh->pVetexBuffer[3], &pMesh->pVetexBuffer[6],nearPoint,farPoint);
    //정점. 1,3,2
    bool b2 = mPicking.sglIsPicking(&pMesh->pVetexBuffer[3], &pMesh->pVetexBuffer[9], &pMesh->pVetexBuffer[6],nearPoint,farPoint);
    if(b1 || b2)  
    {
        
        return true;  
    }
    return false;
}
//End ARender ------------------------------------------------




