//
//  CArrowMotionSprite.cpp
//  SongGL
//
//  Created by 호학 송 on 12. 8. 16..
//
//
#include <math.h>
#include "CHWorld.h"
#include "CItemSprite.h"
#include "CMS3DModelASCII.h"
#include "CSGLCore.h"
#include "CHWorld.h"
#include "CAniRuntimeEnergyUP.h"
#include "CAniRuntimeBubble.h"
#include "sGLUtils.h"
#include "CMutiplyProtocol.h"
#define LIVEMAXTIME 30000 //30초

CItemSprite::CItemSprite(PROPERTY_RUNTIMEITEM* pItem,IHWorld* pWorld,NETWORK_TEAMTYPE NetworkTeamType) : CSprite(pItem->nItemID,SGL_TEAMALL,-1,NULL,pWorld,NULL,NetworkTeamType)
{
    mpModel = NULL;
    mpItemInfo = pItem;
    mbMove = 0.f;
    mnType = ACTORTYPE_TYPE_ITEM;
//    mbAleardyUsed = false;
}

CItemSprite::~CItemSprite()
{
    if(mpModel)
	{
		delete mpModel;
		mpModel = NULL;
	}
    mpItemInfo = 0;
}


int CItemSprite::Initialize(SPoint *pPosition,SVector *pvDirection)
{
    int nResult = 0;
    
    mpModel->scale[0] = 0.7f;
	mpModel->scale[1] = 0.7f;
	mpModel->scale[2] = 0.7f;
    mpModel->orientation[0] = 0;
    
    mnEndTime = GetGGTime() + LIVEMAXTIME;
    
    SetPosition(pPosition);
    return nResult;
}

void CItemSprite::SetModelDirection(SVector* pvd)
{
	if(mpModel)
	{
		//	라디안 = 각도 * Math.PI / 180;
		//	각도 = 라디안 * 180 / Math.PI;
		//	Yaw:y Pich:x Roll:z 축회전
		mpModel->orientation[0] = 0; //Pich = x
		mpModel->orientation[1] = atan2(pvd->z,pvd->x) * 180.0 / PI; //Yaw = y
		mpModel->orientation[2] = 0; //Roll = z
		
	}
	CSprite::SetModelDirection(pvd);
}

int CItemSprite::LoadResoure(CModelMan* pModelMan)
{
    //bool CItemMan::Initialize() 에서 이미 초기화를 했기 때문에 vector ture가 의미 없다.
//    mpModel = new CMS3DModelASCII(((CHWorld*)mpWorld)->GetSGLCore()->GetMS3DModelASCIIMan()->Load(mpItemInfo->sModelPath,true));
    ((CHWorld*)mpWorld)->GetSGLCore()->GetModelMan()->getModelf(mpItemInfo->sModelPath, (CMyModel**)&mpModel,true);
	return E_SUCCESS;
}

