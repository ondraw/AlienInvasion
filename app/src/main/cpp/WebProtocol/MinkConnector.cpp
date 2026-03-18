#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include "MinkConnector.h"
#include "MinkRecordOrg.h"
#include "MinkRecordSet.h"


MinkConnector::MinkConnector(
		const char* strUserID,
		const char* strPassword,
		const char* strDeviceID,
		const char* strURL,
		const char* strTarget,HttpRequestXml pSendFunction) : MinkProtocol(strURL,60,pSendFunction)
{
	//_strLastError = NULL;
	_sndMink = NULL;
	_sndCmd = NULL;

	
	_strTarget = new char[strlen(strTarget) + 1];
	strcpy(_strTarget,strTarget);

	_strUserID = new char[strlen(strUserID) + 1];
	strcpy(_strUserID,strUserID);

	_strPassword = new char[strlen(strPassword) + 1];
	strcpy(_strPassword,strPassword);

	_strDeviceID = new char[strlen(strDeviceID) + 1];
	strcpy(_strDeviceID,strDeviceID);

}

MinkConnector::~MinkConnector(void)
{
	SendClear();
	if(_strTarget)
	{
		delete[] _strTarget;
		_strTarget= NULL;
	}

	if(_strUserID)
	{
		delete[] _strUserID;
		_strUserID= NULL;
	}

	if(_strPassword)
	{
		delete[] _strPassword;
		_strPassword= NULL;
	}

	if(_strDeviceID)
	{
		delete[] _strDeviceID;
		_strDeviceID= NULL;
	}
}

void MinkConnector::SendClear()
{
	if(m_pDoc)
	{
		xmlFreeDoc(m_pDoc);
		m_pDoc = NULL;
	}
}

bool MinkConnector::InitInfo(bool bAutoCommit)
{
	SendClear();
	_sndMink = Mink(MINKPROTOCALVERSION,_strUserID,_strPassword,_strDeviceID);
	if(!_sndMink) return false;	
	_sndCmd = AddCommand(_sndMink,MINKPROTOCALCLASSID,MINKPROTOCALCMDREFID,_strTarget,bAutoCommit);
	if(!_sndCmd) return false;
	return true;
}

bool MinkConnector::AddExecute(int nUnitRefID,const char* strSQL)
{	
	xmlNode* xnParam = NULL;
	xmlNode* xnUnit = GetRUnit(_sndCmd,nUnitRefID);
	if(xnUnit == NULL)
		xnUnit = AddUnit(_sndCmd,"execute",nUnitRefID);
	xnParam = AddParam(xnUnit,strSQL,"in","string");

	
	if(!xnParam) return false;
	return true;
}

bool MinkConnector::AddFetch(int nUnitRefID,const char* strSQL)
{
	xmlNode* xnParam = NULL;
	xmlNode* xnUnit = GetRUnit(_sndCmd,nUnitRefID);
	if(xnUnit == NULL)
		xnUnit = AddUnit(_sndCmd,"fetch",nUnitRefID);	
	xnParam = AddParam(xnUnit,strSQL,"in","table");
	if(!xnParam) return false;	
	return true;
}


bool MinkConnector::AppendResult()
{
	char sPath[128];
	xmlXPathContext  *xpathCtx = NULL; 
    xmlXPathObject   *xpathObj = NULL; 
	xmlNodeSet		 *xnSet = NULL;
	
	//XPath 의 예제 이다. 잘 활용하기를 바란다.
		
	/* Create xpath evaluation context */
    xpathCtx = xmlXPathNewContext(m_pDoc);
    if(xpathCtx == NULL) {     
        //KANG
		//setError(-1, "Error: unable to create new XPath context");
        return false;
    }

	sprintf(sPath,"/mink/cmd[@refid='%d']/unit",MINKPROTOCALCMDREFID);
	 
	/* Evaluate xpath expression */
    xpathObj = xmlXPathEvalExpression(BAD_CAST sPath, xpathCtx);
    if(xpathObj == NULL) {
     
        //KANG
		//setError(-1, "Error: unable to evaluate xpath expression");
        xmlXPathFreeContext(xpathCtx); 
        return false;
    }
	
	int nSize = 0;
	int i = 0;
		
	xnSet = xpathObj->nodesetval;
	nSize = (xnSet) ? xnSet->nodeNr : 0;
	
	for(i = nSize - 1; i >= 0; i--)
	{
		xmlNode* pParam = AddParam(xnSet->nodeTab[i],NULL,"result","table");
		if(pParam == NULL)
		{
			/* Cleanup of XPath data */
			xmlXPathFreeObject(xpathObj);
			xmlXPathFreeContext(xpathCtx); 
			return false;
		}
	}
	
	 /* Cleanup of XPath data */
    xmlXPathFreeObject(xpathObj);
    xmlXPathFreeContext(xpathCtx); 
	return true;
}

/*return char* 메모리 해제 할것...*/
char* MinkConnector::GetReqMessage(bool bIsEnc, int* nOrgContentsLen, int* nReqBodyContentsLen)
{

	if(!AppendResult()) return NULL;
	return GetReqMessageMinkProtocol(bIsEnc, nOrgContentsLen, nReqBodyContentsLen);
}

