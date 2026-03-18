#include "hstring.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <ctype.h>

HString::HString()
{
		m_String = new char[1];
		m_String[0] = '\0';
		m_Len = 0;
}


/*-----------------------------------------------------------------------------
Function		: HString::HString
Description		: 
  (By Song)
Return type		: 
Argument		: int len
-----------------------------------------------------------------------------*/
HString::HString(int len)
{
		m_String = new char[len+1];
		//for(int i = 0; i <= len; i++)
		//		m_String[i] = '\0';
		m_String[0] = 0;
		m_Len = len;
}


/*-----------------------------------------------------------------------------
Function		: HString::HString
Description		: 
  (By Song)
Return type		: 
Argument		: const char * const cString
-----------------------------------------------------------------------------*/
HString::HString(const char * const cString)
{
	if(cString)
	{
		m_Len = (int)H_STRLEN(cString);
		m_String = new char[m_Len+1];
		H_STRCPY(m_String,cString);		
	}
	else
	{
		m_Len = 0;
		m_String = new char[1];
		m_String[0] = 0;		
	}

}


/*-----------------------------------------------------------------------------
Function		: HString::HString
Description		: 
  (By Song)
Return type		: 
Argument		: const HString & rhs
-----------------------------------------------------------------------------*/
HString::HString(const HString & rhs)
{
		m_Len = rhs.GetLength();
		m_String = new char[m_Len + 1];

		//for(int i = 0; i < m_Len; i++)
		//	m_String[i] = rhs[i];
		//m_String[m_Len] = '\0';
		H_STRCPY(m_String,rhs.GetString());		
}


/*-----------------------------------------------------------------------------
Function		: HString::~HString
Description		: 
  (By Song)
Return type		: 
-----------------------------------------------------------------------------*/
HString::~HString()
{
		delete[] m_String;
		m_Len = 0;
}


/*-----------------------------------------------------------------------------
Function		: HString::operator=
Description		: 
  (By Song)
Return type		: HString& 
Argument		: const HString & rhs
-----------------------------------------------------------------------------*/
HString& HString::operator=(const HString & rhs)
{
		if(this == &rhs)
				return * this;
		delete[] m_String;
		m_Len = rhs.GetLength();
		m_String = new char [m_Len+1];
		
		//for(int i = 0; i < m_Len; i++)
		//		m_String[i] = rhs[i];
		//m_String[m_Len] = '\0';

		H_STRCPY(m_String,rhs.GetString());		
		return *this;
}


/*-----------------------------------------------------------------------------
Function		: +
Description		: 
  (By Song)
Return type		: HString& HString::operator 
Argument		: const HString & rhs
-----------------------------------------------------------------------------*/
HString& HString::operator +(const HString & rhs)
{
	int len = rhs.GetLength();
	m_Len += len;
	char * temp = new char[m_Len+1];
	H_STRCPY(temp,m_String);
	H_STRCAT(temp,rhs.GetString());
	delete[] m_String;
	m_String = temp;
	return *this;
}


/*-----------------------------------------------------------------------------
Function		: +=
Description		: 
  (By Song)
Return type		: HString& HString::operator 
Argument		: const HString & rhs
-----------------------------------------------------------------------------*/
HString& HString::operator +=(const HString & rhs)
{
//	*this = *this + rhs;
//	return *this;

	m_Len += rhs.m_Len;
	char* pNew = new char[m_Len + 1];
	//ASSERT(pNew != 0);
	strcpy(pNew, m_String);
	strcat(pNew, rhs.m_String);
	delete[] m_String;
	m_String = pNew;

	return *this;

}


/*-----------------------------------------------------------------------------
Function		: 
Description		: 
  (By Song)
Return type		: HString operator+ 
Argument		: const HString& lhs
Argument		: const HString& rhs
-----------------------------------------------------------------------------*/
//HString AFXAPI operator+ (const HString& lhs,const HString& rhs)
//{
//	int totalLen = lhs.GetLength() + rhs.GetLength();
//	HString temp(totalLen);
//	
//	H_STRCPY((char*)temp.GetString(),lhs.GetString());
//	H_STRCAT((char*)temp.GetString(),rhs.GetString());
//	return temp;
//}


/*-----------------------------------------------------------------------------
Function		: HString::operator[]
Description		: 
  (By Song)
Return type		: char & 
Argument		: int offset
-----------------------------------------------------------------------------*/
char & HString::operator[](int offset)
{
		if(offset > m_Len)
				return m_String[m_Len-1];
		else
				return m_String[offset];
}


