//
//  CUserInfo.cpp
//  SongGL
//
//  Created by 송 호학 on 11. 12. 19..
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include <algorithm>
#include "CProperty.h"
#ifdef ANDROID
#include "sGLDefine.h"
#include "HDoc.h"
#include "CProperty.h"


#ifndef NOTOPENGL
#include "CLoader.h"
#endif //NOTOPENGL

#else
#import <Foundation/Foundation.h>
#include "sGLText.h"
#endif

#include "CUserInfo.h"
#include "ResID.h"
#include <time.h>

//#ifdef ANDROID
string gDevideID = "H!@#$*&12"; //안드로이드의 UserID정보를 크랙하지 못하게 저장한다.
extern "C" int XorCrypto(unsigned char* key, int keyLength, unsigned char* in, int nBytes);

#ifdef NOTOPENGL
int XorCrypto(unsigned char* key, int keyLength, unsigned char* in, int nBytes)
{
    int i = 0;
    
    if( nBytes < keyLength ) { return -1; }
    
    while( i < nBytes )
    {
        in[i] ^= key[i%keyLength];
        i++;
    }
    
    return 0;
}
#endif

typedef struct  tagBLOB
{
	unsigned long cbSize;
	unsigned char *pBlobData;
}	BLOB;

char chBase64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// Encode and return the bytes in base 64
char* ToBase64(const void* pv, int cb, bool bCRLF)
{
	int cbPerLine = (72 / 4) * 3;
	int nLine = (cb + 3) / cbPerLine + 2;
	int cbNeed = nLine * (72 + 2 /*CRLF*/) + 1 /*NULL*/;
    
	unsigned char* pb = (unsigned char*)pv;
	char* p = new char[cbNeed];
	if (!p) return p;
	
	char* pch = p;
	int cbLine = 0;
    
	while (cb >= 3)
	{
		unsigned char b0 = (char)((pb[0]>>2) & 0x3F);
		unsigned char b1 = (char)(((pb[0]&0x03)<<4) | ((pb[1]>>4) & 0x0F));
		unsigned char b2 = (char)(((pb[1]&0x0F)<<2) | ((pb[2]>>6) & 0x03));
		unsigned char b3 = (char)((pb[2]&0x3F));
		*pch++ = chBase64[b0];
		*pch++ = chBase64[b1];
		*pch++ = chBase64[b2];
		*pch++ = chBase64[b3];
		pb += 3;
		cb -= 3;
        
		if (bCRLF)
		{
			cbLine += 4;
			if (cbLine >= 72)
			{
				*pch++ = '\r';
				*pch++ = '\n';
				cbLine = 0;
			}
		}
	}
    
	if (cb == 1)
	{
		unsigned char b0 = (unsigned char)(char)((pb[0]>>2) & 0x3F);
		unsigned char b1 = (unsigned char)(char)(((pb[0]&0x03)<<4) | 0);
		*pch++ = chBase64[b0];
		*pch++ = chBase64[b1];
		*pch++ = '=';
		*pch++ = '=';
	}
	else if (cb == 2)
	{
		unsigned char b0 = (unsigned char)(char)((pb[0]>>2) & 0x3F);
		unsigned char b1 = (unsigned char)(char)(((pb[0]&0x03)<<4) | ((pb[1]>>4) & 0x0F));
		unsigned char b2 = (unsigned char)(char)(((pb[1]&0x0F)<<2) | 0);
		*pch++ = chBase64[b0];
		*pch++ = chBase64[b1];
		*pch++ = chBase64[b2];
		*pch++ = '=';
	}
	if (bCRLF)
	{
		*pch++ = '\r';
		*pch++ = '\n';
	}
	*pch = NULL;
	return p;
}

// Decode and return the Base64 encoded bytes
BLOB FromBase64(const char* pv)
{
	BLOB blob;
	blob.cbSize = 0;
	blob.pBlobData = NULL;
    
	unsigned char* p = new unsigned char[strlen((char*)pv)];
	if (!p) return blob;
    
	unsigned char mpwchb[256];
	unsigned char bBad = (unsigned char)-1;
	memset(&mpwchb[0], bBad, 256); // Initialize our decoding array
	for (int i=0; i<64; i++)
	{
		unsigned char wch = chBase64[i];
		mpwchb[wch] = (unsigned char)i;
	}
	// Loop over the entire input buffer
	unsigned long bCurrent = 0;		// what we're in the process of filling up
	int  cbitFilled = 0;	// how many bits in it we've filled
	unsigned char* pb = p;			// current destination (not filled)
	for ( ; *pv; pv++)
	{
		if (*pv==0x0A || *pv==0x0D || *pv==0x20 || *pv==0x09) continue;	// Ignore white space
		if (*pv=='=') break;		// Have we reached the end?
		unsigned char bDigit = mpwchb[*pv];	// How much is this character worth?
		if (bDigit==bBad) return blob;
		// Add in its contribution
		bCurrent <<= 6;
		bCurrent |= bDigit;
		cbitFilled += 6;
		// If we've got enough, output a byte
		if (cbitFilled >= 8)
		{
			unsigned long b = (bCurrent >> (cbitFilled-8));		// get's top eight valid bits
			*pb++ = (unsigned char)(b&0xFF);						// store the byte away
			cbitFilled -= 8;
		}
	}
	blob.pBlobData = p;
	blob.cbSize = pb-p;
	blob.pBlobData[blob.cbSize] = 0;
	return blob;
}
//#endif //ANDROID

CUserInfo::CUserInfo()
{
    mLastSelectedMapIndex = 0;
    mLastSelectedTankIndex = 0;
    
    mLastSelectedMapID = 0;
    mLastSelectedTankID = ACTORID_K9;
    
    mfGlold = 0.f;
    mnRank = 0;
    
    mbOnSoundFX = 1;
    mbOnMusic = 1;
    mbOnVibration = 1;
    mnTotalScore = 0;
    mnLastSelectedItemPageIndex = 1;
    mnLastSelectedBombPageIndex = 1;
    mnLastSelectedBlzPageIndex = 1;
    
    mnLastBombItemSwitch = 1; //1:Bomb, 0: Item
    
    mbPromotionSloder = false;
    msUserPwd = "";
    msUserID = "****";
    
    mnScoreGrade = 0;
    mnBeforeScoreGrade = 0;
    memset(&mBeforeScoreGradeTime, 0, sizeof(timeval));
    
    mnCompletedRank = 1;
    
    mnBuyCnt = 0;
    mnClickPromotionCnt = 0;
    
    //-----------------------------------------------------------------
    // 사용자의 사용스타일을 올려보자.
    //-----------------------------------------------------------------
    mnBGetherScore = 0;
    mnBGetherTime = 0;
    mnGetherKillCount = 0;
    mnGetherBuildBlzCount = 0;
    mnGetherNowMapID = 0;
    mbSucceedMap = false;
    mnRunTimeUpgrade = 0;
    mnALevel = 0;
    mnDLevel = 0;
    //-----------------------------------------------------------------
    
    mnTotalPlayTime = 0;
    mbMultiplayOn = true;
    mnGameCenterStatus = 0;
}


