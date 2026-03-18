#include <android/bitmap.h>
#include <android/log.h>
#include "SongGLJNI.h"
#include "sGL.h"
#include "CScenario.h"
#include "IHWorld.h"
#include "CProperty.h"
#include "CMutiplyProtocol.h"
extern "C" void FindPlyer(int nResetFindTime,unsigned int nMatchFilter,unsigned int nMatchGroup);
extern "C" bool sglNoFree();
extern "C" void SendUserInfo(void* pUserInfo);
extern "C" void ShowSNSDlg();
extern "C" void InitFindPlayer();
extern "C" void PlayOut();
extern "C" void SendMultiplayData(void* pObject,int nSize, char* sData,bool bReliable);
extern "C" void ShowNewVersionDlg(const char* /*sURL*/); //gsNewAppVersion를 직접 사용한다.

#define  LOG_TAG    "Invasion"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOGN(...)  __android_log_print(ANDROID_LOG_INFO,"InvasionNet",__VA_ARGS__)

static bool gbPlayBG = false;
static string gsLastName;
string gsLocalDir; //다중언어지원함.

#ifdef NOFREE
bool sglNoFree() { return true;}
#else
bool sglNoFree() { return false;}
#endif

#ifdef ANDROIDAPPSTOREKIT
fShowShopDlg gShowShopDlg = NULL;
#endif //ANDROIDAPPSTOREKIT

fShowBestScoreDlg gShowBestScoreDlg = NULL;
extern bool ShowBestScoreDlg(void* pUserInfo);

fShowReviewDlg      gShowReviewDlg = NULL;
extern bool ShowReviewDlg(void* pUserInfo);

//Added 2014.12.05 Function
fShowNewVersionDlg  gShowNewVersionDlg = ShowNewVersionDlg;
fShowSNSDlg         gShowSNSDlg = ShowSNSDlg;
fFindPlyer          gFindPlyer = FindPlyer;
fPlayOut            gPlayOut = PlayOut;
fHWorldEnd          gHWorldEnd = NULL;
fSendMultiplayData  gSendMultiplayData = SendMultiplayData;
fInitFindPlayer     gInitFindPlayer = InitFindPlayer;
fSendUserInfo       gSendUserInfo = SendUserInfo;


JNIEnv * gEnv = 0;
jclass  gjNativesCls = 0;
jmethodID  gjMDID = 0;
JavaVM *gVM = 0;
jlong glGLContext = 0;

//jmethodID  gjReadCacheProduct = 0;

void sglLog ( const char *formatStr, ... )
{
    va_list params;
    char *buf = NULL;//[BUFSIZ];
    va_start ( params, formatStr );
    vasprintf(&buf,formatStr,params);
	fprintf(stderr,"%s", buf);
    va_end ( params );

    LOGI("%s",buf);
    if(buf) free(buf);
}

void sglLogE ( const char *formatStr, ... )
{
	   va_list params;
	    char *buf = NULL;//[BUFSIZ];
	    va_start ( params, formatStr );
	    vasprintf(&buf,formatStr,params);
		fprintf(stderr,"%s", buf);
	    va_end ( params );

	    LOGE("%s",buf);
	    if(buf) free(buf);

}

void sglLogNet ( const char *formatStr, ... )
{
	va_list params;
	char *buf = NULL;//[BUFSIZ];
	va_start ( params, formatStr );
	vasprintf(&buf,formatStr,params);
	fprintf(stderr,"%s", buf);
	va_end ( params );

	LOGN("%s",buf);
	if(buf) free(buf);
}


void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglSetLocalizationDir(JNIEnv * env, jobject thisz,jstring sPath)
{
	jboolean isSucceed;
	const char *csPath = (env)->GetStringUTFChars(sPath, &isSucceed);
	gsLocalDir = csPath;
	(env)->ReleaseStringUTFChars(sPath, csPath);
}

//960보다 크면 아이패드 타입. 아니면 폰타입.
void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglSetDisplayType(JNIEnv * env, jobject thisz,jlong jType)
{
	gnDisplayType = (DISPLAYTYPE)jType;

#ifdef ANDROIDAPPSTOREKIT
	if(gShowShopDlg == NULL)
		gShowShopDlg = ShowShopDlg;
#endif //ANDROIDAPPSTOREKIT
	gShowBestScoreDlg = ShowBestScoreDlg;
	gShowReviewDlg = ShowReviewDlg;
}


//컨텐츠를 초기화한다. World를 만든다.
jlong Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglInitialize(JNIEnv * env, jobject thisz)
{
	SGLContext* plGLContext = NULL;
	plGLContext = sglInitialize(NULL,(void*)thisz);
    if(plGLContext)
    {
    	plGLContext->pViewContext = (void*)thisz;
    	/* theKWorld를 만든다. */
    	plGLContext->pWorld = (void*) (new CScenario);
    }


	return (jlong)plGLContext;
}

void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglTextUnitInitialize(JNIEnv * env, jobject thisz,jstring sPath)
{
	jboolean isSucceed;
	const char *csPath = (env)->GetStringUTFChars(sPath, &isSucceed);

	//언어 파싱을 위해서 추가됨.
	//sglSetRootPath(csPath);
	SGLTextUnit::Initialize();
	(env)->ReleaseStringUTFChars(sPath, csPath);
}

//Added By Song 2015.01.06 멀티플레이를 지원하면서 GetUserInfo를 어디선가 불러쓰기 때문에 시작시에 미리 정의 하자.
void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglSetRootPath(JNIEnv * env, jobject thisz,jstring sPath)
{
	jboolean isSucceed;
	const char *csPath = (env)->GetStringUTFChars(sPath, &isSucceed);
	//언어 파싱을 위해서 추가됨.
	sglSetRootPath(csPath);
	(env)->ReleaseStringUTFChars(sPath, csPath);

}

void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglTextUnitRelease(JNIEnv * env, jobject thisz)
{
	SGLTextUnit::Release();
}


jmethodID  gjHttpRequestXmlOrgID_MDID = 0;
jclass     gjSongGLLibClass = 0; //앞으로 gjNativesCls를 제거하고 gjSongGLLibClass만 사용하게 정리해야 한다.
jmethodID  gjSendMultiplayMDID = 0;
//Draw Render에서 초기화를 해준다.
jint Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglResource(JNIEnv * env, jobject thisz, jlong lGLContext,jstring sPath)
{
	jint nResult = E_SUCCESS;
	SGLContext* sContext = (SGLContext*)lGLContext;
	CScenario* pWorld = (CScenario*)sContext->pWorld;

	////////////////////////////////////////////////
	gEnv = env;
	gEnv->GetJavaVM(&gVM);
	glGLContext = lGLContext;

	gjNativesCls = (gEnv)->FindClass("kr/co/songs/android/AlienInvasion/SongGLLib");
	gjSongGLLibClass = static_cast<jclass>(env->NewGlobalRef(gjNativesCls));

	gjSendMultiplayMDID = (env)->GetStaticMethodID(gjSongGLLibClass,"sglSendMultiplayData","([BI)I" );
	if(gjSendMultiplayMDID == 0)
	{
		HLogE("Error Call C to Java .. not found sglSendMultiplayData\n");
		return 0;
	}

//	if ( gjNativesCls != 0 )
//	{
//		gjReadCacheProduct = (gEnv)->GetStaticMethodID(gjNativesCls,"ReadCacheProduct","()Ljava/lang/String;" );
//	}

//	//쓰레드 안쪽에서 돌지 않게 미리 해주자.
	GetAppVersion();
	GetBundleID();
	GetUniqueID();
	GetModel();
	CountryName();

//	//Thread 에서 사용되는 것은 gEnv가 할성되었을 때 미리 함수를 찾아놓는다.
	if(gjNativesCls != 0)
		gjHttpRequestXmlOrgID_MDID = (gEnv)->GetStaticMethodID(gjNativesCls,"HttpRequestXmlOrg","(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");

	////////////////////////////////////////////////
	jboolean isSucceed;
	const char *csPath = (env)->GetStringUTFChars(sPath, &isSucceed);

	nResult = pWorld->Initialize((void*)csPath);
	(env)->ReleaseStringUTFChars(sPath, csPath);

	return nResult;
}

