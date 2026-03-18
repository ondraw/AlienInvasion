//
//  AlienInvasionProtocolMan.cpp
//  SongGL
//
//  Created by 호학 송 on 13. 10. 14..
//
//
#include "sGLDefine.h"
#include "AlienInvasionProtocolMan.h"
#include "MinkConnector.h"
#include "MinkRecordSet.h"
#include <map>
#include <vector>
#include <string>
#include "sGLUtils.h"
#include "sGL.h"
using namespace std;
extern void* AlienInvasionProtocolThread(void *data);
extern int gDisplayWidth;
extern int gDisplayHeight;

AlienInvasionProtocolMan::AlienInvasionProtocolMan(CUserInfo* pUserInfo)
{
    mpUserInfo = pUserInfo;
#ifdef SEND_CUSTOM_USER_INFO
    mThreadMainID = 0;
    mnType = 1;
#endif //SEND_CUSTOM_USER_INFO
}

AlienInvasionProtocolMan::~AlienInvasionProtocolMan()
{
#ifdef SEND_CUSTOM_USER_INFO
    int nResult = 0;
    if(mThreadMainID != 0)
	{
		pthread_join(mThreadMainID, (void **) &nResult);
	}
#endif //SEND_CUSTOM_USER_INFO
}

//서버에 데이터를 올려준다.
void AlienInvasionProtocolMan::SendUserInfo()
{
#ifdef SEND_CUSTOM_USER_INFO
    HLogD("SendUserInfo\n");
    //이미 데이터를 보내고 있다.
    if(mThreadMainID) return;
    
    mnType = 1;
    int nThreadResult = pthread_create(&mThreadMainID, 0, (void* (*)(void*))AlienInvasionProtocolThread, (void*)this);
	if(nThreadResult != 0)
	{
        HLogE("Can not create Thread SendUserInfo , pthread_create\n");
        return ;
	}
#endif
    if(gSendUserInfo) gSendUserInfo((void*)mpUserInfo);
}

void AlienInvasionProtocolMan::SendTrainningStep(int nStep)
{
#ifdef SEND_CUSTOM_USER_INFO
    HLogD("SendTrainningStep %d\n",nStep);
    //이미 데이터를 보내고 있다.
    if(mThreadMainID) return;
    
    mnType = 2;
    mnTrainningStep = nStep;
    int nThreadResult = pthread_create(&mThreadMainID, 0, (void* (*)(void*))AlienInvasionProtocolThread, (void*)this);
	if(nThreadResult != 0)
	{
        HLogE("Can not create Thread SendUserInfo , pthread_create\n");
        return ;
	}
#endif //SEND_CUSTOM_USER_INFO
}


