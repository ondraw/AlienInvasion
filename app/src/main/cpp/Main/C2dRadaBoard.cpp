//
//  C2dRadaBoard.cpp
//  SongGL
//
//  Created by Songs on 11. 4. 1..
//  Copyright 2011 thinkm. All rights reserved.
//

#include "C2dRadaBoard.h"
#include "CHWorld.h"
#include "CTextureMan.h"
#include "CK9Sprite.h"
#include <math.h>
#include "sGLText.h"
#include "sGLUtils.h"

//#define ENAMYRECT 10  
//#define BOMBRECT  5


//적탱크
GLfloat C2dRadaBoard::g_EnamyTankVertex[12]={0};

//폭탄
GLfloat C2dRadaBoard::g_BombVertex[12]={0};

SPoint C2dRadaBoard::gBoadCenter;

C2dRadaBoard::C2dRadaBoard(IHWorld* pWorld) : CControl(NULL,pWorld->GetTextureMan())
{
    vector<float> layouts;
    
    mpWorld = pWorld;
    
    PROPERTYI::GetPropertyFloatList("Info_RadaBoard", layouts);
    
    //160,160
    mfBOARD_CENTER_X = layouts[0];
    mfBOARD_CENTER_Y = layouts[1];
    mnBOARD_CIRCLE_COUNT = layouts[2];
    mnBOARD_CIRCLE_LINES = layouts[3];
    mfBOARD_CIRCLE_RADIAN_INTERVAL = layouts[4];
    
   mPoints = new GLfloat*[mnBOARD_CIRCLE_COUNT];
   for (int i = 0; i < mnBOARD_CIRCLE_COUNT; i++) 
   {
       mPoints[i] = new GLfloat[mnBOARD_CIRCLE_LINES*3];
   }
    
    mWorldPoints = new GLfloat*[mnBOARD_CIRCLE_COUNT];
    for (int i = 0; i < mnBOARD_CIRCLE_COUNT; i++) 
    {
        mWorldPoints[i] = new GLfloat[mnBOARD_CIRCLE_LINES*3];
    }
    pmObjects = NULL;
    m_fBeforeTAngled = -9999.0;
    m_fBeforeTPoAngled = -9999.0;
    m_fBeforeTArrow = -9999.0;
    mlTouch = -1;
    
    //스태틱을 초기화해준다.
    gBoadCenter.x = 0;
    gBoadCenter.y = 0;
    mpDMainTower = NULL;
    CControl::DtoGL(mfBOARD_CIRCLE_RADIAN_INTERVAL * mnBOARD_CIRCLE_COUNT, &mfMaxRadian);
}

C2dRadaBoard::~C2dRadaBoard()
{
    if(mPoints)
    {
        for (int i = 0; i < mnBOARD_CIRCLE_COUNT; i++) {
            delete mPoints[i];
        }
        delete mPoints;
    }
    if(mWorldPoints)
    {
        for (int i = 0; i < mnBOARD_CIRCLE_COUNT; i++) {
            delete mWorldPoints[i];
        }
        delete mWorldPoints;
    }
}


float C2dRadaBoard::MapRate(bool bReset)
{
    static float fR = -99999999.0f;
    if(fR == -99999999.0f || bReset)
    {
        vector<float> layouts;
        PROPERTYI::GetPropertyFloatList("Info_RadaBoard", layouts);
        CControl::DtoGL(layouts[4]*layouts[2], &fR);
    }
    return fR;
}

void C2dRadaBoard::SetObjects(CListNode<RadaBoardObject>* pObjects)
{
    pmObjects = pObjects;
}



