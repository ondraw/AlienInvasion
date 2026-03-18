//
//  CMoveAI.cpp
//  SongGL
//
//  Created by Songs on 11. 7. 20..
//  Copyright 2011 thinkm. All rights reserved.
//
#include <math.h>
#include "CMoveAI.h"
#include "sGLTrasform.h"
#include "CSGLCore.h"
#include "CSprite.h"
#include "CSglTerrian.h"
#include "sGLUtils.h"
#include "CHWorld.h"
#include "AAICore.h"
#include "CMutiplyProtocol.h"



CMoveAI::CMoveAI(CSprite *pSprite)
{
    mPathList = 0;

    mpSprite = pSprite;
    mState = MOVEAI_NONE;    
    mbSimpleMove = false;

    mfRotationVelocity = 0.0f;
    mfToRotationAngle = NONE_VALUE;
    
    mvMoveDirection.x = NONE_VALUE;
    mfMoveVelocity = 0.0f;
    mbAvoidingToComactObject = NULL;
    
    mfEndTrunAndMoveDirection = 1.0f;
    mAvoidState.AvoidState = AVOID_NONE;
    mptLastTarget.x = NONE_VALUE;
    mAvoidCnt = 0;
    mbActorAutoAvoid = false; //혹시 너무 무한 루푸를 돌지 못하게 하려고 한다.
    
    mDonAvoidCompactTime = 0;
}


CMoveAI::~CMoveAI()
{
    if(mPathList)
    {
        delete mPathList;
        mPathList = 0;
    }
}

//적탱크로 이동한다.
void CMoveAI::Command(int nCommandID,void *pData)
{
////    if(nCommandID == AICOMMAND_MOVE_SIMPLE_POS)
////    {
////        SetSimpleMove(true);
////        CmdMoveTo((SPoint*)pData,false);
////    }
////    else if(nCommandID ==  AICOMMAND_COMPACTTOSPRITE)
////    {
////        //객체랑 충돌을 하였다.
////        AvoidCompactSprite((CSprite*)pData);
////    }
}


void    CMoveAI::SetRotationVelocity(float v)
{ 
    mfRotationVelocity = v;
}
void    CMoveAI::SetToRotationAngle(float v) 
{ 
    mfToRotationAngle = v;
}

float   CMoveAI::GetRotationVelocity() { 
    return mfRotationVelocity;
}

void CMoveAI::StopMove()
{

    //움직임을 멈춘다.
    mpSprite->SetMoveVelocity(0.0f);
    mpSprite->SetRotationVelocity(0.0f);
    mState = MOVEAI_NONE;
    mbAvoidingToComactObject = NULL;
    mAvoidCnt = 0;
    SetAvoidState(AVOID_NONE,NONE_VALUE,NONE_VALUE);
    mdMigrationLength = 10000000000.0f;
    
}



//bFront : true (가기전) , false : 정확하게 그지점
void CMoveAI::CmdMoveTo(CSprite *pSprite,bool bFront)
{    
    SPoint ptT;
    pSprite->GetPosition(&ptT);
    CmdMoveTo(&ptT,bFront);
}


bool CMoveAI::GetCenterPointAtNextPath(SPoint* ptNow)
{
    if(mPathList == 0) return false;
    
    int nSize = mPathList->size();
    if(nSize == 1) 
    {
        memcpy(ptNow, &mptLastTargetAtPath, sizeof(SPoint));
        if(mPathList)
        {
            delete mPathList;
            mPathList = 0;
        }
        return true;
    }

    int nPos = mPathList->at(nSize - 2); //시작
    

    CSglTerrian::GetIndexToCenterPoint(nPos,ptNow);

//    int nGX = (CSGLCore::m_gTerrianSize - 1) / GROUP_SIZE;    
//    float fHalfTerrian = (float)((CSGLCore::m_gTerrianSize - 1) / 2.0f) * PIXELSIZE;
//    
//    //다음 맵그룹의 중앙을 월드좌표로 환산한다.
//    ptNow->x = (nPos % nGX) * nGX + (GROUP_SIZE / 2) - fHalfTerrian;
//    ptNow->y = fHalfTerrian - ((nPos / nGX) * nGX + (GROUP_SIZE / 2));
//    ptNow->z = 0;
    
    mPathList->pop_back(); //어래이에서 제거한다.
    return true;
    
}

