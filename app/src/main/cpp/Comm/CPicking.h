//
//  CPicking.h
//  SongGL
//
//  Created by Song Hohak on 11. 10. 26..
//  Copyright (c) 2011 thinkm. All rights reserved.
//

#ifndef SongGL_CPicking_h
#define SongGL_CPicking_h

class CPicking
{
public:
    CPicking();
    ~CPicking();
    
    
    
    void ResetPickingViewPort();
    
    //-------------------------------------------------------------------
    //피킹정보를 가져온다.
    //You have to get the state of GL_VIEWPORT and GL_PROJECTION_MATRIX right after you create them, which is right after the
    //glMatrixMode(GL_PROJECTION);
    //glLoadIdentity(); 
    //이것을 한후에 vipoert와 프로젝션매트릭스 정보를 가져와야한다.
    //출처...http://stackoverflow.com/questions/4752595/ray-triangle-intersection-picking-not-working
    //-------------------------------------------------------------------
    bool sglIsPicking(float vertex0[3],float vertex1[3],float vertex2[3],SPoint& nearPoint,SPoint& farPoint);
    void GetUnProject(GLint winPos[2],SPoint& outPoint,int nType);
   
    
    //Removed By Song ResetPickingViewPort에서 처리를 해야 된다.
    //주의 
    //glMatrixMode(GL_PROJECTION);
    //glLoadIdentity();
    //위의 초기화 이후에 함수를 불러서 초기화를 해야한다.
//    static void Initialize();
    
protected:
    static GLint viewport[4];
    static float projection[16];
    GLfloat modelview[16];
};

GLint gluUnProject(GLfloat winx, GLfloat winy, GLfloat winz,
                   const GLfloat model[16], const GLfloat proj[16],
                   const GLint viewport[4],
                   GLfloat * objx, GLfloat * objy, GLfloat * objz);
#endif
