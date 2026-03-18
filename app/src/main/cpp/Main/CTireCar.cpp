//
//  CArmoredCar.cpp
//  SongGL
//
//  Created by itsme on 2014. 5. 19..
//
//
#include <math.h>
#include "CTireCar.h"
#include "CMoveAI.h"
#include "CAICore.h"
#include "CStrategyAssistanceAI.h"
#include "CHWorld.h"
#include "CTankModelASCII.h"
#include "CShadowParticle.h"
#include "CParticleEmitterMan.h"
#include "CSpriteStatus.h"
#include "CMutiplyProtocol.h"

CTireCar::CTireCar(unsigned int nID,unsigned char cTeamID,int nModelID,IAction *pAction,IHWorld* pWorld,PROPERTY_TANK* pProperty,NETWORK_TEAMTYPE NetworkTeamType) : CK9Sprite(nID,cTeamID,nModelID,pAction,pWorld,pProperty,NetworkTeamType)
{
    
    mpModelBody = NULL;
}

CTireCar::~CTireCar()
{
    if(mpModelBody)
    {
        delete mpModelBody;
        mpModelBody = NULL;
    }
}


//Data를 읽어온다.
int CTireCar::LoadResoure(CModelMan* pModelMan)
{
    string sPath;
    sPath = mProperty.sModelPath + string("Body.txt");
	pModelMan->getModelf(sPath.c_str(), &mpModelBody,true);
    
    sPath = mProperty.sModelPath + string("Header.txt");
    pModelMan->getModelfTankHeader(sPath.c_str(), (CMyModel**)&mpModelHeader,true);
    
    
    sPath = mProperty.sModelPath + string("RootReal.ms3d");
	pModelMan->getModelf(sPath.c_str(), &mpModel,true);
    
    
    sPath = mProperty.sModelPath + string("Root.ani");
    CAniLoader::loadAnimationf(sPath.c_str(), *(CMS3DModel*)mpModel);
    
    
    if(mpWorld->GetUserInfo())
        mnMapID = mpWorld->GetUserInfo()->GetLastSelectedMapID(); //달맵은 절벽이 없이 잘 움직이게 해주자.
    
    

    if(GetModelID() != ACTORID_AMBULANCE)
    {
        mpStatusBar = new CSpriteStatus(this);
        mpStatusBar->Initialize(NULL, NULL);
    }
    
	return E_SUCCESS;
}

int	 CTireCar::Initialize(SPoint *pPosition,SVector *pvDirection)
{
    mpModelBody->pfinalmulMaxtix = new GLfloat[16];
    int nR = CK9Sprite::Initialize(pPosition, pvDirection);
    
    mpModelBody->scale[0] = mProperty.fScale;
	mpModelBody->scale[1] = mProperty.fScale;
	mpModelBody->scale[2] = mProperty.fScale;
    
    mpModelBody->position[0] = pPosition->x;
    mpModelBody->position[1] = pPosition->y;
    mpModelBody->position[2] = pPosition->z;
    
    mpModelBody->orientation[1] = atan2(pvDirection->z,pvDirection->x) * 180.0 / PI;
    
    mpModelBody->InitFinalMatrix();

    
    AAI* pMoveAI = new CMoveAI(this);
    mpAICore = new CAICore(this,pMoveAI,new CStrategyAssistanceAI(this,pMoveAI,mnPoStartCnt,mnGunStartCnt ? true : false));
    return nR;
}

bool CTireCar::IsTabIn(CPicking* pPick,SPoint& nearPoint,SPoint& farPoint,int nPosX,int nPosY)
{
    if(GetType() == ACTORTYPE_TYPE_GHOST) return false;
    return mpModelBody->IsPicking(pPick,nearPoint,farPoint,nPosX,nPosY);
}

bool CTireCar::IsTabInZeroGroup(CPicking* pPick,int nPosX,int nPosY)
{
    if(GetType() == ACTORTYPE_TYPE_GHOST) return false;
    
    return mpModelBody->IsPickingZeroGroup(pPick,nPosX,nPosY);
}


