//
//  CRollMapCtl.cpp
//  SongGL
//
//  Created by 송 호학 on 11. 12. 13..
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//


#include "CRollMapCtl.h"
#include "CMS3DASCII.h"
#include "CTextureMan.h"
#include "IAction.h"
#include "sGLUtils.h"
#include "sGLTrasform.h"
#include "sGL.h"
#include "CProperty.h"
#include "CUserInfo.h"

#if defined ANDROID
#elif defined IPHONE
#else //MAC
#include <GLUT/GLUT.h>
#endif


CRollMapCtl::CRollMapCtl(CControl* pParent,CTextureMan *pTextureMan,CUserInfo   *pUserInfo,bool bRevolvingType,float fFar,float fTourchSesitivity,int nRollFaceCount) : ARollCtl(pParent,pTextureMan,bRevolvingType,fFar,fTourchSesitivity,nRollFaceCount)
{
    mpModel = new CMS3DASCII(pTextureMan);
//    mIDMapEmpty = 0;
    mMapFrameTexture = 0;
    mMapCompleteCountTexture = 0;
    mpUserInfo= pUserInfo;
    mnNowMapIndex = 0;
}


CRollMapCtl::~CRollMapCtl()
{
    if(mpModel)
    {
        delete mpModel;
        mpModel = NULL;
    }
    
    vector<MAPTITLE*>::iterator b = mLstMapTitle.begin();
    vector<MAPTITLE*>::iterator e = mLstMapTitle.end();
    for (vector<MAPTITLE*>::iterator it = b; it != e; it++)
    {
        delete *it;
    }
}

void CRollMapCtl::ResetMapUpgradeRank()
{
    int nCmpRankMap = mpUserInfo->GetCompletedRank();
    int nSize = mLstMapTitle.size();
    for (int i = 1; i < nSize; i++) //첫번째는 훈련모드 이다. 그것만 빼고 다시 리셋 해준다.
    {
        mLstMapTitle[i]->bCompleted =  false;
    }
    mnNowMapIndex = 0;
    
    if(nCmpRankMap > 1)
    {
        mnNowMapIndex = 1;
        if(nCmpRankMap > 6) nCmpRankMap = 6;
        char sPath[256];
        sprintf(sPath,"CompletedMap%d.png",nCmpRankMap);
        mMapCompleteCountTexture = mpTextureMan->GetTextureID(sPath);
    }
    
    
}

int CRollMapCtl::LoadResource()
{
    if(mpModel->Load(MODEL_ROLL,true) < 0)
    {
        HLogE("[Error] CRollMapCtl can't load\n");
        return -1;
    }
    
    //nid:ID
    //sv1:Name
    //sv2:FilePath
    //sv3:Desc
    //nv1:Type : 아직 정의 되지 않았음.
    vector<PROPERTY_MAP*>list;
    PROPERTY_MAP::GetPropertyMapList(list);
    int nCnt = list.size();
    for (int i = 0; i < nCnt; i++) 
    {
        PROPERTY_MAP* prop = list[i];
        if(prop->nType == 0) continue; //사용유무
        
        RollItem *pItem = new RollItem;
        string sModelImgPath = prop->sModelPath;
        sModelImgPath += string(".png");
        
        pItem->nID = prop->nID;
        
        pItem->pRender = 0;
        
        //PNG는 FALSE로 읽어온다.
        pItem->nTextureID = mpTextureMan->GetTextureID(sModelImgPath.c_str(),0,0);
#ifdef ANDROID
        pItem->sFileName = sModelImgPath;
#endif
        pItem->pRollProperty = prop;
        
        mlstData.push_back(pItem);
    }   
    //mIDMapEmpty = mpTextureMan->GetTextureID(IMG_ROLL_EMPTY);
    mMapFrameTexture = mpTextureMan->GetTextureID(IMG_ROLL_FRAME);
    
    
    bool bFirst = true;
    nCnt = mlstData.size();
    for (int i = 0; i < nCnt; i++)
    {
        PROPERTY_MAP* pItemP = (PROPERTY_MAP*)mlstData[i]->pRollProperty;
        MAPTITLE *pMapTitle = new MAPTITLE;
        
        float fU,fV;
        //            const char* sTitle = SGLTextUnit::GetText(pItemP->sName); 안드로이드에서는 이렇게 하면 에러가 난다. 이유 몰라.
        GLuint nTextID = mpTextureMan->MakeText(pItemP->sName,fU,fV);
        pMapTitle->nTextureID = nTextID;
        pMapTitle->fU = fU;
        pMapTitle->fV = fV;
        
        pMapTitle->bCompleted = mpUserInfo->IsCompletedMap(pItemP->nID);

        if(pMapTitle->bCompleted == false && bFirst)
        {
            mnNowMapIndex = i;
            bFirst = false;
        }
        
        pMapTitle->nCompletedCnt = mpUserInfo->GetCompletedRank();
        mLstMapTitle.push_back(pMapTitle);
    }
    
    mMapCompleteTexture = mpTextureMan->GetTextureID("CompletedMap.png");
    mMapCompleteKeyTexture = mpTextureMan->GetTextureID("CompletedMapKey.png");
    
    
    if(mpUserInfo->GetCompletedRank() > 1)
    {
        char sPath[256];
        sprintf(sPath,"CompletedMap%d.png",mpUserInfo->GetCompletedRank());
        mMapCompleteCountTexture = mpTextureMan->GetTextureID(sPath);
    }
    
    return ARollCtl::LoadResource();
}


