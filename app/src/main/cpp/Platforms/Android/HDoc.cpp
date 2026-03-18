// HDoc.cpp: implementation of the CHDoc class.
//
//////////////////////////////////////////////////////////////////////
#include "sGLDefine.h"
#include "HDoc.h"
#include "hstring.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

NodeData* MakeNodeData(HNode* pNode)
{
    NodeData* pNodeData = NULL;
    const char* sName = pNode->GetName();
    
    //HLogD("<%s>%s\n",sName,(const char*)pNode->GetText());
    
    if (strcmp(sName, "array") == 0)
    {
        pNodeData = new NodeArray(pNode);
    }
    else if(strcmp(sName, "dict") == 0)
    {
        pNodeData = new NodeDic(pNode);
    }
    else
    {
        NodeDataType type = NodeDataNone;
        if(strcmp(sName, "integer") == 0)
        {
            type = NodeDataInt;
        }
        else if(strcmp(sName, "string") == 0)
        {
            type = NodeDataSring;
        }
        else if(strcmp(sName, "real") == 0)
        {
            type = NodeDataReal;
        }
        else
        {
            HLogE("[ERROR] Not Found Tag %s\n",sName);
        }
        pNodeData = new NodeDataValue(pNode->GetText(),type);
    }
    
    
    return pNodeData;
}

NodeData::NodeData()
{
}
NodeData::~NodeData()
{
}

NodeDataValue::NodeDataValue(const char* sData,NodeDataType v)
{
    mType = v;
    mData = sData;
}

NodeDataValue::~NodeDataValue()
{
}

NodeArray::NodeArray(HNode* pNode)
{
    if(pNode)
    {
        HNodes* pList = pNode->GetChilds();
        if(pList)
        {
            ByNode<HNode>* be = pList->begin();
            for(ByNode<HNode>* it = be; it != pList->end();)
            {
                HNode* pNode = it->GetObject();
                mList.push_back(MakeNodeData(pNode));
                it = it->GetNextNode();
            }
        }
    }
}

NodeArray::~NodeArray()
{
    mList.clear();
}

NodeDic::NodeDic(HNode* pNode)
{
    string sKey;
    NodeData* pNodeData;
    HNodes* pList = pNode->GetChilds();
    if(pList)
    {
        int nCnt = pList->Size();
        for(int i =0; i < nCnt;)
        {
            HNode* pNode = pList->GetAt(i++);
            //key
            sKey = pNode->GetText();
            //Value
            pNode = pList->GetAt(i++);
            pNodeData = MakeNodeData(pNode);
            mMap[sKey] = pNodeData;
        }
    }
}

