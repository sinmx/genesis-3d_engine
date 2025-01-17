/****************************************************************************
Copyright (c) 2006, Radon Labs GmbH
Copyright (c) 2011-2013,WebJet Business Division,CYOU

http://www.genesis-3d.com.cn

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
package org.genesis.lib;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.content.res.AssetManager;
import android.opengl.GLSurfaceView;
import android.os.Message;
import android.util.Log;

import org.genesis.lib.GenesisGLSurfaceView;

public class GenesisRenderer implements GLSurfaceView.Renderer {


	private final static long NANOSECONDSPERSECOND = 1000000000L;
	private final static long NANOSECONDSPERMICROSECOND = 1000000;

	private static long sAnimationInterval = (long) (1.0 / 60 * NANOSECONDSPERSECOND);

	// ===========================================================
	// Fields
	// ===========================================================

	private long mLastTickInNanoSeconds;
	private int mScreenWidth;
	private int mScreenHeight;
	
	private float mScreenDPIWidth;
	private float mScreenDPIHeight;
	
	private AssetManager mAssetManager;
	
	private GenesisGLSurfaceView mGenesisGLSurfaceView;
	long startTime = System.nanoTime();
	int frames = 0;

	private GenesisHandler handler;
	// ===========================================================
	// Constructors
	// ===========================================================

	// ===========================================================
	// Getter & Setter
	// ===========================================================
	
	public GenesisRenderer(GenesisActivity act)
	{
		handler = new GenesisHandler(act);
	}
	public static void setAnimationInterval(final double pAnimationInterval) {
		sAnimationInterval = (long) (pAnimationInterval * NANOSECONDSPERSECOND);
	}

	public void setScreenWidthAndHeight(final int pSurfaceWidth, final int pSurfaceHeight,final float dpiW,final float dpiH,AssetManager mgr) {
		this.mScreenWidth = pSurfaceWidth;
		this.mScreenHeight = pSurfaceHeight;
		mScreenDPIWidth = dpiW;
		mScreenDPIHeight = dpiH;		
		mAssetManager = mgr;
	}
	
	public void setConfig(final String gamedir, final String scenename,final boolean bShader) {
		nativeConfig(gamedir,scenename,bShader);
	}

	public void setStorage(final int type, final String packagename, final String apklocation) {
		nativeStorage(type,packagename,apklocation);
	}
	// ===========================================================
	// Methods for/from SuperClass/Interfaces
	// ===========================================================

	@Override
	public void onSurfaceCreated(final GL10 pGL10, final EGLConfig pEGLConfig) {
			
		nativeInit(this.mScreenWidth, this.mScreenHeight,mScreenDPIWidth,mScreenDPIHeight,mAssetManager);
		this.mLastTickInNanoSeconds = System.nanoTime();
		
		
		Message msg = new Message();
		msg.what = GenesisHandler.HANDLER_CANCEL_BACKGROUND;
		msg.obj = null;
		this.handler.sendMessage(msg);
	
		
	}

	@Override
	public void onSurfaceChanged(final GL10 pGL10, final int pWidth, final int pHeight) 
	{
		//Cocos2dxRenderer.nativeInit(this.mScreenWidth, this.mScreenHeight);
	}
	
	public void SetGenesisGLSurfaceView(GenesisGLSurfaceView view)
	{
		mGenesisGLSurfaceView = view;
	}
	
	@Override
	public void onDrawFrame(final GL10 gl) {
		
		
		//mGenesisGLSurfaceView.setBackgroundResource(0);
		final long nowInNanoSeconds = System.nanoTime();
		final long interval = nowInNanoSeconds - this.mLastTickInNanoSeconds;

		// should render a frame when onDrawFrame() is called or there is a
		// "ghost"
		nativeRender();
		
		frames++;
	    if(System.nanoTime() - startTime >= 1000000000) {
	        Log.d("FPSCounter", "fps: " + frames);
	        frames = 0;
	        startTime = System.nanoTime();
	    }


		
		

		this.mLastTickInNanoSeconds = nowInNanoSeconds;
	}

	// ===========================================================
	// Methods
	// ===========================================================

	private static native void nativeTouchesBegin(final int pID, final float pX, final float pY);
	private static native void nativeTouchesEnd(final int pID, final float pX, final float pY);
	private static native void nativeTouchesMove(final int[] pIDs, final float[] pXs, final float[] pYs);
	private static native void nativeTouchesCancel(final int[] pIDs, final float[] pXs, final float[] pYs);
	private static native boolean nativeKeyDown(final int pKeyCode);
	private static native void nativeRender();
	private static native void nativeInit(final int pWidth, final int pHeight,final float dpiW,final float dpiH , AssetManager assetManager);
	private static native void nativeConfig(final String gamedir, final String scenename,final boolean bShader);
	private static native void nativeStorage(final int type, final String pakagename, final String apklocation);
	private static native void nativeOnPause();
	private static native void nativeOnResume();
	private static native void nativeOnDestroy();
	private static native void nativeOnStop();

	public void handleActionDown(final int pID, final float pX, final float pY) {
		GenesisRenderer.nativeTouchesBegin(pID, pX, pY);
	}

	public void handleActionUp(final int pID, final float pX, final float pY) {
		GenesisRenderer.nativeTouchesEnd(pID, pX, pY);
	}

	public void handleActionCancel(final int[] pIDs, final float[] pXs, final float[] pYs) {
		GenesisRenderer.nativeTouchesCancel(pIDs, pXs, pYs);
	}

	public void handleActionMove(final int[] pIDs, final float[] pXs, final float[] pYs) {
		GenesisRenderer.nativeTouchesMove(pIDs, pXs, pYs);
	}

	public void handleKeyDown(final int pKeyCode) {
		GenesisRenderer.nativeKeyDown(pKeyCode);
	}

	public void handleOnPause() {
		GenesisRenderer.nativeOnPause();
	}

	public void handleOnResume() {
		GenesisRenderer.nativeOnResume();
	}
	
	public void handleOnDestroy()
	{
		//Cocos2dxRenderer.nativeOnDestroy();
	}
	
	public void handleOnStop()
	{
		GenesisRenderer.nativeOnStop();
	}

	private static native void nativeInsertText(final String pText);
	private static native void nativeDeleteBackward();
	private static native String nativeGetContentText();

	public void handleInsertText(final String pText) {
		GenesisRenderer.nativeInsertText(pText);
	}

	public void handleDeleteBackward() {
		GenesisRenderer.nativeDeleteBackward();
	}

	public String getContentText() {
		return GenesisRenderer.nativeGetContentText();
	}


}