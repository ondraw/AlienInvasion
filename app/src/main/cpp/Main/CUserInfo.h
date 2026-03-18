//
//  CUserInfo.h
//  SongGL
//  사용자 정보를 가지고 있다.
//  Created by 송 호학 on 11. 12. 19..
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef SongGL_CUserInfo_h
#define SongGL_CUserInfo_h
#include <vector>
#include <map>
#include <string>
#include "sGLUtils.h"
#include "CProperty.h"
using namespace std;


//2:일때는 fGold를 %f로 했다.
//3:부터는 fGold를 %d로 해준다. (이유는 0.01이 0.00999가 될 수 있기 때문이다.)
#define USERVER_VERSION 3
class USERINFO
{
public:
    USERINFO() { pProp = NULL;}
    virtual ~USERINFO() { if(pProp) {delete pProp; pProp = NULL;}}
    
    int nID;
    int nCnt;
    int nType; //0:Count 1:영구사용  2:Upgrade 3:한번만사용
    
    PROPERTYI* pProp;
};

//사용자 현재 정보이다...
class CUserInfo 
{
public:
    
    CUserInfo();
    ~CUserInfo();
    
    static CUserInfo* GetDefaultUserInfo();
    
    
    void Load();
    void Save();
    
    //각 탱그별로 기본 폭탄 종류가 틀리다. 이전의 영구 폭탄을 전부 제거하고 , 
    //디폴트 영구폭탄을 Add한다.
    void SetDefaultBomb(int nBombID);
    float GetGold() {
//#ifdef DEBUG //Test
//        return 899999;
//#endif
        if(mfGlold < 0.f)            mfGlold = 0.f;
        return mfGlold;
    }
    
    void SetGold(float v)
    {
        mfGlold = v;
    }
    
    int  GetReport() { return mnReport;}
    void SetReport(int v) { mnReport = v;}
    
    unsigned int GetRank() { return mnRank;}
    void SetRank(unsigned int v) { mnRank = v;}
    
    void SetLastSelectedItemPageIndex(int v) { mnLastSelectedItemPageIndex = v;}
    int GetLastSelectedItemPageIndex() { return mnLastSelectedItemPageIndex;}
    
    void SetLastSelectedBombPageIndex(int v) { mnLastSelectedBombPageIndex = v;}
    int GetLastSelectedBombPageIndex() { return mnLastSelectedBombPageIndex;}
    
    void SetLastSelectedBlzPageIndex(int v) { mnLastSelectedBlzPageIndex = v;}
    int GetLastSelectedBlzPageIndex() { return mnLastSelectedBlzPageIndex;}
    
    //0:Bomb, 1: Item
    void SetLastBombItemSwitch(int v) { mnLastBombItemSwitch = v;};
    int  GetLastBombItemSwitch() { return mnLastBombItemSwitch;}
    
    int GetLastSelectedMapIndex() { return mLastSelectedMapIndex;}
    void SetLastSelectedMapIndex(int v) { mLastSelectedMapIndex = v;} 
    int GetLastSelectedTankIndex() { return mLastSelectedTankIndex;}
    void SetLastSelectedTankIndex(int v) { mLastSelectedTankIndex = v;}

    
    int GetLastSelectedMapID() { return mLastSelectedMapID;}
    void SetLastSelectedMapID(int v) { mLastSelectedMapID = v;} 
    
    int GetLastSelectedTankID() { return mLastSelectedTankID;}
    void SetLastSelectedTankID(int v) { mLastSelectedTankID = v;}
    
    int GetSoundFX() { return mbOnSoundFX;}
    void SetSoundFX(int v) { mbOnSoundFX = v;}
    
    int GetMusic() { return mbOnMusic;}
    void SetMusic(int v) { mbOnMusic = v;}
    
