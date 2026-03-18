//
//  CRoll3dModel.h
//  SongGL
//
//  Created by 송 호학 on 11. 12. 11..
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef SongGL_CRoll3dModel_h
#define SongGL_CRoll3dModel_h
#include "ARender.h"
#include "CProperty.h"


class CMyModel;
class CModelMan;

class CRoll3dModel : public ARender {
    
public:
    enum CRoll3dModelType { 
        CRoll3dModel_NONE , 
        CRoll3dModel_K9 = 1,
        CRoll3dModel_L1 = 2
    };
    
    CRoll3dModel(CModelMan* pModelMan,PROPERTY_TANK* Propety);
    virtual ~CRoll3dModel();
    
    int RenderBegin(int nTime,float fCurrentAngle);
public:
    virtual int Initialize(SPoint *pPosition,SVector *pvDirection);
	virtual int RenderBegin(int nTime);
	virtual int Render();
	virtual int RenderEnd();	
    virtual int OnEvent(SGLEvent *pEvent);
    void SetSelectedAnimationAngle(float fAniAngle);
    void ArrangeHeaderSprite(int xHeader,int yHeader,int zHeader);
    
    CMyModel* GetModel() { return mpModel;}
    CMyModel* GetModelHeader() { return mpModelHeader;}

protected:  
    CMyModel*	mpModel;
	CMyModel*   mpModelHeader;
    CModelMan*  mpModelMan;    
    float mHeaderPosition[3];
    
    //선택되었을때는 애니메이션을 회전시켜준다.
    float mfSelectedAnimationAngle;

    float mfBeforeDirAngle;
    PROPERTY_TANK mProperty;
    
    //날라다니는 객체는 부스터가 필요하다.
    GLfloat mWorldVertex[2][12];
    int              **mpBoosterPos;
    int                mnBoosterCnt;
    int RenderBeginBooster(int nTime);
    void GetBoosterPos(int nIndex,float** pOutFront,float** pOutBack );
    int RenderBooster(int nRand);
};

#endif
