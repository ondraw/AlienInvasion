//
//  CControl.cpp
//  SongGL
//
//  Created by 송 호학 on 11. 11. 30..
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include "CControl.h"
#include "sGLTrasform.h"
//#include "C2dRender.h"
#include "sGLUtils.h"
#include "sGL.h"

#define ZOOMZ 120
#define ZOOMINOUTZ 20


#ifdef MAC
CControl* CControl::gLastSelControl = NULL;
#endif //MAC


extern unsigned int mEscapeT;

CControl::CControl(CControl* pParent,CTextureMan *pTextureMan)
{
    mParent = pParent;
    mpTextureMan = pTextureMan;
    mlTouchID = -1;
    mbEnable = true;
    mbPushed = false;
    mbHide = false;
    
    mbTouchMoving = false;
    
    mfDPosX = 0.0f;
    mfDPosY = 0.0f;
    mfDPosZ = 0.0f;
    
    mfDWidth = 0.0f;
    mfDHeight = 0.0f;
    mnBackID = 0;
    mbCanMove = false;
    
    mfBackColor[0] = 1.0f; //r
    mfBackColor[1] = 1.0f; //g
    mfBackColor[2] = 1.0f; //b
    mfBackColor[3] = 1.0f; //a

    mfTextColor[0] = 0.0f; //r
    mfTextColor[1] = 0.0f; //g
    mfTextColor[2] = 0.0f; //b
    mfTextColor[3] = 1.0f; //a
    
    mAccelatorVector[0] = 0.0f;
    mAccelatorVector[1] = 0.0f;
    mfTourchSensitivity = 0.5;
    memset(mfWorldBackVertex, 0, sizeof(mfWorldBackVertex));
    
    mlData = 0;
    mlData2= 0;
    mAniType = CONTROLANI_NONE;
    mfDPosOrgX = mfDPosX;
    mfDPosOrgY = mfDPosY;
    mfAniAccel = 2.0;
    mnContentsID = 0;
    msContentsImage = NULL;
    mfWorldContentsCoordTex = NULL;
    mfBackDisalbeColor = NULL;
    
    mfbRotationZ = 0.f;
//    mfbRotationY = 0.f;
}


CControl::~CControl()
{
    if(msContentsImage)
    {
        delete[] msContentsImage;
        msContentsImage = NULL;
    }
    if(mfWorldContentsCoordTex)
    {
        delete[] mfWorldContentsCoordTex;
        mfWorldContentsCoordTex = NULL;
    }
    if(mfBackDisalbeColor)
    {
        delete [] mfBackDisalbeColor;
        mfBackDisalbeColor = NULL;
    }
    RemoveAll();
}

void CControl::SetDisableColor(float* fBackDisalbeColor)
{
    if(mfBackDisalbeColor)
    {
        delete[] mfBackDisalbeColor;
        mfBackDisalbeColor = NULL;
    }
    
    if(fBackDisalbeColor)
    {
        mfBackDisalbeColor = new float[sizeof(float)*4];
        memcpy(mfBackDisalbeColor, fBackDisalbeColor, sizeof(float)*4);
    }
    
}

CControl* CControl::Clone(CControl* pNew)
{
    if(pNew == NULL)
        pNew = new CControl(mParent,mpTextureMan);
    
    int nCnt = 0;

    pNew->mParent = mParent;
    pNew->mpTextureMan = mpTextureMan;
    
    pNew->mnID = mnID;
    pNew->mbEnable = mbEnable;
    pNew->mbPushed = mbPushed;
    pNew->mbHide = mbHide;
    pNew->mfDPosX = mfDPosX;
    pNew->mfDPosY = mfDPosY;    
    pNew->mfDPosZ = mfDPosZ;
    
    pNew->mfDWidth = mfDWidth;
    pNew->mfDHeight = mfDHeight;
    
    memcpy(pNew->mfWorldBackVertex,mfWorldBackVertex,sizeof(mfWorldBackVertex));
    pNew->mnBackID = mnBackID;
    pNew->mnContentsID = mnContentsID;
    pNew->mbCanMove = mbCanMove;

    memcpy(pNew->mfBackColor,mfBackColor,sizeof(mfBackColor));
    memcpy(pNew->mfTextColor,mfTextColor,sizeof(mfTextColor));
    memcpy(pNew->mAccelatorVector,mAccelatorVector,sizeof(mAccelatorVector));
    pNew->mfTourchSensitivity = mfTourchSensitivity;
    
    pNew->mlData = mlData;
    pNew->mlData2 = mlData2;
    
    pNew->mfDPosOrgX = mfDPosOrgX;
    pNew->mfDPosOrgY = mfDPosOrgY;
    pNew->mfAniAccel = mfAniAccel;
    pNew->mAniType = mAniType;
    
    pNew->mfBackDisalbeColor = NULL;
    if(mfBackDisalbeColor)
    {
        pNew->mfBackDisalbeColor = new float[sizeof(float)*4];
        memcpy(pNew->mfBackDisalbeColor, mfBackDisalbeColor, sizeof(float)*4);
    }
    
#if defined(ANDROID) || defined(MAC)
    //OnPause후에 OnResume을 하면 오픈지엘 거시기가 다시 만들어진다.
    //그래서 TextureID를 재할당해주어야 한다.
    nCnt = mlstImage.size();
    for(int i = 0; i < nCnt; i++)
    {
        pNew->mlstImage.push_back(mlstImage[i].c_str());
    }
#endif
    memcpy(pNew->mfWorldBackCoordTex,mfWorldBackCoordTex,sizeof(mfWorldBackCoordTex));

    if (msContentsImage)
    {
        pNew->msContentsImage = new char[strlen(msContentsImage) + 1];
        strcpy(pNew->msContentsImage, msContentsImage);
    }
    
    if(mfWorldContentsCoordTex)
    {
        pNew->mfWorldContentsCoordTex = new float[8];
        memcpy(pNew->mfWorldContentsCoordTex, mfWorldContentsCoordTex, sizeof(float)*8);
    }
    
    nCnt = mlstChild.size();
    for(int i = 0; i < nCnt; i++)
    {
        pNew->mlstChild.add(mlstChild[i]->Clone());
    }
    return pNew;
}

