//
//  C2dStick.cpp
//  SongGL
//
//  Created by 호학 송 on 11. 4. 4..
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#include <math.h>
#include "C2dStick.h"
#include "CHWorld.h"
#include "CTextureMan.h"
#include "sGL.h"
#include "sGLText.h"
#include "CButtonCtl.h"
#include "CSGLCore.h"
#include "sGLUtils.h"
#include "CMutiplyProtocol.h"
C2dStick::C2dStick(IHWorld* pWorld) : CControl(NULL,pWorld->GetTextureMan())
{
    mlTouch1 = -1;
    mlTouch2 = -1;
//    mbZoomInOut = false;
//    mbTurnView = false;
//    mbPoUpDown = false;
//    mbFreeFire = false;
//    mpParking = NULL;
//    mpParkingFly = NULL;
    mpMovingFly = NULL;
    mnStickType = STIKCKTYPE_NONE;
    mpWorld= pWorld;
    mbMovingFlyCenter[0] = -9999.0f;
    

}

C2dStick::~C2dStick()
{   
//    if(mpParkingFly)
//    {
//        delete mpParkingFly;
//        mpParkingFly= NULL;
//    }
//    if(mpParking)
//    {
//        delete mpParking;
//        mpParking = NULL;
//    }
    if(mpMovingFly)
    {
        delete mpMovingFly;
        mpMovingFly= NULL;
    }
}


#ifdef ANDROID
void C2dStick::ResetTexture()
{
    CTextureMan *pMan = mpWorld->GetTextureMan();
    mpMovingFly->ResetTexture();
    mfMovingFlyTextureID = pMan->GetTextureID(IMG_PARKING);
    
}
#endif

int C2dStick::Initialize(SPoint *pPosition,SVector *pvDirection)
{
    vector<string>  lstImage;
    vector<float>     layInfo;
    CTextureMan *pMan = mpWorld->GetTextureMan();

//    lstImage.push_back(IMG_PARKING);
//    lstImage.push_back("none");
    PROPERTYI::GetPropertyFloatList(L_M_Next, layInfo);
//    mpParking = new CButtonCtl(NULL,pMan);
//    mpParking->Initialize(100,layInfo[1], gDisplayHeight - layInfo[2] - layInfo[3], layInfo[0], layInfo[2] , lstImage);
    
//    lstImage.clear();
//    lstImage.push_back(IMG_PARKING_FLY);
//    lstImage.push_back("none");
//    mpParkingFly = new CLabelCtl(NULL,pMan);
//    mpParkingFly->Initialize(101,layInfo[1], gDisplayHeight - layInfo[2] - layInfo[3], layInfo[0], layInfo[2] , lstImage);
    

    lstImage.clear();
    lstImage.push_back(IMG_PARKING_FLY);
    lstImage.push_back("none");
    mpMovingFly = new CLabelCtl(NULL,pMan);
    
    mpMovingFly->Initialize(103,layInfo[1], gDisplayHeight - layInfo[2] - layInfo[3], layInfo[0], layInfo[2] , lstImage,0.f,0.f,1.f,1.f);
    mpMovingFly->SetHide(true);
    
    mfMovingFlyTextureID = pMan->GetTextureID(IMG_PARKING);

    return E_SUCCESS;
}





int C2dStick::RenderBegin(int nTime)
{
    mpMovingFly->RenderBegin(nTime);
    if((mnStickType & (int)STIKCKTYPE_ACTORMOVE) && mbMovingFlyCenter[0] != -9999.0f)
    {
        float matrix[16];
        sglLoadIdentityf(matrix);
        sglTranslateMatrixf(matrix, mbMovingFlyCenter[0], mbMovingFlyCenter[1] + 1.0f, mbMovingFlyCenter[2]);
        sglScaleMatrixf(matrix, 5.0f, 5.0f, 5.0f);
        
        sglMultMatrixVectorf(mfMovingFlyVertex, matrix,gPanelVertexZ);
        sglMultMatrixVectorf(mfMovingFlyVertex+3, matrix,gPanelVertexZ+3);
        sglMultMatrixVectorf(mfMovingFlyVertex+6, matrix,gPanelVertexZ+6);
        sglMultMatrixVectorf(mfMovingFlyVertex+9, matrix,gPanelVertexZ+9);
    }
    
    return E_SUCCESS;
}

