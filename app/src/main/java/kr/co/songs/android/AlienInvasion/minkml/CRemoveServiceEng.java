package kr.co.songs.android.AlienInvasion.minkml;

import java.util.ArrayList;
import java.util.HashMap;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

import kr.co.songs.android.AlienInvasion.com.CMinkLogMan;
import kr.co.songs.android.AlienInvasion.com.CStringMan;
import kr.co.songs.android.AlienInvasion.com.IMinkCoreDefine;

public class CRemoveServiceEng extends CMinkML {
	
	protected HashMap<Integer,Node> m_mapCMDID;
	protected HashMap<Integer,Node> m_mapUnitID;		
	protected int					  m_nAutoIncrementUnitID;
	protected String				  m_sLastError;
	
	public String getLastError() { return m_sLastError;}
	
	
	
	public CRemoveServiceEng(String sUserId, String sPassword,String sDeviceID,String sInstallName)
	{
		super(sUserId,sPassword,sDeviceID,sInstallName);
		m_mapCMDID = null;
		m_mapUnitID = null;
	}	
		
	public boolean Begine()
	{
		Node xnMink;
		Clear();		
		xnMink = MakeMink(MINKPROTOCALVERSION, m_sUser, m_sPassword,m_sDeviceID);
		if(xnMink == null) return false;
		return true;
	}	
	
	/**
	 * 트랜잭션을 추가한다. 서버에 트랜잭션 처리를 하지 않음 (Select에서 자주 사용한다.), 
	 * @param sClsId Spring의 클래스 아이디
	 * @param sTarget JNDI 명이나 기타 컨넥트 리소스
	 * nTransactionID = 1
	 * @return
	 */
	public Node AddTransaction(String sClsId, String sTarget)
	{
		return AddTransaction(sClsId,sTarget,false);
	}
	
	/**
	 * 트랜잭션을 추가한다. 
	 * @param sClsId Spring의 클래스 아이디
	 * @param sTarget JNDI 명이나 기타 컨넥트 리소스
	 * @param bAutoCommit true: 서버에서 트랜잭션처리를 한다, false 서버에서 트랜책션처리를 하지 않음
	 * nTransactionID = 1
	 * @return
	 */
	public Node AddTransaction(String sClsId, String sTarget,boolean bAutoCommit)
	{
		return AddTransaction(sClsId,sTarget,bAutoCommit,1,null);
	}
	
	/**
	 * 트랜잭션을 추가한다.
	 * @param sClsId : Spring의 클래스 아이디
	 * @param sTarget : JNDI 명이나 기타 컨넥트 리소스
	 * @param bAutoCommit : 자동 커밋
	 * @param nTransactionID : 하나의 트랜잭션의 아이디
	 * @param sActionID : 아직 사용하지 않음
	 * @return
	 */
	//protected Node AddCommand(Node xnMink,String strClassID,int refID,String strTarget,
	//		String company,String companyCode , String companyId)
	public Node AddTransaction(String sClsId, String sTarget,boolean bAutoCommit,int nTransactionID,String sActionID)
	{		
		Node xnMink,xnCmd;
		xnMink = m_Doc.getDocumentElement();	
		xnCmd = m_mapCMDID.get(nTransactionID);
		if(xnCmd != null)
			m_mapCMDID.remove(nTransactionID);			
		
		
        //버전 관련 해서 추가 했음  by seo
		xnCmd = AddCommandVerMan(xnMink);
			m_mapCMDID.put(nTransactionID, xnCmd);	
	
		xnCmd = AddCommand(xnMink,sClsId,nTransactionID,sTarget,bAutoCommit,sActionID);		
		m_mapCMDID.put(nTransactionID, xnCmd);	
		return xnCmd;
	}	
	
	/**
	 * 하나의 트랙잭션에 여러개의 함수를 Add한다.
	 * nTransactionID = 1, FunctionID는 자동 증가 한다.
	 * @param sFunctionName
	 * @param lstParam
	 * @return
	 * @throws CMinkMLException 
	 */
	public void  AddFunction(String sFunctionName,ArrayList<CMinkParam> lstParam) throws CMinkMLException
	{		
		AddFunction(1,m_nAutoIncrementUnitID++,sFunctionName,lstParam);
	}
	
	/**
	 * 하나의 트랙잭션에 여러개의 함수를 Add한다.
	 * nTransactionID = 1, FunctionID는 자체 관리를 한다.
	 * @param sFunctionName
	 * @param lstParam
	 * @return
	 * @throws CMinkMLException 
	 */
	public void AddFunction(int nFunctionID,String sFunctionName,ArrayList<CMinkParam> lstParam) throws CMinkMLException
	{
		m_nAutoIncrementUnitID = nFunctionID;					
		AddFunction(1,m_nAutoIncrementUnitID,sFunctionName,lstParam);
	}
	
