//
//  CDTower.cpp
//  SongGL
//
//  Created by 호학 송 on 13. 7. 30..
//
//
#include <stdlib.h>
#include <math.h>
#include <stdlib.h>
#include "CDTower.h"
#include "CHWorld.h"
#include "CSGLCore.h"
#include "CSpriteStatus.h"
#include "sGLUtils.h"
#include "CAICore.h"
#include "CStrategyBlzAI.h"
#include "CFireParticle.h"
#include "CBombTailParticle.h"
#include "CAniRuntimeEnergyUP.h"
#include "CCircleUpgradeMark.h"
#include "CMutiplyProtocol.h"

CDTower::CDTower(unsigned int nID,unsigned char cTeamID,int nModelID,CTextureMan *pTextureMan,IAction *pAction,IHWorld* pWorld,PROPERTY_TANK* pProperty,NETWORK_TEAMTYPE NetworkTeamType):
CSprite(nID,cTeamID,nModelID,pAction,pWorld,pProperty,NetworkTeamType)
{
    mpModel = NULL;
    mpWorldMesh = NULL;
    mfRotationAngle = 0.f;
    mfRotationVector = 0.f;
    mfRotationToAngle = 0.f;		//이각도 방향으로 이동
    
    mfPoUpAngle = 0.f;
    mfPoUpToAngle = 0.f;
    mfPo_UpDonwDir = 0.f; //-1아래 1위방향으로 이동
    mnPoUpgrade = 0;
    
    
    mptStartMissile = NULL;
    mnPoStartCnt = 0;
    mnPoIndex= 0;
    
    mnAniGGTime = 0;
    mfAniBottomAngle = 0.f;
    mfAniHeaderAngle = 0.f;
    mfAniBottomY = 0.f;
    mfAniBottomZ = 0.f;
    mfAniHeaderY = 0.f;
    mfAniHeaderX = 0.f;
    mbOnAmiedNeedFire = false;
    mnMakingTime = 0;
    mExcuteCmd = 0;
    mnFireInterval = 0;
    

    mnConstMakingTime = MAKINGTIME;
}


CDTower::~CDTower()
{
    if(mptStartMissile)
    {
        int nCnt = mnPoStartCnt;
        for (int i = 0; i < nCnt; i++)
        {
            delete[] mptStartMissile[i];
        }
        delete[] mptStartMissile;
        mptStartMissile = NULL;
    }
    
    if(mpWorldMesh)
    {
        int nSize = mpModel->GetMeshSize();
        for(int i = 0; i < nSize; i++)
        {
            delete[] mpWorldMesh[i].pVetexBuffer;
#if CMS3DModelASCII_ORGNORMAL
            if(mpModel->IsNormalVector())
                delete[] mpWorldMesh[i].pNormalBuffer;
#endif
        }
        delete[] mpWorldMesh;
    }
}


int CDTower::GetFirePerSec() {
    return ((float)mProperty.nFirePerSec / mBombProperty.fFirePerSec) * (1.f / ((float)mnPoUpgrade + 1.f));}    //폭탄을 시간당 쏠수있는 능력

int CDTower::LoadResoure(CModelMan* pModelMan)
{
    CHWorld* pWorld = (CHWorld*)mpWorld;
    string sPath = mProperty.sModelPath;
    sPath.append(".txt");
    
    pWorld->GetSGLCore()->GetModelMan()->getModelfAscii(sPath.c_str(), &mpModel,true);
    //mpModel = pWorld->GetSGLCore()->GetMS3DModelASCIIMan()->Load(sPath.c_str(),true);
    if(mpModel == NULL)
    {
        HLogE("[Error] %s can't load\n",sPath.c_str());
        return -1;
    }
    
    int nSize = mpModel->GetMeshSize();
    sglMesh *arrMesh = mpModel->GetMesh();
    mpWorldMesh = new sglMesh[nSize];
    for(int i = 0; i < nSize; i++)
    {
        mpWorldMesh[i].pVetexBuffer = new float[arrMesh[i].nVetexCnt * 3];
#if CMS3DModelASCII_ORGNORMAL
        if(mpModel->IsNormalVector())
            mpWorldMesh[i].pNormalBuffer = new float[arrMesh[i].nNormalCnt * 3];
#endif

    }
	mpAICore = new CAICore(this,NULL,new CStrategyBlzAI(this));
    ResetPower();
    mpStatusBar->SetIsSubBar(true);
    return E_SUCCESS;
}

void CDTower::GetInfomation(float* pfOutDLevel,float* pfOutALevel,float* pfOutDPower,float* pfOutAPower)
{
    //Added By Song 2013.10.30 Map Rank
    CUserInfo* pUserInfo = mpWorld->GetUserInfo();
    float fMapRank = pUserInfo ? pUserInfo->GetCompletedRank() : 1.f;
    float fMapRankAValue = (fMapRank - 1) * POWWER_PER_AMAPRANK;
    float fMapRankDValue = (fMapRank - 1) * POWWER_PER_DMAPRANK;
    
    float fRunTimeUp = 0.f;
    float fDUp = 0.0f;
    if(GetNetworkTeamType() == NETWORK_MYSIDE)
    {

        
        if(GetModelID() == ACTORID_BLZ_DTOWER)
            fDUp = pUserInfo->GetCntByIDWithItem(ID_ITEM_UPGRADE_DEFENCE_TOWER) * 2.f;
        else if(GetModelID() == ACTORID_BLZ_DMISSILETOWER)
            fDUp = pUserInfo->GetCntByIDWithItem(ID_ITEM_UPGRADE_MISSILE_TOWER) * 2.f;
        
        fMapRankAValue = pUserInfo->GetRank(); //Actorsms 맵에 따라 파워가 올라가지 않음.
        if(fMapRankAValue == 0.f) fMapRankAValue = 2.0f; //훈련소모드일때 너무 약하기 때문에 랭크 1로 해준다.
        fMapRankDValue = fMapRankAValue;
        
        if(mpRuntimeUpgradeList)
        {
            RUNTIME_UPGRADE *prop = 0;
            for (vector<USERINFO*>::iterator it = mpRuntimeUpgradeList->begin(); it != mpRuntimeUpgradeList->end();)
            {
                prop = (RUNTIME_UPGRADE*)(*it)->pProp;
                if(prop->nID == RUNTIME_UPDATE_UPGRADE)
                    fRunTimeUp = prop->nUpgradeCnt;
                else if(prop->nID == RUNTIME_UPDATE_TOWER_POUP)
                    mnPoUpgrade = prop->nUpgradeCnt;
                it++;
            }
        }
    }
    
    *pfOutDLevel = DUnitLevelToTotalLevel(mProperty.nMaxDefendPower,fMapRankDValue ,fDUp + fRunTimeUp * 3.0f);
    *pfOutDPower = DDefenceLevelToPower(*pfOutDLevel);
    *pfOutALevel = DUnitLevelToTotalLevel(mProperty.nMaxAttackPower,fMapRankAValue ,fDUp + fRunTimeUp * 3.0f);
    *pfOutAPower = DAttackLevelToPower(*pfOutALevel);
    
}

