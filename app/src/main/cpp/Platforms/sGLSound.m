//
//  sGLSound.m
//  SongGL
//
//  Created by Songs on 11. 9. 1..
//  Copyright 2011 thinkm. All rights reserved.
//

#include "sGLSound.h"
#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h>
#if defined (IPHONE)
#import <UIKit/UIKit.h>
#endif
#include <stdlib.h>
#import <AVFoundation/AVFoundation.h>
#include "CUserInfo.h"
AVAudioPlayer* gpPlayer = nil;

int MakeSystemSoundID(const char* sName)
{
    SystemSoundID idSound = 0;
    @try
    {
        
//#if defined(IPHONE) || defined(MAKE_VIDEO)
    NSString* nsName = [[NSString alloc]
                        initWithUTF8String:sName];
    
    
    CFURLRef clafURL = (CFURLRef) [NSURL fileURLWithPath:[[NSBundle mainBundle] pathForResource:nsName ofType:@"caf"]];
    AudioServicesCreateSystemSoundID(clafURL, &idSound);
    
    /*
     오디오 설정을 무시하고 항상 소리가 나게 하려면
     AudioServicesPropertyID flag = 0; // 0은 항상 재생을 의미한다.
     AudioServicesSetProperty(kAudioServicesPropertyIsUISound,sizeof(SystemSoundID),&nSoundID,sizeof(AudioServicesPropertyID),&flag);
    */
    
    [nsName release];
    }
    @catch (NSException* e)
    {
        
    }
//#endif
    return idSound;
}

void PlaySoundByID(int nSoundID)
{
//#if defined(IPHONE) || defined(MAKE_VIDEO)
    AudioServicesPlaySystemSound((SystemSoundID)nSoundID);
//#endif
}

void PlayVibrate()
{
#ifndef MAC
#if defined (IPHONE)
    AudioServicesPlaySystemSound(kSystemSoundID_Vibrate);
#endif//
#endif
}

void PlaySoundAndVibrate(int nSoundID)
{
    AudioServicesPlayAlertSound((SystemSoundID)nSoundID);
}

void CloseSystemSoundID(int nSoundID)
{
//#if defined(IPHONE) || defined(MAKE_VIDEO)
    AudioServicesDisposeSystemSoundID (nSoundID);
//#endif
}


void PlayBgSound(const char* sName)
{
//#if defined(IPHONE) || defined(MAKE_VIDEO)
    StopBgSound();
    CUserInfo* ui = CUserInfo::GetDefaultUserInfo();
    if(ui->GetMusic() == 1)
    {
        NSString* sP = [NSString stringWithCString:sName encoding:NSUTF8StringEncoding];
        sP = [[NSBundle mainBundle] pathForResource:sP ofType:@"mp3"];
        gpPlayer = [[AVAudioPlayer alloc]
                    initWithContentsOfURL:
                    [NSURL URLWithString:sP] error:nil];
    //    gpPlayer.delegate = self;
        gpPlayer.volume = 0.5f;
        gpPlayer.numberOfLoops = -1; //-1:무한 0:한번
        [gpPlayer prepareToPlay];
        [gpPlayer play];
    }
//#endif
}

void SetBgVolume(float fVolume)
{
    if(gpPlayer == nil) return;
    [gpPlayer setVolume:fVolume];
}

void StopBgSound()
{
    if(gpPlayer)
    {
        [gpPlayer stop];
        [gpPlayer release];
        gpPlayer = nil;
    }
    
}

float GetVolume()
{
    //현재 볼륨갑 가져오기.
    Float32 deviceVolume;
#if defined (IPHONE)
    
    if ([[[UIDevice currentDevice] systemVersion] floatValue] < 7.f)
    {
        UInt32 size = sizeof(deviceVolume);
        AudioSessionGetProperty(kAudioSessionProperty_CurrentHardwareOutputVolume, &size, &deviceVolume);
    }
    else
        deviceVolume = [[AVAudioSession sharedInstance] outputVolume];
    
#endif
    return deviceVolume;
}