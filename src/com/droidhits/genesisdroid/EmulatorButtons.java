
package com.droidhits.genesisdroid;

import com.droidhits.genesisdroid.Preferences;

import android.app.Activity;
import android.content.Context;
import android.content.res.Configuration;
import android.util.DisplayMetrics;
import android.util.Log;


// Mimic the C++ side for compatibility
public enum EmulatorButtons
{
    BUTTON_INDEX_A,
    BUTTON_INDEX_B,
    BUTTON_INDEX_C,
    BUTTON_INDEX_X,
    BUTTON_INDEX_Y,
    BUTTON_INDEX_Z,
    BUTTON_INDEX_START,
    BUTTON_INDEX_LEFT,
    BUTTON_INDEX_UP,
    BUTTON_INDEX_RIGHT,
    BUTTON_INDEX_DOWN,
    BUTTON_INDEX_COUNT;
	
	private static String LOG_TAG = "EmulatorButtons"; 
     
     public static void resetInput(final Activity activity, final Context context)
     {          
    	 float screenWidth = Preferences.getRealScreenWidth(activity, context);
    	 float screenHeight = Preferences.getRealScreenHeight(activity, context);
    	           
          Log.d(LOG_TAG, "resetInput(" + screenWidth + ", " + screenHeight + ")");          
          
          float maxSize = (screenWidth > screenHeight ? screenWidth : screenHeight);

          float dpadWidth = maxSize * 0.25f;
          // tablet, reduce dpad defaults
          if (android.os.Build.VERSION.SDK_INT >= 11 && android.os.Build.VERSION.SDK_INT < 14)
          {
         	 dpadWidth = maxSize * 0.15f;
          }
          else if (android.os.Build.VERSION.SDK_INT >= 14)
          {
        	  if ((activity.getResources().getConfiguration().screenLayout &  Configuration.SCREENLAYOUT_SIZE_MASK) == Configuration.SCREENLAYOUT_SIZE_LARGE)
        	  {
              	 dpadWidth = maxSize * 0.15f;
        	  }
        	  else if ((activity.getResources().getConfiguration().screenLayout &  Configuration.SCREENLAYOUT_SIZE_MASK) == Configuration.SCREENLAYOUT_SIZE_XLARGE)
        	  {
               	 dpadWidth = maxSize * 0.10f;
        	  }        	  
          }
          float dpadHeight = dpadWidth;

          float dPadX = 0 + maxSize * 0.01f;
          float dPadY = screenHeight - dpadHeight - (maxSize * 0.01f);

          float buttonsWidth = maxSize * 0.085f;
          float buttonsHeight = maxSize * 0.085f;   
          float buttonSep = maxSize * 0.010f;
          
          // tablet, reduce button defaults
          if (android.os.Build.VERSION.SDK_INT >= 11 && android.os.Build.VERSION.SDK_INT < 14)
          {
              buttonsWidth = maxSize * 0.050f;
              buttonsHeight = maxSize * 0.050f;   
              buttonSep = maxSize * 0.20f;        	 
          }
          else if (android.os.Build.VERSION.SDK_INT >= 14)
          {
        	  if ((activity.getResources().getConfiguration().screenLayout &  Configuration.SCREENLAYOUT_SIZE_MASK) == Configuration.SCREENLAYOUT_SIZE_LARGE) 
        	  {
      			  	// on a large screen device ...
                  	buttonsWidth = maxSize * 0.050f;
                  	buttonsHeight = maxSize * 0.050f;   
                  	buttonSep = maxSize * 0.020f;        	 
        	  }        	  
        	  else if ((activity.getResources().getConfiguration().screenLayout &  Configuration.SCREENLAYOUT_SIZE_MASK) == Configuration.SCREENLAYOUT_SIZE_XLARGE) 
        	  {
      			  	// on a xlarge screen device ...
                  	buttonsWidth = maxSize * 0.045f;
                  	buttonsHeight = maxSize * 0.045f;   
                  	buttonSep = maxSize * 0.015f;        	 
        	  }        	  
          }
          
          float aButtonX = screenWidth - (buttonsWidth * 3) - buttonSep * 3;
          float aButtonY = screenHeight - buttonsHeight - (buttonsHeight / 4);
          
          float bButtonX = aButtonX + buttonsWidth + buttonSep;
          float bButtonY = aButtonY - (buttonsHeight / 3);
          
          float cButtonX = bButtonX + buttonsWidth + buttonSep;
          float cButtonY = bButtonY - (buttonsHeight / 3);
          
          float xButtonX = aButtonX;
          float xButtonY = aButtonY - buttonsHeight - buttonSep;
          
          float yButtonX = bButtonX;
          float yButtonY = bButtonY - buttonsHeight - buttonSep;
          
          float zButtonX = cButtonX;
          float zButtonY = cButtonY - buttonsHeight - buttonSep;          

          float startButtonWidth = maxSize * 0.075f;
          float startButtonHeight = maxSize * 0.04f;

          float startButtonX = (screenWidth / 2) - (startButtonWidth / 2);
          float startButtonY = screenHeight - startButtonHeight - (maxSize * 0.01f);  
          
          float ffButtonWidth = maxSize * 0.05f;
          float ffButtonHeight = maxSize * 0.025f;
          float ffButtonX = screenWidth - (ffButtonWidth*2 + (maxSize * 0.015f));
          float ffButtonY = 0 + ffButtonHeight + (maxSize * 0.015f);
          
          float rrButtonWidth = maxSize * 0.05f;
          float rrButtonHeight = maxSize * 0.025f;
          float rrButtonX = 0 + rrButtonWidth + (maxSize * 0.015f);
          float rrButtonY = 0 + rrButtonHeight + (maxSize * 0.015f);          
          
       // default textures
          String aTex = "Textures/ButtonA.png";
          String bTex = "Textures/ButtonB.png";
          String cTex = "Textures/ButtonC.png";
          String xTex = "Textures/ButtonX.png";
          String yTex = "Textures/ButtonY.png";
          String zTex = "Textures/ButtonZ.png";
          String startTex = "Textures/StartButton.png";
          String dPadTex = "Textures/DirectionalPad.png";
          
          
          // default button codes - using Xperia Play
          int aCode = 23; // X
          int bCode = 99; // Square
          int cCode = 4; // CIRCLE
          int xCode = 100; // Triangle
          int yCode = 102; // L!
          int zCode = 103; // R1
          int startCode = 108; // Startt
          int leftCode = 21;
          int upCode = 19;
          int rightCode = 22;
          int downCode = 20;
          
          InputPreferences.clearButtons(context);
          
          InputPreferences.setButton(context, 
                  cButtonX, cButtonY, buttonsWidth, buttonsHeight, 
                  cCode, cTex, EmulatorButtons.BUTTON_INDEX_C.ordinal());          
          
          InputPreferences.setButton(context, 
                    bButtonX, bButtonY, buttonsWidth, buttonsHeight, 
                    bCode, bTex, EmulatorButtons.BUTTON_INDEX_B.ordinal());
          
          InputPreferences.setButton(context, 
                    aButtonX, aButtonY, buttonsWidth, buttonsHeight, 
                    aCode, aTex, EmulatorButtons.BUTTON_INDEX_A.ordinal());
          
          InputPreferences.setButton(context, 
                  xButtonX, xButtonY, buttonsWidth, buttonsHeight, 
                  xCode, xTex, EmulatorButtons.BUTTON_INDEX_X.ordinal());
          
          InputPreferences.setButton(context, 
                  yButtonX, yButtonY, buttonsWidth, buttonsHeight, 
                  yCode, yTex, EmulatorButtons.BUTTON_INDEX_Y.ordinal());
          
          InputPreferences.setButton(context, 
                  zButtonX, zButtonY, buttonsWidth, buttonsHeight, 
                  zCode, zTex, EmulatorButtons.BUTTON_INDEX_Z.ordinal());          
          
          InputPreferences.setButton(context, 
                    startButtonX, startButtonY, startButtonWidth, startButtonHeight, 
                    startCode, startTex, EmulatorButtons.BUTTON_INDEX_START.ordinal());
                  
          
          // set the pseudo buttons for movements
          InputPreferences.setButton(context, 
                    0, 0, 0, 0, 
                    leftCode, null, EmulatorButtons.BUTTON_INDEX_LEFT.ordinal());          

          InputPreferences.setButton(context, 
                    0, 0, 0, 0, 
                    upCode, null, EmulatorButtons.BUTTON_INDEX_UP.ordinal());
          
          InputPreferences.setButton(context, 
                    0, 0, 0, 0, 
                    rightCode, null, EmulatorButtons.BUTTON_INDEX_RIGHT.ordinal());
          
          InputPreferences.setButton(context, 
                    0, 0, 0, 0, 
                    downCode, null, EmulatorButtons.BUTTON_INDEX_DOWN.ordinal());          
          
          InputPreferences.setAnalog(context, 
                    dPadX, dPadY, dpadWidth, dpadHeight, 
                    leftCode, upCode, rightCode, downCode,
                    dPadTex, 
                    0);                  
     }     
}

