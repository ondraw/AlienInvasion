/*
 *  sGLUtils.c
 *  SongGL
 *
 *  Created by 호학 송 on 10. 11. 19..
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#include <vector>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "CLoader.h"
#include "sGLUtils.h"
#include "CSGLMesh.h"
#include "CTextureMan.h"
#include "sGLTrasform.h"

#if CGDisplayMode == 1
int gnCGDiplayMode = 0; //일반
#endif

unsigned long mGGTime = 0L;
unsigned long mGGTimeAtPause = 0L; //Pause할때도 계속해서 시간의 흐름을 알고 싶다면 (즉 컨트롤 흔들어줄때 사용한다.)
unsigned int mEscapeT = 0;
timeval     mlastT;
timeval     mcurrT;

bool         mbPause = false; //시간을 멈춘다.

unsigned long GetGGTime() { return mGGTime;}
unsigned long GetGGTimeAtPause() { return mGGTimeAtPause;}


int XorCrypto(unsigned char* key, int keyLength, unsigned char* in, int nBytes)
{
    int i = 0;
    
    if( nBytes < keyLength ) { return -1; }
    
    while( i < nBytes )
    {
        in[i] ^= key[i%keyLength];
        i++;
    }
    
    return 0;
}

////UTC 현재 시간을 얻어온다.
//tm * GetUTCTime()
//{
//    timeval now;
//    /* 현재 시간을 얻어온다. */
//    gettimeofday(&now, NULL);
//    return (struct tm*)gmtime((time_t*)&now.tv_sec);
//}

//시간을 멈춘다.
void SetPauseTime(bool v)
{
    mbPause = v;
}

bool IsPauseTime()
{
    return mbPause;
}


timeval GetClock()
{
    gettimeofday(&mcurrT, 0);
    return mcurrT;
}

GLuint GetClockDelta()
{
    
#if CGDisplayMode == 1
    if(gnCGDiplayMode == 1)
        return 3; //시간을 1로 해준다.
#endif
    
    static GLboolean reset = GL_TRUE;
	if (reset)
	{
		gettimeofday(&mlastT, 0);
		mcurrT = mlastT;
		reset = GL_FALSE;
	}
	else
	{
		mlastT = mcurrT;
		gettimeofday(&mcurrT, 0);
	}
    
    //밀리초 단위.. 예)10.394밀리초
    mEscapeT = (unsigned int)((mcurrT.tv_sec - mlastT.tv_sec) * 1000l + (mcurrT.tv_usec - mlastT.tv_usec) / 1000l);

    //너무 차이가 나면.
    if(mEscapeT > 1000) mEscapeT = 400;
    
    
    if(mbPause)
    {
        mGGTimeAtPause += mEscapeT;
        return 0;
    }
    
    mGGTime += mEscapeT;
    mGGTimeAtPause += mEscapeT;
	return mEscapeT; 
}

#if VIEWROTATION_ANI
timeval gDifTime;
void    BeginClock()
{
    gettimeofday(&gDifTime, 0);
}
GLfloat EndClock()
{
    timeval time;
    gettimeofday(&time, 0);
    return(time.tv_sec - gDifTime.tv_sec) * 1000.0f + (time.tv_usec - gDifTime.tv_usec) / 1000.0f;
}
#endif

GLuint GetClockDeltaConst()
{
    
#if CGDisplayMode == 1
    if(gnCGDiplayMode == 1)
        return 3; //시간을 1로 해준다.
#endif

    if(mbPause) return 0;
    return mEscapeT;
}


