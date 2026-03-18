//
//  AlienInvasionProtocolMan.h
//  SongGL
//
//  Created by 호학 송 on 13. 10. 14..
//
//

#ifndef __SongGL__AlienInvasionProtocolMan__
#define __SongGL__AlienInvasionProtocolMan__
#include <iostream>
#include "CUserInfo.h"


#define COMMAND_UPDATEUSER  "UpdateUser"
#define COMMAND_UPDATESTEP      "UpdateStep"
class AlienInvasionProtocolMan
{
public:
    AlienInvasionProtocolMan(CUserInfo* pUserInfo);
    ~AlienInvasionProtocolMan();
    
    //UserInfo의 정보를 서버에 올려준다.
    void SendUserInfo();
    void SendTrainningStep(int nStep);
#ifdef SEND_CUSTOM_USER_INFO
    CUserInfo* GetUserInfo() { return mpUserInfo;}
    void SetTheadEnded() { mThreadMainID = NULL;}
    
    void AlienInvasionProtocolThread_SendUserInfo();
    void AlienInvasionProtocolThread_SendTrainningStep(int nStep);
    
    int mnType; //1:SendUserInfo 2:Trainning
    int mnTrainningStep;
    
    
protected:
    
    pthread_t mThreadMainID;
#endif //SEND_CUSTOM_USER_INFO
    CUserInfo* mpUserInfo;
    
};
#endif /* defined(__SongGL__AlienInvasionProtocolMan__) */
