/*
 *  MinkProtocol.cpp
 *  TestMnk
 *
 *  Created by 호학 송 on 10. 3. 23..
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#include <string>
#include <time.h>

#include "des3.h"
#include <zlib.h>

#include "MinkRecordOrg.h"
#include "MinkRecordSet.h"

#include "MinkProtocol.h"
#include "StringBuffer.h"

#define TAG_ROLB		"rcolb"
#define TAG_RCOLH		"rcolh"
#define TAG_RCMD		"rcmd"
#define TAG_RUNIT		"runit"
#define TAG_RPARAM		"rparam"
#define TAG_RCOLHS		"rcolhs"
#define TAG_RCOLBS      "rcolbs"



//참조 http://www.xmlsoft.org
static xmlSAXHandler simpleSAXHandlerStruct = {
    NULL,										/* internalSubset */
    NULL,										/* isStandalone   */
    NULL,										/* hasInternalSubset */
    NULL,										/* hasExternalSubset */
    NULL,										/* resolveEntity */
    NULL,										/* getEntity */
    NULL,										/* entityDecl */
    NULL,										/* notationDecl */
    NULL,										/* attributeDecl */
    NULL,										/* elementDecl */
    NULL,										/* unparsedEntityDecl */
    NULL,										/* setDocumentLocator */
    NULL,										/* startDocument */
    NULL,										/* endDocument */
	NULL,										/* startElement*/
    NULL,										/* endElement */
    NULL,										/* reference */
	MinkProtocol::charactersFoundSAX,			/* characters */
    NULL,										/* ignorableWhitespace */
    NULL,										/* processingInstruction */
    NULL,										/* comment */
    NULL,										/* warning */
	MinkProtocol::errorEncounteredSAX,			/* error */
    NULL,										/* fatalError //: unused error() get all the errors */
    NULL,										/* getParameterEntity */
    NULL,										/* cdataBlock */
    NULL,										/* externalSubset */
    XML_SAX2_MAGIC,								//
    NULL,
	MinkProtocol::startElementSAX,				/* startElementNs */
	MinkProtocol::endElementSAX,				/* endElementNs */
    NULL,										/* serror */
};

MinkProtocol::MinkProtocol(const char* strURL,int nResponseTimeOut,HttpRequestXml pSendFunction)
{
	m_pDoc = NULL;
    
    memset(msSession, 0x00, 50);
    strcpy(msSession, "session");
	msLastError = NULL;
	msURL = new char[strlen(strURL) + 1];
//	memset(msURL, 0x00, strlen(strURL) + 1);
	strcpy(msURL, strURL);
	mnResponseTimeOut = nResponseTimeOut;
    mpSendFunction = pSendFunction;
}

MinkProtocol::~MinkProtocol()
{
	if(m_pDoc)
	{
		xmlFreeDoc(m_pDoc);
		m_pDoc = NULL;
	}
	
	
	xmlCleanupParser();
	
	if(msURL)
	{
		delete[] msURL;
		msURL = NULL;
	}
	if(msLastError)
	{
		delete[] msLastError;
		msLastError = NULL;
	}
}


//MinkConnector 에서 Override되었다.MinkRecordSet*  
void MinkProtocol::SetLastError(const char* sLastError)
{
	if(msLastError)
	{
		delete[] msLastError;
		msLastError = NULL;
	}
	
	if(sLastError)
	{
		msLastError = new char[strlen(sLastError) + 1];
		strcpy(msLastError,sLastError);
	}	
}

const char* MinkProtocol::GetLastError()
{
	if(msLastError == NULL)
	{
		msLastError = new char[1];
		msLastError[0] = 0;
	}
	
	return msLastError;
}


