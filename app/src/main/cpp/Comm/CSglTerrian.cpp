//
//  CSglModel.cpp
//  SongGL
//
//  Created by Songs on 11. 5. 13..
//  Copyright 2011 thinkm. All rights reserved.
//
#include "CSGLCore.h"
#include "CSglTerrian.h"
#include "sGLTrasform.h"
#include "CTextureMan.h"
#include "pathfind.h"
#include "CWorld.h"
#include "CMS3DASCIIMan.h"
#include "CMoveAI.h"
#include "CAICore.h"
#include "CHWorld.h"
#include "sGLUtils.h"

//        N                  0
//     NNW ENN              1 15
//    NW     EN            2   14  
//  NWW       EEN         3     13
// W             E       4       12
//  WWS       SEE         5     11
//    SW     SE            6   10
//     SSW SSE              7 9
//        W                  8
#define H_N  0			//북		(0,y > 0)
#define H_NNW 1			//북쪽에 가까운 서쪽 ( x < 0 && z > 0 && z/x < -1)
#define H_NW 2			//북서	(-1,1)
#define H_NWW 3         //서쪽에 가까운 북쪽 ( x < 0 && z > 0 && z/x > -1)

#define H_W 4			//서		(x < 0,0)
#define H_WWS 5			//서쪽에 가까운 남쪽 ( x < 0 && z < 0 && z/x < 1)
#define H_SW 6			//남서   (-1,-1)
#define	H_SSW 7			//남쪽에 가까운 서쪽 ( x < 0 && z < 0 && z/x > 1)

#define H_S 8			//남     (0, y < 0)
#define H_SSE 9			//남쪽에 가까운 동쪽 ( x > 0 && z < 0 && z/x < -1)
#define H_SE 10			//남동    (1,-1)
#define	H_SEE 11		//동쪽에 가까운 남쪽 ( x > 0 && z < 0 && z/x > -1)

#define H_E 12			//동      (x > 0 ,0)
#define H_EEN 13		//동쪽에 가까운 북쪽 (x > 0 && z > 0 && z/x < 1)
#define H_EN 14			//동북     (1,1)
#define H_ENN 15		//북쪽에 가까운 동쪽 (x > 0 && z > 0 && z/x > 1)


using namespace PathFind;
static const long long int s_nodesLimit = 100000000L;

void * CSglTerrian::mpLoadTileMap = 0;
void * CSglTerrian::mpFindPath = 0;
bool   CSglTerrian::mbChangedTerrianImgAtDesign = false;

pthread_mutex_t CSglTerrian::mFindPathSyncMutex;  //Search함수는 쓰레드에 안전하지 않기 때문에...

CWorld*   CSglTerrian::gpWorld = NULL;

CSglTerrian::CSglTerrian(CWorld* pWorld,CTextureMan *pTextureMan) : CMS3DASCII(pTextureMan)
{

    m_bFlat  = true;
    m_ptBlockCenter.x = 0;
//    m_ptBlockCenter.y = H_TERRIAN_DEFAULT_HIGHT;
    m_ptBlockCenter.y = -999.f;
    m_ptBlockCenter.z = 0;
    
    m_nHeightVertexCnt = 0;
    m_pHeightVertex = 0;
    m_nHeightNormalCnt = 0;
    m_pHeightNormal = 0;
    m_fHeightMax = 0;
    m_nBlockType = 0;
    
    mpLoadTileMap = NULL;
    mpFindPath = NULL;
    gpWorld = pWorld;
    msFilePath = NULL;
    m_nTotalMapIndex = 0;
    pthread_mutex_init(&mFindPathSyncMutex, NULL);
    
    m_fHeightMax = -999.0f;
    m_ptCompacBlzLeftTop.x = 99999.0f;
    m_ptCompacBlzLeftTop.z = 99999.0f;
    m_ptCompacBlzLeftTop.y = 0;
    
    m_ptCompacBlzRightBottom.x = -99999.0f;
    m_ptCompacBlzRightBottom.z = -99999.0f;
    m_ptCompacBlzRightBottom.y = 0;
    

}

CSglTerrian::~CSglTerrian()
{
    if(m_pHeightVertex)
    {
        delete[] m_pHeightVertex;
        m_pHeightVertex = 0;
    }
    if(m_pHeightNormal)
    {
        delete[] m_pHeightNormal;
        m_pHeightNormal = 0;
    }
    
    if(msFilePath)
    {
        delete[] msFilePath;
        msFilePath = NULL;
    }
    
    if(mpLoadTileMap)
    {
        delete (Tiling*)mpLoadTileMap;
        mpLoadTileMap = 0;
    }
    
    if(mpFindPath)
    {
        delete (Search*)mpFindPath;
        mpFindPath= 0;
    }
    
    SetClear();

#ifndef ANDROID  //안드로이드 최신 버전에서는 죽어버린다. 2020.12.13 죽는다.
    pthread_mutex_destroy(&mFindPathSyncMutex);
#endif
}

int CSglTerrian::LoadASCII(CMS3DASCIIMan* pAsciiMan,TerrianMakeMeshInfo* pMakeInfo,int nChangeIndex)
{
    int nIndex = -1;
    CMS3DASCII *pModel = NULL;
    SetBlockType(GetBlockType() | pMakeInfo->cType);
    
    if(pMakeInfo->cType == 0x01)
    {
        pModel = pAsciiMan->Load(pMakeInfo->sFileName,pMakeInfo);
        if(nChangeIndex != -1)
        {
            CMS3DASCII* Asc = m_TerGround[nChangeIndex];
            delete Asc;
            m_TerGround[nChangeIndex] = pModel;
            nIndex = nChangeIndex;
        }
        else 
        {
            m_TerGround.push_back(pModel);
            nIndex = m_TerGround.size() - 1;
        }         
    }
    else if(pMakeInfo->cType == 0x02)
    {
        pModel = pAsciiMan->Load(pMakeInfo->sFileName,pMakeInfo);
        
        if(nChangeIndex != -1)
        {
            CMS3DASCII* Asc = m_TerObject[nChangeIndex];
            delete Asc;
            m_TerObject[nChangeIndex] = pModel;
            nIndex = nChangeIndex;
        }
        else
        {
            m_TerObject.push_back(pModel);
            nIndex = m_TerObject.size() - 1;

        }
    }
    else if(pMakeInfo->cType == 0x04)//0x04
    {
        pModel = pAsciiMan->Load(pMakeInfo->sFileName,pMakeInfo);
        
        if(nChangeIndex != -1)
        {
            CMS3DASCII* Asc = m_TerTranObject[nChangeIndex];
            delete Asc;
            m_TerTranObject[nChangeIndex] = pModel;
            nIndex = nChangeIndex;
        }
        else
        {
            pModel = pAsciiMan->Load(pMakeInfo->sFileName,pMakeInfo);
            m_TerTranObject.push_back(pModel);
            nIndex = m_TerTranObject.size() - 1;
        }
    }
    else 
    {
        HLogE("[Error] LoadAcii %d\n",pMakeInfo->cType);
    }
    return nIndex;
}



void CSglTerrian::SetFilePath(char* sFilePath)
{
    if(msFilePath) delete[] msFilePath;
    if(sFilePath == NULL) 
    {
        msFilePath = NULL;
        return;
    }
    msFilePath = new char[strlen(sFilePath) + 1];
    strcpy(msFilePath, sFilePath);
}

const char* CSglTerrian::GetFilePath()
{
    return msFilePath;
}




void CSglTerrian::ClearListTerrian(vector<CMS3DASCII*>  &lstTerrian)
{
    int nCnt = lstTerrian.size();
    for (int i = 0; i < nCnt; i++) {
        delete lstTerrian[i];
    }
    lstTerrian.clear();
}

void CSglTerrian::SetClear()
{
    SetFilePath((char*)NULL);
    
    ClearListTerrian(m_TerGround);
    ClearListTerrian(m_TerObject);
    ClearListTerrian(m_TerTranObject);
    
    m_nBlockType = 0;
    
    
    m_fHeightMax = -999.0f;
    m_ptCompacBlzLeftTop.x = 99999.0f;
    m_ptCompacBlzLeftTop.z = 99999.0f;
    m_ptCompacBlzLeftTop.y = 0;
    
    m_ptCompacBlzRightBottom.x = -99999.0f;
    m_ptCompacBlzRightBottom.z = -99999.0f;
    m_ptCompacBlzRightBottom.y = 0;

}





void CSglTerrian::ArrangeClearBuffer()
{

// 불필요한 메모리를 지우려면 .....
//    int nBlockType = GetBlockType();
//    //불필요한 메모리는 지운다.
//    if(nBlockType == 0)
//    {   
////        //필요 없는 데이터는 지운다.
////        m_nHeightVertexCnt = 0;
////        delete[] m_pHeightVertex;
////        m_pHeightVertex = 0;
////        
////        m_nHeightNormalCnt = 0;
////        delete[] m_pHeightNormal;
////        m_pHeightNormal = 0;
//
//    }
}



