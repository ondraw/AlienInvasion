package kr.co.songs.android.AlienInvasion;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Enumeration;
import java.util.Locale;
import java.util.zip.ZipEntry;
import java.util.zip.ZipException;
import java.util.zip.ZipFile;

import kr.co.songs.android.AlienInvasion.http.CExceptionNetwork;
import kr.co.songs.android.AlienInvasion.http.CHttpRequest;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager.NameNotFoundException;
import android.graphics.Bitmap;
import android.graphics.Bitmap.CompressFormat;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Typeface;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.os.PowerManager;
import android.provider.Settings.Secure;
import android.util.Log;

import com.android.googleservice.GameHelper;

public class SongGLLib {
    public final static String LEADERBOARDID = "CgkImuz5oOcbEAIQAw";
    public final static String ATACHID_UPGRADE_ATTACK = "CgkImuz5oOcbEAIQEQ";
    public final static String ATACHID_UPGRADE_DEFENCE = "CgkImuz5oOcbEAIQEA";
    public final static String ATACHID_UPGRADE_TOWER = "CgkImuz5oOcbEAIQEg";
    public final static String ATACHID_UPGRADE_MISSTOWER = "CgkImuz5oOcbEAIQEw";
    public final static String ATACHID_YOUAREWINNER = "CgkImuz5oOcbEAIQFA";
    public final static int SGL_MATCHING_MSG         = 0x020046; //매칭중에 있습니다. 잠시만 기다려 주세요.
    public final static int SGL_SHOW_ENEMYTOOLS      = 0x020047; //접속한 상대방의 정보를 보여준다.
    public final static int SGL_WAIT_TO_LOGO_ANDROID = 0x02001E;// Android에서
    // Wait단계에서
    // NextLogo로
    // 변환하라는 이벤트
    // public final static int SGL_SHOP_PRODUCT_GOLDTTYPE = 0x02001D; //골드를 사는
    // 프러덕을 샀다.
    public final static int SGL_BUY_PRODUCT_GOLDTTYPE_FOR_ANDROID = 0x02001F; // 안드로이드는
    // 바로
    // 사지
    // 않고
    // 시간차가
    // 존재하기
    // 때문에
    // 나중에
    // 적용된다.
    // 일단
    // 파일에
    // 저장후
    // 다시
    // 활용하자.
    public final static int SGL_CLICKED_ADMOB_ANDROID = 0x020033;
    public final static int JAVA_AD_LAYOUT_MODE = 12;
    public final static int AD_LAYOUT_BOTTOM_DOCKED = 0;
    public final static int AD_LAYOUT_BOTTOM_OVERLAY = 1;
    static public final int DISPLAY_IPAD = 1;
    static public final int DISPLAY_IPHONE = 2;
    public static int gDisplayType = SongGLLib.DISPLAY_IPAD;

    /**
     * JNI 함수들
     * ------------------------------------------------------------------
     * -------------
     */
    static {
        System.loadLibrary("songgl");
    }

    //-------------------------------------------------------------- IAP Version 2
//	public static native void sglResultPurchase(String sProductID,
//			int bSuccess, String sMessage);
//	public static native void sglAddProductsList(String[] rowData);
//	public static native void sglAddProductsListEnd(String sConnID,
//			String sTrancID, String rcvEvent);
//	public static native void sglErrorProductsList(String sConnID,
//			String sTrancID, String rcvEvent, String sError);
    //-------------------------------------------------------------- IAP Version 2 End


    public static native void sglSetLocalizationDir(String sPath);

    public static native void sglSetDisplayType(long lDisplayType);

    public static native void sglTextUnitInitialize(String sPath);

    public static native void sglTextUnitRelease();

    protected static native long sglInitialize();

    protected static native void sglRelease(long lGLContext);

    protected static native void sglRender(long lGLContext);

    protected static native int sglResize(long lGLContext, int nWidth,
                                          int nHeight);

    protected static native void sglResetViewPort(long lGLContext);

    protected static native int sglResource(long lGLContext, String sPath);