CUserInfo::~CUserInfo()
{
    Save();
    ReleaseUserInfoRelease(mListBomb);
    ReleaseUserInfoRelease(mListItem);
    ReleaseUserInfoRelease(mListBlz);
    ReleaseUserProperty();
}

CUserInfo* CUserInfo::GetDefaultUserInfo()
{
    static CUserInfo usinfo;
    static bool bLoaded = false;
    if(bLoaded == false)
    {
        usinfo.Load();
        bLoaded = true;
    }
    return &usinfo;
}

void CUserInfo::Load()
{
#ifdef ANDROID
    //---------------------------------------------------------------------------------------------------------------
    //Base64디코딩후 데이터를 푼다.
    int nLen = GetUserInfo("SongsLen", 0);
    if(nLen == 0) return;
    char sCrc[512];
    memset(sCrc, 0, sizeof(sCrc));
    CHDoc* pDic = (CHDoc*)DefaultUserProperty();
    NodeDataValue* pObj = (NodeDataValue*)pDic->GetData("Songs");
    if(pObj == NULL) return;
    strcpy(sCrc,pObj->GetString());
    BLOB Base64 = FromBase64((const char*)sCrc);
    if(Base64.pBlobData == NULL) return;
    
    memset(sCrc, 0, sizeof(sCrc));
    memcpy(sCrc, (char*)Base64.pBlobData,nLen);
    
    XorCrypto((unsigned char*)gDevideID.c_str(), (int)strlen(gDevideID.c_str()), (unsigned char*)sCrc, (int)nLen);
    if(Base64.pBlobData) delete[] Base64.pBlobData;;
    mnVersion = GetUserInfo("Version",USERVER_VERSION);
    mfGlold = GetUserInfoF("Gold");
    mnRank = GetUserInfo("Level");
    mnTotalScore = GetUserInfo("TotalScore");
    mLastSelectedMapIndex = GetUserInfo("LastSelectedMapIndex");
    mLastSelectedTankIndex = GetUserInfo("LastSelectedTankIndex");
    mLastSelectedMapID = GetUserInfo("LastSelectedMapID");
    mLastSelectedTankID = GetUserInfo("LastSelectedTankID");
    mnLastSelectedItemPageIndex = GetUserInfo("LastSelectedItemPage");
    mnLastSelectedBombPageIndex = GetUserInfo("LastSelectedBombPage");
    mnLastSelectedBlzPageIndex = GetUserInfo("LastSelectedBlzPage");
    mnLastBombItemSwitch = GetUserInfo("LastBombItemSwitch");
    if(mLastSelectedTankID == 0) mLastSelectedTankID = ACTORID_K9; //1은 theK 탱크이다.
    GetUserInfoList("ListBomb",mListBomb);
    GetUserInfoList("ListItem",mListItem);
    GetUserInfoList("CompletedMap",mCompletedMap);
    mAttackUpgrade = GetUserInfo("UpgradeAttack2");
    mDefendUpgrade = GetUserInfo("UpgradeDepend2");

    
    mbOnSoundFX = GetUserInfo("SoundFX",1);
    mbOnMusic = GetUserInfo("Music",1);
    mbOnVibration = GetUserInfo("Vibration",1);
    
    mnBeforeScoreGrade = GetUserInfo("BeforeScoreGrade",0);
    time_t sec = GetUserInfo("BeforeScoreGradeTime",0);
    mBeforeScoreGradeTime.tv_sec = sec;
    mBeforeScoreGradeTime.tv_usec = 0;
    
    mnCompletedRank = GetUserInfo("CompletedRank",1);
    msUserID = GetUserInfoS("UserID");
    msYourMessage =GetUserInfoS("YourMessage");
    mnReport = GetUserInfo("Report",0);
    
    
    mnBuyCnt = GetUserInfo("BuyCnt",0);
    mnClickPromotionCnt = GetUserInfo("ClickPromotionCnt",0);
    mnTotalPlayTime = GetUserInfo("TotalPlayTime",0);
    mnScoreGrade = GetUserInfo("ScoreGrade",0);
    mbMultiplayOn = GetUserInfo("MultiplayOn",1);
    
    
    
    //---------------------------------------------------------------------------------------------------------------
    //CRC정보를 만든다.
    string sCrc2;
    char sNum[20];
    sprintf(sNum, "%d",(int)mfGlold);
    sCrc2.append(sNum);
    sprintf(sNum, "%d",mnRank);
    sCrc2.append(sNum);
    sprintf(sNum, "%d",mnTotalScore);
    sCrc2.append(sNum);
    int nSize = (int)mListBomb.size();
    for(int i = 0; i < nSize; i++)
    {
        sprintf(sNum, "%d",mListBomb[i]->nCnt);
        sCrc2.append(sNum);
    }
    nSize = (int)mListItem.size();
    for(int i = 0; i < nSize; i++)
    {
        sprintf(sNum, "%d",mListItem[i]->nCnt);
        sCrc2.append(sNum);
    }
    //---------------------------------------------------------------------------------------------------------------
    if(strcmp(sCrc, sCrc2.c_str()) != 0)
    {
        if(mnVersion >= 3) //2:일때는 fGold를 %f로 했다. //3:부터는 fGold를 %d로 해준다. (이유는 0.01이 0.00999가 될 수 있기 때문이다.)
        {
            ReleaseUserInfoRelease(mListBomb);
            ReleaseUserInfoRelease(mListItem);
            ReleaseUserInfoRelease(mListBlz);
            
            mfGlold = 0.f;
            mnRank = 0;
            mnTotalScore = 0;
        }
    }
    
    if(mnVersion != USERVER_VERSION)
        mnVersion = USERVER_VERSION;
#else
    
    //---------------------------------------------------------------------------------------------------------------
    //Base64디코딩후 데이터를 푼다.
    int nLen = GetUserInfo("SongsLen", 0);
    if(nLen == 0) return;
    char sCrc[512];
    memset(sCrc, 0, sizeof(sCrc));
    strcpy(sCrc, GetUserInfoS("Songs"));
    BLOB Base64 = FromBase64((const char*)sCrc);
    if(Base64.pBlobData == NULL) return;
    memset(sCrc, 0, sizeof(sCrc));
    memcpy(sCrc, (char*)Base64.pBlobData,nLen);
    XorCrypto((unsigned char*)gDevideID.c_str(), (int)strlen(gDevideID.c_str()), (unsigned char*)sCrc, (int)nLen);
    if(Base64.pBlobData) delete[] Base64.pBlobData;
    
    mnVersion = GetUserInfo("Version",USERVER_VERSION);
    mfGlold = GetUserInfoF("Gold");

    mnRank = GetUserInfo("Level");
    mnTotalScore = GetUserInfo("TotalScore");
    
    //if(mnRank == 0)    mnRank = 1;
    
    mLastSelectedMapIndex = GetUserInfo("LastSelectedMapIndex");
    mLastSelectedTankIndex = GetUserInfo("LastSelectedTankIndex");
    mLastSelectedMapID = GetUserInfo("LastSelectedMapID");
    mLastSelectedTankID = GetUserInfo("LastSelectedTankID");
    mnLastSelectedItemPageIndex = GetUserInfo("LastSelectedItemPage");
    mnLastSelectedBombPageIndex = GetUserInfo("LastSelectedBombPage");
    mnLastSelectedBlzPageIndex = GetUserInfo("LastSelectedBlzPage");
    mnLastBombItemSwitch = GetUserInfo("LastBombItemSwitch");
    
    if(mLastSelectedTankID == 0) mLastSelectedTankID = ACTORID_K9; //1은 theK 탱크이다.
    
    GetUserInfoList("ListBomb",mListBomb);
    GetUserInfoList("ListItem",mListItem);
    GetUserInfoList("CompletedMap",mCompletedMap);
    
    mAttackUpgrade = GetUserInfo("UpgradeAttack2");
    mDefendUpgrade = GetUserInfo("UpgradeDepend2");
    
    
    mbOnSoundFX = GetUserInfo("SoundFX",1);
    mbOnMusic = GetUserInfo("Music",1);
    mbOnVibration = GetUserInfo("Vibration",1);
    
    mnBeforeScoreGrade = GetUserInfo("BeforeScoreGrade",0);
    time_t sec = GetUserInfo("BeforeScoreGradeTime",0);
    mBeforeScoreGradeTime.tv_sec = sec;
    mBeforeScoreGradeTime.tv_usec = 0;
    
    
    mnCompletedRank = GetUserInfo("CompletedRank",1);
    msUserID = GetUserInfoS("UserID");
    msYourMessage =GetUserInfoS("YourMessage");
    
    mnReport = GetUserInfo("Report",0);
    
    mnBuyCnt = GetUserInfo("BuyCnt",0);
    mnClickPromotionCnt = GetUserInfo("ClickPromotionCnt",0);

    mnTotalPlayTime = GetUserInfo("TotalPlayTime",0);
    mnScoreGrade = GetUserInfo("ScoreGrade",0);
    
    mbMultiplayOn = GetUserInfo("MultiplayOn",1);
    
    
    
    
    //---------------------------------------------------------------------------------------------------------------
    //CRC정보를 만든다.
    string sCrc2;
    char sNum[20];
    sprintf(sNum, "%d",(int)mfGlold);
    sCrc2.append(sNum);
    sprintf(sNum, "%d",mnRank);
    sCrc2.append(sNum);
    sprintf(sNum, "%d",mnTotalScore);
    sCrc2.append(sNum);
    int nSize = (int)mListBomb.size();
    for(int i = 0; i < nSize; i++)
    {
        sprintf(sNum, "%d",mListBomb[i]->nCnt);
        sCrc2.append(sNum);
    }
    nSize = (int)mListItem.size();
    for(int i = 0; i < nSize; i++)
    {
        sprintf(sNum, "%d",mListItem[i]->nCnt);
        sCrc2.append(sNum);
    }
    //---------------------------------------------------------------------------------------------------------------
#ifdef DEBUG
    if(strcmp(sCrc, sCrc2.c_str()) != 0)
    {
        HLogE("Changed CUserInfo File\n");
    }
#else
    if(strcmp(sCrc, sCrc2.c_str()) != 0)
    {
        if(mnVersion >= 3) //2:일때는 fGold를 %f로 했다. //3:부터는 fGold를 %d로 해준다. (이유는 0.01이 0.00999가 될 수 있기 때문이다.)
        {
            ReleaseUserInfoRelease(mListBomb);
            ReleaseUserInfoRelease(mListItem);
            ReleaseUserInfoRelease(mListBlz);
            mfGlold = 0;
            mnRank = 0;
            mnTotalScore = 0;
        }
        else //이전 버전부터 게임을 하고 있었다면
        {
            //그냥 무시해 버리자.....
        }
    }
#endif //DEBUG
    
    
    if(mnVersion != USERVER_VERSION)
        mnVersion = USERVER_VERSION;
    
#endif
}