int C2dRadaBoard::Initialize(SPoint *pPosition,SVector *pvDirection)
{
    vector<float> layouts;
//    float ENAMYRECT = 10;
//    float BOMBRECT  = 5;
//    if(gnDisplayType == DISPLAY_IPAD)
//    {
//        ENAMYRECT *= 2;
//        BOMBRECT *= 2;
//    }
    PROPERTYI::GetPropertyFloatList("Info_RadaActor", layouts);
    float ENAMYRECT = layouts[2];
    float BOMBRECT  = layouts[3];
    
    makecirclef();
    SetBoadPosition(mfBOARD_CENTER_X,mfBOARD_CENTER_Y);
    
    CTextureMan *pMan = mpWorld->GetTextureMan();
    //float fXCenter,fYCenter;
    
    mglTexEnmayMainID = pMan->GetTextureID(SGL_2D_FILENAME_ENAMY_MAIN);
    mglTexEnamyTankID = pMan->GetTextureID(SGL_2D_FILENAME_ENAMY);
    mglTexRadaFireID = pMan->GetTextureID(SGL_2D_FILENAME_RADAFIRE);
    mglTexEnamyFighterID = pMan->GetTextureID(SGL_2D_FILENAME_ENAMYFIGTHER);
    mglTexMyTeamTankID = pMan->GetTextureID("RadiaMyTeamTank.tga");
    mglTexDTower = pMan->GetTextureID("RaidaBlzIcon.tga");
    

    
    mglTexTBodyID = pMan->GetTextureID(SGL_2D_FILENAME_RADATBODY);
    mglTexTHeaderID = pMan->GetTextureID(SGL_2D_FILENAME_RADATHEADER);
    mglTexRadaPannel = pMan->GetTextureID(SGL_2D_FILENAME_RADAPANNEL);
    mglTexImpactPoint= pMan->GetTextureID(SGL_2D_FILENAME_TARGET);
    mglTexArrow = pMan->GetTextureID("Arrow.tga");
    
//----------------------------------------------------------------------------------------------    
    float fBackRect = mfBOARD_CIRCLE_RADIAN_INTERVAL * (float)mnBOARD_CIRCLE_COUNT + 10.f;
    //left,top
    m_BoardVertex[0] =  - fBackRect;
    m_BoardVertex[2] = 0.00007f;  //배경을 좀더 뒤에서 그린다.
    m_BoardVertex[1] =  - fBackRect;
    DtoGL(m_BoardVertex[0], m_BoardVertex[1], &m_BoardVertex[0],&m_BoardVertex[1]);
    
    //left,bottom
    m_BoardVertex[3] =  - fBackRect;
    m_BoardVertex[5] = 0.00007f;  //배경을 좀더 뒤에서 그린다.
    m_BoardVertex[4] = fBackRect;
    DtoGL(m_BoardVertex[3], m_BoardVertex[4], &m_BoardVertex[3],&m_BoardVertex[4]);
    
    //right,bottom
    m_BoardVertex[6] = fBackRect;
    m_BoardVertex[8] = 0.00007f;  //배경을 좀더 뒤에서 그린다.
    m_BoardVertex[7] = fBackRect;    
    DtoGL(m_BoardVertex[6], m_BoardVertex[7], &m_BoardVertex[6],&m_BoardVertex[7]);
    
    //right,top
    m_BoardVertex[9] = fBackRect;
    m_BoardVertex[11] = 0.00007f; //배경을 좀더 뒤에서 그린다.
    m_BoardVertex[10] = - fBackRect; 
    DtoGL(m_BoardVertex[9], m_BoardVertex[10], &m_BoardVertex[9],&m_BoardVertex[10]);

    MakeWorldStickVertexGL(m_BoardVertex,m_BoardVertex,gBoadCenter.x, gBoadCenter.y);
//---------------------------------------------------------------------------------------------- 

    
//----------------------------------------------------------------------------------------------    
    //회전하기때문에 DtoGL을 해버리면 원점에서 회전해야 하는에 이동된 중신점을 기준으로 회전해서 문제가 생긴다.
    //left, top
    m_TankBody[0] =  ENAMYRECT / 1.5;
    m_TankBody[2] = 0.00001f;  
    m_TankBody[1] =  ENAMYRECT ;

    
    //left,bottom
    m_TankBody[3] =  ENAMYRECT / 1.5;
    m_TankBody[5] = 0.00001f;  
    m_TankBody[4] = - ENAMYRECT / 1.3;

    
    //right,bottom
    m_TankBody[6] = - ENAMYRECT / 1.5;
    m_TankBody[8] = 0.00001f;
    m_TankBody[7] = - ENAMYRECT / 1.3;

    
    //right,top
    m_TankBody[9] = - ENAMYRECT / 1.5;
    m_TankBody[11] = 0.00001f; 
    m_TankBody[10] = ENAMYRECT; 

    float fAngle  = -90 * PI / 180.f;
    int nIndex0 = 0;
    float x = 0;
    float y = 0;
    for (int i = 0; i < 4; i++)
    {
        nIndex0 = i * 3;
        
        //회전공식
        x = m_TankBody[nIndex0] * cosf(fAngle) - m_TankBody[nIndex0 + 1] * sinf(fAngle);
        y = m_TankBody[nIndex0] * sinf(fAngle) + m_TankBody[nIndex0 + 1] * cosf(fAngle);
        m_TankBody[nIndex0] = x;
        m_TankBody[nIndex0 + 1] = y;
    }
//---------------------------------------------------------------------------------------------- 
    
    
//----------------------------------------------------------------------------------------------    
    //회전하기때문에 DtoGL을 해버리면 원점에서 회전해야 하는에 이동된 중신점을 기준으로 회전해서 문제가 생긴다.
    //left top
    m_TankHeader[0] =  ENAMYRECT / 3;
    m_TankHeader[2] = 0.00001f;  
    m_TankHeader[1] =  ENAMYRECT * 1.5;
    
    
    //left,bottom
    m_TankHeader[3] =  ENAMYRECT / 3;
    m_TankHeader[5] = 0.00001f;  
    m_TankHeader[4] = 0;
    
    
    //right,bottom
    m_TankHeader[6] = - ENAMYRECT / 3;
    m_TankHeader[8] = 0.00001f;  
    m_TankHeader[7] = 0;    
    
    //right,top
    m_TankHeader[9] = - ENAMYRECT / 3;
    m_TankHeader[11] = 0.00001f; 
    m_TankHeader[10] = ENAMYRECT * 1.5; 
    for (int i = 0; i < 4; i++)
    {
        nIndex0 = i * 3;
        
        //회전공식
        x = m_TankHeader[nIndex0] * cosf(fAngle) - m_TankHeader[nIndex0 + 1] * sinf(fAngle);
        y = m_TankHeader[nIndex0] * sinf(fAngle) + m_TankHeader[nIndex0 + 1] * cosf(fAngle);
        m_TankHeader[nIndex0] = x;
        m_TankHeader[nIndex0 + 1] = y;
    }
//---------------------------------------------------------------------------------------------- 
   
    
    
//----------------------------------------------------------------------------------------------    
    g_EnamyTankVertex[0] =   ENAMYRECT;
    g_EnamyTankVertex[2] = 0.00005f;  //배경을 좀더 뒤에서 그린다.
    g_EnamyTankVertex[1] =   ENAMYRECT;
    DtoGL(g_EnamyTankVertex[0], g_EnamyTankVertex[1], &g_EnamyTankVertex[0],&g_EnamyTankVertex[1]);
    
    //left,bottom
    g_EnamyTankVertex[3] =   ENAMYRECT;
    g_EnamyTankVertex[5] = 0.00005f;  //배경을 좀더 뒤에서 그린다.
    g_EnamyTankVertex[4] = - ENAMYRECT;
    DtoGL(g_EnamyTankVertex[3], g_EnamyTankVertex[4], &g_EnamyTankVertex[3],&g_EnamyTankVertex[4]);
    
    //right,bottom
    g_EnamyTankVertex[6] = - ENAMYRECT;
    g_EnamyTankVertex[8] = 0.00005f;  //배경을 좀더 뒤에서 그린다.
    g_EnamyTankVertex[7] = - ENAMYRECT;
    DtoGL(g_EnamyTankVertex[6], g_EnamyTankVertex[7], &g_EnamyTankVertex[6],&g_EnamyTankVertex[7]);
    
    //right,top
    g_EnamyTankVertex[9] = - ENAMYRECT;
    g_EnamyTankVertex[11] = 0.00005f; //배경을 좀더 뒤에서 그린다.
    g_EnamyTankVertex[10] = ENAMYRECT; 
    DtoGL(g_EnamyTankVertex[9], g_EnamyTankVertex[10], &g_EnamyTankVertex[9],&g_EnamyTankVertex[10]);
//---------------------------------------------------------------------------------------------- 
    
    
//----------------------------------------------------------------------------------------------    
    g_BombVertex[0] =  - BOMBRECT;
    g_BombVertex[2] = 0.00005f;  //배경을 좀더 뒤에서 그린다.
    g_BombVertex[1] =  - BOMBRECT;
    DtoGL(g_BombVertex[0], g_BombVertex[1], &g_BombVertex[0],&g_BombVertex[1]);
    
    //left,bottom
    g_BombVertex[3] =  - BOMBRECT;
    g_BombVertex[5] = 0.00005f;  //배경을 좀더 뒤에서 그린다.
    g_BombVertex[4] = BOMBRECT;
    DtoGL(g_BombVertex[3], g_BombVertex[4], &g_BombVertex[3],&g_BombVertex[4]);
    
    //right,bottom
    g_BombVertex[6] = BOMBRECT;
    g_BombVertex[8] = 0.00005f;  //배경을 좀더 뒤에서 그린다.
    g_BombVertex[7] = BOMBRECT;    
    DtoGL(g_BombVertex[6], g_BombVertex[7], &g_BombVertex[6],&g_BombVertex[7]);
    
    //right,top
    g_BombVertex[9] = BOMBRECT;
    g_BombVertex[11] = 0.00005f; //배경을 좀더 뒤에서 그린다.
    g_BombVertex[10] = - BOMBRECT; 
    DtoGL(g_BombVertex[9], g_BombVertex[10], &g_BombVertex[9],&g_BombVertex[10]);
//----------------------------------------------------------------------------------------------
    
    
    
    
//----------------------------------------------------------------------------------------------
//회전하기때문에 DtoGL을 해버리면 원점에서 회전해야 하는에 이동된 중신점을 기준으로 회전해서 문제가 생긴다.
    //left, top
    m_Arrow[0] =  ENAMYRECT;
    m_Arrow[2] = 0.00009f;
    m_Arrow[1] =  ENAMYRECT * 3.5f;
    
    
    //left,bottom
    m_Arrow[3] =  ENAMYRECT;
    m_Arrow[5] = 0.00009f;
    m_Arrow[4] =  ENAMYRECT * 1.5f;
    
    
    //right,bottom
    m_Arrow[6] = - ENAMYRECT;
    m_Arrow[8] = 0.00009f;
    m_Arrow[7] = ENAMYRECT * 1.5f;
    
    
    //right,top
    m_Arrow[9] = - ENAMYRECT;
    m_Arrow[11] = 0.00009f;
    m_Arrow[10] = ENAMYRECT * 3.5f;
//----------------------------------------------------------------------------------------------
    
        
    
    m_SticIndices[0] = 0;
    m_SticIndices[1] = 1;
    m_SticIndices[2] = 3;
    m_SticIndices[3] = 2;
    
    

    
    m_SticCoordTex[0] = 0.0f;
    m_SticCoordTex[1] = 1.0f;
    
    m_SticCoordTex[2] = 0.0f;
    m_SticCoordTex[3] = 0.0f;
    
    m_SticCoordTex[4] = 1.0f;
    m_SticCoordTex[5] = 0.0f;
    
    m_SticCoordTex[6] = 1.0f;
    m_SticCoordTex[7] = 1.0f;
    
    m_SticIndicesCnt = sizeof(m_SticIndices) /sizeof(unsigned short);
    return E_SUCCESS;
}






