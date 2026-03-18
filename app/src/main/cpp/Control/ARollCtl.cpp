//
//  CRollCtl.cpp
//  SongGL
//
//  Created by Song Hohak on 11. 10. 15..
//  Copyright 2011 thinkm. All rights reserved.
//

#include <iostream>
#include "ARollCtl.h"
#include "CMS3DASCII.h"
#include "CTextureMan.h"
#include "IAction.h"
#include "sGLUtils.h"
#include "sGLTrasform.h"
#include "sGL.h"
#include "CScenario.h"

#if defined ANDROID
#elif defined IPHONE
#else //MAC
#include <GLUT/GLUT.h>
#endif

namespace
{
    const float kRollTouchBaseWidth = 480.0f;
    const float kRollMinInertiaDeltaMs = 16.0f;
    const float kRollMaxStepPerFaceRate = 0.35f;
    const float kRollMaxAccelPerFaceRate = 0.08f;
    const float kRollSnapCommitRate = 0.2f;
}


ARollCtl::ARollCtl(CControl* pParent,CTextureMan *pTextureMan,bool bRevolvingType,float fFar,float fTourchSesitivity,int nRollFaceCount) : CControl(pParent,pTextureMan)
{
    
    mfCurrentAngle = 0;
    
    mFaceCount = nRollFaceCount;
    mnBeforeIndex = 0;
    mnBeforeAngle = -9999;
    
    mlTouchID = -1;
    mnTouchedIndex = -1;
    mnTouchStartPos = 0;
    mnRotDirection = 0;
    mAccelator = 0.0f;
    
    
    //작은 컨트롤
    //mTourceSensitivity = 0.4;
    //ptCamera.z = mViewZ + 700; 이렇게 카메라를 멀리하면 컨트롤이 작아진다.
    
    //큰컨트롤
    mfTourceSensitivity = fTourchSesitivity;
    mfFar = fFar;
    mfFarOrg = -9999;
    mViewZ = 16.0;
    mbRevolvingType = bRevolvingType;
    mAniType = CONTROLANI_NONE;
    mfSelectedAngle = -999;
}


ARollCtl::~ARollCtl()
{
    CloseResource();
}


void ARollCtl::SetAniZoomIn(float fAccel)
{
    mAniType = CONTROLANI_ZOOM_IN; //ZoomIn
    mfAniAccel = fAccel;
    mViewZ = mViewZ * 2; 
}

void ARollCtl::SetAniZoomOut(float fAccel)
{
    mAniType = CONTROLANI_ZOOM_OUT; //ZoomOut
    mfAniAccel = fAccel;
}


int ARollCtl::LoadResource()
{
    mfCurrentAngle = 0.0f;
    return E_SUCCESS;
}


void ARollCtl::OnStopMotion()
{
    RollItem* pItem = 0;
    int nIndex = GetCurrentPos();
    if(nIndex  <= mlstData.size() - 1)
        pItem = mlstData[nIndex];
    
    CScenario::SendMessage(SGL_MESSAGE_ROLLSTOP,mnID,nIndex,(long)pItem,(long)this);
}

void ARollCtl::OnSelected()
{
    RollItem* pItem = 0;
    int nIndex = GetCurrentPos();
    if(nIndex  <= mlstData.size() - 1)
        pItem = mlstData[nIndex];
    CScenario::SendMessage(SGL_MESSAGE_ROOLSEL,mnID,nIndex,(long)pItem,(long)this);
}

bool ARollCtl::isLeftRoll(bool bTouch)
{
    int nCnt = (int)mlstData.size();
    int nPos = GetCurrentPos();
    if(bTouch)
    {
        if(nPos >= (nCnt - 1))
            return false;
    }
    return true;
    
}

bool ARollCtl::isRightRoll()
{
    
    int nPos = GetCurrentPos();
    if(nPos == 0)
        return false;
    return true;
}