    protected static native boolean sglEvent(long lGLContext, int nEventID,
                                             int param1, int param2);

    protected static native void sglBeginTouch(long lGLContext, long ID,
                                               float fx, float fy);

    protected static native void sglMoveTouch(long lGLContext, long ID,
                                              float fx, float fy);

    protected static native void sglEndTouch(long lGLContext, long ID,
                                             float fx, float fy);

    protected static native int sglSetTexture(long lGLContext, String sKey,
                                              Bitmap bitmap);

    protected static native void sglResetTexture(long lGLContext);

    public static native void sglSetUserInfo(String sUserID, String sMessage);

    public static native void sglSetRateInfo(long nType);

    public static native void sglSendBestScoreNative();

    // World에 메세지를 보내자.
    public static native void sglSendMessage(int nMsgID, long lParam,
                                             long lParam2, long lParam3, long lObject);

    // Activity의 Resume이나 Play에서 설정해준다.
    public static native void sglPlayBgSoundOnAcitvity(boolean bPlay);

    public static native void sglAccelertionData(long lGLContext,float fx,float fy,float fz);
    public static native void sglSignInState(long nState); //0:에러 1:로그인중 2:성공.
    public static native void sglOnSigned(long lGLContext,boolean bSuccess);
    public static native void sglSetRootPath(String sPath);//Added By Song 2015.01.06 멀티플레이를 지원하면서 GetUserInfo를 어디선가 불러쓰기 때문에 시작시에 미리 정의 하자.
    public static native void sglSetMultiplayState(long lGLContext,long nState);
    public static native void sglResetFindTime(long lGLContext,long nWaiteTime);
    public static native void sglStartMultiplayGame(long lGLContext);
    public static native void sglStopMultiplayGame(long lGLContext);
    public static native void sglOnReceivedData(long lGLContext,String sSender,byte[] btData);
    public static native void sglSetMyID(long lGLContext,String sMyID);
    public static native void sglSetAppNewVersion(boolean bNew);
    // ------------------------------------------------------------------------------------------

    long mlGLContext;

    public long getGLContext() {
        return mlGLContext;
    }

    public SongGLLib() {
        mlGLContext = 0;
    }

    public boolean Initialize() {
        boolean bResult = true;
        mlGLContext = sglInitialize();
        if (mlGLContext == 0)
            bResult = false;
        return bResult;
    }

    public void ResetTexture() {
        if (mlGLContext != 0) {
            sglResetTexture(mlGLContext);
        }
    }

    public void Release() {
        if (mlGLContext != 0) {
            sglRelease(mlGLContext);
            mlGLContext = 0;
        }
    }

    public void Render() {
        if (mlGLContext != 0) {
            sglRender(mlGLContext);
        }
    }

    public int Resize(int nWidth, int nHeight) {
        int nResult = 0;
        if (mlGLContext != 0) {
            nResult = sglResize(mlGLContext, nWidth, nHeight);
        }
        return nResult;

    }

    public void ResetViewPortX() {
        if (mlGLContext != 0) {
            sglResetViewPort(mlGLContext);
        }
        Log.i("JAVA", "1 ResetViewPort()");
    }

    public int Resource(String sPath) {
        int nResult = 0;
        if (mlGLContext != 0) {
            nResult = sglResource(mlGLContext, sPath);
        }
        return nResult;
    }

    /**
     * GSL_UP 0x000001 GSL_DOWN 0x000002 GSL_RIGHT 0x000004 GSL_LEFT 0x000008
     * GSL_TRUNRIGHT 0x000010 GSL_TRUNLEFT 0x000020
     *
     * @param lGLContext
     * @param nEventID
     * @param param1
     * @param param2
     * @return
     */
    public boolean Event(int nEventID, int param1, int param2) {
        boolean bResult = true;
        if (mlGLContext != 0) {
            bResult = sglEvent(mlGLContext, nEventID, param1, param2);
        }
        return bResult;
    }

    public void BeginTouch(long ID, float fx, float fy) {
        if (mlGLContext != 0) {
            sglBeginTouch(mlGLContext, ID, fx, fy);
        }
    }

