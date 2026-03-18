//
//  CRoll3dModel.cpp
//  SongGL
//
//  Created by 송 호학 on 11. 12. 11..
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include "CRoll3dModel.h"
#include "CModelMan.h"
#include "CMS3DModel.h"
#include "sGLTrasform.h"
#include "sGLUtils.h"
#include "CAniLoader.h"
#include "sGLText.h"
#include "CBombTailParticle.h"


#define K9_SCALE 0.019f
#define K9_HEADERX 0		
#define K9_HEADERY 0		
#define K9_HEADERZ 48       //몸통이 0,0,0 위치에 회전이 없을때 탱크의 머리는 (0,48)의 위치에 있다.

#define ET1_SCALE 0.019f
#define ET1_HEADERX 0		
#define ET1_HEADERY 0		
#define ET1_HEADERZ 48       //몸통이 0,0,0 위치에 회전이 없을때 탱크의 머리는 (0,48)의 위치에 있다.


CRoll3dModel::CRoll3dModel(CModelMan* pModelMan,PROPERTY_TANK* Propety)
{
    mpModelMan = pModelMan;
    mpModel = 0;
    mpModelHeader = 0;
    mfBeforeDirAngle = -999;
    mfSelectedAnimationAngle = 0.0f;
    
    mProperty = *Propety;
//    CopyPropertyTank(Propety,&mProperty);
    mpBoosterPos = 0;
    mnBoosterCnt = 0;
    
//    mpBootMan = 0;
}

CRoll3dModel::~CRoll3dModel()
{
    if(mpModel)
    {
        delete mpModel;
        mpModel = 0;
    }
    if(mpModelHeader)
    {
        delete mpModelHeader;
        mpModelHeader = 0;
    }
    
    if(mpBoosterPos)
    {
        int nCnt = mnBoosterCnt;
        for (int i = 0; i < nCnt; i++)
        {
            delete[] mpBoosterPos[i];
        }
        delete[] mpBoosterPos;
        mpBoosterPos = NULL;
    }
//    if(mpBootMan)
//    {
//        delete mpBootMan;
//        mpBootMan = NULL;
//    }
}