void C2dRadaBoard::MakeWorldStickVertexRotGL(GLfloat *pWorldVertex,GLfloat *pOrgVertex,float fAngle,float xPos,float yPos,int nType)
{
    float x,z;
    int nIndex0 = 0;
    
    if(nType == 1) //해더
    {
        if(m_fBeforeTAngled == fAngle) return;
        m_fBeforeTAngled = fAngle;
        fAngle  = fAngle * PI / 180;
    }
    else if(nType == 0) //바디
    {
        if(m_fBeforeTPoAngled == fAngle) return;
        m_fBeforeTPoAngled = fAngle;
        fAngle  = fAngle * PI / 180;
    }
    else //
    {
        if(m_fBeforeTArrow == fAngle) return;
        m_fBeforeTArrow = fAngle;
        fAngle  = fAngle * PI / 180;
    }
        
        
    for (int i = 0; i < 4; i++) 
    {
        nIndex0 = i * 3;
        
        //회전공식
        x = pOrgVertex[nIndex0] * cosf(fAngle) - pOrgVertex[nIndex0 + 1] * sinf(fAngle);
        z = pOrgVertex[nIndex0] * sinf(fAngle) + pOrgVertex[nIndex0 + 1] * cosf(fAngle);
        DtoGL(x, z,&x,&z);
        
        pWorldVertex[nIndex0]      =       xPos - x    - (xmax_3d * CAMERA_YPOS_2D);
        pWorldVertex[nIndex0 + 1]  =       yPos - z    + (ymax_3d * CAMERA_YPOS_2D);
        pWorldVertex[nIndex0 + 2]  =       pOrgVertex[nIndex0 + 2];
    }  

}