MinkRecordSet* MinkProtocol::LoadXML(char* strXML)
{
	/* Sample For DOM
	m_pDoc = xmlReadMemory(strXML, strlen(strXML),NULL, "UTF-8", 0);	
	if(m_pDoc == NULL)
	{
		LOG.error("%s","xmlLoadError");
		return false;
	}	
	xmlNode* pxnRoot = xmlDocGetRootElement(m_pDoc);
	int nIndex = xmlChildElementCount(pxnRoot);	
	 */
	MinkRecordSet *pNewRecordSet = new MinkRecordSet();
	xmlParserCtxtPtr context;
	context = xmlCreatePushParserCtxt(&simpleSAXHandlerStruct, pNewRecordSet, NULL, 0, NULL);	
	
	//부분적으로 데이터를 읽어서 파싱하는 구조로 하면 좋지만,
	//통신시 패킷을 압축하여 한꺼번에 데이터를 보내기 때문에 청크를 사용 할 필요는 없다.
	//하지만, 예문으로 사용한다.
	xmlParseChunk(context, (const char *)strXML, (int)strlen(strXML), 0);
	xmlParseChunk(context, NULL, 0, 1);	
	xmlFreeParserCtxt(context);

	//Kang 파싱에러의 경우는 -1로 보이며, 이값 넘겨서 처리하기 위해 널리턴 하지는 않는다..
	//MinkRmRec::GetErrorMsg 메소드 참고.
/*	
	if(pNewRecordSet->GetParsError() < 0)
	{
		SetLastError("서버에서 받은데이터를 파싱을 할 수가 없습니다.");
		delete pNewRecordSet;
		pNewRecordSet = NULL;
	}
*/	
	return pNewRecordSet;
}

char* MinkProtocol::GetReqMessageMinkProtocol(bool bIsEnc, int* nOrgContentsLen, int* nReqBodyContentsLen)
{
	char *pSndXml = NULL;	
	int   nSndXml = 0;
	
	*nOrgContentsLen = 0;
	*nReqBodyContentsLen = 0;
	
	int contentLength = 0;
	
	//------------------------------------------------------
	//Declare Encoding
	int nEnSize = 0;
	BYTE *msgToSend = NULL;
	BYTE *compr = NULL;
	BYTE *pEnData = NULL;
	//------------------------------------------------------

	/*암호화*/	
	char sError[512] = {0x00,};
	int err = 0;
	
	SetLastError(NULL);
	
	xmlDocDumpMemory(m_pDoc, (xmlChar**)&pSndXml, &nSndXml);
	

	
	contentLength = (int)strlen(pSndXml);
	if(!bIsEnc) {
		*nOrgContentsLen = contentLength;
		*nReqBodyContentsLen =contentLength;
		return pSndXml;
	}

	
	
	uLong comprLen = contentLength;
	compr = (BYTE*)malloc(contentLength);
	memset(compr, 0x00, contentLength);
	
	
	err = compress(compr, &comprLen, (Bytef*)pSndXml, contentLength);
	if (err != Z_OK) {
		//char sError[1024];
		sprintf(sError, "ZLIB: error occurred compressing data.");		
//		SetLastError(sError);
		goto _EXIT_REQ;
	}
	
	nEnSize = MinkDes((BYTE*)compr, comprLen, (BYTE**)&pEnData, 0);
	if(nEnSize <= 0)
	{
		//char sError[1024];
		sprintf(sError,  "MinkDes: error occurred Encyper data.");
		SetLastError(sError);
		goto _EXIT_REQ;
	}	
	msgToSend = pEnData;				
	
	*nOrgContentsLen = contentLength;
	*nReqBodyContentsLen = nEnSize;
	
	
	
_EXIT_REQ:
	if(compr) { free(compr);compr = NULL;}
	if(pSndXml) { free(pSndXml);pSndXml = NULL;}

	
	return (char*)msgToSend;
}