void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglRelease(JNIEnv * env, jobject thisz, jlong lGLContext)
{
	sglRelease((SGLContext*)lGLContext);
}

int OnBeginRender(void* pThis,SGLContext* pSContext)
{
	return E_SUCCESS;
}

int OnRender(void* pThis,SGLContext* pSContext)
{
	return E_SUCCESS;
}

int OnEndRender(void* pThis,SGLContext* pSContext)
{
	glFlush ();
	return E_SUCCESS;
}


void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglRender(JNIEnv * env, jobject thisz, jlong lGLContext)
{
	sglRender((SGLContext*)lGLContext, OnBeginRender, OnRender, OnEndRender, (void*)thisz);
}

bool gbResetViewPort = false;
jint Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglResize(JNIEnv * env, jobject thisz, jlong lGLContext,jint nWidth,jint nHeight)
{
	SGLContext* sContext = (SGLContext*)lGLContext;
	CScenario* pWorld = (CScenario*)sContext->pWorld;

	jint jResult = sglResize((SGLContext*)lGLContext, nWidth, nHeight,pWorld->GetViewAperture());

	//내부적으로 사이즈가 변경될때만 뷰어의 초기화를 한다.
	if(jResult == 1 ||(pWorld != NULL && pWorld->getResetFrustum()))
	{
		sglProjection(sContext,NULL,(void*)thisz);
		pWorld->ResetFrustum(false);
	}

	if(gbResetViewPort)
	{
	    glViewport (0, 0, nWidth, nHeight);
		sglProjection(sContext,NULL,(void*)thisz);
		pWorld->ResetFrustum(false);
		gbResetViewPort = false;
		HLogD("gbResetViewPort %d,%d",nWidth,nHeight);
	}

	return jResult;
}

void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglResetViewPort(JNIEnv * env, jobject thisz,jlong lGLContext)
{

	SGLContext* sContext = (SGLContext*)lGLContext;
	CScenario* pWorld = (CScenario*)sContext->pWorld;
	glViewport (0, 0, 1, 1);
	if(pWorld != NULL && pWorld->getResetFrustum())
	{
		sglProjection(sContext,NULL,(void*)thisz);
		pWorld->ResetFrustum(false);
	}
	gbResetViewPort = true;

	HLogD("sglResetViewPort");
}

jboolean Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglEvent(JNIEnv * env, jobject thisz, jlong lGLContext,jint nEventID,jlong param1,jlong param2)
{
	jboolean bResult = true;

//	SGLEvent Event;
//	Event.nMsgID = nEventID;
//	Event.lParam = param1;
//	Event.lObject = param2;

	//SGLContext* sContext = (SGLContext*)lGLContext;
	//CScenario* pWorld = (CScenario*)sContext->pWorld;
	//if(pWorld)
	//{
		//pWorld->OnEvent(&Event);
	//}

	CScenario::SendMessage(nEventID,param1,0,0,param2);
	return bResult;
}

void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglBeginTouch(JNIEnv * env, jobject thisz, jlong lGLContext,jlong id,jfloat fx,jfloat fy)
{
	SGLContext* sContext = (SGLContext*)lGLContext;
	IHWorld* pWorld = (IHWorld*)sContext->pWorld;
	if(pWorld == NULL) return ;
	pWorld->BeginTouch(id,fx,fy);
}
void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglMoveTouch(JNIEnv * env, jobject thisz, jlong lGLContext,jlong id,jfloat fx,jfloat fy)
{
	SGLContext* sContext = (SGLContext*)lGLContext;
	IHWorld* pWorld = (IHWorld*)sContext->pWorld;
	if(pWorld == NULL) return ;
	pWorld->MoveTouch(id,fx,fy);
}


void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglEndTouch(JNIEnv * env, jobject thisz, jlong lGLContext,jlong id,jfloat fx,jfloat fy)
{
	SGLContext* sContext = (SGLContext*)lGLContext;
	IHWorld* pWorld = (IHWorld*)sContext->pWorld;
	if(pWorld == NULL) return ;
	pWorld->EndTouch(id,fx,fy);

}


jint Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglSetTexture(JNIEnv * env, jobject thisz, jlong lGLContext,jstring sKey,jobject bitmap)
{
	jboolean isSucceed;
	const char *csKey = (env)->GetStringUTFChars(sKey, &isSucceed);
	jint nResult =  sglGetTextureSub(env,lGLContext,csKey,bitmap);
	(env)->ReleaseStringUTFChars(sKey,csKey);
	return nResult;
}

void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglResetTexture(JNIEnv * env, jobject thisz, jlong lGLContext)
{
	SGLContext* sContext = (SGLContext*)lGLContext;
	IHWorld* pWorld = (IHWorld*)sContext->pWorld;
	if(pWorld == NULL) return ;


	//OnPause할때 sglStopBgSound 함수를 부루면 에러가 난다. gEnv가 다른 스레드이다.
	//즉 gEnv의 쓰레드 영역이 메인스레드이고 현재 스레드 영역은 opengl영역안에 있어서 gEnv의 OpenGL영역으로 변경해준다.
	gEnv = env;
	pWorld->ResetTexture();
}



//OnPause할때 sglStopBgSound 함수를 부루면 에러가 난다.
//gEnv가 다른 스레드이다.
//Activity에서 OnResume이나. OnPause를 할때 배경 음악을 종료하거나 다시 재실행해줄때 사용한다.
//그이유는 미리 플레이를 종료/설정해주어야 C=>JAVA의 에러가 나지 않음.
void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglPlayBgSoundOnAcitvity(JNIEnv * env, jobject thisz, jboolean bPlay)
{
	//OnPause할때 sglStopBgSound 함수를 부루면 에러가 난다. gEnv가 다른 스레드이다.
	//그래서 gEnv를 새로 할당해준다.
	gEnv = env;


	//HLogD("== sglPlayBgSoundOnAcitvity %d",bPlay);
	if(bPlay == true && gsLastName.length() != 0)
	{
		CUserInfo* ui = CUserInfo::GetDefaultUserInfo();
		if(ui->GetMusic() == 1)
			sglPlayBgSound(gsLastName.c_str());
	}
	else if(bPlay == false)
		sglStopBgSound();
	//HLogD("== sglPlayBgSoundOnAcitvity %d End",bPlay);
}

void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglSendMessage(JNIEnv * env, jobject thisz, jint nMsgID,jlong lParam,jlong lParam2,jlong lParam3,jlong lObject)
{
	CScenario::SendMessage(nMsgID,lParam,lParam2,lParam3,lObject);
}

void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglSetUserInfo(JNIEnv * env, jobject thisz,jstring sUserID,jstring sMessage)
{
	jboolean isSucceed;
	const char *csUserID = (env)->GetStringUTFChars(sUserID, &isSucceed);
	const char *csMessage = (env)->GetStringUTFChars(sMessage, &isSucceed);
	CUserInfo* userInfo = CUserInfo::GetDefaultUserInfo();
	userInfo->SetUserID(csUserID);
	userInfo->SetYourMessage(csMessage);
	userInfo->Save();
	(env)->ReleaseStringUTFChars(sUserID,csUserID);
	(env)->ReleaseStringUTFChars(sMessage,csMessage);
}