void CDTower::OnCompletedUpdated(RUNTIME_UPGRADE* prop,CArchive* pReadArc)
{
    int nID = GetID();
    if(prop->nID == RUNTIME_UPDATE_TOWER_POUP)
    {
        if(pReadArc == NULL)
        {
            string sDesc;
            CHWorld* pWorld = (CHWorld*)GetWorld();
            sDesc = SGLTextUnit::GetOnlyText(prop->sName);
            sDesc.append(" ");
            sDesc.append(SGLTextUnit::GetOnlyText("Completed Repare"));
            pWorld->OnSelectedObjectShowDescBar(this,sDesc.c_str());
            
            
            if(!pWorld->GetMutiplyProtocol()->IsStop())
            {
                CArchive arc;
                arc << nID;
                arc << prop->nID;
                arc << prop->nUpgradeCnt;
                pWorld->GetMutiplyProtocol()->SendSyncCompletedRuntimeUpgrade(&arc);
            }
        }
        else
        {
            *pReadArc >> nID;
            *pReadArc >> prop->nID;
            *pReadArc >> prop->nUpgradeCnt;
            
            delete pReadArc;
        }
    }
    else
        CSprite::OnCompletedUpdated(prop, pReadArc);
}

void CDTower::ResetPower(bool bRunTimeUpgrade)
{
    float fDLevel,fALevel;
    //상대팀은 mfMaxDefendPowerActor,m_fMaxAttackPower가 넘오온다.
    if(GetNetworkTeamType() == NETWORK_MYSIDE)
        GetInfomation(&fDLevel,&fALevel,&mfMaxDefendPowerActor,&m_fMaxAttackPower);
    
    if(bRunTimeUpgrade == false)
    {
        ((CHWorld*)mpWorld)-> OnSelectedObjectShowDescBar(this);
        SetCurrentDefendPower(mfMaxDefendPowerActor);
    }
    else
    {
        ((CHWorld*)mpWorld)-> OnSelectedObjectShowDescBar(this,SGLTextUnit::GetOnlyText("Completed Level Upgrade"));
        
        if(mpCircleUpgradeMark == NULL)
        {
            CSGLCore* pCore = ((CHWorld*)mpWorld)->GetSGLCore();
            mpCircleUpgradeMark = new CCircleUpgradeMark(mpWorld,this,10.f);
            mpCircleUpgradeMark->Initialize(NULL, NULL);
            pCore->AddParticle(mpCircleUpgradeMark);
        }
        else
        {
            mpCircleUpgradeMark->SetCircleCount(mpCircleUpgradeMark->GetCircleCount() + 1);
        }
    }
}

int CDTower::Initialize(SPoint *pPosition,SVector *pvDirection)
{
    //7 ~ 18
    mfRadian = mProperty.fHalfX;
    if(mProperty.sMisailStartPos)
    {
        vector<string> lstData;
        char* sOrg;
        char* sTok = new char[strlen(mProperty.sMisailStartPos) + 1];
        strcpy(sTok,mProperty.sMisailStartPos);
        sOrg = sTok;
        sTok = strtok(sTok, ";");
        while (sTok)
        {
            lstData.push_back(sTok);
            sTok = strtok(NULL, ";");
        }
        delete[] sOrg;
        
        
        mnPoStartCnt = lstData.size();
        mptStartMissile = new int*[mnPoStartCnt];
        for (int i = 0; i < mnPoStartCnt; i++)
        {
            mptStartMissile[i] = new int[3];
            
            sTok = new char[lstData[i].length() + 1];
            strcpy(sTok, lstData[i].c_str());
            sOrg = sTok;
            
            int nIndex = 0;
            sTok = strtok(sTok, ",");
            while (sTok)
            {
                mptStartMissile[i][nIndex] = atoi(sTok);
                sTok = strtok(NULL, ",");
                nIndex++;
            }
            delete[] sOrg;
        }
    }
    
    mnType = mProperty.nType;
    
    InitRuntimeUpgrade();
    
    if(mpWorld->GetUserInfo()->GetRank() == 0)
        mnConstMakingTime = MAKINGTIME / 5; //건물 짓는데 30초 걸린다.
    else if(mpWorld->GetUserInfo()->GetCntByIDWithItem(ID_TIEM_FASTBUILD))
        mnConstMakingTime = MAKINGTIME / 2;
    
    mnMakingTime = GetGGTime() + mnConstMakingTime; //건물 짓는데 30초 걸린다.
        
    mpStatusBar->SetSubBarRate(0.f);
    return CSprite::Initialize(pPosition, pvDirection);
}

void CDTower::RenderBeginCore(int nTime)
{
    CSprite* pAttackTo = mpAICore->GetAttackTo();
    CSprite::RenderBeginCore(nTime);
    if(pAttackTo && pAttackTo->GetState() != SPRITE_RUN)
    {
        mfRotationVector = 0.f;
        mfRotationToAngle = 0.f;
        mfPo_UpDonwDir = 0.f;
        mfPoUpToAngle = 0.f;
    }
    
    if(mnMakingTime)
    {
        int nInterval = mnMakingTime - GetGGTime();
        if(nInterval > mnConstMakingTime)
            nInterval = mnConstMakingTime;
        mpStatusBar->SetSubBarRate((float)(mnConstMakingTime - nInterval) / (float)mnConstMakingTime);
    }
//    else
//    {
//        //Added By Song 2013.09.09 아직 공격 받지 않으면 먼거리에서 아군 탱크를 지원해준다.
//        if(pAttackTo == NULL && pActor->GetState() == SPRITE_RUN && GetNetworkTeamType() == NETWORK_MYSIDE)
//        {
//            pAttackTo = pActor->GetAttackTo();
//            if(pAttackTo && pAttackTo->GetState() == SPRITE_RUN)
//                Command(AICOMMAND_MOVEIN, pAttackTo);
//        }
//    }
}


