//
//  CMS3DModelASCII.h
//  SongGL
//
//  Created by 호학 송 on 12. 8. 16..
//
//

#ifndef SongGL_CMS3DModelASCII_h
#define SongGL_CMS3DModelASCII_h
#include "CMS3DASCII.h"
#include "CMyModel.h"
class CTextureMan;
class CMS3DModelASCII : public CMyModel
{
public:
    CMS3DModelASCII(CMS3DASCII *pMS3DASCII);
    virtual ~CMS3DModelASCII();
    virtual MODELTYPE GetType() { return MODELTYPE_MS3D_ASSCII;}
    
    virtual GLvoid		updatef(GLuint time);
    virtual GLvoid		renderf() const;

   
    //createBoundingSpheref함수를 콜해야 생성된다.
    virtual void GetCenter(GLfloat *fOutCenter);
    
    virtual bool    IsPicking(CPicking* pPick,int nWinPosX,int nWinPosY);
    virtual bool    IsPicking(CPicking* pPick,SPoint& nearPoint,SPoint& farPoint,int nWinPosX,int nWinPosY);
    //주인공은 버텍스 갯수가 너무 많아서 그룹 0만 검색하자.
    virtual bool IsPickingZeroGroup(CPicking* pPick,int nWinPosX,int nWinPosY);
    
    virtual GLboolean	collidesBoundingSpheref(GLbyte* search, GLubyte searchSize, GLbyte* hitJoint, CMyModel& model, GLbyte* searchModel, GLubyte searchSizeModel, GLbyte* hitJointModel, GLboolean exact, GLbyte stopAtLevel);

    
    
    virtual float GetCompactOtherSpriteInterval();
    //createBoundingSpheref함수를 콜해야 생성된다.
    virtual float GetRadius();

    //scale을 한다음에 정의 해야 한다.
    virtual GLvoid		createBoundingSpheref(GLbyte level);
    
//불필요함.
    virtual GLvoid		setNextAnimationf(GLshort next, GLboolean loop, GLboolean wait) {};
    void InitFinalMatrix(){};
    GLboolean	isAnimationRunningf() { return false;}
    bool      calFinallyMatrix() { return true;}
    virtual GLshort		getCurrentAnimationf() { return -1;}
    
//고유함수
    sglMesh*    GetWorldMesh() { return mpWorldMesh;}
    int GetMeshSize();
    sglMesh* GetMesh();

    CMS3DASCII* GetMS3DASCII() { return mpMS3DASCII;}
protected:
    int init();
    
    
//public:
//    float orientation[3];
//    float position[3];
//    float scale[3];
//    float *pfinalmulMaxtix; //처가해야할 매트릭스가 있으면 첨가해준다. 탱크의 해더 기울기를 그려준다.
    
protected:
    sglMesh     *mpWorldMesh;
    CMS3DASCII  *mpMS3DASCII;
//    float       mfCenter[3];
    float       mfColidedDistance;
};

#endif
