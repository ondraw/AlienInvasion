//
//  CMutiplyProtocol.h
//  SongGL
//
//  Created by itsme on 2014. 7. 14..
//
//

#ifndef __SongGL__CMutiplyProtocol__
#define __SongGL__CMutiplyProtocol__
#include <pthread.h>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include "ListNode.h"
using namespace std;

//1:맵 아이디 별로만 매칭 하게 하였다.
//2:사용자가 없어 맵별로 하지 않고 전부 하였다.
#define PROTOCOL_VERSION                        2

#define CMD_START_SYNC                          1       //싱크를 하려고 정보를 보낸다.
#define CMD_CLOCK_SPEED                         2       //평균 클럭 속도.                                                sizeof(float)
#define CMD_START_PLAY                          3       //플레이를 시작한다.
#define CMD_SYNC                                4       //나의 정보를 상대에 보내에 객체를 동기화 해준다.
#define CMD_OBJ_MOVETO                          5       //객체가 움직이는 정보.
#define CMD_OBJ_CLICK_BYSTICK                   6       //땅을 클릭했을 경우.
#define CMD_OBJ_CLICK_SPRITE_BYSTICK            7       //객체를 클릭했을 경우
#define CMD_OBJ_CLICKED_NEW_BOMB                8       //클릭으로 인해 폭탄을 발사할경우.
#define CMD_CHANGE_BOMB                         9       //폭탄을 변경하였다.
#define CMD_COMPLETED_RUNTIMEUPGRADE            10      //런타임 업그레이드가 다 되었을 경우.
#define CMD_STOP_MOVE                           11
#define CMD_MAKE_SPRITE                         12      //가드타워 및 메인타워건설.
#define CMD_MOVEIN_TANK                         13      //적군이 사정거리 안쪽에 왔을 경우
#define CMD_MOVEIN_TANK_BYGUN                   14      //적군이 기관총 사정거리 안쪽에 왔을 경우
#define CMD_DELETE_OBJECT                       15      //죽었다.
#define CMD_OBJ_MOVETO_FIGHTER                  16      //비행기 이동
#define CMD_FIGHTER_SPRITE_SYNC                 17      //비행기 객체 전체 싱크
#define CMD_BUBBLE_SYNC                         18      //버블을 싱크 시킨다.
#define CMD_CURRENT_ENERGY_SYNC                 19      //현재 에너지 시킨다.
#define CMD_REQUEST_ALIANCE                     20      //동맹을 요청한다.
#define CMD_RESPONSE_ALIANCE                    21      //동맹에 응답한다.

//CMD_START_SYNC에 딸려있다.
#define SUBCMD_VERSION                  1
#define SUBCMD_PLYER_STATE              2       //플레이어의 현재 상태 (스타트중, 플레이시작, 플레이중 CSCENARIO_STEP)    sizeof(int)
#define SUBCMD_PLYER_NAME               3       //플레이어의 이름                                                 msAlias.length()
#define SUBCMD_START_CLOCK              4       //시작 시간.                                                    sizeof(unsigned long)
#define SUBCMD_SEL_INDEX_MAP            5       //현재 선택한 맵
#define SUBCMD_OBJ_ALL_INFO             6       //한객체의 모든 정보
#define SUBCMD_PLYER_TEAM               7       //팀정보
#define SUBCMD_ENMEY_MAINTOWER_MAXID    8       //Entower의 맥스 아이디를 동기화 해주어야 같은 객체로 생각한다.
#define SUBCMD_PLYER_RANK               9
#define SUBCMD_ENMEY_MAINTOWER_START_INDEX    10


//CHWorld Thread안쪽에서 명령어
#define CHWORLD_INTHREAD_MULTIPLAY_NEEDALLTANKDATA  1 //동기화해야 할 나의 모든 객체 정보를 보내려고 할때
//#define CHWORLD_INTHREAD_MULTIPLAY_SYNC_TANK        2 //상대의 정보와 내가 가지고 있는 정보를 싱크시켜준다.


typedef enum {
    SYNC_NONE,
    SYNC_FINDPLAYER,    //플레이어를 찾고 있다.
    SYNC_MATCHING,      //매칭중에 있다.
    SYNC_MATCHINGERROR,
    SYNC_PLYERT_STATE,  //나의 플레이어 상태를 보내준다.
    SYNC_START_CLOCK,   //클럭을 보내준다.
    SYNC_READYPLAY,     //자신의 팀아이디 등등을 보내준다.
    SYNC_PLAYING        //플레이중
} MultiplyState;

