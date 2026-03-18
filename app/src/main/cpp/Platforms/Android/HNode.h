// HNode.h: interface for the HNode class.
//
//////////////////////////////////////////////////////////////////////
// Revision Log
//
// Date       Who SAR     Notes
// ========== === ======= =====================================
// 1-26-20-3- sho         Initial coding by Song Ho Hak
//                        ThinkM Software - thinkm@hhsong.co.kr
// 
//

#if !defined(AFX_HNODE_H__B8B3B585_A5BD_46B8_933A_DCF599CCBC53__INCLUDED_)
#define AFX_HNODE_H__B8B3B585_A5BD_46B8_933A_DCF599CCBC53__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string.h>
#include "hstring.h"
#include "ListNode.h"


// Entity Encode/Decode Support
typedef struct _tagXmlEntity
{
	char entity;					// entity ( & " ' < > )
	char ref[10];					// entity reference ( &amp; &quot; etc )
	int ref_len;					// entity reference length
}XENTITY,*LPXENTITY;

typedef enum 
{
	PIE_PARSE_WELFORMED	= 0,
	PIE_ALONE_NOT_CLOSED,
	PIE_NOT_CLOSED,
	PIE_NOT_NESTED,
	PIE_ATTR_NO_VALUE
}PCODE;

// Parse info.
typedef struct _tagParseInfo
{
	bool		trim_value;			// [set] do trim when parse?
	bool		entity_value;		// [set] do convert from reference to entity? ( &lt; -> < )

	char*		xml;				// [get] xml source
	bool		erorr_occur;		// [get] is occurance of error?
	char*		error_pointer;		// [get] error position of xml source
	PCODE		error_code;			// [get] error code
	HString		error_string;		// [get] error string

	_tagParseInfo() 
	{ 
		trim_value = false; 
		entity_value = true; 
		xml = NULL; 
		erorr_occur = false; 
		error_pointer = NULL; 
		error_code = PIE_PARSE_WELFORMED; 
	}
}PARSEINFO,*LPPARSEINFO;

class HNode;
class HElement;
class HDocument;
class HElementData;

typedef CListNode<HNode> HNodes;
typedef CListNode<HElementData> HElementDatas;

class HElementData {
	friend class HNode;
	friend class HElement;
public:
	HElementData();
	~HElementData();

	HString& GetAttName();
	HString& GetAttValue();

	void SetAttName(char* pstrName) { m_strAttName = pstrName;};
	void SetAttValue(char* pstrVal) { m_strAttValue = pstrVal;};
protected:
	HString& GetDeCodeAttValue();

private:
	HString		m_strAttName;
	HString		m_strAttValue;	
	HString		m_strDeGod;
};

class HElement
{
	friend class HNode;
public:
	HElement();
	~HElement();

	int  GetCnt();
	HElementData*  GetAt(int i );
	HElementData* operator [] ( int i ) { return GetAt(i); };
	bool Compare(char* attName,char* AttVal);
	void AddElm(char* AddName,char* AttValue);
	bool DelElm(char* AddName);


protected:
	void AddElm(HElementData* pElms);
protected:
	HElementDatas*	m_pElms;

};

class HNode  
{
	friend class HDocument;
public:
	HNode(const char* pName,const char* pTxt = NULL);
	HNode();
	virtual ~HNode();
	virtual const char* LoadXML(char* pstrXMLData,LPPARSEINFO pi);		
	virtual const char* LoadXMLIN(char* pstrXMLData,LPPARSEINFO pi);		
	virtual CListNode<HNode>* GetChilds() { return m_pChildNodes; };
	virtual HNode* GetChild(int i);	
	virtual int GetChildCnt();
	virtual bool AppendChildFirst(HNode* pNode);
	virtual bool AppendChild(HNode*    pNode);
	virtual bool AppendChild(HNodes* pNodes);
	//virtual bool AppendChildDontDoc(HNode* pNode);
	/////////////////////////////////////////////////////////////
	virtual bool AppendElement(HElementData* pElementData);	
	virtual bool AppendElement(const char* strName,const char* strValue);
	virtual bool AppendElement(const char* strName,long lValue);
	virtual bool AppendElement(const char* strName,int nValue);
	////////////////////////////////////////////////////////////

	virtual bool RemoveChild(HNode* pNode);		
	virtual void RemoveChilds();
	virtual bool GetXML(HString& str,int depth=0);	
	virtual bool GetXMLIN(HString& str,int depth);	
	virtual bool GetOrgXML(HString& str);
	virtual bool GetOrgXMLIN(HString& str);
	virtual bool GetOrgXML(HString* pstr);
	virtual bool GetOrgXMLIN(HString* pstr);

	HNode* operator [] ( int i ) { return GetChild(i); }
	virtual bool RemoveElement(char* pstrAttName);	
	virtual HNode* FindNode(char* pstrFindNode);	
	virtual HNodes* FindNodes(char* pstrFindNode);
	virtual bool SetText(char* pstrText);	
	virtual HDocument* GetDoc() const { return m_pOwnerDoc;};
	virtual HString& GetName();
	virtual void SetName(const char* pstrText);
	virtual HString& GetText();
	virtual HNode*   GetParentNode() const { return m_pParentNode;};
	virtual HElementDatas*	GetElementDatas();
	virtual HElement*		GetElement() { return m_pElement;};
	virtual const char*			GetElementToName(const char* pstrName);
	virtual bool			GetElementToName(const char* pstrName,long* plOut);
	virtual bool			GetElementToName(const char* pstrName,int* pnOut);
	virtual bool			GetElementToName(const char* pstrName,bool* pbOut);

	virtual void SetDoc(HDocument* pDoc) {  m_pOwnerDoc = pDoc;};

	HNode* Clone();
	HNode* CloneOnly();

	bool HTRACE(int depth=0);
	bool HTRACEP(int depth = 0);
protected:
	//존재하면 수정하고 존재하지 않으면 Add한다.
	virtual bool SetAtt(char* pstrAttName,char* pstrAttValue);
	HNode* FindNode(HString* NodeName,HString* AttName,HString* AttValue,int depth,
		char* pstrNext);
	HNodes* FindNodes(HString* NodeName,HString* AttName,HString* AttValue,int depth,
		char* pstrNext);
	void SetDocToChildNode(HNode* pNode,HDocument* pDoc);
	void Clone(HNode* pNowNode,HNode* pNewNode,bool bOnly=false);
protected:
	bool    Init();
	bool	ChildClear();
	char* LoadAttributes( const char* pszAttrs , LPPARSEINFO pi);
	HString& GetDeCodeText();
protected:
	HDocument*			m_pOwnerDoc;

protected:	
	HString				m_strTagName;
	HString				m_strText;
	HString				m_strXMLVer;

	HElement*			m_pElement;
	HNodes*				m_pChildNodes;	
	HNode*				m_pParentNode;
private:
	HString				m_strDeGod;
};


class HDocument : public HNode
{
public:
	HDocument();
	~HDocument();

	bool LoadXML(char *pstrXMLData);	
	bool Load(char* pstrFileName);	
	bool Save(char* pstrFileName);

	HNode* CreateNode(char* TagName,char* Text=NULL);
	HNode* GetRootNode() const { return m_pRootNode;};
private:
	HNode* m_pRootNode;
};


#endif // !defined(AFX_HNODE_H__B8B3B585_A5BD_46B8_933A_DCF599CCBC53__INCLUDED_)
