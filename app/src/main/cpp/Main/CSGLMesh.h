/*
 *  CMesh.h
 *  SongGL
 *
 *  Created by 호학 송 on 10. 11. 17..
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */


#include "sGLDefine.h"
#include "AMesh.h"

#ifndef _CMESH_H
#define _CMESH_H

class CSGLMesh : public AMesh
{
	
public:
	CSGLMesh(CListNode<SGLResource> *pResource);
	virtual ~CSGLMesh();
	
public:
	virtual int Size() { return mplstMesh->Size();}
	virtual SGLResource* GetAt(int nIndex) { return (*mplstMesh)[nIndex];}
	virtual CListNode<SGLResource>* GetResources() { return mplstMesh;}
	
protected:
	CListNode<SGLResource>	*mplstMesh;
};
#endif //_AMESH_H