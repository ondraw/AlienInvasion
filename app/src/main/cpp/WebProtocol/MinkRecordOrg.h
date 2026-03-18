/*
 *  MinkRecordSet.h
 *  TestMnk
 *
 *  Created by 호학 송 on 10. 3. 24..
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _MINKRECORD_ORG_H
#define _MINKRECORD_ORG_H
#include <string>
#include <vector>
#include <map>
//#include <list.h>
#include "StringBuffer.h"

using namespace std;
class MinkRecordHeader 
{
public:
	MinkRecordHeader() { 
		sName = NULL;
		nLength = 0;
		sType = NULL;
	}
	char* sName;
	int   nLength;
	char* sType;
	
};

class MinkRecord
{
public:
	vector<StringBuffer*> msbField;
};

class MinkRecordOrg {

public:
	MinkRecordOrg();
	~MinkRecordOrg();
	
public:
	//Control Record
	bool MoveFirst();
	bool MoveNext();
	bool MovePosition(int nIndex);
	int  GetRecCount();
    
    /*Add Kang*/
    int  GetColumCount();
    const char* GetColumName(int nIndex);
	
	bool IsBOF();
	bool IsEOF();
	const char* GetData(const char* sName);
	const char* GetData(int nIndex);
    void SetData(int nIndex,const char* sData);
	
	//Get Command Identified 
	int GetRefID() { return mRefID;}
	//Set Command Identified
	void SetRefID(int v) { mRefID = v;}
	
	void AddInfo(MinkRecordHeader* pHeader);
	void AddRecord(MinkRecord* pRec);
	MinkRecord* GetLastRec() { return mvtRecord.back();}
	
protected:
	int mCurrent;
	int mRefID;
	vector<MinkRecordHeader*> mvtHeaderInfo;
	map<char*,int> mmapColNameMap;
	vector<MinkRecord*> mvtRecord;
};

#endif // _MINKRECORD_ORG_H