#if defined ANDROID || defined IPHONE
void gluLookAt(GLfloat eyex, GLfloat eyey, GLfloat eyez,
			   GLfloat centerx, GLfloat centery, GLfloat centerz,
			   GLfloat upx, GLfloat upy, GLfloat upz)
{
    GLfloat m[16];
    GLfloat x[3], y[3], z[3];
    GLfloat mag;
    
    /* Make rotation matrix */
    
    /* Z vector */
    z[0] = eyex - centerx;
    z[1] = eyey - centery;
    z[2] = eyez - centerz;
    mag = sqrt(z[0] * z[0] + z[1] * z[1] + z[2] * z[2]);
    if (mag) {          /* mpichler, 19950515 */
        z[0] /= mag;
        z[1] /= mag;
        z[2] /= mag;
    }
    
    /* Y vector */
    y[0] = upx;
    y[1] = upy;
    y[2] = upz;
    
    /* X vector = Y cross Z */
    x[0] = y[1] * z[2] - y[2] * z[1];
    x[1] = -y[0] * z[2] + y[2] * z[0];
    x[2] = y[0] * z[1] - y[1] * z[0];
    
    /* Recompute Y = Z cross X */
    y[0] = z[1] * x[2] - z[2] * x[1];
    y[1] = -z[0] * x[2] + z[2] * x[0];
    y[2] = z[0] * x[1] - z[1] * x[0];
    
    /* mpichler, 19950515 */
    /* cross product gives area of parallelogram, which is < 1.0 for
     * non-perpendicular unit-length vectors; so normalize x, y here
     */
    
    mag = sqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
    if (mag) {
        x[0] /= mag;
        x[1] /= mag;
        x[2] /= mag;
    }
    
    mag = sqrt(y[0] * y[0] + y[1] * y[1] + y[2] * y[2]);
    if (mag) {
        y[0] /= mag;
        y[1] /= mag;
        y[2] /= mag;
    }
    
#define M(row,col)  m[col*4+row]
    M(0, 0) = x[0];
    M(0, 1) = x[1];
    M(0, 2) = x[2];
    M(0, 3) = 0.0;
    M(1, 0) = y[0];
    M(1, 1) = y[1];
    M(1, 2) = y[2];
    M(1, 3) = 0.0;
    M(2, 0) = z[0];
    M(2, 1) = z[1];
    M(2, 2) = z[2];
    M(2, 3) = 0.0;
    M(3, 0) = 0.0;
    M(3, 1) = 0.0;
    M(3, 2) = 0.0;
    M(3, 3) = 1.0;
#undef M
    glMultMatrixf(m);
    
    /* Translate Eye to Origin */
    glTranslatef(-eyex, -eyey, -eyez);
}
#endif



GLuint sglLoadTgaToBind(const char* pFileName,int nBindType,int *pnOutWidth,int *pnOutHeight,int *pnOutDepth,unsigned char** ppOutImage)
{
    GLuint uidTextureID = 0;
    //Fire이미지를 제너레이터 한다.
	int nResult = 0;
	
    char sFullPath[MAXLENGTH];
    sprintf(sFullPath, "%s%s",CLoader::getSearchPath(),(const char*)pFileName);
     
    sglTexture texture;
    
    if(strstr(pFileName, ".png"))
    {
        nResult = CTextureMan::LoadPNG((char*)sFullPath,texture);
    }
	else if(strstr(pFileName, ".bmp"))
    {
        nResult = CTextureMan::LoadBMP((char*)sFullPath,texture);
    }
    else
    {
        nResult = CTextureMan::LoadTGA((char*)sFullPath,texture);
    }
    
	if(nResult != 0)
	{
		HLogE("[Error] Can't load tga %s",sFullPath);
		return uidTextureID;
	}
    
    *pnOutWidth = texture.nWidth;
    *pnOutHeight = texture.nHeight;
    *pnOutDepth = texture.nDepth;
    *ppOutImage = texture.pImageBuffer;
    
    glGenTextures(1, &uidTextureID);
    
    //sglLog("Created MapID = %d\n",pResource->Texture.glTextureID);
    
    glBindTexture(GL_TEXTURE_2D, uidTextureID);
    
    //축소시 가장가까운 픽셀을 참조하여 축소를 한다.
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    //GL_NEAREST 이미지가 고르게 보이지 않고 가장자리가 거칠어서 GL_LINEAR로 하니 부드럽게 변경되었다.
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    
    //확대시 선형으로 확대를 하여 고르게 한다.
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    
    
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,nBindType);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,nBindType); 
    
    //GL_REPLACE 텍스처를 물체에 입힌다.
    //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_REPLACE);
    
    if(*pnOutDepth == 24)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 
                     *pnOutWidth, *pnOutHeight, 
                     0, GL_RGB, GL_UNSIGNED_BYTE, *ppOutImage);
    }
    else if(*pnOutDepth == 32)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 
                     *pnOutWidth, *pnOutHeight, 
                     0, GL_RGBA, GL_UNSIGNED_BYTE, *ppOutImage);
    }
    else //24 비트와 32비트만 지원한다.
    {
        glDeleteTextures(1, &uidTextureID);
       
        free(*ppOutImage);
        *ppOutImage = NULL;
        
        return 0;
    }
    
    //delete[] pImage;
    return uidTextureID;
    
}

