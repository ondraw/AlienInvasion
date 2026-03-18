/*
 *  sGLDefine.h
 *  SongGL
 *
 *  Created by 호학 송 on 10. 11. 9..
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _SGLDEFINE_H
#define _SGLDEFINE_H

#ifndef NOTOPENGL //PropertyForAndroidTest 에서 정의 한다.(그것은 단순하게 opengl을 검사하는것이 아니다.)

#define ALWAYSAD     1 //항상광고이면 1, 플레이시에 광고를 하지 않으면 0으로 설정하자.

#if defined ANDROID
#include <GLES/gl.h>
#include <GLES/glext.h>
#elif defined IPHONE
#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#else //MAC
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <OpenGL/glu.h>
#endif

#endif //NOTOPENGL

#include "ResID.h"

#define ACCELATOR //중력가속도
//#define PINCHZOOM //핀치줌을 주어 줌인아웃 할 수 있게 한다.

//뷰를 회전할때 회전가속도표현할때는 1 현재는 1 (즉 회전에 대한 랙이 존조해서 헷깔리는것 같다.)
#define VIEWROTATION_ANI 1

//CMS3DModelASCII/CDMainTower/CDTowere의 노벌정점을 계산을 하면 느리고, 계산하지 않으면 빨라진다.
#define CMS3DModelASCII_ORGNORMAL 0 //1:회전, 이동 할때마다 모든 것을 계산 한다. 0:계산하지 않고 읽은 노멀값으로만 사용한다.


#define HALF_PICK   0 //픽킹 검사는 속도가 걸리는 작업이라서 검색량을 반으로 줄여서 검사하자.
#define SHOW_JOISTICK 0 //조이스틱을 보이게 하느냐 마느냐.

#define SGL_SON_VERSION 4 //테리안의 파일 버전. (2버전은 Z축이 높이 이다. 3부터는 Y축이 높이다. 포그정보가 존재한다.)

#ifdef DEBUG
//화면 캪쳐를 할때 1로 설정을 하면 , 적이 폭탄을 안쓰고, 레이다가 사라지고, 스테이터스가 사라지게 한다.
#define CGDisplayMode 0
#if CGDisplayMode == 1
extern int gnCGDiplayMode; //0:일반 1:시간느림 2:총속도느림
#endif
#endif

#ifdef ANDROID
#define MUL_FILTER_ALL          0x00
#else
#define MUL_FILTER_ALL          0xFFFFFFFF
#endif
//즉 AND로는 0, OR로는 0xFFFFFFFF 가 대어야만 대칭이다. 대칭일때만 연결이 된다.
//맵 1만 매칭
#define MUL_FILTER_MAP11         0x1FFFFF0F //맵1이면서 TEAM1
#define MUL_FILTER_MAP12         0xE00000F0 //맵1이면서 TEAM4
//맵 2만
#define MUL_FILTER_MAP21         0x2FFFFF0F //맵2이면서 TEAM1
#define MUL_FILTER_MAP22         0xD00000F0 //맵2이면서 TEAM4
//맵 3만
#define MUL_FILTER_MAP31         0x4FFFFF0F //맵3이면서 TEAM1
#define MUL_FILTER_MAP32         0xB00000F0 //맵3이면서 TEAM4
//맵 4만
#define MUL_FILTER_MAP41         0x8FFFFF0F //맵4이면서 TEAM1
#define MUL_FILTER_MAP42         0x7F0000F0 //맵4이면서 TEAM4
//맵 5만
#define MUL_FILTER_MAP51         0xF1FFFF0F //맵5이면서 TEAM1
#define MUL_FILTER_MAP52         0xFE0000F0 //맵5이면서 TEAM4
//맵 6만
#define MUL_FILTER_MAP61         0xF2FFFF0F //맵6이면서 TEAM1
#define MUL_FILTER_MAP62         0xFD0000F0 //맵6이면서 TEAM4
//맵 7만
#define MUL_FILTER_MAP71         0xF4FFFF0F //맵7이면서 TEAM1
#define MUL_FILTER_MAP72         0xFB0000F0 //맵7이면서 TEAM4
//맵 8만
#define MUL_FILTER_MAP81         0xF8FFFF0F //맵8이면서 TEAM1
#define MUL_FILTER_MAP82         0xF70000F0 //맵8이면서 TEAM4
//맵 9만
#define MUL_FILTER_MAP91         0xFF1FFF0F //맵9이면서 TEAM1
#define MUL_FILTER_MAP92         0xFFE000F0 //맵9이면서 TEAM4













//#define SEND_CUSTOM_USER_INFO //내컴퓨터에 데이터를 올려보내려면
//----------------------------------------------------------------------------------------
//레벨 파워....
//Rank (n:점수 R:랭크)
//n = 500 * R * ( R - 1) ^2 + 300
//Level (레벨을 100으로 본다)
//L = 100/19 * R  = 5.26 R ( 1: 19 = L : 100)
//랭크가 올라가면 5.26파워가 올라간다.
//AP = L * 0.1
//DP=  L * 5 + 50
//L = (AP / 0.1  + (DP-50)/5 ) /2
//01:이병 =          300,	AP=2.630000,	DP=76.300	Level=5
//02:일병 =         1300,	AP=5.260000,	DP=102.600	Level=10
//03:상병 =         6300,	AP=7.890000,	DP=128.900	Level=15
//04:병장 =        18300,	AP=10.520000,	DP=155.200	Level=20
//05:하사 =        40300,	AP=13.150001,	DP=181.500	Level=25
//06:중사 =        75300,	AP=15.780001,	DP=207.800	Level=30
//07:상사 =       126300,	AP=18.410000,	DP=234.100	Level=35
//08:원사 =       196300,	AP=21.040001,	DP=260.400	Level=40
//09:준위 =       288300,	AP=23.670002,	DP=286.700	Level=45
//10:소위 =       405300,	AP=26.300001,	DP=313.000	Level=50
//11:중위 =       550300,	AP=28.930000,	DP=339.300	Level=55
//12:대위 =       726300,	AP=31.560001,	DP=365.600	Level=60
//13:소령 =       936300,	AP=34.190002,	DP=391.900	Level=65
//14:중령 =      1183300,	AP=36.820000,	DP=418.200	Level=70
//15:대령 =      1470300,	AP=39.450001,	DP=444.500	Level=75
//16:소장 =      1800300,	AP=42.080002,	DP=470.800	Level=80
//17:준장 =      2176300,	AP=44.710003,	DP=497.100	Level=85
//18:중장 =      2601300,	AP=47.340004,	DP=523.400	Level=90
//19:대장 =      3078300,	AP=49.970001,	DP=549.700	Level=95

#define DAttackLevelToPower(l)  l * 0.35f
#define DDefenceLevelToPower(l) l * 7.f + 50.f
#define DPowerToLevel(AP,DP)    (AP / 0.1f + (DP - 50.f) / 5.f) / 2.f
#define DRankToLevel(R)         5.26f * R
#define DUnitLevelToTotalLevel(T,R,U)       ((float)T + DRankToLevel(R)) / 2.f + (float)U * 4.5f
//----------------------------------------------------------------------------------------


//폭탄을 밀크세이프로 1: .m3ds로 할것인지 2 : .txt로 할것인지를 경정한다.
#define OBJMilkShape 0


//#define LARGESCREEN //For IPad

//#define SGL_INFO			//디버깅정보를 찍다.
//1: 땅의 이미지를 보이는 만큼 읽어들이고 지운다. (텍스쳐 맵핑을 그때그때 마다하면 느려진다.)
//0: 모든것을 다 읽은후 렌더링한다.
//#define LARGEMAP			0	

//#define NULL 0
#define E_SUCCESS			0
#define E_POS_COMPACT_WALL  -5 //벽에 충돌하였을 경우

//#define MAX_OBJ_SIZE 7.0f 

#define VIEW_APERTURE_WORLD		60.0f //World의 사이각.
#define VIEW_APERTURE           45.0f //Menu 사이각

#define FARVIEW				1000    //하늘을 그리기위해서는 멀리까지 보아야한다이
#define CAMERA_YPOS_2D      100.0f    //2D일 경우 카메라의 가까운 위치...


#define VISIBLEFARVIEW              320.0f
#define VISIBLEFARVIEW_DOUBLE		102400.f

#define RADAVISIBLE         380.0f      //레이더의 Radian
#define RADAVISIBLE_DOUBLE  144400.f    //380*380



#define GROUP_SIZE			32

#define CAMERT_NORMAL		1   //모델의 눈에서 바라 봄
#define CAMERT_BACK         2   //모델의 뒤에서 바라 봄
#define CAMERT_TANKHEADER   4   //탱크의 머리 방향으로 카메라를 돌린다.
#define CAMERT_BACK_UP      3   //뒤쪽 위에서 바라볼때.

#define CAMERT_UP_VISIBLE   100  //하늘위에서 바라볼수 있는 위치만
#define CAMERT_UP           101  //하늘위에서 아래로 전체 내려다 볼때

//0,1,2,3
//0xFFFF
//0x0X00 : 일반캐릭터.(회전0) X(1)는 팀아이디
//0x1X00 : 일반캐릭터.(회전0) X(2)는 팀아이디
//0x2X00 : 일반캐릭터.(회전0) X(4)는 팀아이디
//0x3X00 : 일반캐릭터.(회전0) X(8)는 팀아이디

//0x4000 : 나무,등등 캐릭터(회전0)
//0x5000 : 나무,등등 캐릭터(회전1)
//0x6000 : 나무,등등 캐릭터(회전2)
//0x7000 : 나무,등등 캐릭터(회전3)

//0x8000 : 시작위치.
//0x9000 : 메인타워 시작위치. + 아이디.
//0xA000 : (예약)
//0xB000 : (예약)

#define MAXPOUPANGLE 50.f
#define ACTORTYPE_FIGHTER           5 //Tank Property type 이다.
#define ACTORTYPE_EMAINBLZ          1000 //Tank Property type 이다.
#define ACTORTYPE_TYPE_BLZ             3 //일반빌딩
#define ACTORTYPE_TYPE_GHOST           0x1006
#define ACTORTYPE_TYPE_ITEM            0x1007
#define ACTORTYPE_TYPE_GHOSTANITEM     0x1000




#define ACTORID_K9			1


//어시스트 탱크 아이디는 500이상
#define ACTORID_MISSILE		500         //미사일 발사 탱크
#define ACTORID_ARMEREDCAR  501
#define ACTORID_AMBULANCE   502
#define ACTORID_XTANK       503
#define ACTORID_ASSISTE_END 599         //어시스트탱크 아이디 종료

#define ACTORID_BLZ_DTOWER  0x0900          //지구연합군의 가드타워
#define ACTORID_BLZ_DMISSILETOWER  0x0901   //지구연합군의 가드타워
#define ACTORID_BLZ_MAINTOWER  0x0902   //지구연합군의 메인타워
#define ACTORID_BLZ_DGUNTOWER  0x0903   //지구연합군의 가드타워

//여기 사이에 아이디를 만들자.
#define ACTORID_BLZ_END         0x0999      //지구연합군의 마지막 아이디

#define ACTORID_ET1			0x0A00 //적 탱크는 2560 ~ 0x0E00사이에 있다.
#define ACTORID_ET2			0x0A01
#define ACTORID_ET3			0x0A02
#define ACTORID_ET4			0x0A03
#define ACTORID_ET5			0x0A04
#define ACTORID_ET7			0x0A06
#define ACTORID_ET8			0x0A07
#define ACTORID_ET9			0x0A08
#define ACTORID_ET10		0x0A09

#define ACTORID_EF1         0x0B00 //적 비행기
#define ACTORID_EF2         0x0B01 //적 비행기 단순 레이져
#define ACTORID_EF3         0x0B02 //적 비행기
#define ACTORID_EF4         0x0B03 //적 비행기 두개의 레이져
#define ACTORID_EF5         0x0B04 //적 비행기 4개의 레이져



#define ACTORID_GHOST       0x0E00
#define ACTORID_ARROWMOTION 0x0E01 //Arrow Motion Sprite
#define ACTORID_ARROWMOTION_TRAIN 0x0E01

#define ACTORID_START_POINT 0x8000 //모델은 아니고 시작지점을 표시한다. 0x1000
#define ACTORID_BLZ_EMAIN   0x8001 //적의 메인 타워


#define ACTORID_BACKGROUND  0x9000 //백그라운드



//#define ACTORNAME_K9            "K9Root.ms3d"
//#define ACTORNAME_K9_HEADER     "K9Header.txt"
//#define ACTORNAME_ET1           "ET1Root.ms3d"
//#define ACTORNAME_ET1_HEADER    "ET1Header.txt"
//#define ACTORNAME_ET2           "ET2Root.ms3d"
//#define ACTORNAME_ET3           "ET3Root.ms3d"
//#define ACTORNAME_ET3_HEADER    "ET3Header.txt"
//#define ACTORNAME_ET4           "ET4Root.ms3d"
//#define ACTORNAME_ET4_HEADER    "ET4Header.txt"
//#define ACTORNAME_ET5           "ET5Root.ms3d"
//#define ACTORNAME_ET5_HEADER    "ET5Header.txt"
//#define ACTORNAME_ET7           "ET7Root.ms3d"
//#define ACTORNAME_ET7_HEADER    "ET7Header.txt"
//#define ACTORNAME_ET8           "ET8Root.ms3d"
//#define ACTORNAME_ET8_HEADER    "ET8Header.txt"

//#define ACTORNAME_EF1           "EF1.txt"
//#define ACTORNAME_EF2           "EF2.txt"








#define ACTORNAME_ARROWMOTION   "ArrowMotion.txt"
#define ANINAME_ANIENERGYUP     "AniEnergyUp.txt"
#define ANINAME_BUBBLE          "Bubble.txt"

#define BOMBET_IMG      "NormalBombET1.tga" //적폭탄이미지
#define BOMBETTAIL_IMG  "NormalBombET1Tail.tga" //적폭탄 꼬리
#define BOMBTAIL_IMG    "NormalBombTail.tga"
#define BOMBHE610A      "HE610A.tga"
#define NormalBomb_IMG  "NormalBomb.bmp"
#define THUNDERBOLTSTART_IMB "ThunderBoltStart.tga"
#define RASERBUNSTART_IMG "RaserStart.tga"
#define RASER1_IMG "Raser1.tga"
#define RASER2_IMG "Raser2.tga"
#define RASER3_IMG "Raser3.tga"
#define THUNDERBOLT_IMG "ThunderBolt.tga"
#define MachineCun_IMG "MachineGun.bmp"


//#define BOMB_K9				0
//#define BOMB_ET1			1
#define BOMB_ID_2SHOT       6
#define BOMB_FASET_BOMB_TYPE      0x1000 //0x1001 :Gun , 0x1002 :Raser)

#define ITEMID_RUNTIME_FILLENERGY 5
#define ITEMID_RUNTIME_PLUSBONUS  6
#define ITEMID_RUNTIME_BUBBLE     7
#define ITEMID_RUNTIME_PLUSBONUS2 17
#define ITEMID_RUNTIME_L6        501
#define ITEMID_RUNTIME_L5        502
#define ITEMID_RUNTIME_L4        503
#define ITEMID_RUNTIME_L2        500

#define POWWER_PER_AMAPRANK 15.f //적어도 15배가 넘어야 하지 않을까?
#define POWWER_PER_DMAPRANK 5.f //적어도 5배가 넘어야 하지 않을까?

#define PI 3.1415926535897932384626433832795f


#define H_TERRIAN_DEFAULT_HIGHT     10.0f  //50.0f / 5.0f

//SGLMoveEvent 
#define GSL_MOVE_STOP		0x000080
#define	GSL_MOVE_UP			0x000001
#define	GSL_MOVE_DOWN		0x000002

#define	GSL_MOVE_RIGHT		0x000004
#define	GSL_MOVE_LEFT		0x000008



#define	GSL_TRUN_RIGHT	0x000010
#define	GSL_TRUN_LEFT	0x000020
#define GSL_TRUN_STOP   0x000040



#define GSL_THEADER_TRUNRIGHT	0x000100
#define GSL_THEADER_TRUNLEFT	0x000200
#define GSL_THEADER_STOP		0x000400

#define GSL_BOMB_FIRE			0x001000
#define GSL_DONT_AUTO_FIRE		0x001001


#define GSL_PO_UP				0x002000
#define GSL_PO_DOWN				0x004000
#define GSL_PO_STOP_UPDOWN		0x008000



#define GSL_CHANGE_VIEWTYPE     0x010000



//Progress Event
#define	GSL_PROGRESS_START          0x010001 //시작
#define SGL_PROGRESS_INITIALIZE     0x010002 //데이터 파일을 읽어온다.
#define SGL_PROGRESS_INITIALIZE_1   0x010003 //다음.
#define SGL_PROGRESS_INITIALIZE_2   0x010004 //다음.
#define SGL_PROGRESS_INITIALIZE_3   0x010005 //다음.
#define SGL_PROGRESS_END            0x010F00 //종료


#define SGL_MESSAGE_CLICKED     0x020001 //버튼을 클릭을 하였다.
#define SGL_MESSAGE_DRAG        0x020002 //컨트롤을 드래그한다.

#define SGL_MESSAGE_ROLLSTOP    0x020004 //롤이 멈추었을때
#define SGL_MESSAGE_ROOLSEL     0x020005 //롤을 선택하였을 경우.


#define SGL_MESSAGE_AUTO_ATTACK_TARGET  0x020006 //레이더의 탱크를 클릭하였을 경우.
#define SGL_MESSAGE_DIED_ACTOR          0x020007 //주인공이 죽었다.
#define SGL_MESSAGE_CHANGE_MENU         0x020008 //메인 메뉴로 변경하는 이벤트

#define SGL_MESSAGE_MAKE_SCORE          0x020009 //적군을 죽여서 돈을 멀다.

#define SGL_MESSAGE_MAKE_TANK           0x020010 //메인타워에서 탱크를 만들라는 명령어.
#define SGL_MESSAGE_PROGRESS_STEP       0x020011 //프로그래스바를 다음단계로 옮겨준다.
#define SGL_MESSAGE_PROGRESS_POS        0x020012 //프로그래스바를 특정 위치로 옮겨준다.
#define SGL_MESSAGE_SUCCESS_MISSION     0x020013 //미션에 성공하였을 경우.

#define SGL_MESSAGE_FIRE_MISSILE        0x020014 //미사일을 발사하였다.

#define SGL_MESSAGE_PROMOTION_RANK        0x020015 //지급하는 메세지

#define	GSL_VIEW_MOVEUP             0x020015    //카메라 위치 업
#define	GSL_VIEW_MOVEDOWN           0x020016    //카메라 위치 다운
#define	GSL_VIEW_MOVESTOP           0x020017    //카메라 위치 이동 중지

#define	GSL_VIEW_TURNLEFT           0x020018    //카메라 회전 오른쪽
#define	GSL_VIEW_TURNRIGHT          0x020019    //카메라 위치 다운
#define	GSL_VIEW_TURNSTOP           0x02001A    //카메라 위치 이동 중지

#define SGL_MOVE_TO_POINT_SIMPLE    0x02001B    //단순하게 위치로 직선으로 이동한다.

#define SGL_GET_ITEM                0x02001C    //아이템을 습득하였다.
#define SGL_SHOP_PRODUCT_GOLDTTYPE   0x02001D    //골드를 사는 프러덕을 샀다.

#define SGL_WAIT_TO_LOGO_ANDROID   0x02001E    //Android에서 Wait단계에서 NextLogo로 변환하라는 이벤트
#define SGL_BUY_PRODUCT_GOLDTTYPE_FOR_ANDROID   0x02001F    //골드를 사는 프러덕을 샀다.

#define SGL_DESIGN_PUSH_COMMAND_KEY 0x020020 //
#define SGL_DESIGN_UPPUSH_COMMAND_KEY 0x020021 //
#define SGL_DESIGN_MAKE_TEXTTOIMAGE 0x020022 //글자를 이미지 만들기.
#define SGL_DESIGN_APPLYTOCONTROL_TEXTTOIMAGE 0x020023 //글자를 컨트롤에 맞게 맞춘다.
#define SGL_DESIGN_PREVIEWCONTROL_TEXTTOIMAGE 0x020024
#define SGL_PLAYSOUND 0x020025
#define SGL_CLEAR_TOURCH 0x020026 //터치이벤트를 클리어 해준다.
#define SGL_MAKE_BLZ 0x020027
#define SGL_SELECTED_SPRITE 0x020028 //안드로이드는 터지시 텍스쳐바운딩하면 에러가 나기때문에 메세지를 보내서 처리하자.

#define SGL_REALIVE_RESETSLOT 0x020029 //슬롯을 다시 그려준다. (쓰레드에서 다시그리기가 거시기하자나(

#define SGL_TRAINING_NEXT_END 0x020030 //슬롯을 다시 그려준다. (쓰레드에서 다시그리기가 거시기하자나(

#define SGL_RESULT_REVIEW 0x020031 //리졸트 리뷰가 존재하는지를 검사한다.
#define SGL_COMPLETED_RUNTIME_UPGRADE 0x020032 //

#define SGL_CLICKED_ADMOB_ANDROID 0x020033 //
#define SGL_CHGOLD_IN_DMAINTOWER 0x020034 //Added By Song 2014.05.20 ExcuteCmd() 쓰레드 안쪽에서 실행되면 종종 죽는다.
#define SGL_KILLED_ASSITANCETANK 0x020035 //Added By Song 2014.05.23 ExcuteCmd() 쓰레드 안쪽에서 실행되면 종종 죽는다.
#define SGL_MAKING_ASSISTOBJECTS 0x020036

#define SGL_MOVE_TO_MULTIPLAY               0x020037
#define SGL_CLICK_TO_POINT_MULTIPLAY        0x020038 //상대방이 땅을 클릭했을때
#define SGL_CLICK_TO_SPRITE_MULTIPLAY        0x020039 //상대방이 객체를 클릭했을때
#define SGL_CLICKED_NEW_BOMB_MULTIPLAY        0x020040 //안보이는 영역에서 폭탄을 발사했을 경우.
#define SGL_CHANGE_BOM_SEL_MULTIPLAY          0x020041
#define SGL_COMPLETED_RUNTIME_UPGRADE_MULTIPLAY          0x020042
#define SGL_MAKE_SPRITE_MULTIPLAY          0x020043
#define SGL_MOVE_TO_FIGHTER_MULTIPLAY       0x020044
#define SGL_SYNC_MULTIPLAY                  0x020045 //동기화를 한다.
#define SGL_MATCHING_MSG                    0x020046 //매칭중에 있습니다. 잠시만 기다려 주세요.
#define SGL_SHOW_ENEMYTOOLS                 0x020047 //접속한 상대방의 정보를 보여준다.
#define SGL_ALIANCE_MULTIPLAY               0x020048
#define SGL_DELETE_WAITCURSOR               0x020049


#define SGL_2D_FILENAME_STICK           "Stick.tga"
#define SGL_2D_FILENAME_ENAMY           "EnamyTank.tga"
#define SGL_2D_FILENAME_ENAMY_MAIN      "EMainCenterRaida.tga"
#define SGL_2D_FILENAME_RADAFIRE        "RadaFire.tga"
#define SGL_2D_FILENAME_RADATBODY       "RadaTBody.tga"
#define SGL_2D_FILENAME_RADATHEADER     "RadaTHeader.tga"
#define SGL_2D_FILENAME_RADAPANNEL      "RadaPannel.tga"
#define SGL_3D_FILENAME_ET1BOOSTER1     "TE1Booster1.tga"
#define SGL_3D_FILENAME_ET1BOOSTER2     "TE1Booster2.tga"
#define SGL_3D_FILENAME_ET1BOOSTER3     "TE1Booster3.tga"
#define SGL_3D_FILENAME_FIRETAIL        "FireTail.tga"
#define SGL_3D_FILENAME_FIRETAILET1     "FireTailET1.tga"
#define SGL_2D_FILENAME_TARGET         "Target.png"
#define SGL_2D_FILENAME_ENAMYFIGTHER           "EnamyFighter.tga"

//#define SGL_MAX_POPULATION 16
#define SGL_MAX_POPULATION 11
//#define SGL_MAX_POPULATION 2

//비행기의 고도를 Min~Max사이의 고도를 유지하자.
#define FIGHTER_MIN_HEIGHT 23.f
#define FIGHTER_MAX_HEIGHT 40.f


#define SGL_NEED_EXECUTE_MASK                   0xFFFF0000   //마스크 이외의 값은 데이터.
#define SGL_NEED_EXECUTE_IN_DMAINTOWER          0x00010000   //연합군의 메인타워속으로 드러간다. 즉 에너지 업을 해주어야 한다.
#define SGL_NEED_EXECUTE_CHGOLD_IN_DMAINTOWER   0x00020000   //스코어를 돈으로 변경한다.

//팀은 동맹을 할수 있다. SGL_TEAM1|SGL_TEAM2 (은 둘이 동맹을 하고 있다.)
#define SGL_TEAMALL 0
#define SGL_TEAM1 1
#define SGL_TEAM2 2
#define SGL_TEAM3 4
#define SGL_TEAM4 8


//공중부양
#define ET1_GROUND_HEIGHT 1.7f
//너무 땅에 붙어있으면 울퉁불퉁한 면에 가려지기 때문에 살짝 위로 올려준다.
#define SPRITE_GROUND_HEIGHT 0.9f 



#define SOUND_ID_K9_FIRE 1      //K9폭탄 발사.
#define SOUND_ID_NEARBOMBED 2   //가까운거리에서 폭탄이 터졌다.
#define SOUND_ID_FARBOMBED 3    //먼 거리에서 폭탄이 터졌다.
#define SOUND_ID_ET1_FIRE 4      //ET1폭탄 발사.
#define SOUND_ID_EnergyUP 5      //에너지 업
#define SOUND_ID_Relive 6      //에너지 업
#define SOUND_ID_Click 7        //버튼 클릭
#define SOUND_ID_Buzzer 8       //살수 없는 아이템을 선택했을 때.
#define SOUND_ID_SAW 9       //CET2 공격
#define SOUND_ID_GUN 10       //기관총
#define SOUND_ID_RASER 11       //레이져




#define NONE_VALUE -9999.0f

#define ID_ITEM_ATTACKBYRADA                0
#define ID_ITEM_FILL_POWER                  2
#define ID_ITEM_UPGRADE_ATTACK              3
#define ID_ITEM_UPGRADE_DEPEND              4
#define ID_ITEM_UPGRADE_MAGNETIC            8
#define ID_ITEM_UPGRADE_DEFENCE_TOWER       9
#define ID_ITEM_UPGRADE_MISSILE_TOWER       10
#define ID_ITEM_GOLD_4000                   11 //실제 돈주고 사는 아이템
#define ID_ITEM_GOLD_9000                   12 //실제 돈주고 사는 아이템
#define ID_ITEM_REMOVE_AD                   13 //실제 돈주고 사는 아이템

#define ID_ITEM_UPGRADE_PACKAGE             14 //실제 돈주고 사는 아이템
#define ID_TIEM_FASTBUILD                   15 //실제 돈주고 사는 아이템
#define ID_TIEM_FASTGOLD                    16 //실제 돈주고 사는 아이템



#define WORLD_TYPE_CScenario                0
#define WORLD_TYPE_LOGO                     1
#define WORLD_TYPE_PROGRESS                 2
#define WORLD_TYPE_MENU                     3
#define WORLD_TYPE_TRAINING                 4
#define WORLD_TYPE_H_WORLD                  5
#define WORLD_TYPE_TERRIAN_MAC              6




//-------------------------------------------------------------------------------------
//Protocol
//-------------------------------------------------------------------------------------
#ifdef DEBUG
#define HTTPSERVER_IP           "http://58.181.28.39:8080/AlienInvasion/AlienSvc"
#else
#define HTTPSERVER_IP           "http://hohak.iptime.org:8080/AlienInvasion/AlienSvc"
#endif

#define SPRING_ID               "AlienInvasion"
//-------------------------------------------------------------------------------------



////사용자들이 기본적인 돈이 없어서 게임의 흥미가 없는거 같다. 그래서 처음 실행시 3000골드를 주자.ㅇ
//#define FirstRunGold 4000

#define MAXSLOTCNT 4

//현재 건물을 제거한다. 1000이 넘으면 런타임 업그레이드
#define RUNTIME_UPDATE_START        1000 //검색범위
#define RUNTIME_UPDATE_UPGRADE      1001 //업그레이드
#define RUNTIME_UPDATE_RNGUPGRADE   1002 //범위업그레이드
#define RUNTIME_UPDATE_REPAIRE      1003 //수선
#define RUNTIME_UPDATE_REMOVE       1004 //제거
#define RUNTIME_UPDATE_BUBBLE       1005 //버블
#define RUNTIME_UPDATE_RASER        1006 //레이져
#define RUNTIME_UPDATE_GUNMACHINE   1007 //K9 기간총탑제..
#define RUNTIME_UPDATE_TOWER_POUP   1008 //미사일타워와 가드타워의 포 업그레이드

#define RUNTIME_UPDATE_ARMCAR       1009 //501 //장갑차
#define RUNTIME_UPDATE_AMBU         1010 //502 //앰블런스
#define RUNTIME_UPDATE_TANKCAR      1011 //503 //탱크장갑차
#define RUNTIME_UPDATE_MISSILET     1012 //500 //미사일차


#define RUNTIME_UPDATE_END          1999 //검색 범위



#define GoldPerScore .75f // 10 Number
#define GoldPerScoreZoreRank 0.125f // 10 Number

#define MAKINGTIME 20000 //20초
#define FireAnimationMax 30.f //포사격후 포신이 뒤로 제껴지는 맥스값..
typedef struct {
	GLfloat x,y,z;
} SVector;

typedef struct
{
	GLfloat   m[4][4];
} SMatrix;

typedef struct {
	GLfloat x,y,z;
} SPoint;


typedef struct {
	SPoint	viewPos;				// View position
	SVector viewDir;				// View direction vector
	SVector viewUp;					// View up direction
	SVector rotPoint;				// Point to rotate about
	GLfloat aperture;				// pContextInfo->camera aperture
	GLint viewWidth, viewHeight;	// current window/screen height and width
	
	
	float   ViewNear;				//Projection에서 자동으로 설정된다.
	float   ViewFar;				//원거리 한계점 (그룹 사이즈)
	float   ViewFarOrg;             //각 액터마다 원거리가 틀리다. 일단 초기화시에 설정된 값을 저장해서 나중에 복원할때 사용한다.
	
} SGLCAMERA;

typedef struct
{
	void* pViewContext;
	void* pWorld;
}SGLContext;

typedef struct _SGLTexture
{
	int nWidth;
	int nHeight;
	int nDepth;
	unsigned char* pImageBuffer;
	
    int			   nCoordRef;
	int            nCoordCount;
	GLfloat*       pCoord;
    
    int    nglTextureRef;
	GLuint glTextureID;
}SGLTexture;

typedef struct _SGLResource
{
	int		nVertexCnt;
	GLfloat *pVertex; //정점
		
	int     nNormalCnt;
	GLfloat *pNormal; //세게의 정점으로 이루어진 법선벡터	

	int				 nIndecesRef; //똑같은 정점 그리는 순서의 데이터가 여러개일 필요는 없다 그래서 존재하는 정점의 인덱스를 가져온다.
	unsigned short   *pshIndeces; //점정그리는 순서.
	unsigned short	 shIndicesCnt;
	SGLTexture Texture;

    
    //-------------------------------------------------------------------------
    //추가됨....
    int     nWorldVertexCnt;
    GLfloat *pWorldVertex;
    
    unsigned short sGType; //G Type으로 테리안의 속성 1:일반, 2:건물, 3:건물, 4:기타(나무,가로등)
    
//    MS3D ASCII파일명 (00G_00B.txt)은 불러온다.
//    예) R,B TYPE = 80 , 1 일때 080_001.txt
//    80 , 3 일때 080_001.txt <= 080_003이 아닌 3은 단순한   
//    방향성이기때문에((nBType - 1) / 4) * 4 + 1
//    의 결과 값으로 항상 방향성의 제일 처음 001을 나타낸다.
//    즉 10: (10-1)/4 * 4 + 1 = 9 이다.
//    11: (11-1)/4 * 4 + 1 = 9 이다 즉 integer 계산을 한다.
    unsigned short sBType; //B의 값은 4의 배수 + 1 부터 시작한다 1:0도 2:90도회전 3:180도회전 4:270도회전 의 의미한다.
    float fMaxHeight; //건물 위로 미사일이 날라갈때 건물의 높이보다 낮으면 충돌처리를 한다.
    //-------------------------------------------------------------------------
}SGLResource;


typedef struct _SGLTerrianInfo
{
	int nImageWidth;
	int nImageHeight;
	int nBytePerPixel;
	unsigned char* pImgBuffer;
	//void* plstTerrian;
	int nGroupSize;
}SGLTerrianInfo;


typedef struct _SGLEvent
{
	int nMsgID;
	long  lParam;
	long  lParam2;
    long  lParam3;
    long  lObject;
}SGLEvent;

enum DISPLAYTYPE {
    DISPLAY_IPAD = 1,
    DISPLAY_IPHONE = 2
    };

extern GLfloat gPanelVertexY[];
extern GLfloat gPanelVertexZ[];
extern GLfloat gPanelVertexZR[];
extern unsigned short gPanelIndices[];
extern GLfloat gPanelCoordTex[8];
extern unsigned short gshPanelIndicesCnt;
extern enum DISPLAYTYPE gnDisplayType; //IPhone = 1, IPad = 2




#ifdef __cplusplus
extern "C" {
#endif
	extern void sglLog ( const char *formatStr, ... );
    extern void sglLogE ( const char *formatStr, ... );
    extern char* HttpRequestXmlOrg(char* url,char* post);
    const char* CountryName();
    extern const char* GetUniqueID();
    extern const char* GetBundleID();
    extern const char* GetAppVersion();
    extern const char* GetModel();
    extern bool IsFree();
#ifdef __cplusplus
}
#endif

#ifdef  DEBUG
#define HLog(f,...)      sglLog(f, ##__VA_ARGS__)   //정보용
#define HLogE(f,...)     sglLogE(f, ##__VA_ARGS__)
#define HLogD(f,...)     sglLog(f, ##__VA_ARGS__)   //현재 디버깅용 다사용하면 지운다.
#define HLogA(f,...)                                //안드로이드 전용.
#define HLogN(f,...)     sglLogNet(f, ##__VA_ARGS__)   //멀티플레이 정보
//#define  DEBUG_MSG_FINDPATH //Path 경로를 찍자.
#else
#define HLog(f,...)      
#define HLogE(f,...)    sglLogE(f, ##__VA_ARGS__)
#define HLogD(f,...)
#define HLogA(f,...)    sglLog(f, ##__VA_ARGS__)
#define HLogN(f,...)
#endif


#define PATH_TEST 0
#endif //_SGLDEFINE_H
