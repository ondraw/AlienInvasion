package kr.co.songs.android.AlienInvasion;

import android.content.Context;
import android.graphics.PixelFormat;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;

public class AlienInvasionView extends GLSurfaceView {

	protected AlienInvasionRender mRender;
	public AlienInvasionView(Context context) {
		super(context);

		Log.i("JAVA", "AlienInvasionView");

		mRender = new AlienInvasionRender();
		// Stencil Buffer
		setEGLConfigChooser(8, 8, 8, 8, 16, 8);
		getHolder().setFormat(PixelFormat.RGBA_8888);
		// Keep the GL surface below regular Android views so AdMob banners can render above it.
		setZOrderOnTop(false);
		// setEGLConfigChooser(5,6,5,0,16,8);
		setRenderer(mRender);
	}

	public AlienInvasionView(Context context, AttributeSet attrs) {
		super(context, attrs);
		Log.i("JAVA", "AlienInvasionView");

		mRender = new AlienInvasionRender();
		// Stencil Buffer
		setEGLConfigChooser(8, 8, 8, 8, 16, 8);
		getHolder().setFormat(PixelFormat.RGBA_8888);
		// Keep the GL surface below regular Android views so AdMob banners can render above it.
		setZOrderOnTop(false);
		// setEGLConfigChooser(5,6,5,0,16,8);
		setRenderer(mRender);
	}

	@Override
	public boolean onTouchEvent(final MotionEvent event) {
		// Deletede By Song 2013.01.09 큐에서 보내면 핀치기능에서 두번째 탭이벤트가 연속해서 여러번 날라오거나
		// 업에 대한 이벤트가 존재 하지 않음.
		// 내부적으로 GlTextureBound를 하면 죽는다. 현재 터치시 폭탄을 발사하고 잇는데 여기서 gl함수를 사용해서 죽어버려
		// 미사일 설정시 model을 읽어오게 하여 이미 만들어진 객체를 재사용하게 하였다. void
		// CK9Sprite::SetCurrentSelMissaileInfo(USERINFO* v)
		// queueEvent(new Runnable()
		// {
		// public void run()
		// {
		mRender.onTouchEvent(event);
		// }
		// });
		return true;
	}

	@Override
	public boolean onTrackballEvent(MotionEvent event) {

		return super.onTrackballEvent(event);
	}

	public void Destory() {
		if (mRender != null) {
			Log.i("JAVA", "AlienInvasionView Destory");
			mRender.Destory();
			mRender = null;
		}
	}

	public boolean SendMessage(final int nEventID, final long param1,
			final long param2) {
		queueEvent(new Runnable() {
			public void run() {
				if (mRender != null) {
					mRender.SendMessage(nEventID, param1, param2);
				}
			}
		});

		return true;
	}

}