void CSglTerrian::ArrangeLoadedASCII(CSGLCore* pSglCore,int nIndex,TerrianMakeMeshInfo *pMakeMeshInfo,float fTerrianShadow)
{
 
    //즉 맵의 가장 마지막 위치에서는 높이를 구해오지 못한다.
    int n512 = (CSGLCore::m_gTerrianSize - 1) / 2;
    
    CMS3DASCII* pModel = NULL;
    float matrix[16];
    float fMaxH = -9999.0f;
    SPoint ptPos;
    float fPrevY = 0.0f;


    if(pMakeMeshInfo->cType == 0x01) //땅
        pModel = m_TerGround[nIndex];
    else if(pMakeMeshInfo->cType == 0x02) //건물
        pModel = m_TerObject[nIndex];
    else
        pModel = m_TerTranObject[nIndex];
    
    //건물은 울퉁불퉁 지형에 무관 하다.
    sglLoadIdentityf(matrix);
    

    //이동을 먼저한후에
    if(CSglTerrian::mbChangedTerrianImgAtDesign)
    {
        if(pMakeMeshInfo->cType == 0x01)
        {
            if(GetFlat() == false) //울퉁불퉁이면 0.0f로 셋팅해주고
                pMakeMeshInfo->fPosition[1] = 0.0f;
            else
            {
                
                //----------------------------------------------
                //아직 높이가 정해지지 않았기 때문에
                pMakeMeshInfo->fPosition[1] = H_TERRIAN_DEFAULT_HIGHT;
            }
        }
        
    }
    sglTranslateMatrixf(matrix,pMakeMeshInfo->fPosition[0],pMakeMeshInfo->fPosition[1],pMakeMeshInfo->fPosition[2]);
    
    //90도 회전 한다.
//    sglRotateRzRyRxMatrixf(matrix,
//                           pMakeMeshInfo->fRotation[0],
//                           pMakeMeshInfo->fRotation[1], 
//                           pMakeMeshInfo->fRotation[2]);
    
    //짐벌락현상 해결함. 즉 x을 먼저 회전하고 그다음 y,z회전하면 정확하게 회전을 할수가 있다.
    GLfloat	rotationMatrix[16];
    GLfloat	quaternion0[4];
    sglAngleToQuaternionf(quaternion0, 0.f,pMakeMeshInfo->fRotation[1],pMakeMeshInfo->fRotation[2]);
    sglQuaternionToMatrixf(rotationMatrix, quaternion0);
    sglMultMatrixf(matrix, matrix, rotationMatrix);
    
    //이것보다 아래의 것이 좀더 빠를것 같아서 수정했다.
//    sglAngleToQuaternionf(quaternion0, pMakeMeshInfo->fRotation[0],0.f,0.f);
//    sglQuaternionToMatrixf(rotationMatrix, quaternion0);
//    sglMultMatrixf(matrix, matrix, rotationMatrix);
    
    sglRotateRzRyRxMatrixf(matrix,
                           pMakeMeshInfo->fRotation[0],
                           0.f,
                           0.f);

    if(pModel == 0)
    {
        sglLog("Error pModel is null\n");
        return ;
    }
    sglMesh* lstMesh = pModel->GetMesh();
    int nCnt = pModel->GetMeshSize();
    int nIndexColor = 0;

    for (int j = 0; j < nCnt; j++) {
        sglMesh* pMesh = &lstMesh[j];
        
        for(int i = 0; i < pMesh->nVetexCnt; i++)
        {
            
            
            nIndex = i*3;
            nIndexColor = i*4;

            
            //좌표를 이동한다.
            sglMultMatrixVectorf(&pMesh->pVetexBuffer[nIndex], matrix,&pMesh->pVetexBuffer[nIndex]);
            
            
            if(pMakeMeshInfo->cType == 0x02) //건물
            {
                // Compact 정보를 만든다.
                if(pMesh->pVetexBuffer[nIndex + 1] > fMaxH)
                    fMaxH = pMesh->pVetexBuffer[nIndex + 1];
                
                if(pMesh->pVetexBuffer[nIndex] < m_ptCompacBlzLeftTop.x)
                    m_ptCompacBlzLeftTop.x = pMesh->pVetexBuffer[nIndex];
                
                if(pMesh->pVetexBuffer[nIndex+2] < m_ptCompacBlzLeftTop.z)
                    m_ptCompacBlzLeftTop.z = pMesh->pVetexBuffer[nIndex+2];
                
                
                if(pMesh->pVetexBuffer[nIndex] > m_ptCompacBlzRightBottom.x)
                    m_ptCompacBlzRightBottom.x = pMesh->pVetexBuffer[nIndex];
                
                if(pMesh->pVetexBuffer[nIndex+2] > m_ptCompacBlzRightBottom.z)
                    m_ptCompacBlzRightBottom.z = pMesh->pVetexBuffer[nIndex+2];
            }
            else if(pMakeMeshInfo->cType == 0x01) //땅일때
            {
                if(pMesh->pColorBuffer == NULL && fTerrianShadow != NONE_VALUE)
                    pMesh->pColorBuffer = new float[pMesh->nVetexCnt * 4 * sizeof(GLfloat)];
                
                SVector Normal;
                if(GetFlat() == false) //울툴불퉁은 지형에 맞게 위치를 조정해준다.
                {
                    
                    ptPos.x = pMesh->pVetexBuffer[nIndex];
                    ptPos.z = pMesh->pVetexBuffer[nIndex+2];
                    
                    //건물보다 일단 아주 높게 설정하여 성공 시켜서 지형을 정상적으로 만든다.
                    ptPos.y = 9999.0f;
                    
                    
                    // Added By Song 2013.02.12 범위에 벗어나는 경우가 생겨서...
                    //x 512에서 값을 구해오지 못한다.
                    if(ptPos.x == n512)          ptPos.x = n512 - 1;
                    //z 512에서 값을 구해오지 못한다.
                    if(ptPos.z == n512)          ptPos.z = n512 - 1;
                        
                    int nResult = pSglCore->GetPositionY(&ptPos,false,&Normal);
                    if(nResult != E_SUCCESS)
                    {
                        HLogD("지형의 Z값을 구할 수가 없다 %d\n",nResult);
                        ptPos.y = fPrevY;
                    }
                    else
                        fPrevY = ptPos.y;
                    
                    pMesh->pVetexBuffer[nIndex + 1] += ptPos.y;
                    
                    if(fTerrianShadow != NONE_VALUE)
                    {
                        //지형의 음영을 구해온다.
                        float fColor = 5.f * ptPos.y / 256.f + 0.5f / fTerrianShadow; //높이에 대한 음영
                        //최종적으로 안쓰는 이유 (안드로이드에서는 평면과 울툴불퉁이 연결될때 경계선에 밝은 라인이 그려진다
                        //아이폰에서는 안그러는데.. 아 왜그럴까?
                        //인덱스의 삼각형의 법선벡터를 사용해야하는데 현제 정점의 노멀벡터를 사용하면 이상하게 점박이가 생긴다. 이유가 무얼까?
                        //float fColor = sglPhongShadeColor(&Normal,fTerrianShadow);
                        pMesh->pColorBuffer[nIndexColor] = fColor;
                        pMesh->pColorBuffer[nIndexColor + 1] = fColor;
                        pMesh->pColorBuffer[nIndexColor + 2] = fColor;
                        pMesh->pColorBuffer[nIndexColor + 3] = 1.0f;
                        
                        //pMesh->shIndicesCnt * 3 * 3 Normal Vector
                        //CalcColor(&pMesh->pNormalBuffer[nIndex]);
                    }
                }
                else
                {
                    
                    if(fTerrianShadow != NONE_VALUE)
                    {
                        //지형의 음영을 구해온다.
                        float fColor = 5.f * pMesh->pVetexBuffer[nIndex+1]/ 256.f + 0.5f / fTerrianShadow; //높이에 대한 음영
//                        Normal.x = 0.0f;
//                        Normal.y = 1.0f;
//                        Normal.z = 0.0f;
//                        float fColor = sglPhongShadeColor(&Normal,fTerrianShadow);
                        pMesh->pColorBuffer[nIndexColor] = fColor;
                        pMesh->pColorBuffer[nIndexColor + 1] = fColor;
                        pMesh->pColorBuffer[nIndexColor + 2] = fColor;
                        pMesh->pColorBuffer[nIndexColor + 3] = 1.0f;
                    }
                }
            }
        }
    }
    
    if(fMaxH != -9999.0f && fMaxH > m_fHeightMax)
        m_fHeightMax = fMaxH;        

    
}



