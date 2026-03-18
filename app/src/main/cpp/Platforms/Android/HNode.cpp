// HNode.cpp: implementation of the HNode class.
//
//////////////////////////////////////////////////////////////////////


#include "HNode.h"
#include "sGLDefine.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>

#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#include <stdio.h>
#ifndef _WIN32
#define __GetBuffer(xxx) xxx.GetBuffer()
#define __Message		HTRACE0
#else
#define __GetBuffer(xxx) (const char*)xxx
#define __Message
#endif


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define TRACE0

static const char chXMLTagOpen		= '<';
static const char chXMLTagClose	= '>';
static const char chXMLTagPre		= '/';
static const char chXMLEscape		= 0;	// for value field escape


// default entity table
static const XENTITY x_EntityTable[] = {
		{ '&', _T("&amp;"), 5 } ,
		{ '\"', _T("&quot;"), 6 } ,
		{ '\'', _T("&apos;"), 6 } ,
		{ '<', _T("&lt;"), 4 } ,
		{ '>', _T("&gt;"), 4 } 
	};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

char *strpbrk(const char* s1,const char* s2)
{
	const char *scanp;
	int c, sc;
    
	while ((c = *s1++) != 0) {
		for (scanp = s2; (sc = *scanp++) != 0;)
			if (sc == c)
				return ((char *)(s1 - 1));
	}
	return (NULL);
}


//========================================================
// Name   : _tcschrs
// Desc   : same with _tcspbrk 
// Param  :
// Return :
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2002-10-29
//========================================================
char* _tcschrs( const char* psz, const char* pszchs )
{
	while( psz && *psz )
	{
		if( strchr( pszchs, *psz ) )
			return (char*)psz;
		psz++;
	}
	return NULL;
}

//========================================================
// Name   : _tcsskip
// Desc   : skip space
// Param  : 
// Return : skiped string
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2002-10-29
//========================================================
char* _tcsskip( const char* psz )
{
	while( psz && *psz == ' ' ) psz++;
		
	return (char*)psz;
}

//========================================================
// Name   : _tcsechr
// Desc   : similar with _tcschr with escape process
// Param  : escape - will be escape character
// Return : 
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2002-10-29
//========================================================
char* _tcsechr( const char* psz, int ch, int escape )
{
	char* pch = (char*)psz;
	while( pch && *pch )
	{
		if( *pch == escape )
			pch++;
		else
		if( *pch == ch )
			return (char*)pch;
		pch++;
	}
	return pch;
}

//========================================================
// Name   : _tcselen
// Desc   : similar with _tcslen with escape process
// Param  : escape - will be escape character
// Return : 
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2002-10-29
//========================================================
int _tcselen( int escape, const char* srt, char* end = NULL ) 
{
	int len = 0;
	char* pch = (char*)srt;
	if( end==NULL ) end = (char*)sizeof(long);
	while( pch && *pch && pch<end )
	{
		if( *pch != escape )
			len++;
		pch++;
	}
	return len;
}

//========================================================
// Name   : _tcsecpy
// Desc   : similar with _tcscpy with escape process
// Param  : escape - will be escape character
// Return : 
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2002-10-29
//========================================================
void _tcsecpy( char* psz, int escape,const char* srt, char* end = NULL )
{
	char* pch = (char*)srt;
	if( end==NULL ) end = (char*)sizeof(long);
	while( pch && *pch && pch<end )
	{
		if( *pch != escape )
			*psz++ = *pch;
		pch++;
	}

	*psz = '\0';
}

//========================================================
// Name   : _tcsepbrk
// Desc   : similar with _tcspbrk with escape process
// Param  : escape - will be escape character
// Return : 
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2002-10-29
//========================================================
char* _tcsepbrk( const char* psz, const char* chset, int escape )
{
	char* pch = (char*)psz;
   while( pch && *pch )
   {
      if( *pch == escape )
         pch++;
      else
#ifdef WIN32
      if( _tcschr( chset, *pch ) )
#else
      if(strchr(chset,* pch))
#endif
         return (char*)pch;

      pch++;
   }
   return pch;
}

//========================================================
// Name   : _SetString
// Desc   : put string of (psz~end) on ps string
// Param  : trim - will be trim?
// Return : 
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2002-10-29
//========================================================
void _SetString( const char* psz, char* end, HString* ps, bool trim = false, int escape = 0 )
{
	//trim
	if( trim )
	{
		while( psz && psz < end && (*psz == ' ' || *psz == 9) ) psz++;
		while( (end-1) && psz < (end-1) && (*(end-1) == ' ' || *(end-1) == 9 ) ) end--;
	}
	int len = (int)(end - psz);
	if( len <= 0 ) return;
	if( escape )
	{
		len = _tcselen( escape, psz, end );
		char* pss = new char[len+1];
		_tcsecpy( pss, escape, psz, end );
		*ps = pss;
		delete[] pss;
	}
	else
	{
		char* pss = new char[len+1];
		memcpy( pss, psz, len );
		pss[len] = '\0';
		*ps = pss;
		delete[] pss;
	}
}

LPXENTITY GetEntity( char entity )
{
	for( int i = 0 ; i < sizeof(x_EntityTable)/sizeof(x_EntityTable[0]); i ++ )
	{
		if( x_EntityTable[i].entity == entity )
			return LPXENTITY(&x_EntityTable[i]);
	}
	return NULL;
}

LPXENTITY GetEntity( char* entity)
{
	for( int i = 0 ; i < sizeof(x_EntityTable)/sizeof(x_EntityTable[0]); i ++ )
	{
		char* ref = (char*)x_EntityTable[i].ref;
		char* ps = entity;
		while( ref && *ref )
			if( *ref++ != *ps++ )
				break;
		if( ref && !*ref )	// found!
			return LPXENTITY(&x_EntityTable[i]);
	}
	return NULL;
}

int GetEntityCount( const char* str )
{
	int nCount = 0;
	char* ps = (char*)str;
	while( ps && *ps )
		if( GetEntity( *ps++ ) )
			nCount ++;
	return nCount;
}



int Ref2Entity( const char* estr, char* str, int strlen )
{
	char* pes = (char*)estr;
	char* ps = str;
	char* ps_end = ps+strlen;
	while( pes && *pes && ps < ps_end )
	{
		LPXENTITY ent = GetEntity( pes );
		if( ent )
		{
			// copy entity meanning char
			*ps = ent->entity;
			pes += ent->ref_len;
		}
		else
			*ps = *pes++;	// default character copy
		ps++;
	}
	*ps = '\0';
	
	// total copied characters
	return (int)(ps-str);
}

int Entity2Ref( const char* str, char* estr, int estrlen )
{
	char* ps = (char*)str;
	char* pes = (char*)estr;
	char* pes_end = pes+estrlen;
	while( ps && *ps && pes < pes_end )
	{
		LPXENTITY ent = GetEntity( *ps );
		if( ent )
		{
			// copy entity string
			char* ref = (char*)ent->ref;
			while( ref && *ref )
				*pes++ = *ref++;
		}
		else
			*pes++ = *ps;	// default character copy
		ps++;
	}
	*pes = '\0';
	
	// total copied characters
	return (int)(pes-estr);
}


HString Ref2Entity( const char* estr )
{
	HString Temp;
	char* esbuf = NULL;
	if( estr )
	{
		int len = (int)H_STRLEN(estr);
		esbuf = new char[len + 1];
		if( esbuf )
			Ref2Entity( estr, esbuf, len );

		Temp = esbuf;
		delete[] esbuf;
	}
	return Temp;
}

HString Entity2Ref( const char* str )
{	
	HString Temp;
	char* sbuf = NULL;
	if( str )
	{
		int nEntityCount = GetEntityCount(str);
		if( nEntityCount == 0 )
			return HString(str);
		int len = (int)H_STRLEN(str) + nEntityCount*10 ;
		sbuf = new char[len + 1];
		if( sbuf )
			Entity2Ref( str, sbuf, len );

		Temp = sbuf;
		delete[] sbuf;
	}
	return Temp;
}


bool _FindTool(char* strIN,char* strFnd,HString& OutString,int& OutNextPos)
{
 int pos;
   char* pstrTmp = NULL;

#ifdef WIN32
   pstrTmp = _tcsstr(strIN,strFnd);
#else
   pstrTmp = strstr(strIN,strFnd);
#endif

   if(pstrTmp)
   {
      pos = (int)(pstrTmp - strIN);
      pstrTmp = new char[pos+1];
#ifdef WIN32
      _tcsncpy(pstrTmp,strIN,pos);
#else
      strncpy(pstrTmp,strIN,pos);
#endif
      *(pstrTmp+pos) = 0;
      OutString = pstrTmp;
      OutNextPos = (int)(pos+strlen(strFnd));
      delete[] pstrTmp;
   }
   else
   {
      OutString = strIN;
      OutNextPos = 0; //¥ı¿ÃªÛ µ⁄ø°πÆ¿⁄∞° æ¯¥Ÿ.
   }
   return true;
}

bool _FindAtt(char* strIN,HString& Name,HString& AttName,HString& AttVal)
{

	HString strTmp;
	int Pos = 0,TotalPos = 0;;
	_FindTool(strIN,(char*)_T("[@"),Name,Pos);

	if(Pos == 0)
		return true;

	TotalPos = Pos;

	_FindTool(strIN+TotalPos,(char*)_T("="),AttName,Pos);
	if(Pos == 0)
		return false;
	TotalPos += Pos;
	_FindTool(strIN+TotalPos,(char*)_T("]"),AttVal,Pos);
	
	if(AttVal.IsEmpty())
		return false;

	strTmp = AttVal;
	if(strTmp[0] == _T('\''))
	{
		_FindTool((char*)(const char*)strTmp+1,(char*)_T("\'"),AttVal,Pos);
	}
	else
		return false;
	return true;
}