int CUserInfo::AddAttackUpgrade()
{
    mAttackUpgrade += 1;
    return mAttackUpgrade;
}

int CUserInfo::AddDependUpgrade()
{
    mDefendUpgrade += 1;
    return mDefendUpgrade;
}

int CUserInfo::GetAttackUpgrade()
{
    return mAttackUpgrade;
}
int CUserInfo::GetDependUpgrade()
{
    return mDefendUpgrade;
}


//각 탱그별로 기본 폭탄 종류가 틀리다.
void CUserInfo::SetDefaultBomb(int nBombID)
{
    vector<USERINFO*>::iterator i;
    for (i = mListBomb.begin(); i != mListBomb.end(); i++) 
    {
        if((*i)->nType == 1) //영구인 폭탄은 제거한다.
        {
            delete *i;
            mListBomb.erase(i);
            break;
        }
    }
    
    PROPERTY_BOMB prop;
    if(PROPERTY_BOMB::GetPropertyBomb(nBombID, prop))
    {
        USERINFO* pInfo = new USERINFO;
        pInfo->nID = prop.nID;
        pInfo->nCnt = -1;
        pInfo->nType = 1; //0:일반 1:영구. (항상 기본 폭탄은 영구로 해준다.)
        mListBomb.insert( mListBomb.begin(), pInfo);
    }
    else
    {
        HLogE("[Error] Not Found %d nBombID",nBombID);
    }
    
}

