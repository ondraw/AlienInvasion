package kr.co.songs.android.AlienInvasion.com;

public class CMinkLogMan {
	
	private static AMinkLog m_log = null;
	
	public static int getLogLevel()
	{
		if(m_log != null)
			return m_log.getLogLevel();
		return 0;
	}
	/**
	 * 
	 * @param log : 
	 */
	public static void InitLog(AMinkLog log)
	{
		m_log = log;
	}
	
	public static void WriteE(int nCode,String sMessage) {
		if(m_log != null) m_log.WriteE(nCode, sMessage);		
	}
	
	public static void WriteI(String sMessage) 
	{
		if(m_log != null) m_log.WriteI(sMessage);
	}	
	
	public static void WriteT(String sMessage)
	{
		if(m_log != null) m_log.WriteT(sMessage);
	}
	
	public static void DeleteLogFile(String sFileName)
	{
		if(m_log != null) m_log.DeleteLogFile(sFileName);
	}
	
//	///////////////////////////////////////////////////////////////////////////////
//	
//    static String sSwid="";
//    static String sVersionid = "";
//    static String sLastversionid ="";
//    static String sIndex ="";	
//    
//	public  static void setSwid(String s){
//		sSwid = s ;
//	}
//	public static String getSwid(){
//		return sSwid;
//	}
//	public  static void setVersionid(String s){
//		sVersionid = s ;
//	}
//	public static String getVersionid(){
//		return sVersionid;
//	}
//	public  static void setLastversionid (String s){
//		sLastversionid = s ;
//	}
//	public static String getLastversionid (){
//		return sLastversionid;
//	}
//	public  static void setIndex(String s){
//		sIndex = s ;
//	}
//	public static String getIndex()
//	{
//		return sIndex;
//	}
	
}








