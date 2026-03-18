package com.android.googleservice;

import java.util.ArrayList;
import java.util.List;

import kr.co.songs.android.AlienInvasion.AlienInvasion;
import kr.co.songs.android.AlienInvasion.SongGLLib;

import com.google.android.gms.games.Games;
import com.google.android.gms.games.GamesStatusCodes;
import com.google.android.gms.games.multiplayer.Participant;
import com.google.android.gms.games.multiplayer.realtime.RealTimeMessage;
import com.google.android.gms.games.multiplayer.realtime.RealTimeMessageReceivedListener;
import com.google.android.gms.games.multiplayer.realtime.Room;
import com.google.android.gms.games.multiplayer.realtime.RoomConfig;
import com.google.android.gms.games.multiplayer.realtime.RoomStatusUpdateListener;
import com.google.android.gms.games.multiplayer.realtime.RoomUpdateListener;

import android.content.Intent;
import android.os.Bundle;
import android.os.Message;
import android.util.Log;
import android.view.WindowManager;

/**
FindPlyer nResetFindTime=20	nMatchFilter=0	nMatchGroup=6684672

접속 순서.
onRoomCreated roomID ChoKCQia7Pmg5xsQAhABGAAg____________ARDmjMfkvO3GxakB
onPeerJoined
onRoomConnecting
onP2PConnected
onConnectedToRoom
onPeersConnected list Cnt = 1
onRoomConnected roomID ChoKCQia7Pmg5xsQAhABGAAg____________ARDmjMfkvO3GxakB


끊기는 순서 (상대의 접속이 끊길때 발생한다.)
onDisconnectedFromRoom
onPeerLeft
onPeersDisconnected list Cnt = 1
onLeftRoom roomID ChoKCQia7Pmg5xsQAhABGAAg____________ARDmjMfkvO3GxakB
*/