//충돌값은 없다.
bool CItemSprite::CollidesBounding(CSprite* Sprite,bool bExact)
{
    //주인공과 부딛혔을 경우......... 아이템을 먹은 것으로 간주한다.
    if(((CHWorld*)mpWorld)->GetActor() == Sprite)
    {
        if(GetState() != SPRITE_RUN)   return false;
        SPoint pt1,pt2;
        GetPosition(&pt1);
        Sprite->GetPosition(&pt2);
    
        float fRadius = Sprite->GetRadius();
        float fRadiusSum = fRadius + 3.0f; //아이템의 라디안은 대략 3.0f으로 한다.
        
        float fx = pt1.x - pt2.x;
        float fy = pt1.y - pt2.y;
        float fz = pt1.z - pt2.z;
        
        float fd = (fx * fx + fy * fy + fz * fz);
        if( fd < (fRadiusSum * fRadiusSum))
        {
            SetState(SPRITE_READYDELETE);
            
            //-----------------------------------
            //Item의 Animation 파티클을 추가해준다.
            //-----------------------------------
            if(mpItemInfo->nType == 1)
            {
                if(mpItemInfo->nItemID == ITEMID_RUNTIME_FILLENERGY)
                {
                    //Fill Up~~~~을 할경우..
                    CAniRuntimeEnergyUP *pItem = new CAniRuntimeEnergyUP(mpWorld,Sprite);
                    pItem->Initialize(&pt2, NULL);
                    CSGLCore *pCore = ((CHWorld*)mpWorld)->GetSGLCore();
                    pCore->AddParticle(pItem);
                }
                else if(mpItemInfo->nItemID == ITEMID_RUNTIME_BUBBLE)
                {
                    CAniRuntimeBubble* pItem = Sprite->GetAniRuntimeBubble(); //이미 아이템이 존재하는지를 체크한다.
                    if(Sprite->GetState() == SPRITE_RUN)
                    {
                        if (pItem && pItem->GetState() == SPRITE_RUN)
                        {
                            pItem->ResetBubble(mpItemInfo->nCount);
                        }
                        else
                        {
                            pItem = new CAniRuntimeBubble(mpWorld,Sprite,mpItemInfo->nCount);
                            Sprite->SetAniRuntimeBubble(pItem);
                            pItem->Initialize(&pt2, NULL);
                            CSGLCore *pCore = ((CHWorld*)mpWorld)->GetSGLCore();
                            pCore->AddParticle(pItem);
                        }
                        
                        if(Sprite->GetNetworkTeamType() == NETWORK_MYSIDE)
                            GetWorld()->GetMutiplyProtocol()->SendSyncBubble(Sprite);
                    }
                    
                    
                }
            }
            CScenario::SendMessage(SGL_GET_ITEM,(long)GetRutimeItemInfo(),0,0,0); //밖에서 하지말고 이쪽에서 직접하자.. 회피모드일때 이상하게 여러번 드러가는 경우가 생긴다.
            return true;
        }
    }
    return false;
}

bool CItemSprite::SetPosition(SPoint* sp)
{
    
    ((CHWorld*)mpWorld)->GetMovePosiztionY(sp);
    
    mpModel->position[0] = sp->x;
    mpModel->position[1] = sp->y;
    mpModel->position[2] = sp->z;
    
    SGLCAMERA* cm = ((CHWorld*)mpWorld)->GetCamera();
    mpModel->orientation[1] = atan2(cm->viewDir.z,cm->viewDir.x) * 180.0 / PI;
    
    CSprite::SetPosition(sp);
    return true;
}


int CItemSprite::RenderBegin(int nTime)
{
    float fM;
    float fR = 0.2f * nTime;
    if(GetGGTime() > mnEndTime)
    {
        SetState(SPRITE_READYDELETE);
        return E_SUCCESS;
    }
    
    if(mbMove)
    {
        fR = nTime;
        
        CWorld* pWorld = (CWorld*)GetWorld();
        CSprite* pActor = pWorld->GetActor();
        if(pActor && pActor->GetState() != SPRITE_RUN)
        {
            SetMoveVelocity(0.0f);
            return E_SUCCESS;
        }
        
        SVector vtDir;
        SPoint ptTarget;
        pActor->GetPosition(&ptTarget);
        
        vtDir.x = ptTarget.x - mpModel->position[0];
        vtDir.y = 0;
        vtDir.z = ptTarget.z - mpModel->position[2];
        
        sglNormalize(&vtDir);
     
        fM = (float)nTime / 12.f;
        
        ptTarget.x = mpModel->position[0] + vtDir.x * fM;
        ptTarget.y = 0;
        ptTarget.z = mpModel->position[2] + vtDir.z * fM;
        ((CHWorld*)mpWorld)->GetMovePosiztionY(&ptTarget);
        
        mpModel->position[0] = ptTarget.x;
        mpModel->position[1] = ptTarget.y;
        mpModel->position[2] = ptTarget.z;
        CSprite::SetPosition(&ptTarget);

        //주인공이 움직이지 않으면 CollideBounding이 안이러난다.
        if(pActor->isMoving() == false)
            CollidesBounding(pActor,false);
        
        
    }
    

    //회전한다.
    mpModel->orientation[1] += fR;
    //마지막 매트릭스를 구해온다.
    mpModel->updatef(nTime);
    return E_SUCCESS;
}


int CItemSprite::Render()
{
    if(GetState() != SPRITE_RUN) return E_SUCCESS;
    
    mpModel->renderf();
    return E_SUCCESS;
}



void CItemSprite::SetMoveVelocity(float v)
{
    if(v == 0.0f) mbMove = false;
    mbMove = true;
}