void ARollCtl::RotMotion(int nTime)
{
    //터치하고 있지 않다면 각도를 정중앙으로 옮겨준다.
    if(mlTouchID == -1 && mAccelator != 0) //엑셀레이터가 존재하면.
    {
        if(mAccelator > 0)
        {
            
            if(mbRevolvingType == false && isRightRoll() == false ) 
                mAccelator *= 0.00001;
            
            mfCurrentAngle += (float)mAccelator ; 
            mAccelator -= nTime * mfTourceSensitivity * 0.05;
            
            
            if(mAccelator <= 0.0)
            {
                mAccelator = 0.0f;
            }
        }
        else
        {
            
            if(mbRevolvingType== false && isLeftRoll() == false ) 
                mAccelator *= 0.00001;
            
            mfCurrentAngle += (float)mAccelator; 
            mAccelator += nTime * mfTourceSensitivity * 0.05;
            
            if(mAccelator >= 0.0f)
            {
                mAccelator = 0.0f;
            }
            
        }
        
        if(mfCurrentAngle == 360)           mfCurrentAngle = 0;
        else if(mfCurrentAngle > 360)       mfCurrentAngle = (int)mfCurrentAngle % 360;
        else if(mfCurrentAngle == -360)     mfCurrentAngle = 0;
        else if(mfCurrentAngle < -360)      mfCurrentAngle = (int)mfCurrentAngle % 360;
        
        
    }
    else if(mlTouchID == -1) //위치로 ...
    {
        int nPosIndex = GetCurrentPos();
        int nTempAngle = 360  / mFaceCount;
        float nAngle = -((float)nPosIndex * (float)nTempAngle);//여기에 맞추어야 한다.
        
        float fv;
        
        if(nAngle == 0 && mfCurrentAngle!= 0 && mfCurrentAngle > 0 && mfCurrentAngle < nTempAngle / 2.0)
        {
            fv = 360 - mfCurrentAngle;
        }
        else if(nAngle == 0 && mfCurrentAngle!= 0 && mfCurrentAngle < 0 && mfCurrentAngle < -(360 - nTempAngle / 2.0))
        {
            fv = 360 + mfCurrentAngle;
        }
        else if(mfCurrentAngle <= 0)
        {
            if(mbRevolvingType== false && isLeftRoll() == false)
                fv = mfCurrentAngle + nAngle;
            else
                fv = mfCurrentAngle - nAngle;
        }
        else
        {
            if(mbRevolvingType== false && isRightRoll() == false)
                fv = mfCurrentAngle + (360 + nAngle);
            else
                fv = mfCurrentAngle - (360 + nAngle);
        }
        
        if ((fv < 1 && fv > -1) ||
            (fv <360 && fv > 359) //첫번째 인덱스는 선택할때 간혹 359.44 이렇게 나올경우가 있다.
            ) 
        {
            
            if(mfSelectedAngle != mfCurrentAngle)
            {
                //정지됨
                OnStopMotion();
            }
            
            mfCurrentAngle = nAngle;
            mfSelectedAngle = mfCurrentAngle;
            mnRotDirection = 0;
        }
        else
        {
            if(fv < 0)
            {
                //재자리로 돌아오다가 방향이 넘어가 버렸다. 
                if(mnRotDirection > 0)
                {
                    mfCurrentAngle = nAngle;
                    mnRotDirection = 0;
                }
                else
                {
                    mnRotDirection = -1;
                    mfCurrentAngle += nTime / 15;
                }
            }
            else if(fv > 0)
            {
                //재자리로 돌아오다가 방향이 넘어가 버렸다. 
                if(mnRotDirection < 0)
                {
                    mfCurrentAngle = nAngle;
                    mnRotDirection = 0;
                }
                else
                {
                    mnRotDirection = 1;
                    mfCurrentAngle -= nTime / 15 ;
                }
            }
        }
        
        if(mfCurrentAngle == 360)           mfCurrentAngle = 0;
        else if(mfCurrentAngle > 360)       mfCurrentAngle = (int)mfCurrentAngle % 360;
        else if(mfCurrentAngle == -360)     mfCurrentAngle = 0;
        else if(mfCurrentAngle < -360)      mfCurrentAngle = (int)mfCurrentAngle % 360;
    }
}



int ARollCtl::CloseResource()
{
  
    for (std::vector<RollItem*>::const_iterator i = mlstData.begin(); i != mlstData.end(); ++i) 
    {
        if((*i)->pRender)
        {
            delete (*i)->pRender;
        }
        if((*i)->pRollProperty)
        {
            delete (*i)->pRollProperty;
        }
        delete *i;
    }
    mlstData.clear();
    mfCurrentAngle = 0.0f;
    mnBeforeIndex = 0;
    mnBeforeAngle = -9999;
    
    return E_SUCCESS;
}


