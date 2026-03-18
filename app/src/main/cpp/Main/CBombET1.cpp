/*
 *  CBomb.cpp
 *  SongGL
 *
 *  Created by 호학 송 on 11. 3. 2..
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
#include <math.h>
#include "IHWorld.h"
#include "CBombET1.h"
#include "sGLTrasform.h"
#include "CCompatFireParticle.h"
#include "CModelMan.h"
#include "CSGLCore.h"

//#define K9_BOMBET1_SCALE 0.04f
/*
CBombET1::CBombET1(CSprite* pOwner,unsigned int nWhosBombID,unsigned char cTeamID,int nModelID,IAction *pAction,IHWorld* pWorld) : CBomb(pOwner,nWhosBombID,cTeamID,nModelID,pAction,pWorld)
{

	
}

CBombET1::~CBombET1()
{

	if(mpModel)
	{
		delete mpModel;
		mpModel = NULL;
	}
}

int CBombET1::LoadResoure(CModelMan* pModelMan)
{
	mpModel = new CMS3DModel;
	pModelMan->getModelf("bomb_et1.ms3d", *mpModel);
	
	//읽어 들이는데 속도가 너무 느리다.
	mpModel->createBoundingSpheref(0);
	return E_SUCCESS;
}


int CBombET1::Initialize(SPoint *pPosition,SVector *pvDirAngle,SVector *pvtDir)
{
	int nResult = 0;
	//초기발사속도
	//방향
	//각도
	//등등의 초기값을 사용한다.
	nResult = LoadResoure(mpWorld->GetModelMan());
	
	memcpy(&mptStart,pPosition,sizeof(SPoint));
    
    mptStart.x += pvtDir->x * 4.0;
    mptStart.y += pvtDir->y * 4.0;
    mptStart.z += pvtDir->z * 4.0;
	
    
	SetPosition(&mptStart);
    
	mArrAngle[0] = pvDirAngle->x;
	mArrAngle[1] = pvDirAngle->y;
	mArrAngle[2] = pvDirAngle->z;
	
	mpModel->orientation[0] = pvDirAngle->x;
	mpModel->orientation[2] = pvDirAngle->z;
	
	mpModel->scale[0] = K9_BOMBET1_SCALE;
	mpModel->scale[1] = 0.1;
	mpModel->scale[2] = K9_BOMBET1_SCALE;
	
	return nResult;
}

*/

