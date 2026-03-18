/*
 *  untitled.cpp
 *  SongGL
 *
 *  Created by 호학 송 on 10. 11. 10..
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#include <algorithm>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "CHWorld.h"
#include "ARender.h"
#include "AMesh.h"
#include "sGL.h"
#include "sGLTrasform.h"
#include "sGLFile.h"
#include "CK9Sprite.h"
#include "sGLUtils.h"
#include "CSGLMesh.h"
#include "CFireParticle.h"
#include "CBombTailParticle.h"
#include "CCompatFireParticle.h"
#include "CSGLCore.h"
#include "CTextureMan.h"
#include "C2dRadaBoard.h"
#include "CET1.h"
#include "CET2.h"
#include "CTE1BoosterParticle.h"
#include "CGhostSprite.h"
#include "CEMainBlzSprite.h"
#include "CSGLController.h"
#include "AAICore.h"
#include "CBackground.h"
#include "sGLText.h"
#include "CLabelCtl.h"
#include "CNumberCtl.h"
#include "CButtonCtl.h"
#include "CTableCtl.h"
#include "CCellCtl.h"
#include "CBombCell.h"
#include "CItemCell.h"
#include "CMessageCtl.h"
#include "sGLText.h"
#include "CParticleEmitterMan.h"
#include "CStrategyAI.h"
#include "sGLSound.h"
#include "CImpactFlyParticle.h"
#include "CItemMan.h"
#include "CItemSprite.h"
#include "CSpriteStatus.h"
#include "CAniRuntimeBubble.h"
#include "CET2AttackParticle.h"
#include "CImgProgressCtl.h"
#include "CProgressCtl.h"
#include "CArchive.h"
#include "CDTower.h"
#include "CDMainTower.h"
#include "CMutiplyProtocol.h"
#include "CIndicatorCtl.h"

#define BTN_PLAY_SHOP            1030
#define BTN_BOMBTABLE            1031
#define BTN_ITEMTABLE            1032
#define BTN_BOMBBUY              1033
#define BTN_ITEMBUY              1034
#define BTN_BOMBTAB         1035
#define BTN_ITEMTAB         1036

#define BTN_BOMBBUYOK           1037
#define BTN_ITEMBUYOK           1038
#define BTN_ITEMBOMBBUYCANCEL   1039

#define CTL_TANK_DESC_TANKNAME  1040
#define CTL_TANK_DESC_LEVEL     1041
#define CTL_TANK_DESC_ICON      1042
#define CTL_TANK_DESC_ATTACK    1043
#define CTL_TANK_DESC_DEFEND    1044
#define CTL_TANK_DESC_DESC      1045

#define BTN_MULTIPLAY_ALIAS     1046 //동맹 버튼

extern unsigned int mEscapeT;
#if defined(ANDROIDAPPSTOREKIT)
extern "C" const char* sglReadCacheProduct();
#endif

CHWorld::CHWorld(CScenario* pScenario) : CWorld(pScenario)
{
    
    mBackground = NULL;
    m_pSGLCore = new CSGLCore(this);
    m_pSGLCore->SetPicking(&mPicking);
    mpThread = new CSThread();
    mpThreadQueue = new HQueue<int>();
    
    
    //0x00 00 00 00
    mMaxID = mpScenario->GetMutiplyProtocol()->GetMyDeviceActorTeamID() << 15; //팀별로 유니크 해야 한다.
    
    mpRadaBoard = new C2dRadaBoard(this);
    mpStick = new C2dStick(this);
    
	mpCurrRuntimeRenderObjects = NULL;
    mpAliveNumCtl = NULL;
    
    
    mpConfirmMsgCtl = NULL;
    mpPauseMsgCtl = NULL;
    mpMissionMsgCtl = NULL;
    mfAnimationCameraAttacked = 0.0f;
    mnAnimationCameraAttackedCnt = 0;
    
    mRequestTry = 0;
    
    mpRightTools = new CLabelCtl(NULL,m_pSGLCore->GetTextureMan());
    mpLeftTools = NULL;
    mpGoldBar = NULL;
    mpParticleMan = new CParticleEmitterMan;
    
    
    mpArrowMotionSprite = NULL;
    pImpactParticle = NULL;
    mfScore = 0.f;
    mnBonus = 0;
    mpItemSlotBar = NULL;
    mpItemSlotSwitchBtn = NULL;
    mpLeftToolsEnergyBar= NULL;
    mpLeftToolsFireBar = NULL;
    mpSelectedWeaponBtn = NULL;
    mpShopBtn = NULL;
    
    mnArrEnSpriteCnt = 0;
    mnArrEnSpriteTable = NULL;
    
    mnMagnet = 0;
    mpBlzSelectedIcon = NULL;
    
    mpClickedSprite = NULL;
    
    mpSelectedObjectDescBar = NULL;
    mpDescCtl = NULL;
    mpDescCtl2 = NULL;
    mpDescCtl3 = NULL;
    mpCarmeraRotationTartketPoint = NULL;
    
    
    InitAllians();
    
    
    //Factory Table Bar
    mpFrameBar = NULL;
    mpFrameBombTable = NULL;
    mpFrameItembTable = NULL;
    mpFrameBombItemTab = NULL;
    mpFrameBarTexureMan = NULL;
    mpBuyDlg = NULL;
    
    mnNeedDefenceTowerUpgrade = -1;
    mnSelectedObjectDescBarVisibleTime= 0;
    
    mfGoldPerScore = GoldPerScoreZoreRank;
    
#ifdef ANDROID
    mbNeedDelFrameBar = false;
#endif
    
    CUserInfo* pUserInfo = GetUserInfo();
    pUserInfo->mnBGetherTime = GetGGTime();
    pUserInfo->mnGetherBuildBlzCount = 0;
    pUserInfo->mnGetherKillCount = 0;
    pUserInfo->mnBGetherScore = GetUserInfo()->GetTotalScore();
    pUserInfo->mnGetherNowMapID = GetUserInfo()->GetLastSelectedMapID();
    pUserInfo->mbSucceedMap = false;
    pUserInfo->mnRunTimeUpgrade = 0;
    pUserInfo->mnALevel = 0;
    pUserInfo->mnDLevel = 0;
    
    mlstRemoveSlotInfo = NULL;
    mlstMainTowerSlotInfo = NULL;
    mpWaitFindMultiplayCursor = NULL;
    mpEnemyTools = NULL;
    mbFastBuild = false;
    mbIsDestroyEMainTower = false;
    mbIsDestoryEnemyActor = false;
}


CHWorld::~CHWorld()
{
    StopBgSound();
    if(mpThread)
    {
        delete mpThread;
        mpThread = NULL;
    }
    
	if(mpThreadQueue)
	{
		int nSize = mpThreadQueue->Size();
		for(int i = nSize - 1; i >= 0; i--)
		{
			RuntimeRenderObjects* Objects = (RuntimeRenderObjects*)(int*)mpThreadQueue->GetAt(i);
			
			delete Objects->plistBomb;
			delete Objects->plistSprite;
            delete Objects->plistParticle;
			delete Objects;
			
			//CListNode<CSprite> *pList = (CListNode<CSprite>*)(int*)mpThreadQueue->GetAt(i);
			//delete pList;
		}
		delete mpThreadQueue;
		mpThreadQueue = NULL;
	}
    
    if(mpArrowMotionSprite)
    {
        delete mpArrowMotionSprite;
        mpArrowMotionSprite = NULL;
    }
    
    if(mBackground)
    {
        delete mBackground;
        mBackground = NULL;
    }
    
    SetRotationCarmeraToPoint(NULL);
    
    //End 2D controls -------------------------
    CFireParticle::FireParticleUninitialize();
    CCompatFireParticle::CompatFireParticleUninitialize();
    CET2AttackParticle::ET2AttackParticleUninitialize();
    CImpactFlyParticle::ImpactFlyParticleUninitialize();
	//-----------------------------------------------------
    
    
    if(m_pSGLCore)
    {
        delete m_pSGLCore;
        m_pSGLCore = NULL;
    }
    
    if(mpRightTools)
    {
        delete mpRightTools;
        mpRightTools = NULL;
    }
    
    if(mpLeftTools)
    {
        delete mpLeftTools;
        mpLeftTools = NULL;
    }
    
    if(mpGoldBar)
    {
        delete mpGoldBar;
        mpGoldBar = NULL;
    }
    
    if(mpRadaBoard) delete mpRadaBoard;
    if(mpStick) delete mpStick;
    
    
    
    if(mpConfirmMsgCtl)
    {
        delete mpConfirmMsgCtl;
        mpConfirmMsgCtl = NULL;
    }
    
    if(mpPauseMsgCtl)
    {
        delete mpPauseMsgCtl;
        mpPauseMsgCtl = NULL;
    }
    
    if(mpShopBtn)
    {
        delete mpShopBtn;
        mpShopBtn = NULL;
    }
    
    
    if(mpMissionMsgCtl)
    {
        delete mpMissionMsgCtl;
        mpMissionMsgCtl = NULL;
    }
    
    if(mpParticleMan)
    {
        delete mpParticleMan;
        mpParticleMan = NULL;
    }
    
    if(pImpactParticle)
    {
        delete pImpactParticle;
        pImpactParticle = NULL;
    }
    
    if(mpItemSlotBar)
    {
        delete mpItemSlotBar;
        mpItemSlotBar= NULL;
    }
    
    if(mlstRemoveSlotInfo)
    {
        USERINFO* pU = mlstRemoveSlotInfo->at(0);
        //        delete pU->pProp; //pU안쪽에서 해제 한다.
        delete pU;
        delete mlstRemoveSlotInfo;
        mlstRemoveSlotInfo = NULL;
    }
    
    if(mlstMainTowerSlotInfo)
    {
        delete mlstMainTowerSlotInfo;
        mlstMainTowerSlotInfo = NULL;
    }
    
    
    if(mnArrEnSpriteTable)
    {
        delete[] mnArrEnSpriteTable;
        mnArrEnSpriteTable = NULL;
    }
    
    
    if(mpFrameBar)
    {
        delete mpFrameBar;
        mpFrameBar = NULL;
    }
    if(mpBuyDlg)
    {
        delete mpBuyDlg;
        mpBuyDlg = NULL;
    }
    if(mpFrameBarTexureMan)
    {
        delete mpFrameBarTexureMan;
        mpFrameBarTexureMan = NULL;
    }
    
    CStrategyAI::Close_Static();
}


int CHWorld::Initialize(void* pResource)
{
    CWorld::Initialize(pResource);
	char sPath[MAXLENGTH];
    char sFullPath[MAXLENGTH];
    
	sprintf(sPath,"%s/",(const char*)pResource);
	CLoader::setSearchPath((const char*)sPath);
	mnStatus = 0x0001;
	Clear();
    
    
    CUserInfo* pUserInfo = 0;
    if(mpScenario)
        pUserInfo = mpScenario->GetUserInfo();
    
    if(pUserInfo)
    {
        PROPERTY_MAP property;
        if(PROPERTY_MAP::GetPropertyMap(pUserInfo->GetLastSelectedMapID(), property))
            sprintf(sFullPath, "%s%s.son",CLoader::getSearchPath(),property.sModelPath);
        else
            sprintf(sFullPath, "%s%s",CLoader::getSearchPath(),"Default.son");
        
        mnMapLevel = property.nMinLevel;
    }
    else
    {
        sprintf(sFullPath, "%s%s",CLoader::getSearchPath(),"Default.son");
        mnMapLevel = 1;
    }
    
    //Progress 1단계.
    CScenario::SendMessage(SGL_MESSAGE_PROGRESS_STEP);
    if(pUserInfo) m_pSGLCore->LoadTerrian(sFullPath);
    
    //내부에서 해준다.
    //CScenario::SendMessage(SGL_PROGRESS_INITIALIZE_1,(long)nGX,(long)nGY);
    
    else
    {
        m_pSGLCore->LoadTerrianOrgEx(sFullPath);
        SGLEvent e;
        e.lParam = m_pSGLCore->GetMapWidth();
        e.lParam2 = m_pSGLCore->GetMapHeight();
        Initialize1(&e);
    }
    
    mfScore = 0.f;
    mnBonus = 0;
    //    mnGGTime = 0;
    
    pUserInfo->GetListBlz()->clear();
    
    return E_SUCCESS;
}

//InThread
int CHWorld::Initialize1(SGLEvent *pEvent)
{
    int nGX = (int)pEvent->lParam;
    int nGY = (int)pEvent->lParam2;
    
    m_pSGLCore->LoadTerrianNextEx(nGX, nGY);
    
    //Progress 8단계.
    CScenario::SendMessage(SGL_MESSAGE_PROGRESS_STEP);
    //Progress를 다음으로 해준다.
    CScenario::SendMessage(SGL_PROGRESS_INITIALIZE_2);
    
    if(mpScenario == 0)
    {
        Initialize2(0);
        Initialize3(0);
    }
    
    return E_SUCCESS;
}

int CHWorld::SetMakePosition(float fRadius,int nIndex,CSprite *pTank,SPoint* ptCenter,SPoint* ptOutPoint)
{
    SPoint ptOut;
    //    float fRadius = mpSprite->GetRadius() * 4.f;
    float f[3];
    float matrix[16];
    
    f[0] = fRadius;
    f[1] = 0;
    f[2] = 0;
    
    if(nIndex > 8)    nIndex = 0;
    
    sglLoadIdentityf(matrix);
    sglRotateRzRyRxMatrixf(matrix,
                           0,
                           40 * nIndex,
                           0);
    
    
    //좌표를 이동한다.
    sglMultMatrixVectorf(f,matrix,f);
    
    memcpy(&ptOut, ptCenter, sizeof(SPoint));
    
    ptOut.x += f[0];
    ptOut.y = 0;
    ptOut.z += f[2];
    
    if(CSglTerrian::IsObstacle(&ptOut))
    {
        return SetMakePosition(fRadius,nIndex + 1,pTank,ptCenter);
    }
    
    GetPositionY(&ptOut);
    
    if(pTank->GetType() == ACTORTYPE_FIGHTER) //땅에 기어다니는 것은 장애물지역인지 아닌지를 재조정해준다.
        ptOut.y += FIGHTER_MIN_HEIGHT;
    
    if(!pTank->SetPosition(&ptOut))
    {
        return SetMakePosition(fRadius,nIndex + 1,pTank,ptCenter);
    }
    if(ptOutPoint) memcpy(ptOutPoint, &ptOut, sizeof(SPoint));
    return nIndex;
}

void CHWorld::MakeActorAtStartPostion()
{
    CUserInfo* pUserInfo = mpScenario->GetUserInfo();
    //## Test
    //10개의 맵의 시작지점을 미리 등록하자.
    const int multiplayStart[] =
    {
        0,0, //0
        0,1, //1 
        0,1, //2
        0,1, //3
        0,1, //4
        0,1, //5
        0,1, //6
        0,1, //7
        0,1, //8
        0,1  //9
    };
    
    int nSize = (int)m_pSGLCore->mlstStartPoint.size();
    int nv = 0;
    if(!pUserInfo->GetMultiplayOn()) //멀티플레이가 아닐때는 난수로 그렇지 않을 때는 거시기..
    {
        if(nSize > 1)
        {
            srand((unsigned int)time(NULL));
            nv = rand();
            nv = nv % nSize;
        }
        else
            nv = 0;
    }
    else
    {
        int nMapIndex = pUserInfo->GetLastSelectedMapIndex();
        if(SGL_TEAM1 == GetMutiplyProtocol()->GetMyDeviceActorTeamID())
            nv = multiplayStart[nMapIndex*2];
        else
            nv = multiplayStart[nMapIndex*2 + 1];
        
    }
    
    
    
    
    STARTPOINT *pPoint = m_pSGLCore->mlstStartPoint[nv];
    
    SVector vtDir;
    vtDir.x = pPoint->fDirX;
    vtDir.y = pPoint->fDirY;
    vtDir.z = pPoint->fDirZ;
    
    SPoint  ptPos;
    ptPos.x = pPoint->fPosX;
    ptPos.y = pPoint->fPosY;
    ptPos.z = pPoint->fPosZ;
    
    
    //주인공------------------------------------------------------------------------------------------------------------------------------
    CSprite*   pSprite = CSGLCore::MakeSprite(GetNewID(),mpScenario->GetMutiplyProtocol()->GetMyDeviceActorTeamID(),pPoint->nModelID,GetTextureMan(),m_pSGLCore->GetAction(),this,NETWORK_MYSIDE);
    pSprite->LoadResoure(GetModelMan());
    pSprite->Initialize(&ptPos, &vtDir);
    m_pSGLCore->AddSprite(pSprite);
    SetActor(pSprite); //주인공이다.
    pSprite->SetCameraType(CAMERT_BACK); //실제로 카메라 타입을 셋한다.
    
    
    
    //폭탄을 변경해준다.
    vector<USERINFO*>*lstBomb = pUserInfo->GetListBomb();
    int nBombSize = (int)lstBomb->size();
    if(nBombSize == 1)
    {
        CK9Sprite *pSP = dynamic_cast<CK9Sprite*>(pSprite);
        if(pSP)
            //pSP->SetCurrentSelMissaileInfo((*lstBomb)[0]);
            ChangeBombDisplay((*lstBomb)[0]);
    }
    else if(nBombSize > 1)
    {
        CK9Sprite *pSP = dynamic_cast<CK9Sprite*>(pSprite);
        if(pSP)
            //pSP->SetCurrentSelMissaileInfo((*lstBomb)[1]);
            ChangeBombDisplay((*lstBomb)[1]);
    }
    
    //## Test
    //
    SPoint ptMain;
    CDTower* pNew = NULL;
    float fRadias = 15.f * 2.f;
    
    pNew = (CDTower*)CSGLCore::MakeSprite(GetNewID(), pSprite->GetTeamID(), ACTORID_BLZ_MAINTOWER,m_pSGLCore->GetTextureMan(),m_pSGLCore->GetAction(), this,NETWORK_MYSIDE);
    pNew->LoadResoure(m_pSGLCore->GetModelMan());
    SetMakePosition(fRadias,1,pNew,&ptPos,&ptMain);
    pNew->Initialize(&ptMain, &vtDir);
    m_pSGLCore->AddSprite(pNew);

    //----------------------------------------------------------------------
    if(pUserInfo->GetCompletedRank() == 1 && pUserInfo->GetLastSelectedMapIndex() == 1) //너무 처음에 못한다. 건물을 많이 지어주자.
    {
        pNew->GetPosition(&ptPos);

        pNew = (CDTower*)CSGLCore::MakeSprite(GetNewID(), pSprite->GetTeamID(), ACTORID_BLZ_DMISSILETOWER,m_pSGLCore->GetTextureMan(),m_pSGLCore->GetAction(), this,NETWORK_MYSIDE);
        pNew->LoadResoure(m_pSGLCore->GetModelMan());
        pNew->Initialize(&ptPos, &vtDir);
        SetMakePosition(fRadias,1,pNew,&ptPos);
        m_pSGLCore->AddSprite(pNew);


        pNew = (CDTower*)CSGLCore::MakeSprite(GetNewID(), pSprite->GetTeamID(), ACTORID_BLZ_DGUNTOWER,m_pSGLCore->GetTextureMan(),m_pSGLCore->GetAction(), this, NETWORK_MYSIDE);
        pNew->LoadResoure(m_pSGLCore->GetModelMan());
        pNew->Initialize(&ptPos, &vtDir);
        SetMakePosition(fRadias,3,pNew,&ptPos);
        m_pSGLCore->AddSprite(pNew);
    }
    //----------------------------------------------------------------------
}

bool MainTowerSort(STARTPOINT* a, STARTPOINT* b)
{
    return a->fDirY < b->fDirY; //내림차수 즉 제일 마지막에 만들어진것이 마지막으로 온다.
}


void CHWorld::MakeBLZMainTower()
{
    SPoint ptMainTowerPos;
    STARTPOINT *pPoint = NULL;
    
    pPoint = m_pSGLCore->mlstAddedSprite[mnCurrentETMainTowerStartPointIndex];
    CSprite* pSprite = CSGLCore::MakeSprite(GetNewID(),pPoint->nTeamID,pPoint->nModelID,GetTextureMan(),m_pSGLCore->GetAction(),this,NETWORK_MYSIDE);
    if(pSprite)
    {
        SVector vtDir;
        vtDir.x = pPoint->fDirX;
        vtDir.y = pPoint->fDirY;
        vtDir.z = pPoint->fDirZ;
        
        SPoint  ptPos;
        ptPos.x = pPoint->fPosX;
        ptPos.y = pPoint->fPosY;
        ptPos.z = pPoint->fPosZ;
        
        
        memcpy(&ptMainTowerPos,&ptPos,sizeof(SPoint));
        
        //GetPositionZ(&ptPos);
        pSprite->LoadResoure(GetModelMan());
        pSprite->Initialize(&ptPos, &vtDir);
        m_pSGLCore->AddSprite(pSprite);
        
        //시작시 적의 위치를 알수있게...
        SetRotationCarmeraToPoint(&ptPos);
        mpRadaBoard->SetDMainTower(pSprite);
        
    }
    
    ptMainTowerPos.x += 20.0;
    ptMainTowerPos.y += 20.0;
    CStrategyAI::Close_Static();
    CStrategyAI::Init_Static(&ptMainTowerPos, &m_pSGLCore->mlstStartPoint);
    
}


bool ArrTankSort( PROPERTY_TANK* a, PROPERTY_TANK* b )
{
    return a->nMaxDefendPower < b->nMaxDefendPower;    //오름차순.
}


void CHWorld::InitEnemySprite()
{
    //-------------------------------------------------------------------
    //    float fTotalDefence = 0.f;
    short nMapLevel = GetMapLevel();
    SPoint ptNow;
    SVector ptDir;
    ptDir.x = 1.f;
    ptDir.y = 0.f;
    ptDir.z = 0.f;
    ptNow.x = 0;
    ptNow.y = 0;
    ptNow.z = 0;
    
    
    CSprite* pTank = NULL;
    //탱크는 맵레벨의 이하로 어레이 아이디로 할당되고, 디펜딩 파워의 크기의 올림차순으로 소팅해서 가지고 있자.
    vector<PROPERTY_TANK*> arrTank;
    PROPERTY_TANK::GetPropertyTankList(arrTank);
    
    //Vector Sorting
    sort(arrTank.begin(), arrTank.end(),ArrTankSort);
    
    
    mnArrEnSpriteCnt = 0;
    int* ptempArrObjectTable = new int[arrTank.size()];
    vector<PROPERTY_TANK*>::iterator b = arrTank.begin();
    vector<PROPERTY_TANK*>::iterator e = arrTank.end();
    for (vector<PROPERTY_TANK*>::iterator it = b; it != e; it++)
    {
        //MapLevel -2 보다는 크게 즉 너무 약한 놈은 내보내지 말자꾸나.
        if((int)(*it)->nMinLevel > (int)(nMapLevel - 2) && (*it)->nMinLevel <= nMapLevel && (*it)->nEnemy == 1) //적군과 맵레벨이하.
        {
            ptempArrObjectTable[mnArrEnSpriteCnt++] = (*it)->nID;
            pTank = CSGLCore::MakeSprite(GetNewID(), 0, (*it)->nID,GetTextureMan(),(IAction*)this, this,NETWORK_MYSIDE);
            if(pTank == NULL)
            {
                HLogD("Can not make tanke ID %d\n",(*it)->nID);
                return;
            }
            pTank->LoadResoure(GetModelMan());
            
            //            fTotalDefence += pTank->GetMaxScore();
            HLogD("MinLevel=%d,DPPow=%f,APPow=%f,Score=%f,ID(%d)=%s\n",
                  (*it)->nMinLevel,
                  pTank->GetMaxDefendPower(),
                  pTank->GetMaxAttackPower(),
                  pTank->GetMaxScore(),
                  (*it)->nID,
                  (*it)->sModelPath);
            
            delete pTank;
        }
    }
    
    
    mnArrEnSpriteTable = NULL;
    if(mnArrEnSpriteCnt > 0)
    {
        mnArrEnSpriteTable = new int[mnArrEnSpriteCnt];
        for(int i = 0; i < mnArrEnSpriteCnt; i++)
        {
            mnArrEnSpriteTable[i] = ptempArrObjectTable[i];
        }
        int nRank = GetUserInfo()->GetRank();
        if(nRank)
        {
            if(GetUserInfo()->GetCntByIDWithItem(ID_TIEM_FASTGOLD) == 0)
                mfGoldPerScore = GoldPerScore;
            else
                mfGoldPerScore = GoldPerScore * 3.0f;
        }
        else
        {
            mfGoldPerScore = GoldPerScoreZoreRank; //0.00025
        }
        
        HLogD("Gold Per Score %f\n",mfGoldPerScore);
    }
    
    if(ptempArrObjectTable)
        delete[] ptempArrObjectTable;
    
    PROPERTY_TANK::ReleaseList((vector<PROPERTYI*>*)&arrTank);
    //-------------------------------------------------------------------
}

int CHWorld::Initialize2(SGLEvent *pEvent)
{
    //    CUserInfo* pUserInfo = 0;
    //    SPoint ptActor,ptEMain;
    SVector ptDir;
    ptDir.x = 0;
    ptDir.y = 1;
    ptDir.z = 0;
    
    
    //    if(mpScenario)
    //        pUserInfo = mpScenario->GetUserInfo();
    
    //스프라이트 정보를 셋하기 전에 해준다.
    CTE1BoosterParticle::Initialize_ToCTE1Booster(this);
    CBombTailParticle::BombTailParticleInitialize(this);
    
	//Far View 원거리 설정은 여기서 한다.
    mCamera.ViewFar = FARVIEW;
    mCamera.ViewFarOrg = mCamera.ViewFar;
    
    int nSize = (int)m_pSGLCore->mlstStartPoint.size();
    if(nSize > 0)
    {
        MakeActorAtStartPostion();
    }
    else
    {
        HLogE("[Error] not fount start point");
    }
    
    //MotionArrow 만들기
    mpArrowMotionSprite = CSGLCore::MakeSprite(GetNewID(), 0, ACTORID_ARROWMOTION, GetTextureMan(),m_pSGLCore->GetAction(),this,NETWORK_NONE);
    if(mpArrowMotionSprite)
    {
        mpArrowMotionSprite->LoadResoure(GetModelMan());
        mpArrowMotionSprite->Initialize(NULL, NULL);
    }
    
    
    InitEnemySprite();
    
    mnCurrentETMainTowerStartPointIndex = 0;
    
    //적메인타워 여러개 형성함.
    nSize = (int)m_pSGLCore->mlstAddedSprite.size();
    if(nSize >= 1)
    {
        if(nSize > 1)
            sort(m_pSGLCore->mlstAddedSprite.begin(), m_pSGLCore->mlstAddedSprite.end(),MainTowerSort);
        
        //## Test
        MakeBLZMainTower();
        
        //SetAllians(SGL_TEAM1,SGL_TEAM4);
        //        SetAllians(SGL_TEAM1,SGL_TEAM2);
        //        SetAllians(SGL_TEAM4,SGL_TEAM2);
        
    }
    else if( nSize == 0)
    {
        HLogE("[Error] not fount ET Main Tower");
        //        return E_SUCCESS;
    }
    
    if(mBackground != NULL)
    {
        delete mBackground;
        mBackground = NULL;
    }
    
    mBackground = new CBackground(ACTORID_BACKGROUND,SGL_TEAMALL,0,m_pSGLCore->GetTextureMan(),m_pSGLCore->GetAction(),this);
    if(mBackground->LoadResoure(m_pSGLCore->GetModelMan()) == E_SUCCESS)
    {
        mBackground->Initialize(NULL, NULL);
    }
    else
    {
        delete mBackground;
        mBackground = NULL;
    }
    
    
    
    int nCnt = 0;
    
    const char* sModeASCII[] = {ANINAME_ANIENERGYUP,ANINAME_BUBBLE};
    nCnt = sizeof(sModeASCII) / sizeof(const char*);
    for (int i = 0; i < nCnt; i++)
    {
        CMyModel* pModel;
        //        m_pSGLCore->GetMS3DModelASCIIMan()->Load(sModeASCII[i]);
        GetModelMan()->getModelf(sModeASCII[i], &pModel);
        delete pModel;
    }
    
    //폭탄은 쓰레드 안쪽에서 생성 되기 때문에 텍스쳐 바운딩이 안된다.
    const char* sTextureName[] = {BOMBET_IMG,BOMBETTAIL_IMG,BOMBTAIL_IMG,BOMBHE610A,NormalBomb_IMG,
        THUNDERBOLTSTART_IMB,RASERBUNSTART_IMG,MachineCun_IMG};
    nCnt = sizeof(sTextureName) / sizeof(const char*);
    for (int i = 0; i < nCnt; i++)
    {
        m_pSGLCore->GetTextureMan()->GetTextureID(sTextureName[i]);
    }
    
    //UV텐스쳐 (레이져 같은 것은 UV로 읽어야 한다.)
    const char* sTextureNameUV[] = {RASER1_IMG,RASER2_IMG,RASER3_IMG,THUNDERBOLT_IMG};
    nCnt = sizeof(sTextureNameUV) / sizeof(const char*);
    for (int i = 0; i < nCnt; i++)
    {
        m_pSGLCore->GetTextureMan()->GetTextureID(sTextureNameUV[i],0,1);
    }
    
    
    //부스터가 쓰레드 안쪽에서 할당된다.
    //    m_pSGLCore->GetTextureMan()->GetTextureID((const char*)"HE610ATail.tga");
#ifdef ANDROID
    //RunTimeSlot Init
    vector<string> slotIcon;
    RUNTIME_UPGRADE::InitTextureRunUpgradeItemList(&slotIcon);
    nCnt = slotIcon.size();
    for (int i = 0; i < nCnt; i++)
    {
        m_pSGLCore->GetTextureMan()->GetTextureID(slotIcon[i].c_str());
    }
#endif //ANDROID
    
    //Progress 9단계.
    CScenario::SendMessage(SGL_MESSAGE_PROGRESS_STEP);
    CScenario::SendMessage(SGL_PROGRESS_INITIALIZE_3);
	return E_SUCCESS;
}

void CHWorld::HideEnemyTools()
{
    if(mpEnemyTools)
    {
        mpEnemyTools->SetAni(CONTROLANI_TOUT,0.1);
    }
}

void CHWorld::ShowEnemyTools(const char* sEndisplayName,int nEnRank,bool bAliens)
{
    vector<string> lstImage;
    
    //-----------------------------------------------------------------------------
    CControl* pDisplayName = mpEnemyTools->FindControl(CTL_ENDISPLAY_NAME);    
    const char* sDisplayName = SGLTextUnit::GetTextAddGLData(sEndisplayName,"EDiplay_Name");
    lstImage.clear();
    //lstImage.push_back("res_slot.png");
    lstImage.push_back("none");
    lstImage.push_back(sDisplayName);
    pDisplayName->SetImageData(lstImage);
    //-----------------------------------------------------------------------------
    
    
    //적계급아이콘
    CControl* pEnemyRankIcon = mpEnemyTools->FindControl(CTL_ENRANK_ICON);

    lstImage.clear();
    lstImage.push_back(::GetRankPath(nEnRank));
    lstImage.push_back("none");
    //계급이 잘 안보이는 현상을 수정함( 대장계급이 깨져보이는 현상)
    if(nEnRank < 9)
        pEnemyRankIcon->SetBackCoordTex(0.2f, 0.2f, 0.8f, 0.8f);
    else if(nEnRank == 9 || nEnRank == 10 || nEnRank == 11 || nEnRank == 13 || nEnRank == 16)
        pEnemyRankIcon->SetBackCoordTex(0.2f, 0.0f, 0.8f, 0.9f);
    else if( nEnRank == 12)
        pEnemyRankIcon->SetBackCoordTex(0.1f, 0.0f, 0.9f, 0.9f);
    else //14,15,17,18,19
        pEnemyRankIcon->SetBackCoordTex(0.0f, 0.0f, 1.f, 0.9f);
    
    pEnemyRankIcon->SetImageData(lstImage);
    
    
    CControl* pBtnAlias = mpEnemyTools->FindControl(BTN_MULTIPLAY_ALIAS);
    //동맹 : 0.f,0.f,0.921875,0.4609375   비동맹:0.f,0.4609375f,0.921875f,0.921875f
    if(bAliens)
    {
        pBtnAlias->SetBackCoordTex(0.f,0.f,0.921875f,0.4609375f);
    }
    else
    {
        pBtnAlias->SetBackCoordTex(0.f,0.4609375f,0.921875f,0.921875f);
    }
    
    mpEnemyTools->SetAni(CONTROLANI_TIN,0.1);
    if(mpWaitFindMultiplayCursor) mpWaitFindMultiplayCursor->SetHide(true);
}

void CHWorld::InitializeByResized()
{
    if(mnStatus & 0x0002) //초기화가 되어 있다면 다시 초기화를 해준다.
    {
        initProjection(); //xmax_3d,ymax_3d 재계한되어야 한다.
        mnStatus &= ~0x0002;
        if(mpRadaBoard)
        {
            delete mpRadaBoard;
            mpRadaBoard = NULL;
        }
        
        if(mpStick)
        {
            delete mpStick;
            mpStick = NULL;
        }
        
        if(mpGoldBar)
        {
            delete mpGoldBar;
            mpGoldBar = NULL;
        }
        
        if(mpLeftTools)
        {
            delete mpLeftTools;
            mpLeftTools = NULL;
        }
        
        if(mpRightTools)
        {
            delete mpRightTools;
            mpRightTools = NULL;
        }
        
        if(mpItemSlotBar)
        {
            delete mpItemSlotBar;
            mpItemSlotBar = NULL;
        }
        mpRadaBoard = new C2dRadaBoard(this);
        mpStick = new C2dStick(this);
        mpRightTools = new CLabelCtl(NULL,m_pSGLCore->GetTextureMan());
        
        
        Init2DControl();
        mnStatus |= 0x0002;
    }
}

void CHWorld::Init2DControl()
{
    vector<float> layout;
    float fWhite[] = {1.0f,1.0f,1.0f,1.0f};
    float fRed[] = {1.0f,1.0f,0.0f,1.0f};
    
    InitItemSlot();
    
    mpRadaBoard->Initialize(NULL,NULL);
    int nCnt = GetUserInfo()->GetCntByIDWithItem(ID_ITEM_ATTACKBYRADA);
    if(nCnt == 0)
        mpRadaBoard->SetHide(true); //레이더 아이템이 없으면 레이다를 보이지 않게 한다.
    
    //마그네틱 사용...
    mnMagnet = GetUserInfo()->GetCntByIDWithItem(ID_ITEM_UPGRADE_MAGNETIC);
    
    mpStick->Initialize(NULL, NULL);
    
    
    vector<string>  lstImage;
    
    //Gold Bar....
    lstImage.clear();
    lstImage.push_back("res_slot.png");
    lstImage.push_back("none");
    
    mpGoldBar = new CLabelCtl(NULL,m_pSGLCore->GetTextureMan());
    layout.clear();
    PROPERTYI::GetPropertyFloatList("G_GoldBar", layout);
    mpGoldBar->Initialize(0, layout[0], layout[1], layout[2], layout[3], lstImage, 0.0029296875, 0.939453125, 0.5234375, 0.998046875);
    
    
    //Gold
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    
    CNumberCtl* pGold = new CNumberCtl(mpGoldBar,m_pSGLCore->GetTextureMan(),true);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("G_Gold", layout);
    pGold->Initialize(0, layout[0], layout[1], layout[2], layout[3], layout[4],layout[5],layout[6],lstImage,false);
    mpGoldBar->AddControl(pGold);
    pGold->SetAlign(CNumberCtl::ALIGN_CENTER);
    pGold->SetTextColor(fWhite);
    pGold->SetNumber(GetUserInfo()->GetGold());
    
    
    lstImage.clear();
    lstImage.push_back("res_slot.png");
    lstImage.push_back("none");
    
    mpLeftTools = new CLabelCtl(NULL,m_pSGLCore->GetTextureMan());
    layout.clear();
    PROPERTYI::GetPropertyFloatList("W_LeftTools", layout);
    mpLeftTools->Initialize(0, layout[0], layout[1], layout[2], layout[3], lstImage, 0.025390625, 0.474609375, 0.529296875, 0.58984375);
    
    int nRank = GetUserInfo()->GetRank();
    //Rank
    lstImage.clear();
    lstImage.push_back(mpScenario->GetRankPath());
    lstImage.push_back("none");
    CControl *pRankIcon = new CLabelCtl(mpLeftTools,m_pSGLCore->GetTextureMan());
    layout.clear();
    PROPERTYI::GetPropertyFloatList("W_RankIcon", layout);
    
    //계급이 잘 안보이는 현상을 수정함( 대장계급이 깨져보이는 현상)
    if(nRank < 9)
        pRankIcon->Initialize(0, layout[0], layout[1], layout[2], layout[3], lstImage, 0.2f, 0.2f, 0.8f, 0.8f);
    else if(nRank == 9 || nRank == 10 || nRank == 11 || nRank == 13 || nRank == 16)
        pRankIcon->Initialize(0, layout[0], layout[1], layout[2], layout[3], lstImage, 0.2f, 0.0f, 0.8f, 0.9f);
    else if( nRank == 12)
        pRankIcon->Initialize(0, layout[0], layout[1], layout[2], layout[3], lstImage, 0.1f, 0.0f, 0.9f, 0.9f);
    else //14,15,17,18,19
        pRankIcon->Initialize(0, layout[0], layout[1], layout[2], layout[3], lstImage, 0.0f, 0.0f, 1.f, 0.9f);
    
    mpLeftTools->AddControl(pRankIcon);
    
    
    //Scroe Label
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("PSCORE");
    CControl *pScoreLabel = new CLabelCtl(mpLeftTools,m_pSGLCore->GetTextureMan());
    layout.clear();
    PROPERTYI::GetPropertyFloatList("W_ScoreLabel", layout);
    pScoreLabel->Initialize(0, layout[0], layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
    pScoreLabel->SetTextColor(fWhite);
    mpLeftTools->AddControl(pScoreLabel);
    
    //Scroe
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    mpScoreNumCtl = new CNumberCtl(mpLeftTools,m_pSGLCore->GetTextureMan(),true);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("W_ScoreNumCtl", layout);
    mpScoreNumCtl->Initialize(BTN_PLAY_TOP_GOLD_NUM, layout[0], layout[1], layout[2], layout[3],layout[4], layout[5], layout[6], lstImage);
    mpScoreNumCtl->SetTextColor(fWhite);
    mpScoreNumCtl->SetAlign(CNumberCtl::ALIGN_CENTER);
    mpScoreNumCtl->SetNumber(0);
    mpLeftTools->AddControl(mpScoreNumCtl);
    
    
    //Alive Number
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    mpAliveNumCtl = new CNumberCtl(mpLeftTools,m_pSGLCore->GetTextureMan(),true);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("W_RealiveNumCtl", layout);
    
    
    mpAliveNumCtl->Initialize(0, layout[0], layout[1], layout[2], layout[3],layout[4], layout[5], layout[6], lstImage);
    mpAliveNumCtl->SetTextColor(fRed);
    mpAliveNumCtl->SetAlign(CNumberCtl::ALIGN_CENTER);
    mpAliveNumCtl->SetNumber(0);
    mpLeftTools->AddControl(mpAliveNumCtl);
    
    
    int nLiveCnt = GetActor()->GetAliveCount();
    if(nLiveCnt == 0)
        mpAliveNumCtl->SetHide(true);
    else
        mpAliveNumCtl->SetNumber(nLiveCnt);
    
    //Energy Bar
    lstImage.clear();
    lstImage.push_back("res_slot.png");
    lstImage.push_back("none");
    mpLeftToolsEnergyBar = new CImgProgressCtl(mpLeftTools,m_pSGLCore->GetTextureMan());
    layout.clear();
    PROPERTYI::GetPropertyFloatList("W_LeftToolsEnergyBar", layout);
    mpLeftToolsEnergyBar->Initialize(0, layout[0], layout[1], layout[2], layout[3], lstImage,
                                     0.029296875, 0.603515625, 0.369140625, 0.6484375,
                                     0.029296875,0.658203125,0.369140625,0.703125
                                     );
    mpLeftToolsEnergyBar->SetPosition(100);
    mpLeftTools->AddControl(mpLeftToolsEnergyBar);
    
    
    //Fire Bar
    lstImage.clear();
    lstImage.push_back("res_slot.png");
    lstImage.push_back("none");
    layout.clear();
    PROPERTYI::GetPropertyFloatList("W_LeftToolsFireBar", layout);
    mpLeftToolsFireBar = new CImgProgressCtl(mpLeftTools,m_pSGLCore->GetTextureMan());
    mpLeftToolsFireBar->Initialize(0, layout[0], layout[1], layout[2], layout[3], lstImage,
                                   0.029296875, 0.88671875, 0.369140625, 0.931640625,
                                   0.029296875,0.658203125,0.369140625,0.703125
                                   );
    mpLeftToolsFireBar->SetPosition(100);
    mpLeftTools->AddControl(mpLeftToolsFireBar);
    
    
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    layout.clear();
    PROPERTYI::GetPropertyFloatList("W_RightTools", layout);
    mpRightTools->Initialize(2030, gDisplayWidth - layout[0], layout[1], layout[2], layout[3],lstImage,0.f,0.f,1.f,1.f);
    
    
    //플레이할때 매칭되지 않은 상태에서 플레이를 한다면 검색을 해준다.
    if(GetUserInfo()->GetMultiplayOn())
    {
        //적정보툴즈
        lstImage.clear();
        lstImage.push_back("none");
        lstImage.push_back("none");
        mpEnemyTools = new CLabelCtl(mpRightTools,m_pSGLCore->GetTextureMan());
        layout.clear();
        PROPERTYI::GetPropertyFloatList("W_EnemyTools", layout);
        mpEnemyTools->Initialize(BTN_PLAY_PAUSE, gDisplayWidth - layout[0], layout[1], layout[2], layout[3], lstImage,0.f,0.f,0.f,0.f);
        
        layout.clear();
        PROPERTYI::GetPropertyFloatList("W_EnemyRank", layout);
        //적계급아이콘
        CControl* pEnemyRankIcon = new CLabelCtl(mpEnemyTools,m_pSGLCore->GetTextureMan());
        lstImage.clear();
        lstImage.push_back(mpScenario->GetRankPath());
        lstImage.push_back("none");
        //계급이 잘 안보이는 현상을 수정함( 대장계급이 깨져보이는 현상)
        if(nRank < 9)
            pEnemyRankIcon->Initialize(CTL_ENRANK_ICON, gDisplayWidth - layout[0] + 2, layout[1] + 2, layout[2] - 4, layout[3]- 4, lstImage, 0.2f, 0.2f, 0.8f, 0.8f);
        else if(nRank == 9 || nRank == 10 || nRank == 11 || nRank == 13 || nRank == 16)
            pEnemyRankIcon->Initialize(CTL_ENRANK_ICON, gDisplayWidth - layout[0] + 2, layout[1] + 2, layout[2]- 4, layout[3]- 4, lstImage, 0.2f, 0.0f, 0.8f, 0.9f);
        else if( nRank == 12)
            pEnemyRankIcon->Initialize(CTL_ENRANK_ICON, gDisplayWidth - layout[0] + 2, layout[1] + 2, layout[2]- 4, layout[3]- 4, lstImage, 0.1f, 0.0f, 0.9f, 0.9f);
        else //14,15,17,18,19
            pEnemyRankIcon->Initialize(CTL_ENRANK_ICON, gDisplayWidth - layout[0] + 2, layout[1] + 2, layout[2]- 4, layout[3]- 4, lstImage, 0.0f, 0.0f, 1.f, 0.9f);
        mpEnemyTools->AddControl(pEnemyRankIcon);
        
        
        //이름
        lstImage.clear();
        lstImage.push_back("none");
        lstImage.push_back("none");
        CControl* pEnemyName = new CLabelCtl(mpEnemyTools,m_pSGLCore->GetTextureMan());
        layout.clear();
        PROPERTYI::GetPropertyFloatList("W_EnemyName", layout); //49,482,181,24
        pEnemyName->Initialize(CTL_ENDISPLAY_NAME, gDisplayWidth - layout[0], layout[1], layout[2], layout[3], lstImage,0.f,0.f,0.f,0.f);
        pEnemyName->SetEnable(false);
        mpEnemyTools->AddControl(pEnemyName);
        
        //동맹
        lstImage.clear();
        lstImage.push_back("alians.png");
        lstImage.push_back("none");
        CControl* pAlienBtn = new CButtonCtl(mpEnemyTools,m_pSGLCore->GetTextureMan());
        layout.clear();
        PROPERTYI::GetPropertyFloatList("W_PauseCtl", layout);
        //동맹 : 0.f,0.f,0.921875,0.4609375   비동맹:0.f,0.4609375f,0.921875f,0.921875f
        pAlienBtn->Initialize(BTN_MULTIPLAY_ALIAS, gDisplayWidth - layout[0], layout[1], layout[2], layout[3], lstImage,0.f,0.4609375f,0.921875f,0.921875f);
        mpEnemyTools->AddControl(pAlienBtn);
        mpRightTools->AddControl(mpEnemyTools);
        
        if(GetMutiplyProtocol()->IsStop())
        {
            layout.clear();
            PROPERTYI::GetPropertyFloatList("W_PauseCtl", layout);
            mpWaitFindMultiplayCursor = new CIndicatorCtl(mpRightTools,m_pSGLCore->GetTextureMan());
            mpWaitFindMultiplayCursor->Initialize(0, gDisplayWidth - layout[0], layout[1], layout[2], layout[3],true);
            mpWaitFindMultiplayCursor->Start();
            mpRightTools->AddControl(mpWaitFindMultiplayCursor);
            mpEnemyTools->SetHide(true);
        }
        else
        {
            if(strlen(GetMutiplyProtocol()->GetAliasEnemy()) > 0)
            {
                ShowEnemyTools(GetMutiplyProtocol()->GetAliasEnemy(), GetMutiplyProtocol()->GetRankEnemy(), false);
            }
        }
    }
    else
    {
        //PAUSE
        lstImage.clear();
        lstImage.push_back("res_slot.png");
        lstImage.push_back("none");
        CControl* pPauseCtl = new CButtonCtl(mpRightTools,m_pSGLCore->GetTextureMan());
        layout.clear();
        PROPERTYI::GetPropertyFloatList("W_PauseCtl", layout);
        pPauseCtl->Initialize(BTN_PLAY_PAUSE, gDisplayWidth - layout[0], layout[1], layout[2], layout[3], lstImage,0.033203125,0.751953125,0.1484375,0.8671875);
        mpRightTools->AddControl(pPauseCtl);
    }
    
    
    //HOME
    lstImage.clear();
    lstImage.push_back("res_slot.png");
    lstImage.push_back("none");
    CControl* pHomeCtl = new CButtonCtl(mpRightTools,m_pSGLCore->GetTextureMan());
    layout.clear();
    PROPERTYI::GetPropertyFloatList("W_HomeCtl", layout);
    pHomeCtl->Initialize(BTN_PLAY_HOME, gDisplayWidth - layout[0], layout[1], layout[2], layout[3], lstImage,0.166015625,0.751953125,0.28125,0.8671875);
    mpRightTools->AddControl(pHomeCtl);
    
    
    //상점
    lstImage.clear();
    lstImage.push_back("res_slot.png");
    lstImage.push_back("none");
    mpShopBtn = new CButtonCtl(NULL,m_pSGLCore->GetTextureMan());
    layout.clear();
    
    
    PROPERTYI::GetPropertyFloatList("W_ShopBtn", layout);
    
    //    float fX = gDisplayWidth - (layout[0] + layout[2]);
    //    float fY = gDisplayHeight - (layout[1] + layout[3]);
    //    HLogD("%f,%f\n",fX,fY);
    
    
    mpShopBtn->Initialize(BTN_PLAY_SHOP, gDisplayWidth - (layout[0] + layout[2]), gDisplayHeight - (layout[1] + layout[3]), layout[2], layout[3], lstImage,0.55859375,0.900390625,0.78515625,0.99609375);
    
    
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("World_Shop");
    CControl* pShopText = new CLabelCtl(mpShopBtn,m_pSGLCore->GetTextureMan());
    layout.clear();
    
    
    
    PROPERTYI::GetPropertyFloatList("W_ShopBtnTxt", layout);
    
    
    
    InitSelectedObjectDescBar();
    
#ifdef ANDROID
    //간혹가다 이상하게 그려지는 경우가 존재한다. 그래서 미리 텍스쳐를 그려준다.
    float fOU,fOV;
    char sNumber[][5] = {"0","1","2","3","4","5","6","7","8","9","b0","b1","b2","b3","b4","b5","b6","b7","b8","b9"};
    for (int i = 0; i < 20; i++) {
        m_pSGLCore->GetTextureMan()->MakeText(sNumber[i], fOU, fOV);
    }
#endif //ANDROID
    
    
    
    
    pShopText->Initialize(0, gDisplayWidth - (layout[0] + layout[2]), gDisplayHeight - (layout[1] + layout[3]), layout[2], layout[3], lstImage,0.f,0.f,0.f,0.f);
    pShopText->SetEnable(false);
    mpShopBtn->AddControl(pShopText);
    pShopText->SetTextColor(fWhite);
    
    ResetDisplayItemSlot();
    
    //End 2D controls -------------------------
    //pBomb->SetCompactBomb(); 쓰레드 안에서 SetCompactBmp를 불르면 내부적으로 FireFrame.txt를 불러오는데 거기에 Fire.tga가 존재한다.
    m_pSGLCore->GetTextureMan()->GetTextureID("Fire.tga");
    CFireParticle::FireParticleInitialize("Fire.tga");
    CCompatFireParticle::CompatFireParticleInitialize("Fire.tga");
    CET2AttackParticle::ET2AttackParticleInitialize("sawtooth.tga");
    CImpactFlyParticle::ImpactFlyParticleInitialize(SGL_2D_FILENAME_TARGET);
    //-----------------------------------------------------
    
    ResetFrustum(true); //모든것이 초기화 되었기때문에 프리즘을 다시 설정한다.
    
    //디자인 정보를 지운다.
    CSGLCore* pSglCore = GetSGLCore();
    pSglCore->ClearDesignMakeInfo();
    
    CUserInfo *pUserInfo = GetUserInfo();
    
    //------------------------------------------------------
    //플레이 시작한다.
    CMutiplyProtocol* pro = mpScenario->GetMutiplyProtocol();
    if(!pro->IsStop())
    {
        pro->SetMultiplayState(SYNC_PLAYING);
        pro->SendSyncStartPlay(); //플레이를 시작한다고 메세지를 보낸다.
    }
    else if(GetUserInfo()->GetMultiplayOn() && GetUserInfo()->GetGameCenterAuthStatus() == 2)
    {
        int nFilter = 0;
        
        int nTeamID = pro->GetMyDeviceActorTeamID();
        int nMapIndex = GetUserInfo()->GetLastSelectedMapIndex();
        
        if(nMapIndex == 1 && nTeamID == SGL_TEAM1)            nFilter = MUL_FILTER_MAP11;
        else if(nMapIndex == 1)                               nFilter = MUL_FILTER_MAP12;
        if(nMapIndex == 2 && nTeamID == SGL_TEAM1)            nFilter = MUL_FILTER_MAP21;
        else if(nMapIndex == 2)                               nFilter = MUL_FILTER_MAP22;
        if(nMapIndex == 3 && nTeamID == SGL_TEAM1)            nFilter = MUL_FILTER_MAP31;
        else if(nMapIndex == 3)                               nFilter = MUL_FILTER_MAP32;
        if(nMapIndex == 4 && nTeamID == SGL_TEAM1)            nFilter = MUL_FILTER_MAP41;
        else if(nMapIndex == 4)                               nFilter = MUL_FILTER_MAP42;
        if(nMapIndex == 5 && nTeamID == SGL_TEAM1)            nFilter = MUL_FILTER_MAP51;
        else if(nMapIndex == 5)                               nFilter = MUL_FILTER_MAP52;
        if(nMapIndex == 6 && nTeamID == SGL_TEAM1)            nFilter = MUL_FILTER_MAP61;
        else if(nMapIndex == 6)                               nFilter = MUL_FILTER_MAP62;
        if(nMapIndex == 7 && nTeamID == SGL_TEAM1)            nFilter = MUL_FILTER_MAP71;
        else if(nMapIndex == 7)                               nFilter = MUL_FILTER_MAP72;
        if(nMapIndex == 8 && nTeamID == SGL_TEAM1)            nFilter = MUL_FILTER_MAP81;
        else if(nMapIndex == 8)                               nFilter = MUL_FILTER_MAP82;
        if(nMapIndex == 9 && nTeamID == SGL_TEAM1)            nFilter = MUL_FILTER_MAP91;
        else if(nMapIndex == 9)                               nFilter = MUL_FILTER_MAP92;
        
        if(nMapIndex > 0)
        {
            if(gFindPlyer)
                gFindPlyer(20,nFilter,pro->GetFindMatchingGroup());
        }
    }
    
    if(pUserInfo->GetCntByIDWithItem(ID_TIEM_FASTBUILD) != 0)
        mbFastBuild = true;
    
    
    if(mpWaitFindMultiplayCursor && (pUserInfo->GetLastSelectedMapIndex() == 0 || GetUserInfo()->GetGameCenterAuthStatus() != 2))
        mpWaitFindMultiplayCursor->SetHide(true);

}

int CHWorld::Initialize3(SGLEvent *pEvent)
{
   
    
    
	int nThreadResult = 0;
    
    //    initProjection();
    
    //Start 2D controls -------------------------
    Init2DControl();
    //------------------------------------------------------
    
	mnStatus |= 0x0002;
    
    nThreadResult = mpThread->CreateThread((void*)CoreThread, this);
    if(nThreadResult != 0)
    {
        HLogE("[Error] Create not Thread\n");
        return nThreadResult;
    }
    
    
    CParticleEmitter::MakeRandomTable();
	
    //Progress 10단계.
    CScenario::SendMessage(SGL_MESSAGE_PROGRESS_STEP);
    CScenario::SendMessage(SGL_PROGRESS_END);
    
    //배경음악을 시작한다.
    PlayBgSound("Super_Team");
    return E_SUCCESS;
}

void CHWorld::SetItemDescSlot(int nID)
{
    int grade = 0;
    char sTempDesc2[256];
    const char* sFormat = NULL;
    CUserInfo *pUserInfo = GetUserInfo();
    PROPERTY_ITEM prop;
    PROPERTY_ITEM::GetPropertyItem(nID, prop);
    string sDesc1 = SGLTextUnit::GetOnlyText(prop.sDesc);
    string sDesc2 = prop.sDesc2;
    
    if(sDesc1.length() > 0 && sDesc2.length() > 0)
    {
        if(nID == ID_ITEM_UPGRADE_ATTACK)
        {
            grade = pUserInfo->GetAttackUpgrade() * 5.f;
            sFormat = SGLTextUnit::GetOnlyText(sDesc2.c_str());
            sprintf(sTempDesc2, sFormat,grade);
        }
        else if(nID == ID_ITEM_UPGRADE_DEPEND)
        {
            grade = pUserInfo->GetDependUpgrade() * 5.f;
            sFormat = SGLTextUnit::GetOnlyText(sDesc2.c_str());
            sprintf(sTempDesc2, sFormat,grade);
        }
        else if(nID == ID_ITEM_UPGRADE_MISSILE_TOWER || nID == ID_ITEM_UPGRADE_DEFENCE_TOWER)
        {
            grade = (pUserInfo->GetCntByIDWithItem(nID) + 1) * 5.f;
            sFormat = SGLTextUnit::GetOnlyText(sDesc2.c_str());
            sprintf(sTempDesc2, sFormat,grade);
        }
        else
            strcpy(sTempDesc2, SGLTextUnit::GetOnlyText(sDesc2.c_str()));
        
        sDesc1.append(" ");
        sDesc1.append(sTempDesc2);
    }
    OnSelectedObjectShowDescBar(sDesc1.c_str());
}



void CHWorld::SetBombDescSlot(int nID)
{
    PROPERTY_BOMB prop;
    PROPERTY_BOMB::GetPropertyBomb(nID, prop);
    OnSelectedObjectShowDescBar(prop.sDesc);
}


void CHWorld::SetSpriteDescSlot(int nID)
{
    PROPERTY_TANK prop;
    PROPERTY_TANK::GetPropertyTank(nID, prop);
    OnSelectedObjectShowDescBar(prop.sDesc);
}

void CHWorld::SetUpgradeDescSlot(int nID)
{
    RUNTIME_UPGRADE prop;
    RUNTIME_UPGRADE::GetPropertyRunUpgradeItem(nID, prop);
    OnSelectedObjectShowDescBar(prop.sDesc);
}

void CHWorld::OnSelectedObjectShowDescBar(const char* sInDesc,bool bSpriteGroup)
{
    if(mpSelectedObjectDescBar == NULL) return ;
    
    if(bSpriteGroup == false)
    {
        CControl* pBar = mpSelectedObjectDescBar->GetChild(0);
        pBar->SetHide(true);
        
        mnSelectedObjectDescBarVisibleTime = GetGGTime() + 3000;
        mpSelectedObjectDescBar->SetHide(false);
    }
    
    vector<string>  lstImage;
    
    //설명
    string sDesc;
    string sDesc2;
    string sDesc3;
    char* sFnd2 = NULL;
    const char* sDescription = GetText(sInDesc);
    char* sFnd = (char*)strstr(sDescription, "#");
    if(sFnd)
    {
        *sFnd = 0;
    }
    
    sFnd = (char*)strstr(sDescription, "^");
    if(sFnd)
    {
        *sFnd = 0;
        sDesc2 = sFnd + 1;
    }
    
    sDesc = sDescription;
    
    string sFormat;
    sFormat = GetText("M_Tank_Desc_Format");
    sFnd = (char*)strstr(sFormat.c_str(), "#");
    sFormat = sFnd + 1;
    
    sDesc.append("#");
    sDesc.append(sFormat);// = sFnd + 1;
    
    
    //설명...그룹
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back(sDesc.c_str());
    mpDescCtl->SetImageData(lstImage);
    
    
    if(sDesc2.length() > 0)
    {
        if(mpDescCtl3)
        {
            sDescription = sDesc2.c_str();
            sFnd2 = (char*)strstr(sDescription, "^");
            if(sFnd2)
            {
                *sFnd2 = 0;
                sDesc3 = sFnd2 + 1;
            }
            
            if(sDesc3.length() > 0)
            {
                sDesc = sDesc3;
                sDesc.append("#");
                sDesc.append(sFormat);// = sFnd + 1;
                
                //설명...그룹
                lstImage.clear();
                lstImage.push_back("none");
                lstImage.push_back(sDesc.c_str());
                mpDescCtl3->SetImageData(lstImage);
                mpDescCtl3->SetHide(false);
            }
            else
            {
                mpDescCtl3->SetHide(true);
            }
        }
        else //아이패드는 ㅌㅌㅌ^             ㅌㅌㅌㅌ 의 글자를 ㅌㅌㅌㅌㅌㅌ로 변경해야 한다.
        {
            sDescription = sDesc2.c_str();
            sFnd2 = (char*)strstr(sDescription, "^");
            if(sFnd2)
            {
#ifdef DEBUG
                if(strlen(sDesc2.c_str()) < (26+27))
                {
                    HLogE("1 %s",sDesc2.c_str());
                }
#endif
                
                
                *sFnd2 = 0;
                sDesc2 = sDescription + 26;
#ifdef DEBUG
                if(strlen(sDesc2.c_str()) < (27))
                {
                    HLogE("2 %s",sDesc2.c_str());
                }
#endif
                sDesc2.append(sFnd2 + 27);
            }
            
        }
        
        sDesc = sDesc2.c_str();
        sDesc.append("#");
        sDesc.append(sFormat);// = sFnd + 1;
        
        //설명...그룹
        lstImage.clear();
        lstImage.push_back("none");
        lstImage.push_back(sDesc.c_str());
        mpDescCtl2->SetImageData(lstImage);
        mpDescCtl2->SetHide(false);
    }
    else
    {
        mpDescCtl2->SetHide(true);
        if(mpDescCtl3) mpDescCtl3->SetHide(true);
    }
}

void CHWorld::OnSelectedObjectShowDescBar(CSprite* pSprite,const char* sDesc)
{
    if(mpSelectedObjectDescBar == NULL) return ;
    
    float fDLevel,fALevel,fDPower,fAPower;
    pSprite->GetInfomation(&fDLevel,&fALevel,&fDPower,&fAPower);
    
    vector<string>  lstImage;
    CControl* pBar = mpSelectedObjectDescBar->GetChild(0);
    
    pBar->SetHide(false);
    
    PROPERTY_TANK property;
    
    if(!PROPERTY_TANK::GetPropertyTank(pSprite->GetModelID(),property))
    {
        HLogE("Can not find tank id %d\n",pSprite->GetModelID());
        return ;
    }
    string sModelImgPath = property.sModelPath;
    if(property.nType == 1 || property.nType == 2)          sModelImgPath.append("Icon.png");
    else                                                    sModelImgPath.append(".tga");
    CControl* pIconCtl = pBar->FindControl(CTL_TANK_DESC_ICON);
    if(pIconCtl)
        pIconCtl->SetContentsData(sModelImgPath.c_str());
    
    CImgProgressCtl* pAttackCtl = (CImgProgressCtl*)pBar->FindControl(CTL_TANK_DESC_ATTACK);
    pAttackCtl->SetPosition(fALevel);
    
    
    CImgProgressCtl* pDefendCtl = (CImgProgressCtl*)pBar->FindControl(CTL_TANK_DESC_DEFEND);
    pDefendCtl->SetPosition(fDLevel);
    
    
    int nLevel = (fALevel + fDLevel) / 2.f + 0.5f;
    CNumberCtl* pLevelCtl = (CNumberCtl*)pBar->FindControl(CTL_TANK_DESC_LEVEL);
    pLevelCtl->SetNumber(nLevel);
    
    CControl* pTankName = pBar->FindControl(CTL_TANK_DESC_TANKNAME);
    
    
    string sN;
    const char* sFormat = GetText("T_A513");
    char* sFnd = (char*)strstr(sFormat, "#");
    sN = SGLTextUnit::GetOnlyText(property.sName);
    sN.append(sFnd);
    
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back(sN);
    pTankName->SetImageData(lstImage);
    
    if(sDesc == NULL)
        OnSelectedObjectShowDescBar((const char*)property.sDesc,true);
    else
        OnSelectedObjectShowDescBar(sDesc,true);
    
    
    
    mnSelectedObjectDescBarVisibleTime = GetGGTime() + 3000;
    mpSelectedObjectDescBar->SetHide(false);
}

void CHWorld::InitSelectedObjectDescBar()
{
    vector<string>  lstImage;
    vector<float> layout;
    float fWhite[] = { 1.f,1.f,1.f,1.f};
    
    
    
    
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    mpSelectedObjectDescBar =  new CLabelCtl(mpLeftTools,m_pSGLCore->GetTextureMan());
    layout.clear();
    PROPERTYI::GetPropertyFloatList("DescST_TankSelectedDescBarW", layout);
    mpSelectedObjectDescBar->Initialize(0, layout[0], gnDisplayType == DISPLAY_IPHONE ? layout[1] : gDisplayHeight - layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
    
    
    //객체관련 레벨 설명 그룹
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    CControl* pSpriteGroup = new CLabelCtl(mpSelectedObjectDescBar,m_pSGLCore->GetTextureMan());
    pSpriteGroup->Initialize(0, layout[0], gnDisplayType == DISPLAY_IPHONE ? layout[1] : gDisplayHeight - layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
    mpSelectedObjectDescBar->AddControl(pSpriteGroup);
    
    
    
    //탱크이름
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    CControl* pTankescName =  new CLabelCtl(pSpriteGroup,m_pSGLCore->GetTextureMan());
    layout.clear();
    PROPERTYI::GetPropertyFloatList("DescST_TankescNameW", layout);
    pTankescName->Initialize(CTL_TANK_DESC_TANKNAME, layout[0], gnDisplayType == DISPLAY_IPHONE ? layout[1] : gDisplayHeight - layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
    pTankescName->SetTextColor(fWhite);
    pSpriteGroup->AddControl(pTankescName);
    
    
    //탱크레벨타이틀
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("M_Tank_Desc_Level");
    CControl* pTankDescLevelTitle =  new CLabelCtl(pSpriteGroup,m_pSGLCore->GetTextureMan());
    layout.clear();
    PROPERTYI::GetPropertyFloatList("DescST_TankDescLevelTW", layout);
    pTankDescLevelTitle->Initialize(0, layout[0], gnDisplayType == DISPLAY_IPHONE ? layout[1] : gDisplayHeight - layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
    pTankDescLevelTitle->SetTextColor(fWhite);
    pSpriteGroup->AddControl(pTankDescLevelTitle);
    
    //텡크레벨
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    CNumberCtl* pTankDescLevel =  new CNumberCtl(pSpriteGroup,m_pSGLCore->GetTextureMan(),true);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("DescST_TankDescLevelW", layout);
    pTankDescLevel->Initialize(CTL_TANK_DESC_LEVEL, layout[0], gnDisplayType == DISPLAY_IPHONE ? layout[1] : gDisplayHeight - layout[1], layout[2], layout[3],layout[4], layout[5], layout[6],lstImage);
    pTankDescLevel->SetAlign(CNumberCtl::ALIGN_RIGHT);
    float fTextColor[] = { 1.0f,1.0f,0.0f,1.0f}; //노랑
    pTankDescLevel->SetTextColor(fTextColor);
    pTankDescLevel->SetNumber(0);
    pSpriteGroup->AddControl(pTankDescLevel);
    
    
    //탱크아이콘 배경
    lstImage.clear();
    lstImage.push_back("res_slot.png");
    lstImage.push_back("none");
    CControl* pTankDescIconBack =  new CLabelCtl(pSpriteGroup,m_pSGLCore->GetTextureMan());
    layout.clear();
    PROPERTYI::GetPropertyFloatList("DescST_TankDescIconBackW", layout);
    pTankDescIconBack->Initialize(CTL_TANK_DESC_ICON, layout[0], gnDisplayType == DISPLAY_IPHONE ? layout[1] : gDisplayHeight - layout[1], layout[2], layout[3], lstImage, 0.7890625, 0.90625, 0.880859375, 0.998046875);
    pSpriteGroup->AddControl(pTankDescIconBack);
    
    
    //탱크 Attack상태 라벨
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("M_Tank_Desc_Power");
    CLabelCtl* pTankPowerStateTitle = new CLabelCtl(pSpriteGroup,m_pSGLCore->GetTextureMan());
    layout.clear();
    PROPERTYI::GetPropertyFloatList("DescST_TankPowerStateTitleW", layout);
    pTankPowerStateTitle->Initialize(0, layout[0], gnDisplayType == DISPLAY_IPHONE ? layout[1] : gDisplayHeight - layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
    pTankPowerStateTitle->SetTextColor(fWhite);
    pSpriteGroup->AddControl(pTankPowerStateTitle);
    
    //탱크 Attack상태
    lstImage.clear();
    lstImage.push_back("res_slot.png");
    lstImage.push_back("none");
    CImgProgressCtl* pTankPowerState = new CImgProgressCtl(pSpriteGroup,m_pSGLCore->GetTextureMan());
    layout.clear();
    PROPERTYI::GetPropertyFloatList("DescST_TankPowerStateW", layout);
    pTankPowerState->Initialize(CTL_TANK_DESC_ATTACK, layout[0], gnDisplayType == DISPLAY_IPHONE ? layout[1] : gDisplayHeight - layout[1], layout[2], layout[3], lstImage,
                                0.03125, 0.712890625, 0.181640625, 0.72265625,
                                0.03125, 0.728515625, 0.191640625, 0.73828125
                                );
    pSpriteGroup->AddControl(pTankPowerState);
    
    
    //탱크 Defence상태 라벨
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("M_Tank_Desc_Defense");
    CLabelCtl* pTankDefenceStateTitle = new CLabelCtl(pSpriteGroup,m_pSGLCore->GetTextureMan());
    layout.clear();
    PROPERTYI::GetPropertyFloatList("DescST_TankDefenceStateTW", layout);
    pTankDefenceStateTitle->Initialize(0, layout[0], gnDisplayType == DISPLAY_IPHONE ? layout[1] : gDisplayHeight - layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
    pTankDefenceStateTitle->SetTextColor(fWhite);
    pSpriteGroup->AddControl(pTankDefenceStateTitle);
    
    //탱크 Defence상태
    lstImage.clear();
    lstImage.push_back("res_slot.png");
    lstImage.push_back("none");
    CImgProgressCtl* pTankDefenceState = new CImgProgressCtl(pSpriteGroup,m_pSGLCore->GetTextureMan());
    layout.clear();
    PROPERTYI::GetPropertyFloatList("DescST_TankDefenceStW", layout);
    
    
    
    pTankDefenceState->Initialize(CTL_TANK_DESC_DEFEND, layout[0], gnDisplayType == DISPLAY_IPHONE ? layout[1] : gDisplayHeight - layout[1], layout[2], layout[3], lstImage,
                                  0.03125, 0.712890625, 0.181640625, 0.72265625,
                                  0.03125, 0.728515625, 0.191640625, 0.73828125
                                  //0.03125, 0.953125, 0.181640625, 0.962890625
                                  );
    pSpriteGroup->AddControl(pTankDefenceState);
    
    
    
    
    //설명
    string sDesc;
    const char* sDescription = GetText("abcdefg");
    char* sFnd = (char*)strstr(sDescription, "#");
    if(sFnd)
        sDesc = sFnd + 1;
    else
        sDesc = sDescription;
    
    
    
    const char* sFormat = GetText("M_Tank_Desc_Format");
    sFnd = (char*)strstr(sFormat, "#");
    if(sFnd)
    {
        sDesc.append("#");
        sDesc.append(sFnd + 1);// = sFnd + 1;
    }
    else
    {
        sDesc.append("#");
        sDesc.append(sFormat);
    }
    
    
    
    
    //설명...그룹
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back(sDesc.c_str());
    
    mpDescCtl = new CLabelCtl(mpSelectedObjectDescBar,m_pSGLCore->GetTextureMan());
    layout.clear();
    PROPERTYI::GetPropertyFloatList("DescST_TankDescW", layout);
    
    mpDescCtl->Initialize(CTL_TANK_DESC_DESC, layout[0], gnDisplayType == DISPLAY_IPHONE ? layout[1] : gDisplayHeight - layout[1], layout[2], layout[3], lstImage,
                          0.f, 0.f, 0.f, 0.f
                          );
    
    mpDescCtl->SetTextColor(fWhite);
    mpSelectedObjectDescBar->AddControl(mpDescCtl);
    
    
    float fHX = layout[3];
    
    //설명...그룹
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back(sDesc.c_str());
    
    mpDescCtl2 = new CLabelCtl(mpSelectedObjectDescBar,m_pSGLCore->GetTextureMan());
    layout.clear();
    PROPERTYI::GetPropertyFloatList("DescST_TankDescW", layout);
    
    mpDescCtl2->Initialize(CTL_TANK_DESC_DESC, layout[0], gnDisplayType == DISPLAY_IPHONE ? layout[1] + fHX + 2 : gDisplayHeight - layout[1] + ( fHX + 4), layout[2], layout[3], lstImage,
                           0.f, 0.f, 0.f, 0.f
                           );
    
    mpDescCtl2->SetTextColor(fWhite);
    mpDescCtl2->SetHide(true);
    mpSelectedObjectDescBar->AddControl(mpDescCtl2);
    
    if(gnDisplayType == DISPLAY_IPHONE)
    {
        //설명...그룹
        lstImage.clear();
        lstImage.push_back("none");
        lstImage.push_back(sDesc.c_str());
        
        mpDescCtl3 = new CLabelCtl(mpSelectedObjectDescBar,m_pSGLCore->GetTextureMan());
        layout.clear();
        PROPERTYI::GetPropertyFloatList("DescST_TankDescW", layout);
        
        mpDescCtl3->Initialize(CTL_TANK_DESC_DESC, layout[0], layout[1] + fHX * 2 + 2 , layout[2], layout[3], lstImage,
                               0.f, 0.f, 0.f, 0.f
                               );
        
        mpDescCtl3->SetTextColor(fWhite);
        mpDescCtl3->SetHide(true);
        mpSelectedObjectDescBar->AddControl(mpDescCtl3);
        
    }
    
    
    
    
    
    mpLeftTools->AddControl(mpSelectedObjectDescBar);
    OnSelectedObjectShowDescBar(GetActor());
}

void CHWorld::InitItemSlot()
{
    CUserInfo* pUserInfo = mpScenario->GetUserInfo();
    CTextureMan* pTextureMan = m_pSGLCore->GetTextureMan();
    float fDisableColor[] = {0.5f,0.5f,0.5f,0.9f};
    
    vector<string>  lstImage;
    vector<float> layout;
    
    
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    
    mpItemSlotBar =  new CLabelCtl(NULL,pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SL_ItemSlotBar_World", layout);
    
    //246,-80
    float fX = layout[0];
    float fY = gDisplayHeight + layout[1];
    
    if(gnDisplayType == DISPLAY_IPHONE)
    {
        fX = gDisplayWidth + layout[0];
        fY = layout[1];
    }
    
    mpItemSlotBar->Initialize(0, fX, fY, layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
    
    if(gnDisplayType == DISPLAY_IPAD)
    {
        lstImage.clear();
        lstImage.push_back("res_slot.png");
        lstImage.push_back("none");
        CControl* pBottomLeftItemBack =  new CLabelCtl(mpItemSlotBar,pTextureMan);
        layout.clear();
        PROPERTYI::GetPropertyFloatList("Back_BottomLeftItemBack", layout);
        pBottomLeftItemBack->Initialize(0, layout[0], gDisplayHeight - layout[1], (gDisplayWidth - layout[0]) / 2.f, layout[3], lstImage, 0.0, 0.0f, 1.0f, 0.15234375);
        pBottomLeftItemBack->SetEnable(false);
        mpItemSlotBar->AddControl(pBottomLeftItemBack);
        
        lstImage.clear();
        lstImage.push_back("res_slot.png");
        lstImage.push_back("none");
        CControl* pBottomRightItemBack =  new CLabelCtl(mpItemSlotBar,pTextureMan);
        layout.clear();
        PROPERTYI::GetPropertyFloatList("Back_BottomRightItemBack", layout);
        pBottomRightItemBack->Initialize(0, layout[0] + (gDisplayWidth - layout[0])/2.f, gDisplayHeight - layout[1], (gDisplayWidth - layout[0])/2.f, layout[3], lstImage, 0.0, 0.16015625, 1.0f, 0.3125);
        pBottomRightItemBack->SetEnable(false);
        mpItemSlotBar->AddControl(pBottomRightItemBack);
    }
    
    lstImage.clear();
    lstImage.push_back("res_slot.png");
    lstImage.push_back("none");
    
    //아이템 페이지.
    CControl* pItemPageNumberBack =  new CLabelCtl(mpItemSlotBar,pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SL_ItemPageNumberBack", layout);
    pItemPageNumberBack->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.43359375, 0.330078125, 0.525390625, 0.421875);
    mpItemSlotBar->AddControl(pItemPageNumberBack);
    
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    CNumberCtl* pItemPageNumber =  new CNumberCtl(pItemPageNumberBack,pTextureMan,true);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SL_ItemPageNumber", layout);
    pItemPageNumber->Initialize(CTL_TABLE_ITEM_SLOT_PAGE, fX + layout[0], fY + layout[1], layout[2], layout[3],layout[4],layout[5],layout[6],lstImage);
    float fWhite[] = { 1.f,1.f,1.f,1.f};
    pItemPageNumber->SetTextColor(fWhite);
    pItemPageNumber->SetAlign(CNumberCtl::ALIGN_CENTER);
    pItemPageNumberBack->AddControl(pItemPageNumber);
    
    //float fRed[] = { 1.0f,0.0f,0.0f,1.0f};
    lstImage.clear();
    lstImage.push_back("res_slot.png");
    if(pUserInfo->GetLastBombItemSwitch() == 0)
        lstImage.push_back("M_Slot_Bomb");
    else if(pUserInfo->GetLastBombItemSwitch() == 1)
        lstImage.push_back("M_Slot_Item");
    else //2
        lstImage.push_back("M_Slot_BLZ");
    
    mpItemSlotSwitchBtn =  new CButtonCtl(mpItemSlotBar,pTextureMan,BUTTON_NORMAL);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SL_ItemSlotSwitchBtn", layout);
    mpItemSlotSwitchBtn->Initialize(BTN_ITEMSLOTSWITCH, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage,
                                    0.025390625, 0.33203125, 0.26953125, 0.44921875,
                                    0.025390625, 0.33203125, 0.26953125, 0.44921875);
    mpItemSlotSwitchBtn->SetTextColor(fWhite);
    
    mpItemSlotSwitchBtn->SetDisableColor(fDisableColor);
    mpItemSlotBar->AddControl(mpItemSlotSwitchBtn);
    
    //Slot3
    lstImage.clear();
    lstImage.push_back("res_slot.png");
    lstImage.push_back("none");
    CButtonCtl* pItemBox3Back =  new CButtonCtl(mpItemSlotBar,pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SL_ItemBox3Back", layout);
    pItemBox3Back->Initialize(CTL_TABLE_ITEM_SLOT3, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.2890625, 0.328125, 0.416015625, 0.455078125,
                              0.654296875,0.328125,0.78125,0.45703125);
    mpItemSlotBar->AddControl(pItemBox3Back);
    
    //Icon
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    CControl* pItemBox3 =  new CLabelCtl(pItemBox3Back,pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SL_ItemBox3", layout);
    pItemBox3->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 1.f, 1.f);
    pItemBox3->SetEnable(false);
    pItemBox3Back->AddControl(pItemBox3);
    
    //Gold or Percent
    CNumberCtl* pItemBox3Num =  new CNumberCtl(pItemBox3,pTextureMan,true);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SL_ItemBox3Num", layout);
    pItemBox3Num->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3],layout[4],layout[5],layout[6],lstImage);
    pItemBox3Num->SetAlign(CNumberCtl::ALIGN_RIGHT);
    pItemBox3Num->SetTextColor(fWhite);
    pItemBox3Num->SetEnable(false);
    pItemBox3->AddControl(pItemBox3Num);
    //Gold Icon
    CLabelCtl* pGoldImg3 =  new CLabelCtl(pItemBox3,pTextureMan);
    lstImage.clear();
    lstImage.push_back("gold.png");
    lstImage.push_back("none");
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SL_ItemBox3Gold", layout);
    
    pGoldImg3->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3],lstImage, 0.f, 0.f, 1.f, 1.f);
    pGoldImg3->SetTextColor(fWhite);
    pGoldImg3->SetEnable(false);
    pItemBox3->AddControl(pGoldImg3);
    
    
    //Progress
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    CProgressCtl* pProgress3 =  new CProgressCtl(pItemBox3Back);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SL_ItemBox3Progress", layout);
    pProgress3->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3]);
    pProgress3->SetEnable(false);
    pProgress3->SetTranslate(0.8f);
    pItemBox3Back->AddControl(pProgress3);
    pProgress3->SetHide(true);
    
    
    //Slot4
    lstImage.clear();
    lstImage.push_back("res_slot.png");
    lstImage.push_back("none");
    CButtonCtl* pItemBox4Back =  new CButtonCtl(mpItemSlotBar,pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SL_ItemBox4Back", layout);
    pItemBox4Back->Initialize(CTL_TABLE_ITEM_SLOT4, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.2890625, 0.328125, 0.416015625, 0.4550781255,
                              0.654296875,0.328125,0.78125,0.45703125);
    mpItemSlotBar->AddControl(pItemBox4Back);
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    CControl* pItemBox4 =  new CLabelCtl(pItemBox4Back,pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SL_ItemBox4", layout);
    pItemBox4->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 1.f, 1.f);
    pItemBox4->SetEnable(false);
    pItemBox4Back->AddControl(pItemBox4);
    
    CNumberCtl* pItemBox4Num =  new CNumberCtl(pItemBox4,pTextureMan,true);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SL_ItemBox4Num", layout);
    pItemBox4Num->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3],layout[4],layout[5],layout[6],lstImage);
    pItemBox4Num->SetAlign(CNumberCtl::ALIGN_RIGHT);
    pItemBox4Num->SetTextColor(fWhite);
    pItemBox4Num->SetEnable(false);
    pItemBox4->AddControl(pItemBox4Num);
    
    
    CLabelCtl* pGoldImg4 =  new CLabelCtl(pItemBox4,pTextureMan);
    lstImage.clear();
    lstImage.push_back("gold.png");
    lstImage.push_back("none");
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SL_ItemBox4Gold", layout);
    pGoldImg4->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3],lstImage, 0.f, 0.f, 1.f, 1.f);
    pGoldImg4->SetTextColor(fWhite);
    pGoldImg4->SetEnable(false);
    pItemBox4->AddControl(pGoldImg4);
    
    
    //Progress
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    CProgressCtl* pProgress4 =  new CProgressCtl(pItemBox4Back);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SL_ItemBox4Progress", layout);
    pProgress4->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3]);
    pProgress4->SetEnable(false);
    pProgress4->SetTranslate(0.8f);
    pItemBox4Back->AddControl(pProgress4);
    pProgress4->SetHide(true);
    
    //Slot5
    lstImage.clear();
    lstImage.push_back("res_slot.png");
    lstImage.push_back("none");
    CButtonCtl* pItemBox5Back =  new CButtonCtl(mpItemSlotBar,pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SL_ItemBox5Back", layout);
    pItemBox5Back->Initialize(CTL_TABLE_ITEM_SLOT5, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.2890625, 0.328125, 0.416015625, 0.4550781255,
                              0.654296875,0.328125,0.78125,0.45703125);
    mpItemSlotBar->AddControl(pItemBox5Back);
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    CControl* pItemBox5 =  new CLabelCtl(pItemBox5Back,pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SL_ItemBox5", layout);
    pItemBox5->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 1.f, 1.f);
    pItemBox5->SetEnable(false);
    pItemBox5Back->AddControl(pItemBox5);
    
    CNumberCtl* pItemBox5Num =  new CNumberCtl(pItemBox5,pTextureMan,true);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SL_ItemBox5Num", layout);
    pItemBox5Num->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3],layout[4],layout[5],layout[6],lstImage);
    pItemBox5Num->SetAlign(CNumberCtl::ALIGN_RIGHT);
    pItemBox5Num->SetTextColor(fWhite);
    pItemBox5Num->SetEnable(false);
    pItemBox5->AddControl(pItemBox5Num);
    
    CLabelCtl* pGoldImg5 =  new CLabelCtl(pItemBox5,pTextureMan);
    lstImage.clear();
    lstImage.push_back("gold.png");
    lstImage.push_back("none");
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SL_ItemBox5Gold", layout);
    pGoldImg5->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3],lstImage, 0.f, 0.f, 1.f, 1.f);
    pGoldImg5->SetTextColor(fWhite);
    pGoldImg5->SetEnable(false);
    pItemBox5->AddControl(pGoldImg5);
    
    
    //Progress
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    CProgressCtl* pProgress5 =  new CProgressCtl(pItemBox5Back);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SL_ItemBox5Progress", layout);
    pProgress5->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3]);
    pProgress5->SetEnable(false);
    pProgress5->SetTranslate(0.8f);
    pItemBox5Back->AddControl(pProgress5);
    pProgress5->SetHide(true);
    
    //Slot6
    lstImage.clear();
    lstImage.push_back("res_slot.png");
    lstImage.push_back("none");
    CButtonCtl* pItemBox6Back =  new CButtonCtl(mpItemSlotBar,pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SL_ItemBox6Back", layout);
    pItemBox6Back->Initialize(CTL_TABLE_ITEM_SLOT6, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.2890625, 0.328125, 0.416015625, 0.4550781255,
                              0.654296875,0.328125,0.78125,0.45703125);
    mpItemSlotBar->AddControl(pItemBox6Back);
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    CControl* pItemBox6 =  new CLabelCtl(pItemBox6Back,pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SL_ItemBox6", layout);
    pItemBox6->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 1.f, 1.f);
    pItemBox6->SetEnable(false);
    pItemBox6Back->AddControl(pItemBox6);
    
    
    
    
    CNumberCtl* pItemBox6Num =  new CNumberCtl(pItemBox6,pTextureMan,true);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SL_ItemBox6Num", layout);
    pItemBox6Num->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3],layout[4],layout[5],layout[6],lstImage);
    pItemBox6Num->SetAlign(CNumberCtl::ALIGN_RIGHT);
    pItemBox6Num->SetTextColor(fWhite);
    pItemBox6Num->SetEnable(false);
    pItemBox6->AddControl(pItemBox6Num);
    
    
    CLabelCtl* pGoldImg =  new CLabelCtl(pItemBox6,pTextureMan);
    lstImage.clear();
    lstImage.push_back("gold.png");
    lstImage.push_back("none");
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SL_ItemBox6Gold", layout);
    pGoldImg->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3],lstImage, 0.f, 0.f, 1.f, 1.f);
    pGoldImg->SetTextColor(fWhite);
    pGoldImg->SetEnable(false);
    pItemBox6->AddControl(pGoldImg);
    
    
    //Progress
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    CProgressCtl* pProgress6 =  new CProgressCtl(pItemBox6Back);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SL_ItemBox6Progress", layout);
    pProgress6->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3]);
    pProgress6->SetEnable(false);
    pProgress6->SetTranslate(0.8f);
    pItemBox6Back->AddControl(pProgress6);
    pProgress6->SetHide(true);
    
    
    lstImage.clear();
    lstImage.push_back("res_slot.png");
    lstImage.push_back("none");
    
    //아이템페이지 updown
    CControl* pItemPageBack =  new CLabelCtl(mpItemSlotBar,pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SL_ItemPageBack", layout);
    pItemPageBack->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage,
                              0.544921875, 0.330078125, 0.634765625, 0.46484375);
    mpItemSlotBar->AddControl(pItemPageBack);
    
    CControl* pItemPageUp =  new CButtonCtl(pItemPageBack,pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SL_ItemPageUp", layout);
    pItemPageUp->Initialize(BTN_ITEMSLOTUPPAGE, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage,
                            0.404296875, 0.607421875, 0.49609375, 0.69921875);
    
    pItemPageUp->SetDisableColor(fDisableColor);
    pItemPageBack->AddControl(pItemPageUp);
    
    CControl* pItemPageDown =  new CButtonCtl(pItemPageBack,pTextureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("SL_ItemPageDown", layout);
    pItemPageDown->Initialize(BTN_ITEMSLOTDOWNPAGE, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage,
                              0.404296875, 0.83984375, 0.49609375, 0.931640625);
    pItemPageDown->SetDisableColor(fDisableColor);
    pItemPageBack->AddControl(pItemPageDown);
    
    
    
    mpItemSlotBar->SetTranslate(0.7f);
}


void CHWorld::ResetDisplayItemSlot()
{
    CSprite* pActor = GetActor();
    vector<string>  lstImage;
    int nMaxPageCnt = 0;
    const int nMaxItem = MAXSLOTCNT;
    vector<CControl*> lstSlotCtl;
    CUserInfo* pUserInfo = mpScenario->GetUserInfo();
    vector<USERINFO*>  lstType3Org;
    vector<USERINFO*>* lstItem = NULL;
    int nType = 0;
    int nBombID = 0;
    
    if(mpClickedSprite == NULL)
    {
        nType = pUserInfo->GetLastBombItemSwitch(); // 0 : Bomb 1:Item 2:Blz 3:ClickedTower
        if(nType == 1)
        {
            lstItem = pUserInfo->GetListItem();
        }
        else if(nType == 0)
        {
            lstItem = pUserInfo->GetListBomb();
            if(pActor && pActor->GetState() == SPRITE_RUN)
                nBombID = pActor->GetCurrentWeaponID();
        }
        else //2 : Blz
        {
            lstItem = pUserInfo->GetListBlz();
        }
        mpItemSlotSwitchBtn->SetEnable(true);
    }
    else
    {
        nType = 3;
        if(mpClickedSprite)
        {
            if(mpClickedSprite->IsMaking())
            {
                if(mlstRemoveSlotInfo == NULL)
                {
                    mlstRemoveSlotInfo = new vector<USERINFO*>;
                    USERINFO* pRemoveSlotInfo = new USERINFO;
                    RUNTIME_UPGRADE* pUp = new RUNTIME_UPGRADE;
                    RUNTIME_UPGRADE::GetPropertyRunUpgradeItem(RUNTIME_UPDATE_REMOVE, *pUp);
                    pRemoveSlotInfo->nID = pUp->nID;
                    pRemoveSlotInfo->nType = pUp->nTime;
                    pRemoveSlotInfo->nCnt = pUp->nGold;
                    pRemoveSlotInfo->pProp = pUp;
                    mlstRemoveSlotInfo->push_back(pRemoveSlotInfo);
                    lstItem = mlstRemoveSlotInfo;
                }
                else
                    lstItem = mlstRemoveSlotInfo;
            }
            else
            {
                lstItem =  mpClickedSprite->GetRuntimeUpgradList();
                if(lstItem == NULL) return;
                
                //메인타워는 탱크를 생산한다. 그래서 탱크가 생산되는 즉 업그레이드(생산되는)표시가 있는 것만 표시해준다.
                if(mpClickedSprite->GetModelID() == ACTORID_BLZ_MAINTOWER)
                {
                    if(mlstMainTowerSlotInfo == NULL)
                        mlstMainTowerSlotInfo = new vector<USERINFO*>;
                    else
                        mlstMainTowerSlotInfo->clear();
                    
                    for (vector<USERINFO*>::iterator it = lstItem->begin(); it != lstItem->end(); it++)
                    {
                        RUNTIME_UPGRADE* pProperty = (RUNTIME_UPGRADE*)(*it)->pProp;
                        if(pProperty->nID >= RUNTIME_UPDATE_ARMCAR && pProperty->nID <= RUNTIME_UPDATE_MISSILET)
                        {
                            //RUNTIME_UPDATE_ARMCAR 아이템 계열은 런타임시 업그레이드가 될때만 보여지게 해준다.
                            if(pProperty->nUpgreadTime == -1)   continue;
                            mlstMainTowerSlotInfo->push_back(*it);
                        }
                        else
                            mlstMainTowerSlotInfo->push_back(*it);
                    }
                    lstItem = mlstMainTowerSlotInfo;
                    
                }
                
            }
        }
        mpItemSlotSwitchBtn->SetEnable(false);
    }
    
    mpSelectedWeaponBtn = NULL;
    
    CNumberCtl* pItemPageNumber = (CNumberCtl*)mpItemSlotBar->FindControl(CTL_TABLE_ITEM_SLOT_PAGE);
    
    int nSelIndex = 1;
    if(nType == 1)
        nSelIndex = pUserInfo->GetLastSelectedItemPageIndex();
    else if(nType == 0)
        nSelIndex = pUserInfo->GetLastSelectedBombPageIndex();
    else if(nType == 2)
        nSelIndex = pUserInfo->GetLastSelectedBlzPageIndex();
    
    
    
    lstImage.clear();
    lstImage.push_back("res_slot.png");
    
    if(mpClickedSprite == NULL)
    {
        if(pUserInfo->GetLastBombItemSwitch() == 0)
            lstImage.push_back("M_Slot_Bomb");
        else if(pUserInfo->GetLastBombItemSwitch() == 1)
            lstImage.push_back("M_Slot_Item");
        else if(pUserInfo->GetLastBombItemSwitch() == 2)
            lstImage.push_back("M_Slot_BLZ");
    }
    else
    {
        lstImage.push_back("none"); //## 업그레이드
    }
    mpItemSlotSwitchBtn->SetImageData(lstImage);
    
    nMaxPageCnt = (int)lstItem->size() / nMaxItem;
    if((lstItem->size() % nMaxItem) != 0)
        nMaxPageCnt += 1;
    
    if(nSelIndex > nMaxPageCnt)
        nSelIndex = nMaxPageCnt;
    else if( nSelIndex < 1)
        nSelIndex = 1;
    
    pItemPageNumber->SetNumber(nSelIndex);
    
    
    CControl* pUp = mpItemSlotBar->FindControl(BTN_ITEMSLOTUPPAGE);
    CControl* pDown= mpItemSlotBar->FindControl(BTN_ITEMSLOTDOWNPAGE);
    
    if(nSelIndex == 1)
        pDown->SetEnable(false);
    else
        pDown->SetEnable(true);
    
    if(nSelIndex == nMaxPageCnt)
        pUp->SetEnable(false);
    else
        pUp->SetEnable(true);
    
    CButtonCtl* pBtnSlot = NULL;
    
    for(int i = CTL_TABLE_ITEM_SLOT3; i <= CTL_TABLE_ITEM_SLOT6; i++)
    {
        lstSlotCtl.push_back(mpItemSlotBar->FindControl(i));
    }
    
    
    if(nType == 3 && mpClickedSprite->IsMaking()) //지금 짖고 있다면 제거 아이템만 보이게 한다.
    {
        pBtnSlot = (CButtonCtl*)lstSlotCtl[0];
        lstImage.clear();
        lstImage.push_back("RevTower.tga");
        lstImage.push_back("none");
        
        pBtnSlot->GetChild(0)->SetImageData(lstImage);
        pBtnSlot->GetChild(0)->SetBackCoordTex(0.0f, 0.0f, 0.96875f, 0.96875f);
        pBtnSlot->SetLData((long)lstItem->at(0));
        pBtnSlot->SetPushed(false);
        
        pBtnSlot->GetChild(0)->GetChild(1)->SetHide(true); //골드아이콘
        pBtnSlot->GetChild(0)->GetChild(0)->SetHide(true); //골드
        pBtnSlot->GetChild(1)->SetHide(true); //프로그래스
        pBtnSlot->SetButtonType(BUTTON_NORMAL);
        
        for (int i = 1; i < nMaxItem; i++)
        {
            pBtnSlot = (CButtonCtl*)lstSlotCtl[i];
            lstImage.clear();
            lstImage.push_back("none");
            lstImage.push_back("none");
            pBtnSlot->GetChild(0)->SetImageData(lstImage);
            pBtnSlot->GetChild(0)->GetChild(0)->SetHide(true); //골드
            pBtnSlot->GetChild(0)->GetChild(1)->SetHide(true); //골드아이콘
            pBtnSlot->GetChild(1)->SetHide(true); //프로그래스
            pBtnSlot->SetLData(-1);
            
            pBtnSlot->SetButtonType(BUTTON_NORMAL);
            pBtnSlot->SetPushed(false);
            pBtnSlot->SetEnable(false);
        }
        return ;
    }
    
    int nIndex = 0;
    int nMaxCnt = (int)lstItem->size();
    string sPath;
    
    for (int i = 0; i < nMaxItem; i++)
    {
        nIndex = (nSelIndex - 1) * nMaxItem + i;
        pBtnSlot = (CButtonCtl*)lstSlotCtl[i];
        
        if(nIndex >= nMaxCnt || nMaxCnt == 0)
        {
            lstImage.clear();
            lstImage.push_back("none");
            lstImage.push_back("none");
            pBtnSlot->GetChild(0)->SetImageData(lstImage);
            pBtnSlot->GetChild(0)->GetChild(0)->SetHide(true); //골드
            pBtnSlot->GetChild(0)->GetChild(1)->SetHide(true); //골드아이콘
            pBtnSlot->GetChild(1)->SetHide(true); //프로그래스
            pBtnSlot->SetLData(-1);
            
            pBtnSlot->SetButtonType(BUTTON_NORMAL);
            pBtnSlot->SetPushed(false);
            pBtnSlot->SetEnable(false);
        }
        else
        {
            pBtnSlot->SetEnable(true);
            if(nType == 1)
            {
                int nID = lstItem->at(nIndex)->nID;
                PROPERTY_ITEM property;
                if(PROPERTY_ITEM::GetPropertyItem(nID, property) == NULL) continue;
                
                lstImage.clear();
                sPath = property.sModelPath;
                sPath.append(".tga");
                lstImage.push_back(sPath);
                lstImage.push_back("none");
                
                pBtnSlot->GetChild(0)->SetImageData(lstImage);
                pBtnSlot->GetChild(0)->SetBackCoordTex(0.0f, 0.0f, 0.96875f, 0.96875f);
                pBtnSlot->GetChild(0)->GetChild(1)->SetHide(true);
                pBtnSlot->GetChild(1)->SetHide(true); //프로그래스
                
                pBtnSlot->SetLData((long)lstItem->at(nIndex));
                pBtnSlot->SetPushed(false);
                
                pBtnSlot->SetButtonType(BUTTON_NORMAL);
                
                if(property.nType == 0 || property.nType == 6) //Count
                {
                    CNumberCtl* pNumCtl = (CNumberCtl*)lstSlotCtl[i]->GetChild(0)->GetChild(0);
                    pNumCtl->SetNumber(pUserInfo->GetCntByIDWithItem(nID));
                    pBtnSlot->GetChild(0)->GetChild(0)->SetHide(false);
                }
                else if(property.nType == 2) //Upgrade
                {
                    PROPERTY_TANK property;
                    PROPERTY_TANK::GetPropertyTank(pUserInfo->GetLastSelectedTankID(),property);
                    if(nID == ID_ITEM_UPGRADE_ATTACK)
                    {
                        CNumberCtl* pNumCtl = (CNumberCtl*)pBtnSlot->GetChild(0)->GetChild(0);
                        int nUpgrade = pUserInfo->GetAttackUpgrade() * 5.f; //100으로 처리하자.
                        pNumCtl->SetNumber(nUpgrade,CNumberCtlType_Percent);
                        pBtnSlot->GetChild(0)->GetChild(0)->SetHide(false);
                        
                    }
                    else if(nID == ID_ITEM_UPGRADE_DEPEND)
                    {
                        CNumberCtl* pNumCtl = (CNumberCtl*)pBtnSlot->GetChild(0)->GetChild(0);
                        int nUpgrade = pUserInfo->GetDependUpgrade() * 5.f; //100으로 처리하자.
                        pNumCtl->SetNumber(nUpgrade,CNumberCtlType_Percent);
                        pBtnSlot->GetChild(0)->GetChild(0)->SetHide(false);
                        
                    }
                    else if(nID == ID_ITEM_UPGRADE_DEFENCE_TOWER || nID == ID_ITEM_UPGRADE_MISSILE_TOWER)
                    {
                        CNumberCtl* pNumCtl = (CNumberCtl*)lstSlotCtl[i]->GetChild(0)->GetChild(0);
                        int nUpgrade = pUserInfo->GetCntByIDWithItem(nID) * 5; //100으로 처리하자.
                        pNumCtl->SetNumber(nUpgrade,CNumberCtlType_Percent);
                        lstSlotCtl[i]->GetChild(0)->GetChild(0)->SetHide(false);
                    }
                    else
                    {
                        //pNumCtl->ClearNumber();
                        pBtnSlot->GetChild(0)->GetChild(0)->SetHide(true);
                    }
                }
                else
                {
                    //                pNumCtl->ClearNumber();
                    pBtnSlot->GetChild(0)->GetChild(0)->SetHide(true);
                }
            }
            else if(nType == 0) //Bomb
            {
                int nID = lstItem->at(nIndex)->nID;
                PROPERTY_BOMB property;
                PROPERTY_BOMB::GetPropertyBomb(nID, property);
                
                lstImage.clear();
                lstImage.push_back(property.sBombTableImgPath);
                lstImage.push_back("none");
                pBtnSlot->GetChild(0)->SetImageData(lstImage);
                //폭탄이미지는 0.0,0.0,1.0,1.0 이 아니다.
                //0.2265625 => 0.1265625 (비율때문)
                //0.7734375 => 0.8734375 (비율때문)
                pBtnSlot->GetChild(0)->SetBackCoordTex(0.20703125, 0.1265625, 0.79296875, 0.8734375);
                pBtnSlot->GetChild(0)->GetChild(1)->SetHide(true);
                pBtnSlot->GetChild(1)->SetHide(true); //프로그래스
                
                pBtnSlot->SetLData((long)lstItem->at(nIndex));
                pBtnSlot->SetButtonType(BUTTON_GROUP_TOGGLE);
                
                pBtnSlot->SetLData2(property.nType); //카운터.. 비카운터
                if(nBombID == nID)
                {
                    pBtnSlot->SetPushed(true);
                    mpSelectedWeaponBtn = pBtnSlot;
                }
                else
                    pBtnSlot->SetPushed(false);
                
                
                if(property.nType == 0) //Count
                {
                    CNumberCtl* pNumCtl = (CNumberCtl*)lstSlotCtl[i]->GetChild(0)->GetChild(0);
                    pNumCtl->SetNumber(pUserInfo->GetCntByIDWithBomb(nID));
                    pBtnSlot->GetChild(0)->GetChild(0)->SetHide(false);
                }
                else
                {
                    pBtnSlot->GetChild(0)->GetChild(0)->SetHide(true);
                }
            }
            else if(nType == 2) //건물
            {
                USERINFO* pInfo = lstItem->at(nIndex);
                int nID = pInfo->nID;
                PROPERTY_TANK property;
                PROPERTY_TANK::GetPropertyTank(nID, property);
                
                lstImage.clear();
                sPath = property.sModelPath;
                sPath.append(".tga");
                lstImage.push_back(sPath);
                lstImage.push_back("none");
                
                pBtnSlot->GetChild(0)->SetImageData(lstImage);
                pBtnSlot->GetChild(0)->SetBackCoordTex(0.0f, 0.0f, 0.96875f, 0.96875f);
                pBtnSlot->SetLData((long)pInfo);
                pBtnSlot->SetPushed(false);
                
                pBtnSlot->SetButtonType(BUTTON_NORMAL);
                
                if(pInfo->nType == 0) //Count
                {
                    CNumberCtl* pNumCtl = (CNumberCtl*)lstSlotCtl[i]->GetChild(0)->GetChild(0);
                    pNumCtl->SetNumber(pInfo->nCnt);
                    pBtnSlot->GetChild(0)->GetChild(0)->SetHide(false);
                }
                else
                {
                    pBtnSlot->GetChild(0)->GetChild(0)->SetHide(true);
                }
                
                pBtnSlot->GetChild(0)->GetChild(1)->SetHide(false); //골드아이콘
                pBtnSlot->GetChild(1)->SetHide(true); //프로그래스
                
            }
            else if(nType == 3)
            {
                //------------------------------------------------------------------------------------------------
                USERINFO* pInfo = lstItem->at(nIndex);
                RUNTIME_UPGRADE* pProperty = (RUNTIME_UPGRADE*)pInfo->pProp;
                lstImage.clear();
                sPath = pProperty->sPath;
                lstImage.push_back(pProperty->sPath);
                lstImage.push_back("none");
                
                pBtnSlot->GetChild(0)->SetImageData(lstImage);
                pBtnSlot->GetChild(0)->SetBackCoordTex(0.0f, 0.0f, 0.96875f, 0.96875f);
                pBtnSlot->SetLData((long)pInfo);
                pBtnSlot->SetPushed(false);
                
                pBtnSlot->SetButtonType(BUTTON_NORMAL);
                
                if(pInfo->nCnt != 0 || (pInfo->nID >= RUNTIME_UPDATE_ARMCAR && pInfo->nID <= RUNTIME_UPDATE_MISSILET))
                {
                    CNumberCtl* pNumCtl = (CNumberCtl*)lstSlotCtl[i]->GetChild(0)->GetChild(0);
                    pNumCtl->SetNumber(pInfo->nCnt);
                    
                    
                    if(pInfo->nID >= RUNTIME_UPDATE_ARMCAR && pInfo->nID <= RUNTIME_UPDATE_MISSILET)
                    {
                        pBtnSlot->GetChild(0)->GetChild(0)->SetHide(true); //Gold Number
                        pBtnSlot->GetChild(0)->GetChild(1)->SetHide(true);  //골드는 숨긴다.
                    }
                    else
                    {
                        pBtnSlot->GetChild(0)->GetChild(0)->SetHide(false); //Gold Number
                        pBtnSlot->GetChild(0)->GetChild(1)->SetHide(false); //Gold Icon
                    }
                    
                    if(pProperty->nUpgreadTime == -1)
                    {
                        
                        //메인타워인경우 탱크만드는 아이콘이라면, 생산이 다되었으면 보여주지 말자꾸나.
                        if(mpClickedSprite->GetModelID() == ACTORID_BLZ_MAINTOWER &&
                           pProperty->nID >= RUNTIME_UPDATE_ARMCAR && pProperty->nID <= RUNTIME_UPDATE_MISSILET)
                        {
                            pBtnSlot->GetChild(1)->SetHide(true); //프로그래스 완성되어 있다면 보여지지 말자.
                            //안보이게 한다.
                            pBtnSlot->GetChild(0)->GetChild(0)->SetHide(true); //돈
                            pBtnSlot->GetChild(0)->GetChild(1)->SetHide(true); //골드아이콘
                        }
                        else
                        {
                            pBtnSlot->GetChild(1)->SetHide(true); //프로그래스 완성되어 있다면 보여지지 말자.
                            
                            RUNTIME_UPGRADE* pRunTime = (RUNTIME_UPGRADE*)pInfo->pProp;
                            if(pRunTime)
                            {
                                if(pRunTime->nUpgradeCnt >= pRunTime->nUpgradeMaxCnt)
                                {
                                    //안보이게 한다.
                                    pBtnSlot->GetChild(0)->GetChild(0)->SetHide(true); //돈
                                    pBtnSlot->GetChild(0)->GetChild(1)->SetHide(true); //골드아이콘
                                }
                            }
                        }
                        
                    }
                    else
                    {
                        CProgressCtl* pro = (CProgressCtl*)pBtnSlot->GetChild(1);
                        pro->SetPosition((float)pProperty->nUpgreadTime / (float)pProperty->nTime * 100.f);
                        pBtnSlot->GetChild(1)->SetHide(false); //프로그래스
                        pBtnSlot->SetEnable(false); //다시클릭되지 않게 막아준다
                    }
                }
                else
                {
                    pBtnSlot->GetChild(0)->GetChild(0)->SetHide(true);
                    pBtnSlot->GetChild(0)->GetChild(1)->SetHide(true);
                    pBtnSlot->GetChild(1)->SetHide(true); //프로그래스
                }
                //------------------------------------------------------------------------------------------------
            }
        }
    }
    
    if(nType == 1)
        pUserInfo->SetLastSelectedItemPageIndex(nSelIndex);
    else if(nType == 0)
        pUserInfo->SetLastSelectedBombPageIndex(nSelIndex);
    else if(nType == 2)
        pUserInfo->SetLastSelectedBlzPageIndex(nSelIndex);
}

void CHWorld::RenderClickedSpriteSlot(CSprite* pSprite)
{
    if(mpClickedSprite == pSprite)
    {
        CControl* pBtnSlot = 0;
        for(int i = CTL_TABLE_ITEM_SLOT3; i <= CTL_TABLE_ITEM_SLOT6; i++)
        {
            pBtnSlot = mpItemSlotBar->FindControl(i);
            if(pBtnSlot->GetLData() == -1) continue;
            USERINFO* pInfo = (USERINFO*)pBtnSlot->GetLData();
            RUNTIME_UPGRADE* pProperty = (RUNTIME_UPGRADE*)pInfo->pProp;
            if(pProperty->nUpgreadTime != -1)
            {
                CProgressCtl* pro = (CProgressCtl*)pBtnSlot->GetChild(1);
                pro->SetPosition((float)pProperty->nUpgreadTime / (float)pProperty->nTime * 100.f);
            }
        }
    }
}



void CHWorld::OnCompletedUpdated(CSprite* pSprite,int nID)
{
    if(mpClickedSprite == pSprite)
    {
        //-------------------------------------------------------------
        //슬롯에서 제거하기 때문에 .....
        if(mpClickedSprite->GetModelID() == ACTORID_BLZ_MAINTOWER &&
           (nID >= RUNTIME_UPDATE_ARMCAR && nID <= RUNTIME_UPDATE_MISSILET))
        {
            ResetDisplayItemSlot();
            return;
        }
        //-------------------------------------------------------------
        
        CControl* pBtnSlot = 0;
        for(int i = CTL_TABLE_ITEM_SLOT3; i <= CTL_TABLE_ITEM_SLOT6; i++)
        {
            pBtnSlot = mpItemSlotBar->FindControl(i);
            USERINFO* pInfo = (USERINFO*)pBtnSlot->GetLData();
            if(pInfo->nID == nID)
            {
                pBtnSlot->GetChild(1)->SetHide(true); //프로그래스
                pBtnSlot->SetEnable(true);
                
                RUNTIME_UPGRADE* pRunTime = (RUNTIME_UPGRADE*)pInfo->pProp;
                if(pRunTime)
                {
                    if(pRunTime->nUpgradeCnt >= pRunTime->nUpgradeMaxCnt)
                    {
                        //안보이게 한다.
                        pBtnSlot->GetChild(0)->GetChild(0)->SetHide(true); //돈
                        pBtnSlot->GetChild(0)->GetChild(1)->SetHide(true); //골드아이콘
                    }
                }
                
                break;
            }
        }
    }
}


void CHWorld::ResetClickedSpriteSlotFromMakingTime(CSprite* pSprite)
{
    if(mpClickedSprite == pSprite)
    {
        ResetDisplayItemSlot();
    }
}

void CHWorld::Clear()
{
    m_pSGLCore->Clear();
}


CListNode<CSprite>* CHWorld::GetSpriteList()
{
    return m_pSGLCore->GetSpriteList();
}


CSprite* CHWorld::GetActor()
{
    return m_pSGLCore->GetActor();
}


int CHWorld::initProjection()
{
    
    CWorld::initProjection();
    
#ifndef ANDROID //IOS에서는 한번만 셋팅해주면 정의가 된다.
    //방향벡터의 normalize를 하지 않음. 그래서 gl에서 해주어야 샤이닝이 이러난다.
    //모델의 빛 반사를 정의 하려면 GL_NORMALIZE화 해주어야 한다.
    glEnable(GL_NORMALIZE);
#endif
    
    return E_SUCCESS;
}

CTextureMan* CHWorld::GetTextureMan()
{
    return m_pSGLCore->GetTextureMan();
}


float CHWorld::GetViewAperture()
{
    if(sglIsFastModel())
        return VIEW_APERTURE_WORLD;
    return VIEW_APERTURE;
}

CModelMan* CHWorld::GetModelMan()
{
    return m_pSGLCore->GetModelMan();
}

int CHWorld::ActivateCamera()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    
    if(mnAnimationCameraAttackedCnt == 0)
    {
        gluLookAt (mCamera.viewPos.x,
                   mCamera.viewPos.y,
                   mCamera.viewPos.z,
                   mCamera.viewPos.x + mCamera.viewDir.x,
                   mCamera.viewPos.y + mCamera.viewDir.y,
                   mCamera.viewPos.z + mCamera.viewDir.z,
                   mCamera.viewUp.x,
                   mCamera.viewUp.y ,
                   mCamera.viewUp.z);
    }
    else
    {
        //한번만 흔들자.. 왔다갔다. 1 = 왼쪽, 2 = 반대방향 3 = 멈춘다.
        if(mnAnimationCameraAttackedCnt == 2)
            mfAnimationCameraAttacked = -mfAnimationCameraAttacked;
        
        gluLookAt (mCamera.viewPos.x,
                   mCamera.viewPos.y,
                   mCamera.viewPos.z,
                   mCamera.viewPos.x + mCamera.viewDir.x + mfAnimationCameraAttacked,
                   mCamera.viewPos.y + mCamera.viewDir.y - mfAnimationCameraAttacked,
                   mCamera.viewPos.z + mCamera.viewDir.z + mfAnimationCameraAttacked,
                   mCamera.viewUp.x,
                   mCamera.viewUp.y ,
                   mCamera.viewUp.z);
        
        if(mnAnimationCameraAttackedCnt >= 3)
        {
            //애니메이션폭탄 애니메이션은 한번만 한다.
            mfAnimationCameraAttacked = 0.0f;
            mnAnimationCameraAttackedCnt = 0;
        }
        else
            mnAnimationCameraAttackedCnt ++;
    }
	return E_SUCCESS;
}


int CHWorld::RenderBegin()
{
	int nTime = 0;
    
	//초기화가 안되었거나 초기화중이다.
	if( (mnStatus & 0x0002) != 0x0002)
		return E_SUCCESS;
    
    mpThread->BeginPong();
    
    
	nTime = GetClockDeltaConst();
    
    mpArrowMotionSprite->RenderBegin(nTime);
    mpStick->RenderBegin(nTime);
    if(!IsPauseTime())
        m_pSGLCore->RenderBegin(nTime);
	mpCurrRuntimeRenderObjects = (RuntimeRenderObjects*)mpThreadQueue->Poll();
    if(mpCurrRuntimeRenderObjects == 0)
    {
        mpThread->EndPong();
        return E_SUCCESS;
    }
    
	if(mpWaitFindMultiplayCursor)
    {
        if(GetMutiplyProtocol()->GetMultiplayState() >= (int)SYNC_PLYERT_STATE) //누군가와 매칭되었다면 검색웨이팅을 보여주지 말자.
        {
            mpRightTools->Remove(mpWaitFindMultiplayCursor);
            mpWaitFindMultiplayCursor = NULL;
        }
        else
            mpWaitFindMultiplayCursor->RenderBegin(nTime);
    }
    
    if(!IsPauseTime())
    {
        //1> 폭탄을 그린다.
        ByNode<CBomb>* bb = mpCurrRuntimeRenderObjects->plistBomb->begin();
        for (ByNode<CBomb>* it = bb; it != mpCurrRuntimeRenderObjects->plistBomb->end(); )
        {
            CBomb* Object = it->GetObject();
            Object->RenderBegin(nTime);
            it = it->GetNextNode();
        }
        
        //2> 스프라이트를 그린다.
        ByNode<CSprite>* sb =  mpCurrRuntimeRenderObjects->plistSprite->begin();
        for(ByNode<CSprite>*it = sb; it != mpCurrRuntimeRenderObjects->plistSprite->end();)
        {
            CSprite* Object = it->GetObject();
            Object->RenderBegin(nTime);
            it = it->GetNextNode();
        }
        
        //3> 파티클
        ByNode<AParticle>* ab = mpCurrRuntimeRenderObjects->plistParticle->begin();
        for(ByNode<AParticle>* it = ab; it != mpCurrRuntimeRenderObjects->plistParticle->end();)
        {
            AParticle* Object = it->GetObject();
            Object->RenderBegin(nTime);
            it = it->GetNextNode();
        }
        
        
        mpItemSlotBar->RenderBegin(nTime);
        if(mpFrameBar) mpFrameBar->RenderBegin(nTime);
        if(mpBuyDlg)   mpBuyDlg->RenderBegin(nTime);
        
    }
    else
    {
        mpItemSlotBar->RenderBegin(mEscapeT); //상점에서 아이템을 샀을 경우 흔들어 준다.
        if(mpFrameBar) mpFrameBar->RenderBegin(mEscapeT);
        if(mpBuyDlg)   mpBuyDlg->RenderBegin(mEscapeT);
        
        
        
    }
    
    
    
    if(mpSelectedObjectDescBar->GetHide() == false && GetGGTime() > mnSelectedObjectDescBarVisibleTime)
    {
        mpSelectedObjectDescBar->SetHide(true);
        mnSelectedObjectDescBarVisibleTime = 0;
    }
    
    if(mBackground)
        mBackground->RenderBegin(nTime);
    
    
    mpRadaBoard->SetObjects(mpCurrRuntimeRenderObjects->plistBoardObjects);
    mpRadaBoard->RenderBegin(nTime);
    
    
    
    
    mpLeftTools->RenderBegin(nTime);
    mpParticleMan->RenderBegin(nTime);
    mpGoldBar->RenderBegin(nTime);
    
    //원하는 위치로 화면을 회전해준다.
    RotationCarmeraToPoint(nTime);
    
    
    
    
    //시간이 포즈가 되어도 실행되게끔 해주어야 한다.
    if(mpMissionMsgCtl)
        mpMissionMsgCtl->RenderBegin(50);
    
    //------------------------------------------------
    //애니메이션을 보여준다.
    if(mpEnemyTools && mpEnemyTools->Animating() != CONTROLANI_NONE)
        mpEnemyTools->RenderBegin(nTime);
    //------------------------------------------------
	
    mpThread->EndPong();
	
	return E_SUCCESS;
}

#ifdef ANDROID
void CHWorld::SetLight()
{
    
    const GLfloat			lightAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
	const GLfloat			lightDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
	const GLfloat			lightPosition[] = {1.0f, 1.0f, 0.0f, 0.0f};
    const GLfloat			lightSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    
	//Configure OpenGL lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
}

void CHWorld::ResetTexture()
{
    CWorld::ResetTexture();
    
    HLogA("CHWorld::ResetTexture Start");
    mpRightTools->ResetTexture();
    m_pSGLCore->ResetTexture();
    
    if(mpConfirmMsgCtl) mpConfirmMsgCtl->ResetTexture();
    if(mpMissionMsgCtl) mpMissionMsgCtl->ResetTexture();
    if(mpPauseMsgCtl) mpPauseMsgCtl->ResetTexture();
    if(mpShopBtn) mpShopBtn->ResetTexture();
    
    
    mpItemSlotBar->ResetTexture();
    
    mpRadaBoard->ResetTexture();
    
    if(mBackground)
    {
        mBackground->ResetTexture();
    }
    mpStick->ResetTexture();
    
    mpLeftTools->ResetTexture();
    
    mpGoldBar->ResetTexture();
    
    if(mpFrameBarTexureMan) mpFrameBarTexureMan->Reset();
    if(mpFrameBar) mpFrameBar->ResetTexture();
    if(mpBuyDlg) mpBuyDlg->ResetTexture();
    
    //End 2D controls -------------------------
    CFireParticle::g_FireTextureID = 0;
    CFireParticle::FireParticleInitialize("Fire.tga");
    CCompatFireParticle::CompatFireParticleInitialize("Fire.tga");
    CET2AttackParticle::g_CET2TextureID = 0;
    CET2AttackParticle::ET2AttackParticleInitialize("sawtooth.tga");
    CImpactFlyParticle::g_CompatFireTextureID = 0;
    CImpactFlyParticle::ImpactFlyParticleInitialize(SGL_2D_FILENAME_TARGET);
	//-----------------------------------------------------
    CTE1BoosterParticle::Initialize_ToCTE1Booster(this);
    HLogA("CHWorld::ResetTexture End");
}
#endif


int CHWorld::Render()
{
    //버퍼를 클리어 해준다.
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_ALPHA_TEST); //속도가 높아진다는 소리가 있어서 한번해봤다.
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
    
#ifdef ANDROID //Android에서는 매번 셋팅해주어야 한다.
    //방향벡터의 normalize를 하지 않음. 그래서 gl에서 해주어야 샤이닝이 이러난다.
    //모델의 빛 반사를 정의 하려면 GL_NORMALIZE화 해주어야 한다.
    glEnable(GL_NORMALIZE);
#endif
    glColor4f(1.0f,1.0f,1.0f,1.0f);	 //glColor4f(1.0f,1.0f,1.0f,alpha);
	
	int nCameraType = 0;
	//초기화가 안되었거나 초기화중이다.
	if( (mnStatus & 0x0002) != 0x0002 || mpCurrRuntimeRenderObjects == 0) return E_SUCCESS;
    
    
    nCameraType = GetCameraType();
    
    //---------------------------------------------------
    //안개
    if(m_pSGLCore->m_fFogDentity != 0.0f)
    {
        //        GLfloat fcolor[4] = {0.7f, 0.7f, 0.7f, 1.0f};
        glEnable(GL_FOG);
#ifdef MAC
        glFogi(GL_FOG_MODE, GL_LINEAR);
#else
        glFogx(GL_FOG_MODE, GL_LINEAR);
#endif
        glFogf(GL_FOG_START, m_pSGLCore->m_fFogStart);//150.f);
        glFogf(GL_FOG_END, m_pSGLCore->m_fFogEnd);//350.f);
        glFogf(GL_FOG_DENSITY,m_pSGLCore->m_fFogDentity);//2.f);
        glFogfv(GL_FOG_COLOR, m_pSGLCore->m_fFogColor);
    }
    //---------------------------------------------------
    
    //하늘에서 내려다 보는 것 아닌 것은 하늘을 그린다.
	if(nCameraType < CAMERT_UP_VISIBLE)
    {
        if(mBackground)
        {
            mBackground->ActivateCamera(GetCamera());
            mBackground->Render();
        }
    }
    
	ActivateCamera();
    
    //1>그리는 순서를 캐릭터를 먼져그리고, 지형을 나중에 그린다.
    //그렇게 하면 나무같은 곳에 숨겨지기때문에 현실감이 있다.
    //스프라이트를 그린다.
#ifdef ANDROID
    SetLight();
#endif
    
    
    
    GLfloat ambient[4] = {0.3f, 0.3f, 0.3f, 1.0f}; //전체적인 밝기
    GLfloat diffuse[4] = {0.8f, 0.8f, 0.8f, 1.0f}; //빛의 산란
    GLfloat specular[4] = {0.8f, 0.8f, 0.8f, 1.0f}; //스포트 라이트
    //    GLfloat emission[4] = {0.1f, 0.1f, 0.1f, 1.0f}; //자체발광 : 0
    GLfloat shininess = 1; //0(가장 반짝이게)~128(반짝이지 않게)값이된다.
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    //    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
    
    ByNode<CSprite>* sb =  mpCurrRuntimeRenderObjects->plistSprite->begin();
    for(ByNode<CSprite>*it = sb; it != mpCurrRuntimeRenderObjects->plistSprite->end();)
	{
        CSprite* Object = it->GetObject();
		Object->Render();
        it = it->GetNextNode();
	}
    
    mpArrowMotionSprite->Render();
    
    //2>지형을 그랜다.
    m_pSGLCore->Render();
    
    
    //----------------------------------------
    //3> 폭탄을 그린다. 지형에 숨겨지지 않게 그리면 폭탄의 계적이 보여서 좋다.
    //순서의 반대로 그려준다.
    ByNode<CBomb>* be = mpCurrRuntimeRenderObjects->plistBomb->end()->GetPrevNode();
	for (ByNode<CBomb>* it = be; it != &mpCurrRuntimeRenderObjects->plistBomb->GetHead(); )
    {
		CBomb* Object = it->GetObject();
		Object->Render();
        it = it->GetPrevNode();
	}
	
    
    //-------------------------------
    glEnableClientState(GL_VERTEX_ARRAY);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glDepthMask(GL_FALSE);
    
    //Bomb Tracking..
    mpParticleMan->Render();
    
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnable(GL_TEXTURE_2D);
    
    
    mpStick->RenderMoveingFly();
    
    if (!IsPauseTime())
    {
        //3> 파티클
        ByNode<AParticle>* ab = mpCurrRuntimeRenderObjects->plistParticle->begin();
        for(ByNode<AParticle>* it = ab; it != mpCurrRuntimeRenderObjects->plistParticle->end();)
        {
            AParticle* Object = it->GetObject();
            Object->Render();
            it = it->GetNextNode();
        }
    }
    
    if(m_pSGLCore->m_fFogDentity != 0.0f)
        glDisable(GL_FOG);
    
    //2D는 뎁스를 끄고 그려준다.
    glDisable(GL_DEPTH_TEST);
    
    
    //-----------------------------------------------------------------------------
    //4> 스프라이트 에너지 상태를 나타낸다...
    glDisable(GL_TEXTURE_2D);
    glEnableClientState(GL_COLOR_ARRAY);
    for(ByNode<CSprite>*it = sb; it != mpCurrRuntimeRenderObjects->plistSprite->end();)
	{
        CSprite* Object = it->GetObject();
		if(Object->GetState() == SPRITE_RUN && Object->GetSpriteStatus())
            Object->GetSpriteStatus()->Render();
        it = it->GetNextNode();
	}
    glDisableClientState(GL_COLOR_ARRAY);
    glEnable(GL_TEXTURE_2D);
    //-----------------------------------------------------------------------------
    
    
    CControl::ActivateCamera();
    mpStick->Render();
    mpRadaBoard->Render();
    
    mpGoldBar->Render();
    mpLeftTools->Render();
    mpItemSlotBar->Render();
    mpShopBtn->Render();
    
    
    //    mpScoreNumCtl->Render();
    mpRightTools->Render();
    
    
    if(mpFrameBar) mpFrameBar->Render();
    if(mpBuyDlg) mpBuyDlg->Render();
    
    
    if(mpConfirmMsgCtl) mpConfirmMsgCtl->Render();
    if(mpPauseMsgCtl) mpPauseMsgCtl->Render();
    if(mpMissionMsgCtl) mpMissionMsgCtl->Render();
    
    
#ifdef MAC
    RenderDesign();
#endif
    
    
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    
    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    //-------------------------------
    
	return E_SUCCESS;
}


int CHWorld::RenderEnd()
{
	
    //초기화가 안되었거나 초기화중이다.
	if( (mnStatus & 0x0002) != 0x0002 || mpCurrRuntimeRenderObjects == 0)
		return E_SUCCESS;
    
    mpArrowMotionSprite->RenderEnd();
    
    //1> 폭탄을 그린다.
    ByNode<CBomb>* bb = mpCurrRuntimeRenderObjects->plistBomb->begin();
	for (ByNode<CBomb>* it = bb; it != mpCurrRuntimeRenderObjects->plistBomb->end(); )
    {
		CBomb* Object = it->GetObject();
		Object->RenderEnd();
        it = it->GetNextNode();
	}
	
    //2> 스프라이트를 그린다.
    ByNode<CSprite>* sb =  mpCurrRuntimeRenderObjects->plistSprite->begin();
    for(ByNode<CSprite>*it = sb; it != mpCurrRuntimeRenderObjects->plistSprite->end();)
	{
        CSprite* Object = it->GetObject();
		Object->RenderEnd();
        it = it->GetNextNode();
	}
    
    //3> 파티클
    ByNode<AParticle>* ab = mpCurrRuntimeRenderObjects->plistParticle->begin();
	for(ByNode<AParticle>* it = ab; it != mpCurrRuntimeRenderObjects->plistParticle->end();)
	{
		AParticle* Object = it->GetObject();
		Object->RenderEnd();
        it = it->GetNextNode();
	}
    
    //4>레이다 객체들..
    ByNode<RadaBoardObject>* rb = mpCurrRuntimeRenderObjects->plistBoardObjects->begin();
    for(ByNode<RadaBoardObject>* it = rb; it != mpCurrRuntimeRenderObjects->plistBoardObjects->end();)
    {
        RadaBoardObject* Object =  it->GetObject();
        delete[] Object->pWorldVertex;
        delete Object;
        it = it->GetNextNode();
    }
    
    if(mpFrameBar) mpFrameBar->RenderEnd();
    if(mpBuyDlg) mpBuyDlg->RenderEnd();
    
#ifdef ANDROID
    if(mbNeedDelFrameBar)
    {
        if(mpFrameBar){
            delete mpFrameBar;
            mpFrameBar = NULL;
        }
        if(mpFrameBarTexureMan)
        {
            delete mpFrameBarTexureMan;
            mpFrameBarTexureMan = NULL;
        }
        mbNeedDelFrameBar = false;
    }
#endif
    
    mpGoldBar->RenderEnd();
    mpLeftTools->RenderEnd();
    mpItemSlotBar->RenderEnd();
    
    
	if(mpCurrRuntimeRenderObjects)
	{
		delete mpCurrRuntimeRenderObjects->plistBomb;
		delete mpCurrRuntimeRenderObjects->plistSprite;
        delete mpCurrRuntimeRenderObjects->plistBoardObjects;
        delete mpCurrRuntimeRenderObjects->plistParticle;
		delete mpCurrRuntimeRenderObjects;
		mpCurrRuntimeRenderObjects = NULL;
	}
    m_pSGLCore->RenderEnd();
    
    
    //    if(mpStick)
    //    {
    //        if(mpStick->IsPoUpDown())
    //        {
    //            SGLEvent Event;
    //            Event.nMsgID = GSL_PO_STOP_UPDOWN;
    //            Event.lParam = 0;
    //            Event.lParam2 = 0;
    //            OnEvent(&Event);
    //            mpStick->SetPoUpDown(false);
    //        }
    //    }
	return E_SUCCESS;
}

bool CHWorld::IsViewRotationByTouch()
{
    return mpStick->GetStickType() & STIKCKTYPE_ROTATIONVIEW;
}

void CHWorld::SetActor(CSprite* pSprite)
{
	float fViewFar = 0.0f;
	
    if(pSprite == NULL)
    {
        m_pSGLCore->SetActor(NULL);
        return;
    }
    
    //CAMERA를 새로 정의 한다.
    m_pSGLCore->SetActor(pSprite);
	
	//각 Actor별로 카메라 원거리가 다르다.
	fViewFar = mCamera.ViewFar;
	
	//땅의 관점을 변경한다
	pSprite->SetActor((void*)&mCamera);
	
	//현재 카메라 타입에 따라서 카메라 정보를 재조정해준다.
	pSprite->SetCameraType(pSprite->GetCameraType());
    
    pSprite->SetIntervalToCamera(0.f);
	
	//각 스프라이트는 내부에서 카메라의 원거리가 변경될수 있다.
	if(fViewFar != mCamera.ViewFar) //카메라의 원거리가 변경되었다면
	{
		//프리즘을 반드시 재설정을 해야한다.
		ResetFrustum(true);
	}
    
    if (pImpactParticle)
    {
        delete pImpactParticle;
        pImpactParticle = NULL;
    }
    
    //탄착점 깃발을 추가한다. (파티클로 한이유는 투명하기 때문이다.)
    pImpactParticle = new CImpactFlyParticle(this);
    pImpactParticle->Initialize(NULL, NULL);
}


void CHWorld::SetCameraType(int v)
{
    CSprite* pActor = m_pSGLCore->GetActor();
	if(pActor)
	{
		pActor->SetCameraType(v);
		ResetFrustum(true);
	}
}


int CHWorld::GetCameraType()
{
    CSprite* pActor = m_pSGLCore->GetActor();
	if(pActor)
	{
		return pActor->GetCameraType();
	}
	
	return 0;
}


int CHWorld::GetPositionY(SPoint* pNow)
{
	int nResult = 0;
    nResult = m_pSGLCore->GetPositionY(pNow);
	return nResult;
}


int CHWorld::GetMovePosiztionY(SPoint* pNow)
{
    int nResult = 0;
    nResult = m_pSGLCore->GetPositionY(pNow,true);
    
    
    if(nResult == 0)
        pNow->y+=SPRITE_GROUND_HEIGHT; //너무 땅에 딱 붙어 있으면 땅속으로 드러가는 부분이 생긴다.
    return nResult;
}

void CHWorld::ShowMissionEndMessage(MissionEndCtlType type,int nScore,int nBonus,bool bTraining)
{
    if(mpMissionMsgCtl) delete mpMissionMsgCtl;
    mpMissionMsgCtl = new CMissionEndCtl(GetGoldPerScore(),m_pSGLCore->GetTextureMan(),mpScenario->GetUserInfo());
    mpMissionMsgCtl->Initialize();
    mpMissionMsgCtl->Show(type,nScore,nBonus,bTraining);
}

void CHWorld::ChangeBombDisplay(USERINFO* userinfo)
{
    if(userinfo == NULL) return ;
    CK9Sprite *pSP = dynamic_cast<CK9Sprite*>(GetActor());
    if(pSP)
    {
        pSP->SetCurrentSelMissaileInfo(userinfo);
        CMutiplyProtocol* pro = mpScenario->GetMutiplyProtocol();
        pro->SendSyncChandeBomb(pSP->GetID(), userinfo->nID);
    }
}

void CHWorld::ChangeBombDisplay(CSprite* pSprite,int nBombID)
{
    CK9Sprite *pSP = dynamic_cast<CK9Sprite*>(pSprite);
    if(pSP)
        pSP->SetCurrentSelMissaileInfo(nBombID);
}

void CHWorld::TransferScoreToGold()
{
    const float fTransUnit = 10.f;
    float fGoldUnit;
    CUserInfo* pUserInfo = GetUserInfo();
    if(mfScore > fTransUnit)
    {
        mfScore -= fTransUnit;
        mpScoreNumCtl->SetNumber(mfScore);
        pUserInfo->AddTotalScore((int)fTransUnit);
        fGoldUnit = fTransUnit * GetGoldPerScore();
        float fGold = pUserInfo->GetGold() + fGoldUnit;
        pUserInfo->SetGold(fGold);
        ((CNumberCtl*)mpGoldBar->GetChild(0))->SetNumber((float)fGold);
        if(mpGoldBar->GetChild(0)->Animating() == CONTROLANI_NONE)
            mpGoldBar->GetChild(0)->SetAni(CONTROLANI_WIGWAG,0.3f);
    }
}

int CHWorld::OnEvent(SGLEvent *pEvent)
{
    
    if(pEvent->nMsgID == SGL_MESSAGE_AUTO_ATTACK_TARGET)
    {
        CSprite* pTargetSprite = NULL;
        //Target ID 를 가지고 객체를 가져온다.
        pTargetSprite = m_pSGLCore->FindSprite((int)pEvent->lParam);
        
        CK9Sprite *pK9 = dynamic_cast<CK9Sprite*>(GetActor());
        if(pK9 != NULL && pTargetSprite != NULL)
        {
            pK9->AimAutoToEnemy(pTargetSprite);
            
            //보낸다.
            GetScenario()->GetMutiplyProtocol()->SendSyncClickSpriteToByStick(pK9->GetID(),pEvent->lParam);
        }
        
    }
    else if(pEvent->nMsgID == SGL_MESSAGE_DIED_ACTOR)
    {
        ShowMissionEndMessage(MissionEndCtlType_GoToMenu_Died,mfScore,mnBonus);
    }
    else if(pEvent->nMsgID == SGL_MESSAGE_CHANGE_MENU)
    {
        if(gPlayOut) gPlayOut();
        GetMutiplyProtocol()->SetMultiplayState(SYNC_NONE);
        if(gHWorldEnd) gHWorldEnd();
        mpScenario->GoToWorld(NULL, CSCENARIO_MAINMENU);
    }
    else if(pEvent->nMsgID == SGL_MESSAGE_SUCCESS_MISSION)
    {
        int nMoreBonus = 0;
        CUserInfo* pUserInfo = GetUserInfo();
        if(pUserInfo->IsCompletedMap(pUserInfo->GetLastSelectedMapID()))
            nMoreBonus = mnBonus + 150;
        else
            nMoreBonus = mnBonus + 1000;
        
        pUserInfo->mbSucceedMap = true;
        //ShowMissionEndMessage(MissionEndCtlType_GoToMenu_Successed,mfScore,mnBonus + 1000);
        ShowMissionEndMessage(MissionEndCtlType_GoToMenu_Successed,mfScore,mnBonus + 1000);
        SetPauseTime(true); //시간을 멈춘다.
    }
    else if(pEvent->nMsgID == SGL_MOVE_TO_POINT_SIMPLE)
    {
        CSprite* pMoveActor = (CSprite*)pEvent->lObject;
        float ptTabPoint[3];
        if(m_pSGLCore->IsTabInOnlyTerrian(pEvent->lParam, pEvent->lParam2, ptTabPoint))
        {
            //Fire을 한다.
            SPoint ptEn;
            ptEn.x = ptTabPoint[0];
            ptEn.y = ptTabPoint[1];
            ptEn.z = ptTabPoint[2];
            
            
            pMoveActor->Command(AICOMMAND_MOVE_SIMPLE_POS, &ptEn); //간단하게 이동을 한다.
            mpArrowMotionSprite->SetPosition(&ptEn);
        }
    }
    else if(pEvent->nMsgID == SGL_MOVE_TO_MULTIPLAY)
    {
        CSprite* pMoveActor = (CSprite*)pEvent->lObject;
        SPoint* ptTabPoint = (SPoint*)pEvent->lParam;
        pMoveActor->Command(AICOMMAND_MOVE_MULTIPLAY, pEvent);
        delete[] ptTabPoint;
    }
    
    else if(pEvent->nMsgID == SGL_MOVE_TO_FIGHTER_MULTIPLAY)
    {
        CSprite* pMoveActor = (CSprite*)pEvent->lObject;
        if(pMoveActor->GetState() == SPRITE_RUN)
            pMoveActor->Command(AICOMMAND_MOVE_FIGHTER_MULTIPLAY, pEvent);
    }
    else if(pEvent->nMsgID == SGL_CLICK_TO_POINT_MULTIPLAY) //상대방이 땅을 클릭했을 경우
    {
        SPoint* ptTabPoint = (SPoint*)pEvent->lParam;
        CSprite* pOwner = (CSprite*)pEvent->lObject;
        SGLEvent Event;
        Event.lParam = 0;
        Event.lParam2 = 0;
        Event.nMsgID = GSL_DONT_AUTO_FIRE;
        Event.lObject = (long)pOwner;
        OnEvent(&Event);
        if(pOwner && pOwner->GetState() == SPRITE_RUN)
            pOwner->AimAutoToEnemy(ptTabPoint);
        
        delete[] ptTabPoint;
    }
    else if(pEvent->nMsgID == SGL_CLICK_TO_SPRITE_MULTIPLAY) //상대방이 객체를 클릭했을 경우
    {
        
        CSprite* pAttackTo = (CSprite*)pEvent->lParam;
        CSprite* pOwner = (CSprite*)pEvent->lObject;
        SGLEvent Event;
        Event.lParam = 0;
        Event.lParam2 = 0;
        Event.nMsgID = GSL_DONT_AUTO_FIRE;
        Event.lObject = (long)pOwner;
        OnEvent(&Event);
        if(pOwner && pOwner->GetState() == SPRITE_RUN && pAttackTo && pAttackTo->GetState() == SPRITE_RUN)
            pOwner->AimAutoToEnemy(pAttackTo);
        
    }
    else if(pEvent->nMsgID == SGL_CLICKED_NEW_BOMB_MULTIPLAY) //폭탄을 발사한다.
    {
        
        MutiplyProtocol* pro = (MutiplyProtocol*)pEvent->lParam;
        CSprite* pOwner = (CSprite*)pEvent->lObject;
        CArchive ar(pro->pData,pro->nDataSize);
        
        SGLEvent Event;
        Event.lParam = 0;
        Event.lParam2 = 0;
        Event.nMsgID = GSL_DONT_AUTO_FIRE;
        Event.lObject = (long)pOwner;
        OnEvent(&Event);
        
        int nOwnerID;
        SPoint ptNow;
        SVector vtDir,vDirAngle;
        ar >> nOwnerID;
        ar >> ptNow;
        ar >> vtDir;
        ar >> vDirAngle;
        ((CK9Sprite*)pOwner)->NewMissile(ptNow,vtDir,vDirAngle,false);
        delete[] pro->pData;
        delete pro;
        
    }
    else if(pEvent->nMsgID == SGL_GET_ITEM)
    {
        //아이템을 습득하였다.
        PROPERTY_RUNTIMEITEM* ItemInfo = (PROPERTY_RUNTIMEITEM*)pEvent->lParam;
        if(ItemInfo->nType == 0) //bomb
        {
            GetSGLCore()->PlaySystemSound(SOUND_ID_EnergyUP);
            CSprite* pActor = GetActor();
            CUserInfo* pUserInfo = GetUserInfo();
            pActor->TakeRunTimeBomb(ItemInfo->nItemID,ItemInfo->nCount);
            
            //무기슬롯으로 변경하고 무기가 변경된 것을 보여주어야 한다.
            pUserInfo->SetLastBombItemSwitch(0);
            int nPage = VisibleDisplayItemSlotPage(ItemInfo->nItemID);
            pUserInfo->SetLastSelectedBombPageIndex(nPage);
            ResetDisplayItemSlot();
            AnimationDisplayItemSlot(ItemInfo->nItemID);
        }
        else //Item
        {
            if(ItemInfo->nItemID == ITEMID_RUNTIME_FILLENERGY)
            {
                CSprite* pActor = GetActor();
                float fMax = pActor->GetMaxDefendPower();
                float fCur = pActor->GetCurrentDefendPower();
                fCur += fMax / 3.f;
                if(fCur > fMax)
                    fCur = fMax;
                pActor->SetCurrentDefendPower(fCur);
                GetSGLCore()->PlaySystemSound(SOUND_ID_EnergyUP);
                
                if(pActor->GetNetworkTeamType() == NETWORK_MYSIDE)
                    GetMutiplyProtocol()->SendSyncCurrentEnergy(pActor);
            }
            else if(ItemInfo->nItemID == ITEMID_RUNTIME_PLUSBONUS || ItemInfo->nItemID == ITEMID_RUNTIME_PLUSBONUS2)
            {
                GetSGLCore()->PlaySystemSound(SOUND_ID_EnergyUP);
                //mnBonus += ItemInfo->nCount;
                CUserInfo* pUserInfo = GetUserInfo();
                if(pUserInfo)
                    pUserInfo->SetGold(pUserInfo->GetGold() + ItemInfo->nCount);
                ((CNumberCtl*)mpGoldBar->GetChild(0))->SetNumber(pUserInfo->GetGold());
                mpGoldBar->GetChild(0)->SetAni(CONTROLANI_WIGWAG,0.3f);
            }
            else if(ItemInfo->nItemID == ITEMID_RUNTIME_BUBBLE)
            {
                GetSGLCore()->PlaySystemSound(SOUND_ID_EnergyUP);
            }
            else if(ItemInfo->nItemID >= ITEMID_RUNTIME_L2 && ItemInfo->nItemID <= ITEMID_RUNTIME_L4)
            {
                //아이템에 거시기를 넣어준다.
                GetSGLCore()->PlaySystemSound(SOUND_ID_EnergyUP);
                
                CUserInfo* pUserInfo = GetUserInfo();
                //Item추가함.
                pUserInfo->AddListToItem(ItemInfo->nItemID);
                //무기슬롯으로 변경하고 아이템이 추가되는 것을 보여준다.
                pUserInfo->SetLastBombItemSwitch(1);
                int nPage = VisibleDisplayItemSlotPage(ItemInfo->nItemID);
                pUserInfo->SetLastSelectedBombPageIndex(nPage);
                ResetDisplayItemSlot();
                AnimationDisplayItemSlot(ItemInfo->nItemID);
            }
        }
        
    }
    else if(pEvent->nMsgID == SGL_CHANGE_BOM_SEL_MULTIPLAY) //멀티 플레이 무기를 바꾸어준다.
    {
        ChangeBombDisplay((CSprite*)pEvent->lObject,(int)pEvent->lParam);
    }
    else if(pEvent->nMsgID == SGL_MESSAGE_CLICKED)
    {
        if(pEvent->lParam == BTN_PLAY_HOME)
        {
            if(!mpConfirmMsgCtl)
            {
                mpConfirmMsgCtl = new CMessageCtl(m_pSGLCore->GetTextureMan(),CMessageCtlType_GoToMenu);
                mpConfirmMsgCtl->Initialize();
                mpConfirmMsgCtl->Show();
                SetPauseTime(true); //시간을 멈춘다.
            }
        }
        else if(pEvent->lParam == BTN_PLAY_SHOP)
        {
            ShowFrameTable();
        }
        else if(pEvent->lParam == BTN_PLAY_CONFIRM_YES)
        {
            CMessageCtlType type = mpConfirmMsgCtl->GetType();
            if(mpConfirmMsgCtl)
            {
                delete mpConfirmMsgCtl;
                mpConfirmMsgCtl = NULL;
            }
            
            if(type == CMessageCtlType_Request_Aliance)
            {
                CMutiplyProtocol* pMul = GetMutiplyProtocol();
                pMul->SendResponseAliance(true);
                SetAllians(pMul->GetTeamID(), pMul->GetEnemyTeamID());
                CControl* pBtnAlias = mpEnemyTools->FindControl(BTN_MULTIPLAY_ALIAS);
                pBtnAlias->SetBackCoordTex(0.f,0.f,0.921875f,0.4609375f);
            }
            else
                ShowMissionEndMessage(MissionEndCtlType_GoToMenu_Died,mfScore,mnBonus);
        }
        
        else if(pEvent->lParam == BTN_PLAY_GoTo_Menu)
        {
            mpThread->Stop(); //쓰레드를 멈추도록 요청을 한다.
            //여기서 바로 메인 시나리오로 변경하면 데드락 걸린다.
            //쓰레드는 아직 멈추지 않음.. 그래서 나중에 지울때 락걸린다.
            CScenario::SendMessage(SGL_MESSAGE_CHANGE_MENU);
            SetPauseTime(false); //시간을 흘러가게 한다.
        }
        else if(pEvent->lParam == BTN_PLAY_CONFIRM_Succ_YES)
        {
            //ShowMissionEndMessage(MissionEndCtlType_GoToMenu_Successed,mfScore,mnBonus + 1000);
        }
        else if(pEvent->lParam == BTN_PLAY_CONFIRM_NO)
        {
            CMessageCtlType type = mpConfirmMsgCtl->GetType();
            if(mpConfirmMsgCtl)
            {
                delete mpConfirmMsgCtl;
                mpConfirmMsgCtl = NULL;
            }
            //            mpConfirmMsgCtl->Hide();
            if(type == CMessageCtlType_Request_Aliance)
            {
                CMutiplyProtocol* pMul = GetMutiplyProtocol();
                pMul->SendResponseAliance(false);
            }
            else
                SetPauseTime(false); //시간을 흘러가게 한다.
        }
        else if(pEvent->lParam == BTN_PLAY_CONFIRM_Retry) //미션을 다시 시작한다.
        {
            mRequestTry = 1; //리케스트를 요청한다.
        }
        else if(pEvent->lParam == BTN_PLAY_SETTINGS)
        {
            HLog("Clicked BTN_PLAY_SETTINGS\n");
        }
        else if(pEvent->lParam == BTN_PLAY_CAMERA)
        {
            SGLEvent Event;
            Event.nMsgID = GSL_CHANGE_VIEWTYPE;
            Event.lParam = 0;
            Event.lParam2 = 0;
            OnEvent(&Event);
        }
        
        else if(pEvent->lParam == BTN_PLAY_BOM_SEL) //무기를 바꾸어준다.
        {
            CBombCell* pCtl = (CBombCell*)pEvent->lObject;
            USERINFO* userinfo = (USERINFO*)pCtl->GetLData();
            ChangeBombDisplay(userinfo);
        }
        else if(pEvent->lParam == BTN_ITEMSLOTUPPAGE) //아이템 슬롯 다음 페이지
        {
            int nMaxPageCnt = 0;
            const int nMaxItem = MAXSLOTCNT;
            vector<CControl*> lstSlotCtl;
            CUserInfo* pUserInfo = mpScenario->GetUserInfo();
            
            int nType = pUserInfo->GetLastBombItemSwitch();
            vector<USERINFO*>* lstItem = NULL;
            if(nType == 1)
                lstItem = pUserInfo->GetListItem();
            else if(nType == 0)
                lstItem = pUserInfo->GetListBomb();
            else
                lstItem = pUserInfo->GetListBlz();
            
            
            int nSelIndex = 1;
            if(nType == 1)
                nSelIndex = pUserInfo->GetLastSelectedItemPageIndex() + 1;
            else if(nType == 0)
                nSelIndex = pUserInfo->GetLastSelectedBombPageIndex() + 1;
            else
                nSelIndex = pUserInfo->GetLastSelectedBlzPageIndex() + 1;
            
            
            nMaxPageCnt = (int)lstItem->size() / nMaxItem;
            if((lstItem->size() % nMaxItem) != 0)
                nMaxPageCnt += 1;
            
            if(nSelIndex <= nMaxPageCnt)
            {
                if(nType == 1)
                    pUserInfo->SetLastSelectedItemPageIndex(nSelIndex);
                else if(nType == 0)
                    pUserInfo->SetLastSelectedBombPageIndex(nSelIndex);
                else
                    pUserInfo->SetLastSelectedBlzPageIndex(nSelIndex);
                
                ResetDisplayItemSlot();
                pUserInfo->Save();
            }
        }
        else if(pEvent->lParam == BTN_ITEMSLOTDOWNPAGE) //아이템 슬롯 이전 페이지.
        {
            vector<CControl*> lstSlotCtl;
            CUserInfo* pUserInfo = mpScenario->GetUserInfo();
            int nType = pUserInfo->GetLastBombItemSwitch();
            
            int nSelIndex = 1;
            if(nType == 1)
                nSelIndex = pUserInfo->GetLastSelectedItemPageIndex() - 1;
            else if(nType == 0)
                nSelIndex = pUserInfo->GetLastSelectedBombPageIndex() - 1;
            else
                nSelIndex = pUserInfo->GetLastSelectedBlzPageIndex() - 1;
            
            if( nSelIndex >= 1)
            {
                if(nType == 1)
                    pUserInfo->SetLastSelectedItemPageIndex(nSelIndex);
                else if(nType == 0)
                    pUserInfo->SetLastSelectedBombPageIndex(nSelIndex);
                else
                    pUserInfo->SetLastSelectedBlzPageIndex(nSelIndex);
                
                ResetDisplayItemSlot();
                pUserInfo->Save();
            }
        }
        else if(pEvent->lParam == CTL_TABLE_ITEM_SLOT3 ||
                pEvent->lParam == CTL_TABLE_ITEM_SLOT4 ||
                pEvent->lParam == CTL_TABLE_ITEM_SLOT5 ||
                pEvent->lParam ==CTL_TABLE_ITEM_SLOT6 )
        {
            CButtonCtl* pCtl = (CButtonCtl*)pEvent->lObject;
            USERINFO* userinfo = (USERINFO*)pCtl->GetLData();
            if(pCtl->GetLData() == -1) return E_SUCCESS;
            if(userinfo->nID == RUNTIME_UPDATE_REMOVE)
            {
                if(mpClickedSprite && mpClickedSprite->GetState() == SPRITE_RUN)
                {
                    
                    GetMutiplyProtocol()->SendSyncDeleteObj(mpClickedSprite->GetID(),0,true);
                    mpClickedSprite->SetState(BOMB_COMPACT_ANI);
                    SetBlzSlotCount(mpClickedSprite->GetModelID(),true,mpClickedSprite);
                    
                }
                
                RUNTIME_UPGRADE prop;
                RUNTIME_UPGRADE::GetPropertyRunUpgradeItem(userinfo->nID , prop);
                OnSelectedObjectShowDescBar(prop.sDesc);
                
            }
            else if(userinfo->nID > RUNTIME_UPDATE_START &&
                    userinfo->nID < RUNTIME_UPDATE_END )
            {
                if(mpClickedSprite && mpClickedSprite->GetState() == SPRITE_RUN)
                {
                    if(mpClickedSprite->SetRuntimeUpgrade(userinfo->nID))
                    {
                        pCtl->SetEnable(false); //다시 클릭되지 않게 막아준다.
                        pCtl->GetChild(1)->SetHide(false); //
                    }
                }
                SetUpgradeDescSlot(userinfo->nID);
            }
            else
            {
                CUserInfo* pUserInfo = mpScenario->GetUserInfo();
                int nType = pUserInfo->GetLastBombItemSwitch();
                if(nType == 0) //0:Bomb 1:Item 2:Blz
                {
                    mpSelectedWeaponBtn = pCtl;
                    pCtl->ArrangeToggleButton();
                    ChangeBombDisplay(userinfo);
                    SetBombDescSlot(userinfo->nID);
                }
                else if(nType == 1)
                    SetItemDescSlot(userinfo->nID);
                else if(nType == 2)
                    SetSpriteDescSlot(userinfo->nID);
            }
        }
        else if(pEvent->lParam == BTN_ITEMSLOTSWITCH)
        {
            CUserInfo* pUserInfo = mpScenario->GetUserInfo();
            
            int nType = pUserInfo->GetLastBombItemSwitch();
            if(nType == 0)
                pUserInfo->SetLastBombItemSwitch(1);
            else if(nType == 1)
                pUserInfo->SetLastBombItemSwitch(2);
            else if(nType == 2)
                pUserInfo->SetLastBombItemSwitch(0);
            
            
            ResetDisplayItemSlot();
            
        }
        else if(pEvent->lParam == BTN_PLAY_PAUSE)
        {
            vector<string> lstImage;
            vector<float> layout;
            lstImage.push_back("res_slot.png");
            lstImage.push_back("none");
            layout.clear();
            PROPERTYI::GetPropertyFloatList("W_PauseMsgCtl", layout);
            float fX = (gDisplayWidth - layout[2]) / 2.f;
            float fY = (gDisplayHeight - layout[3]) / 2.f;
            
            mpPauseMsgCtl = new CButtonCtl(NULL,m_pSGLCore->GetTextureMan());
            mpPauseMsgCtl->Initialize(BTN_PLAY_PAUSE_RESTART, fX, fY, layout[2], layout[3], lstImage, 0.5546875, 0.474609375, 0.984375, 0.90625);
            SetPauseTime(true); //시간을 멈춘다.
        }
        else if(pEvent->lParam == BTN_PLAY_PAUSE_RESTART)
        {
            if(mpPauseMsgCtl)
            {
                delete mpPauseMsgCtl;
                mpPauseMsgCtl = NULL;
            }
            SetPauseTime(false);
        }
        else if(pEvent->lParam == BTN_ITEMTAB)
        {
            CButtonCtl* pCtl = (CButtonCtl*)pEvent->lObject;
            pCtl->ArrangeToggleButton();
            
            mpFrameBombTable->SetHide(true);
            mpFrameItembTable->SetHide(false);
            GetSGLCore()->PlaySystemSound(SOUND_ID_Click);
        }
        else if(pEvent->lParam == BTN_BOMBTAB)
        {
            CButtonCtl* pCtl = (CButtonCtl*)pEvent->lObject;
            pCtl->ArrangeToggleButton();
            
            mpFrameBombTable->SetHide(false);
            mpFrameItembTable->SetHide(true);
            GetSGLCore()->PlaySystemSound(SOUND_ID_Click);
            
        }
        else if(pEvent->lParam == BTN_ITEMTABLE || pEvent->lParam == BTN_BOMBTABLE) //아이폰
        {
            int nType = (int)pEvent->lParam - BTN_BOMBTABLE;
            CControl* pCtn = (CControl*)pEvent->lObject;
            ShowBuyDlg((int)pCtn->GetLData(), nType);
            GetSGLCore()->PlaySystemSound(SOUND_ID_Click);
        }
        else if(pEvent->lParam == BTN_BOMBBUY || pEvent->lParam == BTN_ITEMBUY) //아이패드
        {
            int nType = (int)pEvent->lParam - BTN_BOMBBUY;
            CControl* pCtn = (CControl*)pEvent->lObject;
            ShowBuyDlg((int)pCtn->GetLData(), nType);
            GetSGLCore()->PlaySystemSound(SOUND_ID_Click);
        }
        else if(pEvent->lParam == BTN_BOMBBUYOK)
        {
            CControl* pCtn = (CControl*)pEvent->lObject;
            if(OnPurchasedBomb((int)pCtn->GetLData()))
            {
                if(mpBuyDlg) { delete mpBuyDlg; mpBuyDlg = NULL;}
            }
        }
        else if(pEvent->lParam == BTN_ITEMBUYOK)
        {
            CControl* pCtn = (CControl*)pEvent->lObject;
            if(OnPurchasedItem((int)pCtn->GetLData()))
            {
                if(mpBuyDlg) { delete mpBuyDlg; mpBuyDlg = NULL;}
            }
        }
        else if(pEvent->lParam == BTN_ITEMBOMBBUYCANCEL)
        {
            if(mpBuyDlg) { delete mpBuyDlg; mpBuyDlg = NULL;}
            GetSGLCore()->PlaySystemSound(SOUND_ID_Click);
        }
        else if(pEvent->lParam == BTN_MULTIPLAY_ALIAS)
        {
            CMutiplyProtocol* pMul = GetMutiplyProtocol();
            if(!pMul->IsReauestingAliance())
            {
                if(IsAllians(pMul->GetTeamID(), pMul->GetEnemyTeamID()))
                {
                    if(pMul->SendRequestAliance(false)) //동맹끊기로 요청한다.
                    {
                        CControl* pBtnAlias = mpEnemyTools->FindControl(BTN_MULTIPLAY_ALIAS);
                        pBtnAlias->SetBackCoordTex(0.f,0.4609375f,0.921875f,0.921875f);
                        SetNoneAllians(pMul->GetTeamID(),pMul->GetEnemyTeamID());
                    }
                }
                else
                    pMul->SendRequestAliance(true); //동맹을 요청한다.
            }
        }
    }
    else if(pEvent->nMsgID == SGL_MESSAGE_MAKE_SCORE) //
    {
        if(mpScenario)
        {
            mfScore += (float)pEvent->lParam * 0.001f;
            mpScoreNumCtl->SetNumber(mfScore);
        }
    }
    else if(pEvent->nMsgID == SGL_MESSAGE_FIRE_MISSILE)
    {
        int nCurrentBombID = GetActor()->GetCurrentWeaponID();
        if(pEvent->lParam == 0 && pEvent->lParam2 == 0) //Count = 0이되면
        {
            CUserInfo* pUserInfo = mpScenario->GetUserInfo();
            vector<USERINFO*>* list = pUserInfo->GetListBomb();
            //현재 거시기를 제거한다.
            vector<USERINFO*>::iterator it;
            vector<USERINFO*>::iterator b = list->begin();
            
            //Modified By Song 2013.04.07 수정...
            for (it = b; it != list->end(); it++)
            {
                USERINFO* u = *it;//list[i];
                //0값은 저장할때 지운다.
                if(u->nID == nCurrentBombID)
                {
                    //메모리상에 지운다.
                    list->erase(it);
                    break;
                }
            }
            
            //Default로 변경해준다.
            USERINFO* pDefaultUserInfo = (*list)[0];
            if(list->size() > 1) pDefaultUserInfo = (*list)[1];
            ChangeBombDisplay(pDefaultUserInfo);
            
            //무기슬롯으로 변경하고 무기가 변경된 것을 보여주어야 한다.
            pUserInfo->SetLastBombItemSwitch(0);
            int nPage = VisibleDisplayItemSlotPage(pDefaultUserInfo->nID);
            pUserInfo->SetLastSelectedBombPageIndex(nPage);
            ResetDisplayItemSlot();
            AnimationDisplayItemSlot(pDefaultUserInfo->nID);
            
            //            if(mpSelectedWeaponBtn)
            //                ResetDisplayItemSlot();
        }
        else if(mpSelectedWeaponBtn && pEvent->lParam == 0)
        {
            CNumberCtl* pNum = (CNumberCtl*) mpSelectedWeaponBtn->GetChild(0)->GetChild(0);
            pNum->SetNumber((int)pEvent->lParam2);
        }
    }
    else if(pEvent->nMsgID == SGL_PLAYSOUND)
    {
        GetSGLCore()->PlaySystemSound((int)pEvent->lParam);
    }
    else if(pEvent->nMsgID == SGL_CLEAR_TOURCH)
    {
        //비정상적이 작업으로 화면이 나갔다가 드러가면.
        if(mpStick)
        {
            if( (mnStatus & 0x0002) == 0x0002)
                mpStick->ClearTouch();
        }
    }
    else if(pEvent->nMsgID == SGL_MAKE_BLZ)
    {
        float* fpos = (float*)pEvent->lObject;
        SVector vDir;
        vDir.x = -1;
        vDir.y = 0;
        vDir.z = -1;
        sglNormalize(&vDir);
        CSprite* pBlz = CSGLCore::MakeSprite(GetNewID(), pEvent->lParam2, (int)pEvent->lParam, GetTextureMan(),m_pSGLCore->GetAction(), this,NETWORK_MYSIDE);
        pBlz->LoadResoure(GetModelMan());
        SPoint ptNow;
        ptNow.x = fpos[0];
        ptNow.y = fpos[1];
        ptNow.z = fpos[2];
        pBlz->Initialize(&ptNow, &vDir);
        m_pSGLCore->AddSprite(pBlz);
        GetMutiplyProtocol()->SendSyncMakeSprite(pBlz);
        if(fpos) delete[] fpos;
        SetBlzSlotCount((int)pEvent->lParam, false, NULL); //건물을 세운다.
    }
    else if(pEvent->nMsgID == SGL_MAKE_SPRITE_MULTIPLAY)
    {
        CArchive* pArc = (CArchive*)pEvent->lObject;
        
        int nDummy;
        int nID,nModelID;
        char cTeamID;
        *pArc >> nID;
        *pArc >> nModelID;
        *pArc >> cTeamID;
        
        
        SPoint ptNow;
        SVector vDir;
        vDir.x = -1;
        vDir.y = 0;
        vDir.z = 0;
        
        *pArc >> nDummy;
        
        *pArc >> ptNow.x;
        *pArc >> ptNow.y;
        *pArc >> ptNow.z;
        pArc->Begin();

        
        sglNormalize(&vDir);

        
        CSprite* pNewSPrite = CSGLCore::MakeSprite(nID, cTeamID, nModelID, GetTextureMan(),m_pSGLCore->GetAction(), this,NETWORK_OTHERSIDE);
        pNewSPrite->LoadResoure(GetModelMan());
        pNewSPrite->Initialize(&ptNow, &vDir);
        pNewSPrite->ArchiveMultiplay(SUBCMD_OBJ_ALL_INFO, *pArc, false);
        m_pSGLCore->AddSprite(pNewSPrite);
        delete pArc;
    }
    else if(pEvent->nMsgID == SGL_SELECTED_SPRITE)
    {
        SetClickedSprite((CSprite*)pEvent->lObject);
    }
    else if(pEvent->nMsgID == SGL_REALIVE_RESETSLOT)
    {
        CSprite* pActor = GetActor();
        if(GetUserInfo()->GetLastBombItemSwitch() == 1) // 0 : Bomb 1:Item 2:Blz 3:ClickedTower
            ResetDisplayItemSlot();
        
        if(pActor && pActor->GetState() == SPRITE_RUN)
        {
            int nLiveCnt = pActor->GetAliveCount();
            if(nLiveCnt == 0)
                mpAliveNumCtl->SetHide(true);
            else
                mpAliveNumCtl->SetNumber(nLiveCnt);
        }
        
    }
    else if(pEvent->nMsgID == SGL_COMPLETED_RUNTIME_UPGRADE)
    {
        CSprite* pUpSp = (CSprite*)pEvent->lParam;
        if(pUpSp && pUpSp->GetState() == SPRITE_RUN)
        {
            pUpSp->OnCompletedUpdated((RUNTIME_UPGRADE*)pEvent->lObject);
        }
    }
    else if(pEvent->nMsgID == SGL_COMPLETED_RUNTIME_UPGRADE_MULTIPLAY)
    {
        CSprite* pUpSp = (CSprite*)pEvent->lParam;
        if(pUpSp && pUpSp->GetState() == SPRITE_RUN)
            pUpSp->OnCompletedUpdated((RUNTIME_UPGRADE*)pEvent->lObject,(CArchive*)pEvent->lParam2);
    }
    else if(pEvent->nMsgID == SGL_SHOP_PRODUCT_GOLDTTYPE) //Gold Type의 Product를 샀다.
    {
        CUserInfo* pUserInfo = mpScenario->GetUserInfo();
        //---------------------------------------
        //Product을 적용해준다.
        //---------------------------------------
        if(pEvent->lParam == ID_ITEM_GOLD_4000 || pEvent->lParam == ID_ITEM_GOLD_9000) //골드타임
        {
            
            float fG = 4000.f;
            if(pEvent->lParam == ID_ITEM_GOLD_9000)
                fG = 9000.f;
            else if(pEvent->lParam == ID_ITEM_GOLD_4000)
                fG = 4000.f;
            
            pUserInfo->SetGold(fG + pUserInfo->GetGold());
            
            ResetDisplayTopGold();
        }
        else if(pEvent->lParam == ID_ITEM_ATTACKBYRADA || pEvent->lParam == ID_ITEM_REMOVE_AD || pEvent->lParam == ID_ITEM_UPGRADE_PACKAGE || pEvent->lParam == ID_TIEM_FASTBUILD || pEvent->lParam == ID_TIEM_FASTGOLD)
        {
            OnPurchasedProductItem((int)pEvent->lParam);
        }
        
        
        pUserInfo->SetBuy();
        pUserInfo->Save();
        GetSGLCore()->PlaySystemSound(SOUND_ID_EnergyUP);
        HLogD("Producted SGL_SHOP_PRODUCT_GOLDTTYPE ID %d\n",pEvent->lParam);
    }
    else if(pEvent->nMsgID == SGL_BUY_PRODUCT_GOLDTTYPE_FOR_ANDROID)
    {
#if defined(ANDROIDAPPSTOREKIT)
        extern char* strtolower(const char *s);
        const char* sData = sglReadCacheProduct();
        char sBundle[100];
        char* slogBundle;
        char* sTData;
        char* sTDataX;
        
        if(strlen(sData) == 0) return 0;
        
        sTDataX = sTData = new char[strlen(sData)+1];
        strcpy(sTData, sData);
        strcpy(sBundle, GetBundleID());
        slogBundle = strtolower(sBundle);
        
        char* sTok = strtok(sTData,"\n");
        while (sTok)
        {
            const char* sID = strstr(sTok, slogBundle);
            if(sID)
            {
                sID = sID + strlen(slogBundle) + 1;
                PROPERTY_ITEM item;
                PROPERTY_ITEM::GetPropertyItem(sID, item); //대문자 소문자 구분하지 않고 찾아옴
                CScenario::SendMessage(SGL_SHOP_PRODUCT_GOLDTTYPE,item.nID,item.nType,0,0);
            }
            sTok = strtok(NULL,"\n");
        }
        delete[] slogBundle;
        delete[] sTDataX;
        
#endif //ANDROIDAPPSTOREKIT
        
    }
    else if(pEvent->nMsgID == SGL_CHGOLD_IN_DMAINTOWER)
    {
        TransferScoreToGold();
    }
    else if(pEvent->nMsgID == SGL_KILLED_ASSITANCETANK)
    {
        CSprite* pActor = m_pSGLCore->GetActor();
        if(pActor)
        {
            //사용자에서 지워야할 아이디를 지워준다.
            CUserInfo* pUserInfo = mpScenario->GetUserInfo();
            pUserInfo->RemoveListToItem((int)pEvent->lParam);
            if(mpClickedSprite == NULL)
            {
                if(pUserInfo->GetLastBombItemSwitch() == 1)
                    ResetDisplayItemSlot();
            }
            pUserInfo->Save();
        }
    }
    else if(pEvent->nMsgID == SGL_RESULT_REVIEW) //Android에서는 죽는다.
    {
        return 0;
    }
    else if(pEvent->nMsgID == SGL_SYNC_MULTIPLAY)
    {
        MutiplyProtocol* pro = (MutiplyProtocol*)pEvent->lObject;
        MutiplyProtocol_Sub* pSub = (MutiplyProtocol_Sub*)pro->pData;
        for (int i = 0; i < pro->cSubCnt; i++)
        {
            CArchive* pNewArc = new CArchive(pSub[i].pData,pSub[i].nDataSize);
            SGLEvent event;
            event.nMsgID = SGL_MAKE_SPRITE_MULTIPLAY;
            event.lObject = (long)pNewArc;
            OnEvent(&event);
        }
        delete[] pro->pData;
        delete pro;
        
        
        if(mpEnemyTools->GetHide())
            ShowEnemyTools(GetMutiplyProtocol()->GetAliasEnemy(), GetMutiplyProtocol()->GetRankEnemy(), false);
        
    }
    else if(pEvent->nMsgID == SGL_SHOW_ENEMYTOOLS)
    {
        
        if(mpEnemyTools->GetHide() && pEvent->lParam == 1)
            ShowEnemyTools(GetMutiplyProtocol()->GetAliasEnemy(), GetMutiplyProtocol()->GetRankEnemy(), false);
        else if (!mpEnemyTools->GetHide() && pEvent->lParam == 0)
            HideEnemyTools();
    }

    else if(pEvent->nMsgID == SGL_MATCHING_MSG)
    {
        
    }
    else if(pEvent->nMsgID == SGL_CLICKED_ADMOB_ANDROID)
    {
        GetUserInfo()->SetClickedProp();
    }
    else if(pEvent->nMsgID == SGL_ALIANCE_MULTIPLAY)
    {
        CMutiplyProtocol* pMul = GetMutiplyProtocol();
        
        if(pEvent->lParam2 == 0)
        {
            //멀티플레이요청이 있었다.
            if(pEvent->lParam)
            {
                if(mpConfirmMsgCtl) delete mpConfirmMsgCtl;
                mpConfirmMsgCtl = new CMessageCtl(m_pSGLCore->GetTextureMan(),CMessageCtlType_Request_Aliance);
                mpConfirmMsgCtl->Initialize();
                mpConfirmMsgCtl->Show();
            }
            else //끊기
            {
                SetNoneAllians(pMul->GetTeamID(),pMul->GetEnemyTeamID());
                CControl* pBtnAlias = mpEnemyTools->FindControl(BTN_MULTIPLAY_ALIAS);
                pBtnAlias->SetBackCoordTex(0.f,0.4609375f,0.921875f,0.921875f);
            }
        }
        else if(pEvent->lParam2 == 1) //동맹을 맺겠다고 응답이 왔다.
        {
            //멀티플레이요청이 있었다.
            if(pEvent->lParam)
            {
                SetAllians(pMul->GetTeamID(), pMul->GetEnemyTeamID());
                CControl* pBtnAlias = mpEnemyTools->FindControl(BTN_MULTIPLAY_ALIAS);
                pBtnAlias->SetBackCoordTex(0.f,0.f,0.921875f,0.4609375f);
            }
        }
    }
    else if(pEvent->nMsgID == SGL_DELETE_WAITCURSOR)
    {
        if(mpWaitFindMultiplayCursor)
        {
            mpRightTools->Remove(mpWaitFindMultiplayCursor);
            mpWaitFindMultiplayCursor = NULL;
        }
    }
    else
    {
        CSprite* pSprite =  (CSprite*)pEvent->lObject;
        if(pSprite)
            pSprite->OnEvent(pEvent);
        else
        {
            pSprite = m_pSGLCore->GetActor();;
            pSprite->OnEvent(pEvent);
        }
        
    }
	return 0;
}


void CHWorld::SetAnimationCameraAttacked()
{
    mfAnimationCameraAttacked = 0.03;
    mnAnimationCameraAttackedCnt = 1;
}


CSprite* CHWorld::GetTargetNearByPoint(unsigned char nTeam,float finx,float finz)
{
    CSprite* pTarget = NULL;
    SPoint pt2;
    
    float fx,fz;
    float flen2;
    float fMaxLen = 3200;
    CSGLCore* pCore = GetSGLCore();
    float fMin = 99999999.0f;
    
    
    CListNode<CSprite>* pList = pCore->GetSpriteList();
    ByNode<CSprite>* b = pList->begin();
    for(ByNode<CSprite>* it = b; it != pList->end(); )
    {
        CSprite* sp = it->GetObject();
        //CGhostSprite ...
        //        if(sp->GetState() != SPRITE_RUN || sp->GetModelID() == -1)
        if(sp->GetState() != SPRITE_RUN || sp->GetType() & ACTORTYPE_TYPE_GHOSTANITEM)
        {
            it = it->GetNextNode();
            continue;
        }
        
        if(IsAllians(nTeam, sp->GetTeamID()) == false) //적일 경우
        {
            sp->GetPosition(&pt2);
            
            fx = pt2.x - finx;
            fz = pt2.z - finz;
            
            flen2 = fx * fx + fz * fz;
            if (flen2 <= fMaxLen)
            {
                if(flen2 < fMin)
                {
                    fMin = flen2;
                    pTarget = sp;
                }
            }
        }
        it = it->GetNextNode();
    }
    
    return pTarget;
}


bool CHWorld::IsCenterClick(float fx,float fy)
{
    float fgx,fgy;
    CControl::DtoGL(fx,fy,&fgx, &fgy);
    
    //레이다 아이템이 없으면 레이더 어텍을 막자.. 루푸의 가장 처음에 있기 때문에 속도 저하는 없다.
    //    int nCnt = GetUserInfo()->GetCntByIDWithItem(ID_ITEM_ATTACKBYRADA);
    //    if(nCnt != 0 && mpRadaBoard->IsRadaCenter(fgx, fgy)) return false;
    
    if (mpRadaBoard->GetHide() == false && mpRadaBoard->IsRadaCenter(fgx, fgy)) return false;
    else if(mpRightTools->IsTabIn(fx, fy)) return false;
    else if(mpItemSlotBar->IsTabIn(fx, fy)) return false;
    else if(mpShopBtn->IsTabIn(fx, fy)) return false;
    return true;
}


void CHWorld::MakeRuntimeItem(SPoint* ptPostion)
{
    CSGLCore *pSGLCore = GetSGLCore();
    //----------------------------------------
    //적의 죽은 위치에 아이템을 생성한다.
    //----------------------------------------
    //    if(pSGLCore->mpItemMan->IsNeedItem())
    pSGLCore->mpItemMan->AddItem(ptPostion);
    //----------------------------------------
}

void CHWorld::KilledActoerToMission(CSprite* pSprite,bool bRcv)
{
    if(pSprite->GetModelID() == ACTORID_BLZ_EMAIN)
    {
        
        CMutiplyProtocol* pMul = GetMutiplyProtocol();
        
        if(m_pSGLCore->mlstAddedSprite.size() - 1 == mnCurrentETMainTowerStartPointIndex)
        {
            mpRadaBoard->SetDMainTower(NULL);
            if(pMul->IsStop()) //싱글플레이면 종료한다.
                CScenario::SendMessage(SGL_MESSAGE_SUCCESS_MISSION,0);
            else if(IsAllians(pMul->GetEnemyTeamID(), pMul->GetTeamID())) //동맹관계이면 종료한다.
               CScenario::SendMessage(SGL_MESSAGE_SUCCESS_MISSION,0);
            else if(mbIsDestoryEnemyActor) //동맹이 안된상태에서 이미 적이 죽어 있으면 종료한다.
                CScenario::SendMessage(SGL_MESSAGE_SUCCESS_MISSION,0);
            mbIsDestroyEMainTower = true;
        }
        else
        {
            mnCurrentETMainTowerStartPointIndex ++;
            //다음 메인타워를 세운다.
            MakeBLZMainTower();
            MultiplyState state = pMul->GetMultiplayState();
            if(pMul->IsStop() && (state == SYNC_FINDPLAYER || state == SYNC_MATCHING) && gPlayOut && mpWaitFindMultiplayCursor) //매칭을 취소한다.
            {
                CScenario::SendMessage(SGL_DELETE_WAITCURSOR); //성공이다.
                gPlayOut();
            }
            
        }
    }
    else
    {
        bool bActor = pSprite->GetAICore()->IsActor();
        //메인타워를 다 제거하였고, 내가 직접 , 죽은 넘이 내가 아니고, 죽은 넘이 액터 타입이고, 동맹관계가 아니면 성공메세지를 보여준다.
        if(mbIsDestroyEMainTower && pSprite != GetActor() && bActor && !IsAllians(pSprite->GetTeamID(), GetActor()->GetTeamID()))
            CScenario::SendMessage(SGL_MESSAGE_SUCCESS_MISSION,0); //성공이다.
        else if(pSprite != GetActor() && bActor && bRcv) //적 액터가 죽었다면,
        {
            CMutiplyProtocol* pMul = GetMutiplyProtocol();
            if(pMul->IsStop() == false && gPlayOut) //Disconnection을 해버린다.
            {
                gPlayOut();
                CScenario::SendMessage(SGL_SHOW_ENEMYTOOLS,0);
            }
            mbIsDestoryEnemyActor = true;
        }
        
    }
}

void CHWorld::CompactedToBomb(CSprite* pSprite,CBomb* pBomb,SPoint* ptSprite,float fCompactGrade,int nActorTeamID,int nBombTeamID,float* pfModney)
{
    float fAttackInter = 0.0f,fAttackRate = 0.0f;
    CSprite* pOwnerBomb = pBomb->GetOwner();
    CSprite* pActor = GetActor();
    fAttackInter = pSprite->GetCurrentDefendPower();
    if(pSprite->CompactToBomb(pBomb,fCompactGrade)) //명중하여 .....
    {
        if(pSprite->CheckAlive()) return;
        
        GetMutiplyProtocol()->SendSyncDeleteObj(pSprite->GetID(), 0, pSprite->IsVisibleRender());
        pSprite->SetState(BOMB_COMPACT);
        
        //맞아서 죽으면 기준치값을 받아온다. 같은팀이 죽이면 점수를 올려준다.
        if(nActorTeamID == nBombTeamID)
        {
            if(pActor == pOwnerBomb)
            {
                fAttackInter -= pSprite->GetCurrentDefendPower();
                fAttackRate = fAttackInter / pSprite->GetMaxDefendPower();
                *pfModney += pSprite->GetMaxScore() * fAttackRate;
            }
            
            //----------------------------------------
            //적의 죽은 위치에 아이템을 생성한다.
            //----------------------------------------
            MakeRuntimeItem(ptSprite);
            //----------------------------------------
        }
        
        KilledActoerToMission(pSprite,false);
        
        if(pSprite->GetPropertyTank()->nType == 3) //Blz 타입이면
        {
            SetBlzSlotCount(pSprite->GetModelID(), true, pSprite);
        }
    }
    else
    {
        CAniRuntimeBubble* pBubble = pSprite->GetAniRuntimeBubble();
        if(pBubble) pBubble->Attacted();
        
        //맞았지만 죽지는 않았다면 기준치의 /3 한다.
        if(nActorTeamID == pBomb->GetTeamID() && pActor != pSprite)
        {
            fAttackInter -= pSprite->GetCurrentDefendPower();
            fAttackRate = fAttackInter / pSprite->GetMaxDefendPower();
            *pfModney += pSprite->GetMaxScore() * fAttackRate;
        }
        else if(pSprite == pActor) //버블혹은 레이저 타입은 ....
        {
            
            //버블이 있을 경우 그리고 폭탄이 레이져일 경우 메인타워 버블안쪽에 있는 경우.
            if(pBubble == NULL && !(pBomb->GetBombType() & BOMB_FASET_BOMB_TYPE) && pSprite->GetMainTowerBubbleCntInRange() == 0)
            {
                SetAnimationCameraAttacked();
                GetSGLCore()->PlayVibrate();
            }
            
        }
    }
    
    if(pOwnerBomb)
        pOwnerBomb->Command(AICOMMAND_HITTHEMARK, (void*)1); //명중했다라고 ....
}

void CHWorld::CheckAttectedByEnSpriteAtRaser(CBomb* pBomb)
{
    float fModney = 0.f;
    CSprite *pActor = GetActor();
    int nActorTeamID = pActor->GetTeamID();
    int nBombTeamID = pBomb->GetTeamID();
    CSprite *pOwnerBomb = pBomb->GetOwner();
    CSprite* pSprite = pBomb->GetTarget();
    int nSpriteTeamID = 0;
    SPoint ptSp;
    if(pSprite && pSprite->GetState() == SPRITE_RUN)
    {
        nSpriteTeamID = pSprite->GetTeamID();
        //Actor는 아군도 죽일수 있다.
        if(IsAllians(nSpriteTeamID, nBombTeamID) && (pActor != pOwnerBomb || pOwnerBomb == pSprite)) return;
        pSprite->GetPosition(&ptSp);
        CompactedToBomb(pSprite,pBomb,&ptSp,1.0f,nActorTeamID,nBombTeamID,&fModney);
        
        if(fModney != 0.f)
            CScenario::SendMessage(SGL_MESSAGE_MAKE_SCORE,fModney * 1000.f);
    }
}

//폴탄이 터질때 그주위에 스프라이트가 존재하면 공격을 받았다고 생각을 한다.
void CHWorld::CheckAttectedByEnSpriteAtBomb(CListNode<CSprite> *pCoreListSprite,CBomb *pBomb)
{
    float fSPAndBombDistance;
    CSprite *pSprite;
    CSprite *pActor = GetActor();
    CSprite *pOwnerBomb = pBomb->GetOwner();
    int spState;
    SPoint ptSp,ptBomb;
    CSGLCore *pSGLCore = GetSGLCore();
    
    //    float subX;
    //    float subZ;
    
    float fCompactGrade = 0.f;
    float fModney = 0.f;
    //    float fAttackRate = 0.0f;
    //    float fAttackInter = 0.0f;
    
    int nActorTeamID = pActor->GetTeamID();
    int nBombTeamID = pBomb->GetTeamID();
    int nSpriteTeamID = 0;
    
    pBomb->GetPosition(&ptBomb);
    if(pBomb->GetBombType() & BOMB_FASET_BOMB_TYPE)
    {
        pSprite = pBomb->GetTarget();
        
        if(pSprite && pSprite->GetState() == SPRITE_RUN)
        {
            spState = pSprite->GetState();
            nSpriteTeamID = pSprite->GetTeamID();
            pSprite->GetPosition(&ptSp);
            fSPAndBombDistance = sglSquareDistanceSPointSPointf(&ptSp, &ptBomb);
            
            if(pActor == pSprite)
            {
                if(pBomb->GetBombType() & BOMB_FASET_BOMB_TYPE)
                {
                    pSGLCore->PlaySystemSound(SOUND_ID_ET1_FIRE);
                }
                else
                {
                    if(fSPAndBombDistance < 4900.f) //가까운거리에서 폭팔
                        pSGLCore->PlaySystemSound(SOUND_ID_NEARBOMBED);
                    
                    else if(fSPAndBombDistance < 22500.f) //먼거리에서 폭팔
                        pSGLCore->PlaySystemSound(SOUND_ID_FARBOMBED);
                }
            }
            
            
            if(pSprite->GetType() & ACTORTYPE_TYPE_GHOSTANITEM || IsAllians(nSpriteTeamID, nBombTeamID)) return;
            
            //SPRITE_READYDELETE하면 다음단계에서 캐릭터는 SPRITE_READYDELETE => SPRITE_DELETED 로 해버린다. 그럴경운 그다음 단계에 CET1의 부스터의 단계는 SPRITE_DELETED로 변경되어 에러가 발생한다.
            if(pBomb->GetResultRaser() == 0) //결과가 명중일때만 데미지를 준다.
                CompactedToBomb(pSprite,pBomb,&ptSp,1.0f,nActorTeamID,nBombTeamID,&fModney);
            
        }
    }
    else
    {
        
        for(ByNode<CSprite>* it = pCoreListSprite->end()->GetPrevNode(); it != &pCoreListSprite->GetHead();)
        {
            pSprite = it->GetObject();
            it = it->GetPrevNode();
            spState = pSprite->GetState();
            
            if( spState != SPRITE_RUN) continue;
            nSpriteTeamID = pSprite->GetTeamID();
            
            
            pSprite->GetPosition(&ptSp);
            fSPAndBombDistance = sglSquareDistanceSPointSPointf(&ptSp, &ptBomb);
            
            if(pActor == pSprite)
            {
                if(pBomb->GetBombType() & BOMB_FASET_BOMB_TYPE)
                {
                    pSGLCore->PlaySystemSound(SOUND_ID_ET1_FIRE);
                }
                else
                {
                    if(fSPAndBombDistance < 4900.f) //가까운거리에서 폭팔
                        pSGLCore->PlaySystemSound(SOUND_ID_NEARBOMBED);
                    
                    else if(fSPAndBombDistance < 22500.f) //먼거리에서 폭팔
                        pSGLCore->PlaySystemSound(SOUND_ID_FARBOMBED);
                }
            }
            
            if(pSprite->GetType() & ACTORTYPE_TYPE_GHOSTANITEM || IsAllians(nSpriteTeamID, nBombTeamID)) continue; //동맹군이나 팀은 자신의 건물에 상처를 내지 말자.
            
            if(fSPAndBombDistance < 90000.f && pOwnerBomb)
            {
                pSprite->Command(AICOMMAND_ATTACKEDBYEMSPRITE, pOwnerBomb);
            }
            
            if(fSPAndBombDistance < 2000.f)
            {
                if( pSprite->GetState() != SPRITE_DELETED) //왜 SPRITE_DELETED로 했을까 SPRITE_RUN일때만 하면 안될까? 어찌하였던 폭탄은 이미 터졌기 때문에 다시 드러오지 않는다.
                {
                    fCompactGrade = pBomb->InsideRangeCompact(fSPAndBombDistance,pSprite);
                    if(fCompactGrade != 0.f)
                    {
                        //SPRITE_READYDELETE하면 다음단계에서 캐릭터는 SPRITE_READYDELETE => SPRITE_DELETED 로 해버린다. 그럴경운 그다음 단계에 CET1의 부스터의 단계는 SPRITE_DELETED로 변경되어 에러가 발생한다.
                        CompactedToBomb(pSprite,pBomb,&ptSp,fCompactGrade,nActorTeamID,nBombTeamID,&fModney);
                    }
                }
            }
        }
    }
    
    //돈을 벌었으니 화면에 반영해준다.
    if(fModney != 0.f)
        CScenario::SendMessage(SGL_MESSAGE_MAKE_SCORE,fModney * 1000.f);
}


void CHWorld::RequestTry(CListNode<CSprite> *sp,CListNode<CBomb> *bm,CListNode<AParticle> *pa)
{
    CSprite *pActor = GetActor();
    CSGLCore *pSGLCore = GetSGLCore();
    RuntimeRenderObjects *pRuntimeObjects;
    CListNode<CSprite> *plistSprite = NULL;
	CListNode<CBomb>   *plistBomb   = NULL;
    CListNode<AParticle> *plistParticle = NULL;
    
    
    mfScore = 0.f;
    mnBonus = 0;
    //    mnGGTime = 0;
    
    pRuntimeObjects = new RuntimeRenderObjects;
    if(mRequestTry == 1 || mRequestTry == 2)
    {
        int nCnt = sp->Size();
        plistSprite = new CListNode<CSprite>;
        for (int i = nCnt - 1; i >= 0; i--) {
            CSprite* s = sp->GetAt(i);
            if(mRequestTry == 1)
            {
                s->SetState(SPRITE_READYDELETE);
                plistSprite->AddTTail(s);
            }
            else
                s->SetState(SPRITE_DELETED);
            
        }
        
        nCnt = bm->Size();
        plistBomb = new CListNode<CBomb>;
        for (int i = nCnt - 1; i >= 0; i--) {
            CBomb* s = bm->GetAt(i);
            if(mRequestTry == 1)
            {
                s->SetState(SPRITE_READYDELETE);
                plistBomb->AddTTail(s);
            }
            else
                s->SetState(SPRITE_DELETED);
            
        }
        
        nCnt = pa->Size();
        plistParticle = new CListNode<AParticle>;
        for (int i = nCnt - 1; i >= 0; i--) {
            AParticle* s = pa->GetAt(i);
            if(mRequestTry == 1)
            {
                s->SetState(SPRITE_READYDELETE);
                plistParticle->AddTTail(s);
            }
            else
                s->SetState(SPRITE_DELETED);
            
        }
        if(mRequestTry == 1)
            mRequestTry = 2;
        else
            mRequestTry = 3;
        
    }
    else
    {
        plistSprite = new CListNode<CSprite>;
        plistBomb = new CListNode<CBomb>;
        plistParticle = new CListNode<AParticle>;
        
        int nCnt = sp->Size();
        for (int i = nCnt - 1; i >= 0; i--) {
            CSprite* s = sp->GetAt(i);
            if(pActor == s)
            {
                SetActor(NULL);
            }
            //메모리를 지운다. 리스트에서 영원히 지운다.
            pSGLCore->DelSprite(i);
        }
        
        nCnt = bm->Size();
        for (int i = nCnt - 1; i >= 0; i--) {
            CBomb* s = bm->GetAt(i);
            delete s; //메모리를 지운다.
            bm->Delete(i); //리스트에서 영원히 지운다.
        }
        
        nCnt = pa->Size();
        for (int i = nCnt - 1; i >= 0; i--) {
            AParticle* s = pa->GetAt(i);
            delete s; //메모리를 지운다.
            pa->Delete(i); //리스트에서 영원히 지운다.
        }
        
        
        CUserInfo* pUserInfo = mpScenario->GetUserInfo();
        int nSize = (int)m_pSGLCore->mlstStartPoint.size();
        if(nSize > 0)
        {
            int nv = 0;
            if(nSize > 1)
            {
                srand((unsigned int)time(NULL));
                nv = rand();
                nv = nv % nSize;
            }
            else
                nv = 0;
            
            STARTPOINT *pPoint = m_pSGLCore->mlstStartPoint[nv];
            
            CSprite* pSprite = CSGLCore::MakeSprite(GetNewID(),pPoint->nTeamID,pPoint->nModelID,GetTextureMan(),m_pSGLCore->GetAction(),this,NETWORK_MYSIDE);
            if(pSprite)
            {
                SVector vtDir;
                vtDir.x = pPoint->fDirX;
                vtDir.y = pPoint->fDirY;
                vtDir.z = pPoint->fDirZ;
                
                SPoint  ptPos;
                ptPos.x = pPoint->fPosX;
                ptPos.y = pPoint->fPosY;
                ptPos.z = pPoint->fPosZ;
                
                GetMovePosiztionY(&ptPos);
                pSprite->LoadResoure(GetModelMan());
                pSprite->Initialize(&ptPos, &vtDir);
                m_pSGLCore->AddSprite(pSprite);
                SetActor(pSprite); //주인공이다.
                pSprite->SetCameraType(CAMERT_BACK); //실제로 카메라 타입을 셋한다.
                
                
                vector<USERINFO*>*lstBomb = pUserInfo->GetListBomb();
                int nBombSize = (int)lstBomb->size();
                if(nBombSize > 0)
                {
                    CK9Sprite *pSP = dynamic_cast<CK9Sprite*>(pSprite);
                    if(pSP)
                        pSP->SetCurrentSelMissaileInfo((*lstBomb)[0]);
                }
                else if(nBombSize > 1)
                {
                    CK9Sprite *pSP = dynamic_cast<CK9Sprite*>(pSprite);
                    if(pSP)
                        pSP->SetCurrentSelMissaileInfo((*lstBomb)[1]);
                }
            }
        }
        
        
        //적메인타워 여러개 형성함.
        SPoint ptMainTowerPos;
        STARTPOINT *pPoint = NULL;
        nSize = (int)m_pSGLCore->mlstAddedSprite.size();
        if(nSize >= 1)
        {
            if(nSize > 1)
                sort(m_pSGLCore->mlstAddedSprite.begin(), m_pSGLCore->mlstAddedSprite.end(),MainTowerSort);
            pPoint = m_pSGLCore->mlstAddedSprite[nSize - 1];
        }
        else if( nSize == 0)
        {
            HLogE("[Error] not fount ET Main Tower");
            return ;
        }
        
        CSprite* pSprite = CSGLCore::MakeSprite(GetNewID(),pPoint->nTeamID,pPoint->nModelID,GetTextureMan(),m_pSGLCore->GetAction(),this,NETWORK_MYSIDE);
        if(pSprite)
        {
            SVector vtDir;
            vtDir.x = pPoint->fDirX;
            vtDir.y = pPoint->fDirY;
            vtDir.z = pPoint->fDirZ;
            
            SPoint  ptPos;
            ptPos.x = pPoint->fPosX;
            ptPos.y = pPoint->fPosY;
            ptPos.z = pPoint->fPosZ;
            
            if(pPoint->nModelID == ACTORID_BLZ_EMAIN)
                memcpy(&ptMainTowerPos,&ptPos,sizeof(SPoint));
            
            //GetPositionZ(&ptPos);
            pSprite->LoadResoure(GetModelMan());
            pSprite->Initialize(&ptPos, &vtDir);
            m_pSGLCore->AddSprite(pSprite);
        }
        
        
        
        mRequestTry = 0;
        if(mpConfirmMsgCtl)
        {
            delete mpConfirmMsgCtl;
            mpConfirmMsgCtl = NULL;
        }
        //        mpConfirmMsgCtl->Hide();
    }
    
    pRuntimeObjects->plistSprite = plistSprite;
    pRuntimeObjects->plistBomb = plistBomb;
    pRuntimeObjects->plistBoardObjects = new CListNode<RadaBoardObject>;
    pRuntimeObjects->plistParticle = plistParticle;
    
    
    
    mpThreadQueue->Offer((int*)pRuntimeObjects);
    mpThread->EndPing();
    
    
    
}


void CHWorld::InitAllians()
{
    mAllians[SGL_TEAMALL] = SGL_TEAM1|SGL_TEAM2|SGL_TEAM3|SGL_TEAM4;
    mAllians[SGL_TEAM1] = SGL_TEAM1;
    mAllians[SGL_TEAM2] = SGL_TEAM2;
    mAllians[SGL_TEAM3] = SGL_TEAM3;
    mAllians[SGL_TEAM4] = SGL_TEAM4;
}

void CHWorld::SetAllians(unsigned int nTeam,unsigned int nOtherTeam)
{
    mAllians[nTeam] |= nOtherTeam;
    mAllians[nOtherTeam] |= nTeam;
}
void CHWorld::SetNoneAllians(unsigned int nTeam,unsigned int nOtherTeam)
{
    mAllians[nTeam] &= ~nOtherTeam;
    mAllians[nOtherTeam] &= ~nTeam;
}

bool CHWorld::IsAllians(unsigned int nTeam,unsigned int nOtherTeam) { return mAllians[nTeam] & nOtherTeam;}

void CHWorld::BeginTouch(long lTouchID,float x, float y)
{
    bool bIs = true;
    
    
    if(mpConfirmMsgCtl == NULL && mpMissionMsgCtl == NULL && mpPauseMsgCtl == NULL && mpFrameBar == NULL)
    {
#ifdef MAC
        bIs = mpLeftTools->BeginTouch(lTouchID, x, y);
        if(bIs == false) return;
#endif
        
        if(bIs == true)
        {
            bIs = mpItemSlotBar->BeginTouch(lTouchID, x, y);
            if(bIs == true)
            {
                bIs = mpStick->BeginTouch(lTouchID,x, y);
                if(bIs == true)
                {
                    bIs = mpRadaBoard->BeginTouch(lTouchID, x, y);
                    if(bIs == true)
                    {
                        bIs  = mpRightTools->BeginTouch(lTouchID, x, y);
                        if(bIs == true)
                        {
                            bIs = mpShopBtn->BeginTouch(lTouchID, x, y);
                            if(bIs == true)
                            {
                                
                            }
                            else
                                return;
                        }
                        else
                            return;
                    }
                }
            }
            else if(mpClickedSprite == NULL) //타워 정보슬롯이 아니면
            {
                CUserInfo* pUserInfo = mpScenario->GetUserInfo();
                if(pUserInfo->GetLastBombItemSwitch() == 2)
                {
                    CList<CControl*>* pChildCtl = mpItemSlotBar->GetChilds();
                    int nSize = pChildCtl->size();
                    for (int i = 0; i < nSize; i++)
                    {
                        
                        CControl* pCon = pChildCtl->get(i);
                        if(pCon->GetPushed() && pCon->GetID() >= CTL_TABLE_ITEM_SLOT3 && pCon->GetID() <= CTL_TABLE_ITEM_SLOT6)
                        {
                            if(mpBlzSelectedIcon) delete mpBlzSelectedIcon;
                            mpBlzSelectedIcon = pCon->Clone();
                            mpBlzSelectedIcon->SetEnable(false); //버튼이기 때문에 클릭하지 못하게 한다.
                            break;
                        }
                    }
                }
            }
            
        }
    }
    else
    {
        if(mpConfirmMsgCtl)
            mpConfirmMsgCtl->BeginTouch(lTouchID, x, y);
        
        if(mpMissionMsgCtl)
            mpMissionMsgCtl->BeginTouch(lTouchID, x, y);
        
        if(mpPauseMsgCtl)
            mpPauseMsgCtl->BeginTouch(lTouchID, x, y);
        
        if(mpFrameBar)
        {
            if(mpBuyDlg)
            {
                mpBuyDlg->BeginTouch(lTouchID, x, y);
            }
            else if(mpFrameBar->BeginTouch(lTouchID, x, y) == true)
            {
                
#ifdef ANDROID //안드로이드가 아닐 경우 텍스쳐를 지운다.
                mbNeedDelFrameBar = true;
#else
                //안드로이드는 BeginRender에서 지우자.
                delete mpFrameBar;
                mpFrameBar = NULL;
                delete mpFrameBarTexureMan;
                mpFrameBarTexureMan = NULL;
#endif
                SetPauseTime(false);
            }
        }
    }
}


void CHWorld::MoveTouch(long lTouchID,float x, float y)
{
    if(mpConfirmMsgCtl || mpMissionMsgCtl || mpPauseMsgCtl) return;
    
    if(mpFrameBar && mpBuyDlg == NULL)
    {
        mpFrameBar->MoveTouch(lTouchID, x, y);
        return;
    }
    
    if(mpBlzSelectedIcon)
    {
        mpBlzSelectedIcon->SetPosition(x, y);
        mpStick->ShowFly(true, x, y);
        return;
    }
    
    bool bIs = true;
    bIs = mpStick->MoveTouch(lTouchID,x, y);
    if(bIs == true)
    {
        
    }
    
}


void CHWorld::SendMakeBlzMessage()
{
    //건물을 짖는다.
    float* fNew = new float[3];
    memcpy(fNew,mpStick->GetFlyPosition(),sizeof(float)*3);
    if(fNew[0] != -9999.f)
    {
        USERINFO* pInfo = (USERINFO*)mpBlzSelectedIcon->GetLData();
        if(GetUserInfo()->GetGold() >= pInfo->nCnt)
        {
            CScenario::SendMessage(SGL_MAKE_BLZ,pInfo->nID,GetActor()->GetTeamID(),0,(long)fNew);
        }
    }
}

void CHWorld::EndTouch(long lTouchID,float x, float y)
{
#ifdef MAC
    bool bIs;
    bIs = mpLeftTools->EndTouch(lTouchID, x, y);
    if(bIs == false) return;
#endif
    
    
    if(mpBlzSelectedIcon)
    {
        bool bIs = false;
        bIs = mpItemSlotBar->EndTouch(lTouchID, x, y); //Up을 발생한다.
        if(bIs == false)
        {
            if(mpItemSlotBar->FindControl(mpBlzSelectedIcon->GetID())->IsTabIn(x, y) == false) //자기 자신을 클릭하지 않으면 건물을 만든다.
            {
                
                SendMakeBlzMessage();
            }
        }
        
        mpStick->ShowFly(false, 0.f, 0.f);
        delete mpBlzSelectedIcon;
        mpBlzSelectedIcon = NULL;
        
        if(mpConfirmMsgCtl)
            mpConfirmMsgCtl->EndTouch(lTouchID, x, y);
        if(mpMissionMsgCtl)
            mpMissionMsgCtl->EndTouch(lTouchID, x, y);
        if(mpPauseMsgCtl)
            mpPauseMsgCtl->EndTouch(lTouchID, x, y);
        if(mpBuyDlg)
            mpBuyDlg->EndTouch(lTouchID, x, y);
        if(mpFrameBar)
            mpFrameBar->EndTouch(lTouchID, x, y);
        
        
        //스틱이 움직이고 있을때 핸들을 읽어버린다.
        mpStick->EndTouch(lTouchID,9999, y);
        mpRadaBoard->EndTouch(lTouchID, 9999, y);
        mpRightTools->EndTouch(lTouchID, 9999, y);
        mpShopBtn->EndTouch(lTouchID, 9999, y);
        return;
    }
    
    
    if(mpConfirmMsgCtl == NULL && mpMissionMsgCtl == NULL && mpPauseMsgCtl == NULL && mpFrameBar == NULL)
    {
        bool bIs = true;
        bIs = mpItemSlotBar->EndTouch(lTouchID, x, y);
        if(bIs == true)
        {
            SetClickedSprite(NULL);
            
            bIs = mpStick->EndTouch(lTouchID,x, y);
            if(bIs == true)
            {
                bIs = mpRadaBoard->EndTouch(lTouchID, x, y);
                if(bIs == true)
                {
                    bIs = mpRightTools->EndTouch(lTouchID, x, y);
                    if(bIs == true)
                    {
                        bIs = mpShopBtn->EndTouch(lTouchID, x, y);
                        if(bIs == true)
                        {
                        }
                    }
                }
            }
        }
    }
    else
    {
        if(mpConfirmMsgCtl)
            mpConfirmMsgCtl->EndTouch(lTouchID, x, y);
        if(mpMissionMsgCtl)
            mpMissionMsgCtl->EndTouch(lTouchID, x, y);
        if(mpPauseMsgCtl)
            mpPauseMsgCtl->EndTouch(lTouchID, x, y);
        if(mpBuyDlg)
            mpBuyDlg->EndTouch(lTouchID, x, y);
        if(mpFrameBar)
            mpFrameBar->EndTouch(lTouchID, x, y);
        
        //스틱이 움직이고 있을때 핸들을 읽어버린다.
        mpStick->EndTouch(lTouchID,9999, y);
        mpRadaBoard->EndTouch(lTouchID, 9999, y);
        mpRightTools->EndTouch(lTouchID, 9999, y);
        mpShopBtn->EndTouch(lTouchID, 9999, y);
    }
    
    
}


void CHWorld::SetBlzSlotCount(int nBlzID, bool bRemove,CSprite* pDeletedSprite)
{
    CUserInfo* pUserInfo = GetUserInfo();
    if(bRemove == false && mpClickedSprite == NULL)
    {
        PROPERTY_TANK pt;
        PROPERTY_TANK::GetPropertyTank(nBlzID, pt);
        pUserInfo->SetGold(pUserInfo->GetGold() - pt.nGold);
        ResetDisplayTopGold();
        
        GetUserInfo()->mnGetherBuildBlzCount += 1;
    }
    else if(bRemove && mpClickedSprite == pDeletedSprite)
    {
        PROPERTY_TANK pt;
        PROPERTY_TANK::GetPropertyTank(nBlzID, pt);
        pUserInfo->SetGold(pUserInfo->GetGold() + pt.nGold / 2.f);
        ResetDisplayTopGold();
        SetClickedSprite(NULL);
    }
}


void CHWorld::SetClickedSprite(CSprite* pClickedSprite)
{
    if(mpClickedSprite == NULL && pClickedSprite == NULL) return;
    mpClickedSprite = pClickedSprite;
    ResetDisplayItemSlot();
    if(pClickedSprite) OnSelectedObjectShowDescBar(pClickedSprite);
}


//주인공의 위치에서 pTartketPoint 방향으로 카메라를 회전한다.
void CHWorld::SetRotationCarmeraToPoint(SPoint* pTartketPoint)
{
    if(mpCarmeraRotationTartketPoint)
    {
        delete mpCarmeraRotationTartketPoint;
        mpCarmeraRotationTartketPoint = NULL;
    }
    
    if(pTartketPoint)
    {
        mpCarmeraRotationTartketPoint = new SPoint;
        memcpy(mpCarmeraRotationTartketPoint, pTartketPoint, sizeof(SPoint));
    }
}


void CHWorld::RotationCarmeraToPoint(int nTime)
{
    if(mpCarmeraRotationTartketPoint == NULL) return ;
    
    CK9Sprite *pActor1 = (CK9Sprite*)GetActor();
    SPoint ptActor;
    pActor1->GetPosition(&ptActor);
    
    //180 - 을 왜 했냐.. z축이 -로 되어 있다. 모든 계산이 비정상적이다 나중에 왕창수정해야 할것 같다.
    float fAngle = AngleArrangeEx(180.f - sglNowToPosAngle(&ptActor,mpCarmeraRotationTartketPoint));
    float fMV = pActor1->CatRotation(pActor1->GetCameraAngle(), fAngle) * nTime * 0.1;
    if(sglCheckRotation(pActor1->GetCameraAngle(),fAngle,&fMV))
    {
        SetRotationCarmeraToPoint(NULL); //앞으로 카메라 회전을 멈춘다.
    }
    fAngle = fMV + pActor1->GetCameraAngle();
    pActor1->SetCameraAngle(fAngle);
}


void CHWorld::ShowFrameTable()
{
    vector<string>  lstImage;
    vector<float> layout;
    vector<float> layout2;
    float fMoveY = 0.f;
    float fWhite[] = {1.0f,1.0f,1.0f,1.0f};
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    
    mpFrameBarTexureMan = new CTextureMan();
    mpFrameBar =  new CLabelCtl(NULL,mpFrameBarTexureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("FT_FrameBar", layout);
    
    
    if(gnDisplayType == DISPLAY_IPHONE)
    {
        PROPERTYI::GetPropertyFloatList("FT_TableBombTab", layout2);
        fMoveY = layout2[3];
    }
    
    float fX = (gDisplayWidth - layout[2] ) / 2.0f + layout[0]; //282;
    float fY = (gDisplayHeight - layout[3] ) / 2.4f+ layout[1];//106; //약간 상단에 위치하길 원한다.
    
    mpFrameBar->Initialize(0, fX, fY  - fMoveY, layout[2], layout[3] + fMoveY, lstImage, 0.f, 0.f, 0.f, 0.f);
    //    mpFrameBar->SetHide(true); //일단 숨겨주자.
    
    
    lstImage.clear();
    lstImage.push_back(IMG_TABLE_BANDI);
    lstImage.push_back("none");
    
    //테이블 테두리
    CControl* pTableRecBack =  new CLabelCtl(mpFrameBar,mpFrameBarTexureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("FT_TableRecBack", layout);
    pTableRecBack->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.0087890625, 0.2119140625, 0.46484375, 0.6044921875);
    mpFrameBar->AddControl(pTableRecBack);
    
    
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    mpFrameBombItemTab = new CLabelCtl(mpFrameBar,mpFrameBarTexureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("FT_FrameBombItemTab", layout);
    mpFrameBombItemTab->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.0087890625, 0.2119140625, 0.46484375, 0.6044921875);
    
    //폭탄탭
    lstImage.clear();
    lstImage.push_back(IMG_TABLE_BANDI);
    lstImage.push_back("M_Tap_Bomb");
    CButtonCtl* pTableBombTab = new CButtonCtl(mpFrameBombItemTab,mpFrameBarTexureMan,BUTTON_GROUP_TOGGLE);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("FT_TableBombTab", layout);
    pTableBombTab->Initialize(BTN_BOMBTAB, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage,
                              0.5498046875, 0.21875, 0.6279296875, 0.2529296875,
                              0.466796875, 0.21875, 0.544921875, 0.2529296875
                              );
    pTableBombTab->SetPushed(true);
    pTableBombTab->SetTextColor(fWhite);
    mpFrameBombItemTab->AddControl(pTableBombTab);
    
    
    //아이템탭
    lstImage.clear();
    lstImage.push_back(IMG_TABLE_BANDI);
    lstImage.push_back("M_Tap_Item");
    CButtonCtl* pTableItemTab = new CButtonCtl(mpFrameBombItemTab,mpFrameBarTexureMan,BUTTON_GROUP_TOGGLE);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("FT_TableItemTab", layout);
    pTableItemTab->Initialize(BTN_ITEMTAB, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage,
                              0.5498046875, 0.21875, 0.6279296875, 0.2529296875,
                              0.466796875, 0.21875, 0.544921875, 0.2529296875
                              );
    pTableItemTab->SetTextColor(fWhite);
    mpFrameBombItemTab->AddControl(pTableItemTab);
    
    mpFrameBar->AddControl(mpFrameBombItemTab);
    
    
    //테이블 백그라운드
    CControl* pTableBack =  new CLabelCtl(pTableRecBack,mpFrameBarTexureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("FT_TableBack", layout);
    pTableBack->Initialize(0, fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage, 0.0087890625, 0.6123046875, 0.392578125, 0.9453125);
    pTableRecBack->AddControl(pTableBack);
    
    
    //282,106
    InitFrameBombTable(pTableBack,fX,fY);
    InitFrameItemTable(pTableBack,fX,fY);
    
    if(GetMutiplyProtocol()->IsStop()) //멀티플레이중이면 시간을 멈추지 말자.
        SetPauseTime(true); //시간을 멈춘다.
    
}

void CHWorld::InitFrameItemTable(CControl* pTableBack,float fX,float fY)
{
    float fWhite[] = {1.f,1.f,1.f,1.f};
    CUserInfo* pUserInfo = mpScenario->GetUserInfo();
    vector<string>  lstImage;
    vector<float> layout;
    int nItemCnt = 0;
    lstImage.clear();
    lstImage.push_back(IMG_TABLE_BANDI);
    lstImage.push_back("none");
    
    //if(mpFrameItembTable) delete[] mpFrameItembTable; mpFrameItembTable = NULL;
    
    //BombTable
    //테이블
    mpFrameItembTable = new CTableCtl(pTableBack,mpFrameBarTexureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("TB_FrameBombTable", layout);
    mpFrameItembTable->Initialize(BTN_ITEMTABLE,fX + layout[0], fY + layout[1], layout[2], layout[3], lstImage,0.0087890625, 0.6123046875, 0.392578125, 0.9453125);
    ((CScrollCtl*)mpFrameItembTable)->SetContentsBounds(layout[2], 0);
    
    if(gnDisplayType == DISPLAY_IPAD)
        mpFrameItembTable->SetRowSelMode(false); //cell단위로 이벤트를 받지 않음.
    else
        mpFrameItembTable->SetRowSelMode(true);
    
    mpFrameItembTable->SetHide(true);
    
    
    vector<PROPERTY_ITEM*> list;
    PROPERTY_ITEM::GetPropertyItemList(list);
    int nCnt = (int)list.size();
    
    for (int i = 0; i < nCnt; i++)
    {
        PROPERTY_ITEM* prop = list[i];
        if(prop->nType == 3) continue;
        else if(prop->nType == 1 || prop->nType == 5) //영구사용물을 이미 샀으면 리스트에 추가하지 말자.
        {
            if(pUserInfo)
            {
                bool bIs = false;
                vector<USERINFO*>* pUserItemList = pUserInfo->GetListItem();
                int nSize = (int)pUserItemList->size();
                for(int i = 0; i < nSize; i++)
                {
                    USERINFO *pInfo = (*pUserItemList)[i];
                    if(pInfo->nID == prop->nID)
                    {
                        bIs = true;
                        break;
                    }
                }
                if(bIs) continue;
            }
        }
        
        nItemCnt = pUserInfo->GetCntByIDWithItem(prop->nID);
        
        //        if(prop->nID == ID_ITEM_UPGRADE_DEFENCE_TOWER || prop->nID == ID_ITEM_UPGRADE_MISSILE_TOWER)
        //        {
        //            if(nItemCnt >= 2) //2개이상 사면 리스트에서 사라진다.
        //            {
        //                continue;
        //            }
        //        }
        
        
        //사실상 CellBack 없이 Cell에서 이미지를 넣을수 잇지만.. 테두리를 만들기 위해서 하지 않음.
        lstImage.clear();
        lstImage.push_back("none");
        lstImage.push_back("none");
        CCellCtl* pCell = new CCellCtl(mpFrameBarTexureMan);
        layout.clear();
        PROPERTYI::GetPropertyFloatList("TB_Cell", layout);
        pCell->Initialize(layout[2], layout[3] , lstImage,0.0f,0.0f,1.0f,1.0f);
        pCell->SetLData(prop->nID);
        
        //CellBack
        lstImage.clear();
        lstImage.push_back(IMG_TABLE_BANDI);
        lstImage.push_back("none");
        CControl* pCellBack =  new CLabelCtl(pCell,mpFrameBarTexureMan);
        layout.clear();
        PROPERTYI::GetPropertyFloatList("TB_CellBack", layout);
        pCellBack->Initialize(0, layout[0], layout[1], layout[2], layout[3], lstImage, 0.466796875, 0.3232421875, 0.6982421875, 0.4296875);
        pCell->AddControl(pCellBack);
        pCell->SetLData(prop->nID);
        
        //1은 테이블에서 부름
        InitItemCell(prop,pCellBack,0,0,1);
        
        
        int nGold = prop->nPrice;
        if(prop->nID == ID_ITEM_UPGRADE_ATTACK)
        {
            int grade = pUserInfo?pUserInfo->GetAttackUpgrade() + 1: 1;
            nGold = prop->nPrice * grade;
        }
        else if(prop->nID == ID_ITEM_UPGRADE_DEPEND)
        {
            int grade = pUserInfo?pUserInfo->GetDependUpgrade() + 1: 1;
            nGold = prop->nPrice * grade;
        }
        else if(prop->nID == ID_ITEM_UPGRADE_DEFENCE_TOWER || prop->nID == ID_ITEM_UPGRADE_MISSILE_TOWER)
        {
            nGold = prop->nPrice * (nItemCnt + 1);
        }
        
        if(gnDisplayType == DISPLAY_IPAD)
        {
            //가격
            lstImage.clear();
            lstImage.push_back(IMG_TABLE_BANDI);
            lstImage.push_back("none");
            CControl* pCellValue =  new CLabelCtl(pCell,GetTextureMan());
            layout.clear();
            PROPERTYI::GetPropertyFloatList("TB_CellValue", layout);
            pCellValue->Initialize(0, layout[0], layout[1], layout[2], layout[3], lstImage, 0.4716796875, 0.5107421875, 0.5888671875, 0.546875);
            pCell->AddControl(pCellValue);
            
            //GoldICon
            if(!(prop->nType == 4 || prop->nType == 5))
            {
                lstImage.clear();
                lstImage.push_back("gold.png");
                lstImage.push_back("none");
                CControl* pGoldIcon =  new CLabelCtl(pCellValue,GetTextureMan());
                layout.clear();
                PROPERTYI::GetPropertyFloatList("TB_GoldIcon", layout);
                pGoldIcon->Initialize(0, layout[0], layout[1], layout[2], layout[3], lstImage, 0.0f, 0.0f, 1.0f, 1.0f);
                pCellValue->AddControl(pGoldIcon);
                
                lstImage.clear();
                lstImage.push_back("none");
                lstImage.push_back("none");
                CNumberCtl* pGold =  new CNumberCtl(pCellValue,GetTextureMan(),true);
                layout.clear();
                PROPERTYI::GetPropertyFloatList("TB_Gold", layout);
                pGold->Initialize(9998, layout[0], layout[1], layout[2], layout[3],layout[4], layout[5], layout[6],lstImage);
                pGold->SetAlign(CNumberCtl::ALIGN_LEFT);
                pGold->SetTextColor(fWhite);
                pGold->SetNumber(nGold);
                pCellValue->AddControl(pGold);
                
            }
            else
            {
                lstImage.clear();
                lstImage.push_back("dollar.png");
                lstImage.push_back("none");
                CControl* pGoldIcon =  new CLabelCtl(pCellValue,GetTextureMan());
                layout.clear();
                PROPERTYI::GetPropertyFloatList("TB_GoldIcon", layout);
                pGoldIcon->Initialize(0, layout[0], layout[1], layout[2], layout[3], lstImage, 0.0f, 0.0f, 1.0f, 1.0f);
                pCellValue->AddControl(pGoldIcon);
                
                lstImage.clear();
                lstImage.push_back("none");
                lstImage.push_back("none");
                CNumberCtl* pGold =  new CNumberCtl(pCellValue,GetTextureMan(),true);
                layout.clear();
                PROPERTYI::GetPropertyFloatList("TB_Gold", layout);
                pGold->Initialize(9998, layout[0], layout[1], layout[2], layout[3],layout[4], layout[5], layout[6],lstImage);
                pGold->SetAlign(CNumberCtl::ALIGN_LEFT);
                pGold->SetTextColor(fWhite);
                pGold->SetNumberF(nGold / 1000.f);
                pCellValue->AddControl(pGold);
                
                
            }
            
            
            //Buy Button
            lstImage.clear();
            lstImage.push_back(IMG_TABLE_BANDI);
            lstImage.push_back("M_IB_BUY");
            CControl* pCellBuyButton =  new CButtonCtl(pCell,GetTextureMan());
            layout.clear();
            PROPERTYI::GetPropertyFloatList("TB_CellBuyButton", layout);
            pCellBuyButton->Initialize(BTN_ITEMBUY, layout[0], layout[1], layout[2], layout[3], lstImage, 0.5908203125, 0.5107421875, 0.708984375, 0.546875);
            pCellBuyButton->SetLData(prop->nID);
            pCellBuyButton->SetTextColor(fWhite);
            pCell->AddControl(pCellBuyButton);
        }
        else
        {
            
            //GoldICon
            if(!(prop->nType == 4 || prop->nType == 5))
            {
                lstImage.clear();
                lstImage.push_back("gold.png");
                lstImage.push_back("none");
                CControl* pGoldIcon =  new CLabelCtl(pCellBack,GetTextureMan());
                layout.clear();
                PROPERTYI::GetPropertyFloatList("TB_GoldIcon", layout);
                pGoldIcon->Initialize(0, layout[0], layout[1], layout[2], layout[3], lstImage, 0.0f, 0.0f, 1.0f, 1.0f);
                pCellBack->AddControl(pGoldIcon);
                
                lstImage.clear();
                lstImage.push_back("none");
                lstImage.push_back("none");
                CNumberCtl* pGold =  new CNumberCtl(pCellBack,GetTextureMan(),true);
                layout.clear();
                PROPERTYI::GetPropertyFloatList("TB_Gold", layout);
                pGold->Initialize(9998, layout[0], layout[1], layout[2], layout[3],layout[4], layout[5], layout[6],lstImage);
                pGold->SetAlign(CNumberCtl::ALIGN_LEFT);
                pGold->SetTextColor(fWhite);
                pGold->SetNumber(nGold);
                pCellBack->AddControl(pGold);
            }
            else
            {
                lstImage.clear();
                lstImage.push_back("dollar.png");
                lstImage.push_back("none");
                CControl* pGoldIcon =  new CLabelCtl(pCellBack,GetTextureMan());
                layout.clear();
                PROPERTYI::GetPropertyFloatList("TB_GoldIcon", layout);
                pGoldIcon->Initialize(0, layout[0], layout[1], layout[2], layout[3], lstImage, 0.0f, 0.0f, 1.0f, 1.0f);
                pCellBack->AddControl(pGoldIcon);
                
                
                lstImage.clear();
                lstImage.push_back("none");
                lstImage.push_back("none");
                CNumberCtl* pGold =  new CNumberCtl(pCellBack,GetTextureMan(),true);
                layout.clear();
                PROPERTYI::GetPropertyFloatList("TB_Gold", layout);
                pGold->Initialize(9998, layout[0], layout[1], layout[2], layout[3],layout[4], layout[5], layout[6],lstImage);
                pGold->SetAlign(CNumberCtl::ALIGN_LEFT);
                pGold->SetTextColor(fWhite);
                pGold->SetNumberF(nGold / 1000.f);
                pCellBack->AddControl(pGold);
            }
            
            
        }
        
        
        
        mpFrameItembTable->AddItem(pCell);
    }
    pTableBack->AddControl(mpFrameItembTable);
    PROPERTY_ITEM::ReleaseList((vector<PROPERTYI*>*)&list);
}


void CHWorld::InitFrameBombTable(CControl* pTableBack,float fX,float fY)
{
    float fWhite[] = {1.f,1.f,1.f,1.f};
    vector<string>  lstImage;
    vector<float> layout;
    lstImage.clear();
    lstImage.push_back(IMG_TABLE_BANDI);
    lstImage.push_back("none");
    
    //BombTable
    //테이블
    mpFrameBombTable = new CTableCtl(pTableBack,mpFrameBarTexureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("TB_FrameBombTable", layout);
    mpFrameBombTable->Initialize(BTN_BOMBTABLE,fX + layout[0],fY + layout[1], layout[2], layout[3] , lstImage,0.0087890625, 0.6123046875, 0.392578125, 0.9453125);
    ((CScrollCtl*)mpFrameBombTable)->SetContentsBounds(layout[2], 0);
    
    if(gnDisplayType == DISPLAY_IPAD)
        mpFrameBombTable->SetRowSelMode(false); //cell단위로 이벤트를 받지 않음.
    else
        mpFrameBombTable->SetRowSelMode(true);
    
    
    vector<PROPERTY_BOMB*> list;
    PROPERTY_BOMB::GetPropertyBombList(list);
    int nCnt = (int)list.size();
    
    for (int i = 0; i < nCnt; i++) {
        
        PROPERTY_BOMB* prop = list[i];
        if(prop->nMenuVisible == 0) continue;  //기본 폭탄,적 폭탄 은 안보이게 한다.
        
        
        //사실상 CellBack 없이 Cell에서 이미지를 넣을수 잇지만.. 테두리를 만들기 위해서 하지 않음.
        lstImage.clear();
        lstImage.push_back("none");
        lstImage.push_back("none");
        CCellCtl* pCell = new CCellCtl(mpFrameBarTexureMan);
        layout.clear();
        PROPERTYI::GetPropertyFloatList("TB_Cell", layout);
        pCell->Initialize(layout[2], layout[3] , lstImage,0.0f,0.0f,1.0f,1.0f);
        
        lstImage.clear();
        lstImage.push_back(IMG_TABLE_BANDI);
        lstImage.push_back("none");
        CControl* pCellBack =  new CLabelCtl(pCell,mpFrameBarTexureMan);
        layout.clear();
        PROPERTYI::GetPropertyFloatList("TB_CellBack", layout);
        pCellBack->Initialize(0, layout[0], layout[1], layout[2], layout[3], lstImage, 0.466796875, 0.3232421875, 0.6982421875, 0.4296875);
        pCell->AddControl(pCellBack);
        pCell->SetLData(prop->nID);
        
        InitBombCell(prop,pCellBack,0,0,1);
        
        
        
        if(gnDisplayType == DISPLAY_IPAD)
        {
            //가격
            lstImage.clear();
            lstImage.push_back(IMG_TABLE_BANDI);
            lstImage.push_back("none");
            CControl* pCellValue =  new CLabelCtl(pCell,mpFrameBarTexureMan);
            layout.clear();
            PROPERTYI::GetPropertyFloatList("TB_CellValue", layout);
            pCellValue->Initialize(0, layout[0], layout[1], layout[2], layout[3], lstImage, 0.4716796875, 0.5107421875, 0.5888671875, 0.546875);
            pCell->AddControl(pCellValue);
            
            //GoldICon
            lstImage.clear();
            lstImage.push_back("gold.png");
            lstImage.push_back("none");
            CControl* pGoldIcon =  new CLabelCtl(pCellValue,mpFrameBarTexureMan);
            layout.clear();
            PROPERTYI::GetPropertyFloatList("TB_GoldIcon", layout);
            pGoldIcon->Initialize(0, layout[0], layout[1], layout[2], layout[3], lstImage, 0.0f, 0.0f, 1.0f, 1.0f);
            pCellValue->AddControl(pGoldIcon);
            
            lstImage.clear();
            lstImage.push_back("none");
            lstImage.push_back("none");
            CNumberCtl* pGold =  new CNumberCtl(pCellValue,mpFrameBarTexureMan,true);
            layout.clear();
            PROPERTYI::GetPropertyFloatList("TB_Gold", layout);
            pGold->Initialize(0, layout[0], layout[1], layout[2], layout[3],layout[4], layout[5], layout[6],lstImage);
            pGold->SetAlign(CNumberCtl::ALIGN_LEFT);
            pGold->SetTextColor(fWhite);
            pGold->SetNumber(prop->nPrice);
            pCellValue->AddControl(pGold);
            
            //Buy Button
            lstImage.clear();
            lstImage.push_back(IMG_TABLE_BANDI);
            lstImage.push_back("M_IB_BUY");
            CControl* pCellBuyButton =  new CButtonCtl(pCell,mpFrameBarTexureMan);
            layout.clear();
            PROPERTYI::GetPropertyFloatList("TB_CellBuyButton", layout);
            pCellBuyButton->Initialize(BTN_BOMBBUY, layout[0], layout[1], layout[2], layout[3], lstImage, 0.5908203125, 0.5107421875, 0.708984375, 0.546875);
            pCellBuyButton->SetLData(prop->nID);
            pCellBuyButton->SetTextColor(fWhite);
            pCell->AddControl(pCellBuyButton);
        }
        else //아이폰일 경우 Buy버튼이 필요가 없다.
        {
            //GoldICon
            lstImage.clear();
            lstImage.push_back("gold.png");
            lstImage.push_back("none");
            CControl* pGoldIcon =  new CLabelCtl(pCellBack,mpFrameBarTexureMan);
            layout.clear();
            PROPERTYI::GetPropertyFloatList("TB_GoldIcon", layout);
            pGoldIcon->Initialize(0, layout[0], layout[1], layout[2], layout[3], lstImage, 0.0f, 0.0f, 1.0f, 1.0f);
            pCellBack->AddControl(pGoldIcon);
            
            lstImage.clear();
            lstImage.push_back("none");
            lstImage.push_back("none");
            CNumberCtl* pGold =  new CNumberCtl(pCellBack,mpFrameBarTexureMan,true);
            layout.clear();
            PROPERTYI::GetPropertyFloatList("TB_Gold", layout);
            pGold->Initialize(0, layout[0], layout[1], layout[2], layout[3],layout[4], layout[5], layout[6],lstImage);
            pGold->SetAlign(CNumberCtl::ALIGN_LEFT);
            pGold->SetTextColor(fWhite);
            pGold->SetNumber(prop->nPrice);
            pCellBack->AddControl(pGold);
        }
        
        mpFrameBombTable->AddItem(pCell);
    }
    
    pTableBack->AddControl(mpFrameBombTable);
    PROPERTY_BOMB::ReleaseList((vector<PROPERTYI*>*)&list);
}



//nType : 1 (Table) , 0 (PopupDlg) , 2 (Desc)
void CHWorld::InitBombCell(PROPERTY_BOMB* prop,CControl* pParent,float fOffsetX,float fOffsetY,int nType)
{
    float fWhite[] = {1.f,1.f,1.f,1.f};
    float fRed[] = {1.f,0.f,0.f,1.f};
    float fGreen[] = {0.f,1.0f,0.0f,1.0f};
    vector<string>  lstImage;
    vector<float> layout;
    
    //CellTitleBack
    lstImage.clear();
    if(nType != 2)
        lstImage.push_back(IMG_TABLE_BANDI);
    else
        lstImage.push_back("none");
    lstImage.push_back("none");
    CControl* pCellTitleBack =  new CLabelCtl(pParent,mpFrameBarTexureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("BCe_CellTitleBack", layout);
    pCellTitleBack->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3], lstImage, 0.466796875, 0.2978515625, 0.69921875, 0.3203125);
    pParent->AddControl(pCellTitleBack);
    
    
    //폰의 속성 : (375(56),168(11) = 56,15) 고폭탄/유도탄
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back(prop->sDesc);
    
    CControl* pBombType = new CLabelCtl(pCellTitleBack,mpFrameBarTexureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("BCe_BombType", layout);
    pBombType->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
    pBombType->SetTextColor(fWhite);
    pCellTitleBack->AddControl(pBombType);
    
    //폭탄이름
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back(prop->sName);
    CControl* pBombName = new CLabelCtl(pCellTitleBack,mpFrameBarTexureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("BCe_BombName", layout);
    pBombName->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
    pBombName->SetTextColor(fWhite);
    pCellTitleBack->AddControl(pBombName);
    
    //CellDescBack
    lstImage.clear();
    lstImage.push_back(IMG_TABLE_BANDI);
    lstImage.push_back("none");
    CControl* pCellDescBack =  new CLabelCtl(pParent,mpFrameBarTexureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("BCe_CellDescBack", layout);
    pCellDescBack->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3], lstImage, 0.466796875, 0.4326171875, 0.69921875, 0.501953125);
    pParent->AddControl(pCellDescBack);
    
    //폭탄이미지.
    lstImage.clear();
    lstImage.push_back(prop->sBombTableImgPath);
    lstImage.push_back("none");
    CControl* pBombIcon = new CLabelCtl(pCellDescBack,mpFrameBarTexureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("BCe_BombIcon", layout);
    pBombIcon->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3], lstImage, 0.20703125, 0.2265625, 0.79296875, 0.7734375);
    pCellDescBack->AddControl(pBombIcon);
    
    //Power Label.
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("M_IB_Power");
    CControl* pBombSPLabel = new CLabelCtl(pCellDescBack,mpFrameBarTexureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("BCe_BombSPLabel", layout);
    pBombSPLabel->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
    pBombSPLabel->SetTextColor(fWhite);
    pCellDescBack->AddControl(pBombSPLabel);
    //Power
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    CNumberCtl* pBombSP = new CNumberCtl(pCellDescBack,mpFrameBarTexureMan,true);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("BCe_BombSP", layout);
    pBombSP->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3],layout[4],layout[5],layout[6], lstImage);
    pBombSP->SetTextColor(fRed);
    pBombSP->SetNumber((int)prop->fMaxAttackPower);
    pBombSP->SetAlign(CNumberCtl::ALIGN_LEFT);
    pCellDescBack->AddControl(pBombSP);
    
    //Range Label.
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("M_IB_Range");
    CControl* pBombRngLabel = new CLabelCtl(pCellDescBack,mpFrameBarTexureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("BCe_BombRngLabel", layout);
    pBombRngLabel->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
    pBombRngLabel->SetTextColor(fWhite);
    pCellDescBack->AddControl(pBombRngLabel);
    
    
    //Range.
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    CNumberCtl* pBombRng = new CNumberCtl(pCellDescBack,mpFrameBarTexureMan,true);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("BCe_BombRng", layout);
    pBombRng->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3],layout[4],layout[5],layout[6], lstImage);
    pBombRng->SetTextColor(fGreen);
    pBombRng->SetNumber(prop->fMaxRadianDetect);
    pBombRng->SetAlign(CNumberCtl::ALIGN_LEFT);
    pCellDescBack->AddControl(pBombRng);
    
    
    //Ammo Label.
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("M_IB_Ammo");
    CControl* pBombAmmoLabel = new CLabelCtl(pCellDescBack,mpFrameBarTexureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("BCe_BombAmmoLabel", layout);
    pBombAmmoLabel->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
    pBombAmmoLabel->SetTextColor(fWhite);
    pCellDescBack->AddControl(pBombAmmoLabel);
    
    
    //Ammo.
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back("none");
    CNumberCtl* pBombAmmo = new CNumberCtl(pCellDescBack,mpFrameBarTexureMan,true);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("BCe_BombAmmo", layout);
    pBombAmmo->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3],layout[4],layout[5],layout[6], lstImage);
    pBombAmmo->SetTextColor(fWhite);
    pBombAmmo->SetNumber(prop->nPackCount);
    pBombAmmo->SetAlign(CNumberCtl::ALIGN_LEFT);
    pCellDescBack->AddControl(pBombAmmo);
    
}

//nType : 1 (Table) , 0 (PopupDlg) , 2 (Desc)
void CHWorld::InitItemCell(PROPERTY_ITEM* prop,CControl* pParent,float fOffsetX,float fOffsetY,int nType)
{
    float fWhite[] = {1.0f,1.0f,1.0f,1.0f};
    vector<string>  lstImage;
    vector<float> layout;
    string sTemp;
    char sTempDesc2[128];
    const char* sFormat;
    
    CUserInfo* pUserInfo = mpScenario->GetUserInfo();
    string sDesc1 = prop->sDesc;
    string sDesc2 = prop->sDesc2;
    int grade = -1;
    
    
    //CellTitleBack
    lstImage.clear();
    if(nType != 2) //Des가 아니면 타이틀 테두리가 필요하다.
        lstImage.push_back(IMG_TABLE_BANDI);
    else
        lstImage.push_back("none");
    lstImage.push_back("none");
    CControl* pCellTitleBack =  new CLabelCtl(pParent,mpFrameBarTexureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("ICe_CellTitleBack", layout);
    pCellTitleBack->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3], lstImage, 0.466796875, 0.2978515625, 0.69921875, 0.3203125);
    pParent->AddControl(pCellTitleBack);
    
    //아이템이름
    lstImage.clear();
    lstImage.push_back("none");
    lstImage.push_back(prop->sName);
    CControl* pItemType = new CLabelCtl(pCellTitleBack,mpFrameBarTexureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("ICe_ItemType", layout);
    pItemType->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
    pItemType->SetTextColor(fWhite);
    pCellTitleBack->AddControl(pItemType);
    
    //아이템이미지백.
    lstImage.clear();
    lstImage.push_back(IMG_TABLE_BANDI);
    lstImage.push_back("none");
    CControl* pItemIconBack = new CLabelCtl(pParent,mpFrameBarTexureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("ICe_ItemIconBack", layout);
    pItemIconBack->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3], lstImage, 0.40234375, 0.685546875, 0.4697265625, 0.7529296875);
    pParent->AddControl(pItemIconBack);
    
    //아이템이미지백.
    sTemp = prop->sModelPath;
    sTemp.append(".tga");
    lstImage.clear();
    lstImage.push_back(sTemp);
    lstImage.push_back("none");
    CControl* pItemIcon = new CLabelCtl(pItemIconBack,mpFrameBarTexureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("ICe_ItemIcon", layout);
    pItemIcon->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3], lstImage, 0.0f, 0.0f, 0.96875, 0.96875);
    pItemIconBack->AddControl(pItemIcon);
    
    //아이템Desc백.
    lstImage.clear();
    lstImage.push_back(IMG_TABLE_BANDI);
    lstImage.push_back("none");
    CControl* pItemDescBack = new CLabelCtl(pParent,mpFrameBarTexureMan);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("ICe_ItemDescBack", layout);
    pItemDescBack->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3], lstImage, 0.3994140625, 0.6103515625, 0.55859375, 0.6787109375);
    pParent->AddControl(pItemDescBack);
    
    if(sDesc1.length() > 0 && sDesc2.length() > 0)
    {
        //Desc2_1 (설명이 두줄 첫번째)
        lstImage.clear();
        lstImage.push_back("none");
        lstImage.push_back(prop->sDesc);
        CControl* pDesc21 = new CLabelCtl(pItemDescBack,mpFrameBarTexureMan);
        layout.clear();
        PROPERTYI::GetPropertyFloatList("ICe_Desc21", layout);
        pDesc21->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
        pDesc21->SetTextColor(fWhite);
        pItemDescBack->AddControl(pDesc21);
        
        
        
        //-------------------------------------------
        //중의 : 어의가 없는 에러 즉 이상하게 에러가나는데 이유가 없음
        // 한글 리소스 중에 "I_D-PWD Cap.Desc2" = "방어파워%d%%업그레이드";
        // 이렇게 하면 아주 심각한 에러가 종종발생한다. "I_D-PWD Cap.Desc2" = "방어파워 %d%% 업그레이드";
        // 뛰어쓰기를 해주면 에러가 안난다. 왜일까나...
        // 암튼 글자를 만들거나 할때 문제가 생기는 것같음.
        if(prop->nID == ID_ITEM_UPGRADE_ATTACK)
        {
            //grade = pUserInfo?pUserInfo->GetAttackUpgrade(pUserInfo->GetLastSelectedTankID()) + 1: 1;
            if(nType != 2)
                grade = (pUserInfo->GetAttackUpgrade() + 1) * 5.f;
            else
                grade = pUserInfo->GetAttackUpgrade() * 5.f;
            
            sFormat = SGLTextUnit::GetText(prop->sDesc2);
            sprintf(sTempDesc2, sFormat,grade);
            
        }
        else if(prop->nID == ID_ITEM_UPGRADE_DEPEND)
        {
            //grade = pUserInfo?pUserInfo->GetDependUpgrade(pUserInfo->GetLastSelectedTankID()) + 1: 1;
            if(nType != 2)
                grade = (pUserInfo->GetDependUpgrade() + 1) * 5.f;
            else
                grade = pUserInfo->GetDependUpgrade() * 5.f;
            
            sFormat = SGLTextUnit::GetText(prop->sDesc2);
            sprintf(sTempDesc2, sFormat,grade);
        }
        else if(prop->nID == ID_ITEM_UPGRADE_MISSILE_TOWER || prop->nID == ID_ITEM_UPGRADE_DEFENCE_TOWER)
        {
            //grade = pUserInfo?pUserInfo->GetDependUpgrade(pUserInfo->GetLastSelectedTankID()) + 1: 1;
            grade = (pUserInfo->GetCntByIDWithItem(prop->nID) + 1) * 5.f;
            sFormat = SGLTextUnit::GetText(prop->sDesc2);
            sprintf(sTempDesc2, sFormat,grade);
        }
        else
            strcpy(sTempDesc2, prop->sDesc2);
        //-------------------------------------------
        
        //Desc2_2 (설명이 두줄 두번째)
        lstImage.clear();
        lstImage.push_back("none");
        lstImage.push_back(sTempDesc2);
        //lstImage.push_back("none");
        CControl* pDesc22 = new CLabelCtl(pItemDescBack,mpFrameBarTexureMan);
        layout.clear();
        PROPERTYI::GetPropertyFloatList("ICe_Desc22", layout);
        pDesc22->Initialize(9999, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
        pDesc22->SetTextColor(fWhite);
        pItemDescBack->AddControl(pDesc22);
    }
    else
    {
        //Desc1 (설명이 한줄)
        lstImage.clear();
        lstImage.push_back("none");
        lstImage.push_back(prop->sDesc);
        CControl* pDesc11 = new CLabelCtl(pItemDescBack,mpFrameBarTexureMan);
        layout.clear();
        PROPERTYI::GetPropertyFloatList("ICe_Desc11", layout);
        pDesc11->Initialize(0, fOffsetX + layout[0], fOffsetY + layout[1], layout[2], layout[3], lstImage, 0.f, 0.f, 0.f, 0.f);
        pDesc11->SetTextColor(fWhite);
        pItemDescBack->AddControl(pDesc11);
        
    }
}





void CHWorld::ShowBuyDlg(int nID,int nType)
{
    float fWhite[] = {1.f,1.f,1.f,1.f};
    if(mpBuyDlg)
    {
        delete[] mpBuyDlg;
        mpBuyDlg = NULL;
    }
    
    vector<string>  lstImage;
    vector<float> layout;
    vector<float> layout0;
    lstImage.clear();
    lstImage.push_back(IMG_TABLE_BANDI);
    lstImage.push_back("none");
    
    
    //테이블 백그라운드 319, 157, 393, 341 의 테이블 백그라운드
    layout0.clear();
    PROPERTYI::GetPropertyFloatList("BD_BuyDlg0", layout0);
    layout.clear();
    PROPERTYI::GetPropertyFloatList("BD_BuyDlg", layout);
    
    mpBuyDlg = new CLabelCtl(NULL,mpFrameBarTexureMan);
    float fX = (layout0[2] - layout[2]) / 2.0f + layout0[0];
    float fY = (layout0[3] - layout[3]) / 2.0f + layout0[1];
    
    if( gnDisplayType == DISPLAY_IPHONE)
    {
        fX = (gDisplayWidth - layout[2]) / 2.f + layout[0];
        fY = (gDisplayHeight - layout[3]) / 2.f+ layout[1];
    }
    
    
    mpBuyDlg->Initialize(nType, fX, fY, layout[2], layout[3], lstImage, 0.0068359375, 0.01171875, 0.3564453125, 0.20703125);
    
    
    mpBuyDlg->SetLData(nID);
    
    float fXT,fYT;
    
    if(nType == 0) //Bomb
    {
        PROPERTY_BOMB Prop;
        PROPERTY_BOMB::GetPropertyBomb(nID, Prop);
        layout.clear();
        PROPERTYI::GetPropertyFloatList("BD_BombCell", layout);
        fXT = fX + layout[2];
        fYT = fY + layout[3];
        
        InitBombCell(&Prop, mpBuyDlg, fXT, fYT, 0);
        
        lstImage.clear();
        lstImage.push_back("gold.png");
        lstImage.push_back("none"); //B_Dlg_Gold
        CControl* pGoldLabel = new CLabelCtl(mpBuyDlg,mpFrameBarTexureMan);
        layout.clear();
        PROPERTYI::GetPropertyFloatList("BD_GoldLabel", layout);
        fXT = fX + layout[0];
        fYT = fY + layout[1];
        pGoldLabel->Initialize(0, fXT, fYT, layout[2], layout[3], lstImage, 0.0f, 0.0f, 1.0f, 1.0f);
        pGoldLabel->SetTextColor(fWhite);
        mpBuyDlg->AddControl(pGoldLabel);
        
        
        lstImage.clear();
        lstImage.push_back("none");
        lstImage.push_back("none");
        CNumberCtl* pGold = new CNumberCtl(mpBuyDlg,mpFrameBarTexureMan,true);
        layout.clear();
        PROPERTYI::GetPropertyFloatList("BD_Gold", layout);
        fXT = fX + layout[0];
        fYT = fY + layout[1];
        pGold->Initialize(0, fXT, fYT, layout[2], layout[3],layout[4],layout[5],layout[6], lstImage);
        pGold->SetTextColor(fWhite);
        pGold->SetAlign(CNumberCtl::ALIGN_LEFT);
        pGold->SetNumber(Prop.nPrice);
        mpBuyDlg->AddControl(pGold);
        
        
        lstImage.clear();
        lstImage.push_back(IMG_TABLE_BANDI);
        lstImage.push_back("M_IB_OK");
        CControl* pBtnOK = new CButtonCtl(mpBuyDlg,mpFrameBarTexureMan);
        layout.clear();
        PROPERTYI::GetPropertyFloatList("BD_BtnOK", layout);
        fXT = fX + layout[0];
        fYT = fY + layout[1];
        pBtnOK->Initialize(BTN_BOMBBUYOK, fXT, fYT, layout[2], layout[3], lstImage, 0.3623046875, 0.0908203125, 0.4814453125, 0.1279296875);
        pBtnOK->SetLData(nID);
        pBtnOK->SetTextColor(fWhite);
        mpBuyDlg->AddControl(pBtnOK);
        
        
        lstImage.clear();
        lstImage.push_back(IMG_TABLE_BANDI);
        lstImage.push_back("M_IB_CANCEL");
        CControl* pBtnCancel = new CButtonCtl(mpBuyDlg,mpFrameBarTexureMan);
        layout.clear();
        PROPERTYI::GetPropertyFloatList("BD_BtnCancel", layout);
        fXT = fX + layout[0];
        fYT = fY + layout[1];
        pBtnCancel->Initialize(BTN_ITEMBOMBBUYCANCEL, fXT, fYT, layout[2], layout[3], lstImage, 0.3623046875, 0.1318359375, 0.478515625, 0.1767578125);
        pBtnCancel->SetTextColor(fWhite);
        mpBuyDlg->AddControl(pBtnCancel);
    }
    else if(nType == 1) //Item
    {
        CUserInfo* pUserInfo = mpScenario->GetUserInfo();
        PROPERTY_ITEM Prop;
        PROPERTY_ITEM::GetPropertyItem(nID, Prop);
        
        if(!(Prop.nType == 4 || Prop.nType == 5))
        {
            
            //        layout.clear();
            //        PROPERTYI::GetPropertyFloatList("BD_GoldLabel", layout);
            //        fXT = fX + layout[2];
            //        fYT = fY + layout[3];
            
            layout.clear();
            PROPERTYI::GetPropertyFloatList("BD_BombCell", layout);
            fXT = fX + layout[2];
            fYT = fY + layout[3];
            
            //0 은 popup dlg에서 부름
            InitItemCell(&Prop, mpBuyDlg, fXT, fYT, 0);
            
            lstImage.clear();
            lstImage.push_back("gold.png");
            lstImage.push_back("none"); //B_Dlg_Gold
            CControl* pGoldLabel = new CButtonCtl(mpBuyDlg,mpFrameBarTexureMan);
            layout.clear();
            PROPERTYI::GetPropertyFloatList("BD_GoldLabel", layout);
            fXT = fX + layout[0];
            fYT = fY + layout[1];
            pGoldLabel->Initialize(0, fXT, fYT, layout[2], layout[3], lstImage, 0.0f, 0.0f, 1.0f, 1.0f);
            pGoldLabel->SetTextColor(fWhite);
            mpBuyDlg->AddControl(pGoldLabel);
            
            
            int nGold = Prop.nPrice;
            if(Prop.nID == ID_ITEM_UPGRADE_ATTACK)
            {
                int grade = pUserInfo?pUserInfo->GetAttackUpgrade() + 1: 1;
                nGold = Prop.nPrice * grade;
            }
            else if(Prop.nID == ID_ITEM_UPGRADE_DEPEND)
            {
                int grade = pUserInfo?pUserInfo->GetDependUpgrade() + 1: 1;
                nGold = Prop.nPrice * grade;
            }
            else if(Prop.nID == ID_ITEM_UPGRADE_DEFENCE_TOWER || Prop.nID == ID_ITEM_UPGRADE_MISSILE_TOWER)
            {
                int grade = pUserInfo->GetCntByIDWithItem(Prop.nID) + 1;
                nGold = Prop.nPrice * grade;
            }
            
            
            lstImage.clear();
            lstImage.push_back("none");
            lstImage.push_back("none");
            CNumberCtl* pGold = new CNumberCtl(mpBuyDlg,mpFrameBarTexureMan,true);
            layout.clear();
            PROPERTYI::GetPropertyFloatList("BD_Gold", layout);
            fXT = fX + layout[0];
            fYT = fY + layout[1];
            pGold->Initialize(0, fXT, fYT, layout[2], layout[3],layout[4],layout[5],layout[6], lstImage);
            pGold->SetTextColor(fWhite);
            pGold->SetAlign(CNumberCtl::ALIGN_LEFT);
            pGold->SetNumber(nGold);
            mpBuyDlg->AddControl(pGold);
            
            
            lstImage.clear();
            lstImage.push_back(IMG_TABLE_BANDI);
            lstImage.push_back("M_IB_OK");
            CControl* pBtnOK = new CButtonCtl(mpBuyDlg,mpFrameBarTexureMan);
            layout.clear();
            PROPERTYI::GetPropertyFloatList("BD_BtnOK", layout);
            fXT = fX + layout[0];
            fYT = fY + layout[1];
            pBtnOK->Initialize(BTN_ITEMBUYOK, fXT, fYT, layout[2], layout[3], lstImage, 0.3623046875, 0.0908203125, 0.4814453125, 0.1279296875);
            pBtnOK->SetLData(nID);
            pBtnOK->SetTextColor(fWhite);
            mpBuyDlg->AddControl(pBtnOK);
            
            
            lstImage.clear();
            lstImage.push_back(IMG_TABLE_BANDI);
            lstImage.push_back("M_IB_CANCEL");
            CControl* pBtnCancel = new CButtonCtl(mpBuyDlg,mpFrameBarTexureMan);
            layout.clear();
            PROPERTYI::GetPropertyFloatList("BD_BtnCancel", layout);
            fXT = fX + layout[0];
            fYT = fY + layout[1];
            pBtnCancel->Initialize(BTN_ITEMBOMBBUYCANCEL, fXT, fYT, layout[2], layout[3], lstImage, 0.3623046875, 0.1318359375, 0.478515625, 0.1767578125);
            pBtnCancel->SetTextColor(fWhite);
            mpBuyDlg->AddControl(pBtnCancel);
            
        }
        else
        {
            if(mpBuyDlg)
            {
                delete mpBuyDlg;
                mpBuyDlg = NULL;
            }
            
#if defined(APPSTOREKIT) || defined(ANDROIDAPPSTOREKIT)
            //직접 돈주고 사게 해준다.
            //#ifdef ANDROIDAPPSTOREKIT
            //            //kr.co.songhh.AlienInvasion.testproductid
            //            //kr.co.songhh.AlienInvasion.gold.1000
            //            gShowShopDlg(ANDROID_PRODUCT_ID,(void*)this);
            //#else
            string sID = GetBundleID();
            sID.append(".");
            sID.append(Prop.sID);
            gShowShopDlg(sID.c_str(),(void*)this,Prop.nType); //SongGL프로젝트는 NOFREE가 없다.
            //#endif //ANDROIDAPPSTOREKIT
#endif //defined(APPSTOREKIT) || defined(ANDROIDAPPSTOREKIT)
        }
    }
    
}


#ifdef ACCELATOR
void CHWorld::SetAccelatorUpVector(float fv)
{
    CSprite* pActor = GetActor();
    if(pActor)
        pActor->SetAccelatorUpVector(fv);
}
#endif //ACCELATOR

bool CHWorld::OnPurchasedProductItem(int nID)
{
    PROPERTY_ITEM Prop;
    PROPERTY_ITEM::GetPropertyItem(nID, Prop);
    CUserInfo* pUserInfo = mpScenario->GetUserInfo();
    
    pUserInfo->SetLastBombItemSwitch(1);
    
    pUserInfo->AddListToItem(nID,Prop.nType); //이미존재하면
    if(Prop.nType == 5) //영구이면 테이블의 로를 지운다.
    {
        RemveItemTableRow(nID);
    }
    int nPage = VisibleDisplayItemSlotPage(Prop.nID);
    pUserInfo->SetLastSelectedItemPageIndex(nPage);
    
    //스롯을 다시그려준다.
    ResetDisplayItemSlot();
    
    AnimationDisplayItemSlot(nID);
    
    if(nID == ID_ITEM_ATTACKBYRADA)
        mpRadaBoard->SetHide(false); //레이더 아이템이 없으면 레이다를 보이지 않게 한다.
    else if(nID == ID_ITEM_UPGRADE_PACKAGE)
    {
        pUserInfo->AddAttackUpgrade();
        pUserInfo->AddAttackUpgrade();
        pUserInfo->AddAttackUpgrade();
        
        pUserInfo->AddDependUpgrade();
        pUserInfo->AddDependUpgrade();
        pUserInfo->AddDependUpgrade();
        
        GetActor()->ResetPower(); //실시간으로 파워를 적용해준다.
    }
    else if(nID == ID_TIEM_FASTGOLD)
    {
        pUserInfo->AddListToItem(nID,1);
        mfGoldPerScore = GoldPerScore * 3.0f;
    }
    else if(nID == ID_TIEM_FASTBUILD)
    {
        pUserInfo->AddListToItem(nID,1);
        mbFastBuild = true;
    }
    
    pUserInfo->Save();
    //    GetSGLCore()->PlaySystemSound(SOUND_ID_EnergyUP);
    return true;
    
}

bool CHWorld::OnPurchasedItem(int nID)
{
    int grade = 1;
    PROPERTY_ITEM Prop;
    PROPERTY_ITEM::GetPropertyItem(nID, Prop);
    CUserInfo* pUserInfo = mpScenario->GetUserInfo();
    
    int nGold = pUserInfo->GetGold();
    int nPrice = Prop.nPrice;
    
    
    pUserInfo->SetLastBombItemSwitch(1);
    
    
    
    //Added By Song 2013.02.14 업그레이드 돈을 수정하였다.
    if(Prop.nType == 2) //Upgrade
    {
        if(Prop.nID == ID_ITEM_UPGRADE_ATTACK) //AttackUpgrade
            grade = pUserInfo->GetAttackUpgrade() + 1;
        else if(Prop.nID == ID_ITEM_UPGRADE_DEPEND) //AttackUpgrade
            grade = pUserInfo->GetDependUpgrade() + 1;
        else if(Prop.nID == ID_ITEM_UPGRADE_DEFENCE_TOWER || Prop.nID == ID_ITEM_UPGRADE_MISSILE_TOWER)
            grade = pUserInfo->GetCntByIDWithItem(Prop.nID) + 1;
        nPrice = Prop.nPrice * grade;
    }
    
    if(nGold >= nPrice)
    {
        nGold -= nPrice;
        pUserInfo->SetGold(nGold);
        
        //화면에 정의 해준다.
        ResetDisplayTopGold();
        
        //무기 슬롯테이블에 넣어준다.
        pUserInfo->AddListToItem(nID,Prop.nType); //이미존재하면
        if(Prop.nType == 1) //영구이면 테이블의 로를 지운다.
        {
            RemveItemTableRow(nID);
            
            if(Prop.nID == ID_ITEM_ATTACKBYRADA)
                mpRadaBoard->SetHide(false); //레이더 아이템이 없으면 레이다를 보이지 않게 한다.
            else if(Prop.nID == ID_ITEM_UPGRADE_MAGNETIC)
                mnMagnet = 1;
        }
        else if(Prop.nType == 2) //Upgrade 이면
        {
            if(Prop.nID == ID_ITEM_UPGRADE_ATTACK) //AttackUpgrade
            {
                pUserInfo->AddAttackUpgrade();
                GetActor()->ResetPower(); //실시간으로 파워를 적용해준다.
            }
            else if(Prop.nID == ID_ITEM_UPGRADE_DEPEND)
            {
                pUserInfo->AddDependUpgrade();
                GetActor()->ResetPower(); //실시간으로 파워를 적용해준다.
            }
            else if(Prop.nID == ID_ITEM_UPGRADE_DEFENCE_TOWER || Prop.nID == ID_ITEM_UPGRADE_MISSILE_TOWER)
            {
                //                int nCnt = pUserInfo->GetCntByIDWithItem(Prop.nID); //Max 2단계 업그레이드 이면 리스트에서 지운다.
                //                if(nCnt >= 2)
                //                    RemveItemTableRow(nID);
                mnNeedDefenceTowerUpgrade = Prop.nID; //객체의 업그레이드가 필요하다.
            }
        }
        
        int nPage = VisibleDisplayItemSlotPage(Prop.nID);
        pUserInfo->SetLastSelectedItemPageIndex(nPage);
        
        //스롯을 다시그려준다.
        ResetDisplayItemSlot();
        
        //아이템 테이블의 현재 리소스에 바로적용해준다.
        if(mpFrameItembTable && Prop.nType == 2) //Upgrade
        {
            char sTempDesc2[128];
            const char *sFormat;
            int nRowCnt = mpFrameItembTable->GetRowCnt();
            for (int i = 0; i < nRowCnt; i++)
            {
                int nID = (int)mpFrameItembTable->GetRowCtl(i)->GetLData();
                if(nID == Prop.nID)
                {
                    
                    CControl* pDesc = mpFrameItembTable->GetRowCtl(i)->FindControl(9999);
                    sFormat = SGLTextUnit::GetText(Prop.sDesc2);
                    grade = grade + 1;
                    //                    if(nID == ID_ITEM_UPGRADE_DEFENCE_TOWER || nID == ID_ITEM_UPGRADE_MISSILE_TOWER)
                    //                        sprintf(sTempDesc2, sFormat,grade * 50);
                    //                    else
                    sprintf(sTempDesc2, sFormat,grade * 5);
                    
                    vector<string> lstImage;
                    lstImage.push_back("none");
                    lstImage.push_back(sTempDesc2);
                    pDesc->SetImageData(lstImage);
                    
                    CNumberCtl* pGold = (CNumberCtl*)mpFrameItembTable->GetRowCtl(i)->FindControl(9998);
                    pGold->SetNumber(Prop.nPrice * grade);
                    break;
                }
            }
        }
        
        if(Prop.nID == ID_ITEM_FILL_POWER)
        {
            CSprite* pActor = GetActor();
            if(pActor && pActor->GetState() != SPRITE_RUN)
            {
                int nLiveCnt = pActor->GetAliveCount();
                mpAliveNumCtl->SetHide(false);
                mpAliveNumCtl->SetNumber(nLiveCnt);
            }
        }
        
        pUserInfo->Save();
        AnimationDisplayItemSlot(nID);
        GetSGLCore()->PlaySystemSound(SOUND_ID_EnergyUP);
    }
    else
    {
        //돈이 부족하다.
        GetSGLCore()->PlaySystemSound(SOUND_ID_Buzzer);
        mpBuyDlg->SetAni(CONTROLANI_WIGWAG,0.4);
        return false;
    }
    return true;
    
}

void CHWorld::RemveItemTableRow(int nID)
{
    int nIndex = -1;
    int nCnt = mpFrameItembTable->GetRowCnt();
    for(int i = 0; i < nCnt; i++)
    {
        if(mpFrameItembTable->GetRowCtl(i)->GetLData() == nID)
        {
            nIndex = i;
            break;
        }
    }
    if(nIndex != -1)
    {
        mpFrameItembTable->RemoveItem(nIndex);
    }
    //            mpItemTableCtl->RemoveItem(mpBuyPopupCtl->GetRow());
    //            mpBuyPopupCtl->SetAni(CONTROLANI_ZOOM_IN_HIDE,0.5); //다이얼로그 박스를 살아지게 한다.
    
}

void CHWorld::ResetDisplayTopGold()
{
    CUserInfo* pUserInfo = GetUserInfo();
    ((CNumberCtl*)mpGoldBar->GetChild(0))->SetNumber(pUserInfo->GetGold());
}

int CHWorld::VisibleDisplayItemSlotPage(int nID)
{
    vector<string>  lstImage;
    const int nMaxItem= MAXSLOTCNT;
    CUserInfo* pUserInfo = mpScenario->GetUserInfo();
    
    int nType = pUserInfo->GetLastBombItemSwitch();
    
    vector<USERINFO*>* lstItem = NULL;
    if(nType == 1)
        lstItem = pUserInfo->GetListItem();
    else
        lstItem = pUserInfo->GetListBomb();
    
    int nIndex = 0;
    for(int i = 0; i < lstItem->size(); i++)
    {
        if(lstItem->at(i)->nID == nID)
        {
            nIndex = i;
            break;
        }
    }
    return (nIndex / nMaxItem) + 1;
}


void CHWorld::AnimationDisplayItemSlot(int nID)
{
    for(int i = CTL_TABLE_ITEM_SLOT3; i <= CTL_TABLE_ITEM_SLOT6; i++)
    {
        CControl* pCtl = mpItemSlotBar->FindControl(i);
        if(pCtl->GetLData() == -1 ) continue;
        USERINFO* pInfo = (USERINFO*)pCtl->GetLData();
        if(pInfo && pInfo->nID == nID)
        {
            pCtl->SetAni(CONTROLANI_WIGWAG,0.5f);
            break;
        }
    }
}

bool CHWorld::OnPurchasedBomb(int nID)
{
    PROPERTY_BOMB Prop;
    PROPERTY_BOMB::GetPropertyBomb(nID, Prop);
    CUserInfo* pUserInfo = mpScenario->GetUserInfo();
    
    int nGold = pUserInfo->GetGold();
    int nPrice = Prop.nPrice;
    
    pUserInfo->SetLastBombItemSwitch(0);
    
    
    
    if(nGold >= nPrice)
    {
        nGold -= nPrice;
        pUserInfo->SetGold(nGold);
        
        //화면에 정의 해준다.
        ResetDisplayTopGold();
        
        
        //무기 슬롯테이블에 넣어준다.
        pUserInfo->AddListToBomb(nID,Prop.nPackCount); //이미존재하면
        int nPage = VisibleDisplayItemSlotPage(Prop.nID);
        pUserInfo->SetLastSelectedBombPageIndex(nPage);
        ResetDisplayItemSlot();
        pUserInfo->Save();
        
        AnimationDisplayItemSlot(nID);
        GetSGLCore()->PlaySystemSound(SOUND_ID_EnergyUP);
    }
    else
    {
        //돈이 부족하다.
        GetSGLCore()->PlaySystemSound(SOUND_ID_Buzzer);
        mpBuyDlg->SetAni(CONTROLANI_WIGWAG,0.3);
        return false;
    }
    return true;
}

void CHWorld::Serialize(CArchive& ar,bool bWrite)
{
    CSprite* pSprite = NULL;
    CSGLCore *pSGLCore = GetSGLCore();
    CListNode<CSprite> *pCoreListSprite = pSGLCore->GetSpriteList();
    CListNode<CBomb> *pCoreListBomb = pSGLCore->GetBombList();
    if(bWrite)
    {
        
        //        ar << mbNeedDefenceTowerUpgrade;
        ar << mnBonus;
        ar << mfScore;
        
        for (ByNode<CSprite>* it2 = pCoreListSprite->begin(); it2 != pCoreListSprite->end(); )
        {
            pSprite = it2->GetObject();
            it2 = it2->GetNextNode();
            
            if( pSprite->GetState() == SPRITE_RUN)
                pSprite->Serialize(ar, true);
        }
        for (ByNode<CBomb>* it2 = pCoreListBomb->begin(); it2 != pCoreListBomb->end(); )
        {
            pSprite = it2->GetObject();
            it2 = it2->GetNextNode();
            
            if( pSprite->GetState() == SPRITE_RUN)
                pSprite->Serialize(ar, true);
        }
    }
}

CSprite* CHWorld::GetMainTowerNearestInThread(SPoint* ptCenterSprite,int nTeamID)
{
    CSprite* pSprite = NULL;
    CSprite* pFndMainTower = NULL;
    CSGLCore *pSGLCore = GetSGLCore();
    CListNode<CSprite> *pCoreListSprite = pSGLCore->GetSpriteList();
    float fFar = 0.0f;
    float fMinFar = 0.0f;
    SPoint ptS;
    
    
    for (ByNode<CSprite>* it2 = pCoreListSprite->begin(); it2 != pCoreListSprite->end(); )
    {
        pSprite = it2->GetObject();
        it2 = it2->GetNextNode();
        if( pSprite->GetModelID() == ACTORID_BLZ_MAINTOWER &&
           pSprite->GetTeamID() == nTeamID &&
           pSprite->GetState() == SPRITE_RUN)
        {
            pSprite->GetPosition(&ptS);
            
            fFar = (ptCenterSprite->x - ptS.x) * (ptCenterSprite->x - ptS.x) + (ptCenterSprite->z - ptS.z) * (ptCenterSprite->z - ptS.z);
            
            if(fFar < fMinFar || fMinFar == 0.0f)
            {
                pFndMainTower = pSprite;
                fMinFar = fFar;
            }
        }
    }
    if(pFndMainTower == NULL)
        pFndMainTower = GetActor();
    return pFndMainTower;
}


bool SpriteOnIDSort(CSprite* a, CSprite* b)
{
    return a->GetID() < b->GetID(); //내림차순
}


/**
 그릴 객체의 리스트를 쌓는다.
 */