void CControl::SetAni(CONTROLANI nAniType,float fAccel)
{
    mAniType = nAniType;
    mfAniAccel = fAccel;
    if(nAniType == CONTROLANI_LIN) //왼쪽으로 숨긴다.
    {
        float fMoveVector = -mfDWidth - mfDPosX; 
        MoveVector(fMoveVector, 0.0f);
    }
    else if(nAniType == CONTROLANI_RIN)
    {
        float fMoveVector = gDisplayWidth - mfDPosX; 
        MoveVector(fMoveVector, 0.0f); //오른쪽으로 숨긴다.
    }
    else if(nAniType == CONTROLANI_TIN) //우로 숨는다.
    {
        float fMoveVector = -mfDHeight - mfDPosY; 
        MoveVector(0.0f, fMoveVector);
    }
    else if(nAniType == CONTROLANI_BIN) //아래로 숨긴다.
    {
        float fMoveVector = gDisplayHeight - mfDPosY;
        MoveVector(0.0f, fMoveVector);
    }
    else if(nAniType == CONTROLANI_ZOOM_IN)
    {
        float fMoveVector = -ZOOMZ; 
        ZoomVector(fMoveVector); // -ZOOMZ뒤로 이동한다.
    }
    else if(nAniType == CONTROLANI_WIGWAG)
    {
        nmAnimationTime = GetGGTimeAtPause() + 800; //0.8초동안
    }
    //숨긴것을 보여준다.
    SetHide(false);
}

void CControl::AddControl(CControl* pControl)
{
    mlstChild.add(pControl);
}

void CControl::InsertControl(int nIndex,CControl* pControl)
{
    mlstChild.add(nIndex,pControl);
}

void CControl::Remove(CControl* pControl)
{
    int nSize = mlstChild.size();
    for (int i = 0; i < nSize; i++) 
    {
        CControl* con = mlstChild[i];
        if(pControl == con)
        {
            mlstChild.remove(i);
            delete con;
            break;
        }
    }
    
}

void CControl::Remove(int nIndex)
{
    CControl* con = mlstChild[nIndex];
    delete con;
    mlstChild.remove(nIndex);
}

void CControl::RemoveAll()
{
    int nSize = mlstChild.size();
    for (int i = 0; i < nSize; i++) 
    {
        CControl* con = mlstChild[i];
        delete con;
    }
    mlstChild.clear();
}

void CControl::TopFront() //가장 상단으로 옮겨준다.
{
    if(mParent)
    {
        
        CList<CControl*>* list = mParent->GetChilds();
        int nCnt = list->size();
        for (int i = 0; i < nCnt; i++) 
        {
            if(list->get(i) == this)
            {
                list->remove(i);
                break;
            }
        }
        list->add(this);
    }
}

//CControl* CControl::GetChildByID(int nID)
//{
//    int nSize = mlstChild.size();
//    if(GetID() == nID) return this;
//    for (int i = 0; i < nSize; i++) 
//    {
//        CControl* con = mlstChild[i];
//        if(con->GetID() == nID) return con;
//    }
//    return NULL;
//}

int CControl::Initialize(int nControlID,float x, float y, float nWidth,float nHeight,vector<string>& lstImage,float fsu,float fsv,float feu,float fev)
{
    mfDPosX = x;
    mfDPosY = y;
    mfDWidth = nWidth;
    mfDHeight = nHeight;
    mfDPosOrgX = mfDPosX;
    mfDPosOrgY = mfDPosY;
    mnID= nControlID;
    sglGet2DTextureCoord(fsu,fsv,feu,fev,mfWorldBackCoordTex);
    SetImageData(lstImage);
    return E_SUCCESS;
}

void CControl::SetBackCoordTex(float fsu,float fsv,float feu,float fev)
{
    sglGet2DTextureCoord(fsu,fsv,feu,fev,mfWorldBackCoordTex);
}


void CControl::SetRotationZ(float fAngle)
{
    mfbRotationZ = fAngle;
    int nSize = mlstChild.size();
    //하단이 존재하면 하단의 멘트가 발생하게 한다.
    for (int i = 0; i < nSize; i++)
    {
        CControl* ChildCon = mlstChild[i];
        ChildCon->SetRotationZ(fAngle);
    }
}

//void CControl::SetRotationY(float fAngle)
//{
//    mfbRotationY = fAngle;
//    int nSize = mlstChild.size();
//    //하단이 존재하면 하단의 멘트가 발생하게 한다.
//    for (int i = 0; i < nSize; i++)
//    {
//        CControl* ChildCon = mlstChild[i];
//        ChildCon->SetRotationY(fAngle);
//    }
//}