void CDTower::RenderBeginCore_Invisiable(int nTime)
{
    CSprite* pAttackTo = mpAICore->GetAttackTo();
//    CSprite* pActor = mpWorld->GetActor();
    if(pAttackTo && pAttackTo->GetState() != SPRITE_RUN)
    {
        mfRotationVector = 0.f;
        mfRotationToAngle = 0.f;
        mfPo_UpDonwDir = 0.f;
        mfPoUpToAngle = 0.f;
    }
    
    //    bool bNon = true;
    if(mState == SPRITE_RUN)
    {
        
        CSprite::RenderBeginCore_Invisiable(nTime);
        
        if(mnMakingTime)
        {
            if(GetGGTime() >= mnMakingTime)
            {
                mnMakingTime = 0;
                mpStatusBar->SetIsSubBar(false);
                SetCurrentDefendPower(GetMaxDefendPower());
            }
            else
                return;
        }
        
        RuntimeUpgrading(nTime);
        
        //자동으로 적 탱크로 해더를 돌려보자. (가상으로 돌린다.)
        RenderBeginAutoTurnHeader(nTime * .3f);
        
//        if(isMovingAutoHeader() == false && mpAICore->GetAttackTo())
        if(mfRotationVector == 0.f && mpAICore->GetAttackTo())
        {
            mfPo_UpDonwDir = 0.f;
            FireMissileInVisible();
        }

    }
    else if(mState == BOMB_COMPACT || mState == BOMB_COMPACT_ANI)
    {
        SetState(SPRITE_READYDELETE);
    }
}

int CDTower::RenderBegin(int nTime)
{

    float fScale = mProperty.fScale; //0.07
    SPoint ptNow;
    float matrix[16];
    float matrix2[16];
    float matrix3[16];
    GLfloat	rotationMatrix[16];
    GLfloat	quaternion0[4];

    
    int nIndex = 0;
    int nSize = mpModel->GetMeshSize();
    sglMesh *arrMesh = mpModel->GetMesh();
    sglMesh *arrWroldMesh = mpWorldMesh;

    
    if(mState == BOMB_COMPACT || mState == BOMB_COMPACT_ANI)
    {
        CampactAnimaion(nTime);
    }
    
    if(mnMakingTime != 0 && GetGGTime() >= mnMakingTime)
    {
        mnMakingTime = 0;
        mpStatusBar->SetIsSubBar(false);
        SetCurrentDefendPower(GetMaxDefendPower());
        ((CHWorld*)GetWorld())->ResetClickedSpriteSlotFromMakingTime(this);
    }
    
    if(mnMakingTime == 0)
    {
        RuntimeUpgrading(nTime);
        
        //자동으로 적 탱크로 해더를 돌려보자.
        RenderBeginAutoTurnHeader(nTime * .3f);
        
        //바닥..
        sglLoadIdentityf(matrix);
        GetPosition(&ptNow);
        sglTranslateMatrixf(matrix,ptNow.x,ptNow.y + 1.f,ptNow.z); //회전전에 이동을 해야 한다.
        sglScaleMatrixf(matrix, fScale, fScale, fScale);
        
        //mfRotationAngle+=(float)nTime/10.f;
       
        //해더
        sglLoadIdentityf(matrix2);
        sglTranslateMatrixf(matrix2,ptNow.x,ptNow.y + 1.f + mfAniBottomY,ptNow.z + mfAniBottomZ); //회전전에 이동을 해야 한다.
        sglRotateRzRyRxMatrixf(matrix2,
                               0.f,
                               mfRotationAngle,
                               0.f);
        
        if(mfAniBottomAngle != 0.f)
            sglRotateRzRyRxMatrixf(matrix2,
                                   0.f,
                                   0.f,
                                   mfAniBottomAngle);
            
        sglScaleMatrixf(matrix2, fScale, fScale, fScale);

        if(mfPoUpToAngle > MAXPOUPANGLE)    mfPoUpToAngle = MAXPOUPANGLE;
        
        if(mfPo_UpDonwDir != 0.f)
        {
            mfPoUpAngle += nTime/20.f* mfPo_UpDonwDir;
        
            if(mfPo_UpDonwDir < 0.f && mfPoUpAngle <= mfPoUpToAngle)
            {
                mfPoUpAngle = mfPoUpToAngle;
                
                mfPoUpToAngle = 0.f;
                mfPo_UpDonwDir = 0.f;
                mpAction->OnAutoAimed(this);
            }
            else if(mfPo_UpDonwDir > 0.f && mfPoUpAngle >= mfPoUpToAngle)
            {
                mfPoUpAngle = mfPoUpToAngle;
                mfPoUpToAngle = 0.f;
                mfPo_UpDonwDir = 0.f;
                mpAction->OnAutoAimed(this);
            }
        }
        else if(mbOnAmiedNeedFire && mpAICore->GetAttackTo())
        {
            mpAction->OnAutoAimed(this);
            mbOnAmiedNeedFire = false;
        }
        
        if(mfPoUpAngle > MAXPOUPANGLE)      mfPoUpAngle = MAXPOUPANGLE;
        
        //포
        sglLoadIdentityf(matrix3);
        sglTranslateMatrixf(matrix3,ptNow.x + mfAniHeaderX,ptNow.y + 1.f + mfAniHeaderY,ptNow.z); //회전전에 이동을 해야 한다.
        sglAngleToQuaternionf(quaternion0, 0.f, mfRotationAngle, mfPoUpAngle);
        sglQuaternionToMatrixf(rotationMatrix, quaternion0);
        
        if(mfAniHeaderAngle != 0.f)
            sglRotateRzRyRxMatrixf(matrix3,
                                   mfAniHeaderAngle,
                                   0.f,
                                   0.f);
        
        sglMultMatrixf(matrix3, matrix3, rotationMatrix);
        
        sglScaleMatrixf(matrix3, fScale, fScale, fScale);

        
        for(int j = 0; j < nSize; j++)
        {
            sglMesh* pMesh = &arrMesh[j];
            sglMesh* pWorldMesh = &arrWroldMesh[j];
            
            for(int i = 0; i < pMesh->nVetexCnt; i++)
            {
                nIndex = i*3;
                
                if(mnPoUpgrade == 0 && j > 2) continue; //포가 하나.
                else if(mnPoUpgrade == 1 && j == 2) continue; //포기 두개
                
                if(j == 1)
                {
                    
                    
                    //좌표를 이동한다.
                    sglMultMatrixVectorf(&pWorldMesh->pVetexBuffer[nIndex],
                                         matrix2,
                                         &pMesh->pVetexBuffer[nIndex]);
                    
                    
                }
                else if(j > 1)
                {
                    
                    
                    //좌표를 이동한다.
                    sglMultMatrixVectorf(&pWorldMesh->pVetexBuffer[nIndex],
                                         matrix3,
                                         &pMesh->pVetexBuffer[nIndex]);
                    
                }
                else
                {
                    //좌표를 이동한다.
                    sglMultMatrixVectorf(&pWorldMesh->pVetexBuffer[nIndex],
                                         matrix,
                                         &pMesh->pVetexBuffer[nIndex]);
                    
                }
            }
            
#if CMS3DModelASCII_ORGNORMAL
            if(mpModel->IsNormalVector())
            {
                for(int i = 0; i < pMesh->nNormalCnt; i++)
                {
                    nIndex = i*3;
                    
                    if(mnPoUpgrade == 0 && j > 2) continue; //포가 하나.
                    else if(mnPoUpgrade == 1 && j == 2) continue; //포기 두개
                    
                    if(j == 1)
                    {
                        
                        
                        //좌표를 이동한다.
                        sglMultMatrixVectorf(&pWorldMesh->pNormalBuffer[nIndex],
                                             matrix2,
                                             &pMesh->pNormalBuffer[nIndex]);
                        
                        
                    }
                    else if(j > 1)
                    {
                        
                        
                        //좌표를 이동한다.
                        sglMultMatrixVectorf(&pWorldMesh->pNormalBuffer[nIndex],
                                             matrix3,
                                             &pMesh->pNormalBuffer[nIndex]);
                        
                    }
                    else
                    {
                        //좌표를 이동한다.
                        sglMultMatrixVectorf(&pWorldMesh->pNormalBuffer[nIndex],
                                             matrix,
                                             &pMesh->pNormalBuffer[nIndex]);
                        
                    }
                }

            }
#endif
        }
        
//        mpStatusBar->RenderBegin(nTime);

    }
    else
    {
        //바닥..
        sglLoadIdentityf(matrix);
        GetPosition(&ptNow);
        sglTranslateMatrixf(matrix,ptNow.x,ptNow.y + 1.f,ptNow.z); //회전전에 이동을 해야 한다.
        sglScaleMatrixf(matrix, fScale, fScale, fScale);

        sglMesh* pMesh = &arrMesh[0];
        sglMesh* pWorldMesh = &arrWroldMesh[0];
        
        for(int i = 0; i < pMesh->nVetexCnt; i++)
        {
            nIndex = i*3;
            
            
            //좌표를 이동한다.
            sglMultMatrixVectorf(&pWorldMesh->pVetexBuffer[nIndex],
                                 matrix,
                                 &pMesh->pVetexBuffer[nIndex]);
            
        }
        
//        mpStatusBar->RenderBegin(nTime);
        
        
    }
    CSprite::RenderBegin(nTime);
    return E_SUCCESS;
}