void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglSetRateInfo(JNIEnv * env, jobject thisz,jlong nType)
{
	jboolean isSucceed;

	CUserInfo* userInfo = CUserInfo::GetDefaultUserInfo();
	if(nType == 0)
		userInfo->SetReport(0);
	else if(nType == 1) //평가 No
	{
		userInfo->SetReport(userInfo->GetRank() + 2); //나중에 3마다 메세지가 뜨게 한다.
		#define BTN_NEXT            1006
		CScenario::SendMessage(SGL_MESSAGE_CLICKED,BTN_NEXT); //게임을 바로 시작한다.
	}
	else //평가 Yes
		userInfo->SetReport(-1);

	userInfo->Save();
}

void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglAccelertionData(JNIEnv * env, jobject thisz, jlong lGLContext,jfloat x,jfloat y,jfloat z)
{
	SGLContext* sContext = (SGLContext*)lGLContext;
	IHWorld* pWorld = (IHWorld*)sContext->pWorld;
	if(pWorld == NULL) return ;
	pWorld->SetAccelatorUpVector(x);
}


int gnARate = 0;
int gnDRate = 0;
int gnTRate = 0;
int gnMTRate = 0;
JNIEnv* gsglSendBestScoreNativeEnv = 0; //SendUserInfo를 콜할때 자바에서 콜하기 때문에 자바에서 콜하는 env를 사용하여야 한다.
void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglSendBestScoreNative(JNIEnv * env, jobject thisz)
{
	HLogD("Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglSendBestScoreNative");
    CUserInfo* userInfo = (CUserInfo*)CUserInfo::GetDefaultUserInfo();
    if(gnARate == 0) gnARate = userInfo->GetCntByIDWithItem(ID_ITEM_UPGRADE_ATTACK);
    if(gnDRate == 0) gnDRate = userInfo->GetCntByIDWithItem(ID_ITEM_UPGRADE_DEPEND);
    if(gnTRate == 0) gnTRate = userInfo->GetCntByIDWithItem(ID_ITEM_UPGRADE_DEFENCE_TOWER);
    if(gnMTRate == 0) gnMTRate = userInfo->GetCntByIDWithItem(ID_ITEM_UPGRADE_MISSILE_TOWER);

    gsglSendBestScoreNativeEnv = env;
	SendUserInfo((void*)userInfo);
}

void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglSignInState(JNIEnv * env, jobject thisz,jlong nState)
{
	CUserInfo* userInfo = (CUserInfo*)CUserInfo::GetDefaultUserInfo();
	userInfo->SetGameCenterStatus(nState);
}

void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglOnSigned(JNIEnv * env, jobject thisz,jlong lGLContext,jboolean bSuccess)
{
	CUserInfo* userInfo = (CUserInfo*)CUserInfo::GetDefaultUserInfo();
	if(bSuccess)
	{
		 SGLContext* sContext = (SGLContext*)lGLContext;
		 CScenario* pWorld = (CScenario*)sContext->pWorld;
		userInfo->SetGameCenterStatus(2);
		CMutiplyProtocol* proto = pWorld->GetMutiplyProtocol();
//		proto->SetAlias(localPlayer.alias.UTF8String);
//		[self SubmitScore];
//		[self.mGCMan installInvitation];

		//이전에 파인딩을 하였다면, 몇초후에 자동으로 시작하는 것을 막자.
		if(pWorld->GetWorldType() == WORLD_TYPE_CScenario)
		{
			CScenario* pS = (CScenario*)pWorld;
			if(pS->GetCurrentWorld()->GetWorldType() == WORLD_TYPE_MENU)
				proto->SetFindWaitTime(0l);

		}
		if(pWorld->GetWorldType() == WORLD_TYPE_MENU)
			proto->SetFindWaitTime(0l);
	}
	else
		userInfo->SetGameCenterStatus(0);

}


void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglSetMultiplayState(JNIEnv * env, jobject thisz,jlong lGLContext,jlong nState)
{
	SGLContext* sContext = (SGLContext*)lGLContext;
	CScenario* pWorld = (CScenario*)sContext->pWorld;
	HLogD("sglSetMultiplayStat %d",(long)nState);
	pWorld->GetMutiplyProtocol()->SetMultiplayState((MultiplyState)(long)nState);
}


void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglResetFindTime(JNIEnv * env, jobject thisz,jlong lGLContext,jlong nWaitSec)
{
	HLogD("sglResetFindTime %d",(int)nWaitSec);
	SGLContext* sContext = (SGLContext*)lGLContext;
	CScenario* pWorld = (CScenario*)sContext->pWorld;
    CMutiplyProtocol* proto = pWorld->GetMutiplyProtocol();
    if(nWaitSec)
        proto->SetFindWaitTime(GetGGTime() + (long)nWaitSec * 1000l);
    else
        proto->SetFindWaitTime(nWaitSec);
}



void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglStartMultiplayGame(JNIEnv * env, jobject thisz,jlong lGLContext)
{
//	gjSendMultiplayCls = (env)->FindClass("kr/co/songs/android/AlienInvasion/SongGLLib");
//	if ( gjSendMultiplayCls == 0 )
//	{
//		HLogE("Error Call C to Java .. not found kr/co/songs/android/AlienInvasion/SongGLLib Class\n");
//		return ;
//	}
//
//	gjSendMultiplayMDID = (env)->GetStaticMethodID(gjSendMultiplayCls,"sglSendMultiplayData","([BI)I" );
//	if(gjSendMultiplayMDID == 0)
//	{
//		HLogE("Error Call C to Java .. not found sglSendMultiplayData\n");
//		return ;
//	}

	SGLContext* sContext = (SGLContext*)lGLContext;
	CScenario* pWorld = (CScenario*)sContext->pWorld;
	CMutiplyProtocol* proto = pWorld->GetMutiplyProtocol();
	proto->Start();

}
void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglStopMultiplayGame(JNIEnv * env, jobject thisz,jlong lGLContext)
{
	SGLContext* sContext = (SGLContext*)lGLContext;
	CScenario* pWorld = (CScenario*)sContext->pWorld;
	CMutiplyProtocol* proto = pWorld->GetMutiplyProtocol();
	proto->Stop();
}

void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglOnReceivedData(JNIEnv * env, jobject thisz,jlong lGLContext,jstring sSender,jbyteArray btDatas)
{
	jsize nSize = (env)->GetArrayLength(btDatas);
	jbyte *pbyte = (env)->GetByteArrayElements(btDatas, 0);
	jboolean isSucceed;
	if(nSize == 0) return;
	const char *csSender = (env)->GetStringUTFChars(sSender, &isSucceed);
	SGLContext* sContext = (SGLContext*)lGLContext;
	CScenario* pWorld = (CScenario*)sContext->pWorld;
	CMutiplyProtocol* proto = pWorld->GetMutiplyProtocol();
	if(pbyte)
		proto->ReceiveData(csSender,(int)nSize,(char*)pbyte);

	(env)->ReleaseByteArrayElements(btDatas, pbyte, JNI_ABORT);
	(env)->ReleaseStringUTFChars(sSender, csSender);
}

void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglSetMyID(JNIEnv * env, jobject thisz,jlong lGLContext,jstring sMyID)
{
	jboolean isSucceed;
	const char *csMyID = (env)->GetStringUTFChars(sMyID, &isSucceed);

	SGLContext* sContext = (SGLContext*)lGLContext;
	CScenario* pWorld = (CScenario*)sContext->pWorld;
	CMutiplyProtocol* proto = pWorld->GetMutiplyProtocol();
	proto->SetAlias(csMyID);

	(env)->ReleaseStringUTFChars(sMyID, csMyID);
}

void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglSetAppNewVersion(JNIEnv * env, jobject thisz,jboolean bNew)
{
	extern char* gsNewAppVersion;
	if(bNew && gsNewAppVersion == NULL)
	{
		gsNewAppVersion = new char[10];
		strcpy(gsNewAppVersion,"New");
	}

}