bool ARollCtl::BeginTouch(long lTouchID, float x, float y)
{
    bool bIs = true;
    
    
    int nIndex = 0;
    int nPosIndex = GetCurrentPos();
    int nS = nPosIndex - 1;
    int nE = nPosIndex + 1; //다음꺼는 보여야 한다.
    
    if (mbEnable == false || mbHide)     return bIs; //Disable은 클릭을 막는다.
    
    //int nDataMax = mlstData.size();
    
    //이미 터지된상태에 멀티터치는 이벤트를 발생하지 말자.
    if (mlTouchID != -1) return bIs;
    
    for(int j = nS; j <= nE; j++)
    {
        nIndex = j;
        if( j < 0)                      nIndex= mFaceCount + j;
        else if(j >= mFaceCount)        nIndex = j - mFaceCount;
        if(IsTabIn(nIndex, x, y))
        {
            mnTouchedIndex = nIndex;
            mlTouchID = lTouchID;
            mbTouchMoving = false;
            
            mfBeforeX = x;
            mfTouchStartAngle = mfCurrentAngle;
            mnTouchStartPos = nPosIndex;
            //mfBeforeY = y;
            
            mAccelator = 0.0f;
            
            bIs = false;
            break;
        }
    }
    return bIs;
}


bool ARollCtl::MoveTouch(long lTouchID,float x, float y)
{
    
    //float fGLX,fGLY;
    if(mlTouchID != -1 && mlTouchID == lTouchID)
    { 
        float rawVector = x - mfBeforeX;
        float vector = rawVector;
        if(gDisplayWidth > 0.0f)
            vector *= kRollTouchBaseWidth / gDisplayWidth;
        
        if(mbTouchMoving == false)
        {
            //3은 너무 움직임에 민감하면 단순 탭을 하기 어렵기때문에 3point가 움직이면 움직인 것으로 생각한다.
            if(rawVector > 4) vector = 1;
            //else if(vector > 0) vector = 1;
            else if(rawVector < -4) vector = -1;
        }
        
        if(mbRevolvingType == false)
        {
            if(vector < 0 && isLeftRoll(true) == false )
                vector *= 0.00001;
            else if(vector> 0 && isRightRoll() == false ) 
                vector *= 0.00001;
            
            //너무빨리 돌아가면 제한을 주자.
            if(vector > 0 && vector * mfTourceSensitivity > (360 / mFaceCount))
                vector *= 0.5;
            else if(vector < 0 && vector * mfTourceSensitivity < -(360 / mFaceCount))
                vector *= 0.5;
        }
        
        float fAngleDelta = vector * mfTourceSensitivity;
        float fMaxAngleStep = (360.0f / mFaceCount) * kRollMaxStepPerFaceRate;
        if(fAngleDelta > fMaxAngleStep)
            fAngleDelta = fMaxAngleStep;
        else if(fAngleDelta < -fMaxAngleStep)
            fAngleDelta = -fMaxAngleStep;

        mfCurrentAngle += fAngleDelta;
        mbTouchMoving = true; 
        
        //GameTime::getTimeDeltaConst()로 계산하지 않고 실제로 계산을해지 
        //탭을 노았을때.. 가속도를 정확하게 측정할수가 있다.
        float fDeltaMs = (float)GetClockDeltaConst();
        if(fDeltaMs < kRollMinInertiaDeltaMs)
            fDeltaMs = kRollMinInertiaDeltaMs;

        mAccelator += fAngleDelta / fDeltaMs;

        float fMaxAccel = (360.0f / mFaceCount) * kRollMaxAccelPerFaceRate;
        if(mAccelator > fMaxAccel)
            mAccelator = fMaxAccel;
        else if(mAccelator < -fMaxAccel)
            mAccelator = -fMaxAccel;
        
        mfBeforeX = x;
        
        return true;
    }
    return false;
}