bool _FindeP(char* strIN,char** strOut,HString& NodeName,HString& AttName,HString& AttValue,
	   int& Type)
{
	HString strTmpNodeName;
//	char *pstrTmp=NULL;
	int    pos=0,FindPos = 0;
	if(H_STRCMP(_T("/"),strIN) == 0)
	{
		//Root
		Type = -1;
		return true;
	}
	int cnt = (int)strlen(strIN);
	if(cnt < 3)
		return false;
	if(strIN[0] == _T('/') && strIN[1] == _T('*') && strIN[2] == _T('/'))
	{		
		pos = 3;
		_FindTool(strIN+pos,(char*)_T("/"),NodeName,FindPos);
		if(FindPos == 0)
			*strOut = NULL;
		else
		{
			*strOut = strIN+pos+FindPos-1;
		}
		if(!NodeName.IsEmpty())
		{
			strTmpNodeName = NodeName;
			_FindAtt((char*)(const char*)strTmpNodeName,NodeName,AttName,AttValue);
		}
		Type = 500000;//
		return true;
	}
	else if(strIN[0] == _T('/')  && strIN[1] == _T('/'))
	{		
		pos = 2;
		_FindTool(strIN+pos,(char*)_T("/"),NodeName,FindPos);
		if(FindPos == 0)
			*strOut = NULL;
		else
		{
			*strOut = strIN+pos+FindPos-1;
		}
		if(!NodeName.IsEmpty())
		{
			strTmpNodeName = NodeName;
			_FindAtt((char*)(const char*)strTmpNodeName,NodeName,AttName,AttValue);
		}
		Type = 1;//
		return true;
	}
	else
	{		
		pos = 1;
		_FindTool(strIN+pos,(char*)_T("/"),NodeName,FindPos);
		if(FindPos == 0)
			*strOut = NULL;
		else
		{
			*strOut = strIN+pos+FindPos-1;
		}
		if(!NodeName.IsEmpty())
		{
			strTmpNodeName = NodeName;
			_FindAtt((char*)(const char*)strTmpNodeName,NodeName,AttName,AttValue);
		}
		Type = 0;//
		return true;
	}
	return true;
}




/////////////////////////////////////////////////////////////////////////////////////////////
//
//				HNode
//
//
/////////////////////////////////////////////////////////////////////////////////////////////

HNode::HNode()
{
	m_pElement			= NULL;
	m_pOwnerDoc			= NULL;
	m_pChildNodes		= NULL;
	m_pParentNode		= NULL;
}

HNode::HNode(const char* pName,const char* pTxt)
{
	m_pElement			= NULL;
	m_pOwnerDoc			= NULL;
	m_pChildNodes		= NULL;
	m_pParentNode		= NULL;

	m_strTagName = pName;
	if(pTxt)
		m_strText = pTxt;
}

HNode::~HNode()
{
	if(m_pElement)
	{
		delete m_pElement;
		m_pElement = NULL;
	}
	ChildClear();
}

bool HNode::Init()
{
	if(!ChildClear())
		return false;
	
	return true;
}


//===============================================================================//
// Who				: Ho Hak
// Date				: 2003-11-26
//-------------------------------------------------------------------------------//
// Function name	: HNode::FindNode
// Description	    : /dkdk[@dkdkd='song']
// Return type		: HNode* 
// Argument         : char* pstrFindNode -> 
//===============================================================================//
HNode* HNode::FindNode(char* pstrFindNode)
{	
	char* pstrNext=NULL;
	HString NodeName,AttName,AttValue;
	int type;
	_FindeP(pstrFindNode,&pstrNext,NodeName,AttName,AttValue,type);
	return FindNode(&NodeName,&AttName,&AttValue,type,pstrNext);
}


//===============================================================================//
// Who				: Ho Hak
// Date				: 2003-11-26
//-------------------------------------------------------------------------------//
// Function name	: HNode::FindNodes
// Description	    :
// Return type		: HNodes*
// Argument         : char* pstrFindNode ->
//===============================================================================//
HNodes* HNode::FindNodes(char* pstrFindNode)
{
	char* pstrNext=NULL;
	HString NodeName,AttName,AttValue;
	int type;
	_FindeP(pstrFindNode,&pstrNext,NodeName,AttName,AttValue,type);
	return FindNodes(&NodeName,&AttName,&AttValue,type,pstrNext);
}


//===============================================================================//
// Who				: Ho Hak
// Date				: 2003-11-26
//-------------------------------------------------------------------------------//
// Function name	: HNode::RemoveChild
// Description	    : ¡÷¿««“¡°¿∫ for(πÆ¿∏∑Œ ¡ˆøÔ∂ß ø™º¯¿∏∑Œ «ÿæﬂ«—¥Ÿ.
//for(i = cnt-1; i>=0;i--) RemovereChild(i);
// Return type		: bool
// Argument         : HNode* pNode ->
//===============================================================================//
bool HNode::RemoveChild(HNode* pNode)
{
	HNode* pParentNode = NULL;
	if(!m_pChildNodes)
		return false;
	if(!pNode)
		return false;

	pParentNode = pNode->GetParentNode();
	if(!pParentNode)
		return false;
	return (bool)pParentNode->m_pChildNodes->Delete(pNode);
}

void HNode::RemoveChilds()
{
	ChildClear();
}


//===============================================================================//
// Who				: Ho Hak
// Date				: 2003-11-26
//-------------------------------------------------------------------------------//
// Function name	: HNode::ChildClear
// Description	    :
// Return type		: bool
//===============================================================================//
bool HNode::ChildClear()
{
//	HNode* pTempNode = NULL;
	if(!m_pChildNodes)
		return false;

	m_pChildNodes->Clear();
	if(m_pChildNodes)
	{
		delete m_pChildNodes;
		m_pChildNodes = NULL;
	}
	return true;
}


//===============================================================================//
// Who				: Ho Hak
// Date				: 2003-11-26
//-------------------------------------------------------------------------------//
// Function name	: HNode::GetChildCnt
// Description	    :
// Return type		: int
//===============================================================================//
int HNode::GetChildCnt()
{
	if(!m_pChildNodes)
		return 0;
	return m_pChildNodes->Size();
}



//===============================================================================//
// Who				: Ho Hak
// Date				: 2003-11-26
//-------------------------------------------------------------------------------//
// Function name	: HNode::GetChild
// Description	    :
// Return type		: HNode*
// Argument         : int i ->
//===============================================================================//
HNode* HNode::GetChild(int i)
{
	if(!m_pChildNodes)
		return NULL;
	return m_pChildNodes->GetAt(i);
}


//===============================================================================//
// Who				: Ho Hak
// Date				: 2003-11-26
//-------------------------------------------------------------------------------//
// Function name	: HNode::GetElementDatas
// Description	    :
// Return type		: HElementDatas*
//===============================================================================//
HElementDatas* HNode::GetElementDatas()
{
	if(!m_pElement)
		return NULL;
	return m_pElement->m_pElms;
}


//===============================================================================//
// Who				: Ho Hak
// Date				: 2003-11-26
//-------------------------------------------------------------------------------//
// Function name	: HNode::SetDocToChildNode
// Description	    :
// Return type		: void
// Argument         : HNode* pNode ->
// Argument         : HDocument* pDoc ->
//===============================================================================//
void HNode::SetDocToChildNode(HNode* pNode,HDocument* pDoc)
{
	HNodes *m_pChildNodes = pNode->GetChilds();
	if(!m_pChildNodes) return ;
	for( int i = 0; i < m_pChildNodes->Size(); i++)
	{
		m_pChildNodes->GetAt(i)->SetDoc(m_pOwnerDoc);
		SetDocToChildNode(m_pChildNodes->GetAt(i),pDoc);
	}
}


//===============================================================================//
// Who				: Ho Hak
// Date				: 2003-12-02
//-------------------------------------------------------------------------------//
// Function name	: HNode::Clone
// Description	    : HNode∏¶ ∫πªÁ«—¥Ÿ.
// Return type		: void
// Argument         : HNode* pNowNode ->
// Argument         : HNode* pNewNode ->
//===============================================================================//
void HNode::Clone(HNode* pNowNode,HNode* pNewNode,bool bOnly)
{
	pNewNode->m_strTagName = pNowNode->m_strTagName;
	pNewNode->m_strText	= pNowNode->m_strText;

	//ChildNode∏¶ ∫πªÁ«—¥Ÿ.
	//¿⁄Ω≈¿« ≥ÎµÂ∏∏ ƒ´«««“∞Õ¿Ã≥ƒ «œ¿ß ≥ÎµÂ±Ó¡ˆ ƒ´«««“∞Õ¿Ã≥ƒ
	if(pNowNode->m_pChildNodes && bOnly == false)
	{
		pNewNode->m_pChildNodes = new HNodes;
		int cnt = pNowNode->m_pChildNodes->Size();
		for(int i = 0; i < cnt; i++)
		{
			HNode* pNewChildNode = new HNode;
			pNewNode->m_pChildNodes->AddTTail(pNewChildNode);
			//Parent Node;
			pNewChildNode->m_pParentNode = pNewNode;
			Clone( pNowNode->m_pChildNodes->GetAt(i),pNewChildNode);
		}
	}
	//Element∏¶ ∫πªÁ«—¥Ÿ.
	if(pNowNode->m_pElement)
	{
		pNewNode->m_pElement = new HElement;
		int cnt = pNowNode->m_pElement->m_pElms->Size();
		for(int i = 0; i < cnt; i++)
		{
			HElementData* pElmData = pNowNode->m_pElement->m_pElms->GetAt(i);
			pNewNode->m_pElement->AddElm((char*)(const char*)pElmData->GetAttName(),
				(char*)(const char*)pElmData->GetAttValue());
		}
	}
}