void CUserInfo::Save()
{
#ifdef ANDROID
    char sNum[20];
    string sCrc;
    
    SetUserInfo("Version",mnVersion);
    SetUserInfoF("Gold", mfGlold);
    SetUserInfo("Level", mnRank);
    SetUserInfo("TotalScore",mnTotalScore);
    
    SetUserInfo("LastSelectedMapIndex",mLastSelectedMapIndex);
    SetUserInfo("LastSelectedTankIndex",mLastSelectedTankIndex);
    
    SetUserInfo("LastSelectedMapID",mLastSelectedMapID);
    SetUserInfo("LastSelectedTankID",mLastSelectedTankID);
    SetUserInfo("LastSelectedItemPage",mnLastSelectedItemPageIndex);
    SetUserInfo("LastSelectedBombPage",mnLastSelectedBombPageIndex);
    SetUserInfo("LastSelectedBlzPage",mnLastSelectedBlzPageIndex);
    SetUserInfo("LastBombItemSwitch",mnLastBombItemSwitch);
    
    
    SetUserInfoList("ListBomb",mListBomb);
    SetUserInfoList("ListItem",mListItem);
    SetUserInfoList("CompletedMap",mCompletedMap);
    
//    SetUserUpgradeMap("UpgradeAttack",mAttackUpgrade);
//    SetUserUpgradeMap("UpgradeDepend",mDefendUpgrade);
    SetUserInfo("UpgradeAttack2",mAttackUpgrade);
    SetUserInfo("UpgradeDepend2",mDefendUpgrade);

    
    SetUserInfo("SoundFX",mbOnSoundFX);
    SetUserInfo("Music",mbOnMusic);
    SetUserInfo("Vibration", mbOnVibration);
    
    //가장처음 실행했을때 기본적인 골드를 넣어준다.
//    SetUserInfo("FirstRun",1);
    SetUserInfo("BeforeScoreGrade",mnBeforeScoreGrade);
    SetUserInfo("BeforeScoreGradeTime",mBeforeScoreGradeTime.tv_sec);
    
    SetUserInfo("CompletedRank",mnCompletedRank);
    SetUserInfo("UserID",msUserID.c_str());
    SetUserInfo("YourMessage",msYourMessage.c_str());
    
    SetUserInfo("Report",mnReport);
    
    SetUserInfo("BuyCnt",mnBuyCnt);
    SetUserInfo("ClickPromotionCnt",mnClickPromotionCnt);
    SetUserInfo("TotalPlayTime",mnTotalPlayTime);
    SetUserInfo("ScoreGrade",mnScoreGrade);
    SetUserInfo("MultiplayOn",mbMultiplayOn);
    
    //---------------------------------------------------------------------------------------------------------------
    //CRC정보를 만든다.
    sprintf(sNum, "%d",(int)mfGlold);
    sCrc.append(sNum);
    sprintf(sNum, "%d",mnRank);
    sCrc.append(sNum);
    sprintf(sNum, "%d",mnTotalScore);
    sCrc.append(sNum);
    int nSize = (int)mListBomb.size();
    for(int i = 0; i < nSize; i++)
    {
        sprintf(sNum, "%d",mListBomb[i]->nCnt);
        sCrc.append(sNum);
    }
    nSize = (int)mListItem.size();
    for(int i = 0; i < nSize; i++)
    {
        sprintf(sNum, "%d",mListItem[i]->nCnt);
        sCrc.append(sNum);
    }
    //---------------------------------------------------------------------------------------------------------------
    char strValue[512];
    memset(strValue, 0, sizeof(strValue));
    //strcpy(strValue,"Hellow My Name is hohak ,,,,, U Don't Hack..humhum");
    strcpy(strValue,sCrc.c_str());
    int nLen = (int)strlen(strValue);
    XorCrypto((unsigned char*)gDevideID.c_str(), (int)strlen(gDevideID.c_str()), (unsigned char*)strValue, (int)nLen);
    char* sBase64 = ToBase64((const void*)strValue,(int)nLen,false);
    CHDoc* pDic = (CHDoc*)DefaultUserProperty();
    NodeDataValue* pValue = new NodeDataValue(sBase64,NodeDataSring);
    pDic->SetData("Songs",pValue);
    delete[] sBase64;
    SetUserInfo("SongsLen", nLen);
    //---------------------------------------------------------------------------------------------------------------
    SaveUserProperty();
#else
    
    SetUserInfo("Version",mnVersion);
    SetUserInfoF("Gold", mfGlold);
    SetUserInfo("Level", mnRank);
    SetUserInfo("TotalScore",mnTotalScore);
    
    SetUserInfo("LastSelectedMapIndex",mLastSelectedMapIndex);
    SetUserInfo("LastSelectedTankIndex",mLastSelectedTankIndex);
    
    SetUserInfo("LastSelectedMapID",mLastSelectedMapID);
    SetUserInfo("LastSelectedTankID",mLastSelectedTankID);
    SetUserInfo("LastSelectedItemPage",mnLastSelectedItemPageIndex);
    SetUserInfo("LastSelectedBombPage",mnLastSelectedBombPageIndex);
    SetUserInfo("LastSelectedBlzPage",mnLastSelectedBlzPageIndex);
    SetUserInfo("LastBombItemSwitch",mnLastBombItemSwitch);
    
    SetUserInfoList("ListBomb",mListBomb);
    SetUserInfoList("ListItem",mListItem);
    SetUserInfoList("CompletedMap",mCompletedMap);
    
//    SetUserUpgradeMap("UpgradeAttack",mAttackUpgrade);
//    SetUserUpgradeMap("UpgradeDepend",mDefendUpgrade);
    SetUserInfo("UpgradeAttack2",mAttackUpgrade);
    SetUserInfo("UpgradeDepend2",mDefendUpgrade);
    
    SetUserInfo("SoundFX",mbOnSoundFX);
    SetUserInfo("Music",mbOnMusic);
    SetUserInfo("Vibration", mbOnVibration);
    
    //가장처음 실행했을때 기본적인 골드를 넣어준다.
//    SetUserInfo("FirstRun",1);
    
    SetUserInfo("BeforeScoreGrade",mnBeforeScoreGrade);
    SetUserInfo("BeforeScoreGradeTime",mBeforeScoreGradeTime.tv_sec);
    
    
    SetUserInfo("CompletedRank",mnCompletedRank);
    SetUserInfo("UserID",msUserID.c_str());
    SetUserInfo("YourMessage",msYourMessage.c_str());
    
    SetUserInfo("Report",mnReport);
    
    SetUserInfo("BuyCnt",mnBuyCnt);
    SetUserInfo("ClickPromotionCnt",mnClickPromotionCnt);
    SetUserInfo("TotalPlayTime",mnTotalPlayTime);
    SetUserInfo("ScoreGrade",mnScoreGrade);
    SetUserInfo("MultiplayOn",mbMultiplayOn);

    
    char sNum[20];
    string sCrc;
    //---------------------------------------------------------------------------------------------------------------
    //CRC정보를 만든다.
    sprintf(sNum, "%d",(int)mfGlold);
    sCrc.append(sNum);
    sprintf(sNum, "%d",mnRank);
    sCrc.append(sNum);
    sprintf(sNum, "%d",mnTotalScore);
    sCrc.append(sNum);
    int nSize = (int)mListBomb.size();
    for(int i = 0; i < nSize; i++)
    {
        sprintf(sNum, "%d",mListBomb[i]->nCnt);
        sCrc.append(sNum);
    }
    nSize = (int)mListItem.size();
    for(int i = 0; i < nSize; i++)
    {
        sprintf(sNum, "%d",mListItem[i]->nCnt);
        sCrc.append(sNum);
    }
    //---------------------------------------------------------------------------------------------------------------
    char strValue[512];
    memset(strValue, 0, sizeof(strValue));
    //strcpy(strValue,"Hellow My Name is hohak ,,,,, U Don't Hack..humhum");
    strcpy(strValue,sCrc.c_str());
    int nLen = (int)strlen(strValue);
    XorCrypto((unsigned char*)gDevideID.c_str(), (int)strlen(gDevideID.c_str()), (unsigned char*)strValue, (int)nLen);
    char* sBase64 = ToBase64((const void*)strValue,(int)nLen,false);
    SetUserInfo("Songs",sBase64);
    delete[] sBase64;
    SetUserInfo("SongsLen", nLen);
    //---------------------------------------------------------------------------------------------------------------

    SaveUserProperty();
#endif
}


