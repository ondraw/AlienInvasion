//
//  CRollTank.cpp
//  SongGL
//
//  Created by 송 호학 on 11. 12. 11..
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//
#include <math.h>
#include <list>
#include "CRollTankCtl.h"
#include "CRoll3dModel.h"
#include "CModelMan.h"
#include "sGLText.h"
#include "CProperty.h"


CRollTankCtl::CRollTankCtl(CControl* pParent,CTextureMan *pTextureMan,bool bRevolvingType,float fFar,float fTourchSesitivity,int nRollFaceCount) : ARollCtl(pParent,pTextureMan,bRevolvingType,fFar,fTourchSesitivity,nRollFaceCount)
{
    mpModelMan = new CModelMan(pTextureMan);
    mnBeforeSelectIndex = -999;
}


CRollTankCtl::~CRollTankCtl()
{
    CloseResource(); //먼저 롤값을 지우자. (상위 클래스에서 클로즈를 해버리면 롤의 정보가 mpModelMan에 있는 값을 읽어오기 때문에 롤객체들을 먼저 해제한후에 man을 제거하자.)
    if(mpModelMan)
    {
        delete mpModelMan;
        mpModelMan = 0;
    }
}

int CRollTankCtl::LoadResource()
{
    
    float fx,fy = -0.4f,fz;
    
    mfPos[0][0] = 0.f;
    mfPos[0][1] = fy;
    mfPos[0][2] = 7.5f;
    
    float fDivAngle = 360 / ROLL_FACE_COUNT;
    float fRadianAngle = - fDivAngle * PI / 180.0f;  
    
    
    for (int i = 1; i < ROLL_FACE_COUNT; i++) 
    {
        float fcos = cosf(fRadianAngle * (float)i);
        float fsin = sinf(fRadianAngle * (float)i);
        
        fx = mfPos[0][0] * fcos - mfPos[0][2] * fsin;
        fz = mfPos[0][0] * fsin + mfPos[0][2] * fcos;
        
        mfPos[i][0] = fx;
        mfPos[i][1] = fy;
        mfPos[i][2] = fz;
    }
    
    
    
    //nid:ID
    //sv1:Name
    //sv2:Desc
    //nv1:Min Level
    SVector direction;
    SPoint position;
    
    vector<PROPERTY_TANK*>list;
    PROPERTY_TANK::GetPropertyTankList(list);
    int nCnt = list.size();
    
    for (int i = 0,j = 0; i < nCnt; i++) 
    {
        PROPERTY_TANK* prop = list[i];
        
//        if(prop->nEnemy == 1 || prop->nType != 1) continue; //적형태이거나 nType건물 형태이면
        
        if(!(prop->nEnemy == 0 && (prop->nType == 0x01 || prop->nType == 0x02))) continue;
        
        
        //건설형태의 탱크는 메뉴에서 보이지 않게 하자. AlienInvastion에서는 보였지만 좀 이상해보인다.
        if(prop->nID >= 500 && prop->nID <= 503) continue;
        ARender *pModel = new CRoll3dModel(mpModelMan,prop);
        position.x = mfPos[j][0];
        position.y = mfPos[j][1];
        position.z = mfPos[j][2];
        direction.x = 0;
        direction.y = 0;
        direction.z = 1;
        pModel->Initialize(&position, &direction);
        
        RollItem *pItem = new RollItem;
        pItem->nID = prop->nID;
        pItem->pRender = pModel;
        pItem->nTextureID = 0;
        pItem->pRollProperty = prop;
#ifdef ANDROID
        pItem->sFileName = "";
#endif
        mlstData.push_back(pItem);
        j++;
    }
    
    //~CRollTankCtl에서 제거된다.
    //PROPERTY_TANK::ReleaseList((vector<PROPERTYI*>*)&list);
    return ARollCtl::LoadResource();
}


int CRollTankCtl::Initialize(int nControlID,SPoint *pPosition,SVector *pvDirection)
{
    mnID = nControlID;
    return Initialize(pPosition,pvDirection);
}


int CRollTankCtl::Initialize(SPoint *pPosition,SVector *pvDirection)
{
    
//    mViewHeight = (fMaxY - fMinY) / 2.0f; //화면의 정중앙에 오게 한다.
//    mViewZ      = (fMaxX - fMinX) / 2.0f; //150.0f는 적당하게 확인해본값.
    mViewHeight = 2;
    mViewZ = 6;
    
    OnStopMotion(); //첫번째 아이템을 자동선택한다.
    return E_SUCCESS;
}

