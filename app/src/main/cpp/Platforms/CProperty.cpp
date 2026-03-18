//
//  CProperty.cpp
//  SongGL
//
//  Created by Song Hohak on 12. 1. 31..
//  Copyright (c) 2012 thinkm. All rights reserved.
//
#include "sGLDefine.h"
#include "CProperty.h"
#include "sGL.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
using namespace std;
extern "C" bool sglNoFree();

#ifdef ANDROID

#ifndef NOTOPENGL
#include "CLoader.h"
#endif

#include "HDoc.h"
#else
#import <Foundation/Foundation.h>
#endif

void* PROPERTYI::gProperty = 0;

#ifdef ANDROID
//extern string gsLocalDir; //다중언어지원함.

CHDoc* DefaultPropertyX()
{
    if(PROPERTYI::gProperty) return (CHDoc*)PROPERTYI::gProperty;
#ifdef NOTOPENGL
    string sPath = TESTPATH;
    sPath += "PropertyList.plist";
#else
    string sPath = CLoader::getSearchPath();
//    sPath.append(gsLocalDir);
    if(gnDisplayType == DISPLAY_IPHONE)
    {
        sPath += "PropertyList.plist";
    }
    else
    {
        sPath += "PropertyList_IPad.plist";
    }

#endif    
    PROPERTYI::gProperty = new CHDoc();
    if(((CHDoc*)PROPERTYI::gProperty)->Load(sPath.c_str()) == false)
    {
        HLogE("[ERROR] DefaultPropertyX Load %s\n",sPath.c_str());
        return NULL;
    }
    
    return (CHDoc*)PROPERTYI::gProperty;
}

int GetIntProp(NodeArray* arr,int nIndex)
{
    if(arr->mList[nIndex]->GetType() >= NodeDataInt)
    {
        NodeDataValue* pValue = (NodeDataValue*)arr->mList[nIndex];
        return pValue->GetInt();
    }
    return 0;
}

const char* GetStrProp(NodeArray* arr,int nIndex)
{
    if(arr->mList[nIndex]->GetType() >= NodeDataInt)
    {
        NodeDataValue* pValue = (NodeDataValue*)arr->mList[nIndex];
        return pValue->GetString();
    }
    return NULL;
}
float GetFloatProp(NodeArray* arr,int nIndex)
{
    if(arr->mList[nIndex]->GetType() >= NodeDataInt)
    {
        NodeDataValue* pValue = (NodeDataValue*)arr->mList[nIndex];
        return pValue->GetReal();
    }
    return 0.0f;
}

#else

NSDictionary* DefaultPropertyX()
{
    if(PROPERTYI::gProperty) return (NSDictionary*)PROPERTYI::gProperty;
    
    NSString* sPath = nil;
    if(gnDisplayType == DISPLAY_IPHONE)
        sPath = [[NSBundle mainBundle] pathForResource:@"PropertyList" ofType:@"plist"];
    else
        sPath = [[NSBundle mainBundle] pathForResource:@"PropertyList_IPad" ofType:@"plist"];
    
    PROPERTYI::gProperty = [[NSDictionary alloc] initWithContentsOfFile:sPath];
    
    return (NSDictionary*)PROPERTYI::gProperty;
}

int GetIntProp(NSArray* arr,int nIndex)
{
    NSNumber* v = [arr objectAtIndex:nIndex];
    return v.intValue;
}
const char* GetStrProp(NSArray* arr,int nIndex)
{
    NSString* v = [arr objectAtIndex:nIndex];
    return v.UTF8String;
}
float GetFloatProp(NSArray* arr,int nIndex)
{
    NSNumber* v = [arr objectAtIndex:nIndex];
    return v.doubleValue;
}
#endif


extern int gDisplayWidth;
extern int gDisplayHeight;
vector<float>* PROPERTYI::GetPropertyFloatList(const char* sKey,vector<float>& list)
{
#ifdef ANDROID
    CHDoc* pDic = DefaultPropertyX();
    NodeArray* pArrData = (NodeArray*)pDic->GetData(sKey);
    
    if(pArrData == NULL)
    {
        HLogE("[ERROR]PropetyList not Found %s\n",sKey);
        return NULL;
    }
    
    //크기를 변경한다. (폰타입과 아이패드 타입으로 분리하여 화면의 사이즈에 맞게 재 조정해준다)
    float fWidth = 480.f;
    float fHeight = 320.f;
    if(gnDisplayType == DISPLAY_IPAD)
    {
        fWidth =  1024.f;
        fHeight = 786.f;
        //fHeight = 646.f; //화면이 너무 작아보여서  (겔럭시 노트일경우 그래서 비율을 조금 높혀보자.)
    }
    
    //늘어나는 비율이 다르면 찌그러진다. (Wdith가 보통 많이 변한다.)
    float fRate = (float)gDisplayWidth / (float)fWidth  * 0.9f;
    //float fRate = (float)gDisplayHeight / (float)fHeight;
    
    int nCnt = (int)pArrData->mList.size();

    if(nCnt == 4) 
    {
        list.push_back((float)((NodeDataValue*)pArrData->mList[0])->GetReal() * fRate);
        list.push_back((float)((NodeDataValue*)pArrData->mList[1])->GetReal() * fRate);
        list.push_back((float)((NodeDataValue*)pArrData->mList[2])->GetReal() * fRate);
        list.push_back((float)((NodeDataValue*)pArrData->mList[3])->GetReal() * fRate);
    }
    else if(nCnt == 5) //Info_RadaBoard
    {
        list.push_back((float)((NodeDataValue*)pArrData->mList[0])->GetReal() * fRate);
        list.push_back((float)((NodeDataValue*)pArrData->mList[1])->GetReal() * fRate);
        list.push_back(((NodeDataValue*)pArrData->mList[2])->GetReal());
        list.push_back(((NodeDataValue*)pArrData->mList[3])->GetReal());
        list.push_back((float)((NodeDataValue*)pArrData->mList[4])->GetReal() * fRate);
    }
    else
    {
        
        //HLogE("[ERROR]PropetyList not %s Count %d",nCnt,sKey);
        for(int i = 0; i < nCnt; i++)
        {
            
            list.push_back(((NodeDataValue*)pArrData->mList[i])->GetReal() * fRate);
        }

    }
#elif defined(MAC)
    //크기를 변경한다. (폰타입과 아이패드 타입으로 분리하여 화면의 사이즈에 맞게 재 조정해준다)
    float fWidth = 480.f;
    float fHeight = 320.f;
    if(gnDisplayType == DISPLAY_IPAD)
    {
        fWidth =  1024.f;
        fHeight = 786.f;
        //fHeight = 646.f; //화면이 너무 작아보여서  (겔럭시 노트일경우 그래서 비율을 조금 높혀보자.)
    }
    
    //늘어나는 비율이 다르면 찌그러진다. (Wdith가 보통 많이 변한다.)
    float fRate = (float)gDisplayHeight / (float)fHeight;
//    float fRate = 1.0f;

    
    NSString* nsKey = [NSString stringWithCString:sKey encoding:NSUTF8StringEncoding];
    NSDictionary* pDic = DefaultPropertyX();
    NSArray* pArrData = [pDic objectForKey:nsKey];
    if(pArrData == nil)
    {
        HLogE("[ERROR]PropetyList not Found %s\n",sKey);
        return nil;
    }
    int nCnt = pArrData.count;
    if(nCnt != 5)
    {
        for (NSNumber* sv in pArrData)
        {
            list.push_back([sv floatValue] * fRate);
        }
    }
    else //레이다
    {
        
        list.push_back([[pArrData objectAtIndex:0] floatValue] * fRate);
        list.push_back([[pArrData objectAtIndex:1] floatValue] * fRate);
        list.push_back([[pArrData objectAtIndex:2] floatValue]);
        list.push_back([[pArrData objectAtIndex:3] floatValue]);
        list.push_back([[pArrData objectAtIndex:4] floatValue] * fRate);
    }
#else
    if(sglGetDisplayScale() > 1.0f)
    {
        //크기를 변경한다. (폰타입과 아이패드 타입으로 분리하여 화면의 사이즈에 맞게 재 조정해준다)
        float fWidth = 480.f;
        float fHeight = 320.f;
        if(gnDisplayType == DISPLAY_IPAD)
        {
            fWidth =  1024.f;
            fHeight = 786.f;
        }
        
        //늘어나는 비율이 다르면 찌그러진다. (Wdith가 보통 많이 변한다.)
        float fRate = (float)gDisplayHeight / (float)fHeight;
        NSString* nsKey = [NSString stringWithCString:sKey encoding:NSUTF8StringEncoding];
        NSDictionary* pDic = DefaultPropertyX();
        NSArray* pArrData = [pDic objectForKey:nsKey];
        if(pArrData == nil)
        {
            HLogE("[ERROR]PropetyList not Found %s\n",sKey);
            return nil;
        }
        int nCnt = (int)pArrData.count;
        if(nCnt != 5)
        {
            for (NSNumber* sv in pArrData)
            {
                list.push_back([sv floatValue] * fRate);
            }
        }
        else //레이다
        {
            
            list.push_back([[pArrData objectAtIndex:0] floatValue] * fRate);
            list.push_back([[pArrData objectAtIndex:1] floatValue] * fRate);
            list.push_back([[pArrData objectAtIndex:2] floatValue]);
            list.push_back([[pArrData objectAtIndex:3] floatValue]);
            list.push_back([[pArrData objectAtIndex:4] floatValue] * fRate);
        }

    }
    else
    {
        NSString* nsKey = [NSString stringWithCString:sKey encoding:NSUTF8StringEncoding];
        NSDictionary* pDic = DefaultPropertyX();
        NSArray* pArrData = [pDic objectForKey:nsKey];
        if(pArrData == nil)
        {
            HLogE("[ERROR]PropetyList not Found %s\n",sKey);
            return nil;
        }
        
        for (NSNumber* sv in pArrData)
        {
            list.push_back([sv floatValue]);
        }
    }
#endif
    return &list;
}

