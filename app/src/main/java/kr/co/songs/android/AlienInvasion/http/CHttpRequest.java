package kr.co.songs.android.AlienInvasion.http;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.UnsupportedEncodingException;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.security.KeyManagementException;
import java.security.NoSuchAlgorithmException;
import java.security.SecureRandom;
import java.security.cert.CertificateException;
import java.security.cert.X509Certificate;
import java.util.Date;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.zip.Deflater;
import java.util.zip.GZIPInputStream;
import java.util.zip.InflaterInputStream;

import javax.net.ssl.HostnameVerifier;
import javax.net.ssl.HttpsURLConnection;
import javax.net.ssl.SSLContext;
import javax.net.ssl.SSLSession;
import javax.net.ssl.TrustManager;
import javax.net.ssl.X509TrustManager;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;

import org.w3c.dom.Document;

import kr.co.songs.android.AlienInvasion.minkml.CMinkException;
import kr.co.songs.android.AlienInvasion.minkml.CMinkML;
import kr.co.songs.android.AlienInvasion.http.CThreadEvent;
import kr.co.songs.android.AlienInvasion.com.CMinkLogMan;
import kr.co.songs.android.AlienInvasion.com.IMinkCoreDefine;
import kr.co.songs.android.AlienInvasion.enc.Encryption3Des;

public class CHttpRequest extends ASendDataToServer 
{
	
	//Added By Song.... SSL통
	static class FakeX509TrustManager implements X509TrustManager {
	    private static TrustManager[] trustManagers;
	    private static final X509Certificate[] _AcceptedIssuers = new X509Certificate[] {};

	    public void checkClientTrusted(X509Certificate[] chain, String authType)
	            throws CertificateException {
	    }

	    public void checkServerTrusted(X509Certificate[] chain, String authType)
	            throws CertificateException {
	    }

	    public boolean isClientTrusted(X509Certificate[] chain) {
	        return true;
	    }

	    public boolean isServerTrusted(X509Certificate[] chain) {
	        return true;
	    }

	    public X509Certificate[] getAcceptedIssuers() {
	        return _AcceptedIssuers;
	    }

	    public static void allowAllSSL() {
	    	
	    	
	        HttpsURLConnection.setDefaultHostnameVerifier(new HostnameVerifier() {

	                public boolean verify(String hostname, SSLSession session) {
	                    return true;
	                }
	            });
	        SSLContext context = null;
	        if (trustManagers == null) {
	            trustManagers = new TrustManager[] { new FakeX509TrustManager() };
	        }
	        try {
	            context = SSLContext.getInstance("TLS");
	            context.init(null, trustManagers, new SecureRandom());
	        } catch (NoSuchAlgorithmException e) {
	            e.printStackTrace();
	        } catch (KeyManagementException e) {
	            e.printStackTrace();
	        }
	        
	        
	        HttpsURLConnection.setDefaultSSLSocketFactory(context
	                                                      .getSocketFactory());
	    }
	}
	
	
	protected String msBlobFilePath;
	public String getBlobFilePath() { return msBlobFilePath;}
	
	/**
	 * Blob적용전 http 요청 생성
	 * @param surl
	 * @param bDesZip
	 * @param proc
	 * @param nRequestTimeOut
	 */
	public CHttpRequest(String surl,boolean bDesZip , IProgress proc, int nRequestTimeOut) {
		super(surl, null,bDesZip, proc,nRequestTimeOut);
		msBlobFilePath = null;
	}
	
	/**
	 * Blob적용전 http 요청 생성
	 * @param surl
	 * @param bDesZip
	 * @param proc
	 */
	public CHttpRequest(String surl,boolean bDesZip , IProgress proc) {
		super(surl, null,bDesZip, proc,DEFAULT_TIMEOUT);
		msBlobFilePath = null;
	}
	
	/**
	 * Blob적용후 http 요청 생성
	 * @param surl
	 * @param sDir : 블럽 데이터가 저장될 경로
	 * @param bDesZip
	 * @param proc
	 * @param nRequestTimeOut
	 */
	public CHttpRequest(String surl,String sDir,boolean bDesZip , IProgress proc, int nRequestTimeOut) {
		super(surl, sDir,bDesZip, proc,nRequestTimeOut);		
		
		msBlobFilePath = null;
	}
	
