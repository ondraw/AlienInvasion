/*
 *  MinkRecordSet.cpp
 *  TestMnk
 *
 *  Created by 호학 송 on 10. 3. 24..
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#include "MinkRecordOrg.h"
#include "MinkRecordSet.h"

MinkRecordSet::MinkRecordSet()
{
	mIndexCurrentField = -1;
	mCurrent = -1;
	mbField = false;
	mbFieldData = false;
	mParsError = 0;
	mSystemErrorCode = -1000;			//초기값(정상적으로 처리될 때 0또는 0보다 큰값으로 셋팅되어야 함.
	bResTypeString = false;
	
	memset(mszErrorMsg, 0x00, MAX_ERROR_MSGLEN);
	memset(mszResString, 0x00, MAX_ERROR_MSGLEN);	
}

MinkRecordSet::~MinkRecordSet()
{
	int nSize = (int)mvtRecord.size();
	for(int i = 0; i < nSize; i++)
	{
		MinkRecordOrg* v = (MinkRecordOrg*)mvtRecord[i];
		delete v;
	}
	mvtRecord.clear();
}


int  MinkRecordSet::GetRecCount()
{
	
	if(mCurrent == -1) return 0;
	return mvtRecord[mCurrent]->GetRecCount();
}

int  MinkRecordSet::GetColumCount()
{
	
	if(mCurrent == -1) return 0;
	return mvtRecord[mCurrent]->GetColumCount();
}


bool MinkRecordSet::MovePosition(int nIndex)
{
	if(mCurrent == -1) return false;
	return mvtRecord[mCurrent]->MovePosition(nIndex);
	
}


bool MinkRecordSet::MoveFirst()
{
	if(mCurrent == -1) return false;
	
	return mvtRecord[mCurrent]->MoveFirst();
}

bool MinkRecordSet::MoveNext()
{
	if(mCurrent == -1) return false;
	
	return mvtRecord[mCurrent]->MoveNext();
}


bool MinkRecordSet::IsBOF()
{
	if(mCurrent == -1) return false;
	
	return mvtRecord[mCurrent]->IsBOF();
}

bool MinkRecordSet::IsEOF()
{
	if(mCurrent == -1) return false;
	
	
	return mvtRecord[mCurrent]->IsEOF();
}

const char* MinkRecordSet::GetData(const char* sName)
{
	if(mCurrent == -1) return NULL;
	return mvtRecord[mCurrent]->GetData(sName);
}

void MinkRecordSet::SetData(int nIndex,const char* sData)
{
	if(mCurrent == -1) return ;
    mvtRecord[mCurrent]->SetData(nIndex,sData);
}

const char* MinkRecordSet::GetColumName(int nIndex)
{
	if(mCurrent == -1) return NULL;
	return mvtRecord[mCurrent]->GetColumName(nIndex);
}


const char* MinkRecordSet::GetData(int nIndex)
{
	if(mCurrent == -1) return NULL;
	return mvtRecord[mCurrent]->GetData(nIndex);
}

void MinkRecordSet::AddRecordOrg(MinkRecordOrg* pRecord)
{
	mvtRecord.push_back(pRecord);
}

// 해더 정보를 Add 한다.
void MinkRecordSet::AddInfo(MinkRecordHeader* pHeader)
{
	if(mCurrent == -1) return ;
	mvtRecord[mCurrent]->AddInfo(pHeader);
	
}

// 레코드를 Add한다.
void MinkRecordSet::AddRecord(MinkRecord* pRec)
{
	if(mCurrent == -1) return ;
	mvtRecord[mCurrent]->AddRecord(pRec);
}


int MinkRecordSet::GetRefID()
{
	return mvtRecord[mCurrent]->GetRefID();
}

bool MinkRecordSet::SetRefID(int v)
{
	int nSize = (int)mvtRecord.size();
	mCurrent = -1;
	for(int i = 0; i < nSize; i++)
	{
		MinkRecordOrg* vOrg = (MinkRecordOrg*)mvtRecord[i];
		if(vOrg->GetRefID() == v)
		{
			mCurrent = i;
			return true;
		}
	}
	return false;
}

MinkRecord* MinkRecordSet::GetLastRec() 
{ 
	if(mCurrent == -1) return NULL;
	return mvtRecord[mCurrent]->GetLastRec();
}



void  MinkRecordSet::SetSystemErrorMsg(char* pszMsg, int len) { 
	memset(mszErrorMsg, 0x00, MAX_ERROR_MSGLEN);
	if (len>0)
	{
		if (len >= MAX_ERROR_MSGLEN)
			len = MAX_ERROR_MSGLEN - 1;
		
		memcpy(mszErrorMsg, (char*)pszMsg, len);
	}
}


void  MinkRecordSet::SetResultStringMsg(char* pszMsg, int len) { 
	memset(mszResString, 0x00, MAX_ERROR_MSGLEN);
	if (len>0)
	{
		if (len >= MAX_ERROR_MSGLEN)
			len = MAX_ERROR_MSGLEN - 1;
		
		memcpy(mszResString, (char*)pszMsg, len);
	}
}



