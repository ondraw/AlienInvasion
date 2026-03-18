//
//  CControl.h
//  SongGL
//
//  Created by 송 호학 on 11. 11. 30..
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef SongGL_CControl_h
#define SongGL_CControl_h

#include <string>
#include <vector>
#include "CList.h"
#include "ARender.h"
#include "CTextureMan.h"
using namespace std;

enum CONTROLANI 
{ 
    CONTROLANI_NONE,
    CONTROLANI_ZOOM_IN,CONTROLANI_ZOOM_OUT,CONTROLANI_ZOOM_OUT_END,
    CONTROLANI_LIN,CONTROLANI_LOUT,CONTROLANI_LOUT_END,
    CONTROLANI_RIN,CONTROLANI_ROUT,CONTROLANI_ROUT_END,
    CONTROLANI_TIN,CONTROLANI_TOUT,CONTROLANI_TOUT_END,
    CONTROLANI_BIN,CONTROLANI_BOUT,CONTROLANI_BOUT_END,
    CONTROLANI_ZOOM_IN_OUT,CONTROLANI_ZOOM_IN_OUT2, //석택했을때 in=>out효과.
    CONTROLANI_ZOOM_IN_HIDE,CONTROLANI_ZOOM_IN_HIDE_END, //Zoom In 하면서 살아진다.
    CONTROLANI_WIGWAG,CONTROLANI_WIGWAG_END //컨트롤을 흔들다.
};   
    
class CTextureMan;
class CControl;
class CControl : public ARender
{
public:
    CControl(CControl* pParent,CTextureMan *pTextureMan);
    virtual ~CControl();
    
    virtual CControl* Clone(CControl* pNew = NULL);
    
    CTextureMan* GetTextureMan() { return mpTextureMan;}
    virtual void AddControl(CControl* pControl);
    virtual void InsertControl(int nIndex,CControl* pControl);
    
    virtual void Remove(CControl* pControl);
    virtual void Remove(int nIndex);
    virtual void RemoveAll();
    
    // x ,y : Parent의 상대적 좌표이다.
    virtual int Initialize(int nControlID,float x, float y, float nWidth,float nHeight,vector<string>& lstImage,
                           float fsu,float fsv,float feu,float fev);
    
    //ARender 가상화
    virtual int Initialize(SPoint *pPosition,SVector *pvDirection) { return E_SUCCESS;}
	virtual int RenderBegin(int nTime);
	virtual int Render();
	virtual int RenderEnd();	
    virtual int OnEvent(SGLEvent *pEvent);
    
    
    virtual bool BeginTouch(long lTouchID,float x, float y);
    virtual bool MoveTouch(long lTouchID,float x, float y);
    virtual bool EndTouch(long lTouchID,float x, float y);
    virtual bool IsTabIn(float x,float y);
    
    
    virtual bool OnButtonDown();
    virtual bool OnButtonUp(bool bInSide);
    virtual bool OnButtonMove(float fVectorX,float fVectorY);
    
    //CScrollContentCtl에서 컨텐츠가 스크롤링한다면 드래그 이벤트가 발생한다. 그럴때 스크롤된 상태를 적용해주라고 이벤트가 날라온다.
    virtual bool OnDragInvalidate(float fDVectorX, float fDVectorY) { return true; }
    
    virtual void SetEnable(bool bEnable);
    virtual void SetHide(bool bHide);
    virtual bool GetHide() { return mbHide;}
    virtual void SetImageData(vector<string>& lstImage);
    
    //배경이미지 말고 그 안쪽에 드러가는 컨텐츠 이미지 즉 테두리 안쪽에 드러가는 아이템,폭탄, 탱크 같은 맵...
    virtual void SetContentsData(const char* sContentsImage);
    virtual float* GetBounds() {return 0;}
    
    virtual void FlipVertical();
    virtual void SetBackCoordTex(float fsu,float fsv,float feu,float fev);
    void TopFront(); //가장 상단으로 옮겨준다.
    
    void GetPosition(float& fDPosX,float& fDPosY) { fDPosX = mfDPosX; fDPosY= mfDPosY;}
    void SetPosition(float  fX,float fY);
    void MoveVector(float  fDVectorX,float  fDVectorY);
    void ZoomVector(float  fDVectorZ);
    
    void GetSize(float& fDWidth,float& fDHeight) { fDWidth = mfDWidth;fDHeight = mfDHeight;}
    int  GetID() { return mnID;}
    void SetID(int nID) { mnID = nID;}
    CList<CControl*>* GetChilds() { return &mlstChild;}
    CControl* GetChild(int nIndex) { return mlstChild.get(nIndex);}
    
    CControl* FindControl(int nID);
    

    /**
     스크롤뷰의 컨텐츠컨트롤은 드래그하여 좌우로 움직일 수가 있다.
     */
    void SetCanDrag(bool v) { mbCanMove = v; }
    