int CControl::RenderBegin(int nTime)
{
    if (mbHide) return E_SUCCESS;
    
    TourchMotion(nTime);
    
    //애니메이션을 한다.
    if(mAniType != CONTROLANI_NONE)
    {
        
        
        if(mAniType == CONTROLANI_LIN) //왼쪽으로 나온다. (1) = (7) 1은 7과 로직이 같다.
        {
            float fMove = nTime * mfAniAccel;
            float fPos = mfDPosX + fMove;
            if( fPos <= mfDPosOrgX)
            {
                MoveVector(fMove, 0.0f);
                if(fPos == mfDPosOrgX)
                    mAniType = CONTROLANI_NONE;
            }
            else
            {
                fMove = mfDPosOrgX - mfDPosX;
                MoveVector(fMove, 0.0f);
                mAniType = CONTROLANI_NONE;
            }
        } 
        else if(mAniType == CONTROLANI_RIN) //오른쪽에서 나온다. (2) = (4)
        {
            float fMove = nTime * -mfAniAccel;
            float fPos = mfDPosX + fMove;
            if( fPos >= mfDPosOrgX)
            {
                MoveVector(fMove, 0.0f);
                if(fPos == mfDPosOrgX)
                    mAniType = CONTROLANI_NONE;
            }
            else
            {
                fMove = mfDPosOrgX - mfDPosX;
                MoveVector(fMove, 0.0f);
                mAniType = CONTROLANI_NONE;
            }
        }
        else if(mAniType == CONTROLANI_ROUT_END)
        {
            //숨긴다.
            SetHide(true); 
            //다시 원 위치.
            MoveVector(-(gDisplayWidth - mfDPosOrgX), 0.0f);
            mAniType = CONTROLANI_NONE;
        }
        else if(mAniType == CONTROLANI_ROUT) //오르쪽으로 숨는다. (3) = (5)
        {
            //아래로 이동해야한다.
            float fMove = nTime * mfAniAccel;
            float fPos = mfDPosX + fMove;
            if( fPos <= gDisplayWidth)
            {
                MoveVector(fMove, 0.0f);
                if(fPos == gDisplayWidth)
                    mAniType = CONTROLANI_ROUT_END;
            }
            else
            {
                fMove = gDisplayWidth - mfDPosX;
                MoveVector(fMove, 0.0f);
                mAniType = CONTROLANI_ROUT_END;
            }
        }
        else if(mAniType == CONTROLANI_LOUT_END)
        {
            //숨긴다.
            SetHide(true);
            //다시 원 위치.
            float fMove =  mfDWidth + mfDPosOrgX; 
            MoveVector(fMove, 0.0f);
            mAniType = CONTROLANI_NONE;

        }
        else if(mAniType == CONTROLANI_LOUT) //왼쪽에서 나간다. (8) = (9)
        {
            float fMove = nTime * -mfAniAccel;
            float fPos = mfDPosX + fMove;
            if( fPos >= -mfDWidth)
            {
                MoveVector(fMove, 0.0f);
                if(fPos == -mfDWidth)
                    mAniType = CONTROLANI_LOUT_END;
            }
            else
            {
                fMove = -mfDWidth - mfDPosX;
                MoveVector(fMove, 0.0f);
                mAniType = CONTROLANI_LOUT_END;
            }
        }
        else if(mAniType == CONTROLANI_BIN) //아래에서 나온다. (4) = (2)
        {
            float fMove = nTime * -mfAniAccel;
            float fPos = mfDPosY + fMove;
            if( fPos >= mfDPosOrgY)
            {
                MoveVector(0.0f, fMove);
                if(fPos == mfDPosOrgY)
                    mAniType = CONTROLANI_NONE;
            }
            else
            {
                fMove = mfDPosOrgY - mfDPosY;
                MoveVector(0.0f, fMove);
                mAniType = CONTROLANI_NONE;
            }
        }
        if(mAniType == CONTROLANI_TIN) //위에서 나온다. (7) = (1)
        {
            float fMove = nTime * mfAniAccel;
            float fPos = mfDPosY + fMove;
            if( fPos <= mfDPosOrgY)
            {
                MoveVector(0.0f, fMove);
                if(fPos == mfDPosOrgY)
                    mAniType = CONTROLANI_NONE;
            }
            else
            {
                fMove = mfDPosOrgY - mfDPosY;
                MoveVector(0.0f, fMove);
                mAniType = CONTROLANI_NONE;
            }
        } 
        else if(mAniType == CONTROLANI_BOUT_END)
        {
            //숨긴다.
            SetHide(true);
            //다시 원 위치.
            MoveVector(0.0f, -(gDisplayHeight - mfDPosOrgY));
            mAniType = CONTROLANI_NONE;
        }       
        else if(mAniType == CONTROLANI_BOUT) //아래로 숨는다. (5) = (3)
        {
            //아래로 이동해야한다.
            float fMove = nTime * mfAniAccel;
            float fPos = mfDPosY + fMove;
            if( fPos <= gDisplayHeight)
            {
                MoveVector(0.0, fMove);
                if(fPos == gDisplayHeight)
                    mAniType = CONTROLANI_BOUT_END;
            }
            else
            {
                fMove = gDisplayHeight - mfDPosY;
                MoveVector(0.0f, fMove);
                mAniType = CONTROLANI_BOUT_END;
            }
        }
        else if(mAniType == CONTROLANI_TOUT_END)
        {
            //숨긴다.
            SetHide(true);
            //다시 원 위치.
            float fMove =  mfDHeight + mfDPosOrgY; 
            MoveVector(0.0f, fMove);
            mAniType = CONTROLANI_NONE;
        }
        else if(mAniType == CONTROLANI_TOUT) //위쪽에서 나간다. (9) = (8)
        {
            float fMove = nTime * -mfAniAccel;
            float fPos = mfDPosY + fMove;
            if( fPos >= -mfDHeight)
            {
                MoveVector(0.0f, fMove);
                if(fPos == -mfDHeight)
                    mAniType = CONTROLANI_TOUT_END;
            }
            else
            {
                fMove = -mfDHeight - mfDPosY;
                MoveVector(0.0f, fMove);
                mAniType = CONTROLANI_TOUT_END;
            }
        }
        if(mAniType == CONTROLANI_ZOOM_IN) //줌인한다.
        {
            float fMove = nTime * mfAniAccel;
            float fPos = mfDPosZ + fMove;
            if( fPos <= 0)
            {
                ZoomVector(fMove);
                if(fPos == 0)
                    mAniType = CONTROLANI_NONE;
            }
            else
            {
                fMove = 0 - mfDPosZ;
                ZoomVector(fMove);
                mAniType = CONTROLANI_NONE;
            }
        } 
        else if(mAniType == CONTROLANI_ZOOM_OUT) //줌아웃한다.
        {
            //아래로 이동해야한다.
            float fMove = nTime * -mfAniAccel;
            float fPos = mfDPosZ + fMove;
            if( fPos >= -ZOOMZ)
            {
                ZoomVector(fMove);
                if(fPos == -ZOOMZ)
                    mAniType = CONTROLANI_ZOOM_OUT_END;
            }
            else
            {
                fMove = -ZOOMZ - mfDPosZ;
                ZoomVector(fMove);
                mAniType = CONTROLANI_ZOOM_OUT_END;
            }
        }
        else if(mAniType == CONTROLANI_ZOOM_OUT_END)
        {
            //숨긴다.
            SetHide(true);
            //다시 원 위치.
            float fMove =  ZOOMZ + 0; 
            ZoomVector(fMove);
            mAniType = CONTROLANI_NONE;
        }
        else if(mAniType == CONTROLANI_ZOOM_IN_OUT)
        {
            //아래로 이동해야한다.
            float fMove = nTime * mfAniAccel;
            float fPos = mfDPosZ + fMove;
            if( fPos <= ZOOMINOUTZ)
            {
                ZoomVector(fMove);
                if(fPos == ZOOMINOUTZ)
                    mAniType = CONTROLANI_ZOOM_IN_OUT2;
            }
            else
            {
                fMove = ZOOMINOUTZ - mfDPosZ;
                ZoomVector(fMove);
                mAniType = CONTROLANI_ZOOM_IN_OUT2;
            }
        }
        else if(mAniType == CONTROLANI_ZOOM_IN_OUT2)
        {
            float fMove = nTime * -mfAniAccel;
            float fPos = mfDPosZ + fMove;
            if( fPos >= 0)
            {
                ZoomVector(fMove);
                if(fPos == 0)
                    mAniType = CONTROLANI_NONE;
            }
            else
            {
                fMove = 0 - mfDPosZ;
                ZoomVector(fMove);
                mAniType = CONTROLANI_NONE;
            }        
        }
        else if(mAniType== CONTROLANI_ZOOM_IN_HIDE)
        {
            //아래로 이동해야한다.
            float fMove = nTime * mfAniAccel;
            float fPos = mfDPosZ + fMove;
            if( fPos <= ZOOMZ)
            {
                ZoomVector(fMove);
                if(fPos == ZOOMZ)
                    mAniType = CONTROLANI_ZOOM_IN_HIDE_END;
            }
            else
            {
                fMove = ZOOMZ - mfDPosZ;
                ZoomVector(fMove);
                mAniType = CONTROLANI_ZOOM_IN_HIDE_END;
            }
        }
        else if(mAniType== CONTROLANI_ZOOM_IN_HIDE_END)
        {
            //숨긴다.
            SetHide(true);
            //다시 원 위치.
            float fMove =  -ZOOMZ; 
            ZoomVector(fMove);
            mAniType = CONTROLANI_NONE;
        }
        else if(mAniType == CONTROLANI_WIGWAG)
        {
            NextWigWag(nTime);
        }
        else if(mAniType == CONTROLANI_WIGWAG_END)
        {
            //원위치시킨다.
            SetPosition(mfDPosOrgX, mfDPosOrgY);
            mAniType = CONTROLANI_NONE;
        }
        
    }
    
    int nSize = mlstChild.size();
    for (int i = 0; i < nSize; i++) 
    {
        CControl* ChildCon = mlstChild[i];
        if(ChildCon) ChildCon->RenderBegin(nTime);
    }
    
    
    return E_SUCCESS;
}

