//
//  CTrainingWorld.h
//  SongGL
//
//  Created by 호학 송 on 13. 5. 27..
//
//

#ifndef __SongGL__CTrainingWorld__
#define __SongGL__CTrainingWorld__

#include "CHWorld.h"
#define New_CTrainingWorld //새로운 훈련소 모드...
#define BTN_TRAINING_STEP1_OK       1101


#ifdef New_CTrainingWorld
enum TRAININGSTEP
{
    TRAININGSTEP_NONE = 0,
    TRAININGSTEP_INTRODUCE,
    TRAININGSTEP_MOVE,
    TRAININGSTEP_ATTACK,
    TRAININGSTEP_ATTACKRADIA,
    TRAININGSTEP_CHANGE_SLOT, //아이템 슬롯을 변경한다.
    TRAININGSTEP_MAKETOWER,
    TRAININGSTEP_UPGRADE,
    TRAININGSTEP_MISSION,
    TRAININGSTEP_END0,
    TRAININGSTEP_END
    
};


class CArrowMotionSprite;
class CLabelCtl;
class CScenario;
class CTrainingWorld : public CHWorld {
public:
    CTrainingWorld(CScenario* pScenario);
    virtual ~CTrainingWorld();
    virtual int GetWorldType() { return WORLD_TYPE_TRAINING;}
    virtual int Initialize2(SGLEvent *pEvent);
    virtual int Initialize3(SGLEvent *pEvent);
    virtual int RenderBegin();
    virtual int Render();
    virtual int RenderEnd();
    virtual void SendMakeBlzMessage();
    virtual void SetBlzSlotCount(int nBlzID, bool bAdd,CSprite* pDeletedSprite);
    
    //1. 처음 화면에 훈련소에 관련 설명을 해준다. 화인 버튼을 클릭하면
    void Setp1_IntroduceDlg();
    //2. 이동 훈련 입니다. 탱크를 탭하여 이동하려고 하는 곳으로 옮깁니다.
    void Setp2_MoveDlg();
    //3. 공격 훈련 입니다. 적탱크를 클릭하세요.
    void Setp3_Attack();
    
    //5. 공격 훈련 입니다. 레이더위에 적탱크를 클릭하세요.
    void Setp5_AttackRadia();
    
    //4. 슬롯을 변경하는 미션
    void Setp5_1_ChangeSlot();
    
    //6. 가드타워를 만들어라.
    void Setp6_MakeTower();
    
    void Setp6_Upgrade(); //업그레이드 
    
    //7. 미션,  적의 메인타워를 파괴하세요.
    void Setp7_Mission();
    
    //8. 마무리
    void Setp8_End0();
    void Setp8_End();
    //    void SetMakePosition(float fRadius,int nIndex,CSprite *pTank,SPoint* ptCenter);
    
    
    //현제 스텝이 종료되어 다음 스텝이 무엇인지를 정한다.
    void OnNextStep(enum TRAININGSTEP CurrentStep);
    
    virtual int OnEvent(SGLEvent *pEvent);
    virtual void MakeActorAtStartPostion(); //시작지점으로 스템에 맞게 적절히 사용하자.
    
    virtual void BeginTouch(long lTouchID,float x, float y);
    virtual void MoveTouch(long lTouchID,float x, float y);
    virtual void EndTouch(long lTouchID,float x, float y);
    
    //런타임 아이템을 만들지 말자.
    virtual void MakeRuntimeItem(SPoint* ptPostion) {};
    //확장한 이유는 적이 터졌을때 점수를 올려주지 말자.
    virtual void CheckAttectedByEnSpriteAtBomb(CListNode<CSprite> *pCoreListSprite,CBomb *pBomb);
    virtual void SetClickedSprite(CSprite* pClickedSprite);
    
    void SetStepMessage(const char *sTitle,const char *sDesc);
    
    void RenderFingerBegin(int nTime);
    void RenderFinger();
    
    //    void SetRotationCarmeraToPoint(SPoint* pTartketPoint);
    //    void RotationCarmeraToPoint(int nTime);
    void ResetDisplayItemSlot();
    
#ifdef ANDROID
    virtual void ResetTexture();
#endif
    
    //컨트롤을 깜빡깜빡거리게 한다.
    void RenderMarkControl(float* pVertex); 
protected:
    enum TRAININGSTEP mStep;
    
    //훈련소 소개말..
    CLabelCtl*  mpMessageDlg;
    CLabelCtl*  mpStepMessageDlg;
    
    
    CLabelCtl*  mpStepMessageTitle;
    CLabelCtl*  mpStepMessageDes;
    CLabelCtl*  mpStepMessageDes2;
    CLabelCtl*  mpArrowBlz; //건물 설명 화살표
    
    unsigned long mnNextTime;
    
    
    GLfloat     mFingerVertex[12];
    GLuint      mnFingerTexture;
    float       mfFingerUpDwonAction;//위아래로 흔들기
    float       mfFingerUpDwonActionDirection; //-1,+1
    
    SPoint      mptFingerMove; //손가락의 현재 위치
    SVector     mvtFingerMove; //손가락이 이동하려고 하는 벡터
    
    
    GLfloat     mMoveTargetGround[12]; //이동하려고 하는 위치의 땅을 그려준다.
    CArrowMotionSprite    *mpMoveTargetArrowSprite;
    CSprite*    mEnamySprite;
    CSprite*    mEnamySprite2;
    
    //    SPoint      *mpCarmeraRotationTartketPoint;
    GLfloat     *mpRaidaFingerVertex;//[12];
    GLfloat      mRaidaFingerWorldVertex[12];//[12];
    float        mfRaidaFingerMax;
    float        mfRaidaFingerMin;
    
