AlienInvasion w

인증키 : 송호박

컴파일 옵션
1. libpng를 사용하려면 settings에서 Compress PNG Files를 uncheck해야한다.
Remove Text Metadata From PNG Files => NO해주어야한다.
- unknown critical chunk 에러가 난다.


2. SongGLES 에서 Skip Install 을 No=>Yes로 해야. 앱스토어 밸리드체크가 된다. (static library여서 문제가 생기는거 같음.)

XCode5.0 의 SongGLES 의 추가적인 컴파일옵션 Deployment Target 4.3으로 해주어야 6.0에서도 잘돌아간다 (즉 xcode5.0에서는 라이브러리 타겟을 지정할 수가 있다.)


주의 사항
1> 테스트 유저 -
songhohak@gmail.com/Thd11248
hhsong@gmail.com/Thd11248 미국계정 애플 아이디도 만들었음
2>2011.11.30
1. 카메라의 초기화를 Initialize에서 하기 때문에 Initialize당시에 카메라의 width,height가 존재 하지 않다.
   그래서 gDisplayWidth,gDisplayHeight를 사용하여 화면의 크기를 알아야한다.
2. 글자 이미지를 메모리상에서 덤프를 뜨면
    +-------------
    |                  |
    |                  |                이렇게 된다. 음 알수가 없어....
    |  A               |                 
    +------------+   
3>
아이콘의 라운드 테이블
Icon512.png - 512px - 89.825
Icon.png - 57px - 10
Icon@2x.png - 114px - 20
Icon-72.png - 72px - 12.632
Icon-72@2x.png - 144px - 25.263
Icon-Small.png - 29px - 5.088
Icon-Small@2x.png - 58px - 10.175




히스토리.
- 2011.11.30 ~ 2013.12.31
Alien Invasion을 기획하고 만들고 디자인화했다.

- 2014.01.23 ~ 2013.02.06
. 하루 스코어에 대한 버그를 수정하였다. (무조건 -x로 되는 현상)
. 유도탄이 너무 떨리는 현상 수정함.
. 속도를 개선하기위해 탱크의 Root를 .ms3d=>.txt로 변경하였다. 벡터연산없이 하는 로직
. 분산2탄이 무한으로 설정되는 현상 수정함. (훈련병에서 앱이 죽어버리면 나타나는 현상)
. 독일어,프랑스어 선택글자가 깨지는 현상 수정하였다.
. 총알 음영을 주었다.
. 언어를 추가하였다.
. 아이콘을 변경하였다.
. 애플에 리젝당한 내용을 수정하였다. (비소비성 제품은 Restore버튼을 넣어주어야 한다.)

아이폰 버전을 앱스토어에 올렸다. (Version 2.00)
안드로이드 버전을 앱스토어에 올렸다. (Version 2.00)


- 2014.02.06 ~ 2014.02.12 (Android 2.02,IOS 2.00 재심사.)
.안드로이드의 GetUserInfo의  Interger형태를 Float로 가져오는 버그 수정함.
.스코어 차감 로직 반대가 되는 것을 수정함.
.난이도를 쉽게 하였다. (해얀맵은 모든 파워를 반으로 낮추었다.)
.IOS에서 IAD를 제거하였다. (IAD에서는 AdSupport.framework를 사용하지 않아서 제거 하란다. 그래서 Admob을 선택하였다.)



