//
//  CSimpleMilkShape3DASCII.cpp
//  SongGL
//
//  Created by 호학 송 on 11. 4. 22..
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "CTextureMan.h"
#include "CLoader.h"
#include "sGLUtils.h"
#include "CMS3DASCII.h"
#include <string>
#include "CPicking.h"

CMS3DASCII::CMS3DASCII(CTextureMan *pTextureMan)
{
    m_psglMesh = NULL;
    m_nMeshCnt = 0;
    m_pTextureMan = pTextureMan;
    mIsNormalVector = false;
    mfRadius = -1;
}

CMS3DASCII::~CMS3DASCII()
{
    Clear();
}

void CMS3DASCII::Clear()
{
    m_nMeshCnt = 0;
    if(m_psglMesh)
    {
        for(int i = 0; i < m_nMeshCnt; i++)
        {
            sglMesh *pMesh = &m_psglMesh[i];
            
            if(pMesh->pVetexBuffer)
            {
                delete[] pMesh->pVetexBuffer;
                pMesh->pVetexBuffer = NULL;
            }
            
            if(pMesh->pNormalBuffer)
            {
                delete[] pMesh->pNormalBuffer;
                pMesh->pNormalBuffer = NULL;
            }
            
            if(pMesh->pIndicesBuffer)
            {
                delete[] pMesh->pIndicesBuffer;
                pMesh->pIndicesBuffer = NULL;
            }
            
            if(pMesh->pCoordBuffer)
            {
                delete[] pMesh->pCoordBuffer;
                pMesh->pCoordBuffer = NULL;
            }
            if(pMesh->pColorBuffer)
            {
               delete[] pMesh->pColorBuffer;
               pMesh->pColorBuffer = NULL;
           }
            
        }
        
        delete[] m_psglMesh;
        m_psglMesh = NULL;
    }
    
}

CMS3DASCII* CMS3DASCII::Clone(TerrianMakeMeshInfo* pMeshInfo)
{

    CMS3DASCII* pAscii = new CMS3DASCII(m_pTextureMan);
    
    
    pAscii->m_nMeshCnt = this->m_nMeshCnt;
    pAscii->m_psglMesh = new sglMesh[pAscii->m_nMeshCnt];
    
    for(int i = 0; i < pAscii->m_nMeshCnt; i++)
    {
        sglMesh *pMesh = &(pAscii->m_psglMesh[i]);
        sglMesh *pMeshThis = &(this->m_psglMesh[i]);
        memset(pMesh, 0, sizeof(sglMesh));
        
        strcpy(pMesh->sMeshName,pMeshThis->sMeshName);
        if(pMeshInfo && pMeshInfo->cTextureCnt)
        {
            strcpy(pMesh->sTextureName, pMeshInfo->arrTextureFileList[i]);
            pMesh->uMode = pMeshThis->uMode;
            pMesh->vMode = pMeshThis->vMode;
            pMesh->nglTextureRef = pMeshThis->nglTextureRef;
            pMesh->nglTexutre = m_pTextureMan->GetTextureID(pMesh->sTextureName);
        }
        else 
        {
            strcpy(pMesh->sTextureName,pMeshThis->sTextureName);
            pMesh->uMode = pMeshThis->uMode;
            pMesh->vMode = pMeshThis->vMode;
            pMesh->nglTextureRef = pMeshThis->nglTextureRef;
            pMesh->nglTexutre = pMeshThis->nglTexutre;
        }
        
        if(pMeshThis->pVetexBuffer)
        {
            pMesh->nVetexCnt = pMeshThis->nVetexCnt;
            pMesh->pVetexBuffer = new GLfloat[pMesh->nVetexCnt * 3];
            memcpy(pMesh->pVetexBuffer, pMeshThis->pVetexBuffer, pMesh->nVetexCnt * sizeof(GLfloat) * 3);
        }
        
        if(pMeshThis->pNormalBuffer)
        {
            pMesh->nNormalCnt = pMeshThis->nNormalCnt;
            pMesh->pNormalBuffer = new GLfloat[pMesh->shIndicesCnt * 9];
            memcpy(pMesh->pNormalBuffer, pMeshThis->pNormalBuffer, pMesh->shIndicesCnt * 9 * sizeof(GLfloat));
        }
        
        if(pMeshThis->pIndicesBuffer)
        {
            pMesh->shIndicesCnt = pMeshThis->shIndicesCnt;
            pMesh->pIndicesBuffer = new GLushort[pMesh->shIndicesCnt * 3];
            memcpy(pMesh->pIndicesBuffer, pMeshThis->pIndicesBuffer, pMesh->shIndicesCnt * 3 * sizeof(GLushort));
        }
        
        if(pMeshThis->pCoordBuffer)
        {
            pMesh->shCoordCount = pMeshThis->shCoordCount;
            pMesh->pCoordBuffer = new GLfloat[pMesh->shCoordCount * 2];
            memcpy(pMesh->pCoordBuffer, pMeshThis->pCoordBuffer, pMesh->shCoordCount * 2 *sizeof(GLfloat));
        }
    }
    pAscii->m_pTextureMan = this->m_pTextureMan;
    return pAscii;
}