	/**
	 * Blob적용후 http 요청 생성
	 * @param surl
	 * @param sDir : 블럽 데이터가 저장될 경로
	 * @param bDesZip
	 * @param proc
	 */
	public CHttpRequest(String surl,String sDir,boolean bDesZip , IProgress proc) {
		super(surl, sDir,bDesZip, proc,DEFAULT_TIMEOUT);
		
		msBlobFilePath = null;
		
	}
	
	/**
	 * 인터넷에 접속한다. CDMA,GPRS,와이브로,무선랜
	 */
	@Override
	protected void ConnectNet()
	{	
		if( m_syncProgress != null) 
			m_syncProgress.ConnectNet();		
	}	
	
	@Override
	protected void ConnectHttp() throws IOException
	{
		URL url = null;
		
		DisconnectHttp();
		
		if( m_syncProgress != null) 
			m_syncProgress.ConnectHttp();
		
		//1> 연결
		url = new URL(m_sURL);
		
		if(CMinkLogMan.getLogLevel() > 0)
			CMinkLogMan.WriteI("ConnectHttp URL" + m_sURL);
		
		if (url.getProtocol().toLowerCase().equals("https")) 
		{
			System.setProperty("http.keepAlive", "false");
			FakeX509TrustManager.allowAllSSL();
			m_Conn = (HttpsURLConnection) url.openConnection(); 
		}
		else
		{		
			m_Conn = (HttpURLConnection)url.openConnection(); 		
		}
		
		if(m_Conn == null)
		{
			CMinkLogMan.WriteE(IMinkCoreDefine.E_HTTP_Connection, "url.openConnection" + m_sURL); 
		}
	}
	
	@Override
	protected void DisconnectHttp()
	{
		if(CMinkLogMan.getLogLevel() > 1)
			CMinkLogMan.WriteI("DisconnectHttp");
		
		if( m_hThreaWaitEvent != null)
		{
			m_hThreaWaitEvent.signalAll();
			m_hThreaWaitEvent = null;
		}
		
		if( m_syncProgress != null) 
			m_syncProgress.DisconnectHttp();
		
		if(m_Conn != null)
		{
			m_Conn.disconnect();
			m_Conn = null;
		}		
	}
	
	
	public static byte[] readContent(final InputStream in, int bufferSize)  throws IOException 
	{
        ByteArrayOutputStream bout = new ByteArrayOutputStream();
        byte[] buf = new byte[bufferSize];

        int c = 0;
        int b = 0;
        while ((c < buf.length) && (b = in.read(buf, c, buf.length-c)) >= 0) {
            c+=b;
            if (c == bufferSize) {
                bout.write(buf);
                buf = new byte[bufferSize];
                c = 0;
            }
        }
        if (c != 0) {
            bout.write(buf, 0, c);
        }
        return bout.toByteArray();
    }
	
	/**
	 * Added By Song Blob 파일을 out에 저장한다.
	 * @param in
	 * @param out
	 * @param nSize
	 * @throws IOException
	 */
	public void readContent(final InputStream in, OutputStream out,int nSize) throws IOException 
	{
		int nBufferSize = 1024;
		int nReaded = 0;
		int nReadLen = 0;
		
		byte [] btBody = new byte[1024];
		while(true)
		{
			if(nReaded + 1024 > nSize)
			{
				nBufferSize = nSize - nReaded;
				btBody = new byte[nBufferSize];
			}
			
			nReadLen = in.read(btBody);
			
			//Added By Song 2011.06.03 이부분을 빼먹으면 미세하게 데이터가 틀어진다.
			if(nReadLen == 0) continue;
			
			nReaded += nReadLen;
			
			if(m_syncProgress != null)	
				m_syncProgress.RcvData((int)nReadLen);
			
			out.write(btBody);
			if(nReaded >= nSize) 
				break;
		}
	}
	
	private byte[] toEncoding(byte[] inData) throws Exception
	{
		byte[] btResult = null;
		m_Conn.setRequestProperty( "Uncompressed-Content-Length", Integer.toString(inData.length));				
		m_Conn.setRequestProperty("Accept-Encoding",  "gzip,deflate,defdes");
		m_Conn.setRequestProperty("Content-Encoding", "defdes");				
		
		Deflater compressor = new Deflater();
		compressor.setLevel(9); //0~9
		compressor.setInput(inData);
		compressor.finish();	
		
		ByteArrayOutputStream bos =  new ByteArrayOutputStream(inData.length);	            
        byte[] buf = new byte[2048];
        while (!compressor.finished()) 
        {
            int count = compressor.deflate(buf);
            bos.write(buf, 0, count);
        }   
        
        btResult = Encryption3Des.encrypt(bos.toByteArray());
        m_Conn.setRequestProperty("Content-Length", Integer.toString(btResult.length));
        inData = null;
        return btResult;
	}
	