HNode* HNode::CloneOnly()
{
	HNode* pNewNode = new HNode;
	Clone(this,pNewNode,true);
	return pNewNode;
}

//===============================================================================//
// Who				: Ho Hak
// Date				: 2003-12-02
//-------------------------------------------------------------------------------//
// Function name	: HNode::Clone
// Description	    : NowNode∏¶ ∫πªÁ«—¥Ÿ.
// Return type		: HNode* ªı∑Œ ∫πªÁµ» Node¿« ∞™
//===============================================================================//
HNode* HNode::Clone()
{
	HNode* pNewNode = new HNode;
	Clone(this,pNewNode);
	return pNewNode;
}

bool HNode::AppendChildFirst(HNode* pNode)
{
	if(!m_pChildNodes)
		m_pChildNodes = new CListNode<HNode>;

	//////////////////////////////////////////////////////////////////////////////////////
	pNode->m_pOwnerDoc = m_pOwnerDoc;
	pNode->m_pParentNode = this;
	//////////////////////////////////////////////////////////////////////////////////////

	SetDocToChildNode(pNode,pNode->m_pOwnerDoc);
	m_pChildNodes->AddTHead(pNode);
	return true;
}

//===============================================================================//
// Who				: Ho Hak
// Date				: 2003-11-26
//-------------------------------------------------------------------------------//
// Function name	: HNode::AppendChild
// Description	    :
// Return type		: bool
// Argument         : HNode*    pNode ->
//===============================================================================//
bool HNode::AppendChild(HNode *pNode)
{
	if(!m_pChildNodes)
		m_pChildNodes = new CListNode<HNode>;

	//////////////////////////////////////////////////////////////////////////////////////
	pNode->m_pOwnerDoc = m_pOwnerDoc;
	pNode->m_pParentNode = this;
	//////////////////////////////////////////////////////////////////////////////////////

	SetDocToChildNode(pNode,pNode->m_pOwnerDoc);
	m_pChildNodes->AddTTail(pNode);
	return true;
}



//#############################################//
// Who				: By Songhh
// Function name	: HNode::AppendChildDontDoc
// Description	    : º”µµ∏¶ «‚ªÛ«œ±‚ ¿ß«ÿº≠ Doc∏¶ º¬«œ¡ˆ æ ∞‘ «ﬂ¥Ÿ.
// Return type		: virtual bool
// Argument         : HNode* pNode
//#############################################//
//bool HNode::AppendChildDontDoc(HNode* pNode)
//{
//	if(!m_pChildNodes)
//		m_pChildNodes = new CListNode<HNode>;
//
//	//////////////////////////////////////////////////////////////////////////////////////
//	pNode->m_pOwnerDoc = m_pOwnerDoc;
//	pNode->m_pParentNode = this;
//	//////////////////////////////////////////////////////////////////////////////////////
//	m_pChildNodes->AddTTail(pNode);
//	return true;
//}

bool HNode::AppendChild(HNodes* pNodes)
{
	if(!pNodes) return false;
	int cnt = pNodes->Size();
	for(int i = 0; i < cnt; i++)
	{
		HNode* pNode = pNodes->GetAt(i);
		AppendChild(pNode);
	}
	return true;
}

const char*	HNode::GetElementToName(const char* pstrName)
{

	HElementDatas *pElmDatas;
	HElementData  *pElmData;
	pElmDatas = GetElementDatas();

	if(!pElmDatas)
		return NULL;

	if(pElmDatas->Size() == 0)
		return NULL;

	for( int i = 0; i < pElmDatas->Size(); i++)
	{
		pElmData = pElmDatas->GetAt(i);
		if(pElmData->GetAttName().Compare((char*)pstrName) == 0)
		{
			return pElmData->GetAttValue();
		}
	}
	return NULL;
}


bool HNode::GetElementToName(const char* pstrName,int* pnOut)
{
	const char* pstrValue=NULL;
	*pnOut = 0;
	pstrValue = GetElementToName(pstrName);
	if(pstrValue)
		*pnOut = atoi(pstrValue);
	else
		return false;
	return true;
}

bool HNode::GetElementToName(const char* pstrName,bool* pbOut)
{
	int nOut =0;

	*pbOut = false;
	if (false == GetElementToName (pstrName, &nOut)) {
		return false;
	}

	if (0 == nOut) {
		*pbOut = false;
	}
	else {
		*pbOut = true;
	}

	return true;
}

bool HNode::GetElementToName(const char* pstrName,long* plOut)
{
	const char* pstrValue=NULL;
	*plOut = 0;
	pstrValue = GetElementToName(pstrName);
	if(pstrValue)
		*plOut = strtol(pstrValue,0,10);
	else
		return false;
	return true;
}


//===============================================================================//
// Who				: Ho Hak
// Date				: 2003-11-26
//-------------------------------------------------------------------------------//
// Function name	: HNode::AppendElement
// Description	    :
// Return type		: bool
// Argument         : HElementData* pElementData ->
//===============================================================================//
bool HNode::AppendElement(HElementData* pElementData)
{
	SetAtt((char*)(const char*)pElementData->m_strAttName,
		(char*)(const char*)pElementData->m_strAttValue);
	return true;
}

bool HNode::AppendElement(const char* strName,const char* strValue)
{
	HElementData ElmData;
	ElmData.SetAttName((char*)strName);
	ElmData.SetAttValue((char*)strValue);

	SetAtt((char*)(const char*)ElmData.m_strAttName,
		(char*)(const char*)ElmData.m_strAttValue);
	return true;
}

bool HNode::AppendElement(const char* strName,long lValue)
{
	char strValue[20];
#ifdef WIN32
	ltoa(lValue,strValue,10);
#else
	sprintf(strValue,"%ld",lValue);
#endif
	return AppendElement(strName,(const char*) strValue);
}

bool HNode::AppendElement(const char* strName,int nValue)
{
	char strValue[20];
#ifdef WIN32
	itoa(nValue,strValue,10);
#else
	sprintf(strValue,"%d",nValue);
#endif
	return AppendElement(strName,(const char*) strValue);
}


//===============================================================================//
// Who				: Ho Hak
// Date				: 2003-11-26
//-------------------------------------------------------------------------------//
// Function name	: HNode::RemoveElement
// Description	    :
// Return type		: bool
// Argument         : char* pstrAttName ->
//===============================================================================//
bool HNode::RemoveElement(char* pstrAttName)
{
	if(!m_pElement)
		return false;

	return m_pElement->DelElm(pstrAttName);
}



HNodes* HNode::FindNodes(HString* NodeName,HString* AttName,HString* AttValue,int depth,
						char* pstrNext)
{

	if(depth == -1) //∑Á∆Æ
	{
		if(m_pOwnerDoc)
			return m_pOwnerDoc->GetRootNode()->m_pChildNodes;
		else
			return NULL;
	}
	if(depth == 0)
	{
		//¡∂±«¿Ã ∏¬¿∏∏È
		if(m_strTagName.Compare((char*)(const char*)*NodeName) == 0)
		{
			if(!AttName->IsEmpty())
			{
				//Eml ∫Ò±≥ ∆≤∏Æ∏È
				if(!(m_pElement &&
					m_pElement->Compare((char*)(const char*)*AttName,
												(char*)(const char*)*AttValue)))
				{
					return NULL;
				}
			}

			if(!pstrNext)	//√£¿ª «œ¿ß≥ÎµÂ∞° æ¯¥Ÿ∏È
			{
				return m_pChildNodes;
			}
			else			//¿÷¥Ÿ∏È
			{
				char* pchildstrNext=NULL;
				HString childNodeName,childAttName,childAttValue;
				int type;

				if(!m_pChildNodes)
					return NULL;

				_FindeP(pstrNext,&pchildstrNext,childNodeName,childAttName,
								childAttValue,type);
				int ichildcnt;
				HNode *pTempNode = NULL;
				ichildcnt = GetChildCnt();
				for(int j = 0; j < ichildcnt; j++)
				{
					pTempNode = GetChild(j);
					if((pTempNode = pTempNode->FindNode(&childNodeName,&childAttName,
								&childAttValue,type,pchildstrNext)) != 0)
								break;
					pTempNode = NULL;
				}

				return pTempNode?pTempNode->m_pChildNodes:NULL;
			}

		}
		else return NULL;
	}
	else
	{
		//¡∂±«¿Ã ∏¬¿∏∏È
		if(m_strTagName.Compare((char*)(const char*)*NodeName) == 0)
		{
			//Eml ∫Ò±≥ ∆≤∏Æ∏È
			//√£¿ª∂ß±Ó¡ˆ √£¥¬¥Ÿ.
			if(!AttName->IsEmpty())
			{
				if(!(m_pElement &&
					m_pElement->Compare((char*)(const char*)*AttName,
												(char*)(const char*)*AttValue)))
				{
					if(!m_pChildNodes)
						return NULL;
					int ichildcnt;
					HNode *pTempNode = NULL;
					ichildcnt = GetChildCnt();
					for(int j = 0; j < ichildcnt; j++)
					{
						pTempNode = GetChild(j);
						if((pTempNode = pTempNode->FindNode(NodeName,AttName,AttValue,depth-1,pstrNext)) != 0)
									break;
						pTempNode = NULL;
					}
					return pTempNode->m_pChildNodes;
				}
			}
			if(!pstrNext) //√£¿ª «œ¿ß≥ÎµÂ∞° æ¯¥Ÿ∏È
				return m_pChildNodes;

			else //¿÷¥Ÿ∏È
			{
				char* pchildstrNext=NULL;
				HString childNodeName,childAttName,childAttValue;
				int type;

				if(!m_pChildNodes)
					return NULL;

				_FindeP(pstrNext,&pchildstrNext,childNodeName,childAttName,
								childAttValue,type);
				int ichildcnt;
				HNode *pTempNode = NULL;
				ichildcnt = GetChildCnt();
				for(int j = 0; j < ichildcnt; j++)
				{
					pTempNode = GetChild(j);
					if((pTempNode = pTempNode->FindNode(&childNodeName,&childAttName,
								&childAttValue,type,pchildstrNext)) != 0)
								break;
					pTempNode = NULL;
				}
				return pTempNode->m_pChildNodes;
			}
		}
		else  //√£¿ª∂ß±Ó¡ˆ √£¥¬¥Ÿ.
		{
			if(!m_pChildNodes)
				return NULL;
			int ichildcnt;
			HNode *pTempNode = NULL;
			ichildcnt = GetChildCnt();
			for(int j = 0; j < ichildcnt; j++)
			{
				pTempNode = GetChild(j);
				if((pTempNode = pTempNode->FindNode(NodeName,AttName,AttValue,depth-1,pstrNext)) != 0)
							break;
				pTempNode = NULL;
			}

			return pTempNode?pTempNode->m_pChildNodes:NULL;
		}
	}
	return NULL;
}