    public void MoveTouch(long ID, float fx, float fy) {
        if (mlGLContext != 0) {
            sglMoveTouch(mlGLContext, ID, fx, fy);
        }
    }

    public void EndTouch(long ID, float fx, float fy) {
        if (mlGLContext != 0) {
            sglEndTouch(mlGLContext, ID, fx, fy);
        }
    }

    public int SetTexture(String sKey, Bitmap bitmap) {
        if (mlGLContext != 0)
            return sglSetTexture(mlGLContext, sKey, bitmap);
        return 0;
    }

    public void AccelertionData(float x,float y,float z)
    {
        if (mlGLContext != 0)
            sglAccelertionData(mlGLContext, x, y,z);
    }


    // C++ 에서 Java로 텍스트를 요청한다.
    static public int sglRequestProductID(String sProductID,int nType) {
        sProductID = sProductID.toLowerCase(); // 소문자로 변경한다.
        Log.i("JAVA", "IAP ProductID is " + sProductID);
        AlienInvasion.gMainActivity.RequestProductItem(sProductID,nType);
        return 0;
    }

    static public Bitmap sglGetBitmapJava(String sKey)
    {
        Bitmap bitmap = null;
        if (sKey.compareTo("waiting.tga") == 0) {
            try {
                // //진져 브레이드 android 2.3 계열에서 는 drawable/drawable-hdpi 등등으로 나두어서
                // 적절하게 이미지를 넣어주어야.
                // //싸이즈가 변하지 않음. (사이즈가 변하기 때문에 겔럭시 S계열에서는 거시기 나온다.)
                bitmap = BitmapFactory.decodeResource(
                        AlienInvasion.gMainActivity.getResources(),
                        R.drawable.landscape);
            } catch (Exception em) {
                Log.e("AlienInvasion", em.getLocalizedMessage());
            }

        } else {
            Log.e("AlienInvasion", "sglGetBitmapJava" + sKey);
        }

        return bitmap;
    }

    static public int sglClearBitmapJava(Bitmap bitmap) {
        if (bitmap != null) {
            bitmap.recycle();
            bitmap = null;
            return 1;
        }
        return 0;
    }

