//
//  CInputCtl.h
//  SongGL
//
//  Created by 송 호학 on 11. 12. 20..
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef SongGL_CInputCtl_h
#define SongGL_CInputCtl_h
#include "CPopupCtl.h"
#include "CProperty.h"
#include "CUserInfo.h"
#include <string>
using namespace std;

enum CInputCtl_Type {
    CInputCtl_Bomb = 0,
    CInputCtl_Item = 1
    };
class CTextureMan;
class CInputCtl : public CPopupCtl {
public:
    CInputCtl(CUserInfo* pUserInfo,CTextureMan *pTextureMan,int nItemID,CControl* pCountInCell,int nRow);
    virtual ~CInputCtl();    
    virtual int Initialize(int nControlID,PROPERTY_BOMB* Prop,int nWidth,int nHeight,vector<string>& lstImage);
    virtual int Initialize(int nControlID,PROPERTY_ITEM* Prop,int nWidth,int nHeight,vector<string>& lstImage);
    int GetItemID() { return mnItemID;}
    int GetRow() { return mnRow;}
    
    //선택한셀안의 돈컨트롤 => 아이템을 사거나 팔때 컨트롤에 직접 +,-를 반영해준다.
    CControl* GetCountInCell() { return mpCountInCell;}
    
    static void AddBombCell(CTextureMan *pTextureMan,CUserInfo* pUserInfo,PROPERTY_BOMB* prop,CControl* pCell,int x = 0,int y = 0);
    static bool AddItemCell(CTextureMan *pTextureMan,CUserInfo* pUserInfo,PROPERTY_ITEM* prop,CControl* pCell,int x = 0, int y = 0);
    

    void SetUpgrageData(bool bAttack);
    
#ifdef ANDROID
    virtual void ResetTexture();
#endif
    
protected:
    CUserInfo*      mpUserInfo;
    PROPERTY_BOMB*  mBombProp;
    int             mnItemID;
    CControl*       mpCountInCell;
    CInputCtl_Type  mType;
    int             mnRow;
    
    
    
    //Upgrage테이블
    string                       msAttackDesc;
    string                       msDependDesc;

};

#endif