/*-----------------------------------------------------------------------------
Function		: HString::operator[]
Description		: 
  (By Song)
Return type		: char 
Argument		: int offset
-----------------------------------------------------------------------------*/
char HString::operator[](int offset) const
{
		if(offset > m_Len)
				return m_String[m_Len-1];
		else 
				return m_String[offset];
}


/*-----------------------------------------------------------------------------
Function		: HString::Formate
Description		: 버퍼가 MAXSTRING이 엄어갈경우 는 에러가 난다.
  (By Song)
Return type		: void 
Argument		: char *first
Argument		: ...
-----------------------------------------------------------------------------*/
int HString::Format(char *first,...)
{

	int len = 0;
	char temp[MAXSTRING];
	va_list marker;

	delete[] m_String;
	m_Len = 0; 
  
	#ifdef SUN_PORT
	H_VA_START(marker);
	#else
	H_VA_START(marker,first);
	#endif

	memset(temp,0,sizeof(char)*MAXSTRING);
	H_VSWPRINTF(temp,first,marker);  
	H_VA_END(marker);  
	m_Len = (int)H_STRLEN(temp);
	m_String = new char[m_Len+1];
	H_STRCPY(m_String,temp); 
	/*
	ASSERT(first != 0);

	int len;
	char* MaxBuf;

	va_list marker;
	H_VA_START(marker,first);

	for(int i = 5; ; i ++)
	{
		len = (int)pow(2, i);
		MaxBuf = new char[len+1];
		if (!MaxBuf) return 0;
		// some UNIX's do not support vsnprintf and snprintf
		//len = _vsnprintf(MaxBuf, len, first, (char*)(&first + 1));
		len = _vsnprintf(MaxBuf, len, first, marker);
		if (len > 0) break;
		delete []MaxBuf;
		if (len == 0) return 0;
	}
	H_VA_END(marker);  

	if (!m_String)
	{
		m_Len = len;
		m_String = new char[m_Len + 1];
	}
	else if (m_Len < len)
	{
		delete m_String;
		m_Len = len;
		m_String = new char[m_Len + 1];
	}
	if (m_String) 
		strcpy(m_String, MaxBuf);
	else
		len = 0;
	delete []MaxBuf;
	*/
	return len;
}



/*-----------------------------------------------------------------------------
Function		: char*
Description		: 
  (By Song)
Return type		: HString::operator const 
-----------------------------------------------------------------------------*/
HString::operator const char*()
{
	return GetString();
}


/*-----------------------------------------------------------------------------
Function		: HString::MakeUpper
Description		: 
  (By Song)
Return type		: void 
-----------------------------------------------------------------------------*/
void HString::MakeUpper()
{	
#ifdef WIN32
	m_String = H_STRUPR(m_String);
#else
   int len = (int)strlen(m_String);
   for(int i = 0; i < len; i++)
   {
       if(m_String[i] >= 'a' && m_String[i] <= 'z')
       {
           m_String[i] -= 32;
       }
   }
#endif
}


/*-----------------------------------------------------------------------------
Function		: HString::MakeLower
Description		: 
  (By Song)
Return type		: void 
-----------------------------------------------------------------------------*/
void HString::MakeLower()
{	
#ifdef WIN32
   m_String = H_STRLWR(m_String);
#else
   int len = (int)strlen(m_String);
   for(int i = 0; i < len; i++)
   {
       if(m_String[i] >= 'A' && m_String[i] <= 'Z')
        {
             m_String[i] += 32;
         }
    }
#endif
}


/*-----------------------------------------------------------------------------
Function		: HString::Find
Description		: 
  (By Song)
Return type		: int 
Argument		: char ch
-----------------------------------------------------------------------------*/
int HString::Find(char ch) const
{
	int result = -1;

	for(int i = 0; i < m_Len; i++)
	{
		if(ch == m_String[i])
		{
			result = i;
			break;
		}		
	}
	return result;
}


/*-----------------------------------------------------------------------------
Function		: HString::Find
Description		: 
  (By Song)
Return type		: int 
Argument		: const char*  lpszSub
-----------------------------------------------------------------------------*/
int HString::Find( const char*  lpszSub ) const
{
	char * temp;
	int result = -1;
	temp = H_STRSTR(m_String,lpszSub);
	if(temp)
	{
		result = (int)(temp - m_String);
	}
	return result;
}