    static public void saveBitmap(String sText, Bitmap textBitmap) {
        String sPath = "/mnt/sdcard/AlienInvasion/SavedImage/" + sText + ".png";

        File dir = new File("/mnt/sdcard/AlienInvasion/SavedImage");
        if (dir.isDirectory() == false)
            dir.mkdirs();

        File copyFile = new File(sPath);

        OutputStream out = null;

        try {

            copyFile.createNewFile();
            out = new FileOutputStream(copyFile);

            Log.e("AlienInvasion", "-----------" + copyFile);
            if (textBitmap.compress(CompressFormat.PNG, 100, out)) {
            }
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            try {
                out.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    static public Bitmap sglMakeBitmapWithText(String sText, float fTextSize,
                                               int nWidth, int nHeight, float fOffsetX, float fOffsetY,
                                               boolean bBold) {
        // fTextSize = fTextSize * 0.96f;
        float fDummyY = fTextSize / 4.0f; // 글자의 여백이 존재한다. 폰트크기의 / 4 이면 상단으로
        // 정확하게 맞춰진다.

        // Added By Song 2013.01.18 한글일경우 아이패드는 약간 짤린다.
        if (gDisplayType == SongGLLib.DISPLAY_IPAD)
            fDummyY += 1.6f;
        // else
        // fDummyY += 0.4f;

        Bitmap textBitmap = Bitmap.createBitmap(nWidth, nHeight,
                Bitmap.Config.ARGB_8888);
        textBitmap.eraseColor(0x0);

        Canvas canvas = new Canvas(textBitmap);
        Typeface typeface = null;

        // Added By Song 2013.04.23 볼드체를 추가하였음.
        if (bBold)
            typeface = Typeface.create(Typeface.SERIF, Typeface.BOLD);
        else
            typeface = Typeface.create(Typeface.SERIF, Typeface.NORMAL);

        Paint textPaint = new Paint();
        textPaint.setTextSize(fTextSize);
        textPaint.setAntiAlias(true);
        textPaint.setColor(Color.WHITE);
        textPaint.setTypeface(typeface);

        // 아이폰과 안드로이드의 폰트와 픽셀의 크기는 같다 하지만 그려지는 위치가 위아래가 반대이기 때문에 nHeight - xxx로
        // 계산해주면 정확하게 일치 하다.
        // canvas.drawText(sText, fOffsetX, nHeight - (nHeight - (fTextSize -
        // fDummyY + fOffsetY)), textPaint);
        canvas.drawText(sText, fOffsetX, fTextSize - fDummyY + fOffsetY,
                textPaint);
        return textBitmap;
    }

    private static boolean CopyFile(InputStream in, String dtFile) {
        try {
            // File f1 = new File(srFile);
            File f2 = new File(dtFile);
            // InputStream in = new FileInputStream(f1);

            // For Append the file.
            // OutputStream out = new FileOutputStream(f2,true);

            // For Overwrite the file.
            OutputStream out = new FileOutputStream(f2);

            byte[] buf = new byte[1024];
            int len;

            while ((len = in.read(buf)) > 0) {
                out.write(buf, 0, len);
            }

            out.close();

        } catch (FileNotFoundException ex) {
            Log.e("AlienInvasion", ex.getMessage()
                    + " in the specified directory.");
            return false;
        } catch (IOException e) {
            System.out.println(e.getMessage());
            Log.e("AlienInvasion", e.getMessage() + " CopyFile.");
            return false;
        }
        return true;
    }

    static Thread mThread = null;

    // -------------------------------------------
    // Zip 파일이 존재하는지를 확인한다.
    // -------------------------------------------
    static public int sglUnzip3DResource() {
        String sRootPath = getPath();

        // Zip파일이 존재하면.. 다음 로고를 실행한다.

        File f = new File(sRootPath);
        if (!f.exists()) {
            // 디렉토리를 만든다.
            f.mkdirs();
        }

        // ----------------------------------------
        // Added By Song 2012.12.19 풀다가 에러가 나면 더이상 풀지를 못한다.
        // 집을 풀다가 에러가 나서 못풀
        f = new File(sRootPath + "/resourcezip.zip");
        if (f.exists()) {
            f = new File(sRootPath + "/.nomedia");
            f.delete();
        }
        // -----------------------------------------

        // -------------------------------------------------------------
        String s3dResourceVersion = AlienInvasion.gMainActivity.getResources()
                .getString(R.string.resourceversion);
        float f3dResourceVersion = Float.parseFloat(s3dResourceVersion);

        // .nomedia 파일을 만든다.
        f = new File(sRootPath + "/.nomedia");
        boolean bNeedUnzip = false;
        if (f.exists()) {
            if (f.length() != 0) {
                byte[] bC = new byte[(int) f.length()];
                FileInputStream fin = null;
                try {
                    fin = new FileInputStream(f);
                    fin.read(bC);

                    String sVer = new String(bC);
                    float fBefor = Float.parseFloat(sVer);

                    if (f3dResourceVersion > fBefor)
                        bNeedUnzip = true;
                } catch (Exception e) {
                    // Modified by song 2012.12.28 .nomedia일 경우 에러가 난다.
                    bNeedUnzip = true;
                } finally {

                    try {
                        fin.close();
                    } catch (IOException e) {
                        // TODO Auto-generated catch block
                        e.printStackTrace();
                    }
                    fin = null;
                }
            } else
                bNeedUnzip = true; // 아주아주 옛날 버전

            if (bNeedUnzip) {
                Log.i("AlienInvasion", "New Resource Version = "
                        + f3dResourceVersion);
                f.delete();
                f = new File(sRootPath + "/.nomedia");
            }

        }

        if (!f.exists()) {
            FileOutputStream fos = null;
            try {
                fos = new FileOutputStream(f);
                // fos.write("no media".getBytes());
                fos.write(s3dResourceVersion.getBytes());

            } catch (Exception e) {

                e.printStackTrace();
                Log.e("AlienInvasion", e.getMessage());
            } finally {
                try {
                    if (fos != null) {
                        fos.close();
                        fos = null;
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                    Log.e("AlienInvasion", e.getMessage());
                }
            }

            mThread = new Thread() {
                public void run() {
                    String sRootPath = getPath();

                    PowerManager pm = (PowerManager) AlienInvasion.gMainActivity
                            .getSystemService(AlienInvasion.gMainActivity.POWER_SERVICE);
                    PowerManager.WakeLock wl = pm.newWakeLock(
                            PowerManager.SCREEN_DIM_WAKE_LOCK, "My Tag");
                    wl.acquire();

                    // ------------------------------------------------------------
                    Message msg = AlienInvasion.gMainActivity.mUnZipHandler
                            .obtainMessage();
                    Bundle b = new Bundle();
                    b.putInt("id", 0);
                    msg.setData(b);
                    AlienInvasion.gMainActivity.mUnZipHandler.sendMessage(msg);
                    // ------------------------------------------------------------

                    // ----------------------------------
                    // ZipFile을 Storage에 카피한다.
                    // ----------------------------------
                    InputStream input = null;
                    String sTargetPath = sRootPath + "/resourcezip.zip";
                    try {
                        input = AlienInvasion.gMainActivity
                                .getResources()
                                .openRawResource(
                                        kr.co.songs.android.AlienInvasion.R.raw.resourcezip);
                        CopyFile(input, sTargetPath);

                    } catch (Exception e) {
                        Log.e("AlienInvasion", e.getMessage());
                    } finally {
                        if (input != null) {
                            try {
                                input.close();
                            } catch (IOException e) {

                                e.printStackTrace();
                            }
                            input = null;
                        }
                    }
                    // ----------------------------------

                    // UnZip을 한다.
                    unZip(sTargetPath, sRootPath);

                    // zip파일을 지운다.
                    File f = new File(sTargetPath);
                    f.delete();

                    // GLText를 읽어온다.
                    SongGLLib.sglTextUnitInitialize(SongGLLib.getPath());

                    // ------------------------------------------------------------
                    msg = AlienInvasion.gMainActivity.mUnZipHandler
                            .obtainMessage();
                    b = new Bundle();
                    b.putInt("id", 2);
                    msg.setData(b);
                    AlienInvasion.gMainActivity.mUnZipHandler.sendMessage(msg);
                    // ------------------------------------------------------------


                    AlienInvasion.gGLView.SendMessage(SGL_WAIT_TO_LOGO_ANDROID, 0, 0);

                    wl.release();
                }
            };
            mThread.start();
        } else {
            // GLText를 읽어온다.
            SongGLLib.sglTextUnitInitialize(SongGLLib.getPath());

            //2019.02.12
            new Handler().postDelayed(new Runnable() {
                @Override
                public void run() {
                    AlienInvasion.gGLView.SendMessage(SGL_WAIT_TO_LOGO_ANDROID, 0, 0);
                }
            }, 2000);

        }
        return 0;
    }

    static String gPath = null;

    public static String getPath() {
        if (gPath != null)
            return gPath;

        File f = null;
        if (AlienInvasion.gMainActivity != null) {
            f = AlienInvasion.gMainActivity.getExternalFilesDir(null);
            if (f == null) {
                f = AlienInvasion.gMainActivity.getFilesDir();
            }
        }

        if (f == null) {
            f = new File(Environment.getDataDirectory().getPath(), "AlienInvasion");
        }

        if (!f.exists()) {
            f.mkdirs();
        }

        gPath = f.getAbsolutePath();
        return gPath;
    }

    public static void unZip(String strZipPath, String strTargetDir) {
        try {
            String sName;
            int nIndex = 0;
            String strTargetPath;

            //
            // Create an instance of ZipFile to read a zip file
            // called sample.zip
            //
            ZipFile zip = new ZipFile(strZipPath);

            // 포통 540개의 파일이 존재한다고 본다.
            int nInterval = (int) ((540.0 / 100.0) + 0.5);
            //
            // Here we start to iterate each entries inside
            // sample.zip
            //
            int nI = 0;
            for (Enumeration<? extends ZipEntry> e = zip.entries(); e
                    .hasMoreElements();) {
                //
                // Get ZipEntry which is a file or a directory
                //
                ZipEntry entry = (ZipEntry) e.nextElement();

                if (entry.isDirectory()) {
                    continue;
                }

                InputStream is = zip.getInputStream(entry);

                sName = entry.getName();
                ;
                nIndex = sName.indexOf("resourcezip/");
                if (nIndex != -1) {
                    sName = sName.substring(nIndex + 12);
                }

                nIndex = sName.indexOf('.');
                if (nIndex == 0) // .DS_Store같은 파일인
                    continue;
                nIndex = sName.indexOf('/'); // 내부에 폴더가 존재하면 폴더를 만든다.
                if (nIndex != -1) {
                    // 서브 폴더에도 포함된다.
                    if (sName.indexOf(".DS_Store") != -1) // .DS_Store같은 파일인
                        continue;

                    int nBeforIndex = nIndex;
                    while (nIndex != -1) {
                        nIndex = sName.indexOf(nIndex + 1, '/');
                        if (nIndex == -1)
                            break;
                        nBeforIndex = nIndex;
                    }

                    String sDir = strTargetDir + "/"
                            + sName.subSequence(0, nBeforIndex);
                    File ff = new File(sDir);
                    if (!ff.exists())
                        ff.mkdirs();
                }

                strTargetPath = strTargetDir + "/" + sName;
                if (CopyFile(is, strTargetPath) == false) {
                    Log.e("AlienInvasion", "Unzip Failed " + sName);
                }

                if (nI % nInterval == 0) {
                    // ------------------------------------------------------------
                    Message msg = AlienInvasion.gMainActivity.mUnZipHandler
                            .obtainMessage();
                    Bundle b = new Bundle();
                    b.putInt("id", 1);
                    msg.setData(b);
                    AlienInvasion.gMainActivity.mUnZipHandler.sendMessage(msg);
                    // ------------------------------------------------------------
                }
                nI++;
            }
        } catch (ZipException e) {
            e.printStackTrace();
            Log.e("AlienInvasion", e.getMessage());
        } catch (IOException e) {
            e.printStackTrace();
            Log.e("AlienInvasion", e.getMessage());
        }
    }

    // 안드로이드 IAP는 바로 저장하지 않고, 일단 파일에 저장하자.
    static public boolean WriteCacheProduct(String sProductID) {
        boolean bResult = false;
        FileReader reader = null;
        FileWriter write = null;
        File fDir = AlienInvasion.gMainActivity.getCacheDir();
        File fFile = new File(fDir.getAbsolutePath() + "/.file");
        try {
            String sWrite = "";
            // 기존의 데이터가 존재하면 +Cnt 해준다.
            if (fFile.isFile()) {
                char[] buf = new char[(int) fFile.length()];
                reader = new FileReader(fFile);
                reader.read(buf);
                try {
                    reader.close();
                    reader = null;
                } catch (IOException e) {
                    e.printStackTrace();
                }

                sWrite = new String(buf);
                sWrite += "\n";
            }
            write = new FileWriter(fFile, false);
            sWrite += sProductID;
            write.write(sWrite.toCharArray());
            bResult = true;
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            if (reader != null) {
                try {
                    reader.close();
                    reader = null;
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            if (write != null) {
                try {
                    write.close();
                    write = null;
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }

        return bResult;
    }

    static public String ReadCacheProduct() {
        String sResult = "";
        Log.i("JAVA", "ReadCacheProduct");
        FileReader reader = null;
        File fDir = AlienInvasion.gMainActivity.getCacheDir();
        File fFile = new File(fDir.getAbsolutePath() + "/.file");
        try {
            // 기존의 데이터가 존재하면 +Cnt 해준다.
            if (fFile.isFile()) {
                char[] buf = new char[(int) fFile.length()];
                reader = new FileReader(fFile);
                reader.read(buf);
                try {
                    reader.close();
                    reader = null;
                } catch (IOException e) {
                    e.printStackTrace();
                }

                sResult = new String(buf);
                // 파일을 지운다.
                fFile.delete();
            }
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            if (reader != null) {
                try {
                    reader.close();
                    reader = null;
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }

        if(sResult.length() > 0)
        {
            Log.i("JAVA", "적용됨 ID:" + sResult);
        }
        return sResult;
    }

    static public int SendMessageToJava(int bID, int lParam) {
        if (bID == 1)
            AlienInvasion.gMainActivity.ShowAdView(true);
        else if (bID == 0)
            AlienInvasion.gMainActivity.ShowAdView(false);
        else if (bID == 2)
            AlienInvasion.gMainActivity.BuyApp();
        else {
            Message msg = AlienInvasion.gMainActivity.mAppHandler
                    .obtainMessage();
            Bundle b = new Bundle();
            b.putInt("id", bID);
            b.putInt("param", lParam);
            msg.setData(b);
            AlienInvasion.gMainActivity.mAppHandler.sendMessage(msg);
        }
        return 0;
    }

    static public String GetAppVersion() {
        String version = "0";
        try {
            PackageInfo i = AlienInvasion.gMainActivity.getPackageManager()
                    .getPackageInfo(
                            AlienInvasion.gMainActivity.getPackageName(), 0);
            version = i.versionName;
        } catch (NameNotFoundException e) {

        }
        return version;
    }

    static public String GetBundleID() {
        String ID = AlienInvasion.gMainActivity.getPackageName();
        if (ID == null || (ID != null && ID.length() == 0))
            ID = "ID is NULL";
        return ID;
    }

    static public String GetModel() {
        String ID = null;
        try {
            ID = String.format("%s %s %s", Build.DEVICE, Build.BRAND,
                    Build.VERSION.RELEASE);
        } catch (Exception ex) {
            ID = "_XYZ_";
        }

        return ID;

    }

    static public String CountryName() {

        String ID = "";

        try {
            ID = Locale.getDefault().getDisplayCountry();
            if (ID == null || (ID != null && ID.length() == 0))
                ID = "Country is NULL";
            ID = ID + "(" + Locale.getDefault().toString() + ")";
        } catch (Exception ex) {
            ID = "No Country Name";
        }
        return ID;

    }

    static public String GetUniqueID() {
        String ID = Secure.getString(
                AlienInvasion.gMainActivity.getContentResolver(),
                Secure.ANDROID_ID);
        if (ID == null || (ID != null && ID.length() == 0))
            ID = "ID is NULL";
        return "_AND_" + ID;

    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Game Center
    public void SetMyID(String sMyID)
    {
        if (mlGLContext != 0)
            sglSetMyID(mlGLContext,sMyID);
    }
    public void OnReceivedData(String sSender,byte[] btData)
    {
        if (mlGLContext != 0)
            sglOnReceivedData(mlGLContext,sSender,btData);
    }
    public void StartMultiplayGame()
    {
        if (mlGLContext != 0)
            sglStartMultiplayGame(mlGLContext);
    }
    public void StopMultiplayGame()
    {
        if (mlGLContext != 0)
            sglStopMultiplayGame(mlGLContext);
    }

    static public int sglShowBestScoreDlg(int nType, String sUserID,
                                          String sMessage) {
        Log.i("JAVA", "sglShowBestScoreDlg");
        if (nType == 0) {
            Log.i("JAVA", "Best score dialog disabled with Play Games v1 removal.");
        } else // nType == 1 평점. (골드)
        {
            Message msg = AlienInvasion.gMainActivity.mAppHandler
                    .obtainMessage();
            Bundle b = new Bundle();
            b.putInt("id", 3);
            b.putInt("type", nType); // 1:레이더 보여준다. 2:골드 보여준다
            msg.setData(b);
            AlienInvasion.gMainActivity.mAppHandler.sendMessage(msg);
        }
        return 0;
    }

    static public int sglSendBestScore(int nBestScore, int nARate, int nDRate,
                                       int nTRate, int nMTRate, int nWinner) {

        Message msg = AlienInvasion.gMainActivity.mAppHandler.obtainMessage();
        Bundle b = new Bundle();
        b.putInt("id", 6);
        b.putInt(LEADERBOARDID, nBestScore);
        b.putInt(ATACHID_UPGRADE_ATTACK, nARate);
        b.putInt(ATACHID_UPGRADE_DEFENCE, nDRate);
        b.putInt(ATACHID_UPGRADE_TOWER, nTRate);
        b.putInt(ATACHID_UPGRADE_MISSTOWER, nMTRate);
        b.putInt(ATACHID_YOUAREWINNER, nWinner);
        msg.setData(b);
        AlienInvasion.gMainActivity.mAppHandler.sendMessage(msg);

        return 0;
    }

    static public int sglShowSNSDlg()
    {
        SendMessageToJava(7,0);
        return 0;
    }

    static public int sglFindPlyer(int nResetFindTime,int nMatchFilter,int nMatchGroup)
    {
        if(AlienInvasion.gMainActivity.mGLView.mRender != null&& AlienInvasion.gMainActivity.mGLView.mRender.mSongGL != null)
            AlienInvasion.gMainActivity.mGLView.mRender.mSongGL.SetMultiplayState(SongGLLib.SYNC_MATCHINGERROR);
        return 0;
    }

    static public int sglInitFindPlayer()
    {
        if(AlienInvasion.gMainActivity.mGLView.mRender != null&& AlienInvasion.gMainActivity.mGLView.mRender.mSongGL != null)
            AlienInvasion.gMainActivity.mGLView.mRender.mSongGL.SetMultiplayState(SongGLLib.SYNC_NONE);
        return 0;
    }

    static public int sglPlayOut()
    {
        if(AlienInvasion.gMainActivity.mGLView.mRender != null&& AlienInvasion.gMainActivity.mGLView.mRender.mSongGL != null)
            AlienInvasion.gMainActivity.mGLView.mRender.mSongGL.SetMultiplayState(SongGLLib.SYNC_NONE);
        return 0;
    }

    static public int sglSendMultiplayData(byte[] btData,int bReliable)
    {
        return 0;
    }

    public void OnSigned(boolean bSuccess)
    {
        if (mlGLContext != 0)
            sglOnSigned(mlGLContext, bSuccess);
    }

    public final static int SYNC_NONE = 0;
    public final static int SYNC_FINDPLAYER = 1;    //플레이어를 찾고 있다.
    public final static int SYNC_MATCHING = 2;      //매칭중에 있다.
    public final static int SYNC_MATCHINGERROR = 3;
    public final static int SYNC_PLYERT_STATE = 4;  //나의 플레이어 상태를 보내준다.
    public final static int SYNC_START_CLOCK = 5;   //클럭을 보내준다.
    public final static int SYNC_READYPLAY = 6;     //자신의 팀아이디 등등을 보내준다.
    public final static int SYNC_PLAYING = 7;       //플레이중

    public void SetMultiplayState(long nState)
    {
        if (mlGLContext != 0)
            sglSetMultiplayState(mlGLContext, nState);
    }
    public void ResetFindTime(long nWaitTime)
    {
        if (mlGLContext != 0)
            sglResetFindTime(mlGLContext, nWaitTime);
    }
    //Game Center End
    ///////////////////////////////////////////////////////////////////////////////////////////////

    static public String HttpRequestXmlOrg(String url, String post) {

        String sResult = null;
        InputStream in = null;
        CHttpRequest con = new CHttpRequest(url, true, null);
        try {
            in = con.SendData(post.getBytes());
            byte[] btOutData = CHttpRequest.readContent(in, 1024);
            sResult = new String(btOutData, 0, btOutData.length);
        } catch (CExceptionNetwork e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            if (in != null) {
                try {
                    in.close();
                } catch (IOException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }
            }

        }
        // if(sResult != null)
        // {
        // Log.i("AlienInvasion", sResult);
        // }
        return sResult;
    }
}
