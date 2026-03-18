package kr.co.songs.android.AlienInvasion.minkml;

import java.io.ByteArrayInputStream;
import java.util.ArrayList;
import java.util.HashMap;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXParseException;

import kr.co.songs.android.AlienInvasion.com.CMinkLogMan;
import kr.co.songs.android.AlienInvasion.com.CStringMan;
import kr.co.songs.android.AlienInvasion.com.IMinkCoreDefine;

public class CRMRecordSet {
	
	protected Document 								m_Doc;
	protected NodeList								m_lstRecord;
	protected ArrayList<CColInfo> 					m_lstColInfo;
	protected HashMap<String,Integer>				m_hsColIndexMap;	
	protected int									m_nCursor;
	
	public CRMRecordSet() 
	{		
	}
	
	public CRMRecordSet(Document Doc)
	{
		m_Doc = Doc;
		m_lstRecord = null;
		m_lstColInfo = null;
		m_hsColIndexMap = null;
		m_nCursor = -1;
	}
	
	/**
	 * XML 데이터를 바로 CRMRecordSet으로 변경하려고 할때 사용한다.
	 * @param sXML
	 * @return
	 */
	static public CRMRecordSet BuildRMRecordSet(String sXML)
	{
		DocumentBuilderFactory dbf = null;		
		DocumentBuilder db = null;	
		CRMRecordSet rec = null;
		Document doc = null;
		try 
		{
			ByteArrayInputStream bi = new ByteArrayInputStream(sXML.getBytes());
			dbf = DocumentBuilderFactory.newInstance();
			db = dbf.newDocumentBuilder();
			doc = db.parse(bi);	
			rec = new CRMRecordSet(doc);
			rec.setTransaction();
		} 
		catch (SAXParseException e)
		{
			CMinkLogMan.WriteE(IMinkCoreDefine.E_RM_CRMRecordSet, e.getMessage()); 
		}
		catch (Exception e) 
		{
			CMinkLogMan.WriteE(IMinkCoreDefine.E_RM_CRMRecordSet1, e.getMessage()); 
		}
		return rec;
	}
	
	static public Document BuildDocument(String sXML)
	{
		DocumentBuilderFactory dbf = null;		
		DocumentBuilder db = null;	
		Document doc = null;
		try 
		{
			ByteArrayInputStream bi = new ByteArrayInputStream(sXML.getBytes());
			dbf = DocumentBuilderFactory.newInstance();
			db = dbf.newDocumentBuilder();
			doc = db.parse(bi);
		} 
		catch (SAXParseException e)
		{
			CMinkLogMan.WriteE(IMinkCoreDefine.E_RM_CRMRecordSet, e.getMessage()); 
		}
		catch (Exception e) 
		{
			CMinkLogMan.WriteE(IMinkCoreDefine.E_RM_CRMRecordSet1, e.getMessage()); 
		}
		return doc;
	}
	
	public int getColumnIndex(String columnName)
	{
		Integer nIndex = m_hsColIndexMap.get(columnName);
		if(nIndex != null) return nIndex;
		return -1;
	}
	
	public int getPosition() { return m_nCursor;}
	public boolean MovePosition(int nPos) 
	{ 
		if(nPos < 0) return false;
		else if( nPos >= getCount()) return false;
		m_nCursor = nPos;
		return true;
	}
	
	public int getCount()
	{
		if(m_lstRecord != null) 
			return m_lstRecord.getLength();
		return 0;
	}
	
	public void setTransaction() throws CMinkMLException
	{
		setTransaction(1,1);
	}
	
	public boolean isEOF()
	{
		//Added By Song 2010.09.14 데이터가 없을때 처리를 하지 않았음.
		if(m_lstRecord == null || m_lstRecord.getLength() == 0) return true;
		if(getPosition() >= m_lstRecord.getLength()) return true;
		return false;
	}
	
	public boolean isBOF()
	{
		//Added By Song 2010.09.14 데이터가 없을때 처리를 하지 않았음.
		if(m_lstRecord == null || m_lstRecord.getLength() == 0) return true;
		if(m_nCursor < 0) return true;
		return false;
	}
	
	public void setDoc(Document Doc) { m_Doc = Doc;}
	
