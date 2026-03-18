//
//  CFighter.cpp
//  SongGL
//
//  Created by itsme on 2013. 12. 17..
//
//
#include <math.h>
#include "CHWorld.h"
#include "CSGLCore.h"
#include "CFighter.h"
#include "CMS3DModelASCII.h"
#include "CMS3DModelASCIIMan.h"
#include "CFighterTailParticle.h"
#include "CShadowParticle.h"
#include "CFighterMoveAI.h"
#include "CAICore.h"
#include "CFigtherStrategyAI.h"
#include "CBombGun.h"
#include "CMutiplyProtocol.h"
CFighter::CFighter(unsigned int nID,unsigned char cTeamID,int nModelID,IAction *pAction,IHWorld* pWorld,PROPERTY_TANK* pProperty,NETWORK_TEAMTYPE NetworkTeamType) : CSprite(nID,cTeamID,nModelID,pAction,pWorld,pProperty,NetworkTeamType)
{
    mpModel = NULL;
    mpBoosterPos = NULL;
    mnBoosterCnt = 0;
    
    mnPoStartCnt = 0;
    mptStartMissile = NULL;
    
    mnGunStartCnt= 0;
    mptStartGun = NULL;
    
    mfVelocity = 1.0f;
    
    mXAngle = 0.0f;
    mYAngle = 0.0f;
    
    mpMainTower = NULL;
    
    mnCompactAniTime = 0;
    mppTailParticle = NULL;
    
    mnFireInterval = 0;
    mnFireGunInterval = 0;
    mfAttackAngle = 90.0;
    mfAttackFar = 2000000.f;
    
    if(pProperty->nGunID != 0)
    {
        //폭탄정보를 가져온다.
        PROPERTY_BOMB::GetPropertyBomb(pProperty->nGunID, mGunProperty);
    }
}

//소멸자
CFighter::~CFighter()
{
    if(mpModel)
    {
        delete mpModel;
        mpModel= NULL;
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
    if(mppTailParticle)
    {
        delete[] mppTailParticle;
        mppTailParticle = NULL;
    }
    
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
    
    if(mptStartGun)
    {
        int nCnt = mnGunStartCnt;
        for (int i = 0; i < nCnt; i++)
        {
            delete[] mptStartGun[i];
        }
        delete[] mptStartGun;
        mptStartGun = NULL;
    }
}

//밀크쉐이프의 모델을 읽어 온다.
int CFighter::LoadResoure(CModelMan* pModelMan)
{
    string sPath = mProperty.sModelPath + string(".txt");
    //mpModel = new CMS3DModelASCII(((CHWorld*)mpWorld)->GetSGLCore()->GetMS3DModelASCIIMan()->Load(sPath.c_str(),true));
    pModelMan->getModelf(sPath.c_str(), (CMyModel**)&mpModel,true);
    
    
    //-----------------------------------------------------------------------------------------------
    CHWorld* pW = (CHWorld*)GetWorld();
    int nX = pW->GetMapLevel() - mProperty.nMinLevel;
    if(nX > 0)
    {
        mProperty.nMaxAttackPower = mProperty.nMaxAttackPower + (float)mProperty.nMaxAttackPower * 0.5f * nX;
        mProperty.nMaxDefendPower = mProperty.nMaxAttackPower + (float)mProperty.nMaxAttackPower * 0.5f * nX;
    }
    //-----------------------------------------------------------------------------------------------

    
    return E_SUCCESS;
}

//모델이 만들어지면 모델의 초기화를 한다.
//pPosition : 초기 위치
//pvDirection : 모델의 방향
int CFighter::Initialize(SPoint *pPosition,SVector *pvDirection)
{
    mpModel->pfinalmulMaxtix = new GLfloat[16];
	sglLoadIdentityf(mpModel->pfinalmulMaxtix);
    
    mpModel->scale[0] = mProperty.fScale;
	mpModel->scale[1] = mProperty.fScale;
	mpModel->scale[2] = mProperty.fScale;
    
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
                PosParse(sStar2,&mnGunStartCnt,mptStartGun);
            }
            sStar += 1;
            //미사일 위치
            PosParse(sStar,&mnPoStartCnt,mptStartMissile);
            
        }
        else //부스터 위치
        {
            PosParse(sTemp,&mnBoosterCnt,mpBoosterPos);
        }
        delete[] sTemp;
    }


    AAI* pMoveAI = new CFighterMoveAI(this);
	mpAICore = new CAICore(this,pMoveAI,new CFigtherStrategyAI(this,pMoveAI,mnPoStartCnt,mnGunStartCnt ? true : false)); //총과 폭탄을 쓸수 있게 2개로 했다. 총갯수와 포갯수를 하면 너무 많아 지자나 (묶음으로 공격하자)
    
    SPoint ptNow;
    memcpy(&ptNow, pPosition, sizeof(SPoint));
    ptNow.y += FIGHTER_MIN_HEIGHT;
    
    mppTailParticle = new CFighterTailParticle*[mnBoosterCnt];
    
    for (int i = 0; i < mnBoosterCnt; i++)
    {
        mppTailParticle[i] = new CFighterTailParticle(GetWorld(),this,i);
        mppTailParticle[i]->Initialize(NULL, NULL);
        CSGLCore* pCore = ((CHWorld*)mpWorld)->GetSGLCore();
        pCore->AddParticle(mppTailParticle[i]);
    }
    mnType = mProperty.nType;
    
    //그림자 파티클을 생성한다.
    mpShadowParticle = new CShadowParticle(this);
    mpShadowParticle->Initialize(pPosition, pvDirection);
    CSGLCore* pCore = ((CHWorld*)mpWorld)->GetSGLCore();
    pCore->AddParticle(mpShadowParticle);

    
    mpModel->createBoundingSpheref(0);
    return CSprite::Initialize(&ptNow, pvDirection);
}