	/**
	 * 
	 * @param nTransactionID : 존재하는 트랜잭션 아이디를 넣어야 한다.
	 * @param nFunctionID : 펑션 아이디는 유니크해야한다.
	 * @param sFunctionName : 함수 이름
	 * @param lstParam : 파라미터 리스트
	 * @return
	 * @throws CMinkMLException 
	 */
	public void AddFunction(int nTransactionID,int nFunctionID,String sFunctionName,ArrayList<CMinkParam> lstParam) throws CMinkMLException
	{		
		Node xnUnit = null;		
		Node xnCmd = null;
		
		xnCmd = m_mapCMDID.get(nTransactionID);
		if(xnCmd == null) throw (new CMinkMLException(-1,"Transaction ID가 없습니다."));
		
		xnUnit = m_mapUnitID.get(nFunctionID);
		if(xnUnit != null) m_mapUnitID.remove(nFunctionID);
		
		xnUnit = AddUnit(xnCmd,sFunctionName,nFunctionID);
		m_mapUnitID.put(nFunctionID, xnUnit);
		if(lstParam != null)
		{
			for(CMinkParam param : lstParam)
			{
				AddParam(xnUnit,param.getValue(),param.getMode(),param.getType(),param.getName(),(int)param.getLength());
			}			
		}			
	}	
	
	
	@Override
	protected void Clear() {
		m_nAutoIncrementUnitID = 1;
		
		m_mapCMDID = new HashMap<Integer,Node>();
		m_mapUnitID = new HashMap<Integer,Node>();
		m_sLastError = "";
		super.Clear();
	}
	
	
	/**
	 * 응답의 결과 코드를 받아온다.
	 * 하나의 트랜잭션안에 여러개의 Function ID를 검사하여
	 * 한개의 Function이라도 에러가 나면 에러를 리턴한다.
	 * @param docRcv
	 * @param nTransactionID
	 * @return
	 * @throws CMinkMLException 
	 */
	public int getResultCode(Document docRcv,int nTransactionID) throws CMinkMLException
	{
		int nResult = 0;
	
		Element xnRoot = docRcv.getDocumentElement();	
		
		NodeList lstChildRoot = xnRoot.getChildNodes();
		int nCnt = lstChildRoot.getLength();	
		
		
		//Added By Song 2010.07.26 에러가 나면 refid 존재 하지 않다.
		NamedNodeMap attMink = xnRoot.getAttributes();
		Node xnRetCode = attMink.getNamedItem("retcode");
		if(xnRetCode != null)
		{
			nResult = Integer.parseInt(xnRetCode.getNodeValue());
			if(nResult < 0)
			{
				
				NodeList xnChildMink = xnRoot.getChildNodes();						
				int nZCnt = xnChildMink.getLength();						
				for(int Z = 0; Z < nZCnt; Z++)
				{
					Node xnrparam = xnChildMink.item(Z);
					if(xnrparam.getNodeType() != Node.ELEMENT_NODE) continue;
					//에거가 존재하면 리턴값은 에러이다.
					m_sLastError = CStringMan.getString(xnrparam);
					break;					
				}	
				
				return nResult;
			}
		}
		
		
		for(int i = 0; i < nCnt; i++ )
		{
			Node xnrcmd = lstChildRoot.item(i);			
			
			if(xnrcmd.getNodeType() != Node.ELEMENT_NODE) continue;			
							
			
			NamedNodeMap attcmd = xnrcmd.getAttributes();
			Node xnrefid = attcmd.getNamedItem("refid");
			if(xnrefid != null && Integer.parseInt(xnrefid.getNodeValue()) == nTransactionID)
			{
				nResult = getResult(nResult, xnrcmd, attcmd);
				break;
			}	
		}		
		return nResult;
	}



	private int getResult(int nResult, Node xnrcmd, NamedNodeMap attcmd)
			throws CMinkMLException {
		int nRetCode;
		Node xnretcode = attcmd.getNamedItem("retcode");
		if(xnretcode != null)
		{	
			nRetCode = Integer.parseInt(xnretcode.getNodeValue());					
			NodeList xnchildrmd = xnrcmd.getChildNodes();						
			int nJCnt = xnchildrmd.getLength();						
			for(int j = 0; j < nJCnt; j++)
			{
				Node xnrunit = xnchildrmd.item(j);
				if(xnrunit.getNodeType() != Node.ELEMENT_NODE) continue;							
				Node xnMode = xnrunit.getAttributes().getNamedItem("mode");
				if(nRetCode >= 0 && xnMode != null && xnMode.getNodeValue().equals("execute"))
				//if(nRetCode >= 0)
				{	
					NodeList xnchildunit = xnrunit.getChildNodes();						
					int nZCnt = xnchildunit.getLength();						
					for(int Z = 0; Z < nZCnt; Z++)
					{
						Node xnrparam = xnchildunit.item(Z);
						if(xnrparam.getNodeType() != Node.ELEMENT_NODE) continue;							
						
						try
						{
							nResult += Integer.parseInt(CStringMan.getString(xnrparam));
						}
						catch(Exception ex)
						{

							CMinkLogMan.WriteE(IMinkCoreDefine.E_SVCEND_getResultCode, CStringMan.getString(xnrparam));
						}
													
					}						
				}	
				else if(nRetCode < 0) //에러
				{	
					NodeList xnchildunit = xnrunit.getChildNodes();						
					int nZCnt = xnchildunit.getLength();						
					for(int Z = 0; Z < nZCnt; Z++)
					{
						Node xnrparam = xnchildunit.item(Z);
						if(xnrparam.getNodeType() != Node.ELEMENT_NODE) continue;
						//에거가 존재하면 리턴값은 에러이다.
						m_sLastError = CStringMan.getString(xnrparam);
						nResult = nRetCode;
						break;					
					}						
				}	
			}						
							
		}
		else
		{
			throw (new CMinkMLException(IMinkCoreDefine.E_SVCEND_getResultCode1,"retCode가 없습니다."));
		}
		return nResult;
	}
}