#pragma pack(push,1) //데이터를 구조체의 크기가 아니라 char[] 형태로 줄줄이 사탕으로 보내버리자.
typedef struct
{
    char            sStart[4]; //!@#$로 시작해야 한다.
    unsigned char   cCommand;
    unsigned char   cSubCnt;
    unsigned short  nDataSize;
    char*           pData;
    bool            bReliable;
}MutiplyProtocol;

typedef struct
{
    unsigned char cSubCommand;
    unsigned short nDataSize;
    char*         pData;
}MutiplyProtocol_Sub;
#pragma pack(pop)

class CArchive;
class CScenario;
class IHWorld;
class CMutiplyProtocol
{
    friend void* RcvMultiplayProtocolThread(void *data);
    friend void* MultiplayProtocolThread(void *data);
public:
    CMutiplyProtocol(IHWorld* pWorld);
    virtual ~CMutiplyProtocol();
    
    void Start();
    void Stop();
    
    void ClearData();
    void SendData();

    bool IsDelete() { return mbDelete;}
    bool IsStop() { return mbStop;}
    void SendSyncNoPlay();
    void RcvSync();
    void SetAlias(const char* sAlias) {msAlias = sAlias;}
    void ReceiveData(const char* sPlayerID,int nSize,char* sData);
    void SetMultiplayState(MultiplyState v) {mState = v;}
    MultiplyState GetMultiplayState() { return mState;}
    void RcvSyncEvent();
    void SndSyncEvent();

   
    int  GetMyDeviceActorTeamID() { return mnTeamID;}
    
    //CHWorld의 쓰레드 안쪽에서 멀티플레이어에서 필요한 정보가 존재하면 행해준다.
    unsigned int GetNeedCommand() { return mnNeedCommand;}
    void SetNeedCommand(unsigned int v) { mnNeedCommand = v;}
    
    // Send Cmd
    //플레이를 시작한다.
    void SendSyncStartPlay();
    //나의 모든 탱크를 상대에 보내어 싱크해준다.
    void SendSyncObjAll(CListNode<CArchive>* pList);
    //사용자가 어리도 이동하라는 액션을 보낸다.
    void SendSyncMove(int nID,bool bSimpleMode,SPoint* ptTo,bool bFront);
    //비행기를 이동할때
    void SendSyncMoveFighter(int nID,int nToID);
    //사용자가 땅을 클릭했을 경우
    void SendSyncClickToByStick(int pOwnerID,SPoint* ptTo);
    //사용자가 객체를 클릭했을 경우
    void SendSyncClickSpriteToByStick(int pOwnerID,int nID);
    //사용자가 클릭했을때 발사하는 폭탄은 상대방은 안보일때 발사한다.
    void SendSyncClickedNewBomb(int pOwnerID,SPoint* ptNow,SVector* vtDir,SVector* pDirAngle);
    //사용자가 폭탄을 변경하였다.
    void SendSyncChandeBomb(int OwnerID,int BombID);
    //런타임 업그레이드를 완료했을 경우 이벤트
    void SendSyncCompletedRuntimeUpgrade(CArchive* pArc);
    //움직임을 멈춘다.
    void SendSyncStopMove(int OwnerID);
    //가드타워 건설
    void SendSyncMakeSprite(CSprite* pNewSprite);
    
    //상대방과 공격지능에 대하여 동기화를 하자. (포가 하나인경우)nIndex = -1, other 포가 여러개인경우 인덱스.
    void SendSyncMoveInAI(int nOwnerID,int nIndex,int nEnID,float fDistance);
    void SendSyncMoveInGunAI(int nOwnerID,int nEnGunID);
    //객체가 죽었다. WhosKilledID: 누구에 의해서 죽었다. bVisible:죽을당시 보이는 곳에서 아니면 안보이는 곳에서
    void SendSyncDeleteObj(int nID,int WhosKilledID,bool bVisible);
    //한 객체를 싱크 시켜준다.
    void SendSyncFighterSprite(CSprite* pSprite);
    
    void SendSyncBubble(CSprite* pSprite);
    void SendSyncCurrentEnergy(CSprite* pSprite);
    
    bool SendRequestAliance(bool bAliance); //동맹요청/ 비동맹요청
    void SendResponseAliance(bool bAlianceYESNO); //응답 동맹요청에 대한 Yes/No
    
    unsigned long GetFindWaitTime() { return mnFindWaitTime;}
    void SetFindWaitTime(unsigned long v) {
        mnFindWaitTime = v;
    }
    