int CControl::Render()
{
    if (mbHide) return E_SUCCESS;
    
    //Draw.....
    
    int nSize = mlstChild.size();
    for (int i = 0; i < nSize; i++) 
    {
        CControl* ChildCon = mlstChild[i];
        if(ChildCon) ChildCon->Render();
    }
    return E_SUCCESS;
}

int CControl::RenderEnd()
{
    if (mbHide) return E_SUCCESS;
    
    int nSize = mlstChild.size();
    for (int i = 0; i < nSize; i++) 
    {
        CControl* ChildCon = mlstChild[i];
        if(ChildCon) ChildCon->RenderEnd();
    }
    return E_SUCCESS; 
}

void CControl::NextWigWag(int nTime)
{
    static int gTime = 0; //너무빨리 왔다갔다 하는 것을 막자.
    //현재시간보다 많으면 애니메이션을 중지한다.
    if(GetGGTimeAtPause() >= nmAnimationTime)
    {
        mAniType = CONTROLANI_WIGWAG_END;
        return;
    }
    
    if(nTime < 50)
    {
        //너무빨리 흔들면 좀 거시기 하자나.
        gTime += nTime;
        if (gTime > 50) //0.5 초 이상일때만 흔든다.
            gTime -= 50;
        else //그이하의 시간은 스킵하자. 한텝포쉬자.
            return;
    }
    else
        gTime = 0;
    
    if(ptBefore.x >= 0.f)
        ptBefore.x = -10.f * mfAniAccel;
    else
        ptBefore.x = 10.f * mfAniAccel;
        

    if(ptBefore.y >= 0.f)
        ptBefore.y = -10.f * mfAniAccel;
    else
        ptBefore.y = 10.f * mfAniAccel;
    
    MoveVector(ptBefore.x,ptBefore.y);
}