int CUserInfo::GetNotCompletedMap(int& nOutIndex)
{
    int i = 0;
    int nID = -1;
    nOutIndex = -1;
    vector<PROPERTY_MAP*> lst;
    PROPERTY_MAP::GetPropertyMapList(lst);
    for( vector<PROPERTY_MAP*>::iterator it = lst.begin(); it != lst.end(); it++,i++)
    {
        if(IsCompletedMap((*it)->nID)) continue;
        
        nOutIndex = i;
        nID = (*it)->nID;
        break;
    }
    PROPERTYI::ReleaseList((vector<PROPERTYI*>*)&lst);
    return nID;
}

int CUserInfo::GetMIDToIndex(int nInID)
{
    int nIndex = 0;
    vector<PROPERTY_MAP*> lst;
    PROPERTY_MAP::GetPropertyMapList(lst);
    int i = 0;
    for( vector<PROPERTY_MAP*>::iterator it = lst.begin(); it != lst.end(); it++,i++)
    {
        if(nInID == (*it)->nID)
        {
            nIndex = i;
            break;
        }
    }
    PROPERTYI::ReleaseList((vector<PROPERTYI*>*)&lst);
    return nIndex;
}


void CUserInfo::UpgradeCompletedRank()
{
    int nID,nIndex;
    SetCompletedRank(GetCompletedRank() + 1);
    mCompletedMap.clear();
    mCompletedMap.push_back(0); //훈련소는 이미 완료 했다라고 봐야하죠..
    nID = GetNotCompletedMap(nIndex);
    SetLastSelectedMapID(nID);
    SetLastSelectedMapIndex(nIndex);
}


bool BombIDSort( USERINFO* a, USERINFO* b )
{
    if(a->nType == 1) return true; //영구 폭탄은 가장 앞에 가게 하자.
    else if(b->nType == 1) return false; //영구 폭탄은 가장 앞에 가게 하자.
    return a->nID > b->nID;    //오름차순 (폭탄의 아이디가 뒤로 갈수록 강한 폭탄이다)
}

bool CUserInfo::AddListToBomb(int nID,int nCount,int nType)
{
    bool isData = false;
    vector<USERINFO*>::iterator i;
    for (i = mListBomb.begin(); i != mListBomb.end(); i++) 
    {
        if((*i)->nID == nID)
        {
            (*i)->nCnt += nCount;
            isData = true;
            break;
        }
    }
    
    if(isData == false)
    {
        USERINFO* pInfo = new USERINFO;
        pInfo->nID = nID;
        pInfo->nCnt = nCount;
        pInfo->nType = nType;
        mListBomb.push_back(pInfo);
        sort(mListBomb.begin(), mListBomb.end(),BombIDSort);
    }
    return isData;
}


void CUserInfo::RemoveListToBomb(int nID)
{
    vector<USERINFO*>::iterator i;
    for (i = mListBomb.begin(); i != mListBomb.end(); i++) 
    {
        if((*i)->nID == nID)
        {
            if((*i)->nCnt > 0)
            {
                (*i)->nCnt--;
            }
            else
            {
                delete *i;
                mListBomb.erase(i);
            }
            break;
        }
    }
}

vector<USERINFO*>* CUserInfo::GetListBomb()
{
    return &mListBomb;
}

//건물은 탱크 정보에서 읽어온다.
vector<USERINFO*>* CUserInfo::GetListBlz()
{
    if(mListBlz.size() == 0)
    {
        vector<PROPERTY_TANK*> arrTank;
        PROPERTY_TANK::GetPropertyTankList(arrTank);
        for (vector<PROPERTY_TANK*>::iterator it = arrTank.begin(); it != arrTank.end(); it++)
        {
            
            if((*it)->nType == 3)
            {
                USERINFO* pNew = new USERINFO;
                pNew->nID = (*it)->nID;
                pNew->nType = 0;
                pNew->nCnt = (*it)->nGold;
                mListBlz.push_back(pNew);
            }
        }
        PROPERTY_TANK::ReleaseList((vector<PROPERTYI*>*)&arrTank);
    }
    return &mListBlz;
}

bool CUserInfo::AddListToItem(int nID,int nType)
{
    bool isData = false;
    vector<USERINFO*>::iterator i;
    for (i = mListItem.begin(); i != mListItem.end(); i++) 
    {
        if((*i)->nID == nID)
        {
            (*i)->nCnt ++;
            isData = true;
            break;
        }
    }
    if(isData == false)
    {
        USERINFO* pInfo = new USERINFO;
        pInfo->nID = nID;
        pInfo->nCnt = 1;
        pInfo->nType = nType;
        mListItem.push_back(pInfo);
    }
    return isData;
}

void CUserInfo::RemoveListToItem(int nID)
{
    vector<USERINFO*>::iterator i;
    for (i = mListItem.begin(); i != mListItem.end(); i++) 
    {
        if((*i)->nID == nID)
        {
            if((*i)->nCnt > 1)
            {
                (*i)->nCnt--;
            }
            else
            {
                delete *i;
                mListItem.erase(i);
            }
            break;
        }
    }  
}

int CUserInfo::GetCntByIDWithItem(int nID)
{
    vector<USERINFO*>::iterator i;
    for (i = mListItem.begin(); i != mListItem.end(); i++) 
    {
        if((*i)->nID == nID)
        {
            if((*i)->nType == 1) 
                return -999; //영구적인 사용
            return (*i)->nCnt;
        }
    }  
    return 0;
}

