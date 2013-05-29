
package com.droidhits.genesisdroid;

import java.io.File;

import com.droidhits.genesisdroid.Emulator;
import com.droidhits.genesisdroid.Preferences;

import com.tizmoplay.androgens.R;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.AlertDialog.Builder;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.content.SharedPreferences.Editor;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.os.Bundle;
import android.os.Environment;
import android.preference.PreferenceManager;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;


public class MainActivity extends Activity
{          
     private static final String LOG_TAG = "MainActivity";
                    
     //private boolean _useAudio = false; 
     private boolean _init = false;         
            

     @Override
     public void onCreate(Bundle savedInstanceState)
     {
          Log.d(LOG_TAG, "onCreate()");
          
          super.onCreate(savedInstanceState);         

          System.gc();
          
          init();          
     }
              
     
     private boolean verifyExternalStorage()
     {
       // check for sd card first
          boolean mExternalStorageAvailable = false;
          boolean mExternalStorageWriteable = false;
          String state = Environment.getExternalStorageState();

          if (Environment.MEDIA_MOUNTED.equals(state))
          {
              // rw access
              mExternalStorageAvailable = mExternalStorageWriteable = true;
          } 
          else if (Environment.MEDIA_MOUNTED_READ_ONLY.equals(state))
          {
              // r access
              mExternalStorageAvailable = true;
              mExternalStorageWriteable = false;
          }
          else 
          {
              // no access
              mExternalStorageAvailable = mExternalStorageWriteable = false;
          }
          
          // if we do not have storage warn user with dialog
          if (!mExternalStorageAvailable || !mExternalStorageWriteable)
          {
               Builder dialog = new AlertDialog.Builder(this)
                                        .setTitle(getString(R.string.app_name) + " Error")
                                        .setMessage("External Storage not mounted, are you in disk mode?")
                                        .setPositiveButton("Retry", new OnClickListener()
                                        {
                                             
                                             public void onClick(DialogInterface dialog, int which)
                                             {
                                                  init();
                                             }
                                        })
                                        .setNeutralButton("Exit", new OnClickListener()
                                        {
                                             
                                             public void onClick(DialogInterface dialog, int which)
                                             {
                                                  finish();
                                             }
                                        });
                                        
               dialog.show();
               
               return false;
          }          
          
          return true;
     }
     
     
     private void init()
     {
          if (verifyExternalStorage() && !_init)
          {                                                                                     
               // always try and make application dirs
               String extStorageDirectory = Environment.getExternalStorageDirectory().toString();
               
               File myNewFolder = new File(extStorageDirectory + Preferences.DEFAULT_DIR);
               if (!myNewFolder.exists())
               {
                    myNewFolder.mkdir();
               }                             
               
               myNewFolder = new File(extStorageDirectory + Preferences.DEFAULT_DIR_ROMS);
               if (!myNewFolder.exists())
               {
                    myNewFolder.mkdir();
               }
               
               myNewFolder = new File(extStorageDirectory + Preferences.DEFAULT_DIR_STATES);
               if (!myNewFolder.exists())
               {
                    myNewFolder.mkdir();
               }
               
               myNewFolder = new File(extStorageDirectory + Preferences.DEFAULT_DIR_SRAM);
               if (!myNewFolder.exists())
               {
                    myNewFolder.mkdir();
               }
               
               myNewFolder = new File(extStorageDirectory + Preferences.DEFAULT_DIR_TEMPFILES);
               if (!myNewFolder.exists())
               {
                    myNewFolder.mkdir();
               }                 
               
               // do first run welcome screen
               SharedPreferences preferences = PreferenceManager.getDefaultSharedPreferences(getApplicationContext());                             
               boolean firstRun = preferences.getBoolean(Preferences.PREF_FIRST_RUN, true);
               if (firstRun)
               {                                                                 
                    // remove first run flag
                    Editor edit = preferences.edit();
                    edit.putBoolean(Preferences.PREF_FIRST_RUN, false);

                    // default input
                    edit.putBoolean(Preferences.PREF_USE_DEFAULT_INPUT, true);
                    edit.commit();                    
               }
               
               // generate APK path for the native side
               ApplicationInfo appInfo = null;
               PackageManager packMgmr = this.getPackageManager();
               try
               {
                    appInfo = packMgmr.getApplicationInfo(getString(R.string.package_name), 0);
               } 
               catch (NameNotFoundException e)
               {
                    e.printStackTrace();
                    throw new RuntimeException("Unable to locate assets, aborting...");
               }
               String _apkPath = appInfo.sourceDir;                                                       

               // init the emulator
               Emulator.init(_apkPath);
               
               // set the paths
               Emulator.setPaths(extStorageDirectory + Preferences.DEFAULT_DIR,
            		   				extStorageDirectory + Preferences.DEFAULT_DIR_ROMS, 
            		   				extStorageDirectory + Preferences.DEFAULT_DIR_STATES,
            		   				extStorageDirectory + Preferences.DEFAULT_DIR_SRAM, 
            		   				extStorageDirectory + Preferences.DEFAULT_DIR_CHEATS);

               // load up prefs now, never again unless they change
               //PreferenceFacade.loadPrefs(this, getApplicationContext());                   
               
               // load gui
               Log.d(LOG_TAG, "Done init()");
               setContentView(R.layout.main);
               initGUIEvent();               
               
               // set title
               super.setTitle(getString(R.string.app_name));                         
               
               _init = true;
               
               Log.d(LOG_TAG, "Done onCreate()");
          }           
     }
             
     
     private void initGUIEvent()
     {
          Button tmpButton = (Button) findViewById(R.id.buttonLoadRom);
          tmpButton.setOnClickListener(
             new View.OnClickListener()
             {
                public void onClick(View v)
                {
                    spawnFileChooser();
                }
             });               

          tmpButton = (Button) findViewById(R.id.buttonResume);
          tmpButton.setOnClickListener(
             new View.OnClickListener()
             {
                public void onClick(View v)
                {
                    if (Emulator.isRomLoaded())
                    {
                         spawnEmulatorActivity();
                    }
                    else
                    {
                         Toast.makeText(getApplicationContext(), "No ROM loaded...", Toast.LENGTH_SHORT).show();
                    }
                }
             });            
          
          tmpButton = (Button) findViewById(R.id.buttonSettings);
          tmpButton.setOnClickListener(
             new View.OnClickListener()
             {
                public void onClick(View v)
                {
                    spawnSettings();
                }
             });            
          
          tmpButton = (Button) findViewById(R.id.buttonExit);
          tmpButton.setOnClickListener(
             new View.OnClickListener()
             {
                public void onClick(View v)
                {
                    finish();
                }
             });                       
     }                   
     