    int GetVibration() { return mbOnVibration;}
    void SetVibration(int v) { mbOnVibration = v;}
    
    
    //nType = 0 (count) , 1(영구적으로사용)
    bool AddListToBomb(int nID,int nCount = 1,int nType = 0);
    void RemoveListToBomb(int nID);
    vector<USERINFO*>* GetListBomb();
    vector<USERINFO*>* GetListBlz();
    //nType = 0 (count) , 1(영구적으로사용)    
    bool AddListToItem(int nID,int nType = 0);
    void RemoveListToItem(int nID);
    vector<USERINFO*>* GetListItem();

    
    void AddListToCompletedMap(int nID);
    vector<int>* GetListCompletedMap();
    bool IsCompletedMap(int nID);
    
    //-999영구적인 사용
    int GetCntByIDWithItem(int nID);
    //-999영구적인 사용
    int GetCntByIDWithBomb(int nID);
    
    int AddAttackUpgrade();
    int AddDependUpgrade();
    
    int GetAttackUpgrade();
    int GetDependUpgrade();
    
//    map<int,int>* GetAUpgrade() { return &mAttackUpgrade; }
//    map<int,int>* GetDUpgrade() { return &mDefendUpgrade; }
    
    
    //아이템을 마이너스 해준다.
    //bool : true(다시살아날수 있는) false:못살아나는
    bool MinusFillItem(int &outnCnt);
    bool CanAlive();
    
    
    static void* DefaultUserProperty();
    static void SaveUserProperty();
    static void ReleaseUserProperty();
    static int GetUserInfo(const char* sKey,int nDefault = 0);
    static float GetUserInfoF(const char* sKey,float fDefault = 0.f);
    static const char* GetUserInfoS(const char* sKey);
    
    static void SetUserInfo(const char* sKey,int nValue);
    static void SetUserInfo(const char* sKey,const char* sValue);
    static void SetUserInfoF(const char* sKey,float fValue);
    
    static vector<int>* GetUserInfoList(const char* sKey,vector<int>& list);
    static void SetUserInfoList(const char* sKey,vector<int>& list);
    static void SetUserInfoList(const char* sKey,vector<USERINFO*>& list);
    static vector<USERINFO*>* GetUserInfoList(const char* sKey,vector<USERINFO*>& list);
    static void ReleaseUserInfoRelease(vector<USERINFO*>& list);
    
    static void SetUserUpgradeMap(const char* sKey,map<int,int>& map);
    static map<int,int>* GetUserUpgradeMap(const char* sKey,map<int,int>& map);
    
    void AddTotalScore(int nScore) {
        if(nScore < 0 || nScore > 10000000) nScore = 300;
        mnTotalScore += nScore;
    }
    unsigned int GetTotalScore() {
//#ifdef DEBUG //Test
//        return 99999999;
//#endif

        return mnTotalScore;
    }
    
    
    void SetPromotionSloder(bool v) { mbPromotionSloder = v;}
    bool IsPromotionSloder() { return mbPromotionSloder;}
    
    const char* GetUserID() { return msUserID.c_str();}
    void SetUserID(const char* s) { msUserID = s;}
    
    const char* GetUserPwd() { return msUserPwd.c_str();};
    
    void SetScoreGrade(long v) { mnScoreGrade = v;}
    long GetScoreGrade() { return mnScoreGrade;}

    void SetBeforeScoreGrade(long v) { mnBeforeScoreGrade = v;}
    long GetBeforeScoreGrade() { return mnBeforeScoreGrade;}
    
    void SetCompletedRank(int v) { mnCompletedRank = v;}
    int GetCompletedRank() { return mnCompletedRank;}
    
    void SetYourMessage(const char* v) { msYourMessage = v;}
    const char* GetYourMessage() { return msYourMessage.c_str();}
    

    void SetBeforeScoreGradeTime(timeval v) { mBeforeScoreGradeTime = v;}
    timeval GetBeforeScoreGradeTime() { return mBeforeScoreGradeTime;}
    
    int GetNotCompletedMap(int& nOutIndex);
    void UpgradeCompletedRank();
    
    
    
