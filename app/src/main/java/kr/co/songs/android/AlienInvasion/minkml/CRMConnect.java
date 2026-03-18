package kr.co.songs.android.AlienInvasion.minkml;

import java.util.ArrayList;

import org.w3c.dom.Document;
import org.w3c.dom.Node;

import kr.co.songs.android.AlienInvasion.com.IMinkCoreDefine;
import kr.co.songs.android.AlienInvasion.http.ASendDataToServer;
import kr.co.songs.android.AlienInvasion.http.CExceptionNetwork;

public class CRMConnect extends CRemoveServiceEng
{

	protected String m_sTarget;
	protected String m_sClsID;
	protected ASendDataToServer m_ConnObj; 
	
	public void setTarget(String v) { m_sTarget = v;}
	public void setClsID(String v) { m_sClsID = v;}
	public String getClsID() { return m_sClsID;}
	public String getTarget() { return m_sTarget;}
	
	
	public CRMConnect(String userId, String password, String deviceID,String sInstallName, String sClsID,String sTarget,ASendDataToServer ConnObj) 
	{		
		super(userId, password, deviceID,sInstallName);	
		m_sTarget = sTarget;
		m_sClsID  = sClsID;
		m_ConnObj = ConnObj;
	}
	
	public CRMConnect(ASendDataToServer ConnOb) 
	{		
		super(null, null, null,null);	
		m_sTarget = null;
		m_sClsID  = null;
		m_ConnObj = ConnOb;
	}
	
	public CRMRecordSet Fetch(String sSQL) throws CMinkMLException
	{		
		CRMRecordSet Rec = null;
		int nResult = 0;
		ArrayList<CMinkParam> lstParam = new ArrayList<CMinkParam>(1);
		Begine();
		AddTransaction(m_sClsID, m_sTarget);		
		CMinkParam param = new CMinkParam(sSQL);
		lstParam.add(param);
		param = new CMinkParam("","result","table");
		lstParam.add(param);		
		AddFunction("fetch",lstParam);		
		
		//데이터를 받아온다.		
		Document rcvDoc = null;
		try 
		{
			rcvDoc = (Document)m_ConnObj.SendData((CMinkML)this);
			nResult = getResultCode(rcvDoc, 1);		
			if(nResult == 0)
			{ 
				Rec = new CRMRecordSet(rcvDoc);
				Rec.setTransaction();
			}
			else
				throw (new CMinkMLException(IMinkCoreDefine.E_RM_Fetch,m_sLastError));
			
		}
		catch (CExceptionNetwork e) 
		{
			throw (new CMinkMLException(e.getCode(),e.getMessage()));
		} 
		catch (CMinkException e) 
		{
			throw (new CMinkMLException(e.getCode(),e.getMessage()));
		}
		catch (Exception e)
		{
			throw (new CMinkMLException(IMinkCoreDefine.E_RM_Fetch1,e.getMessage()));
		}
		return Rec;
	}
	
	
	