char* CMS3DASCII::FndNextLine(char *pOrg,char* pOutLine)
{
    long nResult = 0;
    char *pFnd = NULL;
    pFnd = strstr(pOrg , "\r\n");
    if(pFnd == NULL) return NULL;
    nResult = pFnd - pOrg;
    if(nResult >= 1024) return NULL;
    strncpy(pOutLine,pOrg,nResult);
    pOutLine[nResult] = 0;
    return pFnd+2; //\r\n이 붙허있다.
}


//    밀크세이프 For ASCII 파일을 읽어온다.
//    순수하게 메쉬 데이터만 읽어온다.
int CMS3DASCII::Load(const char* sFileName,bool bLoadNormalVector)
{
//#ifdef DEBUG
//    HLogD("Sel %s\n",sFileName);
//#endif
    
    long nLen = 0;
    char* pData = NULL;
    FILE* file = NULL;

    sglMesh *pMesh = NULL;
    char *pTemp,*pValue,*pTemp2;
    int nTextureCnt = 0;
    char strLine[1024];
    int nIndex = 0;
    //int nUMode,nVMode;
    char sPath[1024];
    float fMaxU,fMaxV;

   
    std::string* arrTextureName = 0;
    int* arrUMode = 0;
    int* arrVMode = 0;
    GLuint* arrTextureID = 0;

    mIsNormalVector = bLoadNormalVector;
    sprintf(sPath,"%s%s",CLoader::getSearchPath(),sFileName);    
    
	// open filename in "read binary" mode
	file = fopen(sPath, "rb");
	if (!file) return -1;

    nLen = fseek(file, 0, SEEK_END);
    if(nLen != 0) { nLen = -2; goto EXIT;}
    nLen = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    if(nLen == 0) { nLen = -3; goto EXIT;}
    pData = new char[nLen + 1];
    pData[nLen] = 0;
    fread(pData, nLen, 1, file);
    
    Clear();
    
    //문서의 2>fseek(file, 0, SEEK_SET);를 참조한다.
    pTemp = FndNextLine(pData,strLine); if(pTemp == NULL) { nLen = -4; goto EXIT;} // MilkShape 3D ASCII
    pTemp = FndNextLine(pTemp,strLine); if(pTemp == NULL) { nLen = -4; goto EXIT;} // \r\n
    pTemp = FndNextLine(pTemp,strLine); if(pTemp == NULL) { nLen = -6; goto EXIT;}    //Frames: 30
    pTemp = FndNextLine(pTemp,strLine); if(pTemp == NULL) { nLen = -7; goto EXIT;}    //Frame: 1 
    pTemp = FndNextLine(pTemp,strLine); if(pTemp == NULL) { nLen = -4; goto EXIT;} // \r\n
    
    //1>Mesh의 카운트이다.
    pTemp = FndNextLine(pTemp,strLine); if(pTemp == NULL) { nLen = -9; goto EXIT;}    //Meshes: 1
    pValue = strtok(strLine, ": ");
    if(strcmp(pValue, "Meshes") != 0)
    {
        nLen = -10; goto EXIT;   
    }
    pValue = strtok(NULL, ": ");
    m_nMeshCnt = atoi(pValue);
    
    if(m_nMeshCnt == 0) 
    {
        HLogE("[Error] MeshCnt = 0 %s",sFileName);
        nLen = -11; 
        goto EXIT;   
    }
    
    m_psglMesh = new sglMesh[m_nMeshCnt];
    for(int i = 0; i < m_nMeshCnt; i++)
    {
        //Trim을 사용한다.
        pMesh = &m_psglMesh[i];
        memset(pMesh,0,sizeof(sglMesh));
    
        //Mesh Name
        pTemp = FndNextLine(pTemp,strLine); if(pTemp == NULL) { nLen = -11; goto EXIT;}   //"Box01" 0 0
        pValue = strtok(strLine, "\""); //"Box01"
        strcpy(pMesh->sMeshName,pValue);
        
              
        strtok(NULL, "\" ");    //0
        pValue = strtok(NULL, "\" ");    //0 <=Texture Index
        
        pMesh->nglTextureRef = atoi(pValue);
        
        //정점의 수
        pTemp = FndNextLine(pTemp,strLine); if(pTemp == NULL) { nLen = -12; goto EXIT;} 
        pMesh->nVetexCnt = atoi(strLine);
        
        pMesh->pVetexBuffer = new GLfloat[pMesh->nVetexCnt * 3]; //x,y,z
        
        //Indicies 수
        pMesh->shCoordCount  = pMesh->nVetexCnt;
        pMesh->pCoordBuffer = new GLfloat[pMesh->shCoordCount * 2]; //u,v
        
        fMaxU = 0.0f;
        fMaxV = 0.0f;
        for(int j = 0; j < pMesh->nVetexCnt; j++)
        {
            
            //0 (-15.000000 15.000000 20.000000) (0.666667 0.000000) -1  <= x,y,z, u,v
            pTemp = FndNextLine(pTemp,strLine); if(pTemp == NULL) { nLen = -12; goto EXIT;}
#ifdef DEBUG
            pValue = strtok(strLine, " "); //-15.000000 = x
//// 포선택
//            if(*pValue != '0')
//            {
//                HLogD("Sel = %d,%d,0\n",i,j);
//            }
#else
            strtok(strLine, " "); //0
#endif
            
            nIndex = j*3;
            pValue = strtok(NULL, " "); //-15.000000 = x
            pMesh->pVetexBuffer[nIndex] = atof(pValue);
            pValue = strtok(NULL, " "); // 15.000000 = y
            pMesh->pVetexBuffer[nIndex+1] = atof(pValue);
            pValue = strtok(NULL, " "); // 20.000000 = z
            pMesh->pVetexBuffer[nIndex+2] = atof(pValue);
            
            nIndex = j*2;
            pValue = strtok(NULL, " "); // 0.666667 = u
            pMesh->pCoordBuffer[nIndex] = atof(pValue);
            
            if(pMesh->pCoordBuffer[nIndex] >= fMaxU)
                fMaxU = pMesh->pCoordBuffer[nIndex];
            
            pValue = strtok(NULL, " "); // 0.000000 = v
            pMesh->pCoordBuffer[nIndex+1] = atof(pValue);
            
                        
            if(pMesh->pCoordBuffer[nIndex+1] >= fMaxV)
                fMaxV = pMesh->pCoordBuffer[nIndex+1];
        }
        
        //비율이 없을 경우는 1.0f를 기준으로 반대로 나타내준다.
        if(fMaxV < 1.0f)    fMaxV = 1.0f;
        
        //실제 이미지의 방향이 위아래가 바뀌어 있다. 그래서 1.0f - value 를 하여 인덱스 비율을 줄였다.
        for(int j = 0; j < pMesh->nVetexCnt; j++)
        {
            nIndex = j*2;
            pMesh->pCoordBuffer[nIndex+1] = fMaxV - pMesh->pCoordBuffer[nIndex+1];
        }
        
        
        if(fMaxU > 1.0f)
            pMesh->uMode = 1;
        else
            pMesh->uMode = 0;
        
        if(fMaxV > 1.0f)
            pMesh->vMode = 1;
        else
            pMesh->vMode = 0;

        pTemp = FndNextLine(pTemp,strLine); if(pTemp == NULL) { nLen = -13; goto EXIT;}   //법선벡터 갯수
        
        GLfloat* pNormalMapBuffer = 0;
        if(bLoadNormalVector)
        {
            int nNormalMapCnt = atoi(strLine);
            pNormalMapBuffer = new GLfloat[nNormalMapCnt * 3];

            for(int j = 0; j < nNormalMapCnt; j++)
            {
               
                pTemp = FndNextLine(pTemp,strLine); if(pTemp == NULL) { nLen = -14; goto EXIT;}
                
                nIndex = j*3;
                
                pValue = strtok(strLine, " "); //0
                pNormalMapBuffer[nIndex] = atof(pValue);
                
                pValue = strtok(NULL, " "); 
                pNormalMapBuffer[nIndex+1] = atof(pValue);
                
                pValue = strtok(NULL, " "); 
                pNormalMapBuffer[nIndex+2] = atof(pValue);
                
                
                //VectorMathf::normalizef(&pNormalMapBuffer[nIndex]);
            }
            
            pMesh->nNormalCnt = nNormalMapCnt;
        }
        else
        {
            pMesh->nNormalCnt = atoi(strLine);
            for(int j = 0; j < pMesh->nNormalCnt; j++)
            {
                pTemp = FndNextLine(pTemp,strLine); if(pTemp == NULL) { nLen = -14; goto EXIT;}
                //    pOutResource->pNormal = NULL;
                //    pOutResource->nNormalCnt = 0
            }
            
            //법선벡터는 로직이 없다.
            pMesh->nNormalCnt = 0;
        }
        
        pTemp = FndNextLine(pTemp,strLine); if(pTemp == NULL) { nLen = -15; goto EXIT;}   //정점 인덱스 갯수
        pMesh->shIndicesCnt = atoi(strLine);
        pMesh->pIndicesBuffer = new GLushort[pMesh->shIndicesCnt * 3];
                
        if(bLoadNormalVector)
            pMesh->pNormalBuffer = new GLfloat[pMesh->shIndicesCnt * 3 * 3];
        
        
        for(int j = 0; j < pMesh->shIndicesCnt; j++)
        {
            pTemp = FndNextLine(pTemp,strLine); if(pTemp == NULL) { nLen = -16; goto EXIT;}
            
            nIndex = j*3;
            //1 (0 1 2) 0 0 0 1
            strtok(strLine, " "); //1
            
            pValue = strtok(NULL, " "); //0 
            pMesh->pIndicesBuffer[nIndex] = atoi(pValue);
            pValue = strtok(NULL, " "); //1 
            pMesh->pIndicesBuffer[nIndex+1] = atoi(pValue);
            pValue = strtok(NULL, " "); //2
            pMesh->pIndicesBuffer[nIndex+2] = atoi(pValue);
            
            //그 뒤에는 버텍스 인덱스 이다.
            if(bLoadNormalVector)
            {
                nIndex = j*9;
                pValue = strtok(NULL, " "); //0 = 첫번째 정점의 노벌의 인덱스
                int nMapIndex = atoi(pValue) * 3;
                memcpy(&(pMesh->pNormalBuffer[nIndex]),&(pNormalMapBuffer[nMapIndex]),sizeof(GLfloat)*3);
                pValue = strtok(NULL, " "); //0 = 두번째 정점의 노벌의 인덱스
                nMapIndex = atoi(pValue) * 3;
                memcpy(&(pMesh->pNormalBuffer[nIndex+3]),&(pNormalMapBuffer[nMapIndex]),sizeof(GLfloat)*3);
                pValue = strtok(NULL, " "); //0 = 세번째 정점의 노벌의 인덱스
                nMapIndex = atoi(pValue) * 3;
                memcpy(&(pMesh->pNormalBuffer[nIndex+6]),&(pNormalMapBuffer[nMapIndex]),sizeof(GLfloat)*3);
            }
            //pValue = strtok(NULL, " "); //0 = ?
        }
        
        if(pNormalMapBuffer)
        {
            delete[] pNormalMapBuffer;
            pNormalMapBuffer = 0;
        }
    }
    
    
    pTemp = FndNextLine(pTemp,strLine); if(pTemp == NULL) { nLen = -17; goto EXIT;} //\r\n
    //"Materials: 1"
    pTemp = FndNextLine(pTemp,strLine); if(pTemp == NULL) { nLen = -17; goto EXIT;}    
    strtok(strLine, ": "); //Materials
    pValue = strtok(NULL, ": ");    //1
    
    nTextureCnt = atoi(pValue);
    
    if(nTextureCnt > 0)
    {
        arrTextureName = new std::string[nTextureCnt];
        arrUMode = new int[nTextureCnt];
        arrVMode = new int[nTextureCnt];
        arrTextureID = new GLuint[nTextureCnt];
        memset(arrTextureID, 0, sizeof(GLuint) * nTextureCnt);
    }
    
    //메쉬보다 텍스처가 작을수가 있기 때문에 메쉬에 대응하는 것을 일단기억한다.
    for(int j = 0; j < nTextureCnt; j++)
    {
        pMesh = NULL;
        
        //일단 uMode, VMode의 값을 참조하기위해 기준이 되는 메쉬를 찾는다.
        for(int i = 0; i < m_nMeshCnt; i++)
        {
            if(m_psglMesh[i].nglTextureRef == j)
            {
                pMesh = &m_psglMesh[i];
                break;
            }
        }
        
        
        //"Material02" MapName
        pTemp = FndNextLine(pTemp,strLine); if(pTemp == NULL) { nLen = -18; goto EXIT;}    
        strtok(strLine, "\""); //Material02
        
        
        //디렉토리 경로를 제거하고 순수한 파일명만 가져온다.
//        pTemp2 = strstr(pValue,"\\");
//        while(pTemp2)
//        {
//            pValue = pTemp2 + 1;
//            pTemp2 = strstr(pTemp2 + 1, "\\");
//        }
        
//        
        pTemp = FndNextLine(pTemp,strLine); if(pTemp == NULL) { nLen = -18; goto EXIT;}    //0.200000 0.200000 0.200000 1.000000
        pTemp = FndNextLine(pTemp,strLine); if(pTemp == NULL) { nLen = -19; goto EXIT;}    //0.200000 0.200000 0.200000 1.000000
        pTemp = FndNextLine(pTemp,strLine); if(pTemp == NULL) { nLen = -20; goto EXIT;}    //0.200000 0.200000 0.200000 1.000000
        pTemp = FndNextLine(pTemp,strLine); if(pTemp == NULL) { nLen = -21; goto EXIT;}    //0.200000 0.200000 0.200000 1.000000
        pTemp = FndNextLine(pTemp,strLine); if(pTemp == NULL) { nLen = -22; goto EXIT;}    //0.200000 
        pTemp = FndNextLine(pTemp,strLine); if(pTemp == NULL) { nLen = -23; goto EXIT;}    //0.200000 
        
        //FileName
        pTemp = FndNextLine(pTemp,strLine); if(pTemp == NULL) { nLen = -24; goto EXIT;}   
        pValue = strtok(strLine,"\"");
        
        //디렉토리 경로를 제거하고 순수한 파일명만 가져온다.
        pTemp2 = strstr(pValue,"\\");
        while(pTemp2)
        {
            pValue = pTemp2 + 1;
            pTemp2 = strstr(pTemp2 + 1, "\\");
        }
        
        //strcpy(pMesh->sTextureName,pValue);
        arrTextureName[j] = pValue;
        
        
        arrUMode[j] = pMesh != 0 ? pMesh->uMode : 0;
        arrVMode[j] = pMesh != 0 ? pMesh->vMode : 0;
        
        //PNG는 FALSE로 읽어온다.
        arrTextureID[j] = m_pTextureMan->GetTextureID(arrTextureName[j].c_str(),arrUMode[j],arrVMode[j]);
        if(arrTextureID[j] == 0)
        {
            HLogE("[Error] Not Bounding %s Texture \n",arrTextureName[j].c_str());
            m_pTextureMan->Remove(arrTextureName[j].c_str()); //지워서 나중에 다시 그려보자.
        }
        
        pTemp = FndNextLine(pTemp,strLine); if(pTemp == NULL) { nLen = -24; goto EXIT;}
    
    }
    
    //기억한 텍스처를 사용하여 메쉬의 텍스처를 입힌다.
    for(int i = 0; i < m_nMeshCnt; i++)
    {
        pMesh = &m_psglMesh[i];
        
        for(int j = 0; j < nTextureCnt; j++)
        {
            if(m_psglMesh[i].nglTextureRef == j)
            { 
                strcpy(pMesh->sTextureName,arrTextureName[j].c_str());
                pMesh->uMode = arrUMode[j];
                pMesh->vMode = arrVMode[j];
                pMesh->nglTexutre = arrTextureID[j];
                break;
            }
        }
    }
    
    if(arrTextureName) delete[] arrTextureName;
    if(arrUMode) delete[] arrUMode;
    if(arrVMode) delete[] arrVMode;
    if(arrTextureID) delete[] arrTextureID;
    
    
EXIT:
    if(file) fclose(file);
    if(pData) 
        delete[] pData;
    return nLen < 0 ? (int)nLen : E_SUCCESS;
}


