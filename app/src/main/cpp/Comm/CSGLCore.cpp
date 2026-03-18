//
//  CSGLCore.cpp
//  SongGL
//
//  Created by Songs on 11. 5. 13..
//  Copyright 2011 thinkm. All rights reserved.
//
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include "CSGLCore.h"
#include "sGLUtils.h"
#include "sGLTrasform.h"
#include "CTextureMan.h"
#include "CSglTerrian.h"
#include "CSGLController.h"
#include "CSoundMan.h"
#include <pthread.h>
#include <unistd.h> 
#include "CPicking.h"
#include "CEMainBlzSprite.h"
#include "CK9Sprite.h"
#include "CProperty.h"
#include "CMS3DModel.h"
#include "CModelBound.h"
#include "CMS3DASCIIMan.h"
#include "CArrowMotionSprite.h"
#include "CET1.h"
#include "CET2.h"
#include "CET8.h"
#include "CET9.h"
#include "CItemMan.h"
#include "CDTower.h"
#include "CDMainTower.h"
#include "CDGunTower.h"
#include "CFighter.h"
#include "CNewTank.h"
#include "CEF4Fighter.h"
#include "CMissileTank.h"
#include "CArmeredCar.h"
#include "CAmbulance.h"
#include "CMutiplyProtocol.h"
//#define SGL_INFO
#ifndef max
#define max( a, b ) ( ((a) > (b)) ? (a) : (b) )
#endif

#ifndef min
#define min( a, b ) ( ((a) < (b)) ? (a) : (b) )
#endif


int CSGLCore::m_gTerrianSize = 0;
unsigned int      CSGLCore::mnEnemyMainTowerMaxID = SGL_TEAM2 << 15;
CSGLCore::CSGLCore(CWorld* pWorld,bool bGLMake)
{
    m_pBlockTerrian = 0;
    m_pSoundMan = 0;
    mpActor = NULL;
    m_pTextureMan = new CTextureMan;
    m_pModelMan = new CModelMan(m_pTextureMan);
    mpAction = new CSGLController((IHWorld*)pWorld);
    mpWorld = pWorld;
    msTerrianFilePath= NULL;
    mbGLMake = bGLMake;
    mpPicking = NULL;
    
    m_VisibleBlockTerrianCnt = 0;
//    memset(m_VisibleBlockTerrian, 0, sizeof(m_VisibleBlockTerrian));
    mMapWidthIndex = 0;
    mMapHeightIndex = 0;
    m_pMakeTerrinInfo = NULL;
    
    m_pAsciiModelMan = new CMS3DASCIIMan(m_pTextureMan);
//    m_pMS3DModelASCIIMan = NULL;
    mpItemMan = new CItemMan(pWorld);
    mBeforePT.x = -99999;
    
    
    CSGLCore::mnEnemyMainTowerMaxID = SGL_TEAM2 << 15;
    
    CMutiplyProtocol* pMul = pWorld->GetMutiplyProtocol();
    if(pMul->GetEnemyMainTowerStartIndex() != 0)
        CSGLCore::mnEnemyMainTowerMaxID = pMul->GetEnemyMainTowerMaxID() - 1;

    pthread_mutex_init(&mHashMutex, NULL);
}


CSGLCore::~CSGLCore()
{
    Clear();
    
    
    pthread_mutex_lock(&mHashMutex); //락
    mHashSprite.clear();
    pthread_mutex_unlock(&mHashMutex); //언락
    pthread_mutex_destroy(&mHashMutex);
    
    if(mpItemMan)
    {
        delete mpItemMan;
        mpItemMan= NULL;
    }
    
    if(m_pAsciiModelMan)
    {
        delete m_pAsciiModelMan;
        m_pAsciiModelMan = NULL;
    }
    
    if(msTerrianFilePath)
    {
        delete[] msTerrianFilePath;
        msTerrianFilePath = NULL;
    }
    
    if(m_pModelMan)
    {
        delete m_pModelMan;
        m_pModelMan = 0;
    }
    
    if(m_pSoundMan)
    {
        delete m_pSoundMan;
        m_pSoundMan = 0;
    }
    
    if(m_pTextureMan)
    {
        delete m_pTextureMan;
        m_pTextureMan = 0;
    }

    
    if(m_pBlockTerrian)
    {
        delete m_pBlockTerrian;
        m_pBlockTerrian = 0;
    }
    
    if(mpAction)
    {
        delete mpAction;
        mpAction = 0;
    }
    
    if(gpIndicesBuffer)
    {
        delete [] gpIndicesBuffer;
        gpIndicesBuffer = 0;
    }
    
    int nSize = (int)mlstStartPoint.size();
    if(nSize > 0)
    {
        
        for (int i = nSize - 1; i >= 0; i--) 
        {
            delete mlstStartPoint[i];
        }
        mlstStartPoint.clear();
    }

    
    nSize = (int)mlstAddedSprite.size();
    if(nSize > 0)
    {
        
        for (int i = nSize - 1; i >= 0; i--) 
        {
            delete mlstAddedSprite[i];
        }
        mlstAddedSprite.clear();
    }
    
    if(m_pMakeTerrinInfo)
    {
        CSglTerrian::ClearMakeInfo(m_pMakeTerrinInfo);
        delete m_pMakeTerrinInfo;
        m_pMakeTerrinInfo = NULL;
    }
    
    
    
//    if(m_pMS3DModelASCIIMan)
//    {
//        delete m_pMS3DModelASCIIMan;
//        m_pMS3DModelASCIIMan= 0;
//    }
}

void CSGLCore::Clear()
{
    pthread_mutex_lock(&mHashMutex); //락
    mHashSprite.clear();
    pthread_mutex_unlock(&mHashMutex); //언락
    
    int nCnt = mlstSprite.Size();
	for(int i = 0; i < nCnt; i++)
	{
		CSprite* it = mlstSprite.GetAt(i);
		delete it;
	}
	mlstSprite.Clear();	
    
  
	
	nCnt = mlstBomb.Size();
	for(int i = 0; i < nCnt; i++)
	{
		CBomb* it = mlstBomb.GetAt(i);
		delete it;
	}
	mlstBomb.Clear();
    
    
    nCnt = mlstParticle.Size();
	for(int i = 0; i < nCnt; i++)
	{
		ARender* it = mlstParticle.GetAt(i);
		delete it;
	}
	mlstParticle.Clear();
    
//    if(m_pMS3DModelASCIIMan)
//    {
//        delete m_pMS3DModelASCIIMan;
//        m_pMS3DModelASCIIMan = 0;
//    }
    
    
    if(m_pTextureMan)
    {
        m_pTextureMan->Clear();
    }
    
    if(m_pSoundMan)
    {
        m_pSoundMan->Close();
    }
    
    if(m_pModelMan)
    {
        m_pModelMan->deleteAllModelCoresf();
    }

    
    if(m_pBlockTerrian)
    {
        int nSize = m_pBlockTerrian->Size();
        for (int i = 0; i < nSize; i++) {
            CSglTerrian *pTerrian = m_pBlockTerrian->GetAt(i);
            delete pTerrian;
        }
        
        m_pBlockTerrian->Clear();
    }
    
 
    
    m_fFogDentity = 0.0f;
    memset(m_fFogColor, 0, sizeof(m_fFogColor));
    m_fFogStart = 0.0f;
    m_fFogEnd = 0.0f;
    
    m_fTerrianShadow = NONE_VALUE;
    
}


void* LoadTerrianThread(void *data)
{
    CSGLCore* pCore = (CSGLCore*)data;
    pCore->LoadTerrianOrgEx(pCore->msTerrianFilePath);
    return NULL;
}

int CSGLCore::LoadTerrian(const char* sFileName)
{
    if(msTerrianFilePath)
    {
        delete[] msTerrianFilePath;
        msTerrianFilePath = NULL;
    }
    
    msTerrianFilePath = new char[strlen(sFileName) + 1];
    strcpy(msTerrianFilePath, sFileName);
    
    
    if(m_pSoundMan)
    {
        delete m_pSoundMan;
        m_pSoundMan = NULL;
    }

    if(mbGLMake == false)
    {
        m_pSoundMan = new CSoundMan;
        m_pSoundMan->Initialize(1);
    }
//    m_pMS3DModelASCIIMan = new CMS3DModelASCIIMan(m_pTextureMan);
    if(mbGLMake == false)
    {
        pthread_t nThreadID;
        int nThreadResult = pthread_create(&nThreadID, 0, (void* (*)(void*))LoadTerrianThread, (void*)this);
        if(nThreadResult != 0)
        {
//            nThreadResult = 0;
            HLogE("[Error] : Create Thread LoadTerrian\n");
            return -1;
        }
    }
    else
    {
        int nResult =  LoadTerrianOrgEx(msTerrianFilePath);
        if(nResult != E_SUCCESS) return nResult;
    }
    return E_SUCCESS;
    
}

//eㅣ자인할때 사용한다.
int gnIndicesCnt = 0;
GLushort *gpIndicesBuffer = 0;

void CSGLCore::ClearDesignMakeInfo()
{   
    if (m_pMakeTerrinInfo) 
    {
        CSglTerrian::ClearMakeInfo(m_pMakeTerrinInfo);
        delete m_pMakeTerrinInfo;
        m_pMakeTerrinInfo = NULL;
    }
    
    if(m_pAsciiModelMan)
    {
        delete m_pAsciiModelMan;
        m_pAsciiModelMan = NULL;
    }
    
}

