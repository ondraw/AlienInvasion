/*
 *  MinkProtocol.h
 *  TestMnk
 *
 *  Created by 호학 송 on 10. 3. 23..
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _MINKPROTOCAL_H
#define _MINKPROTOCAL_H
#include <libxml/parser.h>
#include "sGLDefine.h"

//XML을 HTTP로 보내준다.. url,post
typedef char*  (*HttpRequestXml)(char*,char*);

typedef struct _xmlSAX2Attributes
{
	const xmlChar* localname;
	const xmlChar* prefix;
	const xmlChar* uri;
	const xmlChar* value;
	const xmlChar* end;
} xmlSAX2Attributes;

class MinkRecordSet;
class MinkProtocol {
public:
	MinkProtocol(const char* strURL,int nResponseTimeOut,HttpRequestXml pSendFunction);
	virtual ~MinkProtocol();
	
	virtual MinkRecordSet* LoadXML(char* strXML);
	virtual MinkRecordSet*  SendData();	
	
	//static function
	static void startElementSAX(void *ctx, 
								   const xmlChar *localname, 
								   const xmlChar *prefix, 
								   const xmlChar *URI, 
								   int nb_namespaces, 
								   const xmlChar **namespaces, 
								   int nb_attributes, 
								   int nb_defaulted, 
								   const xmlChar **attributes);
								   
	static void	endElementSAX(   void *ctx, 
								 const xmlChar *localname, 
								 const xmlChar *prefix, 
								 const xmlChar *URI);
	static void	charactersFoundSAX(void *ctx, const xmlChar *ch, int len);	
	static void errorEncounteredSAX(void *ctx, const char *msg, ...);
	
	
	
public:
	xmlNode* Mink(const char* sVersion,const char* sUserID,const char* sPassword,const char* sDeviceID);
	xmlNode* AddCommand(xmlNode* xnMink,const char* sClassID,int refID,const char* sTarget,bool bAutoCommit = true,const char* sActionID = NULL);
//	xmlNode* GetCmd(xmlNode* xnMink,int nRefID);
//	bool AddFunctionWithUnit(int nCmdID,const char* sName, const char* sValue);
	xmlNode* AddUnit(xmlNode* xnCommand,const char* strMode,int refID);
	xmlNode* AddParam(xmlNode* xnUnit,const char* strValue,const char* strMode = NULL,const char* strType = NULL,const char* strName = NULL,int nLength = 0);
    
    /* Kang CData Block으로 값 셋팅 추가*/
    xmlNode* AddParamCDataBlock(xmlNode* xnUnit,const char* strValue,const char* strMode = NULL,const char* strType = NULL,const char* strName = NULL,int nLength = 0);
    
	xmlNode* CreateNode(const char* strName,const char* strText = NULL);
	xmlNode* CreateNode(xmlDoc *pDoc,const char* strName,const char* strText);
	xmlNode* AppendChild(xmlNode* xnParent,xmlNode* xnNew);
	void AddAttribute(xmlNode* xnNode,const char* strAttName,const char* strAttValue);
	xmlNode* GetRUnit(xmlNode* xnRCmd,int nRefID);

#if 0
	/*version test*/
	xmlNode* AddVerMan(xmlNode* xnMink,const char* sInstallName,const char* sUserID,const char* sDeviceID);
#endif	

public:
	void SetLastError(const char* sLastError);
	const char* GetLastError();
    /*마이더스를 위해 추가 응답데이터를 복호화 압축해제하여 그냥 반환*/
    static char* decryptionMinkProtocol(int nReceived, int nUnCompLen, char* recvBuf);
	
protected:

	xmlDoc *m_pDoc;
	char	msSession[50];
	char   *msLastError;
	char   *msURL;
	int     mnResponseTimeOut;
	
	char*  GetReqMessageMinkProtocol(bool bIsEnc, int* nOrgContentsLen, int* nReqBodyContentsLen);
	MinkRecordSet* GetRespMessageMinkProtocol(bool bIsEnc, int nReceived, int nUnCompLen, char* recvBuf);
    HttpRequestXml mpSendFunction;
    
};

#endif