jint sglGetTextureSub(JNIEnv * env,jlong lGLContext,const char* csKey,jobject bitmap)
{
	unsigned char*              pixels;
	jint nResult = 0;
	AndroidBitmapInfo  info;

	if ((nResult = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
        LOGE("AndroidBitmap_getInfo() failed ! error=%d", nResult);
        return 0;
    }

    if ((nResult = AndroidBitmap_lockPixels(env, bitmap, (void**)&pixels)) < 0) {
        LOGE("AndroidBitmap_lockPixels() failed ! error=%d", nResult);
        return 0;
    }

    SGLContext* sContext = (SGLContext*)lGLContext;
    CScenario* pWorld = (CScenario*)sContext->pWorld;

	if(pWorld == NULL) return 0;

    nResult = pWorld->SetCurrentTexture(
                      csKey,
                      pixels,
                      info.width,
                      info.height,
                      info.format );

    AndroidBitmap_unlockPixels(env, bitmap);

     return nResult;
}


jobject sglGetTextureBitmap(const char* csKey)
{

	gjNativesCls = (gEnv)->FindClass("kr/co/songs/android/AlienInvasion/SongGLLib");
	if ( gjNativesCls == 0 )
	{
		HLogE("Error Call C to Java .. not found kr/co/songs/android/AlienInvasion/SongGLLib Class\n");
		return 0;
	}

	gjMDID = (gEnv)->GetStaticMethodID(gjNativesCls,"sglGetBitmapJava","(Ljava/lang/String;)Landroid/graphics/Bitmap;" );
	if(gjMDID == 0)
	{
		HLogE("Error Call C to Java .. not found sglGetBitmapJava\n");
		return 0;
	}

	jstring sKey = (gEnv)->NewStringUTF(csKey); // convert char array to jstring

	//리턴값이 Object이면 ObjectMethod
	jobject bitmap = (gEnv)->CallStaticObjectMethod(gjNativesCls,gjMDID,sKey);
	if(bitmap == 0)
	{
		HLogE("not found sglGetBitmapJava %s\n",csKey);
		//gVM->DetachCurrentThread();
		return 0;
	}
    return bitmap;
}

jobject sglMakeBitmapWithText(const char* sText, float fTextSize, int nWidth, int nHeight,float fOffsetX,float fOffsetY,bool bBold)
{

	gjNativesCls = (gEnv)->FindClass("kr/co/songs/android/AlienInvasion/SongGLLib");
	if ( gjNativesCls == 0 )
	{
		HLogE("Error Call C to Java .. not found kr/co/songs/android/AlienInvasion/SongGLLib Class\n");
		return 0;
	}

	gjMDID = (gEnv)->GetStaticMethodID(gjNativesCls,"sglMakeBitmapWithText","(Ljava/lang/String;FIIFFZ)Landroid/graphics/Bitmap;" );
	if(gjMDID == 0)
	{
		HLogE("Error Call C to Java .. not found sglMakeBitmapWithText\n");
		return 0;
	}
	jstring jsText = (gEnv)->NewStringUTF(sText); // convert char array to jstring

	//리턴값이 Object이면 ObjectMethod
	jobject bitmap = (gEnv)->CallStaticObjectMethod(gjNativesCls,gjMDID,jsText,fTextSize,nWidth,nHeight,fOffsetX,fOffsetY,bBold);
	if(bitmap == 0)
	{
		HLogE("not found sglMakeBitmapWithText %s\n",sText);
		return 0;
	}
	return bitmap;
}

jint sglClearBitmapJava(jobject bitmap)
{
	gjMDID = (gEnv)->GetStaticMethodID(gjNativesCls,"sglClearBitmapJava","(Landroid/graphics/Bitmap;)I" );
	if(gjMDID == 0)
	{
		HLogE("Error Call C to Java .. not found sglClearBitmapJava\nç");
		return 0;
	}
	//리턴값이 int이 IntMethod
	jint nResult = (gEnv)->CallStaticIntMethod(gjNativesCls,gjMDID,bitmap);
	return nResult;
}

jint sglUnzip3DResource()
{
	//Added By Song 2013.08.19 무슨이유인지 모르나. gjNativesCls값이 변경되어서 에러가 난다.
	gjNativesCls = (gEnv)->FindClass("kr/co/songs/android/AlienInvasion/SongGLLib");
	if ( gjNativesCls == 0 )
	{
		HLogE("Error Call C to Java .. not found kr/co/songs/android/AlienInvasion/SongGLLib  in sglUnzip3DResource\n");
		return 0;
	}

	gjMDID = (gEnv)->GetStaticMethodID(gjNativesCls,"sglUnzip3DResource","()I" );
	if(gjMDID == 0)
	{
		HLogE("Error Call C to Java .. not found sglUnzip3DResource\n");
		return 0;
	}
	//리턴값이 int이 IntMethod
	jint nResult = (gEnv)->CallStaticIntMethod(gjNativesCls,gjMDID);

	//2019.02.12
	extern bool gbUngizInit;
	gbUngizInit = true;
	return nResult;

}


//Sound ----------------------------------------------------------------
jclass  gjSoundCls = 0;
jmethodID  gjSoundMDID = 0;
jmethodID  gjPlaySoundByID_MDID = 0;
jmethodID  gjPlayVibrate_MDID = 0;
jmethodID  gjCloseSystemSoundID = 0;
jmethodID  gjPlayBgSound = 0;
jmethodID  gjStopBgSound = 0;
JNIEnv *AttachJNIEnv(bool* pOutIsAttached) {
	int status;
	JNIEnv * currentEnv = NULL;
	*pOutIsAttached = false;
	status = gVM->GetEnv((void **) &currentEnv, JNI_VERSION_1_4);
	if (status < 0) {
//		JavaVMAttachArgs args;
//		args.version = JNI_VERSION_1_4;
//		args.name = "CoreThread";
//		args.group = NULL;
//		status = gVM->AttachCurrentThread(&currentEnv, &args);
		status = gVM->AttachCurrentThread(&currentEnv, NULL);
		if (status < 0) {
			LOGE("GetJNIEnv: failed to attach current thread");
			return NULL;
		}
		*pOutIsAttached = true;
	}
	return currentEnv;
}

void DetachJNIEnv(bool IsAttached) {
	if (IsAttached)
		gVM->DetachCurrentThread();
}

int JNI_OnLoad(JavaVM* vm, void* reserved) {
	JNIEnv *env;
//	gvm = vm;
	LOGI("JNI_OnLoad called");
	if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
		LOGE("Failed to get the environment using GetEnv()");
		return -1;
	}
	return JNI_VERSION_1_4;
}

void Java_kr_co_songs_android_AlienInvasion_SoundMan_sglInitSound(JNIEnv * env, jobject thisz)
{
	HLogD("sglInitSound");

	jclass localSoundManClass = (env)->FindClass("kr/co/songs/android/AlienInvasion/SoundMan");
	if ( localSoundManClass == 0 )
	{
		HLogE("Error Call C to Java .. not found kr/co/songs/android/AlienInvasion/SoundMan Class\n");
		return ;
	}

	gjSoundCls = static_cast<jclass>(env->NewGlobalRef(localSoundManClass));

	//Thread 에서 사용되는 것은 gEnv가 할성되었을 때 미리 함수를 찾아놓는다.
	gjPlaySoundByID_MDID = (env)->GetStaticMethodID(gjSoundCls,"PlaySoundByID","(I)I" );
	if(gjPlaySoundByID_MDID == 0)
	{
		HLogE("Error Call C to Java .. not found PlaySoundByID\n");
		return ;
	}

	gjPlayVibrate_MDID = (env)->GetStaticMethodID(gjSoundCls,"PlayVibrate","()I" );
	if(gjPlayVibrate_MDID == 0)
	{
		HLogE("Error Call C to Java .. not found PlayVibrate\n");
		return ;
	}

	gjSoundMDID = (env)->GetStaticMethodID(gjSoundCls,"MakeSystemSoundID","(Ljava/lang/String;)I" );
	if(gjSoundMDID == 0)
	{
		HLogE("Error Call C to Java .. not found MakeSystemSoundID\n");
		return ;
	}


	gjCloseSystemSoundID = (env)->GetStaticMethodID(gjSoundCls,"CloseSystemSoundID","()I" );
	if(gjCloseSystemSoundID == 0)
	{
		HLogE("Error Call C to Java .. not found ClearSystemSoundID\n");
		return ;
	}

	gjPlayBgSound = (env)->GetStaticMethodID(gjSoundCls,"PlayBgSound","(Ljava/lang/String;)I" );
	if(gjPlayBgSound == 0)
	{
		HLogE("Error Call C to Java .. not found PlayBgSound\n");
		return ;
	}

	gjStopBgSound = (env)->GetStaticMethodID(gjSoundCls,"StopBgSound","()I" );
	if(gjStopBgSound == 0)
	{
		HLogE("Error Call C to Java .. not found PlayBgSound\n");
		return ;
	}

	HLogD("sglInitSound End");
}