//보이지 않을때는 RenderBegin 가 실행되지 않아서 자동으로 위치이동 되는 명령어의 움직임이 적용이 안된다.
//그래서 코어안에서 안보이는 객체들은 여기서 실행하여준다.
void CFighter::RenderBeginCore_Invisiable(int nTime)
{
    if(mState == SPRITE_RUN)
    {
        CSprite::RenderBeginCore_Invisiable(nTime);
        if(mpMainTower && mpMainTower->GetState() != SPRITE_RUN)
            mpMainTower = NULL;

        FireMissileInVisible();
        FireGunInvisible();
    }
    else if(mState == BOMB_COMPACT || mState == BOMB_COMPACT_ANI)
    {
        SetState(SPRITE_READYDELETE);
    }
}


//CoreThread를 안에서 RenderBeginCore를 실행한다.
void CFighter::RenderBeginCore(int nTime)
{
    CSprite::RenderBeginCore(nTime);
    
    if(mpMainTower && mpMainTower->GetState() != SPRITE_RUN)
        mpMainTower = NULL;
    
}

//렌더링을 하기전에 먼져 수행해야 할 사항들을 기술한다.
//즉 움직임, 회전 등의 연산을 하여 모델에 적용을 한다.
//순수하게 데이터처리만 한다. 렌더링 처리는 RenderBegin에서 한다.
int CFighter::RenderBegin(int nTime)
{
    //먼저 이동을 한ㄷ
	int nResult = E_SUCCESS;
    if(mState == SPRITE_RUN)
    {
        CSprite::RenderBegin(nTime);
        //파이어시 애니메이션을 해준다.
        //RenderBeginFireAnimation(nTime,FireAnimationMax);
        mpModel->updatef(nTime);
        //RenderBeginAutoTurnHeader(nTime * .3f);
        //RenderBeginAutoMovePo_UpDown(nTime);
        mpAction->OnAutoAimed(this);
    }
    else if(mState == BOMB_COMPACT || mState == BOMB_COMPACT_ANI)
    {
        CampactAnimaion(nTime);
        mpModel->updatef(nTime);
    }
    return nResult;
}

int CFighter::Render()
{
	if(mState == SPRITE_RUN || mState == BOMB_COMPACT || mState == BOMB_COMPACT_ANI)
	{
        mpModel->renderf();
	}
	return E_SUCCESS;
}