MinkRecordSet* MinkProtocol::GetRespMessageMinkProtocol(bool bIsEnc, int nReceived, int nUnCompLen, char* recvBuf)
{
	MinkRecordSet *pRecSet = NULL;
	char sError[512] = {0x00};
	
	BYTE *pDeData = NULL;
	BYTE *uncompr = NULL;
	
	char *pTmpRecvBuf = NULL;

	int nEnSize;
	int nUnCompBufLen;
	int err;
	
	if(!bIsEnc && nReceived>0){
		pTmpRecvBuf = new char[nReceived+1];
		memset(pTmpRecvBuf, 0x00, nReceived+1);
		memcpy(pTmpRecvBuf, recvBuf, nReceived); 
        
        
        //KANG
//		LOG.debug("Response Body....");
//		LOG.debug("%s", pTmpRecvBuf);
		pRecSet = LoadXML(pTmpRecvBuf);	
		delete [] pTmpRecvBuf;
	}
	else {
		
		nEnSize = MinkDes((BYTE*)recvBuf, nReceived, (BYTE**)&pDeData,1);
		
		if(nEnSize <= 0)
		{
			sprintf(sError,  "3Des: error occurred Decyper data.");
			SetLastError(sError);
			goto _EXIT_RESP;
		}
		
		nUnCompBufLen = nUnCompLen+1;
		uncompr = (BYTE*)malloc(nUnCompBufLen);
		memset(uncompr, 0x00, nUnCompBufLen);
		
		err = uncompress(uncompr, (uLongf*)&nUnCompBufLen, (Bytef*)pDeData, nEnSize);
		if(err < 0)
		{
			sprintf(sError,  "Zib: error occurred Decyper data.");
			SetLastError(sError);
			goto _EXIT_RESP;
		}
        
        //KANG
//		LOG.debug("Response Body....");
//		LOG.debug("%s", uncompr);
		pRecSet = LoadXML((char*)uncompr);	
	}
	
	
_EXIT_RESP:
	if(pDeData) {free(pDeData);pDeData = NULL;}
	if(uncompr) { free(uncompr);uncompr = NULL;} 
	
	return pRecSet;
		
}


char* MinkProtocol::decryptionMinkProtocol(int nReceived, int nUnCompLen, char* recvBuf)
{
	
	BYTE *pDeData = NULL;
	BYTE *uncompr = NULL;
	
	int nEnSize;
	int nUnCompBufLen;
	int err;
	
	if(nReceived > 0)
    {
		
		nEnSize = MinkDes((BYTE*)recvBuf, nReceived, (BYTE**)&pDeData,1);
		
		if(nEnSize <= 0)
		{
			printf("3Des: error occurred Decyper data.");
			if(pDeData) {free(pDeData);pDeData = NULL;}
            if(uncompr) { free(uncompr);uncompr = NULL;} 
		}
		
		nUnCompBufLen = nUnCompLen+1;
		uncompr = (BYTE*)malloc(nUnCompBufLen);
		memset(uncompr, 0x00, nUnCompBufLen);
		
		err = uncompress(uncompr, (uLongf*)&nUnCompBufLen, (Bytef*)pDeData, nEnSize);
		if(err < 0)
		{
			printf("Zib: error occurred Decyper data.");
			if(pDeData) {free(pDeData);pDeData = NULL;}
            if(uncompr) { free(uncompr);uncompr = NULL;} 
		}
	}
    
	if(pDeData) {free(pDeData);pDeData = NULL;}
	
	return (char*)uncompr;
}


MinkRecordSet*  MinkProtocol::SendData()
{
	MinkRecordSet *pRecSet = NULL;
	char *pSndXml = NULL;	
	int   nSndXml = 0;
	char *pRcvXML = NULL;
	
	SetLastError(NULL);
    

	//xml to stringng
	xmlDocDumpMemory(m_pDoc, (xmlChar**)&pSndXml, &nSndXml);
    if(mpSendFunction == NULL)
    {
        HLogE("Not Found SendFunction\n");
        SetLastError("Not Found Send Function\n");
        return NULL;
    }
    
    
    HLogD("******************* SndXML ******************\n %s \n**********************\n",pSndXml);
    HLogD("mpSendFunction(msURL,pSndXml)\n");
    pRcvXML = mpSendFunction(msURL,pSndXml);
    
	if(pSndXml)
	{
		xmlFree((xmlChar*)pSndXml);
		pSndXml = NULL;
	}	
	
	if(pRcvXML == NULL)
	{
        HLogE("Error RcvXML is null\n");
        SetLastError("Error RcvXML is null\n");
		return NULL;
	}
    
	HLogD("******************* RcvXML ******************\n %s \n**********************\n",pRcvXML);
	pRecSet = LoadXML(pRcvXML);
    if(pRcvXML)
    {
        free(pRcvXML);
        pRcvXML = NULL;
    }
	
	return pRecSet;
}

