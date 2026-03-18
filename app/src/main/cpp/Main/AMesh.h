/*
 *  IMesh.h
 *  SongGL
 *
 *  Created by 호학 송 on 10. 11. 10..
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#include "ListNode.h"
#include "sGLDefine.h"

#ifndef _AMESH_H
#define _AMESH_H

class AMesh
{
public:

	virtual ~AMesh(){};
	
    virtual CListNode<SGLResource>* GetResources() = 0;
	virtual int Size() = 0;
	virtual SGLResource* GetAt(int nIndex) = 0;
};
#endif //_AMESH_H