    void TourchMotion(int nTime);
    void SetParent(CControl* pP) { mParent = pP;}
    CControl* GetParent() { return mParent;}
    void SetBackColor(float fColor[4]) { memcpy(mfBackColor, fColor, sizeof(float)*4);}
    void SetTextColor(float fColor[4]) { memcpy(mfTextColor, fColor, sizeof(float)*4);}
    void GetBackColor(float fOutColor[4]) { memcpy(fOutColor, mfBackColor, sizeof(float)*4);}
    void GetTextColor(float fOutColor[4]) { memcpy(fOutColor, mfTextColor, sizeof(float)*4);}
    
    void SetPushed(bool bPushed) { mbPushed = bPushed;}
    bool GetPushed() { return mbPushed;}
    
    long GetLData() { return mlData;}
    void SetLData(long v) { mlData = v;}
    long GetLData2() { return mlData2;}
    void SetLData2(long v) { mlData2 = v;}
    void SetRotationZ(float fAngle);
//    void SetRotationY(float fAngle);
    
    static void MakeWorldStickVertexGL(GLfloat *pWorldVertex,GLfloat *pOrgVertex,float xPos,float yPos);
    static void MakeWorldStickVertexD(GLfloat *pWorldVertex,GLfloat *pOrgVertex,float xPos,float yPos);
    
    
    virtual void SetAni(CONTROLANI nAniType,float fAccel = 1.0f);
    virtual CONTROLANI Animating() { return mAniType;}
    
    
//    CControl* GetChildByID(int nID);
    bool      GetTouchMoving() { return mbTouchMoving;}
    
    void SetWorldBackCoordTex(float fWorldBackCoordTex[8]) { memcpy(mfWorldBackCoordTex, fWorldBackCoordTex, sizeof(mfWorldBackCoordTex));}
    
    void NextWigWag(int nTime);
    
    virtual void SetDisableColor(float* fBackDisalbeColor);
    virtual float* GetDisableColor() { return mfBackDisalbeColor;}
    
#ifdef ANDROID
    virtual void ResetTexture();
#endif
    
    //투명도 조절한다.
    void SetTranslate(float fAlpha);
    /**
     * 카메라는 2d를 계산하기 편하게 이동한다.
     */
    static int ActivateCamera(); 
    
    /**
     3d를 2d의 화면좌표로 계산하기 위한 작업을 한다.
     divice 좌표는 상단 0.0
     openggldive 좌표  center
     */
    static void DtoGL(GLfloat nLine,GLfloat* fLine);
    static void DtoGL(GLfloat nX,GLfloat nY,GLfloat* fOutX,GLfloat* fOutY);
    static void GLtoD(GLfloat fX,GLfloat fY,GLint* nOutX,GLint* nOutY);

    float* GetWorldVertex() { return mfWorldBackVertex;}
//    void ResetRotationOrgVertex();
#ifdef MAC
    //맥의 디자인에서 컨트롤을 선택했을때 선택표시를 해주기 위해서.
    static CControl* gLastSelControl;
    const char* GetLabel() {
        if(mlstImage.size() == 2) return mlstImage[1].c_str();
        return NULL;
    }
    vector<string>* GetImageData() { return &mlstImage;}
    
    
#endif //MAC
    
protected:
    CList<CControl*> mlstChild;  
    CControl* mParent;
    CTextureMan *mpTextureMan;
    
    long mlTouchID; 
    bool mbTouchMoving;
    int  mnID;
    bool mbEnable;
    bool mbPushed;
    bool mbHide;
    
    float mfDPosX;
    float mfDPosY;
    float mfDPosZ; //0

    float mfDWidth;
    float mfDHeight;
    float                       mfWorldBackVertex[12];
    GLuint                      mnBackID;
    GLuint                      mnContentsID; //액자같은 컨트롤에 외양은 액자. 알맹이(내용은)특정아이콘을 넣을 경우 사용한다.
    
    bool  mbCanMove; //드래그를 인식한다.
    SPoint ptBefore;
    
    float mfBackColor[4];
    float *mfBackDisalbeColor;
    float mfTextColor[4];
    
    //CScrollContentCtl 에서 사용한다.
    float mAccelatorVector[2];//가속도
    float mfTourchSensitivity;
    
    long  mlData;
    long  mlData2;
    
    float mfDPosOrgX;
    float mfDPosOrgY;
    float mfAniAccel;
    CONTROLANI mAniType;
    
#if defined(ANDROID) || defined(MAC)
    //OnPause후에 OnResume을 하면 오픈지엘 거시기가 다시 만들어진다.
    //그래서 TextureID를 재할당해주어야 한다.
    vector<string> mlstImage;
#endif
    float mfWorldBackCoordTex[8];
    char* msContentsImage; //Android때문에 다시 복원하기위해서 이미지의 경로를 가지고 있자꾸나.
    float *mfWorldContentsCoordTex; //8
    unsigned long  nmAnimationTime; //WigWag에서 사용한다.
    
    
    float mfbRotationZ; //웨이팅 형태
//    float mfbRotationY; //증건 대시보드 변경형태
//    float*   mpfRotationOrgVertex; //회전이 존재 할경우.. 원래 위치를 기억한후에 그것을 기준으로 회전을 한다.
};


#endif