//반드시 GroundNormalSea.txt로 그라운드를 만들어야 피킹을 빠르게 판단할수가 있다.
bool    CMS3DASCII::IsPickingTerrian(CPicking* pPick,SPoint& nearPoint,SPoint& farPoint,float* fOP)
{
    bool b1;
    float fOutPoint[9];

    //인덱스가 0,6,9,15를 사용하여(큰사각형으로 먼저 검색을 한후에 세부적인 검색을 하자)
    sglMesh *pMesh = &m_psglMesh[0];
    
//    HLogD("Terrian Vertex Count = %d\n",pMesh->nVetexCnt);
//    if(pMesh->nVetexCnt > 48)
    if(pMesh->nVetexCnt == 318) //Modified By Song 2013.09.24 일반적인 땅은 정확하게 318로 나온다 그래서 주어진 큰사각형을 검사하면 되지만, 그렇지 않을 경우는 정밀하게 검사를 해야한다.
    {
        //15 (-16,16) left,top
        fOutPoint[0] = pMesh->pVetexBuffer[45];
        fOutPoint[1] = pMesh->pVetexBuffer[46];
        fOutPoint[2] = pMesh->pVetexBuffer[47];
        

        //0 (16,16) (right,top)
        fOutPoint[3] = pMesh->pVetexBuffer[0];
        fOutPoint[4] = pMesh->pVetexBuffer[1];
        fOutPoint[5] = pMesh->pVetexBuffer[2];
        
        
        //9 (16,-16) (right,bottom)
        fOutPoint[6] = pMesh->pVetexBuffer[27];
        fOutPoint[7] = pMesh->pVetexBuffer[28];
        fOutPoint[8] = pMesh->pVetexBuffer[29];
        
        
        
        b1 = pPick->sglIsPicking(fOutPoint,&fOutPoint[3],&fOutPoint[6],nearPoint,farPoint);
        if(b1 == false)
        {
            //6 (-16,-16) (left,bottom)
            fOutPoint[0] = pMesh->pVetexBuffer[18];
            fOutPoint[1] = pMesh->pVetexBuffer[19];
            fOutPoint[2] = pMesh->pVetexBuffer[20];
            
            //15 (-16,16) left,top
            fOutPoint[3] = pMesh->pVetexBuffer[45];
            fOutPoint[4] = pMesh->pVetexBuffer[46];
            fOutPoint[5] = pMesh->pVetexBuffer[47];
            
            //9 (16,-16) (right,bottom) 상동
//            fOutPoint[6] = pMesh->pVetexBuffer[27];
//            fOutPoint[7] = pMesh->pVetexBuffer[28];
//            fOutPoint[8] = pMesh->pVetexBuffer[29];
            
            b1 = pPick->sglIsPicking(fOutPoint,&fOutPoint[3],&fOutPoint[6],nearPoint,farPoint);
            if(b1 == false)
                return false;
        }
    }
    
    return IsPicking(pPick,nearPoint,farPoint,fOP);
}