int C2dStick::RenderMoveingFly()
{

    if((mnStickType & STIKCKTYPE_ACTORMOVE) && mbMovingFlyCenter[0] != -9999.0f)
    {
        glBindTexture(GL_TEXTURE_2D, mfMovingFlyTextureID);
        glTexCoordPointer(2, GL_FLOAT, 0, gPanelCoordTex);
        glVertexPointer(3, GL_FLOAT, 0, mfMovingFlyVertex);
        glDrawElements(GL_TRIANGLE_STRIP, gshPanelIndicesCnt, GL_UNSIGNED_SHORT, gPanelIndices);
    }
    
    return E_SUCCESS;
}


int C2dStick::Render()
{
    mpMovingFly->Render();
    return E_SUCCESS;
}

int C2dStick::RenderEnd()
{
    mpMovingFly->RenderEnd();
    return E_SUCCESS;
}

int C2dStick::OnEvent(SGLEvent *pEvent)
{
    return E_SUCCESS;
}


bool C2dStick::IsCenterCenter(float x, float y)
{
    //아이템/레이더/폭탄 테이블이 아닌 센터..
    CHWorld* pWorld = (CHWorld*)mpWorld;
    return pWorld->IsCenterClick(x, y);
}

void C2dStick::OnClick(float fx,float fy)
{
    SGLEvent Event;
    float ptTabPoint[9];
    int nSelIndex;
    CHWorld* pWorld = (CHWorld*)mpWorld;
    CSGLCore* pSGLCore = pWorld->GetSGLCore();
    long  addressAP;
    ptTabPoint[0] = -9999.0f;
    int nResult = pSGLCore->IsTabIn((int)fx,(int)fy,nSelIndex,ptTabPoint,addressAP);
    //0 : no, 0x04<<8:TranObj 0x02<<8:NorObj 0x01<<8:Terrian 0x08<<8:Sprite
    nResult = nResult? nResult >> 8 : 0;
    if(nResult == 0x08) //객체...
    {
        //Fire을 한다.
        if(ptTabPoint[0] != -9999.0f)
        {
            CSprite* pActor = pWorld->GetActor();
            
            int nID = ((CSprite*)addressAP)->GetModelID();
            if(pActor && pActor->GetState() == SPRITE_RUN)
            {
                if(nID >= ACTORID_BLZ_DTOWER && nID < ACTORID_BLZ_END &&
                   ((CSprite*)addressAP)->GetTeamID() == pActor->GetTeamID())
                {
                    //pWorld->SetClickedSprite(((CSprite*)addressAP));
                    CScenario::SendMessage(SGL_SELECTED_SPRITE,0,0,0,(long)addressAP); //안드로이드는 터지시 텍스쳐바운딩하면 에러가 나기때문에 메세지를 보내서 처리하자.
                    return;
                }
            
                Event.lParam = 0;
                Event.lParam2 = 0;
                Event.nMsgID = GSL_DONT_AUTO_FIRE;
                Event.lObject = 0;
                mpWorld->OnEvent(&Event);

                
                pActor->AimAutoToEnemy((CSprite*)addressAP);
                
                //보낸다.
                pWorld->GetScenario()->GetMutiplyProtocol()->SendSyncClickSpriteToByStick(pActor->GetID(),((CSprite*)addressAP)->GetID());
            }
        }
    }
    else if(nResult == 0x01) //땅...
    {
        //---------------------------
        //지점에서 가장가까운 탱크를 찾는다.
        //---------------------------
        CSprite* pActor = pWorld->GetActor();
        if(pActor && pActor->GetState() == SPRITE_RUN)
        {
// Deleted By Song 2013.2.15 너무 쉽게 하지말고, 눌려지는 곳에 타격하게 끔해주자.
//            CSprite* pTarget = pWorld->GetTargetNearByPoint(pActor->GetTeamID(),ptTabPoint[0],ptTabPoint[2]);
//            if(pTarget)
//                pActor->AimAutoToEnemy((CSprite*)pTarget);
//            else
//            {
                if(ptTabPoint[0] != -9999.0f)
                {

                    if(pActor && pActor->GetState() == SPRITE_RUN)
                    {
                        Event.lParam = 0;
                        Event.lParam2 = 0;
                        Event.nMsgID = GSL_DONT_AUTO_FIRE;
                        Event.lObject = 0;
                        mpWorld->OnEvent(&Event);

                        
                        SPoint ptTarget;
                        ptTarget.x = ptTabPoint[0];
                        ptTarget.y = ptTabPoint[1];
                        ptTarget.z = ptTabPoint[2];
                        pActor->AimAutoToEnemy(&ptTarget);
                        
                        //보낸다.
                        pWorld->GetScenario()->GetMutiplyProtocol()->SendSyncClickToByStick(pActor->GetID(),&ptTarget);
                    }
//                }
            }
        }
    }
    else
    {
        Event.lParam = 0;
        Event.lParam2 = 0;
        Event.nMsgID = GSL_BOMB_FIRE;
        Event.lObject = 0;
        mpWorld->OnEvent(&Event);
    }
}