//bFront : true (가기전) , false : 정확하게 그지점
void CMoveAI::CmdMoveTo(SPoint *ptPos,bool bFront)
{
    CHWorld* pWorld = (CHWorld*)mpSprite->GetWorld();
    //------------------------------------------------------------------------------------------------
    if(mpSprite->GetNetworkTeamType() == NETWORK_MYSIDE && mbActorAutoAvoid == false)
        pWorld->GetMutiplyProtocol()->SendSyncMove(mpSprite->GetID(), mbSimpleMove, ptPos, bFront);
    //------------------------------------------------------------------------------------------------
    
    if(mbSimpleMove == false)
    {
        SPoint ptS;
        mpSprite->GetPosition(&ptS);
        
        SPoint ptPathPos;
        SPoint ptNoneObstcle;
        int nIndex = CSglTerrian::FindNoneObstacle(mpSprite,&ptS,&ptNoneObstcle);
        if(nIndex < 0) return;
        
        
        //데이터를 사용하여 위치를 알아온다.
        std::vector<int>* pResult = 0;

        
        pResult = CSglTerrian::FindPath(mpSprite,&ptS,ptPos);
        if(pResult == 0)
        {
            if(ptS.x == ptNoneObstcle.x && ptS.z == ptNoneObstcle.z)
                SetAutoTurnAndMoveToTarget(&ptNoneObstcle,NULL);
            else
            {
                HLogE("Not Found Path And Not Found nooneObstacle\n");
            }
            return;
        }

        if(bFront == true) //정확한 지점으로 이동하지 않고 가기전에 멈춘다...
        {
        
            int nSize = pResult->size();
            if(nSize > 4)
            {
                //앞부분을 지운다.
                std::vector<int>::iterator b = pResult->begin();
                int nn = 0;
                for (std::vector<int>::iterator it = b; it != pResult->end(); ) 
                {
                    if(nn < 4)
                    {
                        it = pResult->erase(it);
                        nn++;
                        continue;
                    }
                    nn++;
                    it++;
                }
                
                nSize = pResult->size();
                int nBlock = pResult->at(nSize - 1);
                CSglTerrian::GetIndexToCenterPoint(nBlock, ptPos);
            }
            else if(nSize > 3)
            {
                //앞부분을 지운다.
                std::vector<int>::iterator b = pResult->begin();
                int nn = 0;
                for (std::vector<int>::iterator it = b; it != pResult->end(); ) 
                {
                    if( nn < 3)
                    {
                        it = pResult->erase(it);
                        nn++;
                        continue;
                    }
                    nn++;
                    it++;
                    
                }

                nSize = pResult->size();
                int nBlock = pResult->at(nSize - 1);
                CSglTerrian::GetIndexToCenterPoint(nBlock, ptPos);
            }
            
        }
        
        //데이터도 같이 지워주자.. 세로운 명령이기 때문에 클리어해준다.
        if(mPathList)
        {
            delete mPathList;
            mPathList = 0;
        }
        
        mPathList = pResult;
        memcpy(&mptLastTargetAtPath, ptPos, sizeof(SPoint));
        
        if(ptS.x == ptNoneObstcle.x && ptS.y == ptNoneObstcle.y)
        {
            GetCenterPointAtNextPath(&ptPathPos);
            SetAutoTurnAndMoveToTarget(&ptPathPos,&ptPathPos,true);   //먼거리는 앞으로 움직인다.
        }
        else //현재 장애물에 존재하여 장에물이 아닌 곳으로 이동을 해야 한다.
        {
            SetAutoTurnAndMoveToTarget(&ptNoneObstcle,&ptNoneObstcle);
        }
    }
    else
    {
        //패스정보없이 움직인다.
        SetAutoTurnAndMoveToTarget(ptPos,ptPos);
    }
}

int  CMoveAI::GetLastPathPos()
{
    if(mPathList)
    {
        int nX = mPathList->size();
        return mPathList->at(nX - 1);
    }
    return -1;
}