     @Override
     public void onPause()
     {
          Log.d(LOG_TAG, "onPause()");
                    
          super.onPause();
     }
     
     
     @Override
     public void onStop()
     {
          Log.d(LOG_TAG, "onStop()");         
          
          super.onStop();
     }
     
     
     @Override
     public void onDestroy()
     {
          Log.d(LOG_TAG, "onDestroy()");

          super.onDestroy();      
                    
          AudioPlayer.destroy();          
          Emulator.destroy();           
          
          // check for auto save
          Preferences.DoAutoSave(getApplicationContext());
          
          // clean temp dir or all files, just files for now
          // @TODO - full clean
          File tempFolder = new File(Environment.getExternalStorageDirectory().toString() + Preferences.DEFAULT_DIR_TEMPFILES + "/");
          if (tempFolder.exists())
          {
               String[] children = tempFolder.list();
               for (int i=0; i<children.length; i++)
               {
                    File tempFile = new File(tempFolder.getAbsoluteFile(), children[i]);
                    if (tempFile.exists())
                    {
                         tempFile.delete();
                    }
               }
          }    
          
          _init = false;
          
          // garbage collect... help android out a bit
          System.gc();
     }

         
     protected void spawnEmulatorActivity()
     {
          Intent myIntent = new Intent(MainActivity.this, EmuActivity.class);
          startActivity(myIntent);
     }
     
     
     protected void spawnFileChooser()
     {
          // spawn the file chooser
          Context context = getApplicationContext();
          Intent myIntent = new Intent(MainActivity.this, FileChooser.class);
          myIntent.putExtra(FileChooser.EXTRA_START_DIR, Preferences.getRomDir(context));
          myIntent.putExtra(FileChooser.EXTRA_EXTENSIONS, Preferences.DEFAULT_ROM_EXTENSIONS);
          myIntent.putExtra(FileChooser.EXTRA_TEMP_DIR, Preferences.getTempDir(context));
          final int result=Preferences.MENU_ROM_SELECT;
          startActivityForResult(myIntent, result);             
     }
     
     
     protected void spawnSettings()
     {
          // spawn the file chooser
          Intent myIntent = new Intent(MainActivity.this, SettingsActivity.class);
          final int result=Preferences.MENU_SETTINGS;
          startActivityForResult(myIntent, result);             
     }     
     
     
     @Override
     protected void onActivityResult(int requestCode,int resultCode,Intent data)
     {         
          Log.d(LOG_TAG, "onActivityResult(" + requestCode + ", " + resultCode + ")");
          
          if (resultCode == RESULT_CANCELED)
          {
               this.finish();
          }
          else if (requestCode == Preferences.MENU_SETTINGS)
          {
               //PreferenceFacade.loadPrefs(this, getApplicationContext());
          }
          else if (requestCode == Preferences.MENU_ROM_SELECT)
          {
               String romFile=data.getStringExtra(FileChooser.PAYLOAD_FILENAME);
               if (romFile != null)
               {                 
            	   Preferences.loadPrefs(this, getApplicationContext());
                    if (Emulator.loadRom(romFile) == 0)
                    {
                         // AutoLoad
                         Preferences.DoAutoLoad(getApplicationContext());
                         
                         spawnEmulatorActivity();
                    }
               }
               else
               {
                    Toast.makeText(getApplicationContext(), "No rom selected!", Toast.LENGTH_SHORT).show();
               }
          }
          
          super.onActivityResult(requestCode, resultCode, data);          
     }                      
}