void PROPERTYI::ReleaseProperty()
{
#ifdef ANDROID
    if(PROPERTYI::gProperty)
    {
        delete (CHDoc*)PROPERTYI::gProperty;
        PROPERTYI::gProperty = NULL;
    }
#else
    if(PROPERTYI::gProperty)
        [((NSDictionary*)PROPERTYI::gProperty) release];
    PROPERTYI::gProperty = nil;
#endif
}

void PROPERTYI::ReleaseList(vector<PROPERTYI*>* plist)
{
    vector<PROPERTYI*>::iterator i;
    for (i = plist->begin(); i != plist->end(); i++) 
    {
        delete *i;
    }
    plist->clear();
}




//--------------------------------------------------------------
PROPERTY_ITEM::PROPERTY_ITEM()
{
    sName = 0;
    sDesc = 0;
    sDesc2 = 0;
    sModelPath = 0;
    sID = 0;
}

PROPERTY_ITEM::~PROPERTY_ITEM()
{
    if(sName)
    {
        delete[] sName;
        sName = 0;
    }
    if(sDesc)
    {
        delete[] sDesc;
        sDesc = 0;
    }
    if(sDesc2)
    {
        delete[] sDesc2;
        sDesc2 = 0;
    }
    if(sModelPath)
    {
        delete[] sModelPath;
        sModelPath = 0;
    }
    if(sID)
    {
        delete[] sID;
        sID = 0;
    }
}

PROPERTY_ITEM& PROPERTY_ITEM::operator=(const PROPERTY_ITEM& SrcProperty)
{
    
    nID = SrcProperty.nID; //ID
    nType = SrcProperty.nType;
    SetName(SrcProperty.sName); //Name
    SetDesc(SrcProperty.sDesc); //Desc
    SetDesc2(SrcProperty.sDesc2); //Desc2
    nMinLevel = SrcProperty.nType;
    SetModelPath(SrcProperty.sModelPath);//모델경울 K9 =>Body=K9Root.ms3d  Header=K9Header.ms3d K9.png 
    nPrice = SrcProperty.nPrice;
    SetID(SrcProperty.sID);
    return *this;
}

void PROPERTY_ITEM::SetID(const char* sInID)
{
    if(sID) delete [] sID;
    sID = 0;
    if(sInID == 0) return;
    
    sID = new char[ strlen(sInID) + 1];
    strcpy(sID,sInID);
}



void PROPERTY_ITEM::SetName(const char* sInName)
{
    if(sName) delete [] sName;
    sName = 0;
    if(sInName == 0) return;
    
    sName = new char[ strlen(sInName) + 1];
    strcpy(sName,sInName);
}

void PROPERTY_ITEM::SetDesc(const char* sInDesc)
{
    if(sDesc) delete [] sDesc;
    sDesc = 0;
    if(sInDesc == 0) return;
    
    sDesc = new char[ strlen(sInDesc) + 1];
    strcpy(sDesc,sInDesc);
}

void PROPERTY_ITEM::SetDesc2(const char* sInDesc)
{
    if(sDesc2) delete [] sDesc2;
    sDesc2 = 0;
    if(sInDesc == 0) return;
    
    sDesc2 = new char[ strlen(sInDesc) + 1];
    strcpy(sDesc2,sInDesc);
}


void PROPERTY_ITEM::SetModelPath(const char* sInModelPath)
{
    if(sModelPath) delete [] sModelPath;
    sModelPath = 0;
    if(sInModelPath == 0) return;
    
    sModelPath = new char[ strlen(sInModelPath) + 1];
    strcpy(sModelPath,sInModelPath);
}


#ifdef ANDROID
void GetPropertyItemX(NodeArray* arr,PROPERTY_ITEM* pProperty)
{
    pProperty->nID = GetIntProp(arr,0); //ID
    pProperty->nType = GetIntProp(arr,1);
    pProperty->SetName(GetStrProp(arr,2)); //Name
    pProperty->SetDesc(GetStrProp(arr,3)); //Desc
    pProperty->SetDesc2(GetStrProp(arr,4)); //Desc
    pProperty->nMinLevel = GetIntProp(arr,5);
    pProperty->SetModelPath(GetStrProp(arr,6));//모델경울 K9 =>Body=K9Root.ms3d  Header=K9Header.ms3d K9.png
    pProperty->nPrice = GetIntProp(arr,7);
    
    if(arr->mList.size() >= 9)
        pProperty->SetID(GetStrProp(arr,8));//
}

#else
void GetPropertyItemX(NSArray* arr,PROPERTY_ITEM* pProperty)
{
    pProperty->nID = GetIntProp(arr,0); //ID
    pProperty->nType = GetIntProp(arr,1);
    pProperty->SetName(GetStrProp(arr,2)); //Name
    pProperty->SetDesc(GetStrProp(arr,3)); //Desc
    pProperty->SetDesc2(GetStrProp(arr,4)); //Desc
    pProperty->nMinLevel = GetIntProp(arr,5);
    pProperty->SetModelPath(GetStrProp(arr,6));//모델경울 K9 =>Body=K9Root.ms3d  Header=K9Header.ms3d K9.png 
    pProperty->nPrice = GetIntProp(arr,7);
    if(arr.count >= 9)
        pProperty->SetID(GetStrProp(arr,8)); //
}
#endif

PROPERTY_ITEM* PROPERTY_ITEM::GetPropertyItem(int nID, PROPERTY_ITEM& property)
{
#ifdef ANDROID
    const char* sKey = "Items";
    CHDoc* pDic = DefaultPropertyX();
    NodeArray* pArrData = (NodeArray*)pDic->GetData(sKey);
    if(pArrData == NULL)
    {
        HLog("GetPropertyItem not Found %s\n","Items");
        return 0;
    }
    int nCnt = (int)pArrData->mList.size();
    for(int i = 0; i < nCnt; i++)
    {
        NodeArray* arr = (NodeArray*)pArrData->mList[i];
        property.nID = ((NodeDataValue*)arr->mList[0])->GetInt();
        if(property.nID == nID)
        {
            GetPropertyItemX((NodeArray*)arr,&property);
            return &property;
        }
    }
#else
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    try {
        NSString* nsKey = [NSString stringWithCString:"Items" encoding:NSUTF8StringEncoding];
        NSDictionary* pDic = DefaultPropertyX();
        NSArray* pArrData = [pDic objectForKey:nsKey];
        if(pArrData == nil)
        {
            HLog("GetPropertyItem not Found %s\n","Items");
            [pool release];
            return 0;
        }
        
        for (NSArray* arr in pArrData) 
        {
            NSNumber* pID = [arr objectAtIndex:0];
            property.nID = pID.intValue;
            if(property.nID == nID)
            {
                GetPropertyItemX((NSArray*)arr,&property);
                [pool release];
                return &property;
            }
        }
    } 
    catch (...) 
    {
        
    }
    [pool release];
#endif
    return 0;
}


//대소자를 구분하지 않음.
PROPERTY_ITEM* PROPERTY_ITEM::GetPropertyItem(const char *sID, PROPERTY_ITEM& property)
{
#ifdef ANDROID
    const char* sKey = "Items";
    CHDoc* pDic = DefaultPropertyX();
    NodeArray* pArrData = (NodeArray*)pDic->GetData(sKey);
    if(pArrData == NULL)
    {
        HLog("GetPropertyItem not Found %s\n","Items");
        return 0;
    }
    int nCnt = (int)pArrData->mList.size();
    for(int i = 0; i < nCnt; i++)
    {
        NodeArray* arr = (NodeArray*)pArrData->mList[i];
        if(arr->mList.size() >= 9)
        {
            const char* pID = ((NodeDataValue*)arr->mList[8])->GetString();
            if(strcasecmp(pID, sID) == 0)
            {
                GetPropertyItemX((NodeArray*)arr,&property);
                return &property;
            }
        }
    }
#else
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    try {
        NSString* nsKey = [NSString stringWithCString:"Items" encoding:NSUTF8StringEncoding];
        NSDictionary* pDic = DefaultPropertyX();
        NSArray* pArrData = [pDic objectForKey:nsKey];
        if(pArrData == nil)
        {
            HLog("GetPropertyItem not Found %s\n","Items");
            [pool release];
            return 0;
        }
        
        for (NSArray* arr in pArrData)
        {
            if(arr.count >= 9)
            {
                NSString* pID = [arr objectAtIndex:8];
                if(strcasecmp(pID.UTF8String, sID) == 0)
                {
                    GetPropertyItemX((NSArray*)arr,&property);
                    [pool release];
                    return &property;
                }
            }
        }
    }
    catch (...)
    {
        
    }
    [pool release];
#endif
    return 0;
}

vector<PROPERTY_ITEM*>* PROPERTY_ITEM::GetPropertyItemList(vector<PROPERTY_ITEM*>& list)
{
#ifdef ANDROID
    
    const char* sKey = "Items";
    CHDoc* pDic = DefaultPropertyX();
    if(pDic == NULL) return NULL;
    NodeArray* pArrData = (NodeArray*)pDic->GetData(sKey);
    if(pArrData == NULL)
    {
        HLog("GetPropertyItemList not Found %s\n","Tanks");
        return NULL;
    }
    
    int nCnt = (int)pArrData->mList.size();
    for(int i = 0; i < nCnt; i++)
    {
        PROPERTY_ITEM *pNewPropery = new PROPERTY_ITEM;
        GetPropertyItemX((NodeArray*)pArrData->mList[i],pNewPropery);
        //유료맵은 광고아이템을 제거해준다.
        if(sglNoFree() && pNewPropery->nID == ID_ITEM_REMOVE_AD)
        {
            delete pNewPropery;
            continue;
        }
        list.push_back(pNewPropery);
    }

#else
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    try 
    {

        NSString* nsKey = [NSString stringWithCString:"Items" encoding:NSUTF8StringEncoding];
        NSDictionary* pDic = DefaultPropertyX();
        NSArray* pArrData = [pDic objectForKey:nsKey];
        if(pArrData == nil)
        {
            HLog("GetPropertyItemList not Found %s\n","Tanks");
            [pool release];
            return 0;
        }
    
        for (NSArray* arr in pArrData) 
        {
            //유료맵은 광고아이템을 제거해준다.
            if(sglNoFree() && GetIntProp(arr,0) == ID_ITEM_REMOVE_AD) continue;
            PROPERTY_ITEM *pNewPropery = new PROPERTY_ITEM;
            GetPropertyItemX(arr,pNewPropery);
            list.push_back(pNewPropery);
        }
    } 
    catch (...) 
    {
        HLogE("[Error] GetPropertyItemList Error\n");
    }
    [pool release];
#endif
    return &list;
}