//int SaveCutToGroupSize(
//					   int nWidth,
//					   int nHeight,
//					   int nDepth,
//					   int nGroupSize,
//					   unsigned char* pImage)
//{
//	TGA    *out = 0;
//	TGAData data;
//	int nGX = (nWidth - 1) / nGroupSize;
//	int nGY = (nHeight - 1) / nGroupSize;
//	
//	int nGroupSize_fromzero = nGroupSize + 1;
//	
//	if(nWidth != nHeight)
//	{
//		sglLog("이미지 사이즈의 넓이와 높이가 다르다");
//		return -1;
//	}
//	
//	if( nWidth % 2 == 0) 
//	{
//		sglLog("이미지의 크기는 홀수로 끝나야 한다.");
//		return -2;
//	}
//	
//	if( (nWidth - 1) % nGroupSize != 0)
//	{
//		sglLog("그룹의 사이즈는 (nImageWidth - 1)/nGroupsize = 0.0f으로 끝나야한다.");
//		return -3;
//	}
//	
//	int fz = 0;
//	int nMapPos = 0;
//	int nGroupImageSize = nGroupSize_fromzero * nGroupSize_fromzero * nDepth/8;
//	unsigned char* pGroupImage = new unsigned char[nGroupImageSize];
//	char sPath[1024];
//	
//	int nGroupIndex = 0;
//	int nIntervalGY = 0;
//	
//	for(int gy = 0; gy < nGY ; gy++)
//	{
//		for(int gx = 0; gx < nGX; gx++)
//		{
//			
//			nGroupIndex = gy * nGY + gx;
//			
//			
//			sprintf(sPath,"/Users/hohak/Temp/dream/img%02d_%02d%02d.tga",nGroupIndex,gx,gy);
//			out = TGAOpen((char*)sPath, (char*)"w");
//			
//			memset(pGroupImage,0,nGroupImageSize);
//			
//			//이미지는 위아래가 바뀐다.
//			nIntervalGY = nGY - gy - 1;
//			
//			nMapPos = 
//			(nIntervalGY * nGroupSize_fromzero - (1 * nIntervalGY)) * nWidth * nDepth/8 + 
//			(gx * nGroupSize_fromzero - (1 * gx)) * nDepth/8;
//			
//			
//			
//			//0 좌표로 중앙을 나타내려면 nWidth - 1 ( ex> 0 - 1 - 3) 		
//			for(int y = 0; y < nGroupSize_fromzero; y++)
//			{				
//				fz = nMapPos + 	y * nWidth * nDepth/8;
//				memcpy(pGroupImage + nGroupSize_fromzero * y * nDepth/8,pImage + fz, 
//					   nGroupSize_fromzero * nDepth/8);
//				
//			}
//			
//			data.img_data = pGroupImage;
//			data.cmap = 0;
//			data.img_id = 0;
//			data.flags = TGA_IMAGE_ID | TGA_IMAGE_DATA | TGA_RGB | TGA_RLE_ENCODE;
//			
//			//out->off = 18;
//			out->off = 0;
//			out->last = 0;
//			out->error = 0;
//			
//			out->hdr.id_len = 0;
//			out->hdr.map_t = 0;
//			out->hdr.img_t = 2;
//			out->hdr.map_first = 0;
//			out->hdr.map_entry = 0;
//			out->hdr.x = 0;
//			out->hdr.y = 0;
//			out->hdr.width = nGroupSize_fromzero;
//			out->hdr.height = nGroupSize_fromzero;
//			out->hdr.depth = nDepth;
//			out->hdr.alpha = 0;
//			out->hdr.horz = 0;
//			out->hdr.vert = 0;
//			
//			TGAWriteImage(out, &data);
//			if (out->last != TGA_OK) 
//			{
//				delete[] pGroupImage;
//				TGA_ERROR(out, out->last);
//				return -1;
//			}
//			TGAClose(out);
//			
//		}
//	}
//	
//	delete[] pGroupImage;	
//	return E_SUCCESS;
//}

