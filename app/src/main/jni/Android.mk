LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := songgl

#절대경로를 맞추어주자 안그러면 안드로이드 스튜디오에서 에러가 난다.
#SONGGL_PATH := /Users/songs/Project/MyDream/IOS/SongGL
SONGGL_PATH := /home/songs/Extension/MyDream/IOS/SongGL

XML2SRC_PATH := $(SONGGL_PATH)/LibXML2
XML2INC_PATH := $(XML2SRC_PATH)/include
ANDROID_PATH := $(SONGGL_PATH)/Platforms/Android
ZIP_PATH := $(ANDROID_PATH)/libz
COMM_PATH := $(SONGGL_PATH)/Comm
PNG_PATH := $(SONGGL_PATH)/PNG
PLATFORMS_PATH := $(SONGGL_PATH)/Platforms
MAIN_PATH := $(SONGGL_PATH)/Main
LIST_PATH := $(SONGGL_PATH)/List
CONTROL_PATH := $(SONGGL_PATH)/Control
ASTAR_PATH := $(SONGGL_PATH)/AStar
WEB_PATH := $(SONGGL_PATH)/WebProtocol


LOCAL_C_INCLUDES := \
				$(NDK_ROOT)/sources/cxx-stl/stlport/stlport \
				$(LOCAL_PATH) \
				$(ANDROID_PATH) \
				$(ZIP_PATH) \
                $(SONGGL_PATH) \
                $(COMM_PATH) \
                $(PNG_PATH) \
				$(PLATFORMS_PATH) \
				$(MAIN_PATH) \
				$(LIST_PATH) \
				$(CONTROL_PATH) \
				$(ASTAR_PATH) \
				$(WEB_PATH) \
				$(XML2INC_PATH)

LOCAL_CFLAGS := -DANDROID -frtti -fno-exceptions -D_STLP_USE_NEWALLOC -DRELEASE -DANDROIDAPPSTOREKIT -DANDROIDADMOB -DGAMECENTER