//GetIndexToCenterPoint 맵의 index를 사용하여 포인트를 가져온다.
void CSglTerrian::GetIndexToCenterPoint(int nIndex,SPoint* pOutPoint)
{
    int  nGroupCount;
	nGroupCount = (CSGLCore::m_gTerrianSize - 1) / GROUP_SIZE;
    int nGroupSize_fromzero = GROUP_SIZE + 1;
    
    int nX = nIndex % nGroupCount;
    int nZ = nIndex / nGroupCount;
    float fCenterPos = (float)((CSGLCore::m_gTerrianSize - 1) / 2.0f);
    //중앙으로 이동한다.
    pOutPoint->x = (- fCenterPos + (nX * nGroupSize_fromzero - (1 * nX)) + GROUP_SIZE / 2);
    pOutPoint->y = 0;
    //중앙으로 이동한다.
    pOutPoint->z = (- fCenterPos + (nZ * nGroupSize_fromzero - (1 * nZ)) + GROUP_SIZE / 2);
    
}

//GetWorldToBlock world 좌표에서 맵좌표로 변경한다.
//nImageSize : 이미지의 넓이 or 높이 (넓이 == 높이)
//pPoint     : world 좌표
//pOutMapPoint : 맵좌표로 변경한다. (0~16)
int CSglTerrian::GetWorldToBlock(SPoint* pPoint,SPoint* pOutMapPoint)
{
    int nImageSize = CSGLCore::m_gTerrianSize;
    float fHalfTerrian = (float)((nImageSize - 1) / 2.0f);
	float fTerrian = fHalfTerrian * 2;
    
	//좌측 상단의 좌표로 변경한다.
	pOutMapPoint->x = fHalfTerrian + pPoint->x; 
	if( pOutMapPoint->x >= fTerrian || pOutMapPoint->x < 0)
    {
        //지형설정할때 9999로 드러와서 1024.2로 약간의 차이가 생기면
        if(pOutMapPoint->z == 9999.0f && pOutMapPoint->x < 1025.0f)
        {
            //스킵하자.
            pOutMapPoint->x = 1023.9;
        }
        else
        {
//            HLog("sglWorldToMapUnit X 맵범위를 벗어남 -2\n");
            return -1;
        }
    }
    
    
	//좌측 상단의 좌표로 변경한다.	
	pOutMapPoint->z = fHalfTerrian + pPoint->z;
	if( pOutMapPoint->z >= fTerrian || pOutMapPoint->z < 0)
    {
        //지형설정할때 9999로 드러와서 1024.2로 약간의 차이가 생기면
        if(pOutMapPoint->z == 9999.0f && pOutMapPoint->z < 1025.0f)
        {
            //스킵하자.
            pOutMapPoint->z = 1023.9;
        }
        else
        {
//            HLog("sglWorldToMapUnit Z 맵범위를 벗어남 -3\n");
            return -2;
        }
    }
	pOutMapPoint->y = pPoint->y;
	return E_SUCCESS;
}

int CSglTerrian::GetWorldToMapIndex(SPoint* pPoint, int *pnMapGroupIndexX,int *pnMapGroupIndexZ,int nDirection)
{
    int nIndexVertexX;
    int nIndexVertexZ;
    float fIndexVertexX;
    float fIndexVertexZ;
    float fUpDown;

    int nResult = 0;
    SPoint ptMap;
    nResult = GetWorldToBlock(pPoint,&ptMap);
    if(nResult != E_SUCCESS) return nResult;    
    return GetMapGroupIndexToMapUnit(
                                     nDirection,&ptMap,pnMapGroupIndexX,pnMapGroupIndexZ,&nIndexVertexX,
                                     &nIndexVertexZ,&fIndexVertexX,&fIndexVertexZ,&fUpDown);
}

int CSglTerrian::GetDirection(SVector *pvDirection)
{
	float x = pvDirection->x;
	float z = -pvDirection->z; 
	
	if( x == 0.0 && z > 0.0)						// 북
		return H_N;
	else if( x < 0.0 && z > 0.0 && z/x < -1.0)		// 북쪽에 가까운 서쪽
		return H_NNW;
	else if( x < 0.0 && z > 0.0 && z == -x)			// 북서
		return H_NW;
	else if( x < 0 && z > 0.0 && z/x > -1.0)		// 서쪽에 가까운 북쪽
		return H_NWW;
	else if( x < 0 && z == 0.0)						//서쪽
		return H_W;
	else if( x < 0 && z < 0 && z/x < 1)				//서쪽에 가까운 남쪽
		return H_WWS;
	else if( x < 0 && z < 0 && z == x)				//서남
		return H_SW;
	else if( x < 0 && z < 0 && z/x > 1)				//남쪽에 가까운 서쪽
		return H_SSW;
	else if( x == 0 && z < 0)						//남
		return H_S;
	else if( x > 0 && z < 0 && z/x < -1 )			//남쪽에 가까운 동쪽
		return H_SSE;
	else if( x > 0 && z < 0 && z == -x)				//남동
		return H_SE;
	else if( x > 0 && z < 0 && z/x > -1)			//동쪽에 가까운 남쪽
		return H_SEE;
	else if( x > 0 && z == 0)						//동
		return H_E;
	else if(x > 0 && z > 0 && z/x < 1)				//동쪽에 가까운 북쪽
		return H_EEN;
	else if(x > 0 && z > 0 && z == x)				//동북
		return H_EN;
	else if(x > 0 && z > 0 && z/x > 1)				//북쪽에 가까운 동쪽
		return H_ENN;
	return -1;
}



//sglGetMapGroupIndexToMapUnit : 맵의 좌표와 방향을 사용하여 맵의 그룹인덱스와 그 그룹의 정점인덱스를 가져온다.
//nGroupSize : 그룹 사이즈
//nDirection : 방향
//pMapPoint : 맵좌표계의 위치
//pnMapGroupIndexX : 맵인덱스 X
//pnMapGroupIndexY : 맵인덱스 Y
//pnIndexVertexX   : 하나의 맵의 정점 좌표 X
//pnIndexVertexY   : 하나의 맵의 정점 좌표 Y
int CSglTerrian::GetMapGroupIndexToMapUnit(
								 int nDirection, 
								 SPoint* pMapPoint,
								 int *pnMapGroupIndexX,
								 int *pnMapGroupIndexZ,
								 int *pnIndexVertexX,
								 int *pnIndexVertexZ,
								 float *pfIndexVertexX,
								 float *pfIndexVertexZ,
								 float *pfUpDown)
{
	
    
	int nGroupSize = GROUP_SIZE;
    
	float fMapGroupIndexX = 0.0f ,fMapGroupIndexZ = 0.0f;
    
	*pnMapGroupIndexX = 0;
	*pnMapGroupIndexZ = 0;
	
	
	float fx = pMapPoint->x;
	float fz = pMapPoint->z;
	
	// -----------------------------------------------------------------------------------
	// fGroupSize = 4
	// 0 => 0
	// 4 => 1
	// 8 => 2
	// -----------------------------------------------------------------------------------
	fMapGroupIndexX = fx / (float)nGroupSize; //그룹의 인덱스를 구한다.
	*pnMapGroupIndexX = (int)fMapGroupIndexX; //정수로 환산한다.
	//------------------------------------------------------------------------------------	
	
	*pfIndexVertexX = fx - (float)(*pnMapGroupIndexX * (float)nGroupSize);
	
	//Modified By Song 2010.11.28 정점인덱스를 최대한 근사치로 만들어보자 + 0.5f 를 추가함
	*pnIndexVertexX = (int)(*pfIndexVertexX + 0.5f);
	if( *pfIndexVertexX == (float)*pnIndexVertexX) //정점이 정수로 끝나면 ( 겹친다.)
	{
		//int nLeftSideGroup = nGroupWidth * (*pnMapGroupIndexX);		
		//정점이 그룹의 0에 있다면 왼쪽 방향으로 바라보고 있다면 그룹인덱스를 이전 것으로 해야한다.
		if(*pnMapGroupIndexX != 0 && (nDirection > 0 && nDirection < 8)) //왼쪽
		{	
			float fDivGroup = *pfIndexVertexX / (float)nGroupSize;
			if( fDivGroup == (float)(int)fDivGroup)
			{
				*pnMapGroupIndexX -= 1;
				*pnIndexVertexX = (float)nGroupSize;
				*pfIndexVertexX = (float)*pnIndexVertexX;
			}
		}
		//else 오른쪽 방향으로 바라보고 있다면 기본이 오른쪽 방향 인덱스를 사용하기 때문에 구지 구할필요가 없다.
		
	}
	
	
	//hLog("[%2.1f]GroupIndexX = %d, VertexIndexX = %d\n", fPointX,*pnMapGroupIndexX,*pnIndexVertexX);
	
	
	// -----------------------------------------------------------------------------------
	// fGroupSize = 4
	// 0 => 0
	// 4 => 1
	// 8 => 2
	// -----------------------------------------------------------------------------------
	fMapGroupIndexZ = fz / (float)nGroupSize; //그룹의 인덱스를 구한다.
	*pnMapGroupIndexZ = (int)fMapGroupIndexZ; //정수로 환산한다.
	//------------------------------------------------------------------------------------	
    
	*pfIndexVertexZ = fz - (float)(*pnMapGroupIndexZ * (float)nGroupSize);
	
	//Modified By Song 2010.11.28 정점인덱스를 최대한 근사치로 만들어보자 + 0.5f 를 추가함
	*pnIndexVertexZ = (int)(*pfIndexVertexZ + 0.5f);
	if( *pfIndexVertexZ == (float)*pnIndexVertexZ) //정점이 정수로 끝나면 ( 겹친다.)
	{
		//int nLeftSideGroup = nGroupWidth * (*pnMapGroupIndexX);		
		//정점이 그룹의 0에 있다면 왼쪽 방향으로 바라보고 있다면 그룹인덱스를 이전 것으로 해야한다.
		if(*pnMapGroupIndexZ != 0 && ((nDirection >= 0 && nDirection < 4) || (nDirection > 12 && nDirection < 16))) //위쪽
		{	
			float fDivGroup = *pfIndexVertexZ / (float)nGroupSize;
			if( fDivGroup == (float)(int)fDivGroup)
			{
				*pnMapGroupIndexZ -= 1;
				*pnIndexVertexZ = (float)nGroupSize;
				*pfIndexVertexZ = (float)*pnIndexVertexZ;
			}
		}
		//else 오른쪽 방향으로 바라보고 있다면 기본이 오른쪽 방향 인덱스를 사용하기 때문에 구지 구할필요가 없다.
	}
	
	
	//------------------------------------------------------------------------------------
	//x + y - 1 = 0;
	float fUnitX,fUnitZ;
	if(*pfIndexVertexX >= 0 && *pfIndexVertexX < 1)
		fUnitX = *pfIndexVertexX; //0으로 나누면 안뒤아.
	else 
		fUnitX = *pfIndexVertexX - (float)(int)*pfIndexVertexX;
	
	if(*pfIndexVertexZ >= 0 && *pfIndexVertexZ < 1)
		fUnitZ = *pfIndexVertexZ; //0으로 나누면 안뒤아.
	else 
		fUnitZ = *pfIndexVertexZ - (float)(int)*pfIndexVertexZ;
	
	
	*pfUpDown = fUnitZ + fUnitX - 1; //fUpDown < 0 상단,  fUpDown > 0 하단
	
	return E_SUCCESS;
}