bool C2dRadaBoard::IsRadaCenter(float x, float y)
{
    if(x >= m_BoardVertex[6] &&  x <= m_BoardVertex[0] && y >= m_BoardVertex[1] &&  y <= m_BoardVertex[4] && mbHide == false)
    {
        return true;
    }
    return false;
}


bool C2dRadaBoard::BeginTouch(long lTouchID,float x, float y)
{
    DtoGL(x, y, &x,&y);
    if(IsRadaCenter(x,y) && mbHide == false)
    {
        
#if CGDisplayMode == 1
        if(gnCGDiplayMode == 0)
            gnCGDiplayMode = 1;
        else if(gnCGDiplayMode == 1)
            gnCGDiplayMode = 2;
        else
            gnCGDiplayMode = 0;
#endif
        
        mlTouch = lTouchID;
        return false;

    }
    return true;
}


bool C2dRadaBoard::MoveTouch(long lTouchID,float x, float y)
{
    return true;
}


bool C2dRadaBoard::EndTouch(long lTouchID,float x, float y)
{
    int nActorTeamID = 0;
    if(mlTouch != -1 && mlTouch == lTouchID && mbHide == false)
    { 
        DtoGL(x, y, &x,&y);
        if(IsRadaCenter(x,y))
        {
            float fTempX,fTempY;
            float fInterActorX,fInterActorZ;
            CSprite* pSprite = NULL;
            SPoint   ptSprite;
            SPRITE_STATE spState;
            CListNode<CSprite>* lstSprite = ((CHWorld*)mpWorld)->GetSpriteList();
//            float fXMaxMapRadian = 0.8; //BOARD_CIRCLE_RADIAN_INTERVAL

//            CControl::DtoGL(mfBOARD_CIRCLE_RADIAN_INTERVAL*mnBOARD_CIRCLE_COUNT, &fXMaxMapRadian);
            
            CSprite* pActorSprite = mpWorld->GetActor();
            SPoint ptActor;
            pActorSprite->GetPosition(&ptActor);            
            nActorTeamID = pActorSprite->GetTeamID();
            
            float WorldVertex[12];
            //int nSCnt = lstSprite->Size();
            //for(int i = nSCnt - 1; i >= 0 ; i--)
            for( ByNode<CSprite>* it = lstSprite->begin(); it != lstSprite->end();)
            {
                
                pSprite = it->GetObject();
                it = it->GetNextNode();
                
                spState = pSprite->GetState();
                pSprite->GetPosition(&ptSprite);
                
                if(spState != SPRITE_RUN)
                {
                    continue;
                }
                
                if(pSprite == pActorSprite) continue;
                
                
                //액터를 중심 좌표료 만든다.
                //VISIBLEFARVIEW
                fInterActorX = - ptSprite.x + ptActor.x;
                fInterActorZ = ptSprite.z - ptActor.z;
                
                //레이더 데이터를 만든다.
                //RADAVISIBLE : fXMaxMapRadian 비율
                
                fTempX = fInterActorX * mfMaxRadian / RADAVISIBLE + C2dRadaBoard::gBoadCenter.x;
                fTempY = fInterActorZ * mfMaxRadian / RADAVISIBLE + C2dRadaBoard::gBoadCenter.y;
                CControl::MakeWorldStickVertexGL(WorldVertex,
                                                  C2dRadaBoard::g_EnamyTankVertex,
                                                  fTempX,
                                                  fTempY);
                
                //IPhone보다 작은 것은 너무 세밀해서 1배크게 하여 클릭 효율을 높히자.
                float fdx = (WorldVertex[9] - WorldVertex[0]) / 2.f;
                float fdy = (WorldVertex[1] - WorldVertex[4]) / 2.f;
                
                if(x >= (WorldVertex[0] - fdx) &&  x <= (WorldVertex[9] + fdx) && y >= (WorldVertex[4] - fdy) &&  y <= (WorldVertex[1] + fdy) )
                {
                    
                    
//                    nModelID = pSprite->GetModelID();
//                    if(nModelID >= ACTORID_BLZ_DTOWER && nModelID < ACTORID_BLZ_END
//                       )
                    if(nActorTeamID == pSprite->GetTeamID())
                    {
                         CScenario::SendMessage(SGL_SELECTED_SPRITE,0,0,0,(long)pSprite); //안드로이드는 터지시 텍스쳐바운딩하면 에러가 나기때문에 메세지를 보내서 처리하자.
                        break;
                    }
                    else //if(nModelID != ACTORID_BLZ_MAINTOWER)
                    {
                    
                        SGLEvent Event;
                        Event.lParam = 0;
                        Event.lParam2 = 0;
                        Event.nMsgID = GSL_DONT_AUTO_FIRE;
                        Event.lObject = 0;
                        mpWorld->OnEvent(&Event);

                        CScenario::SendMessage(SGL_MESSAGE_AUTO_ATTACK_TARGET,pSprite->GetID(),0,0,0);
                    }
                    break;
                }
            }
            
        }
        mlTouch = -1;
        return false;
        
    }
    return true;
}



