

package com.droidhits.genesisdroid;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.util.Log;


/**
 * AudioTrack manager for the emulator.
 * The emulation loop directly writes to this one.
 * No threading, why? Java scheduler fails to live up to expectations.
 */
public class AudioPlayer
{
     private static String     LOG_TAG = "AudioPlayer";
     
     private static AudioTrack _track;
     private static int        _minSamples;
     private static float      _volume = AudioTrack.getMaxVolume();
     
     private static int _rate;
     private static int _bits;
     private static int _channels;
          
     
     private AudioPlayer()
     {               
                       
     }
          
          
     public static void pause()
     {
          if (_track != null && _track.getPlayState() != AudioTrack.PLAYSTATE_PAUSED)
          {
               _track.pause();
          }
     }


     public static void resume()
     {
          if (_track != null && _track.getPlayState() != AudioTrack.PLAYSTATE_PLAYING)
          {          
               _track.play();
          }
     }
     
     
     public int getMinSamples()
     {
          return _minSamples;
     }
     
     
     public static int play(short[] data, int size)
     {
          int retVal = 0;
          //Log.d(LOG_TAG, "PLAYING: " + size);
          if (_track == null)
          {
               return retVal;
          }
          
          retVal = _track.write(data, 0, size);
          
          return retVal;
     }
          
                                       
     void setVolume(int vol)
     {
          final float min = AudioTrack.getMinVolume();
          final float max = AudioTrack.getMaxVolume();
          _volume = min + (max - min) * vol / 100;

          if (_track != null)
          {
               _track.setStereoVolume(_volume, _volume);
          }
     }               


     public static void destroy()
     {    
          Log.d(LOG_TAG, "AudioPlayer.destroy()");

          if (_track != null)
          {
               _track.pause();
               _track.release();
               _track = null;
          }
     }


     public static boolean create(int rate, int bits, int channels)
     {                 
    	 Log.d(LOG_TAG, "create(" + rate + ", " + bits + ", " + channels + ")");
    	 
          // start the audio thread, @HACK - no longer a thread!
          _track = null;          
          _rate = rate;
          _bits = bits;
          _channels = channels;
          
          // generate format
          int format = (_bits == 16
                    ? AudioFormat.ENCODING_PCM_16BIT
                    : AudioFormat.ENCODING_PCM_8BIT);

          // determine channel config
          int channelConfig = (_channels == 2
                    ? AudioFormat.CHANNEL_CONFIGURATION_STEREO
                    : AudioFormat.CHANNEL_CONFIGURATION_MONO);

          int bufferSize = AudioTrack.getMinBufferSize(_rate, channelConfig,
                    format);
          
          _minSamples = bufferSize;
          
          // HACK - double buffer size for anything less than honeycomb
          // 		basically this means any tablet will run perfect
          //		phones get the hack applied
          int bufferScalar = 1;
          if (android.os.Build.VERSION.SDK_INT < 11)
          {
        	  Log.d(LOG_TAG, "AudioPlayer.Create() - Buffer * 2 hack applied!");
        	  bufferScalar = 2;
          }
          
          try
          {
               // @HACK - *2 to mitigate underruns! - not needed it appears and causes lag
               _track = new AudioTrack(
                              AudioManager.STREAM_MUSIC,
                              _rate,
                              channelConfig,
                              format,
                              bufferSize * bufferScalar,
                              AudioTrack.MODE_STREAM);

               if (_track.getState() == AudioTrack.STATE_UNINITIALIZED)
               {
                    _track = null;
               }

          }
          catch (IllegalArgumentException e)
          {
               _track = null;
               return false;
          }

          // set max volume
          _track.setStereoVolume(_volume, _volume);      
                   
          return true;
     }
    
              
     public static int getMaxBufferSize()
     {
          return _minSamples;
     }
}