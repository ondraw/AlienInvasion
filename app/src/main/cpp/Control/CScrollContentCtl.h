//
//  CScrollContentCtl.h
//  SongGL
//
//  Created by 송 호학 on 11. 12. 2..
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef SongGL_CScrollContentCtl_h
#define SongGL_CScrollContentCtl_h
#include "CLabelCtl.h"
class CScrollContentCtl : public CLabelCtl {

public:
    CScrollContentCtl(CControl* pParent,CTextureMan *pTextureMan);
    virtual ~CScrollContentCtl();
    
    
    virtual bool OnButtonMove(float fVectorX,float fVectorY);
    
    

    //차일드또한 움직임 이벤트가 존재하는지 체크하지 않음.(속도를 위해서)
    virtual bool MoveTouch(long lTouchID,float x, float y);

    virtual int RenderBegin(int nTime);
    virtual int Render();
    
    void SetRowSelMode(bool v) { mbRowSelMode = v;}
    bool GetRowSelMode() { return mbRowSelMode;}
    
protected:
    bool mbRowSelMode; //Row선택모드:true 셀선택모드 : false
};


#endif