float CSglTerrian::GetPointY(
                   SPoint	*pPoint,
				   int nIndexVertexX,
				   int nIndexVertexZ,
				   float fIndexVertexX,
				   float fIndexVertexZ,
				   float fUpDown,
                   int *npResult,
                   SVector* pOutNormalVector)
{
    //int nImageSize = CSGLCore::m_gTerrianSize;
    //int nGroupSize = GROUP_SIZE;
	int  nGroupCount;// = (nImageSize - 1) / nGroupSize;
	int nNormalIndex = 0;
    
    //4=>3 정점으로 변경함
	int nVertexIndex = 0;
    
    SVector normalV;
    
    nGroupCount = (CSGLCore::m_gTerrianSize - 1) / GROUP_SIZE;
    nVertexIndex = ( ((float)GROUP_SIZE + 1) * (nIndexVertexZ) + (nIndexVertexX) ) * 3;
    
    if(fIndexVertexX == nGroupCount && fIndexVertexZ == nGroupCount)
		nNormalIndex = ((GROUP_SIZE * 2) * (int)fIndexVertexZ) * 3;
	else 
		nNormalIndex = ((GROUP_SIZE * 2) * (int)fIndexVertexZ + ((int)fIndexVertexX * 2)) * 3 ;
    
    
    *npResult = E_SUCCESS;
	
	//0-1 
	// /
	//2-3 
	//현재 버텍스 인덱스가 fGroupSize 즉 그룹의 가장자리 점에 있으면 
	if((fIndexVertexX == (float)GROUP_SIZE && fIndexVertexZ == (float)GROUP_SIZE) || fUpDown > 0) //하단  나머지는 상단
	{
		//sglLog("fUpDown = %f , fUpDown < 0 상단,  fUpDown > 0 하단\n",fUpDown);
		nNormalIndex += 3;
	}
	
	
    
    if(m_pHeightNormal == 0) 
    {
        //*npResult = -7;
        //return -9999.0f;
        return H_TERRIAN_DEFAULT_HIGHT;
    }
    normalV.x = m_pHeightNormal[nNormalIndex];
	normalV.y = m_pHeightNormal[nNormalIndex + 1];
	normalV.z = m_pHeightNormal[nNormalIndex + 2];
    if(pOutNormalVector)
        memcpy(pOutNormalVector, &normalV, sizeof(SVector));
	
	return sglGetYPlaneEquation(
                                &normalV, 
                                m_pHeightVertex[nVertexIndex], 
                                m_pHeightVertex[nVertexIndex + 1], 
                                m_pHeightVertex[nVertexIndex + 2], 
                                pPoint->x, 
                                pPoint->z);
}










//Start A-Star ---------------------------

void CSglTerrian::LoadMap(char* sMapTitleTxt)
{
    LineReader reader(sMapTitleTxt);
    if(mpLoadTileMap)
    {
        delete (Tiling*)mpLoadTileMap;
        mpLoadTileMap = 0;
    }
    if(mpFindPath)
    {
        delete (Search*)mpFindPath;
        mpFindPath = 0;
    }    
    mpLoadTileMap = new Tiling(reader);
    mpFindPath = new AStar<>;
    ((Search*)mpFindPath)->setNodesLimit(s_nodesLimit);
}

bool CSglTerrian::IsObstacle(SPoint* ptNow)
{
    int nGX = (CSGLCore::m_gTerrianSize - 1) / GROUP_SIZE;
    SPoint ptMapPoint;
    int nIndex = -1;
    int nMapGroupIndexX,nMapGroupIndexZ,nIndexVertexX,nIndexVertexZ;
    float fIndexVertexX,fIndexVertexZ,fUpDown;
    
    nIndex = CSglTerrian::GetWorldToBlock(ptNow,&ptMapPoint);
    if(nIndex != E_SUCCESS) 
    {
        return true;
    }
    
    //2> 기타 그룹 정보를 가져온다.
    CSglTerrian::GetMapGroupIndexToMapUnit(
                                           0, //그냥 북쪽.. 
                                           &ptMapPoint,
                                           &nMapGroupIndexX,
                                           &nMapGroupIndexZ,
                                           &nIndexVertexX,
                                           &nIndexVertexZ,
                                           &fIndexVertexX,
                                           &fIndexVertexZ,
                                           &fUpDown);
    
    if(nMapGroupIndexX < 0) 
    {
        HLog("FinePath Error 방향이 0일경우 -12\n");
        return true; 
    }    
    
    nIndex = ( nGX * nMapGroupIndexZ) + nMapGroupIndexX;
    
    
    
    return IsObstacle(nIndex);
}

bool CSglTerrian::IsObstacle(int nIndex)
{
//    CSGLCore* pCore = gpWorld->GetSGLCore();
//    CSglTerrian* pTerrian = pCore->GetBlockTerrian(nIndex);
//    int nBlockType = pTerrian->GetBlockType();
    //if(pTerrian->GetGType()== H_GTYPE_FLATGROUNT_NORMAL || pTerrian->GetGType()== H_GTYPE_UPDOWNGROUNT_NORMAL)
//    if(!(nBlockType & 0x02)) //건물이 아니면 장애물이 아니다.
//    {   
//        return false;
//    }
    return CSglTerrian::IsObstacleInPath(nIndex);
}

