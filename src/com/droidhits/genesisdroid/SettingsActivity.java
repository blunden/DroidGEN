

package com.droidhits.genesisdroid;

import java.io.File;
import java.util.Map;

import com.droidhits.genesisdroid.R;



import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.app.AlertDialog.Builder;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.content.SharedPreferences.OnSharedPreferenceChangeListener;
import android.os.Bundle;
import android.os.Environment;
import android.preference.CheckBoxPreference;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.PreferenceManager;
import android.preference.Preference.OnPreferenceClickListener;
import android.preference.PreferenceActivity;
import android.util.Log;
import android.view.KeyEvent;
import android.widget.Toast;


/**
 * Settings activity. Allows easy access to any/all desired settings
 * and manages to auto save them
 * 
 * @author halsafar
 * 
 */
public class SettingsActivity extends PreferenceActivity implements
          OnSharedPreferenceChangeListener
{
     private static final String LOG_TAG         = "SettingsActivity";

     private ProgressDialog      _progressDialog = null;
     private String              _dirKeyChanging = Preferences.PREF_DIR_ROMS;

     @Override
     protected void onCreate(Bundle savedInstanceState)
     {
          Log.d(LOG_TAG, "onCreate()");

          super.onCreate(savedInstanceState);

          addPreferencesFromResource(R.layout.settings);

          // add onclick for input setup button
          Preference customPref = (Preference) findPreference("bntInputPref");
          customPref.setOnPreferenceClickListener(new OnPreferenceClickListener()
          {
               public boolean onPreferenceClick(Preference preference)
              {
                    try
                    {
                        Intent intent = new Intent(SettingsActivity.this,
                                  InputConfigActivity.class);
                        final int result = Preferences.MENU_INPUT_CONFIG;
                        startActivityForResult(intent, result);
                    }
                    catch (Exception e)
                    {
                         Log.e(LOG_TAG, "EXCEPTION: " + e.toString());
                    }

                   return true;
              }
          });

          // add onclick for reset input
          customPref = (Preference) findPreference("bntDefaultEmpty");
          customPref.setOnPreferenceClickListener(new OnPreferenceClickListener()
          {
               public boolean onPreferenceClick(Preference preference)
              {                                      
                   SharedPreferences prefs = PreferenceManager
                             .getDefaultSharedPreferences(getApplicationContext());
                   Editor edit = prefs.edit();
                   edit.putBoolean(Preferences.PREF_USE_DEFAULT_INPUT,
                             true);
               
                   edit.commit();
                   
                   // safe to reset input anytime, nice for people who are editing input
                   EmulatorButtons.resetInput(SettingsActivity.this, getApplicationContext());
               
                   return true;
              }
          });
                    
         // add on click for custom keys
          customPref = (Preference) findPreference("bntCustomKeys");
          customPref.setOnPreferenceClickListener(new OnPreferenceClickListener()
          {
               public boolean onPreferenceClick(Preference preference)
              {                                      
                    try
                    {
                        Intent intent = new Intent(SettingsActivity.this,
                                  KeyboardConfigActivity.class);
                        final int result = Preferences.MENU_CUSTOM_KEYS;
                        startActivityForResult(intent, result);
                    }
                    catch (Exception e)
                    {
                         Log.e(LOG_TAG, "EXCEPTION: " + e.toString());
                    }

                   return true;
              }
          });          

          // add on click warning for game genie
          CheckBoxPreference checkPref = (CheckBoxPreference) findPreference(Preferences.PREF_ENABLE_GAME_GENIE);
          if (checkPref != null)
          {
	          checkPref.setOnPreferenceClickListener(new OnPreferenceClickListener()
	          {
	               public boolean onPreferenceClick(Preference preference)
	              {
	                    boolean checkValue = ((CheckBoxPreference)preference).isChecked();                    
	                    if (checkValue)
	                    {
	                         // check for gg rom
	                         String romFile = Environment.getExternalStorageDirectory() + Preferences.DEFAULT_DIR + "/gg.rom";
	                         File f = new File(romFile);
	                         if (!f.exists())
	                         {
	                              Builder dialog = new AlertDialog.Builder(SettingsActivity.this)
	                              .setTitle("Game Genie Not Found")
	                              .setMessage("Game Genie not found, place game genie image at: " + romFile)
	                              .setPositiveButton("Ok", null);
	                              
	                              dialog.show();   
	                              
	                              ((CheckBoxPreference)preference).setChecked(false);
	                         }
	                         
	                         SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(getApplicationContext());
	                         boolean enableAutosave = prefs.getBoolean(Preferences.PREF_ENABLE_AUTO_SAVE, false);                    
	                         if (enableAutosave)
	                         {
	                              Toast.makeText(getApplicationContext(), "Cannot auto load with game genie enabled, will still auto save." , Toast.LENGTH_LONG).show();
	                         }                    
	                         
	                    }
	
	                   return false;
	              }
	          });
          }     
          // add onclick for reset to default shader
          checkPref = (CheckBoxPreference) findPreference(Preferences.PREF_ENABLE_AUTO_SAVE);
          checkPref.setOnPreferenceClickListener(new OnPreferenceClickListener()
          {
               public boolean onPreferenceClick(Preference preference)
              {
                    boolean checkValue = ((CheckBoxPreference)preference).isChecked();                    
                    if (checkValue)
                    {
                         SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(getApplicationContext());
                         boolean enableGameGenie = prefs.getBoolean(Preferences.PREF_ENABLE_GAME_GENIE, false);
                         
                         if (enableGameGenie)
                         {
                              Toast.makeText(getApplicationContext(), "Cannot auto load with game genie enabled, will still auto save." , Toast.LENGTH_LONG).show();
                         }
                         
                    }

                    return false;
              }
          });          
                    
          // set on click for directories
          customPref = (Preference) findPreference(Preferences.PREF_DIR_ROMS);
          customPref.setSummary(Preferences.getRomDir(getApplicationContext()));
          customPref.setOnPreferenceClickListener(new OnPreferenceClickListener()
          {
               public boolean onPreferenceClick(Preference preference)
              {
                    // spawn the file chooser
                    Intent myIntent = new Intent(SettingsActivity.this,
                              FileChooser.class);
                    myIntent.putExtra(FileChooser.EXTRA_SELECT_DIR, "true");
                    myIntent.putExtra(FileChooser.EXTRA_START_DIR, 
                                      Preferences.getRomDir(getApplicationContext()));
                    myIntent.putExtra(FileChooser.EXTRA_EXTENSIONS,
                              "");
                    final int result = Preferences.MENU_DIR_SELECT;
                    startActivityForResult(myIntent, result);

                    _dirKeyChanging = Preferences.PREF_DIR_ROMS;
                    
                    return true;
              }
          });
                                       
          // add preference change listener to update the summaries
          getPreferenceScreen().getSharedPreferences()
                    .registerOnSharedPreferenceChangeListener(this);
     }
     
     
     @Override
     public void onStart()
     {
          super.onStart();
          
          // @HACK - to display current value
          SharedPreferences prefs = PreferenceManager
                    .getDefaultSharedPreferences(getApplicationContext());
          Map<String, ?> allPrefs = prefs.getAll();

          for (String key : allPrefs.keySet())
          {
               Preference pref = findPreference(key);

               if (pref instanceof ListPreference)
               {
                    ListPreference listPref = (ListPreference) pref;
                    pref.setSummary(listPref.getEntry());
               }              
          }          
     }


     public void onSharedPreferenceChanged(SharedPreferences sharedPreferences,
               String key)
     {
          Preference pref = findPreference(key);

          if (pref instanceof ListPreference)
          {
               ListPreference listPref = (ListPreference) pref;
               pref.setSummary(listPref.getEntry());
          }
     }


     @Override
     protected void onActivityResult(int requestCode, int resultCode,
               Intent data)
     {
          Log.d(LOG_TAG, "onActivityResult()");

          if (requestCode == Preferences.MENU_INPUT_CONFIG)
          {
               Log.d(LOG_TAG, "onActivityResult(INPUT_CONFIG)");
          }
          else if (requestCode == Preferences.MENU_DIR_SELECT)
          {
               Log.d(LOG_TAG, "onActivityResult(DIR_SELECT)");
               
               String directory = data.getStringExtra(FileChooser.PAYLOAD_SELECTED_DIR);
               if (directory != null)
               {
                    Log.d(LOG_TAG, "NewDir: " + directory + " @ Key: " + _dirKeyChanging);
               }
               
               if (_dirKeyChanging != null)
               {
                    SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(getApplicationContext());
                    
                    // mod summary
                    Preference dirPref = findPreference(_dirKeyChanging);
                    dirPref.setSummary(directory);
                    
                    Editor edit = prefs.edit();
                    edit.putString(_dirKeyChanging, directory);
                    edit.commit();
               }
          }
     }


     @Override
     public boolean onKeyDown(int keyCode, KeyEvent event)
     {
          Log.d(LOG_TAG, "onKeyDown(" + keyCode + ")");
          if (keyCode == KeyEvent.KEYCODE_BACK)
          {
               Intent intent = new Intent();
               setResult(RESULT_OK, intent);

               finish();

               return true;
          }

          return super.onKeyDown(keyCode, event);
     }
}