	public void setTransaction(int nTransactionID,int nFunctionID) throws CMinkMLException
	{		
		int nRetCode = 0;
		boolean bFind = false;
		Element xnRoot = m_Doc.getDocumentElement();		
		NodeList lstChildRoot = xnRoot.getChildNodes();
		int nCnt = lstChildRoot.getLength();
		
		//-------------------------------------------
		//초기화를 한다.
		m_lstRecord = null;
		m_lstColInfo = new ArrayList<CColInfo>(5);
		m_hsColIndexMap = new HashMap<String,Integer>(5);
		m_nCursor = -1;
		//-------------------------------------------
		
		for(int i = 0; i < nCnt; i++ )
		{
			Node xnrcmd = lstChildRoot.item(i);
			if(xnrcmd.getNodeType() != Node.ELEMENT_NODE) continue;			
			NamedNodeMap attcmd = xnrcmd.getAttributes();			
			Node xnrefid = attcmd.getNamedItem("refid");
			if(xnrefid != null && Integer.parseInt(xnrefid.getNodeValue()) == nTransactionID)
			{				
				Node xnretcode = attcmd.getNamedItem("retcode");
				if(xnretcode != null)
				{	
					nRetCode = Integer.parseInt(xnretcode.getNodeValue());					
					
					if(nRetCode < 0) 
					{
						throw( new CMinkMLException(IMinkCoreDefine.E_RM_setTransaction,"에러인 트랜젝션을 요청한다."));
					}
					
					NodeList xnchildrmd = xnrcmd.getChildNodes();						
					int nJCnt = xnchildrmd.getLength();						
					for(int j = 0; j < nJCnt; j++)
					{
						Node xnrunit = xnchildrmd.item(j);
						if(xnrunit.getNodeType() != Node.ELEMENT_NODE) continue;							
						Node xnrunitref = xnrunit.getAttributes().getNamedItem("refid");
						if(xnrunitref != null && xnrunitref.getNodeValue().equals(Integer.toString(nFunctionID)))
						{								
							bFind = true;
							NodeList xnchildunit = xnrunit.getChildNodes();						
							int nZCnt = xnchildunit.getLength();						
							for(int Z = 0; Z < nZCnt; Z++)
							{
								Node xnrparam = xnchildunit.item(Z);
								if(xnrparam.getNodeType() != Node.ELEMENT_NODE) continue;							
								
								
								NodeList xnchildparam = xnrparam.getChildNodes();						
								int nYCnt = xnchildparam.getLength();						
								for(int Y = 0; Y < nYCnt; Y++)
								{
									Node xncol = xnchildparam.item(Y);
									if(xncol.getNodeType() != Node.ELEMENT_NODE) continue;									
									if(xncol.getNodeName().equals("rcolhs"))
									{			
										setColInfo(xncol);
									}
									else //rcolbs
									{	
										m_lstRecord = xncol.getChildNodes();
									}								
								}							
							}							
							break;
						}	
					}						
									
				}
				else
				{
					CMinkLogMan.WriteE(IMinkCoreDefine.E_ML_recode,"retid Not found.");
				}
				break;
			}			
		}		
		
		if(bFind == false)
			throw( new CMinkMLException(IMinkCoreDefine.E_RM_setTransaction1,"Not found unit's refid."));
		return ;
	}
	
	/**
	 * 컬럼의 개수.
	 * @return
	 */
	public int getColSize()
	{
		return m_lstColInfo.size();
	}
	
	/**
	 * 컬럼 정보를 알아온다.
	 * @param nIndex 0부터 시작한다
	 * @return
	 */
	public CColInfo getColInfo(int nIndex)
	{
		return m_lstColInfo.get(nIndex);
	}
	
	public Object MoveFirst()
	{				
		if(m_lstRecord == null || m_lstRecord.getLength() == 0) return null;
		m_nCursor = 0;
		return m_lstRecord.item(m_nCursor);		
	}
	
	public Object MoveNext()
	{
		if(m_lstRecord == null) return null;
		int nSize = m_lstRecord.getLength();			
		if(m_nCursor >= nSize) return null;		
		return m_lstRecord.item(++m_nCursor);		
	}
	
	/**
	 * 각 컬럼의 값을 가져온다.
	 * @param nIndex : 0부터 시작하는 것을 명심하자.
	 * @return
	 */
	public String getData(int nIndex)
	{
		Node xnRec = m_lstRecord.item(m_nCursor);
		NodeList xnFList = xnRec.getChildNodes();		
		//0부터 시작하는
		return CStringMan.getString(xnFList.item(nIndex));
	}
	
	public void setData(int nIndex,String sValue)
	{
		Node xnRec = m_lstRecord.item(m_nCursor);
		NodeList xnFList = xnRec.getChildNodes();
		xnFList.item(nIndex).setTextContent(sValue);
	}
	
	/**
	 * 각컬럼값을 가져온다.
	 * @param sColName
	 * @return
	 * @throws CMinkException 
	 */
	public String getData(String sColName) throws CMinkException
	{	
		//Node xnRec = m_lstRecord.item(m_nCursor);
		Integer nIndex = m_hsColIndexMap.get(sColName);
		if(nIndex == null)
		{
			
			throw ( new CMinkException(IMinkCoreDefine.E_RM_getData,sColName + "찾을 수가 없다."));
		}
//		NodeList xnFList = xnRec.getChildNodes();		
//		return xnFList.item(nIndex).getNodeValue();
		
		//Modified By Song 2010.09.14 수정함.
		return getData(nIndex);
	}
	
	void setColInfo(Node xnColHeader)
	{
		String sName;
		NodeList lstColh = xnColHeader.getChildNodes();
		int nCnt = lstColh.getLength();
		for(int i = 0; i < nCnt; i++)
		{
			Node xnColh = lstColh.item(i);				
			NamedNodeMap attMap = xnColh.getAttributes();
			
			sName = attMap.getNamedItem("name").getNodeValue();
			Node xnLength = attMap.getNamedItem("length");
			CColInfo param = new CColInfo(		
					sName,
			attMap.getNamedItem("type").getNodeValue(),
			xnLength != null ? xnLength.getNodeValue():"");
			m_lstColInfo.add(param);

			
			m_hsColIndexMap.put(sName,i);
		}
	}
}