void CMoveAI::RenderBeginCore(int nTime)
{ 
    CWorld* pWorld = (CWorld*)mpSprite->GetWorld();
    
    if(mDonAvoidCompactTime != 0 && GetGGTime() > mDonAvoidCompactTime)
        mDonAvoidCompactTime = 0;
    
    if(mbAvoidingToComactObject && mbAvoidingToComactObject->GetState() != SPRITE_RUN)
        mbAvoidingToComactObject = NULL;
        
    
    if(pWorld->GetActor() != mpSprite) //주인공은 RenderBegin()에서 실행되었다.
    {
        TurnAndMove(nTime);
    }
}

void CMoveAI::SetAvoidState(AVOID_STATE st,float fRotAngleDir,float fMoveDir)
{
    mAvoidState.AvoidState = st;
    mAvoidState.fMoveDir = fMoveDir;
    mAvoidState.fRotAngleDir = fRotAngleDir;
}

AVOID_STATE CMoveAI::GetAvoidState()
{
    return mAvoidState.AvoidState;
}

void CMoveAI::NextAvoid() 
{
    switch (mAvoidState.AvoidState) {
        case AVOID_FORNT_COMPACT: //AVOID_FRONT_COMPACT2 or 3가 된다.
        {
            //현재 모델의 방향으로 직진 할예정
            SVector dirVector;
            mpSprite->GetModelDirection(&dirVector);
            
            //현재 모델의 위치
            SPoint ptNow;
            mpSprite->GetPosition(&ptNow);
            
            //다음단계는 
            // 회피 대상이 움직이고 있을때 (AVOID_FORNT_COMPACT2 : 직진한다.)
            // 정지해 있을때. (AVOID_FORNT_COMPACT3 : 좀더 회전하여 직진한후, 이동한다.)
            if(mbAvoidingToComactObject && !mbAvoidingToComactObject->isMoving())
            {
                //좀더 회전한후 직진한다.
                SPoint ptAvoid;
                mpSprite->GetPosition(&ptAvoid);
                float a30 = (mAvoidState.fRotAngleDir / 2.0f) * PI / 180.0f; //좀더 더 회전한다.
                //40 회전
                float x2 = dirVector.x* cosf(a30) - dirVector.z * sinf(a30);
                float z2 = dirVector.x * sinf(a30) + dirVector.z * cosf(a30);
                
                
                mpSprite->SetMoveVelocity(mAvoidState.fMoveDir);
                //현재 모델의 방향으로 직진.
                float fV = GetMoveVelocity();
                SetMoveVelocity(0.0f);
                
                //일단 부딛힌 반대방향으로 움직인다.
                ptAvoid.x += x2 * fV * 1200.0f;  //대략적인 15.0f 시간을 의미함.
                ptAvoid.z -= z2 * fV * 1200.0f;
                SetAutoTurnAndMoveToTarget(&ptAvoid,NULL);
                
                SetAvoidState(AVOID_FORNT_COMPACT3, mAvoidState.fRotAngleDir, mAvoidState.fMoveDir);
                mdMigrationLength = 10000000000.0f;
                
                break;
            }
            else //회피대상이 움직이고 있을때
            {
                //이전 정보의 방향으로 움직일 예정이다.
                mpSprite->SetMoveVelocity(mAvoidState.fMoveDir);
                
                //현재 모델의 방향으로 직진.
                float fV = GetMoveVelocity();
                SetMoveDirection(&dirVector);   
                
                //현재 위치에서 바라보는 방향로 400point 앞에 까지 이동한다.
                mptNowTarget.x = ptNow.x+ dirVector.x * fV * 400.0f;  //대략적인 15.0f 시간을 의미함.
                mptNowTarget.y = 0;
                mptNowTarget.z = ptNow.z- dirVector.z * fV * 400.0f;
                
                mState = MOVEAI_AUTOMOVE;
                SetAvoidState(AVOID_FORNT_COMPACT2, mAvoidState.fRotAngleDir, mAvoidState.fMoveDir);
                mdMigrationLength = 10000000000.0f;
            }
            
            break;
        }
        case AVOID_FORNT_COMPACT2:
        case AVOID_FORNT_COMPACT3:
        {
            //이동할 타겟으로 해준다.
            if(mptLastTarget.x != NONE_VALUE)
            {
                SetMoveVelocity(0.0f); //이동을 멈춘다.
                SetAutoTurnAndMoveToTarget(&mptLastTarget,&mptLastTarget);
                
                mAvoidState.AvoidState = AVOID_NONE;
                mbAvoidingToComactObject = NULL;
            }
            
            mAvoidCnt = 0; //궁국적으로 AVOID_FORNT_COMPACT2,3으로 되면 회피모드가 종료되었다.
            
            break;
        }
        default:
        {
            if(mPathList)
            {
                SPoint ptPathPos;
                if(GetCenterPointAtNextPath(&ptPathPos))
                {
                    //이동하면서 움직이면 좋겠다.
                    mpSprite->SetMoveVelocity(1.0f); //움직임이 자연스럽게 회전하면서 전진하게 한다.
                    SetAutoTurnAndMoveToTarget(&ptPathPos,&ptPathPos,true);  //Path는 먼거리를 이동하는 것으로 생각하여 앞으로 이동하게 한다.
                }
                else 
                {
                    mAvoidState.AvoidState = AVOID_NONE;
                    mbAvoidingToComactObject = NULL;
                }
            }
            else
            {
                mAvoidState.AvoidState = AVOID_NONE;
                mbAvoidingToComactObject = NULL;
            }
            break;
        }
    }
}