	/**
	 * 
	 * @param lstFunctionName
	 * @param lstInParam   : 
	 *     function 1 = name : value : mode : type | name : value : mode : type 으로 구성한다.
	 *     function 2 = name : value : mode : type | name : value : mode : type 어레이로 구성된다.
	 * @param sMoreCmdAttr : Cmd 의 Attribute를 추가한다. name,value 로구성한다.
	 * @return
	 * @throws CMinkMLException
	 */
	public CRMRecordSet Function(String lstFunctionName[], String[] lstInParam,String sMoreCmdAttr) throws CMinkMLException
	{
		String sSeperate = String.format("%c", 0x1E);
		Node xnCmd = null;
		CMinkParam param  = null;
		CRMRecordSet Rec = null;
		ArrayList<CMinkParam> lstParam = new ArrayList<CMinkParam>(1);
		int nResult = 0;
		Begine();
		
		xnCmd = AddTransaction(m_sClsID, m_sTarget);
		
		if(sMoreCmdAttr != null)
		{
			String[] arrMore = sMoreCmdAttr.split(",");
			for(String sMore : arrMore)
			{
				String[] arrM = sMore.split("=");
				AddAttribute(xnCmd, arrM[0], arrM[1]);
			}	
		}
		
		int nIndex = 0;
		for(String sFunction : lstFunctionName)
		{
			if(lstInParam != null)
			{			
				
				if( nIndex < lstInParam.length && lstInParam[nIndex] != null)
				{
					String []splitSP = lstInParam[nIndex].split(sSeperate);
					for(int i = 0 ; i < splitSP.length ; i++)
					{					
						//CMinkParam(String sName,String sValue,String sMode,String sType)
						if(i % 4 == 0 || i == 0)
						{							
							param = new CMinkParam(splitSP[i],splitSP[i+1],splitSP[i+2],splitSP[i+3]);
							lstParam.add(param);
						}
					}
					
					
				}
			}
			
			param = new CMinkParam("","result","table");
			lstParam.add(param);			
			AddFunction(sFunction,lstParam);		
			lstParam.clear(); 
			
			nIndex++;
		}
		
		//데이터를 받아온다.		
		Document rcvDoc = null;
		try 
		{
			rcvDoc = (Document)m_ConnObj.SendData((CMinkML)this);
			nResult = getResultCode(rcvDoc, 1);		
			if(nResult == 0)
			{ 
				Rec = new CRMRecordSet(rcvDoc);
				Rec.setTransaction();
				//로그인 정보 를 받아 올때 정보만 예외 로 해단 코드를 보낸다	
			}
			else
			{
				throw (new CMinkMLException(nResult,m_sLastError));
			}
			
		}
		catch (CExceptionNetwork e) 
		{
			throw (new CMinkMLException(e.getCode(),e.getMessage()));
		} 
		catch (CMinkException e) 
		{
			throw (new CMinkMLException(e.getCode(),e.getMessage()));
		}
		catch (Exception e)
		{
			throw (new CMinkMLException(IMinkCoreDefine.E_RM_Function1,e.getMessage()));
		}
		return Rec;
	}
	
	
	public CRMRecordSet Fetch(String lstSQL[]) throws CMinkMLException
	{
		CRMRecordSet Rec = null;
		ArrayList<CMinkParam> lstParam = new ArrayList<CMinkParam>(1);
		int nResult = 0;
		Begine();
		AddTransaction(m_sClsID, m_sTarget);		
		for(String sSQL : lstSQL)
		{
			CMinkParam param = new CMinkParam(sSQL);
			lstParam.add(param);
			param = new CMinkParam("","result","table");
			lstParam.add(param);
			AddFunction("fetch",lstParam);		
			lstParam.clear(); 
		}
		
		//데이터를 받아온다.		
		Document rcvDoc = null;
		try 
		{
			rcvDoc = (Document)m_ConnObj.SendData((CMinkML)this);
			nResult = getResultCode(rcvDoc, 1);		
			if(nResult == 0)
			{ 
				Rec = new CRMRecordSet(rcvDoc);
				Rec.setTransaction();
			}
			else
				throw (new CMinkMLException(IMinkCoreDefine.E_RM_Fetch2,m_sLastError));
			
		}
		catch (CExceptionNetwork e) 
		{
			throw (new CMinkMLException(e.getCode(),e.getMessage()));
		} 
		catch (CMinkException e) 
		{
			throw (new CMinkMLException(e.getCode(),e.getMessage()));
		}
		catch (Exception e)
		{
			throw (new CMinkMLException(IMinkCoreDefine.E_RM_Fetch3,e.getMessage()));
		}
		return Rec;
	}
	
	
	public CRMRecordSet Fetch(ArrayList<String> lstSQL) throws CMinkMLException
	{
		CRMRecordSet Rec = null;
		ArrayList<CMinkParam> lstParam = new ArrayList<CMinkParam>(1);
		int nResult = 0;
		Begine();
		AddTransaction(m_sClsID, m_sTarget);		
		for(String sSQL : lstSQL)
		{
			CMinkParam param = new CMinkParam(sSQL);
			lstParam.add(param);
			param = new CMinkParam("","result","table");
			lstParam.add(param);
			AddFunction("select",lstParam);		
			lstParam.clear(); 
		}
		
		//데이터를 받아온다.		
		Document rcvDoc = null;
		try 
		{
			rcvDoc = (Document)m_ConnObj.SendData((CMinkML)this);
			nResult = getResultCode(rcvDoc, 1);		
			if(nResult == 0)
			{ 
				Rec = new CRMRecordSet(rcvDoc);
				Rec.setTransaction();
			}
			else
				throw (new CMinkMLException(IMinkCoreDefine.E_RM_Fetch4,m_sLastError));
		}
		catch (CExceptionNetwork e) 
		{
			throw (new CMinkMLException(e.getCode(),e.getMessage()));
		} 
		catch (CMinkException e) 
		{
			throw (new CMinkMLException(e.getCode(),e.getMessage()));
		}
		catch (Exception e)
		{
			throw (new CMinkMLException(IMinkCoreDefine.E_RM_Fetch5,e.getMessage()));
		}
		return Rec;
	}
	
