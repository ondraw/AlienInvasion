//
//  CBombCell.cpp
//  SongGL
//
//  Created by 송 호학 on 12. 1. 5..
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//
#include "CBombCell.h"
#include "CButtonCtl.h"
#include "CHWorld.h"
#include "CNumberCtl.h"


CBombCell::CBombCell(CTextureMan *pTextureMan) : CCellCtl(pTextureMan)
{
    mpBombInfo = 0;
    mnBeforeCnt = 0;
}

CBombCell::~CBombCell()
{
    
}

int CBombCell::Initialize(USERINFO* pBombInfo,int nWidth,int nHeight,vector<string>& lstImage,float fsu,float fsv,float feu,float fev)
{
    vector<string> limg;
    vector<float> layout;
    
    mpBombInfo = pBombInfo;
    this->SetLData((long)pBombInfo);
    
    PROPERTYI::GetPropertyFloatList("W_Top_BombCellNum", layout);
    
    if(mpBombInfo->nType == 0) //영구적이 아니면 카운트가 존재한다.
    {
        limg.clear();
        limg.push_back("none");
        limg.push_back("none");
        CNumberCtl* pBmbCntClt = new CNumberCtl(this,mpTextureMan);
        pBmbCntClt->Initialize(0, layout[0], layout[1], layout[2],layout[3],layout[3],0,0,limg);
        float fColor[] = {1.0f,0.0f,0.0f,1.0f};
        pBmbCntClt->SetTextColor(fColor);
        pBmbCntClt->SetAlign(CNumberCtl::ALIGN_CENTER);
        pBmbCntClt->SetNumber(mpBombInfo->nCnt);
        mnBeforeCnt = mpBombInfo->nCnt;
        this->AddControl(pBmbCntClt);
    }
    return CCellCtl::Initialize(nWidth,nHeight,lstImage,fsu,fsv,feu,fev);
}

int CBombCell::RenderBegin(int nTime)
{
    CCellCtl::RenderBegin(nTime);
    
    //이전과 다르면 세로 설정을 해야한다.
    if (mpBombInfo->nType == 0 && mnBeforeCnt != mpBombInfo->nCnt) 
    {
        mnBeforeCnt = mpBombInfo->nCnt;
        ((CNumberCtl*)mlstChild[0])->SetNumber(mnBeforeCnt);
    }
    
    return E_SUCCESS;
}