int CControl::OnEvent(SGLEvent *pEvent)
{
    int nSize = mlstChild.size();
    for (int i = 0; i < nSize; i++) 
    {
        CControl* ChildCon = mlstChild[i];
        ChildCon->OnEvent(pEvent);
    }
    return E_SUCCESS;
}


bool CControl::BeginTouch(long lTouchID,float x, float y)
{
    bool bR = true;
    if (mbEnable == false || mbHide)     return true; //Disable은 클릭을 막는다.

    
    if (mlTouchID != -1) return false; //이미 지금 컨트롤이 탭되어 있기때문에 다른 컨트롤이 사용하지 못하게 한다
    if(IsTabIn(x, y))
    {
        
        //하단에 이벤트가 존재 하지 않다면 지금 자신의 컨트롤이 클릭되었다.
        mlTouchID = lTouchID;
        mbTouchMoving = false;
        ptBefore.x = x;
        ptBefore.y = y;

        mAccelatorVector[0] = 0.0f;
        mAccelatorVector[1] = 0.0f;
        
        int nSize = mlstChild.size(); 
        if(nSize == 0) //최하단의 컨트롤에 이벤트가 간다.
        {
            OnButtonDown();
            return false;
        }
        
        //하단이 존재하면 하단의 멘트가 발생하게 한다.
        for (int i = nSize - 1; i >= 0; i--)
        {
            CControl* ChildCon = mlstChild[i];
            bR = ChildCon->BeginTouch(lTouchID,x,y);
            if(bR == false) //자식중에 어느 놈이 클릭을 하였다. 
            {
                return bR; //계속 검색하지 않고 넘어간다.
            }
        }
        OnButtonDown();
        return false;
    }
    return true; //다른 하위뷰에 이벤트를 넘겨준다.
}


bool CControl::MoveTouch(long lTouchID,float x, float y)
{
    bool bR = true;
    if(mlTouchID != -1 && mlTouchID == lTouchID)
    { 
        
        int nSize = mlstChild.size(); 
        if(nSize == 0) //최하단의 컨트롤에 이벤트가 간다.
        {
            if(mbCanMove == true)
            {
                float vectorx = x - ptBefore.x;
                float vectory = y - ptBefore.y;
                if(vectorx != 0 || vectory != 0)
                {
                    
                    if(mbTouchMoving == false)
                    {
                        
                        float asbVX = vectorx < 0.0f ? -vectorx : vectorx;
                        float asbVY = vectory < 0.0f ? -vectory : vectory;
                        
                        //3은 너무 움직임에 민감하면 단순 탭을 하기 어렵기때문에 3point가 움직이면 움직인 것으로 생각한다.
                        if(!(asbVX > 3 || asbVY > 3)) return true;
                    }
                    
                    mAccelatorVector[0] += vectorx * mfTourchSensitivity*3 / mEscapeT;
                    mAccelatorVector[1] += vectory * mfTourchSensitivity*3 / mEscapeT;
                    
                    OnButtonMove(vectorx,vectory);
                    mbTouchMoving = true;
                }                
                ptBefore.x = x;
                ptBefore.y = y;
                return false;
            }
            
        }
        
        //하단이 존재하면 하단의 멘트가 발생하게 한다.
        for (int i = nSize - 1; i >= 0; i--)
        {
            CControl* ChildCon = mlstChild[i];
            bR = ChildCon->MoveTouch(lTouchID,x,y);
            if(bR == false) //자식중에 어느 놈이 클릭을 하였다. 
            {
                if(mbCanMove == true && mbTouchMoving == true)
                    mbTouchMoving = false;
                return bR; //계속 검색하지 않고 넘어간다.
            }
        }
        
        if(mbCanMove == true)
        {
            float vectorx = x - ptBefore.x;
            float vectory = y - ptBefore.y;
            if(vectorx != 0 || vectory != 0)
            {
                if(mbTouchMoving == false)
                {
                    
                    float asbVX = vectorx < 0.0f ? -vectorx : vectorx;
                    float asbVY = vectory < 0.0f ? -vectory : vectory;
                    
                    
                    //3은 너무 움직임에 민감하면 단순 탭을 하기 어렵기때문에 3point가 움직이면 움직인 것으로 생각한다.
                    if(!(asbVX > 3 || asbVY > 3)) return true;
                }
                
                
                
                mAccelatorVector[0] += vectorx * mfTourchSensitivity*3 / mEscapeT;
                mAccelatorVector[1] += vectory * mfTourchSensitivity*3 / mEscapeT;
                
                
                OnButtonMove(vectorx,vectory);
                mbTouchMoving = true;
            }
            ptBefore.x = x;
            ptBefore.y = y;
            return false;
        }
    }
    
    return true; //다른 하위뷰에 이벤트를 넘겨준다.
}

