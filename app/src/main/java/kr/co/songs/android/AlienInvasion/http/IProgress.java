package kr.co.songs.android.AlienInvasion.http;

/**
 * 전체 싱크 과정
 * @author Song
 *
 */
public interface IProgress 
{
	public void BegineSync();
	
	
	//- start http -------------------------------
	public void ConnectNet();
	
	public void ConnectHttp();
	
	public void SendTotalSize(int nSize);
	
	
	
	public void RcvTotalSize(int nSize);
	
	public void RcvData(int nSize);
	
	public void DisconnectHttp();
	
	public void DisconnectNet();
	
	
	/**
	 * Client의 Refresh의 이벤트를 날려준다.
	 * nAction = 1(N),2(D),4(Update)
	 */
	public void ActionByServer(int nAction);
	
	/**
	 * 컨넥터에서 리스트의 상세를 가져온다.
	 * @param sGUID
	 */
	public void UpdateDetailByServer(String sGUID) ;
	
	//- end http -------------------------------	
	
	public void EndSync(int nResult,String sLastError);
	
	
	/**
	 * Added 외부에서 싱크 도중에 종료를 요청하였을 경우
	 * @return
	 */
	public boolean isRequestedEnd();
}