int CFighter::OnEvent(SGLEvent *pEvent)
{
	int nMsgID = pEvent->nMsgID;
    if( nMsgID == GSL_BOMB_FIRE)
	{
        FireMissile();
        FireGun();
	}
    else
    {
		return CSprite::OnEvent(pEvent);
    }
    
    return E_SUCCESS;
}

void CFighter::ResetPower(bool bRunTimeUpgrade)
{
    CSprite::ResetPower(bRunTimeUpgrade);
    //CGhostSprite ...
//    if(GetModelID() != -1) //고스트가 아닐때..
//    if(GetType() != ACTORTYPE_TYPE_GHOST) //고스트가 아닐때..
//    {
        ((CHWorld*)mpWorld)-> OnSelectedObjectShowDescBar(this);
//    }
}


bool CFighter::SetPosition(SPoint* sp)
{
    CSprite::SetPosition(sp);
    mpModel->position[0] = sp->x;
    mpModel->position[1] = sp->y;
    mpModel->position[2] = sp->z;


    SPoint spBottom;
    memcpy(&spBottom, sp, sizeof(SPoint));
    
    ((CHWorld*)mpWorld)->GetPositionY(&spBottom);
    spBottom.y += SPRITE_GROUND_HEIGHT;

    
    ArrangeSprite();
    return true;
}


void CFighter::SetModelDirection(SVector* pvd)
{
    //mpModel->orientation[0] = 0;										//Pich	= x
    //mpModel->orientation[1] = 0;										//Yaw	= y
    mpModel->orientation[1] = atan2(pvd->z,pvd->x) * 180.0 / PI;		//Roll	= z
    CSprite::SetModelDirection(pvd);
}

//주의 모델의 처음 조인트에 정점을 할당해야 한다.
//bExact: true 는 버그가 있다. (위치에 따라. 충돌되었어도 검사를 못하는 경우가 있다.)
bool CFighter::CollidesBounding(CSprite* Sprite,bool bExact)
{
    if(GetState() != SPRITE_RUN) return false;
    
    SPoint pt1,pt2;
    GetPosition(&pt1);
    Sprite->GetPosition(&pt2);
    
    //1>두 정점의 거리{ 루트 ( x1*x2 + y1*y2 + z1*z2 )} < (x라디안 + y라디안)
    //  즉 x1*x2 + y1*y2 + z1*z2 < (x라디안 + y라디안) * (x라디안 + y라디안) 와 같다. (루투보다 곱하기 연산이 빠르것지요)

//    float fRadius = Sprite->GetRadius();
//    float fRadiusSum = fRadius + GetRadius();
    
    //(a[0]-b[0])*(a[0]-b[0]) + (a[1]-b[1])*(a[1]-b[1]) + (a[2]-b[2])*(a[2]-b[2]);
    float fx = pt1.x - pt2.x;
    float fy = pt1.y - pt2.y;
    float fz = pt1.z - pt2.z;
    
    float fd = (fx * fx + fy * fy + fz * fz);
    mfColidedDistance = fd;

    Sprite->SetColidedDistance(mfColidedDistance);
    //비행기는 서로 충돌되는 것을 알필요가 없기 때문에 return false;
    
//    if( fd < (fRadiusSum * fRadiusSum))
//    {
//        return true;
//    }
    return false;

}



void CFighter::GetBoosterPos(int nIndex,float** pOutFront,float** pOutBack )
{
    sglMesh     *pWorldMesh = mpModel->GetWorldMesh();
    //nPoIndex 포인덱스를 사용하여 각도를 구해온다. //2,4,2;3,4,2;4,4,2
    *pOutFront = &pWorldMesh[mpBoosterPos[nIndex][0]].pVetexBuffer[mpBoosterPos[nIndex][1] * 3];
    *pOutBack = &pWorldMesh[mpBoosterPos[nIndex][0]].pVetexBuffer[mpBoosterPos[nIndex][2] * 3];
}