NodeDic::~NodeDic()
{
    mMap.clear();
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHDoc::CHDoc()
{
    mpDic = NULL;
}

CHDoc::~CHDoc()
{
    if(mpDic)
    {
        delete mpDic;
        mpDic = NULL;
    }
}


bool CHDoc::Load(const char* sFileName)
{
    if(mpDic)
    {
        delete mpDic;
        mpDic = NULL;
    }
    
    //---------------------------------------------------------
    //PropertyList.plist 는 Key,Array로 반드시 구성하자.
    //---------------------------------------------------------
    HDocument* pHDoc = new HDocument;
    if(((HDocument*)pHDoc)->Load((char*)sFileName) == false)
    {
        HLogE("[ERROR] Property Load failed %s\n",sFileName);
        
        delete pHDoc;
        return false;
    }
    
    HNode* pRoot = pHDoc->GetRootNode();
    int nCnt = pRoot->GetChildCnt();
    
    if(nCnt == 1)
    {
        mpDic = (NodeDic*)MakeNodeData(pRoot->GetChild(0));
    }
    else
    {
        HLogE("[ERROR]Plist 파일이 아닙니다.\n");
    }
    
//    pHDoc->HTRACE();
    
    delete pHDoc;
    return true;
}

bool CHDoc::LoadXML(const char* sXML)
{
    if(mpDic)
    {
        delete mpDic;
        mpDic = NULL;
    }
    
    //---------------------------------------------------------
    //PropertyList.plist 는 Key,Array로 반드시 구성하자.
    //---------------------------------------------------------
    HDocument* pHDoc = new HDocument;
    if(((HDocument*)pHDoc)->LoadXML((char*)sXML) == false)
    {
        HLogE("[ERROR] Property Load failed %s\n",sXML);
        delete pHDoc;
        return false;
    }
    
    HNode* pRoot = pHDoc->GetRootNode();
    int nCnt = pRoot->GetChildCnt();
    
    if(nCnt == 1)
    {
        mpDic = (NodeDic*)MakeNodeData(pRoot->GetChild(0));
    }
    else
    {
        HLogE("[ERROR]Plist 파일이 아닙니다.\n");
    }
    delete pHDoc;
    return true;
}

NodeData* CHDoc::GetData(const char* sKey)
{
    if(mpDic == NULL) return NULL;
    
#ifdef NOTOPENGL
    map<string, NodeData*>::iterator b = mpDic->mMap.begin();
    for(map<string, NodeData*>::iterator  it = b; it != mpDic->mMap.end(); it++)
    {
        HLogD("id =  %s\n",it->first.c_str());
    }
#endif
    
    return mpDic->mMap[string(sKey)];
}


void CHDoc::SetData(const char* sKey,NodeData* pNodeData)
{
    NodeData* pTemp;
    if(mpDic == NULL) return;
    pTemp = mpDic->mMap[sKey];
    if(pTemp) //기존의 데이터를 지운다.
    {
        delete pTemp;
        pTemp = NULL;
    }
    mpDic->mMap[sKey] = pNodeData;
}


bool SaveNodeData(FILE *pFile,NodeData* pNodeData)
{
 
    string sTag;
    int  nSize;
    
    if(pNodeData == NULL)
    {
        HLogE("[ERROR] SaveNodeData Is NULL\n");
        return false;
    }
    
    NodeDataType type = pNodeData->GetType();
    if(type == NodeDataArray)
    {
        sTag = "<array>\n";
        nSize = (int)strlen(sTag.c_str());
        if((int)fwrite(sTag.c_str(),sizeof(char),nSize,pFile) != nSize)
        {
            return false;
        }
        NodeArray *pArr = (NodeArray*)pNodeData;
        nSize = (int)pArr->mList.size();
        for(int i = 0; i < nSize; i++)
        {
            if(!SaveNodeData(pFile, pArr->mList[i]))
                return false;
                
        }
        sTag = "</array>\n";
        nSize = (int)strlen(sTag.c_str());
        if((int)fwrite(sTag.c_str(),sizeof(char),nSize,pFile) != nSize)
        {
            return false;
        }
    }
    else if(type == NodeDataDic)
    {
        sTag = "<dict>\n";
        nSize = (int)strlen(sTag.c_str());
        if((int)fwrite(sTag.c_str(),sizeof(char),nSize,pFile) != nSize)
        {
            return false;
        }
      
        
        NodeDic *pDic = (NodeDic*)pNodeData;
        map<string, NodeData*>::iterator b = pDic->mMap.begin();
        map<string, NodeData*>::iterator e = pDic->mMap.end();
        
        for(map<string, NodeData*>::iterator it = b; it != e;it++)
        {
            
            const char* sKey = it->first.c_str();
            
            sTag = "<key>";
            nSize = (int)strlen(sTag.c_str());
            if((int)fwrite(sTag.c_str(),sizeof(char),nSize,pFile) != nSize)
            {
                return false;
            }
            
            nSize = (int)strlen(sKey);
            if((int)fwrite(sKey,sizeof(char),nSize,pFile) != nSize)
            {
                return false;
            }

            sTag = "</key>\n";
            nSize = (int)strlen(sTag.c_str());
            if((int)fwrite(sTag.c_str(),sizeof(char),nSize,pFile) != nSize)
            {
                return false;
            }
            
            if(!SaveNodeData(pFile, it->second))
                return false;
            
        }
        
        sTag = "</dict>\n";
        nSize = (int)strlen(sTag.c_str());
        if((int)fwrite(sTag.c_str(),sizeof(char),nSize,pFile) != nSize)
        {
            return false;
        }
    }
    else if(type == NodeDataInt)
    {
        sTag = "<integer>";
        nSize = (int)strlen(sTag.c_str());
        if((int)fwrite(sTag.c_str(),sizeof(char),nSize,pFile) != nSize)
        {
            return false;
        }
        
        NodeDataValue *pV = (NodeDataValue*)pNodeData;
        sTag = pV->GetString();
        nSize = (int)strlen(sTag.c_str());
        if((int)fwrite(sTag.c_str(),sizeof(char),nSize,pFile) != nSize)
        {
            return false;
        }
        
        sTag = "</integer>\n";
        nSize = (int)strlen(sTag.c_str());
        if((int)fwrite(sTag.c_str(),sizeof(char),nSize,pFile) != nSize)
        {
            return false;
        }
    }
    else if(type == NodeDataSring)
    {
        sTag = "<string>";
        nSize = (int)strlen(sTag.c_str());
        if((int)fwrite(sTag.c_str(),sizeof(char),nSize,pFile) != nSize)
        {
            return false;
        }
        
        NodeDataValue *pV = (NodeDataValue*)pNodeData;
        sTag = pV->GetString();
        nSize = (int)strlen(sTag.c_str());
        if((int)fwrite(sTag.c_str(),sizeof(char),nSize,pFile) != nSize)
        {
            return false;
        }
        
        sTag = "</string>\n";
        nSize = (int)strlen(sTag.c_str());
        if((int)fwrite(sTag.c_str(),sizeof(char),nSize,pFile) != nSize)
        {
            return false;
        }
    }
    else if(type == NodeDataReal)
    {
        sTag = "<real>";
        nSize = (int)strlen(sTag.c_str());
        if((int)fwrite(sTag.c_str(),sizeof(char),nSize,pFile) != nSize)
        {
            return false;
        }
        
        NodeDataValue *pV = (NodeDataValue*)pNodeData;
        sTag = pV->GetString();
        nSize = (int)strlen(sTag.c_str());
        if((int)fwrite(sTag.c_str(),sizeof(char),nSize,pFile) != nSize)
        {
            return false;
        }
        
        sTag = "</real>\n";
        nSize = (int)strlen(sTag.c_str());
        if((int)fwrite(sTag.c_str(),sizeof(char),nSize,pFile) != nSize)
        {
            return false;
        }
    }
    
    return true;
}

bool CHDoc::Save(const char* sFileName)
{
    string sTag;
    int nSize = 0;
    FILE *pFile;
    
    pFile = fopen(sFileName,"wb");
	if(!pFile)
    {
        HLogE("Save Failed UserInfo %s\n",sFileName);
        fclose(pFile);
        return false;
    }
    
    sTag = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    nSize = (int)strlen(sTag.c_str());
    if((int)fwrite(sTag.c_str(),sizeof(char),nSize,pFile) != nSize)
    {
        fclose(pFile);
        return false;
    }
    
    sTag = "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n<plist version=\"1.0\">";
    nSize = (int)strlen(sTag.c_str());
    if((int)fwrite(sTag.c_str(),sizeof(char),nSize,pFile) != nSize)
    {
        fclose(pFile);
        return false;
    }
	   
    if(!SaveNodeData(pFile,mpDic))
    {
        HLogE("Write Failed UserInfo %s\n",sFileName);
        fclose(pFile);
        return false;
    }
    
    
    sTag = "\n</plist>";
    nSize = (int)strlen(sTag.c_str());
    if((int)fwrite(sTag.c_str(),sizeof(char),nSize,pFile) != nSize)
    {
        fclose(pFile);
        return false;
    }
    
	fclose(pFile);
    return true;
}