int CDTower::Render()
{
    int nSize = mpModel->GetMeshSize();
    sglMesh *arrMesh = mpModel->GetMesh();
    sglMesh *arrWroldMesh = mpWorldMesh;
    
    
    glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnable(GL_TEXTURE_2D);
    
    if(mpModel->IsNormalVector())
    {
        glEnableClientState(GL_NORMAL_ARRAY);
        glEnable(GL_LIGHTING);
#ifdef ANDROID
        glEnable(GL_LIGHT0); //Android
#endif
    }
    
    glColor4f(1.0f,1.0f,1.0f,1.0f);
    for(int j = 0; j < nSize; j++)
    {
        sglMesh* pMesh = &arrMesh[j];
        
        if(mnPoUpgrade == 0 && j > 2) continue; //포가 하나.
        else if(mnPoUpgrade == 1 && j == 2) continue; //포기 두개
            
        glBindTexture(GL_TEXTURE_2D, pMesh->nglTexutre);
        glVertexPointer(3, GL_FLOAT, 0, arrWroldMesh[j].pVetexBuffer);
#if CMS3DModelASCII_ORGNORMAL
        if(mpModel->IsNormalVector())
            glNormalPointer(GL_FLOAT, 0, arrWroldMesh[j].pNormalBuffer);
#else
        if(mpModel->IsNormalVector())
            glNormalPointer(GL_FLOAT, 0, pMesh->pNormalBuffer);
#endif
        glTexCoordPointer(2, GL_FLOAT, 0, pMesh->pCoordBuffer);
        glDrawElements(GL_TRIANGLES, pMesh->shIndicesCnt * 3, GL_UNSIGNED_SHORT, pMesh->pIndicesBuffer);
        
        if(mnMakingTime) break; //첫번째 바닥만 그린다. 건설중이면.

    }
    
    if(mpModel->IsNormalVector())
    {
#ifdef ANDROID
        glDisable(GL_LIGHT0); //Android
#endif
        glDisable(GL_LIGHTING);
        glDisableClientState(GL_NORMAL_ARRAY);
    }
    glDisable(GL_TEXTURE_2D);
    
    glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    return E_SUCCESS;
}

int CDTower::RenderEnd()
{
    return E_SUCCESS;
}

int CDTower::OnEvent(SGLEvent *pEvent)
{
    int nMsgID = pEvent->nMsgID;
    if( nMsgID == GSL_BOMB_FIRE)
	{
#if !PATH_TEST
        FireMissile();
#endif //PATH_TEST
	}
    else
        return CSprite::OnEvent(pEvent);
        
    return E_SUCCESS;
}