LOCAL_SRC_FILES := \
				$(XML2SRC_PATH)/SAX.c \
				$(XML2SRC_PATH)/entities.c \
				$(XML2SRC_PATH)/encoding.c \
				$(XML2SRC_PATH)/error.c \
				$(XML2SRC_PATH)/parserInternals.c \
				$(XML2SRC_PATH)/parser.c \
				$(XML2SRC_PATH)/tree.c \
				$(XML2SRC_PATH)/hash.c \
				$(XML2SRC_PATH)/list.c \
				$(XML2SRC_PATH)/xmlIO.c \
				$(XML2SRC_PATH)/xmlmemory.c \
				$(XML2SRC_PATH)/uri.c \
				$(XML2SRC_PATH)/valid.c \
				$(XML2SRC_PATH)/xlink.c \
				$(XML2SRC_PATH)/HTMLparser.c \
				$(XML2SRC_PATH)/HTMLtree.c \
				$(XML2SRC_PATH)/debugXML.c \
				$(XML2SRC_PATH)/xpath.c \
				$(XML2SRC_PATH)/xpointer.c \
				$(XML2SRC_PATH)/xinclude.c \
				$(XML2SRC_PATH)/nanohttp.c \
				$(XML2SRC_PATH)/nanoftp.c \
				$(XML2SRC_PATH)/DOCBparser.c \
				$(XML2SRC_PATH)/catalog.c \
				$(XML2SRC_PATH)/globals.c \
				$(XML2SRC_PATH)/threads.c \
				$(XML2SRC_PATH)/c14n.c \
				$(XML2SRC_PATH)/xmlstring.c \
				$(XML2SRC_PATH)/xmlregexp.c \
				$(XML2SRC_PATH)/xmlschemas.c \
				$(XML2SRC_PATH)/xmlschemastypes.c \
				$(XML2SRC_PATH)/xmlunicode.c \
				$(XML2SRC_PATH)/xmlreader.c \
				$(XML2SRC_PATH)/relaxng.c \
				$(XML2SRC_PATH)/dict.c \
				$(XML2SRC_PATH)/SAX2.c \
				$(XML2SRC_PATH)/legacy.c \
				$(XML2SRC_PATH)/chvalid.c \
				$(XML2SRC_PATH)/pattern.c \
				$(XML2SRC_PATH)/xmlsave.c \
				$(XML2SRC_PATH)/xmlmodule.c \
				$(XML2SRC_PATH)/xmlwriter.c \
				$(XML2SRC_PATH)/schematron.c \
				\
				$(WEB_PATH)/ArrayElement.cpp \
				$(WEB_PATH)/ArrayList.cpp \
				$(WEB_PATH)/base64.cpp \
				$(WEB_PATH)/baseutils.cpp \
				$(WEB_PATH)/des.cpp \
				$(WEB_PATH)/des3.cpp \
				$(WEB_PATH)/des3enc.cpp \
				$(WEB_PATH)/KeyValuePair.cpp \
				$(WEB_PATH)/md5.cpp \
				$(WEB_PATH)/MinkConnector.cpp \
				$(WEB_PATH)/MinkProtocol.cpp \
				$(WEB_PATH)/MinkRecordOrg.cpp \
				$(WEB_PATH)/MinkRecordSet.cpp \
				$(WEB_PATH)/MinkRmParam.cpp \
				$(WEB_PATH)/StringBuffer.cpp \
				$(WEB_PATH)/XMLProcessor.cpp \
				$(WEB_PATH)/AlienInvasionProtocolMan.cpp \
				\
				$(PNG_PATH)/png.c \
				$(PNG_PATH)/pngerror.c \
				$(PNG_PATH)/pnggccrd.c \
				$(PNG_PATH)/pngget.c \
				$(PNG_PATH)/pngmem.c \
				$(PNG_PATH)/pngpread.c \
				$(PNG_PATH)/pngread.c \
				$(PNG_PATH)/pngrio.c \
				$(PNG_PATH)/pngrtran.c \
				$(PNG_PATH)/pngrutil.c \
				$(PNG_PATH)/pngset.c \
				$(PNG_PATH)/pngtrans.c \
				$(PNG_PATH)/pngvcrd.c \
				$(PNG_PATH)/pngwio.c \
				$(PNG_PATH)/pngwrite.c \
				$(PNG_PATH)/pngwtran.c \
				$(PNG_PATH)/pngwutil.c \
				\
				$(ZIP_PATH)/adler32.c \
				$(ZIP_PATH)/compress.c \
				$(ZIP_PATH)/crc32.c \
				$(ZIP_PATH)/deflate.c \
				$(ZIP_PATH)/gzclose.c \
				$(ZIP_PATH)/gzlib.c \
				$(ZIP_PATH)/gzread.c \
				$(ZIP_PATH)/gzwrite.c \
				$(ZIP_PATH)/infback.c \
				$(ZIP_PATH)/inffast.c \
				$(ZIP_PATH)/inflate.c \
				$(ZIP_PATH)/inftrees.c \
				$(ZIP_PATH)/minigzip.c \
				$(ZIP_PATH)/trees.c \
				$(ZIP_PATH)/uncompr.c \
				$(ZIP_PATH)/zutil.c \
				\
				$(SONGGL_PATH)/sGL.cpp \
				\
				$(COMM_PATH)/sGLTrasform.cpp \
				$(COMM_PATH)/CSoundMan.cpp \
				$(COMM_PATH)/sGLUtils.cpp \
				$(COMM_PATH)/CSGLCore.cpp \
				$(COMM_PATH)/CSGLController.cpp \
				$(COMM_PATH)/CSglTerrian.cpp \
				$(COMM_PATH)/CPicking.cpp \
				$(COMM_PATH)/CMS3DASCII.cpp \
				$(COMM_PATH)/CMS3DModelASCII.cpp \
				$(COMM_PATH)/CItemMan.cpp \
				$(COMM_PATH)/CSThread.cpp \
				$(COMM_PATH)/CTextureMan.cpp \
				$(COMM_PATH)/CMS3DASCIIMan.cpp \
				$(COMM_PATH)/CMS3DModelASCIIMan.cpp \
				$(COMM_PATH)/CTankModelASCII.cpp \
				$(COMM_PATH)/CArchive.cpp \
				$(COMM_PATH)/CMutiplyProtocol.cpp \
				\
				$(PLATFORMS_PATH)/sGLSoundA.cpp \
				$(PLATFORMS_PATH)/CProperty.cpp \
				$(PLATFORMS_PATH)/sGLTextA.cpp \
				\
				$(MAIN_PATH)/CParticleEmitter.cpp \
				$(MAIN_PATH)/CParticleEmitterMan.cpp \
				$(MAIN_PATH)/C2dRadaBoard.cpp \
				$(MAIN_PATH)/C2dStick.cpp \
				$(MAIN_PATH)/CAIBlzCore.cpp \
				$(MAIN_PATH)/CAICore.cpp \
				$(MAIN_PATH)/CAniLoader.cpp \
				$(MAIN_PATH)/CAniRuntimeBubble.cpp \
				$(MAIN_PATH)/CAniRuntimeEnergyUP.cpp \
				$(MAIN_PATH)/CArrowMotionSprite.cpp \
				$(MAIN_PATH)/CBackground.cpp \
				$(MAIN_PATH)/CBomb.cpp \
				$(MAIN_PATH)/CBombCell.cpp \
				$(MAIN_PATH)/CBombET1.cpp \
				$(MAIN_PATH)/CBombTailParticle.cpp \
				$(MAIN_PATH)/CBound.cpp \
				$(MAIN_PATH)/CCompatFireParticle.cpp \
				$(MAIN_PATH)/CEMainBlzSprite.cpp \
				$(MAIN_PATH)/CET1.cpp \
				$(MAIN_PATH)/CET2.cpp \
				$(MAIN_PATH)/CET8.cpp \
				$(MAIN_PATH)/CET2AttackParticle.cpp \
				$(MAIN_PATH)/CFireParticle.cpp \
				$(MAIN_PATH)/CGhostSprite.cpp \
				$(MAIN_PATH)/CHWorld.cpp \
				$(MAIN_PATH)/CTrainingWorld.cpp \
				$(MAIN_PATH)/CImpactFlyParticle.cpp \
				$(MAIN_PATH)/CInputCtl.cpp \
				$(MAIN_PATH)/CItemCell.cpp \
				$(MAIN_PATH)/CItemSprite.cpp \
				$(MAIN_PATH)/CK9Sprite.cpp \
				$(MAIN_PATH)/CLoader.cpp \
				$(MAIN_PATH)/CLogoWorld.cpp \
				$(MAIN_PATH)/CMainMenuWorld.cpp \
				$(MAIN_PATH)/CMessageCtl.cpp \
				$(MAIN_PATH)/CMissionEndCtl.cpp \
				$(MAIN_PATH)/CModelBound.cpp \
				$(MAIN_PATH)/CModelData.cpp \
				$(MAIN_PATH)/CModelMan.cpp \
				$(MAIN_PATH)/CMoveAI.cpp \
				$(MAIN_PATH)/CMS3DLoader.cpp \
				$(MAIN_PATH)/CMS3DModel.cpp \
				$(MAIN_PATH)/CProgressWorld.cpp \
				$(MAIN_PATH)/CRoll3dModel.cpp \
				$(MAIN_PATH)/CRollMapCtl.cpp \
				$(MAIN_PATH)/CRollMapTankCtl.cpp \
				$(MAIN_PATH)/CRollTankCtl.cpp \
				$(MAIN_PATH)/CScenario.cpp \
				$(MAIN_PATH)/CSGLMesh.cpp \
				$(MAIN_PATH)/CShadowParticle.cpp \
				$(MAIN_PATH)/CSprite.cpp \
				$(MAIN_PATH)/CSpriteStatus.cpp \
				$(MAIN_PATH)/CStrategyAI.cpp \
				$(MAIN_PATH)/CStrategyET2AI.cpp \
				$(MAIN_PATH)/CTE1BoosterParticle.cpp \
				$(MAIN_PATH)/CUserInfo.cpp \
				$(MAIN_PATH)/CWorld.cpp \
				$(MAIN_PATH)/CDTower.cpp \
				$(MAIN_PATH)/CStrategyBlzAI.cpp \
				$(MAIN_PATH)/CStrategyActorAI.cpp \
				$(MAIN_PATH)/CStrategyDBlzAI.cpp \
				$(MAIN_PATH)/CDMainTower.cpp \
				$(MAIN_PATH)/CDMainTowerStatus.cpp \
				$(MAIN_PATH)/CShadowAreaParticle.cpp \
				$(MAIN_PATH)/CDGunTower.cpp \
				$(MAIN_PATH)/CFighter.cpp \
				$(MAIN_PATH)/CET9.cpp \
				$(MAIN_PATH)/CBombRaser.cpp \
				$(MAIN_PATH)/CFighterMoveAI.cpp \
				$(MAIN_PATH)/CFigtherStrategyAI.cpp \
				$(MAIN_PATH)/CFighterTailParticle.cpp \
				$(MAIN_PATH)/CMyModel.cpp \
				$(MAIN_PATH)/CCircleUpgradeMark.cpp \
				$(MAIN_PATH)/CNewBoosterParticle.cpp \
				$(MAIN_PATH)/CRaserParticle.cpp \
				$(MAIN_PATH)/CNewTank.cpp \
				$(MAIN_PATH)/CEF4Fighter.cpp \
				$(MAIN_PATH)/CMissileTank.cpp \
				$(MAIN_PATH)/CTireCar.cpp \
				$(MAIN_PATH)/CArmeredCar.cpp \
				$(MAIN_PATH)/CAmbulance.cpp \
				$(MAIN_PATH)/CStrategyAssistanceAI.cpp \
				$(MAIN_PATH)/CStrategyAmbulanceAI.cpp \
				$(MAIN_PATH)/CBombGun.cpp \
				\
				$(LIST_PATH)/CList.cpp \
				$(LIST_PATH)/CListMap.cpp \
				\
				$(CONTROL_PATH)/ARollCtl.cpp \
				$(CONTROL_PATH)/CButtonCtl.cpp \
				$(CONTROL_PATH)/CCellCtl.cpp \
				$(CONTROL_PATH)/CControl.cpp \
				$(CONTROL_PATH)/CLabelCtl.cpp \
				$(CONTROL_PATH)/CNumberCtl.cpp \
				$(CONTROL_PATH)/CPopupCtl.cpp \
				$(CONTROL_PATH)/CProgressCtl.cpp \
				$(CONTROL_PATH)/CRollCtl.cpp \
				$(CONTROL_PATH)/CScrollBarCtl.cpp \
				$(CONTROL_PATH)/CScrollContentCtl.cpp \
				$(CONTROL_PATH)/CScrollCtl.cpp \
				$(CONTROL_PATH)/CTableCtl.cpp \
				$(CONTROL_PATH)/CImgProgressCtl.cpp \
				$(CONTROL_PATH)/CIndicatorCtl.cpp \
				\
				$(ANDROID_PATH)/hstring.cpp \
				$(ANDROID_PATH)/HDoc.cpp \
				$(ANDROID_PATH)/HNode.cpp \
				\
				$(ASTAR_PATH)/astar.cpp \
				$(ASTAR_PATH)/environment.cpp \
				$(ASTAR_PATH)/error.cpp \
				$(ASTAR_PATH)/fringesearch.cpp \
				$(ASTAR_PATH)/idastar.cpp \
				$(ASTAR_PATH)/search.cpp \
				$(ASTAR_PATH)/searchutils.cpp \
				$(ASTAR_PATH)/statistics.cpp \
				$(ASTAR_PATH)/tiling.cpp \
				$(ASTAR_PATH)/util.cpp \
				$(ASTAR_PATH)/version.cpp \
				SongGLJNI.cpp 
				
				
#LOCAL_STATIC_LIBRARIES := libxml2

LOCAL_LDLIBS := -llog -lGLESv1_CM -ljnigraphics
include $(BUILD_SHARED_LIBRARY)
