/*
 *  IAction.h
 *  SongGL
 *
 *  Created by 호학 송 on 11. 3. 16..
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef IACTION_H
#define IACTION_H

//스프라이트의 움직임에대한 또는 어떠한 행위에 대한 이벤트
class CSprite;
class IAction
{
public:
	virtual ~IAction() {};
	virtual void OnAutoAimed(CSprite* pSrcObj) = 0;
    virtual void OnActotorDeleted(CSprite* pActor) = 0;
    virtual void OnAddedSprite(CSprite* pV) = 0;
    virtual void OnDeletedSprite(CSprite* pV) = 0;
    //0:더이상 만들 탱크가 없다.(인구수 초과)
    //other : 만들수있는 객체의 갯수
    //arrResult : In:모델아이디 Out:(1:만들수있다, 0:만들수없다)
    virtual     int OnMainTowerMakeAssistObject(int nTeamID,int nSize,int* arrResult) = 0;

};
	
#endif //IACTION_H