void CDTower::FireMissile()
{
    if(mnMakingTime) return;
    
    unsigned long lN = GetGGTime();
    if(lN < mnFireInterval)
        return;
    
    mnFireInterval = lN + 200;
    if(mnPoUpgrade == 0) //포일은 첫번째 한개만 하면된다.
    {
        mnPoIndex = 0;
    }
    else if(mnPoUpgrade == 1)
    {
        mnPoIndex++;
        if(mnPoIndex > 2 || mnPoIndex == 0)
            mnPoIndex = 1;
    }
    
    else //for(int i = nStart; i < nCnt; i++)
    {
        mnPoIndex++;
        if(mnPoIndex > 2)
            mnPoIndex = 0;
    }
    
    if(!CanFireMissile()) return;
    FireOrgMissile();
}

void CDTower::FireMissileInVisible()
{
    if(mnMakingTime) return;
    
    unsigned long lN = GetGGTime();
    if(lN < mnFireInterval)
        return;
    
    mnFireInterval = lN + 200;
    if(mnPoUpgrade == 0) //포일은 첫번째 한개만 하면된다.
    {
        mnPoIndex = 0;
    }
    else if(mnPoUpgrade == 1)
    {
        mnPoIndex++;
        if(mnPoIndex > 2 || mnPoIndex == 0)
            mnPoIndex = 1;
    }
    
    else //for(int i = nStart; i < nCnt; i++)
    {
        mnPoIndex++;
        if(mnPoIndex > 2)
            mnPoIndex = 0;
    }
    
    if(!CanFireMissile()) return;
    FireOrgMissileInVisible();
}

void  CDTower::FireOrgMissileInVisible()
{
    float fMaxLenDouble,fMaxLen,fDivy;
    float fWAngle,fHAngle;//,vtPoUpPosition[3],vtPoDownPosition[3];
    SVector vDirAngle;
    SPoint  ptNow,ptEnemy;
    SVector vtDir;
    float vectorLen;
    float fUpAngle = mfPoUpAngle;
    CSprite* pEn = mpAICore->GetAttackTo();
    if(pEn == NULL) return;
    pEn->GetPosition(&ptEnemy);
    
    ptEnemy.y += 5.f; //포신이 조금 위에 있자나. 그래서 좀 높혀서 비율에 맞추어봤다.
    GetPosition(&ptNow);
    ptNow.y += 8;
    
    
    //미사일타워는 mfPoUpAngle값이 0이다.
    if(GetModelID() == ACTORID_BLZ_DMISSILETOWER)
        fUpAngle = 60.f; //안보일때 쏠려면 mfPoUpAngle = 0이기 때문에 60각도로 쏘자.
    
//    GetPoAngles(mnPoIndex,&fWAngle,&fHAngle,vtPoUpPosition,vtPoDownPosition,&vtDir);

    //SVector vtDir;
	vtDir.x = ptEnemy.x - ptNow.x;
    vtDir.y = sinf(fUpAngle * PI / 180.f); //ptEnemy.y - ptNow.y;
	vtDir.z = -(ptEnemy.z - ptNow.z); //z.의 방향을 반대이다.
    
//    *pOutWAngle = atan2(pvtDir->z,pvtDir->x) * 180.0 / PI;
//    *pOutHAngle = asinf(pvtDir->y) * 180.0 / PI;
	
    fWAngle = atan2(vtDir.z,vtDir.x) * 180.0 / PI;
    fHAngle = fUpAngle;
    
    fMaxLenDouble = vtDir.x*vtDir.x + vtDir.z*vtDir.z;
    fMaxLen = sqrtf(fMaxLenDouble) - 50.0f;
    float fValue = fMaxLen * 9.8f / (mBombProperty.fVelocity * mBombProperty.fVelocity);
	if(fValue >= 1.0 || fValue <= -1.0)
	{
        //        mbOnAmiedNeedFire = false;
		return;
	}
    
    fDivy = ptEnemy.y - ptNow.y;
    vectorLen = sqrtf(fMaxLenDouble + fDivy*fDivy);
    
    vtDir.x /= vectorLen;
    vtDir.y /= vectorLen;
    vtDir.z /= vectorLen;
    
    
    vDirAngle.x = fHAngle;
    vDirAngle.y = mfRotationAngle;
    vDirAngle.z = fWAngle;
    
    if(mnPoIndex == 0)
    {
        ptNow.y += 1.f;
        ptNow.x -= 1.f;
    }
    else if(mnPoIndex == 1)
    {
        ptNow.y += 1.f;
        ptNow.x += 1.f;
    }
    else if(mnPoIndex == 2)
        ptNow.x -= 1.5f;
    
    NewMissile(ptNow,vtDir,vDirAngle,false);
    
    m_nCurrentPerTime -= GetFirePerSec();
    if(m_nCurrentPerTime < 0)
        m_bHodeFire = true;
}

void  CDTower::FireOrgMissile()
{
    float fWAngle,fHAngle,vtPoUpPosition[3],vtPoDownPosition[3];
    SVector vDirAngle;
    SPoint  ptNow;
    SVector vtDir;
    
    GetPoAngles(mnPoIndex,&fWAngle,&fHAngle,vtPoUpPosition,vtPoDownPosition,&vtDir);
    
    ptNow.x = vtPoUpPosition[0];
    ptNow.y = vtPoUpPosition[1];
    ptNow.z = vtPoUpPosition[2];
    
    vDirAngle.x = fHAngle;
    vDirAngle.y = mfRotationAngle;//mpModelHeader->orientation[1];
    vDirAngle.z = fWAngle;
    
    NewMissile(ptNow,vtDir,vDirAngle);
    
    m_nCurrentPerTime -= GetFirePerSec();
    if(m_nCurrentPerTime < 0)
        m_bHodeFire = true;
}