bool ARollCtl::EndTouch(long lTouchID,float x, float y)
{
    int nPosIndex = GetCurrentPos();
    if(mlTouchID != -1 && mlTouchID == lTouchID)
    {
        
        //움직임..
        if(mbTouchMoving)
        {
            float fFaceAngle = 360.0f / mFaceCount;
            float fMovedAngle = mfCurrentAngle - mfTouchStartAngle;
            int nTargetPos = mnTouchStartPos;
            
            if(fMovedAngle <= -(fFaceAngle * kRollSnapCommitRate))
                nTargetPos = mnTouchStartPos + 1;
            else if(fMovedAngle >= (fFaceAngle * kRollSnapCommitRate))
                nTargetPos = mnTouchStartPos - 1;
            
            if(mbRevolvingType)
            {
                if(nTargetPos < 0)
                    nTargetPos = mFaceCount - 1;
                else if(nTargetPos >= mFaceCount)
                    nTargetPos = 0;
            }
            else
            {
                if(nTargetPos < 0)
                    nTargetPos = 0;
                else if(nTargetPos >= (int)mlstData.size())
                    nTargetPos = (int)mlstData.size() - 1;
            }
            
            if(nTargetPos != mnTouchStartPos)
            {
                mAccelator = 0.0f;
                SetCurrentPos(nTargetPos);
                OnStopMotion();
            }
            else if(mAccelator > -0.01f && mAccelator < 0.01f)
            {
                mAccelator = 0.0f;
            }
        }
        //탭함.
        else
        {
            
            //선택을 하였다.
            if(IsTabIn(nPosIndex, x, y))
            {
                //선택을 하였다.
                OnSelected();
            }
        }
        
        mbTouchMoving = false;
        mlTouchID = -1;
        mnTouchedIndex = -1;
        
        return true;
    }
    return false;
}

int ARollCtl::Initialize(int nControlID,SPoint *pPosition,SVector *pvDirection)
{
    mnID = nControlID;
    return Initialize(pPosition,pvDirection);
}

#ifdef ANDROID
void ARollCtl::ResetTexture()
{
    CControl::ResetTexture();
    
    int nSize = mlstData.size();
    for(int i = 0; i < nSize; i++)
    {
        if(mlstData[i]->sFileName.length() > 0)
            mlstData[i]->nTextureID = mpTextureMan->GetTextureID(mlstData[i]->sFileName.c_str());
    }
}
#endif
//Start ARender ------------------------------------------------

int ARollCtl::Initialize(SPoint *pPosition,SVector *pvDirection)
{
    
    return E_SUCCESS;
}

int ARollCtl::GetCurrentPos()
{
    int nPosIndex = 0;
    int div;
    float nSelIndex;
    
    //각도가 같을때 이전에 계산한 값을 리턴한다. (재계산을 하지 말자.)
    if(mnBeforeAngle == mfCurrentAngle)
        return mnBeforeIndex;
    
    div = 360 / mFaceCount;
    nSelIndex = (mfCurrentAngle < 0.0 ? 360.0f + mfCurrentAngle : mfCurrentAngle) / div;
    
    for( int i = 0; i < mFaceCount; i++)
    {
        float fMin = (float)i - 0.5;
        if(i == 0) fMin = - 0.5;
        float fMax = (float)i + 0.5;
        
        if(nSelIndex > fMin  && nSelIndex <= fMax)
        {
            nPosIndex = i;
            break;
        }
    }
    
    //오른쪽
    if(nPosIndex != 0)                          nPosIndex = mFaceCount - nPosIndex;
    else if(nPosIndex == mFaceCount)            nPosIndex = 0;
    
    mnBeforeIndex = nPosIndex;
    mnBeforeAngle = mfCurrentAngle;
    
    return nPosIndex;
    
}


