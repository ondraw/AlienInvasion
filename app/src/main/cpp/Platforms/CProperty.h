//
//  CProperty.h
//  SongGL
//
//  Created by Song Hohak on 12. 1. 31..
//  Copyright (c) 2012 thinkm. All rights reserved.
//

#ifndef SongGL_CProperty_h
#define SongGL_CProperty_h
#include <vector>
#include <map>
#include <string>
using namespace std;


class PROPERTYI
{
public:
    virtual ~PROPERTYI(){};
    static void* gProperty;
    static void ReleaseList(vector<PROPERTYI*>* plist);
    static void ReleaseProperty();
    static vector<float>* GetPropertyFloatList(const char* sKey,vector<float>& list);
};

class PROPERTY_MAP : public PROPERTYI
{
public:
    PROPERTY_MAP();
    virtual ~PROPERTY_MAP();
    PROPERTY_MAP& operator=(const PROPERTY_MAP& SrcProperty);
    static vector<PROPERTY_MAP*>* GetPropertyMapList(vector<PROPERTY_MAP*>& list);
    static PROPERTY_MAP* GetPropertyMap(int nID, PROPERTY_MAP& property);
    
    void SetName(const char* sInName);
    void SetDesc(const char* sInDesc);
    void SetModelPath(const char* sInModelPath);
    
    int             nID;                //0:    아이디. 0:훈련소
    int             nType;              //1:    아직 사용하지 않음.
    char*           sName;              //2:    이름.
    char*           sDesc;              //3:    설명
    unsigned int    nMinLevel;          //4:    최소레벨
    char*           sModelPath;         //5:    메뉴이미지파일:NormalItemMenu.png
};

#define ITEM_AUTOTARGET 0               //Auto Target 
#define ITEM_LARGETARGET 1              //Large Target Raida  
#define ITEM_FILLDEFENCEPW 2            //Fill the defence pw. 
#define ITEM_UPAPW 3                    //Upgrade Attack Power 
#define ITEM_UPDPW 4                    //Upgrade Defence Power
#define ITEM_OPVIEWER 5                 //O.P Viewer
class PROPERTY_ITEM : public PROPERTYI
{
public:
    PROPERTY_ITEM();
    virtual ~PROPERTY_ITEM();
    PROPERTY_ITEM& operator=(const PROPERTY_ITEM& SrcProperty);
    static vector<PROPERTY_ITEM*>* GetPropertyItemList(vector<PROPERTY_ITEM*>& list);
    static PROPERTY_ITEM* GetPropertyItem(int nID, PROPERTY_ITEM& property);
    static PROPERTY_ITEM* GetPropertyItem(const char* sID, PROPERTY_ITEM& property);
    
    void SetName(const char* sInName);
    void SetDesc(const char* sInDesc);
    void SetDesc2(const char* sInDesc);
    void SetModelPath(const char* sInModelPath);
    void SetID(const char* sInID);
    
    int             nID;                //0:    아이디 ntype:6 이면, 탱크아이디와 동일함.(탱크 900부터시작)
    int             nType;              //1:    0: Count 1:영구사용 2:(업그레이드 같은 것) 3:런타임시만 4:달러 Gold 5:달러 RemoveAd같은 속성 6:아이템탱크
    char*           sName;              //2:    이름.
    char*           sDesc;              //3:    설명.
    char*           sDesc2;             //4:    설명. 두번째 라인..
    unsigned int    nMinLevel;          //5:    최소레벨
    char*           sModelPath;         //6:    메뉴이미지파일:NormalItemMenu.png
    unsigned int    nPrice;             //7     살수있는 가격
    char*           sID;                //8:    인앱 번들아이디.
};

class PROPERTY_RUNTIMEITEM : public PROPERTYI
{
public:
    PROPERTY_RUNTIMEITEM();
    virtual ~PROPERTY_RUNTIMEITEM();
    PROPERTY_RUNTIMEITEM& operator=(const PROPERTY_RUNTIMEITEM& SrcProperty);
    static vector<PROPERTY_RUNTIMEITEM*>* GetPropertyRunTimeItemList(vector<PROPERTY_RUNTIMEITEM*>& list);
    static PROPERTY_RUNTIMEITEM* GetPropertyRunTimeItem(int nID, PROPERTY_RUNTIMEITEM& property);
    void SetModelPath(const char* sInModelPath);
    
    int             nID;            //0: 사용유무 1:사용 0:미사용
    int             nType;          //1: 0:Bomb 1:Item 3:RunTimeItem 4:실제상점(달러)
    int             nItemID;        //2: 실제 아이디
    char*           sModelPath;     //3; 모델 경로
    int             nCount;         //4: 아이템을 먹었을 경우 한번에 올라가는 숫자.
    int             nOther;         //5: 예비.
};