int CRollMapCtl::CloseResource()
{
    //리소스를 지운다.
    if(mpModel)
    {
        delete mpModel;
        mpModel = NULL;
    }
    
    return ARollCtl::CloseResource();
}

int CRollMapCtl::Initialize(int nControlID,SPoint *pPosition,SVector *pvDirection)
{
    mnID = nControlID;
    return Initialize(pPosition,pvDirection);
}

int CRollMapCtl::DrawRoll(bool bshadow)
{
    if(mbHide) return E_SUCCESS;
    
    
    
    sglMesh* pMesh = 0;
    sglMesh* pMeshT = 0;
    sglMesh* pMeshP = 0;
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
    
    glPushMatrix();
    glRotatef(mfCurrentAngle, 0.0f, 1.0f, 0.0f);
    if(bshadow == false)
        mPicking.ResetPickingViewPort(); //회전한 값에 대한 모델값을 가져와야한다.
    
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
        
        pMesh  = &arrMesh[nIndex*3];
        pMeshT = &arrMesh[nIndex*3 + 1];
        pMeshP = &arrMesh[nIndex*3 + 2];
        
        //OpenGL Es 의 플래그는 GL_FRONT_AND_BACK이여야 반응을 한다. (GL_FRONT)는 안된다.
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
        
        
        //맵 프레임
        glBindTexture(GL_TEXTURE_2D, mMapFrameTexture); //다른 데이터를 넣어주어야 한다.
        glVertexPointer(3, GL_FLOAT, 0, pMesh->pVetexBuffer);
        glNormalPointer(GL_FLOAT, 0, pMesh->pNormalBuffer);
        glTexCoordPointer(2, GL_FLOAT, 0, pMesh->pCoordBuffer);
        glDrawElements(GL_TRIANGLES, pMesh->shIndicesCnt * 3, GL_UNSIGNED_SHORT, pMesh->pIndicesBuffer);
        
        if( nIndex < nDataMax)
        {
            MAPTITLE* pMapTitle = mLstMapTitle[nIndex];
            
            if(pMapTitle->bCompleted == false || mnNowMapIndex == nIndex) //잠금표시
            {
                glColor4f(1.0f,1.0f,1.0f,1.0f);
                //1>
                glClientActiveTexture(GL_TEXTURE0);
                glActiveTexture(GL_TEXTURE0);
                glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                glBindTexture(GL_TEXTURE_2D, mlstData[nIndex]->nTextureID);
                //GL_REPLACE 하면 안뒤아..
                //즉 나중에 Revert Back을 할경우 glTexEnv환경이 GL_REPLACE로 되어있어서 바인딩할때 GL_REPLACE게 된다. 난 GL_MODULATE로 해야한다.
                //제거하면 현제 설정된 env로 사용된다.
                glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                glTexCoordPointer(2, GL_FLOAT, 0, pMeshP->pCoordBuffer);
                glEnable(GL_TEXTURE_2D);
                
                //2>
                glClientActiveTexture(GL_TEXTURE1);
                glActiveTexture(GL_TEXTURE1);
                glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                if(mnNowMapIndex != nIndex)
                    glBindTexture(GL_TEXTURE_2D, mMapCompleteTexture);
                else
                    glBindTexture(GL_TEXTURE_2D, mMapCompleteKeyTexture);
                
                glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL); //GL_DECAL : 알파값은 그리지 않게 , GL_REPLACE : 완전하게 덧칠 , GL_MODULATE : 블랜딩효과 (겹치는 효과)
                glTexCoordPointer(2, GL_FLOAT, 0, pMeshP->pCoordBuffer);
                glEnable(GL_TEXTURE_2D);
                
#ifndef ANDROID //안드로이드는 2개까지만 지원한다. Android 4.0.1에서 그래서 제거했다.
                if(mpUserInfo->GetCompletedRank() > 1)
                {
                    glClientActiveTexture(GL_TEXTURE2);
                    glActiveTexture(GL_TEXTURE2);
                    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                    glBindTexture(GL_TEXTURE_2D, mMapCompleteCountTexture);
                    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL); //GL_DECAL : 알파값은 그리지 않게 , GL_REPLACE : 완전하게 덧칠 , GL_MODULATE : 블랜딩효과 (겹치는 효과)
                    glTexCoordPointer(2, GL_FLOAT, 0, pMeshP->pCoordBuffer);
                    glEnable(GL_TEXTURE_2D);
                    
                }
#endif
            }
            else if(pMapTitle->bCompleted == true && mpUserInfo->GetCompletedRank() > 1) //계급장포함.
            {
                //1>
                glClientActiveTexture(GL_TEXTURE0);
                glActiveTexture(GL_TEXTURE0);
                glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                glBindTexture(GL_TEXTURE_2D, mlstData[nIndex]->nTextureID);
                //GL_REPLACE 하면 안뒤아..
                //즉 나중에 Revert Back을 할경우 glTexEnv환경이 GL_REPLACE로 되어있어서 바인딩할때 GL_REPLACE게 된다. 난 GL_MODULATE로 해야한다.
                //제거하면 현제 설정된 env로 사용된다.
                glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                glTexCoordPointer(2, GL_FLOAT, 0, pMeshP->pCoordBuffer);
                glEnable(GL_TEXTURE_2D);
                
                //2>
                glClientActiveTexture(GL_TEXTURE1);
                glActiveTexture(GL_TEXTURE1);
                glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                glBindTexture(GL_TEXTURE_2D, mMapCompleteCountTexture);
                glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL); //GL_DECAL : 알파값은 그리지 않게 , GL_REPLACE : 완전하게 덧칠 , GL_MODULATE : 블랜딩효과 (겹치는 효과)
                glTexCoordPointer(2, GL_FLOAT, 0, pMeshP->pCoordBuffer);
                glEnable(GL_TEXTURE_2D);
                
            }
            else// if(pMapTitle->bCompleted == true && mpUserInfo->GetCompletedRank() == 1) //일반
            {
                glBindTexture(GL_TEXTURE_2D, mlstData[nIndex]->nTextureID);
                glTexCoordPointer(2, GL_FLOAT, 0, pMeshP->pCoordBuffer);
            }

            