bool    CMS3DASCII::IsPicking(CPicking* pPick,SPoint& nearPoint,SPoint& farPoint,float* fOP)
{

    bool b1;
    int nVIndex = 0;
    float fOutPoint[9];
    for (int i = 0; i < m_nMeshCnt; i++) 
    {
        sglMesh *pMesh = &m_psglMesh[i];
        
        int nCntz = pMesh->shIndicesCnt * 3;
        for (int z = 0; z < nCntz; ) 
        {
            nVIndex = pMesh->pIndicesBuffer[z++]*3;
            fOutPoint[0] = pMesh->pVetexBuffer[nVIndex];
            fOutPoint[1] = pMesh->pVetexBuffer[nVIndex + 1];
            fOutPoint[2] = pMesh->pVetexBuffer[nVIndex + 2];
            
            nVIndex = pMesh->pIndicesBuffer[z++]*3;
            fOutPoint[3] = pMesh->pVetexBuffer[nVIndex];
            fOutPoint[4] = pMesh->pVetexBuffer[nVIndex + 1];
            fOutPoint[5] = pMesh->pVetexBuffer[nVIndex + 2];
            
            nVIndex = pMesh->pIndicesBuffer[z++]*3;
            fOutPoint[6] = pMesh->pVetexBuffer[nVIndex];
            fOutPoint[7] = pMesh->pVetexBuffer[nVIndex + 1];
            fOutPoint[8] = pMesh->pVetexBuffer[nVIndex + 2];
            
            b1 = pPick->sglIsPicking(fOutPoint,&fOutPoint[3],&fOutPoint[6],nearPoint,farPoint);
            if(b1)
            {
                if(fOP)
                    memcpy(fOP, fOutPoint, sizeof(float)*9);
                
                return true;
            }
        }
        
    }
    return false;

}


#ifdef ANDROID
void CMS3DASCII::ResetTexture()
{
    for (int i = 0; i < m_nMeshCnt; i++)
    {
        sglMesh *pMesh = &m_psglMesh[i];
        if(pMesh && pMesh->nglTexutre != 0 && strlen(pMesh->sTextureName))
        {
            pMesh->nglTexutre = m_pTextureMan->GetTextureID(pMesh->sTextureName,pMesh->uMode,pMesh->vMode);
        }
    }
}

#endif