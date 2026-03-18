//
//  CSoundMan.cpp
//  SongGL
//
//  Created by Songs on 11. 9. 1..
//  Copyright 2011 thinkm. All rights reserved.
//
#include "sGLDefine.h"
#include "CSoundMan.h"
#include "sGLSound.h"
#include "CUserInfo.h"
#include "sGLUtils.h"

CSoundMan::CSoundMan()
{
    mVibrationTime = 0;
}

CSoundMan::~CSoundMan()
{
    Close();
}

void CSoundMan::Initialize(int nType)
{
    SoundData* pSoundData;
    int nSoundID = 0;//
    
    if(nType == 0)
    {
        nSoundID = MakeSystemSoundID("Click");
        if(nSoundID == 0) HLogE("[Error] Sound %s\n","Click");
        
        pSoundData = new SoundData;
        pSoundData->nSoundID = nSoundID;
        pSoundData->nIntervalTime = 0;
        mSoundMap[SOUND_ID_Click] = pSoundData;
        
        nSoundID = MakeSystemSoundID("Buzzer");
        if(nSoundID == 0) HLogE("[Error] Sound %s\n","Buzzer");
        pSoundData = new SoundData;
        pSoundData->nSoundID = nSoundID;
        pSoundData->nIntervalTime = 0;
        mSoundMap[SOUND_ID_Buzzer] = pSoundData;
        
        nSoundID = MakeSystemSoundID("EneryUp");
        if(nSoundID == 0) HLogE("[Error] Sound %s\n","EneryUP");
        pSoundData = new SoundData;
        pSoundData->nSoundID = nSoundID;
        pSoundData->nIntervalTime = 0;
        mSoundMap[SOUND_ID_EnergyUP] = pSoundData;

    }
    else
    {
        //K9 Fire
        nSoundID = MakeSystemSoundID("Fire1");
        if(nSoundID == 0) HLogE("[Error] Sound %s\n","Fire1");
        pSoundData = new SoundData;
        pSoundData->nSoundID = nSoundID;
        pSoundData->nIntervalTime = 0;
        mSoundMap[SOUND_ID_K9_FIRE] = pSoundData;
        
        nSoundID = MakeSystemSoundID("NearBombed");
        if(nSoundID == 0) HLogE("[Error] Sound %s\n","NearBombed");
        pSoundData = new SoundData;
        pSoundData->nSoundID = nSoundID;
        pSoundData->nIntervalTime = 300; //0.2초안에 같은것이 드러오면 무시
        pSoundData->ggTime = 0L;
        mSoundMap[SOUND_ID_NEARBOMBED] = pSoundData;

        nSoundID = MakeSystemSoundID("FarBombed");
        if(nSoundID == 0) HLogE("[Error] Sound %s\n","FarBombed");
        pSoundData = new SoundData;
        pSoundData->nSoundID = nSoundID;
        pSoundData->nIntervalTime = 300; //0.2초안에 같은것이 드러오면 무시
        pSoundData->ggTime = 0L;
        mSoundMap[SOUND_ID_FARBOMBED] = pSoundData;
        
        
        nSoundID = MakeSystemSoundID("FireET");
        if(nSoundID == 0) HLogE("[Error] Sound %s\n","FireET");
        pSoundData = new SoundData;
        pSoundData->nSoundID = nSoundID;
        pSoundData->nIntervalTime = 200;
        mSoundMap[SOUND_ID_ET1_FIRE] = pSoundData;

        
        nSoundID = MakeSystemSoundID("EneryUp");
        if(nSoundID == 0) HLogE("[Error] Sound %s\n","EneryUP");
        pSoundData = new SoundData;
        pSoundData->nSoundID = nSoundID;
        pSoundData->nIntervalTime = 0;
        mSoundMap[SOUND_ID_EnergyUP] = pSoundData;
        
        nSoundID = MakeSystemSoundID("Relive");
        if(nSoundID == 0) HLogE("[Error] Sound %s\n","Relive");
        pSoundData = new SoundData;
        pSoundData->nSoundID = nSoundID;
        pSoundData->nIntervalTime = 0;
        mSoundMap[SOUND_ID_Relive] = pSoundData;
        
        nSoundID = MakeSystemSoundID("FLY");
        if(nSoundID == 0) HLogE("[Error] Sound %s\n","Relive");
        pSoundData = new SoundData;
        pSoundData->nSoundID = nSoundID;
        pSoundData->nIntervalTime = 1000; //1초간격
        pSoundData->ggTime = 0L;
        mSoundMap[SOUND_ID_SAW] = pSoundData;
        
        
        
        nSoundID = MakeSystemSoundID("gun");
        if(nSoundID == 0) HLogE("[Error] Sound %s\n","GUN");
        pSoundData = new SoundData;
        pSoundData->nSoundID = nSoundID;
        pSoundData->nIntervalTime = 1500; //1초간격
        pSoundData->ggTime = 0L;
        mSoundMap[SOUND_ID_GUN] = pSoundData;
        
        
        nSoundID = MakeSystemSoundID("raser");
        if(nSoundID == 0) HLogE("[Error] Sound %s\n","GUN");
        pSoundData = new SoundData;
        pSoundData->nSoundID = nSoundID;
        pSoundData->nIntervalTime = 1900; //1초간격
        pSoundData->ggTime = 0L;
        mSoundMap[SOUND_ID_RASER] = pSoundData;

    }
}

void CSoundMan::Close()
{
#ifdef ANDROID
    CloseSystemSoundID(0);
    map<int,SoundData*>::iterator p;
    for (p = mSoundMap.begin(); p != mSoundMap.end(); ++p)
    {
        if(p->second)
        {
            delete p->second;
        }
    }
    mSoundMap.clear();
#else
    map<int,SoundData*>::iterator p;
    for (p = mSoundMap.begin(); p != mSoundMap.end(); ++p)
    {
        //delete p->second;
        if(p->second)
        {
            CloseSystemSoundID(p->second->nSoundID);
            delete p->second;
        }
    }
    mSoundMap.clear();
#endif
}


void CSoundMan::PlaySystemSound(int nID)
{


    unsigned long gTime;
    CUserInfo* ui = CUserInfo::GetDefaultUserInfo();
    if(ui->GetSoundFX() == 1)
    {
        SoundData* pData = mSoundMap[nID];
        if(pData == NULL)
        {
            HLogE("Not Found Sound ID %d\n",nID);
            return ;
            
        }
        if(pData->nIntervalTime != 0)
        {
            gTime = GetGGTime();
            if(gTime > pData->ggTime)
            {
                pData->ggTime = gTime + pData->nIntervalTime;
                PlaySoundByID(pData->nSoundID);
            }
        }
        else
            PlaySoundByID(pData->nSoundID);
    }


}

void CSoundMan::PlaySoundVibrate(int nID)
{


    CUserInfo* ui = CUserInfo::GetDefaultUserInfo();
    if(ui->GetSoundFX() == 1 && ui->GetVibration() == 1)
    {
        PlaySoundAndVibrate(mSoundMap[nID]->nSoundID);
    }
    else if(ui->GetSoundFX() == 1)
    {
        PlaySystemSound(mSoundMap[nID]->nSoundID);
    }
    else if(ui->GetVibration() == 1)
    {
        PlayVibrate();
    }

}

void CSoundMan::PlayVibrate()
{


    CUserInfo* ui = CUserInfo::GetDefaultUserInfo();
    if(ui->GetVibration() == 1)
    {
        unsigned long lT = GetGGTime();
        if(lT > mVibrationTime)
        {
            ::PlayVibrate();
            mVibrationTime = lT + 300;
        }
    }
}


