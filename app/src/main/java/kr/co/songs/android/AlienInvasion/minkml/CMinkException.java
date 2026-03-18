package kr.co.songs.android.AlienInvasion.minkml;

public class CMinkException extends Exception {	
	
	/**
	 * 
	 */
	private static final long serialVersionUID = -8255169960564121679L;
	protected long mlCode;
	public long getCode() { return mlCode;}
	public CMinkException(long lCode,String strError)
	{		
		super(strError);
		mlCode= lCode;
	}
}