//            //맵 그림.
//            if(pMapTitle->bCompleted == false && mpUserInfo->GetCompletedRank() == 1)
//            {
//                glBindTexture(GL_TEXTURE_2D, mlstData[nIndex]->nTextureID);
//                glTexCoordPointer(2, GL_FLOAT, 0, pMeshP->pCoordBuffer);
//            }
//            else if(pMapTitle->bCompleted == false && mpUserInfo->GetCompletedRank() > 1)
//            {
//                //1>
//                glClientActiveTexture(GL_TEXTURE0);
//                glActiveTexture(GL_TEXTURE0);
//                glEnableClientState(GL_TEXTURE_COORD_ARRAY);
//                glBindTexture(GL_TEXTURE_2D, mlstData[nIndex]->nTextureID);
//                //GL_REPLACE 하면 안뒤아..
//                //즉 나중에 Revert Back을 할경우 glTexEnv환경이 GL_REPLACE로 되어있어서 바인딩할때 GL_REPLACE게 된다. 난 GL_MODULATE로 해야한다.
//                //제거하면 현제 설정된 env로 사용된다.
//                glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
//                glTexCoordPointer(2, GL_FLOAT, 0, pMeshP->pCoordBuffer);
//                glEnable(GL_TEXTURE_2D);
//                
//                //2>
//                glClientActiveTexture(GL_TEXTURE1);
//                glActiveTexture(GL_TEXTURE1);
//                glEnableClientState(GL_TEXTURE_COORD_ARRAY);
//                glBindTexture(GL_TEXTURE_2D, mMapCompleteCountTexture);
//                glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL); //GL_DECAL : 알파값은 그리지 않게 , GL_REPLACE : 완전하게 덧칠 , GL_MODULATE : 블랜딩효과 (겹치는 효과)
//                glTexCoordPointer(2, GL_FLOAT, 0, pMeshP->pCoordBuffer);
//                glEnable(GL_TEXTURE_2D);
//                
//            }
//            else //MultTexture Mapping
//            {
//                glColor4f(1.0f,1.0f,1.0f,1.0f);
//                //1>
//                glClientActiveTexture(GL_TEXTURE0);
//                glActiveTexture(GL_TEXTURE0);
//                glEnableClientState(GL_TEXTURE_COORD_ARRAY);
//                glBindTexture(GL_TEXTURE_2D, mlstData[nIndex]->nTextureID);
//                //GL_REPLACE 하면 안뒤아..
//                //즉 나중에 Revert Back을 할경우 glTexEnv환경이 GL_REPLACE로 되어있어서 바인딩할때 GL_REPLACE게 된다. 난 GL_MODULATE로 해야한다.
//                //제거하면 현제 설정된 env로 사용된다.
//                glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
//                glTexCoordPointer(2, GL_FLOAT, 0, pMeshP->pCoordBuffer);
//                glEnable(GL_TEXTURE_2D);
//                
//                //2>
//                glClientActiveTexture(GL_TEXTURE1);
//                glActiveTexture(GL_TEXTURE1);
//                glEnableClientState(GL_TEXTURE_COORD_ARRAY);
//                glBindTexture(GL_TEXTURE_2D, mMapCompleteTexture);
//                glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL); //GL_DECAL : 알파값은 그리지 않게 , GL_REPLACE : 완전하게 덧칠 , GL_MODULATE : 블랜딩효과 (겹치는 효과)
//                glTexCoordPointer(2, GL_FLOAT, 0, pMeshP->pCoordBuffer);
//                glEnable(GL_TEXTURE_2D);
//                
//#ifndef ANDROID //안드로이드는 2개까지만 지원한다. Android 4.0.1에서 그래서 제거했다.
//                if(mpUserInfo->GetCompletedRank() > 1)
//                {
//                    glClientActiveTexture(GL_TEXTURE2);
//                    glActiveTexture(GL_TEXTURE2);
//                    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
//                    glBindTexture(GL_TEXTURE_2D, mMapCompleteCountTexture);
//                    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL); //GL_DECAL : 알파값은 그리지 않게 , GL_REPLACE : 완전하게 덧칠 , GL_MODULATE : 블랜딩효과 (겹치는 효과)
//                    glTexCoordPointer(2, GL_FLOAT, 0, pMeshP->pCoordBuffer);
//                    glEnable(GL_TEXTURE_2D);
//                    
//                }
//#endif
//            }
            
            glVertexPointer(3, GL_FLOAT, 0, pMeshP->pVetexBuffer);
            glNormalPointer(GL_FLOAT, 0, pMeshP->pNormalBuffer);
            glDrawElements(GL_TRIANGLES, pMeshP->shIndicesCnt * 3, GL_UNSIGNED_SHORT, pMeshP->pIndicesBuffer);

            if(pMapTitle->bCompleted == true && mpUserInfo->GetCompletedRank() > 1)
            {
                // Revert back
                glActiveTexture(GL_TEXTURE1);
                glDisable(GL_TEXTURE_2D);
                
                
                ////GL_REPLACE 하면 안뒤아.. 즉 나중에 Revert Back을 할경우 glTexEnv환경이 GL_REPLACE로 되어있어서 바인딩할때 GL_REPLACE게 된다. 난 GL_MODULATE로 해야한다.
                glClientActiveTexture(GL_TEXTURE0);
                glActiveTexture(GL_TEXTURE0);
                glEnable(GL_TEXTURE_2D);
                
            }
            else if(pMapTitle->bCompleted == false || mnNowMapIndex == nIndex)
            {
#ifndef ANDROID //안드로이드는 2개까지만 지원한다. Android 4.0.1에서
                // Revert back
                if(mpUserInfo->GetCompletedRank() > 1)
                {
                    glActiveTexture(GL_TEXTURE2);
                    glDisable(GL_TEXTURE_2D);
                }
#endif
                
                // Revert back
                glActiveTexture(GL_TEXTURE1);
                glDisable(GL_TEXTURE_2D);
                
                
                ////GL_REPLACE 하면 안뒤아.. 즉 나중에 Revert Back을 할경우 glTexEnv환경이 GL_REPLACE로 되어있어서 바인딩할때 GL_REPLACE게 된다. 난 GL_MODULATE로 해야한다.
                glClientActiveTexture(GL_TEXTURE0);
                glActiveTexture(GL_TEXTURE0);
                glEnable(GL_TEXTURE_2D);
            }
            