int sglMakeSystemSoundID(const char* sKey)
{
	HLog("sglMakeSystemSoundID");
	bool isAttached = false;
	JNIEnv * currentEnv = NULL;
	currentEnv = AttachJNIEnv(&isAttached);
	if (currentEnv == NULL)
		return 0;

	jstring jsName = (currentEnv)->NewStringUTF(sKey); // convert char array to jstring
	//리턴값이 int이 IntMethod
	jint nResult = (currentEnv)->CallStaticIntMethod(gjSoundCls,gjSoundMDID,jsName);


	DetachJNIEnv(isAttached);
	HLog("sglMakeSystemSoundID End");


	return nResult;
}



void sglPlayVibrate()
{

	HLog("sglPlayVibrate");

	bool isAttached = false;
	JNIEnv * currentEnv = NULL;
	currentEnv = AttachJNIEnv(&isAttached);
	if (currentEnv == NULL)
		return ;

	//리턴값이 int이 IntMethod
	(currentEnv)->CallStaticIntMethod(gjSoundCls,gjPlayVibrate_MDID);

	DetachJNIEnv(isAttached);

	HLog("sglPlayVibrate End");
}

int sglCloseSystemSoundID()
{
	HLog("sglCloseSystemSoundID");
	bool isAttached = false;
	JNIEnv * currentEnv = NULL;
	currentEnv = AttachJNIEnv(&isAttached);
	if (currentEnv == NULL)
		return 0;

	//리턴값이 int이 IntMethod
	jint nResult = (currentEnv)->CallStaticIntMethod(gjSoundCls,gjCloseSystemSoundID);

	DetachJNIEnv(isAttached);

	HLog("sglCloseSystemSoundID End");
	return nResult;
}




//싸운드 플레이는 쓰레드 안쪽에서도 실행되고 메인쓰레드에서도 실행되는 형태이다.
//그래서 AttachCurrentThread 함수를 사용한다.
void sglPlaySoundByID(int nID)
{

	HLog("sglPlaySoundByID");
	bool isAttached = false;
	JNIEnv * currentEnv = NULL;
	currentEnv = AttachJNIEnv(&isAttached);
	if (currentEnv == NULL)
		return ;

	//리턴값이 int이 IntMethod
	(currentEnv)->CallStaticIntMethod(gjSoundCls,gjPlaySoundByID_MDID,nID);

	DetachJNIEnv(isAttached);

	HLog("sglPlaySoundByID End");

}


int sglPlayBgSound(const char* sName)
{
	//HLogD("--sglPlayBgSound");

	//이미 같은 것을 플레이 했으면 플레이 하지자 않음.
	if(gbPlayBG == true && gsLastName.compare(sName) == 0) return 0;

	HLog("sglPlayBgSound");
	bool isAttached = false;
	JNIEnv * currentEnv = NULL;
	currentEnv = AttachJNIEnv(&isAttached);
	if (currentEnv == NULL)
		return 0;

	jstring jsName = (currentEnv)->NewStringUTF(sName); // convert char array to jstring
	//리턴값이 int이 IntMethod
	jint nResult = (currentEnv)->CallStaticIntMethod(gjSoundCls,gjPlayBgSound,jsName);

	gbPlayBG = true;
	gsLastName = sName;

	DetachJNIEnv(isAttached);
	HLog("sglPlayBgSound End");
	return nResult;
}

int sglStopBgSound()
{
	HLog("sglStopBgSound");
	bool isAttached = false;
	JNIEnv * currentEnv = NULL;
	currentEnv = AttachJNIEnv(&isAttached);
	if (currentEnv == NULL)
		return 0;

	if(gbPlayBG == false) return 0; //이미 종료 되었으면 다시 불러오지 않음.
	//HLogD("--sglStopBgSound End");

	//리턴값이 int이 IntMethod
	jint nResult = (currentEnv)->CallStaticIntMethod(gjSoundCls,gjStopBgSound);
	//HLogD("--sglStopBgSound End2");

	gbPlayBG = false;

	DetachJNIEnv(isAttached);
	HLog("sglStopBgSound End");
	return nResult;
}




//-------------------------------------------------------------- IAP Version 2
//void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglAddProductsList(JNIEnv * env,
//		jobject thisz, jobjectArray stringArray) {
////
////	if (gpWorldMan == NULL)
////		return;
////
////	int stringCount = (env)->GetArrayLength(stringArray);
////	if (gNewRecordSet == NULL) {
////		gNewRecordSet = new CProductsRecordSet(GetNewID(), gpWorldMan);
////	}
////
////	VArray* p = new VArray;
////	for (int i = 0; i < stringCount; i++) {
////		jstring string = (jstring)(env)->GetObjectArrayElement(stringArray, i);
////		const char *rawString = (env)->GetStringUTFChars(string, 0);
////		VData v;
////		v = rawString;
////		p->Add(v);
////		(env)->ReleaseStringUTFChars(string, rawString);
////	}
////	gNewRecordSet->AppendRowdatas(p);
//}
//
//void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglAddProductsListEnd(JNIEnv * env,
//		jobject thisz, jstring sConnID, jstring sTrancID, jstring rcvEvent) {
////	if (gpWorldMan == NULL)
////		return;
////
////	const char *cssTrancID = (env)->GetStringUTFChars(sTrancID, 0);
////	const char *csrcvEvent = (env)->GetStringUTFChars(rcvEvent, 0);
////	const char *cssConnID = (env)->GetStringUTFChars(sConnID, 0);
////	gNewRecordSet->SetName(cssTrancID);
////	gNewRecordSet->SetFieldName("ID", 0, 0);
////	gNewRecordSet->SetFieldName("Title", 1, 0);
////	gNewRecordSet->SetFieldName("Desc", 2, 0);
////	gNewRecordSet->SetFieldName("Price", 3, 5);
////	gNewRecordSet->SetFieldName("LocalPrice", 4, 0);
////
////	ARender* pRcvEvent = gpWorldMan->FindRender(csrcvEvent);
////	CProductsCon* pConn = (CProductsCon*) gpWorldMan->GetGlobalObjects(
////			cssConnID);
////	pConn->OnPostQuery(cssTrancID, gNewRecordSet, pRcvEvent);
////	(env)->ReleaseStringUTFChars(sTrancID, cssTrancID);
////	(env)->ReleaseStringUTFChars(rcvEvent, csrcvEvent);
////	(env)->ReleaseStringUTFChars(sConnID, cssConnID);
////
////	if (gNewRecordSet) {
////		gNewRecordSet = NULL;
////	}
//}
//
//void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglErrorProductsList(JNIEnv * env,
//		jobject thisz, jstring sConnID, jstring sTrancID, jstring rcvEvent,
//		jstring sError) {
////	const char *cssTrancID = (env)->GetStringUTFChars(sTrancID, 0);
////	const char *csrcvEvent = (env)->GetStringUTFChars(rcvEvent, 0);
////	const char *cssConnID = (env)->GetStringUTFChars(sConnID, 0);
////	const char *cssError = (env)->GetStringUTFChars(sError, 0);
////	ARender* pRcvEvent = gpWorldMan->FindRender(csrcvEvent);
////	CProductsCon* pConn = (CProductsCon*) gpWorldMan->GetGlobalObjects(
////			cssConnID);
////
////	pConn->OnError(cssTrancID, -1, cssError, pRcvEvent);
////
////	(env)->ReleaseStringUTFChars(sTrancID, cssTrancID);
////	(env)->ReleaseStringUTFChars(rcvEvent, csrcvEvent);
////	(env)->ReleaseStringUTFChars(sConnID, cssConnID);
////	(env)->ReleaseStringUTFChars(sError, cssError);
////
////	if (gNewRecordSet) {
////		delete gNewRecordSet;
////		gNewRecordSet = NULL;
////	}
//}
//
//
//void Java_kr_co_songs_android_AlienInvasion_SongGLLib_sglResultPurchase(JNIEnv * env,
//		jobject thisz, jstring sProductID, jint bSuccess, jstring sError)
//{
////	if (gpWorldMan == NULL)
////		return;
////
////	const char *cssProductID = (env)->GetStringUTFChars(sProductID, 0);
////	const char *cssError = (env)->GetStringUTFChars(sError, 0);
////
////	if (gpWorldMan->GetScript()) {
////		CScript* pScript = gpWorldMan->GetScript();
////		if (bSuccess) {
////			HLog("Success IAP");
////			pScript->AddItemData("_sender_", cssProductID);
////			pScript->AddItemData("_result_", 1);
////			pScript->AddItemData("_count_", 1);
////			pScript->AddItemData("_restore_", false);
////		} else {
////			HLog("Failed IAP");
////			pScript->AddItemData("_sender_", cssProductID);
////			pScript->AddItemData("_result_", 0);
////			pScript->AddItemData("_msg_", cssError);
////		}
////		pScript->RunEvent(eventAppstoreResult);
////	}
////	(env)->ReleaseStringUTFChars(sProductID, cssProductID);
////	(env)->ReleaseStringUTFChars(sError, cssError);
//
//}
//-------------------------------------------------------------- IAP Version 2 END