int CSglTerrian::FindNoneObstacle(CSprite* pSprite,SPoint* ptNow,SPoint* ptOutNoneObstacle,int nType,int nOrgIndex)
{
    bool bInitOrg = false;
    int nGX = (CSGLCore::m_gTerrianSize - 1) / GROUP_SIZE;
    int nIndex = -1;
    int nMapGroupIndexX,nMapGroupIndexZ,nIndexVertexX,nIndexVertexZ;
    float fIndexVertexX,fIndexVertexZ,fUpDown;
    SPoint ptFndNow;
    SPoint ptMapPoint;
    memcpy(&ptFndNow,ptNow,sizeof(SPoint));
    if(nType == 0)
    {
        nType ++;
        bInitOrg = true;
    }
    else if(nType == 1)
    {
        ptFndNow.x += 16;
        nType ++;
    }
    else if(nType == 2)
    {
        ptFndNow.x -= 16;
        nType++;
    }
    else if(nType == 3)
    {
        ptFndNow.z += 16;
        nType++;
    }
    else if(nType == 4)
    {
        ptFndNow.z -= 16;
        nType++;
    }
    else
    {
        HLog("장애물이 아닌 지형을 찾을 수가 없습니다.\n");
        return -1;
    }
    
    
    
    nIndex = CSglTerrian::GetWorldToBlock(&ptFndNow,&ptMapPoint);
    if(nIndex != E_SUCCESS) 
    {
        return FindNoneObstacle(pSprite,ptNow,ptOutNoneObstacle,nType);
    }
    
    //2> 기타 그룹 정보를 가져온다.
    CSglTerrian::GetMapGroupIndexToMapUnit(
                                           0, //그냥 북쪽.. 
                                           &ptMapPoint,
                                           &nMapGroupIndexX,
                                           &nMapGroupIndexZ,
                                           &nIndexVertexX,
                                           &nIndexVertexZ,
                                           &fIndexVertexX,
                                           &fIndexVertexZ,
                                           &fUpDown);
    
    if(nMapGroupIndexX < 0) 
    {
        HLog("FinePath Error 방향이 0일경우 -12\n");
        return FindNoneObstacle(pSprite,ptNow,ptOutNoneObstacle,nType); 
    }    
    
    nIndex = ( nGX * nMapGroupIndexZ) + nMapGroupIndexX;
    if(bInitOrg)
        nOrgIndex = nIndex;
    else 
    {
        if(nOrgIndex ==  nIndex) //장애물이 아니지형을 찾고자 하는데 자신의 위치와 똑같으면 스킵
        {
            nIndex = FindNoneObstacle(pSprite,ptNow,ptOutNoneObstacle,nType,nOrgIndex);
            if(nIndex == -1) return nIndex;
        }
    }

    if(!CSglTerrian::IsObstacleInPath(nIndex)) //장애물이 아니면
    {
        memcpy(ptOutNoneObstacle,&ptFndNow, sizeof(SPoint));
        
        if(pSprite)
        {
            if(nType >= 2  && ((CHWorld*)pSprite->GetWorld())->GetPositionY(ptOutNoneObstacle) == E_SUCCESS)
            {
                float fDif = ptOutNoneObstacle->y - ptNow->y;
                if(fDif > 6.0f || fDif < -6.0f) //땅의 차이가 너무 나면 절벽으로 막혀있다.
                {
                    HLog("땅의 차이가 너무 나면 절벽으로 막혀있다. nType = %d , %f\n",nType,ptOutNoneObstacle->y - ptNow->y);
                    nIndex = FindNoneObstacle(pSprite,ptNow,ptOutNoneObstacle,nType,nOrgIndex);
                }
            }
        }
        return nIndex;
    }
    else  //장애물
    {
        nIndex = FindNoneObstacle(pSprite,ptNow,ptOutNoneObstacle,nType,nOrgIndex);
    }
    return nIndex;
}

void CSglTerrian::SetObstacleInPath(SPoint* ptNow)
{
    int nGX = (CSGLCore::m_gTerrianSize - 1) / GROUP_SIZE;
    SPoint ptMapPoint;
    int nIndex = -1;
    int nMapGroupIndexX,nMapGroupIndexZ,nIndexVertexX,nIndexVertexZ;
    float fIndexVertexX,fIndexVertexZ,fUpDown;
    
    nIndex = CSglTerrian::GetWorldToBlock(ptNow,&ptMapPoint);
    if(nIndex != E_SUCCESS) 
    {
        return ;
    }
    
    //2> 기타 그룹 정보를 가져온다.
    CSglTerrian::GetMapGroupIndexToMapUnit(
                                           0, //그냥 북쪽.. 
                                           &ptMapPoint,
                                           &nMapGroupIndexX,
                                           &nMapGroupIndexZ,
                                           &nIndexVertexX,
                                           &nIndexVertexZ,
                                           &fIndexVertexX,
                                           &fIndexVertexZ,
                                           &fUpDown);
    
    if(nMapGroupIndexX < 0) 
    {
        return ; 
    }    
    
    nIndex = ( nGX * nMapGroupIndexZ) + nMapGroupIndexX;
    SetObstacleInPath(nIndex,true);
}

void CSglTerrian::SetObstacleInPath(int nIndex,bool v)
{
    Tiling* inTiling;
    inTiling = (Tiling*)mpLoadTileMap;
    //쓰레드에 안전하게 사용하자.
    pthread_mutex_lock(&mFindPathSyncMutex); //락
    inTiling->setObstacle(nIndex, v);    
    pthread_mutex_unlock(&mFindPathSyncMutex); //언락 
}

bool CSglTerrian::IsObstacleInPath(int nIndex)
{
    Tiling* inTiling = (Tiling*)mpLoadTileMap;
    return inTiling->IsObstacle(nIndex); 
}

//findpath는 쓰레드에 안전하다.
//return 값은 반드시 메모리 해제하여 주어야 한다.
vector<int>* CSglTerrian::FindPath(CSprite* pFromSprite,SPoint* ptFrom,SPoint* ptTo,void* pTiling)
{
    
    Tiling* inTiling;
    SPoint ptFromNoneObstacle,ptToNoneObstacle;
    vector<int>* pathnull = 0;
    int SGI,TGI;
    
    if(pTiling == NULL)
        inTiling = (Tiling*)mpLoadTileMap;
    else
        inTiling = (Tiling*)pTiling;
    
    if(inTiling == NULL) return pathnull;
    
    
    //From
    SGI = FindNoneObstacle(pFromSprite,ptFrom,&ptFromNoneObstacle);
    if(SGI < 0) 
    {
        HLog("Not Find Path From \n");
        return pathnull;
    }
    
    
    //To
    TGI = FindNoneObstacle(NULL,ptTo,&ptToNoneObstacle);
    if(TGI < 0) 
    {
        HLog("Not Find Path To \n");
        return pathnull;
    }
    
    //Added By Path의 경로가 정확한지를 체크해준다.
    if(((Tiling*)inTiling)->isValidNodeId(SGI) == false) return pathnull;
    if(((Tiling*)inTiling)->isValidNodeId(TGI) == false) return pathnull;
    
    if(TGI == SGI)
    {
        return pathnull;
    }
    
    //쓰레드에 안전하게 사용하자.
    pthread_mutex_lock(&mFindPathSyncMutex); //락
    
    ((Search*)mpFindPath)->findPath(*inTiling, SGI, TGI);
    const vector<int>& path = ((Search*)mpFindPath)->getPath();
    pathnull = new vector<int>;
    *pathnull = path;
    
#ifdef DEBUG_MSG_FINDPATH
    inTiling->printFormatted(cout, path);
#endif
    
    pthread_mutex_unlock(&mFindPathSyncMutex); //언락   
    
    if(pathnull->size() == 0) 
    {
        //HLog("Find Path is same Start with End\n");
        delete pathnull;
        pathnull = NULL;
    }
    return pathnull;
}

//End A-Star ---------------------------


//-------------------------------------------------------------------------------------------
//파일 포맷 정리 함.
// char(Version) | char(Type) | int(nReserved) | 
// int(TerrianMakeBlockInfo count 반복) | char(TerrianMakeBlockInfo 데이터존재유무) 

// char(Terrian Mesh Count) | char(cType,0땅,1건물,2other) | float*3(position) | float*3 (rotation) 
// char(file name len) | filename(파일명)
// char(texture count 반복) | char(file name len) | filename(파일명)

// char(NorObjects Mesh Count) | char(cType,0땅,1건물,2other) | float*3(position) | float*3 (rotation) 
// char(file name len) | filename(파일명)
// char(texture count 반복) | char(file name len) | filename(파일명)


// char(NorTransObjects Mesh Count) | char(cType,0땅,1건물,2other) | float*3(position) | float*3 (rotation) 
// char(file name len) | filename(파일명)
// char(texture count 반복) | char(file name len) | filename(파일명)
//-------------------------------------------------------------------------------------------