void CRollTankCtl::SetLight()
{
    
    const GLfloat			lightAmbient[] = {0.8, 0.8, 0.8, 1.0};
	const GLfloat			lightDiffuse[] = {0.5, 0.5, 0.5, 1.0};
    GLfloat                 lightPosition[4]; 
    lightPosition[0] = 0.0f;
    //45도 각도로 햇빛을 비추자....
    lightPosition[1] = mViewZ + mfFar+500;
    lightPosition[2] = mViewZ + mfFar+500; 
    //0.0 특정 방향만 갖는 direction light , 1.0 방향 및 위치를 가지고 있는 태양광 (position light)
    lightPosition[3] = 0.0f;
    
    glEnable(GL_LIGHTING);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glEnable(GL_LIGHT0);
}

int CRollTankCtl::RenderBegin(int nTime)
{
    if(mbHide) return E_SUCCESS;
    
    //회전값을 계산한다.
    RotMotion(nTime);
    
    //36 = 360 / 10(10개의 원통형)
    int nPosIndex = GetCurrentPos();
    int nDataMax = mlstData.size();
    int nS,nE;
    int nIndex = 0;
    
    nS = nPosIndex - 2;
    nE = nPosIndex + 2; //다음꺼는 보여야 한다.
    
    for(int j = nS; j <= nE; j++)
    {
        nIndex = j;
        
        if( j < 0)                      nIndex= mFaceCount + j;
        else if(j >= mFaceCount)        nIndex = j - mFaceCount;
        
        if(nIndex < nDataMax && mlstData[nIndex]->pRender)
        {
            ((CRoll3dModel*)mlstData[nIndex]->pRender)->RenderBegin(nTime,mfCurrentAngle);
        }
    }
    
    return E_SUCCESS;
}

int CRollTankCtl::DrawRoll(bool bshadow)
{
    if(mbHide) return E_SUCCESS;
    
    //밝게 그려보자꾸나...
    SetLight();
    
    //36 = 360 / 10(10개의 원통형)
    int nPosIndex = GetCurrentPos();
    int nDataMax = mlstData.size();
    int nS,nE;
    int nIndex = 0;
    
    nS = nPosIndex - 2;
    nE = nPosIndex + 2; //다음꺼는 보여야 한다.
    
    glPushMatrix();
    glRotatef(mfCurrentAngle, 0.0f, 1.0f, 0.0f);
    if(bshadow == false)
        mPicking.ResetPickingViewPort(); //회전한 값에 대한 모델값을 가져와야한다.
    
    for(int j = nS; j <= nE; j++)
    {
        nIndex = j;
        
        if( j < 0)                      nIndex= mFaceCount + j;
        else if(j >= mFaceCount)        nIndex = j - mFaceCount;
        
        if(nIndex < nDataMax && mlstData[nIndex]->pRender)
            mlstData[nIndex]->pRender->Render();
    }
    
    glPopMatrix();
    
    return E_SUCCESS;
}

int CRollTankCtl::Render()
{
    
    return E_SUCCESS;
}

int CRollTankCtl::RenderEnd()
{
    return E_SUCCESS;
}


void CRollTankCtl::OnStopMotion()
{
    int nPosIndex = GetCurrentPos();
    if(nPosIndex > mlstData.size() - 1) return;
    if(mnBeforeSelectIndex == nPosIndex) return;
    CRoll3dModel* p3DModel = (CRoll3dModel*)mlstData[nPosIndex]->pRender;
    p3DModel->SetSelectedAnimationAngle(3.0f);    
    if(mnBeforeSelectIndex != -999)
    {
        p3DModel = (CRoll3dModel*)mlstData[mnBeforeSelectIndex]->pRender;
        p3DModel->SetSelectedAnimationAngle(0);
    }
    mnBeforeSelectIndex = nPosIndex;
    ARollCtl::OnStopMotion(); //이벤트를 보낸다.
    
}

bool CRollTankCtl::IsTabIn(int nIndex,float xWinPos,float yWinPos)
{  
    int nPosIndex = GetCurrentPos();
    if(nPosIndex > mlstData.size() - 1) return true;
    CRoll3dModel* p3DModel = (CRoll3dModel*)mlstData[nPosIndex]->pRender;
    if(p3DModel->GetModel() == NULL)
    {
     
        HLogE("IsTabIn nIndex %d\n",nIndex);
        return true; //여기로오면 안되는데 일단....
    }
    if(p3DModel->GetModel()->IsPicking(&mPicking,xWinPos,yWinPos)) return true;
    if(p3DModel->GetModelHeader()->IsPicking(&mPicking,xWinPos,yWinPos)) return true;
    return false;
}


#ifdef ANDROID
void CRollTankCtl::ResetTexture()
{
    ARollCtl::ResetTexture();
    mpModelMan->ResetTexture();
}
#endif