//
//  CArrowMotionSprite.cpp
//  SongGL
//
//  Created by 호학 송 on 12. 8. 16..
//
//
#include "CHWorld.h"
#include "CArrowMotionSprite.h"
#include "CMS3DModelASCII.h"
#include "CSGLCore.h"
#include <math.h>

#define LIVEMAXTIME 2000 //2초
CArrowMotionSprite::CArrowMotionSprite(unsigned int nID,unsigned char cTeamID,int nModelID,IAction *pAction,IHWorld* pWorld) : CSprite(nID,cTeamID,nModelID,pAction,pWorld,NULL,NETWORK_NONE)
{
    mpModel = NULL;
    mbRender = false;
 
}

CArrowMotionSprite::~CArrowMotionSprite()
{
    if(mpModel)
	{
		delete mpModel;
		mpModel = NULL;
	}
}

void CArrowMotionSprite::Reset()
{
    mptTab.x = -9999.0f;
    mVsibleTime = 0;
    mbRender = false;
    mbAniDir = 1;
}

int CArrowMotionSprite::Initialize(SPoint *pPosition,SVector *pvDirection)
{
    int nResult = 0;

    mpModel->scale[0] = 0.7f;
	mpModel->scale[1] = 0.7f;
	mpModel->scale[2] = 0.7f;
    mpModel->orientation[0] = 0;
    Reset();
    return nResult;
}

void CArrowMotionSprite::SetModelDirection(SVector* pvd)
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

int CArrowMotionSprite::LoadResoure(CModelMan* pModelMan)
{
//    mpModel = new CMS3DModelASCII(((CHWorld*)mpWorld)->GetSGLCore()->GetMS3DModelASCIIMan()->Load(ACTORNAME_ARROWMOTION));
    pModelMan->getModelf(ACTORNAME_ARROWMOTION, (CMyModel**)&mpModel);
    
    return E_SUCCESS;
}

SPRITE_STATE CArrowMotionSprite::GetState()
{
    if(GetModelID() == ACTORID_ARROWMOTION_TRAIN) return mState;
    return SPRITE_RUN; //항상살아 있다.
}

//충돌값은 없다.
bool CArrowMotionSprite::CollidesBounding(CSprite* Sprite,bool bExact)
{

    CWorld* pWrold = (CWorld*)mpWorld;
    //주인공과 폭탄과 충돌이 아니
    if(GetModelID() == ACTORID_ARROWMOTION_TRAIN && Sprite == pWrold->GetActor())
    {
        
        //화살표 라디안을 11로 본다. 그이유는 그라운드 화면이 11크기이다.
        float mfRadian = 11.0f;
        
        SPoint pt1,pt2;
        GetPosition(&pt1);
        Sprite->GetPosition(&pt2);
        
        //1>두 정점의 거리{ 루트 ( x1*x2 + y1*y2 + z1*z2 )} < (x라디안 + y라디안)
        //  즉 x1*x2 + y1*y2 + z1*z2 < (x라디안 + y라디안) * (x라디안 + y라디안) 와 같다. (루투보다 곱하기 연산이 빠르것지요)
        //DistanceMethodsf::squareDistancePointPointf(a, b) <= (getRadiusf() + other->getRadiusf()) * (getRadiusf() + other->getRadiusf())
        
        float fRadius = Sprite->GetRadius();
        float fRadiusSum = fRadius + mfRadian;
        
        //(a[0]-b[0])*(a[0]-b[0]) + (a[1]-b[1])*(a[1]-b[1]) + (a[2]-b[2])*(a[2]-b[2]);
        float fx = pt1.x - pt2.x;
        float fy = pt1.y - pt2.y;
        float fz = pt1.z - pt2.z;
        
        float fd = (fx * fx + fy * fy + fz * fz);
        mfColidedDistance = fd;
        if( fd < (fRadiusSum * fRadiusSum))
        {
            
            return true;
        }
//        mfColidedDistance = 0.0f;
    }
    return false;
}

bool CArrowMotionSprite::SetPosition(SPoint* sp)
{
    memcpy(&mptTab, sp, sizeof(SPoint));
    if(mptTab.x == -9999.0f)
    {
        Reset();
        return E_SUCCESS; //중지..
    }
    
    ((CHWorld*)mpWorld)->GetMovePosiztionY(&mptTab);
    
    mpModel->position[0] = mptTab.x;
    mpModel->position[1] = mptTab.y;
    mpModel->position[2] = mptTab.z;
    
    SGLCAMERA* cm = ((CHWorld*)mpWorld)->GetCamera();
    mpModel->orientation[1] = atan2(cm->viewDir.z,cm->viewDir.x) * 180.0 / PI;
    mVsibleTime = 0;

    return true;
}

int CArrowMotionSprite::RenderBegin(int nTime)
{
    if(mptTab.x == -9999.0f) return E_SUCCESS;
    
    //TrainMode가 아닐때는 그냥 사라지게
    if(GetModelID() != ACTORID_ARROWMOTION_TRAIN)
    {
        if(mVsibleTime > LIVEMAXTIME) //2초
        {
            Reset();
            return E_SUCCESS;
        }
        
        //화살표를 상하로 움직인다.
        const float fMaxY = 5.0f;
        if(mbAniDir > 0.0f)
        {
            if(mpModel->position[1] < (mptTab.y + fMaxY))
                mpModel->position[1] += nTime * 0.02;
            else
            {
                //mpModel->position[2] = mptTab.z + fMaxZ;
                mbAniDir = -1.0f;
            }
        }
        else
        {
            if(mpModel->position[1] > mptTab.y)
                mpModel->position[1] -= nTime * 0.02;
            else
            {
                //mpModel->position[2] = mptTab.z;
                mbAniDir = 1.0f;
            }
        }
    }
    
    //회전..
    mpModel->orientation[1] += 0.2 * nTime;
    
    mVsibleTime += nTime;
    //마지막 매트릭스를 구해온다.
    mpModel->updatef(nTime);
    mbRender = true;
    return E_SUCCESS;
}


int CArrowMotionSprite::Render()
{
    if(mptTab.x == -9999.0f) return E_SUCCESS;
    if(mbRender == false) return E_SUCCESS;
    
    mpModel->renderf();
    return E_SUCCESS;
}