//int SaveCut(char* sPath,int nGroupSize)
//{
//	int nResult = 0;
//	int nWidth,nHeight,nDept;
//	unsigned char* pImage = NULL;
//	nResult = LoadTga(sPath, &nWidth, &nHeight, &nDept, &pImage);
//	if(nResult != 0) return nResult;
//	
//	SaveCutToGroupSize(nWidth, nHeight, nDept, nGroupSize, pImage);
//	free(pImage);
//	return 0;
//}



//360도 안쪽으로 정리를 한다.
float AngleArrange(float fAngle)
{
	if(fAngle >= 360)
	{
		int nDiv = (int)fAngle / 360;
		fAngle = fAngle - nDiv*360;
	}
	else if(fAngle <= -360)
	{
		int nDiv = (int)-fAngle / 360;
		fAngle = fAngle + nDiv*360;
	}
	return fAngle;
}

float AngleArrangeEx(float fAngle)
{
    if(fAngle >= -0.001 && fAngle <= 0.001) 
        return 0; //360을 리턴하지 말자꾸나...
    
	if(fAngle >= 360)
	{
		int nDiv = (int)fAngle / 360;
		fAngle = fAngle - nDiv * 360;
	}
	else if(fAngle <= -360)
	{
		int nDiv = (int)-fAngle / 360;
		fAngle = fAngle + nDiv * 360;
        
        
        fAngle = 360 + fAngle; //항상 +의 각으로 표현한다.
	}
    else if(fAngle < 0)
    {
        fAngle = 360 + fAngle; //항상 +의 각으로 표현한다.
    }
    
    //각도가 거의 360이면 0도이다.
    if(fAngle >= 359.999) 
        return 0;
	return fAngle;
}





//2차 방정식 근을 구하기.
//Ax^2 + bx + c = 0 이 근 구하기
// -1==허근 0==(두실근) >0==(중근)
int QuadratiEcquation (float A,float B,float C , float &fV1, float &fV2)
{ 

    float d;
    d=B*B-4.0f*A*C;
    
    if(d>0)
    {
        fV1=(-B+sqrt(d))/(2*A);
        fV2=(-B-sqrt(d))/(2*A);
        return 0;
    }
    else if(d==0)
    {
        fV1 = -B/(2*A);
        return fV1;
    }

    return 0;
}







//출처.. http://www.opengl.org/resources/code/samples/mjktips/TexShadowReflectLight.html
enum {
    X, Y, Z, W
};
enum {
    A, B, C, D
};

/* Create a matrix that will project the desired shadow. */
void sglShadowMatrix(GLfloat shadowMat[4][4],
             GLfloat groundplane[4],
             GLfloat lightpos[4])
{
    GLfloat dot;
    
    /* Find dot product between light position vector and ground plane normal. */
    dot = groundplane[X] * lightpos[X] +
    groundplane[Y] * lightpos[Y] +
    groundplane[Z] * lightpos[Z] +
    groundplane[W] * lightpos[W];
    
    shadowMat[0][0] = dot - lightpos[X] * groundplane[X];
    shadowMat[1][0] = 0.f - lightpos[X] * groundplane[Y];
    shadowMat[2][0] = 0.f - lightpos[X] * groundplane[Z];
    shadowMat[3][0] = 0.f - lightpos[X] * groundplane[W];
    
    shadowMat[X][1] = 0.f - lightpos[Y] * groundplane[X];
    shadowMat[1][1] = dot - lightpos[Y] * groundplane[Y];
    shadowMat[2][1] = 0.f - lightpos[Y] * groundplane[Z];
    shadowMat[3][1] = 0.f - lightpos[Y] * groundplane[W];
    
    shadowMat[X][2] = 0.f - lightpos[Z] * groundplane[X];
    shadowMat[1][2] = 0.f - lightpos[Z] * groundplane[Y];
    shadowMat[2][2] = dot - lightpos[Z] * groundplane[Z];
    shadowMat[3][2] = 0.f - lightpos[Z] * groundplane[W];
    
    shadowMat[X][3] = 0.f - lightpos[W] * groundplane[X];
    shadowMat[1][3] = 0.f - lightpos[W] * groundplane[Y];
    shadowMat[2][3] = 0.f - lightpos[W] * groundplane[Z];
    shadowMat[3][3] = dot - lightpos[W] * groundplane[W];
    
}