//왼쪽 앞이 충돌
void CMoveAI::AvoidCOMACT_F(bool bLeft)
{
    float fDif = 0.0f;
    float fV = GetMoveVelocity();
    
    mAvoidCnt++;
    
    if(fV > 0.0f) //앞으로...
    {
        if(bLeft) fDif = 30.0f;
        else fDif = - 30.0f;
        mfEndTrunAndMoveDirection = 1.0f;

    }
    else //뒤로...
    {
        if(bLeft) fDif = -30.0f;
        else fDif = 30.0f;
        mfEndTrunAndMoveDirection = -1.0f;
    }
    
    //0> Avoid 상태를 AVOID_FORNT_COMPACT를 해준다.
    SetAvoidState(AVOID_FORNT_COMPACT,fDif,mfEndTrunAndMoveDirection);
    
    //1>일단 움직임을 멈춘다.
    mpSprite->SetMoveVelocity(0.0f);
    
    //2>위치를 뒤로 이동시킨후 (부딛혔기 때문에 곂쳐있는것을 분리해두어야 한다.
    //위치를 뒤로 이동시킨후
    //AvoidSetBeforePosition(fV);
    SPoint ptNow;
    SVector vtdir;
    mpSprite->GetMoveDirection(&vtdir);
    mpSprite->GetPosition(&ptNow);
    ptNow.x -= vtdir.x * fV * 40.0f;  //대략적인 15.0f 시간을 의미함.
    ptNow.z += vtdir.z * fV * 40.0f;
    mpSprite->SetPosition(&ptNow);
    
    
    //2>각방향으로 30도 회전한후에 뒤로 움직인다.
    SPoint ptAvoid;
    mpSprite->GetPosition(&ptAvoid);
    float a30 = fDif * PI / 180.0f;
	//40 회전
	float x2 = vtdir.x* cosf(a30) - vtdir.z * sinf(a30);
	float z2 = vtdir.x * sinf(a30) + vtdir.z * cosf(a30);
    //일단 부딛힌 반대방향으로 움직인다.
    ptAvoid.x -= x2 * fV * 300.0f;  //대략적인 15.0f 시간을 의미함.
    ptAvoid.z += z2 * fV * 300.0f;
    SetAutoTurnAndMoveToTarget(&ptAvoid,NULL);

}

bool CMoveAI::GetDonComact()
{
//    if(mDonAvoidCompactTime == 0) return false;
    if(GetGGTime() < mDonAvoidCompactTime || mbAvoidingToComactObject) return true;
    return false;
}

