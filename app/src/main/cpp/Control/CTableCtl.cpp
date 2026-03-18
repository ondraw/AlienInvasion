//
//  CTableCtl.cpp
//  SongGL
//
//  Created by 송 호학 on 11. 12. 4..
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include "CTableCtl.h"
#include "C2dStick.h"
#include "CScrollContentCtl.h"

CTableCtl::CTableCtl(CControl* pParent,CTextureMan *pTextureMan) : CScrollCtl(pParent,pTextureMan)
{
    
}

CTableCtl::~CTableCtl()
{
    
}

CControl* CTableCtl::GetRowCtl(int nRow)
{
    return mlstChild[0]->GetChild(nRow);
}

int CTableCtl::GetRowCnt()
{
    return mlstChild[0]->GetChilds()->size();
}

void CTableCtl::AddItem(CControl* pItemCtl)
{
    int nIndex = 0;
    float fContentHeight,fContentWidth;
    float fEndPosY;
    
    float fPosX,fPosY,fLastCellWidth,fLastCellHeight;
    float fItemWidth,fItemHeight;
    
    CList<CControl*>*Rows = mlstChild[0]->GetChilds();
    
    mlstChild[0]->GetSize(fContentWidth, fContentHeight);
    
    int nSize = Rows->size();
    if(nSize != 0)
    {
        nIndex = nSize - 1;
        (*Rows)[nIndex]->GetPosition(fPosX, fPosY);
        (*Rows)[nIndex]->GetSize(fLastCellWidth, fLastCellHeight);
        
        fEndPosY = fPosY + fLastCellHeight;
    }
    else
    {
        mlstChild[0]->GetPosition(fPosX, fPosY);
        fEndPosY = fPosY;
    }
    
    pItemCtl->MoveVector(fPosX, fEndPosY);    
    pItemCtl->GetSize(fItemWidth, fItemHeight);
    
    SetContentsBounds(fItemWidth,fContentHeight + fItemHeight);

    pItemCtl->SetParent(mlstChild[0]);
    pItemCtl->SetID(nSize); //현재 Row
    
    mlstChild[0]->AddControl(pItemCtl);
}


void CTableCtl::AddItemHorizon(CControl* pItemCtl)
{
    int nIndex = 0;
    float fContentHeight,fContentWidth;
    float fEndPosX;
    
    float fPosX,fPosY,fLastCellWidth,fLastCellHeight;
    float fItemWidth,fItemHeight;
    
    CList<CControl*>*Rows = mlstChild[0]->GetChilds();
    
    mlstChild[0]->GetSize(fContentWidth, fContentHeight);
    
    int nSize = Rows->size();
    if(nSize != 0)
    {
        nIndex = nSize - 1;
        (*Rows)[nIndex]->GetPosition(fPosX, fPosY);
        (*Rows)[nIndex]->GetSize(fLastCellWidth, fLastCellHeight);
        
        fEndPosX = fPosX + fLastCellWidth;
    }
    else
    {
        mlstChild[0]->GetPosition(fPosX, fPosY);
        fEndPosX = fPosX;
    }
    
    pItemCtl->MoveVector(fEndPosX, fPosY);    
    pItemCtl->GetSize(fItemWidth, fItemHeight);
    
    SetContentsBounds(fContentWidth+ fItemWidth,fItemHeight);
    
    pItemCtl->SetParent(mlstChild[0]);
    pItemCtl->SetID(mlstChild[0]->GetID() + nSize);
    
    mlstChild[0]->AddControl(pItemCtl);  
}


void CTableCtl::RemoveItem(int nIndex)
{
    
    float fNowCellWidth,fNowCellHeight;
    float fContentHeight,fContentWidth;
    CList<CControl*>*Rows = mlstChild[0]->GetChilds();
    CControl* pNowCtl = (*Rows)[nIndex];
    
    float fYPos = this->GetScrollVPosition();
    SetScrollVPosition(0.0f); //처음으로 이동한다.
    
    pNowCtl->GetSize(fNowCellWidth, fNowCellHeight);
    mlstChild[0]->GetSize(fContentWidth, fContentHeight);
    
    int nSize = Rows->size();
    int nEnd = nIndex;
    int nBegin = nSize - 1;
    for (int i = nBegin; i > nEnd; i--) 
    {
        CControl* pCon = (CControl*)(*Rows)[i];
        pCon->MoveVector(0.0f, -fNowCellHeight);
        pCon->SetID(pCon->GetID() - 1); //RowID를 -1 해준다.
    }
    mlstChild[0]->Remove(nIndex); //제거한다.
    SetContentsBounds(fContentWidth,fContentHeight - fNowCellHeight); 
    
    SetScrollVPosition(fYPos);//이전값을 다시 복원한다.
}

bool CTableCtl::IsVisibleRow(int nIndex)
{
    float fTableW,fTableH;
    float fTableX,fTableY;
    
    float fCellW,fCellH;
    float fCellX,fCellY;
    
    CControl* pRow = GetRowCtl(nIndex);
    if(pRow == NULL) return false;

    GetSize(fTableW, fTableH);
    GetPosition(fTableX, fTableY);
    
    pRow->GetSize(fCellW, fCellH);
    pRow->GetPosition(fCellX, fCellY);
    
    if( (fCellX >= fTableX && fCellX <= (fTableX+fTableW)) &&
        ((fCellX + fCellW) >= fTableX && (fCellX + fCellW) <= (fTableX+fTableW)) &&
        (fCellY >= fTableY && fCellY <= (fTableY+fTableH)) &&
       ((fCellY + fCellH) >= fTableY && (fCellY + fCellH)<= (fTableY+fTableH))
       )
        return true;
    return false;
}