void sglFindPlane(GLfloat plane[4],
          GLfloat v0[3], GLfloat v1[3], GLfloat v2[3])
{
    GLfloat vec0[3], vec1[3];
    
    /* Need 2 vectors to find cross product. */
    vec0[X] = v1[X] - v0[X];
    vec0[Y] = v1[Y] - v0[Y];
    vec0[Z] = v1[Z] - v0[Z];
    
    vec1[X] = v2[X] - v0[X];
    vec1[Y] = v2[Y] - v0[Y];
    vec1[Z] = v2[Z] - v0[Z];
    
    /* find cross product to get A, B, and C of plane equation */
    plane[A] = vec0[Y] * vec1[Z] - vec0[Z] * vec1[Y];
    plane[B] = -(vec0[X] * vec1[Z] - vec0[Z] * vec1[X]);
    plane[C] = vec0[X] * vec1[Y] - vec0[Y] * vec1[X];
    
    plane[D] = -(plane[A] * v0[X] + plane[B] * v0[Y] + plane[C] * v0[Z]);
}



//현재 점과 가야할 점의 각도...
float sglNowToPosAngle(SPoint* ptNow,SPoint* ptTo)
{
    //현재 방향
    SVector vtDir;
    vtDir.x = ptTo->x - ptNow->x;
    vtDir.y = 0;
    vtDir.z = -(ptTo->z - ptNow->z);
    sglNormalize(&vtDir);
    return AngleArrangeEx(atan2(vtDir.z,vtDir.x) * 180.0 / PI);
    
}

float sglGetRotationVelocity(bool bOnlyFront,float fNowDirAngle,float* fToDirAngle,float* fMoveVector)
{
    float fNowAngle = AngleArrangeEx(fNowDirAngle);
    //사이각
    float fDifAngle = AngleArrangeEx (*fToDirAngle -  fNowAngle);
    
    
    //  180 ==> 200 = 20
    //  359 ==> 1 = 2
    if( fDifAngle <= 180.0f)        
    {
        if(!bOnlyFront)
        {
    //        HLogD("DifAngle %f  %d\n",fDifAngle,fDifAngle > 90.0f ? true : false);
            if(fDifAngle < 90)  //앞으로 이동한다.
            {
                *fMoveVector = 1.0f;
                return 1.0f;
            }
            else
            {
                *fMoveVector = -1.0f;
                *fToDirAngle =  AngleArrangeEx(*fToDirAngle - 180.0f);
                return -1.0f;   //뒤로 이동한다.
            }
        }
        
        *fMoveVector = 1.0f; //앞으로
        return 1.0f; //회전방향
    }
    else                         
    {
        if(!bOnlyFront)
        {
            float fD = fDifAngle - 180.0f;
            if(fD >= 90) //앞으로 가야 한다.
            {
                *fMoveVector = 1.0f;
                return -1.0f;
            }
            else 
            {
                *fMoveVector = -1.0f;
                *fToDirAngle =  AngleArrangeEx(*fToDirAngle - 180);
                return 1.0f; //뒤로 가야 한다.
            }
        }
        
        *fMoveVector = 1.0f; //무조건 앞으로
        return -1.0f; //회전방향.
    }
    
    return 0.0f;
}