int CSGLCore::LoadTerrianOrgEx(const char* sFileName,int nWidth,int nHeight)
{
    sglTexture texture;
    char sImageFileName[512];
    
    if (m_pMakeTerrinInfo) 
    {
        CSglTerrian::ClearMakeInfo(m_pMakeTerrinInfo);
        delete m_pMakeTerrinInfo;
        m_pMakeTerrinInfo = NULL;
    }
        
    
    
    //테리안 정보 파일을 읽어온다----------------------------------------
    if(sFileName)
    {
        m_pMakeTerrinInfo = CSglTerrian::LoadMakeInfo(sFileName);
        if(m_pMakeTerrinInfo == NULL)
        {
            HLogE("[Error] 파일을 찾을 수가 없습니다.");
            return -1;
        }
    }
    else 
        m_pMakeTerrinInfo = CSglTerrian::DefaultMakeInf(nWidth, nHeight);
    
    
    if(m_pMakeTerrinInfo->sFileName) //지형파일이 존재한다면...
    {
    
        sprintf(sImageFileName, "%s%s",CLoader::getSearchPath(),m_pMakeTerrinInfo->sFileName);
    
        //false 원본이미지로 읽어온다.
        if(CTextureMan::LoadPNG(sImageFileName, texture, false) != 0)
        {
            CSglTerrian::ClearMakeInfo(m_pMakeTerrinInfo);
            delete m_pMakeTerrinInfo;
            m_pMakeTerrinInfo = NULL;
            return -1;
        }
        
        if(texture.nWidth != m_pMakeTerrinInfo->shWidth)
        {
            
            HLogE("[Error] 지형 이미지와 지형 정보의 사이즈가 다르다.");
            if(texture.pImageBuffer)
                free(texture.pImageBuffer);

            CSglTerrian::ClearMakeInfo(m_pMakeTerrinInfo);
            delete m_pMakeTerrinInfo;
            m_pMakeTerrinInfo = NULL;
            
            return -1;
        }
        
        if(texture.nWidth != texture.nHeight)
        {
            HLogE("[Error] 이미지 사이즈의 넓이와 높이가 다르다");
            if(texture.pImageBuffer)
                free(texture.pImageBuffer);
            
            CSglTerrian::ClearMakeInfo(m_pMakeTerrinInfo);
            delete m_pMakeTerrinInfo;
            m_pMakeTerrinInfo = NULL;
            
            return -1;
        }
        
        if( texture.nWidth % 2 == 0) 
        {
            HLogE("[Error] 이미지의 크기는 홀수로 끝나야 한다.");
            if(texture.pImageBuffer)
                free(texture.pImageBuffer);
            
            CSglTerrian::ClearMakeInfo(m_pMakeTerrinInfo);
            delete m_pMakeTerrinInfo;
            m_pMakeTerrinInfo = NULL;
            
            return -2;
        }
        
        if( (texture.nWidth - 1) % GROUP_SIZE != 0)
        {
            HLogE("[Error] 그룹의 사이즈는 (nImageWidth - 1)/nGroupsize = 0.0f으로 끝나야한다.");
            if(texture.pImageBuffer)
                free(texture.pImageBuffer);
            
            CSglTerrian::ClearMakeInfo(m_pMakeTerrinInfo);
            delete m_pMakeTerrinInfo;
            m_pMakeTerrinInfo = NULL;
            
            return -3;
        }
    }
    else 
    {
        //이미지가 없으면 son의 크기에 맞게 이미지 파일을 만든다.
        texture.nWidth  = m_pMakeTerrinInfo->shWidth;
        texture.nHeight = m_pMakeTerrinInfo->shWidth;
        texture.nDepth  = 8;
        texture.pImageBuffer = (unsigned char*)malloc(texture.nWidth * texture.nHeight);
        memset(texture.pImageBuffer, H_TERRIAN_DEFAULT_HIGHT*5, texture.nWidth*texture.nHeight);
    }
    
    /////////////////////////////////////////////////////////
    memcpy(m_fFogColor, m_pMakeTerrinInfo->fFogColor, sizeof(m_pMakeTerrinInfo->fFogColor));
    m_fFogStart = m_pMakeTerrinInfo->fFogStart;
    m_fFogEnd = m_pMakeTerrinInfo->fFogEnd;
    m_fFogDentity = m_pMakeTerrinInfo->fFogDentity;
    m_fTerrianShadow = m_pMakeTerrinInfo->fTerrianShadow;
    /////////////////////////////////////////////////////////
    

    if(m_pBlockTerrian)
    {
        int nSize = m_pBlockTerrian->Size();
        for (int i = 0; i < nSize; i++) {
            CSglTerrian *pTerrian = m_pBlockTerrian->GetAt(i);
            delete pTerrian;
        }
        
        m_pBlockTerrian->Clear();
        delete m_pBlockTerrian;
        m_pBlockTerrian = NULL;
    }
    
    m_pBlockTerrian = new  CListNode<CSglTerrian>;
    
    CSGLCore::m_gTerrianSize = texture.nWidth;
    
    CSglTerrian* pBlockTerrian;
    int nGX = (texture.nWidth - 1) / GROUP_SIZE;
	int nGZ = (texture.nWidth - 1) / GROUP_SIZE;
    int nVertexCnt,nNormalCnt,nGroupIndex;
    GLfloat *pVertexBuffer;
    GLfloat *pNormalBuffer;
    int nCountTotal = nGZ * nGX;
    int nGroupSize_fromzero = GROUP_SIZE + 1;
    int n2 = nCountTotal / 6;
    int n3 = n2*2;
    int n4 = n2*3;
    int n5 = n2*4;
    int n6 = n2*5;
    int n7 = n2*6;
    int nMapPos = 0;
    int nBytePerPixel = texture.nDepth / 8;
    int nVertexIndex = 0;
    int nNormalIndex = 0;
    int nIndicesIndex = 0;
    unsigned char* pAddress = 0;
    float fCenterPos = (float)(texture.nWidth - 1)/2.0f;
    float fy = 0.0f;
    float fyValue = 0.0f;
    float fTerrianZMin = 9999.0;
    TerrianMakeBlockInfo *BlockInfo = NULL;
    bool  bFlat = false;
//    SPoint ptBlockCenterPosition;
    SVector hNormal;
    
    
    mMapWidthIndex = nGX;
    mMapHeightIndex = nGZ;
    
	for(int gz = 0; gz < nGZ; gz++)
	{
		for(int gx = 0; gx < nGX; gx++)
		{
            
            pBlockTerrian = new CSglTerrian(mpWorld,m_pTextureMan);
            nGroupIndex = gz * nGZ + gx;
            pBlockTerrian->m_nTotalMapIndex = nGroupIndex;
            
            
            BlockInfo = m_pMakeTerrinInfo->Blocks[nGroupIndex];
            
            
            
            
            if(mbGLMake == false)
            {   
                //Progress 2단계.
                if(n2 == nGroupIndex) CScenario::SendMessage(SGL_MESSAGE_PROGRESS_STEP);
                //Progress 3단계.
                else if(n3 == nGroupIndex) CScenario::SendMessage(SGL_MESSAGE_PROGRESS_STEP);
                //Progress 4단계.
                else if(n4 == nGroupIndex) CScenario::SendMessage(SGL_MESSAGE_PROGRESS_STEP);
                //Progress 5단계.
                else if(n5 == nGroupIndex) CScenario::SendMessage(SGL_MESSAGE_PROGRESS_STEP);
                //Progress 6단계.
                else if(n6 == nGroupIndex) CScenario::SendMessage(SGL_MESSAGE_PROGRESS_STEP);
                //Progress 7단계.
                else if(n7 == nGroupIndex) CScenario::SendMessage(SGL_MESSAGE_PROGRESS_STEP);
                
                
                //실제 게임에서는 블럭의 데이터가 존재 하지 않으면 스킵한다.
                if(BlockInfo == NULL) 
                {
                    pBlockTerrian->SetFlat(true);
                    m_pBlockTerrian->AddTTail(pBlockTerrian);
                    continue;
                }

            }

            //4=>3 정점으로 변경함
			nVertexCnt = nGroupSize_fromzero * nGroupSize_fromzero;
			pVertexBuffer = new GLfloat[nVertexCnt * 3];
			
			nNormalCnt = nGroupSize_fromzero * nGroupSize_fromzero * 2;
			pNormalBuffer = new GLfloat[nNormalCnt * 3];
            
			nMapPos = 
			(gz * nGroupSize_fromzero - (1 * gz)) * texture.nWidth * nBytePerPixel +
			(gx * nGroupSize_fromzero - (1 * gx)) * nBytePerPixel ;

            pBlockTerrian->SetHeightVertexCnt(nVertexCnt);
            pBlockTerrian->SetHeightVertex(pVertexBuffer);
            
			pBlockTerrian->SetHeightNormalCnt(nNormalCnt);
            pBlockTerrian->SetHeightNormal(pNormalBuffer);
            
            if(mbGLMake == true && gx == 0 && gz == 0 && gpIndicesBuffer == NULL)
            {
                gnIndicesCnt = GROUP_SIZE * GROUP_SIZE * 6; 
                gpIndicesBuffer = new GLushort[gnIndicesCnt];
            }
            
            pBlockTerrian->SetFlat(false);
            
            if(BlockInfo && BlockInfo->cCanMoving & 0x01)
                pBlockTerrian->SetBlockType(pBlockTerrian->GetBlockType() | 0x0100);
            else if(BlockInfo && BlockInfo->cCanMoving & 0x02)
                pBlockTerrian->SetBlockType(pBlockTerrian->GetBlockType() | 0x0200);
            
            nVertexIndex = 0;
            fTerrianZMin = 9999.0;
            bFlat        = true;
            
            //0 좌표로 중앙을 나타내려면 nWidth - 1 ( ex> 0 - 1 - 3) 		
			for(int z = 0; z < nGroupSize_fromzero; z++)
			{				
				for(int x = 0;x < nGroupSize_fromzero; x++)
				{
                    pAddress = (unsigned char*)(texture.pImageBuffer+ 
												nMapPos + 
												(z * texture.nWidth + x) * nBytePerPixel);
					
					fy = (GLfloat)*(pAddress);
					pVertexBuffer[nVertexIndex++] = 
					(- fCenterPos + (gx * nGroupSize_fromzero - (1 * gx)) + x) ;	//중앙으로 이동한다.
                    
                    fyValue=  fy / 5;//단계로 정의 //* MAX_OBJ_SIZE * 3 / 255.0f;
					pVertexBuffer[nVertexIndex++] =  fyValue;

                    
					pVertexBuffer[nVertexIndex++] =
					(-fCenterPos + (gz * nGroupSize_fromzero - (1 * gz) + z)) ;	//중앙으로 이동한다.
                    
                    
                    //가장 낮은 위치에 건물을 건설해야 한다.
                    //건설부지의 땅의 좌표를 알아온다.
                    if(fyValue < fTerrianZMin)
                        fTerrianZMin = fyValue;
                    
                    //울퉁불퉁한 지형인지를 알아온다.
                    //if(nGType == H_GTYPE_FLATGROUNT_NORMAL && fzValue != H_TERRIAN_DEFAULT_HIGHT)
                    if(fyValue != H_TERRIAN_DEFAULT_HIGHT)
                    {
                        bFlat = false;
                    }
                }
            }
            
            
            pBlockTerrian->SetFlat(bFlat);
            
            //메모리를 절약하기 위해서 
            //정보가 있고 울툴불퉁 하거나 디자인일때만 노멀 벡터를 만들어서 ....
            if(mbGLMake == true || (pBlockTerrian->GetFlat() == false && BlockInfo))
            {
                nNormalIndex = 0;
                nIndicesIndex = 0;
                
                // 범위를 nGroupWidth nGroupHeight로 하지 않은이유는
                // 0 위치에 다음 x + 1, y + 1을 하여 계산을 하기 때문에 0,1,2 까지 하면 에러가 난다. 
                for( int z = 0; z < GROUP_SIZE; z++)
                {
                    for(int x = 0;x < GROUP_SIZE; x++)
                    {
                        
                        //1> 법선벡터
                        //0---1 (0,1,2)
                        //|  /
                        //2
                        int nA = (z* nGroupSize_fromzero + x) * 3;// + nMapPos;
                        int nB = (z* nGroupSize_fromzero + x + 1) * 3;// + nMapPos;
                        
                        //4=>3 정점으로 변경함
                        int nC = ((z+1) * nGroupSize_fromzero + x) * 3;// + nMapPos;
                        
                        sglNormalVector(&hNormal, 
                                        pVertexBuffer[nA], pVertexBuffer[nA+1], pVertexBuffer[nA+2], 
                                        pVertexBuffer[nB], pVertexBuffer[nB+1], pVertexBuffer[nB+2], 
                                        pVertexBuffer[nC], pVertexBuffer[nC+1], pVertexBuffer[nC+2]);
                        
                        pNormalBuffer[nNormalIndex++] = hNormal.x;
                        pNormalBuffer[nNormalIndex++] = hNormal.y;
                        pNormalBuffer[nNormalIndex++] = hNormal.z;
                        
                        if(mbGLMake == true && gx == 0 && gz == 0)
                        {
                            //반시계..
                            gpIndicesBuffer[nIndicesIndex++] = z * nGroupSize_fromzero + x + 1;
                            gpIndicesBuffer[nIndicesIndex++] = z * nGroupSize_fromzero + x;
                            gpIndicesBuffer[nIndicesIndex++] = (z+1) * nGroupSize_fromzero + x;
                        }
                        
                        
                        
                        //hLog("Indi %02d = (%d,%d,%d)\n",nLogIndex++ , pIndeces[nIndex - 3],pIndeces[nIndex - 2],pIndeces[nIndex - 1]);
                        
                        //3> 법선벡터
                        //    1 (1,3,4)
                        //  / |
                        //2---3
                        //4=>3 정점으로 변경함
                        nA = (z* nGroupSize_fromzero + x + 1) * 3;// + nMapPos;
                        nB = ((z + 1)* nGroupSize_fromzero + x + 1) * 3;// + nMapPos;
                        nC = ((z + 1) * nGroupSize_fromzero + x) * 3;// + nMapPos;
                        
                        
                        sglNormalVector(&hNormal, 
                                        pVertexBuffer[nA], pVertexBuffer[nA+1], pVertexBuffer[nA+2], 
                                        pVertexBuffer[nB], pVertexBuffer[nB+1], pVertexBuffer[nB+2], 
                                        pVertexBuffer[nC], pVertexBuffer[nC+1], pVertexBuffer[nC+2]);
                        
                        pNormalBuffer[nNormalIndex++] = hNormal.x;
                        pNormalBuffer[nNormalIndex++] = hNormal.y;
                        pNormalBuffer[nNormalIndex++] = hNormal.z;
                        
                        if(mbGLMake == true && gx == 0 && gz == 0)
                        {
                            //반시계..
                            gpIndicesBuffer[nIndicesIndex++] = (z+1) * nGroupSize_fromzero + x + 1;
                            gpIndicesBuffer[nIndicesIndex++] = z* nGroupSize_fromzero + x + 1;
                            gpIndicesBuffer[nIndicesIndex++] = (z+1) * nGroupSize_fromzero + x;
                        }
                        
                    }
                } 
            }
            else  //정보가 있고 울툴불퉁 하거나 디자인일때만 노멀 벡터를 만들어서 ....
            {   
                TerrianMakeBlockInfo* pBlockInfo = m_pMakeTerrinInfo->Blocks[nGroupIndex];
                
                if(pBlockInfo == 0) //어떠한 정보도 없다면 정점 또한 메모리에서 지운다.
                {
                    if(pBlockTerrian->GetHeightVertex())
                        delete [] pBlockTerrian->GetHeightVertex();
                    pBlockTerrian->SetHeightVertex(0);
                    pBlockTerrian->SetHeightVertexCnt(0);
                }
                
                //평평한지역으로 노멀벡터를 지운다.
                if(pBlockTerrian->GetHeightNormal())
                    delete [] pBlockTerrian->GetHeightNormal();
                pBlockTerrian->SetHeightNormal(0);
                pBlockTerrian->SetHeightNormalCnt(0);
                pBlockTerrian->SetFlat(true);
                
            }
            m_pBlockTerrian->AddTTail(pBlockTerrian);
        }
    }
    
    if(texture.pImageBuffer)
        free(texture.pImageBuffer);
    
    if(mbGLMake == false)
        CScenario::SendMessage(SGL_PROGRESS_INITIALIZE_1,(long)nGX,(long)nGZ);
    
    return E_SUCCESS;
}




