#ifndef _MINKCONNECTOR_H
#define _MINKCONNECTOR_H



//#include "base/fscapi.h"
//#include "base/globalsdef.h"
#include <list>
#include "MinkProtocol.h"
#include "MinkRmParam.h"



#define MINKPROTOCALCMDREFID  1
#define MINKPROTOCALCLASSID  "minkModule"
#define MINKPROTOCALVERSION  "1.1"
	
class MinkRecordSet;
class MinkConnector : public MinkProtocol
{
public:
	MinkConnector(
		const char* strUserID,
		const char* strPassword,
		const char* strDeviceID,
		const char* strURL,
		const char* strTarget,HttpRequestXml pSendFunction);
	virtual ~MinkConnector(void);

public:
	bool InitInfo(bool bAutoCommit = true);
	bool InitInfo(const char* strClsId, const char* strTarget, bool bAutoCommit = false);
	bool AddExecute(int nUnitRefID,const char* strSQL);
	bool AddFetch(int nUnitRefID,const char* strSQL);	
	void AddCmdExtendAttInfo(const char* sName, const char* sValue);
	bool AddFunction(int nUnitRefID,const char* strFunctionName,std::list<RM_PARAM*>* pPARAMList);
	bool AddFunctionCDataBlock(int nUnitRefID,const char* strFunctionName,std::list<RM_PARAM*>* pPARAMList);
//libcurl 사용안하면서, GetReqMessage, GetRespRecordSet 로 대체한다. 	
	int  Execute(MinkRecordSet** outResult);		
	bool Fetch(const char* strSQL,MinkRecordSet** outResult);
	int  Execute(const char* strSQL,MinkRecordSet** outResult,bool bAutoCommit = false);	
	int  Function(const char* strClsId, const char* strTarget,const char* strFunctionName,std::list<RM_PARAM*>* pPARAMList,MinkRecordSet** outResult,bool bAutoCommit = false);	
	
	void SendClear();
	bool AppendResult();
	xmlNode* GetCmd() { return _sndCmd;}
	xmlNode* GetMink() { return _sndMink;}
	
	char* GetServiceURL()
	{
		return msURL;
	}
		
	/*req xml 문자열 반환함수 추가 add KANG (2010.10.13)
	// return null 이면, 에러로 판단..기존 에러 처리 수정 작업 해야함..
	// return char* 사용후 메모리 해제 할것...*/
	char* GetReqMessage(bool bIsEnc, int* nOrgContentsLen, int* nReqBodyContentsLen);
	
	/*req xml 문자열 반환함수 추가 add KANG (2010.10.13)
	 // return 값이 <0 에러로 처리..기존 에러처리 로직 수정 필요함.
	*/
	int GetRespRecordSet(bool bIsEnc, int nReceived, int nUnCompLen, char* respMsg, MinkRecordSet** outResult);

	
	
#if 0
	/*version test*/
	bool InitVerManager(const char* strClsId, const char* strTarget,bool bAutoCommit
									   ,const char* sInstallName, const char* sUserID, const char* sDeviceID );
#endif
	
protected:
	xmlNode* _sndMink;
	xmlNode* _sndCmd;
	char *_strTarget;
	char *_strUserID;
	char *_strPassword;
	char *_strDeviceID;

	
};


#endif //_MINKCONNECTOR_H