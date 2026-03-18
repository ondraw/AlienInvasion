//
//  CMissionEnd.h
//  SongGL
//
//  Created by 호학 송 on 12. 9. 5..
//
//

#ifndef SongGL_CMissionEnd_h
#define SongGL_CMissionEnd_h
#include "CLabelCtl.h"


enum MissionEndCtlType {
    MissionEndCtlType_GoToMenu_Died, //죽었을 경우 (다시 시도, 메뉴)
    MissionEndCtlType_GoToMenu_Successed, //성공했을 경우 +Gold
};

class CUserInfo;
class CNumberCtl;
class CMissionEndCtl : public CLabelCtl
{
public:
    CMissionEndCtl(float fGoldPerScore,CTextureMan *pTextureMan,CUserInfo* pUserInfo);
    virtual ~CMissionEndCtl();
    virtual int RenderBegin(int nTime);
    
    int Initialize();
    
    //차일드까지 할필요가 없어서 오버라이드했다.
    void Show(MissionEndCtlType Type,int nScore,int nBonus,bool bTraining);
    
protected:
    CControl* mpIcon;   //icon
    CControl* mpMissionTitle;  //icon 옆에 미션 성공 실패를 나타낸다.
    CNumberCtl* mpScore;
    CNumberCtl* mpBonus;
    CNumberCtl* mpGold;
    CNumberCtl* mpTotal;
    
    MissionEndCtlType mType;
    CUserInfo* mpUserInfo;

    float mfRenderScore;
    float mfRenderBonus;
    float mfRenderGold;
    float mfRenderTotal;
    
    unsigned int mnTotalTime;
    
    int  mnStep; //0:처음 점수를 돈으로 환산 1:환산후 잠깐 기다린다음 종료한다.
    bool mbTraining;  //훈련모드일때는 총점을 계산하지 않고 단순히 골드만 한다.
    float mfGoldPerScore;
};

#endif