void CMoveAI::AvoidCompactSprite(CSprite* pCompactSP)
{
    //회피모드일때 계속해서 회피하지 못하게하자.
    if(GetDonComact()) return;
    
//    float fColidedDistance  = mpSprite->GetColidedDistance();
    if(pCompactSP == NULL || (pCompactSP && pCompactSP->GetState() != SPRITE_RUN)) return ;
    
    else if(mbAvoidingToComactObject && mbAvoidingToComactObject->GetState() != SPRITE_RUN)
    {
        SetMoveVelocity(0.0f);
        //회피가 끝났다.
        SetAutoTurnAndMoveToTarget(&mptLastTarget,&mptLastTarget);
        mbAvoidingToComactObject = NULL;
        mAvoidState.AvoidState = AVOID_NONE;
        return ;
    }
    
    //현재 회피중에 있고 적당이 멀리있다면 스킵하자 
    //하지만 너무 겹쳐있으면 회피모드로 변경해준다.
    //else if(mbAvoidingToComactObject == pCompactSP && fColidedDistance < 150.0f)  //fColidedDistance > 150.0f 같은 객체가 넘어오는데 거리가 너무 작으면 강력한 외피모드를 해야한다.
//    else if(mbAvoidingToComactObject == pCompactSP && (mAvoidState.AvoidState != AVOID_NONE || fColidedDistance < 150.0f)) //Modifeid By Song 2012.10.30
    else if(mbAvoidingToComactObject == pCompactSP && mAvoidState.AvoidState != AVOID_NONE) //Modifeid By Song 2012.10.30
        return ; //현재 회피중이다. 더이상 작업을 하지 않음.
    

    mbAvoidingToComactObject = pCompactSP; //회피 모드로 전환한다.
    
    SPoint ptNow,ptEnNow;
    SVector vtDir;
    float fAvoidDir,fToAvoidAngle;
    
    mpSprite->GetPosition(&ptNow);
    pCompactSP->GetPosition(&ptEnNow);
    mpSprite->GetModelDirection(&vtDir);
    W_COMACT comact = sglWhichCompact(&ptNow, &vtDir, mpSprite->GetOrientaion()[1], &ptEnNow, &fAvoidDir, &fToAvoidAngle);
    
    float fEnRadius = mbAvoidingToComactObject->GetRadius();
    float fRadius = (mpSprite->GetRadius() + fEnRadius) * (mpSprite->GetRadius() + fEnRadius);
    fRadius = fRadius - (fRadius / 2.8f); //2.8분의 1 이상 곂쳐지면 절대 움직이지 못하게 해야한다.
    
    float fV = GetMoveVelocity();
    switch(comact) 
    {
    //왼쪽 앞
    case W_COMACT_LF: //HLogD("Left Front %f,%f\n",mpSprite->GetColidedDistance(),fRadius);
            if(fV > 0.0f) //앞으로 가고 있을때 회피한다. 뒤로 가고 있을때 앞쪽이 걸리면 무시한다.
                AvoidCOMACT_F(false);
        break;
    //왼쪽 앞옆
    case W_COMACT_LFL: //HLogD("Left Front Left %f,%f\n",mpSprite->GetColidedDistance(),fRadius);
            if( mpSprite->GetColidedDistance() < fRadius)
                AvoidCOMACT_F(false);
        break;
    //왼쪽 뒤
    case W_COMACT_LB: //HLogD("Left Back %f,%f\n",mpSprite->GetColidedDistance(),fRadius);
            if(fV < 0.0f) //뒤로 가고 있을때 회피한다. 앞으로 가고 있을때는 뒤쪽이 걸리면 무시한다.
                AvoidCOMACT_F(false);
        break;
    //왼쪽 뒤옆
    case W_COMACT_LBL: //HLogD("Left Back Left %f,%f\n",mpSprite->GetColidedDistance(),fRadius);
            if( mpSprite->GetColidedDistance() < fRadius)
                AvoidCOMACT_F(false);
        break;
    //오른쪽 뒤
    case W_COMACT_RB: //HLogD("Right Back %f,%f\n",mpSprite->GetColidedDistance(), fRadius);
            if(fV < 0.0f) //뒤로 가고 있을때 회피한다. 앞으로 가고 있을때는 뒤쪽이 걸리면 무시한다.
                AvoidCOMACT_F(true);
        break;
    //오른쪽 뒤옆
    case W_COMACT_RBR: //HLogD("Right Back Right %f,%f\n",mpSprite->GetColidedDistance(),fRadius);
            if( mpSprite->GetColidedDistance() < fRadius)
                AvoidCOMACT_F(true);
        break;
    //오른쪽 앞
    case W_COMACT_RF: //HLogD("Right Front %f,%f\n",mpSprite->GetColidedDistance(),fRadius);
            if(fV > 0.0f) //앞으로 가고 있을때 회피한다. 뒤로 가고 있을때 앞쪽이 걸리면 무시한다.
                AvoidCOMACT_F(true);
        break;
    //오른쪽 앞옆
    case W_COMACT_RFR: //HLogD("Right Front Right %f,%f\n",mpSprite->GetColidedDistance(),fRadius);
            if( mpSprite->GetColidedDistance() < fRadius)
                AvoidCOMACT_F(true);
        break;
    }
    if(mAvoidCnt >= 3)
    {
        //상대팀이면 정지한다. 즉 움직이지않고 공격해야하잖아.
        if(!(mbAvoidingToComactObject->GetTeamID() & mpSprite->GetTeamID()))
        {
            //움직임을 멈춘다.
            CHWorld *pWorld = (CHWorld*)mpSprite->GetWorld();
            if(mpSprite != pWorld->GetActor())
                StopMove();
            mAvoidCnt = 0;
            return ;
        }
        else
        {
            float fV = mpSprite->GetMoveVelocity();
            float fR = mpSprite->GetRadius() * 3;
            if(fV > 0.0f) //앞으로...
            {
                ptNow.x += vtDir.x * fR;
                ptNow.z -= vtDir.z * fR;
            }
            else
            {
                ptNow.x -= vtDir.x * fR;
                ptNow.z += vtDir.z * fR;
            }
            ((CHWorld*)mpSprite->GetWorld())->GetPositionY(&ptNow);
            mDonAvoidCompactTime = GetGGTime() + 5000l;
            return;
        }
    }
}