//회전이 지나쳤는지 확인한다.
bool sglCheckRotation(float fNowAngle,float fToAngle,float* fRotAngle)
{
    bool bNeedStopTurnning = false;
    
    if(*fRotAngle == 0.f)
        return true; //Added By Song 2014.02.25 0이면 정확하다.
    
    float fNextAngle = AngleArrangeEx(fNowAngle + *fRotAngle);
    
    if(*fRotAngle > 0) // + 방향으로 움직인다.
    {
        if(fNowAngle <= fNextAngle) //  10 => 20로 변경되엇을 경우
        {
            //10(Now) , 16(To), 20(Next) 즉 지나쳤다.
            if(fToAngle >= fNowAngle && fToAngle <= fNextAngle) //각도를 지나쳤을 경우.
            {
                *fRotAngle = fToAngle - fNowAngle;
                bNeedStopTurnning = true;
            }
        }
        else //(fnNowAngle > fNextAngle) 359(Now) => 2(Next) 로 변경되었을 경우. (0도를 지나쳤다면)
        {
            
            //350(Now) 355(to) 2(Next) 일경우
            if( fToAngle > fNextAngle && fToAngle > fNowAngle)
            {
                *fRotAngle = 360 - (fToAngle - fNowAngle);
                bNeedStopTurnning = true;
            }
            else if(fToAngle < fNextAngle) //1(to) 2(Next)
            {
                *fRotAngle = (fToAngle + 360) - fNowAngle;
                bNeedStopTurnning = true;
            }
        }
    }
    else // < 0  - 방향으로 움직인다.
    {
        if(fNowAngle > fNextAngle) // 20(Now) => 10(Next)로 변경되었다.
        {
            //20(Now) , 16(To), 10(Next)
            if(fToAngle <= fNowAngle && fToAngle >= fNextAngle) //각도를 지나쳤을 경우.
            {
                *fRotAngle = fToAngle - fNowAngle;
                bNeedStopTurnning = true;
            }
            
        }
        else //(fNowAngle <= fNextAngle) 2(Now) => 359(Next) 
        {
            //2(Now) 259(To) 350(Next)
            if( fToAngle >= fNowAngle && fToAngle >= fNextAngle)
            {
                *fRotAngle = fToAngle - (fNowAngle + 360);
                bNeedStopTurnning = true;
            }
            //2(Now) 1(To) 350(Next)
            else if(fToAngle < fNowAngle && fToAngle <= fNowAngle )
            {
                *fRotAngle = fToAngle - (fNowAngle + 360);
                bNeedStopTurnning = true;
            }
            //2(Now) 350(Next) 340(To)
        }
    }
    
    return bNeedStopTurnning;
}



float sglPhongShadeColor(SVector* pNormal,float fTerrianShadow)
{
    SVector Light;
    Light.x = 0.f;
    Light.y = 0.f;
    Light.z = -10.f;
    sglNormalize(&Light);
    float fCos;
//이미 Normalize를 하기 때문에 /A/*/B/의 값은 1이다. A dot B = /A/*/B/CosQ
//CosQ의 값은 빛의 반사도로 본다. 값. == A dot B / /A/*/B/
    float fMag = sqrtf(pNormal->x*pNormal->x + pNormal->y*pNormal->y + pNormal->z*pNormal->z);
    fCos = (Light.x * pNormal->x + Light.y * pNormal->y + Light.z * pNormal->z) / fMag;
    //fCos * 128.f * 2.0f (음영의값을준다.) + 200.f(기본적으로밝기)
    return (fCos * 120 * fTerrianShadow + 150) / 256.f;
}