- 2014.02.13 ~ 2014.02.19 (IOS:2.10 Android 2.12)
. 시야각을 45~60으로 보다 많이 볼수 있게 하였다. (Android 2.3.x 버전과 iPad1,iPhone2버전은 45도로 제한함.)
. 시야각에 퍼포먼스를 좀높혀봤다. 열심히 테스트해봐야 한다.
. 카메라뒤쪽으로 클릭되는 현상을 수정함. (즉 뒤로가는 현상 수정)
. 맵을 계급별로 제한함. 초반에 너무 나른 맵으로 시작해서 어렵다.
. UserInfo 의 version 추가함.
  0: 이전에 앱 소유자
  1: 0=>1 (GetNotCompletedMap 을 무조건 1로 다운해준다. 너무어렵다.
  #define USERVER_VERSION 1
. 겔노트 버튼이 너무 작아서 비율을 조금 높혔다.
. 해상도와 레포트 정보를 올려보자.
. 이등병 초기값을 5000으로 상향했다.
. 레포트 권유를 20분 사용이후로 설정하였다. => 10분으로 설정한다.

- 2014.02.19 ~ 2014.02.24 (IOS
. 레포트 권유를 노해도 레이더 맵이 없더라도 제공해준다.
. 훈련소 맵을 수정하였다.
. 런타임 업그레이드를 하며 아우라가 나오게 하였다.
. 약간의 해상도를 올렸다.

- 2014.02.24 ~ (Android 2.18)
. 안드로이드 화면이 이상하게 변하는 오류 (즉 Apeture가 변경되어서 나오는 오류임) 수정
. 안드로이드 광고 클릭횟수 서버에 안올리는 버그 수정함.
. 훈련소 5단계에서 더이상 진행 못하는 현상 수정
. 3단계에서 적군이 클릭이 안되는 현상 수정함.



-2014.03.09 ~ 03.25 (IOS Android 2.25 Version)
. 그림자를 세밀하게 변경하였다.
. 아군탱크를 추가함. L1,
. 뷰의 회전벡터를 주었다. (패드와 폰구별하여 조절함)
. 미사일 타입은 각도가 무조건 60도로 유지하여 발사한다.
. 메인타워 버블 업그레이드기능 추가함.
. CET2/DTower의 공격방법 추가함.
. L1레이져 업그레이드 지원
. 맵/탱크 롤 패드와/아이폰의 움직임을 조절해주었다.(회전벡터를 조절)



-2014.03.25 ~ 2014.04.10 (IOS Android 2.28 Version)
. 훈련소 모드를 무조건 성공하게 하였다.
. 훈련소 모드를 좀더 직관적으로 개선하였다.
. K9의 기관총 탑제  업그레이드를 지원한다.
. 평점을 묻는 로직을 변경하였다.
. 성공시 주어지는 무기를 미사일로 변경하였다.
. 처음/두번째 맵에 기본적으로 생성되는 건물을 제거하였다.
. 업그레이드 돈을 조금 비싸게 변경하였다. 대신 강도를 높혔다.
. 훈련소 모드에서 타워를 세우는 가이드 손가락을 수정하였다.
. 아이폰 평점 유도 URL이 틀린부분 수정함.
. 훈련소 모드에서 업그레이드 방법 추가함.


-2014.04.14
. 훈련소 모드의 타임아웃을 주었다.
. 랭크 6이상 골드모으는 비율을 줄였다. (돈이 넘쳐나는 것을 막아보자)
. 처음 1판을 깨지 못하는 유저가 너무 많다. 건물을 1/2맵은 몇개 정도 지어준다.
. 건물을 짓는 속도를 빠르게 ($1달라)
. 돈모으는 속도 빠르게 ($3달라)
. 3단계 업그레이드 패키지 ($2달라)
. 맵의 갯수를 160개로 제한 (로직의 버그 수정함)
. K9또한 기관총을 발사 할수 있게 하였다. (건설치아이콘은 제거하였음)
. Raida 라인 굵어지는 현상 수정함.
. 4분산탄을 기본으로 200개로 추가하여 반드시 1판을 깰수 있게 해주자.
. 어떤이유에서인지 포가 빙빙돈다면 돌지못하게 리셋해준다.
. 레이져의 약간의 버그를 수정하였다.
. 레이져가 안보이는 곳에서 생성될때 텍스쳐를 만들지 못하는 현상수정.

-2014.05.
. mpGoldBar 에서 쓰레드안쪽에서 Set하면 그려지는 곳에서 죽는 경우가 생긴다. (안드로이드에서 심각함)
. 장갑차/수선/탱크 추가함
. 맵을 깨지 못한것을 실행하지 못하게(자물쇠)

---------------------------------------------------------
- 슈퍼 업그레이드 아이템             (오천원)
- 대장계급장이 이상하게 보인다.
---------------------------------------------------------














- 2015.01

1>
탱크스페셜 포스에 적용해 주어야한다. (난이도 조절함)
void CSprite::GetInfomation(float* pfOutDLevel,float* pfOutALevel,float* pfOutDPower,float* pfOutAPower)
{
//Added By Song 2013.10.30 Map Rank
CUserInfo* pUserInfo = mpWorld->GetUserInfo();
CSprite* pActor = GetWorld()->GetActor();
float fMapRank = pUserInfo ? pUserInfo->GetCompletedRank() : 1.f;
float fMapRankAValue = (fMapRank - 1) * POWWER_PER_AMAPRANK;
float fMapRankDValue = (fMapRank - 1) * POWWER_PER_DMAPRANK;
float fDUp = 0.f;
float fDAt = 0.f;
float fRunTimeUp = 0.f;
if(pActor && pActor->GetTeamID() == GetTeamID())
{
if(pActor == this)
{
//주인공은 랭크만,업그레이드,를 적용해준다.
fMapRankAValue = pUserInfo->GetRank();
if(fMapRankAValue == 0.f) fMapRankAValue = 5.0f; //훈련소모드일때 너무 약하기 때문에 랭크 1로 해준다.
fMapRankDValue = fMapRankAValue;
fDUp = pUserInfo->GetDependUpgrade() * 2.5f;
fDAt = pUserInfo->GetAttackUpgrade() * 2.f;
}
else
{
fMapRankAValue = pUserInfo->GetRank();
fMapRankDValue = fMapRankAValue;
fDUp = (float)pUserInfo->GetDependUpgrade() * 2.5f;
fDAt = (float)pUserInfo->GetAttackUpgrade() * 2.f;
}


PropertyList.plist 에서  탱크를 변경

2> 훈련소를깨지않았는데 멀티플레이를 시작하는 현상 수정함.
//매칭으로 시작하지 않으면 친구찾기를 한다.
if(pEvent->lParam2 != 0xFF)
{
//훈련소맵이지만 아직 깨지를 못하였다면
if(!pUserInfo->IsCompletedMap(0) && nCurrentMapIndex != 0)
{
pUserInfo->SetLastSelectedMapID(0);
pUserInfo->SetLastSelectedMapIndex(0);
MENUANI step = GetNowMenuStep();
if(step == MENUANI_WORLD_TO_MAP)
SetAniStep(MENUANI_MAP_TO_WORLD);
else if(step == MENUANI_WORLD_TO_TANK)
SetAniStep(MENUANI_TANK_TO_WORLD);
else if(step == MENUANI_WORLD_TO_FACTORY)
SetAniStep(MENUANI_FACTORY_TO_WORLD);

break;
}


3>구글 admob 을  변경해야함.