int CRoll3dModel::Initialize(SPoint *pPosition,SVector *pvDirection)
{
    string sPath;
    switch (mProperty.nType) 
    {
        case CRoll3dModel_L1:
            sPath = mProperty.sModelPath + string("Root.txt");
            mpModelMan->getModelf(sPath.c_str(), &mpModel,true);
            
            mpModel->scale[0] = K9_SCALE;
            mpModel->scale[1] = K9_SCALE;
            mpModel->scale[2] = K9_SCALE;
            
            mpModel->position[0] = pPosition->x;
            mpModel->position[1] = pPosition->y + .5f;
            mpModel->position[2] = pPosition->z;
            
            
            mpModel->orientation[0] = 0;
            mpModel->orientation[1] = 180;
            mpModel->orientation[2] = 0;
            
            sPath = mProperty.sModelPath + string("Header.txt");
            mpModelMan->getModelf(sPath.c_str(), &mpModelHeader,true);
            mpModelHeader->scale[0] = K9_SCALE;
            mpModelHeader->scale[1] = K9_SCALE;
            mpModelHeader->scale[2] = K9_SCALE;
            
            mHeaderPosition[0] = mProperty.fHeaderPosX;
            mHeaderPosition[1] = mProperty.fHeaderPosY;
            mHeaderPosition[2] = mProperty.fHeaderPosZ;
            
            ArrangeHeaderSprite(mHeaderPosition[0], mHeaderPosition[1], mHeaderPosition[2]);
            
            
        {
            
            CBombTailParticle::BombTailParticleInitialize(NULL);
            //포의 시작 위치를 파싱하여 가져온다.
            if(mProperty.sMisailStartPos)
            {
                char* sTemp = new char[strlen(mProperty.sMisailStartPos) + 1];
                strcpy(sTemp, mProperty.sMisailStartPos);
                char* sStar = strstr(sTemp, "*");
                char* sStar2 = NULL;
                
                if(sStar) //미사일
                {
                    *sStar = 0;
                    //sTemp //부스터 위치
                    PosParse(sTemp,&mnBoosterCnt,mpBoosterPos);
                    
                    sStar2 = strstr(sStar + 1, "*"); //총위 위치
                    if (sStar2)
                    {
                        *sStar2 = 0;
                        //총의 위치
                        sStar2+=1;
                        //PosParse(sStar2,&mnGunStartCnt,mptStartGun);
                    }
                    sStar += 1;
                    //미사일 위치
                    //PosParse(sStar,&mnPoStartCnt,mptStartMissile);
                    
                }
                else //부스터 위치
                {
                    PosParse(sTemp,&mnBoosterCnt,mpBoosterPos);
                }
                delete[] sTemp;
            }
            
        }
            break;
        case CRoll3dModel_K9:
            sPath = mProperty.sModelPath + string("Root.ms3d");
            mpModelMan->getModelf(sPath.c_str(), &mpModel);
            
            sPath = mProperty.sModelPath + string("Root.ani");
            
            if(mpModel->GetType() == MODELTYPE_MS3D)
                CAniLoader::loadAnimationf(sPath.c_str(), *(CMS3DModel*)mpModel);
            mpModel->setNextAnimationf(2, GL_FALSE, GL_FALSE);
            
            mpModel->scale[0] = K9_SCALE;
            mpModel->scale[1] = K9_SCALE;
            mpModel->scale[2] = K9_SCALE;
            
            mpModel->position[0] = pPosition->x;
            mpModel->position[1] = pPosition->y;
            mpModel->position[2] = pPosition->z;
            
            
            mpModel->orientation[0] = 0;
            mpModel->orientation[1] = 180;
            mpModel->orientation[2] = 0;
            
//            mpModelHeader = new CMS3DModel;
            sPath = mProperty.sModelPath + string("Header.ms3d");
            mpModelMan->getModelf(sPath.c_str(), &mpModelHeader);
            mpModelHeader->scale[0] = K9_SCALE;
            mpModelHeader->scale[1] = K9_SCALE;
            mpModelHeader->scale[2] = K9_SCALE;
            
            mHeaderPosition[0] = mProperty.fHeaderPosX;
            mHeaderPosition[1] = mProperty.fHeaderPosY;
            mHeaderPosition[2] = mProperty.fHeaderPosZ;
            
            ArrangeHeaderSprite(mHeaderPosition[0], mHeaderPosition[1], mHeaderPosition[2]);
            break;
         
            
        default:
            break;
    }
    return E_SUCCESS;
}

void CRoll3dModel::ArrangeHeaderSprite(int xHeader,int yHeader,int zHeader)
{
	GLfloat ptHeaderPos[3];
	GLfloat matrix[16];
	GLfloat pos[3];
    
	pos[0] = xHeader * K9_SCALE;
	pos[1] = yHeader * K9_SCALE;
	pos[2] = zHeader * K9_SCALE;
	
	memset(ptHeaderPos,0,sizeof(GLfloat) * 3);
	
	sglLoadIdentityf(matrix);	
    sglRotateRzRyRxMatrixf(matrix,
                           mpModel->orientation[0],
                           mpModel->orientation[1], 
                           mpModel->orientation[2]);
    sglMultMatrixVectorf(ptHeaderPos, matrix,pos);
	
	
	//탱크의 머리는 몸체의 방향과 동일하다.
	mpModelHeader->orientation[0] = mpModel->orientation[0];
    if(mfSelectedAnimationAngle == 0.0f)
        mpModelHeader->orientation[1] = mpModel->orientation[1];
    else
    {
        mpModelHeader->orientation[1] = AngleArrange(mpModelHeader->orientation[1] - mfSelectedAnimationAngle * 1.5);
    }
	mpModelHeader->orientation[2] = mpModel->orientation[2];
	
	mpModelHeader->position[0] =  mpModel->position[0] + ptHeaderPos[0];
	mpModelHeader->position[1] =  mpModel->position[1] + ptHeaderPos[1];
	mpModelHeader->position[2] =  mpModel->position[2] + ptHeaderPos[2];
}