void MinkProtocol::startElementSAX(void *ctx, 
								   const xmlChar *localname, 
								   const xmlChar *prefix, 
								   const xmlChar *URI, 
								   int nb_namespaces, 
								   const xmlChar **namespaces, 
								   int nb_attributes, 
								   int nb_defaulted, 
								   const xmlChar **attributes) 
{
	
	int i = 0;
	MinkRecordSet *pSetGroup = (MinkRecordSet*)ctx;
	
	//가장 많이 나오는 것을 사용한다.
	if(*(char*)localname == 'f')
	{
		pSetGroup->SetFieldWritterableIndex(pSetGroup->GetFieldWritterableIndex() + 1);
		pSetGroup->SetFieldWritterable(true);
		pSetGroup->IsSetFieldData(false);
	}	
	else if(strcmp((char*)localname,TAG_ROLB) == 0)
	{
		MinkRecord *pRec = new MinkRecord;
		pSetGroup->AddRecord(pRec);
		pSetGroup->SetFieldWritterableIndex(-1);
		
	}
	else if(strcmp((char*)localname,TAG_RCOLH) == 0)
	{		
		MinkRecordHeader *pNewHeaderInfo = new MinkRecordHeader();
		//memset(pNewHeaderInfo,0,sizeof(MinkRecordHeader));
		
		for(i = 0; i < nb_attributes; i++)
		{
			xmlSAX2Attributes *att = (((xmlSAX2Attributes*)attributes) + i);			
			long nLen = att->end - att->value;
			char* pAtt = NULL;
			
			if(nLen > 0) 
			{
				pAtt = new char[nLen+1];
				memset(pAtt,0, nLen+1);
				strncpy(pAtt,(const char*)att->value,nLen);
			}			
			if(pAtt && strcmp((const char*)att->localname,"name") == 0)
			{
				pNewHeaderInfo->sName = pAtt;
				pAtt = NULL;
			}
			else if(pAtt && strcmp((const char*)att->localname,"type") == 0)
			{	
				pNewHeaderInfo->sType = pAtt;
				pAtt = NULL;
			}
			else if(pAtt && strcmp((const char*)att->localname,"length") == 0)
			{	
				pNewHeaderInfo->nLength = atoi(pAtt);
			}			
			if(pAtt) { delete[] pAtt; pAtt = NULL;}
		}
		
		pSetGroup->AddInfo(pNewHeaderInfo);
	}
	else if (strcmp((char*)localname,TAG_RCMD) == 0) 
	{
		int nRefID = -1;
		int nRetCode = -1;
		for(i = 0; i < nb_attributes; i++)
		{
			xmlSAX2Attributes *att = (((xmlSAX2Attributes*)attributes) + i);			
			long nLen = att->end - att->value;
			char* pAtt = NULL;			
			if(nLen > 0) 
			{
				pAtt = new char[nLen+1];
				memset(pAtt,0, nLen+1);
				strncpy(pAtt,(const char*)att->value,nLen);
			}
			if(pAtt && strcmp((const char*)att->localname,"retcode") == 0)
			{
				nRetCode = atoi(pAtt);
			}
			else if(pAtt && strcmp((const char*)att->localname,"refid") == 0)
			{	
				nRefID = atoi(pAtt);
			}			
			if(pAtt) { delete[] pAtt; pAtt = NULL;}
		}
		
		//정상(0) 에러(<0)
		pSetGroup->SetSystemError(nRetCode);
		
		//성공일때는 그냥 스킾
		//실패일때 처리로직을 넣어야 한다.
	}	
	else if(strcmp((char*)localname, TAG_RUNIT) == 0)
	{
		MinkRecordOrg *pNewRec = new MinkRecordOrg;
		int nRefID = -1;
		int nRetCode = 0;
		for(i = 0; i < nb_attributes; i++)
		{
			xmlSAX2Attributes *att = (((xmlSAX2Attributes*)attributes) + i);			
			long nLen = att->end - att->value;
			char* pAtt = NULL;
			
			if(nLen > 0) 
			{
				pAtt = new char[nLen+1];
				memset(pAtt,0, nLen+1);
				strncpy(pAtt,(const char*)att->value,nLen);
			}
			
			//unit 의 retcode가 없을 수도 있다.
			if(pAtt && strcmp((const char*)att->localname,"retcode") == 0)
			{
				nRetCode = atoi(pAtt);
			}
			else if(pAtt && strcmp((const char*)att->localname,"refid") == 0)
			{	
				nRefID = atoi(pAtt);
				pNewRec->SetRefID(nRefID);
			}
			
			if(pAtt) { delete[] pAtt; pAtt = NULL;}
		}
		
		if(nRetCode >= 0)
		{			
			pSetGroup->AddRecordOrg(pNewRec);
			
			
			//Default는 처음 Unit 아이디로 설정을 한다.
			//Deleted By Song 2010.05.12 쩜쩜쩜
			//if(pSetGroup->GetUnitCount() == 1)
			pSetGroup->SetRefID(nRefID);
		}
		else
		{
			//에러일 경우 추가 해야 한다.
            HLogE("Error SaX\n");
		}
	}
	
	//시스템 에러 처리.(<mink> 쪽에 에러코드가 들어옴, 안들어온 경우는 정상(0) )
	else if(strcmp((char*)localname, "mink") == 0)
	{
		int nRetCode = 0;
		for(i = 0; i < nb_attributes; i++)
		{
			xmlSAX2Attributes *att = (((xmlSAX2Attributes*)attributes) + i);			
			long nLen = att->end - att->value;
			char* pAtt = NULL;
			
			if(nLen > 0) 
			{
				pAtt = new char[nLen+1];
				memset(pAtt, 0, nLen+1);
				strncpy(pAtt,(const char*)att->value,nLen);
			}
			
			//unit 의 retcode가 없을 수도 있다.
			if(pAtt && strcmp((const char*)att->localname,"retcode") == 0)
			{
				nRetCode = atoi(pAtt);
				printf("************  MINK ERROR[%d]\n", nRetCode);
				
			}
			if(pAtt) { delete[] pAtt; pAtt = NULL;}
		}
		//정상(0) 에러(<0)
		pSetGroup->SetSystemError(nRetCode);

	}
/*	
	else if(strcmp((char*)localname, "systemerror") == 0)
	{
	}
*/	
	
	
	else if(strcmp((char*)localname,TAG_RPARAM) == 0)
	{	
		//기본 결과값은 테이블 타입임.
		pSetGroup->SetResultStringType(false);
		for(i = 0; i < nb_attributes; i++)
		{
			xmlSAX2Attributes *att = (((xmlSAX2Attributes*)attributes) + i);			
			long nLen = att->end - att->value;
			char* pAtt = NULL;
			
			if(nLen > 0) 
			{
				pAtt = new char[nLen+1];
				memset(pAtt,0, nLen+1);
				strncpy(pAtt,(const char*)att->value,nLen);
			}
			
			//TAG_RPARAM type이 스트링이면
			if(pAtt && strcmp((const char*)att->localname,"type") == 0)
			{
				if(strcmp((char*)pAtt,"string") == 0)
				{
					pSetGroup->SetResultStringType(true);
				}
				
			}
			if(pAtt) { delete[] pAtt; pAtt = NULL;}
		}
	}	
	else if(strcmp((char*)localname,TAG_RCOLHS) == 0)
	{		
	}
	else if(strcmp((char*)localname, TAG_RCOLBS) == 0)
    {
        
    }
	else
	{
		HLogE("Error SaX %s\n",localname);
	}
}


