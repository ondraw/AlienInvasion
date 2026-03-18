//
//  CNumberCtl.cpp
//  SongGL
//
//  Created by 송 호학 on 11. 12. 8..
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//
#include <stdlib.h>
#include "CNumberCtl.h"
#include "CButtonCtl.h"
#include "CScenario.h"

CNumberCtl::CNumberCtl(CControl* pParent,CTextureMan *pTextureMan,bool bBold) : CLabelCtl(pParent,pTextureMan)
{
    mbBoldType = bBold;
    //memset(msNumber, 0, sizeof(msNumber));
    mnNumberHeight = 0;
    mnNumberWidth = 0;
    strcpy(msNumber, "0");
    mnAlign = ALIGN_LEFT; //Left : 0 , Right : 1
    mButtonCtl = 0;
    mbButtonType = false;
    
    moffsetx = 0;
    moffsety = 0;
    mPlusMark = CNumberCtlType_Normal;
    
}

CNumberCtl::~CNumberCtl()
{
    
}

CControl* CNumberCtl::Clone(CControl* pNew)
{
    CNumberCtl* pNew2;
    if(pNew == NULL)
        pNew = new CNumberCtl(GetParent(),GetTextureMan());
    
    pNew2 = (CNumberCtl*)pNew;
    
    
    strcpy(pNew2->msNumber,msNumber);
    pNew2->mnNumberHeight = mnNumberHeight;
    pNew2->mnNumberWidth = mnNumberWidth;
    pNew2->mnAlign = mnAlign;
    pNew2->mbButtonType= mbButtonType;
    
    pNew2->moffsetx= moffsetx;
    pNew2->moffsety= moffsety;
    pNew2->mPlusMark= mPlusMark;
    pNew2->mbBoldType= mbBoldType;
    
    //어떤것인지 알수가 없다.
    pNew2->mButtonCtl = NULL;
    
    return CLabelCtl::Clone(pNew);
}

void CNumberCtl::ClearNumber()
{
    strcpy(msNumber, "");
    RemoveAll();
}


void CNumberCtl::SetNumber(int nNumber,CNumberCtlType PlusMark )
{
    vector<string> listImage;
    int nLen = 0;
    char sNumb[3];
    sNumb[1] = 0;
    sNumb[2] = 0;
    if(nNumber < 0) return ;
    
    mPlusMark = PlusMark;
    
    if(mPlusMark == CNumberCtlType_Normal)
        sprintf(msNumber, "%d",nNumber);
    else if(mPlusMark == CNumberCtlType_Plus)
        sprintf(msNumber, "+%d",nNumber);
    else if(mPlusMark == CNumberCtlType_Minus)
        sprintf(msNumber, "-%d",nNumber);
    else if(mPlusMark == CNumberCtlType_Bracket)
        sprintf(msNumber, "(%d)",nNumber);
    else if(mPlusMark == CNumberCtlType_Percent)
        sprintf(msNumber, "%d%%",nNumber);
    else
        sprintf(msNumber, "%d",nNumber);
        
    nLen = (int)strlen(msNumber);
    
    RemoveAll();  //차일드를 다지운다.
    
    if(mbButtonType) //그리는 부분이 없고 영역만 존재하는 버튼 컨트롤을 만들어서 이벤트를 받는다.
    {
        vector<string>  lstNone;
        lstNone.push_back("none");
        lstNone.push_back("none");

        
        CControl* pCon = new CButtonCtl(this,mpTextureMan);
        pCon->Initialize(GetID(), mfDPosX, mfDPosY, mfDWidth, mfDHeight, lstNone,0.f,0.f,1.f,1.f);
        AddControl(pCon);
        mButtonCtl = pCon;
    }

    
    float fDigitWidthRate = mbBoldType ? 0.62f : 0.7f;
    int nNumberWidth = (mnNumberWidth - moffsetx*2) * fDigitWidthRate;
    int nNumberHeight = mnNumberHeight - moffsety*2;
    int nRateWidth = mnNumberWidth * fDigitWidthRate;
    
    if (mnAlign == ALIGN_LEFT) //좌측정렬
    { 
        float nPosX = mfDPosX + moffsetx;
        float nPosY = mfDPosY + moffsety;
        
        for (int i = 0; i < nLen; i++) 
        {
            listImage.clear();
            if(mbBoldType == false)
                sNumb[0] = msNumber[i];
            else
            {
                sNumb[0] = 'b';
                sNumb[1] = msNumber[i];
            }
            listImage.push_back("none");
            listImage.push_back(sNumb);
            CControl* pNumCtl = new CLabelCtl(this,mpTextureMan);
            pNumCtl->Initialize((int)(long)pNumCtl, nPosX+nRateWidth*i, nPosY, nNumberWidth, nNumberHeight, listImage,0.f,0.f,1.f,1.f);
            pNumCtl->SetTextColor(mfTextColor);
            AddControl(pNumCtl);
        }
    }
    else if(mnAlign == ALIGN_RIGHT)//우측정렬
    {
        
        float nPosX = mfDPosX + mfDWidth - moffsetx;
        float nPosY = mfDPosY + moffsety;
        for (int i = nLen - 1; i >= 0; i--) 
        {
            listImage.clear();
            if(mbBoldType == false)
                sNumb[0] = msNumber[(nLen - 1)- i];
            else
            {
                sNumb[0] = 'b';
                sNumb[1] = msNumber[(nLen - 1)- i];
            }
            listImage.push_back("none");
            listImage.push_back(sNumb);
            CControl* pNumCtl = new CLabelCtl(this,mpTextureMan);
            pNumCtl->Initialize((int)(long)pNumCtl, nPosX - (nRateWidth*(i+1)), nPosY, nNumberWidth, nNumberHeight, listImage,0.f,0.f,1.f,1.f);
            pNumCtl->SetTextColor(mfTextColor);
            AddControl(pNumCtl);
        }
    }
    else //가운데 정렬
    {
        float nPosX = mfDPosX + moffsetx;
        float nPosY = mfDPosY + moffsety;
        
        //숫자가 너크면 그냥 가운데 정렬 할수 있게 조절해준다.
        float fD =  mfDWidth - nNumberWidth * nLen;
        if(fD <= 0)
            nPosX = mfDPosX +  (mfDWidth - (nRateWidth * nLen)) / 2.0f + moffsetx;
        else
        {
            float fS = mfDPosX + fD/2.f;
            float fW = mfDWidth - fD;
            nPosX = fS +  (fW - (nRateWidth * nLen)) / 2.0f + moffsetx;
        }
            
        
        for (int i = 0; i < nLen; i++) 
        {
            listImage.clear();
            if(mbBoldType == false)
                sNumb[0] = msNumber[i];
            else
            {
                sNumb[0] = 'b';
                sNumb[1] = msNumber[i];
            }
            listImage.push_back("none");
            listImage.push_back(sNumb);
            CControl* pNumCtl = new CLabelCtl(this,mpTextureMan);
            pNumCtl->Initialize((int)(long)pNumCtl, nPosX+nRateWidth*i, nPosY, nNumberWidth, nNumberHeight, listImage,0.f,0.f,1.f,1.f);
            pNumCtl->SetTextColor(mfTextColor);
            AddControl(pNumCtl);
        }
    }
}


