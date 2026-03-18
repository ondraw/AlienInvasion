//
//  sGLSound.h
//  SongGL
//
//  Created by Songs on 11. 9. 1..
//  Copyright 2011 thinkm. All rights reserved.
//

#ifndef SGLSOUND_H
#define SGLSOUND_H

extern "C" {
//시스템 사운드의 아이디를 만든다.
int MakeSystemSoundID(const char* sName);
//시스템 사운드를 플레이 한다.
void PlaySoundByID(int nSoundID);
//시스템 사운드,진동 플레이 한다.
void PlaySoundAndVibrate(int nSoundID);
//진동을 플레이 한다.
void PlayVibrate();
//사운드를 해제 한다.
void CloseSystemSoundID(int nSoundID);

//배경음악
void PlayBgSound(const char* sName);
void SetBgVolume(float fVolume);
void StopBgSound();
float GetVolume();
};

#endif //SGLSOUND_H