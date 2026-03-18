#ifndef _MINKRMPARAM_H
#define _MINKRMPARAM_H


#include <string>
#include <vector>
//#include <map.h>
//#include "base/util/StringBuffer.h"
using namespace std;

class RM_PARAM 
{
public:
	RM_PARAM();
	~RM_PARAM();
	
public:
	long		   nLength; //
	unsigned char  *btValue; 
	string strMode; //in,out,result
	string strType; //int,String	
	string strName; //

public:

	void SetValue(const char* strValue);
	void SetValue(long lValue);
	const char* GetValueS();
	long GetValueL();
};
#endif //_MINKRMPARAM_H