int CSGLCore::LoadTerrianNextEx(int nGX,int nGZ)
{
    int nGroupIndex = 0;
    CSglTerrian* pBlockTerrian;
    char *pLoadMap = 0;
    int nIndexActorPos = -9999;
//    int nIndexEMainPos = -9999;
    TerrianMakeBlockInfo *MakeBlockInfo = NULL;
    pLoadMap = new char[nGX*nGZ];
    int nCnt;
    
    for(int gz = 0; gz < nGZ; gz++)
    {
        for(int gx = 0; gx < nGX; gx++)
        {
            nGroupIndex = gz * nGZ + gx;
            pBlockTerrian = m_pBlockTerrian->GetAt(nGroupIndex);
            if(pBlockTerrian == 0) 
            {
                pLoadMap[nGroupIndex] = '@';
                continue;
            }
            
            MakeBlockInfo = m_pMakeTerrinInfo->Blocks[nGroupIndex];
            if(MakeBlockInfo == 0) 
            {
// 불필요한 메모리를 지우려면 .....
//                if(pBlockTerrian != 0)
//                {
//                    sglLog("게임 시에는 메모리에서 삭제해야한다. (pBlockTerrian)");
//                }
                pLoadMap[nGroupIndex] = '.';
                continue;
            }
            
            int nCntTer = (int)MakeBlockInfo->Terrian.size();
            for (int i = 0; i < nCntTer; i++) {
                pBlockTerrian->LoadASCII(m_pAsciiModelMan, MakeBlockInfo->Terrian[i]);
                pBlockTerrian->ArrangeLoadedASCII(this,i,MakeBlockInfo->Terrian[i],m_fTerrianShadow);
            }
            
            int nCntObj = (int)MakeBlockInfo->NorObjects.size();
            for (int i = 0; i < nCntObj; i++) {
                pBlockTerrian->LoadASCII(m_pAsciiModelMan, MakeBlockInfo->NorObjects[i]);
                pBlockTerrian->ArrangeLoadedASCII(this,i,MakeBlockInfo->NorObjects[i],m_fTerrianShadow);
            }
            
            int nCntTranObj = (int)MakeBlockInfo->NorTransObjects.size();
            for (int i = 0; i < nCntTranObj; i++) {
                pBlockTerrian->LoadASCII(m_pAsciiModelMan, MakeBlockInfo->NorTransObjects[i]);
                pBlockTerrian->ArrangeLoadedASCII(this,i,MakeBlockInfo->NorTransObjects[i],m_fTerrianShadow);
            }
            
            
            
            //시작 지점이나, 메인타워 정보등드의 값을 정의 한다.
            int nCntOtherObj = (int)MakeBlockInfo->OtherInfo.size();
            
            //객체의 위치값이 틀려지기 때문에 쉽게 지워버리자.
            if(CSglTerrian::mbChangedTerrianImgAtDesign)
            {
                for (int i = 0; i < nCntOtherObj; i++) 
                {   
                    TerrianMakeMeshInfo *pMeshInfo = MakeBlockInfo->OtherInfo[i];
                    CSglTerrian::ClearMakeMeshInfo(pMeshInfo);

                }
                MakeBlockInfo->OtherInfo.clear();
                nCntOtherObj = 0;
            }
            
            
            for (int i = 0; i < nCntOtherObj; i++) 
            {   
                TerrianMakeMeshInfo *pMeshInfo = MakeBlockInfo->OtherInfo[i];
                
                if(pMeshInfo->nReserved == ACTORID_START_POINT)
                {
                    if(mbGLMake == false)
                    {
                        //할일 : 시작지점을 기억해준다.
                        STARTPOINT *pNewPos = new STARTPOINT;
//                        SPoint ptTemp;
//                        CSglTerrian::GetIndexToCenterPoint(nGroupIndex, &ptTemp);
//                        GetPositionZ(&ptTemp);
                        pNewPos->fPosX = pMeshInfo->fPosition[0];
                        pNewPos->fPosY = pMeshInfo->fPosition[1];
                        pNewPos->fPosZ = pMeshInfo->fPosition[2];
                        
                        pNewPos->nTeamID = pMeshInfo->cType;
                        
                        pNewPos->fDirX = pMeshInfo->fRotation[0];
                        pNewPos->fDirY = pMeshInfo->fRotation[1]; 
                        pNewPos->fDirZ = pMeshInfo->fRotation[2];
                        
                        CUserInfo *pUserInfo = mpWorld->GetUserInfo();
                        int nModelID = ACTORID_K9;
                        if(pUserInfo) 
                            nModelID = pUserInfo->GetLastSelectedTankID();
                        
                        pNewPos->nModelID = nModelID;
                        mlstStartPoint.push_back(pNewPos);
                    }
                    else
                    {
                        CSprite* pSprite = MakeSprite(mpWorld->GetNewID(),pMeshInfo->cType,ACTORID_K9,GetTextureMan(),GetAction(),mpWorld,NETWORK_MYSIDE);

                        SPoint ptPos;
                        ptPos.x = pMeshInfo->fPosition[0];
                        ptPos.y = pMeshInfo->fPosition[1];
                        ptPos.z = pMeshInfo->fPosition[2];
                        
                        SVector vtDir;
//                        CSglTerrian::GetIndexToCenterPoint(nGroupIndex, &ptPos);
//                        GetPositionZ(&ptPos);

                        vtDir.x = pMeshInfo->fRotation[0];
                        vtDir.y = pMeshInfo->fRotation[1];
                        vtDir.z = pMeshInfo->fRotation[2];
                        
                        pSprite->LoadResoure(GetModelMan());
                        pSprite->Initialize(&ptPos, &vtDir);
                        AddSprite(pSprite);
                    }
                }
                else if(pMeshInfo->nReserved == ACTORID_BLZ_EMAIN)
                {
                    if(mbGLMake == false)
                    {
                        
                        //할일 : 시작지점을 기억해준다.
                        STARTPOINT *pNewPos = new STARTPOINT;
                        
//                        SPoint ptTemp;
//                        CSglTerrian::GetIndexToCenterPoint(nGroupIndex, &ptTemp);
//                        GetPositionZ(&ptTemp);
                        pNewPos->fPosX = pMeshInfo->fPosition[0];
                        pNewPos->fPosY = pMeshInfo->fPosition[1];
                        pNewPos->fPosZ = pMeshInfo->fPosition[2];
                        
                        pNewPos->nTeamID = pMeshInfo->cType;
                        
                        pNewPos->fDirX = pMeshInfo->fRotation[0];
                        pNewPos->fDirY = pMeshInfo->fRotation[1]; 
                        pNewPos->fDirZ = pMeshInfo->fRotation[2];
                        
                        
                        pNewPos->nModelID = pMeshInfo->nReserved;
                        mlstAddedSprite.push_back(pNewPos);
                    }
                    else
                    {
                        CSprite* pSprite = MakeSprite(mpWorld->GetNewID(),pMeshInfo->cType,pMeshInfo->nReserved,GetTextureMan(),GetAction(),mpWorld,NETWORK_MYSIDE);
                        
                        SPoint ptPos;
                        SVector vtDir;
                        
                        ptPos.x = pMeshInfo->fPosition[0];
                        ptPos.y = pMeshInfo->fPosition[1];
                        ptPos.z = pMeshInfo->fPosition[2];
                        
                        vtDir.x = pMeshInfo->fRotation[0];
                        vtDir.y = pMeshInfo->fRotation[1];
                        vtDir.z = pMeshInfo->fRotation[2];                        
                        pSprite->LoadResoure(GetModelMan());
                        pSprite->Initialize(&ptPos, &vtDir);
                        AddSprite(pSprite);
                    }   
                }
                

            }
            
            //땅만 존재하면... 갈수 있는 길이다.
            if(nCntTer != 0 && nCntObj == 0)// && nCntTranObj == 0)
            {
                if(pBlockTerrian && pBlockTerrian->GetBlockType() & 0x0100) //Obtacle 
                    pLoadMap[nGroupIndex] = '@'; //장애물
                else if(pBlockTerrian && pBlockTerrian->GetBlockType() & 0x0200) //Obtacle on path
                    pLoadMap[nGroupIndex] = '@'; //장애물
                else
                    pLoadMap[nGroupIndex] = '.';
                
                if(nIndexActorPos == -9999)
                {
                    nIndexActorPos = nGroupIndex; //처음.
                }
//                nIndexEMainPos = nGroupIndex; //마지막
            }
            else  //장애물
            {
                pLoadMap[nGroupIndex] = '@';
            }
        }
    }
    
    
    //추가적인 장애물 인덱스
    //장애물 블럭 인덱스 (적메인타워 같은 것은 크기 때문에 지나갈수 있는 땅이라고 생각하지 말자)
    nCnt = (int)mlstAddedSprite.size();
    for (int i = 0; i < nCnt; i++) 
    {
        STARTPOINT *pPoint = mlstAddedSprite[i];                    
        //메인타워는 커서
        if(pPoint->nModelID >= ACTORID_BLZ_EMAIN)
        {
            SPoint ptPos;
            ptPos.x = pPoint->fPosX;
            ptPos.y = pPoint->fPosY;
            ptPos.z = pPoint->fPosZ;
            
            int nGX = (CSGLCore::m_gTerrianSize - 1) / GROUP_SIZE;
            int nIndexX,nIndexY;
            CSglTerrian::GetWorldToMapIndex(&ptPos, &nIndexX, &nIndexY);
            int nBlock = ( nGX * nIndexY) + nIndexX; //이부분도 장애땅으로 생각하자.
            
            //장애물로 간주한다.
            pLoadMap[nBlock] = '@';
        }
    }
    
    
    
//    CSglTerrian::GetIndexToCenterPoint(nIndexActorPos,ptOutActor);
//    CSglTerrian::GetIndexToCenterPoint(nIndexEMainPos,ptOutEMain);
    
    char sTempLoadMap[200],sHeaderLoadMap[512];
    strcpy(sHeaderLoadMap,"type tile\n");
    sprintf(sTempLoadMap, "height %d\n",nGZ);
    strcat(sHeaderLoadMap, sTempLoadMap);
    sprintf(sTempLoadMap, "width %d\n",nGX);
    strcat(sHeaderLoadMap, sTempLoadMap);
    strcat(sHeaderLoadMap,"map\n");
    
    //nGY '\n'의 갯수..
    char* mpLoadMap = new char[nGZ*nGX + strlen(sHeaderLoadMap) + nGZ + 20];
    strcpy(mpLoadMap,sHeaderLoadMap);
    for(int gz = 0; gz < nGZ; gz++)
    {
        for(int gx = 0; gx < nGX; gx++)
        {
            nGroupIndex = gz * nGZ + gx;
            pBlockTerrian = m_pBlockTerrian->GetAt(nGroupIndex);
            if(pBlockTerrian == 0) 
            {
                sTempLoadMap[0] = pLoadMap[gz * nGZ + gx];
                sTempLoadMap[1] = 0;
                strcat(mpLoadMap, sTempLoadMap);
#ifdef DEBUG_MSG_FINDPATH
                HLog(sTempLoadMap);
#endif
                continue;
            }
            pBlockTerrian->ArrangeClearBuffer(); //ArrangeLoadedASCII 에서 버퍼를 지우면 에러가 난다.
            
            sTempLoadMap[0] = pLoadMap[gz * nGZ + gx];
            sTempLoadMap[1] = 0;
            strcat(mpLoadMap, sTempLoadMap);
#ifdef DEBUG_MSG_FINDPATH
            HLog(sTempLoadMap);
#endif
        }
        sTempLoadMap[0] = '\n';
        sTempLoadMap[1] = 0;
        strcat(mpLoadMap, sTempLoadMap);
#ifdef DEBUG_MSG_FINDPATH
        HLog(sTempLoadMap);
#endif
    }
    
    int nLastC = (int)strlen(mpLoadMap);
    mpLoadMap[nLastC - 1] = 0; //마지막문자가 /n인것을 0으로 변경한다.
    //버퍼가 1024이다. sglLog(mpLoadMap);
    CSglTerrian::LoadMap(mpLoadMap);
    delete[] mpLoadMap;
    
    
    //아이템 메니져를 초기화 한다.
    mpItemMan->Initialize();
    return E_SUCCESS;
}





