package kr.co.songs.android.AlienInvasion.minkml;

import java.io.ByteArrayInputStream;
import java.io.InputStream;
import java.util.Calendar;
import java.util.Date;
import java.util.Locale;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;

import org.w3c.dom.Attr;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

import kr.co.songs.android.AlienInvasion.com.ASyncMLObj;
import kr.co.songs.android.AlienInvasion.com.CDateUtils;
import kr.co.songs.android.AlienInvasion.com.CStringMan;
import kr.co.songs.android.AlienInvasion.com.IMinkCoreDefine;
import kr.co.songs.android.AlienInvasion.com.CDateUtils.DateToStringFormat;

public class CMinkML extends ASyncMLObj
{	
	protected String m_sSession;
	protected String m_sVersion;	
	protected String m_sTime;
	protected String m_sDeviceID;
	protected String m_sUser;
	protected String m_sPassword;	
	
	protected String m_sInstallName;
	
	protected final String MINKPROTOCALVERSION = "1.1"; 
	protected Document m_Doc = null;
	
	public void setUserID(String v) { m_sUser = v;}
	public void setDeviceID(String v) { m_sDeviceID = v;}
	public void setPassword(String v) { m_sPassword = v;}
	public void setInstallName(String v) { m_sInstallName = v;}
	
		
	public CMinkML(Document Doc)
	{
		m_sVersion = MINKPROTOCALVERSION;
		m_sUser = "";
		m_sPassword = "";
		m_sDeviceID = "";		
		Clear();
		
		m_Doc = Doc;
	}
	
	
	public CMinkML(String sUserId, String sPassword,String sDeviceID,String sInstallName)
	{
		m_sVersion = MINKPROTOCALVERSION;
		m_sUser = sUserId;
		m_sPassword = sPassword;
		m_sDeviceID = sDeviceID;
		m_sInstallName = sInstallName;	
		
		Clear();	
	}
	
	
	
	public void LoadXml(InputStream in) throws CMinkMLException
	{
		DocumentBuilderFactory dbf = null;		
		DocumentBuilder db = null;		
		Node	xnNode,xnRoot;
		
		try
		{
			dbf = DocumentBuilderFactory.newInstance();
			db = dbf.newDocumentBuilder();
			m_Doc = db.parse(in);	
			
					
			//lstChild = m_Doc.getElementsByTagName("mink");
			//m_xnMink = lstChild.item(0);
			xnRoot = m_Doc.getDocumentElement();
			NamedNodeMap mapAtt = xnRoot.getAttributes();
			
			xnNode = mapAtt.getNamedItem("session");
			if(xnNode != null)
				m_sSession = CStringMan.getString(xnNode);
					
			xnNode = mapAtt.getNamedItem("version");
			if(xnNode != null)
				m_sVersion = CStringMan.getString(xnNode);
			
			xnNode = mapAtt.getNamedItem("time");
			if(xnNode != null)
				m_sTime = CStringMan.getString(xnNode);
			
			xnNode = mapAtt.getNamedItem("deviceid");
			if(xnNode != null)
				m_sDeviceID = CStringMan.getString(xnNode);
			
			xnNode = mapAtt.getNamedItem("userid");
			if(xnNode != null)
				m_sUser = CStringMan.getString(xnNode);
							
			xnNode = mapAtt.getNamedItem("password");
			if(xnNode != null)
				m_sPassword = CStringMan.getString(xnNode);					
		}
//		catch(CExceptionMinkml ex)
//		{
//			throw(ex);
//		}
		catch(Exception ex)
		{	
			throw(new CMinkMLException(IMinkCoreDefine.E_ML_Load,ex.getMessage()));
		}
		finally
		{			
		}
	}
	
	
	
	
	
	protected void Clear()
	{
		m_sSession = "";		
		m_sTime = "";			
		m_Doc = null;		
	}
	
	//현재 시간을 받아 온다(applist 에서 현재 시간 필요) by seo4872
	public String getCurrentTime(){
		Locale loc = Locale.getDefault();
		Calendar cal = Calendar.getInstance(loc);
		Date toDay = cal.getTime();
		String sTimeSet = CDateUtils.toDateString(toDay, DateToStringFormat.yyyyMMddHHmmss);
		
		return sTimeSet.substring(0, 12);
	}
	
