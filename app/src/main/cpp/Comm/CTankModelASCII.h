//
//  CTankModelASCII.h
//  SongGL
//
//  Created by itsme on 13. 9. 13..
//
//

#ifndef __SongGL__CTankModelASCII__
#define __SongGL__CTankModelASCII__

#include "CMS3DModelASCII.h"

class CTankModelASCII : public CMS3DModelASCII
{
public:
    CTankModelASCII(CMS3DASCII *pMS3DASCII);
    virtual ~CTankModelASCII();
    virtual GLvoid		updatef(GLuint time);
     virtual GLvoid		renderf() const;
    
    float GetmPoUpAngle() { return mfPoUpAngle;}
    void SetmPoUpAngle(float v) { mfPoUpAngle = v;}
    
    void SetPointStatrMissile(int** ppV) { mptStartMissile = ppV;}
    
    void SetHideMeshGroup(int nIndex,bool bV) { arrHide[nIndex] = bV;}
    bool GetHideMeshGroup(int nIndex) { return arrHide[nIndex];}
    
protected:
    float mfPoUpAngle;
    int **mptStartMissile;  //미사일 시작위치.
    bool *arrHide;
};

#endif /* defined(__SongGL__CTankModelASCII__) */
