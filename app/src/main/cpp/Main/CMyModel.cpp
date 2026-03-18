//
//  CMyModel.cpp
//  SongGL
//
//  Created by 호학 송 on 2014. 1. 28..
//
//
#include <iostream>
#include "CMyModel.h"

CMyModel::CMyModel()
{
    pfinalmulMaxtix = NULL;
    memset(orientation, 0, sizeof(orientation));
    memset(position, 0, sizeof(position));
    memset(scale, 0, sizeof(scale));
}

CMyModel::~CMyModel()
{
    if(pfinalmulMaxtix)
	{
		delete[] pfinalmulMaxtix;
		pfinalmulMaxtix = 0;
	}
}