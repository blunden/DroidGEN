
package com.droidhits.genesisdroid;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.preference.PreferenceManager;
import android.util.Log;


public class InputPreferences
{
     private static final String LOG_TAG = "InputPreferences";
     
     private static final String PREF_NUM_ANALOGS = "input_numAnalogs";
     private static final String PREF_NUM_BUTTONS = "input_numButtons";
     private static final String PREF_BUTTON_PREFIX = "input_button";
     private static final String PREF_ANALOG_PREFIX = "input_analog";
     
     //
     // GENERIC
     //
     private static String buildKey(String prefix, int index)
     {
          return prefix + "_" + index + "_";
     }
     
     private static int getIntPref(final Context context, String prefix, String value, int index)
     {
          SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(context);
          return prefs.getInt(buildKey(prefix, index) + value, 0);
     }
     
     private static float getFloatPref(final Context context, String prefix, String value, int index)
     {
          SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(context);
          return prefs.getFloat(buildKey(prefix, index) + value, 0.0f);
     }    
     
     private static String getStringPref(final Context context, String prefix, String value, int index)
     {
          SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(context);
          return prefs.getString(buildKey(prefix, index) + value, null);
     }     
          
     
     private static int getNumInput(final Context context, final String key)
     {
          SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(context);
          
          int n = prefs.getInt(key, 0);
          
          return n;          
     }
          
          
     //
     // BUTTONS
     //
     public static int getNumButtons(final Context context)
     {
          //return getNumInput(context, PREF_NUM_BUTTONS);
    	 return EmulatorButtons.BUTTON_INDEX_COUNT.ordinal();
     }

     public static void setButton(Context context, String textureFile, int buttonIndex)
     {
          float x = getButtonX(context, buttonIndex);
          float y = getButtonY(context, buttonIndex);
          float w = getButtonWidth(context, buttonIndex);
          float h = getButtonHeight(context, buttonIndex);
          int keycode = getButtonCode(context, buttonIndex);
          
          setButton(context, x, y, w, h, keycode, textureFile, buttonIndex);
     }
     
    
     public static void setButton(Context context, int keycode, int buttonIndex)
     {
          float x = getButtonX(context, buttonIndex);
          float y = getButtonY(context, buttonIndex);
          float w = getButtonWidth(context, buttonIndex);
          float h = getButtonHeight(context, buttonIndex);
          String textureFile = getButtonTexture(context, buttonIndex);
          setButton(context, x, y, w, h, keycode, textureFile, buttonIndex);
     }     
     
     public static void setButton(Context context, float x, float y, float w, float h, int buttonIndex)
     {
          int keycode = getButtonCode(context, buttonIndex);
          String textureFile = getButtonTexture(context, buttonIndex);
          setButton(context, x, y, w, h, keycode, textureFile, buttonIndex);
     }
     
     
     public static void setButton(Context context, float x, float y, float w, float h, int keycode, String textureFile, int buttonIndex)
     {
          Log.d(LOG_TAG, "setButton(" + x + ", " + y + ", " + w + ", " + h + 
                    ", " + keycode + 
                    ", " + textureFile + ", " + buttonIndex);          
          
          int numButtons = getNumButtons(context);
          String buttonKey = buildKey(PREF_BUTTON_PREFIX, buttonIndex);
          
          SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(context);
          Editor edit = prefs.edit();
          if (!prefs.contains(buttonKey))
          {
               edit.putInt(PREF_NUM_BUTTONS, numButtons + 1);
          }
                    
          edit.putBoolean(buttonKey, true);
          edit.putFloat(buttonKey + "x", x);
          edit.putFloat(buttonKey + "y", y);
          edit.putFloat(buttonKey + "w", w);
          edit.putFloat(buttonKey + "h", h);
          edit.putInt(buttonKey + "code", keycode);
          edit.putInt(buttonKey + "map", buttonIndex);
          edit.putString(buttonKey + "texture", textureFile);
                    
          edit.commit();
     }    
     
     public static float getButtonX(final Context context, int buttonIndex)
     {
          return getFloatPref(context, PREF_BUTTON_PREFIX, "x", buttonIndex);
     }
     

     public static float getButtonY(final Context context, int buttonIndex)
     {
          return getFloatPref(context, PREF_BUTTON_PREFIX, "y", buttonIndex);
     }
     
     
     public static float getButtonWidth(final Context context, int buttonIndex)
     {
          return getFloatPref(context, PREF_BUTTON_PREFIX, "w", buttonIndex);
     }
     
     
     public static float getButtonHeight(final Context context, int buttonIndex)
     {
          return getFloatPref(context, PREF_BUTTON_PREFIX, "h", buttonIndex);
     }
     
     
     public static int getButtonCode(final Context context, int buttonIndex)
     {
          return getIntPref(context, PREF_BUTTON_PREFIX, "code", buttonIndex);
     }
     
     
     public static int getButtonMap(final Context context, int buttonIndex)
     {
          return getIntPref(context, PREF_BUTTON_PREFIX, "map", buttonIndex);
     }     
     
     public static String getButtonTexture(final Context context, int index)
     {
          return getStringPref(context, PREF_BUTTON_PREFIX, "texture", index);
     }     
     
