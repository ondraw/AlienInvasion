package kr.co.songs.android.AlienInvasion.http;

import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;

import kr.co.songs.android.AlienInvasion.minkml.CMinkException;

public abstract class ASendDataToServer 
{		
	protected IProgress m_syncProgress;
	protected String m_sURL; 
	protected int m_nRetry;
	protected int m_nConnectionTimeOut;
	protected int m_nRequestTimeOut;
	protected int m_nEncoding;
	protected int m_nResponseContentLength;
	protected boolean m_bDesZip;	
	protected HttpURLConnection m_Conn;
	protected String m_nFilePath;
	
	public void setURL(String sURL) { m_sURL = sURL;}
	
	// JCW_U 2010.12.30 START
//	public static int DEFAULT_TIMEOUT = 40000;
	public static int DEFAULT_TIMEOUT = 120000;
	// JCW_U 2010.12.30 END
	
	public ASendDataToServer(String sURL,String sFilePath,boolean bDesZip,IProgress proc,int nRequestTimeOut) 
	{ 
		m_syncProgress = proc;
		m_sURL = sURL;
		
		m_nConnectionTimeOut = 15000;
		m_nRequestTimeOut =  nRequestTimeOut;
		m_nRetry = 3;
		m_nEncoding = 0;
		m_nResponseContentLength = 0;	
		m_Conn = null;
		m_bDesZip = bDesZip;
		m_nFilePath = sFilePath;
	}
	
	public void Settings(int iRetry,String sFilePath,int nConnectionTimeOut,int nRequestTimeOut,int nEncoding)
	{
		m_nRetry = iRetry;
		m_nConnectionTimeOut = nConnectionTimeOut;
		m_nRequestTimeOut = nRequestTimeOut;
		m_nEncoding = nEncoding;
		m_nFilePath = sFilePath;
	}
	
	public int GetResponseContentsLength()
	{ 
		return m_nResponseContentLength;
	}
	
	/**
	 * 인터넷에 접속한다. CDMA,GPRS,와이브로,무선랜
	 */
	protected abstract void ConnectNet();
	
	protected abstract void ConnectHttp() throws IOException;
	
	protected abstract void DisconnectHttp();
	
	/**
	 * 인터넷에 연결이 안되어 있으면 인터넷에 연결을 하여 데이터를 보내고 
	 * 
	 * @param sndSyncML
	 * @return 받은 SyncML 객체
	 */
	public abstract InputStream SendData(byte btXmlSend[]) throws CExceptionNetwork;
	public abstract Object SendData(Object objXmlSend) throws CExceptionNetwork, CMinkException;
	
	
		
	
	/**
	 * 연결된것을 구지 끊지는 말자
	 *deprecated
	 */
	public abstract void DisconnectNet();
}
