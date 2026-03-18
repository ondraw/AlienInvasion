// HDoc.h: interface for the CHDoc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HDOC_H__8C0DBB9D_9320_4C96_AEB9_4AA133B778CB__INCLUDED_)
#define AFX_HDOC_H__8C0DBB9D_9320_4C96_AEB9_4AA133B778CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HNode.h"
#include <vector>
#include <map>
#include <string>
using namespace std;

enum NodeDataType { NodeDataNone = 1,NodeDataDic,NodeDataArray = 3,NodeDataInt,NodeDataSring,NodeDataReal};
    
class NodeData
{
public:
    NodeData();
    virtual ~NodeData();
    virtual NodeDataType GetType() { return NodeDataNone;}
};

class NodeDataValue : public NodeData
{
public:
    NodeDataValue(const char* sData,NodeDataType v);
    virtual ~NodeDataValue();
    virtual NodeDataType GetType() { return mType;};
    
    virtual const char* GetString() { return mData.c_str();}
    virtual int GetInt() { return (int)strtol(mData.c_str(), 0, 10);}
    virtual float GetReal() { return strtof(mData.c_str(), 0);}
public:
    string mData;
    
protected:
    NodeDataType mType;
};

class NodeArray : public NodeData
{
public:
    NodeArray(HNode* pNode);
    virtual ~NodeArray();
    virtual NodeDataType GetType() { return NodeDataArray;}
    
public:
    vector<NodeData*> mList;
};

class NodeDic : public NodeData
{
public:
    NodeDic(HNode* pNode);
    virtual ~NodeDic();
    virtual NodeDataType GetType() { return NodeDataDic;};
    
public:
    map<string, NodeData*> mMap;
};

class CHDoc
{
public:
	CHDoc();
	virtual ~CHDoc();
    bool Load(const char* sFileName);
    bool LoadXML(const char* sXML);
    
    NodeData* GetData(const char* sKey);
    void SetData(const char* sKey,NodeData* pNodeData);
    bool Save(const char* sFileName);
    
protected:
    NodeDic* mpDic;
};

#endif // !defined(AFX_HDOC_H__8C0DBB9D_9320_4C96_AEB9_4AA133B778CB__INCLUDED_)