/*-----------------------------------------------------------------------------
Function		: HString::Find
Description		: 
  (By Song)
Return type		: int -1은 찾지 못했다.
Argument		:  const char*  lpszSub
Argument		: int nStart
-----------------------------------------------------------------------------*/
int HString::Find( const char*  lpszSub, int nStart ) const
{
	char * temp;
	int result = -1;

	if(nStart >= m_Len)
		return result;

	temp = H_STRSTR(m_String+nStart,lpszSub);
	if(temp)
	{
		result = (int)(temp - m_String);
	}

	return result;
}

void HString::TrimLeft()
{
   char* lpsz;
   char* lptmp;
   if(IsEmpty()) return ;
   lpsz = m_String;
#ifdef WIN32
   while (_istspace(*lpsz))
      lpsz = _tcsinc(lpsz);
#else
   while (isspace(*lpsz))
       lpsz += 1;
#endif

   m_Len = (int)H_STRLEN(lpsz);
   lptmp = new char[m_Len+1];
   H_STRCPY(lptmp,lpsz);
   delete[] m_String;
   m_String = lptmp;
}

void HString::TrimRight()
{
   char* lpsz = m_String;
   char* lpszLast = NULL;
   if(IsEmpty())
      return ;
   while (*lpsz != '\0')
   {
#ifdef WIN32
      if (_istspace(*lpsz))
#else
      if (isspace(*lpsz))
#endif
      {
         if (lpszLast == NULL)
            lpszLast = lpsz;
      }
      else
         lpszLast = NULL;
#ifdef WIN32
      lpsz = _tcsinc(lpsz);
#else
      lpsz += 1;
#endif
   }

   if (lpszLast != NULL)
   {
      // truncate at trailing space start
      *lpszLast = '\0';
      m_Len = (int)(lpszLast - m_String);
   }
}

void HString::Trim()
{
	TrimLeft();
	TrimRight();
}

/*-----------------------------------------------------------------------------
Function		: HString::IsEmpty
Description		: 
  (By Song)
Return type		: bool 
-----------------------------------------------------------------------------*/
bool HString::IsEmpty() const
{
	if(m_Len == 0)
		return true;
	return false;
}


/*-----------------------------------------------------------------------------
Function		: HString::Empty
Description		: 
  (By Song)
Return type		: void 
-----------------------------------------------------------------------------*/
void HString::Empty()
{
	m_Len = 0;
	delete[] m_String;
	m_String = NULL;
}



/*-----------------------------------------------------------------------------
Function		: HString::Compare
Description		: 
  (By Song)
Return type		: int 
Argument		: char* lpsz
-----------------------------------------------------------------------------*/
int HString::Compare( char* lpsz ) const
{
	return H_STRCMP(m_String,lpsz);
}


/*-----------------------------------------------------------------------------
Function		: HString::Replace
Description		: 
  (By Song)
Return type		: int  변경된 갯수
Argument		:  char* pOld
Argument		: char* lNew
-----------------------------------------------------------------------------*/
int HString::Replace( char* pOld, char* lNew )
{
	int inter=0;
	int cnt=0;
	int pos=0;
	char* prev;
	char* pfor;
	int lNewLen = (int)H_STRLEN(lNew),
		foreLen =0;
	int lOldLen = (int)H_STRLEN(pOld);


	if(*pOld == *lNew && *pOld == *(lNew+1))
		inter +=2;
		

	for(int i = 0; i < m_Len; )
	{
		if((pos = Find(pOld,pos)) != -1)
		{				
			prev = new char[pos];
			memcpy(prev,m_String,sizeof(char)*pos);
			
			foreLen = (int)H_STRLEN(m_String+pos+lOldLen);
			pfor = new char[foreLen+1];			
			H_STRCPY(pfor,m_String+pos+lOldLen);

			delete[] m_String;
			m_Len = pos+lNewLen+foreLen;			

			m_String = new char[m_Len+1];			
			memcpy(m_String,prev,sizeof(char)*pos);
			memcpy(m_String+pos,lNew,sizeof(char)*lNewLen);
			memcpy(m_String+pos+lNewLen,pfor,foreLen+1);

			delete[] prev;
			delete[] pfor;
			pos+= inter;
			i = pos;
			cnt++;
			
			continue;
		}
		else
			break;
	}
	return cnt;
}
int HString::GetLength() const
{
	return m_Len;
}
//bool __stdcall operator==(const HString& s1, const HString& s2)
//{ 
//	return s1.Compare((char*)s2.GetString()) == 0; 
//}