public class Multiplay implements RoomStatusUpdateListener, RoomUpdateListener,
		RealTimeMessageReceivedListener {

	static final String TAG = "Multiplay";
	AlienInvasion mActivity = null;
	// at least 1 players required for our game
	final static int MIN_PLAYERS = 1;
	String mRoomId = null;
	public boolean mbNeedLeftRoomID = false;
	
    ArrayList<Participant> mParticipants = null;
    String mMyId = null;

	boolean mDebugLog = false;

	boolean mMatching = false;
    boolean mMatchStarted = false;
    
	void debugLog(String message) {
		if (mDebugLog) {
			Log.d(TAG, "Multiplay: " + message);
		}
	}
	
	void logError(String message) {
		Log.e(TAG, "*** Multiplay ERROR: " + message);
	}

	public void enableDebugLog(boolean enabled) {
		mDebugLog = enabled;
		if (enabled) {
			debugLog("Debug log enabled.");
		}
	}

	public Multiplay(AlienInvasion ativity) {
		mActivity = ativity;
	}

	// Leave the room.
    void leaveRoom() {
        if (mRoomId != null) {
            Games.RealTimeMultiplayer.leave(mActivity.getApiClient(), this, mRoomId);
            mRoomId = null;
        }
    }
    
    public void SendData(byte[] btData,boolean bReiable)
    {
    	// Send to every other participant.
        for (Participant p : mParticipants) {
            if (p.getParticipantId().equals(mMyId))
                continue;
            if (p.getStatus() != Participant.STATUS_JOINED)
                continue;
            if (bReiable) 
            {
                // final score notification must be sent via reliable message
                Games.RealTimeMultiplayer.sendReliableMessage(mActivity.getApiClient(), null, btData, mRoomId, p.getParticipantId());
            } 
            else 
            {
                // it's an interim score notification, so we can use unreliable
                Games.RealTimeMultiplayer.sendUnreliableMessage(mActivity.getApiClient(), btData, mRoomId, p.getParticipantId());
            }
        }
    }
	// create a RoomConfigBuilder that's appropriate for your implementation
	private RoomConfig.Builder makeBasicRoomConfigBuilder() {
		return RoomConfig.builder(this).setMessageReceivedListener(this)
				.setRoomStatusUpdateListener(this);
	}

	public void FindPlyer(long nResetFindTime, long nMatchFilter,
			long nMatchGroup) {
		
		debugLog("FindPlyer nResetFindTime="+nResetFindTime +"\tnMatchFilter="+nMatchFilter+"\tnMatchGroup="+nMatchGroup);
		
		if(mMatching && mRoomId == null)
		{
			//이전에 FindPlayer을 했지만 아직 CreateRoom이 안된 상태이다. CreateRoom이 되면 제거해주어야 한다.
			mbNeedLeftRoomID = true;
		}
		mMatching = true;
		mMatchStarted = false;
		mRoomId = null; 
		
		// auto-match criteria to invite one random automatch opponent.
		Bundle am = RoomConfig.createAutoMatchCriteria(MIN_PLAYERS,
				MIN_PLAYERS, nMatchFilter);

		// build the room config:
		RoomConfig.Builder roomConfigBuilder = makeBasicRoomConfigBuilder();
		roomConfigBuilder.setAutoMatchCriteria(am);
		RoomConfig roomConfig = roomConfigBuilder.build();

		// create room:
		Games.RealTimeMultiplayer.create(mActivity.getApiClient(), roomConfig);

		AlienInvasion.gMainActivity.getSongGLLib().ResetFindTime(nResetFindTime);
		
		// prevent screen from sleeping during handshake
		mActivity.getWindow().addFlags(
				WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
	}
	
	public void LogOut() 
	{
		debugLog("LogOut");
		
		
		
		if(mMatching && mRoomId == null)
		{
			//이전에 FindPlayer을 했지만 아직 CreateRoom이 안된 상태이다. CreateRoom이 되면 제거해주어야 한다.
			mbNeedLeftRoomID = true;
		}
		leaveRoom();
		SongGLLib mLib = AlienInvasion.gMainActivity.getSongGLLib();
		if(mLib != null)
		{
			mLib.SetMultiplayState(SongGLLib.SYNC_NONE);
			mLib.ResetFindTime(0);
		}
		
		if(mMatchStarted)
		{
			mActivity.getSongGLLib().StopMultiplayGame();
		}
		mMatching = false;
		mMatchStarted = false;
	}

	// returns whether there are enough players to start the game
	boolean shouldStartGame(Room room) {
		int connectedPlayers = 0;
		for (Participant p : room.getParticipants()) {
			if (p.isConnectedToRoom())
				++connectedPlayers;
		}
		return connectedPlayers >= MIN_PLAYERS;
	}
	
	void updateRoom(Room room) 
	{
        if (room != null)
            mParticipants = room.getParticipants();
    }
	
	// 기다리는 화면이 안보여도 자동으로 연결을 할 수가 있다.
    void showWaitingRoom(Room room) {
        // minimum number of players required for our game
        // For simplicity, we require everyone to join the game before we start it
        // (this is signaled by Integer.MAX_VALUE).
        final int MIN_PLAYERS = Integer.MAX_VALUE;
        Intent i = Games.RealTimeMultiplayer.getWaitingRoomIntent(mActivity.getApiClient(), room, MIN_PLAYERS);

        // show waiting room UI
        mActivity.startActivityForResult(i, GameHelper.RC_WAITING_ROOM);
    }

	// //////////// RoomStatusUpdateListener
	/**
	 * 룸에 접속하면 룸 아이디가 생성된다.
	 */
	@Override
	public void onConnectedToRoom(Room room) {
		debugLog("onConnectedToRoom");
		// get room ID, participants and my ID:
        mRoomId = room.getRoomId();
        mParticipants = room.getParticipants();
        mMyId = room.getParticipantId(Games.Players.getCurrentPlayerId(mActivity.getApiClient()));
	}

	/**
	 * 당신의 플레이어가 서버 혹은 네트워크로 인하여 접속이 끊겼을 때 ...
	 */
	@Override
	public void onDisconnectedFromRoom(Room room) {
		debugLog("onDisconnectedFromRoom");
		// leave the room
		leaveRoom();
		mRoomId = null;
		// clear the flag that keeps the screen on
		mActivity.getWindow().clearFlags(
				WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
		}

	@Override
	public void onP2PConnected(String arg0) {
		// TODO Auto-generated method stub
		debugLog("onP2PConnected");
		
		AlienInvasion.gMainActivity.getSongGLLib().ResetFindTime(25);
		SongGLLib.sglSendMessage(SongGLLib.SGL_MATCHING_MSG, 0, 0, 0, 0);
		AlienInvasion.gMainActivity.getSongGLLib().SetMultiplayState(SongGLLib.SYNC_MATCHING);
	}

	@Override
	public void onP2PDisconnected(String arg0) {
		// TODO Auto-generated method stub
		debugLog("onP2PDisconnected");
	}

	@Override
	public void onPeerDeclined(Room arg0, List<String> arg1) {
		// TODO Auto-generated method stub
		debugLog("onPeerDeclined");
	}

	@Override
	public void onPeerInvitedToRoom(Room arg0, List<String> arg1) {
		// TODO Auto-generated method stub
		debugLog("onPeerInvitedToRoom");
	}

	@Override
	public void onPeerJoined(Room room, List<String> arg1) {
		debugLog("onPeerJoined");
		updateRoom(room);
	}

	@Override
	public void onPeerLeft(Room room, List<String> arg1) {
		debugLog("onPeerLeft");
		updateRoom(room);

	}

	@Override
	public void onPeersConnected(Room room, List<String> arg1) {
		debugLog("onPeersConnected list Cnt = " + arg1.size());
		updateRoom(room);
		
	}

	@Override
	public void onPeersDisconnected(Room room, List<String> arg1) {
		try
		{
			debugLog("onPeersDisconnected list Cnt = " + arg1.size());
			updateRoom(room);
			// cancel the game
			Games.RealTimeMultiplayer.leave(mActivity.getApiClient(), null,
					room.getRoomId());
			mActivity.getWindow().clearFlags(
					WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
			
			// $$ 상대팀이 나갔다.
		}
		catch(Exception e)
		{
			Log.e("Java", "onPeersDisconnected " + e.getLocalizedMessage());
		}
	}

	@Override
	public void onRoomAutoMatching(Room room) {
		debugLog("onRoomAutoMatching");
		updateRoom(room);
	}

	@Override
	public void onRoomConnecting(Room room) {
		debugLog("onRoomConnecting");
		updateRoom(room);
	}

	// /////////////////// End RoomStatusUpdateListener

	// /////////////////// RoomUpdateListener
	@Override
	public void onJoinedRoom(int statusCode, Room room) {
		debugLog("onJoinedRoom roomID " + room.getRoomId());
		if (statusCode != GamesStatusCodes.STATUS_OK) {
			// let screen go to sleep
			mActivity.getWindow().clearFlags(
					WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
			// $$ show error message, return to main screen.
			AlienInvasion.gMainActivity.getSongGLLib().SetMultiplayState(SongGLLib.SYNC_MATCHINGERROR);
			logError("onJoinedRoom statusCode = " + statusCode);
			mMatching = false;
			return;
		}
		//기다리는 화면이 안보여도 자동으로 연결을 할 수가 있다
		//showWaitingRoom(room);
	}

	/**
	 * 자신이 룸에서 나오면 이벤트가 발생한다 Games.RealTimeMultiplayer.leave(mGoogleApiClient,
	 * null, mRoomId);를 콜하면 룸에서 빠져나온 이벤트 발생.
	 */
	@Override
	public void onLeftRoom(int arg0, String roomid) {
		debugLog("onLeftRoom roomID " + roomid);
		//상단 상대 정보를 지워주자.
		SongGLLib.sglSendMessage(SongGLLib.SGL_SHOW_ENEMYTOOLS, 0, 0, 0, 0);
	}

	@Override
	public void onRoomConnected(int statusCode, Room room) {
		debugLog("onRoomConnected roomID " + room.getRoomId());
		if (statusCode != GamesStatusCodes.STATUS_OK) {
			// let screen go to sleep
			mActivity.getWindow().clearFlags(
					WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

			// $$ show error message, return to main screen.
			AlienInvasion.gMainActivity.getSongGLLib().SetMultiplayState(SongGLLib.SYNC_MATCHINGERROR);
			logError("onJoinedRoom statusCode = " + statusCode);
			mMatching = false;
			return;
		}
		updateRoom(room);

		if (shouldStartGame(room)) 
		{
			
			mMatching = false;
			mMatchStarted = true;
			AlienInvasion.gMainActivity.getSongGLLib().ResetFindTime(0);
			// $$ 누군가 접속하였다. 게임을 시작하자.
			//게임을 시작하자.
			mActivity.getSongGLLib().StartMultiplayGame();
		}
	}

	@Override
	public void onRoomCreated(int statusCode, Room room) {

		debugLog("onRoomCreated roomID " + room.getRoomId());
		if (statusCode != GamesStatusCodes.STATUS_OK) {
			// let screen go to sleep
			mActivity.getWindow().clearFlags(
					WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

			// 룸을 만드는데 실패하였다.
			// $$ show error message, return to main screen.
			AlienInvasion.gMainActivity.getSongGLLib().SetMultiplayState(SongGLLib.SYNC_MATCHINGERROR);
			mRoomId = null;
			mMatching = false;
			return;
		}
		
		//Matching중인데 onCreatedRoom이 되지 않은상태에서 FindMatch을 해버리면 기존의 것을 leav해주어야 한다.
		if(mbNeedLeftRoomID && mRoomId == null)
		{
			//룸을 나온다.
			Message msg = AlienInvasion.gMainActivity.mAppHandler
					.obtainMessage();
			Bundle b = new Bundle();
			b.putInt("id", 10);
			b.putString("roomid", room.getRoomId());
			msg.setData(b);
			AlienInvasion.gMainActivity.mAppHandler.sendMessage(msg);
		}
		mbNeedLeftRoomID = false;
		//기다리는 화면이 안보여도 자동으로 연결을 할 수가 있다
		//showWaitingRoom(room);
		mRoomId = room.getRoomId();
	}

	// /////////////////// End RoomUpdateListener

	// /////////////////// RealTimeMessageReceivedListener
	@Override
	public void onRealTimeMessageReceived(RealTimeMessage rtm) 
	{
		byte[] btData = rtm.getMessageData();
        String sSender = rtm.getSenderParticipantId();
        mActivity.getSongGLLib().OnReceivedData(sSender, btData);
	}
	// /////////////////// End RealTimeMessageReceivedListener
}