//            if(pMapTitle->bCompleted == false && mpUserInfo->GetCompletedRank() > 1)
//            {
//                // Revert back
//                glActiveTexture(GL_TEXTURE1);
//                glDisable(GL_TEXTURE_2D);
//                
//                
//                ////GL_REPLACE 하면 안뒤아.. 즉 나중에 Revert Back을 할경우 glTexEnv환경이 GL_REPLACE로 되어있어서 바인딩할때 GL_REPLACE게 된다. 난 GL_MODULATE로 해야한다.
//                glClientActiveTexture(GL_TEXTURE0);
//                glActiveTexture(GL_TEXTURE0);
//                glEnable(GL_TEXTURE_2D);
//
//            }
//            else if(pMapTitle->bCompleted)
//            {
//#ifndef ANDROID //안드로이드는 2개까지만 지원한다. Android 4.0.1에서
//                // Revert back
//                if(mpUserInfo->GetCompletedRank() > 1)
//                {
//                    glActiveTexture(GL_TEXTURE2);
//                    glDisable(GL_TEXTURE_2D);
//                }
//#endif
//                
//                // Revert back
//                glActiveTexture(GL_TEXTURE1);
//                glDisable(GL_TEXTURE_2D);
//                
//                
//                ////GL_REPLACE 하면 안뒤아.. 즉 나중에 Revert Back을 할경우 glTexEnv환경이 GL_REPLACE로 되어있어서 바인딩할때 GL_REPLACE게 된다. 난 GL_MODULATE로 해야한다.
//                glClientActiveTexture(GL_TEXTURE0);
//                glActiveTexture(GL_TEXTURE0);
//                glEnable(GL_TEXTURE_2D);
//            }
            
            memcpy(mfWorldTextCoordTex, gPanelCoordTex, sizeof(gPanelCoordTex));
            for (int i = 0; i < 8; )
            {
                
                mfWorldTextCoordTex[i++] *= pMapTitle->fU;
                mfWorldTextCoordTex[i] =  (1.0f - mfWorldTextCoordTex[i]) * pMapTitle->fV;
                i++;
            }
            glBindTexture(GL_TEXTURE_2D, pMapTitle->nTextureID);
            glVertexPointer(3, GL_FLOAT, 0, pMeshT->pVetexBuffer);
            glNormalPointer(GL_FLOAT, 0, pMeshT->pNormalBuffer);
            glTexCoordPointer(2, GL_FLOAT, 0, mfWorldTextCoordTex);
            glDrawElements(GL_TRIANGLES, pMeshT->shIndicesCnt * 3, GL_UNSIGNED_SHORT, pMeshT->pIndicesBuffer);
        }
    }
    
    glPopMatrix();
    
    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY); 
    
    
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);

    return E_SUCCESS;
}