int CSGLCore::GetPositionY(SPoint	*pPoint,bool bMovableCheck,SVector* pOutNormalVector,int* pOutGIndex)
{
    float fY;
    int nResult = E_SUCCESS;
    //int		nGroupSize = GROUP_SIZE;
    SVector	vViewDirection;
	SVector normalizeDirection;
    //Output
    SPoint	MapPoint;
    int   nMapGroupIndexX;
    int   nMapGroupIndexZ;
    int   Direction;
    
    int   nIndexVertexX;
    int   nIndexVertexZ;
    
    
	float fIndexVertexX = 0.0f,fIndexVertexZ = 0.0f;
	float fUpDown = 0.0f; //fUpDown < 0 상단,  fUpDown > 0 하단
	int GI = -1;
    CSglTerrian* pBlockTerrian = NULL;
	
    //그룹의 블럭사이즈. 사이즈 ....
    
    int  nGroupWidth = (CSGLCore::m_gTerrianSize - 1) / GROUP_SIZE;
    
    //이방향이 정확하다.
    vViewDirection.x = 1;
    vViewDirection.y = 0;
    vViewDirection.z = 0;
    
    
	//1> 실제 자표에서 맵좌표로 변경한다.(범위에서 벗어났다.)
	nResult = CSglTerrian::GetWorldToBlock(pPoint,&MapPoint);
	if(nResult != E_SUCCESS) 
        return nResult;
	
    
	sglCopyVector(&vViewDirection,&normalizeDirection);
	normalizeDirection.y = 0; //순수하게 x/y 에 관련 된 값만 계산한다.
	sglNormalize(&normalizeDirection);
	
	//pvViewDirection : Normalize를 한상태이다.
	Direction = CSglTerrian::GetDirection(&normalizeDirection);
	
	//2> 기타 그룹 정보를 가져온다.
    CSglTerrian::GetMapGroupIndexToMapUnit(
                                           Direction, 
                                           &MapPoint,
                                           &nMapGroupIndexX,
                                           &nMapGroupIndexZ,
                                           &nIndexVertexX,
                                           &nIndexVertexZ,
                                           &fIndexVertexX,
                                           &fIndexVertexZ,
                                           &fUpDown);
    
	if(nMapGroupIndexX < 0) 
    {
        HLog("[Error] 방향이 0일경우 -3\n");
        return -3; //방향이 0일 경우   
    }
    
    
    //GI = ( nGroupSize * nMapGroupIndexY) + nMapGroupIndexX;
    GI = ( nGroupWidth * nMapGroupIndexZ) + nMapGroupIndexX;
    
    if (GI >= m_pBlockTerrian->Size())
    {
        HLogE("[Error] More Index------- \n");
        return -3;
    }
    
    
    pBlockTerrian = m_pBlockTerrian->GetAt(GI);
    
    if (bMovableCheck && pBlockTerrian->m_nBlockType & 0x0100) {
        return -4; //움직일수 없는 블럭이다.
    }

    
    int nBlockType = pBlockTerrian->GetBlockType();
    
    //Info높이 계산 정리
    //건물에 대한 충돌.. 일반적인 건물의 충돌....
//    if(pBlockTerrian->GetGType() >= H_GTYPE_BUILD_NORMAL 
//       && pBlockTerrian->GetGType() < H_GTYPE_OTHER_OBJECTS 
//       && pPoint->z < pBlockTerrian->GetHeightMax() )
    if(nBlockType & 0x02 && pPoint->y < pBlockTerrian->GetHeightMax() ) //건물 블럭이면
    {
        SPoint *pLeftTop = pBlockTerrian->GetCompacBlzLeftTop();
        SPoint *pRightTop = pBlockTerrian->GetCompacBlzRightBottom(); 
        
       //sglLog("(%d)%f,%f   min(%f,%f) max(%f,%f)\n",pBlockTerrian->GetBType(),pPoint->x,pPoint->x,pLeftTop->x,pLeftTop->y,pRightTop->x,pRightTop->y);
       if((pPoint->x >= pLeftTop->x && pPoint->x <= pRightTop->x ) && (pPoint->z >= pLeftTop->z  && pPoint->z <= pRightTop->z))
       {
           pPoint->y = pBlockTerrian->GetHeightMax();
           return E_POS_COMPACT_WALL;
       }
    }
    
    //3> Z위치를 가져온다.
	fY = pBlockTerrian->GetPointY(
                                         pPoint,
                                         nIndexVertexX,
                                         nIndexVertexZ,
                                         fIndexVertexX,
                                         fIndexVertexZ,
                                         fUpDown,
                                  &nResult,pOutNormalVector);
    
    if(nResult != E_SUCCESS)
        return nResult;
    
    pPoint->y = fY;
    if(pOutGIndex) *pOutGIndex = GI;
    return E_SUCCESS;
}