//--------------------------------------------------------------
PROPERTY_MAP::PROPERTY_MAP()
{
    sName = 0;
    sDesc = 0;
    sModelPath = 0;
}

PROPERTY_MAP::~PROPERTY_MAP()
{
    if(sName)
    {
        delete[] sName;
        sName = 0;
    }
    if(sDesc)
    {
        delete[] sDesc;
        sDesc = 0;
    }
    if(sModelPath)
    {
        delete[] sModelPath;
        sModelPath = 0;
    }
}

PROPERTY_MAP& PROPERTY_MAP::operator=(const PROPERTY_MAP& SrcProperty)
{
    
    nID = SrcProperty.nID; //ID
    nType = SrcProperty.nType;
    SetName(SrcProperty.sName); //Name
    SetDesc(SrcProperty.sDesc); //Desc
    nMinLevel = SrcProperty.nType;
    SetModelPath(SrcProperty.sModelPath);//모델경울 K9 =>Body=K9Root.ms3d  Header=K9Header.ms3d K9.png 
    return *this;
}


void PROPERTY_MAP::SetName(const char* sInName)
{
    if(sName) delete [] sName;
    sName = 0;
    if(sInName == 0) return;
    
    sName = new char[ strlen(sInName) + 1];
    strcpy(sName,sInName);
}

void PROPERTY_MAP::SetDesc(const char* sInDesc)
{
    if(sDesc) delete [] sDesc;
    sDesc = 0;
    if(sInDesc == 0) return;
    
    sDesc = new char[ strlen(sInDesc) + 1];
    strcpy(sDesc,sInDesc);
}


void PROPERTY_MAP::SetModelPath(const char* sInModelPath)
{
    if(sModelPath) delete [] sModelPath;
    sModelPath = 0;
    if(sInModelPath == 0) return;
    
    sModelPath = new char[ strlen(sInModelPath) + 1];
    strcpy(sModelPath,sInModelPath);
}


#ifdef ANDROID
void GetPropertyMapX(NodeArray* arr,PROPERTY_MAP* pProperty)
{
    pProperty->nID = GetIntProp(arr,0); //ID
    pProperty->nType = GetIntProp(arr,1);
    pProperty->SetName(GetStrProp(arr,2)); //Name
    pProperty->SetDesc(GetStrProp(arr,3)); //Desc
    pProperty->nMinLevel = GetIntProp(arr,4);
    pProperty->SetModelPath(GetStrProp(arr,5));//모델경울 K9 =>Body=K9Root.ms3d  Header=K9Header.ms3d K9.png
}

#else
void GetPropertyMapX(NSArray* arr,PROPERTY_MAP* pProperty)
{
    pProperty->nID = GetIntProp(arr,0); //ID
    pProperty->nType = GetIntProp(arr,1);
    pProperty->SetName(GetStrProp(arr,2)); //Name
    pProperty->SetDesc(GetStrProp(arr,3)); //Desc
    pProperty->nMinLevel = GetIntProp(arr,4);
    pProperty->SetModelPath(GetStrProp(arr,5));//모델경울 K9 =>Body=K9Root.ms3d  Header=K9Header.ms3d K9.png 
}
#endif

PROPERTY_MAP* PROPERTY_MAP::GetPropertyMap(int nID, PROPERTY_MAP& property)
{
#ifdef ANDROID

    const char* sKey = "Maps";
    CHDoc* pDic = DefaultPropertyX();
    NodeArray* pArrData = (NodeArray*)pDic->GetData(sKey);
    if(pArrData == NULL)
    {
        HLog("GetPropertyItem not Found %s\n","Maps");
        return 0;
    }
    
    int nCnt = (int)pArrData->mList.size();
    for(int i = 0; i < nCnt; i++)
    {
        
        NodeArray* arr = (NodeArray*)pArrData->mList[i];
        property.nID = ((NodeDataValue*)arr->mList[0])->GetInt();
        
        if(property.nID == nID)
        {
            GetPropertyMapX((NodeArray*)arr,&property);
            return &property;
        }
        }
#else
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    try {
        NSString* nsKey = [NSString stringWithCString:"Maps" encoding:NSUTF8StringEncoding];
        NSDictionary* pDic = DefaultPropertyX();
        NSArray* pArrData = [pDic objectForKey:nsKey];
        if(pArrData == nil)
        {
            HLog("GetPropertyItem not Found %s\n","Maps");
            [pool release];
            return 0;
        }
        
        for (NSArray* arr in pArrData) 
        {
            NSNumber* pID = [arr objectAtIndex:0];
            property.nID = pID.intValue;
            if(property.nID == nID)
            {
                GetPropertyMapX((NSArray*)arr,&property);
                [pool release];
                return &property;
            }
        }
    } 
    catch (...) 
    {
        HLogE("[Error] Exception GetPropertyMap %s\n","Maps");
    }
    [pool release];
#endif
    return 0;
}

vector<PROPERTY_MAP*>* PROPERTY_MAP::GetPropertyMapList(vector<PROPERTY_MAP*>& list)
{
#ifdef ANDROID

    const char* sKey = "Maps";
    CHDoc* pDic = DefaultPropertyX();
    NodeArray* pArrData = (NodeArray*)pDic->GetData(sKey);
    if(pArrData == NULL)
    {
        HLog("GetPropertyItemList not Found %s\n","Maps");
        return 0;
    }
    
    //for (NSArray* arr in pArrData)
    int nCnt = (int)pArrData->mList.size();
    for(int i = 0; i < nCnt; i++)
    {
        PROPERTY_MAP *pNewPropery = new PROPERTY_MAP;
        GetPropertyMapX((NodeArray*)pArrData->mList[i],pNewPropery);
        list.push_back(pNewPropery);
        }
#else
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    try {
        NSString* nsKey = [NSString stringWithCString:"Maps" encoding:NSUTF8StringEncoding];
        NSDictionary* pDic = DefaultPropertyX();
        NSArray* pArrData = [pDic objectForKey:nsKey];
        if(pArrData == nil)
        {
            HLog("GetPropertyItemList not Found %s\n","Maps");
            [pool release];
            return 0;
        }
        
        for (NSArray* arr in pArrData) 
        {
            PROPERTY_MAP *pNewPropery = new PROPERTY_MAP;
            GetPropertyMapX(arr,pNewPropery);
            list.push_back(pNewPropery);
        }
    } 
    catch (...) 
    {
        HLogE("[Error] Exception GetPropertyMapList %s\n","Maps");
    }
    [pool release];
#endif
    return &list;
}


//--------------------------------------------------------------
PROPERTY_BOMB::PROPERTY_BOMB()
{
    sName = 0;
    sDesc = 0;
    sModelPath = 0;
    sBombTableImgPath = 0;
    sBombBombImgPath = 0;
    sBombTailImgPath = 0;
    nID = -1;
    fFirePerSec= 1.f;
}

PROPERTY_BOMB::~PROPERTY_BOMB()
{
    if(sName)
    {
        delete[] sName;
        sName = 0;
    }
    if(sDesc)
    {
        delete[] sDesc;
        sDesc = 0;
    }
    if(sModelPath)
    {
        delete[] sModelPath;
        sModelPath = 0;
    }
    if(sBombTableImgPath)
    {
        delete[] sBombTableImgPath;
        sBombTableImgPath = 0;
    }
    
    if(sBombBombImgPath)
    {
        delete[] sBombBombImgPath;
        sBombBombImgPath = 0;
    }
    
    if(sBombTailImgPath)
    {
        delete[] sBombTailImgPath;
        sBombTailImgPath = 0;
    }
}

PROPERTY_BOMB& PROPERTY_BOMB::operator=(const PROPERTY_BOMB& SrcProperty)
{
    
    nID = SrcProperty.nID; //ID
    nType = SrcProperty.nType;
    SetName(SrcProperty.sName); //Name
    SetDesc(SrcProperty.sDesc); //Desc
    nBombType = SrcProperty.nBombType;
    SetModelPath(SrcProperty.sModelPath);//모델경울 K9 =>Body=K9Root.ms3d  Header=K9Header.ms3d K9.png
    SetTableImgPath(SrcProperty.sBombTableImgPath);
    
        
    fMaxAttackPower = SrcProperty.fMaxAttackPower;
    fMaxRadianDetect = SrcProperty.fMaxRadianDetect;
    fScale = SrcProperty.fScale;
    fVelocity = SrcProperty.fVelocity;
    fAutoDectect = SrcProperty.fAutoDectect;
    nPrice = SrcProperty.nPrice;
    nMenuVisible = SrcProperty.nMenuVisible;
    fStartPosition = SrcProperty.fStartPosition;
    nBombTailMaxAliveTime = SrcProperty.nBombTailMaxAliveTime;
    fBombTailScale = SrcProperty.fBombTailScale;
    
    nSoundFilreID = SrcProperty.nSoundFilreID;
    nSoundCompactID = SrcProperty.nSoundCompactID;
    nSoundFarCompactID = SrcProperty.nSoundFarCompactID;
    nPackCount = SrcProperty.nPackCount;
    
    
    cDivide = SrcProperty.cDivide;;
    nDividedID = SrcProperty.nDividedID;
    cRotation = SrcProperty.cRotation;
    
    
    SetBombBombImgPath(SrcProperty.sBombBombImgPath);
    SetBombTailImgPath(SrcProperty.sBombTailImgPath);

    fFirePerSec = SrcProperty.fFirePerSec;
    return *this;
}


