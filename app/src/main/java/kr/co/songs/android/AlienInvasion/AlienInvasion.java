package kr.co.songs.android.AlienInvasion;

import java.util.Date;

import org.jsoup.Jsoup;

import android.Manifest;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.util.Log;
import android.view.Display;
import android.view.KeyEvent;
import android.view.View;
import android.view.WindowManager;
import android.widget.Toast;

import com.android.googleservice.GameHelper;
import com.android.googleservice.IAPMan;
import com.android.googleservice.Multiplay;
import com.google.android.gms.ads.AdListener;
import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.AdView;
import com.google.android.gms.ads.MobileAds;
import com.google.android.gms.common.GooglePlayServicesUtil;
import com.google.android.gms.common.api.GoogleApiClient;
import com.google.android.gms.games.Games;
import com.google.android.gms.plus.Plus;
import com.google.android.gms.plus.model.people.Person;

public class AlienInvasion extends Activity implements
		SensorEventListener,
		GameHelper.GameHelperListener // GoogleApiClient.ConnectionCallbacks,
										// GoogleApiClient.OnConnectionFailedListener
{
	static final int PROGRESS_DIALOG = 0;
	protected AlienInvasionView mGLView = null;
	protected AdView mADView = null;
	protected boolean mbHideAd = false;
	boolean mFinish = false; // 종료일때는 Pause를 하지 말자.
	public static AlienInvasion gMainActivity = null;
	public static AlienInvasionView gGLView = null;

	// 핸들러는 메인스레드에서 생성해야 하고 gl스레드에서 생성하면 에러가 난다.
	public Handler mSoundPoolHandler = null;

	private SensorManager mSensorManager;
    private Sensor accSensor;
    
//	protected GameHelper mHelper = null; //2019.02 Deleted GameHelper
	protected Multiplay	mMultiplay = null;
	public GoogleApiClient getApiClient() {
//		if (mHelper != null) {
//			return mHelper.getApiClient();
//		}
		return null;
	}
	public Multiplay getMultiplay() { return mMultiplay;}
	public SongGLLib getSongGLLib() { return mGLView.mRender.mSongGL;}
	// UnZip 웨인팅 커셔....
	public ProgressDialog mProDlg;
	public Handler mUnZipHandler = new Handler() {
		public void handleMessage(Message msg) {
			int nID = msg.getData().getInt("id");
			if (nID == 0) // 다이얼로그를 보이게 한다.
			{
				showDialog(PROGRESS_DIALOG);
			} else if (nID == 1) // 프로그레스바를 움직인다.
			{
				int nN = mProDlg.getProgress() + 1;
				if (nN >= 100)
					nN = 100;
				mProDlg.setProgress(nN);
			} else // 프로그레스 바를 종료한다.
			{
				if (mProDlg != null)
					mProDlg.dismiss();
				mProDlg = null;
			}
		}
	};

	public Handler mAppHandler = new Handler() {
		// final int GSL_PROGRESS_START=0x010001; //시작
		public void handleMessage(Message msg) {
			int nID = msg.getData().getInt("id");
			if (nID == 0) // 다이얼로그를 보이게 한다.
			{
				// Modified 2014.12.08 구글 플레이로 애드몹 서비스.
				// mADView.loadAd(new AdRequest()); //광고를 다시 로딩한다.
				mADView.loadAd(new AdRequest.Builder().addTestDevice(
						AdRequest.DEVICE_ID_EMULATOR).build());// 광고를 다시 로딩한다.
				mbHideAd = false;
			} else if (nID == 1) // 프로그레스바를 움직인다.
			{
				// Modified 2014.12.08 구글 플레이로 애드몹 서비스.
				// mADView.stopLoading(); //광고로딩을 스톱한다.
				mADView.setVisibility(View.GONE);
				mbHideAd = true;
				// SongGLLib.sglSendMessage(GSL_PROGRESS_START, 0, 0, 0, 0);
			} else if (nID == 2) {
				Intent intent = new Intent(
						Intent.ACTION_VIEW,
						Uri.parse("market://details?id=" + SongGLLib.GetBundleID()));
				startActivity(intent);
			} else if (nID == 3) {
				String sMsg = getResources().getString(R.string.ratingreportR);
				String sTitle = getResources()
						.getString(R.string.ratingreportT);
				String sYes = getResources().getString(R.string.yes);
				String sNo = getResources().getString(R.string.no);
				int nType = msg.getData().getInt("type");
				if (nType == 2) // 1:레이더. 2:골드.
					sMsg = getResources().getString(R.string.ratingreportG);

				AlertDialog.Builder alert_confirm = new AlertDialog.Builder(
						AlienInvasion.gMainActivity);
				alert_confirm
						.setTitle(sTitle)
						.setMessage(sMsg)
						.setCancelable(false)
						.setPositiveButton(sYes,
								new DialogInterface.OnClickListener() {
									@Override
									public void onClick(DialogInterface dialog,
											int which) {
										// 'YES'
										startActivity(new Intent(
												Intent.ACTION_VIEW,
												Uri.parse("market://details?id="
														+ SongGLLib
																.GetBundleID())));
										SongGLLib.sglSetRateInfo(-1);
										return;
									}
								})
						.setNegativeButton(sNo,
								new DialogInterface.OnClickListener() {
									@Override
									public void onClick(DialogInterface dialog,
											int which) {
										// 'No'
										SongGLLib.sglSetRateInfo(1);
										return;
									}
								});

				AlertDialog alert = alert_confirm.create();
				alert.show();
			} else if (nID == 6) // BestScore에 올려준다.
			{
				// -----------------------------------------------------------------------------------------------------
				// Added Google LeaderBoard
				// -----------------------------------------------------------------------------------------------------
				try {
					GoogleApiClient apiClient = AlienInvasion.gMainActivity
							.getApiClient();
					if (apiClient == null
							|| (apiClient != null && !apiClient.isConnected()))
						return;

					int nBestScore = msg.getData().getInt(
							SongGLLib.LEADERBOARDID);
					int nARate = msg.getData().getInt(
							SongGLLib.ATACHID_UPGRADE_ATTACK);
					int nDRate = msg.getData().getInt(
							SongGLLib.ATACHID_UPGRADE_DEFENCE);
					int nTRate = msg.getData().getInt(
							SongGLLib.ATACHID_UPGRADE_TOWER);
					int nMTRate = msg.getData().getInt(
							SongGLLib.ATACHID_UPGRADE_MISSTOWER);
					int nWinner = msg.getData().getInt(
							SongGLLib.ATACHID_YOUAREWINNER);

					if (nBestScore != 0)
						Games.Leaderboards.submitScore(apiClient,
								SongGLLib.LEADERBOARDID, nBestScore);
					if (nARate != 0)
						Games.Achievements.increment(apiClient,
								SongGLLib.ATACHID_UPGRADE_ATTACK, nARate);
					if (nDRate != 0)
						Games.Achievements.increment(apiClient,
								SongGLLib.ATACHID_UPGRADE_DEFENCE, nDRate);
					if (nTRate != 0)
						Games.Achievements.increment(apiClient,
								SongGLLib.ATACHID_UPGRADE_TOWER, nTRate);
					if (nMTRate != 0)
						Games.Achievements.increment(apiClient,
								SongGLLib.ATACHID_UPGRADE_MISSTOWER, nMTRate);
					if (nWinner > 1)
						Games.Achievements.unlock(apiClient,
								SongGLLib.ATACHID_YOUAREWINNER);
				} catch (Exception ex) {
					Log.i("JAVA",
							"sglSendBestScore: Exception %s"
									+ ex.getLocalizedMessage());
				}
				
				// -----------------------------------------------------------------------------------------------------
			}
			else if(nID == 7) //SNS에 데이터를 올려준다.
			{
				Intent mg = new Intent(Intent.ACTION_SEND);
				String sContents = getResources().getString(R.string.app_name);
				mg.addCategory(Intent.CATEGORY_DEFAULT);
				mg.putExtra(Intent.EXTRA_SUBJECT, getResources().getString(R.string.app_name));
				
				sContents += "\n";
				sContents += getResources().getString(R.string.SNSContents);
				sContents += "\n";
				sContents += "market://details?id=" + SongGLLib.GetBundleID();
				
				mg.putExtra(Intent.EXTRA_TEXT, sContents);
				
				//mg.putExtra(Intent.EXTRA_TITLE, "제목");
				mg.setType("text/plain");    
				startActivity(Intent.createChooser(mg, getResources().getString(R.string.SNSMsgTitle)));
			}
			else if(nID == 8) //sglFindPlayer
			{
				try {
					GoogleApiClient apiClient = AlienInvasion.gMainActivity
							.getApiClient();
					if (apiClient == null
							|| (apiClient != null && !apiClient.isConnected()))
						return;
					
					int arr[] = msg.getData().getIntArray("ArrData");
					
					mMultiplay.FindPlyer(arr[0], arr[1], arr[2]);
				}
				catch( Exception ex)
				{
					Log.e("JAVA", ex.getLocalizedMessage());
				}
			}
			else if(nID == 9) //sglPlayOut
			{
				try {
					GoogleApiClient apiClient = AlienInvasion.gMainActivity
							.getApiClient();
					if (apiClient == null
							|| (apiClient != null && !apiClient.isConnected()))
						return;
					mMultiplay.LogOut();
				}
				catch( Exception ex)
				{
					Log.e("JAVA", ex.getLocalizedMessage());
				}
			}
			else if(nID == 10) //matching중에 다시 find해버리면 roomID에서 나가주어야 한다.
			{
				
				try {
					GoogleApiClient apiClient = AlienInvasion.gMainActivity
							.getApiClient();
					if (apiClient == null
							|| (apiClient != null && !apiClient.isConnected()))
						return;
					
					String roomID = msg.getData().getString("roomid");
					Games.RealTimeMultiplayer.leave(apiClient,AlienInvasion.gMainActivity.getMultiplay(), roomID);
					Log.i("Multiplay", "RealTimeMultiplayer leav = roomid " + roomID);
				}
				catch( Exception ex)
				{
					Log.e("JAVA", ex.getLocalizedMessage());
				}
			}
			else if(nID == 11) //새로운 앱이 존재 한다.
			{
				SongGLLib.sglSetAppNewVersion(true);
			}
		}
	};

	protected Dialog onCreateDialog(int id) {
		switch (id) {
		case PROGRESS_DIALOG:
			mProDlg = new ProgressDialog(this);
			mProDlg.setProgressStyle(ProgressDialog.STYLE_HORIZONTAL);
			mProDlg.setMessage(gMainActivity.getResources().getString(
					kr.co.songs.android.AlienInvasion.R.string.unpackmsg));
			mProDlg.setCancelable(false);
			return mProDlg;
		}
		return null;
	}

	// UnZip 웨인팅 커셔.... End

	// public static int gDisplayType = SongGLLib.DISPLAY_IPAD;

	// IAP Start
	/**
	 * The SharedPreferences key for recording whether we initialized the
	 * database. If false, then we perform a RestoreTransactions request to get
	 * all the purchases for this user.
	 */
	private static final String TAG = "AlienInvasionIAP";
	private static final String DB_INITIALIZED = "db_initialized";
	
	
	private IAPMan mIAPMan;

	// IAP End

	void InitDisplayType() {
		WindowManager window = (WindowManager) getSystemService(Context.WINDOW_SERVICE);
		Display display = window.getDefaultDisplay();

		// int width = display.getWidth();
		int height = display.getHeight();

		// if(width >= 960) //아이패드 타입
		// 높이를 기준으로 변경하였다.
		// 요즘 와이드로 나오기 때문에 높이는 작은 경우가 있다.
		// 높이가 작으면 폰형태로 보여지게 해주고, 크면 패드 형태로 벼여주는게 좋을 듯하다.
		if (height >= 1000) // 아이패드 타입
		{
			SongGLLib.gDisplayType = SongGLLib.DISPLAY_IPAD;
		} 
		else 
		{
			SongGLLib.gDisplayType = SongGLLib.DISPLAY_IPHONE;
		}
		
		SongGLLib.sglSetDisplayType(SongGLLib.gDisplayType);

	}

	static final int REQUEST_CODE_RECOVER_PLAY_SERVICES = 1001;

	void showErrorDialog(int code) {
		GooglePlayServicesUtil.getErrorDialog(code, this,
				REQUEST_CODE_RECOVER_PLAY_SERVICES).show();
	}

	// private boolean checkPlayServices() {
	// int status = GooglePlayServicesUtil.isGooglePlayServicesAvailable(this);
	// if (status != ConnectionResult.SUCCESS) {
	// if (GooglePlayServicesUtil.isUserRecoverableError(status)) {
	// showErrorDialog(status);
	// } else {
	// Toast.makeText(this, "This device is not supported.",
	// Toast.LENGTH_LONG).show();
	// finish();
	// }
	// return false;
	// }
	// return true;
	// }

	// private TimerTask mADTask;
	// private final Handler mADHanlder = new Handler();

	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		Log.i("JAVA", "Activity onCreate");

		mSoundPoolHandler = new Handler();
		// 다중언어지
		SongGLLib.sglSetLocalizationDir(getResources().getString(
				R.string.local_dir));
		SoundMan.sglInitSound();
		//Added By Song 2015.01.06 멀티플레이를 지원하면서 GetUserInfo를 어디선가 불러쓰기 때문에 시작시에 미리 정의 하자.
		SongGLLib.sglSetRootPath(SongGLLib.getPath());
		InitDisplayType();

		setContentView(R.layout.main);

		mGLView = (AlienInvasionView) findViewById(R.id.glViewMine);
		mADView = (AdView) findViewById(R.id.adView);
		mADView.setVisibility(View.VISIBLE); // 광고를 미리 보여지게 하여야 지엘뷰어가 자리를 잡는다.

		//2019.02 Deleted GameHelper
//		if (mHelper == null)
//			mHelper = new GameHelper(this, GameHelper.CLIENT_GAMES
//					| GameHelper.CLIENT_PLUS);
//		mHelper.enableDebugLog(true);
//		mHelper.setup(this);

		gGLView = mGLView;
		gMainActivity = this;

		// IAP Start
		mIAPMan = new IAPMan(this);
		mIAPMan.InitIAP(getString(R.string.inapp_key));

		//2019.02 광고 수정함.
		MobileAds.initialize(this,"ca-app-pub-2468045858112255~2953424669");
		
		
		mADView.setAdListener(new AdListener() {
			// Modified 2014.12.08 이베트 변경됨.
			public void onAdLoaded() {
				Log.i("JAVA", "onAdLoaded");
				if(mbHideAd == false)
					mADView.setVisibility(View.VISIBLE);
			}

			public void onAdFailedToLoad(int errorCode) {
				Log.i("JAVA", "onAdFailedToLoad");
				mADView.setVisibility(View.GONE);
			}

			public void onAdOpened() {
				Log.i("JAVA", "onAdOpened");
				AlienInvasion.gGLView.SendMessage(
						SongGLLib.SGL_CLICKED_ADMOB_ANDROID, 0, 0);
			}

			public void onAdClosed() {
				Log.i("JAVA", "onAdClosed");
				mADView.setVisibility(View.GONE);
			}

			public void onAdLeftApplication() {
				Log.i("JAVA", "onAdClosed");
			}
		});
		
		
		mMultiplay = new Multiplay(this);
		mMultiplay.enableDebugLog(true);
		//센서 매니저 얻기
        mSensorManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
        //엑셀러로미터 센서(가속)
        accSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);

		checkPermission();
	}

	//2019.02.12
	static final int MY_PERMISSION_REQUEST_WRITE_EXTERNAL_STORAGE = 100;
	private void checkPermission() {
		try
		{
			if (ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) == PackageManager.PERMISSION_GRANTED)
			{
				SongGLLib.sglUnzip3DResource();
			}
			else if (ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED)
			{

				// Should we show an explanation?
				if (ActivityCompat.shouldShowRequestPermissionRationale(this, Manifest.permission.WRITE_EXTERNAL_STORAGE)) {
				}
				//Location정보를 추가한다.
				ActivityCompat.requestPermissions(this,new String[]{ Manifest.permission.WRITE_EXTERNAL_STORAGE },
						MY_PERMISSION_REQUEST_WRITE_EXTERNAL_STORAGE);

			}
		}
		catch(Exception e){
			e.printStackTrace();
			Log.e("JavaSongs",e.getLocalizedMessage());
		}
	}

	@Override
	public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
		try
		{
			switch (requestCode) {
				case MY_PERMISSION_REQUEST_WRITE_EXTERNAL_STORAGE:
					if (grantResults[0] == PackageManager.PERMISSION_GRANTED) {
						SongGLLib.sglUnzip3DResource();
					}
					else
					{

						Log.d("JavaSongs", "onRequestPermissionsResult Permission always deny");
					}
					break;
			}
		}
		catch(Exception e)
		{
			Log.e("JavaSongs", "onRequestPermissionsResult " + e.getLocalizedMessage());
		}
	}
	
	
	private long value(String string) {
	    string = string.trim();
	    if( string.contains( "." )){ 
	        final int index = string.lastIndexOf( "." );
	        return value( string.substring( 0, index ))* 100 + value( string.substring( index + 1 )); 
	    }
	    else {
	        return Long.valueOf( string ); 
	    }
	}
	
	private boolean isNewVersion(){
	    try
	    {
	        String curVersion = getPackageManager().getPackageInfo(SongGLLib.GetBundleID(), 0).versionName;
	        String newVersion = curVersion;
	        newVersion = Jsoup.connect("https://play.google.com/store/apps/details?id=" + SongGLLib.GetBundleID() + "&hl=en")
	                .timeout(30000)
	                .userAgent("Mozilla/5.0 (Windows; U; WindowsNT 5.1; en-US; rv1.8.1.6) Gecko/20070725 Firefox/2.0.0.6")
	                .referrer("http://www.google.com")
	                .get()
	                .select("div[itemprop=softwareVersion]")
	                .first()
	                .ownText();
	        return (value(curVersion) < value(newVersion)) ? true : false;
	    } catch (Exception e) {
	        e.printStackTrace();
	        return false;
	    }

	}
	private void CheckNewVersion()
	{
		new Thread(new Runnable() {
            @Override
            public void run() 
            {	
                synchronized(AlienInvasion.class) 
                {
                	if(isNewVersion())
                	{
                		try {
							Message msg = AlienInvasion.gMainActivity.mAppHandler.obtainMessage();
							Bundle b = new Bundle();
							b.putInt("id", 11);
							msg.setData(b);
							AlienInvasion.gMainActivity.mAppHandler.sendMessage(msg);
						}
						catch(Exception e)
						{
							Log.e("JavaSongs","CheckNewVierion " + e.getLocalizedMessage());
						}
                	}
                }
            }
        }).start();
	}
	
	

	/**
	 * Called when this activity becomes visible.
	 */
	@Override
	protected void onStart() {
		super.onStart();

		Log.i("JAVA", "Activity onStart");
        //2019.02 Deleted GameHelper
//		mHelper.onStart(this);
		CheckNewVersion();
	}

	/**
	 * Called when this activity is no longer visible.
	 */
	@Override
	protected void onStop() {
		super.onStop();
		Log.i("JAVA", "Activity onStop");

		//2019.02 Deleted GameHelper
		// Modified 2014.12.08 Google Game Service
//		mHelper.onStop();
	}

	@Override
	protected void onPause() {
		super.onPause();
		Log.i("JAVA", "Activity onPause");

		if (mFinish == false) {
			if (mGLView != null) {
				// 다른 것을 해제 하기전에 뷰를 먼저 포즈시켜야 rendering할때 에러가 안난다.
				mGLView.onPause();

				//2018.02.12
				if (ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) == PackageManager.PERMISSION_GRANTED)
					SongGLLib.sglPlayBgSoundOnAcitvity(false);

			}
			// Moidfied 2014.12.08 애드
			if (mADView != null)
				mADView.pause();

			Log.i("JAVA", "Activity onPause mFinish false ");
		}
		mSensorManager.unregisterListener(this);
		}

	@Override
	protected void onResume() {

		super.onResume();
		Log.i("JAVA", "Activity onResume");

		if (mGLView != null) {
			// 재시작
			SongGLLib.sglPlayBgSoundOnAcitvity(true);
			mGLView.onResume();

			if (mADView.getVisibility() == View.VISIBLE) {
				// Moidfied 2014.12.08 애드
				if (mADView != null)
					mADView.resume();
			}

			Log.i("JAVA",
					"Activity onResume mGLView != null and mGLView.onResume()");
		}
        mSensorManager.registerListener(this, accSensor,SensorManager.SENSOR_DELAY_FASTEST);
	}

	@Override
	protected void onSaveInstanceState(Bundle outState) {
		Log.i("JAVA", "Activity onSaveInstanceState");
		super.onSaveInstanceState(outState);

	}

	@Override
	protected void onRestoreInstanceState(Bundle savedInstanceState) {
		Log.i("JAVA", "Activity onRestoreInstanceState");
		super.onRestoreInstanceState(savedInstanceState);

	}

	@Override
	public void finish() {
		Log.i("JAVA", "Activity finish");

		// finish를 하면 onPause를 먼저 부른다.
		// 그렇게 되면 mSurface에서 종료이벤트를 발생하여
		// 나중에 Destory()에서 또한번 종료하기 때문에 에러가 난다.
		mFinish = true;
		super.finish();
	}

	@Override
	protected void onDestroy() {
		Log.i("JAVA", "Activity Destory");

		if (mADView != null) {
			mADView.destroy();
			mADView = null;
		}

		if (mGLView != null) {
			mGLView.Destory();
			mGLView = null;
			mFinish = false;
		}

		mIAPMan.onDestroy();
		SongGLLib.sglTextUnitRelease();
		super.onDestroy();
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		if (event.getAction() == KeyEvent.ACTION_DOWN) {
			switch (keyCode) {
			case KeyEvent.KEYCODE_BACK:
				// finish();
				return false;
			}
		}
		return super.onKeyDown(keyCode, event);
	}

	/**
	 * IAP 아이템을 요청한다.
	 * 
	 * @param sID
	 *            : ProductID
	 */
	public void RequestProductItem(String sID,int nType) {
		
		if(nType == 4)
			mIAPMan.Buy(sID,true);
		else
			mIAPMan.Buy(sID,false);
	}

	public void BuyApp() {
		Message msg = AlienInvasion.gMainActivity.mAppHandler.obtainMessage();
		Bundle b = new Bundle();
		b.putInt("id", 2);
		msg.setData(b);
		AlienInvasion.gMainActivity.mAppHandler.sendMessage(msg);
	}

	public void ShowAdView(boolean bShow) {
		if (bShow) {
			Message msg = AlienInvasion.gMainActivity.mAppHandler
					.obtainMessage();
			Bundle b = new Bundle();
			b.putInt("id", 0);
			msg.setData(b);
			AlienInvasion.gMainActivity.mAppHandler.sendMessage(msg);
		} else {
			Message msg = AlienInvasion.gMainActivity.mAppHandler
					.obtainMessage();
			Bundle b = new Bundle();
			b.putInt("id", 1);
			msg.setData(b);
			AlienInvasion.gMainActivity.mAppHandler.sendMessage(msg);
		}
	}

	@Override
	public void onActivityResult(int requestCode, int resultCode, Intent data) {
		super.onActivityResult(requestCode, resultCode, data);
		
		if(mIAPMan != null && IAPMan.RC_REQUEST == requestCode)
		{
			mIAPMan.onActivityResult(requestCode, resultCode, data);
			return ;
		}
		//2019.02 Deleted GameHelper
//		mHelper.onActivityResult(requestCode, resultCode, data);
	}

	// ---------------------------------------------------------------
	// Google Game Service
	@Override
	public void onSignInFailed() {
		Toast.makeText(this, "can not sign in google games.",Toast.LENGTH_LONG).show();
		if(gGLView != null && gGLView.mRender != null && gGLView.mRender.mSongGL != null)
			gGLView.mRender.mSongGL.OnSigned(false);
	}

	@Override
	public void onSignInSucceeded() 
	{
		SongGLLib.sglSendBestScoreNative();
		
		if(gGLView != null && gGLView.mRender != null && gGLView.mRender.mSongGL != null)
			gGLView.mRender.mSongGL.OnSigned(true);
		
		if (Plus.PeopleApi.getCurrentPerson(getApiClient()) != null) {
	        final Person currentPerson = Plus.PeopleApi.getCurrentPerson(getApiClient());
	        String personName = currentPerson.getDisplayName();
	        getSongGLLib().SetMyID(personName);
	    }
		
	}
	// ---------------------------------------------------------------
	
	// ---------------------------------------------------------------
	// 가속
	//정확도에 대한 메소드 호출 (사용안함)
    public void onAccuracyChanged(Sensor sensor, int accuracy) {
  
    }
      
    Date mbeforeDt = new Date();
    //센서값 얻어오기
    public void onSensorChanged(SensorEvent event) 
    {
    	Date now = new Date();
    	long intervalTime = now.getTime() - mbeforeDt.getTime(); //밀리
    	if(intervalTime > 200) //센서가 너무 빨리 드러와서 0.2초의 
    	{
	        //Sensor sensor = event.sensor;
	        if (event.sensor.getType() == Sensor.TYPE_ACCELEROMETER)
	        {
	        	if(gGLView != null && gGLView.mRender != null && gGLView.mRender.mSongGL != null)
	        		gGLView.mRender.mSongGL.AccelertionData(event.values[0]*0.1f,0.f,0.f);
	        }
	        mbeforeDt = now;
    	}
    }
 // ---------------------------------------------------------------
}