int CSGLCore::GetVisibleTerrian(
                                SPoint	*pPoint,
                                SVector	*pvViewDirection,
                                float		fVewVisibleFar,
                                long    *pVisibleTerianMap,
                                int    *pVisibleTerranMapCnt)					  
{
    int nResult = E_SUCCESS;
	SVector normalizeDirection;
    //Output
    SPoint	MapPoint;
    int   nMapGroupIndexX;
    int   nMapGroupIndexZ;
    int   Direction;
    
    int   nIndexVertexX;
    int   nIndexVertexZ;
    
    
	float fIndexVertexX = 0.0f,fIndexVertexZ = 0.0f;
	float fUpDown = 0.0f; //fUpDown < 0 상단,  fUpDown > 0 하단
    
	//1> 실제 자표에서 맵좌표로 변경한다.
	nResult = CSglTerrian::GetWorldToBlock(pPoint,&MapPoint);
	if(nResult != E_SUCCESS) return nResult;
	
    
	sglCopyVector(pvViewDirection,&normalizeDirection);
	normalizeDirection.y = 0; //순수하게 x/y 에 관련 된 값만 계산한다.
	sglNormalize(&normalizeDirection);
	
	
	//pvViewDirection : Normalize를 한상태이다.
	Direction = CSglTerrian::GetDirection(&normalizeDirection);
	
	//2> 기타 그룹 정보를 가져온다.
    CSglTerrian::GetMapGroupIndexToMapUnit(
                                           Direction, 
                                           &MapPoint,
                                           &nMapGroupIndexX,
                                           &nMapGroupIndexZ,
                                           &nIndexVertexX,
                                           &nIndexVertexZ,
                                           &fIndexVertexX,
                                           &fIndexVertexZ,
                                           &fUpDown);
    
	if(nMapGroupIndexX < 0) 
    {
        HLog("[Error] 방향이 0일경우 -12\n");
        return -12; //방향이 0일 경우   
    }
    
    GetVisibleTerrian(pPoint,
                      fVewVisibleFar,
                      nMapGroupIndexX,
                      nMapGroupIndexZ,
                      &normalizeDirection,
                      pVisibleTerianMap,
                      pVisibleTerranMapCnt);
	
    
	return nResult;
	
}

int SortVisible( const void *a, const void *b ) 
{ 
    CSglTerrian *pT1 = (CSglTerrian*) *(long*)a;
    CSglTerrian *pT2 = (CSglTerrian*) *(long*)b;
    if(pT1->m_DistanceFromNowToVisible > pT2->m_DistanceFromNowToVisible) return -1;
    if(pT1->m_DistanceFromNowToVisible < pT2->m_DistanceFromNowToVisible) return 1;
    return 0;
} 


bool CSGLCore::VisibleByCameraTerrian(SVector *pdir,SPoint	*pViewPoint,SPoint *ptTarget)
{
    bool bFastCPU = sglIsFastModel();
    float fD1[3];
    float fD2[3];

    fD1[0] = pdir->x;
    fD1[1] = pdir->y;
    fD1[2] = pdir->z;
//    sglNormalizef(fD1);

    fD2[0] = ptTarget->x - pViewPoint->x;
    fD2[1] = 0.f;
    fD2[2] = ptTarget->z - pViewPoint->z;
    
    float fAmount = sqrtf(fD2[0]*fD2[0] + fD2[2]*fD2[2]);
    
    
    fD2[0] /= fAmount;
    fD2[2] /= fAmount;
    
    //fDV > 0 90보다 작다
    //fDV < 0 90보다 크다.
    //fDV = 0 90
    float fDV = sglDotProductf(fD1, fD2);
    
    //Modified 2014.11.30 IPhone 6 + 화면이 넓어졌다. 0.50에서 0.47로 변경해야 잔상이 안남는다.
    if(bFastCPU && fDV > 0.47f) return true;
    else if(bFastCPU == false && fDV > 0.65f) return true;
    
    if(bFastCPU)
    {
        if(fAmount < 115.f && fDV > 0.f) return true; //앞쪽 가까운 거리는 보이게 해준다.
        if(fAmount < 40.f && fDV < 0) return true; //뒤쪽 40이내는 보이게 해준다.
    }
    else
    {
        if(fAmount < 100.f && fDV > 0.f) return true; //앞쪽 가까운 거리는 보이게 해준다.
        if(fAmount < 40.f && fDV < 0) return true; //뒤쪽 40이내는 보이게 해준다.
    }
    
    return false;
    
}

int CSGLCore::GetVisibleTerrian(
                      SPoint	*pViewPoint,
					  float fVewVisibleFar,
					  int   nMapGroupIndexX,
					  int   nMapGroupIndexZ,
					  SVector *pNormalizeDirection,
					  long *pVisibleTerianMap,
                      int    *pVisibleTerranMapCnt)
{
//    SPoint mBeforePT; //View위치가 같으면 계산을 하지 말자.
    
    int nZWidth = 0;
    int nImageSize = CSGLCore::m_gTerrianSize;
    int nGroupSize = GROUP_SIZE;
    CSglTerrian *pTerrian;
#ifndef MAC
    if(mBeforePT.x == pViewPoint->x && mBeforePT.z == pViewPoint->z)
    {
        *pVisibleTerranMapCnt = mBeforePT.y;
        if(*pVisibleTerranMapCnt != 0)
            return E_SUCCESS;
        else
        {
            mBeforePT.x = pViewPoint->x;
            mBeforePT.z = pViewPoint->z;
        }
    }
    else
    {
        mBeforePT.x = pViewPoint->x;
        mBeforePT.z = pViewPoint->z;
    }
#endif
    
	if(pVisibleTerianMap == NULL) return E_SUCCESS; //무시한다.
	int  nGroupWidth = (nImageSize - 1) / nGroupSize;
	int nGroupFar = ceil(fVewVisibleFar / (float)nGroupSize);

    SPoint *pT;
    SPoint ptViewPointBack;
    ptViewPointBack.x = pViewPoint->x;// - (pNormalizeDirection->x * 32.f);
    ptViewPointBack.y = pViewPoint->y;// - (pNormalizeDirection->y * 32.f);
    ptViewPointBack.z = pViewPoint->z;// - (pNormalizeDirection->z * 32.f);

    int nMaxVisibleTerrian = MAX_ARRAY_VISIBLE_TERRIAN_CNT - 1;
	int nX,nZ;
    int nCnt = *pVisibleTerranMapCnt = 0;
	for(int j = -nGroupFar; j <= nGroupFar; j++)
	{
		nZ = nMapGroupIndexZ + j;
		if(nZ < 0) continue;
		else if(nZ >= nGroupWidth) break;
		
        nZWidth = nGroupWidth * nZ;
		for(int i = -nGroupFar; i <= nGroupFar; i++)
		{
			nX = nMapGroupIndexX + i;
			if(nX < 0) continue;
			else if(nX >= nGroupWidth) break;
            pTerrian = m_pBlockTerrian->GetAt(nZWidth + nX);
            pT = pTerrian->GetBlockCenter();
            if(pT->y == -999.f)
            {
                CSglTerrian::GetIndexToCenterPoint(nZWidth + nX, pT);
                GetPositionY(pT);
                pTerrian->SetBlockCenter(pT);
            }
            if(!VisibleByCameraTerrian(pNormalizeDirection,&ptViewPointBack,pT)) continue;
            pTerrian->m_DistanceFromNowToVisible = i*i + j*j;
            pVisibleTerianMap[nCnt] = (long)pTerrian;
            nCnt++;
            if(nCnt >= nMaxVisibleTerrian) //pVisibleTerranMap의 사이즈 Array로 되어 있다. 그래서 범위에 벗어나지 못하게 하자.
            {
                j = i = nGroupFar + 1;
                break;
            }
		}
	}
    
    //Sort를 한다.
    qsort( pVisibleTerianMap, nCnt, sizeof(long), SortVisible );

    
    if(nCnt > MAX_VISIBLE_TERRIAN_CNT)
    {
        int nX = nCnt - MAX_VISIBLE_TERRIAN_CNT;
        memcpy(pVisibleTerianMap, pVisibleTerianMap + nX, sizeof(long) * MAX_VISIBLE_TERRIAN_CNT);
        nCnt = MAX_VISIBLE_TERRIAN_CNT; //소팅된후로 뒤엣 부분을 제거해준다.
    }
    mBeforePT.y = *pVisibleTerranMapCnt = nCnt;
    
    return E_SUCCESS;
}