#ifdef ANDROID
void CRollMapCtl::ResetTexture()
{
    ARollCtl::ResetTexture();
    mpModel->ResetTexture();
//    mIDMapEmpty = mpTextureMan->GetTextureID(IMG_ROLL_EMPTY);
    mMapFrameTexture = mpTextureMan->GetTextureID(IMG_ROLL_FRAME);
    
    vector<MAPTITLE*>::iterator b = mLstMapTitle.begin();
    vector<MAPTITLE*>::iterator e = mLstMapTitle.end();
    for (vector<MAPTITLE*>::iterator it = b; it != e; it++)
    {
        delete *it;
    }
    mLstMapTitle.clear();
    
    int nCnt = mlstData.size();
    for (int i = 0; i < nCnt; i++)
    {
        PROPERTY_MAP* pItemP = (PROPERTY_MAP*)mlstData[i]->pRollProperty;
        MAPTITLE *pMapTitle = new MAPTITLE;
        
        float fU,fV;
        //            const char* sTitle = SGLTextUnit::GetText(pItemP->sName); 안드로이드에서는 이렇게 하면 에러가 난다. 이유 몰라.
        GLuint nTextID = mpTextureMan->MakeText(pItemP->sName,fU,fV); //내부적으로 SGLTextUnit::GetText을 찾아서 사용한다.
        pMapTitle->nTextureID = nTextID;
        pMapTitle->fU = fU;
        pMapTitle->fV = fV;
        
        pMapTitle->bCompleted = mpUserInfo->IsCompletedMap(pItemP->nID);
        pMapTitle->nCompletedCnt = mpUserInfo->GetCompletedRank();
        
        mLstMapTitle.push_back(pMapTitle);
    }
    mMapCompleteTexture = mpTextureMan->GetTextureID("CompletedMap.png");
    mMapCompleteKeyTexture = mpTextureMan->GetTextureID("CompletedMapKey.png");
    if(mpUserInfo->GetCompletedRank() > 1)
    {
        char sPath[256];
        sprintf(sPath,"CompletedMap%d.png",mpUserInfo->GetCompletedRank());
        mMapCompleteCountTexture = mpTextureMan->GetTextureID(sPath);
    }
}
#endif

