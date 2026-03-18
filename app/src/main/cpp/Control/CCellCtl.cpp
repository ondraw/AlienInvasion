//
//  CCellCtl.cpp
//  SongGL
//
//  Created by 송 호학 on 11. 12. 4..
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include "CCellCtl.h"
#include "CScrollContentCtl.h"
#include "CScenario.h"

CCellCtl::CCellCtl(CTextureMan *pTextureMan) : CButtonCtl(NULL,pTextureMan)
{   
    
}

CCellCtl::~CCellCtl()
{
    
}

CControl* CCellCtl::Clone(CControl* pNew)
{
    return CButtonCtl::Clone(pNew);
}

int CCellCtl::Initialize(float nWidth,float nHeight,vector<string>& lstImage,float fsu,float fsv,float feu,float fev)
{
    return Initialize(0,0,0,nWidth,nHeight,lstImage,fsu,fsv,feu,fev);
}

// nControlID , x, y는 무시한다.
int CCellCtl::Initialize(int nControlID,float x, float y, float nWidth,float nHeight,vector<string>& lstImage,float fsu,float fsv,float feu,float fev)
{
    return CButtonCtl::Initialize(nControlID,x,y,nWidth,nHeight,lstImage,fsu,fsv,feu,fev);
}

//클릭된 효과 없게 한다.
void CCellCtl::RenderBackgroundPushed()
{
    RenderBackgroundNormal();
}

bool CCellCtl::BeginTouch(long lTouchID,float x, float y)
{
    bool bR = true;
    if (mbEnable == false || mbHide)     return true; //Disable은 클릭을 막는다.
    
    
    if (mlTouchID != -1) return false; //이미 지금 컨트롤이 탭되어 있기때문에 다른 컨트롤이 사용하지 못하게 한다
    CScrollContentCtl* pContentCtl = (CScrollContentCtl*)mParent;
    if(IsTabIn(x, y))
    {
        if(pContentCtl->GetRowSelMode() == false)
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
        }
        else
        {
            
            //하단에 이벤트가 존재 하지 않다면 지금 자신의 컨트롤이 클릭되었다.
            mlTouchID = lTouchID;
            mbTouchMoving = false;
            ptBefore.x = x;
            ptBefore.y = y;
            
            mAccelatorVector[0] = 0.0f;
            mAccelatorVector[1] = 0.0f;
            
//            int nSize = mlstChild.size(); 
//            //하단이 존재하면 하단의 멘트가 발생하게 한다.
//            for (int i = 0; i < nSize; i++) 
//            {
//                CControl* ChildCon = mlstChild[i];
//                ChildCon->SetPushed(true); //모든 차일드를 선택되었다고 마크한다.
//            }
            

            OnButtonDown();
        }
        return false;
    }
    return true; //다른 하위뷰에 이벤트를 넘겨준다.
}

bool CCellCtl::OnButtonUp(bool bInSide)
{
    if(mButtonType == BUTTON_NORMAL)
    {
        mbPushed = false;
        if(bInSide)
        {
            CScenario::SendMessage(SGL_MESSAGE_CLICKED,GetParent()->GetParent()->GetID(),mbPushed,GetID(),(long)this);
        }
        
    }
    else if(mButtonType == BUTTON_TOGGLE) //버튼안쪽이 아니면 다시 원래데로 해준다.
    {
        if(bInSide)
            CScenario::SendMessage(SGL_MESSAGE_CLICKED,GetParent()->GetParent()->GetID(),mbPushed,GetID(),(long)this);
        else
            mbPushed = !mbPushed;
    }
    else if(mButtonType == BUTTON_GROUP_TOGGLE)
    {
        if(bInSide && mbOrgPushed == false)
            CScenario::SendMessage(SGL_MESSAGE_CLICKED,GetParent()->GetParent()->GetID(),mbPushed,GetID(),(long)this);
        else
            mbPushed = mbOrgPushed;
    }
    
    return true;
}

#ifdef MAC
bool CCellCtl::SubLastSelControl(CControl* pCtl,float x, float y)
{
    CList<CControl*>* pChild = pCtl->GetChilds();
    int nSize = pChild->size();
    
    if(nSize == 0 && pCtl->IsTabIn(x, y))
    {
        CControl::gLastSelControl = pCtl;
        return true;
    }
    
    for (int i = 0; i < nSize; i++)
    {
        CControl* ChildCon = pChild->get(i);
        if(SubLastSelControl(ChildCon,x,y))
        {
            return true;
        }
    }
    
    return false;
}
#endif

bool CCellCtl::EndTouch(long lTouchID,float x, float y)
{
    bool bR = true;
    
    CScrollContentCtl* pContentCtl = (CScrollContentCtl*)mParent;
    if(pContentCtl->GetRowSelMode() == false)
    {
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
            
            if(pContentCtl->GetTouchMoving() == false && IsTabIn(x,y))
                OnButtonUp(true);
            else
                OnButtonUp(false);
            mbTouchMoving = false;
            
            return false;
        }  
    } 
    else
    {
        
        if(mlTouchID != -1 && mlTouchID == lTouchID)
        {
            mlTouchID = -1;
            
//            int nSize = mlstChild.size(); 
//            //하단이 존재하면 하단의 멘트가 발생하게 한다.
//            for (int i = 0; i < nSize; i++) 
//            {
//                CControl* ChildCon = mlstChild[i];
//                ChildCon->SetPushed(false);
//            }
            
            if(pContentCtl->GetTouchMoving() == false && IsTabIn(x,y))
            {
#ifdef MAC
                //마지막 컨트롤..
                CControl::gLastSelControl = this;
                
                int nSize = mlstChild.size(); 
                for (int i = 0; i < nSize; i++)
                {
                    CControl* ChildCon = mlstChild[i];
                    //보통 컨트롤을 밑으로 하지 않기 때문에 밑에서 체크되었는지를 확힌하지 않고
                    //현재 형재 노드에서 가장 마지막위치....
//                    if(SubLastSelControl(ChildCon,x,y))
//                        break;
                    SubLastSelControl(ChildCon,x,y);
                }
                
#endif //MAC
                OnButtonUp(true);
            }
            else
                OnButtonUp(false);
            mbTouchMoving = false;
            
            return false;
        }  
    }
    return true; //다른 하위뷰에 이벤트를 넘겨준다.
}