void CFighter::GetFirePos(int nIndex,float** pOutFront,float** pOutBack )
{
    sglMesh     *pWorldMesh = mpModel->GetWorldMesh();
    //nPoIndex 포인덱스를 사용하여 각도를 구해온다. //2,4,2;3,4,2;4,4,2
    *pOutFront = &pWorldMesh[mptStartMissile[nIndex][0]].pVetexBuffer[mptStartMissile[nIndex][1] * 3];
    *pOutBack = &pWorldMesh[mptStartMissile[nIndex][0]].pVetexBuffer[mptStartMissile[nIndex][2] * 3];
}

void CFighter::GetFireGunPos(float** pOutFront)
{
    sglMesh     *pWorldMesh = mpModel->GetWorldMesh();
    *pOutFront = &pWorldMesh[mptStartGun[0][0]].pVetexBuffer[mptStartGun[0][1] * 3];
}

int CFighter::ArrangeSprite()
{
    float yaw = mXAngle * 15.f; //좌우
    float pitch = mYAngle * 15.f; //상하
	sglLoadIdentityf(mpModel->pfinalmulMaxtix);
	sglRotateRzRyRxMatrixf(mpModel->pfinalmulMaxtix, yaw, 0, pitch);
    return E_SUCCESS;
}

void CFighter::SetMoveVelocity(float v)
{
    mfVelocity = v;
}

float CFighter::GetMoveVelocity()
{
    return mProperty.fMoveRate * mfVelocity;
}

void* CFighter::GetSomethingValue(int nID)
{
    if(nID == 3)
    {
        return (void*)&mProperty.fRotationRate;
    }
    else if (nID == 4)
    {
        return (void*)mpMainTower;
    }
    return NULL;
}

//각 하위 클래스의 어떤 정보들을 가져오려고 하는데 아이디로 확장을 하자.
void  CFighter::SetSomethingValue(int nID,void* v)
{
    if(nID == 1)
    {
        mXAngle = (float)*(float*)v / 10.f;
    }
    else if(nID == 2)
    {
        mYAngle = (float)*(float*)v / 10.f;
    }
    else if(nID == 5)
    {
        mfAttackAngle =  (float)*(float*)v;
    }
    else if(nID == 6)
    {
        mfAttackFar =  (float)*(float*)v;
    }
}

bool CFighter::VisibleByCamera()
{
    float fD1[3];
    float fD2[3];
    
    SGLCAMERA* pcam = mpWorld->GetCamera();
    fD1[0] = pcam->viewDir.x;
    //    fD1[1] = pcam->viewDir.y; //높이말고 방향만 하자 왜냐면 높게 떠 있으면 그림자가 사라진다.
    fD1[1] = 0.f;
    fD1[2] = pcam->viewDir.z;
    sglNormalizef(fD1);
    
    fD2[0] = mpModel->position[0] - pcam->viewPos.x;
    //    fD2[1] = mpModel->position[1] - pcam->viewPos.y;
    fD2[1] = 0.f;
    fD2[2] = mpModel->position[2] - pcam->viewPos.z;
    sglNormalizef(fD2);
    
    //fDV > 0 90보다 작다
    //fDV < 0 90보다 크다.
    //fDV = 0 90
    float fDV = sglDotProductf(fD1, fD2);
    
    if(fDV > 0.6f) return true;
    
    return false;
    
}

float CFighter::GetRadius()
{
    return mProperty.fHalfX;
}

//방어력
float CFighter::GetMaxDefendPower()
{
    return mfMaxDefendPowerActor;
}


float CFighter::GetMaxAttackPower()
{
    return m_fMaxAttackPower;
}


void CFighter::CampactAnimaion(int nTime)
{
    if(mnCompactAniTime == 0)
    {
        mnCompactAniTime = GetGGTime() + 4000; //4초...
    }
    else if( GetGGTime() > mnCompactAniTime)
    {
        SetState(SPRITE_READYDELETE);
    }
}

int CFighter::GetFirePerSec() { return (float)mProperty.nFirePerSec / mBombProperty.fFirePerSec;}    //폭탄을 시간당 쏠수있는 능력