//MakeInfo정보를 읽어들인다.
TerrianMakeInfo* CSglTerrian::LoadMakeInfo(const char* sFileName)
{
    TerrianMakeInfo* pMakeInfo = NULL;
    FILE* fp = fopen(sFileName, "rb");
    if (!fp) 
    {
        HLogE("[Error] opening %s from TerrianMakeInfo\n", sFileName);
        return NULL;
    }
    
//1>     TerrianMakeInfo를 읽어들임.
//    typedef struct {
//        char  cVersion;                     //Version 1:
//        char  cType;                        //
//        int   nReserved;                    //예약됨.
//        vector<TerrianMakeBlockInfo*>     Blocks;//
//    }TerrianMakeInfo;

    pMakeInfo = new TerrianMakeInfo;
    fread(&pMakeInfo->cVersion, 1, 1, fp);
    fread(&pMakeInfo->cType, 1, 1, fp);
    fread(&pMakeInfo->shWidth, 2, 1, fp);
    fread(&pMakeInfo->nReserved, 4, 1, fp);
    
    
    
    int nCnt = 0;
    fread(&nCnt, 1, 1, fp);
    pMakeInfo->sFileName = 0;
    if(nCnt > 0)
    {
        pMakeInfo->sFileName = new char[nCnt + 1];
        fread(pMakeInfo->sFileName, nCnt, 1, fp);
        pMakeInfo->sFileName[nCnt] = 0;
    }
    
    pMakeInfo->sBackgroundTexture = 0;
    
    if(pMakeInfo->cVersion > 1) //versition 부터는 백그라운드 정보가 생성되어 있다.
    {
        fread(&nCnt, 1, 1, fp);
        if(nCnt > 0)
        {
            pMakeInfo->sBackgroundTexture = new char[nCnt + 1];
            fread(pMakeInfo->sBackgroundTexture, nCnt, 1, fp);
            pMakeInfo->sBackgroundTexture[nCnt] = 0;
        }
        fread(pMakeInfo->fBackgroundPosition, sizeof(pMakeInfo->fBackgroundPosition), 1, fp);
        fread(pMakeInfo->fBackgroundRotation, sizeof(pMakeInfo->fBackgroundRotation), 1, fp);
    }
    
    pMakeInfo->fFogColor[0] = 0.0f;
    pMakeInfo->fFogColor[1] = 0.0f;
    pMakeInfo->fFogColor[2] = 0.0f;
    pMakeInfo->fFogColor[3] = 1.0f; //항상 1.0f으로 정의 한다.
    pMakeInfo->fFogStart = 0.0f;
    pMakeInfo->fFogEnd = 0.0f;
    pMakeInfo->fFogDentity = 0.0f;
    pMakeInfo->fTerrianShadow = NONE_VALUE;
    if(pMakeInfo->cVersion >=3 )
    {
        fread(pMakeInfo->fFogColor, sizeof(pMakeInfo->fFogColor), 1, fp);
        fread(&pMakeInfo->fFogStart, sizeof(float), 1, fp);
        fread(&pMakeInfo->fFogEnd, sizeof(float), 1, fp);
        fread(&pMakeInfo->fFogDentity, sizeof(float), 1, fp);
    }
    
    if(pMakeInfo->cVersion >=4)
    {
        fread(&pMakeInfo->fTerrianShadow, sizeof(float), 1, fp);
    }
    
    
    
    fread(&nCnt, 4, 1, fp);
    for (int i = 0; i < nCnt; i++) 
    {
//2> TerrianMakeBlockInfo 블럭읽어들이기.
//        typedef struct 
//        {  
//            vector<TerrianMakeMeshInfo*> Terrian;       
//            vector<TerrianMakeMeshInfo*> NorObjects;
//            vector<TerrianMakeMeshInfo*> NorTransObjects;
//        }TerrianMakeBlockInfo;
        
        unsigned char cIsInfo = 0;
        fread(&cIsInfo, 1, 1, fp); //정보가 존재하는지의 유무
        if(cIsInfo == 0) 
        {
            pMakeInfo->Blocks.push_back(NULL);
            continue; //정보가 없다.
        }
        TerrianMakeBlockInfo* BlockInfo = new TerrianMakeBlockInfo;
        
        //읽을때 BlockInfo->cInfo
        fread(&BlockInfo->cCanMoving, 1, 1, fp);
        
        //Terrian
        unsigned char cMeshCnt = 0;
        fread(&cMeshCnt, 1, 1, fp);
        for (int j = 0; j < cMeshCnt; j++) 
        {
            TerrianMakeMeshInfo * MeshInfo = new TerrianMakeMeshInfo;
            LoadMakeMeshInfo(fp,MeshInfo,pMakeInfo->cVersion);
            //MeshInfo
            BlockInfo->Terrian.push_back(MeshInfo);
        }
        
        //NorObjects
        cMeshCnt = 0;
        fread(&cMeshCnt, 1, 1, fp);
        for (int j = 0; j < cMeshCnt; j++) 
        {
            TerrianMakeMeshInfo * MeshInfo = new TerrianMakeMeshInfo;
            LoadMakeMeshInfo(fp,MeshInfo,pMakeInfo->cVersion);
            //MeshInfo
            BlockInfo->NorObjects.push_back(MeshInfo);
        }
        
        //NorTransObjects
        cMeshCnt = 0;
        fread(&cMeshCnt, 1, 1, fp);
        for (int j = 0; j < cMeshCnt; j++) 
        {
            TerrianMakeMeshInfo * MeshInfo = new TerrianMakeMeshInfo;
            LoadMakeMeshInfo(fp,MeshInfo,pMakeInfo->cVersion);
            //MeshInfo
            BlockInfo->NorTransObjects.push_back(MeshInfo);
        }
        
        
        //OtherInfo
        cMeshCnt = 0;
        fread(&cMeshCnt, 1, 1, fp);
        for (int j = 0; j < cMeshCnt; j++) 
        {
            TerrianMakeMeshInfo * MeshInfo = new TerrianMakeMeshInfo;
            LoadMakeMeshInfo(fp,MeshInfo,pMakeInfo->cVersion,true);
            //MeshInfo
            BlockInfo->OtherInfo.push_back(MeshInfo);
        }
        
        
        pMakeInfo->Blocks.push_back(BlockInfo);
    }
    
    fclose(fp);
    return pMakeInfo;
}

int CSglTerrian::LoadMakeMeshInfo(FILE* fp,TerrianMakeMeshInfo* pMeshInfo,unsigned char nVersion,bool bOther)
{
//3> TerrianMakeMeshInfo 메쉬정보를 읽어온다.       
//        typedef struct 
//        {
//            char  cType; //cType:0(땅) 1(건물) 2(나무,가로등)
//            float fPosition[3];
//            float fRotation[3];
//            char* sFileName;
//            int   nTextureCnt;              //텍스쳐 리스트
//            char** arrTextureFileList;      //텍스쳐 파일내밈 리스트 (sFileName에 포함된 내용을 무시하고 여기에 있는 내용으로 대처한다.)
//            int   nReserved;
//        }TerrianMakeMeshInfo;
   
    fread(&pMeshInfo->cType, 1, 1, fp); //cType:0(땅) 1(건물) 2(나무,가로등)

    fread(&pMeshInfo->fPosition, sizeof(float) * 3, 1, fp);
    
    if(nVersion < 3) //Version 2까지는 Z축이 높이이다.
    {
        float f = pMeshInfo->fPosition[2];
        pMeshInfo->fPosition[2] = -pMeshInfo->fPosition[1];
        pMeshInfo->fPosition[1] = f;
    }
    
    fread(&pMeshInfo->fRotation, sizeof(float) * 3, 1, fp);

    if(nVersion < 3) //Version 2까지는 Z축이 높이이다.
    {
        if(bOther == false)
        {
            pMeshInfo->fRotation[0] -= 90;
            float f = pMeshInfo->fRotation[2];
            pMeshInfo->fRotation[2] = pMeshInfo->fRotation[1];
            pMeshInfo->fRotation[1] = f;
        }
        else //단순하게 객체의 방향을 나타낸다.
        {
            float f = pMeshInfo->fRotation[1];
            pMeshInfo->fRotation[1] = pMeshInfo->fRotation[2];
            pMeshInfo->fRotation[2] = f;
        }
    }
    
    
    unsigned char cLen = 0;
    fread(&cLen, 1, 1, fp); 
    pMeshInfo->sFileName = 0;
    if(cLen > 0)
    {
        pMeshInfo->sFileName = new char[cLen + 1];
        fread(pMeshInfo->sFileName, cLen, 1, fp); 
        pMeshInfo->sFileName[cLen] = 0;
    }
    
    fread(&pMeshInfo->cTextureCnt, 1, 1, fp);
    
    pMeshInfo->arrTextureFileList = 0;
    if(pMeshInfo->cTextureCnt > 0)
        pMeshInfo->arrTextureFileList = new char*[pMeshInfo->cTextureCnt];
    for (int i = 0; i < pMeshInfo->cTextureCnt; i++) {
        fread(&cLen, 1, 1, fp); 
        if(cLen > 0)
        {
            pMeshInfo->arrTextureFileList[i] = new char[cLen + 1];
            fread(pMeshInfo->arrTextureFileList[i], cLen, 1, fp); 
            pMeshInfo->arrTextureFileList[i][cLen] = 0;
        }
    }
    
    fread(&pMeshInfo->nReserved, 4,1,fp);
    return E_SUCCESS;
}


