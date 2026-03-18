package kr.co.songs.android.AlienInvasion;



import kr.co.songs.android.AlienInvasion.http.CHttpRequest;
import kr.co.songs.android.AlienInvasion.minkml.CMinkMLException;
import kr.co.songs.android.AlienInvasion.minkml.CRMConnect;
import kr.co.songs.android.AlienInvasion.minkml.CRMRecordSet;
import android.app.Activity;
import android.app.ProgressDialog;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;

public class BestScore extends Activity  implements OnClickListener 
{
	protected EditText mName;
	protected EditText mMessage;
	protected Button   mSave;
	protected ListView mList;
	protected BestScoreAdapter mAdapter;
	Handler mHandler = new android.os.Handler();
	Handler mHandler2 = new android.os.Handler();
	ProgressDialog mWaiting;
	private Thread mRequestThread;
	private Thread mUpdatetThread;
	
	String msUserID = "";
	String msMessage = "";
	Animation mShake;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.bestscore);
		mAdapter = new BestScoreAdapter();
		mShake = AnimationUtils.loadAnimation(this, R.anim.shakeani);
		mName = (EditText) findViewById(R.id.editName);
		mMessage = (EditText) findViewById(R.id.editMessage);
		mSave = (Button) findViewById(R.id.btnSave);
		mSave.setOnClickListener(this);
		mList = (ListView) findViewById(R.id.listView1);
		mList.setAdapter(mAdapter);		
		
		Bundle extras = getIntent().getExtras();
		if (extras != null) 
		{
			msUserID = extras.getString("USERID");
			msMessage = extras.getString("MESSAGE");
			  
		} 
		mName.setText(msUserID);
		mMessage.setText(msMessage);
		   
		
		
		mWaiting = ProgressDialog.show(this, "", "Loading. Please wait...", true);
		mWaiting.setCancelable(true);
		mRequestThread = new Thread()
		{
			public void run() 
			{
				//String userId, String password, String deviceID,String sInstallName, String sClsID,String sTarget,ASendDataToServer ConnObj
				CRMConnect conn = new CRMConnect(msUserID,"pwd",SongGLLib.GetUniqueID(),SongGLLib.GetBundleID(),"AlienInvasion","alieninvasion",
						new CHttpRequest("http://hohak.iptime.org:8080/AlienInvasion/AlienSvc",true,null));
				
				String[] lstFunc = new String[1];
				lstFunc[0] = "listscore";
				
				//name : value : mode : type | name : value : mode : type
				String[] lstInParam = new String[1];
				lstInParam[0] = String.format("%s%c%s%c%s%c%s", "XmlVersion",0x1E,"1.0",0x1E,"in",0x1E,"string");
				lstInParam[0] += String.format("%c%s%c%s%c%s%c%s", 0x1E,"DeviceID",0x1E,SongGLLib.GetUniqueID(),0x1E,"in",0x1E,"string");
				try 
				{
					CRMRecordSet mRec = conn.Function(lstFunc, lstInParam, null);
					if(mRec != null)
					{
						mAdapter.setRecordSet(mRec);
						mHandler.post(new Runnable() 
						{

				            public void run() 
				            {
				            	mAdapter.notifyDataSetChanged();
				            	
				            	CRMRecordSet Rec = mAdapter.getRecordSet();
				            	
				            	
				            	if(Rec.getCount() == 100)
				            		mList.setSelection(50-1);
				            	else
				            	{
				            		int nCnt = Rec.getCount();
				            		for(int i = 0; i < nCnt; i++)
				            		{
				            			Rec.MovePosition(i);
				            			if(Rec.getData(0).compareTo(msUserID) == 0)
				            			{
				            				if(i >= 1)
				            					mList.setSelection(i-1);
				            				break;
				            			}
				            		}
				            	}
				            		
				            	mWaiting.dismiss();
				            	mWaiting = null;
				            }
				        });
						
					}
				} 
				catch (CMinkMLException e) 
				{
					e.printStackTrace();
					Log.e("JAVA","BestScoreReauest : " + e.getMessage());
				}
				catch (Exception e)
				{
					Log.e("JAVA","BestScoreReauest Exception : " + e.getMessage());
				}
				
			};
		};
		mRequestThread.start(); 
		
	}
	
	
	@Override
	public void onClick(View arg0) {
		 
		
		if(mSave == arg0)
		{
			String sName = mName.getText().toString();
			if(sName.length() == 0)
			{
				mName.startAnimation(mShake);
				return ;
			}
				
			mWaiting = ProgressDialog.show(this, "", "Loading. Please wait...", true);
			mUpdatetThread = new Thread()
			{
				public void run() 
				{
					//String userId, String password, String deviceID,String sInstallName, String sClsID,String sTarget,ASendDataToServer ConnObj
					CRMConnect conn = new CRMConnect(msUserID,"pwd",SongGLLib.GetUniqueID(),SongGLLib.GetBundleID(),"AlienInvasion","alieninvasion",
							new CHttpRequest("http://hohak.iptime.org:8080/AlienInvasion/AlienSvc",true,null));
					
					String[] lstFunc = new String[1];
					lstFunc[0] = "updatename";
					
					 
					
					//name : value : mode : type | name : value : mode : type
					String[] lstInParam = new String[1];
					lstInParam[0] = String.format("%s%c%s%c%s%c%s", "XmlVersion",0x1E,"1.0",0x1E,"in",0x1E,"string");
					lstInParam[0] += String.format("%c%s%c%s%c%s%c%s", 0x1E,"DeviceID",0x1E,SongGLLib.GetUniqueID(),0x1E,"in",0x1E,"string");
					lstInParam[0] += String.format("%c%s%c%s%c%s%c%s", 0x1E,"AppID",0x1E,SongGLLib.GetBundleID(),0x1E,"in",0x1E,"string");
					lstInParam[0] += String.format("%c%s%c%s%c%s%c%s", 0x1E,"UserID",0x1E,mName.getText().toString(),0x1E,"in",0x1E,"string");
					
					if(mMessage.getText().toString().length() == 0)
						lstInParam[0] += String.format("%c%s%c%s%c%s%c%s", 0x1E,"Message",0x1E,"",0x1E,"in",0x1E,"string");
					else
						lstInParam[0] += String.format("%c%s%c%s%c%s%c%s", 0x1E,"Message",0x1E,mMessage.getText().toString(),0x1E,"in",0x1E,"string");
					
					try 
					{
						CRMRecordSet Rec = conn.Function(lstFunc, lstInParam, null);
						if(Rec != null)
						{
							mHandler2.post(new Runnable() 
							{
					            public void run() 
					            {	
					            	msUserID = mName.getText().toString();
					            	msMessage = mMessage.getText().toString();
					            	SongGLLib.sglSetUserInfo(msUserID, msMessage);
					            	
					            	if(mAdapter != null && mAdapter.getRecordSet() != null)
					            	{
					            		CRMRecordSet rc = mAdapter.getRecordSet();
					            		int nCount = rc.getCount();
					            		for (int i = 0; i < nCount; i++) {
					            			rc.MovePosition(i);
					            			if(SongGLLib.GetUniqueID().compareTo(rc.getData(0)) == 0)
					            			{
						            			 //device_id,rownum,user_id,total_score,rank,country,gold,message
						            			rc.setData(2, msUserID);
						            			rc.setData(7, msMessage);
						            			mAdapter.notifyDataSetChanged();
						            			break;
					            			}
											
										}
					            	}
					            	
					            	mWaiting.dismiss();
					            	mWaiting = null;
					            }
					        });
							
						}
					} 
					catch (CMinkMLException e) 
					{
						e.printStackTrace();
						Log.e("JAVA","BestScoreReauest : " + e.getMessage());
					}
					catch (Exception e)
					{
						Log.e("JAVA","BestScoreReauest Exception : " + e.getMessage());
					}
					
				};
			};
			mUpdatetThread.start(); 
		}
	}

}
