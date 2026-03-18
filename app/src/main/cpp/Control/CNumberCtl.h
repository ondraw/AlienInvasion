//
//  CNumberCtl.h
//  SongGL
//
//  Created by 송 호학 on 11. 12. 8..
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef SongGL_CNumberCtl_h
#define SongGL_CNumberCtl_h
#include "CLabelCtl.h"
enum CNumberCtlType { CNumberCtlType_Normal,CNumberCtlType_Plus,CNumberCtlType_Minus,CNumberCtlType_Bracket,CNumberCtlType_Percent};
class CNumberCtl : public CLabelCtl {
public:
    enum ALIGN { ALIGN_LEFT,ALIGN_RIGHT,ALIGN_CENTER};
    CNumberCtl(CControl* pParent,CTextureMan *pTextureMan,bool bBold = false);
    virtual ~CNumberCtl();
    CControl* Clone(CControl* pNew = NULL);
    
    //이벤트를 받아야 하면 bButton = true로 하면 클릭했을 때 이벤트를 받는다.
    //nNumberWidth 하나의 숫자의 넓이
    //offsetx 하나의 숫자의 offset
    virtual int Initialize(int nControlID,
                           float x, float y, float nWidth,float nHeight,float nNumberWidth,
                           float offsetx,float offsety,vector<string>& lstImage,bool bButton = false);
    
    //0: Normal 1:PlusMark -1:MinusMark
    void SetNumber(int nNumber,CNumberCtlType PlusMark = CNumberCtlType_Normal);
    void SetNumberF(float fNumber,CNumberCtlType PlusMark = CNumberCtlType_Normal);
    
    int GetNumber();
    float GetNumberF();
    void SetAlign(ALIGN v) { mnAlign = v;}
    void ClearNumber();

public:
    char msNumber[10];
    float  mnNumberHeight;
    float  mnNumberWidth;
    ALIGN  mnAlign;
    
    CControl* mButtonCtl;
    bool mbButtonType;
    
    
    float moffsetx;
    float moffsety;
    CNumberCtlType mPlusMark;
    
    bool mbBoldType;
};


#endif
