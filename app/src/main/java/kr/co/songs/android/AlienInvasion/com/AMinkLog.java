package kr.co.songs.android.AlienInvasion.com;

public abstract class AMinkLog {
	
	protected int m_nType = 0;
		
	public AMinkLog(int nType)
	{
		m_nType = nType;
	}
	/**
	 * 
	 * @param nType : 0: only Error 1:Info 2:Trace
	 * 
	 */
	public void LogLevel(int nType) 
	{
		m_nType = nType;
	}
	
	public int getLogLevel() { return m_nType;}
	
	public abstract void WriteE(int nCode,String sMessage);	
	public abstract void WriteI(String sMessage);	
	public abstract void WriteT(String sMessage);
	public abstract void DeleteLogFile(String sFileName);
}