void PROPERTY_BOMB::SetName(const char* sInName)
{
    if(sName) delete [] sName;
    sName = 0;
    if(sInName == 0) return;
    
    sName = new char[ strlen(sInName) + 1];
    strcpy(sName,sInName);
}

void PROPERTY_BOMB::SetDesc(const char* sInDesc)
{
    if(sDesc) delete [] sDesc;
    sDesc = 0;
    if(sInDesc == 0) return;
    
    sDesc = new char[ strlen(sInDesc) + 1];
    strcpy(sDesc,sInDesc);
}


void PROPERTY_BOMB::SetModelPath(const char* sInModelPath)
{
    if(sModelPath) delete [] sModelPath;
    sModelPath = 0;
    if(sInModelPath == 0) return;
    
    sModelPath = new char[ strlen(sInModelPath) + 1];
    strcpy(sModelPath,sInModelPath);
}

void PROPERTY_BOMB::SetTableImgPath(const char* sInBombTableImg)
{
    if(sBombTableImgPath) delete [] sBombTableImgPath;
    sBombTableImgPath = 0;
    if(sInBombTableImg == 0) return;
    
    sBombTableImgPath = new char[ strlen(sInBombTableImg) + 1];
    strcpy(sBombTableImgPath,sInBombTableImg);
}


void PROPERTY_BOMB::SetBombBombImgPath(const char* sInBombBombImgPath)
{
    if(sBombBombImgPath) delete [] sBombBombImgPath;
    sBombBombImgPath = 0;
    if(sInBombBombImgPath == 0) return;
    
    sBombBombImgPath = new char[ strlen(sInBombBombImgPath) + 1];
    strcpy(sBombBombImgPath,sInBombBombImgPath);
}

void PROPERTY_BOMB::SetBombTailImgPath(const char* sInBombTailImgPath)
{
    if(sBombTailImgPath) delete [] sBombTailImgPath;
    sBombTailImgPath = 0;
    if(sInBombTailImgPath == 0) return;
    
    sBombTailImgPath = new char[ strlen(sInBombTailImgPath) + 1];
    strcpy(sBombTailImgPath,sInBombTailImgPath);

}

#ifdef ANDROID
void GetPropertyBombX(NodeArray* arr,PROPERTY_BOMB* pProperty)
{
    pProperty->nID = GetIntProp(arr,0); //ID
    pProperty->nType = GetIntProp(arr,1);
    pProperty->SetName(GetStrProp(arr,2)); //Name
    pProperty->SetDesc(GetStrProp(arr,3)); //Desc
    pProperty->nBombType = GetIntProp(arr,4);
    pProperty->SetModelPath(GetStrProp(arr,5));
    pProperty->SetTableImgPath(GetStrProp(arr,6));
    pProperty->fMaxAttackPower = GetFloatProp(arr,7);
    pProperty->fMaxRadianDetect = GetFloatProp(arr,8);
    pProperty->fScale = GetFloatProp(arr,9);
    pProperty->fVelocity = GetFloatProp(arr,10);
    pProperty->fAutoDectect = GetFloatProp(arr,11);
    pProperty->nPrice = GetIntProp(arr,12);
    pProperty->nMenuVisible = GetIntProp(arr, 13);
    pProperty->fStartPosition = GetFloatProp(arr, 14);
    pProperty->nBombTailMaxAliveTime = GetIntProp(arr, 15);
    pProperty->fBombTailScale = GetFloatProp(arr, 16);
    
    pProperty->nSoundFilreID = GetIntProp(arr,17);
    pProperty->nSoundCompactID = GetIntProp(arr,18);
    pProperty->nSoundFarCompactID = GetIntProp(arr,19);
    pProperty->nPackCount = GetIntProp(arr,20);
    
    pProperty->cDivide = GetIntProp(arr,21);
    pProperty->nDividedID = GetIntProp(arr,22);
    pProperty->cRotation = GetIntProp(arr,23);
    
    pProperty->SetBombBombImgPath(GetStrProp(arr,24));
    pProperty->SetBombTailImgPath(GetStrProp(arr,25));
    pProperty->fFirePerSec = GetFloatProp(arr, 26);
}

#else
void GetPropertyBombX(NSArray* arr,PROPERTY_BOMB* pProperty)
{
    pProperty->nID = GetIntProp(arr,0); //ID
    pProperty->nType = GetIntProp(arr,1);
    pProperty->SetName(GetStrProp(arr,2)); //Name
    pProperty->SetDesc(GetStrProp(arr,3)); //Desc
    pProperty->nBombType = GetIntProp(arr,4);
    pProperty->SetModelPath(GetStrProp(arr,5));
    pProperty->SetTableImgPath(GetStrProp(arr,6));
    pProperty->fMaxAttackPower = GetFloatProp(arr,7);
    pProperty->fMaxRadianDetect = GetFloatProp(arr,8);
    pProperty->fScale = GetFloatProp(arr,9);
    pProperty->fVelocity = GetFloatProp(arr,10);
    pProperty->fAutoDectect = GetFloatProp(arr,11);
    pProperty->nPrice = GetIntProp(arr,12);
    pProperty->nMenuVisible = GetIntProp(arr, 13);
    pProperty->fStartPosition = GetFloatProp(arr, 14);
    pProperty->nBombTailMaxAliveTime = GetIntProp(arr, 15);
    pProperty->fBombTailScale = GetFloatProp(arr, 16);
    
    pProperty->nSoundFilreID = GetIntProp(arr,17);
    pProperty->nSoundCompactID = GetIntProp(arr,18);
    pProperty->nSoundFarCompactID = GetIntProp(arr,19);
    pProperty->nPackCount = GetIntProp(arr,20);
    
    pProperty->cDivide = GetIntProp(arr,21);
    pProperty->nDividedID = GetIntProp(arr,22);
    pProperty->cRotation = GetIntProp(arr,23);
    
    pProperty->SetBombBombImgPath(GetStrProp(arr,24));
    pProperty->SetBombTailImgPath(GetStrProp(arr,25));
    pProperty->fFirePerSec = GetIntProp(arr, 26);
}
#endif

PROPERTY_BOMB* PROPERTY_BOMB::GetPropertyBomb(int nID, PROPERTY_BOMB& property)
{
#ifdef ANDROID

    const char* sKey = "Bombs";
    CHDoc* pDic = DefaultPropertyX();
    NodeArray* pArrData = (NodeArray*)pDic->GetData(sKey);
    if(pArrData == NULL)
    {
        HLog("GetPropertyBomb not Found %s\n","Bombs");
        return NULL;
    }
    
    int nCnt = (int)pArrData->mList.size();
    for(int i = 0; i < nCnt; i++)
    {
        NodeArray* arr = (NodeArray*)pArrData->mList[i];
        property.nID = ((NodeDataValue*)arr->mList[0])->GetInt();
        if(property.nID == nID)
        {
            GetPropertyBombX((NodeArray*)arr,&property);
            return &property;
        }
    }

#else
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    try 
    {
        NSString* nsKey = [NSString stringWithCString:"Bombs" encoding:NSUTF8StringEncoding];
        NSDictionary* pDic = DefaultPropertyX();
        NSArray* pArrData = [pDic objectForKey:nsKey];
        if(pArrData == nil)
        {
            HLog("GetPropertyBomb not Found %s\n","Bombs");
            [pool release];
            return 0;
        }
        
        for (NSArray* arr in pArrData) 
        {
            NSNumber* pID = [arr objectAtIndex:0];
            property.nID = pID.intValue;
            if(property.nID == nID)
            {
                GetPropertyBombX((NSArray*)arr,&property);
                [pool release];
                return &property;
            }
        }
    } 
    catch (...)
    {
        HLogE("[Error] Exception GetPropertyBomb %s\n","Bombs");
    }
    [pool release];
#endif
    return 0;
}

vector<PROPERTY_BOMB*>* PROPERTY_BOMB::GetPropertyBombList(vector<PROPERTY_BOMB*>& list)
{
#ifdef ANDROID
    const char* sKey = "Bombs";
    CHDoc* pDic = DefaultPropertyX();
    NodeArray* pArrData = (NodeArray*)pDic->GetData(sKey);
    if(pArrData == NULL)
    {
        HLog("GetPropertyItemList not Found %s\n","Bombs");
        return 0;
    }
    
    //for (NSArray* arr in pArrData)
    int nCnt = (int)pArrData->mList.size();
    for(int i = 0; i < nCnt; i++)
    {
        PROPERTY_BOMB *pNewPropery = new PROPERTY_BOMB;
        GetPropertyBombX((NodeArray*)pArrData->mList[i],pNewPropery);
        list.push_back(pNewPropery);
    }
#else
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    try {
        NSString* nsKey = [NSString stringWithCString:"Bombs" encoding:NSUTF8StringEncoding];
        NSDictionary* pDic = DefaultPropertyX();
        NSArray* pArrData = [pDic objectForKey:nsKey];
        if(pArrData == nil)
        {
            HLog("GetPropertyBombList not Found %s\n","Bombs");
            [pool release];
            return 0;
        }
        
        for (NSArray* arr in pArrData) 
        {
            PROPERTY_BOMB *pNewPropery = new PROPERTY_BOMB;
            GetPropertyBombX(arr,pNewPropery);
            list.push_back(pNewPropery);
        }
    } 
    catch (...) 
    {
        HLogE("[Error] Exception GetPropertyBombList %s\n","Bombs");
    }
    [pool release];
#endif
    return &list;
}