	private InputStream toDecoding(InputStream inStream) throws Exception
	{
		InputStream inputResult = null;
		//defdes
		byte[] btDes3  = readContent(inStream, m_nResponseContentLength);					
		byte[] btNomal = Encryption3Des.decrypt(btDes3);
		ByteArrayInputStream btDes3Obj = new ByteArrayInputStream(btNomal);		
		inputResult = new InflaterInputStream(btDes3Obj);		
		
		if(inStream != null) 
		{
			inStream.close();
			inStream = null;
		}
		return inputResult;
		
	}
	
	/**
	 * Bolob 의 파일이 같이 날라올때 사용하는 함수이다. 위의 함수와의 차이점은 읽을 컨텐츠의 길이를 외부에서 보내주고,
	 * inStream을 Close하지 않음.
	 * @param inStream
	 * @param nContentLength
	 * @return
	 * @throws Exception
	 */
	private InputStream toDecodingEx(InputStream inStream,int nContentLength) throws Exception
	{
		InputStream inputResult = null;
		//defdes
		byte[] btDes3  = readContent(inStream, nContentLength);					
		byte[] btNomal = Encryption3Des.decrypt(btDes3);
		ByteArrayInputStream btDes3Obj = new ByteArrayInputStream(btNomal);		
		inputResult = new InflaterInputStream(btDes3Obj);		
		return inputResult;
		
	}
	
	/**
	 * Bolob 의 파일이 같이 날라올때 사용하는 함수이다. 위의 함수와의 차이점은 읽을 컨텐츠의 길이를 외부에서 보내주고,
	 * inStream을 Close하지 않음.
	 * @param inStream
	 * @param nUnCompressLength
	 * @return
	 * @throws Exception
	 */
	
	private InputStream toDecodingZipEx(InputStream inStream,int nUnCompressLength) throws Exception
	{
		
		InputStream inputResult = null;
		
		inputResult = new GZIPInputStream(inStream);
		byte[] btDes3  = readContent(inputResult, nUnCompressLength);	
		inputResult.close();
		
		inputResult = new ByteArrayInputStream(btDes3);		
		return inputResult;
		
	}
	