void CRoll3dModel::SetSelectedAnimationAngle(float fAniAngle)
{
    mfSelectedAnimationAngle = fAniAngle;
}

int CRoll3dModel::RenderBegin(int nTime,float fCurrentAngle)
{
    if(mpModel == 0) return E_SUCCESS;
    
    float fAngle = 0.0f;
    if(mfBeforeDirAngle == -999)
    {
        mfBeforeDirAngle = fCurrentAngle;
    }
    else
    {
        fAngle = mfBeforeDirAngle - fCurrentAngle;
        mfBeforeDirAngle = fCurrentAngle;
    }
    
    if(fAngle != 0.0f)
    {
        mpModel->orientation[1] = AngleArrange(mpModel->orientation[1] + fAngle + mfSelectedAnimationAngle);
        ArrangeHeaderSprite(mHeaderPosition[0], mHeaderPosition[1], mHeaderPosition[2]);
    }
    else if(mfSelectedAnimationAngle != 0.0f)
    {
        mpModel->orientation[1] = AngleArrange(mpModel->orientation[1] + mfSelectedAnimationAngle);
        ArrangeHeaderSprite(mHeaderPosition[0], mHeaderPosition[1], mHeaderPosition[2]);
    }
    
    
    return RenderBegin(nTime);
}

int CRoll3dModel::RenderBegin(int nTime)
{
    if(mpModel == 0) return E_SUCCESS;
    
    mpModel->updatef(nTime);
    mpModelHeader->updatef(nTime);
    
    
    return E_SUCCESS;    
}

int CRoll3dModel::Render()
{
    if(mpModel == 0) return E_SUCCESS;
    
    mpModelHeader->renderf();
    mpModel->renderf();
    if(mpBoosterPos)
    {
        RenderBeginBooster(GetClockDeltaConst());
    }
    
    if(mfSelectedAnimationAngle == 0.0f)
    {
        if(mpModel->isAnimationRunningf())
            mpModel->setNextAnimationf(2, GL_FALSE, GL_FALSE);
    }
    else if(mfSelectedAnimationAngle != 0.0f)
    {
        if(mpModel->isAnimationRunningf() == false)
            mpModel->setNextAnimationf(0, GL_TRUE, GL_FALSE);
    }
    
    
    return E_SUCCESS;    
}

int CRoll3dModel::RenderEnd()
{
    return E_SUCCESS;    
}

int CRoll3dModel::OnEvent(SGLEvent *pEvent)
{
    return E_SUCCESS;    
}



#include "CMS3DModelASCII.h"
#include "CBombTailParticle.h"
void CRoll3dModel::GetBoosterPos(int nIndex,float** pOutFront,float** pOutBack )
{
    sglMesh     *pWorldMesh = ((CMS3DModelASCII*)mpModel)->GetWorldMesh();
    //nPoIndex 포인덱스를 사용하여 각도를 구해온다. //2,4,2;3,4,2;4,4,2
    *pOutFront = &pWorldMesh[mpBoosterPos[nIndex][0]].pVetexBuffer[mpBoosterPos[nIndex][1] * 3];
    *pOutBack = &pWorldMesh[mpBoosterPos[nIndex][0]].pVetexBuffer[mpBoosterPos[nIndex][2] * 3];
}