	public int Execute(String sSQL) throws CMinkMLException
	{
		return Execute(sSQL,null);
	}
	public int Execute(ArrayList<String> lstSQL) throws CMinkMLException
	{
		return Execute(lstSQL,null);
	}
	
	public int Execute(String sSQL,CRMRecordSet outSet) throws CMinkMLException
	{
		int nResult = 0;
		ArrayList<CMinkParam> lstParam = new ArrayList<CMinkParam>(1);
		Begine();
		AddTransaction(m_sClsID, m_sTarget);		
		CMinkParam param = new CMinkParam(sSQL,"in","string");
		lstParam.add(param);
		param = new CMinkParam("","result","table");
		lstParam.add(param);		
		AddFunction("execute",lstParam);		
		
		//데이터를 받아온다.		
		Document rcvDoc = null;
		try 
		{
			rcvDoc = (Document)m_ConnObj.SendData((CMinkML)this);
			nResult = getResultCode(rcvDoc, 1);		
			if(nResult < 0)
				throw (new CMinkMLException(IMinkCoreDefine.E_HTTP_Execute,m_sLastError));
			
			if(outSet != null)
				outSet.setDoc(rcvDoc);
			
		}
		catch (CExceptionNetwork e) 
		{
			throw (new CMinkMLException(e.getCode(),e.getMessage()));
		} 
		catch (CMinkException e) 
		{
			throw (new CMinkMLException(e.getCode(),e.getMessage()));
		}
		catch (Exception e)
		{

			throw (new CMinkMLException(IMinkCoreDefine.E_HTTP_Execute1,e.getMessage()));
		}
		return nResult;
	}
	
	
	public int Execute(ArrayList<String> lstSQL,CRMRecordSet outSet) throws CMinkMLException
	{
		int nResult = 0;
		ArrayList<CMinkParam> lstParam = new ArrayList<CMinkParam>(1);
		Begine();
		AddTransaction(m_sClsID, m_sTarget);	
		
		for(String sSQL : lstSQL)
		{
			CMinkParam param = new CMinkParam(sSQL,"in","string");
			lstParam.add(param);
			param = new CMinkParam("","result","table");
			lstParam.add(param);
			AddFunction("execute",lstParam);
			lstParam.clear();
		}
		
		//데이터를 받아온다.		
		Document rcvDoc = null;
		try 
		{
			rcvDoc = (Document)m_ConnObj.SendData((CMinkML)this);
			nResult = getResultCode(rcvDoc, 1);			
			if(nResult < 0)
				throw (new CMinkMLException(IMinkCoreDefine.E_HTTP_Execute2,m_sLastError));
			
			if(outSet != null)
				outSet.setDoc(rcvDoc);
			
		}
		catch (CExceptionNetwork e) 
		{
			throw (new CMinkMLException(e.getCode(),e.getMessage()));
		} 
		catch (CMinkException e) 
		{
			throw (new CMinkMLException(e.getCode(),e.getMessage()));
		}
		catch (Exception e)
		{
			throw (new CMinkMLException(IMinkCoreDefine.E_HTTP_Execute3,e.getMessage()));
		}
		return nResult;
	}	
	
}