HNode* HNode::FindNode(HString* NodeName,HString* AttName,HString* AttValue,int depth,
						char* pstrNext)
{

	if(depth == -1) //∑Á∆Æ
	{
		if(m_pOwnerDoc)
			return m_pOwnerDoc->GetRootNode();
		else
			return NULL;
	}
	if(depth == 0)
	{
		//¡∂±«¿Ã ∏¬¿∏∏È
		if(m_strTagName.Compare((char*)(const char*)*NodeName) == 0)
		{
			if(!AttName->IsEmpty())
			{
				//Eml ∫Ò±≥ ∆≤∏Æ∏È
				if(!(m_pElement &&
					m_pElement->Compare((char*)(const char*)*AttName,
												(char*)(const char*)*AttValue)))
				{
					return NULL;

				}
			}
			if(!pstrNext) //√£¿ª «œ¿ß≥ÎµÂ∞° æ¯¥Ÿ∏È
				return this;
			else //¿÷¥Ÿ∏È
			{
				char* pchildstrNext=NULL;
				HString childNodeName,childAttName,childAttValue;
				int type;


				if(!m_pChildNodes)
					return NULL;

				_FindeP(pstrNext,&pchildstrNext,childNodeName,childAttName,
								childAttValue,type);
				int ichildcnt;
				HNode *pTempNode = NULL;
				ichildcnt = GetChildCnt();
				for(int j = 0; j < ichildcnt; j++)
				{
					pTempNode = GetChild(j);
					if((pTempNode = pTempNode->FindNode(&childNodeName,&childAttName,
								&childAttValue,type,pchildstrNext)) != 0)
								break;
					pTempNode = NULL;
				}
				return pTempNode;
			}

		}
		else return NULL;
	}
	else
	{
		//¡∂±«¿Ã ∏¬¿∏∏È		// Depth2¥¬ ±◊≥… ¡ˆ≥™ƒ£¥Ÿ.
		if(m_strTagName.Compare((char*)(const char*)*NodeName) == 0 && depth != 1)
		{
			//Eml ∫Ò±≥ ∆≤∏Æ∏È
			//√£¿ª∂ß±Ó¡ˆ √£¥¬¥Ÿ.
			if(!AttName->IsEmpty())
			{
				if(!(m_pElement &&
					m_pElement->Compare((char*)(const char*)*AttName,
												(char*)(const char*)*AttValue)))
				{
					if(!m_pChildNodes)
						return NULL;
					int ichildcnt;
					HNode *pTempNode = NULL;
					ichildcnt = GetChildCnt();
					for(int j = 0; j < ichildcnt; j++)
					{
						pTempNode = GetChild(j);
						if((pTempNode = pTempNode->FindNode(NodeName,AttName,AttValue,depth-1,pstrNext)) != 0)
									break;
						pTempNode = NULL;
					}
					return pTempNode;
				}
			}

			if(!pstrNext) //√£¿ª «œ¿ß≥ÎµÂ∞° æ¯¥Ÿ∏È
				return this;
			else //¿÷¥Ÿ∏È
			{
				char* pchildstrNext=NULL;
				HString childNodeName,childAttName,childAttValue;
				int type;

				if(!m_pChildNodes)
					return NULL;

				_FindeP(pstrNext,&pchildstrNext,childNodeName,childAttName,
								childAttValue,type);
				int ichildcnt;
				HNode *pTempNode = NULL;
				ichildcnt = GetChildCnt();
				for(int j = 0; j < ichildcnt; j++)
				{
					pTempNode = GetChild(j);
					if((pTempNode = pTempNode->FindNode(&childNodeName,&childAttName,
								&childAttValue,type,pchildstrNext)) != 0)
								break;
					pTempNode = NULL;
				}
				return pTempNode;
			}
		}
		else  //√£¿ª∂ß±Ó¡ˆ √£¥¬¥Ÿ.
		{
			if(!m_pChildNodes)
				return NULL;
			int ichildcnt;
			HNode *pTempNode = NULL;
			ichildcnt = GetChildCnt();
			for(int j = 0; j < ichildcnt; j++)
			{
				pTempNode = GetChild(j);
				if((pTempNode = pTempNode->FindNode(NodeName,AttName,AttValue,depth-1,pstrNext)) != 0)
							break;
				pTempNode = NULL;
			}
			return pTempNode;
		}
	}
	return NULL;
}

HString& HNode::GetName()
{
	return m_strTagName;
}

void HNode::SetName(const char* pstrText)
{
	m_strTagName = pstrText;
}

HString& HNode::GetText()
{
//	char* pstrTmp = NULL;
	m_strText.Trim();
//	int cnt = m_strText.GetLength();
//	//æ’∫Œ∫–¿« \t∞˙ \n \n¡¶∞≈
//	for(int i = 0; i < cnt ; )
//	{
//		if(m_strText[i] == _T('\t') || m_strText[i] == _T('\r') || m_strText[i] == _T('\n'))
//		{
//			i++;
//		}
//		else
//			break;
//	}
//	if(i != 0)
//	{
//		pstrTmp = (char*)(const char*)m_strText.GetString();
//		m_strText = (pstrTmp+i);
//	}
//
//	cnt = m_strText.GetLength();
//	if(cnt < 2)
//		return m_strText;
//	if((m_strText[cnt-2] == _T('\r') || m_strText[cnt-2] == _T('\n'))
//		&& m_strText[cnt-1] == _T('\n'))
//		m_strText[cnt-2] = 0;
	return m_strText;
}

HString& HNode::GetDeCodeText()
{
//	char* pstrTmp = NULL;
	m_strDeGod = Entity2Ref(m_strText);
	m_strDeGod.Trim();
//∏«∏∂¡ˆ∏∑¿« \n¿∫ ª´¥Ÿ.
//	m_strText = Entity2Ref(m_strText);
//	int cnt = m_strText.GetLength();
//
//	//æ’∫Œ∫–¿« \t∞˙ \n \n¡¶∞≈
//	for(int i = 0; i < cnt ; )
//	{
//		if(m_strText[i] == _T('\t') || m_strText[i] == _T('\r') || m_strText[i] == _T('\n'))
//		{
//			i++;
//		}
//		else
//			break;
//	}
//	if(i != 0)
//	{
//		pstrTmp = (char*)(const char*)m_strText.GetString();
//		m_strText = (pstrTmp+i);
//	}
//
//	cnt = m_strText.GetLength();
//	if(cnt < 2)
//		return m_strText;
//	if((m_strText[cnt-2] == _T('\r') || m_strText[cnt-2] == _T('\n'))
//		&& m_strText[cnt-1] == _T('\n'))
//		m_strText[cnt-2] = 0;
	return m_strDeGod;
}

bool HNode::SetText(char* pstrText)
{
	m_strText = Ref2Entity(pstrText);
	return true;
}

bool HNode::SetAtt(char* pstrAttName,char* pstrAttValue)
{
	if(!m_pElement)
	{
		m_pElement = new HElement;
	}
	m_pElement->AddElm(pstrAttName,pstrAttValue);
	return true;
}


/*
bool HNode::GetXML(HString& str,int depth)
{
	HString strDeCodeText;
	int tabcnt;
	HString strTab;
	char startTag[] = _T("<");
	char endTag[] = _T(">");
	char Q[] = _T("'");
	char S[] = _T(" ");
	char E[] = _T("=");
	char SS[] = _T("/");

	if(m_strTagName.IsEmpty())
		return false;

	strDeCodeText = GetDeCodeText();

	if(!(m_pChildNodes == NULL && strDeCodeText.IsEmpty()))
	{
		for(tabcnt = 0; tabcnt < depth;tabcnt++)
		{
			strTab += _T("\t");
		}
		str += strTab;
	}
	str += startTag;
	str += m_strTagName;

	if(m_pElement)
	{
		int ielmcnt;
		HElementData * pTemElmData;
		ielmcnt = m_pElement->GetCnt();
		str += " ";
		for( int i = 0; i < ielmcnt; i++)
		{
			pTemElmData = m_pElement->GetAt(i);
			str += pTemElmData->m_strAttName;
			str += E;
			str += Q;

			str += pTemElmData->GetDeCodeAttValue();
			//str += pTemElmData->m_strAttValue;
			str += Q;

			if(i != (ielmcnt-1))
				str += S;
		}
	}
	//≥ªøÎ∞˙ Child∞° ¡∏¿Á«œ∏È <TagName ¿« ≈◊±◊∏¶ ¥›æ∆¡ÿ¥Ÿ. ¡Ô(>))

	if(!(m_pChildNodes == NULL && strDeCodeText.IsEmpty()))
	{
		str += endTag;
	}

	if(!strDeCodeText.IsEmpty())
	{
		str += _T("\n");
		str += strTab;
		str += strDeCodeText;
		//str += m_strText;
	}
	if(!(m_pChildNodes == NULL && strDeCodeText.IsEmpty()))
		str += _T("\n");
	if(m_pChildNodes)
	{
		int ichildcnt;
		HNode *pTempNode;
		ichildcnt = GetChildCnt();
		for(int j = 0; j < ichildcnt; j++)
		{
			pTempNode = GetChild(j);
			if(!pTempNode->GetXML(str,depth+1))
				return false;
		}
	}

	str += strTab;
	if(m_pChildNodes == NULL && strDeCodeText.IsEmpty())
	{

		str += SS;
		str += endTag;
	}
	else
	{

		str += startTag;
		str += SS;
		str += m_strTagName;
		str += endTag;
	}
	str += _T("\n");
	return true;
}

*/