	protected Node MakeMink(String sVersion, String sUserId, String sPassword,String sDeviceID)
	{
		String strXML;
		m_sSession = "session";
		//userid , password  trim() 추가     2010/09/15   by seo4872
		//userid . password 에 xml 특수 문자가 있을시 치환 한다
	 
		strXML = String.format("<?xml version=\"1.0\" " +
				"encoding=\"utf-8\" ?>\r\n" +
				"<mink session='%s' version='%s' deviceid='%s' time='%s' userid='%s' password='%s'></mink>",
				m_sSession,
				sVersion,
				sDeviceID,
				this.getCurrentTime(), 
				sUserId,
				sPassword);
		
		m_sUser = sUserId;
		m_sVersion = sVersion;
		m_sPassword = sPassword;
		m_sDeviceID = sDeviceID;	
		InputStream in = new  ByteArrayInputStream(strXML.getBytes());		
		
		try 
		{			
			LoadXml(in);
		} 
		catch (CMinkMLException e) 
		{
			return null;
		}
		return m_Doc.getDocumentElement();
	}
	

	protected void setNodeText(Node xnNode,String strText)
	{			
		xnNode.appendChild(m_Doc.createTextNode(strText));
	}
	
	
	protected Node AddCommand(Node xnMink,String strClassID,int refID,String strTarget,boolean autoCommit,String strActionID)
	{		
		Element xnCmd = null;	

		xnCmd = CreateElemnt("cmd","");
		if(xnCmd == null) return null;

		AddAttribute(xnCmd,"classid",strClassID);	
		AddAttribute(xnCmd,"refid",Integer.toString(refID));
		AddAttribute(xnCmd,"autocommit",Boolean.toString(autoCommit));
		if(strTarget != null && strTarget.length() != 0)
			AddAttribute(xnCmd,"target",strTarget);
		if(strActionID != null && strActionID.length() != 0)
			AddAttribute(xnCmd,"actionid",strActionID);	
		
		xnMink.appendChild(xnCmd);
		return xnCmd;
	}
	
	//버전 관리 
	protected Node AddCommandVerMan(Node xnMink)
	{		
		Element xnCmd = null;	
		Element xnCmd1 = null;
		Element xnCmd2 = null;
		Element xnCmd3 = null;

		xnCmd = CreateElemnt("verman","");
		if(xnCmd == null) return null;

		

		
		AddAttribute(xnCmd,"installname",m_sInstallName);	
		AddAttribute(xnCmd,"userid",m_sUser);
		AddAttribute(xnCmd,"deviceid",m_sDeviceID);
			
		
		xnMink.appendChild(xnCmd);
		return xnCmd;
	}
	
	
	protected Node AddCommand(Node xnMink,String strClassID,int refID,String strTarget,
			String company,String companyCode , String companyId)
	{		
		Element xnCmd = null;	

		xnCmd = CreateElemnt("cmd","");
		if(xnCmd == null) return null;

		AddAttribute(xnCmd,"classid",strClassID);	
		AddAttribute(xnCmd,"refid",Integer.toString(refID));	
		if(strTarget != null && strTarget.length() != 0)
			AddAttribute(xnCmd,"target",strTarget);
		AddAttribute(xnCmd,"company",(company));	
		AddAttribute(xnCmd,"companycode",(companyCode));	
		AddAttribute(xnCmd,"companyid",(companyId));	
		
		xnMink.appendChild(xnCmd);
		return xnCmd;
	}
	
	protected Node AddUnit(Node xnCommand,String strMode,int refID)
	{		
		Element xnUnit = null;	
		xnUnit = CreateElemnt("unit","");
		if(xnUnit == null) return null;
		AddAttribute(xnUnit,"mode",strMode);		
		AddAttribute(xnUnit,"refid",Integer.toString(refID));
		xnCommand.appendChild(xnUnit);
		return xnUnit;
	}
	
		
	protected Node AddParam(Node xnUnit,String strValue,String strMode,String strType,String strName,int nLength)
	{
		Element xnParam = null;	
		String sRepValue = "";
		sRepValue = (strValue);
		xnParam = CreateElemnt("param",  ( sRepValue ) );	// 엘리먼트 값에서 특수 문자 치환 by seo4872 2010/10/07	
		if(strName != null && strName.length() != 0)
			AddAttribute(xnParam,"name",strName);		
		AddAttribute(xnParam,"mode",strMode);
		AddAttribute(xnParam,"type",strType);		
		AddAttribute(xnParam,"length",Integer.toString(sRepValue.length()));
		xnUnit.appendChild(xnParam);	
		
		return xnParam;
	}


