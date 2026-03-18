#include <string.h>
#include <stdio.h>
#include <jni.h>

#ifndef _SONGGLJNI_H
#define _SONGGLJNI_H
#ifdef __cplusplus
extern "C" {
#endif
	void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglSetLocalizationDir(JNIEnv * env, jobject thisz,jstring sPath);
	void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglTextUnitInitialize(JNIEnv * env, jobject thisz,jstring sPath);
	void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglTextUnitRelease(JNIEnv * env, jobject thisz);

	void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglSetDisplayType(JNIEnv * env, jobject thisz,jlong jType);

	//초기화를 한다.
	jlong Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglInitialize(JNIEnv * env, jobject thisz);
	
	//LoadResource
	//jint  Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglLoadResource(JNIEnv * env, jobject thisz);

	//리소스를 해제한다.
	void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglRelease(JNIEnv * env, jobject thisz, jlong lGLContext);
	
	//렌더링을 한다.
	void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglRender(JNIEnv * env, jobject thisz, jlong lGLContext);
	
	//View 사이즈가 변경됨
	jint Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglResize(JNIEnv * env, jobject thisz, jlong lGLContext,jint nWidth,jint nHeight);
	
	void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglResetViewPort(JNIEnv * env, jobject thisz,jlong lGLContext);

	jint Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglResource(JNIEnv * env, jobject thisz, jlong lGLContext,jstring sPath);
	
	jboolean Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglEvent(JNIEnv * env, jobject thisz, jlong lGLContext,jint nEventID,jlong param1,jlong param2);
	
	void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglBeginTouch(JNIEnv * env, jobject thisz, jlong lGLContext,jlong id,jfloat fx,jfloat fy);
	void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglMoveTouch(JNIEnv * env, jobject thisz, jlong lGLContext,jlong id,jfloat fx,jfloat fy);
	void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglEndTouch(JNIEnv * env, jobject thisz, jlong lGLContext,jlong id,jfloat fx,jfloat fy);
	
	jint Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglSetTexture(JNIEnv * env, jobject thisz, jlong lGLContext,jstring sKey,jobject bitmap);
	
	void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglResetTexture(JNIEnv * env, jobject thisz, jlong lGLContext);
	void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglPlayBgSoundOnAcitvity(JNIEnv * env, jobject thisz, jboolean bPlay);
	void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglSendMessage(JNIEnv * env, jobject thisz, jint nMsgID,jlong lParam,jlong lParam2,jlong lParam3,jlong lObject);
	void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglSetUserInfo(JNIEnv * env, jobject thisz,jstring sUserID,jstring sMessage);
	void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglSetRateInfo(JNIEnv * env, jobject thisz,jlong nType);

	//Used Game Center
	void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglSendBestScoreNative(JNIEnv * env, jobject thisz);
	void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglAccelertionData(JNIEnv * env, jobject thisz, jlong lGLContext,jfloat x,jfloat y,jfloat z);
	void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglSignInState(JNIEnv * env, jobject thisz,jlong nState);
	void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglOnSigned(JNIEnv * env, jobject thisz,jlong lGLContext,jboolean bSuccess);
	//Added By Song 2015.01.06 멀티플레이를 지원하면서 GetUserInfo를 어디선가 불러쓰기 때문에 시작시에 미리 정의 하자.
	void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglSetRootPath(JNIEnv * env, jobject thisz,jstring sPath);
	void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglSetMultiplayState(JNIEnv * env, jobject thisz,jlong lGLContext,jlong nState);
	void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglResetFindTime(JNIEnv * env, jobject thisz,jlong lGLContext,jlong nWaitSec);
	void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglStartMultiplayGame(JNIEnv * env, jobject thisz,jlong lGLContext);
	void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglStopMultiplayGame(JNIEnv * env, jobject thisz,jlong lGLContext);
	void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglOnReceivedData(JNIEnv * env, jobject thisz,jlong lGLContext,jstring sSender,jbyteArray btDatas);
	void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglSetMyID(JNIEnv * env, jobject thisz,jlong lGLContext,jstring sMyID);
	void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglSetAppNewVersion(JNIEnv * env, jobject thisz,jboolean bNew);

	void Java_kr_co_songs_android_AlienInvasion_SoundMan_sglInitSound(JNIEnv * env, jobject thisz);
	//-------------------------------------------------------------- IAP Version 2
//	void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglAddProductsList(JNIEnv * env, jobject thisz,jobjectArray stringArray);
//	void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglAddProductsListEnd(JNIEnv * env, jobject thisz,jstring sConnID, jstring sTrancID,jstring rcvEvent);
//	void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglErrorProductsList(JNIEnv * env, jobject thisz,jstring sConnID, jstring sTrancID,jstring rcvEvent,jstring sError);
//	void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglResultPurchase(JNIEnv * env, jobject thisz,jstring sProductID, jint bSuccess,jstring sError);
	//-------------------------------------------------------------- IAP Version 2 END


	jint sglGetTextureSub(JNIEnv * env,jlong lGLContext,const char* csKey,jobject bitmap);
	jobject sglGetTextureBitmap(const char* csKey);
	jobject sglMakeBitmapWithText(const char* sText,float fTextSize, int nWidth, int nHeight,float fOffsetX,float fOffsetY,bool bBold);
	jint sglClearBitmapJava(jobject bitmap);
	jint sglUnzip3DResource();
	

	int sglPlayBgSound(const char* sName);
	int sglStopBgSound();
	

	int sglMakeSystemSoundID(const char* sKey);
	int sglCloseSystemSoundID();
	void sglPlaySoundByID(int nID);
	void sglPlayVibrate();
	


	#ifdef ANDROIDAPPSTOREKIT
	//안드로이의 APPSTOREKIT
	bool ShowShopDlg(const char* sProductID,void* pWorld,int nType);
	const char* sglReadCacheProduct();
	#endif //ANDROIDAPPSTOREKIT

	void ShowIDA(bool bShow);
#ifdef ANDROIDADMOB
	int sglSendMessageToJava(int nID,int nParam);
#endif //ANDROIDADMOB

#ifdef __cplusplus
}
#endif

#endif //_SONGGLJNI_H