	private InputStream toDecodingZip(InputStream inStream,int nUnCompressLength) throws Exception
	{
		
		InputStream inputResult = null;
		inputResult = new GZIPInputStream(inStream);
		byte[] btDes3  = readContent(inputResult, nUnCompressLength);	
		inputResult.close();
		
		inputResult = new ByteArrayInputStream(btDes3);		
		
		
		if(inStream != null) 
		{
			inStream.close();
			inStream = null;
		}
		return inputResult;
		
	}
	
	
	int  m_nThreadError = 0;
	String m_sThreadError = "";
	InputStream  m_outThreadStream = null;
	CThreadEvent m_hThreaWaitEvent = null;
	Thread		m_hThreaWaitThread = null;
	byte[]      m_btInXmlSend = null;
	
	
	/**
	 * 타임 아웃이 제대로 먹지 않을 경우 쓰레드를 사용하여 정리한다.
	 */
	public void SendDataThread(byte btXmlSend[])
	{
		int nResponse;		
		int nBlobFileSize = 0;
		OutputStream outStream = null;
		//InputStream  inStream = null;		
		try
		{	
			
			
			if(CMinkLogMan.getLogLevel() > 0)
				CMinkLogMan.WriteI("Before Connect : " + (new Date()).toString());
			
			m_Conn.connect();			
			
			if(CMinkLogMan.getLogLevel() > 0)
				CMinkLogMan.WriteI("After Connect and Before Send: " + (new Date()).toString());
			
			m_nResponseContentLength = 0;
			
			//네트워크에 접속이 안되어 있으면 접속한다.			
			if( m_syncProgress != null) 
				m_syncProgress.SendTotalSize(btXmlSend.length);
			
			//2> 포스트 데이터
			outStream = m_Conn.getOutputStream();
			if(outStream == null)
			{
				m_nThreadError = IMinkCoreDefine.E_HTTP_SendData;
				m_sThreadError = "outStream 보낼수가 없습니다"; 
				return;
			}
			outStream.write(btXmlSend);
			outStream.flush();
			outStream.close();
			outStream = null;
			
			if(CMinkLogMan.getLogLevel() > 0)
				CMinkLogMan.WriteI("After Send and Before get response: " + (new Date()).toString());
			
			//3>응답
			nResponse = m_Conn.getResponseCode();	
			
			if(CMinkLogMan.getLogLevel() > 0)
				CMinkLogMan.WriteI("after get response: " + (new Date()).toString());
			
			switch(nResponse)
			{
			case HttpURLConnection.HTTP_OK:
				break;
			default:
				throw new CExceptionNetwork(-1,m_Conn.getResponseMessage());
			}			
			
			//-----------------------------------------------------------------
			//4>해더 -- Debug
			if(CMinkLogMan.getLogLevel() >= 1)
			{
				Map<String,List<String>> Headers = m_Conn.getHeaderFields();
				//int nHeaderSize = Headers.size();
				Iterator<String> it = Headers.keySet().iterator();
				
				while(it.hasNext())
				{
					String sHeaderKey = it.next();
					List<String> lstHeaderData = Headers.get(sHeaderKey);
					for(String s : lstHeaderData)
					{					
						CMinkLogMan.WriteI(sHeaderKey + " : " + s);
					}
					
				}
			}
			
		
			//-----------------------------------------------------------------			
			//5>응답
			m_nResponseContentLength = m_Conn.getContentLength();
			
			
			
			if(CMinkLogMan.getLogLevel() > 0)
				CMinkLogMan.WriteI("Length = " + Integer.toString(m_nResponseContentLength));
			
			if( m_syncProgress != null) 
				m_syncProgress.RcvTotalSize(m_nResponseContentLength);	
			
			
			
			//-----------------------------------------------------------------
			//Added By Song BLOB형태의 파일 데이터 추가
			//-----------------------------------------------------------------
			nBlobFileSize = m_Conn.getHeaderFieldInt("FILE-SIZE", 0);
			//-----------------------------------------------------------------
			
			if(nBlobFileSize == 0)
			{
				m_outThreadStream = m_Conn.getInputStream();
				//인코딩 방식은 defdes만 지원한다.
				String sDefDes = m_Conn.getHeaderField("Content-Encoding");
				if(sDefDes != null && sDefDes.equals("defdes"))			
					m_outThreadStream = toDecoding(m_outThreadStream);
				
				else if(sDefDes != null && sDefDes.equals("gzip"))
				{
					String sLength = m_Conn.getHeaderField("Uncompressed-Content-Length");
					m_outThreadStream = toDecodingZip(m_outThreadStream,Integer.parseInt(sLength));
				}
			}
			else
			{
				InputStream  inTempThreadStream = null;
				
				try
				{
					inTempThreadStream = m_Conn.getInputStream();
				
					//인코딩 방식은 defdes만 지원한다.
					String sDefDes = m_Conn.getHeaderField("Content-Encoding");
					if(sDefDes != null && sDefDes.equals("defdes"))
					{
						m_outThreadStream = toDecodingEx(inTempThreadStream,m_nResponseContentLength - nBlobFileSize);
					}
					else if(sDefDes != null && sDefDes.equals("gzip"))
					{
						String sLength = m_Conn.getHeaderField("Uncompressed-Content-Length");
						m_outThreadStream = toDecodingZipEx(m_outThreadStream,Integer.parseInt(sLength));
					}
					
					if(m_syncProgress != null)	m_syncProgress.RcvData((int)(m_nResponseContentLength - nBlobFileSize));
					//------------------------------------------------------
					//$$FILE-SIZE를 파일에 저장을 한다.
					readBlobData(inTempThreadStream);
				}
				catch(Exception ex)
				{
					if(msBlobFilePath != null)
					{
						//$$에러가 났기 때문에 받은 파일을 전부 지워주어야 한다.
					}
					throw ex;
				}
				finally
				{
					if(inTempThreadStream != null)
					{
						inTempThreadStream.close();
						inTempThreadStream = null;
					}
				}
				
			}
			
		}
		catch (MalformedURLException e) 
		{
			m_nThreadError = IMinkCoreDefine.E_HTTP_SendData;
			m_sThreadError = e.getMessage(); 
		} 
		catch (IOException e) 
		{
			m_nThreadError = IMinkCoreDefine.E_HTTP_SendData1;
			m_sThreadError = e.getMessage(); 
		} 
		catch (Exception e)
		{
			m_nThreadError = IMinkCoreDefine.E_HTTP_SendData2;
			m_sThreadError = e.getMessage();  
		}
		finally
		{
			if(outStream != null)
			{
				try 
				{
					outStream.flush();
					outStream.close();
				} 
				catch (IOException e) 
				{
					m_nThreadError = IMinkCoreDefine.E_HTTP_SendData2;
					m_sThreadError = "outStream Close Error " + e.getMessage();  
				}				
				outStream = null;
			}		
			
			if(m_hThreaWaitEvent != null)
			{
				m_hThreaWaitEvent.signal();	
			}
		}		
	}