void ARollCtl::SetCurrentPos(int nIndex)
{
    int div;
    float fSelIndex;
    div = 360 / mFaceCount;
    //fSelIndex = (mfCurrentAngle < 0.0 ? 360.0f + mfCurrentAngle : mfCurrentAngle) / (float)div;
    
    fSelIndex = mfCurrentAngle / (float)div;
    if(fSelIndex < 0) fSelIndex = -fSelIndex;
    
    
    if(fSelIndex == nIndex) return ;

//게임 종료하면, 다음 맵으로 돌아가 버린다. 그래서 그냥 정확하게 설정하자.
//    mfCurrentAngle = -div * nIndex + div / 2.0f; // 원래 정확하게 -div * nIndex를 해주어야 하는데 약간의 돌아가는 모습을 보여주기 위해서 + div/2.0f로 해서 반바퀴 도는 효과를 주었다.
    mfCurrentAngle = -div * nIndex;
//    mfCurrentAngle = mfCurrentAngle < 0.0 ? 360.0f + mfCurrentAngle : mfCurrentAngle;
    
    mnBeforeIndex = nIndex;
    mnBeforeAngle = mfCurrentAngle;

}


int ARollCtl::RenderBegin(int nTime)
{
    if(mbHide) return E_SUCCESS;
    
    glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
#if defined ANDROID || defined IPHONE
    glClearDepthf(1.0f);
#else //MAC
    glClearDepth(1.0f);
#endif
	glClearStencil(0);

    ActivateCamera();
    
    SetLight();
    
    //회전값을 계산한다.
    RotMotion(nTime);
    return E_SUCCESS;
}


void ARollCtl::SetLight()
{
    
    const GLfloat			lightAmbient[] = {0.2, 0.2, 0.2, 1.0};
	const GLfloat			lightDiffuse[] = {0.5, 0.5, 0.5, 1.0};
    GLfloat                 lightPosition[4]; 
    lightPosition[0] = 0.0f;
    //45도 각도로 햇빛을 비추자....
    lightPosition[1] = mViewZ + 500;
    lightPosition[2] = mViewZ + 500; 
    //0.0 특정 방향만 갖는 direction light , 1.0 방향 및 위치를 가지고 있는 태양광 (position light)
    lightPosition[3] = 0.0f;
    
    glEnable(GL_LIGHTING);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glEnable(GL_LIGHT0);
}



int ARollCtl::ActivateCamera()
{
    SPoint ptCamera;
    
    int nTime = GetClockDeltaConst();
    
    ptCamera.x = 0.0f;
    ptCamera.y = mViewHeight;
    
    if (mAniType == CONTROLANI_ZOOM_IN) 
    {
        float fPos = (float)nTime * -mfAniAccel;
        float fVZ = mViewZ + fPos;
        if(fVZ >= mfFarOrg)
        {
            mViewZ = fVZ;
            if(mViewZ == mfFarOrg)
                mAniType = CONTROLANI_NONE;
        }
        else
        {
            mViewZ = mfFarOrg;
            mAniType = CONTROLANI_NONE;
        }
    }
    else if(mAniType == CONTROLANI_ZOOM_OUT) //멀어진다.
    {
        float fR = mfFarOrg * 2;
        float fPos = (float)nTime * mfAniAccel;
        float fVZ = (float)mViewZ + fPos;
        if(fVZ <= fR)
        {
            mViewZ = fVZ;
            if(mViewZ == fR)
                mAniType = CONTROLANI_ZOOM_OUT_END;
        }
        else
        {
            mViewZ = fR;
            mAniType = CONTROLANI_ZOOM_OUT_END;
        }
    }
    else if(mAniType == CONTROLANI_ZOOM_OUT_END)
    {
        SetHide(true);
        mAniType = CONTROLANI_NONE;
        mViewZ = mfFarOrg;
    }
    
    ptCamera.z = mViewZ;
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    gluLookAt (ptCamera.x, 
               ptCamera.y, 
               ptCamera.z,
               
               0.0, 
               mViewHeight,  //카메라의 위치의 값이 똑같아여 픽킹이 정확한다.
               0.0f,
               
               0.0f, 
               1.0f , //y축 상향.
               0.0f);
    
    
    return E_SUCCESS;
}



int ARollCtl::Render()
{
    
    if(mbHide) return E_SUCCESS;
    
        
    return E_SUCCESS;
}


int ARollCtl::RenderEnd()
{
    if(mbHide) return E_SUCCESS;
    
    return E_SUCCESS;
}


int ARollCtl::OnEvent(SGLEvent *pEvent)
{
    return E_SUCCESS;
}

bool ARollCtl::IsTabIn(int nIndex,float x,float y)
{
    return false;
}
//End ARender ------------------------------------------------