#ifdef ANDROIDAPPSTOREKIT
//안드로이의 APPSTOREKIT
bool ShowShopDlg(const char* sProductID,void* pWorld,int nType)
{
	gjNativesCls = (gEnv)->FindClass("kr/co/songs/android/AlienInvasion/SongGLLib");
	if ( gjNativesCls == 0 )
	{
		HLogE("Error Call C to Java .. not found kr/co/songs/android/AlienInvasion/SongGLLib Class\n");
		return false;
	}

	gjMDID = (gEnv)->GetStaticMethodID(gjNativesCls,"sglRequestProductID","(Ljava/lang/String;I)I" );
	if(gjMDID == 0)
	{
		HLogE("Error Call C to Java .. not found sglRequestProductID\n");
		return false;
	}

	jstring jsProductID = (gEnv)->NewStringUTF(sProductID); // convert char array to jstring

	//리턴값이 Object이면 ObjectMethod
	(gEnv)->CallStaticIntMethod(gjNativesCls,gjMDID,jsProductID,nType);

	return true;
}

const char* sglReadCacheProduct()
{
	static string sID;
	jint nResult = 0;
	gjNativesCls = (gEnv)->FindClass("kr/co/songs/android/AlienInvasion/SongGLLib");
	if ( gjNativesCls == 0 )
	{
		HLogE("Error Call C to Java .. not found kr/co/songs/android/AlienInvasion/SongGLLib Class\n");
		return 0;
	}

	gjMDID = (gEnv)->GetStaticMethodID(gjNativesCls,"ReadCacheProduct","()Ljava/lang/String;" );
	if(gjMDID == 0)
	{
		HLogE("Error Call C to Java .. not found ReadCacheProduct\n");
		return 0;
	}

	//리턴값이 Object이면 ObjectMethod
	jstring sData = (jstring)(gEnv)->CallStaticObjectMethod(gjNativesCls,gjMDID);

	jboolean isSucceed;
	const char *csResult = (gEnv)->GetStringUTFChars(sData, &isSucceed);
	sID = csResult;
	(gEnv)->ReleaseStringUTFChars(sData, csResult);


	return sID.c_str();
}
#endif //ANDROIDAPPSTOREKIT


void ShowIDA(bool bShow)
{
#ifdef ANDROIDADMOB
	if(bShow == false)
		sglSendMessageToJava(0,0); //자바 내부에서 GSL_PROGRESS_START를 콜한다.
	else
		sglSendMessageToJava(1,0); //자바 내부에서 GSL_PROGRESS_START를 콜한다.
#else
	//무시한다...
#endif
}

#ifdef ANDROIDADMOB
int sglSendMessageToJava(int nID,int nParam)
{
	jint nResult = 0;
	gjNativesCls = (gEnv)->FindClass("kr/co/songs/android/AlienInvasion/SongGLLib");
	if ( gjNativesCls == 0 )
	{
		HLogE("Error Call C to Java .. not found kr/co/songs/android/AlienInvasion/SongGLLib Class\n");
		return 0;
	}

	gjMDID = (gEnv)->GetStaticMethodID(gjNativesCls,"SendMessageToJava","(II)I" );
	if(gjMDID == 0)
	{
		HLogE("Error Call C to Java .. not found sglShowAdView\n");
		return 0;
	}
	//리턴값이 Object이면 ObjectMethod
	return (gEnv)->CallStaticIntMethod(gjNativesCls,gjMDID,nID,nParam);
}
#endif //ANDROIDADMOB

//End Sound ----------------------------------------------------------------


const char* CountryName()
{
    static string sID;
    if(sID.length()) return sID.c_str();

	gjNativesCls = (gEnv)->FindClass("kr/co/songs/android/AlienInvasion/SongGLLib");
	if ( gjNativesCls == 0 )
	{
		HLogE("Error Call C to Java .. not found kr/co/songs/android/AlienInvasion/SongGLLib  in GetUniqueID\n");
		return 0;
	}

	gjMDID = (gEnv)->GetStaticMethodID(gjNativesCls,"CountryName","()Ljava/lang/String;" );
	if(gjMDID == 0)
	{
		HLogE("Error Call C to Java .. not found CountryName\n");
		return 0;
	}
	//리턴값이 int이 IntMethod
	jboolean isSucceed;
	jstring sResult = (jstring)(gEnv)->CallStaticObjectMethod(gjNativesCls,gjMDID);
	const char *csResult = (gEnv)->GetStringUTFChars(sResult, &isSucceed);
	sID = csResult;
	(gEnv)->ReleaseStringUTFChars(sResult, csResult);

    return sID.c_str();
}

const char* GetUniqueID()
{
    static string sID;
    if(sID.length()) return sID.c_str();

	gjNativesCls = (gEnv)->FindClass("kr/co/songs/android/AlienInvasion/SongGLLib");
	if ( gjNativesCls == 0 )
	{
		HLogE("Error Call C to Java .. not found kr/co/songs/android/AlienInvasion/SongGLLib  in GetUniqueID\n");
		return 0;
	}

	gjMDID = (gEnv)->GetStaticMethodID(gjNativesCls,"GetUniqueID","()Ljava/lang/String;" );
	if(gjMDID == 0)
	{
		HLogE("Error Call C to Java .. not found GetUniqueID\n");
		return 0;
	}
	//리턴값이 int이 IntMethod
	jboolean isSucceed;
	jstring sResult = (jstring)(gEnv)->CallStaticObjectMethod(gjNativesCls,gjMDID);
	const char *csResult = (gEnv)->GetStringUTFChars(sResult, &isSucceed);
	sID = csResult;
	(gEnv)->ReleaseStringUTFChars(sResult, csResult);

    return sID.c_str();
}

