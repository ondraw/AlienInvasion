#include "MinkRmParam.h"
RM_PARAM::RM_PARAM() {		nLength = 0;		btValue = NULL;	}
RM_PARAM::~RM_PARAM() {		if(btValue)		{			delete[] btValue;			btValue = NULL;		} }
void RM_PARAM::SetValue(const char* strValue)
{
	if(strValue == NULL) return;		
	nLength = strlen(strValue) * sizeof(char);
	btValue = (unsigned char*)new char[nLength + 1];
	strcpy((char*)btValue,strValue);
}

void RM_PARAM::SetValue(long lValue)
{
    char sTemp[40];
    sprintf(sTemp, "%ld",lValue);
    SetValue((const char*)sTemp);
}

const char* RM_PARAM::GetValueS()
{
	return (const char*)btValue;
}


long RM_PARAM::GetValueL()
{
	long lValue = 0;
	memcpy((unsigned char*)&lValue,btValue,sizeof(long));
	return lValue;
}