void  CDTower::NewMissile(SPoint& ptNow,SVector& vtDir,SVector& vDirAngle,bool bNeedParticle)
{

    CSprite* pTargetSprite = mpAICore->GetAttackTo();
    CSGLCore* pCore = ((CHWorld*)mpWorld)->GetSGLCore();

    CBomb *pNewBomb = NULL;
    CFireParticle *pNewFireParticle = NULL;
    CBombTailParticle *pNewBombTailParticle = NULL;
    
    //유도기능이 존재한다.면, 타겟이 없으면 타겟을
    if(pTargetSprite == NULL && mBombProperty.fAutoDectect != 0.0f)
    {
        //타겟을 멎추어준다.
        pTargetSprite = GetTargetBySensor();
    }
    
    pNewBomb = new CBomb(pTargetSprite,this,GetID(),GetTeamID(),mBombProperty.nID,mpAction,GetWorld(),&mBombProperty);
    pNewBomb->Initialize( &ptNow, &vDirAngle,&vtDir);
    pCore->AddBomb(pNewBomb);
    if(bNeedParticle)
    {
        float fIntervalToActor = GetIntervalToCamera();
        
        pNewFireParticle = new CFireParticle(GetWorld());
        pNewBombTailParticle = new CBombTailParticle(mpWorld,pNewBomb);
        
    #ifdef ANDROID //JAVA의 터치시에 플래이가 JNI로 연결되어 잇어서 에러가 난다.
        if(fIntervalToActor < 3500)
            CScenario::SendMessage(SGL_PLAYSOUND,mBombProperty.nSoundFilreID,0);
    #else
        if(fIntervalToActor < 3500)
            pCore->PlaySystemSound(mBombProperty.nSoundFilreID);
    #endif
        
        //파티클을 추가한다.
        if(pNewFireParticle)
        {
            pNewFireParticle->Initialize(&ptNow, &vtDir);
            pCore->AddParticle(pNewFireParticle);
        }
        //BombParticle을 추가한다.
        pNewBombTailParticle->Initialize(&ptNow, &vtDir);
        pCore->AddParticle(pNewBombTailParticle);

    }
}

bool CDTower::CollidesBounding(CSprite* Sprite,bool bExact)
{
    
    if(GetState() != SPRITE_RUN) return false;
    
    SPoint pt1,pt2;
    GetPosition(&pt1);
    Sprite->GetPosition(&pt2);
    
    //1>두 정점의 거리{ 루트 ( x1*x2 + y1*y2 + z1*z2 )} < (x라디안 + y라디안)
    //  즉 x1*x2 + y1*y2 + z1*z2 < (x라디안 + y라디안) * (x라디안 + y라디안) 와 같다. (루투보다 곱하기 연산이 빠르것지요)
    //DistanceMethodsf::squareDistancePointPointf(a, b) <= (getRadiusf() + other->getRadiusf()) * (getRadiusf() + other->getRadiusf())
    float fRadius = Sprite->GetRadius();
    float fRadiusSum = (fRadius + mfRadian) / 3.f; //너무 자리를 많이 차지하여서 2로 나누어서 비켜갈수 있게 하였다.
    
    //(a[0]-b[0])*(a[0]-b[0]) + (a[1]-b[1])*(a[1]-b[1]) + (a[2]-b[2])*(a[2]-b[2]);
    float fx = pt1.x - pt2.x;
    float fy = pt1.y - pt2.y;
    float fz = pt1.z - pt2.z;
    
    float fd = (fx * fx + fy * fy + fz * fz);
    mfColidedDistance = fd;
    Sprite->SetColidedDistance(mfColidedDistance);
    if( fd < (fRadiusSum * fRadiusSum))
    {
        return true;
    }
//    mfColidedDistance = 0.0f;
    return false;
}


bool CDTower::IsTabIn(CPicking* pPick,SPoint& nearPoint,SPoint& farPoint,int nPosX,int nPosY)
{
    bool b1;
    int nVIndex1,nVIndex2,nVIndex3;
    sglMesh *arrMesh = mpModel->GetMesh();
    // 속도를 위해서 전체를 검사하지말고 해더만 검사하자.
//    int i = 1;    
//    if(mnMakingTime) i = 0;
    
    int nSize = 2; //포신이 3개 더 있지만. 자꾸 거기에 걸린다.
    if(mnMakingTime) nSize = 1;
    for (int i = 0; i < nSize; i++)
    {
        sglMesh *pWorldMesh = &mpWorldMesh[i];
        sglMesh *pMesh = &arrMesh[i];
        
        int nCntz = pMesh->shIndicesCnt * 3;
        for (int z = 0; z < nCntz; )
        {
            nVIndex1 = pMesh->pIndicesBuffer[z++]*3;
            nVIndex2 = pMesh->pIndicesBuffer[z++]*3;
            nVIndex3 = pMesh->pIndicesBuffer[z++]*3;
            b1 = pPick->sglIsPicking(&pWorldMesh->pVetexBuffer[nVIndex1],
                                     &pWorldMesh->pVetexBuffer[nVIndex2],
                                     &pWorldMesh->pVetexBuffer[nVIndex3],
                                     nearPoint,farPoint);
            if(b1)
            {
                nearPoint.x = pWorldMesh->pVetexBuffer[nVIndex1];
                nearPoint.y = pWorldMesh->pVetexBuffer[nVIndex1+1];
                nearPoint.z = pWorldMesh->pVetexBuffer[nVIndex1+2];
                
                farPoint.x = pWorldMesh->pVetexBuffer[nVIndex2];
                farPoint.y = pWorldMesh->pVetexBuffer[nVIndex2+1];
                farPoint.z = pWorldMesh->pVetexBuffer[nVIndex2+2];
                
                return true;
            }
//#if HALF_PICK
            z+=3;  //속도를 위해서 전체를 검사하지말고 버텍스의 반만 검사하자.
//#endif //HALF_PICK
        }
    }
    return false;
}

//방어력 50.f 발을 맞아야 한다.
float     CDTower::GetMaxDefendPower()
{
    return mfMaxDefendPowerActor;
}


float CDTower::GetMaxAttackPower()
{
    return m_fMaxAttackPower;
}

float CDTower::CatRotation(float fNowAngle,float fToAngle)
{
    float fDifAngle = AngleArrangeEx(fToAngle - fNowAngle);
    if(fDifAngle <= 1.f || (fDifAngle <= 365.f && fDifAngle >= 364.f)) //거의 정확한 방향으로 되어 있다면.
	{
        return 0.f; //정진
	}
	else
	{
		if(fDifAngle < 180)
			return 1.0f; //오른쪽 회전
		else
			return -1.0f; //왼쪽 회전
	}
    
}