bool CControl::EndTouch(long lTouchID,float x, float y)
{
    bool bR = true;
    if(mlTouchID != -1 && mlTouchID == lTouchID)
    {
        mlTouchID = -1;
        
        int nSize = mlstChild.size(); 
        if(nSize == 0) //최하단의 컨트롤에 이벤트가 간다.
        {
            if(mbTouchMoving == false && IsTabIn(x,y))
            {
#ifdef MAC
                CControl::gLastSelControl = this;
#endif //MAC
                OnButtonUp(true);
            }
            else
                OnButtonUp(false);
            
            mbTouchMoving = false;
            return false;
        }
        
        //하단이 존재하면 하단의 멘트가 발생하게 한다.
        for (int i = nSize - 1; i >= 0; i--)
        {
            CControl* ChildCon = mlstChild[i];
            bR = ChildCon->EndTouch(lTouchID,x,y);
            if(bR == false) //자식중에 어느 놈이 클릭을 하였다. 
            {
                mbTouchMoving = false;
                return bR; //계속 검색하지 않고 넘어간다.
            }
        }
        
        if(mbTouchMoving == false && IsTabIn(x,y))
            OnButtonUp(true);
        else
            OnButtonUp(false);
        mbTouchMoving = false;
        
        return false;
    }   
    return true; //다른 하위뷰에 이벤트를 넘겨준다.
}

bool CControl::IsTabIn(float x,float y)
{
    //    -94.341965,55.475338,0.000000
    //    -94.341965,51.407902,0.000000
    //    -78.524155,51.407902,0.000000
    //    -78.524155,55.475338,0.000000
    float fx,fy;
    if(GetHide()) return false;
    CControl::DtoGL(x,y,&fx, &fy);
    if(fx >= mfWorldBackVertex[0] && fx<= mfWorldBackVertex[6] &&
       fy >= mfWorldBackVertex[7] && fy<= mfWorldBackVertex[1])
        return true;
    return false;
}

bool CControl::OnButtonDown()
{
    mbPushed = true;
    return true;
}

bool CControl::OnButtonMove(float fVectorX,float fVectorY)
{
    //sglLog("buttonmove %d\n",mnID);
    return true;
}

bool CControl::OnButtonUp(bool bInSide)
{
    mbPushed = false;
    return true;
}


void CControl::SetEnable(bool bEnable) 
{ 
    mbEnable = bEnable;
// 하단을 자동으로 모두 셋해버리면 이전에 셋해놓은 모든 설정이 변경되어 버린다.
//    int nSize = mlstChild.size(); 
//    //하단이 존재하면 하단의 멘트가 발생하게 한다.
//    for (int i = 0; i < nSize; i++) 
//    {
//        CControl* ChildCon = mlstChild[i];
//        ChildCon->SetEnable(bEnable);
//    }
}

void CControl::SetHide(bool bHide)
{
    mbHide = bHide;
// 하단을 자동으로 모두 셋해버리면 이전에 셋해놓은 모든 설정이 변경되어 버린다.
//    int nSize = mlstChild.size(); 
//    //하단이 존재하면 하단의 멘트가 발생하게 한다.
//    for (int i = 0; i < nSize; i++) 
//    {
//        CControl* ChildCon = mlstChild[i];
//        ChildCon->SetHide(bHide);
//    }
}

void CControl::SetImageData(vector<string>& lstImage)
{
    
#if defined(ANDROID) || defined(MAC)
    mlstImage.clear();
    int nCnt = lstImage.size();
    for (int i = 0; i < nCnt; i++) {
        mlstImage.push_back(lstImage[i]);
    }
#endif
}

void CControl::SetContentsData(const char* sContentsImage)
{
    if(msContentsImage)
    {
        delete[] msContentsImage;
        msContentsImage = NULL;
    }
    if(mfWorldContentsCoordTex)
    {
        delete[] mfWorldContentsCoordTex;
        mfWorldContentsCoordTex = NULL;
    }
    mnContentsID = 0;
    
    if(sContentsImage == NULL) return;
    
    msContentsImage = new char[strlen(sContentsImage) + 1];
    strcpy(msContentsImage, sContentsImage);
    
    
    mfWorldContentsCoordTex = new float[8];
    memcpy(mfWorldContentsCoordTex, gPanelCoordTex, sizeof(gPanelCoordTex));
}