    int          mMoveMotionStep;// 0 : 위에서 아래로  1:목표지점으로 이동
    
    //컨트롤을 깜빡깜빡거리게 한다.
    unsigned long mnRenderMarkControlTime;
    bool         mbRenderMarkControlTime;
    bool         mbUpgrading;
};
#else
//enum TRAININGSTEP
//{
//    TRAININGSTEP_NONE = 0,
//    TRAININGSTEP_INTRODUCE,
//    TRAININGSTEP_MOVE,
//    TRAININGSTEP_ATTACK,
//    TRAININGSTEP_ATTACKRADIA,
//    TRAININGSTEP_CHANGE_SLOT, //아이템 슬롯을 변경한다.
//    TRAININGSTEP_MAKETOWER,
//    TRAININGSTEP_MISSION,
//    TRAININGSTEP_END0,
//    TRAININGSTEP_END
//    
//};
//
//
//class CArrowMotionSprite;
//class CLabelCtl;
//class CScenario;
//class CTrainingWorld : public CHWorld {
//public:
//    CTrainingWorld(CScenario* pScenario);
//    virtual ~CTrainingWorld();
//    
//    virtual int Initialize2(SGLEvent *pEvent);
//    virtual int Initialize3(SGLEvent *pEvent);
//    virtual int RenderBegin();
//    virtual int Render();
//    virtual int RenderEnd();
//    virtual void SendMakeBlzMessage();
//    virtual void SetBlzSlotCount(int nBlzID, bool bAdd,CSprite* pDeletedSprite);
//    
//    //1. 처음 화면에 훈련소에 관련 설명을 해준다. 화인 버튼을 클릭하면
//    void Setp1_IntroduceDlg();
//    //2. 이동 훈련 입니다. 탱크를 탭하여 이동하려고 하는 곳으로 옮깁니다.
//    void Setp2_MoveDlg();
//    //3. 공격 훈련 입니다. 적탱크를 클릭하세요.
//    void Setp3_Attack();
//    
//    //5. 공격 훈련 입니다. 레이더위에 적탱크를 클릭하세요.
//    void Setp5_AttackRadia();
//    
//    //4. 슬롯을 변경하는 미션
//    void Setp5_1_ChangeSlot();
//    
//    //6. 가드타워를 만들어라.
//    void Setp6_MakeTower();
//    
//    //7. 미션,  적의 메인타워를 파괴하세요.
//    void Setp7_Mission();
//    
//    //8. 마무리
//    void Setp8_End0();
//    void Setp8_End();
////    void SetMakePosition(float fRadius,int nIndex,CSprite *pTank,SPoint* ptCenter);
//    
//    
//    //현제 스텝이 종료되어 다음 스텝이 무엇인지를 정한다.
//    void OnNextStep(enum TRAININGSTEP CurrentStep);
//    
//    virtual int OnEvent(SGLEvent *pEvent);
//    virtual void MakeActorAtStartPostion(); //시작지점으로 스템에 맞게 적절히 사용하자.
//    
//    virtual void BeginTouch(long lTouchID,float x, float y);
//    virtual void MoveTouch(long lTouchID,float x, float y);
//    virtual void EndTouch(long lTouchID,float x, float y);
//    
//    //런타임 아이템을 만들지 말자.
//    virtual void MakeRuntimeItem(SPoint* ptPostion) {};
//    //확장한 이유는 적이 터졌을때 점수를 올려주지 말자.
//    virtual void CheckAttectedByEnSpriteAtBomb(CListNode<CSprite> *pCoreListSprite,CBomb *pBomb);
//    virtual void SetClickedSprite(CSprite* pClickedSprite);
//    
//    void SetStepMessage(const char *sTitle,const char *sDesc);
//    
//    void RenderFingerBegin(int nTime);
//    void RenderFinger();
//
////    void SetRotationCarmeraToPoint(SPoint* pTartketPoint);
////    void RotationCarmeraToPoint(int nTime);
//    void ResetDisplayItemSlot();
//    
//#ifdef ANDROID
//    virtual void ResetTexture();
//#endif
//    
//protected:
//    enum TRAININGSTEP mStep;
//    
//    //훈련소 소개말..
//    CLabelCtl*  mpMessageDlg;
//    CLabelCtl*  mpStepMessageDlg;
//    
//    
//    CLabelCtl*  mpStepMessageTitle;
//    CLabelCtl*  mpStepMessageDes;
//    CLabelCtl*  mpStepMessageDes2;
//    CLabelCtl*  mpArrowBlz; //건물 설명 화살표
//    
//    unsigned long mnNextTime;
//    
//    
//    GLfloat     mFingerVertex[12];
//    GLuint      mnFingerTexture;
//    float       mfFingerUpDwonAction;//위아래로 흔들기
//    float       mfFingerUpDwonActionDirection; //-1,+1
//    
//    SPoint      mptFingerMove; //손가락의 현재 위치
//    SVector     mvtFingerMove; //손가락이 이동하려고 하는 벡터
//    
//    
//    GLfloat     mMoveTargetGround[12]; //이동하려고 하는 위치의 땅을 그려준다.
//    CArrowMotionSprite    *mpMoveTargetArrowSprite;
//    CSprite*    mEnamySprite;
//    CSprite*    mEnamySprite2;
//
////    SPoint      *mpCarmeraRotationTartketPoint;
//    GLfloat     *mpRaidaFingerVertex;//[12];
//    GLfloat      mRaidaFingerWorldVertex[12];//[12];
//    float        mfRaidaFingerMax;
//    float        mfRaidaFingerMin;
//    
//    int          mMoveMotionStep;// 0 : 위에서 아래로  1:목표지점으로 이동
//};
#endif //New_CTrainingWorld


#endif /* defined(__SongGL__CTrainingWorld__) */