void	MinkProtocol::endElementSAX(void *ctx, const xmlChar *localname, const xmlChar *prefix, const xmlChar *URI) {   
	MinkRecordSet *pSetGroup = (MinkRecordSet*)ctx;	
	if(*(char*)localname == 'f')
	{
		//</f> 로 내려 올경우에는 
		if(pSetGroup->IsFieldData() == false)
		{
			MinkRecord* pRec = pSetGroup->GetLastRec();
			pRec->msbField.push_back(new StringBuffer(""));
		}
		
		pSetGroup->SetFieldWritterable(false);
	}
/*	
	else if(strcmp((char*)localname, "systemerror") == 0)
	{
		
		printf("...............endElementSAX prefix[%s], URI[%s]\n", (char*)prefix, (char*)URI);
//		if(pSetGroup->IsFieldData() == false)
//		{
//			MinkRecord* pRec = pSetGroup->GetLastRec();
//			pRec->msbField.push_back(new StringBuffer(""));
//		}
//		
//		pSetGroup->SetFieldWritterable(false);
	}
*/ 
	
}

void	MinkProtocol::charactersFoundSAX(void *ctx, const xmlChar *ch, int len) {
	MinkRecordSet *pSetGroup = (MinkRecordSet*)ctx;	
	
	/*서버 반환 에러일 경우*/
	if (pSetGroup->GetSystemError()<0)
	{
		if(ch) {
			pSetGroup->SetSystemErrorMsg((char*)ch, len);
//			char tempBuf[1024] = {0x00,};
//			memcpy(tempBuf, ch, len);
//			LOG.error("******charactersFoundSAX [%s]", tempBuf);
		}
		
		return;
	}
	
	/*결과 형식이 스트링 타입일 경우*/
	if (pSetGroup->isResultStringType())
	{
		pSetGroup->SetResultStringMsg((char*)ch, len);
		return;
	}
	
	
	if(pSetGroup->GetFieldWritteralbe())
	{		
		MinkRecord* pRec = pSetGroup->GetLastRec();
		
		if(pRec->msbField.size() ==  pSetGroup->GetFieldWritterableIndex())
		{
			pRec->msbField.push_back(new StringBuffer((const char*)ch,len));
			pSetGroup->IsSetFieldData(true);
		}
		else
		{
			StringBuffer *sLarg = pRec->msbField[pSetGroup->GetFieldWritterableIndex()];	
			sLarg->append((const char*)ch,len);
		}
	}
}