int C2dRadaBoard::RenderBegin(int nTime)
{
    CK9Sprite* pActor = (CK9Sprite*)mpWorld->GetActor();
    if(pActor == 0) return E_SUCCESS;
    if(pActor->IsGhost()) return E_SUCCESS;
    
    float fAngle = - pActor->GetBodyAngle();
    float fPoAngle = - pActor->GetHeaderPoAngle();
    
    SPoint ptActor,ptTarget;
    pActor->GetPosition(&ptActor);
    

    
    //탱크를 회전한다
    MakeWorldStickVertexRotGL(m_WorldTankBody,m_TankBody,  fAngle ,gBoadCenter.x,gBoadCenter.y,1);
    MakeWorldStickVertexRotGL(m_WorldTankHeader,m_TankHeader,  fAngle + fPoAngle ,gBoadCenter.x,gBoadCenter.y,0);
    
    
    if(mpDMainTower && mpDMainTower->GetState() == SPRITE_RUN)
    {
        mpDMainTower->GetPosition(&ptTarget);
        float fAngleArrow = -AngleArrangeEx(sglNowToPosAngle(&ptActor,&ptTarget)) - 90;
        MakeWorldStickVertexRotGL(m_WorldArrow,m_Arrow,  fAngleArrow ,gBoadCenter.x,gBoadCenter.y,2);
    }
    else
        mpDMainTower = NULL;
    
    SGLCAMERA* cam = pActor->GetCamera();
    mfAngleCamera = atan2(cam->viewDir.z,-cam->viewDir.x);
    
    return E_SUCCESS;
}