void C2dStick::ShowFly(bool bv, float x, float y)
{
    if(bv)
    {
        float fW,fH;
        CHWorld* pWorld = (CHWorld*)mpWorld;
        CSGLCore* pSGLCore = pWorld->GetSGLCore();
        mpMovingFly->SetHide(false);
        mpMovingFly->GetSize(fW, fH);
        mpMovingFly->SetPosition(x - fW/2.f, y - fH/2.f);
        mbMovingFlyCenter[0] = -9999.0f;
        
        //오른쪽.
        //        fX = x + mMovingFlyDif[0] / 2.8f;
        //        fY = y + mMovingFlyDif[1] / 2.8f;
        
        
        if(pSGLCore->IsTabInOnlyTerrian(x, y, mbMovingFlyCenter))
        {
            //            HLogD("(%f,%f) Move\n",x,y);
        }
        mptBefore1[0] = x;
        mptBefore1[1] = y;
        mnStickType |= (int)STIKCKTYPE_ACTORMOVE;
    }
    else
    {
        mpMovingFly->SetHide(true);
        mbMovingFlyCenter[0] = -9999.0f;
        mnStickType &= ~(int)STIKCKTYPE_ACTORMOVE;
    }
}


CSprite* C2dStick::IsTabInZeroGroup(float x,float y)
{
    //2> 스프라이트를 그린다.
    CSprite* pActor = mpWorld->GetActor();
    CHWorld* pWorld = (CHWorld*)mpWorld;
    CSGLCore* pSGLCore = pWorld->GetSGLCore();
    
    if(pActor == NULL || (pActor && pActor->GetState() != SPRITE_RUN)) return NULL;

    CListNode<CSprite>* pList = pSGLCore->GetSpriteList();
    for(ByNode<CSprite>* it = pList->begin(); it != pList->end();)
    {
        CSprite* Object = it->GetObject();
        //주인공이거나.. 미사일탱크일때
        if(Object->GetState() == SPRITE_RUN &&
           Object->IsVisibleRender() &&
           Object->GetTeamID() == pActor->GetTeamID() &&
           (pActor == Object || Object->IsAssistance()))
        {
            if(Object->IsTabInZeroGroup(pWorld->GetPicking(),  x, y))
                return Object;
        }
        it = it->GetNextNode();
    }
    return NULL;
}

bool C2dStick::BeginTouch(long lTouchID,float x, float y)
{
    if(IsCenterCenter(x,y) == false) return true;
    CHWorld* pWorld = (CHWorld*)mpWorld;
    CSprite* pActor = pWorld->GetActor();
    if(pActor== NULL || (pActor && pActor->GetState() != SPRITE_RUN)) return true;
    
    mClickEscape = GetGGTime();
    if(mlTouch1 == -1 && mlTouch2 == -1) //주인공 이동
    {
        mptBefore1[0] = x;
        mptBefore1[1] = y;
        mlTouch1 = lTouchID;
        if((mpActorMove = IsTabInZeroGroup(x, y)))
        {
            float fW,fH;
            mpMovingFly->GetSize(fW, fH);
            mpMovingFly->SetPosition(x - fW/2.f, y - fH/2.f);
            mpMovingFly->SetHide(false);
            mnStickType |= (int)STIKCKTYPE_ACTORMOVE;
        }
        else
        {
#if VIEWROTATION_ANI
            BeginClock();
            mfStartRotX = x;
#endif
        }
        return false;
    }
    else if( (mnStickType & (int)STIKCKTYPE_ACTORMOVE) && mlTouch2 == -1) //이동중이고 공격을 위해 클릭했으면
    {
    }
    else if( mlTouch2 == -1) //핀치줌
    {
#ifdef PINCHZOOM
        mptBefore2[0] = x;
        mptBefore2[1] = y;
        mlTouch2 = lTouchID;
        //두손가락의 거리...
        mLenBefore = (mptBefore1[0] - mptBefore2[0]) * (mptBefore1[0] - mptBefore2[0]) + (mptBefore1[1] - mptBefore2[1]) * (mptBefore1[1] - mptBefore2[1]);
#endif //PINCHZOOM
    }
    else
    {
        ClearTouch();
    }
    return true;
    
}

