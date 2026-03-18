//
//  CItemMan.h
//  SongGL
//
//  Created by itsme on 12. 9. 13..
//
//

#ifndef SongGL_CItemMan_h
#define SongGL_CItemMan_h

#include <string>
#include "sGLDefine.h"
#include "CProperty.h"
class CWorld;
class CItemMan
{
public:
    CItemMan(CWorld* pWorld);
    ~CItemMan();

    bool Initialize();
    void BeginCore(int nTime);
    
    //Thread 단에서 아이템을 생성해주어야 한다.
    void AddItem(SPoint* ptItem);
    
    //아이탬이 생성되어야 하는지 판단할때 사용한다.
//    bool IsNeedItem() { return mbNeedItem;}
    
    
public:
    CWorld* mpWorld;
    unsigned long mnM;
    bool mbNeedItem;
    unsigned long*    mItemList; //폭탄: 0xFF00 , 아이템: 0x00FF
    int     mItemListCnt;
    
    int     mnIndexBonus2;
    short     mnIndexL6;
    short     mnIndexL5;
    short     mnIndexL4;
    short     mnIndexL2;
    
    vector<PROPERTY_RUNTIMEITEM*> mItems;
    
    bool    mbIsCompletedMap; //첫맵은 탱크를 기본으로 주지만, 이미 깬 맴이면은 주지 말자.
    bool    mIsOneRule; //처음시작시 탱크를 기본으로 주자. 다주었으면 false로 해서 로직을 막자.
};

#endif
