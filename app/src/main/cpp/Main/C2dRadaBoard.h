//
//  C2dRadaBoard.h
//  SongGL
//
//  Created by Songs on 11. 4. 1..
//  Copyright 2011 thinkm. All rights reserved.
//
#ifndef C2DRADABOARD_H
#define C2DRADABOARD_H
#include "sGLDefine.h"
#include "ListNode.h"
#include "CControl.h"

class CSprite;
typedef struct 
{
    int nBoardType; //1:적탱크 2:폭탄 3:메인타워 4:탁착지점. 5:DTower 6:비행기 7:아군탱크
    float fX;
    float fY;
    float *pWorldVertex;
    CSprite *pSprite;
}RadaBoardObject;


class IHWorld;
class C2dRadaBoard : public CControl
{
public:
    C2dRadaBoard(IHWorld* pWorld);
    virtual ~C2dRadaBoard();
    
    virtual int Initialize(SPoint *pPosition,SVector *pvDirection);
	virtual int RenderBegin(int nTime);
	virtual int RenderEnd();		
	virtual int OnEvent(SGLEvent *pEvent);    
    virtual int Render();
    void SetObjects(CListNode<RadaBoardObject>* pObjects);
    /**
     레이더 보드의 중심 화면좌표이다.
     */
    void SetBoadPosition(int nX,int nY);
    bool IsRadaCenter(float x, float y);
    
    bool BeginTouch(long lTouchID,float x, float y);
    bool MoveTouch(long lTouchID,float x, float y);
    bool EndTouch(long lTouchID,float x, float y);
    
    
    void SetDMainTower(CSprite* pMainTowere) { mpDMainTower = pMainTowere;}
    static float MapRate(bool bReset = false);
    
#ifdef ANDROID
    virtual void ResetTexture();
#endif
    float* GetBoardVertex() { return m_BoardVertex;}    
protected:
    void makecirclef();
    void DrawEnemyTank(RadaBoardObject * pObject);
    void MakeWorldStickVertexRotGL(GLfloat *pWorldVertex,GLfloat *pOrgVertex,float fAngle,float xPos,float yPos,int nType);

    
public:
    //적탱크
    static GLfloat g_EnamyTankVertex[12];
    
    //폭탄
    static GLfloat g_BombVertex[12];
    
    /**
     레이더 보드의 중심 화면좌표이다.
     */
    static SPoint  gBoadCenter;
    
protected:
    
    long mlTouch;
    /**
     레이더 보드의 중심 화면좌표이다.
     */
    //SPoint  mBoadCenter;

    //윈의 레이더 좌표.
    GLfloat **mPoints;
    GLfloat **mWorldPoints;

    //그려질 오브젝트들의 리스트
    CListNode<RadaBoardObject>* pmObjects;
    
    GLuint  mglTexEnmayMainID;
    GLuint  mglTexEnamyTankID;
    GLuint  mglTexRadaFireID;
    GLuint  mglTexTBodyID;
    GLuint  mglTexTHeaderID;
    GLuint  mglTexRadaPannel;
    GLuint  mglTexImpactPoint;
    GLuint  mglTexDTower;
    GLuint  mglTexArrow;
    GLuint  mglTexEnamyFighterID;
    GLuint  mglTexMyTeamTankID;
    
    GLfloat m_BoardVertex[12];

    //Arrow
    GLfloat m_Arrow[12];
    //Arrow
    GLfloat m_WorldArrow[12];
    
    //탱크 바디
    GLfloat m_TankBody[12];
    
    //그려질 탱크 바디
    GLfloat m_WorldTankBody[12];
    
    //탱크 해더
    GLfloat m_TankHeader[12];
    
    //그려질 탱크 해더
    GLfloat m_WorldTankHeader[12];
    
    
    
    
    
    //탱크의 바디의 각을 항상 계산하지말자 .. 속도향상
    GLfloat m_fBeforeTAngled;
    GLfloat m_fBeforeTPoAngled;
    GLfloat m_fBeforeTArrow;
    
    
    short           m_SticIndices[4];
    GLfloat         m_SticCoordTex[8];
    unsigned short  m_SticIndicesCnt;

    float mfBOARD_CENTER_X;
    float mfBOARD_CENTER_Y;
    int mnBOARD_CIRCLE_COUNT;
    int mnBOARD_CIRCLE_LINES;
    float mfBOARD_CIRCLE_RADIAN_INTERVAL;
    
    IHWorld* mpWorld;

    CSprite* mpDMainTower;
    
    
    GLfloat mfMaxRadian;
    GLfloat mfAngleCamera;

};

#endif //C2DRADABOARD_H