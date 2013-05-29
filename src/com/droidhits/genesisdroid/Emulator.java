
package com.droidhits.genesisdroid;

/**
 * MAJOR TODO 	- remove app specific stuff in PreferenceFacade
 */

/**
 * Emulator bridge to native side.
 * All Java to Native communication happens through this one class.
 * See jni/EmulatorBridge.cpp to see the implementation 
 *
 */
public class Emulator
{	
     // load our native library
	static
	{
       System.loadLibrary("gnupng");	     
	   System.loadLibrary("zip");
	   System.loadLibrary("genesis");	   
	}	
	
	// lock instantiation
	private Emulator()
	{
		
	}
	
	public static native int init(final String apkAbsolutePath);
	
	public static native int setPaths(final String externalStoragePath, 
									final String romPath,
									final String stateSavePath,
									final String sramPath,
									final String cheatPath);
	
	public static native int initGraphics();
	
    public static native int initAudioBuffer(final int sizeInSamples);	
	
	public static native int loadRom(final String fileName);
	
	public static native boolean isRomLoaded();
	
	public static native void onTouchDown(final int finger, final float x, final float y, final float radius);
	
	public static native void onTouchUp(final int finger, final float x, final float y, final float radius);
	
	public static native void onTouchMove(final int finger, final float x, final float y, final float radius);
	
	public static native void onKeyDown(int keyCode);
	
	public static native void onKeyUp(int keyCode);
	
	public static native int mixAudioBuffer(short[] buf);
		
	public static native void setAudioSampleRate(final int rate);		
	
	public static native void setAudioEnabled(final boolean b);
	
	public static native void setSensitivity(final float x, final float y);
	
	public static native void setButton(final int buttonIndex, final float x, final float y, final float width, final float height, int keycode, boolean visible);	
		
	public static native void setAnalog(final float x, final float y, final float width, final float height,
	                                   int leftcode, int upcode, int rightcode, int downcode, boolean visible);
	
	public static native void setSmoothFiltering(boolean b);
	
	public static native void setViewport(final int width, final int height);
	
	public static native void saveState(final int i);
	
	public static native void loadState(final int i);
	
	public static native void selectState(final int i);
	
	public static native void resetGame();
	
	public static native void unzipFile(final String zipFileName, final String extractFileName, final String outLocation);
	
	public static native void setAspectRatio(final float ratio);
	
	public static native void setFrameSkip(final int i);
	
	public static native void setGameGenie(final boolean b);	
		
	public static native void destroy();
	
	public static native void step();
	
	public static native void draw();
}