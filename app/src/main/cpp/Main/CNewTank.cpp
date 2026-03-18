//
//  CNewTank.cpp
//  SongGL
//
//  Created by 호학 송 on 2014. 3. 6..
//
//

#include "CNewTank.h"
#include <math.h>
#include "CHWorld.h"
#include "sGLTrasform.h"
#include "CBombET1.h"
#include "CFireParticle.h"
#include "CBombTailParticle.h"
#include "sGLUtils.h"
#include "CTextureMan.h"
#include "CTE1BoosterParticle.h"
#include "CAICore.h"
#include "CSGLCore.h"
#include "CShadowParticle.h"
#include "CTankModelASCII.h"
#include "CNewBoosterParticle.h"
#include "CBombRaser.h"
#include "CMutiplyProtocol.h"
CNewTank::CNewTank(unsigned int nID,unsigned char cTeamID,int nModelID,IAction *pAction,IHWorld* pWorld,PROPERTY_TANK* pProperty,NETWORK_TEAMTYPE NetworkTeamType) : CK9Sprite(nID,cTeamID,nModelID,pAction,pWorld,pProperty,NetworkTeamType)
{
	mfModelHeaderAngle = 0.0f;
	mfModelHeaderRotVector= 0.0f;
    mnPoUpDownState = 0;
	
	//--------------------------
	//자동화 타겟으로 ....
	//--------------------------
	mfAutoMovePo_UpDonwDir = 0.0f;
    mpCurrentSelMissaileInfo = NULL;
    
    
    
    mnBoosterCnt = 0;
    mpBoosterPos = NULL;

    mpTailParticle = NULL;
}

CNewTank::~CNewTank()
{
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

//---------------------------------------------------------------------
//Start CSprite Virtual
int	 CNewTank::Initialize(SPoint *pPosition,SVector *pvDirection)
{
    //레이져는 쓰레드 안쪽에서 생성되는 경우가 생기니 초기화시 텍스쳐바운딩을 하자.
    string sTempPath = mGunProperty.sBombBombImgPath;
    char* sTemp = (char*)sTempPath.c_str();
    char* sStr = strstr(sTemp,".");
    if(sStr)
    {
        *(sTemp-1) = 0;
        string sPath = sTemp;
        sPath.append("1.tga");
        mpWorld->GetTextureMan()->GetTextureID(sPath.c_str());
        
        sPath = sTemp;
        sPath.append("2.tga");
        mpWorld->GetTextureMan()->GetTextureID(sPath.c_str());
        
        sPath = sTemp;
        sPath.append("3.tga");
        mpWorld->GetTextureMan()->GetTextureID(sPath.c_str());
    }
    return CK9Sprite::Initialize(pPosition, pvDirection);
}

void CNewTank::ParsMisailStartPos()
{
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
        
        mpModelHeader->SetPointStatrMissile(mptStartMissile);
    }

    CSGLCore* pCore = ((CHWorld*)mpWorld)->GetSGLCore();
    //포의 시작 위치를 파싱하여 가져온다.
    
    mpTailParticle = new CNewBoosterParticle(GetWorld(),this);
    mpTailParticle->Initialize(NULL, NULL);
    pCore->AddParticle(mpTailParticle);
}


void CNewTank::GetBoosterPos(int nIndex,float** pOutFront,float** pOutBack )
{
    if(mpModel->GetType() == MODELTYPE_MS3D_ASSCII)
    {
        sglMesh     *pWorldMesh = ((CMS3DModelASCII*)mpModel)->GetWorldMesh();
        //nPoIndex 포인덱스를 사용하여 각도를 구해온다. //2,4,2;3,4,2;4,4,2
        *pOutFront = &pWorldMesh[mpBoosterPos[nIndex][0]].pVetexBuffer[mpBoosterPos[nIndex][1] * 3];
        *pOutBack = &pWorldMesh[mpBoosterPos[nIndex][0]].pVetexBuffer[mpBoosterPos[nIndex][2] * 3];
    }
}

void CNewTank::GetFirePos(int nIndex,float** pOutFront,float** pOutBack )
{
    if(mpModelHeader->GetType() == MODELTYPE_MS3D_ASSCII)
    {
        sglMesh     *pWorldMesh = ((CMS3DModelASCII*)mpModelHeader)->GetWorldMesh();
        //nPoIndex 포인덱스를 사용하여 각도를 구해온다. //2,4,2;3,4,2;4,4,2
        *pOutFront = &pWorldMesh[mptStartMissile[nIndex][0]].pVetexBuffer[mptStartMissile[nIndex][1] * 3];
        *pOutBack = &pWorldMesh[mptStartMissile[nIndex][0]].pVetexBuffer[mptStartMissile[nIndex][2] * 3];
    }
}

