package kr.co.songs.android.AlienInvasion;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.opengl.GLSurfaceView;
import android.util.Log;
import android.view.MotionEvent;

public class AlienInvasionRender implements GLSurfaceView.Renderer
{
	protected SongGLLib mSongGL = null;
	protected boolean  mbLoaded = false;
	
	public void onSurfaceCreated(GL10 gl, EGLConfig config) 
	{
		if(mbLoaded == false && mSongGL == null)
		{
			mSongGL = new SongGLLib();
			
			//초기화를 한다.
			mSongGL.Initialize();
			Log.i("JAVA","onSurfaceCreated mSongGL.Initialize()");
			
		}
		else if(mSongGL != null)
		{
			Log.i("JAVA","onSurfaceCreated mSongGL.ResetTexture()");
		
			mSongGL.ResetTexture();
		}
		
		Log.i("JAVA","Render onSurfaceCreated Resize Start");
		
		//OnPause After OnResume을 했을때 Black Screen으로 되는 현상은
		//카메라 뷰포트 프리즘 이러한 것을 다시 재설정을 해주어야 한다.
		//그래서 나의 로직은 Resize을 1,1로 하여 작게 하였다가.
		//onSurfaceChanged가 발생할때 정상적으로 뷰표트를 재조정하게 하였다.
		
		mSongGL.ResetViewPortX();
		
		//다시 실행 할때 리뷰 리졸트를 실행한다.
		int SGL_RESULT_REVIEW = 0x020031;
		SendMessage(SGL_RESULT_REVIEW,0,0);
		
		Log.i("JAVA","Render onSurfaceCreated Resize");
	}
	
	public void onDrawFrame(GL10 gl) 
	{
		if(mbLoaded == false && mSongGL != null)
		{
			String sPath = SongGLLib.getPath();
			mSongGL.Resource(sPath);
			mbLoaded = true;
		}
		//렌더링을 한다.
		if(mSongGL != null) mSongGL.Render();
	}

	public void onSurfaceChanged(GL10 gl, int width, int height) 
	{
		//뷰포트가 변환될때 발행한다.
		if(mSongGL != null) 
		{
			Log.i("JAVA","Render onSurfaceChanged = " + width+","+ height);
			mSongGL.Resize(width, height);
		}
			
	}

	public boolean onTouchEvent(MotionEvent event) 
	{
		int action = event.getAction() & MotionEvent.ACTION_MASK;
        int pointerIndex = (event.getAction() & MotionEvent.ACTION_POINTER_INDEX_MASK) >> MotionEvent.ACTION_POINTER_INDEX_SHIFT;
        int pointerId = event.getPointerId(pointerIndex);
        float x = event.getX(pointerIndex),y = event.getY(pointerIndex);
        
        if(mSongGL == null) return true;
        switch (action) {
        case MotionEvent.ACTION_DOWN:
        case MotionEvent.ACTION_POINTER_DOWN:
	        mSongGL.BeginTouch(pointerId, x, y);
            break;
        case MotionEvent.ACTION_UP:
        case MotionEvent.ACTION_POINTER_UP:
        case MotionEvent.ACTION_CANCEL:
            mSongGL.EndTouch(pointerId, x, y);
            break;
            
        case MotionEvent.ACTION_MOVE:
            mSongGL.MoveTouch(pointerId, x, y);
            break;
        default:
        	Log.i("AlienInvasion", "Default"+pointerId+"("+x+","+y+")" + "Action = "+action);
        	break;
        }
		return true;
	}
	
	
	public void Destory()
	{
		Log.i("JAVA","Render Destory");
		if(mSongGL != null)
		{
			mSongGL.Release();
			mSongGL = null;
			mbLoaded = false;
		}
		Log.i("JAVA","Render Destory After");
	}
	public long getGLContext() {
		 return mSongGL != null ? mSongGL.getGLContext() : 0;
	}	
	
	public boolean SendMessage(int nEventID,long param1,long param2)
	{
		if (mSongGL == null) {
			Log.w("JAVA", "SendMessage ignored because mSongGL is null. event=" + nEventID);
			return false;
		}
		return mSongGL.Event(nEventID, (int)param1, (int)param2);
	}
	
}