	protected Element CreateElemnt(String sTagName,String sText)
	{
		Element elm;
		elm = m_Doc.createElement(sTagName);
		elm.appendChild(m_Doc.createTextNode(sText));
		
		return elm;
	}
	
	protected Attr AddAttribute(Node xnNode,String sAttgName,String sText)
	{
		Attr att = m_Doc.createAttribute(sAttgName);
		//Not support on Android 
		//att.appendChild(m_Doc.createTextNode(sText));
		att.setNodeValue(sText);		
		xnNode.getAttributes().setNamedItem(att);	
		
		return att;
	}
		
	/**
	 * �덈뱶濡쒖씠�쒖뿉���⑥닔瑜�吏�썝�섏� �딄린 �뚮Ц��
	 * �닿� 吏곸젒 留뚮뱾�덈떎.
	 * @param buffXML
	 * @param xnNode
	 */
	protected void ToXmlString(StringBuffer buffXML,Node xnNode)
	{		
		String sTagName="";
		NamedNodeMap Map = xnNode.getAttributes();
		int nAttCnt = Map.getLength();
		buffXML.append("<");
		sTagName = xnNode.getNodeName();
		buffXML.append(sTagName);
		for(int i = 0; i < nAttCnt; i++)
		{
			Node xnAtt = Map.item(i);
			buffXML.append(" ");
			buffXML.append(xnAtt.getNodeName());
			buffXML.append("=\""); 		
			buffXML.append(  getReplaceXMLChar(  xnAtt.getNodeValue() )); //엘리 먼트 내의 속성값 치환 by seo4872 2010/10/07			
			buffXML.append("\"");				
		}		
		buffXML.append(">");
		
		NodeList chroot = xnNode.getChildNodes();
		int nChildCnt = chroot.getLength();
		for(int i = 0; i < nChildCnt; i++)
		{
			Node xnChild = chroot.item(i);
			if(xnChild.getNodeType() == Node.TEXT_NODE)
			{
				buffXML.append(  xnChild.getNodeValue()  );				
			}
			else if(xnChild.getNodeType() == Node.ELEMENT_NODE)		
			{
				ToXmlString(buffXML,xnChild);					
			}
		}		
		buffXML.append("</");
		buffXML.append(sTagName);
		buffXML.append(">");		
	}
	
	
	@Override
	protected StringBuffer ToXmlString() throws CMinkMLException {
		StringBuffer buffXML = new StringBuffer(1024);
		Node root = m_Doc.getDocumentElement();
		//Not support on Android 
		//buffXML.append("<?xml version=\""+m_Doc.getXmlVersion()+"\" encoding=\""+m_Doc.getXmlEncoding()+"\"?>\r\n"); 
		buffXML.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n");	 
		ToXmlString(buffXML,root);	
		return buffXML;
	}
	
	
	public StringBuffer toStringBuffer() throws CMinkMLException
	{
		return ToXmlString();
	}
	
	 /**
     * @description 입력 문자 값에 특수 문자가 있을경우 xml 치환
     * @param str 입력 문자
     * @return boolean
     */
	public String  getReplaceXMLChar(String shtml_char)
	{
		 String dhtml_char;
		 StringBuffer sb = new StringBuffer();
		 if(shtml_char != null && shtml_char != ""){
			for(int i=0; i < shtml_char.length(); i++) 
			{
			    char c = shtml_char.charAt(i);
			    switch (c) 
			   {
			      case '<' : 
			        sb.append("&lt;");
			        break;
			     case '>' : 
			        sb.append("&gt;");
			        break;
			     case '&' :
			        sb.append("&amp;");
			        break;
			     case '"' :
			        sb.append("&quot;");
			        break;
			     case '\'' :
			        sb.append("&apos;");
			        break;
			     default:
			        sb.append(c);
			     } 
			 }
			dhtml_char = sb.toString();
		 }else{
			 dhtml_char="";
		 }
	   return dhtml_char.trim();
	 }
}