bool CDTower::AimAutoToEnemy(CSprite *pEnemy)
{
	SPoint ptEnemy;
    pEnemy->GetPosition(&ptEnemy);
	return AimAutoToEnemy(&ptEnemy);
}

bool CDTower::AimAutoToEnemy(SPoint* ptEnemy)
{
	if(!CanFireMissile()) return false;
    if(isMovingAutoHeader()) return false;
    
    
	SVector v;
	SPoint ptMine;
    GetPosition(&ptMine);
    
    mfPo_UpDonwDir = 0.f;
	mfRotationVector= 0.f;
//	mbAutoMovePo_UpDown = false;
	
	v.x = ptEnemy->x - ptMine.x;
	v.y = ptEnemy->y - ptMine.y;
	v.z = -(ptEnemy->z - ptMine.z); //z.의 방향이 반대이기 때문에 벡터의 방향을 반대로 설정해준다.
	
    float fLen = sqrtf(v.x*v.x + v.z*v.z) - 50.0f;
    float fLenT = sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
    v.x /= fLenT;
    v.y /= fLenT;
    v.z /= fLenT;
    
    if(fLenT < 0.0000001f) //거의 0일때.
        return false;
    
    //탱크의 각도 + 포의 각도가 실제 각도이다.
    float fNowPoDir = AngleArrangeEx ( mfRotationAngle );
    
	//--------------------------------
	// 포의 회전각 = 기본적으로 탱크의 방향(180 반대?) + 두점사이각
	//--------------------------------
    mfRotationToAngle = AngleArrangeEx(atan2(v.z,v.x) * 180.0 / PI); //이 각도로 회전해야 한다.
    
    if(ACTORID_BLZ_DMISSILETOWER != GetModelID()) //미사일타워는 거리를 구할 필요가 없다. 타겟만 정해지면 유도해서 날아간다.
    {
        // -------------------------------
        //두 점사이의 거리....
        // -------------------------------
        //포가 공중에 있는데 너무 멀리간다.(- 50.f) 정확하지 않지만 거리를 조정해준다.
        float fValue = fLen * 9.8f / (mBombProperty.fVelocity * mBombProperty.fVelocity);
        if(fValue >= 1.0 || fValue <= -1.0)
        {
            mbOnAmiedNeedFire = false;
            return false;
        }
        
        //+asinf(v.y) 높이/빞변 v.y = v.y/fLenT 평자가 아니라 산악일때
        float fHeightAngleRadian = asinf(fValue) / 2.0f + asinf(v.y);
        mfPoUpToAngle = 180 * fHeightAngleRadian / PI;
    }
    
    float fRotDir = CatRotation(fNowPoDir,mfRotationToAngle);
    if(fRotDir == 0.f)
    {
        CheckAutoMove_PoUpDown();
        mfRotationVector = 0.f;
    }
    else
    {
        mfRotationVector = fRotDir * mProperty.fHeaderTurnSize;
    }
    
    if(mfRotationVector == 0.f && mfPo_UpDonwDir == 0.f)
        mbOnAmiedNeedFire = true;
	return true;
}


void CDTower::RenderBeginAutoTurnHeader(int nTime)
{
	if(mfRotationVector != 0.0f)
	{
        float fNowAngle = mfRotationAngle;
        float fToPoAngle = fNowAngle + mfRotationVector * (float)nTime;
        float fRotDir = CatRotation(fToPoAngle, mfRotationToAngle);
        if(fRotDir == 0.f)
        {
            mfRotationVector = 0.f;
            mfRotationToAngle = 0.f;
            CheckAutoMove_PoUpDown();
            if(mfRotationVector == 0.f && mfPo_UpDonwDir == 0.f)
                mbOnAmiedNeedFire = true;
            return ;
        }
        else
        {
            
            if((mfRotationVector > 0 && fRotDir < 0) || ( mfRotationVector < 0 && fRotDir > 0)) //방향이 지나쳤다.
            {
                mfRotationAngle = mfRotationToAngle;
                
                mfRotationVector = 0.f;
                mfRotationToAngle = 0.f;
                CheckAutoMove_PoUpDown();
                if(mfRotationVector == 0.f && mfPo_UpDonwDir == 0.f)
                    mbOnAmiedNeedFire = true;
                return ;
            }
            mfRotationAngle = fToPoAngle;
        }
	}
}


void CDTower::CheckAutoMove_PoUpDown()
{
    if(ACTORID_BLZ_DMISSILETOWER != GetModelID()) //미사일타워는 거리를 구할 필요가 없다. 타겟만 정해지면 유도해서 날아간다.
    {
        if(m_bIsVisibleRender)
        {
            float pW,pH;
            float fUpPos[3],fDownPos[3];
            SVector vtDir;
            
            if(mnPoUpgrade != 1)
                GetPoAngles(0,&pW,&pH,fUpPos,fDownPos,&vtDir);
            else //한단계 업그레이드를 하면 포일이 없다. 그래서 기준을 포2로 맞추어야 한다.
                GetPoAngles(1,&pW,&pH,fUpPos,fDownPos,&vtDir);
            
            // 위의 방향으로 이동한다.
            if(mfPoUpToAngle  > pH)
                mfPo_UpDonwDir = 1.f; //위방향
            else if(mfPoUpToAngle  < pH)
                mfPo_UpDonwDir = -1.f; //아래방향
            else
                mfPo_UpDonwDir = 0.f;
        }
        else
        {
            mfPoUpAngle = mfPoUpToAngle;
            mfPo_UpDonwDir = 0.0f;
        }
    }
}