int CSGLCore::RenderBegin(int nTime)
{
    SPoint ptActor;
    SVector vActorDir;
    if(mpActor == NULL) return E_SUCCESS;
    SGLCAMERA *pCamera = mpActor->GetCamera();
	int nCameraType = mpActor->GetCameraType();
    if(pCamera == NULL) return E_SUCCESS;
    
    m_VisibleBlockTerrianCnt = 0;
	if(nCameraType < CAMERT_UP_VISIBLE)
	{
		GetVisibleTerrian(&(pCamera->viewPos),
                          &(pCamera->viewDir),
						  VISIBLEFARVIEW,
						  m_VisibleBlockTerrian,
						  &m_VisibleBlockTerrianCnt);
	}
	else if( nCameraType == CAMERT_UP_VISIBLE ) //위에서 바라보는 형태
	{
        mpActor->GetPosition(&ptActor);
        mpActor->GetModelDirection(&vActorDir);
        
		GetVisibleTerrian(&ptActor,
						  &vActorDir,
						  VISIBLEFARVIEW,
						  m_VisibleBlockTerrian,
						  &m_VisibleBlockTerrianCnt);
	}
    return E_SUCCESS;
}

void CSGLCore::RenderBlock(vector<CMS3DASCII*>* pTerrian,float fTerrianShadow)
{
    GLuint glBeforeBind = 0;
    CMS3DASCII* pASCII;
    int nMeshCnt = 0;
    int nTerrianCnt = (int)pTerrian->size();
    sglMesh* plistMesh;
    sglMesh* pMesh;
    
    if(fTerrianShadow != NONE_VALUE)
        glEnableClientState(GL_COLOR_ARRAY);
    
    for (int t = 0; t < nTerrianCnt; t++) 
    {
        pASCII = (*pTerrian)[t];
        nMeshCnt = pASCII->GetMeshSize();
        plistMesh = pASCII->GetMesh();
        for (int m = 0; m < nMeshCnt; m++) 
        {
            pMesh = &plistMesh[m];
            if(pMesh->nglTexutre != 0 && glBeforeBind != pMesh->nglTexutre)
            {
                glBindTexture(GL_TEXTURE_2D, pMesh->nglTexutre);
                glBeforeBind = pMesh->nglTexutre;
            }
            
            glVertexPointer(3, GL_FLOAT, 0, pMesh->pVetexBuffer);
            if(fTerrianShadow != NONE_VALUE)
                glColorPointer(4, GL_FLOAT, 0, pMesh->pColorBuffer);
            glTexCoordPointer(2, GL_FLOAT, 0, pMesh->pCoordBuffer);
            glDrawElements(GL_TRIANGLES, pMesh->shIndicesCnt * 3, GL_UNSIGNED_SHORT, pMesh->pIndicesBuffer);
            
            

        }
    }
    if(fTerrianShadow != NONE_VALUE)
        glDisableClientState(GL_COLOR_ARRAY);
}

void CSGLCore::RenderOrg(CSglTerrian* pRec)
{
    int nBlockType = pRec->GetBlockType();
    if(nBlockType == 0) return;
    
    if(nBlockType & 0x01)
    {
        RenderBlock(pRec->GetListTerrian(),m_fTerrianShadow);
    }
    
    if(nBlockType & 0x02)
    {
        RenderBlock(pRec->GetListNorObjects(),NONE_VALUE);
    }
    
    
    if(nBlockType & 0x04)
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        glColor4f(1.0f,1.0f,1.0f,1.0f);
        glDisable(GL_CULL_FACE);
        glDepthMask(GL_FALSE);
        
        RenderBlock(pRec->GetListNorTransObjects(),NONE_VALUE);
        
        glDisable(GL_BLEND);
        glEnable(GL_CULL_FACE);
        glDepthMask(GL_TRUE);
    }
}
void CSGLCore::RenderTotal()
{
    
	CListNode<CSglTerrian> *pV = NULL;
	
    pV = m_pBlockTerrian;	
	int nCnt = pV->Size();
    
    
    glEnableClientState(GL_VERTEX_ARRAY); 
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnable(GL_TEXTURE_2D);
	
	for(int i = 0; i < nCnt; i++)
	{
		CSglTerrian* pRec = pV->GetAt(i);
        RenderOrg(pRec);
	}
    
    glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY); 

}
void CSGLCore::RenderVisible()
{
    int nCnt = m_VisibleBlockTerrianCnt;
    
    glEnableClientState(GL_VERTEX_ARRAY); 
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnable(GL_TEXTURE_2D);
	
	for(int i = 0; i < nCnt; i++)
	{
		CSglTerrian* pRec = (CSglTerrian*)m_VisibleBlockTerrian[i];
        RenderOrg(pRec);
	}   
    
    glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY); 
}


int CSGLCore::Render()
{
    if(mpActor == NULL) return E_SUCCESS;
    int nCameraType = mpActor->GetCameraType();
	if(nCameraType != CAMERT_UP) 
    {
        if (mpPicking) 
            mpPicking->ResetPickingViewPort(); 
        
		RenderVisible();
    }
	else
    {
        if (mpPicking) 
            mpPicking->ResetPickingViewPort(); 
        
		RenderTotal();
    }
    return E_SUCCESS;
}

int CSGLCore::RenderEnd()
{
    //m_VisibleBlockTerrian.Clear();
    return E_SUCCESS;
}



void CSGLCore::AddSprite(CSprite* pSprite)
{
	mlstSprite.AddTTail(pSprite);
    //CGhostSprite ...
    if(!(pSprite->GetType() & ACTORTYPE_TYPE_GHOSTANITEM)) //아이템이 아닌것은
        mpAction->OnAddedSprite(pSprite);
    
    pthread_mutex_lock(&mHashMutex); //락
    mHashSprite[pSprite->GetID()] = pSprite;
    pthread_mutex_unlock(&mHashMutex); //언락
}

void CSGLCore::DelSprite(int nIndex)
{
    CSprite* pS = mlstSprite.GetAt(nIndex);
    
    pthread_mutex_lock(&mHashMutex); //언락
    map<int, CSprite*>::iterator it = mHashSprite.find(pS->GetID());
    if(it != mHashSprite.end())
        mHashSprite.erase(it);
    pthread_mutex_unlock(&mHashMutex); //언락

    
    mpAction->OnDeletedSprite(pS);
    delete pS;
    mlstSprite.Delete(nIndex);
    
}

void CSGLCore::DelSprite(CSprite* pSprite)
{
    int nCnt = mlstSprite.Size();
    for (int i = 0; i < nCnt; i++) 
    {
        CSprite* pS = mlstSprite.GetAt(i);
        if(pS == pSprite)
        {
            pthread_mutex_lock(&mHashMutex); //락
            map<int, CSprite*>::iterator it = mHashSprite.find(pS->GetID());
            if(it != mHashSprite.end())
                mHashSprite.erase(it);
            pthread_mutex_unlock(&mHashMutex); //언락

            
            if(!(pSprite->GetType() & ACTORTYPE_TYPE_GHOSTANITEM)) //아이템이 아닌것은
                mpAction->OnDeletedSprite(pS);
            
            delete pS;
            mlstSprite.Delete(i);
            break;
        }
    }
}



void CSGLCore::AddBomb(CBomb* pBomb)
{
	mlstBomb.AddTTail(pBomb);
}

void CSGLCore::AddParticle(AParticle* pPaticle)
{
    mlstParticle.AddTTail(pPaticle);
}

void CSGLCore::PlaySystemSound(int nID)
{
    if(m_pSoundMan)
        m_pSoundMan->PlaySystemSound(nID);
}

void CSGLCore::PlayVibrate()
{
    if(m_pSoundMan)
        m_pSoundMan->PlayVibrate();
}

CSprite* CSGLCore::FindSprite(int nID)
{
    map<int, CSprite*>::iterator fnd;
    CSprite* pSprite = NULL;
    pthread_mutex_lock(&mHashMutex); //락
    fnd = mHashSprite.find(nID);
    if(fnd != mHashSprite.end())
        pSprite = (*fnd).second;
    pthread_mutex_unlock(&mHashMutex); //언락
    
    
#ifdef DEBUG
    if(pSprite == NULL)
    {
        HLogE("Not Found Sprite %d",nID);
    }
#endif
    
    return pSprite;
}


void IsTabInOrgBock(CListNode<CMS3DASCII>* pTerrian)
{
    GLuint glBeforeBind = 0;
    CMS3DASCII* pASCII;
    int nMeshCnt = 0;
    int nTerrianCnt = pTerrian->Size();
    sglMesh* plistMesh;
    sglMesh* pMesh;
    
    for (int t = 0; t < nTerrianCnt; t++) 
    {
        pASCII = pTerrian->GetAt(t);
        nMeshCnt = pASCII->GetMeshSize();
        plistMesh = pASCII->GetMesh();
        for (int m = 0; m < nMeshCnt; m++) 
        {
            pMesh = &plistMesh[m];
            if(pMesh->nglTexutre != 0 && glBeforeBind != pMesh->nglTexutre)
            {
                glBindTexture(GL_TEXTURE_2D, pMesh->nglTexutre);
                glBeforeBind = pMesh->nglTexutre;
            }
            
            glVertexPointer(3, GL_FLOAT, 0, pMesh->pVetexBuffer);
            glTexCoordPointer(2, GL_FLOAT, 0, pMesh->pCoordBuffer);
            glDrawElements(GL_TRIANGLES, pMesh->shIndicesCnt * 3, GL_UNSIGNED_SHORT, pMesh->pIndicesBuffer);
        }
    }
}


bool CSGLCore::IsTabInOrgOnlyTerrian(CSglTerrian *pTerrian, SPoint& nearPoint,SPoint& farPoint,float fOutPoint[9])
{
    SGLCAMERA *pCamera;
    vector<CMS3DASCII*>* lstObject = 0;
    CMS3DASCII* pASCII;
    int nTerrianCnt = 0;
    //땅 정보...
    lstObject = pTerrian->GetListTerrian();
    nTerrianCnt = (int)lstObject->size();
    for (int t = 0; t < nTerrianCnt; t++)
    {
        pASCII = (*lstObject)[t];
        if(pASCII->IsPickingTerrian(mpPicking, nearPoint, farPoint,fOutPoint))
        {
            pCamera = mpWorld->GetCamera();
            float ox = pCamera->viewPos.x - fOutPoint[0];
            float oy = pCamera->viewPos.y - fOutPoint[1];
            float oz = pCamera->viewPos.z - fOutPoint[2];
            float on = sqrtf(ox*ox + oy*oy + oz*oz);
            if(on > 25.f) //탱크 뒤쪽의 건물을 클릭했으면 무시하자.
            {
                return true;
            }
        }
    }
    return false;
}