void* CoreThread(void *data)
{
    const float fNeerItem = VISIBLEFARVIEW_DOUBLE * 0.6f;
    const float fFarItem = VISIBLEFARVIEW_DOUBLE;
    
    
    SPoint ptSprite;
    SPoint ptBomb;
    //    SPoint ptImpact;
    float fInterActorX,fInterActorY,fInterLen;
	CBomb* pBomb = NULL;
    AParticle* pParticle = NULL;
	CSprite* pSprite = NULL;
	CSprite* pEnemySprite = NULL;
    CSprite* pOwnerBomb = NULL;
	int nResult = 0;
	CHWorld* pWorld = (CHWorld*)data;
	GLuint nTime = 0;
    SPoint ptActor;
    CSprite *pActor;
    bool bCompactedVSprite;
    
    int   nModelID,nTeamID;
    bool    bCompactCol;
    int nBarPosition;
    
    CSGLCore *pSGLCore = pWorld->GetSGLCore();
    CListNode<CSprite> *pCoreListSprite = pSGLCore->GetSpriteList();
    CListNode<CBomb> *pCoreListBomb = pSGLCore->GetBombList();
    CListNode<AParticle> *pCoreListParticle = pSGLCore->GetParticleList();
	
	CListNode<CSprite> *plistSprite = NULL;
	CListNode<CBomb>   *plistBomb   = NULL;
    CListNode<RadaBoardObject> *plistBoardObjects = NULL;
    CListNode<AParticle> *plistParticle = NULL;
    CMutiplyProtocol* pMutiplay = pWorld->mpScenario->GetMutiplyProtocol();
    CListNode<CArchive> lstNeedAllTankData;
    unsigned int nMultiplayCmd;
	RuntimeRenderObjects *pRuntimeObjects = NULL;
    
#ifndef OLD_SP
    //소팅을 하는 이유는 멀티플레이를 할때.. 적들을 공격하는 순서를 맞추기위해서 한다.
    vector<CSprite*> sortSpriteOnID; //ID로 소팅된..
    vector<CSprite*>::iterator sortSpriteOnIDBegin;
    vector<CSprite*>::iterator sortSpriteOnIDEnd;
    vector<CSprite*>::iterator sortSpriteOnIDIT,sortSpriteOnIDIT2;
#endif
    
    
    
    //맵의 지도 -----
    float fXMaxMapRadian = C2dRadaBoard::MapRate(); //BOARD_CIRCLE_RADIAN_INTERVAL
	while(!pWorld->mpThread->IsStop())
	{
        pWorld->mpThread->BeginPing();
        
#ifndef OLD_SP
        //----------------------------------------------------------------------------------------------------
        // 소팅하장....
        //----------------------------------------------------------------------------------------------------
        sortSpriteOnID.clear();
        for( ByNode<CSprite>* it2 = pCoreListSprite->end()->GetPrevNode(); it2 != &pCoreListSprite->GetHead();)
        {
            sortSpriteOnID.push_back(it2->GetObject());
            it2 = it2->GetPrevNode();
        }
        sort(sortSpriteOnID.begin(),sortSpriteOnID.end(),SpriteOnIDSort);
        sortSpriteOnIDBegin = sortSpriteOnID.begin();
        sortSpriteOnIDEnd = sortSpriteOnID.end();
        //----------------------------------------------------------------------------------------------------
#endif
        
		
        nMultiplayCmd = pMutiplay->GetNeedCommand();
        
		//CoreThread가 먼저 실행되고 BeginRender가 그다음 실행된다.
		nTime = GetClockDelta();
        
        if(pWorld->mRequestTry != 0)
        {
            pWorld->RequestTry(pCoreListSprite, pCoreListBomb, pCoreListParticle);
            continue;
        }
        
		pActor = pWorld->GetActor();
        
		//sglLog("CoreThread=%d\n",nTime);
		pRuntimeObjects = new RuntimeRenderObjects;
        
        plistBoardObjects = new CListNode<RadaBoardObject>;
        
		//2> 캐릭터를 조절한다.
        //2-1> 폭탄
		plistBomb = new CListNode<CBomb>;
        SPRITE_STATE spState;
        
        if(pActor) //주인공이 터져 버렸을 경우 에러가 나지 않게 한다.
        {
            pActor->GetPosition(&ptActor);
            //폭탄이 모득객체와 충돌했는지를 검사한다.
            //그리고 레이다에 위치를 표현해준다.
            for (ByNode<CBomb>*it1 = pCoreListBomb->end()->GetPrevNode(); it1 != &pCoreListBomb->GetHead(); )
            {
                pBomb = it1->GetObject();
                spState = pBomb->GetState();
                pBomb->GetPosition(&ptBomb);
                
                //Modified By Song 2014.08.14 SPRITE_RUN일때만 하면 SPRITE_BOMB_NEXT_COMPACT이후에 시간에 오너가 죽으면
                //거시기 하죠잉
                pBomb->RenderBeginCore(nTime);
                
                if( spState == SPRITE_RUN)
                {
                    //땅이나 범위밖으로 변경되는지를 파악한다.
                    pBomb->CheckCollidesToTerrian();
                    //pBomb->RenderBeginCore(nTime);
                }
                else
                {
                    if(spState == SPRITE_READYDELETE)
                    {
                        //여기서 지워지면 model render를 할때 쓰레드로 거의 동시에 이루어지기 때문에
                        //에러가 날 소지가 있다.. 확실하게 한프레임을 넘겨준다.
                        //폰탄은 리스트에서 지워질 예정이기때문에 ..
                        pBomb->SetState(SPRITE_DELETED);
                        
                        //내가 쏜 폭탄일 터질때 주위에 적탱크가 존재하는지 체크한다.
                        pWorld->CheckAttectedByEnSpriteAtBomb(pCoreListSprite,pBomb);
                        
                        pOwnerBomb = pBomb->GetOwner();
                        if(pOwnerBomb)
                            pOwnerBomb->Command(AICOMMAND_BOMBED, pBomb); //일단 탄착지점의 카운터를 높인다.
                        
                        it1 = it1->GetPrevNode();
                        continue;
                    }
                    else if(spState == SPRITE_DELETED)
                    {
                        ByNode<CBomb>* itOrg = it1;
                        it1 = it1->GetPrevNode();
                        delete pBomb; //메모리를 지운다.
                        pCoreListBomb->Delete(itOrg); //리스트에서 영원히 지운다.
                        continue;
                    }
                }
                it1 = it1->GetPrevNode();
                
                if(pBomb->CheckCompact()) //폭탄이 너무 멀리있으면 객체들이 맞았는지 체크할 필요가 없다.
                {
#ifdef OLD_SP
                    for( ByNode<CSprite>* it2 = pCoreListSprite->end()->GetPrevNode(); it2 != &pCoreListSprite->GetHead();)
                    {
                        pSprite = it2->GetObject();
                        it2 = it2->GetPrevNode();
#else
                        for (sortSpriteOnIDIT = sortSpriteOnIDBegin; sortSpriteOnIDIT != sortSpriteOnIDEnd; sortSpriteOnIDIT++)
                        {
                            pSprite = *sortSpriteOnIDIT;
#endif
                            
                            if( pSprite->GetState() == SPRITE_RUN
                               && pSprite->GetTeamID() != pBomb->GetTeamID() //자신의 팀이 맞았을때 터지지 않게 하자.
                               && pSprite != pBomb->GetOwner()
                               && pBomb->CollidesBounding(pSprite,false)
                               )
                            {
                                //레이져 타임이 아니면 바로 스킵한다.
                                if(pBomb->GetBombType() == 0x1002 && pBomb->GetResultRaser() == 0)
                                    pWorld->CheckAttectedByEnSpriteAtRaser(pBomb);
                                pBomb->SetCompactBomb();
                                break;
                            }
                        }
                    }
                    
                    if(!(pBomb->GetBombType() & BOMB_FASET_BOMB_TYPE)) //레이져 같은 빠른 총알은 레이더에 담지 말자.
                    {
                        //레이더 데이터를 만든다.
                        //RADAVISIBLE : fXMaxMapRadian 비율
                        //VISIBLEFARVIEW
                        fInterActorX = - ptBomb.x + ptActor.x;
                        fInterActorY = ptBomb.z - ptActor.z;
                        
                        RadaBoardObject *pNewBoardObject = new RadaBoardObject;
                        pNewBoardObject->nBoardType = 2;
                        pNewBoardObject->fX = fInterActorX * fXMaxMapRadian / RADAVISIBLE + C2dRadaBoard::gBoadCenter.x;
                        pNewBoardObject->fY = fInterActorY * fXMaxMapRadian / RADAVISIBLE + C2dRadaBoard::gBoadCenter.y;
                        
                        pNewBoardObject->pSprite = pBomb;
                        pNewBoardObject->pWorldVertex = new float[12];
                        CControl::MakeWorldStickVertexGL(pNewBoardObject->pWorldVertex,
                                                         C2dRadaBoard::g_BombVertex,
                                                         pNewBoardObject->fX,
                                                         pNewBoardObject->fY);
                        
                        plistBoardObjects->AddTTail(pNewBoardObject); //폭탄
                    }
                    
                    plistBomb->AddTTail(pBomb);
                }
            }
            
            
            
            
            //2-2>캐릭터 ----------------------------------------------------------------------------------------------
            plistSprite = new CListNode<CSprite>;
            
            
#ifdef OLD_SP
            for( ByNode<CSprite>* cit = pCoreListSprite->end()->GetPrevNode(); cit != &pCoreListSprite->GetHead();)
            {
                pSprite = cit->GetObject();
#else
                for (sortSpriteOnIDIT = sortSpriteOnID.begin(); sortSpriteOnIDIT != sortSpriteOnID.end(); )
                {
#endif
                    pSprite = *sortSpriteOnIDIT;
                    
                    fInterLen = 0;
                    spState = pSprite->GetState();
                    pSprite->GetPosition(&ptSprite);
                    
                    if(spState == BOMB_COMPACT)
                    {
                        pSprite->SetState(BOMB_COMPACT_ANI);
#ifdef OLD_SP
                        cit = cit->GetPrevNode();
#else
                        sortSpriteOnIDIT++;
#endif
                        continue;
                    }
                    else
                    {
                        if(spState == SPRITE_READYDELETE)
                        {
                            //-------------------------------------------------------------------------
                            //죽으면 거시기 메세지를 보내서 거시기 해준다.
                            if(pActor->GetTeamID() == pSprite->GetTeamID() && pSprite->GetModelID() >= ACTORID_MISSILE && pSprite->GetModelID() < ACTORID_ASSISTE_END)
                            {
                                CScenario::SendMessage(SGL_KILLED_ASSITANCETANK,pSprite->GetModelID());
                            }
                            //-------------------------------------------------------------------------
                            
                            //여기서 지워지면 model render를 할때 쓰레드로 거의 동시에 이루어지기 때문에
                            //에러가 날 소지가 있다.. 확실하게 한프레임을 넘겨준다.
                            //폰탄은 리스트에서 지워질 예정이기때문에 ..
                            pSprite->SetState(SPRITE_DELETED);
#ifdef OLD_SP
                            cit = cit->GetPrevNode();
#else
                            sortSpriteOnIDIT++;
#endif
                            continue;
                        }
                        else if(spState == SPRITE_DELETED)
                        {
                            
                            if(pActor != pSprite)
                            {
#ifdef OLD_SP
                                cit = cit->GetPrevNode();
#else
                                sortSpriteOnIDIT = sortSpriteOnID.erase(sortSpriteOnIDIT);
                                
#endif
                                //메모리를 지운다. 리스트에서 영원히 지운다.
                                pSGLCore->DelSprite(pSprite);
                                
                                pWorld->GetUserInfo()->mnGetherKillCount += 1; //카운트를 높혀준다.
                            }
                            else //주인공이 죽었을 경우.
                            {
                                //------------------------------------------------
                                //2012.11.11 Added By Song 폭탄의 Target과 Owner를 NULL로 변경하여 주어야 프로그램이 죽지 않음.
                                for (ByNode<CBomb>*it1 = pCoreListBomb->end()->GetPrevNode(); it1 != &pCoreListBomb->GetHead(); )
                                {
                                    pBomb = it1->GetObject();
                                    pBomb->ResetAtKilledActor();
                                    it1 = it1->GetPrevNode();
                                }
                                //------------------------------------------------
                                
                                pWorld->SetActor(NULL);
                                pSGLCore->GetAction()->OnActotorDeleted(pSprite);
                                
                                if(pWorld->mpThread->IsStop() == false)
                                {
                                    if(pSprite->GetWorld()->GetUserInfo() && pSprite->GetWorld()->GetUserInfo()->GetRank() == 0) //훈련소는 주인공이 죽어도 성공으로 처리 하자.
                                    {
                                        CScenario::SendMessage(SGL_MESSAGE_SUCCESS_MISSION,0);
                                    }
                                    else
                                        CScenario::SendMessage(SGL_MESSAGE_DIED_ACTOR);
                                }
#ifdef OLD_SP
                                cit = cit->GetPrevNode();
#else
                                sortSpriteOnIDIT = sortSpriteOnID.erase(sortSpriteOnIDIT);
#endif
                                //메모리를 지운다. 리스트에서 영원히 지운다.
                                pSGLCore->DelSprite(pSprite);
                                //다시 액터를 구해온다.
                                pActor = pWorld->GetActor();
                                
                            }
                            continue;
                        }
                    }
#ifdef OLD_SP
                    cit = cit->GetPrevNode();
#else
                    sortSpriteOnIDIT++;
#endif
                    
                    nModelID = pSprite->GetModelID();
                    nTeamID  = pSprite->GetTeamID();
                    if(pSprite != pActor && pActor != NULL && nModelID != -1)
                    {
                        //액터를 중심 좌표료 만든다.
                        //VISIBLEFARVIEW
                        fInterActorX = - ptSprite.x + ptActor.x;
                        fInterActorY = ptSprite.z - ptActor.z;
                        
                        fInterLen = fInterActorX*fInterActorX + fInterActorY*fInterActorY;
                        pSprite->SetIntervalToCamera(fInterLen); //객체가 카메라와 얼마큼 떨어져 있는지
                        
                        if(fInterLen <= RADAVISIBLE_DOUBLE)
                        {
                            
                            //레이더 데이터를 만든다.
                            //RADAVISIBLE : fXMaxMapRadian 비율
                            RadaBoardObject *pNewBoardObject = new RadaBoardObject;
                            if(nModelID == ACTORID_BLZ_EMAIN)
                                pNewBoardObject->nBoardType = 3;
                            else if(nModelID == ACTORID_BLZ_DMISSILETOWER || nModelID == ACTORID_BLZ_DTOWER || nModelID == ACTORID_BLZ_MAINTOWER ||
                                    nModelID == ACTORID_BLZ_DGUNTOWER)
                            {
                                pNewBoardObject->nBoardType = 5;
                                
                                if(pWorld->mnNeedDefenceTowerUpgrade == pSprite->GetModelID()) //객체의 업그레이가 필요하다.
                                {
                                    pSprite->ResetPower();
                                }
                            }
                            else if(pSprite->GetType() == ACTORTYPE_FIGHTER)
                                pNewBoardObject->nBoardType = 6;
                            else if(pSprite->GetTeamID() == pActor->GetTeamID()) //아군 어시스트
                                pNewBoardObject->nBoardType = 7;
                            else
                                pNewBoardObject->nBoardType = 1;
                            
                            pNewBoardObject->fX = fInterActorX * fXMaxMapRadian / RADAVISIBLE + C2dRadaBoard::gBoadCenter.x;
                            pNewBoardObject->fY = fInterActorY * fXMaxMapRadian / RADAVISIBLE + C2dRadaBoard::gBoadCenter.y;
                            
                            pNewBoardObject->pSprite = pSprite;
                            pNewBoardObject->pWorldVertex = new float[12];
                            
                            CControl::MakeWorldStickVertexGL(pNewBoardObject->pWorldVertex,
                                                             C2dRadaBoard::g_EnamyTankVertex,
                                                             pNewBoardObject->fX,
                                                             pNewBoardObject->fY);
                            
                            plistBoardObjects->AddTTail(pNewBoardObject);
                        }
                    }
                    else if(pSprite == pActor ) //탄착지점을 알아온다.
                    {
                        //에너지바와 FirB를 계산하여준다.
                        nBarPosition = pSprite->GetCurrentDefendPower() / (float)pSprite->GetMaxDefendPower() * 100;
                        if(nBarPosition < 0) nBarPosition = 0;
                        pWorld->mpLeftToolsEnergyBar->SetPosition(nBarPosition);
                        
                        nBarPosition = pSprite->GetCurrentAttackPerTime() / (float)pSprite->GetMaxAttackTime() * 100;
                        if(nBarPosition < 0) nBarPosition = 0;
                        pWorld->mpLeftToolsFireBar->SetPosition(nBarPosition);
                        
                    }
                    else if(nModelID == -1 && pSprite != pActor && pActor != NULL)
                    {
                        //액터를 중심 좌표료 만든다.
                        //VISIBLEFARVIEW
                        fInterActorX = - ptSprite.x + ptActor.x;
                        fInterActorY = ptSprite.z - ptActor.z;
                        fInterLen = fInterActorX*fInterActorX + fInterActorY*fInterActorY;
                        pSprite->SetIntervalToCamera(fInterLen); //객체가 카메라와 얼마큼 떨어져 있는지
                    }
                    
                    
                    //Added By Song 2013.04.07 BombAni메이션이 존재하면 종종에러가 난다. 일단은 spState == Run일때만 로직을 처리하자.
                    if(spState == SPRITE_RUN)
                    {
                        bCompactedVSprite = false;
                        
#ifdef OLD_SP
                        for (ByNode<CSprite>* it3 = pCoreListSprite->end()->GetPrevNode(); it3 != &pCoreListSprite->GetHead();)
                        {
                            pEnemySprite = it3->GetObject();
                            it3 = it3->GetPrevNode();

#else
                            sortSpriteOnIDBegin = sortSpriteOnID.begin();
                            sortSpriteOnIDEnd = sortSpriteOnID.end();
                            for (sortSpriteOnIDIT2 = sortSpriteOnIDBegin; sortSpriteOnIDIT2 !=sortSpriteOnIDEnd; sortSpriteOnIDIT2++)
                            {
                                pEnemySprite = *sortSpriteOnIDIT2;
#endif
                                
                                if(pEnemySprite->GetState() != SPRITE_RUN || pEnemySprite == pSprite || nModelID == -1) continue;
                                
                                //건물끼리는 충돌 체크를 하지 말자.
                                if(!(nModelID == ACTORID_BLZ_EMAIN && pEnemySprite->GetType() == ACTORTYPE_TYPE_BLZ))
                                    bCompactCol = pSprite->CollidesBounding(pEnemySprite,false);
                                else
                                    bCompactCol = false;
                                
                                
                                if(bCompactCol)
                                {
                                    if(!(pEnemySprite->GetType() & ACTORTYPE_TYPE_GHOSTANITEM) && bCompactedVSprite == false && (pSprite->isMoving() || pSprite->isTurning()))
                                    {
                                        if(pSprite != pActor && pEnemySprite != pSprite && (pEnemySprite->isMoving() || pEnemySprite->isTurning()))
                                            pEnemySprite->Command(AICOMMAND_COMPACTTOSPRITE, pSprite);
                                        
                                        pSprite->Command(AICOMMAND_COMPACTTOSPRITE, pEnemySprite);
                                        bCompactedVSprite = true;
                                    }
                                }
                                
                                
                                //CGhostSprite ...
                                //메인타워는 자기 팀만 에너지 업을 해준다.
                                if(nTeamID == pEnemySprite->GetTeamID() && (nModelID == ACTORID_BLZ_MAINTOWER || nModelID == ACTORID_AMBULANCE ) &&
                                   !(pEnemySprite->GetType() & ACTORTYPE_TYPE_GHOSTANITEM)) //같은 팀이고 객체가 메인타워 일경우.. 내부에 존재하는지 안하는지를 알아온다.
                                {
                                    if (pSprite->GetColidedDistance() <= pSprite->GetDefendRadianBounds())
                                        pSprite->Command(AICOMMAND_MOVEIN, pEnemySprite);
                                    else
                                        pSprite->Command(AICOMMAND_MOVEOUT, pEnemySprite);
                                }
                                else if(pWorld->IsAllians(nTeamID, pEnemySprite->GetTeamID()) == false &&
                                        !(pEnemySprite->GetType() & ACTORTYPE_TYPE_GHOSTANITEM)
                                        ) //같은 팀이 아닐경우 공격에 대한 정보를 보낸다.
                                {
                                    if (pSprite->GetColidedDistance() <= pSprite->GetDefendRadianBounds())
                                        pSprite->Command(AICOMMAND_MOVEIN, pEnemySprite);
                                    else
                                        pSprite->Command(AICOMMAND_MOVEOUT, pEnemySprite);
                                }
                            }
                        
                        //충돌하지 않았따.
                        if(bCompactedVSprite == false)
                            pSprite->Command(AICOMMAND_COMPACTTOSPRITE, 0);
                        
                    }
                    
                    
                    if(fInterLen <= VISIBLEFARVIEW_DOUBLE) //너무 멀리있는것은 렌더링을 하지 말자.
                    {
                        if(pSprite->VisibleByCamera())
                        {
                            //--------------------------------------------------------------------------
                            //아이템이면 Actor가 아이템 자석을 가지고 있다면, 거리안쪽에 존재하면 아이템을 끌려오게 한다.
                            if(pSprite->GetType() == ACTORTYPE_TYPE_ITEM )
                            {
                                if(pWorld->mnMagnet == 0 && fInterLen <= fNeerItem)
                                    pSprite->SetMoveVelocity(1.0f);
                                else if(fInterLen <= fFarItem) //pWorld->mnMagnet == -999 (마그네틱은 영구적이기 때문에 -999)를 리턴한다.
                                    pSprite->SetMoveVelocity(1.0f);
                            }
                            //--------------------------------------------------------------------------
                            
                            pSprite->RenderBeginCore(nTime);
                            pSprite->SetVisibleRender(true);
                            plistSprite->AddTTail(pSprite);
                        }
                        else
                        {
                            pSprite->SetVisibleRender(false);
                            pSprite->RenderBeginCore_Invisiable(nTime);
                        }
                    }
                    else
                    {
                        pSprite->SetVisibleRender(false);
                        pSprite->RenderBeginCore_Invisiable(nTime);
                    }
                    
                    
                    if(!pMutiplay->IsStop() && nMultiplayCmd)
                    {
                        //나의 모든 탱크의 정보를 보낸다.
                        if(nMultiplayCmd & CHWORLD_INTHREAD_MULTIPLAY_NEEDALLTANKDATA)
                        {
                            if(
                               pSprite->GetNetworkTeamType() == NETWORK_MYSIDE &&
                               !(pSprite->GetType() & ACTORTYPE_TYPE_GHOSTANITEM) &&
                               pSprite->GetModelID() != ACTORID_BLZ_EMAIN //메인타워는 동기화를 하지 않는다.
                               )
                            {
                                CArchive* pNew = new CArchive();
                                pSprite->ArchiveMultiplay(SUBCMD_OBJ_ALL_INFO, *pNew, true);
                                lstNeedAllTankData.AddTTail(pNew);
                            }
                        }
                    }
                }
                
                
                if(nMultiplayCmd)
                {
                    //나의 모든 탱크의 정보를 보낸다.
                    if(nMultiplayCmd & CHWORLD_INTHREAD_MULTIPLAY_NEEDALLTANKDATA)
                    {
                        //내부에서 lstNeedAllTankDatad의 데이터를 보내고 지운다(Clear).
                        pMutiplay->SendSyncObjAll(&lstNeedAllTankData);
                        pMutiplay->SetNeedCommand(pMutiplay->GetNeedCommand() & ~CHWORLD_INTHREAD_MULTIPLAY_NEEDALLTANKDATA);
                    }
                }
                
                //2-2>캐릭터 End----------------------------------------------------------------------------------------------
                
                //2-1> 파티클 
                //int nPCnt = pCoreListParticle->Size();
                plistParticle = new CListNode<AParticle>;
                SPRITE_STATE paState;
                //for (int z = nPCnt - 1; z >=0 ; z--)
                for (ByNode<AParticle>* pit = pCoreListParticle->end()->GetPrevNode(); pit != &pCoreListParticle->GetHead();)
                {
                    pParticle = pit->GetObject();
                    paState = pParticle->GetState();
                    //ptBomb = pParticle->GetPosition();
                    
                    if(paState == SPRITE_READYDELETE) 
                    {
                        //여기서 지워지면 model render를 할때 쓰레드로 거의 동시에 이루어지기 때문에 
                        //에러가 날 소지가 있다.. 확실하게 한프레임을 넘겨준다.
                        //폰탄은 리스트에서 지워질 예정이기때문에 ..
                        pParticle->SetState(SPRITE_DELETED);
                        pit = pit->GetPrevNode();
                        continue;
                    }
                    else if(paState == SPRITE_DELETED)
                    {
                        ByNode<AParticle>* itOrg = pit;
                        
                        pit = pit->GetPrevNode();
                        //메모리를 지운다.
                        pCoreListParticle->Delete(itOrg); //리스트에서 영원히 지운다.
                        delete pParticle; //Added By Song 2014.03.14 Memory Leak이 났었다. 허허허.
                        continue;
                    }
                    pit = pit->GetPrevNode();
                    
                    pParticle->RenderBeginCore(nTime);
                    plistParticle->AddTTail(pParticle);		
                }
                
                pSGLCore->mpItemMan->BeginCore(nTime);
                
                pRuntimeObjects->plistBomb = plistBomb;
                pRuntimeObjects->plistSprite = plistSprite;
                pRuntimeObjects->plistBoardObjects = plistBoardObjects;
                pRuntimeObjects->plistParticle = plistParticle;
                
                pWorld->mnNeedDefenceTowerUpgrade = -1;
                
                //3> 화면에 그리고자하는 객체를 Add 한다.
                pWorld->mpThreadQueue->Offer((int*)pRuntimeObjects);
                
                //다른 프로세스에서 일을 하게끔 순서를 넘겨준다.
                pWorld->mpThread->EndPing();
                
            }
            
            
            //EndPing이 안된상태에서 거시기 해버리면 멈춰버린다.
            pWorld->mpThread->EndPing();
            HLog("Closed Thread\n");
            return (void*)(long)nResult;
        }
        
        