int CSglTerrian::SaveMakeMeshInfo(FILE* fp,TerrianMakeMeshInfo* pMeshInfo)
{
    //3> TerrianMakeMeshInfo 메쉬정보를 읽어온다.       
    //        typedef struct 
    //        {
    //            char  cType; //cType:0(땅) 1(건물) 2(나무,가로등)
    //            float fPosition[3];
    //            float fRotation[3];
    //            char* sFileName;
    //            int   nTextureCnt;              //텍스쳐 리스트
    //            char** arrTextureFileList;      //텍스쳐 파일내밈 리스트 (sFileName에 포함된 내용을 무시하고 여기에 있는 내용으로 대처한다.)
    //            int   nReserved;
    //        }TerrianMakeMeshInfo;
    
    fwrite(&pMeshInfo->cType, 1, 1, fp); //cType:0x01(땅) 0x02(건물) 0x04(나무,가로등)
    fwrite(&pMeshInfo->fPosition, sizeof(float) * 3, 1, fp); 
    fwrite(&pMeshInfo->fRotation, sizeof(float) * 3, 1, fp); 
    
    
    unsigned char cLen = 0;
    if(pMeshInfo->sFileName)
        cLen = strlen(pMeshInfo->sFileName);
    
    fwrite(&cLen, 1, 1, fp); 
    if(cLen > 0)
        fwrite(pMeshInfo->sFileName, cLen, 1, fp); 
    
    
    fwrite(&pMeshInfo->cTextureCnt, 1, 1, fp);
    
    for (int i = 0; i < pMeshInfo->cTextureCnt; i++) {
        
        cLen = 0;
        if(pMeshInfo->arrTextureFileList[i])
            cLen = strlen(pMeshInfo->arrTextureFileList[i]);
        
        fwrite(&cLen, 1, 1, fp); 
        if(cLen > 0)
        {
            fwrite(pMeshInfo->arrTextureFileList[i], cLen, 1, fp); 
        }
    }
    
    fwrite(&pMeshInfo->nReserved, 4,1,fp);
    return E_SUCCESS;
}

int CSglTerrian::SaveMakeInfo(const char* sFileName,TerrianMakeInfo* pMakeInfo)
{
    FILE* fp = fopen(sFileName, "wb");
    if (!fp) 
    {
        HLogE("[Error] saving %s from TerrianMakeInfo\n", sFileName);
        return -1;
    }
    
    //1>     TerrianMakeInfo를 읽어들임.
    //    typedef struct {
    //        char  cVersion;                     //Version 1:
    //        char  cType;                        //
    //        int   nReserved;                    //예약됨.
    //        vector<TerrianMakeBlockInfo*>     Blocks;//
    //    }TerrianMakeInfo;
    
    if(SGL_SON_VERSION != pMakeInfo->cVersion)
        pMakeInfo->cVersion = SGL_SON_VERSION;
       
    
    fwrite(&pMakeInfo->cVersion, 1, 1, fp);
    fwrite(&pMakeInfo->cType, 1, 1, fp);
    fwrite(&pMakeInfo->shWidth, 2, 1, fp);
    fwrite(&pMakeInfo->nReserved, 4, 1, fp);
    
    char cCnt = 0;
    if(pMakeInfo->sFileName)
    {
        cCnt = strlen(pMakeInfo->sFileName);
        fwrite(&cCnt, 1, 1, fp);
        fwrite(pMakeInfo->sFileName, cCnt, 1, fp);
    }
    else 
    {
        fwrite(&cCnt, 1, 1, fp);
    }
    
    
    //Version 2부터는 백그라운드 텍스쳐 이미지 정보가 저장된다.
    cCnt = 0;
    if(pMakeInfo->sBackgroundTexture)
    {
        cCnt = strlen(pMakeInfo->sBackgroundTexture);
        fwrite(&cCnt, 1, 1, fp);
        fwrite(pMakeInfo->sBackgroundTexture, cCnt, 1, fp);
    }
    else
    {
        fwrite(&cCnt, 1, 1, fp);
    }
    
    fwrite(pMakeInfo->fBackgroundPosition, sizeof(pMakeInfo->fBackgroundPosition), 1, fp);
    fwrite(pMakeInfo->fBackgroundRotation, sizeof(pMakeInfo->fBackgroundRotation), 1, fp);
    
    
    fwrite(pMakeInfo->fFogColor, sizeof(pMakeInfo->fFogColor), 1, fp);
    fwrite(&pMakeInfo->fFogStart, sizeof(float), 1, fp);
    fwrite(&pMakeInfo->fFogEnd, sizeof(float), 1, fp);
    fwrite(&pMakeInfo->fFogDentity, sizeof(float), 1, fp);
    fwrite(&pMakeInfo->fTerrianShadow, sizeof(float), 1, fp);

    
    int nCnt = pMakeInfo->Blocks.size();
    fwrite(&nCnt, 4, 1, fp);
    for (int i = 0; i < nCnt; i++) 
    {
        //2> TerrianMakeBlockInfo 블럭읽어들이기.
        //        typedef struct 
        //        {  
        //            vector<TerrianMakeMeshInfo*> Terrian;       
        //            vector<TerrianMakeMeshInfo*> NorObjects;
        //            vector<TerrianMakeMeshInfo*> NorTransObjects;
        //        }TerrianMakeBlockInfo;
        
        TerrianMakeBlockInfo* BlockInfo = pMakeInfo->Blocks[i];
        
        unsigned char cIsInfo = 1;
        if(BlockInfo == NULL) cIsInfo = 0;
        
        fwrite(&cIsInfo, 1, 1, fp); //정보가 존재하는지의 유무
        if(cIsInfo == 0) 
            continue; //정보가 없다.
        
        //cInfo
        fwrite(&(BlockInfo->cCanMoving),1,1,fp);
    
        //Terrian
        unsigned char cMeshCnt = BlockInfo->Terrian.size();
        fwrite(&cMeshCnt, 1, 1, fp);
        for (int j = 0; j < cMeshCnt; j++) 
        {
            TerrianMakeMeshInfo * MeshInfo = BlockInfo->Terrian[j];
            SaveMakeMeshInfo(fp,MeshInfo);
        }
        
        //NorObjects
        cMeshCnt = BlockInfo->NorObjects.size();
        fwrite(&cMeshCnt, 1, 1, fp);
        for (int j = 0; j < cMeshCnt; j++) 
        {
            TerrianMakeMeshInfo * MeshInfo = BlockInfo->NorObjects[j];
            SaveMakeMeshInfo(fp,MeshInfo);
        }
        
        //NorTransObjects
        cMeshCnt = BlockInfo->NorTransObjects.size();
        fwrite(&cMeshCnt, 1, 1, fp);
        for (int j = 0; j < cMeshCnt; j++) 
        {
            TerrianMakeMeshInfo * MeshInfo = BlockInfo->NorTransObjects[j];
            SaveMakeMeshInfo(fp,MeshInfo);
        }
        
        //OtherInfo
        cMeshCnt = BlockInfo->OtherInfo.size();
        fwrite(&cMeshCnt, 1, 1, fp);
        for (int j = 0; j < cMeshCnt; j++) 
        {
            TerrianMakeMeshInfo * MeshInfo = BlockInfo->OtherInfo[j];
            SaveMakeMeshInfo(fp,MeshInfo);
        }
    }
    
    fclose(fp);
    return E_SUCCESS;
}