//--------------------------------------------------------------
PROPERTY_TANK::PROPERTY_TANK()
{
    sName = 0;           
    sDesc = 0;           
    sModelPath = 0;      
    sEnemyBoosterPath = 0;
    sMisailStartPos = 0;
    fDefendRadianBounds = 77841.f;  //279.f * 279.f;
    nMaxAttackPower = 100;
    nGunID = 0;
}

PROPERTY_TANK::~PROPERTY_TANK()
{
    if(sName)
    {
        delete[] sName;
        sName = 0;
    }
    if(sDesc)
    {
        delete[] sDesc;
        sDesc = 0;
    }
    if(sModelPath)
    {
        delete[] sModelPath;
        sModelPath = 0;
    }
    if(sEnemyBoosterPath)
    {
        delete[] sEnemyBoosterPath;
        sEnemyBoosterPath = 0;
    }
    if(sMisailStartPos)
    {
        delete[] sMisailStartPos;
        sMisailStartPos = 0;
    }
}


PROPERTY_TANK& PROPERTY_TANK::operator=(const PROPERTY_TANK& SrcProperty)
{
    
    nID = SrcProperty.nID; //ID
    nType = SrcProperty.nType; //1:K9시리즈 2:ET1시리즈 3:지구연함군 건물
    nEnemy =  SrcProperty.nEnemy; //0: 지구연합군 1:Enemy
    SetName(SrcProperty.sName); //Name
    SetDesc(SrcProperty.sDesc); //Desc
    nMinLevel = SrcProperty.nMinLevel; //MinLevel
    SetModelPath(SrcProperty.sModelPath);//모델경울 K9 =>Body=K9Root.ms3d  Header=K9Header.ms3d K9.png 
    SetEnemyBoosterPath(SrcProperty.sEnemyBoosterPath);//적일때는 적의 날라다니는 이미지 경로   
    nFirePerSec = SrcProperty.nFirePerSec; //초당 발사 속도
    nMaxDefendPower = SrcProperty.nMaxDefendPower; //방어력
    fRotationRate = SrcProperty.fRotationRate; //해더의 회전속도 0.1
    fMoveRate = SrcProperty.fMoveRate; //속도.0.015f
    fHalfX = SrcProperty.fHalfX; //탱크의 넓이 / 2 K9:63.0
    fHalfZ = SrcProperty.fHalfZ; //탱크의 넓이 / 2 K9:90.0
    fHeaderPosX = SrcProperty.fHeaderPosX; //K9 : 0
    fHeaderPosY = SrcProperty.fHeaderPosY; //K9 : 48 //몸체에서 (0,48) 뒤의 위치에 있다.
    fHeaderPosZ = SrcProperty.fHeaderPosZ; //K9 : 0
    
    fScale = SrcProperty.fScale; //크기 K9 : 0.06
    fHeaderTurnSize =  SrcProperty.fHeaderTurnSize; //0.6
    fFireAnimationAcc = SrcProperty.fFireAnimationAcc; //0.25  탱크의 파이어시 해더의 움직임 속도 조절
    fMaxScore = SrcProperty.fMaxScore; //탱크의 점수
    nPoStep = SrcProperty.nPoStep; //포의 애니메이션 단계 48
    nDefaultBombID = SrcProperty.nDefaultBombID;
    SetMisailStartPos(SrcProperty.sMisailStartPos);
    fDefendRadianBounds = SrcProperty.fDefendRadianBounds;
    nMaxAttackPower = SrcProperty.nMaxAttackPower;
    nGunID = SrcProperty.nGunID;
    return *this;
}


void PROPERTY_TANK::SetMisailStartPos(const char* sInMisailStartPos)
{
    if(sMisailStartPos) delete [] sMisailStartPos;
    sMisailStartPos = 0;
    if(sInMisailStartPos == 0) return;
    
    sMisailStartPos = new char[ strlen(sInMisailStartPos) + 1];
     strcpy(sMisailStartPos,sInMisailStartPos);
}
   
    
void PROPERTY_TANK::SetName(const char* sInName)
{
    if(sName) delete [] sName;
    sName = 0;
    if(sInName == 0) return;
    
    sName = new char[ strlen(sInName) + 1];
    strcpy(sName,sInName);
}

void PROPERTY_TANK::SetDesc(const char* sInDesc)
{
    if(sDesc) delete [] sDesc;
    sDesc = 0;
    if(sInDesc == 0) return;
    
    sDesc = new char[ strlen(sInDesc) + 1];
    strcpy(sDesc,sInDesc);
}

void PROPERTY_TANK::SetModelPath(const char* sInModelPath)
{
    if(sModelPath) delete [] sModelPath;
    sModelPath = 0;
    if(sInModelPath == 0) return;

    
    sModelPath = new char[ strlen(sInModelPath) + 1];
    strcpy(sModelPath,sInModelPath);
}

void PROPERTY_TANK::SetEnemyBoosterPath(const char* sInEnemyBoosterPath)
{
    if(sEnemyBoosterPath) delete [] sEnemyBoosterPath;
    sEnemyBoosterPath = 0;
    if(sInEnemyBoosterPath == 0) return;

    sEnemyBoosterPath = new char[ strlen(sInEnemyBoosterPath) + 1];
    strcpy(sEnemyBoosterPath,sInEnemyBoosterPath);
}

#ifdef ANDROID
void GetPropertyTankX(NodeArray* arr,PROPERTY_TANK* pProperty)
{
    pProperty->nID = GetIntProp(arr,0); //ID
    pProperty->nType = GetIntProp(arr,1); //1:K9시리즈 2:ET1시리즈
    pProperty->nEnemy = GetIntProp(arr,2); //0: Actor 1:Enemy
    pProperty->SetName(GetStrProp(arr,3)); //Name
    pProperty->SetDesc(GetStrProp(arr,4)); //Desc
    pProperty->nMinLevel = GetIntProp(arr,5); //MinLevel
    pProperty->SetModelPath(GetStrProp(arr,6)); //모델경울 K9 =>Body=K9Root.ms3d  Header=K9Header.ms3d K9.png
    pProperty->SetEnemyBoosterPath(GetStrProp(arr,7)); //적일때는 적의 날라다니는 이미지 경로
    pProperty->nFirePerSec = GetIntProp(arr,8); //초당 발사 속도
    pProperty->nMaxDefendPower = GetIntProp(arr,9); //방어력
    pProperty->fRotationRate = GetFloatProp(arr,10); //해더의 회전속도 0.1
    pProperty->fMoveRate = GetFloatProp(arr,11); //속도.0.015f
    pProperty->fHalfX = GetFloatProp(arr,12); //탱크의 넓이 / 2 K9:63.0
    pProperty->fHalfZ = GetFloatProp(arr,13); //탱크의 넓이 / 2 K9:90.0
    pProperty->fHeaderPosX = GetFloatProp(arr,14); //K9 : 0
    pProperty->fHeaderPosY = GetFloatProp(arr,15); //K9 : 48 //몸체에서 (0,48) 뒤의 위치에 있다.
    pProperty->fHeaderPosZ = GetFloatProp(arr,16); //K9 : 0
    
    pProperty->fScale = GetFloatProp(arr,17); //크기 K9 : 0.06
    pProperty->fHeaderTurnSize = GetFloatProp(arr,18); //0.6
    pProperty->fFireAnimationAcc = GetFloatProp(arr,19); //0.25  탱크의 파이어시 해더의 움직임 속도 조절
    pProperty->fMaxScore = GetFloatProp(arr,20); //탱크의 점수
    pProperty->nPoStep = GetIntProp(arr,21); //포의 애니메이션 단계 48
    pProperty->nDefaultBombID = GetIntProp(arr, 22); //Default 미사일 아이디.
    pProperty->SetMisailStartPos(GetStrProp(arr,23)); //미사일 시작 위치. up,down,up,down
    pProperty->fDefendRadianBounds = GetFloatProp(arr, 24);
    pProperty->nMaxAttackPower = GetIntProp(arr, 25);
    pProperty->nGold = GetIntProp(arr, 26);
    pProperty->nGunID = GetIntProp(arr, 27);
}
#else
void GetPropertyTankX(NSArray* arr,PROPERTY_TANK* pProperty)
{
    pProperty->nID = GetIntProp(arr,0); //ID
    pProperty->nType = GetIntProp(arr,1); //1:K9시리즈 2:ET1시리즈
    pProperty->nEnemy = GetIntProp(arr,2); //0: Actor 1:Enemy
    pProperty->SetName(GetStrProp(arr,3)); //Name
    pProperty->SetDesc(GetStrProp(arr,4)); //Desc
    pProperty->nMinLevel = GetIntProp(arr,5); //MinLevel
    pProperty->SetModelPath(GetStrProp(arr,6)); //모델경울 K9 =>Body=K9Root.ms3d  Header=K9Header.ms3d K9.png 
    pProperty->SetEnemyBoosterPath(GetStrProp(arr,7)); //적일때는 적의 날라다니는 이미지 경로   
    pProperty->nFirePerSec = GetIntProp(arr,8); //초당 발사 속도
    pProperty->nMaxDefendPower = GetIntProp(arr,9); //방어력
    pProperty->fRotationRate = GetFloatProp(arr,10); //해더의 회전속도 0.1
    pProperty->fMoveRate = GetFloatProp(arr,11); //속도.0.015f
    pProperty->fHalfX = GetFloatProp(arr,12); //탱크의 넓이 / 2 K9:63.0
    pProperty->fHalfZ = GetFloatProp(arr,13); //탱크의 넓이 / 2 K9:90.0
    pProperty->fHeaderPosX = GetFloatProp(arr,14); //K9 : 0
    pProperty->fHeaderPosY = GetFloatProp(arr,15); //K9 : 48 //몸체에서 (0,48) 뒤의 위치에 있다.
    pProperty->fHeaderPosZ = GetFloatProp(arr,16); //K9 : 0
    
    pProperty->fScale = GetFloatProp(arr,17); //크기 K9 : 0.06
    pProperty->fHeaderTurnSize = GetFloatProp(arr,18); //0.6
    pProperty->fFireAnimationAcc = GetFloatProp(arr,19); //0.25  탱크의 파이어시 해더의 움직임 속도 조절
    pProperty->fMaxScore = GetFloatProp(arr,20); //탱크의 점수
    pProperty->nPoStep = GetIntProp(arr,21); //포의 애니메이션 단계 48
    pProperty->nDefaultBombID = GetIntProp(arr, 22); //Default 미사일 아이디.
    pProperty->SetMisailStartPos(GetStrProp(arr,23)); //미사일 시작 위치. up,down,up,down
    pProperty->fDefendRadianBounds = GetFloatProp(arr, 24);
    pProperty->nMaxAttackPower = GetIntProp(arr, 25);
     pProperty->nGold = GetIntProp(arr, 26);
    pProperty->nGunID = GetIntProp(arr, 27);
}
#endif

