//
//  CSimpleMilkShape3DASCII.h
//  SongGL
//
//  Created by 호학 송 on 11. 4. 22..
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#ifndef _CSimpleMilkShape3DASCII_H
#define _CSimpleMilkShape3DASCII_H
#include "sGLDefine.h"
#include "TerrianMakeType.h"


typedef struct _sglMesh
{    
    char    sMeshName[20];          //메쉬 이름
    
    int nVetexCnt;                  //정점의 갯수
    GLfloat *pVetexBuffer;          //정점 버퍼 nVetexCnt * 3 * sizeof(GLFloat)
    GLfloat *pColorBuffer;          //정점의 갯수와 같다. nVetexCnt * 4 * sizeof(GLFloat)
    
    int nNormalCnt;                 //법선 백터의 갯수
    GLfloat *pNormalBuffer;         //법선 벡터 pMesh->shIndicesCnt  * 3 * 3 sizeof(GLFloat)
    
    GLushort shIndicesCnt;          //정점 인덱스의 수
    GLushort *pIndicesBuffer;       //정점 인덱스 갯수 nIndicesCnt * 3(삼각형순서) * sizeof(GLushort) 
    
	GLushort       shCoordCount;    //텍스쳐 맵 인덱스 수
	GLfloat*       pCoordBuffer;    //텍스쳐 맵 버퍼 shCoordCount * 2 * sizeof(GLFloat)
    
    
    //gID의 배열 인덱스이다.
    GLint        nglTextureRef;    //텍스쳐 맵 레퍼번스 (Global 변수에 저장되어 있으며 그것의 인덱스를 말한다.)
    char         uMode;            //0 일반 1리피트
    char         vMode;            //0 일반 1리피트
    GLuint       nglTexutre;       //바인드된 텍스쳐  
    char         sTextureName[40];  //텍스쳐 (이미지 파일명)
    
    
//    float fAmbient[4];
//    float fDiffuse[4];
//    float fSpecular[4];
//    float fEmission[4];
//    float fShininess[4];
}sglMesh;

class CTextureMan;
class CPicking;
class CMS3DASCII
{
    friend CMS3DASCII* Clone();
public:
    CMS3DASCII(CTextureMan *pTextureMan);
    virtual ~CMS3DASCII();
    
    CTextureMan* GetTextureMan() { return m_pTextureMan;}
    
//    밀크세이프 For ASCII 파일을 읽어온다.
//    순수하게 메쉬 데이터만 읽어온다.
    //건물과 땅은 벡터정보를 읽어오지 않음. 지형의 벡터를 사용한다.
    //벡터정보를 읽어오기를 원한다면 bLoadNormalVector를 true로 한다.
    virtual int Load(const char* sFileName,bool bLoadNormalVector = false);
    
    int     GetMeshSize() { return m_nMeshCnt;}
    sglMesh* GetMesh() { return m_psglMesh;}
    bool    IsPicking(CPicking* pPick,SPoint& nearPoint,SPoint& farPoint,float* fOP = NULL);
    //반드시 GroundNormalSea.txt로 그라운드를 만들어야 피킹을 빠르게 판단할수가 있다.
    bool    IsPickingTerrian(CPicking* pPick,SPoint& nearPoint,SPoint& farPoint,float* fOP = NULL);
    
    //데이터를 카피한다.
    CMS3DASCII* Clone(TerrianMakeMeshInfo* pMeshInfo = NULL);
    
#ifdef ANDROID
    void ResetTexture();
#endif
    bool   IsNormalVector() { return mIsNormalVector;}
    
public:
    float mfRadius; //탱크의 바디에서 사용된다. -1일때는 값을 정의 하지 않았음.
    
protected:
//     다음 라인을 찾는다.
    char* FndNextLine(char *pOrg,char* pOutLine);
    
//  모든 데이터를 초기화 한다.
    void Clear(); 
    
    
protected:
    int     m_nMeshCnt;  
    sglMesh *m_psglMesh;
    CTextureMan *m_pTextureMan;
    bool        mIsNormalVector;
};

#endif //_CSimpleMilkShape3DASCII_H