int CUserInfo::GetCntByIDWithBomb(int nID)
{
    vector<USERINFO*>::iterator i;
    for (i = mListBomb.begin(); i != mListBomb.end(); i++) 
    {
        if((*i)->nID == nID)
        {
            if((*i)->nType == 1) 
                return -999; //영구적인 사용
            return (*i)->nCnt;
        }
    }  
    return 0;
}

bool CUserInfo::CanAlive()
{
    vector<USERINFO*>::iterator i;
    for (i = mListItem.begin(); i != mListItem.end(); i++)
    {
        if((*i)->nID == ID_ITEM_FILL_POWER)
        {
            if((*i)->nCnt == 0) return false;
            else
            {
                return true;
            }
        }
    }
    return false;
}

bool CUserInfo::MinusFillItem(int &outnCnt)
{
    vector<USERINFO*>::iterator i;
    outnCnt = -999;
    for (i = mListItem.begin(); i != mListItem.end(); i++)
    {
        if((*i)->nID == ID_ITEM_FILL_POWER)
        {
            if((*i)->nCnt == 0) return false;
            else
            {
                (*i)->nCnt -= 1;
                outnCnt = (*i)->nCnt;
                
                if(outnCnt == 0)
                    mListItem.erase(i);
                
                return true;
            }
        }
    }
    return false;
}


vector<USERINFO*>* CUserInfo::GetListItem()
{
    return &mListItem;
}


void CUserInfo::AddListToCompletedMap(int nID)
{
    bool isData = false;
    vector<int>::iterator i;
    for (i = mCompletedMap.begin(); i != mCompletedMap.end(); i++) 
    {
        if((*i) == nID)
        {
            isData = true;
            break;
        }
    }
    
    if(isData == false)
        mCompletedMap.push_back(nID);
}


vector<int>* CUserInfo::GetListCompletedMap()
{
    return &mCompletedMap;
}

bool CUserInfo::IsCompletedMap(int nID)
{
    vector<int>::iterator i;
    for (i = mCompletedMap.begin(); i != mCompletedMap.end(); i++)
    {
        if((*i) == nID)
            return true;
        
    }
    return false;
}


void* CUserInfo::gUserProperty = 0;

void* CUserInfo::DefaultUserProperty()
{
    if(CUserInfo::gUserProperty) return CUserInfo::gUserProperty;
    
#ifdef ANDROID
#ifdef NOTOPENGL
    string sPath = TESTPATH;
#else
    string sPath = CLoader::getSearchPath();
#endif
    sPath += ".UserInfo.plist";
    
//    const char* sPath = "/Users/hohak/Temp/UserInfo.plist";
    CUserInfo::gUserProperty = new CHDoc;
    if(((CHDoc*)CUserInfo::gUserProperty)->Load(sPath.c_str()) == false)
    {
        //없으면 Default로 만든다.
        const char* sXML = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><plist version=\"1.0\"><dict></dict></plist>";
        if(((CHDoc*)CUserInfo::gUserProperty)->LoadXML(sXML) == false)
        {
            HLogE("[ERROR] UserInfo Make Failed\n");
            delete (CHDoc*)CUserInfo::gUserProperty;
            CUserInfo::gUserProperty  = NULL;
            return NULL;
        }
    }
#else
    NSArray *sPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [sPaths objectAtIndex:0];
    NSString *sPath = [documentsDirectory stringByAppendingPathComponent: @".UserInfo.plist" ];
    
    
    CUserInfo::gUserProperty = [[NSMutableDictionary alloc] initWithContentsOfFile:sPath];
    if(CUserInfo::gUserProperty == nil)
        CUserInfo::gUserProperty = [[NSMutableDictionary alloc] initWithCapacity:10];
#endif
    return CUserInfo::gUserProperty;
    
}

void CUserInfo::SaveUserProperty()
{
    if(CUserInfo::gUserProperty == 0) return ;
#ifdef ANDROID
    //TODO::
    //const char* sPath = "/Users/hohak/Temp/UserInfo.plist";
    #ifdef NOTOPENGL
    string sPath = TESTPATH;
    #else
    string sPath = CLoader::getSearchPath();
    #endif
    sPath += ".UserInfo.plist";
    if(((CHDoc*)CUserInfo::gUserProperty)->Save(sPath.c_str()) == false)
    {
        HLogE("[ERROR] Save UserInfo %s\n",sPath.c_str());
    }
    
#else
    NSArray *sPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [sPaths objectAtIndex:0];
    NSString *sPath = [documentsDirectory stringByAppendingPathComponent: @".UserInfo.plist" ];
    
    [((NSMutableDictionary*)CUserInfo::gUserProperty) writeToFile:sPath atomically:YES];
#endif
}

void CUserInfo::ReleaseUserProperty()
{
#ifdef ANDROID
    if(CUserInfo::gUserProperty)
    {
        delete (CHDoc*)CUserInfo::gUserProperty;
        CUserInfo::gUserProperty= NULL;
    }
#else
    if(CUserInfo::gUserProperty)
    {
        [((NSMutableDictionary*)CUserInfo::gUserProperty) release];
    }
    CUserInfo::gUserProperty = 0;
#endif
}



int CUserInfo::GetUserInfo(const char* sKey,int nDefault)
{
#ifdef ANDROID
    CHDoc* pDic = (CHDoc*)DefaultUserProperty();
    NodeDataValue* pObj = (NodeDataValue*)pDic->GetData(sKey);
    if(pObj == NULL)
    {
        return nDefault;
    }
    return pObj->GetInt();
#else

    NSString* nsKey = [NSString stringWithCString:sKey encoding:NSUTF8StringEncoding];
    NSDictionary* pDic = (NSMutableDictionary*)DefaultUserProperty();
    NSNumber* pObj = [pDic objectForKey:nsKey];
    if(pObj == nil)
    {
        return nDefault;
    }
    return pObj.intValue;
#endif
}

float CUserInfo::GetUserInfoF(const char* sKey,float fDefault)
{
#ifdef ANDROID
    CHDoc* pDic = (CHDoc*)DefaultUserProperty();
    NodeDataValue* pObj = (NodeDataValue*)pDic->GetData(sKey);
    if(pObj == NULL)
    {
        return fDefault;
    }
    return pObj->GetReal();
#else
    
    NSString* nsKey = [NSString stringWithCString:sKey encoding:NSUTF8StringEncoding];
    NSDictionary* pDic = (NSMutableDictionary*)DefaultUserProperty();
    NSNumber* pObj = [pDic objectForKey:nsKey];
    if(pObj == nil)
    {
        return fDefault;
    }
    return pObj.floatValue;
#endif
}