PROPERTY_TANK* PROPERTY_TANK::GetPropertyTank(int nID, PROPERTY_TANK& property)
{
#ifdef ANDROID
    const char* sKey = "Tanks";
    CHDoc* pDic = DefaultPropertyX();
    NodeArray* pArrData = (NodeArray*)pDic->GetData(sKey);
    if(pArrData == NULL)
    {
        HLog("GetPropertyTankList not Found %s\n","Tanks");
        return NULL;
    }
    
    int nCnt = (int)pArrData->mList.size();
    for(int i = 0; i < nCnt; i++)
    {
        NodeArray* arr = (NodeArray*)pArrData->mList[i];
        property.nID = ((NodeDataValue*)arr->mList[0])->GetInt();
        if(property.nID == nID)
        {
            GetPropertyTankX((NodeArray*)arr,&property);
            return &property;
        }
    }
#else
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];

    try 
    {
        NSString* nsKey = [NSString stringWithCString:"Tanks" encoding:NSUTF8StringEncoding];
        NSDictionary* pDic = DefaultPropertyX();
        NSArray* pArrData = [pDic objectForKey:nsKey];
        if(pArrData == nil)
        {
            HLog("GetPropertyTankList not Found %s\n","Tanks");
            [pool release];
            return 0;
        }
    
        for (NSArray* arr in pArrData) 
        {
            NSNumber* pID = [arr objectAtIndex:0];
            property.nID = pID.intValue;
            if(property.nID == nID)
            {
                GetPropertyTankX((NSArray*)arr,&property);
                [pool release];
                return &property;
            }
        }    
    } 
    catch (...) 
    {
        HLogE("[Error] Exception GetPropertyTank %s\n","Tanks");
    }
    [pool release];
#endif
    return 0;
}


vector<PROPERTY_TANK*>* PROPERTY_TANK::GetPropertyTankList(vector<PROPERTY_TANK*>& list)
{
#ifdef ANDROID
    const char* sKey = "Tanks";
    CHDoc* pDic = DefaultPropertyX();
    NodeArray* pArrData = (NodeArray*)pDic->GetData(sKey);
    if(pArrData == NULL)
    {
        HLog("GetPropertyTankList not Found %s\n","Tanks");
        return 0;
    }
    
    //for (NSArray* arr in pArrData)
    int nCnt = (int)pArrData->mList.size();
    for(int i = 0; i < nCnt; i++)
    {
        PROPERTY_TANK *pNewPropery = new PROPERTY_TANK;
        GetPropertyTankX((NodeArray*)pArrData->mList[i],pNewPropery);
        list.push_back(pNewPropery);
    }

#else
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    try {
        NSString* nsKey = [NSString stringWithCString:"Tanks" encoding:NSUTF8StringEncoding];
        NSDictionary* pDic = DefaultPropertyX();
        NSArray* pArrData = [pDic objectForKey:nsKey];
        if(pArrData == nil)
        {
            HLog("GetPropertyTankList not Found %s\n","Tanks");
            [pool release];
            return 0;
        }
        
        for (NSArray* arr in pArrData) 
        {
            PROPERTY_TANK *pNewPropery = new PROPERTY_TANK;
            GetPropertyTankX(arr,pNewPropery);
            list.push_back(pNewPropery);
        }
    } 
    catch (...) 
    {
        HLogE("[Error] Exception GetPropertyTankList %s\n","Tanks");
    }
    [pool release];
#endif
    return &list;
}




//--------------------------------------------------------------
PROPERTY_RUNTIMEITEM::PROPERTY_RUNTIMEITEM()
{
    nID = 0;            //0: 아이디
    nType = 0;          //1: 0:Bomb 1:Item
    nItemID = 0;        //2: 실제 아이디
    sModelPath = NULL;     //3; 모델 경로
    nCount = 0;         //4: 아이템을 먹었을 경우 한번에 올라가는 숫자.
    nOther = 0;         //5: 예비.
}
PROPERTY_RUNTIMEITEM::~PROPERTY_RUNTIMEITEM()
{
    if(sModelPath)
    {
        delete[] sModelPath;
        sModelPath = NULL;
    }
}

PROPERTY_RUNTIMEITEM& PROPERTY_RUNTIMEITEM::operator=(const PROPERTY_RUNTIMEITEM& SrcProperty)
{
    nID = SrcProperty.nID;
    nType = SrcProperty.nType;
    nItemID = SrcProperty.nItemID;
    SetModelPath(SrcProperty.sModelPath);
    nCount = SrcProperty.nCount;
    nOther = SrcProperty.nOther;
    return *this;
}

#ifdef ANDROID
void GetPropertyRuntimeItemX(NodeArray* arr,PROPERTY_RUNTIMEITEM* pProperty)
{
    pProperty->nID = GetIntProp(arr,0);
    pProperty->nType = GetIntProp(arr,1);
    pProperty->nItemID = GetIntProp(arr,2);
    pProperty->SetModelPath(GetStrProp(arr,3));
    pProperty->nCount = GetIntProp(arr,4);
    pProperty->nOther = GetIntProp(arr,5);
    
}
#else
void GetPropertyRuntimeItemX(NSArray* arr,PROPERTY_RUNTIMEITEM* pProperty)
{
    pProperty->nID = GetIntProp(arr,0);
    pProperty->nType = GetIntProp(arr,1);
    pProperty->nItemID = GetIntProp(arr,2);
    pProperty->SetModelPath(GetStrProp(arr,3));
    pProperty->nCount = GetIntProp(arr,4);
    pProperty->nOther = GetIntProp(arr,5);
    
}
#endif

void PROPERTY_RUNTIMEITEM::SetModelPath(const char* sInModelPath)
{
    if(sModelPath) delete [] sModelPath;
    sModelPath = 0;
    if(sInModelPath == 0) return;
    
    sModelPath = new char[ strlen(sInModelPath) + 1];
    strcpy(sModelPath,sInModelPath);
}

vector<PROPERTY_RUNTIMEITEM*>* PROPERTY_RUNTIMEITEM::GetPropertyRunTimeItemList(vector<PROPERTY_RUNTIMEITEM*>& list)
{
#ifdef ANDROID
    const char* sKey = "RuntimItem";
    CHDoc* pDic = DefaultPropertyX();
    NodeArray* pArrData = (NodeArray*)pDic->GetData(sKey);
    if(pArrData == NULL)
    {
        HLog("GetPropertyRunTimeItem not Found %s\n","RuntimItem");
        return 0;
    }
    
    //for (NSArray* arr in pArrData)
    int nCnt = (int)pArrData->mList.size();
    for(int i = 0; i < nCnt; i++)
    {
        PROPERTY_RUNTIMEITEM *pNewPropery = new PROPERTY_RUNTIMEITEM;
        GetPropertyRuntimeItemX((NodeArray*)pArrData->mList[i],pNewPropery);
        list.push_back(pNewPropery);
    }

#else
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    try {
        NSString* nsKey = [NSString stringWithCString:"RuntimItem" encoding:NSUTF8StringEncoding];
        NSDictionary* pDic = DefaultPropertyX();
        NSArray* pArrData = [pDic objectForKey:nsKey];
        if(pArrData == nil)
        {
            HLog("GetPropertyRunTimeItem not Found %s\n","RuntimItem");
            [pool release];
            return 0;
        }
        
        for (NSArray* arr in pArrData)
        {
            PROPERTY_RUNTIMEITEM *pNewPropery = new PROPERTY_RUNTIMEITEM;
            GetPropertyRuntimeItemX(arr,pNewPropery);
            list.push_back(pNewPropery);
        }
    }
    catch (...)
    {
        HLogE("[Error] Exception GetPropertyRunTimeItem %s\n","RuntimItem");
    }
    [pool release];
#endif
    return &list;
}


PROPERTY_RUNTIMEITEM* PROPERTY_RUNTIMEITEM::GetPropertyRunTimeItem(int nID, PROPERTY_RUNTIMEITEM& property)
{
#ifdef ANDROID

    const char* sKey = "RuntimItem";
    CHDoc* pDic = DefaultPropertyX();
    NodeArray* pArrData = (NodeArray*)pDic->GetData(sKey);
    if(pArrData == NULL)
    {
        HLog("GetPropertyRunTimeItem not Found %s\n","RuntimItem");
        return NULL;
    }
    
    int nCnt = (int)pArrData->mList.size();
    for(int i = 0; i < nCnt; i++)
    {
        NodeArray* arr = (NodeArray*)pArrData->mList[i];
        property.nID = ((NodeDataValue*)arr->mList[0])->GetInt();
        if(property.nID == nID)
        {
            GetPropertyRuntimeItemX((NodeArray*)arr,&property);
            return &property;
        }
    }
#else
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    
    try
    {
        NSString* nsKey = [NSString stringWithCString:"RuntimItem" encoding:NSUTF8StringEncoding];
        NSDictionary* pDic = DefaultPropertyX();
        NSArray* pArrData = [pDic objectForKey:nsKey];
        if(pArrData == nil)
        {
            HLog("GetPropertyRunTimeItemList not Found %s\n","RuntimItem");
            [pool release];
            return 0;
        }
        
        for (NSArray* arr in pArrData)
        {
            NSNumber* pID = [arr objectAtIndex:0];
            property.nID = pID.intValue;
            if(property.nID == nID)
            {
                GetPropertyRuntimeItemX((NSArray*)arr,&property);
                [pool release];
                return &property;
            }
        }
    }
    catch (...)
    {
        HLogE("[Error] Exception GetPropertyRunTimeItemList %s\n","RuntimItem");
    }
    [pool release];
#endif
    return 0;
}