bool CTireCar::SetPosition(SPoint* sp)
{
    SPoint ptBefore;
    int nResult = 0;
	
    //땅위에 0.9높혀서 지형의 높이가 1이하인것에 가리지 않게 한다.
    sp->y += SPRITE_GROUND_HEIGHT;
    
	if(mpModel)
	{
        ptBefore.x = mpModel->position[0];
        ptBefore.y = mpModel->position[1];
        ptBefore.z = mpModel->position[2];
        
        if(!CSprite::SetPosition(sp)) return false;
        
        
		mpModel->position[0] = sp->x;
		mpModel->position[1] = sp->y;
		mpModel->position[2] = sp->z;
        
        mpModelBody->position[0] = sp->x;
        mpModelBody->position[1] = sp->y;
        mpModelBody->position[2] = sp->z;
        
        
        nResult = ArrangeSprite();
		if(nResult != E_SUCCESS)
        {
            if(mptLastPos.x != NONE_VALUE)
            {
                mpModel->position[0] = mptLastPos.x;
                mpModel->position[1] = mptLastPos.y;
                mpModel->position[2] = mptLastPos.z;
                
                mpModelBody->position[0] = mptLastPos.x;
                mpModelBody->position[1] = mptLastPos.y;
                mpModelBody->position[2] = mptLastPos.z;
                
                CSprite::SetPosition(&mptLastPos);
                return false;
            }
            else
            {
                //무한루프 돈다.
                HLogE("위치를 안전한 곳을 찾아서 옮겨주어야 한다.\n");
                return false;
            }
            
        }
        memcpy(&mptLastPos,sp,sizeof(SPoint));
	}
    
    return true;
}

void CTireCar::SetModelDirection(SVector* pvd)
{
    float fBeforeDir;
    int nResult = 0;
    
	if(mpModel)
	{
        //이동하기전에 이전 위치를 저장하여 이동 못하는 위치이면 다시 롤백하여준다.
        fBeforeDir = mpModel->orientation[1];
        
        
		//	라디안 = 각도 * Math.PI / 180;
		//	각도 = 라디안 * 180 / Math.PI;
		//	Yaw:y Pich:x Roll:z 축회전
		//mpModel->orientation[0] = 0;										//Pich	= x
		//mpModel->orientation[1] = 0;										//Yaw	= y
		mpModel->orientation[1] = atan2(pvd->z,pvd->x) * 180.0 / PI;		//Roll	= z
        mpModelBody->orientation[1] = mpModel->orientation[1];
        nResult = ArrangeSprite();
        
		if(nResult != E_SUCCESS)
        {
            if(AvoidDontGoArea() == false)
            {
                mpModel->orientation[1] = fBeforeDir;
                mpModelBody->orientation[1] = mpModel->orientation[1];
                CSprite::SetModelDirection(pvd);
                return ;
            }
        }
	}
    
	CSprite::SetModelDirection(pvd);
}

//일반적으로 평면의 벡터를 구하고 그것의 x축/z축 벡터를 크로스곱을 하면 각이 나오기는 하지만.
//회전의 순서가 x방향을 기준으로 변경되지 않기 때문에??? 암튼 안된다.
//현재 모델을 재조정한다.
int CTireCar::ArrangeSprite()
{
    int nResult = CK9Sprite::ArrangeSprite();
    if(nResult == E_SUCCESS)
    {
        memcpy(mpModelBody->pfinalmulMaxtix,mpModel->pfinalmulMaxtix,sizeof(float) * 16);
    }
    return nResult;
}

