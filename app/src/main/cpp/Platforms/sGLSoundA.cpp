//
//  sGLSound.m
//  SongGL
//
//  Created by Songs on 11. 9. 1..
//  Copyright 2011 thinkm. All rights reserved.
//

#include "sGLSound.h"
#include <stdlib.h>
#include "CUserInfo.h"
#include "SongGLJNI.h"

int MakeSystemSoundID(const char* sName)
{
    return sglMakeSystemSoundID(sName);
}

void PlaySoundByID(int nSoundID)
{
    sglPlaySoundByID(nSoundID);
}

void PlayVibrate()
{
    sglPlayVibrate();
}

void PlaySoundAndVibrate(int nSoundID)
{
}

void CloseSystemSoundID(int nSoundID)
{
    sglCloseSystemSoundID();
}


void PlayBgSound(const char* sName)
{
    StopBgSound();
    CUserInfo* ui = CUserInfo::GetDefaultUserInfo();
    if(ui->GetMusic() == 1)
    {
        sglPlayBgSound(sName);
    }
}

void SetBgVolume(float fVolume)
{
}

void StopBgSound()
{
    sglStopBgSound();
}

float GetVolume()
{
    return 0.0f;
}