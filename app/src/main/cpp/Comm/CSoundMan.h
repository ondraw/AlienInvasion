//
//  CSoundMan.h
//  SongGL
//
//  Created by Songs on 11. 9. 1..
//  Copyright 2011 thinkm. All rights reserved.
//
#ifndef _CSOUNDMAN_H
#define _CSOUNDMAN_H

#include <map>
using namespace std;

typedef struct _SoundData{
    int nSoundID;
    int nIntervalTime; //0:이면 사운드가 조건없이 여러번 플레이할수 있게, !0 이면 이사이에 다른 사운드를 플레이 못하게 무시.
    unsigned long ggTime; //
}SoundData;

class CSoundMan
{
public:
    CSoundMan();
    ~CSoundMan();
    
    //nType = 0 Menu , 1 RealGame
    void Initialize(int nType);
    void Close();
    
    void PlaySystemSound(int nID);
    void PlaySoundVibrate(int nID);
    void PlayVibrate();
    
protected:
    map<int, SoundData*> mSoundMap;
    unsigned long mVibrationTime; //진동이 너무 자주 이러나면 거시기 하다.    
};


#endif //_CSOUNDMAN_H