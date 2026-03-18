package kr.co.songs.android.AlienInvasion;

import kr.co.songs.android.AlienInvasion.AlienInvasion;
import kr.co.songs.android.AlienInvasion.R;
import android.content.Context;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.media.SoundPool;
import android.os.Handler;
import android.os.Vibrator;
import android.util.Log;

public class SoundMan {
	
	public static native void sglInitSound();
	
	private static MediaPlayer gpPlayer = null;
	
	public static SoundPool   gSoundPool = null;
	
	private static Vibrator vibe = null;
	
	public SoundMan() 
	{
		
	}
	
	
	
	static public int PlayVibrate()
	{
		if(vibe == null)
			vibe = (Vibrator) AlienInvasion.gMainActivity.getSystemService(Context.VIBRATOR_SERVICE);
		vibe.vibrate(200);
		return 0;
	}
	
	static public int MakeSystemSoundID(String sID)
	{
		int nID = 0;
		if(gSoundPool == null)
			gSoundPool = new SoundPool(20, AudioManager.STREAM_MUSIC, 0); //soundpool 생성
		
		
		
		if(sID.equals("Click"))
			nID = gSoundPool.load(AlienInvasion.gMainActivity, R.raw.click, 1);
		else if(sID.equals("Buzzer"))
			nID = gSoundPool.load(AlienInvasion.gMainActivity, R.raw.buzzer, 1);
		else if(sID.equals("EneryUp"))
			nID = gSoundPool.load(AlienInvasion.gMainActivity, R.raw.eneryup, 1);
		else if(sID.equals("Fire1"))
			nID = gSoundPool.load(AlienInvasion.gMainActivity, R.raw.fire1, 1);
		else if(sID.equals("NearBombed"))
			nID = gSoundPool.load(AlienInvasion.gMainActivity, R.raw.nearbombed, 1);
		else if(sID.equals("FarBombed"))
			nID = gSoundPool.load(AlienInvasion.gMainActivity, R.raw.farbombed, 1);
		else if(sID.equals("FireET"))
			nID = gSoundPool.load(AlienInvasion.gMainActivity, R.raw.fireet, 1);
		else if(sID.equals("Relive"))
			nID = gSoundPool.load(AlienInvasion.gMainActivity, R.raw.relive, 1);
		else if(sID.equals("FLY"))
			nID = gSoundPool.load(AlienInvasion.gMainActivity, R.raw.fly, 1);
		else if(sID.equals("gun"))
			nID = gSoundPool.load(AlienInvasion.gMainActivity, R.raw.gun, 1);
		else if(sID.equals("raser"))
			nID = gSoundPool.load(AlienInvasion.gMainActivity, R.raw.raser, 1);
		else
			Log.e("AlienInvasion","Not Found Sound ID = " + sID);
		return nID;	
	}
	
	static public int CloseSystemSoundID()
	{
		
		if(gSoundPool != null)
		{
			gSoundPool.release();
			gSoundPool = null;
		}
		return 0;
	}
	
	static public int PlaySoundByID(int nID)
	{
		//직접 사운드 플래이 하면 간혹 랙이 발생하여 오퍼레이션을 할수가 없는 경우가 발생한다.
		if(gSoundPool != null)
		{
			PlaySoundRunable r = new PlaySoundRunable(nID);	
			if(AlienInvasion.gMainActivity.mSoundPoolHandler == null)
				AlienInvasion.gMainActivity.mSoundPoolHandler = new Handler();
			AlienInvasion.gMainActivity.mSoundPoolHandler.postDelayed(r, 0);
			return nID;
		} 
		return 0;
	}
	
	
	private static MediaPlayer.OnPreparedListener mediaPrepared = new MediaPlayer.OnPreparedListener() {

		public void onPrepared(MediaPlayer arg0) {
		} 
	};
	private static MediaPlayer.OnVideoSizeChangedListener mediaSizeChanged = new MediaPlayer.OnVideoSizeChangedListener() {
		
		public void onVideoSizeChanged(MediaPlayer mp, int width, int height) {
			
		}
	};
	
	/**
	 * 배경음악 플래이.
	 * @param sName
	 */
	static public int PlayBgSound(String sName)
	{
		
		try
		{
			int nID = 0;
			StopBgSound();
			if(sName.equals("MenuBak"))
			{
				nID = R.raw.menubak;

			}
			else //Super_Team
			{
				nID = R.raw.super_team;
			}

			gpPlayer = MediaPlayer.create(AlienInvasion.gMainActivity, nID);
			gpPlayer.setOnPreparedListener(mediaPrepared);
			gpPlayer.setOnVideoSizeChangedListener(mediaSizeChanged);
			gpPlayer.setLooping(true);
			gpPlayer.setVolume(1, 1);
			gpPlayer.start();
		}
		catch(Exception e)
		{
			Log.i("JAVA","PlayBgSound Error " + e.getLocalizedMessage());
		}
		return 0;
	}

	
	/**
	 * 배경음악 종료.
	 */
	static public int StopBgSound()
	{
	    if(gpPlayer != null)
	    {
	    	gpPlayer.stop();
	    	gpPlayer.release();
	        gpPlayer = null;
	    }
	    return 0;
	}
}