/*
 A production application should include robust error handling as part of its parsing implementation.
 The specifics of how errors are handled depends on the application.
 */
void MinkProtocol::errorEncounteredSAX(void *ctx, const char *msg, ...) 
{
	MinkRecordSet *pSetGroup = (MinkRecordSet*)ctx;	
	
	//단순하게 파싱할때 에러가 났을 경우 -1을 셋한다.
	pSetGroup->SetParsError(-1);	
	//pSetGroup->SetLastError("Error Parse");
    HLogE("Parse ");
    //KANG
	//LOG.error("Error Parse");
}


xmlNode* MinkProtocol::Mink(const char* sVersion,const char* sUserID,const char* sPassword,const char* sDeviceID)
{
	char sXML[1024] = {0x00,};
	sprintf(sXML,"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\r\n\
			<mink session=\"%s\" version=\"%s\" deviceid=\"%s\" time=\"%d\" userid=\"%s\" password=\"%s\"></mink>",
	msSession,sVersion,sDeviceID,(unsigned int)time(NULL),sUserID,sPassword);
	
	m_pDoc = xmlReadMemory(sXML, (int)strlen(sXML),NULL, "UTF-8", 0);
	if(m_pDoc == NULL)
	{
        //KANG
//		LOG.error("%s","xmlLoadError");
		return NULL;
	}	
	xmlNode* pxnRoot = xmlDocGetRootElement(m_pDoc);
	return pxnRoot;
}