    unsigned int GetEnemyMainTowerMaxID() { return mnMulEnemyMainTowerMaxID;}
//    void SetEnemyMainTowerMaxID(unsigned int v) { mnMulEnemyMainTowerMaxID = v;}
    
    unsigned int GetEnemyMainTowerStartIndex() { return mnMulEnemyMainTowerStartIndex;}
//    void SetEnemyMainTowerStartIndex(unsigned int v) { mnMulEnemyMainTowerStartIndex = v;}
    
    
    unsigned int GetFindMatchingGroup();
    
    void SetEnemyStartPlaying(bool v) { mbEnemyStartPlaying = v;}
    const char* GetAliasEnemy() { return msAliasEnemy.c_str();}
    int GetRankEnemy() { return mnEnemyRank;}
    
    
    bool IsReauestingAliance() { return mbReauestingAliance;} //동맹요청중인지
    void ResetAliance(); //새플레이면 현제 동맹관련 내용을 리셋해준다.
    
    int GetEnemyTeamID() { return mnEnemyTeamID;}
    int GetTeamID() { return mnTeamID;}
    
    
    
protected:
    //1> 처음에 클럭을 여러번 보낸다. (그 다음 빠른 것을 기준으로 동기화를 해주자.)
    void PushData(MutiplyProtocol* pNew);
    void PushRcvData(MutiplyProtocol* pNew);
    int  SetMutiplyProtocol_Sub(MutiplyProtocol_Sub* pSub,unsigned char cCmd,int nDataSize,char* pData);
    void SetMutiplyProtocol(MutiplyProtocol* pPro,unsigned char cCmd,int nSubSize,int nDataSize,char* pData);//,bool bReliable = true); UDP를 사용하지 말자. 받을때 로직이 잘못되었다.
    void SetRcvMutiplyProtocol(MutiplyProtocol* pPro,unsigned char cCmd,int nSubSize,int nDataSize,char* pData);
    void SetRcvMutiplyProtocol_Sub(MutiplyProtocol_Sub* pSub,unsigned char cCmd,int nDataSize,char* pData);
    
protected:
    vector<MutiplyProtocol*> mBuffer;
    vector<MutiplyProtocol*> mRcvBuffer;
    
    pthread_mutex_t          mBufferMutex;
    pthread_mutex_t          mRcvBufferMutex;
    
   
    //event
    pthread_mutex_t          mRcvMutex;
    pthread_cond_t           mRcvSyncCond;
    
    pthread_mutex_t          mSndMutex;
    pthread_cond_t           mSndSyncCond;
    
    pthread_t                mThreadMainID;
    pthread_t                mThreadRcvMainID;
    
    bool                     mbStop;
    MultiplyState            mState; //MultiplyState
    CScenario*               mpWorld;
    string                   msAlias;
    string                   msAliasEnemy;
    int                      mnEnemyRank;
    
    
//    int                     mnMyLastState; //enum CSCENARIO_STEP
    unsigned long           mnMyLastTime;  //StartTime
    unsigned long           mnMyLastTimeEnd; //약 1초동안
    float                   mfMyEscepAv;
    
    unsigned long           mnEnemyLastTime;  //StartTime
    
    char*                   mRcvData;
    int                     mRcvSize;
    int                     mAssignedSize;
    bool                    mbDelete;
    
    float                   mfDifSpeed; //상대단말기와 나와의 클럭의 평균치(+빠르다,-느리다)
    int                     mnTeamID;
    int                     mnEnemyTeamID;
    
    bool                    mbEnemyStartPlaying;
    bool                    mbMyStartPlaying;
    
    
    unsigned int            mnNeedCommand; //CHWorld의 쓰레드 안쪽에서 멀티플레이어에서 필요한 정보가 존재하면 행해준다.
//    map<int, CArchive*>     *mpSyncObjData; //CHWORLD_INTHREAD_MULTIPLAY_NEEDALLTANKDATA
    
    unsigned long           mnFindWaitTime; //찾는시간이 너무 오래 걸리지 않게 사용하자.
    unsigned int            mnMulEnemyMainTowerMaxID;
    int                     mnMulEnemyMainTowerStartIndex;
    
    
    short                   msCntAliance;   //동맹을 너무 자주 요청하지 못하게 막는다.
    bool                    mbReauestingAliance;//동맹요청중
};
#endif /* defined(__SongGL__CMutiplyProtocol__) */