bool HNode::GetXML(HString& str,int depth)
{
	if(!m_strXMLVer.IsEmpty())
	{
		str = "<?";
		str += m_strXMLVer;
		str += "?>\n";
	}
	return GetXMLIN(str,depth);
}

bool HNode::GetXMLIN(HString& str,int depth)
{
	HString strTmp;
	HString strDeCodeText;
	int tabcnt;
	HString strTab;
	char startTag[] = _T("<");
	char endTag[] = _T(">");
	char Q[] = _T("'");
	char S[] = _T(" ");
	char E[] = _T("=");
	char SS[] = _T("/");

	if(m_strTagName.IsEmpty())
		return false;

	strDeCodeText = GetDeCodeText();

	if(!(m_pChildNodes == NULL && strDeCodeText.IsEmpty()))
	{
		for(tabcnt = 0; tabcnt < depth;tabcnt++)
		{
			strTab += _T("\t");
		}
		strTmp += strTab;
	}
	strTmp += startTag;
	strTmp += m_strTagName;

	if(m_pElement)
	{
		int ielmcnt;
		HElementData * pTemElmData;
		ielmcnt = m_pElement->GetCnt();
		strTmp += " ";
		for( int i = 0; i < ielmcnt; i++)
		{
			pTemElmData = m_pElement->GetAt(i);
			strTmp += pTemElmData->m_strAttName;
			strTmp += E;
			strTmp += Q;

			strTmp += pTemElmData->GetDeCodeAttValue();
			//str += pTemElmData->m_strAttValue;
			strTmp += Q;

			if(i != (ielmcnt-1))
				strTmp += S;
		}
	}
	//≥ªøÎ∞˙ Child∞° ¡∏¿Á«œ∏È <TagName ¿« ≈◊±◊∏¶ ¥›æ∆¡ÿ¥Ÿ. ¡Ô(>))

	if(!(m_pChildNodes == NULL && strDeCodeText.IsEmpty()))
	{
		strTmp += endTag;
	}

	if(!strDeCodeText.IsEmpty())
	{
		strTmp += _T("\n");
		strTmp += strTab;
		strTmp += strDeCodeText;
		//str += m_strText;
	}
	if(!(m_pChildNodes == NULL && strDeCodeText.IsEmpty()))
		strTmp += _T("\n");

	if(m_pChildNodes)
	{
		int ichildcnt;
		HNode *pTempNode;
		ichildcnt = GetChildCnt();
		for(int j = 0; j < ichildcnt; j++)
		{
			pTempNode = GetChild(j);
			HString strChildTmp;
			if(!pTempNode->GetXMLIN(strChildTmp,depth+1))
				return false;

			strTmp += strChildTmp;
		}
	}

	strTmp += strTab;
	if(m_pChildNodes == NULL && strDeCodeText.IsEmpty())
	{

		strTmp += SS;
		strTmp += endTag;
	}
	else
	{

		strTmp += startTag;
		strTmp += SS;
		strTmp += m_strTagName;
		strTmp += endTag;
	}
	strTmp += _T("\n");

	str += strTmp;
	return true;
}

bool HNode::HTRACE(int depth)
{
	HString strDeCodeText;
	int tabcnt;
	HString strTab;
	char startTag[] = _T("<");
	char endTag[] = _T(">");
	char Q[] = _T("'");
	char S[] = _T(" ");
	char E[] = _T("=");
	char SS[] = _T("/");

	if(m_strTagName.IsEmpty())
		return false;

	strDeCodeText = GetDeCodeText();

	if(!(m_pChildNodes == NULL && strDeCodeText.IsEmpty()))
	{
		for(tabcnt = 0; tabcnt < depth;tabcnt++)
		{
			HLogD(_T("\t"));
		}
		HLogD( strTab);
	}
	HLogD( startTag);
	HLogD( m_strTagName);


	if(m_pElement)
	{
		int ielmcnt;
		HElementData * pTemElmData;
		ielmcnt = m_pElement->GetCnt();
		HLogD(" ");
		for( int i = 0; i < ielmcnt; i++)
		{
			pTemElmData = m_pElement->GetAt(i);
			HLogD(pTemElmData->m_strAttName);
			HLogD(E);
			HLogD(Q);

			HLogD(pTemElmData->GetDeCodeAttValue());
			HLogD(Q);

			if(i != (ielmcnt-1))
				HLogD(S);
		}
	}
	//≥ªøÎ∞˙ Child∞° ¡∏¿Á«œ∏È <TagName ¿« ≈◊±◊∏¶ ¥›æ∆¡ÿ¥Ÿ. ¡Ô(>))

	if(!(m_pChildNodes == NULL && strDeCodeText.IsEmpty()))
	{
		HLogD(endTag);
	}

	if(!strDeCodeText.IsEmpty())
	{
		HLogD(_T("\n"));
		HLogD(strTab);
		HLogD(strDeCodeText);
	}
	if(!(m_pChildNodes == NULL && strDeCodeText.IsEmpty()))
		HLogD(_T("\n"));
	if(m_pChildNodes)
	{
		int ichildcnt;
		HNode *pTempNode;
		ichildcnt = GetChildCnt();
		for(int j = 0; j < ichildcnt; j++)
		{
			pTempNode = GetChild(j);
			if(!pTempNode->HTRACE(depth+1))
				return false;
		}
	}

	HLogD(strTab);
	if(m_pChildNodes == NULL && strDeCodeText.IsEmpty())
	{

		HLogD(SS);
		HLogD(endTag);
	}
	else
	{

		HLogD(startTag);
		HLogD(SS);
		HLogD(m_strTagName);
		HLogD(endTag);
	}
	HLogD(_T("\n"));

#ifdef WIN32
	Sleep(100);
#endif
	return true;
}

bool HNode::HTRACEP(int depth)
{
//	HString strDeCodeText;
//	int tabcnt;
//	HString strTab;
//	char startTag[] = _T("<");
//	char endTag[] = _T(">");
//	char Q[] = _T("'");
//	char S[] = _T(" ");
//	char E[] = _T("=");
//	char SS[] = _T("/");
//
//	if(m_strTagName.IsEmpty())
//		return false;
//
//	strDeCodeText = GetDeCodeText();
//
//	if(!(m_pChildNodes == NULL && strDeCodeText.IsEmpty()))
//	{
//		for(tabcnt = 0; tabcnt < depth;tabcnt++)
//		{
//			printf(_T("\t"));
//		}
//		printf( strTab);
//	}
//	printf( startTag);
//	printf( m_strTagName);
//
//
//	if(m_pElement)
//	{
//		int ielmcnt;
//		HElementData * pTemElmData;
//		ielmcnt = m_pElement->GetCnt();
//		printf(" ");
//		for( int i = 0; i < ielmcnt; i++)
//		{
//			pTemElmData = m_pElement->GetAt(i);
//			printf(pTemElmData->m_strAttName);
//			printf(E);
//			printf(Q);
//
//			printf(pTemElmData->GetDeCodeAttValue());
//			printf(Q);
//
//			if(i != (ielmcnt-1))
//				printf(S);
//		}
//	}
//	//≥ªøÎ∞˙ Child∞° ¡∏¿Á«œ∏È <TagName ¿« ≈◊±◊∏¶ ¥›æ∆¡ÿ¥Ÿ. ¡Ô(>))
//
//	if(!(m_pChildNodes == NULL && strDeCodeText.IsEmpty()))
//	{
//		printf(endTag);
//	}
//
//	if(!strDeCodeText.IsEmpty())
//	{
//		printf(_T("\n"));
//		printf(strTab);
//		printf(strDeCodeText);
//	}
//	if(!(m_pChildNodes == NULL && strDeCodeText.IsEmpty()))
//		printf(_T("\n"));
//	if(m_pChildNodes)
//	{
//		int ichildcnt;
//		HNode *pTempNode;
//		ichildcnt = GetChildCnt();
//		for(int j = 0; j < ichildcnt; j++)
//		{
//			pTempNode = GetChild(j);
//			if(!pTempNode->HTRACEP(depth+1))
//				return false;
//		}
//	}
//
//	printf(strTab);
//	if(m_pChildNodes == NULL && strDeCodeText.IsEmpty())
//	{
//
//		printf(SS);
//		printf(endTag);
//	}
//	else
//	{
//
//		printf(startTag);
//		printf(SS);
//		printf(m_strTagName);
//		printf(endTag);
//	}
//	printf(_T("\n"));
	return true;
}