TerrianMakeBlockInfo* CSglTerrian::CloneMakeBlockInfo(TerrianMakeBlockInfo* BlockInfo)
{
    TerrianMakeBlockInfo* pNewBlockInfo = new TerrianMakeBlockInfo;
    pNewBlockInfo->cCanMoving = BlockInfo->cCanMoving; //일반
    
    vector<TerrianMakeMeshInfo*>::iterator b = BlockInfo->Terrian.begin();
    vector<TerrianMakeMeshInfo*>::iterator e = BlockInfo->Terrian.end();
    
    for (vector<TerrianMakeMeshInfo*>::iterator it = b; it != e; it++) 
    {
        TerrianMakeMeshInfo* pNew = new TerrianMakeMeshInfo;
        pNew->cType = (*it)->cType;
        memcpy(pNew->fPosition, (*it)->fPosition, sizeof(float) * 3);
        memcpy(pNew->fRotation, (*it)->fRotation, sizeof(float) * 3);
        
        pNew->sFileName = 0;
        if((*it)->sFileName)
        {
            pNew->sFileName = new char[strlen((*it)->sFileName) + 1];
            strcpy(pNew->sFileName, (*it)->sFileName);
        }
        pNew->nReserved = (*it)->nReserved;
        pNew->cTextureCnt = (*it)->cTextureCnt;
        pNew->arrTextureFileList = 0;
        if(pNew->cTextureCnt)
        {
            pNew->arrTextureFileList = new char*[pNew->cTextureCnt];
            for (int i = 0; i < pNew->cTextureCnt; i++) {
                pNew->arrTextureFileList[i] = new char[strlen((*it)->arrTextureFileList[i]) + 1];
                strcpy(pNew->arrTextureFileList[i], (*it)->arrTextureFileList[i]);
            }
        }
        pNewBlockInfo->Terrian.push_back(pNew);
    }
    
    
    b = BlockInfo->NorObjects.begin();
    e = BlockInfo->NorObjects.end();
    for (vector<TerrianMakeMeshInfo*>::iterator it = b; it != e; it++) 
    {
        TerrianMakeMeshInfo* pNew = new TerrianMakeMeshInfo;
        pNew->cType = (*it)->cType;
        memcpy(pNew->fPosition, (*it)->fPosition, sizeof(float) * 3);
        memcpy(pNew->fRotation, (*it)->fRotation, sizeof(float) * 3);
        
        pNew->sFileName = 0;
        if((*it)->sFileName)
        {
            pNew->sFileName = new char[strlen((*it)->sFileName) + 1];
            strcpy(pNew->sFileName, (*it)->sFileName);
        }
        pNew->nReserved = (*it)->nReserved;
        pNew->cTextureCnt = (*it)->cTextureCnt;
        if(pNew->cTextureCnt)
        {
            pNew->arrTextureFileList = new char*[pNew->cTextureCnt];
            for (int i = 0; i < pNew->cTextureCnt; i++) {
                pNew->arrTextureFileList[i] = new char[strlen((*it)->arrTextureFileList[i]) + 1];
                strcpy(pNew->arrTextureFileList[i], (*it)->arrTextureFileList[i]);
            }
        }
        pNewBlockInfo->NorObjects.push_back(pNew);
    }
    
    
    b = BlockInfo->NorTransObjects.begin();
    e = BlockInfo->NorTransObjects.end();
    for (vector<TerrianMakeMeshInfo*>::iterator it = b; it != e; it++) 
    {
        TerrianMakeMeshInfo* pNew = new TerrianMakeMeshInfo;
        pNew->cType = (*it)->cType;
        memcpy(pNew->fPosition, (*it)->fPosition, sizeof(float) * 3);
        memcpy(pNew->fRotation, (*it)->fRotation, sizeof(float) * 3);
        
        pNew->sFileName = 0;
        if((*it)->sFileName)
        {
            pNew->sFileName = new char[strlen((*it)->sFileName) + 1];
            strcpy(pNew->sFileName, (*it)->sFileName);
        }
        pNew->nReserved = (*it)->nReserved;
        pNew->cTextureCnt = (*it)->cTextureCnt;
        if(pNew->cTextureCnt)
        {
            pNew->arrTextureFileList = new char*[pNew->cTextureCnt];
            for (int i = 0; i < pNew->cTextureCnt; i++) {
                pNew->arrTextureFileList[i] = new char[strlen((*it)->arrTextureFileList[i]) + 1];
                strcpy(pNew->arrTextureFileList[i], (*it)->arrTextureFileList[i]);
            }
        }
        pNewBlockInfo->NorTransObjects.push_back(pNew);
    }
    
    
    
    b = BlockInfo->OtherInfo.begin();
    e = BlockInfo->OtherInfo.end();
    for (vector<TerrianMakeMeshInfo*>::iterator it = b; it != e; it++) 
    {
        TerrianMakeMeshInfo* pNew = new TerrianMakeMeshInfo;
        pNew->cType = (*it)->cType;
        memcpy(pNew->fPosition, (*it)->fPosition, sizeof(float) * 3);
        memcpy(pNew->fRotation, (*it)->fRotation, sizeof(float) * 3);
        
        pNew->sFileName = 0;
        if((*it)->sFileName)
        {
            pNew->sFileName = new char[strlen((*it)->sFileName) + 1];
            strcpy(pNew->sFileName, (*it)->sFileName);
        }
        pNew->nReserved = (*it)->nReserved;
        pNew->cTextureCnt = (*it)->cTextureCnt;
        if(pNew->cTextureCnt)
        {
            pNew->arrTextureFileList = new char*[pNew->cTextureCnt];
            for (int i = 0; i < pNew->cTextureCnt; i++) {
                pNew->arrTextureFileList[i] = new char[strlen((*it)->arrTextureFileList[i]) + 1];
                strcpy(pNew->arrTextureFileList[i], (*it)->arrTextureFileList[i]);
            }
        }
        pNewBlockInfo->OtherInfo.push_back(pNew);
    }
    
    return pNewBlockInfo;
}

void CSglTerrian::ClearMakeMeshInfo(TerrianMakeMeshInfo* MeshInfo)
{
    if(MeshInfo->sFileName)
        delete [] MeshInfo->sFileName;
    
    if(MeshInfo->arrTextureFileList)
    {
        for (int i = 0; i < MeshInfo->cTextureCnt; i++) 
        {
            delete [] MeshInfo->arrTextureFileList[i];
        }
        if(MeshInfo->cTextureCnt>0)
            delete[] MeshInfo->arrTextureFileList;
    }
    MeshInfo->cTextureCnt = 0;
    delete MeshInfo;

}
void CSglTerrian::ClearMakeMeshInfoList(vector<TerrianMakeMeshInfo*>& MeshInfoList)
{
    vector<TerrianMakeMeshInfo*>::iterator b1 = MeshInfoList.begin();
    vector<TerrianMakeMeshInfo*>::iterator e1 = MeshInfoList.end();
    for (vector<TerrianMakeMeshInfo*>::iterator it1 = b1; it1 != e1; it1++) 
    {
        TerrianMakeMeshInfo * MeshInfo = *it1;
        ClearMakeMeshInfo(MeshInfo);
//        MeshInfoList.erase(it1);
    }
    MeshInfoList.clear();
}

void CSglTerrian::ClearMakeInfo(TerrianMakeInfo* pMakeInfo) //메모리를 제거한다.
{  
    vector<TerrianMakeBlockInfo*>::iterator b = pMakeInfo->Blocks.begin();
    vector<TerrianMakeBlockInfo*>::iterator e = pMakeInfo->Blocks.end();
    for (vector<TerrianMakeBlockInfo*>::iterator it = b; it != e; it++) 
    {
        TerrianMakeBlockInfo* BlockInfo = *it;
        if(BlockInfo == NULL) continue;
        
        ClearMakeMeshInfoList(BlockInfo->Terrian);
        ClearMakeMeshInfoList(BlockInfo->NorObjects);
        ClearMakeMeshInfoList(BlockInfo->NorTransObjects);
        ClearMakeMeshInfoList(BlockInfo->OtherInfo);
        delete BlockInfo;
        //Deleted By Song 2013.06.27 Memory Reack
//        pMakeInfo->Blocks.erase(it);
    }
    
    if(pMakeInfo->sFileName)
        delete [] pMakeInfo->sFileName;
    pMakeInfo->sFileName = 0;
    
    if(pMakeInfo->sBackgroundTexture)
        delete[] pMakeInfo->sBackgroundTexture;
    pMakeInfo->sBackgroundTexture = 0;
    
    
    pMakeInfo->Blocks.clear();
}

TerrianMakeInfo* CSglTerrian::DefaultMakeInf(int nWidth,int nHeight)
{
    TerrianMakeInfo* MakeInf = new TerrianMakeInfo;
    MakeInf->cType = 1;
    MakeInf->cVersion = SGL_SON_VERSION;
    MakeInf->nReserved = 1;
    MakeInf->sFileName = 0;
    MakeInf->shWidth = nWidth;
    
    MakeInf->sBackgroundTexture = 0;
    memset(MakeInf->fBackgroundPosition,0,sizeof(MakeInf->fBackgroundPosition));
    memset(MakeInf->fBackgroundRotation,0,sizeof(MakeInf->fBackgroundRotation));
    
    MakeInf->fFogColor[0] = 0.0f;
    MakeInf->fFogColor[1] = 0.0f;
    MakeInf->fFogColor[2] = 0.0f;
    MakeInf->fFogColor[3] = 1.0f;
    
    MakeInf->fFogStart = 0.0f;
    MakeInf->fFogEnd = 0.0f;
    MakeInf->fFogDentity = 0.0f;
    MakeInf->fTerrianShadow = NONE_VALUE;
    
    for (int j = 0; j < nHeight; j++) {
        for (int i = 0; i < nWidth; i++) {
            MakeInf->Blocks.push_back(NULL);
        }
    }
    return MakeInf;
}


#ifdef ANDROID
void CSglTerrian::ResetTexture()
{

//    vector<CMS3DASCII*>        m_TerGround;
//    vector<CMS3DASCII*>        m_TerObject;
//    vector<CMS3DASCII*>        m_TerTranObject;
    
    int nCnt = m_TerGround.size();
    for(int i = 0; i < nCnt; i++)
    {
        m_TerGround[i]->ResetTexture();
    }
    
    nCnt = m_TerObject.size();
    for(int i = 0; i < nCnt; i++)
    {
        m_TerObject[i]->ResetTexture();
    }
    
    nCnt = m_TerTranObject.size();
    for(int i = 0; i < nCnt; i++)
    {
        m_TerTranObject[i]->ResetTexture();
    }
    
}
#endif