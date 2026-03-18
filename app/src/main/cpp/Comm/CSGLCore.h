//
//  CSGLCore.h
//  SongGL
//
//  Created by Songs on 11. 5. 13..
//  Copyright 2011 thinkm. All rights reserved.
//
#ifndef _CSGLCORE_H
#define _CSGLCORE_H

#include <pthread.h>
#include <stdio.h>
#include <map>
#include "sGLDefine.h"
#include "sGL.h"
#include "ListNode.h"
#include "CSprite.h"
#include "CBomb.h"
#include "AParticle.h"
#include "CModelMan.h"
#include "CSglTerrian.h"
#include "CMS3DModelASCIIMan.h"
//땅의 기본 위치 즉 수평선의 높이 땅의 기본높이
#define H_GTYPE_BUILD_NORMAL        160         //일반건물
#define H_GTYPE_OTHER_OBJECTS       220
#define H_GTYPE_FLATGROUNT_NORMAL   80          //평평한 땅의 지형
#define H_GTYPE_UPDOWNGROUNT_NORMAL 81          //울툴불퉁한 땅의 지형

#define MAX_ARRAY_VISIBLE_TERRIAN_CNT 180
#define MAX_VISIBLE_TERRIAN_CNT 160

typedef struct _STARTPOINT
{
    float fPosX;
    float fPosY;
    float fPosZ;
    
    float fDirX;
    float fDirY;
    float fDirZ;
    
    int   nModelID;
    int   nTeamID;
}STARTPOINT;

extern int gnIndicesCnt;
extern GLushort *gpIndicesBuffer;


class CSoundMan;
class CTextureMan;
class CItemMan;
class CSglTerrian;
class CSGLController;
class CWorld;
class CPicking;
class CMS3DASCIIMan;
class CSGLCore : public ARender
{
public:
    CSGLCore(CWorld* pWorld,bool bGLMake = false);
    virtual ~CSGLCore();
    
    virtual int Initialize(SPoint *pPosition,SVector *pvDirection) { return E_SUCCESS;}
    virtual int OnEvent(SGLEvent *pEvent) {return E_SUCCESS;};
	virtual int RenderBegin(int nTime);
	virtual int Render();
	virtual int RenderEnd();
        
    void Clear();
    void SetActor(CSprite* pActor) { mpActor = pActor;}
    CSprite* GetActor() { return mpActor;}
    void SetPicking(CPicking* v) { mpPicking = v;}
    
    // 기본적으로 .sgl 파일을 만들 정보를 읽어와서 데이터화 한다.
//    int MakeSGL(SGLTerrianInfo* pImage);
    
    //파일이 존재하면 sFileName,존재하지않으면 NULL,imagesize
    int LoadTerrianOrgEx(const char* sFileName,int nWidth = 0,int nHeight = 0);
    int LoadTerrianNextEx(int nGX,int nGY);
    
    
    
    int LoadTerrian(const char* sFileName);

    
    
    int GetPositionY(SPoint	*pPoint,bool bMovableCheck = false,SVector* pOutNormalVector = NULL,int* pOutGIndex = NULL);
    int GetVisibleTerrian(
                          SPoint	*pPoint,
                          SVector	*pvViewDirection,
                          float		fVewVisibleFar,
                          long      *pVisibleTerianMap,
                          int    *pVisibleTerranMapCnt);
     
    
    CTextureMan* GetTextureMan() { return m_pTextureMan;}
    CModelMan* GetModelMan() { return m_pModelMan;}
//    CMS3DModelASCIIMan* GetMS3DModelASCIIMan() { return m_pMS3DModelASCIIMan;}
    CSglTerrian* GetBlockTerrian(int nPos) { return m_pBlockTerrian->GetAt(nPos);}
    
    
    CListNode<CSprite>* GetSpriteList() { return &mlstSprite;}
    CListNode<CBomb>*   GetBombList() { return &mlstBomb;}
    CListNode<AParticle>* GetParticleList() { return &mlstParticle;}
    
    void AddSprite(CSprite* pSprite); 
    void DelSprite(int nIndex);
    void DelSprite(CSprite* pSprite);
    void AddBomb(CBomb* pBomb);
    void AddParticle(AParticle* pPaticle);
    void PlaySystemSound(int nID);
    void PlayVibrate();
    
    IAction* GetAction() { return mpAction;}
    CSprite* FindSprite(int nID);
    
    //땅만검사한다.
    bool IsTabInOnlyTerrian(float xWinPos,float yWinPos,float fCenterPoint[3]);
    