#ifdef ANDROID
CHDoc       * gpGLTextUnit = NULL;
#else
NSDictionary* gpGLTextUnit = nil;
#endif

SGLTextUnit::SGLTextUnit()
{

}

SGLTextUnit::~SGLTextUnit()
{
    
}

void SGLTextUnit::Release()
{
#ifdef ANDROID
    if(gpGLTextUnit)
    {
        delete gpGLTextUnit;
        gpGLTextUnit= 0;
    }
#else
    if(gpGLTextUnit)
    {
        [gpGLTextUnit release];
        gpGLTextUnit= 0;
    }
#endif
}

void SGLTextUnit::Initialize()
{

#ifdef ANDROID
    extern string gsLocalDir; //다중언어지원함.
    
#ifndef NOTOPENGL
    string sPath = CLoader::getSearchPath();
    sPath.append(gsLocalDir);
    if(gnDisplayType == DISPLAY_IPHONE)
        sPath += "/GLText.plist";
    else
        sPath += "/GLText_ipad.plist";
#else
    string sPath = "/Volumes/DataHD/Project/88.MyDream/Android/resourcezip/GLText_ipad.plist";
#endif
    
    
    if(gpGLTextUnit)
    {
        delete gpGLTextUnit;
        gpGLTextUnit= 0;
    }
    
    gpGLTextUnit = new CHDoc();
    if(gpGLTextUnit->Load(sPath.c_str()) == false)
    {
        HLogE("SGLTextUnit::Initialize() %s\n",sPath.c_str());
        return;
    }
    
#else
    NSString *sName = @"GLText";
    if(gnDisplayType == DISPLAY_IPAD)
        sName = @"GLText_ipad";
    
    NSString* sPath = [[NSBundle mainBundle] pathForResource:sName ofType:@"plist"];
    
    if(sPath == nil)
    {
        HLogE("[ERROR] Not Found GLText.plist\n");
        return;
    }
    
    if(gpGLTextUnit)
    {
        [gpGLTextUnit release];
        gpGLTextUnit= 0;
    }
    
    gpGLTextUnit = [[NSDictionary alloc] initWithContentsOfFile:sPath];
#endif
}


const char* SGLTextUnit::GetOnlyText(const char* sID)
{
#ifdef ANDROID
    static string s;
    static map<string,string> LanTable;
    extern bool LanguageTable(std::map<string,string>* pTable);
    if(LanTable.size() == 0) LanguageTable(&LanTable);
    s = LanTable[sID];
    //Added By Song 2012.12.15 키값에 대한 데이터가 없으면 그대로 리턴한다.
    if(s.length() == 0)
    {
#ifdef DEBUG
        HLogE("Not Found GetOnlyText %s\n",sID);
#endif
        s = sID;
        return s.c_str();
    }
    return s.c_str();
#else
    static string sr;
    NSString * nsID = [NSString stringWithCString:sID encoding:NSUTF8StringEncoding];
    NSString * nsLocal = nil;
    nsLocal = NSLocalizedString(nsID, NULL);
    sr = nsLocal.UTF8String;
    return sr.c_str();
#endif
}


const char* SGLTextUnit::GetText(const char* sID)
{
#ifdef ANDROID
    static string s;
    static map<string,string> LanTable;
    extern bool LanguageTable(std::map<string,string>* pTable);
    NodeDataValue* pData = (NodeDataValue*)gpGLTextUnit->GetData(sID);
//    if(pData == NULL)
//    {
//        s = sID;
//        return s.c_str();
//    }
    if(LanTable.size() == 0) LanguageTable(&LanTable);
    s = LanTable[sID];
    //Added By Song 2012.12.15 키값에 대한 데이터가 없으면 그대로 리턴한다.
    if(s.length() == 0)
    {
        const char* sv = pData?pData->GetString():"";
//#ifdef DEBUG
//        HLogE("Not Found LanguageTable %s\n",sID);
//        if (*sv == 0)
//        {
//            HLogE("Not Found GLText or ipad.plist %s\n",sID);
//        }
//#endif
        s = sID;
        if(*sv != 0) //데이터가 존재하면.
        {
            s.append("#");
            s.append(sv);
        }
        return s.c_str();
    }
    
    if(pData)
    {
        s.append("#");
        s.append(pData->GetString());
    }
    return s.c_str();
#else
    static string sr;

#ifdef DEBUG
    if(sID == NULL)
    {
        HLogE("GetText is NULL \n");
        return NULL;
    }
#endif
    
    NSString * nsID = [NSString stringWithCString:sID encoding:NSUTF8StringEncoding];
    NSString * nsLocal = nil;
    nsLocal = NSLocalizedString(nsID, NULL);
    if(nsLocal == nil || gpGLTextUnit == nil) return sID; //존재 하지 않으면 그냥 리턴한다.
    NSString* nsData = [gpGLTextUnit objectForKey:nsID];
    NSString* nsResult = nil;
    if(nsData)
    {
        nsResult = [NSString stringWithFormat:@"%@#%@",nsLocal,nsData];
    }
    else
        nsResult = nsLocal;
    
    sr = nsResult.UTF8String;
    return sr.c_str();
#endif
}

const char* SGLTextUnit::GetTextAddGLData(const char* sText,const char* sID)
{
#ifdef ANDROID
    static string s;
    static map<string,string> LanTable;
    extern bool LanguageTable(std::map<string,string>* pTable);
    NodeDataValue* pData = (NodeDataValue*)gpGLTextUnit->GetData(sID);
    if(pData == NULL) return sText;
    if(LanTable.size() == 0) LanguageTable(&LanTable);
    s = LanTable[sText];
    //Added By Song 2012.12.15 키값에 대한 데이터가 없으면 그대로 리턴한다.
    if(s.length() == 0)
    {
        const char* sv = pData->GetString();
        s = sText;
        if(*sv != 0) //데이터가 존재하면.
        {
            s.append("#");
            s.append(sv);
        }
        return s.c_str();
    }
    s.append("#");
    s.append(pData->GetString());
    return s.c_str();
#else
    static string sr;
    NSString * nsID = [NSString stringWithCString:sID encoding:NSUTF8StringEncoding];
    NSString * nsText = [NSString stringWithCString:sText encoding:NSUTF8StringEncoding];
    NSString * nsLocal = nil;
    nsLocal = NSLocalizedString(nsText, NULL);
    if(nsLocal == nil || gpGLTextUnit == nil) return sText; //존재 하지 않으면 그냥 리턴한다.
    NSString* nsData = [gpGLTextUnit objectForKey:nsID];
    NSString* nsResult = nil;
    if(nsData)
        nsResult = [NSString stringWithFormat:@"%@#%@",nsLocal,nsData ];
    else
        nsResult = nsLocal;
    sr = nsResult.UTF8String;
    return sr.c_str();
#endif

}









RUNTIME_UPGRADES::RUNTIME_UPGRADES()
{
    nID = 0;
    nCount = 0;
}
RUNTIME_UPGRADES::~RUNTIME_UPGRADES()
{
}

RUNTIME_UPGRADES& RUNTIME_UPGRADES::operator=(const RUNTIME_UPGRADES& SrcProperty)
{
    nID = SrcProperty.nID;
    nCount = SrcProperty.nCount;
    for (int i = 0; nCount; i++)
    {
        nUpgradeIDs.push_back(SrcProperty.nUpgradeIDs.at(i));
    }
    return *this;
}

#ifdef ANDROID
void GetPropertyRuntimeUpgradesX(NodeArray* arr,RUNTIME_UPGRADES* pProperty)
{
    pProperty->nID = GetIntProp(arr,0);
    pProperty->nCount = GetIntProp(arr,1);
    for (int i = 0; i < pProperty->nCount; i++)
    {
        pProperty->nUpgradeIDs.push_back(GetIntProp(arr,2+i));
    }
}
#else
void GetPropertyRuntimeUpgradesX(NSArray* arr,RUNTIME_UPGRADES* pProperty)
{
    pProperty->nID = GetIntProp(arr,0);
    pProperty->nCount = GetIntProp(arr,1);
    for (int i = 0; i < pProperty->nCount; i++)
    {
        pProperty->nUpgradeIDs.push_back(GetIntProp(arr,2+i));
    }
}
#endif


RUNTIME_UPGRADES* RUNTIME_UPGRADES::GetPropertyRunUpgradesItem(int nID, RUNTIME_UPGRADES& property)
{
#ifdef ANDROID
    
    const char* sKey = "RuntimUpgrades";
    CHDoc* pDic = DefaultPropertyX();
    NodeArray* pArrData = (NodeArray*)pDic->GetData(sKey);
    if(pArrData == NULL)
    {
        HLog("GetPropertyRunUpgradesItem not Found %s\n","RuntimUpgrades");
        return NULL;
    }
    
    int nCnt = (int)pArrData->mList.size();
    for(int i = 0; i < nCnt; i++)
    {
        NodeArray* arr = (NodeArray*)pArrData->mList[i];
        property.nID = ((NodeDataValue*)arr->mList[0])->GetInt();
        if(property.nID == nID)
        {
            GetPropertyRuntimeUpgradesX((NodeArray*)arr,&property);
            return &property;
        }
    }
#else
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    
    try
    {
        NSString* nsKey = [NSString stringWithCString:"RuntimUpgrades" encoding:NSUTF8StringEncoding];
        NSDictionary* pDic = DefaultPropertyX();
        NSArray* pArrData = [pDic objectForKey:nsKey];
        if(pArrData == nil)
        {
            HLog("GetPropertyRunUpgradesItem not Found %s id = %d\n","RuntimUpgrades",nID);
            [pool release];
            return 0;
        }
        
        for (NSArray* arr in pArrData)
        {
            NSNumber* pID = [arr objectAtIndex:0];
            property.nID = pID.intValue;
            if(property.nID == nID)
            {
                GetPropertyRuntimeUpgradesX((NSArray*)arr,&property);
                [pool release];
                return &property;
            }
        }
    }
    catch (...)
    {
        HLogE("[Error] Exception GetPropertyRunUpgradesItem %s\n","RuntimItem");
    }
    [pool release];
#endif
    return 0;
}