int MinkConnector::GetRespRecordSet(bool bIsEnc, int nReceived, int nUnCompLen, char* respMsg, MinkRecordSet** outResult)
{
	MinkRecordSet *pRcvSet = NULL;
	int nResult = 0;
	
	pRcvSet = GetRespMessageMinkProtocol(bIsEnc, nReceived, nUnCompLen, respMsg);
	
	if (pRcvSet == NULL) {
		nResult = -1;
	}
	*outResult = pRcvSet;
	return nResult;
}


int MinkConnector::Execute(MinkRecordSet** outResult)
{


	MinkRecordSet *pRcvSet = NULL;
	int nResult = 0;	
	if(!AppendResult()) return -1;
    
	pRcvSet =	SendData();
	if(pRcvSet == NULL)  //System Error					
		return -1;
    *outResult = pRcvSet;
	return nResult;
}

bool MinkConnector::Fetch(const char* strSQL,MinkRecordSet** outResult)
{
	InitInfo(true);
	AddFetch(1,strSQL);
	if(Execute((MinkRecordSet**)outResult) != 0)		return false;
	if(!(*outResult)->SetRefID(1)) return false;
	return true;
}

int MinkConnector::Execute(const char* strSQL,MinkRecordSet** outResult,bool bAutoCommit)
{
	InitInfo(bAutoCommit);
	AddExecute(1,strSQL);	
	return Execute(outResult);
}

int  MinkConnector::Function(const char* strClsId, const char* strTarget,const char* strFunctionName,std::list<RM_PARAM*>* pPARAMList,MinkRecordSet** outResult,bool bAutoCommit)
{	
	InitInfo(strClsId,strTarget,bAutoCommit);
	AddFunction(MINKPROTOCALCMDREFID,strFunctionName,pPARAMList);
	return Execute((MinkRecordSet**)outResult);
}

bool MinkConnector::InitInfo(const char* strClsId, const char* strTarget,bool bAutoCommit)
{
	SendClear();
	_sndMink = Mink(MINKPROTOCALVERSION, _strUserID, _strPassword,_strDeviceID);
	if(!_sndMink) return false;	
	_sndCmd = AddCommand(_sndMink,strClsId,MINKPROTOCALCMDREFID,strTarget,bAutoCommit);
	if(!_sndCmd) return false;
	return true;
}

#if 0
/*version test*/
bool MinkConnector::InitVerManager(const char* strClsId, const char* strTarget,bool bAutoCommit
								   ,const char* sInstallName, const char* sUserID, const char* sDeviceID )
{
	SendClear();
	_sndMink = Mink(MINKPROTOCALVERSION, _strUserID, _strPassword,_strDeviceID);
	if(!_sndMink) return false;
	
	
	_sndCmd = AddCommand(_sndMink,strClsId,MINKPROTOCALCMDREFID,strTarget,bAutoCommit);
	if(!_sndCmd) return false;
	return true;
}
#endif

void MinkConnector::AddCmdExtendAttInfo(const char* sName, const char* sValue)
{
	AddAttribute(_sndCmd,sName,sValue);
}



bool  MinkConnector::AddFunctionCDataBlock(int nUnitRefID,const char* strFunctionName,std::list<RM_PARAM*>* pPARAMList)
{
	xmlNode* xnParam = NULL;
	xmlNode* xnUnit = GetRUnit(_sndCmd,nUnitRefID);
	
	if(xnUnit == NULL)
		xnUnit = AddUnit(_sndCmd,strFunctionName,nUnitRefID);
    
	if(pPARAMList)
	{
		std::list<RM_PARAM*>::iterator it;
		for (it = pPARAMList->begin(); it != pPARAMList->end(); it++) 
		{
			RM_PARAM* pR = *it;	
			
			xnParam = AddParamCDataBlock(
                                         xnUnit,
                                         (const char*)pR->btValue,
                                         pR->strMode.c_str(),
                                         pR->strType.c_str(),
                                         pR->strName.c_str(),
                                         (int)pR->nLength);
			
			if(!xnParam) return false;
			delete pR;
		}	
	}
	return true;
}


bool  MinkConnector::AddFunction(int nUnitRefID,const char* strFunctionName,std::list<RM_PARAM*>* pPARAMList)
{
	xmlNode* xnParam = NULL;
	xmlNode* xnUnit = GetRUnit(_sndCmd,nUnitRefID);
	
	if(xnUnit == NULL)
		xnUnit = AddUnit(_sndCmd,strFunctionName,nUnitRefID);

	if(pPARAMList)
	{
		std::list<RM_PARAM*>::iterator it;
		for (it = pPARAMList->begin(); it != pPARAMList->end(); it++) 
		{
			RM_PARAM* pR = *it;	
			
			xnParam = AddParam(
			xnUnit,
			(const char*)pR->btValue,
			pR->strMode.c_str(),
			pR->strType.c_str(),
			pR->strName.c_str(),
			(int)pR->nLength);
			
			if(!xnParam) return false;
			delete pR;
		}	
	}
	return true;
}