void CFighter::SetVisibleRender(bool v)
{
    if(mState == SPRITE_RUN)
    {
        if(m_bIsVisibleRender == v) return;
        
        m_bIsVisibleRender = v;
        if(mpShadowParticle)
        {
            mpShadowParticle->SetVisibleRender(v);
        }
        
        for (int i = 0; i < mnBoosterCnt; i++)
        {
            mppTailParticle[i]->SetVisibleRender(v);
        }
    }
}

void CFighter::GetPoAngles(int nInIndex,float* pOutWAngle,float* pOutHAngle,float* pOutUpPos,float* pOutDownPos,SVector *pvtDir)
{
    float *vtUp,*vtDown;
    GetFirePos(nInIndex,&vtUp,&vtDown);
    
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

bool CFighter::CanFireMissile()
{
    if(CSprite::CanFireMissile() == false) return false;
    if(mfAttackAngle > 50.f) return false; //각도가 너무 벗어나면
    return true;
}




void  CFighter::FireMissile()
{
    unsigned long lN = GetGGTime();
    if(lN < mnFireInterval)
        return;
    mnFireInterval = lN + 200;
    
    int nCnt = mnPoStartCnt; //UP/Down 이기 때문에 나누기 2한다.
    for(int i = 0; i < nCnt; i++)
    {
        if(!CanFireMissile()) return;
        FireOrgMissile(i);
    }
}

void  CFighter::FireOrgMissile(int nIndex)
{
    float fWAngle,fHAngle,vtPoUpPosition[3],vtPoDownPosition[3];
    SVector vDirAngle;
    SPoint  ptNow;
    SVector vtDir;
    
    GetPoAngles(nIndex,&fWAngle,&fHAngle,vtPoUpPosition,vtPoDownPosition,&vtDir);
    
    ptNow.x = vtPoUpPosition[0];
    ptNow.y = vtPoUpPosition[1];
    ptNow.z = vtPoUpPosition[2];
    
    vDirAngle.x = fHAngle;
    vDirAngle.y = mpModel->orientation[1];
    vDirAngle.z = fWAngle;//mpModelHeader->orientation[2] + mfModelHeaderAngle;
    
    NewMissile(nIndex,ptNow,vtDir,vDirAngle);
    
    //Start FireAnimation
    //mFireAnimationVector = mProperty.fFireAnimationAcc;
    m_nCurrentPerTime -= GetFirePerSec();
    
    if(m_nCurrentPerTime < 0)
        m_bHodeFire = true;
}



void CFighter::FireMissileInVisible()
{
    unsigned long lN = GetGGTime();
    if(lN < mnFireInterval)
        return;
    mnFireInterval = lN + 200;
    
    int nCnt = mnPoStartCnt; //UP/Down 이기 때문에 나누기 2한다.
    for(int i = 0; i < nCnt; i++)
    {
        if(!CanFireMissile()) return;
        FireOrgMissileInVisible(i);
    }
}


void  CFighter::FireOrgMissileInVisible(int nIndex)
{
    float fUpAngle = mYAngle;
    
    float fRadius = GetRadius();
    float fWAngle,fHAngle;
    SVector vDirAngle;
    SPoint  ptNow,ptEnemy;
    SVector vtDir;
    float vectorLen;
    CSprite* pEn = mpAICore->GetAttackTo(nIndex);
    if(pEn == NULL) return;
    pEn->GetPosition(&ptEnemy);
    GetPosition(&ptNow);
    
    //SVector vtDir;
	vtDir.x = ptEnemy.x - ptNow.x;
    vtDir.y = sinf(fUpAngle * PI / 180.f); //ptEnemy.y - ptNow.y;
	vtDir.z = -(ptEnemy.z - ptNow.z); //z.의 방향을 반대이다.
    
    fWAngle = atan2(vtDir.z,vtDir.x) * 180.0 / PI;
    fHAngle = fUpAngle;
    
	vectorLen = sqrtf(vtDir.x*vtDir.x + vtDir.y*vtDir.y + vtDir.z*vtDir.z);
    vtDir.x /= vectorLen;
    vtDir.y /= vectorLen;
    vtDir.z /= vectorLen;
    
    if(nIndex % 2)
        ptNow.x += fRadius;
    else
        ptNow.z += fRadius;
    
    vDirAngle.x = fHAngle;
    vDirAngle.y = mpModel->orientation[1];
    vDirAngle.z = fWAngle;
    
    NewMissile(nIndex,ptNow,vtDir,vDirAngle,false);
    
    m_nCurrentPerTime -= GetFirePerSec();
    if(m_nCurrentPerTime < 0)
        m_bHodeFire = true;
}


void  CFighter::NewMissile(int nIndex,SPoint& ptNow,SVector& vtDir,SVector& vDirAngle,bool bNeedParticle)
{
    CSprite* pTargetSprite = mpAICore->GetAttackTo(nIndex);
    CSGLCore* pCore = ((CHWorld*)mpWorld)->GetSGLCore();
    CBomb *pNewBomb = NULL;
    CBombTailParticle *pNewBombTailParticle = NULL;
    
    pNewBomb = new CBomb(pTargetSprite,this,GetID(),GetTeamID(),mBombProperty.nID,mpAction,GetWorld(),&mBombProperty);
    pNewBomb->Initialize( &ptNow, &vDirAngle,&vtDir);
    pCore->AddBomb(pNewBomb);
    
    if(bNeedParticle)
    {
        float fIntervalToActor = GetIntervalToCamera();
        
#ifdef ANDROID //JAVA의 터치시에 플래이가 JNI로 연결되어 잇어서 에러가 난다.
        if(fIntervalToActor < 3500)
            CScenario::SendMessage(SGL_PLAYSOUND,mBombProperty.nSoundFilreID,0);
#else
        if(fIntervalToActor < 3500)
            pCore->PlaySystemSound(mBombProperty.nSoundFilreID);
#endif
        pNewBombTailParticle = new CBombTailParticle(mpWorld,pNewBomb);
//        pNewFireParticle = new CFireParticle(GetWorld());
//        if(mpCurrentSelMissaileInfo)
//        {
//            //파티클을 추가한다.
//            if(pNewFireParticle)
//            {
//                pNewFireParticle->Initialize(&ptNow, &vtDir);
//                pCore->AddParticle(pNewFireParticle);
//            }
//        }
        //BombParticle을 추가한다.
        pNewBombTailParticle->Initialize(&ptNow, &vtDir);
        pCore->AddParticle(pNewBombTailParticle);
    }
}

bool CFighter::CanFireGun()
{
    CSprite* pEn = mpAICore->GetAttackToByGun();
    if(pEn == NULL) return false;
    unsigned long lN = GetGGTime();
    if(lN < mnFireGunInterval || mfAttackAngle > 80.f)
        return false;
    //    mnFireGunInterval = lN + 200;
    return true;
}

void  CFighter::FireGun()
{
    SPoint ptNow;
    if(!CanFireGun()) return;
    float *pPoPos = NULL;
    GetFireGunPos(&pPoPos);
    ptNow.x = pPoPos[0];
    ptNow.y = pPoPos[1];
    ptNow.z = pPoPos[2];
    NewGun(ptNow,true);
}

void  CFighter::FireGunInvisible()
{
    SPoint ptNow;
    if(!CanFireGun()) return;
    GetPosition(&ptNow);
    ptNow.y-= 5.f;
    NewGun(ptNow,false);
}

float CFighter::GetMaxScore()
{
    return mProperty.fMaxScore;
}

void CFighter::NewGun(SPoint& ptNow,bool bNeedParticle)
{
    CBombGun *pNewBomb = NULL;
    CSGLCore* pCore = ((CHWorld*)mpWorld)->GetSGLCore();
    int nTargetResult = 0;
    SVector vtDir,vDirAngle;
    SPoint ptTarget;
    float fLength = 0.f;

    CSprite* pTargetSprite = mpAICore->GetAttackToByGun(); //NULL검사는 하지 않음 앞에 CanFireGun에서 해버렸다.
    pTargetSprite->GetPosition(&ptTarget);
    ptTarget.y += 5.f;
    
    pNewBomb = new CBombGun(pTargetSprite,this,GetID(),GetTeamID(),mGunProperty.nID,mpAction,GetWorld(),&mGunProperty);
    nTargetResult = GetTargetPos(&ptNow, &ptTarget,&vtDir,&fLength);
    if(nTargetResult == -1)
    {
        delete pNewBomb;
        return;
    }
    
    mnFireGunInterval = GetGGTime() + 200;
    
    vDirAngle.x = asinf(vtDir.y) * 180.0 / PI;
    vDirAngle.y = mpModel->orientation[1];
    vDirAngle.z = atan2(-vtDir.z,vtDir.x) * 180.0 / PI;
    
    pNewBomb->Initialize(&ptNow,&ptTarget, &vDirAngle,&vtDir,fLength,nTargetResult);
    pCore->AddBomb(pNewBomb);
    
    if(bNeedParticle)
    {
        float fIntervalToActor = GetIntervalToCamera();
    #ifdef ANDROID //JAVA의 터치시에 플래이가 JNI로 연결되어 잇어서 에러가 난다.
        if(fIntervalToActor < 20000)
            CScenario::SendMessage(SGL_PLAYSOUND,mGunProperty.nSoundFilreID,0);
    #else
        if(fIntervalToActor < 20000)
            pCore->PlaySystemSound(mGunProperty.nSoundFilreID);
    #endif
    }
}

//장애물에 막힌지점
int CFighter::GetTargetPos(SPoint *pPosition,SPoint *pToOutPosion,SVector* pOutDirection,float* pOutLength)
{
    float fx =  pToOutPosion->x - pPosition->x;
    float fy =  pToOutPosion->y - pPosition->y;
    float fz =  pToOutPosion->z - pPosition->z;
    
    SPoint pt,pt2;
    memcpy(&pt, pPosition, sizeof(SPoint));
    float fLen = sqrtf(fx * fx + fy * fy + fz *fz);
    if(fLen > sqrtf(GetDefendRadianBounds() / 2.f))
    {
        //범위에서 너무 벗어 났다.
        return -1;
    }
    
    if(isnan(fLen))
    {
        HLogE("CFighter::GetTargetPos : fLen Nan\n");
        return -1;
    }
    
    *pOutLength = fLen;
    
    fx = fx/ fLen;
    fy = fy/ fLen;
    fz = fz/ fLen;
    
    pOutDirection->x = fx;
    pOutDirection->y = fy;
    pOutDirection->z = fz;
    
    float fUnit = 10.f; //길이 15단위로 나누어서 계산한다.
    float fLenTotal = 0.f;
    while (true)
    {
        if(fLenTotal >= fLen)
        {
            return 0; //명중
        }
        
        pt.x += fUnit * fx;
        pt.y += fUnit * fy;
        pt.z += fUnit * fz;
        
        memcpy(&pt2, &pt, sizeof(SPoint));
        if(((CHWorld*)mpWorld)->GetPositionY(&pt2) != E_SUCCESS)
        {
            break;
        }
        else if(pt.y < pt2.y) //땅밑으로 갔다면.
        {
            break;
        }
        fLenTotal += fUnit;
    }
    memcpy(pToOutPosion, &pt, sizeof(SPoint));
    fx =  pToOutPosion->x - pPosition->x;
    fy =  pToOutPosion->y - pPosition->y;
    fz =  pToOutPosion->z - pPosition->z;
    *pOutLength = sqrtf(fx * fx + fy * fy + fz *fz);
    return 1; //장애물에 막혔다. 즉 땅속이나 건물에 막혀있다.
}

void CFighter::ArchiveMultiplay(int nCmd,CArchive& ar,bool bWrite)
{
    CSprite::ArchiveMultiplay(nCmd, ar, bWrite);
    
    if(nCmd == SUBCMD_OBJ_ALL_INFO)
    {
        if(bWrite)
        {
            ar <<           mXAngle;
            ar <<           mYAngle;
            ar <<           mfAttackAngle;
            ar <<           mfAttackFar;
        }
        else
        {
            ar >>           mXAngle;
            ar >>           mYAngle;
            ar >>           mfAttackAngle;
            ar >>           mfAttackFar;
        }
    }
    if(mpAICore->GetMoveAI())
    {
        mpAICore->GetMoveAI()->ArchiveMultiplay(nCmd, ar, bWrite);
    }
}