xmlNode* MinkProtocol::AddCommand(xmlNode* xnMink,const char* sClassID,int refID,const char* sTarget,bool bAutoCommit,const char* sActionID)
{
	char sRefID[10];
	xmlNode *xnCmd = NULL;
	
	xnCmd = CreateNode("cmd");
	if(xnCmd == NULL) return NULL;
	AddAttribute(xnCmd,"classid",sClassID);
	
	sprintf(sRefID,"%d",refID);
	AddAttribute(xnCmd,"refid",sRefID);
	
	if(bAutoCommit)
		AddAttribute(xnCmd,"autocommit","true");
	else
		AddAttribute(xnCmd,"autocommit","false");

	if(sTarget)
		AddAttribute(xnCmd,"target",sTarget);
	if(sActionID)
		AddAttribute(xnCmd,"actionid",sActionID);
	
	AppendChild(xnMink,xnCmd);
	
	return xnCmd;
}

#if 0
/*version test*/
xmlNode* MinkProtocol::AddVerMan(xmlNode* xnMink,const char* sInstallName,const char* sUserID,const char* sDeviceID)
{
//	char sRefID[10];
	xmlNode *xnVerMan = NULL;
	
	xnVerMan = CreateNode("verman");
	if(xnVerMan == NULL) return NULL;
	
	AddAttribute(xnVerMan,"installname",sInstallName);
	AddAttribute(xnVerMan,"userid",sUserID);
	AddAttribute(xnVerMan,"deviceid",sDeviceID);

	AppendChild(xnMink,xnVerMan);
	
	return xnVerMan;
}
#endif

//xmlNode* MinkProtocol::GetCmd(xmlNode* xnMink,int nRefID)
//{
//	//xmlNode *xnRoot = NULL,
//	xmlNode *xnCmd = NULL,
//	//*xnUnit=NULL,
//	*xcCur=NULL;
//	xmlNode *xnResult = NULL;
//	
//	if(m_pDoc == NULL) return NULL;
//	
////	//<mink>
////	xnRoot = xmlDocGetRootElement(m_pDoc);
////	
////	//<cmd>
////	xnCmd = xnRoot->children;
//	
//	
//	//<cmd>
//	xnCmd = xnMink->children;
//	
//	
//	for (xcCur = xnCmd; xcCur; xcCur = xcCur->next) 
//	{
//		if (xcCur->type == XML_ELEMENT_NODE) 
//		{
//			struct _xmlAttr *attCur = NULL;
//			struct _xmlAttr *pAtt = xcCur->properties;
//			
//			for (attCur = pAtt; attCur; attCur = attCur->next) 
//			{
//				if(strcmp("refid",(const char*) attCur->name) == 0)
//				{
//					//Attribute의 텍스트를 알아오는 예제이다.
//					char* sData = (char*)xmlNodeGetContent(attCur->children);
//					if(atoi(sData) == nRefID)
//					{
//						xmlFree(BAD_CAST sData);
//						xnResult = xcCur;
//					}
//					
//					xmlFree(BAD_CAST sData);
//				}
//			}
//			
//		}	   
//	}		
//	
//	return xnResult;	
//}
//
//bool MinkProtocol::AddFunctionWithUnit(int nCmdID,const char* sName, const char* sValue)
//{
//	xmlNode *xnMink = NULL;
//	xmlNode *xnCmd = NULL;
//	xnMink = xmlDocGetRootElement(m_pDoc);
//	xnCmd = GetCmd(xnMink,nCmdID);
//	
//	if(xnCmd == NULL) return false;
//	AddAttribute(xnCmd,sName,sValue);
//	return true;
//}

//<unit mode="execute" ;="1">
//mode="execute | fetch | function | others"
xmlNode* MinkProtocol::AddUnit(xmlNode* xnCommand,const char* strMode,int refID)
{
	char strRefID[10];
	xmlNode *xnUnit = NULL;	
	xnUnit = CreateNode("unit");
	if(xnUnit == NULL) return NULL;
	
	AddAttribute(xnUnit,"mode",strMode);
	sprintf(strRefID,"%d",refID);
	AddAttribute(xnUnit,"refid",strRefID);	
	AppendChild(xnCommand,xnUnit);
	return xnUnit;
}