const char* GetBundleID()
{
    static string sID;
    if(sID.length()) return sID.c_str();

    gjNativesCls = (gEnv)->FindClass("kr/co/songs/android/AlienInvasion/SongGLLib");
	if ( gjNativesCls == 0 )
	{
		HLogE("Error Call C to Java .. not found kr/co/songs/android/AlienInvasion/SongGLLib  in GetBundleID\n");
		return 0;
	}

	gjMDID = (gEnv)->GetStaticMethodID(gjNativesCls,"GetBundleID","()Ljava/lang/String;" );
	if(gjMDID == 0)
	{
		HLogE("Error Call C to Java .. not found GetBundleID\n");
		return 0;
	}
	//리턴값이 int이 IntMethod
	jboolean isSucceed;
	jstring sResult = (jstring)(gEnv)->CallStaticObjectMethod(gjNativesCls,gjMDID);
	const char *csResult = (gEnv)->GetStringUTFChars(sResult, &isSucceed);
	sID = csResult;
	(gEnv)->ReleaseStringUTFChars(sResult, csResult);

    return sID.c_str();
}

const char* GetAppVersion()
{
    static string sVersion;
    if(sVersion.length()) return sVersion.c_str();

    gjNativesCls = (gEnv)->FindClass("kr/co/songs/android/AlienInvasion/SongGLLib");
	if ( gjNativesCls == 0 )
	{
		HLogE("Error Call C to Java .. not found kr/co/songs/android/AlienInvasion/SongGLLib  in GetAppVersion\n");
		return 0;
	}

	gjMDID = (gEnv)->GetStaticMethodID(gjNativesCls,"GetAppVersion","()Ljava/lang/String;" );
	if(gjMDID == 0)
	{
		HLogE("Error Call C to Java .. not found GetAppVersion\n");
		return 0;
	}
	//리턴값이 int이 IntMethod
	jboolean isSucceed;
	jstring sResult = (jstring)(gEnv)->CallStaticObjectMethod(gjNativesCls,gjMDID);
	const char *csResult = (gEnv)->GetStringUTFChars(sResult, &isSucceed);
	sVersion = csResult;
	(gEnv)->ReleaseStringUTFChars(sResult, csResult);

    return sVersion.c_str();
}

const char* GetModel()
{
    static string sVersion;
    if(sVersion.length()) return sVersion.c_str();

    gjNativesCls = (gEnv)->FindClass("kr/co/songs/android/AlienInvasion/SongGLLib");
	if ( gjNativesCls == 0 )
	{
		HLogE("Error Call C to Java .. not found kr/co/songs/android/AlienInvasion/SongGLLib  in GetAppVersion\n");
		return 0;
	}

	gjMDID = (gEnv)->GetStaticMethodID(gjNativesCls,"GetModel","()Ljava/lang/String;" );
	if(gjMDID == 0)
	{
		HLogE("Error Call C to Java .. not found GetModel\n");
		return 0;
	}
	//리턴값이 int이 IntMethod
	jboolean isSucceed;
	jstring sResult = (jstring)(gEnv)->CallStaticObjectMethod(gjNativesCls,gjMDID);
	const char *csResult = (gEnv)->GetStringUTFChars(sResult, &isSucceed);
	sVersion = csResult;
	(gEnv)->ReleaseStringUTFChars(sResult, csResult);

    return sVersion.c_str();
}

char* HttpRequestXmlOrg(char* url,char* post)
{
	char* RU = NULL;
	jmethodID  localjMDID = 0;
	jclass     localNativesCls = 0;

	HLogD("--HttpRequestXmlOrg");
	//VM을 통해서 현재 쓰레드에 어테치한다.
	JNIEnv * LocalEnv = 0;
	JavaVMAttachArgs args;
	args.version = JNI_VERSION_1_2;
	args.name = "AlienInvasionProtocolThread"; //쓰레드이 함수 이름.
	args.group = NULL;
	gVM->AttachCurrentThread(&LocalEnv,&args);

	jstring surl = (LocalEnv)->NewStringUTF(url); // convert char array to jstring
	jstring spost = (LocalEnv)->NewStringUTF(post); // convert char array to jstring


	//리턴값이 int이 IntMethod
	jstring sResult = (jstring)(LocalEnv)->CallStaticObjectMethod(gjNativesCls,gjHttpRequestXmlOrgID_MDID,surl,spost);

	jboolean isSucceed;
	if(sResult != 0)
	{
		const char *csResult = (LocalEnv)->GetStringUTFChars(sResult, &isSucceed);
		RU = (char*)malloc(strlen(csResult));
		memcpy(RU,csResult,strlen(csResult));
		(LocalEnv)->ReleaseStringUTFChars(sResult, csResult);

		HLogD("Received Some Data");
	}
	//gEnv는 mainThread이고 LocalEnv는 Attach된 Thread핸들이다. 그래서
	//이것이 틀리면 Detach해준다.
	if(LocalEnv != gEnv)
	{
		gVM->DetachCurrentThread();
	}
	HLogD("--HttpRequestXmlOrgEnd");
	return RU;
}


bool ShowBestScoreDlg(void* pUserInfo)
{
	gjNativesCls = (gEnv)->FindClass("kr/co/songs/android/AlienInvasion/SongGLLib");
	if ( gjNativesCls == 0 )
	{
		HLogE("Error Call C to Java .. not found kr/co/songs/android/AlienInvasion/SongGLLib Class\n");
		return false;
	}

	gjMDID = (gEnv)->GetStaticMethodID(gjNativesCls,"sglShowBestScoreDlg","(ILjava/lang/String;Ljava/lang/String;)I" );
	if(gjMDID == 0)
	{
		HLogE("Error Call C to Java .. not found sglShowBestScoreDlg\n");
		return false;
	}

	CUserInfo* userInfo = (CUserInfo*)pUserInfo;
	int nR = ((CUserInfo*)pUserInfo)->GetReport();
	int nRank = ((CUserInfo*)pUserInfo)->GetRank();
	jstring suserid = (gEnv)->NewStringUTF(userInfo->GetUserID()); // convert char array to jstring
	jstring smessage = (gEnv)->NewStringUTF(userInfo->GetYourMessage()); // convert char array to jstring

//	if(
//	       (nR == 0 && nRank >= 5) ||
//	       (nR != 0 && nR != -1 && nRank >= (nR + 4)) //nR+5 4단계이후에 다시 한번 물어보자.
//	       )
//	 {
//		//평점을 물어보자.
//		(gEnv)->CallStaticIntMethod(gjNativesCls,gjMDID,1,suserid,smessage);
//	 }
//	else
//	{
		//최고점수 게시판을 보여주자.
		(gEnv)->CallStaticIntMethod(gjNativesCls,gjMDID,0,suserid,smessage);
//	}

    return true;
}


bool ShowReviewDlg(void* pUserInfo)
{
	gjNativesCls = (gEnv)->FindClass("kr/co/songs/android/AlienInvasion/SongGLLib");
	if ( gjNativesCls == 0 )
	{
		HLogE("Error Call C to Java .. not found kr/co/songs/android/AlienInvasion/SongGLLib Class\n");
		return false;
	}

	gjMDID = (gEnv)->GetStaticMethodID(gjNativesCls,"sglShowBestScoreDlg","(ILjava/lang/String;Ljava/lang/String;)I" );
	if(gjMDID == 0)
	{
		HLogE("Error Call C to Java .. not found sglShowBestScoreDlg\n");
		return false;
	}

	CUserInfo* userInfo = (CUserInfo*)pUserInfo;

	int nR = ((CUserInfo*)pUserInfo)->GetReport();
	int nRank = ((CUserInfo*)pUserInfo)->GetRank();
	jstring suserid = (gEnv)->NewStringUTF(userInfo->GetUserID()); // convert char array to jstring
	jstring smessage = (gEnv)->NewStringUTF(userInfo->GetYourMessage()); // convert char array to jstring
	int nMagnetic = userInfo->GetCntByIDWithItem(ID_ITEM_UPGRADE_MAGNETIC);
	if(nMagnetic == -999 || nMagnetic == 0)
		(gEnv)->CallStaticIntMethod(gjNativesCls,gjMDID,1,suserid,smessage); //레이더.
	else
		(gEnv)->CallStaticIntMethod(gjNativesCls,gjMDID,2,suserid,smessage); //골드.

    return true;
}