int CRoll3dModel::RenderBeginBooster(int nTime)
{
    int nIndex = 0;
    GLfloat mWorldMatrix[16];
    GLfloat mWorldMatrix1[16];
    float color[4];
    float *fFront=NULL,*fBack=NULL;
    SPoint ptNow;

    int nRand = rand() % 3;
    float fAni = 0.09f;
    for (int j = 0; j < mnBoosterCnt; j++)
    {
        
        GetBoosterPos(j,&fFront,&fBack);
        ptNow.x = fBack[0];
        ptNow.y = fBack[1];
        ptNow.z = fBack[2];
        
        color[0] = 0.7f;
        color[1] = 0.7f;
        color[2] = 0.7f;
        color[3] = 0.5f;
        
        
        
        sglLoadIdentityf(mWorldMatrix);
        
        //이동을 먼저한후에
        if(nRand == 1)
            sglTranslateMatrixf(mWorldMatrix,ptNow.x,ptNow.y,ptNow.z);
        else if(nRand == 2)
            sglTranslateMatrixf(mWorldMatrix,ptNow.x,ptNow.y - 0.05,ptNow.z);
        else
            sglTranslateMatrixf(mWorldMatrix,ptNow.x,ptNow.y - fAni,ptNow.z);
        memcpy(mWorldMatrix1, mWorldMatrix, sizeof(mWorldMatrix));
        
        CMS3DModelASCII* pModel = (CMS3DModelASCII*)mpModel;
        
        //짐벌락현상 해결함.
        GLfloat	rotationMatrix[16];
        GLfloat	quaternion0[4];
        sglAngleToQuaternionf(quaternion0, pModel->orientation[0], pModel->orientation[1], pModel->orientation[2] + 90.f);
        sglQuaternionToMatrixf(rotationMatrix, quaternion0);
        sglMultMatrixf(mWorldMatrix, mWorldMatrix, rotationMatrix);
        sglRotateRzRyRxMatrixf(mWorldMatrix,0.f, 90.f, 0.f);
        sglScaleMatrixf(mWorldMatrix, 0.08f, 0.08f, 0.08f);
        
        
        sglMultMatrixf(mWorldMatrix1, mWorldMatrix1, rotationMatrix);
        sglAngleToQuaternionf(quaternion0, 0.f, 90.f, 90.f);
        sglQuaternionToMatrixf(rotationMatrix, quaternion0);
        sglMultMatrixf(mWorldMatrix1, mWorldMatrix1, rotationMatrix);
        sglScaleMatrixf(mWorldMatrix1, 0.08f, 0.08f, 0.08f);
        
        for(int i = 0; i < CBombTailParticle::g_shBombTailIndicesCnt; i++)
        {
            nIndex = i * 3;
            
            sglMultMatrixVectorf(mWorldVertex[0] + nIndex, mWorldMatrix,CBombTailParticle::m_gBombTailParticleVertex + nIndex);
            sglMultMatrixVectorf(mWorldVertex[1] + nIndex, mWorldMatrix1,CBombTailParticle::m_gBombTailParticleVertex + nIndex);
            
        }
        RenderBooster(nRand);
    }
    return E_SUCCESS;
}

int CRoll3dModel::RenderBooster(int nRand)
{

    glDisable(GL_CULL_FACE); //후면추려내기를 하지말자.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
//	glEnableClientState(GL_NORMAL_ARRAY);
    
    //부스터가 움직이는 효과를 내준다.
    if(nRand == 1)
        glColor4f(1.0f,1.0f,1.0f,1.0f);
    else if(nRand == 2)
        glColor4f(1.0f,1.0f,1.0f,.5f);
    else
        glColor4f(1.0f,1.0f,1.0f,0.1f);
    
    CMS3DModelASCII* pModel = (CMS3DModelASCII*)mpModel;
    glBindTexture(GL_TEXTURE_2D, pModel->GetMS3DASCII()->GetTextureMan()->GetTextureID("NormalBombTail.tga"));
    glTexCoordPointer(2, GL_FLOAT, 0, CBombTailParticle::m_gBombTailParticleCoordTex);
    
    glVertexPointer(3, GL_FLOAT, 0, mWorldVertex[0]);
    glDrawElements(GL_TRIANGLE_STRIP, CBombTailParticle::g_shBombTailIndicesCnt, GL_UNSIGNED_SHORT, CBombTailParticle::m_gBombTailParticleIndices);
    
    glVertexPointer(3, GL_FLOAT, 0, mWorldVertex[1]);
    glDrawElements(GL_TRIANGLE_STRIP, CBombTailParticle::g_shBombTailIndicesCnt, GL_UNSIGNED_SHORT, CBombTailParticle::m_gBombTailParticleIndices);
    
    glDisable(GL_TEXTURE_2D);
//    mpBootMan->Render();
    
    glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    
//	glDisableClientState(GL_NORMAL_ARRAY);
    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);

    return E_SUCCESS;
}