void CMoveAI::SetAutoTurnAndMoveToTarget(SPoint* ptPathPos,SPoint* ptLastTarget,bool bOnlyFront)
{

    float fToAngle = 0.0f;
    SPoint ptNow;
    
        
    if(ptLastTarget)
        memcpy(&mptLastTarget,ptLastTarget,sizeof(SPoint));
    
    memcpy(&mptNowTarget,ptPathPos,sizeof(SPoint));
    
    //먼저 회전을 해야 한다.
    mState = MOVEAI_AUTOTURN;
    
    mpSprite->GetPosition(&ptNow);
    
    //어느 방향으로 턴해야 하는지 설정한다.
    fToAngle = sglNowToPosAngle(&ptNow,&mptNowTarget);
    
    mpSprite->SetRotationVelocity(sglGetRotationVelocity(bOnlyFront,mpSprite->GetOrientaion()[1],&fToAngle,&mfEndTrunAndMoveDirection));
    mpSprite->SetToRotationAngle(fToAngle);
}



void CMoveAI::TurnAndMove(int nTime)
{
    CHWorld* pWorld = (CHWorld*)mpSprite->GetWorld();
    bool bActor = pWorld->GetActor() == mpSprite;
    int nCameraType = CAMERT_NORMAL;
    float fcos = 0;
    float fsin = 0;
    bool bMoved = false;
    int nResult = 0;
    float fx,fy,fz;
    SVector vVelocity;
    SPoint  ptMove;
    
    SVector dirModel;
    SVector dirMove; 
    SPoint  ptNow;
    
    float*   fOrientation2 = mpSprite->GetOrientaion();
    CMS3DModel* pModel = (CMS3DModel*)mpSprite->GetModel();
    
    if(mbAvoidingToComactObject && mbAvoidingToComactObject->GetState() != SPRITE_RUN)
        mbAvoidingToComactObject = NULL;
    
    //SetCameraType 함수에서 HandleCarmera를 사용하여 카메라를 움직인다.
    //IsMoving IsTuning을 조사하기 때문에 중간에 움직임을 정지해버리면 그 움직임 값의 흔들림이 생기기 때문에
    //SetRotationVelocity을 하지 않음.
    bool bNeedStopTurnning = false;
    float fArrangedModelOrientation = 0.0f;
    if(fOrientation2) 
        fArrangedModelOrientation = AngleArrangeEx(fOrientation2[1]);
    
    mpSprite->GetModelDirection(&dirModel);
    GetMoveDirection(&dirMove);
    mpSprite->GetPosition(&ptNow);
    
    float fRotVeclocity = GetRotationVelocity();
    float fMoveVelocity = GetMoveVelocity();
    
    if(fRotVeclocity != 0.0f)
	{
        
        float fRotAngle = fRotVeclocity * (float)nTime;
        float fToAngle = GetToRotationAngle(); //사이각
        
        //사이각이 존재하면 이각도 이상 벗어나지 않게 한다. (즉 정확하게 회전한다.)
        if(fToAngle != NONE_VALUE)
        {
            bNeedStopTurnning = sglCheckRotation(fArrangedModelOrientation,fToAngle,&fRotAngle);
        }
        
        nCameraType = mpSprite->GetCameraType();
        

        float fRadianAngle = fRotAngle * PI / 180.0f;
        fcos = cosf(fRadianAngle);
        fsin = sinf(fRadianAngle);
        
        SVector vRot;
        vRot.x = fcos * dirModel.x - fsin * dirModel.z;
        vRot.y = 0;
        vRot.z = fcos * dirModel.z + fsin * dirModel.x;
        
        sglNormalize(&vRot);
        mpSprite->SetModelDirection(&vRot);
        
        mpSprite->DontRotationHeader(fRotAngle);
        
        if(isMoving())
        {
            //방향과 움직임은 틀리다. -------------------
            vRot.x = fcos * dirMove.x - fsin * dirMove.z;
            vRot.y = 0;
            vRot.z = fcos * dirMove.z + fsin * dirMove.x;
//            sglNormalize(&vRot);
            SetMoveDirection(&vRot); 
            
            //속력을 낸다.
            //mvMove.z 는 실제 속력 (이벤트의 입력값이다.)
            vVelocity.x = vRot.x * fMoveVelocity * (float)nTime;
            vVelocity.y=  0;
            vVelocity.z = -vRot.z * fMoveVelocity * (float)nTime;
            
            //이동한다.
            sglTranslateEx(&fx, &fy, &fz, ptNow.x, ptNow.y, ptNow.z, &vVelocity);
            ptMove.x = fx;
            ptMove.y = 0;
            ptMove.z = fz;
            
            nResult = pWorld->GetMovePosiztionY(&ptMove);

            if(nResult == E_SUCCESS) 
            {
                mpSprite->SetPosition(&ptMove);
            }
            
            bMoved = true; //이미 한번 했다.
        }
        
        //카메라의 위에서 전체 지도를 보는 것 외의 것은 카메라를 움직인다.
		if(bActor && nCameraType != CAMERT_UP ) //주인공은 카메라를 따라가게 해야 한다.
		{
			mpSprite->SetCameraType(nCameraType); 
		}
        
        

        if(pModel)
        {
            if(fRotVeclocity > 0.0f && !pModel->isAnimationRunningf() && pModel->getCurrentAnimationf() != 1) //앞으로
            {
                pModel->setNextAnimationf(1, GL_TRUE, GL_FALSE);
            }
            else if(!pModel->isAnimationRunningf() && pModel->getCurrentAnimationf() != 0) //두로 
            {
                pModel->setNextAnimationf(0, GL_TRUE, GL_FALSE);
            }
        }
	}
    
    
    if(bMoved == false && isMoving())
	{
        //mvMove.z 는 실제 속력 (이벤트의 입력값이다.)
		vVelocity.x = dirMove.x * fMoveVelocity * (float)nTime;
		vVelocity.y=  0;
		vVelocity.z = -dirMove.z * fMoveVelocity * (float)nTime;
		
        nCameraType = mpSprite->GetCameraType();
        
		sglTranslateEx(&fx, &fy, &fz, ptNow.x, ptNow.y, ptNow.z, &vVelocity);
		ptMove.x = fx;
		ptMove.y = 0;
		ptMove.z = fz;
		
        if(mState == MOVEAI_AUTOMOVE)
        {
            double dx = ptNow.x - mptNowTarget.x;
            double dz = ptNow.z - mptNowTarget.z;
            double dl = dx*dx + dz*dz; //두점 거리
            if( dl < mdMigrationLength)
                mdMigrationLength = dl;
            else //점점작아졌다가 다시 커지면 목표지점을 지나쳤다. 
            {
                //다른 상태로 이동한다.
                if(mbSimpleMove && dl > 256 && mbActorAutoAvoid == false) //한번만 장애물을 만들면 그다음은 자애물이 없을 것이다.
                {
                    mpSprite->SetMoveVelocity(0.0f);
                    mdMigrationLength = 10000000000.0f;
                    SetToRotationAngle(NONE_VALUE); //더이상 회전 하지 않음.
                    SetRotationVelocity(0.0f); //더이상 회전하지 않음.
                    mState = MOVEAI_NONE;
                    
                    //단순이동을 패스이동으로 변경한다.
                    SetSimpleMove(false);
                    
                    mbActorAutoAvoid = true;
                    CmdMoveTo(&mptLastTarget,false);
                    return ;
                }
                
                mpSprite->SetMoveVelocity(0.0f);
                mdMigrationLength = 10000000000.0f;
                mState = MOVEAI_NONE; //중지한다??
                NextAvoid();
            }
        }
        
        nResult = pWorld->GetMovePosiztionY(&ptMove);
		if(nResult == E_SUCCESS) 
		{
			mpSprite->SetPosition(&ptMove);
            
            //카메라의 위에서 전체 지도를 보는 것 외의 것은 카메라를 움직인다.
			if(bActor && nCameraType != CAMERT_UP )
			{
                //쓰레드 안에서 카메라가 변경되면 바로 적용하지 말자.
				mpSprite->SetCameraType(nCameraType); //카메라의 위치를 이동시켜준다.
			}
		}
        
        if(pModel)
        {
            if(fRotVeclocity > 0.0f && !pModel->isAnimationRunningf() && pModel->getCurrentAnimationf() != 1) //앞으로
            {
                pModel->setNextAnimationf(1, GL_TRUE, GL_FALSE);
            }
            else if(!pModel->isAnimationRunningf() && pModel->getCurrentAnimationf() != 0) //두로 
            {
                pModel->setNextAnimationf(0, GL_TRUE, GL_FALSE);
            }
        }

	}
    
    if(bNeedStopTurnning)
    {
        SetToRotationAngle(NONE_VALUE); //더이상 회전 하지 않음.
        SetRotationVelocity(0.0f); //더이상 회전하지 않음.
        mState = MOVEAI_AUTOMOVE;  //움직임 마크로 변경한다.
        
        mpSprite->GetModelDirection(&dirModel);
        mpSprite->SetMoveDirection(&dirModel);
        
        mpSprite->SetMoveVelocity(mfEndTrunAndMoveDirection);
        mdMigrationLength = 10000000000.0f;
    }
    
    if(fRotVeclocity == 0.0f && fMoveVelocity == 0.0f)
    {
        if(pModel && pModel->isAnimationRunningf())
            pModel->setNextAnimationf(2, GL_FALSE, GL_FALSE);
    }
}