class PROPERTY_BOMB : public PROPERTYI
{
public:
    PROPERTY_BOMB();
    virtual ~PROPERTY_BOMB();
    PROPERTY_BOMB& operator=(const PROPERTY_BOMB& SrcProperty);
    static vector<PROPERTY_BOMB*>* GetPropertyBombList(vector<PROPERTY_BOMB*>& list);
    static PROPERTY_BOMB* GetPropertyBomb(int nID, PROPERTY_BOMB& property);
    void SetName(const char* sInName);
    void SetDesc(const char* sInDesc);
    void SetModelPath(const char* sInModelPath);
    void SetTableImgPath(const char* sInBombTableImg);
    void SetBombBombImgPath(const char* sInBombBombImgPath);
    void SetBombTailImgPath(const char* sInBombTailImgPath);
    
    int             nID;                    //0:  아이디. 1:고폭탄 2:A26 3:ET1탄.
    int             nType;                  //1:  0:일반 1:영구사용
    char*           sName;                  //2:  이름.
    char*           sDesc;                  //3:  설명
    unsigned int    nBombType;              //4:  0:일반폭탄(유도탄,고폭탄) 1:미사일  0x1000(4096) : 4097(0x1001 Gun) 4098(0x1002 Raser)
    char*           sModelPath;             //5:  모델명 NormalBomb => 모델:NormalBomb.ms3d 부스트:NormalBombTail.tga, 메뉴이미지파일:NormalBomb.tga
    char*           sBombTableImgPath;      //6:  테이블 이미지.
    float           fMaxAttackPower;        //7:  공격력
    float           fMaxRadianDetect;       //8   반경 피해 범위
    float           fScale;                 //9   크기
    float           fVelocity;              //10  초기발사 속도
    float           fAutoDectect;           //11  유도 발사 범위.(미사일 같은 자동으로 목표를 찾아가는 유도탄) ex)0.5
    unsigned int    nPrice;                 //12  살수있는 가격
    int             nMenuVisible;           //13  메뉴에서 보이게 하는지 말지 0:안보이게 1:보이게
    float           fStartPosition;         //14
    
    int             nBombTailMaxAliveTime;  //15 (Raser:레이져가 살아 있는 시간)
    float           fBombTailScale;         //16
    
    unsigned int    nSoundFilreID;          //17  발사할때 사운드 아이디.
    unsigned int    nSoundCompactID;        //18  가까운 거리에서 터졌을때 사운드 아이디,
    unsigned int    nSoundFarCompactID;     //19  먼거리에서 터졌을때 사운드 아이디,
    unsigned int    nPackCount;             //20. 한번에 살수있는 단위
    
    unsigned char   cDivide;                //21. 몇개로 쪼개지는가 (일반 : 0, 분산탄 : > 1) 
    int             nDividedID;             //22. 분산될때 분산된 폭탄의 아이디. (분산된 탄도 유도탄이 되려면 본체가 유도탄이여야 한다.)           
    unsigned char   cRotation;              //23. 폭탄이 회전을 하는가... (일반 : 0, 1:나서회전 )
    char*           sBombBombImgPath;       //24  폭탄이 터졌을 때 이미지
    char*           sBombTailImgPath;       //25  폭탄 꼬리 이미지. (레져의 텍스쳐아이디로 사용한다.)
    
    float           fFirePerSec;            //26:   ( 탱크의 초당 발사 속도 1초당 3 발이면 = (1000/3) / 폭탄의 fFirePerSec )
};


class PROPERTY_TANK : public PROPERTYI
{
public:
    PROPERTY_TANK();
    virtual ~PROPERTY_TANK();
    
    void SetName(const char* sInName);
    void SetDesc(const char* sInDesc);
    void SetModelPath(const char* sInModelPath);
    void SetEnemyBoosterPath(const char* sInEnemyBoosterPath);
    void SetMisailStartPos(const char* sInMisailStartPos);
    
    PROPERTY_TANK& operator=(const PROPERTY_TANK& SrcProperty);
    static PROPERTY_TANK* GetPropertyTank(int nID, PROPERTY_TANK& property);
    static vector<PROPERTY_TANK*>* GetPropertyTankList(vector<PROPERTY_TANK*>& list);
    
    int             nID;                //0:    아이디. 1:K9 2:A26 3:ET1
    int             nType;              //1:    1:K9시리즈 2:ET1시리즈(날라다니는) 3:빌딩 5:Fighter 6:GhostSprite
    int             nEnemy;             //2:    0: Actor 1:Enemy
    char*           sName;              //3:
    char*           sDesc;              //4:
    unsigned int    nMinLevel;          //5:    Rank로 사용한다.
    
