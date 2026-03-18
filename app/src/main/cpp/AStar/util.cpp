//-----------------------------------------------------------------------------
/** @file util.cpp
    @see util.h

    $Id: util.cpp,v 1.4 2003/06/27 17:55:36 emarkus Exp $
    $Source: /usr/cvsroot/project_pathfind/util.cpp,v $
*/
//-----------------------------------------------------------------------------

#include "util.h"
#include "sGLDefine.h"
#include <sstream>

using namespace std;
using namespace PathFind;

//-----------------------------------------------------------------------------

ReadError::ReadError(int line, const string& info)
{
    ostringstream s;
    s << "Read error line " << line << ": " << info << ends;
    setMessage(s.str());
    
}

//-----------------------------------------------------------------------------

LineReader::LineReader(istream& in)
    : m_lineNumber(0),
      //m_in(in),
      m_lstMap(0)
{
    m_pin = &in;
}
//#include "sGL.h"
LineReader::LineReader(const char* sMap)
: m_lineNumber(0),m_pin(0)
{
    char* sBefore;
    char* sFnd;
    char* sOrgMap = new char[strlen(sMap) + 1];
    strcpy(sOrgMap,sMap);
    m_lstMap = new vector<string>;
    
    sBefore = (char*)sOrgMap;
    sFnd = strstr(sOrgMap,"\n");
    while(sFnd)
    {
        *sFnd = 0;
        m_lstMap->push_back(string(sBefore));
        sBefore = sFnd + 1;
        sFnd = strstr(sFnd + 1,"\n");
    }
    
    if(sBefore)
    {
        m_lstMap->push_back(string(sBefore));
    }
    
    delete[] sOrgMap;
}

LineReader::~LineReader()
{
    if(m_lstMap)
    {
        delete m_lstMap;
        m_lstMap = 0;
    }
}

Error LineReader::createError(const string& message)
{
    ostringstream out;
    out << "Line " << m_lineNumber << ": " << message << ends;
    return Error(out.str());
}

std::string LineReader::readLine()
{
    
    
    if(m_lstMap == 0)
    {
        char buffer[MAX_LINE];
        memset(buffer, 0, sizeof(buffer));
        
//        m_in.getline(buffer, MAX_LINE);
//        if (! m_in)
//            throw Error("Unexpected end of stream.");
        m_pin->getline(buffer, MAX_LINE);
        if (! m_pin)
        {
            //throw Error("Unexpected end of stream.");
            HLogE("[ERROR]Unexpected end of stream.");
            return "";
            
        }
        ++m_lineNumber;
        return string(buffer);

    }
    else
    {
        return m_lstMap->at(m_lineNumber++);
    }
    
}

//-----------------------------------------------------------------------------

char PathFind::getVisitedNodeLabel(int iteration)
{
    char label;
    if (iteration < 10)
        label = ('0' + iteration);
    else if (iteration <= 36)
        label = ('a' + iteration - 10);
    else
        label = '+';
    return label;
}

//-----------------------------------------------------------------------------
