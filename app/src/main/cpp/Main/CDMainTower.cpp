//
//  CDMainTower.cpp
//  SongGL
//
//  Created by itsme on 13. 10. 25..
//
//
#include "sGL.h"
#include "CHWorld.h"
#include "CDMainTower.h"
#include "CSGLCore.h"
#include "sGLUtils.h"
#include "CAICore.h"
#include "CStrategyBlzAI.h"
#include "CMS3DModelASCIIMan.h"
#include "CShadowAreaParticle.h"
#include "CStrategyDBlzAI.h"
#include "CDMainTowerStatus.h"
#include "CMutiplyProtocol.h"


CDMainTower::CDMainTower(unsigned int nID,unsigned char cTeamID,int nModelID,CTextureMan *pTextureMan,IAction *pAction,IHWorld* pWorld,PROPERTY_TANK* pProperty,NETWORK_TEAMTYPE NetworkTeamType) :
CSprite(nID,cTeamID,nModelID,pAction,pWorld,pProperty,NetworkTeamType)
{
    mpModel = NULL;
    mpWorldMesh = NULL;
    mfRotationAngle = 0.0f;
    mpAICore = new CStrategyDBlzAI(this);
    mfNowStatus = MAXMAINSTATUS;
    mnMakingTime = 0;
    
    mnAniGGTime = 0;
    mfAniBottomAngle = 0.f;
    mfAniHeaderAngle = 0.f;
    mfAniBottomY = 0.f;
    mfAniBottomZ = 0.f;
    mfAniHeaderY = 0.f;
    mfAniHeaderX = 0.f;
    mpBubbleParticle = NULL;
    mnConstMakingTime = MAKINGTIME;
//    mpMakingObject[0] = NULL;
//    mpMakingObject[1] = NULL;
    memset(mpMakingObject, 0, sizeof(mpMakingObject));
    mnMakePosIndex = 0;
    mNetworkTeamType = NetworkTeamType;
}

CDMainTower::~CDMainTower()
{
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

int CDMainTower::LoadResoure(CModelMan* pModelMan)
{
    CHWorld* pWorld = (CHWorld*)mpWorld;
    string sPath = mProperty.sModelPath;
    sPath.append(".txt");
    //mpModel = pWorld->GetSGLCore()->GetMS3DModelASCIIMan()->Load(sPath.c_str(),true);
    pWorld->GetSGLCore()->GetModelMan()->getModelfAscii(sPath.c_str(), &mpModel,true);
    
    if(mpModel == NULL)
    {
        HLogE("[Error] %s can't load\n",sPath.c_str());
        return -1;
    }
    
     mnType = mProperty.nType;
    
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
    
    if(GetNetworkTeamType() == NETWORK_MYSIDE)
    {
        if(mpWorld->GetUserInfo()->GetRank() == 0)
            mnConstMakingTime = MAKINGTIME / 5; //건물 짓는데 30초 걸린다.
        else if(mpWorld->GetUserInfo()->GetCntByIDWithItem(ID_TIEM_FASTBUILD))
            mnConstMakingTime = MAKINGTIME / 2;
    }
    
    mnMakingTime = GetGGTime() + mnConstMakingTime; //건물 짓는데 30초 걸린다.

    mpStatusBar = new CDMainTowerStatus(this);
    mpStatusBar->Initialize(NULL, NULL);

    mpStatusBar->SetSubBarRate(0.f);
    return E_SUCCESS;
}

float CDMainTower::GetArearRadus()
{
    return mProperty.fHalfZ;
}

int CDMainTower::Initialize(SPoint *pPosition,SVector *pvDirection)
{
    //7 ~ 18
    mfRadian = mProperty.fHalfX;
//    mnMakingTime = GetGGTime() + MAKINGTIME; //건물 짓는데 30초 걸린다.
//    mpStatusBar->SetSubBarRate(0.f);
    //그림자 파티클을 생성한다.
    mpShadowParticle = new CShadowAreaParticle(mpWorld,this);
    ((CShadowAreaParticle*)mpShadowParticle)->Initialize(pPosition, pvDirection,mProperty.fHalfZ);
    CSGLCore* pCore = ((CHWorld*)mpWorld)->GetSGLCore();
    pCore->AddParticle(mpShadowParticle);
    
    InitRuntimeUpgrade();
    
    return CSprite::Initialize(pPosition, pvDirection);
}

bool CDMainTower::IsTabIn(CPicking* pPick,SPoint& nearPoint,SPoint& farPoint,int nPosX,int nPosY)
{
    bool b1;
    int nVIndex1,nVIndex2,nVIndex3;
    sglMesh *arrMesh = mpModel->GetMesh();
    // 속도를 위해서 전체를 검사하지말고 해더만 검사하자.
//    int i = 1;
//    if(mnMakingTime) i = 0;
    
//    int nSize = mpModel->GetMeshSize();
    int nSize = 2;
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
//    #if HALF_PICK
            z+=3;  //속도를 위해서 전체를 검사하지말고 버텍스의 반만 검사하자.
//    #endif //HALF_PICK
        }
    }
    return false;
}