//0 : no, 0x04<<8|Index:TranObj 0x02<<8|Index:NorObj 0x01<<8|Index:Terrian
int CSGLCore::IsTabInOrg(CSglTerrian *pTerrian, SPoint& nearPoint,SPoint& farPoint,float fOutPoint[9])
{
    int nResult = 0;
    int nVIndex;
    bool b1;
    vector<CMS3DASCII*>* lstObject = 0;
    CMS3DASCII* pASCII;
    int nTerrianCnt = 0;
    SGLCAMERA *pCamera;

    
    if(mbGLMake) //디자인일때만 나무를 선택할 수있게한다.
    {
        lstObject = pTerrian->GetListNorTransObjects();
        nTerrianCnt = (int)lstObject->size();
        for (int t = 0; t < nTerrianCnt; t++) 
        {
            pASCII = (*lstObject)[t];
            if(pASCII->IsPicking(mpPicking, nearPoint, farPoint,fOutPoint))
                return 0x04<<8 | t;
        }
    }
   
    

    lstObject = pTerrian->GetListNorObjects(); //건물을 선택
    nTerrianCnt = (int)lstObject->size();
    
    //자신의 건물이 눈에 안보여도 뒤에서 클릭하면 선택되어 버려서
    //건물은 자신의 위치에서 선택못하게 막자.
    if(pTerrian->m_DistanceFromNowToVisible != 0)
    {
        for (int t = 0; t < nTerrianCnt; t++) 
        {
            pASCII = (*lstObject)[t];
            if(pASCII->IsPicking(mpPicking, nearPoint, farPoint,fOutPoint))
            {
                pCamera = mpWorld->GetCamera();
                
                float fd[3];
                float on;
                float fv[3];
                
                fd[0] =  fOutPoint[0] - pCamera->viewPos.x;
                fd[1] = 0;
                fd[2] =  fOutPoint[2] - pCamera->viewPos.z;
                on = sqrtf(fd[0]*fd[0] +  fd[2]*fd[2]);
                fd[0] /= on;
                fd[2] /= on;
                
                
                fv[0] = pCamera->viewDir.x;
                fv[1] = 0;
                fv[2] = pCamera->viewDir.z;
                on = sqrtf(fv[0]*fv[0] +  fv[2]*fv[2]);
                fv[0] /= on;
                fv[2] /= on;
                
                //fDV > 0 90보다 작다
                //fDV < 0 90보다 크다.
                //fDV = 0 90
                float fangle = sglDotProductf(fd, fv); //카메라 뒷쪽으로 클릭이 되었으면 무시하자.
                if(fangle > 0)
                    return 0x01<<8 | 255;
            }
        }
    }
    
    
    //땅 정보...
    lstObject = pTerrian->GetListTerrian();
    nTerrianCnt = (int)lstObject->size();
    for (int t = 0; t < nTerrianCnt; t++) 
    {
        pASCII = (*lstObject)[t];
        
        if(pASCII->IsPickingTerrian(mpPicking, nearPoint, farPoint,fOutPoint))
        {
            pCamera = mpWorld->GetCamera();
            
            float fd[3];
            float on;
            float fv[3];
            
            fd[0] =  fOutPoint[0] - pCamera->viewPos.x;
            fd[1] = 0;
            fd[2] =  fOutPoint[2] - pCamera->viewPos.z;
            on = sqrtf(fd[0]*fd[0] +  fd[2]*fd[2]);
            fd[0] /= on;
            fd[2] /= on;
            
            
            fv[0] = pCamera->viewDir.x;
            fv[1] = 0;
            fv[2] = pCamera->viewDir.z;
            on = sqrtf(fv[0]*fv[0] +  fv[2]*fv[2]);
            fv[0] /= on;
            fv[2] /= on;
            
            //fDV > 0 90보다 작다
            //fDV < 0 90보다 크다.
            //fDV = 0 90
            float fangle = sglDotProductf(fd, fv); //카메라 뒷쪽으로 클릭이 되었으면 무시하자.
            if(fangle > 0)
                return 0x01<<8 | t;
        }
    }
    
    
    //지형..
    if(gpIndicesBuffer)
    {
        
        GLfloat * pV = pTerrian->GetHeightVertex();
        if(pV)
        {
            for (int z = 0; z < gnIndicesCnt; )
            {
                nVIndex = gpIndicesBuffer[z++]*3;
                fOutPoint[0] = pV[nVIndex];
                fOutPoint[1] = pV[nVIndex + 1];
                fOutPoint[2] = pV[nVIndex + 2];
                
                nVIndex = gpIndicesBuffer[z++]*3;
                fOutPoint[3] = pV[nVIndex];
                fOutPoint[4] = pV[nVIndex + 1];
                fOutPoint[5] = pV[nVIndex + 2];
                
                nVIndex = gpIndicesBuffer[z++]*3;
                fOutPoint[6] = pV[nVIndex];
                fOutPoint[7] = pV[nVIndex + 1];
                fOutPoint[8] = pV[nVIndex + 2];        
                
                b1 = mpPicking->sglIsPicking(fOutPoint,&fOutPoint[3],&fOutPoint[6],nearPoint,farPoint);
                if(b1)
                {
                    pCamera = mpWorld->GetCamera();
                    
                    float fd[3];
                    float on;
                    float fv[3];
                    
                    fd[0] =  fOutPoint[0] - pCamera->viewPos.x;
                    fd[1] = 0;
                    fd[2] =  fOutPoint[2] - pCamera->viewPos.z;
                    on = sqrtf(fd[0]*fd[0] +  fd[2]*fd[2]);
                    fd[0] /= on;
                    fd[2] /= on;
                    
                    
                    fv[0] = pCamera->viewDir.x;
                    fv[1] = 0;
                    fv[2] = pCamera->viewDir.z;
                    on = sqrtf(fv[0]*fv[0] +  fv[2]*fv[2]);
                    fv[0] /= on;
                    fv[2] /= on;
                    
                    //fDV > 0 90보다 작다
                    //fDV < 0 90보다 크다.
                    //fDV = 0 90
                    float fangle = sglDotProductf(fd, fv); //카메라 뒷쪽으로 클릭이 되었으면 무시하자.
                    if(fangle > 0)
                        return 0x01<<8 | 255;
//                    if(on > 25.f) //탱크 뒤쪽의 지형을 클릭했으면 무시하자.
//                        return 0x01<<8 | 255;
                }
            }
        }
        
    }
    
    return nResult;
}

//0 : no, 0x4<<8:TranObj 0x2<<8:NorObj 0x1<<8:Terrian 0x8<<8:Sprite
int CSGLCore::IsTabInVisible(float xWinPos, float yWinPos, int &nOutResult,float fOutPoint[9],long &outSprite)
{
    static int gLastIndex = -1; //최대한의 땅의 픽킹 검색을 적게 해보자.
    
    int nResult = 0;
    int nSize = 0;
    int nIndex = 0;
    GLint nwin[2];
    outSprite = 0;
    
    nwin[0] = xWinPos;
    nwin[1] = yWinPos;
    
    SPoint nearPoint;
    SPoint farPoint;
    
    mpPicking->GetUnProject(nwin, nearPoint, 0);
    mpPicking->GetUnProject(nwin, farPoint, 1);
    
    
    //nSize = mlstSprite.Size();
    //for (int i = 0; i < nSize; i++)
    for(ByNode<CSprite>* it = mlstSprite.begin(); it != mlstSprite.end();)
    {
        CSprite* pSP = it->GetObject();
        it = it->GetNextNode();
        if(pSP->IsVisibleRender() &&
           mpActor != pSP &&
           pSP->GetState() == SPRITE_RUN && //Added By Song 2012.01.13 죽는 현상 수정함.
           pSP->IsTabIn(mpPicking, nearPoint, farPoint, xWinPos, yWinPos))
        {
            nOutResult = nIndex;
            
            outSprite = (long)pSP;
            CMS3DModel*	pModel = (CMS3DModel*) pSP->GetModel();
            if(pModel == 0) 
            {
                fOutPoint[0] = nearPoint.x;
                fOutPoint[1] = nearPoint.y;
                fOutPoint[2] = nearPoint.z;
                
                fOutPoint[3] = farPoint.x;
                fOutPoint[4] = farPoint.y;
                fOutPoint[5] = farPoint.z;
// Main Tower를 사용한다.
//                HLogD("TabIn Tank1(%d) = %f,%f,%f\n",outSprite,fOutPoint[0],fOutPoint[1],fOutPoint[2]);
//                return 0x8<<8;
            }
            else if(pModel->GetType() == MODELTYPE_MS3D)
            {
                pModel->GetCenter(fOutPoint);
//                HLogD("TabIn Tank2(%d) = %f,%f,%f\n",outSprite,fOutPoint[0],fOutPoint[1],fOutPoint[2]);
                fOutPoint[3] = -9999.f;
                fOutPoint[4] = -9999.f;
                fOutPoint[5] = -9999.f;
            }
            else if(pModel->GetType() == MODELTYPE_MS3D_ASSCII) //Added By Song 2014.02.25 적군이 클릭이 안되는 현상 수정함.
            {
//                HLogD("TabIn Tank3(%d) = %f,%f,%f\n",outSprite,fOutPoint[0],fOutPoint[1],fOutPoint[2]);
                pModel->GetCenter(fOutPoint);
                fOutPoint[3] = -9999.f;
                fOutPoint[4] = -9999.f;
                fOutPoint[5] = -9999.f;
            }
            return 0x8<<8;
        }
        nIndex++;
    }
    
    nSize = m_VisibleBlockTerrianCnt;
    CSglTerrian* pTerrian;
    
    //Modified By Song 2013.09.24 이전의 클릭했던 데이터가 존재 하면 먼저 검색해보자.
    if(gLastIndex != -1 && gLastIndex < nSize)
    {
        pTerrian = (CSglTerrian*)m_VisibleBlockTerrian[gLastIndex];
        nResult = IsTabInOrg(pTerrian, nearPoint, farPoint,fOutPoint);
        if(nResult != 0)
        {
            nOutResult = gLastIndex;
            return nResult;
        }
    }
    
    for (int i = nSize - 1; i >= 0; i--) 
    {
        pTerrian = (CSglTerrian*)m_VisibleBlockTerrian[i];
        if(gLastIndex == i) continue;
        
        nResult = IsTabInOrg(pTerrian, nearPoint, farPoint,fOutPoint);
        if(nResult != 0)
        {
            nOutResult = i;
            gLastIndex = i;
            break;
        }
    }
    
    if(nResult == 0)
        gLastIndex = -1;
    
    return nResult;
}