void CNewTank::GetFireGunPos(float** pOutFront)
{
    if(mpModelHeader->GetType() == MODELTYPE_MS3D_ASSCII)
    {
        sglMesh     *pWorldMesh = ((CMS3DModelASCII*)mpModelHeader)->GetWorldMesh();
        *pOutFront = &pWorldMesh[mptStartGun[0][0]].pVetexBuffer[mptStartGun[0][1] * 3];
    }
}


bool CNewTank::SetPosition(SPoint* sp)
{
    if(CK9Sprite::SetPosition(sp))
    {
        //공중부양
        mpModel->position[1] += ET1_GROUND_HEIGHT;
        if(mpModelHeader)
            mpModelHeader->position[1] += ET1_GROUND_HEIGHT;
        return true;
    }
    return false;
}

int CNewTank::Render()
{
    return CK9Sprite::Render();
}

void CNewTank::SetVisibleRender(bool v)
{
    CK9Sprite::SetVisibleRender(v);
    if(mState == SPRITE_RUN)
    {
        if(mpTailParticle)
            mpTailParticle->SetVisibleRender(v);
    }
}


void CNewTank::SetState(SPRITE_STATE v)
{
    CK9Sprite::SetState(v);
    if(mpTailParticle)
    {
        mpTailParticle->SetState(v);
        if(v != SPRITE_RUN)
            mpTailParticle = NULL;
    }
}


void CNewTank::NewGun(SPoint& ptNow,bool bNeedParticle)
{
    CSprite* pTargetSprite = mpAICore->GetAttackToByGun();
    CSGLCore* pCore = ((CHWorld*)mpWorld)->GetSGLCore();
    CBombRaser *pNewBomb = NULL;
    if(pTargetSprite == NULL)        return ;
    
    pNewBomb = new CBombRaser(pTargetSprite,this,GetID(),GetTeamID(),mGunProperty.nID,mpAction,GetWorld(),&mGunProperty);
    mnFireGunInterval = GetGGTime() + mGunProperty.nBombTailMaxAliveTime;
    pNewBomb->Initialize(NULL,NULL,NULL);
    pCore->AddBomb(pNewBomb);

//$$ 소리를 추가해주어야 한다.
//    if(bNeedParticle)
//    {
//        float fIntervalToActor = GetIntervalToCamera();
//#ifdef ANDROID //JAVA의 터치시에 플래이가 JNI로 연결되어 잇어서 에러가 난다.
//        if(fIntervalToActor < 3500)
//            CScenario::SendMessage(SGL_PLAYSOUND,mGunProperty.nSoundFilreID,0);
//#else
//        if(fIntervalToActor < 3500)
//            pCore->PlaySystemSound(mGunProperty.nSoundFilreID);
//#endif
//    }
}

void CNewTank::OnCompletedUpdated(RUNTIME_UPGRADE* prop,CArchive* pReadArc)
{
    int nID = GetID();
    //레이져 업그레이드를 하였다.
    if(prop->nID == RUNTIME_UPDATE_RASER)
    {
        if(pReadArc == NULL)
        {
            //0.4씩 높아진다.
            mGunProperty.fMaxAttackPower = mGunProperty.fMaxAttackPower + mGunProperty.fMaxAttackPower * 0.4f;
            CHWorld* pWorld = (CHWorld*)GetWorld();
            if(!pWorld->GetMutiplyProtocol()->IsStop())
            {
                CArchive arc;
                arc << nID;
                arc << prop->nID;
                arc << prop->nUpgradeCnt;
                arc << mGunProperty.fMaxAttackPower;
                pWorld->GetMutiplyProtocol()->SendSyncCompletedRuntimeUpgrade(&arc);
            }
            
        }
        else
        {
            *pReadArc >> nID;
            *pReadArc >> prop->nID;
            *pReadArc >> prop->nUpgradeCnt;
            *pReadArc >> mGunProperty.fMaxAttackPower;
            
            delete pReadArc;
        }
        
        char* sStr = strstr(mGunProperty.sBombTailImgPath,".");
        if(sStr)
        {
            *(sStr-1) = 0;
            string sPath = mGunProperty.sBombTailImgPath;
            if(prop->nUpgradeCnt == 1)
                sPath.append("2.tga");
            else
                sPath.append("3.tga");
            mGunProperty.SetBombTailImgPath(sPath.c_str());
        }
        return;
    }
    else
        CK9Sprite::OnCompletedUpdated(prop,pReadArc);
}


//End CSprite Virtual
//---------------------------------------------------------------------
