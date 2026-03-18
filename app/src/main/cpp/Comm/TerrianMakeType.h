//
//  TerrianMakeType.h
//  SongGL
//
//  Created by 송 호학 on 12. 4. 3..
//  Copyright (c) 2012년 __MyCompanyName__. All rights reserved.
//

#ifndef SongGL_TerrianMakeType_h
#define SongGL_TerrianMakeType_h
#include <vector>
using namespace std;
typedef struct 
{
    unsigned char  cType; //cType:0x01(땅) 0x02(건물) 0x04(나무,가로등) +++++++ OtherInfo:일경우 SGL_TEAM1,SGL_TEAM2 정보로 사용
    float fPosition[3];
    float fRotation[3];
    char* sFileName;
    unsigned char   cTextureCnt;              //텍스쳐 리스트
    char** arrTextureFileList;      //텍스쳐 파일내밈 리스트 (sFileName에 포함된 내용을 무시하고 여기에 있는 내용으로 대처한다.)
    int   nReserved; //OtherInfo:일경우 1:StartPoint ID,  2:EMainTower Point ID
}TerrianMakeMeshInfo;

typedef struct 
{  
    unsigned char cCanMoving; //0x00 : 일반 지역 , 0x01 : 지나갈수 없는지역
    vector<TerrianMakeMeshInfo*> Terrian;       
    vector<TerrianMakeMeshInfo*> NorObjects;
    vector<TerrianMakeMeshInfo*> NorTransObjects;
    vector<TerrianMakeMeshInfo*> OtherInfo; //탱크의 시작점이나, 적 메인타워등등의 정보...
}TerrianMakeBlockInfo;

typedef struct {
    char  cVersion;                         //Version 1:
    char  cType;                            //아직 활용 안함
    unsigned short shWidth;                 //nWidth = nHeight가 같기 때문에 정보를 더 넣지 않음.
    int   nReserved;                        //예약됨.
    char* sFileName;                        //테리안의 파일명.
    
    char* sBackgroundTexture;               //백그라운드 화면
    float fBackgroundPosition[3];
    float fBackgroundRotation[3];
    float fFogColor[4];
    float fFogStart;
    float fFogEnd;
    float fFogDentity;
    float fTerrianShadow; //NONE_VALUE
    
    vector<TerrianMakeBlockInfo*>           Blocks;
}TerrianMakeInfo;


#endif