    char*           sModelPath;         //6:    모델경울 K9 =>Body=K9Root.ms3d  Header=K9Header.ms3d K9.png 
    char*           sEnemyBoosterPath;  //7:    적일때는 적의 날라다니는 이미지 경로
    unsigned int    nFirePerSec;        //8:    초당 발사 속도 1초당 3 발이면 = (1000/3)
    unsigned int    nMaxDefendPower;    //9:    방어력
    
    float           fRotationRate;      //10:   해더의 회전속도 0.1 (비행기의 회전 각도)
    float           fMoveRate;          //11:   속도.0.015f
    
    float           fHalfX;            //12:    탱크의 넓이 / 2 K9:63.0 (비행기,메인타워 라디언)
    float           fHalfZ;            //13:    탱크의 넓이 / 2 K9:90.0
    
    float           fHeaderPosX;        //14:   K9 : 0
    float           fHeaderPosY;        //15:   K9 : 48 //몸체에서 (0,48) 뒤의 위치에 있다. 2A6 : 15
    float           fHeaderPosZ;        //16:   K9 : 0
    
    float           fScale;             //17:   크기 K9 : 0.06
    float           fHeaderTurnSize;    //18:   0.6
    float           fFireAnimationAcc;  //19:   0.25  탱크의 파이어시 해더의 움직임 속도 조절
    float           fMaxScore;          //20:   탱크의 점수
    
    int             nPoStep;            //21:   포의 애니메이션 단계 48
    int             nDefaultBombID;     //22:   //Default 미사일 아이디.;
    char*           sMisailStartPos;    //23:   미사일`시작 위치 UP(a,b,c)/Down(a,b,c) 샘플:CET3 (DevGetPoIndex)
                                        // 예..nGroup,nTrangleIndex,nVertexIndex;nGroup,nTrangleIndex,nVertexIndex;
                                        // 비행기예 0,4,2;1,4,2;2,4,2*1,4,2;2,4,2 => 부스터*미사일포위치*총위치 (보통 앞/뒤 순서다)
    float           fDefendRadianBounds;//24:   이범위 안쪽에 드러오면 자동으로 방어를 한다. Default:279.0f 레이다범위는 (RADAVISIBLE=380.0f,VISIBLEFARVIEW=280)
    
    unsigned int    nMaxAttackPower;    //25: 기본적인 공격력 100%단위.
    int             nGold;              //26: 포의 가격
    int             nGunID;             //27: 건의 아이디.
};


class RUNTIME_UPGRADE : public PROPERTYI
{
public:
    RUNTIME_UPGRADE();
    virtual ~RUNTIME_UPGRADE();
    RUNTIME_UPGRADE& operator=(const RUNTIME_UPGRADE& SrcProperty);
    static RUNTIME_UPGRADE* GetPropertyRunUpgradeItem(int nID, RUNTIME_UPGRADE& property);
//    static map<int,RUNTIME_UPGRADE*>* GetPropertyRunUpgradeItemList(map<int,RUNTIME_UPGRADE*>& list);
//    void ClosePropertyRunUpgradeItemList(map<int,RUNTIME_UPGRADE*>& list);
#ifdef ANDROID
    static void InitTextureRunUpgradeItemList(vector<string> *sOut);
#endif
    void SetName(const char* v);
    void SetDesc(const char* v);
    void SetPath(const char* v);
    
    int nID; //1:업그레이드 3:범위업그레이드 2:수선 3:파계
    int nTime; //업그레이드에 걸리는 시간
    int nUpgradeMaxCnt;
    char* sName;
    char* sDesc;
    char* sPath; //icon
    int nGold;
    
    //RunTime시...
    int nUpgradeCnt;
    
    long nUpgreadTime;
};

class RUNTIME_UPGRADES : public PROPERTYI
{
public:
    RUNTIME_UPGRADES();
    virtual ~RUNTIME_UPGRADES();
    RUNTIME_UPGRADES& operator=(const RUNTIME_UPGRADES& SrcProperty);
    static RUNTIME_UPGRADES* GetPropertyRunUpgradesItem(int nID, RUNTIME_UPGRADES& property);
    int nID; //Sprite ID
    int nCount;
    vector<int> nUpgradeIDs; //1:업그레이드 3:범위업그레이드 2:수선 3:파계
};

class SGLTextUnit
{
public:
    SGLTextUnit();
    ~SGLTextUnit();

    //Width 480 (IPhone)
    //Width 960 (IPhone Retina)
    static void Initialize();
    static void Release();
    static const char* GetText(const char* sID);
    static const char* GetOnlyText(const char* sID);
    static const char* GetTextAddGLData(const char* sText,const char* sID);
};

#endif