void CMoveAI::ArchiveMultiplay(int nCmd,CArchive& ar,bool bWrite)
{
    
    if(nCmd == SUBCMD_OBJ_ALL_INFO)
    {
        int nSize = 0;
        int nState = (int)mState;
        if(bWrite)
        {
            ar << mvMoveDirection;
            ar << mfMoveVelocity;
            ar << mfEndTrunAndMoveDirection;
            ar << mfRotationVelocity;
            ar << mptLastTarget;
            ar << mptNowTarget;
            ar << mptLastTargetAtPath;
            ar << mbSimpleMove;
            ar << nState;
            if(mPathList)
            {
                nSize = (int)mPathList->size();
                ar << nSize;
                for(vector<int>::iterator it = mPathList->begin(); it != mPathList->end(); it++)
                {
                    ar << *it;
                }
                
            }
            else
                ar << nSize;
        }
        else
        {
            int nMapID;
            ar >> mvMoveDirection;
            ar >> mfMoveVelocity;
            ar >> mfEndTrunAndMoveDirection;
            ar >> mfRotationVelocity;
            ar >> mptLastTarget;
            ar >> mptNowTarget;
            ar >> mptLastTargetAtPath;
            ar >> mbSimpleMove;
            ar >> nState;
            ar >> nSize;
            if(nSize > 0)
            {
                if(mPathList == NULL)
                    mPathList = new vector<int>;
                
                for (int i =0; i < nSize; i++) {
                    ar >> nMapID;
                    mPathList->push_back(nMapID);
                }
            }
            mState = (MOVEAI_STATE)nState;
        }

    }
}