//객체가 어디를 부딕쳤는지를 알아온다.
//pAvoidDir 회피 방향 , pToAvoidAngle 회피 각도 (to로 이동하길 원하면 사용하기 바람)
W_COMACT sglWhichCompact(SPoint *ptNow,SVector *vtDir,float fDirAngle,SPoint *ptEnNow,float* pAvoidDir,float* pToAvoidAngle)
{
    W_COMACT Result = W_COMACT_LF;
    SVector vtComDir;
    float fAngle; 
    
    vtComDir.x = ptEnNow->x - ptNow->x;
    vtComDir.y = 0;
    vtComDir.z = -(ptEnNow->z - ptNow->z);
    
    sglNormalize(&vtComDir);
    
    //사이각
    fAngle = AngleArrangeEx(AngleArrangeEx(atan2(vtComDir.z,vtComDir.x) * 180.0 / PI) - fDirAngle);
    
    if(fAngle <= 90) //왼쪽
    {
        if(fAngle <= 45) //앞쪽에 있다.
        {
            *pAvoidDir = 1.0;
            *pToAvoidAngle = AngleArrangeEx(fDirAngle + 5.0f);
            Result = W_COMACT_LF;
            //HLog("AvoidObstacle (%d/%d) LF=오른쪽회전 %f\n",mpSprite->GetID(),mpSprite->GetID(),70.0f);
        }
        else //옆쪽에 있다. (회전)
        { 
            //                        HLogD("왼쪽 앞옆%fAngle\n",fAngle);
            *pAvoidDir = 1.0;
            *pToAvoidAngle = AngleArrangeEx(fDirAngle + fAngle);
            Result = W_COMACT_LFL;
            //HLog("AvoidObstacle (%d/%d) LFF=오른쪽회전 %f\n",mpSprite->GetID(),mpSprite->GetID(),fAngle);
        }
    }
    else if(fAngle > 90 && fAngle <= 180) //왼쪽 뒤
    {
        if(fAngle > 135) //뒤쪽
        {
            *pToAvoidAngle = 0.0f;
            *pAvoidDir = 0.0f;
            Result = W_COMACT_LB;
            //HLog("AvoidObstacle (%d/%d) LB\n",mpSprite->GetID(),mpSprite->GetID());
        }
        else //옆쪽에 있다. (회전)
        { 
            *pAvoidDir = 1.0;
            *pToAvoidAngle = AngleArrangeEx(fDirAngle - 5.0f);
            Result = W_COMACT_LBL;
            //HLog("AvoidObstacle (%d/%d) LBB=오른쪽회전 %f\n",mpSprite->GetID(),mpSprite->GetID(),-15.0f);
        }
    }
    else if(fAngle >180 && fAngle < 270) //오른쪽 뒤
    {
        if(fAngle <= 225) //뒤쪽옆
        {
            Result = W_COMACT_RB;
            *pToAvoidAngle = 0.0f;
            *pAvoidDir = 0.0f;
            //HLog("AvoidObstacle (%d/%d) RB\n",mpSprite->GetID(),mpSprite->GetID());
        }
        else //옆쪽에 있다. (회전)
        { 
            //                        HLogD("오른쪽 뒤옆%fAngle\n",fAngle);
            *pAvoidDir = -1.0;
            *pToAvoidAngle = AngleArrangeEx(fDirAngle - 15.0f);
            Result = W_COMACT_RBR;
            //HLog("AvoidObstacle (%d/%d) RBB=왼쪽회전 %f\n",mpSprite->GetID(),mpSprite->GetID(),-15.0f);
            
        }
    }
    else //오른쪽 앞
    {
        if(fAngle >= 315) //앞쪽에 있다. 
        {
            *pAvoidDir = -1.0;
            *pToAvoidAngle = AngleArrangeEx(fDirAngle - 5.0f);
            Result = W_COMACT_RF;
//            HLog("AvoidObstacle 앞오른쪽 충돌 %f\n",*pToAvoidAngle);
        }
        else //옆쪽에 있다. 
        {
            // HLogD("오른쪽 앞옆%fAngle\n",fAngle);
            *pAvoidDir = -1.0;
            *pToAvoidAngle = AngleArrangeEx(fDirAngle + ((int)fAngle % 45));
            Result = W_COMACT_RFR;
//            HLog("AvoidObstacle 앞오른쪽오른쪽 충둘 %f\n",*pToAvoidAngle);
            
        }
    }
    return Result;
}


void sglGet2DTextureCoord(float fsu,float fsv,float feu,float fev,float fOutText[8])
{
    
    fsv = 1 - fsv;
    fev = 1 - fev;
    
    fOutText[0] = fsu;
    fOutText[1] = fsv;
    
    fOutText[2] = fsu;
    fOutText[3] = fev;
    
    fOutText[4] = feu;
    fOutText[5] = fev;
    
    fOutText[6] = feu;
    fOutText[7] = fsv;
}


void PosParse(char* sData,int* pCnt,int** &pStartPos)
{
    vector<string> lstData;
    char* sOrg;
    char* sTok = new char[strlen(sData) + 1];
    strcpy(sTok,sData);
    sOrg = sTok;
    sTok = strtok(sTok, ";");
    while (sTok)
    {
        lstData.push_back(sTok);
        sTok = strtok(NULL, ";");
    }
    delete[] sOrg;
    
    
    *pCnt = (int)lstData.size();
    pStartPos = new int*[*pCnt];
    for (int i = 0; i < *pCnt; i++)
    {
        pStartPos[i] = new int[3];
        
        sTok = new char[lstData[i].length() + 1];
        strcpy(sTok, lstData[i].c_str());
        sOrg = sTok;
        
        int nIndex = 0;
        sTok = strtok(sTok, ",");
        while (sTok)
        {
            pStartPos[i][nIndex] = atoi(sTok);
            sTok = strtok(NULL, ",");
            nIndex++;
        }
        delete[] sOrg;
    }
}