//Start ARender ------------------------------------------------

int CRollMapCtl::Initialize(SPoint *pPosition,SVector *pvDirection)
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
    
    mViewZ += mfFar;
    mfFarOrg = mViewZ; //원래 위치를

    return E_SUCCESS;
}

int CRollMapCtl::RenderBegin(int nTime)
{
    if(mbHide) return E_SUCCESS;
    ARollCtl::RenderBegin(nTime);
    return E_SUCCESS;
}


int CRollMapCtl::Render()
{
    return E_SUCCESS;
}

int CRollMapCtl::RenderEnd()
{
    return E_SUCCESS;
}


int CRollMapCtl::OnEvent(SGLEvent *pEvent)
{
    return E_SUCCESS;
}

bool CRollMapCtl::IsTabIn(int nIndex,float xWinPos,float yWinPos)
{  
    
    GLint nwin[2];
//    sglMesh *arrMesh = mpModel->GetMesh();
//    sglMesh* pMesh = &arrMesh[nIndex * 3];
    
    nwin[0] = xWinPos;
    nwin[1] = yWinPos;
    
    SPoint nearPoint;
    SPoint farPoint;
    mPicking.GetUnProject(nwin, nearPoint, 0);
    mPicking.GetUnProject(nwin, farPoint, 1);
    
    
    //정점. 0,1,2
//    bool b1 = mPicking.sglIsPicking(&pMesh->pVetexBuffer[0], &pMesh->pVetexBuffer[3], &pMesh->pVetexBuffer[6],nearPoint,farPoint);
//    //정점. 1,3,2
//    bool b2 = mPicking.sglIsPicking(&pMesh->pVetexBuffer[3], &pMesh->pVetexBuffer[9], &pMesh->pVetexBuffer[6],nearPoint,farPoint);
//    if(b1 || b2)  
//    {
//        return true;  
//    }
    
    if(mpModel->IsPicking(&mPicking, nearPoint, farPoint))
        return true;
    return false;
}
//End ARender ------------------------------------------------