//순수하게 땅만 검사한다.
bool CSGLCore::IsTabInOnlyTerrian(float xWinPos,float yWinPos,float fCenterPoint[3])
{
    static int gLastIndex = -1; //최대한의 땅의 픽킹 검색을 적게 해보자.
    
    GLint nwin[2];
    float fOutPoint[9];
    
    nwin[0] = xWinPos;
    nwin[1] = yWinPos;
    
    SPoint nearPoint;
    SPoint farPoint;
    
    mpPicking->GetUnProject(nwin, nearPoint, 0);
    mpPicking->GetUnProject(nwin, farPoint, 1);
    
    int nSize = m_VisibleBlockTerrianCnt;
    CSglTerrian* pTerrian;
    
    
    //Modified By Song 2013.09.24 이전의 클릭했던 데이터가 존재 하면 먼저 검색해보자.
    if(gLastIndex != -1 && gLastIndex < nSize)
    {
        pTerrian = (CSglTerrian*)m_VisibleBlockTerrian[gLastIndex];
        if(IsTabInOrgOnlyTerrian(pTerrian, nearPoint, farPoint,fOutPoint))
        {
            fCenterPoint[0] = (fOutPoint[0] + fOutPoint[3] + fOutPoint[6]) / 3.f;
            fCenterPoint[1] = (fOutPoint[1] + fOutPoint[4] + fOutPoint[7]) / 3.f;
            fCenterPoint[2] = (fOutPoint[2] + fOutPoint[5] + fOutPoint[8]) / 3.f;
            return true;
        }
    }
    
    for (int i = nSize - 1; i >= 0; i--)
    {
        pTerrian = (CSglTerrian*)m_VisibleBlockTerrian[i];
        if(gLastIndex != i && IsTabInOrgOnlyTerrian(pTerrian, nearPoint, farPoint,fOutPoint))
        {
            fCenterPoint[0] = (fOutPoint[0] + fOutPoint[3] + fOutPoint[6]) / 3.f;
            fCenterPoint[1] = (fOutPoint[1] + fOutPoint[4] + fOutPoint[7]) / 3.f;
            fCenterPoint[2] = (fOutPoint[2] + fOutPoint[5] + fOutPoint[8]) / 3.f;
            gLastIndex = i;
            return true;
        }
    }
    gLastIndex = -1;
    
    return false;
}

//0 : no, 0x4<<8:TranObj 0x2<<8:NorObj 0x1<<8:Terrian 0x8<<8:Sprite
int CSGLCore::IsTabInTotal(float xWinPos, float yWinPos, int &nOutResult,float fOutPoint[9],long &outSprite)
{
    int nResult = 0;
    int nSize = 0;
    GLint nwin[2];
    nwin[0] = xWinPos;
    nwin[1] = yWinPos;
    SPoint nearPoint;
    SPoint farPoint;
    outSprite = 0;
    
    mpPicking->GetUnProject(nwin, nearPoint, 0);
    mpPicking->GetUnProject(nwin, farPoint, 1);
    CSglTerrian* pTerrian;
    
    nSize = mlstSprite.Size();
    for (int i = 0; i < nSize; i++) 
    {
        CSprite* pSP = mlstSprite.GetAt(i);
        if(pSP->IsTabIn(mpPicking, nearPoint, farPoint, xWinPos, yWinPos))
        {
            nOutResult = i;
            outSprite = (long)pSP;
            return 0x8<<8;
        }
    }
    if(m_pBlockTerrian == 0) return 0;
    
    nSize = m_pBlockTerrian->Size();
    for (int i = 0; i < nSize; i++) 
    {
        pTerrian = m_pBlockTerrian->GetAt(i);
        nResult = IsTabInOrg(pTerrian, nearPoint, farPoint,fOutPoint);
        if(nResult != 0)
        {
            nOutResult = i;
            break;
        }
    }
    return nResult;
}

//Result 
//0 : no, 0x4<<8:TranObj 0x2<<8:NorObj 0x1<<8:Terrian 0x8<<8:Sprite
//nOutResult : (m_VisibleBlockTerrian or m_pBlockTerrian) 리스트의 인덱스.
//bVisibleTerianMap
//true : m_VisibleBlockTerrian , false : m_pBlockTerrian;
int CSGLCore::IsTabIn(float xWinPos,float yWinPos,int& nOutResult,float fOutPoint[9],long& outSprite)
{   
    int nResult = 0;
    nOutResult = -1;
    if(mpPicking)
    {
        if(m_VisibleBlockTerrianCnt != 0)
            nResult = IsTabInVisible(xWinPos, yWinPos, nOutResult,fOutPoint,outSprite);
        else 
            nResult = IsTabInTotal(xWinPos, yWinPos, nOutResult,fOutPoint,outSprite);
    }
    return nResult;
}


CSprite* CSGLCore::MakeSprite(unsigned int nID,unsigned char cTeamID,int nModelID,CTextureMan *pTextureMan,IAction *pAction,IHWorld* pWorld,NETWORK_TEAMTYPE NetworkTeamType)
{

    CSprite *pSprite = NULL;
    if( nModelID == ACTORID_BLZ_EMAIN)
    {
        assert(SGL_TEAM2 == cTeamID);
        pSprite = new CEMainBlzSprite(CSGLCore::mnEnemyMainTowerMaxID++,cTeamID,nModelID,pTextureMan,pAction,pWorld,NetworkTeamType);
    }
    else if(nModelID == ACTORID_ARROWMOTION)
    {
        pSprite = new CArrowMotionSprite(nID,SGL_TEAMALL,-1,pAction,pWorld);
    }
    else if(nModelID == ACTORID_BLZ_DTOWER || nModelID == ACTORID_BLZ_DMISSILETOWER)
    {
        PROPERTY_TANK property;
        PROPERTY_TANK::GetPropertyTank(nModelID,property);
        pSprite = new CDTower(nID,cTeamID,nModelID,pTextureMan,pAction,pWorld,&property,NetworkTeamType);
    }
    else if(nModelID == ACTORID_BLZ_DGUNTOWER)
    {
        PROPERTY_TANK property;
        PROPERTY_TANK::GetPropertyTank(nModelID,property);
        pSprite = new CDGunTower(nID,cTeamID,nModelID,pTextureMan,pAction,pWorld,&property,NetworkTeamType);
    }
    else if(nModelID == ACTORID_BLZ_MAINTOWER)
    {
        PROPERTY_TANK property;
        PROPERTY_TANK::GetPropertyTank(nModelID,property);
        pSprite = new CDMainTower(nID,cTeamID,nModelID,pTextureMan,pAction,pWorld,&property,NetworkTeamType);
    }
    else if( nModelID < ACTORID_ET1) //일반적이 탱크.
    {
        PROPERTY_TANK property;
        PROPERTY_TANK::GetPropertyTank(nModelID,property);
        if(property.nType == 2) //날라다니는 것.
        {
            if(nModelID == ACTORID_MISSILE)
                pSprite = new CMissileTank(nID,cTeamID,nModelID,pAction,pWorld,&property,NetworkTeamType);
            else
                pSprite = new CNewTank(nID,cTeamID,nModelID,pAction,pWorld,&property,NetworkTeamType);
        }
        else if(nModelID == ACTORID_ARMEREDCAR)
            pSprite = new CArmeredCar(nID,cTeamID,nModelID,pAction,pWorld,&property,NetworkTeamType);
        else if(nModelID == ACTORID_AMBULANCE)
            pSprite = new CAmbulance(nID,cTeamID,nModelID,pAction,pWorld,&property,NetworkTeamType);
        else if(nModelID == ACTORID_XTANK)
            pSprite = new CTireCar(nID,cTeamID,nModelID,pAction,pWorld,&property,NetworkTeamType);
        else
            pSprite = new CK9Sprite(nID,cTeamID,nModelID,pAction,pWorld,&property,NetworkTeamType);
    }
    else if(nModelID >= ACTORID_ET1) //적탱크
    {
        PROPERTY_TANK property;
        PROPERTY_TANK::GetPropertyTank(nModelID,property);
        if(property.nID == ACTORID_ET2)
            pSprite = new CET2(nID,cTeamID,nModelID,pAction,pWorld,&property,NetworkTeamType);
        else if(property.nID == ACTORID_ET8)
            pSprite = new CET8(nID,cTeamID,nModelID,pAction,pWorld,&property,NetworkTeamType);
        else if(property.nID == ACTORID_ET9 || property.nID == ACTORID_ET10)
            pSprite = new CET9(nID,cTeamID,nModelID,pAction,pWorld,&property,NetworkTeamType);
        else if(property.nID == ACTORID_EF1 || property.nID == ACTORID_EF2 || property.nID == ACTORID_EF3)
            pSprite = new CFighter(nID,cTeamID,nModelID,pAction,pWorld,&property,NetworkTeamType);
        else if(property.nID == ACTORID_EF4 || property.nID == ACTORID_EF5)
            pSprite = new CEF4Fighter(nID,cTeamID,nModelID,pAction,pWorld,&property,NetworkTeamType);
        else //CET2만재외하고 CET1,CET3,CET4모두 같은 로직이다.
            pSprite = new CET1(nID,cTeamID,nModelID,pAction,pWorld,&property,NetworkTeamType);
    }
    else
    {
        HLogE("[Error] MakeSprite Not Found Model ID %d\n",nModelID);
    }
    return pSprite;
}

#ifdef ANDROID
void CSGLCore::ResetTexture()
{
    if(m_pModelMan) m_pModelMan->ResetTexture();

    int nSize = m_pBlockTerrian->Size();
    for(int i = 0; i < nSize; i++)
    {
        m_pBlockTerrian->GetAt(i)->ResetTexture();
    }
    
    nSize = mlstParticle.Size();
	for(int i = 0; i < nSize; i++)
	{
        ARender* it = mlstParticle.GetAt(i);
        it->ResetTexture();
	}
}
#endif