void CTableCtl::SetVisibleVRow(int nIndex)
{
    if(IsVisibleRow(nIndex)) return;
    float fDif = 0;
    float fPos = GetScrollVPosition();
    
    float fTableW,fTableH;
    float fTableX,fTableY;
    
    float fCellW,fCellH;
    float fCellX,fCellY;
    
    CControl* pRow = GetRowCtl(nIndex);
    if(pRow == NULL) return ;
    
    GetSize(fTableW, fTableH);
    GetPosition(fTableX, fTableY);
    
    pRow->GetSize(fCellW, fCellH);
    pRow->GetPosition(fCellX, fCellY);
    
    if( (fCellY + fCellH) < fTableY)
    {
        fDif = fTableY - (fCellY + fCellH);
        fPos -= (fDif + fCellH);
        
    }
    else if( fCellY  < fTableY)
    {
        fDif = fTableY - fCellY;
        fPos -= fDif;
    }
    else if( fCellY < (fTableY + fTableH) && (fCellY + fCellH) > (fTableY + fTableH))
    {
        fDif =  (fCellY + fCellH) - (fTableY + fTableH);
        fPos += fDif;
    }
    else if( (fCellY + fCellH) > (fTableY + fTableH) )
    {
        fDif =  (fCellY + fCellH) - (fTableY + fTableH) + fCellH;
        fPos += fDif;
    }
    SetScrollVPosition(fPos);
}


void CTableCtl::SetVisibleHRow(int nIndex)
{
    if(IsVisibleRow(nIndex)) return;
    float fDif = 0;
    float fPos = GetScrollVPosition();
    
    float fTableW,fTableH;
    float fTableX,fTableY;
    
    float fCellW,fCellH;
    float fCellX,fCellY;
    
    CControl* pRow = GetRowCtl(nIndex);
    if(pRow == NULL) return ;
    
    GetSize(fTableW, fTableH);
    GetPosition(fTableX, fTableY);
    
    pRow->GetSize(fCellW, fCellH);
    pRow->GetPosition(fCellX, fCellY);
    
    if( (fCellX + fCellW) < fTableX)
    {
        fDif = fTableX - (fCellX + fCellW);
        fPos -= (fDif + fCellW);
        
    }
    else if( fCellX  < fTableX)
    {
        fDif = fTableX - fCellX;
        fPos -= fDif;
    }
    else if( fCellX < (fTableX + fTableW) && (fCellX + fCellW) > (fTableX + fTableW))
    {
        fDif =  (fCellX + fCellW) - (fTableX + fTableW);
        fPos += fDif;
    }
    else if( (fCellX + fCellW) > (fTableX + fTableW) )
    {
        fDif =  (fCellX + fCellW) - (fTableX + fTableW) + fCellW;
        fPos += fDif;
    }
    SetScrollHPosition(fPos);
}


void CTableCtl::RemoveItemHorizon(int nIndex)
{
    
    float fNowCellWidth,fNowCellHeight;
    float fContentHeight,fContentWidth;
    CList<CControl*>*Rows = mlstChild[0]->GetChilds();
    CControl* pNowCtl = (*Rows)[nIndex];
    
    float fYPos = this->GetScrollHPosition();
    SetScrollHPosition(0.0f); //처음으로 이동한다.
    
    pNowCtl->GetSize(fNowCellWidth, fNowCellHeight);
    mlstChild[0]->GetSize(fContentWidth, fContentHeight);
    
    int nSize = Rows->size();
    int nEnd = nIndex;
    int nBegin = nSize - 1;
    for (int i = nBegin; i > nEnd; i--) 
    {
        CControl* pCon = (CControl*)(*Rows)[i];
        pCon->MoveVector(-fNowCellWidth, 0.0f);
        pCon->SetID(pCon->GetID() - 1); //RowID를 -1 해준다.
    }
    
    mlstChild[0]->Remove(nIndex); //제거한다.
    SetContentsBounds(fContentWidth - fNowCellWidth,fContentHeight); 
    
    SetScrollHPosition(fYPos);//이전값을 다시 복원한다.
}


void CTableCtl::RemoveAll(bool bVerticalList)
{
    float fNowCellWidth,fNowCellHeight;
    mlstChild[0]->GetSize(fNowCellWidth, fNowCellHeight);
   
    mlstChild[0]->RemoveAll();
    
    if(bVerticalList)
    {
        SetScrollVPosition(0.0f); //처음으로 이동한다.
        SetContentsBounds(fNowCellWidth,0);
    }
    else
    {
        SetScrollHPosition(0.0f); //처음으로 이동한다.
        SetContentsBounds(0,fNowCellHeight);
    }
}

void CTableCtl::SetRowSelMode(bool v)
{
    ((CScrollContentCtl*)mlstChild[0])->SetRowSelMode(v);
}

bool CTableCtl::GetRowSelMode()
{
    return ((CScrollContentCtl*)mlstChild[0])->GetRowSelMode();
}