#ifdef SEND_CUSTOM_USER_INFO
void AlienInvasionProtocolMan::AlienInvasionProtocolThread_SendUserInfo()
{

    char sTemp[40];
    CUserInfo* pUserInfo = GetUserInfo();
    
    MinkConnector Conn(pUserInfo->GetUserID(),pUserInfo->GetUserPwd(),GetUniqueID(),HTTPSERVER_IP,"alieninvasion",HttpRequestXmlOrg);
    MinkRecordSet* pRecordSet = NULL;
    list<RM_PARAM *> lstparam;
    
    RM_PARAM* param = NULL;
    
    
    //Version
    param = new RM_PARAM;
    param->strName = "XmlVersion";
    param->strMode = "in";
    param->strType = "string";
    param->SetValue("1.0");
    lstparam.push_back(param);
    
    
    //Device ID
    param = new RM_PARAM;
    param->strName = "DeviceID";
    param->strMode = "in";
    param->strType = "string";
    param->SetValue(GetUniqueID());
    lstparam.push_back(param);
    
    
    //Bundle ID
    param = new RM_PARAM;
    param->strName = "AppID";
    param->strMode = "in";
    param->strType = "string";
    param->SetValue(GetBundleID());
    lstparam.push_back(param);
    
    
    //User ID
    param = new RM_PARAM;
    param->strName = "UserID";
    param->strMode = "in";
    param->strType = "string";
    param->SetValue(pUserInfo->GetUserID());
    lstparam.push_back(param);
    
    
    //App Version
    param = new RM_PARAM;
    param->strName = "AppVersion";
    param->strMode = "in";
    param->strType = "string";
    param->SetValue(GetAppVersion());
    lstparam.push_back(param);
    
    
    //TotalCore
    param = new RM_PARAM;
    param->strName = "TotalCore";
    param->strMode = "in";
    param->strType = "string";
    sprintf(sTemp, "%ld",(long)pUserInfo->GetTotalScore());
    param->SetValue(sTemp);
    lstparam.push_back(param);
    
    //Gold
    param = new RM_PARAM;
    param->strName = "Gold";
    param->strMode = "in";
    param->strType = "string";
    sprintf(sTemp, "%ld",(long)pUserInfo->GetGold());
    param->SetValue(sTemp);
    
    lstparam.push_back(param);
    
    //Rank
    param = new RM_PARAM;
    param->strName = "Rank";
    param->strMode = "in";
    param->strType = "string";
    sprintf(sTemp, "%ld",(long)pUserInfo->GetRank());
    param->SetValue(sTemp);
    
    lstparam.push_back(param);
    
    //Model
    param = new RM_PARAM;
    param->strName = "Model";
    param->strMode = "in";
    param->strType = "string";
    param->SetValue(GetModel());
    
    lstparam.push_back(param);
    
    //MapRank
    param = new RM_PARAM;
    param->strName = "MapRank";
    param->strMode = "in";
    param->strType = "string";
    sprintf(sTemp, "%ld",(long)pUserInfo->GetCompletedRank());
    param->SetValue(sTemp);
    
    lstparam.push_back(param);
    
    
    //Country
    param = new RM_PARAM;
    param->strName = "Country";
    param->strMode = "in";
    param->strType = "string";
    param->SetValue(CountryName());
    lstparam.push_back(param);
    
    //One Day Up Down Grade
    param = new RM_PARAM;
    param->strName = "ADayGrade";
    param->strMode = "in";
    param->strType = "string";
    if(pUserInfo->GetScoreGrade() != 0)
        param->SetValue((long)(pUserInfo->GetBeforeScoreGrade() - pUserInfo->GetScoreGrade()));
    else
        param->SetValue((long)0);
    
    lstparam.push_back(param);
    
    
    param = new RM_PARAM;
    param->strName = "TotalTime";
    param->strMode = "in";
    param->strType = "string";
    param->SetValue(pUserInfo->GetTotalPalyTime());
    lstparam.push_back(param);


    
    //A Upgrade
    param = new RM_PARAM;
    param->strName = "AUpgrade";
    param->strMode = "in";
    param->strType = "string";
    //Item Count
    sprintf(sTemp, "%d",pUserInfo->GetAttackUpgrade());
    param->SetValue(sTemp);
    lstparam.push_back(param);
    
    
    //A Defend
    param = new RM_PARAM;
    param->strName = "DUpgrade";
    param->strMode = "in";
    param->strType = "string";
    //Item Count
    sprintf(sTemp, "%d",pUserInfo->GetDependUpgrade());
    param->SetValue(sTemp);
    lstparam.push_back(param);
    
    
    param = new RM_PARAM;
    param->strName = "Music";
    param->strMode = "in";
    param->strType = "string";
    //Item Count
    sprintf(sTemp, "%d",pUserInfo->GetMusic());
    param->SetValue(sTemp);
    lstparam.push_back(param);
    
    
    param = new RM_PARAM;
    param->strName = "SoundFX";
    param->strMode = "in";
    param->strType = "string";
    //Item Count
    sprintf(sTemp, "%d",pUserInfo->GetSoundFX());
    param->SetValue(sTemp);
    lstparam.push_back(param);
    
    param = new RM_PARAM;
    param->strName = "Vibration";
    param->strMode = "in";
    param->strType = "string";
    //Item Count
    sprintf(sTemp, "%d",pUserInfo->GetVibration());
    param->SetValue(sTemp);
    lstparam.push_back(param);
    
    
    param = new RM_PARAM;
    param->strName = "DisplayW";
    param->strMode = "in";
    param->strType = "string";
    //Item Count
    sprintf(sTemp, "%d",gDisplayWidth);
    param->SetValue(sTemp);
    lstparam.push_back(param);
    
    param = new RM_PARAM;
    param->strName = "DisplayH";
    param->strMode = "in";
    param->strType = "string";
    //Item Count
    sprintf(sTemp, "%d",gDisplayHeight);
    param->SetValue(sTemp);
    lstparam.push_back(param);
    
    
    param = new RM_PARAM;
    param->strName = "Report";
    param->strMode = "in";
    param->strType = "string";
    //Item Count
    sprintf(sTemp, "%d",pUserInfo->GetReport());
    param->SetValue(sTemp);
    lstparam.push_back(param);
    
    //BuyItem
    if(pUserInfo->GetBuy())
    {
        param = new RM_PARAM;
        param->strName = "BuyItem";
        param->strMode = "in";
        param->strType = "string";
        param->SetValue(pUserInfo->GetBuy());
        lstparam.push_back(param);
    }
    
    //ClickedPro //광고 클릭수
    if(pUserInfo->GetClickedProp())
    {
        param = new RM_PARAM;
        param->strName = "ClickedPro";
        param->strMode = "in";
        param->strType = "string";
        param->SetValue(pUserInfo->GetClickedProp());
        lstparam.push_back(param);
    }
    
    int nScore = 0;
    int nTime = 0;
//    int nKillCount = 0;
//    int nBlzCount = 0;
    if(pUserInfo->mnBGetherTime)
    {
        //--------------------------------------------------------------------
        //Score
        //KillCount
        //BlzCount
        //Time
        //--------------------------------------------------------------------
        nScore = pUserInfo->GetTotalScore() - pUserInfo->mnBGetherScore;;
        nTime = (GetGGTime() - pUserInfo->mnBGetherTime) / 1000; //초단위로 올려준다.
//        nKillCount = pUserInfo->mnGetherKillCount;
//        nBlzCount = pUserInfo->mnGetherBuildBlzCount;
        
        //MapScore
        param = new RM_PARAM;
        param->strName = "MapScore";
        param->strMode = "in";
        param->strType = "string";
        param->SetValue(nScore);
        lstparam.push_back(param);
        
        //MapKillCnt
        param = new RM_PARAM;
        param->strName = "MapKillCnt";
        param->strMode = "in";
        param->strType = "string";
        param->SetValue(pUserInfo->mnGetherKillCount);
        lstparam.push_back(param);
        
        //MapBlzCnt
        param = new RM_PARAM;
        param->strName = "MapBlzCnt";
        param->strMode = "in";
        param->strType = "string";
        param->SetValue(pUserInfo->mnGetherBuildBlzCount);
        lstparam.push_back(param);
        
        
        //MapTime
        param = new RM_PARAM;
        param->strName = "MapTime";
        param->strMode = "in";
        param->strType = "string";
        param->SetValue(nTime);
        lstparam.push_back(param);
        
        
        
        //RunTimeUp
        param = new RM_PARAM;
        param->strName = "RunTimeUp";
        param->strMode = "in";
        param->strType = "string";
        param->SetValue((int)pUserInfo->mnRunTimeUpgrade);
        lstparam.push_back(param);

        
        //DLevel
        param = new RM_PARAM;
        param->strName = "DLevel";
        param->strMode = "in";
        param->strType = "string";
        param->SetValue((int)pUserInfo->mnDLevel);
        lstparam.push_back(param);
        
        //ALevel
        param = new RM_PARAM;
        param->strName = "ALevel";
        param->strMode = "in";
        param->strType = "string";
        param->SetValue((int)pUserInfo->mnALevel);
        lstparam.push_back(param);
        
        //SucceedMap
        param = new RM_PARAM;
        param->strName = "SucceedMap";
        param->strMode = "in";
        param->strType = "string";
        param->SetValue((int)pUserInfo->mbSucceedMap);
        lstparam.push_back(param);
        
        //MapID
        param = new RM_PARAM;
        param->strName = "MapID";
        param->strMode = "in";
        param->strType = "string";
        param->SetValue(pUserInfo->mnGetherNowMapID);
        lstparam.push_back(param);
        
        
        //MapRank
        param = new RM_PARAM;
        param->strName = "MapRank";
        param->strMode = "in";
        param->strType = "string";
        param->SetValue(pUserInfo->GetCompletedRank());
        lstparam.push_back(param);
        
    }
    
    //데이터를 올리면 초기화를 해준다.
    pUserInfo->mnBGetherScore = 0;
    pUserInfo->mnBGetherTime = 0;
    pUserInfo->mnGetherKillCount = 0;
    pUserInfo->mnGetherBuildBlzCount = 0;
    pUserInfo->mnGetherNowMapID = 0;
    pUserInfo->mbSucceedMap = false;
    
    
    //Item
    vector<USERINFO*>* itemp = pUserInfo->GetListItem();
    param = new RM_PARAM;
    param->strName = "ItemInfo";
    param->strMode = "in";
    param->strType = "string";
    //Item Count
    sprintf(sTemp, "%ld",(long)itemp->size());
    param->SetValue(sTemp);
    
    lstparam.push_back(param);
    for (vector<USERINFO*>::iterator it = itemp->begin(); it != itemp->end(); it++)
    {
        param = new RM_PARAM;
        sprintf(sTemp, "%ld",(long)(*it)->nID);
        param->strName = sTemp;
        param->strMode = "in";
        param->strType = "string";
        sprintf(sTemp, "%ld",(long)(*it)->nCnt);
        param->SetValue(sTemp);
        
        lstparam.push_back(param);
    }
    
    //Bomb
    vector<USERINFO*>* bomb = pUserInfo->GetListBomb();
    param = new RM_PARAM;
    param->strName = "BombInfo";
    param->strMode = "in";
    param->strType = "string";
    //Item Count
    sprintf(sTemp, "%ld",(long)bomb->size());
    param->SetValue(sTemp);
    
    lstparam.push_back(param);
    for (vector<USERINFO*>::iterator it = bomb->begin(); it != bomb->end(); it++)
    {
        param = new RM_PARAM;
        sprintf(sTemp, "%ld",(long)(*it)->nID);
        param->strName = sTemp;
        param->strMode = "in";
        param->strType = "string";
        sprintf(sTemp, "%ld",(long)(*it)->nCnt);
        param->SetValue(sTemp);
        lstparam.push_back(param);
    }
    
    HLogD("Conn AlienInvasionProtocolThread_SendUserInfo\n");
    Conn.Function(SPRING_ID, "alieninvasion", COMMAND_UPDATEUSER, &lstparam, &pRecordSet);
    if(pRecordSet)
    {
        if(pRecordSet->GetSystemError() == 0)
        {
            pRecordSet->MoveFirst();
            if(!pRecordSet->IsEOF())
            {
                pUserInfo->SetScoreGrade(atol(pRecordSet->GetData(0)));
                timeval pNow = GetClock();
                timeval pBefore = pUserInfo->GetBeforeScoreGradeTime();
                
                double fDif = difftime(pNow.tv_sec,pBefore.tv_sec);
                int tm_day   = fDif / ( 60 *60 * 24);
                if(tm_day >= 1) //하루가지난것이면
                {
                    pUserInfo->SetBeforeScoreGrade(pUserInfo->GetScoreGrade());//이전스코어에 현재 스코어를 넣어준다.
                    pUserInfo->SetBeforeScoreGradeTime(pNow);
                }
                
                
            }
        }
        else
        {
            HLogE("System Error(%d) %s",pRecordSet->GetSystemError(),pRecordSet->GetSystemErrorMsg());
        }
        
        delete pRecordSet;
        pRecordSet = NULL;
    }
    
    //------------------------------------------------------------------
    //데이터를 올렸는데 받는데 에러가 나는 경우가 많나보다. 그래서 실패유무를 떠나서 지우자.
    pUserInfo->ResetBuy();
    pUserInfo->ResetClickedProp();
    //------------------------------------------------------------------
}