bool C2dStick::MoveTouch(long lTouchID,float x, float y)
{
    
    SGLEvent Event;
    CHWorld* pWorld = (CHWorld*)mpWorld;
    CSGLCore* pSGLCore = pWorld->GetSGLCore();
    if((mnStickType & (int)STIKCKTYPE_ACTORMOVE) && mlTouch1 == lTouchID) //이동
    {
        mbMovingFlyCenter[0] = -9999.0f;
        if(pSGLCore->IsTabInOnlyTerrian(x, y, mbMovingFlyCenter))
        {
            float fW,fH;
            mpMovingFly->GetSize(fW, fH);
            mpMovingFly->SetPosition(x - fW/2.f, y - fH/2.f);
            
            mptBefore1[0] = x;
            mptBefore1[1] = y;
        }
        return false;
    }
    else if((mnStickType & (int)STIKCKTYPE_ACTORMOVE) && mlTouch2 == lTouchID) //이동중 오른쪽이 움직이면 무시..
    {
    }
    else if(!(mnStickType & (int)STIKCKTYPE_ACTORMOVE) && mlTouch1 == lTouchID && mlTouch2 == -1) //뷰전환..
    {
        float fDifX = mptBefore1[0] - x;
        int nId = GSL_VIEW_TURNRIGHT;
        if(fDifX < 0)
        {
            fDifX = -fDifX;
            nId = GSL_VIEW_TURNLEFT;
        }
        
        mnStickType |= STIKCKTYPE_ROTATIONVIEW;
        Event.nMsgID = nId;
        Event.lParam = fDifX;
        Event.lParam2 = 0;
        Event.lObject = 0;
        mpWorld->OnEvent(&Event);
        
        mptBefore1[0] = x;
        mptBefore1[1] = y;
        return false;
    }
    else if(mlTouch1 != -1 && mlTouch2 == lTouchID) //핀치줌
    {
        //두손가락의 거리...
        int nLen = (mptBefore1[0] - x) * (mptBefore1[0] - x) + (mptBefore1[1] - y) * (mptBefore1[1] - y);
        int div = mLenBefore - nLen;
        int nId = GSL_VIEW_MOVEDOWN; //다운
        if(div < 0)
            nId = GSL_VIEW_MOVEUP; //업
        
        mnStickType |= STIKCKTYPE_ZOOM;
        Event.nMsgID = nId;
        Event.lParam = div;
        Event.lParam2 = 0;
        Event.lObject = 0;
        mpWorld->OnEvent(&Event);
        mLenBefore = nLen;
        
        mptBefore2[0] = x;
        mptBefore2[1] = y;
        return false;
    }
    else if(mlTouch2 != -1 && mlTouch1 == lTouchID) //핀치줌
    {
        //두손가락의 거리...
        int nLen = (mptBefore2[0] - x) * (mptBefore2[0] - x) + (mptBefore2[1] - y) * (mptBefore2[1] - y);
        int div = mLenBefore - nLen;
        int nId = GSL_VIEW_MOVEDOWN; //다운
        if(div < 0)
            nId = GSL_VIEW_MOVEUP; //업
        
        mnStickType |= STIKCKTYPE_ZOOM;
        Event.nMsgID = nId;
        Event.lParam = div;
        Event.lParam2 = 0;
        Event.lObject = 0;
        mpWorld->OnEvent(&Event);
        mLenBefore = nLen;
        mptBefore1[0] = x;
        mptBefore1[1] = y;
        return false;
    }
    
    return true;
}