const char* CUserInfo::GetUserInfoS(const char* sKey)
{
    static string sR;
    sR = "";
#ifdef ANDROID
    CHDoc* pDic = (CHDoc*)DefaultUserProperty();
    NodeDataValue* pObj = (NodeDataValue*)pDic->GetData(sKey);
    if(pObj == NULL)
    {
        return sR.c_str();
    }
    sR = pObj->GetString();
    return sR.c_str();
#else
    
    NSString* nsKey = [NSString stringWithCString:sKey encoding:NSUTF8StringEncoding];
    NSDictionary* pDic = (NSMutableDictionary*)DefaultUserProperty();
    NSString* pObj = [pDic objectForKey:nsKey];
    if(pObj == nil)
    {
        return sR.c_str();
    }
    
    sR = pObj.UTF8String;
    return sR.c_str();
#endif
}





void CUserInfo::SetUserInfo(const char* sKey,int nValue)
{
#ifdef ANDROID
    char strValue[20];
    CHDoc* pDic = (CHDoc*)DefaultUserProperty();
    sprintf(strValue,"%d",nValue);
    NodeDataValue* pValue = new NodeDataValue(strValue,NodeDataInt);
    pDic->SetData(sKey,pValue);
#else

    NSString* nsKey = [NSString stringWithCString:sKey encoding:NSUTF8StringEncoding];
    NSMutableDictionary* pDic = (NSMutableDictionary*)DefaultUserProperty();
    //[pDic objectForKey:nsKey];
    NSNumber* pObj = [NSNumber numberWithUnsignedInt:nValue];
    [pDic setValue:pObj forKey:nsKey];
#endif
}


void CUserInfo::SetUserInfoF(const char* sKey,float fValue)
{
#ifdef ANDROID
    char strValue[20];
    CHDoc* pDic = (CHDoc*)DefaultUserProperty();
    sprintf(strValue,"%f",fValue);
    NodeDataValue* pValue = new NodeDataValue(strValue,NodeDataReal);
    pDic->SetData(sKey,pValue);
#else
    
    NSString* nsKey = [NSString stringWithCString:sKey encoding:NSUTF8StringEncoding];
    NSMutableDictionary* pDic = (NSMutableDictionary*)DefaultUserProperty();
    //[pDic objectForKey:nsKey];
    NSNumber* pObj = [NSNumber numberWithFloat:fValue];
    [pDic setValue:pObj forKey:nsKey];
#endif
}


void CUserInfo::SetUserInfo(const char* sKey,const char* sValue)
{
#ifdef ANDROID
    CHDoc* pDic = (CHDoc*)DefaultUserProperty();
    NodeDataValue* pValue = new NodeDataValue(sValue,NodeDataSring);
    pDic->SetData(sKey,pValue);
#else
    
    NSString* nsKey = [NSString stringWithCString:sKey encoding:NSUTF8StringEncoding];
    NSMutableDictionary* pDic = (NSMutableDictionary*)DefaultUserProperty();
    //[pDic objectForKey:nsKey];
    NSString* pObj = [NSString stringWithCString:sValue encoding:NSUTF8StringEncoding];
    [pDic setValue:pObj forKey:nsKey];
#endif
}


void CUserInfo::SetUserInfoList(const char* sKey,vector<int>& list)
{
#ifdef ANDROID
    CHDoc* pDic = (CHDoc*)DefaultUserProperty();
    char strValue[10];
    NodeArray* pArrData = new NodeArray(NULL);
    int nCnt = (int)list.size();
    for (int i = 0; i < nCnt; i++)
    {
    
        sprintf(strValue,"%d",list[i]);
        NodeDataValue* pValue = new NodeDataValue(strValue,NodeDataInt);
        pArrData->mList.push_back(pValue);
    }
    pDic->SetData(sKey,pArrData);
#else

    NSString* nsKey = [NSString stringWithCString:sKey encoding:NSUTF8StringEncoding];
    NSMutableDictionary* pDic = (NSMutableDictionary*)DefaultUserProperty();
    
    NSMutableArray* pArrData = [[NSMutableArray alloc] initWithCapacity:10];
    int nCnt = list.size();
    for (int i = 0; i < nCnt; i++) 
    {
        NSNumber* pNumber = [NSNumber numberWithUnsignedInt:list[i]];
        [pArrData addObject:pNumber];
    }
    [pDic setValue:pArrData forKey:nsKey];
    [pArrData release];
#endif
}


void CUserInfo::SetUserInfoList(const char* sKey,vector<USERINFO*>& list)
{
#ifdef ANDROID
    char strValue[10];
    CHDoc* pDic = (CHDoc*)DefaultUserProperty();
    
    NodeArray* pArrData = new NodeArray(NULL);
    
    vector<USERINFO*>::iterator it;
    vector<USERINFO*>::iterator b = list.begin();
    
    for (it = b; it != list.end();)
    {
        USERINFO* pUserInfo = *it;//list[i];
        
        NodeArray* pArr = new NodeArray(NULL);
        
        sprintf(strValue,"%d",pUserInfo->nID);
        NodeDataValue *pID = new NodeDataValue(strValue,NodeDataInt);
        pArr->mList.push_back(pID);
        
        sprintf(strValue,"%d",pUserInfo->nCnt);
        NodeDataValue *pCnt = new NodeDataValue(strValue,NodeDataInt);
        pArr->mList.push_back(pCnt);
        
        sprintf(strValue,"%d",pUserInfo->nType);
        NodeDataValue *pType = new NodeDataValue(strValue,NodeDataInt);
        pArr->mList.push_back(pType);
    
        //폭탄일때 0값은 저장할때 지운다.
        if(pUserInfo->nType == 0 && pUserInfo->nCnt <= 0)
        {
            //메모리상에 지운다.
            it = list.erase(it);
            delete pArr;
            continue;
        }
        
        pArrData->mList.push_back(pArr);
        it++;
    }
    
    pDic->SetData(sKey,pArrData);
#else

    NSString* nsKey = [NSString stringWithCString:sKey encoding:NSUTF8StringEncoding];
    NSMutableDictionary* pDic = (NSMutableDictionary*)DefaultUserProperty();
    NSMutableArray* pArrData = [[NSMutableArray alloc] initWithCapacity:10];
    
    vector<USERINFO*>::iterator it;
    vector<USERINFO*>::iterator b = list.begin();
//    vector<USERINFO*>::iterator e = list.end();
    
    //Modified By Song 2013.04.07 수정...
    for (it = b; it != list.end(); )
    {
        USERINFO* pUserInfo = *it;//list[i];
        
        NSNumber* nID = [NSNumber numberWithUnsignedInt:pUserInfo->nID];
        NSNumber* nxCnt = [NSNumber numberWithUnsignedInt:pUserInfo->nCnt];
        NSNumber* nType = [NSNumber numberWithUnsignedInt:pUserInfo->nType];
        NSArray *pArr = [NSArray arrayWithObjects:nID,nxCnt,nType,nil];
        
        //0값은 저장할때 지운다.
        if(nType.intValue == 0 && nxCnt.intValue <= 0) 
        {
            //메모리상에 지운다. 
            it = list.erase(it);
            continue;
        }
        
        [pArrData addObject:pArr];
        it++;
    }
    
    
    [pDic setValue:pArrData forKey:nsKey];
    [pArrData release];
#endif
}