void AlienInvasionProtocolMan::AlienInvasionProtocolThread_SendTrainningStep(int nStep)
{

    char sTemp[40];
    CUserInfo* pUserInfo = GetUserInfo();
    
    MinkConnector Conn(pUserInfo->GetUserID(),pUserInfo->GetUserPwd(),GetUniqueID(),HTTPSERVER_IP,"alieninvasion",HttpRequestXmlOrg);
    MinkRecordSet* pRecordSet = NULL;
    list<RM_PARAM *> lstparam;
    
    RM_PARAM* param = NULL;
    
    
    //Version
    param = new RM_PARAM;
    param->strName = "XmlVersion";
    param->strMode = "in";
    param->strType = "string";
    param->SetValue("1.0");
    lstparam.push_back(param);
    
    
    //Device ID
    param = new RM_PARAM;
    param->strName = "DeviceID";
    param->strMode = "in";
    param->strType = "string";
    param->SetValue(GetUniqueID());
    lstparam.push_back(param);
    
    
    //Bundle ID
    param = new RM_PARAM;
    param->strName = "AppID";
    param->strMode = "in";
    param->strType = "string";
    param->SetValue(GetBundleID());
    lstparam.push_back(param);
    
    //Step
    param = new RM_PARAM;
    param->strName = "Step";
    param->strMode = "in";
    param->strType = "string";
    sprintf(sTemp, "%ld",(long)nStep);
    param->SetValue(sTemp);
    lstparam.push_back(param);
    
    
    HLogD("Conn AlienInvasionProtocolThread_SendTrainningStep\n");
    Conn.Function(SPRING_ID, "alieninvasion", COMMAND_UPDATESTEP, &lstparam, &pRecordSet);
    
    if(pRecordSet)
    {
        if(pRecordSet->GetSystemError() == 0)
        {
        }
        else
        {
            HLogE("System Error(%d) %s",pRecordSet->GetSystemError(),pRecordSet->GetSystemErrorMsg());
        }
        delete pRecordSet;
        pRecordSet = NULL;
    }
}

//쓰레드 안쪽에서 데이터를 보내는 작업을 한다.
void* AlienInvasionProtocolThread(void *data)
{
    static long lResult = 0;
    AlienInvasionProtocolMan* pMan = (AlienInvasionProtocolMan*)data;
    if(pMan->mnType == 1)
        pMan->AlienInvasionProtocolThread_SendUserInfo();
    else
        pMan->AlienInvasionProtocolThread_SendTrainningStep(pMan->mnTrainningStep);
    
    pMan->SetTheadEnded();
    return (void*)&lResult;
}
#endif //SEND_CUSTOM_USER_INFO