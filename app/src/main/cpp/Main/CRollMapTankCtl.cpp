 //
//  CRollMapTankCtl.cpp
//  SongGL
//
//  Created by 송 호학 on 11. 12. 11..
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include "CRollMapTankCtl.h"
#include "CRollMapCtl.h"
#include "CMS3DASCII.h"
#include "CRollTankCtl.h"
#include "CMainMenuWorld.h"
#include "sGLTrasform.h"
#include "CUserInfo.h"

CRollMapTankCtl::CRollMapTankCtl(CControl* pParent,
                                 CUserInfo* pUserInfo,
                                 CTextureMan *pTextureMan,
                                 float fFar,          //화면에 꽉차게 = 0.0f 화면에서 멀리하면 컨트롤이 작아진다. 700.0f
                                 float fTourchSesitivity, //fFar = 0.0f이면 0.1f로 하면 되고 700.0f으로 하면 0.4정도가 적당한다.
                                 int nRollFaceCount) : CControl(pParent,pTextureMan)
{
    mType = CRollMapTankCtl_Map;
    mMapCtl = new CRollMapCtl(NULL, pTextureMan,pUserInfo,true,fFar,fTourchSesitivity,nRollFaceCount);
    mTankCtl = new CRollTankCtl(NULL, pTextureMan,false,fFar,fTourchSesitivity,nRollFaceCount);
    mpModelBottom = new CMS3DASCII(pTextureMan);
    mpUserInfo = pUserInfo;
}


CRollMapTankCtl::~CRollMapTankCtl()
{

    if(mpModelBottom)
    {
        delete mpModelBottom;
        mpModelBottom = NULL;
    }
    
    if(mMapCtl)
    {
        delete mMapCtl;
        mMapCtl = NULL;
    }
    
    if(mTankCtl)
    {
        delete mTankCtl;
        mTankCtl = NULL;
    }
}


int CRollMapTankCtl::Initialize(int nControlID)
{
    int nResult = 0;
    
    nResult = mMapCtl->LoadResource();
    if(nResult != 0) return nResult;
    
    nResult = mMapCtl->Initialize(BTN_MENU2_MAP_ROLL, NULL, NULL);
    if(nResult != 0) return nResult;
    
    nResult = mTankCtl->LoadResource();
    if(nResult != 0) return nResult;
    
    nResult = mTankCtl->Initialize(BTN_MENU2_TANK_ROLL, NULL, NULL);
    if(nResult != 0) return nResult;
    
    if(mpModelBottom->Load(MODEL_ROOL_BOTTOM,true) < 0)
    {
        HLogE("[Error] CRollCtl_Bottom can't load\n");
        return -1;
    }
    
    //-------------------------------------------------------------
    //바닥
    float matrix[16];
    int nSize,nIndex;
    sglMesh *arrMesh = NULL;
    
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
    
    mnID = nControlID;
    return E_SUCCESS;
}

void CRollMapTankCtl::ResetMapUpgradeRank()
{
    mMapCtl->ResetMapUpgradeRank();
}

void CRollMapTankCtl::SetCurrentPos(int nPosMap,int nPosTank)
{
    mMapCtl->SetCurrentPos(nPosMap);
    mTankCtl->SetCurrentPos(nPosTank);
}

int  CRollMapTankCtl::GetCurrentPos(CRollMapTankCtl_Type v)
{
    int nR = -1;
    if(v == CRollMapTankCtl_Map)
        nR = mMapCtl->GetCurrentPos();
    else if(v == CRollMapTankCtl_Tank)
        nR = mTankCtl->GetCurrentPos();
    return nR;
}

RollItem* CRollMapTankCtl::GetItem(int nPos,CRollMapTankCtl_Type v)
{
    if(v == CRollMapTankCtl_Map)
        return mMapCtl->GetItem(nPos);
    else if(v == CRollMapTankCtl_Tank)
        return mTankCtl->GetItem(nPos);
    return NULL;
}

