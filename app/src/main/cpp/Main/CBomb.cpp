/*
 *  CBomb.cpp
 *  SongGL
 *
 *  Created by 호학 송 on 11. 3. 2..
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
#include <math.h>
#include "CHWorld.h"
#include "CBomb.h"
#include "sGLTrasform.h"
#include "CSGLCore.h"
#include "CModelBound.h"
#include "CModelMan.h"
#include "sGLUtils.h"
#include "CK9Sprite.h"
#include "CDMainTower.h"
#include "CAniRuntimeBubble.h"
#include "CParticleEmitterMan.h"
#include "CElectricArcParticle.h"

namespace {
void SpawnElectricShell(CHWorld* pWorld,const SPoint& center,float radius)
{
    CElectricArcParticle *pShellMain = new CElectricArcParticle(pWorld);
    pShellMain->Initialize((SPoint*)&center, radius);
    pWorld->GetSGLCore()->AddParticle(pShellMain);

    CElectricArcParticle *pShellInner = new CElectricArcParticle(pWorld);
    pShellInner->Initialize((SPoint*)&center, radius * 0.90f);
    pWorld->GetSGLCore()->AddParticle(pShellInner);
}
}
CBomb::CBomb(CSprite* ptTarget,CSprite* pOwner,unsigned int nWhosBombID,unsigned char cTeamID,int nModelID,IAction *pAction,IHWorld* pWorld,PROPERTY_BOMB* pBombProperty) :
CSprite(nWhosBombID,cTeamID,nModelID,pAction,pWorld,NULL,NETWORK_NONE)
{
    
    mpModel = NULL;
	muiElapsedTime = 0;
	mArrAngle[0] = 30.0f;
	mArrAngle[1] = 0.0f;
	mArrAngle[2] = 0.0f;
	memset(&mptStart,0,sizeof(mptStart));
	mfBeforeY = 0.0f;
	mfBeforeX = 0.0f;
	memset(&mptBeforePos,0,sizeof(SPoint));
    mpOwner = pOwner;
    mBombProperty = *pBombProperty;
    mOutSide = 0;

    //유도기능이 존재한다면....
    if(pBombProperty->fAutoDectect != 0.0f)
        mpTarget = ptTarget;
    else
        mpTarget = NULL;
    
    mfRoationAngle = 0.0f;
    mfRoationDif = 0.0f;
    mnRotationStartPosAngle = 0; 
    mfDivideAngle = 0.0f;
    mfGuideDirectionX = 0.f;
    mfGuideDirectionY = 0.f;
    mbRenderByCamera = true;
    mbCheckCompact = true;
}


CBomb::~CBomb()
{
	if(mpModel)
	{
		delete mpModel;
		mpModel = NULL;
	}
}

int CBomb::LoadResoure(CModelMan* pModelMan)
{
    string sPath = mBombProperty.sModelPath;
    
#if OBJMilkShape
    
    sPath.append(".ms3d");
	mpModel = new CMS3DModel;
    pModelMan->getModelf(sPath.c_str(), *mpModel);
	
#else
    //폭탄생성
    sPath.append(".txt");
//    mpModel = new CMS3DModelASCII(((CHWorld*)mpWorld)->GetSGLCore()->GetMS3DModelASCIIMan()->Load(sPath.c_str(),true));
    pModelMan->getModelf(sPath.c_str(), (CMyModel**)&mpModel,true);
#endif
	return E_SUCCESS;
}



int CBomb::Initialize(SPoint *pPosition,SVector *pvDirAngle,SVector *pvtDir)
{
	int nResult = 0;
	//초기발사속도
	//방향
	//각도
	//등등의 초기값을 사용한다.
    
	nResult = LoadResoure(mpWorld->GetModelMan());
	
	memcpy(&mptStart,pPosition,sizeof(SPoint));
    
    mptStart.x += pvtDir->x * mBombProperty.fStartPosition;
    mptStart.y += pvtDir->y * mBombProperty.fStartPosition;
    mptStart.z += -pvtDir->z * mBombProperty.fStartPosition;
    
    //파티클 에미터에서 사용함.
    SetModelDirection(pvtDir);
    
	SetPosition(&mptStart);
    
	mArrAngle[0] = pvDirAngle->x; //높이
	mArrAngle[1] = pvDirAngle->y; //해덩의 방향
	mArrAngle[2] = pvDirAngle->z; //포의 방향
	
	
	mpModel->orientation[1] = pvDirAngle->z; //포의 방향
    mpModel->orientation[2] = pvDirAngle->x; //상하각
	
	mpModel->scale[0] = mBombProperty.fScale;
	mpModel->scale[1] = mBombProperty.fScale;
	mpModel->scale[2] = mBombProperty.fScale;
	
    
    mpModel->position[0] = mptStart.x;
    mpModel->position[1] = mptStart.y;
    mpModel->position[2] = mptStart.z;
    
//    //Added By Song 2012.03.06 모델을 초기화, 위치설정할때 update()를 해야 finalMatrix가 초기화된다. 
//    //미사일을 발사하자마자 적이 터지는 현상은 생성되자마자 충돌검사를 할때 문제가 생긴다.
//    mpModel->InitFinalMatrix();

    //읽어 들이는데 속도가 너무 느리다.
	mpModel->createBoundingSpheref(0);

    mnType = 0x10000000 | mBombProperty.nBombType; //폭탄은 0x10000000으로 시작한다.
	return nResult;
}


//fDV > 0 90보다 작다
//fDV < 0 90보다 크다.
//fDV = 0 90
bool CBomb::VisibleByCamera()
{
    float fD1[3];
    float fD2[3];
    
    SGLCAMERA* pcam = mpWorld->GetCamera();
    fD1[0] = pcam->viewDir.x;
    fD1[1] = pcam->viewDir.y;
    fD1[2] = pcam->viewDir.z;
    
    fD2[0] = mpModel->position[0] - pcam->viewPos.x;
    fD2[1] = mpModel->position[1] - pcam->viewPos.y;
    fD2[2] = mpModel->position[2] - pcam->viewPos.z;
    sglNormalizef(fD2);
    
    //fDV > 0 90보다 작다
    //fDV < 0 90보다 크다.
    //fDV = 0 90
    float fDV = sglDotProductf(fD1, fD2);
    
    if(fDV > 0.6f) return true;
    
    return false;
    
}

void CBomb::SetCompactBomb()
{
    SPoint ptPos;
    CHWorld *pWorld = (CHWorld*)mpWorld;
    bool bAlienBomb = false;
    
    mState = SPRITE_READYDELETE;
    
    if(mbRenderByCamera)
    {
        GetPosition(&ptPos);
        float fExplosionScale = sqrtf(BOMBMAXRNG * mBombProperty.fMaxRadianDetect) / 18.0f;
        SPoint ptGround = ptPos;
        bool bGroundExplosion = false;

        if(pWorld->GetPositionY(&ptGround) == E_SUCCESS)
        {
            float fGroundThreshold = 12.0f + fExplosionScale * 8.0f;
            if(fabsf(ptPos.y - ptGround.y) <= fGroundThreshold)
                bGroundExplosion = true;
        }

        if((mpOwner && mpOwner->GetModelID() >= ACTORID_ET1) ||
           (mBombProperty.sModelPath && strstr(mBombProperty.sModelPath, "ET") != NULL))
        {
            bAlienBomb = true;
        }

        if(bGroundExplosion && bAlienBomb)
        {
            float shellRadius = 4.0f + fExplosionScale * 7.5f;
            SpawnElectricShell(pWorld, ptPos, shellRadius);
        }
        else if(bGroundExplosion)
            pWorld->GetParticleEmitterMan()->NewGroundExplosion(&ptPos,fExplosionScale);
        else if(bAlienBomb)
        {
            float shellRadius = 4.8f + fExplosionScale * 8.5f;
            SpawnElectricShell(pWorld, ptPos, shellRadius);
        }
        else
            pWorld->GetParticleEmitterMan()->NewAirExplosion(&ptPos,fExplosionScale);
    }
}

//RenderBegine에서 하지 않고 여기서 하는 이유는 안보여지는 영역에서 발사했는데, 그순간 오너가 죽어버리면 RenderBegin에 드러오는 순간 에러가 난다.
void CBomb::RenderBeginCore(int nTime)
{
    if(mpOwner && mpOwner->GetState() != SPRITE_RUN)
        mpOwner = NULL;
    if (mpTarget && mpTarget->GetState() != SPRITE_RUN)
        mpTarget = NULL;
}



int CBomb::RenderBegin(int nTime)
{
	int nResult = CSprite::RenderBegin(nTime);
	if(mState == SPRITE_RUN || mState == SPRITE_BOMB_NEXT_COMPACT) 
	{
		//폭탄의 위치를 정의 한다.
        //0:고폭/유도탄 1:미사일
        if(mBombProperty.nBombType != 1)
            ChainCourse(nTime);
        else
            ChainCourseMissaile(nTime);
        
        mbRenderByCamera = VisibleByCamera();
        if(mbRenderByCamera)
            mpModel->updatef(nTime);
        
        //플래그를 바꾼다.
        if(mState == SPRITE_BOMB_NEXT_COMPACT)
        {
            SetCompactBomb();
        }
	}
   
	return nResult;
}


int CBomb::Render()
{	
	if(mpModel && mbRenderByCamera)
	{
		if(mState == SPRITE_RUN || mState == SPRITE_BOMB_NEXT_COMPACT) 
        {
			mpModel->renderf();
        }
	}
	return E_SUCCESS;
}

int CBomb::RenderEnd()
{
	
	return E_SUCCESS;
}


bool CBomb::SetPosition(SPoint* sp)
{
    if(CSprite::SetPosition(sp))
    {
        mpModel->position[0] = sp->x;
        mpModel->position[1] = sp->y;
        mpModel->position[2] = sp->z;
        return true;
    }
	return false;
}

float CBomb::GetMaxAttackPower()
{ 
    return mBombProperty.fMaxAttackPower;
} //공격력


bool CBomb::GetTargetPosition(SPoint* ptTarget)
{
    if(mBombProperty.fAutoDectect != 0.0f && mpTarget)
    {
        mpTarget->GetPosition(ptTarget);
        return true;
    }
    return false;
}



void CBomb::ChainCourseMissaile(int nTime)
{
    float fGuideAngleY = 0.f;
    
    float fGuideAngleX = 0.f;
    SPoint ptTarget,ptNow;
    SVector vtDir,vtDirTarget;
    GetModelDirection(&vtDir);
    
//    float fTime = (float)nTime / 1000.f;
#if CGDisplayMode == 1
   float fTime = (float)nTime / 1000.f;
    if(gnCGDiplayMode == 2)
        fTime = (float)nTime / 10000000.f;
#else
    
   float fTime = (float)nTime / 1000.f;
#endif

    GetPosition(&ptNow);
    
	//멀리 방향으로 운동
	float vInter = mBombProperty.fVelocity * fTime;
    if(GetTargetPosition(&ptTarget))
    {
        vtDirTarget.x = ptTarget.x - ptNow.x;
        vtDirTarget.y = (ptTarget.y + 3.f) - ptNow.y; //+5.f는 너무 땅이기 때문에 캐릭터가 존재하는 부분까지 ....
        vtDirTarget.z = - (ptTarget.z - ptNow.z);
        float fDifAngleX = AngleArrangeEx(mpModel->orientation[1] - atan2(vtDirTarget.z,vtDirTarget.x) * 180.0 / PI);
        if(fDifAngleX <= 180.f) //Right
        {
            if(mfGuideDirectionX > 0.f) //이전에 왼쪽이라면 급속하게 오른쪽으로 변경하지 말자
            {
                if(fDifAngleX > 5.f) //너무 많이 트러지면 다시 재조정해준다.
                    mfGuideDirectionX = -fTime * 100.f;
                fGuideAngleX = 0.f;
            }
            else
            {
                if(fDifAngleX > mBombProperty.fAutoDectect)
                    fGuideAngleX = mBombProperty.fAutoDectect;
                else
                {
                    fGuideAngleX = fDifAngleX;
                }
                mfGuideDirectionX = -fTime * 100.f;
            }
        }
        else //Left
        {
            fDifAngleX = 360.0f - fDifAngleX;
            if(mfGuideDirectionX < 0.f)
            {
                if(fDifAngleX > 5.f)
                    mfGuideDirectionX = fTime * 100.f;
                fGuideAngleX = 0.f;
            }
            else
            {
                if(fDifAngleX > mBombProperty.fAutoDectect)
                    fGuideAngleX = mBombProperty.fAutoDectect;
                else
                {
                    fGuideAngleX = fDifAngleX;
                }
                mfGuideDirectionX = fTime * 100.f;
            }
            
        }
        
        //float fDifAngleY = AngleArrangeEx(mpModel->orientation[2] - atan2(vtDirTarget.y,vtDirTarget.x) * 180.0 / PI);
        float fDifAngleY = AngleArrangeEx(mpModel->orientation[2] -
                                          atan2(vtDirTarget.y,
                                                sqrtf(vtDirTarget.x * vtDirTarget.x + vtDirTarget.z *vtDirTarget.z)) * 180.0 / PI);
        
        if(fDifAngleY <= 180.f) //Down
        {
            if(mfGuideDirectionY > 0.f)
            {
                if(fDifAngleY > 5.f) //너무 많이 트러지면 다시 재조정해준다.
                    mfGuideDirectionY = -fTime * 100.f;
                fGuideAngleY = 0.f;
            }
            else
            {
                if(fDifAngleY > mBombProperty.fAutoDectect)
                    fGuideAngleY = mBombProperty.fAutoDectect;
                else
                {
                    fGuideAngleY = fDifAngleY;
                }
                mfGuideDirectionY = -fTime * 100.f;
            }
        }
        else //Up
        {
            fDifAngleY = 360.0f - fDifAngleY;
            if(mfGuideDirectionY < 0.f)
            {
                if(fDifAngleY > 5.f)
                    mfGuideDirectionY = fTime * 100.f;
                fGuideAngleY = 0.f;
            }
            else
            {
                if(fDifAngleY > mBombProperty.fAutoDectect)
                    fGuideAngleY = mBombProperty.fAutoDectect;
                else
                {
                    fGuideAngleY = fDifAngleY;
                }
                mfGuideDirectionY = fTime * 100.f;
            }
            
        }
    }
    
    if(fGuideAngleX == 0.f && fGuideAngleY == 0.f)
    {
        vtDir.x *= vInter;
        vtDir.y *= vInter;
        vtDir.z *= vInter;
        
        ptNow.x += vtDir.x;
        ptNow.y += vtDir.y;
        ptNow.z -= vtDir.z;
        
        SetPosition(&ptNow);
        sglNormalize(&vtDir);
        SetModelDirection(&vtDir);
        mpModel->orientation[1] = mpModel->orientation[1] + fGuideAngleX * mfGuideDirectionX;
        
    }
    else
    {
        mpModel->orientation[1] = mpModel->orientation[1] + fGuideAngleX * mfGuideDirectionX;
        mpModel->orientation[2] = mpModel->orientation[2] + fGuideAngleY * mfGuideDirectionY;
        
        //AngleToVector
        sglAngleToVector(mpModel->orientation,&vtDir);
        
        ptNow.x += vtDir.x * vInter;
        ptNow.y += vtDir.y * vInter;
        ptNow.z -= vtDir.z * vInter;
        
        
        SetPosition(&ptNow);
        sglNormalize(&vtDir);
        SetModelDirection(&vtDir);
    }
    
    muiElapsedTime += nTime;
    
    if(muiElapsedTime > 10000) //너무 오래 있지 않게 한다.
        SetState(SPRITE_BOMB_NEXT_COMPACT);
}

void CBomb::ChainCourse(int nTime)
{
    
    float fGuideAngleX = 0.0f;
    SPoint ptTarget;
    SPoint  ptNow;
    
	float fHAngle = mArrAngle[0] * PI / 180.0f; //지표면에 대한 각도로 발사
    
	float fHcos = cosf(fHAngle);
	float fHsin = sinf(fHAngle);
	float fVy0 = mBombProperty.fVelocity * fHsin;	//높이 초기속도
    float fVx0 = mBombProperty.fVelocity * fHcos;	//멀리 초기속도

    //유도탄이 너무 떨리는 현상 수정함. 0.05 => 0.02로 변경함.
    float fTimeV = (float)nTime * 0.02f; //1000.f * 100.f
    //Guide 기능을 추천한다.
    //타겟이 존재하고..
    if(GetTargetPosition(&ptTarget))
    {
        
        SVector vtDir;
        
        //-----------------------------------------
        //전과의 방향설정을 매타임마다 정리한다.
        //-----------------------------------------
        GetPosition(&ptNow);
        
        vtDir.x = ptTarget.x - ptNow.x;
        vtDir.y = ptTarget.y - ptNow.y;
        vtDir.z = - (ptTarget.z - ptNow.z);
        

        
        float fDifAngleX = AngleArrangeEx(mpModel->orientation[1] - atan2(vtDir.z,vtDir.x) * 180.0 / PI);
        
        
        if(fDifAngleX <= 180) //Right
        {
            if(mfGuideDirectionX > 0.f) //이전에 왼쪽이라면 급속하게 오른쪽으로 변경하지 말자
            {
                if(fDifAngleX > 5.f) //너무 많이 트러지면 다시 재조정해준다.
                    mfGuideDirectionX = -fTimeV;
                fGuideAngleX = 0.f;
            }
            else
            {
                if(fDifAngleX > mBombProperty.fAutoDectect)
                    fGuideAngleX = mBombProperty.fAutoDectect;
                else
                {
                    fGuideAngleX = fDifAngleX;
                }
                mfGuideDirectionX = -fTimeV;
            }
            
        }
        else //Left
        {
            fDifAngleX = 360.0f - fDifAngleX;
            if(mfGuideDirectionX < 0.f)
            {
                if(fDifAngleX > 5.f)
                    mfGuideDirectionX = fTimeV;
                fGuideAngleX = 0.f;
            }
            else
            {
                if(fDifAngleX > mBombProperty.fAutoDectect)
                    fGuideAngleX = mBombProperty.fAutoDectect;
                else
                {
                    fGuideAngleX = fDifAngleX;
                }
                mfGuideDirectionX = fTimeV;
            }
            
        }
        
        mpModel->orientation[1] = mArrAngle[2] + fGuideAngleX * mfGuideDirectionX;
        mArrAngle[2] = mpModel->orientation[1];
    }
    
    //폭탄 속도
//    float fTime = (float)muiElapsedTime * .001f;
//    float fTime = (float)muiElapsedTime / 10000.f;
#if CGDisplayMode == 1
    float fTime = (float)muiElapsedTime * .001f;
    if(gnCGDiplayMode == 2)
        fTime = (float)muiElapsedTime * .0000001f;
#else
    
    float fTime = (float)muiElapsedTime * .001f;
#endif
	
	
	//멀리 방향으로 운동
	float fX = fVx0 * fTime;
	
	//높이 방향으로 운동
	float fY = fVy0 * fTime - (9.8f * fTime * fTime) / 2;
	
	float ptModel[3],ptTemp[3];
	
	GLfloat matrix[16];
	
	ptTemp[0] = fX;
	ptTemp[1] = fY;
    ptTemp[2] = 0;

	sglLoadIdentityf(matrix);
    
    if(mBombProperty.cRotation == 1)
    {

        mfRoationAngle += 5; 
        
        if(mfRoationDif < 2.0f)            mfRoationDif += 0.2;
        if(mfRoationDif > 4.0f)            mfRoationDif = 4.0f;
        
        //float a40 = (VIEW_APERTURE + 20.0f) / 2.0f * PI / 180.0f;
        float fRotRadian = mfRoationAngle * PI / 180.0f;
        
        if(mfRoationDif != 0.0f)
        {
            float x2,z2;
            if(mnRotationStartPosAngle == 0)
            {
                x2 = mfRoationDif * cosf(fRotRadian);
                z2 = mfRoationDif * sinf(fRotRadian);
            }
            else if(mnRotationStartPosAngle == 90)
            {
                x2 = - mfRoationDif * sinf(fRotRadian);
                z2 = mfRoationDif * cosf(fRotRadian);
            }
            else if(mnRotationStartPosAngle == 180)
            {
                x2 = -mfRoationDif * cosf(fRotRadian);
                z2 = -mfRoationDif * sinf(fRotRadian);
            }
            else //if(mnRotationStartPosAngle == 270)
            {
                x2 = mfRoationDif * sinf(fRotRadian);
                z2 = - mfRoationDif * cosf(fRotRadian);
            }            
            sglTranslateMatrixf(matrix, x2, z2, 0);
        }
    }

	sglRotateRzRyRxMatrixf(matrix,
									 0,
									 mArrAngle[2],
									 0); //좌우의 방향..
	
	sglMultMatrixVectorf(ptModel, matrix, ptTemp);
	
	ptNow.x = ptModel[0] + mptStart.x;
	ptNow.y = ptModel[1] + mptStart.y;
	ptNow.z = ptModel[2] + mptStart.z;
	
	//현재 위 폭탄위치를 재설 정한다.
	SetPosition(&ptNow);
    
    //타겟이 정해진 다면 ....0.3초내 분리하고 그렇지 않다면 기울기 변화에 분리한다.
    // 11은 적 분산탄이다. 쏠때부터 타겟이 정해저있으니 그냥 일정시간 지나면 터지게 하자.
    if(mpTarget && muiElapsedTime > 300 && GetModelID() != 11) //0.3초후에 분리한다.
    {
        if(mBombProperty.cDivide > 1 && mpOwner && mpOwner->GetState() == SPRITE_RUN)
        {
            DivMissile();
            return;
        }
        else if(!(fY == 0 && mfBeforeY == 0 && fY == 0 && mfBeforeY == 0))
        {
            //상하의 방향...
            mpModel->orientation[2] = atan2((fY - mfBeforeY),(fX - mfBeforeX)) * 180.0 / PI;
            mfDivideAngle = mpModel->orientation[2];
        }
    }
    else
    {
        //위에서 아래로 내려올때 분리한다. 현재는 .3초에 걸려 있어서 이쪽으로 타지는 않다.
        //각위치에 총탄의 상하각도를 나타낸다.
        if(!(fY == 0 && mfBeforeY == 0 && fY == 0 && mfBeforeY == 0))
        {
            //상하의 방향...
            mpModel->orientation[2] = atan2((fY - mfBeforeY),(fX - mfBeforeX)) * 180.0 / PI;
            if(mBombProperty.cDivide > 1 && ((mfDivideAngle > 0 && mpModel->orientation[2] < 0) ||  (mfDivideAngle < 0 && mpModel->orientation[2] > 0)))
            {
                if(mpOwner && mpOwner->GetState() == SPRITE_RUN)
                {
                    DivMissile();
                    return;
                }
            }
            mfDivideAngle = mpModel->orientation[2];
        }
    }
    
    
	muiElapsedTime += nTime;
	
	mfBeforeY = fY;
	mfBeforeX = fX;
}


void CBomb::DivTargetNearBySprite(int nCnt,CSprite* pSprite,int nBombID,SPoint& ptNow,SVector& vtDir,SVector& vDirAngle,float fMaxLen)
{
    SPoint pt,pt2;
    CSprite* pLastTemp = NULL;
    
    int nTeam = GetTeamID();
    float fx,fz;
    float flen2;
//    float fMaxLen = 3200;
    CHWorld* pWorld = (CHWorld*)mpWorld;
    CSGLCore* pCore = pWorld->GetSGLCore();
    CListNode<CSprite> lstV;
    
    pSprite->GetPosition(&pt);
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
        else if(sp == pSprite)
        {
            it = it->GetNextNode();
            continue;
        }
    
        if(pWorld->IsAllians(nTeam, sp->GetTeamID()) == false) //적일 경우
        {
            sp->GetPosition(&pt2);
            
            fx = pt2.x - pt.x;
            fz = pt2.z - pt.z;
            
            flen2 = fx * fx + fz * fz;
            if (flen2 <= fMaxLen)
            {
                pLastTemp = sp;
                lstV.AddTHead(sp);
                
                if(lstV.Size() >= nCnt)
                {
                    break;
                }
            
            }
        }
        it = it->GetNextNode();
    }

    int nVCnt = lstV.Size();
    if(nVCnt < nCnt)
    {
        for (int i = nVCnt; i < nCnt; i++)
        {
            lstV.AddTHead(pLastTemp);
        }
        
    }
    
    
    int nIndex = 1;
    b = lstV.begin();
    for(ByNode<CSprite>* it = b; it != lstV.end(); )
    {
        CSprite* sp = it->GetObject();
        ((CK9Sprite*)mpOwner)->NewDivideMissile(nIndex, sp,mBombProperty.nDividedID, ptNow, vtDir, vDirAngle);
        nIndex++;
        it = it->GetNextNode();
    }
}


void CBomb::DivMissile()
{
    SPoint ptNow;
    GetPosition(&ptNow);
    
    //현재 폭탄은 자동 소멸 한다.
    SetState(SPRITE_READYDELETE);
    
    CK9Sprite* sp = (CK9Sprite*)mpOwner;
    
    SVector vtDir;
    GetModelDirection(&vtDir);
    
    SVector vtDirAngle;
    vtDirAngle.x = mpModel->orientation[2]; 
    vtDirAngle.y = mpModel->orientation[0];
    vtDirAngle.z = mpModel->orientation[1]; //[1],[2]가 바뀐이유는 내부적으로 좌우 방향은 vtDirAngle.z것으로 사용한다.

    CSprite* pTargetx = NULL;
    if(mpTarget && mpTarget->GetState() == SPRITE_RUN)
        pTargetx = mpTarget;
    
    if(pTargetx == NULL)
    {
        //한발은 그냥 조준한 곳을 쏘게... 
        sp->NewDivideMissile(0, pTargetx,mBombProperty.nDividedID, ptNow, vtDir, vtDirAngle);
        
        //타겟이 없으면 폭탄을 기준으로 근거리를 조사한다.
        DivTargetNearBySprite(mBombProperty.cDivide - 1,this,mBombProperty.nDividedID, ptNow, vtDir, vtDirAngle,28200);
    }
    else
    {
        //첫번째는 pTagrgetx로 나머지는 다른 곳을 찾는다.
        sp->NewDivideMissile(0, pTargetx,mBombProperty.nDividedID, ptNow, vtDir, vtDirAngle);
        DivTargetNearBySprite(mBombProperty.cDivide - 1,pTargetx,mBombProperty.nDividedID, ptNow, vtDir, vtDirAngle,6200);
    }
}

void CBomb::CheckCollidesToTerrian()
{
    int nCompact = 0;
    if(mState == SPRITE_RUN)
	{
        //지면과의 충돌 체크
        nCompact = CollidesToTerrian();
        if(nCompact != 0) //충돌
            mState = SPRITE_BOMB_NEXT_COMPACT; //다음 프레임에는 반드시 충돌한다.
    }
}

//폭탄이 터졌을때 영향을 받는 범위안에 있으면 1(거의중앙),2,3  범위 밖에 있으면 0
float CBomb::InsideRangeCompact(float fSPAndBombDistance ,CSprite* Sprite)
{
    static float fR = BOMBMAXRNG * 2;
    static float fR2 = BOMBMAXRNG * 5;
    static float fR3 = BOMBMAXRNG * 10;
    
    float MaxRng;
//    SPoint pt1,pt2;
//    //Runtime Item은 폭탄에 안터진다.
//    //CGhostSprite ...
//    if(Sprite->GetType() & ACTORTYPE_TYPE_GHOSTANITEM && ((CHWorld*)mpWorld)->IsAllians(GetTeamID(), Sprite->GetTeamID()) == false)
//        return 0.f;
    
//    GetPosition(&pt1);
//    Sprite->GetPosition(&pt2);
//    
//    //(a[0]-b[0])*(a[0]-b[0]) + (a[1]-b[1])*(a[1]-b[1]) + (a[2]-b[2])*(a[2]-b[2]);
//    float fx = pt1.x - pt2.x;
//    float fy = pt1.y - pt2.y;
//    float fz = pt1.z - pt2.z;
//    
//    float fd = (fx * fx + fy * fy + fz * fz);
    
    MaxRng = BOMBMAXRNG * mBombProperty.fMaxRadianDetect;
    
    if(fSPAndBombDistance <= MaxRng)
    {
        if(fSPAndBombDistance <= BOMBMAXRNG) //명중 : 탱크는 8 radian
            return 1.f;
        else if(fSPAndBombDistance < fR)
            return 1.5f;
        else if(fSPAndBombDistance < fR2)
            return 2.0f;
        else if(fSPAndBombDistance < fR3)
            return 3.0f;
        else
        {
            return 4.0f;
        }
    }
    return 0.f;
}

bool CBomb::CollidesBounding(CSprite* Sprite,bool bExact)
{
    
    if(GetState() != SPRITE_RUN) return false;
    if(Sprite->GetState() != SPRITE_RUN) return false;
    
#if OBJMilkShape
    bool bCom;
    void *em = Sprite->GetModel();

    if(em == NULL)
    {
        bCom =  Sprite->CollidesBounding(this,bExact);
        mfColidedDistance = Sprite->GetColidedDistance();
        return bCom;
    }
    
	if(!bExact)
    {
		if(mpModel->collidesBoundingSpheref(0, 0, 0, *(CMS3DModel*)Sprite->GetModel(), 0, 0, 0, false, -2))
        {
            mfColidedDistance = mpModel->dynamicBoundingSphere->updateDistanceSprite;
            return true;
        }
        mfColidedDistance = mpModel->dynamicBoundingSphere->updateDistanceSprite;
        return false;
    }
    
	if(mpModel->collidesBoundingSpheref(0, 0, 0, *(CMS3DModel*)Sprite->GetModel(), 0, 0, 0, true, 0))
    {
        mfColidedDistance = mpModel->dynamicBoundingSphere->updateDistanceSprite;
        return true;
    }
#else
    
    SPoint pt1,pt2;
    GetPosition(&pt1);
    Sprite->GetPosition(&pt2);
    
    //1>두 정점의 거리{ 루트 ( x1*x2 + y1*y2 + z1*z2 )} < (x라디안 + y라디안)
    //  즉 x1*x2 + y1*y2 + z1*z2 < (x라디안 + y라디안) * (x라디안 + y라디안) 와 같다. (루투보다 곱하기 연산이 빠르것지요)
    //DistanceMethodsf::squareDistancePointPointf(a, b) <= (getRadiusf() + other->getRadiusf()) * (getRadiusf() + other->getRadiusf())
    
    float fRadius = Sprite->GetRadius();
    float fRadiusSum = fRadius + GetRadius();
    
    
    
    //(a[0]-b[0])*(a[0]-b[0]) + (a[1]-b[1])*(a[1]-b[1]) + (a[2]-b[2])*(a[2]-b[2]);
    float fx = pt1.x - pt2.x;
    float fy = pt1.y - pt2.y;
    float fz = pt1.z - pt2.z;
    
    float fd = (fx * fx + fy * fy + fz * fz);
    mfColidedDistance = fd;
    
    if(Sprite->GetModelID() == ACTORID_BLZ_MAINTOWER)
    {
        CAniRuntimeBubble* pA = Sprite->GetAniRuntimeBubble();
        if(pA)
        {
            float fR = ((CDMainTower*)Sprite)->GetArearRadus();
            if( fd < fR * fR)
            {
                pA->Attacted();
            }
        }
    }
    
    
    if( fd < (fRadiusSum * fRadiusSum))
    {
        
        return true;
    }

#endif
    return false;
}

float CBomb::GetRadius()
{
#if OBJMilkShape
    return mpModel->dynamicBoundingSphere->getSimpleRadiusf();
#else
    return 3.f;
#endif
}

//return 1: 충돌  0:비충돌 other:Error(범위밖)
int CBomb::CollidesToTerrian()
{
	int nResult = 0;
	//현재 라디안 (반지름)을 가져 온다.
	float fRadius = GetRadius();
	
	SVector vtDir; //탄두의 방향
	float fNowPos[3];
	float fNextPos[3];
	float vectorLen = 0; //탄두의 이동길이 (길이 이전 프렘을 기준으로 다음 프레임도 동일하게 움직일 것이다.
	
    mbCheckCompact = true;
    
	//첫프레임에는 충돌인지 모른다.
	//이전위치의 벅터가 없다.
	if((mptBeforePos.x == 0 && mptBeforePos.y == 0 && mptBeforePos.z == 0) ||
	   (mpModel->position[0] == mptBeforePos.x && 
		mpModel->position[1]== mptBeforePos.y && 
		mpModel->position[2] == mptBeforePos.z))
	{
		mptBeforePos.x = mpModel->position[0];
		mptBeforePos.y = mpModel->position[1];
		mptBeforePos.z = mpModel->position[2];
		return 0;
	}
	
	vtDir.x = mpModel->position[0] - mptBeforePos.x;
	vtDir.y = mpModel->position[1] - mptBeforePos.y;
	vtDir.z = mpModel->position[2] - mptBeforePos.z;
	
	vectorLen = sqrtf(vtDir.x*vtDir.x + vtDir.y*vtDir.y + vtDir.z*vtDir.z);
	
	//단위벡터로 변경한다.
	vtDir.x /= vectorLen;
	vtDir.y /= vectorLen;
	vtDir.z /= vectorLen;
	
	
	//탄두의 현재 해더 위치
	fNowPos[0] = mpModel->position[0] + vtDir.x * fRadius;
	fNowPos[1] = mpModel->position[1] + vtDir.y * fRadius;
	fNowPos[2] = mpModel->position[2] + vtDir.z * fRadius;
	
	//탄두의 다음 위치의 예상 이전 프레임의 이동속도를 더한다.
	//vectorLen 가 너무작으면 여러번 검사를 해야하기때문에 작으면 안뒤아. 
	//탄두의 속도는 한프레임에 1보다 커야 하지 않을까나...
	fNextPos[0] = fNowPos[0] + vtDir.x * vectorLen;
	fNextPos[1] = fNowPos[1] + vtDir.y * vectorLen;
	fNextPos[2] = fNowPos[2] + vtDir.z * vectorLen;
	
	//이전위치를 기억한다.
	mptBeforePos.x = mpModel->position[0];
	mptBeforePos.y = mpModel->position[1];
	mptBeforePos.z = mpModel->position[2];
	
	
	SPoint ptInfo;
	ptInfo.x = fNextPos[0];
	ptInfo.y = fNextPos[1];
	ptInfo.z = fNextPos[2];
	
    nResult = ((CHWorld*)mpWorld)->GetPositionY(&ptInfo);
    
	if(nResult == E_SUCCESS) 
	{
		if( fNextPos[1] <= ptInfo.y ) //땅과 충돌...
		{
			return 1;
		}
        
        //땅과의 차이가 얼마나 나는지 검사하자.
//        float fHeight = fNextPos[1] - ptInfo.y;
//        HLogD("Bomb Height=%f\n",fHeight);
        if(GetBombType() == 0 && //일반고폭탄/유도탄일 경우만 체크를 하지 말자.
           (fNextPos[1] - ptInfo.y) > CHECKCOMPACTMINHEIGHT)
        {
            mbCheckCompact = false;
        }
	}
	else
	{
        if(nResult == E_POS_COMPACT_WALL) 
            return 1;
        
        if(ptInfo.y <= H_TERRIAN_DEFAULT_HIGHT)
            return 1;
        else
        {
            
            if(nResult < 0)
            {
                mOutSide ++;
                if(mOutSide > 10 && GetBombType() != 1) //미자일이 아닌경우는 10
                    return 1;
                else if(mOutSide > 40) //미사일인경우는 30 프레임
                    return 1;
            }
            return 0;
        }
        
		return nResult;
	}
	return nResult;
}