int C2dRadaBoard::Render()
{

    if(pmObjects == NULL) return E_SUCCESS;
    if(mbHide) return E_SUCCESS;

    glColor4f(1.0f,1.0f,1.0f,0.3f);
    
    glBindTexture(GL_TEXTURE_2D, mglTexRadaPannel);
    glTexCoordPointer(2, GL_FLOAT, 0, m_SticCoordTex);
    glVertexPointer(3, GL_FLOAT, 0, m_BoardVertex);
    glDrawElements(GL_TRIANGLE_STRIP, m_SticIndicesCnt, GL_UNSIGNED_SHORT, m_SticIndices);
    
    glColor4f(1.0f,1.0f,1.0f,1.0f);
    
    glBindTexture(GL_TEXTURE_2D, mglTexTBodyID);
    glTexCoordPointer(2, GL_FLOAT, 0, m_SticCoordTex);
    glVertexPointer(3, GL_FLOAT, 0, m_WorldTankBody);
    glDrawElements(GL_TRIANGLE_STRIP, m_SticIndicesCnt, GL_UNSIGNED_SHORT, m_SticIndices);
    
    glBindTexture(GL_TEXTURE_2D, mglTexTHeaderID);
    glTexCoordPointer(2, GL_FLOAT, 0, m_SticCoordTex);
    glVertexPointer(3, GL_FLOAT, 0, m_WorldTankHeader);
    glDrawElements(GL_TRIANGLE_STRIP, m_SticIndicesCnt, GL_UNSIGNED_SHORT, m_SticIndices);
    
    if(mpDMainTower && mpDMainTower->GetState() == SPRITE_RUN)
    {
        glBindTexture(GL_TEXTURE_2D, mglTexArrow);
        glTexCoordPointer(2, GL_FLOAT, 0, m_SticCoordTex);
        glVertexPointer(3, GL_FLOAT, 0, m_WorldArrow);
        glDrawElements(GL_TRIANGLE_STRIP, m_SticIndicesCnt, GL_UNSIGNED_SHORT, m_SticIndices);
    }

    
    glColor4f(1.0f,1.0f,1.0f,0.5f);
    
    
    
    //스프라이트의 각 위치를 그려준다.
    //int nCnt = pmObjects->Size();
    //for (int i = 0; i < nCnt; i++)
    for(ByNode<RadaBoardObject>* it = pmObjects->begin(); it != pmObjects->end();)
    {
        
        RadaBoardObject* pObject = it->GetObject();
        DrawEnemyTank(pObject);
        it = it->GetNextNode();
    }
 
    
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);
    
    //레이더 원을 그린다.
    glColor4f(0.5f,0.5f,1.0f,1.0f);
    for(int j = 0; j < mnBOARD_CIRCLE_COUNT; j++)
    { 
        glVertexPointer(3, GL_FLOAT, 0, mWorldPoints[j]);
        glDrawArrays(GL_LINE_LOOP, 0, mnBOARD_CIRCLE_LINES);	
    }
    
    if(mpDMainTower && mpDMainTower->GetState() == SPRITE_RUN)
    {
        glColor4f(0.8f,0.2f,0.0f,1.0f);
        
        //카메라가 바라보는 라인을 그린다.
        GLfloat points[3*2];
        points[0] = gBoadCenter.x;
        points[1] = gBoadCenter.y;
        points[2] = gBoadCenter.z;
        points[3] = gBoadCenter.x + mfMaxRadian * cosf(mfAngleCamera);
        points[4] = gBoadCenter.y + mfMaxRadian * sinf(mfAngleCamera);
        points[5] = 0.0f;
        glVertexPointer(3, GL_FLOAT, 0, points);
        glDrawArrays(GL_LINES, 0, 2);
        
    }

    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnable(GL_TEXTURE_2D);
    return E_SUCCESS;
}


