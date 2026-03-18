//
//  CTableCtl.h
//  SongGL
//
//  Created by 송 호학 on 11. 12. 4..
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef SongGL_CTableCtl_h
#define SongGL_CTableCtl_h
#include "CScrollCtl.h"

class CTableCtl : public CScrollCtl 
{
public:
    CTableCtl(CControl* pParent,CTextureMan *pTextureMan);
    ~CTableCtl();
    
    void AddItem(CControl* pItemCtl);
    void AddItemHorizon(CControl* pItemCtl);
    void RemoveItem(int nIndex);
    void RemoveItemHorizon(int nIndex);
    void RemoveAll(bool bVerticalList = true);
    
    void SetRowSelMode(bool v);
    bool GetRowSelMode();
    CControl* GetRowCtl(int nRow);
    int GetRowCnt();
    
    bool IsVisibleRow(int nIndex);
    void SetVisibleVRow(int nIndex);
    void SetVisibleHRow(int nIndex);
};

#endif