void CNumberCtl::SetNumberF(float fNumber,CNumberCtlType PlusMark )
{
    vector<string> listImage;
    int nLen = 0;
    char sNumb[10];
    sNumb[1] = 0;
    sNumb[2] = 0;
    memset(sNumb, 0, sizeof(sNumb));
    if(fNumber < 0.f) return ;
    
    mPlusMark = PlusMark;
    
    if(mPlusMark == CNumberCtlType_Normal)
        sprintf(msNumber, "%.2f",fNumber);
    else if(mPlusMark == CNumberCtlType_Plus)
        sprintf(msNumber, "+%.2f",fNumber);
    else if(mPlusMark == CNumberCtlType_Minus)
        sprintf(msNumber, "-%.2f",fNumber);
    else if(mPlusMark == CNumberCtlType_Bracket)
        sprintf(msNumber, "(%.2f)",fNumber);
    else if(mPlusMark == CNumberCtlType_Percent)
        sprintf(msNumber, "%.2f%%",fNumber);
    else
        sprintf(msNumber, "%.2f",fNumber);
    
    nLen = strlen(msNumber);
    
    RemoveAll();  //차일드를 다지운다.
    
    if(mbButtonType) //그리는 부분이 없고 영역만 존재하는 버튼 컨트롤을 만들어서 이벤트를 받는다.
    {
        vector<string>  lstNone;
        lstNone.push_back("none");
        lstNone.push_back("none");
        
        
        CControl* pCon = new CButtonCtl(this,mpTextureMan);
        pCon->Initialize(GetID(), mfDPosX, mfDPosY, mfDWidth, mfDHeight, lstNone,0.f,0.f,1.f,1.f);
        AddControl(pCon);
        mButtonCtl = pCon;
    }
    
    
    float fDigitWidthRate = mbBoldType ? 0.62f : 0.7f;
    int nNumberWidth = (mnNumberWidth - moffsetx*2) * fDigitWidthRate;
    int nNumberHeight = mnNumberHeight - moffsety*2;
    int nRateWidth = mnNumberWidth * fDigitWidthRate;
    
    bool bPoint = false;
    
    if (mnAlign == ALIGN_LEFT) //좌측정렬
    {
        float nPosX = mfDPosX + moffsetx;
        float nPosY = mfDPosY + moffsety;
        
        for (int i = 0; i < nLen; i++)
        {
            listImage.clear();
            if(mbBoldType == false)
                sNumb[0] = msNumber[i];
            else
            {
                sNumb[0] = 'b';
                sNumb[1] = msNumber[i];
            }
            
            
            
            listImage.push_back("none");
            listImage.push_back(sNumb);
            CControl* pNumCtl = new CLabelCtl(this,mpTextureMan);
            if(bPoint == false)
            {
                pNumCtl->Initialize((int)(long)pNumCtl, nPosX+nRateWidth*i, nPosY, nNumberWidth, nNumberHeight, listImage,0.f,0.f,1.f,1.f);
                if(msNumber[i] == '.')
                    bPoint = true;
                else
                    bPoint = false;
            }
            else
            {
                nPosX -= nRateWidth / 2.0f;
                pNumCtl->Initialize((int)(long)pNumCtl, nPosX+nRateWidth*i, nPosY, nNumberWidth, nNumberHeight, listImage,0.f,0.f,1.f,1.f);
                bPoint = false;
            }
            pNumCtl->SetTextColor(mfTextColor);
            AddControl(pNumCtl);
        }
    }
    else if(mnAlign == ALIGN_RIGHT)//우측정렬
    {
        
        float nPosX = mfDPosX + mfDWidth - moffsetx;
        float nPosY = mfDPosY + moffsety;
        for (int i = nLen - 1; i >= 0; i--)
        {
            listImage.clear();
            if(mbBoldType == false)
                sNumb[0] = msNumber[(nLen - 1)- i];
            else
            {
                sNumb[0] = 'b';
                sNumb[1] = msNumber[(nLen - 1)- i];
            }
            listImage.push_back("none");
            listImage.push_back(sNumb);
            CControl* pNumCtl = new CLabelCtl(this,mpTextureMan);
            if(bPoint == false)
            {
                pNumCtl->Initialize((int)(long)pNumCtl, nPosX - (nRateWidth*(i+1)), nPosY, nNumberWidth, nNumberHeight, listImage,0.f,0.f,1.f,1.f);
                if(msNumber[i] == '.')
                    bPoint = true;
                else
                    bPoint = false;

                
            }
            else
            {
                nPosX -= nRateWidth / 2.0f;
                pNumCtl->Initialize((int)(long)pNumCtl, nPosX - (nRateWidth*(i+1)), nPosY, nNumberWidth, nNumberHeight, listImage,0.f,0.f,1.f,1.f);
                bPoint = false;
            }
            pNumCtl->SetTextColor(mfTextColor);
            AddControl(pNumCtl);
        }
    }
    else //가운데 정렬
    {
        float nPosX = mfDPosX + moffsetx;
        float nPosY = mfDPosY + moffsety;
        
        //숫자가 너크면 그냥 가운데 정렬 할수 있게 조절해준다.
        float fD =  mfDWidth - nNumberWidth * nLen;
        if(fD <= 0)
            nPosX = mfDPosX +  (mfDWidth - (nRateWidth * nLen)) / 2.0f + moffsetx;
        else
        {
            float fS = mfDPosX + fD/2.f;
            float fW = mfDWidth - fD;
            nPosX = fS +  (fW - (nRateWidth * nLen)) / 2.0f + moffsetx;
        }
        
        
        for (int i = 0; i < nLen; i++)
        {
            listImage.clear();
            if(mbBoldType == false)
                sNumb[0] = msNumber[i];
            else
            {
                sNumb[0] = 'b';
                sNumb[1] = msNumber[i];
            }
            listImage.push_back("none");
            listImage.push_back(sNumb);
            CControl* pNumCtl = new CLabelCtl(this,mpTextureMan);
            if(bPoint == false)
            {
                pNumCtl->Initialize((int)(long)pNumCtl, nPosX+nRateWidth*i, nPosY, nNumberWidth, nNumberHeight, listImage,0.f,0.f,1.f,1.f);
                if(msNumber[i] == '.')
                    bPoint = true;
                else
                    bPoint = false;

            }
            else
            {
                nPosX -= nRateWidth / 2.0f;
                pNumCtl->Initialize((int)(long)pNumCtl, nPosX+nRateWidth*i, nPosY, nNumberWidth, nNumberHeight, listImage,0.f,0.f,1.f,1.f);
                bPoint = false;
            }
            pNumCtl->SetTextColor(mfTextColor);
            AddControl(pNumCtl);
        }
    }
}


int CNumberCtl::Initialize(int nControlID,float x, float y, float nWidth,float nHeight,float nNumberWidth,float offsetx,float offsety,vector<string>& lstImage,bool bButton)
{
    vector<string>  lstModel;
    mnNumberHeight = nHeight;
    mnNumberWidth = nNumberWidth;
    mbButtonType= bButton;
    
    moffsetx = offsetx;
    moffsety = offsety;
    return CLabelCtl::Initialize(nControlID, x, y, nWidth, nHeight, lstImage,0.f,0.f,1.f,1.f);
}


int CNumberCtl::GetNumber()
{
    if(mPlusMark != CNumberCtlType_Normal)
        return atoi(msNumber + 1);
    return atoi(msNumber);
}


float CNumberCtl::GetNumberF()
{
    if(mPlusMark != CNumberCtlType_Normal)
        return atof(msNumber + 1);
    return atof(msNumber);
}