    void SetBuy() { mnBuyCnt++;}
    int  GetBuy() { return mnBuyCnt;}
    void ResetBuy() { mnBuyCnt = 0;}
    
    void SetTotalPlayTime(unsigned int v) { mnTotalPlayTime += v;}
    unsigned int GetTotalPalyTime() { return mnTotalPlayTime;}
    void SetClickedProp() { mnClickPromotionCnt++;}
    int  GetClickedProp() { return mnClickPromotionCnt;}
    void ResetClickedProp() { mnClickPromotionCnt = 0;}
    
    
    int GetMIDToIndex(int nInID);
    
    void SetMultiplayOn(bool v) { mbMultiplayOn = v;}
    bool GetMultiplayOn() { return mbMultiplayOn;}
    
    //0:로그인되지 않음. 1:로그인중 2:로그인됨
    void SetGameCenterStatus(int v) {
        mnGameCenterStatus = v;
    }
    //0:로그인되지 않음. 1:로그인중 2:로그인됨
    int  GetGameCenterAuthStatus() { return mnGameCenterStatus;}
public:
    //-----------------------------------------------------------
    //Gether Info
    int                         mnBGetherScore;             //판을 실행하기 전의
    unsigned  long              mnBGetherTime;
    int                         mnGetherKillCount;          //현재 판의 축인 횟수.
    int                         mnGetherBuildBlzCount;      //정보.
    unsigned short              mnGetherNowMapID;           //
    bool                        mbSucceedMap;               //
    unsigned short              mnRunTimeUpgrade;           //런타임시 업그레이드 수
    unsigned int                mnALevel;
    unsigned int                mnDLevel;
    //-----------------------------------------------------------
    
protected:
    static void* gUserProperty; //NSMutableDictionary
    
protected:
    float mfGlold;
    unsigned int mnRank;
    unsigned int mnTotalScore;                          //지금까지 총 스코어
    unsigned int mnTotalPlayTime;                        //초단위로
        
    
    vector<USERINFO*> mListBomb;                        //현재 보유한 폭탄 종류
    vector<USERINFO*> mListItem;                        //현재 보유한 아이템 종류
    vector<USERINFO*> mListBlz;                         //
    vector<int> mCompletedMap;                          //미션에 성공한 맵 리스트
    
    int         mAttackUpgrade; //id:tankid value:upgrage
    int         mDefendUpgrade; //id:tankid value:upgrage
    
    int         mLastSelectedMapIndex;
    int         mLastSelectedTankIndex;
    int         mLastSelectedMapID;
    int         mLastSelectedTankID;
    int         mnLastSelectedItemPageIndex;
    int         mnLastSelectedBombPageIndex;
    int         mnLastSelectedBlzPageIndex;
    
    int         mnLastBombItemSwitch;//0:Bomb, 1: Item 2:Blz
    
    
    int        mbOnSoundFX;
    int        mbOnMusic;
    int        mbOnVibration;
    
    long       mnScoreGrade; //현재 등수
    long       mnBeforeScoreGrade; //이전 등수
    timeval    mBeforeScoreGradeTime; //이전에 등록한 시간을 가져온다.
    
    bool       mbPromotionSloder; //훈련병에서 이병으로 진급해야 한다면 true 파일에 저장하지 않음.
    string     msUserID;
    string     msUserPwd;
    
    int        mnCompletedRank;
    string     msYourMessage;
    int        mnReport; //평점을 유도하자. 0:평가안함 1:평가메세지만 보이고 무시했음 -1:평가한다고 YES를 했음.(Item을 주어야 한다.) -2:아이템을 주었다.
    
    int        mnBuyCnt; //돈주고 산갯수
    int        mnClickPromotionCnt;//광고 클릭수
    
    short      mnVersion;
    bool       mbMultiplayOn;
    
    int        mnGameCenterStatus; //0:로그인되지 않음. 1:로그인중 2:로그인됨
};


#endif