void SendUserInfo(void* pUserInfo)
{
#ifdef GAMECENTER
	CUserInfo* userInfo = (CUserInfo*)pUserInfo;
	long nScore = userInfo->GetTotalScore();


	JNIEnv* NativeEnv = 0;
	jclass  jNativesCls = 0;
	jmethodID  jMDID = 0;
	if(gsglSendBestScoreNativeEnv)
		NativeEnv = gsglSendBestScoreNativeEnv;
	else
		NativeEnv = gEnv;


	jNativesCls = (NativeEnv)->FindClass("kr/co/songs/android/AlienInvasion/SongGLLib");
	if ( jNativesCls == 0 )
	{
		HLogE("Error Call C to Java .. not found kr/co/songs/android/AlienInvasion/SongGLLib Class\n");
		return ;
	}

	jMDID = (NativeEnv)->GetStaticMethodID(jNativesCls,"sglSendBestScore","(IIIIII)I" );
	if(jMDID == 0)
	{
		HLogE("Error Call C to Java .. not found sglSendBestScore\n");
		return ;
	}

    int nTotalScore = userInfo->GetTotalScore();
    int nARate = userInfo->GetCntByIDWithItem(ID_ITEM_UPGRADE_ATTACK);
    int nDRate = userInfo->GetCntByIDWithItem(ID_ITEM_UPGRADE_DEPEND);
    int nTRate = userInfo->GetCntByIDWithItem(ID_ITEM_UPGRADE_DEFENCE_TOWER);
    int nMTRate = userInfo->GetCntByIDWithItem(ID_ITEM_UPGRADE_MISSILE_TOWER);

    HLogD("SendUserInfo - sglSendBestScore total = %d,AR = %d,DR = %d,TR = %d,MTR = %d,Winnder = %d",
    		nTotalScore,gnARate - nARate,gnDRate - nDRate,gnTRate - nTRate,gnMTRate - nMTRate,userInfo->GetCompletedRank());
	(NativeEnv)->CallStaticIntMethod(jNativesCls,jMDID,
			nTotalScore,gnARate - nARate,gnDRate - nDRate,gnTRate - nTRate,gnMTRate - nMTRate,userInfo->GetCompletedRank());
	gnARate = nARate;
	gnDRate = nDRate;
	gnTRate = nTRate;
	gnMTRate = nMTRate;

	gsglSendBestScoreNativeEnv = 0;
#endif //GAMECENTER
}

void ShowSNSDlg()
{
	gjNativesCls = (gEnv)->FindClass("kr/co/songs/android/AlienInvasion/SongGLLib");
	if ( gjNativesCls == 0 )
	{
		HLogE("Error Call C to Java .. not found kr/co/songs/android/AlienInvasion/SongGLLib Class\n");
		return ;
	}

	gjMDID = (gEnv)->GetStaticMethodID(gjNativesCls,"sglShowSNSDlg","()I" );
	if(gjMDID == 0)
	{
		HLogE("Error Call C to Java .. not found sglShowSNSDlg\n");
		return ;
	}
	(gEnv)->CallStaticIntMethod(gjNativesCls,gjMDID);
}


//0이면 이전에 설정된 필터를 사용한다. 그렇지 않으면 필터에 맞게 검색을 해준다.
void FindPlyer(int nResetFindTime,unsigned int nMatchFilter,unsigned int nMatchGroup)
{
	gjNativesCls = (gEnv)->FindClass("kr/co/songs/android/AlienInvasion/SongGLLib");
	if ( gjNativesCls == 0 )
	{
		HLogE("Error Call C to Java .. not found kr/co/songs/android/AlienInvasion/SongGLLib Class\n");
		return ;
	}

	gjMDID = (gEnv)->GetStaticMethodID(gjNativesCls,"sglFindPlyer","(III)I" );
	if(gjMDID == 0)
	{
		HLogE("Error Call C to Java .. not found sglFindPlyer\n");
		return ;
	}
	HLogD("FindPlyer(III)I nMatchFilter = 0x%x",nMatchFilter);
	(gEnv)->CallStaticIntMethod(gjNativesCls,gjMDID,nResetFindTime,nMatchFilter,nMatchGroup);
}

void InitFindPlayer()
{
	gjNativesCls = (gEnv)->FindClass("kr/co/songs/android/AlienInvasion/SongGLLib");
	if ( gjNativesCls == 0 )
	{
		HLogE("Error Call C to Java .. not found kr/co/songs/android/AlienInvasion/SongGLLib Class\n");
		return ;
	}

	gjMDID = (gEnv)->GetStaticMethodID(gjNativesCls,"sglInitFindPlayer","()I" );
	if(gjMDID == 0)
	{
		HLogE("Error Call C to Java .. not found sglInitFindPlayer\n");
		return ;
	}
	HLogD("InitFindPlayer()I");
	(gEnv)->CallStaticIntMethod(gjNativesCls,gjMDID);
}


void PlayOut()
{
//	gjNativesCls = (gEnv)->FindClass("kr/co/songs/android/AlienInvasion/SongGLLib");
//	if ( gjNativesCls == 0 )
//	{
//		HLogE("Error Call C to Java .. not found kr/co/songs/android/AlienInvasion/SongGLLib Class\n");
//		return ;
//	}
	gjMDID = (gEnv)->GetStaticMethodID(gjSongGLLibClass,"sglPlayOut","()I" );
	if(gjMDID == 0)
	{
		HLogE("Error Call C to Java .. not found sglPlayOut\n");
		return ;
	}
	HLogD("sglPlayOut()I");
	(gEnv)->CallStaticIntMethod(gjSongGLLibClass,gjMDID);
}



void SendMultiplayData(void* pObject,int nSize, char* sData,bool bReliable)
{


	char* RU = NULL;
//	jmethodID  localjMDID = 0;
//	jclass     localNativesCls = 0;

	HLogD("--SendMultiplayData");

	//VM을 통해서 현재 쓰레드에 어테치한다.
	JNIEnv * LocalEnv = 0;
	JavaVMAttachArgs args;
	args.version = JNI_VERSION_1_4;
	args.name = "MultiplayProtocolThread"; //쓰레드이 함수 이름.
	args.group = NULL;
	gVM->AttachCurrentThread(&LocalEnv,&args);


	HLogD("sglSendMultiplayData(BI)I Size = %d",nSize);
	jbyteArray btData;
	btData = (LocalEnv)->NewByteArray(nSize);
	(LocalEnv)->SetByteArrayRegion(btData, 0, nSize, (const jbyte*)sData);
	(LocalEnv)->CallStaticIntMethod(gjSongGLLibClass,gjSendMultiplayMDID,btData,(int)bReliable);

	//gEnv는 mainThread이고 LocalEnv는 Attach된 Thread핸들이다. 그래서
	//이것이 틀리면 Detach해준다.
	if(LocalEnv != gEnv)
	{
		gVM->DetachCurrentThread();
	}
	HLogD("--SendMultiplayData");
}


void ShowNewVersionDlg(const char* sURL) //gsNewAppVersion를 직접 사용한다.
{
	sglSendMessageToJava(2,1); //새로운 버전이 존재 하니 버전업 해준다.
}