RUNTIME_UPGRADE::RUNTIME_UPGRADE()
{
    nID = 0; //1:업그레이드 3:범위업그레이드 2:수선 3:파계
    nTime = 0; //업그레이드에 걸리는 시간
    nUpgradeMaxCnt = 0;
    sName = 0;
    sDesc = 0;
    sPath = 0; //icon
    nGold = 0;
    nUpgradeCnt = 0;
    nUpgreadTime = -1;
}

RUNTIME_UPGRADE::~RUNTIME_UPGRADE()
{
    if(sName)
    {
        delete[] sName;
        sName = 0;
    }
    
    if(sDesc)
    {
        delete[] sDesc;
        sDesc = NULL;
    }
    
    if(sPath)
    {
        delete[] sPath;
        sPath = 0;
    }
}

RUNTIME_UPGRADE& RUNTIME_UPGRADE::operator=(const RUNTIME_UPGRADE& SrcProperty)
{
    nID = SrcProperty.nID;
    nTime = SrcProperty.nTime;
    nUpgradeMaxCnt= SrcProperty.nUpgradeMaxCnt;
    if(SrcProperty.sName)
        SetName(SrcProperty.sName);
    if(SrcProperty.sDesc)
        SetDesc(SrcProperty.sDesc);
    if(SrcProperty.sPath)
        SetPath(SrcProperty.sPath);
    nGold= SrcProperty.nGold;
    
//    nUpgradeCnt= SrcProperty.nUpgradeCnt;
//    nUpgreadTime = SrcProperty.nUpgreadTime;
//    nUpgreadStartTime= SrcProperty.nUpgreadStartTime;
    return *this;
}


void RUNTIME_UPGRADE::SetName(const char* v)
{
    if(sName)
    {
        delete[] sName;
        sName = 0;
    }
    if(v)
    {
        sName = new char[strlen(v) + 1];
        strcpy(sName, v);
    }
}

void RUNTIME_UPGRADE::SetDesc(const char* v)
{
    if(sDesc)
    {
        delete[] sDesc;
        sDesc = 0;
    }
    if(v)
    {
        sDesc = new char[strlen(v) + 1];
        strcpy(sDesc, v);
    }
}

void RUNTIME_UPGRADE::SetPath(const char* v)
{
    if(sPath)
    {
        delete[] sPath;
        sPath = 0;
    }
    if(v)
    {
        sPath = new char[strlen(v) + 1];
        strcpy(sPath, v);
    }
}



#ifdef ANDROID
void GetPropertyRuntimeUpgradeX(NodeArray* arr,RUNTIME_UPGRADE* pProperty)
{
    pProperty->nID = GetIntProp(arr,0);
    pProperty->nTime = GetIntProp(arr,1);
    pProperty->nUpgradeMaxCnt = GetIntProp(arr,2);
    pProperty->SetName(GetStrProp(arr,3));
    pProperty->SetDesc(GetStrProp(arr,4));
    pProperty->SetPath(GetStrProp(arr,5));
    pProperty->nGold = GetIntProp(arr,6);
}
#else
void GetPropertyRuntimeUpgradeX(NSArray* arr,RUNTIME_UPGRADE* pProperty)
{
    pProperty->nID = GetIntProp(arr,0);
    pProperty->nTime = GetIntProp(arr,1);
    pProperty->nUpgradeMaxCnt = GetIntProp(arr,2);
    pProperty->SetName(GetStrProp(arr,3));
    pProperty->SetDesc(GetStrProp(arr,4));
    pProperty->SetPath(GetStrProp(arr,5));
    pProperty->nGold = GetIntProp(arr,6);
}
#endif



RUNTIME_UPGRADE* RUNTIME_UPGRADE::GetPropertyRunUpgradeItem(int nID, RUNTIME_UPGRADE& property)
{
#ifdef ANDROID
    
    const char* sKey = "RuntimUpgrade";
    CHDoc* pDic = DefaultPropertyX();
    NodeArray* pArrData = (NodeArray*)pDic->GetData(sKey);
    if(pArrData == NULL)
    {
        HLog("GetPropertyRunUpgradeItem not Found %s\n","RuntimUpgrade");
        return NULL;
    }
    
    int nCnt = (int)pArrData->mList.size();
    for(int i = 0; i < nCnt; i++)
    {
        NodeArray* arr = (NodeArray*)pArrData->mList[i];
        property.nID = ((NodeDataValue*)arr->mList[0])->GetInt();
        if(property.nID == nID)
        {
            GetPropertyRuntimeUpgradeX((NodeArray*)arr,&property);
            return &property;
        }
    }
#else
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    
    try
    {
        NSString* nsKey = [NSString stringWithCString:"RuntimUpgrade" encoding:NSUTF8StringEncoding];
        NSDictionary* pDic = DefaultPropertyX();
        NSArray* pArrData = [pDic objectForKey:nsKey];
        if(pArrData == nil)
        {
            HLog("GetPropertyRunUpgradeItem not Found %s\n","RuntimUpgrade");
            [pool release];
            return 0;
        }
        
        for (NSArray* arr in pArrData)
        {
            NSNumber* pID = [arr objectAtIndex:0];
            property.nID = pID.intValue;
            if(property.nID == nID)
            {
                GetPropertyRuntimeUpgradeX((NSArray*)arr,&property);
                [pool release];
                return &property;
            }
        }
    }
    catch (...)
    {
        HLogE("[Error] Exception GetPropertyRunUpgradeItem %s\n","RuntimUpgrade");
    }
    [pool release];
#endif
    return 0;
}


#ifdef ANDROID
//안드로이드에서 런타임 슬롯텍스쳐가 안보여지는 경우가 있다.
void RUNTIME_UPGRADE::InitTextureRunUpgradeItemList(vector<string> *sOut)
{
    RUNTIME_UPGRADE NewPropery;
    const char* sKey = "RuntimUpgrade";
    CHDoc* pDic = DefaultPropertyX();
    NodeArray* pArrData = (NodeArray*)pDic->GetData(sKey);
    if(pArrData == NULL)
    {
        HLog("GetPropertyRunUpgradeItemList not Found %s\n","RuntimUpgrade");
        return ;
    }

    //for (NSArray* arr in pArrData)
    int nCnt = (int)pArrData->mList.size();
    for(int i = 0; i < nCnt; i++)
    {
        GetPropertyRuntimeUpgradeX((NodeArray*)pArrData->mList[i],&NewPropery);
        sOut->push_back(NewPropery.sPath);
    }
}
#endif //ANDROID

//void RUNTIME_UPGRADE::ClosePropertyRunUpgradeItemList(map<int,RUNTIME_UPGRADE*>& list)
//{
//    map<int, RUNTIME_UPGRADE*>::iterator b = list.begin();
//    map<int, RUNTIME_UPGRADE*>::iterator e = list.end();
//    
//    for (map<int, RUNTIME_UPGRADE*>::iterator it = b; it != e; it++)
//    {
//        if(it->second)
//            delete it->second;
//    }
//    list.clear();
//}

//map<int,RUNTIME_UPGRADE*>* RUNTIME_UPGRADE::GetPropertyRunUpgradeItemList(map<int,RUNTIME_UPGRADE*>& list)
//{
//#ifdef ANDROID
//    const char* sKey = "RuntimUpgrade";
//    CHDoc* pDic = DefaultPropertyX();
//    NodeArray* pArrData = (NodeArray*)pDic->GetData(sKey);
//    if(pArrData == NULL)
//    {
//        HLog("GetPropertyRunUpgradeItemList not Found %s\n","RuntimUpgrade");
//        return 0;
//    }
//    
//    //for (NSArray* arr in pArrData)
//    int nCnt = (int)pArrData->mList.size();
//    for(int i = 0; i < nCnt; i++)
//    {
//        RUNTIME_UPGRADE *pNewPropery = new RUNTIME_UPGRADE;
//        GetPropertyRuntimeUpgradeX((NodeArray*)pArrData->mList[i],pNewPropery);
//        list.push_back(pNewPropery);
//    }
//    
//#else
//    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
//    try {
//        NSString* nsKey = [NSString stringWithCString:"RuntimUpgrade" encoding:NSUTF8StringEncoding];
//        NSDictionary* pDic = DefaultPropertyX();
//        NSArray* pArrData = [pDic objectForKey:nsKey];
//        if(pArrData == nil)
//        {
//            HLog("GetPropertyRunUpgradeItemList not Found %s\n","RuntimUpgrade");
//            [pool release];
//            return 0;
//        }
//        
//        for (NSArray* arr in pArrData)
//        {
//            RUNTIME_UPGRADE *pNewPropery = new RUNTIME_UPGRADE;
//            GetPropertyRuntimeUpgradeX(arr,pNewPropery);
//            list[pNewPropery->nID] = pNewPropery;
//        }
//    }
//    catch (...)
//    {
//        HLogE("[Error] Exception GetPropertyRunTimeItem %s\n","RuntimUpgrade");
//    }
//    [pool release];
//#endif
//    return &list;
//}