void CDTower::GetPoAngles(int nPoIndex,float* pOutWAngle,float* pOutHAngle,float* pOutUpPos,float* pOutDownPos,SVector *pvtDir)
{

    float *vtUp,*vtDown;
    
    //nPoIndex 포인덱스를 사용하여 각도를 구해온다. //2,4,2;3,4,2;4,4,2
    vtUp = &mpWorldMesh[mptStartMissile[nPoIndex][0]].pVetexBuffer[mptStartMissile[nPoIndex][1] * 3];
    vtDown = &mpWorldMesh[mptStartMissile[nPoIndex][0]].pVetexBuffer[mptStartMissile[nPoIndex][2] * 3];

	pOutUpPos[0] = vtUp[0];
	pOutUpPos[1] = vtUp[1];
	pOutUpPos[2] = vtUp[2];
	
    
	pOutDownPos[0] = vtDown[0];
	pOutDownPos[1] = vtDown[1];
	pOutDownPos[2] = vtDown[2];
	
	
	//SVector vtDir;
	pvtDir->x = vtUp[0] - vtDown[0];
	pvtDir->y = vtUp[1] - vtDown[1];
	pvtDir->z = -(vtUp[2] - vtDown[2]); //z.의 방향을 반대이다.
    
    float vectorLen = sqrtf(pvtDir->x*pvtDir->x + pvtDir->y*pvtDir->y + pvtDir->z*pvtDir->z);
    
    pvtDir->x /= vectorLen;
    pvtDir->y /= vectorLen;
    pvtDir->z /= vectorLen;
	
	*pOutWAngle = atan2(pvtDir->z,pvtDir->x) * 180.0 / PI;
    *pOutHAngle = asinf(pvtDir->y) * 180.0 / PI;
}


void CDTower::CampactAnimaion(int nTime)
{
    if(mnAniGGTime == 0)
        mnAniGGTime = GetGGTime() + 1000l;
    else if( GetGGTime() > mnAniGGTime)
        SetState(SPRITE_READYDELETE);
    
    float fT = nTime;
    mfAniBottomAngle -= fT * 0.1f;
    mfAniHeaderAngle += fT * 0.1f;

    mfAniHeaderY += fT * 0.01;  //10.f * fT / 1000.f;
    mfAniHeaderX += fT * 0.04;  //40.f * fT / 1000.f;
    mfAniBottomY += fT * 0.015; //15.f * fT / 1000.f;
    mfAniBottomZ += fT * 0.03;  //30.f * fT / 1000.f;

}

bool CDTower::VisibleByCamera()
{
    float fD1[3];
    float fD2[3];
    SPoint ptNow;
    GetPosition(&ptNow);
    
    SGLCAMERA* pcam = mpWorld->GetCamera();
    fD1[0] = pcam->viewDir.x;
    fD1[1] = pcam->viewDir.y;
    fD1[2] = pcam->viewDir.z;
    
    fD2[0] = ptNow.x - pcam->viewPos.x;
    fD2[1] = ptNow.y - pcam->viewPos.y;
    fD2[2] = ptNow.z - pcam->viewPos.z;
    sglNormalizef(fD2);
    
    //fDV > 0 90보다 작다
    //fDV < 0 90보다 크다.
    //fDV = 0 90
    float fDV = sglDotProductf(fD1, fD2);
    
    if(fDV > 0.6f) return true;
    
    return false;
    
}

void CDTower::ExcuteCmd()
{
    mshMainTowerBubbleCntInRange = 0;
    if(mExcuteCmd & SGL_NEED_EXECUTE_IN_DMAINTOWER) //메인타워 안쪽으로 드러가서 에너지를 필할수 있게 하였다.
    {
        float fPower = GetMaxDefendPower() / 10.f;
        SetCurrentDefendPower(GetCurrentDefendPower() + fPower);
        CHWorld* pWorld = (CHWorld*)GetWorld();
        
        //Added By Song 2014.05.20 이전버전에서 죽었다. 쓰레드 아닐때만 화면에 셋해야 한다. 그렇지 않으면 Render()시에 죽어 버린다.
        if(IsVisibleRender())
        {
            if(GetIntervalToCamera() < 3500.f)
                pWorld->GetSGLCore()->PlaySystemSound(SOUND_ID_EnergyUP);
            //Fill Up~~~~을 할경우..
            SPoint pt2;
            GetPosition(&pt2);
            CAniRuntimeEnergyUP *pItem = new CAniRuntimeEnergyUP(mpWorld,this);
            pItem->Initialize(&pt2, NULL);
            CSGLCore *pCore = ((CHWorld*)mpWorld)->GetSGLCore();
            pCore->AddParticle(pItem);
        }
        mExcuteCmd &=~SGL_NEED_EXECUTE_IN_DMAINTOWER;
    }
    else if(mExcuteCmd & SGL_NEED_EXECUTE_CHGOLD_IN_DMAINTOWER)
    {
        //mExcuteCmd &= ~SGL_NEED_EXECUTE_CHGOLD_IN_DMAINTOWER;
        mshMainTowerBubbleCntInRange = mExcuteCmd;
        mExcuteCmd = 0;
    }

}


void CDTower::ArchiveMultiplay(int nCmd,CArchive& ar,bool bWrite)
{
    CSprite::ArchiveMultiplay(nCmd, ar, bWrite);
    
    //    bool bBubble = false;
    int nInterval = 0;
    
    //    명령어에 따라 위치와 방향을 읽고 쓴다.
    if(nCmd == SUBCMD_OBJ_ALL_INFO)
    {
        if(bWrite)
        {
            if(mnMakingTime)
                nInterval = mnMakingTime - GetGGTime();
            
            ar <<      mfRotationAngle;
            ar <<      mfRotationVector;
            ar <<      mfRotationToAngle;		//이각도 방향으로 이동
            ar <<      mfPoUpAngle;
            ar <<      mfPoUpToAngle;
            ar <<      mfPo_UpDonwDir; //-1아래 1위방향으로 이동
            ar <<      mnPoUpgrade;
            ar <<      mnPoIndex;
            ar <<      nInterval;
            ar <<      mnConstMakingTime;
        }
        else
        {
            
            ar >>      mfRotationAngle;
            ar >>      mfRotationVector;
            ar >>      mfRotationToAngle;		//이각도 방향으로 이동
            ar >>      mfPoUpAngle;
            ar >>      mfPoUpToAngle;
            ar >>      mfPo_UpDonwDir; //-1아래 1위방향으로 이동
            ar >>      mnPoUpgrade;
            ar >>      mnPoIndex;
            ar >>      nInterval;
            ar >>      mnConstMakingTime;
        }
    }
}