/*
bool HNode::GetOrgXML(HString& str)
{
	HString strDeCodeText;
	char startTag[] = _T("<");
	char endTag[] = _T(">");
	char Q[] = _T("'");
	char S[] = _T(" ");
	char E[] = _T("=");
	char SS[] = _T("/");

	if(m_strTagName.IsEmpty())
		return false;

	strDeCodeText = GetDeCodeText();

	str += startTag;
	str += m_strTagName;

	if(m_pElement)
	{
		int ielmcnt;
		HElementData * pTemElmData;
		ielmcnt = m_pElement->GetCnt();
		str += " ";
		for( int i = 0; i < ielmcnt; i++)
		{
			pTemElmData = m_pElement->GetAt(i);
			str += pTemElmData->m_strAttName;
			str += E;
			str += Q;

			str += pTemElmData->GetDeCodeAttValue();
			//str += pTemElmData->m_strAttValue;
			str += Q;

			if(i != (ielmcnt-1))
				str += S;
		}
	}

	//≥ªøÎ∞˙ Child∞° ¡∏¿Á«œ∏È <TagName ¿« ≈◊±◊∏¶ ¥›æ∆¡ÿ¥Ÿ. ¡Ô(>))
	if(!(m_pChildNodes == NULL && strDeCodeText.IsEmpty()))
	{
		str += endTag;
	}

	if(!strDeCodeText.IsEmpty())
	{
		str += strDeCodeText;
		//str += GetDeCodeText();
		//str += m_strText;
	}

	if(m_pChildNodes)
	{
		int ichildcnt;
		HNode *pTempNode;
		ichildcnt = GetChildCnt();
		for(int j = 0; j < ichildcnt; j++)
		{
			pTempNode = GetChild(j);
			if(!pTempNode->GetOrgXML(str))
				return false;
		}
	}

	if(m_pChildNodes == NULL && strDeCodeText.IsEmpty())
	{
		str += SS;
		str += endTag;
	}
	else
	{
		str += startTag;
		str += SS;
		str += m_strTagName;
		str += endTag;
	}
	return true;
}
*/
bool HNode::GetOrgXML(HString& str)
{
	if(!m_strXMLVer.IsEmpty())
	{
		str = "<?";
		str += m_strXMLVer;
		str += "?>\n";
	}
	return GetOrgXMLIN(str);
}

bool HNode::GetOrgXMLIN(HString& str)
{
	HString strTmp;
	HString strDeCodeText;
	char startTag[] = _T("<");
	char endTag[] = _T(">");
	char Q[] = _T("'");
	char S[] = _T(" ");
//	char E[] = _T("=");
//	char SS[] = _T("/");

	char EQ[] = _T("='");
	char SSendTag[] = _T("/>");
	char startTagSS[] = _T("</");

	if(m_strTagName.IsEmpty())
		return false;

	strDeCodeText = GetDeCodeText();

	strTmp += startTag;
	strTmp += m_strTagName;

	if(m_pElement)
	{
		int ielmcnt;
		HElementData * pTemElmData;
		ielmcnt = m_pElement->GetCnt();
		strTmp += " ";
		for( int i = 0; i < ielmcnt; i++)
		{
			pTemElmData = m_pElement->GetAt(i);
			strTmp += pTemElmData->m_strAttName;
			//strTmp += E;
			//strTmp += Q;
			strTmp += EQ;

			strTmp += pTemElmData->GetDeCodeAttValue();
			strTmp += Q;

			if(i != (ielmcnt-1))
				strTmp += S;
		}
	}

	//≥ªøÎ∞˙ Child∞° ¡∏¿Á«œ∏È <TagName ¿« ≈◊±◊∏¶ ¥›æ∆¡ÿ¥Ÿ. ¡Ô(>))
	if(!(m_pChildNodes == NULL && strDeCodeText.IsEmpty()))
	{
		strTmp += endTag;
	}

	if(!strDeCodeText.IsEmpty())
	{
		strTmp += strDeCodeText;
	}

	if(m_pChildNodes)
	{
		int ichildcnt;
		HNode *pTempNode;
		ichildcnt = GetChildCnt();

		char** strChild;
		if(ichildcnt > 0)
		{
			strChild = new char*[ichildcnt];
			int nTotalChar = 0;
			for(int j = 0; j < ichildcnt; j++)
			{
				pTempNode = GetChild(j);
				HString strChildTmp;
				if(!pTempNode->GetOrgXMLIN(strChildTmp))
					return false;

				nTotalChar+=strChildTmp.GetLength();
				strChild[j] = new char[strChildTmp.GetLength()+1];
				strcpy(strChild[j],strChildTmp);
			}

			char* strChild2 = new char[nTotalChar+1];
			*strChild2 = '\0';
			for(int j = 0; j < ichildcnt; j++)
			{
				strcat(strChild2,strChild[j]);
				delete[] strChild[j];
			}
			strTmp += strChild2;
			delete[] strChild2;
			delete[] strChild;
		}
	}

	if(m_pChildNodes == NULL && strDeCodeText.IsEmpty())
	{
		//strTmp += SS;
		//strTmp += endTag;
		strTmp +=SSendTag;
	}
	else
	{
		//strTmp += startTag;
		//strTmp += SS;
		strTmp += startTagSS;
		strTmp += m_strTagName;
		strTmp += endTag;
	}

	str += strTmp;
	return true;
}

/*
bool HNode::GetOrgXMLIN(HString& str)
{
	HString strTmp;
	HString strDeCodeText;
	char startTag[] = _T("<");
	char endTag[] = _T(">");
	char Q[] = _T("'");
	char S[] = _T(" ");
	char E[] = _T("=");
	char SS[] = _T("/");

	if(m_strTagName.IsEmpty())
		return false;

	strDeCodeText = GetDeCodeText();

	strTmp += startTag;
	strTmp += m_strTagName;

	if(m_pElement)
	{
		int ielmcnt;
		HElementData * pTemElmData;
		ielmcnt = m_pElement->GetCnt();
		strTmp += " ";
		for( int i = 0; i < ielmcnt; i++)
		{
			pTemElmData = m_pElement->GetAt(i);
			strTmp += pTemElmData->m_strAttName;
			strTmp += E;
			strTmp += Q;

			strTmp += pTemElmData->GetDeCodeAttValue();
			//str += pTemElmData->m_strAttValue;
			strTmp += Q;

			if(i != (ielmcnt-1))
				strTmp += S;
		}
	}

	//≥ªøÎ∞˙ Child∞° ¡∏¿Á«œ∏È <TagName ¿« ≈◊±◊∏¶ ¥›æ∆¡ÿ¥Ÿ. ¡Ô(>))
	if(!(m_pChildNodes == NULL && strDeCodeText.IsEmpty()))
	{
		strTmp += endTag;
	}

	if(!strDeCodeText.IsEmpty())
	{
		strTmp += strDeCodeText;
	}

	if(m_pChildNodes)
	{
		int ichildcnt;
		HNode *pTempNode;
		ichildcnt = GetChildCnt();


		for(int j = 0; j < ichildcnt; j++)
		{
			pTempNode = GetChild(j);
			HString strChildTmp;
			if(!pTempNode->GetOrgXMLIN(strChildTmp))
				return false;

			strTmp += strChildTmp;
		}
	}

	if(m_pChildNodes == NULL && strDeCodeText.IsEmpty())
	{
		strTmp += SS;
		strTmp += endTag;
	}
	else
	{
		strTmp += startTag;
		strTmp += SS;
		strTmp += m_strTagName;
		strTmp += endTag;
	}

	str += strTmp;
	return true;
}
*/
/*
bool HNode::GetOrgXML(HString* pstr)
{
	HString strDeCodeText;
	char startTag[] = _T("<");
	char endTag[] = _T(">");
	char Q[] = _T("'");
	char S[] = _T(" ");
	char E[] = _T("=");
	char SS[] = _T("/");

	if(m_strTagName.IsEmpty())
		return false;

	strDeCodeText = GetDeCodeText();

	*pstr += startTag;
	*pstr += m_strTagName;

	if(m_pElement)
	{
		int ielmcnt;
		HElementData * pTemElmData;
		ielmcnt = m_pElement->GetCnt();
		*pstr += " ";
		for( int i = 0; i < ielmcnt; i++)
		{
			pTemElmData = m_pElement->GetAt(i);
			*pstr += pTemElmData->m_strAttName;
			*pstr += E;
			*pstr += Q;

			*pstr += pTemElmData->GetDeCodeAttValue();
			//str += pTemElmData->m_strAttValue;
			*pstr += Q;

			if(i != (ielmcnt-1))
				*pstr += S;
		}
	}
	//≥ªøÎ∞˙ Child∞° ¡∏¿Á«œ∏È <TagName ¿« ≈◊±◊∏¶ ¥›æ∆¡ÿ¥Ÿ. ¡Ô(>))
	if(!(m_pChildNodes == NULL && strDeCodeText.IsEmpty()))
	{
		*pstr += endTag;
	}

	if(!strDeCodeText.IsEmpty())
	{
		*pstr += strDeCodeText;
		//str += GetDeCodeText();
		//str += m_strText;
	}

	if(m_pChildNodes)
	{
		int ichildcnt;
		HNode *pTempNode;
		ichildcnt = GetChildCnt();
		for(int j = 0; j < ichildcnt; j++)
		{
			pTempNode = GetChild(j);
			if(!pTempNode->GetOrgXML(pstr))
				return false;
		}
	}

	if(m_pChildNodes == NULL && strDeCodeText.IsEmpty())
	{

		*pstr += SS;
		*pstr += endTag;
	}
	else
	{
		*pstr += startTag;
		*pstr += SS;
		*pstr += m_strTagName;
		*pstr += endTag;
	}
	return true;
}
*/

bool HNode::GetOrgXML(HString* str)
{
	if(!m_strXMLVer.IsEmpty())
	{
		*str = "<?";
		*str += m_strXMLVer;
		*str += "?>\n";
	}
	return GetOrgXMLIN(str);
}

