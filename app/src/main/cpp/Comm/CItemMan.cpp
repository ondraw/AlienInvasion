//
//  CItemMan.cpp
//  SongGL
//
//  Created by itsme on 12. 9. 13..
//
//
#include <stdlib.h>
#include "CItemMan.h"
#include "CItemSprite.h"
#include "CHWorld.h"
#include "CSGLCore.h"
#include "CProperty.h"
#include "CMS3DModelASCIIMan.h"
#include "CUserInfo.h"
//#define ItemTime_TowMinute   120000
#define ItemTime_TowMinute   20000

CItemMan::CItemMan(CWorld* pWorld)
{
    mnIndexBonus2 = -1;
    mnIndexL6 = -1;
    mnIndexL5 = -1;
    mnIndexL4 = -1;
    mnIndexL2 = -1;
    mpWorld = pWorld;
    mnM = 0;
    
    mItemList = 0;
    mItemListCnt = 0;
    mbNeedItem = false;
    
    mIsOneRule = false;
    mbIsCompletedMap = false;
}


CItemMan::~CItemMan()
{
    if(mItemList)
    {
        delete[] mItemList;
        mItemList = 0;
    }
    
    mpWorld = 0;
}   


bool CItemMan::Initialize()
{
    CModelMan* pMan = ((CHWorld*)mpWorld)->GetSGLCore()->GetModelMan();
    PROPERTY_RUNTIMEITEM::GetPropertyRunTimeItemList(mItems);
    if(pMan == NULL) return false;
    
    for(int i = 0; i < mItems.size(); i++)
    {
        PROPERTY_RUNTIMEITEM* item = mItems.at(i);
        if(item->nItemID == ITEMID_RUNTIME_PLUSBONUS2)
            mnIndexBonus2 = i;
        else if(item->nItemID == ITEMID_RUNTIME_L6)
        {mnIndexL6 = i; item->nID = 0;} //랜덤에 안나타나게 0으로 셋하자.
        else if(item->nItemID == ITEMID_RUNTIME_L5)
        {mnIndexL5 = i; item->nID = 0;} //랜덤에 안나타나게 0으로 셋하자.
        else if(item->nItemID == ITEMID_RUNTIME_L4)
        {mnIndexL4 = i; item->nID = 0;} //랜덤에 안나타나게 0으로 셋하자.
        else if(item->nItemID == ITEMID_RUNTIME_L2)
        {mnIndexL2 = i; item->nID = 0;} //랜덤에 안나타나게 0으로 셋하자.
        
        CMS3DASCII* pModel = NULL;
        pMan->getModelfAscii(item->sModelPath, &pModel,true);
//        delete pModel; //이것은 지우면 안뒤아.
    }
    
    if(mpWorld->GetUserInfo())
        mbIsCompletedMap = mpWorld->GetUserInfo()->IsCompletedMap(mpWorld->GetUserInfo()->GetLastSelectedMapID());
    
    return true;
}

void CItemMan::BeginCore(int nTime)
{
    unsigned long nNow = GetGGTime();
    if(nNow > mnM)
    {
        mbNeedItem = true;
        mnM = nNow + ItemTime_TowMinute;
    }
}

//Thread 단에서 아이템을 생성해주어야 한다.
void CItemMan::AddItem(SPoint* ptItem)
{
    
    CUserInfo* pUserInfo;
    int Index = -1;
    SVector vtDir;
    CSGLCore* pCore = mpWorld->GetSGLCore();
    if(mbNeedItem)
    {
        vtDir.x = 1.f;
        vtDir.y = 0.f;
        vtDir.z = 0.f;
        
        pUserInfo = mpWorld->GetUserInfo();
        
        
        //아이템 룰을 정의 하자.
        //1> 탱크를 기본적으로 주자. 너무 못하는 아해들을 위해서 6,5,4까지 기본제공 해주자.
        if(mIsOneRule == false &&
           pUserInfo->GetCompletedRank() == 1 &&
           pUserInfo->GetLastSelectedMapID() == 1 &&
           mbIsCompletedMap == false)
        {
            bool bIsmnIndexL6 = false;
            bool bIsmnIndexL5 = false;
            bool bIsmnIndexL4 = false;
            
            vector<USERINFO*>* pList = pUserInfo->GetListItem();
            vector<USERINFO*>::iterator i;
            for (i = pList->begin(); i != pList->end(); i++)
            {
                if((*i)->nID == ITEMID_RUNTIME_L6)
                {
                    bIsmnIndexL6 = true;
                }
                else if((*i)->nID == ITEMID_RUNTIME_L5)
                {
                    bIsmnIndexL5 = true;
                }
                else if((*i)->nID == ITEMID_RUNTIME_L4)
                {
                    bIsmnIndexL4 = true;
                }
            }
            
            if(bIsmnIndexL4 && bIsmnIndexL5 && bIsmnIndexL6)
                mIsOneRule = true;
            else if(bIsmnIndexL6 == false)
                Index = mnIndexL6;
            else if(bIsmnIndexL5 == false)
                Index = mnIndexL5;
            else if(bIsmnIndexL4 == false)
                Index = mnIndexL4;
        }
        
        if(Index == -1)
        {
            Index = rand() % mItems.size();
            if( mItems[Index]->nID == 0) //미자용이면 그냥 모너스를 해주자.
                Index = mnIndexBonus2;
        }
        
        
        mbNeedItem = false;
    }
    else
        Index = mnIndexBonus2;

    CItemSprite* pItem = new CItemSprite(mItems[Index],(IHWorld*)mpWorld,NETWORK_MYSIDE);
    pItem->LoadResoure(mpWorld->GetModelMan());
    pItem->Initialize(ptItem,&vtDir);
    pCore->AddSprite(pItem);

}