void CDMainTower::RenderBeginCore(int nTime)
{
    if(mnMakingTime)
    {
        unsigned long nInterval = mnMakingTime - GetGGTime();
        if(nInterval > mnConstMakingTime)
            nInterval = mnConstMakingTime;
        if(GetGGTime() >= mnMakingTime)
        {
//            mnMakingTime = 0;
//            SetCurrentDefendPowerEn(MAXMAINSTATUS);
        }
        else
            mpStatusBar->SetSubBarRate((float)(mnConstMakingTime - nInterval) / (float)mnConstMakingTime);
    }
    else
    {
        if(mfNowStatus < MAXMAINSTATUS)
            SetCurrentDefendPowerEn(mfNowStatus + nTime / 2000.f); //2초에 1올라간다. 즉 10초에 5이 올라간다.
        else
            SetCurrentDefendPowerEn(MAXMAINSTATUS);
    }
    
    if(mpMakingObject[0] && mpMakingObject[0]->GetState() != SPRITE_RUN)
        mpMakingObject[0] = NULL;
    if(mpMakingObject[1] && mpMakingObject[1]->GetState() != SPRITE_RUN)
        mpMakingObject[1] = NULL;

    
    CSprite::RenderBeginCore(nTime);
}

void CDMainTower::RenderBeginCore_Invisiable(int nTime)
{
    if(mState == SPRITE_RUN)
    {
        CSprite::RenderBeginCore_Invisiable(nTime);
        if(mnMakingTime)
        {
            if(GetGGTime() >= mnMakingTime)
            {
                mnMakingTime = 0;
                SetCurrentDefendPowerEn(MAXMAINSTATUS);
            }
        }
        else
        {
            RuntimeUpgrading(nTime);
            
            if(mfNowStatus < MAXMAINSTATUS)
                SetCurrentDefendPowerEn(mfNowStatus + nTime / 2000.f); //2초에 1올라간다. 즉 10초에 5이 올라간다.
            else
                SetCurrentDefendPowerEn(MAXMAINSTATUS);
        }

        if(mpMakingObject[0] && mpMakingObject[0]->GetState() != SPRITE_RUN)
            mpMakingObject[0] = NULL;
        if(mpMakingObject[1] && mpMakingObject[1]->GetState() != SPRITE_RUN)
            mpMakingObject[1] = NULL;

    }
    else if(mState == BOMB_COMPACT || mState == BOMB_COMPACT_ANI)
    {
        SetState(SPRITE_READYDELETE);
    }
}

void CDMainTower::SetState(SPRITE_STATE v)
{
    CSprite::SetState(v);
    
    if(v == SPRITE_READYDELETE)
    {
        if(mpMakingObject[0] && mpMakingObject[0]->GetState() == SPRITE_RUN)
        {
            mpMakingObject[0]->SetState(SPRITE_READYDELETE);
            mpMakingObject[0] = NULL;
        }
        if(mpMakingObject[1] && mpMakingObject[1]->GetState() == SPRITE_RUN)
        {
            mpMakingObject[1]->SetState(SPRITE_READYDELETE);
            mpMakingObject[1] = NULL;
        }
    }
}