void CRollMapTankCtl::SetAni(CONTROLANI nAniType,float fAccel)
{
    if(nAniType == CONTROLANI_ZOOM_IN)
        mMapCtl->SetAniZoomIn(fAccel);
    else
        mMapCtl->SetAniZoomOut(fAccel);
    
    
    if(mType == CRollMapTankCtl_Map)
        mMapCtl->SetHide(false);
    else 
    {
        mMapCtl->SetHide(false);
        mTankCtl->SetHide(false);
    }
}

CONTROLANI CRollMapTankCtl::Animating() 
{ 
    int nType = mMapCtl->Animating();
    if(nType == 1)
        return CONTROLANI_ZOOM_IN;
    else if(nType == 2 || nType == 3)
        return CONTROLANI_ZOOM_OUT;
    else
        return CONTROLANI_NONE;
        
}

bool CRollMapTankCtl::GetHide()
{
    return mMapCtl->GetHide();
}

int CRollMapTankCtl::RenderBegin(int nTime)
{
    mMapCtl->RenderBegin(nTime);
    
    if(mType == CRollMapTankCtl_Tank)
    {
        mTankCtl->RenderBegin(nTime);
    }
    return E_SUCCESS;
}

int CRollMapTankCtl::Render()
{
    if(mbHide) return E_SUCCESS;
    if(mMapCtl->GetHide()) return E_SUCCESS;
    
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
    
    DrawFloor(true);
    
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
    glCullFace(GL_FRONT); //면이 뒤집혔기 때문에... 앞면추려내기를 한다.
    
    glPushMatrix();
    glTranslatef(0, -0.5, 0);
    glScalef(1, -1, 1); //면이 뒤집히고 위아래가 변경된다.
    glColor4f(1.0, 1.0, 1.0, 1.0);
    mMapCtl->DrawRoll(true);
    if(mType == CRollMapTankCtl_Tank)
    {
        mTankCtl->DrawRoll(true);
    }
    glPopMatrix();
    
    
    glCullFace(GL_BACK); //다시 뒷면 추려내기로 변경해준다.
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0); 
    //-------------------------------------------------
    //7>스텐실을 종료한다.
    glDisable(GL_STENCIL_TEST);
    //-------------------------------------------------
    
    
    
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
    mMapCtl->DrawRoll(false);
    glPopMatrix();
    
    if(mType == CRollMapTankCtl_Tank)
    {
        glPushMatrix();
        mTankCtl->DrawRoll(false);
        glPopMatrix();      
    }

    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);  
    //-------------------------------------------------

    
    //mMapCtl->Render();
    return E_SUCCESS;
}

int CRollMapTankCtl::RenderEnd()
{
    mMapCtl->RenderEnd();
    return E_SUCCESS;
}


bool CRollMapTankCtl::BeginTouch(long lTouchID,float x, float y)
{
    if(mType == CRollMapTankCtl_Map)
        return mMapCtl->BeginTouch(lTouchID, x, y);
    else
        return mTankCtl->BeginTouch(lTouchID, x, y);
    return true;
}

bool CRollMapTankCtl::MoveTouch(long lTouchID,float x, float y)
{
    if(mType == CRollMapTankCtl_Map)
        return mMapCtl->MoveTouch(lTouchID, x, y);
    else
        return mTankCtl->MoveTouch(lTouchID, x, y);
    return true;
    
}

bool CRollMapTankCtl::EndTouch(long lTouchID,float x, float y)
{
    if(mType == CRollMapTankCtl_Map)
        return mMapCtl->EndTouch(lTouchID, x, y);
    else
        return mTankCtl->EndTouch(lTouchID, x, y);
    
    return true;
    
}


void CRollMapTankCtl::DrawFloor(bool bReflactor)
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

int  CRollMapTankCtl::GetNowMapIndex()
{
    return mMapCtl->GetNowMapIndex();
}

#ifdef ANDROID
void CRollMapTankCtl::ResetTexture()
{
    CControl::ResetTexture();
    
    mpModelBottom->ResetTexture();
    mMapCtl->ResetTexture();
    mTankCtl->ResetTexture();
    
}

#endif