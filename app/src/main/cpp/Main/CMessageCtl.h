//
//  CMessageCtl.h
//  SongGL
//
//  Created by Song Hohak on 12. 1. 13..
//  Copyright (c) 2012 thinkm. All rights reserved.
//

#ifndef SongGL_CMessageCtl_h
#define SongGL_CMessageCtl_h
#include "CLabelCtl.h"

enum CMessageCtlType {
    CMessageCtlType_GoToMenu, //메뉴버튼 클릭시
    CMessageCtlType_GoToMenu_Died, //죽었을 경우 (다시 시도, 메뉴)
    CMessageCtlType_GoToMenu_Successed, //성공했을 경우 +Gold
    CMessageCtlType_Request_Aliance, //동맹요청이 왔다.
    };

class CTextureMan;
class CMessageCtl : public CLabelCtl
{
public:
    CMessageCtl(CTextureMan *pTextureMan,CMessageCtlType Type);
    virtual ~CMessageCtl();
    int Initialize();  
    
    void Show();
    void Hide();
    CMessageCtlType GetType() { return mType;}
protected:
    virtual void SetHide(bool bHide); //차일드는 설정을 제외함.
protected:
    
    CMessageCtlType mType;
    
};

#endif