int C2dRadaBoard::RenderEnd()
{
    pmObjects = NULL;
    return E_SUCCESS;
}

int C2dRadaBoard::OnEvent(SGLEvent *pEvent)
{
    return E_SUCCESS;
}


void C2dRadaBoard::SetBoadPosition(int nX,int nY)
{
    GLfloat fBoardCenter[3];
    GLfloat matrix[16];
    GLfloat fX,fY;
    CControl::DtoGL(nX, nY, &fX, &fY);
    
    if(gBoadCenter.x == fX && gBoadCenter.y == fY) return;
    
    gBoadCenter.x = fX;
    gBoadCenter.y = fY;
    
    fBoardCenter[0] = gBoadCenter.x;
    fBoardCenter[1] = gBoadCenter.y;
    fBoardCenter[2] = 0.0f;
    
    sglLoadIdentityf(matrix);
    sglTranslateMatrixf(matrix, fBoardCenter[0], fBoardCenter[1], fBoardCenter[2]);
    for(int j = 0; j < mnBOARD_CIRCLE_COUNT; j++)
    {
        for (int i = 0; i < mnBOARD_CIRCLE_LINES; i++)
        {

            sglMultMatrixVectorf(&mWorldPoints[j][i*3], matrix, &mPoints[j][i*3]);
        }
    }    
}