void CControl::SetPosition(float  fX,float fY)
{
    float fVX = fX - mfDPosX;
    float fVY = fY - mfDPosY;
    MoveVector(fVX,fVY);
}


void CControl::MoveVector(float  fDVectorX,float  fDVectorY)
{
    float   matrix[16];
    int     nIndex = 0;
    float   fGLMoveX,fGLMoveY;
    
    mfDPosX+= fDVectorX;
    mfDPosY+= fDVectorY;
    
    CControl::DtoGL(fDVectorX, &fGLMoveX);
    CControl::DtoGL(fDVectorY, &fGLMoveY);
    
    //GL은 Display의 좌표에서 Y가 반대로 움직인다.
    fGLMoveY = -fGLMoveY; 
    
    sglLoadIdentityf(matrix);
    sglTranslateMatrixf(matrix, fGLMoveX, fGLMoveY, 0.0f);
    
    for (int i = 0; i < 4; i++) 
    {
        nIndex = i*3;
        sglMultMatrixVectorf(&mfWorldBackVertex[nIndex], matrix,&mfWorldBackVertex[nIndex]);
    }
    
    int nSize = mlstChild.size(); 
    //하단이 존재하면 하단의 멘트가 발생하게 한다.
    for (int i = 0; i < nSize; i++) 
    {
        CControl* ChildCon = mlstChild[i];
        ChildCon->MoveVector(fDVectorX,fDVectorY);
    }
}

void CControl::ZoomVector(float  fDVectorZ)
{
    float   matrix[16];
    int     nIndex = 0;
    mfDPosZ += fDVectorZ;
    sglLoadIdentityf(matrix);
    sglTranslateMatrixf(matrix, 0.0f, 0.0f, fDVectorZ);
    for (int i = 0; i < 4; i++) 
    {
        nIndex = i*3;
        sglMultMatrixVectorf(&mfWorldBackVertex[nIndex], matrix,&mfWorldBackVertex[nIndex]);
    }
    
    int nSize = mlstChild.size(); 
    //하단이 존재하면 하단의 멘트가 발생하게 한다.
    for (int i = 0; i < nSize; i++) 
    {
        CControl* ChildCon = mlstChild[i];
        ChildCon->ZoomVector(fDVectorZ);
    }
}


void CControl::TourchMotion(int nTime)
{
    if(mbCanMove == false) return;
    
    //터치하고 있지 않다면 각도를 정중앙으로 옮겨준다.
    if(mlTouchID == -1 && (mAccelatorVector[0] != 0.0f || mAccelatorVector[1] != 0.0f)) //엑셀레이터가 존재하면.
    {
        if(mAccelatorVector[0] > 0)
        {
            mAccelatorVector[0] -= nTime * mfTourchSensitivity / mEscapeT;
            if(mAccelatorVector[0] < 0.0)
            {
                mAccelatorVector[0] = 0.0f;
            }
        }
        else if(mAccelatorVector[0] < 0)
        {
            mAccelatorVector[0] += nTime * mfTourchSensitivity / mEscapeT;
            if(mAccelatorVector[0] > 0.0)
            {
                mAccelatorVector[0] = 0.0f;
            }
        }
        
        if(mAccelatorVector[1] > 0)
        {
            mAccelatorVector[1] -= nTime * mfTourchSensitivity / mEscapeT;
            if(mAccelatorVector[1] < 0.0)
            {
                mAccelatorVector[1] = 0.0f;
            }
        }
        else if(mAccelatorVector[1] < 0)
        {
            mAccelatorVector[1] += nTime * mfTourchSensitivity / mEscapeT;
            if(mAccelatorVector[1] > 0.0)
            {
                mAccelatorVector[1] = 0.0f;
            }
            
        }
    }
}




int CControl::ActivateCamera()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    
    //프리즘을 니어를 1로 하였기 때문에 카메마를 최대한 1에 영상을 맞추어야 한다.
    gluLookAt (0, 
               0, 
               CAMERA_YPOS_2D,
               0,
               0,
               0,
               
               0, 
               1,
               0);
    
	return E_SUCCESS;
}


void CControl::DtoGL(GLfloat nLine,GLfloat* fLine)
{
    //    SGLCAMERA    *pCamera = NULL;
    
    //    if(mpWorld == NULL) return;
    //    pCamera = mpWorld->GetCamera();
    *fLine = (xmax_3d * CAMERA_YPOS_2D * 2.0f) * (GLfloat)nLine / (GLfloat)gDisplayWidth;
}


void CControl::DtoGL(GLfloat nX,GLfloat nY,GLfloat* fOutX,GLfloat* fOutY)
{
    //    SGLCAMERA    *pCamera = NULL;
    
    //    if(g_pWorld == NULL) return;
    //    pCamera = g_pWorld->GetCamera();
    
    
    //GluSubsetf::xmax_3d * 2 : mnWidth = *fOutX : nX 값을 추출하여
    //left  (- GluSubsetf::xmax_3d)는 좌측 상단이 아니라 중앙이기때문에 xmax_3d(즉 넓이의 반을) 빼주면 중앙으로 이동한다.,
    //bottom(화면의 좌표는 top인데 opengl좌표는 방향이 반대이다 그래서 -을 해주고 중앙에서 bottom으로 옮기게 + GluSubsetf::ymax_3d을 해준다) 을 센터로 옮긴다.
    
    *fOutX = (xmax_3d * CAMERA_YPOS_2D * 2.0f) * (GLfloat)nX / (GLfloat)gDisplayWidth - (xmax_3d * CAMERA_YPOS_2D);
    *fOutY = -(ymax_3d * CAMERA_YPOS_2D * 2.0f) * (GLfloat)nY / (GLfloat)gDisplayHeight + (ymax_3d * CAMERA_YPOS_2D);
}