//<param name="" mode="result:in:out:ref" type="void | table | long | int | short | double | binary | char | string | bool" length="" /> 
xmlNode* MinkProtocol::AddParamCDataBlock(xmlNode* xnUnit,const char* strValue,const char* strMode,const char* strType,
								const char* strName,int nLength)
{
	char strLength[10];
	xmlNode *xnParam = NULL;	
	xnParam = CreateNode("param");
	if(xnParam == NULL) return NULL;
	
    xmlNode *cdataPtr = xmlNewCDataBlock(NULL, BAD_CAST strValue, (int)strlen(strValue) );
    xmlAddChild( xnParam, cdataPtr );
	
	if(strName)			AddAttribute(xnParam,"name",strName);	
	AddAttribute(xnParam,"mode",strMode);
	AddAttribute(xnParam,"type",strType);
	sprintf(strLength,"%d",nLength);
	AddAttribute(xnParam,"length",strLength);
	AppendChild(xnUnit,xnParam);	
	return xnParam;
}

xmlNode* MinkProtocol::AddParam(xmlNode* xnUnit,const char* strValue,const char* strMode,const char* strType,
								const char* strName,int nLength)
{
	char strLength[10];
	xmlNode *xnParam = NULL;	
	xnParam = CreateNode("param");
	if(xnParam == NULL) return NULL;	
	xmlNodeSetContent(xnParam, BAD_CAST strValue);
	
	if(strName)			AddAttribute(xnParam,"name",strName);	
	AddAttribute(xnParam,"mode",strMode);
	AddAttribute(xnParam,"type",strType);
	sprintf(strLength,"%d",nLength);
	AddAttribute(xnParam,"length",strLength);
	AppendChild(xnUnit,xnParam);	
	return xnParam;
}

xmlNode* MinkProtocol::CreateNode(const char* strName,const char* strText)
{
	return CreateNode(m_pDoc,strName,strText);
}


xmlNode* MinkProtocol::CreateNode(xmlDoc *pDoc,const char* strName,const char* strText)
{
	xmlNode* newEnt;
	if(pDoc == NULL) return NULL;
	
	newEnt = xmlNewNode(NULL, BAD_CAST strName);
	if(newEnt == NULL) return NULL;
	
	if(strText)
		xmlNodeSetContent(newEnt, BAD_CAST strText);
	return newEnt;
}


xmlNode* MinkProtocol::AppendChild(xmlNode* xnParent,xmlNode* xnNew)
{
	xmlNode* addedEnt;	
	addedEnt = xmlAddChild(xnParent, xnNew);
	if(addedEnt == NULL)
		return NULL;
	return addedEnt;
}


void MinkProtocol::AddAttribute(xmlNode* xnNode,const char* strAttName,const char* strAttValue)
{
	xmlNewProp(xnNode, BAD_CAST strAttName, BAD_CAST strAttValue);
}


xmlNode* MinkProtocol::GetRUnit(xmlNode* xnRCmd,int nRefID)
{
	xmlNode *xnRoot = NULL,
			*xnCmd = NULL,
			*xnUnit=NULL,
			*xcCur=NULL;
	xmlNode *xnResult = NULL;
	
	if(m_pDoc == NULL) return NULL;
	   
	//<mink>
	xnRoot = xmlDocGetRootElement(m_pDoc);
	   
	//<cmd>
	xnCmd = xnRoot->children;
	
	   
	//<unit>
	xnUnit = xnCmd->children;
	   
	
	for (xcCur = xnUnit; xcCur; xcCur = xcCur->next) 
	{
		if (xcCur->type == XML_ELEMENT_NODE) 
		{
			struct _xmlAttr *attCur = NULL;
			struct _xmlAttr *pAtt = xcCur->properties;
			
			for (attCur = pAtt; attCur; attCur = attCur->next) 
			{
				if(strcmp("refid",(const char*) attCur->name) == 0)
				{
					//Attribute의 텍스트를 알아오는 예제이다.
					char* sData = (char*)xmlNodeGetContent(attCur->children);
					if(atoi(sData) == nRefID)
					{
						xmlFree(BAD_CAST sData);
						xnResult = xcCur;
					}
					
					xmlFree(BAD_CAST sData);
				}
			}
			
		}	   
	}		
	
	return xnResult;	
}