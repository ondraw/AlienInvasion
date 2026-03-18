package kr.co.songs.android.AlienInvasion;

public class PlaySoundRunable implements Runnable {
    private int s_id; 
    public PlaySoundRunable(int nID) {
    	s_id = nID;
	}
	public void run() {
    	if(SoundMan.gSoundPool != null)
		{
    		SoundMan.gSoundPool.play(s_id, 1, 1, 0, 0, 1);
			return;
		} 
    }
}