     public static void clearButtons(final Context context)
     {
          int numButtons = getNumButtons(context);
          
          SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(context);
          Editor edit = prefs.edit();          
          
          for (int i = 0; i < numButtons; i++)
          {              
               String key = buildKey(PREF_BUTTON_PREFIX, i);
               edit.remove(key);
               edit.remove(key + "x");
               edit.remove(key + "y");
               edit.remove(key + "w");
               edit.remove(key + "h");
               edit.remove(key + "code");
               edit.remove(key + "map");
               edit.remove(key + "texture");
          }
          
          edit.putInt(PREF_NUM_BUTTONS, 0);
          
          edit.commit();
     }
     
          
     // 
     // ANALOGS - DPADS
     //
     public static int getNumAnalogs(Context context)
     {
          return getNumInput(context, PREF_NUM_ANALOGS);
     }     
     
     
     public static void setAnalog(Context context, String textureFile, int buttonIndex)
     {
          float x = getAnalogX(context, buttonIndex);
          float y = getAnalogY(context, buttonIndex);
          float w = getAnalogWidth(context, buttonIndex);
          float h = getAnalogHeight(context, buttonIndex);
          int leftcode = getAnalogLeftCode(context, buttonIndex);
          int upcode = getAnalogUpCode(context, buttonIndex);
          int rightcode = getAnalogRightCode(context, buttonIndex);
          int downcode = getAnalogDownCode(context, buttonIndex);
          
          setAnalog(context, x, y, w, h, leftcode, upcode, rightcode, downcode, textureFile, buttonIndex);
     }
     
     public static void setAnalog(Context context, float x, float y, float w, float h, int buttonIndex)
     {
          int leftcode = getAnalogLeftCode(context, buttonIndex);
          int upcode = getAnalogUpCode(context, buttonIndex);
          int rightcode = getAnalogRightCode(context, buttonIndex);
          int downcode = getAnalogDownCode(context, buttonIndex);
          String textureFile = getAnalogTexture(context, buttonIndex);
          setAnalog(context, x, y, w, h, leftcode, upcode, rightcode, downcode, textureFile, buttonIndex);
     }
     
     
     public static void setAnalog(Context context, 
                                        float x, float y, float w, float h, 
                                        int keycodeLeft, int keycodeUp, int keycodeRight, int keycodeDown,  
                                        final String textureFile,
                                        int index)
     {
          Log.d(LOG_TAG, "setAnalog(" + x + ", " + y + ", " + w + ", " + h + 
                         ", " + keycodeLeft + ", " + keycodeUp + ", " + keycodeRight + ", " + keycodeDown +
                         ", " + textureFile + ", " + index);
          int num = getNumAnalogs(context);
          String buttonKey = buildKey(PREF_ANALOG_PREFIX, index);
          
          SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(context);
          Editor edit = prefs.edit();
          if (!prefs.contains(buttonKey))
          {
               edit.putInt(PREF_NUM_ANALOGS, num + 1);
          }
                    
          edit.putBoolean(buttonKey, true);
          edit.putFloat(buttonKey + "x", x);
          edit.putFloat(buttonKey + "y", y);
          edit.putFloat(buttonKey + "w", w);
          edit.putFloat(buttonKey + "h", h);
          edit.putInt(buttonKey + "codeLeft", keycodeLeft);
          edit.putInt(buttonKey + "codeUp", keycodeUp);
          edit.putInt(buttonKey + "codeRight", keycodeRight);
          edit.putInt(buttonKey + "codeDown", keycodeDown);
          edit.putInt(buttonKey + "map", index);
          edit.putString(buttonKey + "texture", textureFile);
                    
          edit.commit();          
     }
     
     public static float getAnalogX(final Context context, int buttonIndex)
     {
          return getFloatPref(context, PREF_ANALOG_PREFIX, "x", buttonIndex);
     }
     

     public static float getAnalogY(final Context context, int buttonIndex)
     {
          return getFloatPref(context, PREF_ANALOG_PREFIX, "y", buttonIndex);
     }
     
     
     public static float getAnalogWidth(final Context context, int buttonIndex)
     {
          return getFloatPref(context, PREF_ANALOG_PREFIX, "w", buttonIndex);
     }
     
     
     public static float getAnalogHeight(final Context context, int buttonIndex)
     {
          return getFloatPref(context, PREF_ANALOG_PREFIX, "h", buttonIndex);
     }
     
     
     public static int getAnalogLeftCode(final Context context, int buttonIndex)
     {
          return getIntPref(context, PREF_ANALOG_PREFIX, "codeLeft", buttonIndex);
     }
     
     
     public static int getAnalogUpCode(final Context context, int buttonIndex)
     {
          return getIntPref(context, PREF_ANALOG_PREFIX, "codeUp", buttonIndex);
     }
     
     
     public static int getAnalogRightCode(final Context context, int buttonIndex)
     {
          return getIntPref(context, PREF_ANALOG_PREFIX, "codeRight", buttonIndex);
     }
     
     
     public static int getAnalogDownCode(final Context context, int buttonIndex)
     {
          return getIntPref(context, PREF_ANALOG_PREFIX, "codeDown", buttonIndex);
     }     
     
     
     public static int getAnalogMap(final Context context, int buttonIndex)
     {
          return getIntPref(context, PREF_ANALOG_PREFIX, "map", buttonIndex);
     }     
     
     
     public static String getAnalogTexture(final Context context, int index)
     {
          return getStringPref(context, PREF_ANALOG_PREFIX, "texture", index);
     }
     
     
     public static void clearAnalogs(final Context context)
     {
          int num = getNumAnalogs(context);
          
          SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(context);
          Editor edit = prefs.edit();          
          
          for (int i = 0; i < num; i++)
          {              
               String key = buildKey(PREF_ANALOG_PREFIX, i);
               edit.remove(key);
               edit.remove(key + "x");
               edit.remove(key + "y");
               edit.remove(key + "w");
               edit.remove(key + "h");
               edit.remove(key + "code");
               edit.remove(key + "map");
               edit.remove(key + "texture");
          }
          
          edit.putInt(PREF_NUM_ANALOGS, 0);
          
          edit.commit();
     }     
}
