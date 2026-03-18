//
//  CSglModel.h
//  SongGL
//
//  Created by Songs on 11. 5. 13..
//  Copyright 2011 thinkm. All rights reserved.
//
#ifndef _CSglModel_H
#define _CSglModel_H

#include <stdio.h>
#include <string.h>

#include "sGL.h"
#include "CMS3DASCII.h"
#include "ListNode.h"
#include "CWorld.h"
#include "TerrianMakeType.h"
#include <vector>
using namespace std;

class CSGLCore;
class CTextureMan;
class CMS3DASCIIMan;
class CSglTerrian : public CMS3DASCII
{
    friend class CSGLCoreEx;
    friend class CSGLCore;
    friend class CAIController;
    
public:
    CSglTerrian(CWorld* pWorld,CTextureMan *pTextureMan);
    virtual ~CSglTerrian();

    //MakeInfo정보를 읽어들인다.
    static int              LoadMakeMeshInfo(FILE* fp,TerrianMakeMeshInfo* pMeshInfo,unsigned char nVersion,bool bOther = false);
    static TerrianMakeInfo* LoadMakeInfo(const char* sFileName);
    static int              SaveMakeMeshInfo(FILE* fp,TerrianMakeMeshInfo* pMeshInfo);
    static int              SaveMakeInfo(const char* sFileName,TerrianMakeInfo* pMakeInfo);
    static void             ClearMakeInfo(TerrianMakeInfo* pMakeInfo); //메모리를 제거한다.
    static TerrianMakeInfo* DefaultMakeInf(int nWidth,int nHeight);
    static void ClearMakeMeshInfoList(vector<TerrianMakeMeshInfo*>& MeshInfoList);
    static void ClearMakeMeshInfo(TerrianMakeMeshInfo* MeshInfo);
    static TerrianMakeBlockInfo* CloneMakeBlockInfo(TerrianMakeBlockInfo* BlockInfo);
    
    

    int LoadASCII(CMS3DASCIIMan* pAsciiMan,TerrianMakeMeshInfo* pMakeInfo,int nChangeIndex = -1);
    
    //데이터가 없다. 0x01:ground, 0x02:Object, 0x04:TranObj
    unsigned int GetBlockType() { return m_nBlockType;}
//    void SetOrBlockType(unsigned int v) { m_nBlockType |= v;}
    void SetBlockType(unsigned int v) { m_nBlockType = v;}
    
    void ArrangeLoadedASCII(CSGLCore* pSglCore,int nIndex,TerrianMakeMeshInfo *pMakeMeshInfo,float fTerrianShadow);
    
    
    vector<CMS3DASCII*>* GetListTerrian() { return &m_TerGround; }
    vector<CMS3DASCII*>* GetListNorObjects() { return &m_TerObject;}
    vector<CMS3DASCII*>* GetListNorTransObjects() { return &m_TerTranObject;} 
    
    void ClearListTerrian(vector<CMS3DASCII*>  &lstTerrian);
    

    //--------------------------------------------------------------------
    //필요없는 지형의 버퍼를 지운다.
    void ArrangeClearBuffer();
    //--------------------------------------------------------------------
    
    //int GetPointZ(SPoint	*pPoint,float *fResult);
    
     
    //평평한지형
    void SetFlat(bool bFlat) { m_bFlat = bFlat;}
    bool GetFlat() { return m_bFlat;}
    
    //건물을 세울 기준 높이
    void SetBlockCenter(SPoint *ptBlockCenter) { memcpy(&m_ptBlockCenter,ptBlockCenter,sizeof(SPoint));}
    SPoint* GetBlockCenter() { return &m_ptBlockCenter;}
    
    SPoint* GetCompacBlzLeftTop() { return &m_ptCompacBlzLeftTop;}
    SPoint* GetCompacBlzRightBottom() { return &m_ptCompacBlzRightBottom;}
    
    void SetHeightVertexCnt(int v) { m_nHeightVertexCnt = v;}
    int GetHeightVertexCnt() { return m_nHeightVertexCnt;}
    
    void SetHeightVertex(GLfloat* v) { m_pHeightVertex = v;}
    GLfloat* GetHeightVertex() { return m_pHeightVertex;}
    
    void SetHeightNormalCnt(int v) { m_nHeightNormalCnt = v;}
    int GetHeightNormalCnt() { return m_nHeightNormalCnt;}
    
    void SetHeightNormal(GLfloat* v) { m_pHeightNormal = v;}
    GLfloat* GetHeightNormal() { return m_pHeightNormal;}
    GLfloat GetHeightMax() { return m_fHeightMax;}

    void SetClear();
    
