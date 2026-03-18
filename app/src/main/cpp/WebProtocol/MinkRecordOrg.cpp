/*
 *  MinkRecordSet.cpp
 *  TestMnk
 *
 *  Created by 호학 송 on 10. 3. 24..
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "MinkRecordOrg.h"

MinkRecordOrg::MinkRecordOrg()
{
	mCurrent = -1;
}

MinkRecordOrg::~MinkRecordOrg()
{
	//Release Header Info.
	int nSize = (int)mvtHeaderInfo.size();
	for(int i = 0; i < nSize; i++)
	{
		MinkRecordHeader* v = (MinkRecordHeader*)mvtHeaderInfo[i];
		if(v->sName) delete[] v->sName;
		if(v->sType) delete[] v->sType;
		delete v;
	}
	mvtHeaderInfo.clear();
	
	/*위에 mvtHeaderInfo에서 지워지므로, map 데이터는 삭제할 필요 없다. */
	// it->first 는 MinkRecordHeader의 이름이다.
//	map<char*,int>::iterator it;
//	for(it = mmapColNameMap.begin(); it != mmapColNameMap.end(); it++)
//	{
//		delete[] it->first;
//	}
	mmapColNameMap.clear();
	 
	
	nSize = (int)mvtRecord.size();
	for(int i = 0; i < nSize; i++)
	{
		MinkRecord* v = mvtRecord[i];	
		int nSize2 = (int)v->msbField.size();
		for(int j = 0; j < nSize2; j++)
		{
			StringBuffer *sb = v->msbField[j];
			delete sb;
		}
		delete v;
	}
	mvtRecord.clear();	
}

int  MinkRecordOrg::GetRecCount()
{
	return (int)mvtRecord.size();
}

int MinkRecordOrg::GetColumCount()
{
	return (int)mvtRecord[mCurrent]->msbField.size();
}


bool MinkRecordOrg::MovePosition(int nIndex)
{
	mCurrent = nIndex;
	if(IsBOF() || IsEOF()) return false;
	return true;
	
}

bool MinkRecordOrg::MoveFirst()
{
	mCurrent = 0;
	if(mvtRecord.size() == 0) return false;
	return true;
}

bool MinkRecordOrg::MoveNext()
{
	mCurrent++;
	if(IsBOF() || IsEOF()) return false;
	//if(mvtRecord.size() == 0) return false;
	return true;
}


bool MinkRecordOrg::IsBOF()
{
	if(mCurrent < 0) return true;
	return false;
}

bool MinkRecordOrg::IsEOF()
{
	if(mCurrent >= mvtRecord.size()) return true;
	return false;
}

const char* MinkRecordOrg::GetData(const char* sName)
{
	
	//이상하게 find 안된다. 어디서 보니, char* 로 하면, 포인터 주소를 비교해서 안될수 있다고 함.
	//그래서  string 을 사용하라고 하는데 확인은 못함.
//	map<char*,int>::iterator it = mmapColNameMap.find((char*)sName);
	bool bFound = false;
	map<char*,int>::iterator it;
	for(it = mmapColNameMap.begin(); it != mmapColNameMap.end(); it++)
	{
		if(strcmp(sName, it->first)==0)
		{
			bFound = true;
			break;
		}
	}
	
	if(bFound)
		return GetData(it->second);
	return "";		//실패시 빈문자 리턴하는 것으로 함. 어플에서 null 체크 안해서 오류날 것임.
		
		
//	if(it != mmapColNameMap.end())	
//		return GetData(it->second);
//	return NULL;
}
const char* MinkRecordOrg::GetColumName(int nIndex)
{
	map<char*,int>::iterator it;
    bool bFound = false;
       
	for(it = mmapColNameMap.begin(); it != mmapColNameMap.end(); it++)
	{
        
		if(it->second == nIndex)
		{
			bFound = true;
			break;
		}
	}
    
    if(bFound)
		return it->first;
	return "";
}

const char* MinkRecordOrg::GetData(int nIndex)
{
	if (mvtRecord[mCurrent]->msbField.size()<=nIndex ) {
		return "";
	}
	return mvtRecord[mCurrent]->msbField[nIndex]->c_str();
}

void MinkRecordOrg::SetData(int nIndex,const char* sData)
{
	if (mvtRecord[mCurrent]->msbField.size()<=nIndex ) {
		return ;
	}
    *mvtRecord[mCurrent]->msbField[nIndex] = sData;
}




// 해더 정보를 Add 한다.
void MinkRecordOrg::AddInfo(MinkRecordHeader* pHeader)
{
	map<char*,int>::iterator it = mmapColNameMap.find(pHeader->sName);
	if(it == mmapColNameMap.end())	{	
		mmapColNameMap[pHeader->sName] = (int)mvtHeaderInfo.size();
	}
	mvtHeaderInfo.push_back(pHeader);
}

// 레코드를 Add한다.
void MinkRecordOrg::AddRecord(MinkRecord* pRec)
{
	mvtRecord.push_back(pRec);
}