void CUserInfo::SetUserUpgradeMap(const char* sKey,map<int,int>& map)
{
#ifdef ANDROID
    char strValue[10];
    CHDoc* pDic = (CHDoc*)DefaultUserProperty();
    NodeArray* pArrData = new NodeArray(NULL);
    
    std::map<int,int>::iterator it;
    std::map<int,int>::iterator b = map.begin();
    std::map<int,int>::iterator e = map.end();
    
    for (it = b; it != e; it++)
    {
        int nKey = it->first;
        int nValue = it->second;
        if(nValue == 0) continue;
        
        sprintf(strValue,"%d",nKey);
        NodeDataValue* pKey = new NodeDataValue(strValue,NodeDataInt);
        pArrData->mList.push_back(pKey);
        
        sprintf(strValue,"%d",nValue);
        NodeDataValue* pValue = new NodeDataValue(strValue,NodeDataInt);
        pArrData->mList.push_back(pValue);
    }
    pDic->SetData(sKey,pArrData);
#else

    NSString* nsKey = [NSString stringWithCString:sKey encoding:NSUTF8StringEncoding];
    NSMutableDictionary* pDic = (NSMutableDictionary*)DefaultUserProperty();
    NSMutableArray* pArrData = [[NSMutableArray alloc] initWithCapacity:10];
    
    std::map<int,int>::iterator it;
    std::map<int,int>::iterator b = map.begin();
    std::map<int,int>::iterator e = map.end();

    for (it = b; it != e; it++) 
    {
        int nKey = it->first;
        int nValue = it->second;
        if(nValue == 0) continue;
        [pArrData addObject:[NSNumber numberWithInt:nKey]];
        [pArrData addObject:[NSNumber numberWithInt:nValue]];
    }
    
    
    [pDic setValue:pArrData forKey:nsKey];
    [pArrData release];
#endif
}


map<int,int>* CUserInfo::GetUserUpgradeMap(const char* sKey,map<int,int>& map)
{
#ifdef ANDROID
    
    CHDoc* pDic = (CHDoc*)DefaultUserProperty();
    NodeArray* pArrData = (NodeArray*)pDic->GetData(sKey);
    if(pArrData == 0)
    {
        return 0;
    }
    
    int nSize = (int)pArrData->mList.size();
    for (int i = 0; i < nSize; )
    {
        int nK = ((NodeDataValue*)pArrData->mList[i++])->GetInt();
        int nV = ((NodeDataValue*)pArrData->mList[i++])->GetInt();
        map[nK] = nV;
    }
#else
    NSString* nsKey = [NSString stringWithCString:sKey encoding:NSUTF8StringEncoding];
    NSDictionary* pDic = (NSMutableDictionary*)DefaultUserProperty();
    NSArray* pArrData = [pDic objectForKey:nsKey];
    if(pArrData == 0)
    {
        return 0;
    }
    int nSize = pArrData.count;
    for (int i = 0; i < nSize; ) {
        NSNumber* svKey = [pArrData objectAtIndex:i++];
        NSNumber* sv = [pArrData objectAtIndex:i++];
        map[svKey.intValue] = sv.intValue;
    }
#endif
    return &map;
}

vector<int>* CUserInfo::GetUserInfoList(const char* sKey,vector<int>& list)
{
#ifdef ANDROID

    CHDoc* pDic = (CHDoc*)DefaultUserProperty();
    NodeArray* pArrData = (NodeArray*)pDic->GetData(sKey);
    if(pArrData == NULL)
    {
        return NULL;
    }
    int nCnt = (int)pArrData->mList.size();
    for(int i = 0; i < nCnt; i++)
    {
        NodeDataValue* v = (NodeDataValue*)pArrData->mList[i];
        list.push_back(v->GetInt());
    }
#else

    NSString* nsKey = [NSString stringWithCString:sKey encoding:NSUTF8StringEncoding];
    NSDictionary* pDic = (NSMutableDictionary*)DefaultUserProperty();
    NSArray* pArrData = [pDic objectForKey:nsKey];
    if(pArrData == nil)
    {
        return nil;
    }
    
    for (NSNumber* sv in pArrData) 
    {
        list.push_back(sv.intValue);
    }
#endif
    return &list;
}

vector<USERINFO*>* CUserInfo::GetUserInfoList(const char* sKey,vector<USERINFO*>& list)
{
#ifdef ANDROID
    CHDoc* pDic = (CHDoc*)DefaultUserProperty();
    NodeArray* pArrData = (NodeArray*)pDic->GetData(sKey);
    if(pArrData == NULL)
    {
        return NULL;
    }
    
    int nCnt = (int)pArrData->mList.size();
    for(int i = 0; i <nCnt;i++)
    {
        NodeArray* arr = (NodeArray*)pArrData->mList[i];
        USERINFO *pUserInfo = new USERINFO;
        pUserInfo->nID = ((NodeDataValue*)arr->mList[0])->GetInt();
        pUserInfo->nCnt = ((NodeDataValue*)arr->mList[1])->GetInt();
        pUserInfo->nType = ((NodeDataValue*)arr->mList[2])->GetInt();
        list.push_back(pUserInfo);
    }
#else

    NSString* nsKey = [NSString stringWithCString:sKey encoding:NSUTF8StringEncoding];
    NSDictionary* pDic = (NSMutableDictionary*)DefaultUserProperty();
    NSArray* pArrData = [pDic objectForKey:nsKey];
    if(pArrData == nil)
    {
        return nil;
    }
    
    for (NSArray* arr in pArrData) 
    {
        //list.push_back(sv.intValue);
        USERINFO *pUserInfo = new USERINFO;
        
        NSNumber* nID = [arr objectAtIndex:0];
        pUserInfo->nID = nID.intValue;
        NSNumber* nCnt = [arr objectAtIndex:1];
        pUserInfo->nCnt = nCnt.intValue;
        NSNumber* nType = [arr objectAtIndex:2];
        pUserInfo->nType = nType.intValue;
        
        list.push_back(pUserInfo);
        
    }
#endif
    return &list;
}

void CUserInfo::ReleaseUserInfoRelease(vector<USERINFO*>& list)
{
    vector<USERINFO*>::iterator i;
    for (i = list.begin(); i != list.end(); i++) 
    {
        delete *i;
    }
    list.clear();
}