void CDMainTower::ResetPower(bool bRunTimeUpgrade)
{
    if(bRunTimeUpgrade == true)
    {
        float f = GetCurrentDefendPowerEn() + MAXMAINSTATUS / 2.0f;
        if(f > MAXMAINSTATUS) f = MAXMAINSTATUS;
        SetCurrentDefendPowerEn(f);
    }
    CSprite::ResetPower(bRunTimeUpgrade);
    
    if(bRunTimeUpgrade == false)
        ((CHWorld*)mpWorld)-> OnSelectedObjectShowDescBar(this);
    else
    {
        ((CHWorld*)mpWorld)-> OnSelectedObjectShowDescBar(this,SGLTextUnit::GetOnlyText("Completed Level Upgrade"));
    }
}



int     CDMainTower::RenderBegin(int nTime)
{
    SPoint ptNow;
    float fScale = mProperty.fScale; //0.07
    
    float matrix[16];
    float matrix2[16];
    int nIndex = 0;
    int nSize = mpModel->GetMeshSize();
    sglMesh *arrMesh = mpModel->GetMesh();
    sglMesh *arrWroldMesh = mpWorldMesh;

    if(mState == BOMB_COMPACT || mState == BOMB_COMPACT_ANI)
    {
        CampactAnimaion(nTime);
    }

    if(mnMakingTime)
    {
        if(GetGGTime() >= mnMakingTime)
        {
            mnMakingTime = 0;
            SetCurrentDefendPowerEn(MAXMAINSTATUS);
            ((CHWorld*)GetWorld())->ResetClickedSpriteSlotFromMakingTime(this);
        }
    }
    
    if(mnMakingTime == 0)
    {
        RuntimeUpgrading(nTime);
        
        //바닥..
        sglLoadIdentityf(matrix);
        GetPosition(&ptNow);
        sglTranslateMatrixf(matrix,ptNow.x,ptNow.y + 1.f,ptNow.z); //회전전에 이동을 해야 한다.
        sglScaleMatrixf(matrix, fScale, fScale, fScale);
        
        
        //해더
        sglLoadIdentityf(matrix2);
        sglTranslateMatrixf(matrix2,ptNow.x,ptNow.y + 1.f + mfAniBottomY,ptNow.z + mfAniBottomZ); //회전전에 이동을 해야 한다.
        sglRotateRzRyRxMatrixf(matrix2,
                               0.f,
                               mfRotationAngle,
                               0.f);
        mfRotationAngle += nTime / 10.f;
        if(mfAniBottomAngle != 0.f)
            sglRotateRzRyRxMatrixf(matrix2,
                                   0.f,
                                   0.f,
                                   mfAniBottomAngle);
        sglScaleMatrixf(matrix2, fScale, fScale, fScale);
        
        
        for(int j = 0; j < nSize; j++)
        {
            sglMesh* pMesh = &arrMesh[j];
            sglMesh* pWorldMesh = &arrWroldMesh[j];
            
            for(int i = 0; i < pMesh->nVetexCnt; i++)
            {
                nIndex = i*3;
                
                if(j != 0) //해더
                {
                    //좌표를 이동한다.
                    sglMultMatrixVectorf(&pWorldMesh->pVetexBuffer[nIndex],
                                         matrix2,
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
                    
                    if(j != 0) //해더
                    {
                        //좌표를 이동한다.
                        sglMultMatrixVectorf(&pWorldMesh->pNormalBuffer[nIndex],
                                             matrix2,
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
    }
    else
    {
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
    }
    
    CSprite::RenderBegin(nTime);
    return E_SUCCESS;
}

int     CDMainTower::Render()
{
    int nSize = mpModel->GetMeshSize();
    sglMesh *arrMesh = mpModel->GetMesh();
    sglMesh *arrWroldMesh = mpWorldMesh;
    
    glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    
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
    
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    return E_SUCCESS;

}

int CDMainTower::RenderEnd()
{
    return E_SUCCESS;
}

int CDMainTower::OnEvent(SGLEvent *pEvent)
{
    CHWorld* pWorld = (CHWorld*)GetWorld();
    static int arrModelID[] = {ACTORID_ARMEREDCAR,ACTORID_AMBULANCE,ACTORID_XTANK,ACTORID_MISSILE};
    CUserInfo* pUserInfo;
    int nRuntimUpgradeID;
    int nItemID;

    if(pEvent->nMsgID == SGL_MAKING_ASSISTOBJECTS) //CStratgyDBlzAI에서 액터팀일때만 만들게 하였다.
    {
        pUserInfo = pWorld->GetUserInfo();
        
        int nIndex = GetCanMakingObjectIndex();
        if(nIndex != -1)
        {
            int nSize = sizeof(arrModelID) / sizeof(int);
            
            int* pResult = new int[nSize];
            memcpy(pResult, arrModelID, sizeof(arrModelID));
            
            IAction* pAction = pWorld->GetSGLCore()->GetAction();
            
            int nResult = pAction->OnMainTowerMakeAssistObject(GetTeamID(), nSize, pResult);
            
            if(nResult > 0)
            {
                for (int i = 0; i < nSize; i++)
                {
                    if(pResult[i] != -1)
                    {
                        nItemID = arrModelID[i];
                        if(nItemID == ACTORID_ARMEREDCAR)
                            nRuntimUpgradeID = RUNTIME_UPDATE_ARMCAR;
                        else if(nItemID == ACTORID_AMBULANCE)
                            nRuntimUpgradeID = RUNTIME_UPDATE_AMBU;
                        else if(nItemID == ACTORID_XTANK)
                            nRuntimUpgradeID = RUNTIME_UPDATE_TANKCAR;
                        else if(nItemID == ACTORID_MISSILE)
                            nRuntimUpgradeID = RUNTIME_UPDATE_MISSILET;
                        
                        if(pUserInfo->GetCntByIDWithItem(nItemID) > pResult[i])
                        {
                            SetRuntimeUpgrade(nRuntimUpgradeID); //탱크를 만들어준다.
                            break;
                        }
                    }
                }
            }
            
            delete[] pResult;
        }
    }
    return E_SUCCESS;
}


void CDMainTower::SetCurrentDefendPowerEn(float v)
{
    if(mnMakingTime == 0)
    {
        mfNowStatus = v;
        mpStatusBar->SetSubBarRate(1.0f - (MAXMAINSTATUS - mfNowStatus) / MAXMAINSTATUS);
    }
}

bool CDMainTower::VisibleByCamera()
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

bool CDMainTower::CollidesBounding(CSprite* Sprite,bool bExact)
{
    
    if(GetState() != SPRITE_RUN) return false;
//    if(GetTeamID() != Sprite->GetTeamID()) return false; //다른 팀은 거리를 계산하지 말자. 거리를 계산하지 않으면 먼거리에서 적이 가까운 줄알고 공격한다.
    
    SPoint pt1,pt2;
    GetPosition(&pt1);
    Sprite->GetPosition(&pt2);
    float fx = pt1.x - pt2.x;
    float fy = pt1.y - pt2.y;
    float fz = pt1.z - pt2.z;
    
    float fd = (fx * fx + fy * fy + fz * fz);
    mfColidedDistance = fd;
    Sprite->SetColidedDistance(mfColidedDistance);
    return false;
}


void CDMainTower::CampactAnimaion(int nTime)
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

int CDMainTower::GetCanMakingObjectIndex()
{
    if(!mpMakingObject[0]) return 0;
    else if(!mpMakingObject[1]) return 1;
    return -1;
}

void CDMainTower::OnCompletedUpdated(RUNTIME_UPGRADE* prop,CArchive* pReadArc)
{
    bool bMainTowerUpgrade = false;
    CSprite::OnCompletedUpdated(prop,pReadArc);
    
    CSprite* pNewSprite = NULL;
    if(prop->nID == RUNTIME_UPDATE_ARMCAR)
    {
        if(mpMakingObject[0] && mpMakingObject[0]->GetModelID() == ACTORID_ARMEREDCAR)
        {

            //다만들었음
            mpMakingObject[0]->Command(AICOMMAND_COMPLETEDMAKE, NULL);
            pNewSprite = mpMakingObject[0];
            mpMakingObject[0] = NULL;
        }
        else if(mpMakingObject[1] && mpMakingObject[1]->GetModelID() == ACTORID_ARMEREDCAR)
        {
            //다만들었음
            mpMakingObject[1]->Command(AICOMMAND_COMPLETEDMAKE, NULL);
            pNewSprite = mpMakingObject[1];
            mpMakingObject[1] = NULL;
        }
        bMainTowerUpgrade = true;
    }
    else if(prop->nID == RUNTIME_UPDATE_AMBU)
    {
        if(mpMakingObject[0] && mpMakingObject[0]->GetModelID() == ACTORID_AMBULANCE)
        {
            //다만들었음
            mpMakingObject[0]->Command(AICOMMAND_COMPLETEDMAKE, NULL);
            pNewSprite = mpMakingObject[0];
            mpMakingObject[0] = NULL;
        }
        else if(mpMakingObject[1] && mpMakingObject[1]->GetModelID() == ACTORID_AMBULANCE)
        {
            //다만들었음
            mpMakingObject[1]->Command(AICOMMAND_COMPLETEDMAKE, NULL);
            pNewSprite = mpMakingObject[1];
            mpMakingObject[1] = NULL;
        }
        bMainTowerUpgrade = true;
    }
    else if(prop->nID == RUNTIME_UPDATE_TANKCAR)
    {
        if(mpMakingObject[0] && mpMakingObject[0]->GetModelID() == ACTORID_XTANK)
        {
            //다만들었음
            mpMakingObject[0]->Command(AICOMMAND_COMPLETEDMAKE, NULL);
            pNewSprite = mpMakingObject[0];
            mpMakingObject[0] = NULL;
        }
        else if(mpMakingObject[1] && mpMakingObject[1]->GetModelID() == ACTORID_XTANK)
        {
            //다만들었음
            mpMakingObject[1]->Command(AICOMMAND_COMPLETEDMAKE, NULL);
            pNewSprite = mpMakingObject[1];
            mpMakingObject[1] = NULL;
        }
        bMainTowerUpgrade = true;
    }
    else if(prop->nID == RUNTIME_UPDATE_MISSILET)
    {
        if(mpMakingObject[0] && mpMakingObject[0]->GetModelID() == ACTORID_MISSILE)
        {
            //다만들었음
            mpMakingObject[0]->Command(AICOMMAND_COMPLETEDMAKE, NULL);
            pNewSprite = mpMakingObject[0];
            mpMakingObject[0] = NULL;
        }
        else if(mpMakingObject[1] && mpMakingObject[1]->GetModelID() == ACTORID_MISSILE)
        {
            //다만들었음
            mpMakingObject[1]->Command(AICOMMAND_COMPLETEDMAKE, NULL);
            pNewSprite = mpMakingObject[1];
            mpMakingObject[1] = NULL;
        }
        bMainTowerUpgrade = true;
    }

    if(bMainTowerUpgrade)
    {
        int nID,nModelID;
        if(pReadArc == NULL && pNewSprite)
        {
            nID = GetID();
            nModelID = GetModelID();
            CHWorld* pWorld = (CHWorld*)GetWorld();
            if(!pWorld->GetMutiplyProtocol()->IsStop())
            {
                CArchive arc;
                arc << nID;
                arc << prop->nID;
                
                pNewSprite->ArchiveMultiplay(SUBCMD_OBJ_ALL_INFO, arc, true);
                pWorld->GetMutiplyProtocol()->SendSyncCompletedRuntimeUpgrade(&arc);
            }
        }
        else if(pReadArc)
        {
            int nSpriteID;
            int nSpriteModelID;
            *pReadArc >> nID;
            *pReadArc >> prop->nID;
            
            *pReadArc >> nSpriteID;
            *pReadArc >> nSpriteModelID;
            pReadArc->SetPosition(sizeof(int) * 2);

            SPoint ptActor;
            SVector vtDir;
            vtDir.x = 0;
            vtDir.y = 0;
            vtDir.z = 1;
           // memset(&ptActor, 0, sizeof(SPoint));
            GetPosition(&ptActor);
            
            CSGLCore* pSGLCore = ((CHWorld*)GetWorld())->GetSGLCore();
            //MakeSprite를 해준다.
            CSprite* pNewSp = CSGLCore::MakeSprite(nSpriteID, GetTeamID(), nSpriteModelID, GetWorld()->GetTextureMan(), pSGLCore->GetAction(), GetWorld(),NETWORK_OTHERSIDE);
            pNewSp->LoadResoure(GetWorld()->GetModelMan());
            pNewSp->Initialize(&ptActor, &vtDir);
            pNewSp->ArchiveMultiplay(SUBCMD_OBJ_ALL_INFO,*pReadArc,false);
            pSGLCore->AddSprite(pNewSp);
        }
    }
    
}


bool CDMainTower::SetRuntimeUpgrade(int nID)
{
    int nModelID = 0;
    int nMakingIndex = GetCanMakingObjectIndex();
    
    CUserInfo* pUserInfo = GetWorld()->GetUserInfo();
    CHWorld* pHWorld = (CHWorld*)GetWorld();
    RUNTIME_UPGRADE *prop = 0;

    if(nID == RUNTIME_UPDATE_ARMCAR)
        nModelID = ACTORID_ARMEREDCAR;
    else if(nID == RUNTIME_UPDATE_AMBU)
        nModelID = ACTORID_AMBULANCE;
    else if(nID == RUNTIME_UPDATE_TANKCAR)
        nModelID = ACTORID_XTANK;
    else if(nID == RUNTIME_UPDATE_MISSILET)
        nModelID = ACTORID_MISSILE;
    
    //만드는 슬롯은 두개이다. 꽉찼으면 못하게 막자.
    if(nModelID != 0 && nMakingIndex == -1)
        return false;
    
    for (vector<USERINFO*>::iterator it = mpRuntimeUpgradeList->begin(); it != mpRuntimeUpgradeList->end(); it++)
    {
        if((*it)->nID == nID)
        {
            prop = (RUNTIME_UPGRADE*)(*it)->pProp;
            if(pUserInfo->GetGold() < prop->nGold || prop->nUpgradeCnt >= prop->nUpgradeMaxCnt) return false;
            prop->nUpgreadTime = 0;
            mpRuntimeUpgradeingList.push_back(*it);

//#define RUNTIME_UPDATE_ARMCAR       1009 //501 //장갑차
//#define RUNTIME_UPDATE_AMBU         1010 //502 //앰블런스
//#define RUNTIME_UPDATE_TANKCAR      1011 //503 //탱크장갑차
//#define RUNTIME_UPDATE_MISSILET     1012 //500 //미사일차
            
            if(nModelID != 0)
            {
                mpMakingObject[nMakingIndex] =
                CSGLCore::MakeSprite(pHWorld->GetNewID(), GetTeamID(), nModelID, pHWorld->GetTextureMan(), pHWorld->GetSGLCore()->GetAction(), pHWorld,NETWORK_MYSIDE);
                mpMakingObject[nMakingIndex]->LoadResoure(pHWorld->GetModelMan());
                SVector vtDir;
                SPoint ptCenter;
                GetPosition(&ptCenter);
                vtDir.x = -1;
                vtDir.y = 0;
                vtDir.z = 0;
                float fRadias = 30.f;
                
                mpMakingObject[nMakingIndex]->Initialize(&ptCenter, &vtDir);
                
                mnMakePosIndex = pHWorld->SetMakePosition(fRadias,mnMakePosIndex + 1,mpMakingObject[nMakingIndex],&ptCenter);
                pHWorld->GetSGLCore()->AddSprite(mpMakingObject[nMakingIndex]);
                //아무것도 안하는 객체로 만든다.
                mpMakingObject[nMakingIndex]->Command(AICOMMAND_MAINKING, NULL);
                
                if(pHWorld->GetClickedSprite() == this)
                    pHWorld->ResetDisplayItemSlot();
            }

            
            pUserInfo->SetGold(pUserInfo->GetGold() - prop->nGold);
            pHWorld->ResetDisplayTopGold();
            return true;
        }
    }
    return false;
}



void CDMainTower::ArchiveMultiplay(int nCmd,CArchive& ar,bool bWrite)
{
    CSprite::ArchiveMultiplay(nCmd, ar, bWrite);
    int nInterval = 0;
    
    //    명령어에 따라 위치와 방향을 읽고 쓴다.
    if(nCmd == SUBCMD_OBJ_ALL_INFO)
    {
        if(bWrite)
        {
            if(mnMakingTime)
                nInterval = mnMakingTime - GetGGTime();
            ar << mfNowStatus;
            ar << nInterval;
            ar << mnConstMakingTime;
        }
        else
        {
            
            ar >> mfNowStatus;
            ar >> nInterval;
            mnMakingTime = GetGGTime() + nInterval;
            ar >> mnConstMakingTime;
        }
    }
}