    int IsTabIn(float xWinPos,float yWinPos,int& nOutResult,float fOutPoint[9],long& outSprite);
    static CSprite* MakeSprite(unsigned int nID,unsigned char cTeamID,int nModelID,CTextureMan *pTextureMan,IAction *pAction,IHWorld* pWorld,NETWORK_TEAMTYPE NetworkTeamType);

    
    int GetMapWidth() { return mMapWidthIndex;}
    int GetMapHeight() { return mMapHeightIndex;}

    TerrianMakeInfo*            GetMakeTerrinInfo() { return m_pMakeTerrinInfo;}
    void ClearDesignMakeInfo();
    
#ifdef ANDROID
    virtual void ResetTexture();
#endif
    
    bool VisibleByCameraTerrian(SVector *pdir,SPoint	*pViewPoint,SPoint *ptTarget);
    
protected:
    
    void RenderVisible();
    void RenderTotal();
    void RenderOrg(CSglTerrian* pRec);
    virtual int GetVisibleTerrian(
                                  SPoint	*pViewPoint,
                                    float fVewVisibleFar,
                                    int   nMapGroupIndexX,
                                    int   nMapGroupIndexY,
                                    SVector *pNormalizeDirection,
                                    long  *pVisibleTerianMap,
                                    int    *pVisibleTerranMapCnt);
    int IsTabInVisible(float xWinPos, float yWinPos, int &nOutResult,float fOutPoint[9],long &outSprite);
    int IsTabInTotal(float xWinPos, float yWinPos, int &nOutResult,float fOutPoint[9],long &outSprite);
    int IsTabInOrg(CSglTerrian *pTerrian, SPoint& nearPoint,SPoint& farPoint,float fOutPoint[9]);
    bool IsTabInOrgOnlyTerrian(CSglTerrian *pTerrian, SPoint& nearPoint,SPoint& farPoint,float fOutPoint[9]);
    void RenderBlock(vector<CMS3DASCII*>* pTerrian,float fTerrianShadow);
    
protected:
    CListNode<CSglTerrian>      *m_pBlockTerrian;
    CTextureMan                 *m_pTextureMan;
    CSoundMan                   *m_pSoundMan;
    CModelMan                   *m_pModelMan;
    
    
    long                        m_VisibleBlockTerrian[MAX_ARRAY_VISIBLE_TERRIAN_CNT]; //Visible Max 200
    int                         m_VisibleBlockTerrianCnt;
    
    CSprite                     *mpActor;  //현재 주인공이 땅을 바라보는 주인공이 누구인가.
    IAction                     *mpAction;
    
    CListNode<CSprite>          mlstSprite;
	CListNode<CBomb>            mlstBomb;
    CListNode<AParticle>        mlstParticle;
    map<int, CSprite*>          mHashSprite;
    
    CWorld*                     mpWorld;

    bool                        mbGLMake; //SongGLMake이면 true , 아니면 false
    CPicking                    *mpPicking;    
    
    int                         mMapWidthIndex; //맵의 가로 
    int                         mMapHeightIndex;//맵의 세로 
    
    
    TerrianMakeInfo*            m_pMakeTerrinInfo; // 디자인 툴에서만 사용하고 실제 게임에서는 읽어들일때만 사용하고 메모리에서 지운다.
    CMS3DASCIIMan*              m_pAsciiModelMan; // 디자인 툴에서만 사용하고 실제 게임에서는 읽어들일때만 사용하고 메모리에서 지운다.
//    CMS3DModelASCIIMan*         m_pMS3DModelASCIIMan; //아이템메니져.
    
public:
    static int                  m_gTerrianSize; //이미지의 크기는 땅의 Width = Height = m_gTerrianSize
    char*                       msTerrianFilePath;
    
    vector<STARTPOINT*>         mlstStartPoint;     //게임의 시작지점을 기억한다.
    vector<STARTPOINT*>         mlstAddedSprite;    //게임에 추가된 스프라이트리스트.
    CItemMan                    *mpItemMan;
    
    
    GLfloat                 m_fFogColor[4];
    GLfloat                 m_fFogStart;
    GLfloat                 m_fFogEnd;
    GLfloat                 m_fFogDentity;
    GLfloat                 m_fTerrianShadow;
    
    SPoint mBeforePT; //View위치가 같으면 계산을 하지 말자.
    pthread_mutex_t          mHashMutex;

    static unsigned int      mnEnemyMainTowerMaxID;
};


#endif //_CSGLCORE_H