    void SetFilePath(char* sFilePath);
    const char* GetFilePath();
    
    static int GetWorldToMapIndex(SPoint* pPoint, int *pnMapGroupIndexX,int *pnMapGroupIndexZ,int nDirection = 0);
    static void GetIndexToCenterPoint(int nIndex,SPoint* pOutPoint);
    
    
    
    //Start A-Star ---------------------------
    static vector<int>* FindPath(CSprite* pFromSprite,SPoint* ptFrom,SPoint* ptTo,void* pTiling = 0);
    static void SetObstacleInPath(SPoint* ptNow);
    static void SetObstacleInPath(int nIndex,bool v);
    static bool IsObstacleInPath(int nIndex);
    
    //현재지점이 장애물이면 장애물이 아닌 인덱스를 찾아준다.
    //ptNow  현재 위치
    //ptOutNoneObstacle 장애물지형을 벗어난 지역의 포인트
    //nType 내부적으로 사용함.
    //return GroupIndex이다.
    static int FindNoneObstacle(CSprite* pSprite,SPoint* ptNow,SPoint* ptOutNoneObstacle,int nType = 0,int nOrgIndex = 9999);
    static bool IsObstacle(SPoint* ptNow);
    static bool IsObstacle(int nIndex);
    
    
    //Tiling
    static void                        *mpLoadTileMap;
    static void                        *mpFindPath;
    static pthread_mutex_t             mFindPathSyncMutex;  //Search함수는 쓰레드에 안전하지 않기 때문에...
    static void LoadMap(char* sMapTitleTxt);
    //End A-Star ---------------------------
    
    
    int          m_DistanceFromNowToVisible;
    int          m_nTotalMapIndex;  //자신이 리스트에 어디에 위치해 있는지 알아 올수 있게..
    
    static bool mbChangedTerrianImgAtDesign; //디자인틀에서 지형의 맵을 변경할때는 기존의 데이터를 Plus하지말고 그냥 울툴불퉁에 맞추자.
    
    
#ifdef ANDROID
    virtual void ResetTexture();
#endif
    
private:
    float GetPointY(SPoint	*pPoint,
                    int nIndexVertexX,
                    int nIndexVertexZ,
                    float fIndexVertexX,
                    float fIndexVertexZ,
                    float fUpDown,
                    int *npResult,
                    SVector* pOutNormalVector = NULL);
  
    static int GetWorldToBlock(SPoint* pPoint,SPoint* pOutMapPoint);
    static int GetDirection(SVector *pvDirection);
    static int GetMapGroupIndexToMapUnit(
                                               int nDirection, 
                                               SPoint* pMapPoint,
                                               int *pnMapGroupIndexX,
                                               int *pnMapGroupIndexZ,
                                               int *pnIndexVertexX,
                                               int *pnIndexVertexZ,
                                               float *pfIndexVertexX,
                                               float *pfIndexVertexZ,
                                               float *pfUpDown);
    
    
    
protected:    
    bool            m_bFlat; //평평함.
    SPoint          m_ptBlockCenter; //x,y Block 의 Center z는 지형및 조형물의 기본 높이
    

    //-------------------------------------------
    //160 일경우 위치 안에 있을 때만 컴팩트 한다.
    SPoint        m_ptCompacBlzLeftTop;
    SPoint        m_ptCompacBlzRightBottom;
    //-------------------------------------------
    

    vector<CMS3DASCII*>        m_TerGround;
    vector<CMS3DASCII*>        m_TerObject;
    vector<CMS3DASCII*>        m_TerTranObject;
    
    //0x0100  Obtacle (절대 못가는 지역)
    //0x0200  Obtacle on Path (패스로 찾기일때만의 장애물)
    unsigned int              m_nBlockType; //데이터가 없다. 0x0001:ground, 0x0002:Object, 0x0004:TranObj
    
    
    //지형위에 움직인다면 지형위 위지를 얻어올때 사용한다.(높이)
    int                     m_nHeightVertexCnt; //Size= m_nHeightVertexCnt * Glfloat * 3
    GLfloat                 *m_pHeightVertex;
    
    int                     m_nHeightNormalCnt; //Size= m_nHeightNormalCnt * Glfloat * 3
    GLfloat                 *m_pHeightNormal;
    
    //건물 블럭인 경우 건물의 최대 높이 (지형 + 건물최대높이)
    //나중에 공중에 포를 쏘앗을 경우 지형의 충돌처리를 한다.
    GLfloat                 m_fHeightMax;
    static CWorld*          gpWorld;
    char*           msFilePath; //CMS3DASCII
    

};
#endif //_CSglModel_H