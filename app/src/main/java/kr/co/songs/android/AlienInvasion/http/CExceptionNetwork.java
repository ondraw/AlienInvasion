package kr.co.songs.android.AlienInvasion.http;




public class CExceptionNetwork extends Exception {	
	/**
	 * 
	 */
	protected static final long serialVersionUID = 361168513641523207L;
	protected long m_lCode;
	
	public long getCode() { return m_lCode;}
	

	public CExceptionNetwork(long lCode,String strError)
	{		
		super(strError);
		m_lCode = lCode;
	}

}