	/**
	 * Added By Song 2011.03.14 블럽데이터를 읽어 온다.
	 * @param inTempThreadStream
	 * @throws IOException
	 * @throws FileNotFoundException
	 */
	private void readBlobData(InputStream inTempThreadStream)
			throws IOException, FileNotFoundException {
		Date d = new Date();
		long lNow = d.getTime();
		msBlobFilePath = String.format("%s/%ld", this.m_nFilePath,lNow);
		
		byte[] btTotalHeader = new byte[20];
		byte[] btxTemp = new byte[20];
		btTotalHeader  = readContent(inTempThreadStream, 20);
		//totalsize=1290129 이런형태로 데이터가 드러있다.
		System.arraycopy(btTotalHeader, 11, btxTemp, 0, 9);
		String sTotalHeader = new String(btxTemp);
		long   lTotalSize = Long.parseLong(sTotalHeader);
		
		long lReadTotalSize = 0;
		do
		{
			//minkname=%d_%d,minksize=%d
			byte []btSubHeader = new byte[30];
			btSubHeader  = readContent(inTempThreadStream, 30);
			lReadTotalSize += btSubHeader.length;
			
			if(m_syncProgress != null)	m_syncProgress.RcvData((int)btSubHeader.length);
			
			String sSubheader = new String(btSubHeader);
			String []arrSubHeader = sSubheader.split(",");
			
			//--------------------------------
			//arrSubHeader[0] = %d_%d
			//arrSubHeader[1] = Size
			String FileNameEx = arrSubHeader[0].split("=")[1];
			String sFileSize = arrSubHeader[1].split("=")[1];
			long   lFileSize = Long.parseLong(sFileSize);
			String sFilePath = msBlobFilePath + "/" + FileNameEx;
			OutputStream f = null;
			try
			{
				f = new FileOutputStream(sFilePath);
				readContent(inTempThreadStream,f,(int)lFileSize);
				lReadTotalSize += lFileSize;
				if(lTotalSize >= lReadTotalSize) break;
			}
			finally
			{
				if(f != null)
				{
					f.close();
					f = null;
				}
			}
		}
		while(true);
	}
	