void C2dRadaBoard::makecirclef()
{
    GLfloat fRadian;
    CControl::DtoGL(mfBOARD_CIRCLE_RADIAN_INTERVAL, &fRadian);
    for(int j = 0; j < mnBOARD_CIRCLE_COUNT; j++)
    {
        for (int i = 0; i < mnBOARD_CIRCLE_LINES; i++)
        {
			mPoints[j][i*3] = fRadian*(1+j)*(GLfloat)cosf(2.0f*PI*i/mnBOARD_CIRCLE_LINES);
            mPoints[j][i*3+1] = fRadian*(1+j)*(GLfloat)sinf(2.0f*PI*i/mnBOARD_CIRCLE_LINES);
			mPoints[j][i*3+2] = 0.0f;		
        }
    }
}

void C2dRadaBoard::DrawEnemyTank(RadaBoardObject * pObject)
{ 
    if(pObject->nBoardType == 1)
        glBindTexture(GL_TEXTURE_2D, mglTexEnamyTankID);
    else if(pObject->nBoardType == 3)
        glBindTexture(GL_TEXTURE_2D, mglTexEnmayMainID);
    else if(pObject->nBoardType == 4)
        glBindTexture(GL_TEXTURE_2D, mglTexImpactPoint);
    else if(pObject->nBoardType == 2)
        glBindTexture(GL_TEXTURE_2D, mglTexRadaFireID);
    else if(pObject->nBoardType == 6)
        glBindTexture(GL_TEXTURE_2D, mglTexEnamyFighterID);
    else if(pObject->nBoardType == 7)
        glBindTexture(GL_TEXTURE_2D, mglTexMyTeamTankID);
    else //5
        glBindTexture(GL_TEXTURE_2D, mglTexDTower);
    
    glTexCoordPointer(2, GL_FLOAT, 0, m_SticCoordTex);
    glVertexPointer(3, GL_FLOAT, 0, pObject->pWorldVertex);
    
    glDrawElements(GL_TRIANGLE_STRIP, m_SticIndicesCnt, GL_UNSIGNED_SHORT, m_SticIndices);
}


#ifdef ANDROID
void C2dRadaBoard::ResetTexture()
{

    CControl::ResetTexture();
    
    CTextureMan *pMan   = mpWorld->GetTextureMan();
    mglTexEnmayMainID   = pMan->GetTextureID(SGL_2D_FILENAME_ENAMY_MAIN);
    mglTexEnamyTankID   = pMan->GetTextureID(SGL_2D_FILENAME_ENAMY);
    mglTexRadaFireID    = pMan->GetTextureID(SGL_2D_FILENAME_RADAFIRE);
    mglTexTBodyID       = pMan->GetTextureID(SGL_2D_FILENAME_RADATBODY);
    mglTexTHeaderID     = pMan->GetTextureID(SGL_2D_FILENAME_RADATHEADER);
    mglTexRadaPannel    = pMan->GetTextureID(SGL_2D_FILENAME_RADAPANNEL);
    mglTexImpactPoint   = pMan->GetTextureID(SGL_2D_FILENAME_TARGET);
    mglTexEnamyFighterID = pMan->GetTextTextureID(SGL_2D_FILENAME_ENAMYFIGTHER);
    mglTexDTower        = pMan->GetTextureID("RaidaBlzIcon.tga");
    mglTexArrow = pMan->GetTextureID("Arrow.tga");
    mglTexMyTeamTankID = pMan->GetTextureID("RadiaMyTeamTank.tga");

}
#endif