bool HNode::GetOrgXMLIN(HString* pstr)
{
	HString strTmp;
	HString strDeCodeText;
	char startTag[] = _T("<");
	char endTag[] = _T(">");
	char Q[] = _T("'");
	char S[] = _T(" ");
	char E[] = _T("=");
	char SS[] = _T("/");

	if(m_strTagName.IsEmpty())
		return false;

	strDeCodeText = GetDeCodeText();

	strTmp += startTag;
	strTmp += m_strTagName;

	if(m_pElement)
	{
		int ielmcnt;
		HElementData * pTemElmData;
		ielmcnt = m_pElement->GetCnt();
		strTmp += " ";
		for( int i = 0; i < ielmcnt; i++)
		{
			pTemElmData = m_pElement->GetAt(i);
			strTmp += pTemElmData->m_strAttName;
			strTmp += E;
			strTmp += Q;

			strTmp += pTemElmData->GetDeCodeAttValue();
			//str += pTemElmData->m_strAttValue;
			strTmp += Q;

			if(i != (ielmcnt-1))
				strTmp += S;
		}
	}
	//≥ªøÎ∞˙ Child∞° ¡∏¿Á«œ∏È <TagName ¿« ≈◊±◊∏¶ ¥›æ∆¡ÿ¥Ÿ. ¡Ô(>))
	if(!(m_pChildNodes == NULL && strDeCodeText.IsEmpty()))
	{
		strTmp += endTag;
	}

	if(!strDeCodeText.IsEmpty())
	{
		strTmp += strDeCodeText;
	}

	if(m_pChildNodes)
	{
		int ichildcnt;
		HNode *pTempNode;
		ichildcnt = GetChildCnt();
		for(int j = 0; j < ichildcnt; j++)
		{
			pTempNode = GetChild(j);
			HString strChildTmp;
			if(!pTempNode->GetOrgXMLIN(&strChildTmp))
				return false;

			strTmp += strChildTmp;
		}
	}

	if(m_pChildNodes == NULL && strDeCodeText.IsEmpty())
	{

		strTmp += SS;
		strTmp += endTag;
	}
	else
	{
		strTmp += startTag;
		strTmp += SS;
		strTmp += m_strTagName;
		strTmp += endTag;
	}

	*pstr += strTmp;
	return true;
}



// attr1="value1" attr2='value2' attr3=value3 />
//                                            ^- return pointer
//========================================================
// Name   : LoadAttributes
// Desc   : loading attribute plain xml text
// Param  : pszAttrs - xml of attributes
//          pi = parser information
// Return : advanced string pointer.
//--------------------------------------------------------
// Coder    Date                      Desc
// bro      2002-10-29
//========================================================
char* HNode::LoadAttributes( const char* pszAttrs , LPPARSEINFO pi /*= &piDefault*/)
{
	char* xml = (char*)pszAttrs;

	while( xml && *xml )
	{
		//HString strTmpName,strTmpText;
		if( (xml = _tcsskip( xml )) != 0 )
		{
			// close tag
			if( *xml == chXMLTagClose || *xml == chXMLTagPre )
				// wel-formed tag
				return xml;

			// XML Attr Name
#ifdef WIN32
			char* pEnd = _tcspbrk( xml, " =" );
#else
			char* pEnd = strpbrk( xml, " =" );
#endif
			if( pEnd == NULL )
			{
				// error
				if( pi->erorr_occur == false )
				{
					pi->erorr_occur = true;
					pi->error_pointer = xml;
					pi->error_code = PIE_ATTR_NO_VALUE;
					pi->error_string.Format( (char*)_T("<%s> attribute has error "),
						(char*)(const char*)m_strTagName );
				}
				return xml;
			}

			if(!m_pElement)
				m_pElement = new HElement;

			HElementData* pElm = new HElementData;

			// XML Attr Name
			_SetString( xml, pEnd, &pElm->m_strAttName);

			// add new attribute
			m_pElement->AddElm(pElm);

			xml = pEnd;
			// XML Attr Value
			if( (xml = _tcsskip( xml )) != 0 )
			{
				//if( xml = _tcschr( xml, '=' ) )
				if( *xml == '=' )
				{
					if( (xml = _tcsskip( ++xml )) != 0 )
					{
						// if " or '
						// or none quote
						int quote = *xml;
						if( quote == '"' || quote == '\'' )
							pEnd = _tcsechr( ++xml, quote, chXMLEscape );
						else
						{
							//attr= value>
							// none quote mode
							//pEnd = _tcsechr( xml, ' ', '\\' );
							pEnd = _tcsepbrk( xml, _T(" >"), chXMLEscape );
						}

						bool trim = pi->trim_value;
						_SetString( xml, pEnd, &pElm->m_strAttValue, trim, chXMLEscape );
						xml = pEnd;
						// ATTRVALUE
						if( pi->entity_value)
						{
							//TODO:
							pElm->m_strAttValue = Ref2Entity(pElm->m_strAttValue);
						}
						if( quote == '"' || quote == '\'' )
							xml++;
					}
				}
			}
		}
	}
	// not wel-formed tag
	return NULL;
}