int CTireCar::RenderBegin(int nTime)
{
	//먼저 이동을 한ㄷ
	int nResult = E_SUCCESS;
	
    if(GetType() == ACTORTYPE_TYPE_GHOST) return nResult;
    if(mState == SPRITE_RUN)
    {
        CSprite::RenderBegin(nTime);
        if(mProperty.nType == 1 && isMoving())
        {
            SPoint ptP;
            SVector ptDir;
            GetPosition(&ptP);
            GetMoveDirection(&ptDir);
            
            ptDir.x *= -1; //반대
            ptDir.z *= -1; //반대
            
            ptP.x += (ptDir.x * 6.f);
            ptP.y += 2.5f;
            ptP.z -= (ptDir.z * 6.f);
            ((CHWorld*)GetWorld())->GetParticleEmitterMan()->NewBackGas(&ptP, &ptDir);
        }
        
        FireGun();
        
        
        RuntimeUpgrading(nTime);
        
        //파이어시 애니메이션을 해준다.
        RenderBeginFireAnimation(nTime,FireAnimationMax);
        mpModel->updatef(nTime);
        mpModelBody->updatef(nTime);
        
        RenderBeginAutoTurnHeader(nTime * .3f);
        RenderBeginAutoMovePo_UpDown(nTime);
        mpModelHeader->updatef(nTime);
    }
    else if(mState == BOMB_COMPACT || mState == BOMB_COMPACT_ANI)
    {
        CampactAnimaion(nTime);
        mpModel->updatef(nTime);
        mpModelHeader->updatef(nTime);
        
    }
	return nResult;
}

int CTireCar::Render()
{
    if(GetType() == ACTORTYPE_TYPE_GHOST) return E_SUCCESS;
	if(mpModel && mState == SPRITE_RUN)
	{
        if(mnReliveEffect == 0)
        {
            
//#ifndef ANDROID
//#if DEBUG && CGDisplayMode == 0
//            
//            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
//            glDisable(GL_TEXTURE_2D);
//            
//            DisplayBottomStatus();
//            
//            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
//            glEnable(GL_TEXTURE_2D);
//            
//            glColor4f(1.0f,1.0f,1.0f,1.0f);
//#endif
//#endif //ANDROID
            
            mpModel->renderf();
            mpModelBody->renderf();
            mpModelHeader->renderf();
            
        }
        else //깜빡이게 하자.
        {
            bool bVisible = false;
            //CHWorld* pW = (CHWorld*)mpWorld;
            unsigned long nowGGT = GetGGTime();
            
            long lT = nowGGT % 1600l;
            if(lT < 400l)
                bVisible = true;
            else if(lT < 800l)
                bVisible = false;
            else if(lT < 1200l)
                bVisible = true;
            else
                bVisible = false;
            
            if(bVisible)
            {
                mpModel->renderf();
                mpModelBody->renderf();
                mpModelHeader->renderf();
            }
            
            if(nowGGT - mnReliveEffect > 4000l)
            {
                mnReliveEffect = 0l;
                if(mpShadowParticle)
                    mpShadowParticle->SetReliveEffect(false);
            }
        }
	}
    else if(mState == BOMB_COMPACT || mState == BOMB_COMPACT_ANI)
    {
        mpModel->renderf();
        mpModelHeader->renderf();
    }
	return E_SUCCESS;
}


void CTireCar::ArchiveMultiplay(int nCmd,CArchive& ar,bool bWrite)
{

    CK9Sprite::ArchiveMultiplay(nCmd, ar, bWrite);
    if(nCmd == SUBCMD_OBJ_ALL_INFO)
    {
        if(bWrite)
        {
            ar << mpModelBody->orientation[0];
            ar << mpModelBody->orientation[1];
            ar << mpModelBody->orientation[2];
            ar << mpModelBody->position[0];
            ar << mpModelBody->position[1];
            ar << mpModelBody->position[2];
        }
        else
        {

            ar >> mpModelBody->orientation[0];
            ar >> mpModelBody->orientation[1];
            ar >> mpModelBody->orientation[2];
            ar >> mpModelBody->position[0];
            ar >> mpModelBody->position[1];
            ar >> mpModelBody->position[2];
        }
    }
}