	/**
	 * 인터넷에 연결이 안되어 있으면 인터넷에 연결을 하여 데이터를 보내고 
	 * 
	 * @param sndSyncML
	 * @return 받은 SyncML 객체
	 */
	@Override
	public InputStream SendData(byte btXmlSend[]) throws CExceptionNetwork
	{		
		//int nResponse;		
		//OutputStream outStream = null;
		InputStream  inStream = null;		
		try
		{	
			
			m_nThreadError = 0;
			m_sThreadError = "";
			m_outThreadStream = null;
			m_hThreaWaitEvent = null;
			m_hThreaWaitThread = null;
			m_btInXmlSend = null;
			msBlobFilePath = null;
			
			ConnectNet();	
			
			ConnectHttp();
			
			m_btInXmlSend = btXmlSend;
			
			m_Conn.setRequestMethod("POST");
			m_Conn.setDoOutput(true);
			m_Conn.setDoInput(true);
			m_Conn.setUseCaches(false);
			m_Conn.setDefaultUseCaches(false);
			
			m_Conn.setRequestProperty("User-Agent", "Mozilla/4.0");
			m_Conn.setRequestProperty("Content-Type", "application/vnd.syncml+xml");
			
			if(m_bDesZip == false)
				m_Conn.setRequestProperty("Content-Length", Integer.toString(m_btInXmlSend.length));
			else			
				m_btInXmlSend = toEncoding(m_btInXmlSend);	
			
			m_Conn.setConnectTimeout(m_nConnectionTimeOut);
			m_Conn.setReadTimeout(m_nRequestTimeOut);				
			
			m_hThreaWaitEvent = new CThreadEvent();
			m_hThreaWaitThread = new Thread()
			{
				public void run() 
				{
					SendDataThread(m_btInXmlSend);
				};
			};
			m_hThreaWaitThread.start(); 
			
			try 
			{
				//m_hThreaWaitEvent.await(60000); //1분을 기다린다.
				m_hThreaWaitEvent.await(m_nRequestTimeOut + 30000); //5분을 기다린다.
				
			} 
			catch (Exception ex) 
			{			
				CMinkLogMan.WriteE(-1, ex.getMessage());
			}
			finally
			{
				m_btInXmlSend = null;
			}
		}		
		catch (Exception e)
		{
			throw( new CExceptionNetwork(IMinkCoreDefine.E_HTTP_SendData2,e.getMessage())); 
		}
		finally
		{
			inStream = m_outThreadStream;
			m_hThreaWaitEvent = null;
			
			if(CMinkLogMan.getLogLevel() > 0)
				CMinkLogMan.WriteI("SendData");
			
			/**
			 * 에러이면 에러를 발생 한다.
			 */
			if(m_nThreadError < 0)
			{
				throw( new CExceptionNetwork(m_nThreadError,m_sThreadError));
			}
			
		}		
		return inStream;
	}
		
	
	/**
	 * 연결된것을 구지 끊지는 말자
	 * @deprecated
	 */
	@Override
	public void DisconnectNet() 
	{
		if(m_syncProgress != null) 
			m_syncProgress.DisconnectNet();
	}
	
	@Override
	public Object SendData(Object objXmlSend) throws CExceptionNetwork, CMinkException 
	{
		if(objXmlSend instanceof CMinkML)
			return (Object)SendDataCMinkML((CMinkML)objXmlSend);			
		return null;
	}
	
	/**
	 * SyncML을 보낸다.
	 * 
	 * @param sndSyncML
	 * @return 받은 SyncML 객체
	 * @throws CMinkException 
	 * @throws  
	 */
	protected Document SendDataCMinkML(CMinkML sndMinkML) throws CExceptionNetwork, CMinkException
	{
		
		Document Doc = null;
		byte btXmlSend[] = null;		
		InputStream inStream = null;
		DocumentBuilderFactory dbf = null;		
		DocumentBuilder db = null;		
		
		try
		{			
			String sSndData = sndMinkML.toStringBuffer().toString();
			
			if(CMinkLogMan.getLogLevel() > 1)
			{
				CMinkLogMan.WriteI("--- Send Data --\r\n");
				CMinkLogMan.WriteI(sSndData);
				CMinkLogMan.WriteI("\r\n");
			}
			
			btXmlSend = sSndData.getBytes("UTF-8");
			
			
				
			inStream = SendData(btXmlSend);			
			dbf = DocumentBuilderFactory.newInstance();
			db = dbf.newDocumentBuilder();
			Doc = db.parse(inStream);		
			
			if(CMinkLogMan.getLogLevel() > 1)
			{
				CMinkLogMan.WriteI("--- Rcv Data --\r\n");
				CMinkML mLog = new CMinkML(Doc);
				CMinkLogMan.WriteI(mLog.toStringBuffer().toString());
				CMinkLogMan.WriteI("\r\n");
			}
			
		}		
		catch (UnsupportedEncodingException e)
		{
			throw( new CExceptionNetwork(IMinkCoreDefine.E_HTTP_SendDataCMinkML,e.getMessage())); 
		}
		catch (CExceptionNetwork e) {
			throw( e);
		}
		catch (Exception e)
		{
			throw( new CExceptionNetwork(IMinkCoreDefine.E_HTTP_SendDataCMinkML1,e.getMessage())); 
		}
		finally
		{		
			if(inStream != null)
			{
				try 
				{					
					
					inStream.close();
				} 
				catch (IOException e) 
				{
					throw( new CExceptionNetwork(-1,e.getLocalizedMessage()));
				}				
				inStream = null;
			}			
			DisconnectHttp();
		}
		
		return Doc;
	}

	
}
