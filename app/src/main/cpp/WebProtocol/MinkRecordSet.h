/*
 *  MinkRecordSet.h
 *  TestMnk
 *
 *  Created by 호학 송 on 10. 3. 24..
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _MINKRECORD_SET_H
#define _MINKRECORD_SET_H
#include <string>
#include <vector>

#define MAX_ERROR_MSGLEN		128

using namespace std;
class MinkRecordOrg;
class MinkRecordHeader;
class MinkRecord;
class MinkRecordSet{

public:
	MinkRecordSet();
	~MinkRecordSet();
	
public:
	//Control Record
	int  GetRecCount();	
    int  GetColumCount();
    
	bool MovePosition(int nIndex);
	bool MoveFirst();
	bool MoveNext();
	
	bool IsBOF();
	bool IsEOF();
	
	const char* GetData(const char* sName);
	const char* GetData(int nIndex);
    void SetData(int nIndex,const char* sData);
    const char* GetColumName(int nIndex);
	
	//Get Command Identified 
	int GetRefID();
	//Set Command Identified
	bool SetRefID(int v);
	
	void AddRecordOrg(MinkRecordOrg* pRecord);
	void AddInfo(MinkRecordHeader* pHeader);
	void AddRecord(MinkRecord* pRec);
	
	
	//파싱할때 사용하기 위해서
	bool GetFieldWritteralbe() { return mbField;}
	void SetFieldWritterable(bool v) { mbField = v;}	
	
	int GetFieldWritterableIndex() { return mIndexCurrentField;}
	void SetFieldWritterableIndex(int v) { mIndexCurrentField = v;}
	MinkRecord* GetLastRec();
	
	int GetParsError() { return mParsError;}
	void SetParsError(int v) { mParsError = v;}
	
	
	bool IsFieldData() { return mbFieldData;}
	void IsSetFieldData(bool v) { mbFieldData = v;}
	
	int GetUnitCount() { return (int)mvtRecord.size();}
	
	
	
	/*서버 시스템 에러 처리(mink 태그 쪽에 시스템 에러)*/
	int GetSystemError() { return mSystemErrorCode;}
	void SetSystemError(int v) { mSystemErrorCode = v;}
	
	char* GetSystemErrorMsg() { return mszErrorMsg; }
	void  SetSystemErrorMsg(char* pszMsg, int len);
	
	
	/*결과가 스트링 타입일 경우 데이터 얻어오기.*/
	bool isResultStringType() { return bResTypeString;}
	void SetResultStringType(bool v) { bResTypeString = v;}
	
	char* GetResultStringMsg() { return mszResString; }
	void  SetResultStringMsg(char* pszMsg, int len);
	
	
protected:	
	int mCurrent;	
	vector<MinkRecordOrg*> mvtRecord;
	
	
	//파싱할때 사용하기 위해서
	bool mbField;
	bool mbFieldData;
	int mIndexCurrentField;
	int mParsError;
	
	
	//결과 데이터가 스트링이 타입.
	bool bResTypeString;
	char mszResString[MAX_ERROR_MSGLEN];
	
	//시스템 에러
	int mSystemErrorCode;
	char mszErrorMsg[MAX_ERROR_MSGLEN];
};

#endif // _MINKRECORD_SET_H