bool C2dStick::EndTouch(long lTouchID,float x, float y)
{
    SGLEvent Event;
    CHWorld* pWorld = (CHWorld*)mpWorld;
    CSprite* pActor = pWorld->GetActor();
    if(pActor == NULL || (pActor && pActor->GetState() != SPRITE_RUN)) return true;
    
    if((mnStickType & (int)STIKCKTYPE_ACTORMOVE) && mlTouch1 == lTouchID) //이동한다.
    {
        if(mpActorMove)
        {
            if(mpActorMove->IsTabInZeroGroup(pWorld->GetPicking(),x, y) == false) //캐릭터를 움직인다.
            {
                SGLEvent Event;
                Event.nMsgID = SGL_MOVE_TO_POINT_SIMPLE;
                Event.lParam = x;
                Event.lParam2 = y;
                Event.lObject = (long)mpActorMove;
                pWorld->OnEvent(&Event);
            }
            else
            {
                Event.lParam = 0;
                Event.lParam2 = 0;
                Event.lObject = (long)mpActorMove;
                Event.nMsgID = GSL_MOVE_STOP|GSL_TRUN_STOP;
                pWorld->OnEvent(&Event);
                
                pWorld->GetMutiplyProtocol()->SendSyncStopMove(mpActorMove->GetID());
                
                CScenario::SendMessage(SGL_SELECTED_SPRITE,0,0,0,(long)mpActorMove); //안드로이드는 터지시 텍스쳐바운딩하면 에러가 나기때문에 메세지를 보내서 처리하자.
            }
        }
        mpMovingFly->SetHide(true);
        mnStickType &= ~(int)STIKCKTYPE_ACTORMOVE;
        mbMovingFlyCenter[0] = -9999.0f;
        mlTouch1 = -1;
        mpActorMove = NULL;
        return false;
    }
    else if((mnStickType & (int)STIKCKTYPE_ACTORMOVE) && mlTouch2 == lTouchID) //이동중에 공격을 한다.
    {
        if(IsCenterCenter(x,y))
        {
            //겔럭시3는 너무 민감해서 클릭 이벤트 발생이 너무 어렵다.
            //0.15초 안에 클릭하면 폭탄발사로 생각한다.
            unsigned long lEscapeTime = GetGGTime() - mClickEscape;
            if(lEscapeTime < 150) OnClick(x,y);
            
        }
        mlTouch2 = -1;
        return false;
    }
    else if((mnStickType & (int)STIKCKTYPE_ROTATIONVIEW) && mlTouch1 == lTouchID) //뷰젼환.
    {
#if VIEWROTATION_ANI
        float fMoveEscapeTime = EndClock();
#else
        float fMoveEscapeTime = 0.0f;
#endif
        Event.nMsgID = GSL_VIEW_TURNSTOP;
        Event.lParam = fMoveEscapeTime * 1000;
        Event.lParam2 = mfStartRotX - x;
        Event.lObject = 0;
        //-----------------------------------------------------------------------------------------
        //안드로이드같은 것은 너무 민감해서 .........
        unsigned long lEscapeTime = GetGGTime() - mClickEscape;
        if(lEscapeTime < 150 && (Event.lParam2 > -10 && Event.lParam2 < 10) && IsCenterCenter(x,y)) OnClick(x,y); //클릭을 보낸다. (민감하다면)
        //-----------------------------------------------------------------------------------------
        
       
        mpWorld->OnEvent(&Event);
        mnStickType &= ~(int)STIKCKTYPE_ROTATIONVIEW;
        mlTouch1 = -1;
        
//        HLogD("1 = %f 2 = %f\n",mptBefore1[0]-x,mptBefore1[1]-y);
        return false;
    }
    else if((mnStickType & (int)STIKCKTYPE_ZOOM) && (mlTouch1 == lTouchID || mlTouch2 == lTouchID)) //이동중에 공격을 한다.
    {
        //마지막 가속도를 사용하여 뷰의 거시기를 조절한다.
        Event.lParam = 0;
        Event.lParam2 = 0;
        Event.nMsgID = GSL_VIEW_MOVESTOP;
        Event.lObject = 0;
        mpWorld->OnEvent(&Event);
        if(mlTouch1 == lTouchID)
            mlTouch1 = -1;
        else if(mlTouch2 == lTouchID)
            mlTouch2 = -1;
        mnStickType &= ~(int)STIKCKTYPE_ZOOM;
        return false;
    }
    else if(mlTouch1 == lTouchID || mlTouch2 == lTouchID)
    {
        if(IsCenterCenter(x,y)) OnClick(x,y);
        if(mlTouch1 == lTouchID) mlTouch1 = -1;
        else if(mlTouch2 == lTouchID) mlTouch2 = -1;
        return false;
    }
    return true;
}