const char* HNode::LoadXML(char* pstrXMLData,LPPARSEINFO pi)
{
	char* pXMLVer;
	char* pTagEnd = NULL;
	const char* xml = (char*)pstrXMLData;
#ifdef WIN32
	xml = _tcschr( xml, chXMLTagOpen );
#else
	xml = strchr( xml, chXMLTagOpen );
#endif
	if( xml == NULL )
		return xml;

	//<?xml version="1.0" encoding="ks_c_5601-1987"?>
	//¿ª ¿–æÓ ø¬¥Ÿ.
	if(*(xml+1) == '?')
	{
#ifdef WIN32
		pTagEnd = _tcspbrk( xml+2, "?>" );
#else
		pTagEnd = (char*)strpbrk( xml+2, "?>" );
#endif
		if(pTagEnd)
		{

			pXMLVer = new char[pTagEnd-xml+1];
			strncpy(pXMLVer,xml+2,pTagEnd-xml-2);
			pXMLVer[pTagEnd-xml-2] = 0;
			m_strXMLVer = (const char*)pXMLVer;
			delete[] pXMLVer;
			pTagEnd += 2;
		}
		else
			pTagEnd = pstrXMLData;
	}
	else
		pTagEnd = pstrXMLData;

    if(strstr(pTagEnd,"<!DOCTYPE"))
    {
        char* pDocType = strpbrk( pTagEnd, "<!DOCTYPE" );
        if(pDocType)
        {
            pTagEnd = strpbrk( pDocType, """>" );
            pTagEnd = strpbrk( pTagEnd, "<" );
        }
    }



	return LoadXMLIN(pTagEnd,pi);
}

const char* HNode::LoadXMLIN(char* pstrXMLData,LPPARSEINFO pi)
{
	const char* xml = (char*)pstrXMLData;

	// initilize
	Init();

#ifdef WIN32
	xml = _tcschr( xml, chXMLTagOpen );
#else
	xml = strchr( xml, chXMLTagOpen );
#endif
	if( xml == NULL )
		return xml;

	// Close Tag
	if( *(xml+1) == chXMLTagPre ) // </Close
		return xml;

	// XML Node Tag Name Open
	xml++;
#ifdef WIN32
	char* pTagEnd = _tcspbrk( xml, " />" );
#else
	char* pTagEnd = (char*)strpbrk( xml, " />" );
#endif
	_SetString( xml, pTagEnd, &m_strTagName );
	xml = pTagEnd;

	// Generate XML Attributte List
	if( (xml = LoadAttributes( xml, pi )) != 0 )
	{
		// alone tag <TAG ... />
		if( *xml == chXMLTagPre )
		{
			xml++;
			if( *xml == chXMLTagClose )
				// wel-formed tag
				return ++xml;
			else
			{
				// error: <TAG ... / >
				if( pi->erorr_occur == false )
				{
					pi->erorr_occur = true;
					pi->error_pointer = (char*)xml;
					pi->error_code = PIE_ALONE_NOT_CLOSED;
					pi->error_string = _T("Element must be closed.");
				}
				// not wel-formed tag
				return xml;
			}
		}
		else
		// open/close tag <TAG ..> ... </TAG>
		//                        ^- current pointer
		{
			// insert if no text value
			if( m_strText.IsEmpty())
			{
				// Text Value
				char* pEnd = _tcsechr( ++xml, chXMLTagOpen, chXMLEscape );
				if( pEnd == NULL )
				{
					if( pi->erorr_occur == false )
					{
						pi->erorr_occur = true;
						pi->error_pointer = (char*)xml;
						pi->error_code = PIE_NOT_CLOSED;
						pi->error_string.Format((char*)_T("%s must be closed with </%s>"),
							(char*)(const char*)m_strTagName );
					}
					// error cos not exist CloseTag </TAG>
					return xml;
				}

				bool trim = pi->trim_value;
				_SetString( xml, pEnd, &m_strText, trim, chXMLEscape );
				xml = pEnd;
				// TEXTVALUE reference
				if( pi->entity_value)
				{
					//TODO
					m_strText = Ref2Entity(m_strText);
				}
			}

			// generate child nodes
			while( xml && *xml )
			{
				HNode* node = new HNode;

				//////////////////////////////////////////////////////////////////////////////////////
				// π´∏¿« Doc∏¶ ≥—∞‹¡ÿ¥Ÿ.
				// Doc∞° ¡∏¿Á«—¥Ÿ∏È.
				//if(this->m_pParentNode)
				node->m_pOwnerDoc = this->m_pOwnerDoc;
				//////////////////////////////////////////////////////////////////////////////////////

				xml = node->LoadXMLIN( (char*)xml,pi );
				node->m_pParentNode = this;


				if( node->m_strTagName.IsEmpty() == false )
				{
					//m_pChildNodesø°ø° «ˆ¿Á ≥ÎµÂ∏¶ √ﬂ∞°«—¥Ÿ.
					if(!m_pChildNodes)
						m_pChildNodes = new CListNode<HNode>;
					m_pChildNodes->AddTTail(node);
				}
				else
				{
					//m_pChildNodesø°  «ˆ¿Á ≥ÎµÂ∏¶ ¡ˆøÓ¥Ÿ.
					delete node;
				}

				// open/close tag <TAG ..> ... </TAG>
				//                             ^- current pointer
				// CloseTag case
				if( xml && *xml && *(xml+1) && *xml == chXMLTagOpen && *(xml+1) == chXMLTagPre )
				{
					// </Close>
					xml+=2; // C

					if( (xml = _tcsskip( xml )) != 0 )
					{
						HString closename;
						//<Data></Data\n>
#ifdef WIN32
						char* pEnd = _tcspbrk( xml, " \n>" );
#else
						char* pEnd = (char*)strpbrk( xml, " \n>" );
#endif

						if( pEnd == NULL )
						{
							if( pi->erorr_occur == false )
							{
								pi->erorr_occur = true;
								pi->error_pointer = (char*)xml;
								pi->error_code = PIE_NOT_CLOSED;
								pi->error_string.Format((char*)_T("it must be closed with </%s>"),
									(char*)(const char*)m_strTagName );
							}
							// error
							return xml;
						}
						_SetString( xml, pEnd, &closename );
						if( closename.Compare((char*)this->m_strTagName.GetString()) == 0)
						{
							// wel-formed open/close
							xml = pEnd+1;
							// return '>' or ' ' after pointer
							return xml;
						}
						else
						{
							xml = pEnd+1;
							// not welformed open/close
							if( pi->erorr_occur == false )
							{
								pi->erorr_occur = true;
								pi->error_pointer = (char*)xml;
								pi->error_code = PIE_NOT_NESTED;
								pi->error_string.Format((char*)_T("'<%s> ... </%s>' is not wel-formed."), (const char*)m_strTagName, (const char*)closename );

							}
							return xml;
						}
					}
				}
				else	// Alone child Tag Loaded
				{

					if( xml && this->m_strText.IsEmpty() && *xml !=chXMLTagOpen )
					{
						// Text Value
						char* pEnd = _tcsechr( xml, chXMLTagOpen, chXMLEscape );
						if( pEnd == NULL )
						{
							// error cos not exist CloseTag </TAG>
							if( pi->erorr_occur == false )
							{
								pi->erorr_occur = true;
								pi->error_pointer = (char*)xml;
								pi->error_code = PIE_NOT_CLOSED;
								pi->error_string.Format((char*)_T("it must be closed with </%s>"),
									(char*)(const char*)m_strTagName );
							}
							return xml;
						}

						bool trim = pi->trim_value;
						_SetString( xml, pEnd, &m_strText, trim, chXMLEscape );
						xml = pEnd;
						//TEXTVALUE
						if( pi->entity_value)
						{
							m_strText = Ref2Entity(m_strText);
						}
					}
				}
			}
		}
	}

	return xml;
}






/////////////////////////////////////////////////////////////////////////////////////////////
//
//				HElementData
//
//
/////////////////////////////////////////////////////////////////////////////////////////////


HElementData::HElementData()
{


}
HElementData::~HElementData()
{

}

HString& HElementData::GetAttName()
{

	return m_strAttName;
}

HString& HElementData::GetAttValue()
{
	//m_strAttValue = Entity2Ref(m_strAttValue);
	return m_strAttValue;
}

HString& HElementData::GetDeCodeAttValue()
{
	m_strDeGod = Entity2Ref(m_strAttValue);
	return m_strDeGod;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
//				HElement
//
//
/////////////////////////////////////////////////////////////////////////////////////////////
HElement::HElement()
{
	m_pElms = NULL;

}

HElement::~HElement()
{
	if(m_pElms)
	{
		delete m_pElms;
		m_pElms = NULL;
	}
}

void HElement::AddElm(char* AddName,char *AttValue)
{
	HElementData* pTmpElm = NULL;
	if(!m_pElms)
	{
		m_pElms = new CListNode<HElementData>;

		pTmpElm = new HElementData;
		pTmpElm->m_strAttName = AddName;
		pTmpElm->m_strAttValue = Ref2Entity(AttValue);
		m_pElms->AddTTail(pTmpElm);
	}
	else
	{
		int bMod = false;
		int cnt = m_pElms->Size();
		for(int i = 0; i < cnt; i++)
		{
			pTmpElm = m_pElms->GetAt(i);
			if(pTmpElm->GetAttName().Compare(AddName) == 0) //¿Ã∏ß¿Ã ∞∞¿∏∏È ºˆ¡§
			{
				pTmpElm->m_strAttValue = Ref2Entity(AttValue);
				bMod = true;
			}
		}
		if(bMod == false) //ªı∑ŒøÓ µ•¿Ã≈∏¿Ã∏È Add«ÿ¡ÿ¥Ÿ.
		{
			pTmpElm = new HElementData;
			pTmpElm->m_strAttName = AddName;
			pTmpElm->m_strAttValue = Ref2Entity(AttValue);
			m_pElms->AddTTail(pTmpElm);
		}
	}
}

bool HElement::Compare(char* attName,char* AttVal)
{
	HElementData* pTmpElm = NULL;

	if(!m_pElms)
		return false;

	int cnt = m_pElms->Size();
	for(int i = 0; i < cnt; i++)
	{
		pTmpElm = m_pElms->GetAt(i);
		if(pTmpElm->GetAttName().Compare(attName) == 0 &&
			pTmpElm->m_strAttValue.Compare((char*)(const char*)Ref2Entity(AttVal)) == 0
			) //¿Ã∏ß¿Ã ∞∞¿∏∏È ºˆ¡§
			return true;
	}
	return false;
}


void HElement::AddElm(HElementData* pElms)
{

	if(!m_pElms)
		m_pElms = new CListNode<HElementData>;

	m_pElms->AddTTail(pElms);
}

bool HElement::DelElm(char* AddName)
{
	if(!m_pElms)
		return false;

	int cnt = m_pElms->Size();
	for(int i = 0 ; i < cnt ; i++)
		if(m_pElms->GetAt(i)->m_strAttName.Compare(AddName) == 0)
			return (bool)m_pElms->Delete(i);
	return true;
}

HElementData*  HElement::GetAt(int i )
{
	if(!m_pElms)
		return NULL;

	return m_pElms->GetAt(i);
}

int HElement::GetCnt()
{
	if(!m_pElms)
		return 0;
	return m_pElms->Size();
}


/////////////////////////////////////////////////////////////////////////////////////////////
//
//				HDocument
//
//
/////////////////////////////////////////////////////////////////////////////////////////////
HDocument::HDocument()
{
	m_pRootNode = NULL;
}

HDocument::~HDocument()
{
}

HNode* HDocument::CreateNode(char* TagName,char* Text)
{
	HNode* pNewNode=NULL;
	pNewNode = new HNode;
	pNewNode->m_pOwnerDoc = this;


	if(!TagName || !pNewNode->SetText(Text))
	{
		return pNewNode;
	}
	pNewNode->m_strTagName = TagName;
	return pNewNode;
}

bool HDocument::LoadXML(char *pstrXMLData)
{
	char* pstrTemp=NULL;
	bool bResult = false;
	PARSEINFO pi;

	//pi.erorr_occur = false;
	m_pRootNode = (HNode*)this;
	m_pOwnerDoc = this;
	pstrTemp = (char*)HNode::LoadXML(pstrXMLData,&pi);
	if(pi.erorr_occur == false)
		bResult = true;
	else
	{
		//AfxMessageBox(pi.error_string);
		m_pRootNode = NULL;
		m_pOwnerDoc = NULL;
	}
	return bResult;
}


bool HDocument::Load(char* pstrFileName)
{
	bool bResult = false;
	long lfilesize = 0;
	int fh;
	PARSEINFO pi;
	char *buffer = NULL,*pstrTemp = NULL;
#ifdef WIN32
   fh = _open( pstrFileName, _O_RDONLY );
#else
   fh = open( pstrFileName, O_RDONLY );
#endif
	if( fh == -1 )
		return bResult;

#ifdef WIN32
   lfilesize = _filelength(fh);
#else
   struct stat fileinfo;
   memset(&fileinfo,0,sizeof(struct stat));
   fstat(fh,&fileinfo);
   lfilesize = fileinfo.st_size;
#endif

	if( lfilesize == 0)
		return bResult;

	buffer = new char[lfilesize+1];
//	if(_read(fh,buffer,lfilesize) <= 0)
//	{
//		   delete[] buffer;
//		   return bResult;
//	}
	int tlen=0,lensize=100,readed=0;
	while(1)
	{
#ifdef WIN32
      readed = _read(fh,buffer+tlen,lensize);
#else
      readed = (int)read(fh,buffer+tlen,lensize);
#endif
		if(readed < 0)
			tlen = 0;
		else
			tlen += readed;
		if(readed <= 0 )
			break;
	}
#ifdef WIN32
   _close( fh );
#else
   close( fh );
#endif



	m_pRootNode = (HNode*)this;
	m_pOwnerDoc = this;

	//pi.erorr_occur = false;
	pstrTemp = (char*)HNode::LoadXML(buffer,&pi);
	if(pi.erorr_occur == false)
		bResult = true;
	else
	{
		m_pRootNode = NULL;
		m_pOwnerDoc = NULL;
	}
	m_pRootNode = (HNode*)this;
	delete[] buffer;
	return bResult;
}


bool HDocument::Save(char* pstrFileName)
{
	HString str;
	bool bResult = true;
	FILE *pFile;

	if(!GetXML(str))
		return false;

	int  nSize = str.GetLength();
	pFile = fopen(pstrFileName,"wb");
	if(!pFile) return false;

	if((int)fwrite((const char*)str,sizeof(const char),nSize,pFile) != nSize)
	{
		fclose(pFile);
		return false;
	}
	fclose(pFile);
	return bResult;
}


