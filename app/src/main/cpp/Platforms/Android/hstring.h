#ifndef HSTRING_H
#define HSTRING_H

#define WID 0 //if:1 wide  if:0 ascii

#if WID
typedef	unsigned short char;

#define H_STRLEN		wcslen
#define H_VA_START		va_start
#define H_VA_END		va_end
#define H_VA_START		va_start
#define H_VSWPRINTF		vswprintf
#define H_STRCPY		wcscpy
#define H_STRCAT		wcscat
#define H_STRUPR		_wcsupr	
#define H_STRLWR		_wcslwr
#define H_STRSTR		wcsstr
#define H_STRCMP		wcscmp

#ifndef WIN32
#define _T(xx) Lxx
#endif          

#else




#define H_STRLEN		strlen
#define H_VA_START		va_start
#define H_VA_END		va_end
#define H_VA_START		va_start
#define H_VSWPRINTF		vsprintf
#define H_STRCPY		strcpy
#define H_STRCAT		strcat
#define H_STRUPR		_strupr	
#define H_STRLWR		_strlwr
#define H_STRSTR		strstr
#define H_STRCMP		strcmp


#ifndef WIN32
#define _T(xx) xx
#endif

#endif

//맥스 버퍼 사용 //Format 함수를 사용할때만
#define MAXSTRING 2048
class HString
{
public:
	HString();
	HString(const char * const);
	HString(const HString &);
	~HString();
	
	char & operator[](int offset);
	char operator[](int offset) const;
	HString & operator = (const HString &);
	HString & operator + (const HString &);
	HString & operator += (const HString &);	

	operator const char*();
//	friend HString AFXAPI operator+ (const HString& ,const HString&);			
	int GetLength() const;// { return m_Len;}
	const char * GetString() const { return m_String;}	
	
	int Format(char *first, ...);	
	void MakeUpper();
	void MakeLower();

	int Find( char ch ) const;
	int Find( const char*  lpszSub ) const;
	int Find( const char*  lpszSub, int nStart ) const;

	bool IsEmpty() const;
	void Empty();
	int Compare( char* lpsz ) const;
	int Replace( char* pOld, char* lNew );

	void TrimLeft();
	void TrimRight();
	void Trim();

	HString(int);
private:
	
	char * m_String;
	int m_Len;
};
//HString AFXAPI operator+ (const HString& ,const HString&);	

#endif