void   CControl::GLtoD(GLfloat fX,GLfloat fY,GLint* nOutX,GLint* nOutY)
{   
    int nHalfWidth,nHalfHeight;
    nHalfWidth = gDisplayWidth / 2;
    nHalfHeight= gDisplayHeight /2;
    //GluSubsetf::xmax_3d  : mnHalfWidth = fX : *nOutX 값을 추출하여
    //center를 0,0좌료로 옮긴다.+ (GLfloat)mnHalfWidth
    //+ 0.5 반올림    
    *nOutX = (GLfloat)nHalfWidth * fX / (xmax_3d * CAMERA_YPOS_2D) + (GLfloat)nHalfWidth +  0.5f;
    *nOutY = -(GLfloat)nHalfHeight * fY / (ymax_3d * CAMERA_YPOS_2D)+ (GLfloat)nHalfHeight +  0.5f;
}


void    CControl::MakeWorldStickVertexD(GLfloat *pWorldVertex,GLfloat *pOrgVertex,float xPos,float yPos)
{
    int nIndex0 = 0;
    DtoGL(xPos, yPos,&xPos,&yPos);
    for (int i = 0; i < 4; i++) 
    {
        nIndex0 = i * 3;
        pWorldVertex[nIndex0]      =       xPos - pOrgVertex[nIndex0]        - (xmax_3d * CAMERA_YPOS_2D);
        pWorldVertex[nIndex0 + 1]  =       yPos - pOrgVertex[nIndex0 + 1]    + (ymax_3d * CAMERA_YPOS_2D);
        pWorldVertex[nIndex0 + 2]  =       pOrgVertex[nIndex0 + 2];
    }
}

void    CControl::MakeWorldStickVertexGL(GLfloat *pWorldVertex,GLfloat *pOrgVertex,float xPos,float yPos)
{
    
    int nIndex0 = 0;
    
    for (int i = 0; i < 4; i++) 
    {
        nIndex0 = i * 3;
        pWorldVertex[nIndex0]      =       xPos - pOrgVertex[nIndex0]        - (xmax_3d * CAMERA_YPOS_2D);
        pWorldVertex[nIndex0 + 1]  =       yPos - pOrgVertex[nIndex0 + 1]    + (ymax_3d * CAMERA_YPOS_2D);
        pWorldVertex[nIndex0 + 2]  =       pOrgVertex[nIndex0 + 2];
        
    }  
}

void CControl::FlipVertical() //위아래 회전.
{
    
    float fsu = mfWorldBackCoordTex[0];
    float fsv = mfWorldBackCoordTex[1];
    float feu = mfWorldBackCoordTex[4];
    float fev = mfWorldBackCoordTex[5];

    mfWorldBackCoordTex[0] = fsu;
    mfWorldBackCoordTex[2] = fsu;
    
    mfWorldBackCoordTex[4] = feu;
    mfWorldBackCoordTex[6] = feu;
    
    mfWorldBackCoordTex[3] = fsv;
    mfWorldBackCoordTex[5] = fsv;
    
    mfWorldBackCoordTex[1] = fev;
    mfWorldBackCoordTex[7] = fev;
}

CControl* CControl::FindControl(int nID)
{
    int nSize = mlstChild.size();
    for (int i = 0; i < nSize; i++)
    {
        CControl* con = mlstChild[i];
        if(con->GetID() == nID) return con;
        con = con->FindControl(nID);
        if(con) return con;
    }
    return NULL;
}

void CControl::SetTranslate(float fAlpha)
{
    float ft[4];

    int nCnt = mlstChild.size();
    for (int i = 0; i < nCnt; i++)
    {
        CControl* ct = mlstChild.get(i);
        ct->GetTextColor(ft);
        ft[3] = fAlpha;
        ct->SetTextColor(ft);
        
        ct->GetBackColor(ft);
        ft[3] = fAlpha;
        ct->SetBackColor(ft);
        
        ct->SetTranslate(fAlpha);
    }
    
    GetTextColor(ft);
    ft[3] = fAlpha;
    SetTextColor(ft);
    
    GetBackColor(ft);
    ft[3] = fAlpha;
    SetBackColor(ft);

}

#ifdef ANDROID
void CControl::ResetTexture()
{
    if(mlstImage.size() > 0)
    {
        //왜 카피하냐 하면 SetImageData내부에 mlstImage는 클리어하고 다시 카피하기 때문에 자기자신을 카피할수가 없잖아.
        vector<string> lstImage;
        int nCnt = mlstImage.size();
        for (int j = 0; j < nCnt; j++)
        {
            lstImage.push_back(mlstImage[j]);
        }
        SetImageData(lstImage);
    }

    
    int nSize = mlstChild.size();
    for (int i = 0; i < nSize; i++)
    {
        CControl* con = mlstChild[i];
        con->ResetTexture();
    }

    if(msContentsImage)
    {
      mnContentsID = mpTextureMan->GetTextureID(msContentsImage);
    }
    
}
#endif