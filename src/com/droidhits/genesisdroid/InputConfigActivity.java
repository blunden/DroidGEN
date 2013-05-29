
package com.droidhits.genesisdroid;

import java.io.IOException;
import java.io.InputStream;

import com.droidhits.genesisdroid.R;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.AlertDialog.Builder;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.RectF;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.util.Log;
import android.view.GestureDetector;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Toast;

public class InputConfigActivity extends Activity implements GestureDetector.OnGestureListener
{
     private static final String LOG_TAG = "InputConfigActivity";
     
     private RectF _gameRect;
     private RectF _rects[];
     private RectF _analogRects[];
     private Bitmap _bitmaps[];  
     private Bitmap _analogBitmaps[];
     
     private Paint _paintGameRect;
     private Paint _paint;
     
     private RectF _trackRect;
     private RectF _resizingRect;
     private boolean _resizeReady = false;
     
     private GestureDetector _gestures;
     
     private float _screenWidth;
     private float _screenHeight;
     
     private int _buttonCount = 0;
     private int _analogCount = 0;
     
     @Override
     public void onCreate(Bundle icicle)
     {
          super.onCreate(icicle);
          
          Context context = getApplicationContext();

          // number of buttons created
          _buttonCount = InputPreferences.getNumButtons(context);
          _analogCount = InputPreferences.getNumAnalogs(context);
          
          Log.d(LOG_TAG, "onCreate() Parsing NumButtons: " + _buttonCount + ", NumAnalogs: " + _analogCount);
          
          // handle to prefs
          SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(context);
          
          // init  
          _analogRects = 
          _rects = new RectF[_buttonCount];
          _bitmaps = new Bitmap[_buttonCount];
          _analogRects = new RectF[_analogCount];
          _analogBitmaps = new Bitmap[_analogCount];          
                    
    	  _screenWidth = Preferences.getRealScreenWidth(this, context);
    	  _screenHeight = Preferences.getRealScreenHeight(this, context);
                             
          // create the game window rect
          float ratio = 4.0f / 3.0f;
          if (!prefs.getBoolean(Preferences.PREF_MAINTAIN_ASPECT_RATIO, true))
          {
               ratio = _screenWidth / _screenHeight;
          }
                    
          float rectHeight = _screenHeight;
          float rectWidth = rectHeight * ratio;
          float rectX = (_screenWidth / 2.0f) - (rectWidth / 2.0f);
          float rectY = 0;
                    
          _gameRect = new RectF(rectX, rectY, rectX + rectWidth, rectY + rectHeight);
          
          // load the resources
          AssetManager mngr = getAssets();
                                    
          // analogs
          for (int i = 0; i < _analogCount; i++)
          {
               float dPadX = InputPreferences.getAnalogX(context, i);
               float dPadY = InputPreferences.getAnalogY(context, i);
               float dPadWidth = InputPreferences.getAnalogWidth(context, i);
               float dPadHeight = InputPreferences.getAnalogHeight(context, i);
               String textureFile = InputPreferences.getAnalogTexture(context, i);

               try
               {                   
                    if (textureFile != null)
                    {
                         InputStream is = mngr.open(textureFile);
                         _analogBitmaps[i] = BitmapFactory.decodeStream(is);                    
                         is.close();
                    }
               }
               catch (IOException e)
               {
                    e.printStackTrace();
               }                 
               _analogRects[i] = new RectF(dPadX, dPadY, dPadX + dPadWidth, dPadY + dPadHeight);
          }
          
          // buttons
          for (int i = 0; i < _buttonCount; i++)
          {
               float x = InputPreferences.getButtonX(context, i);
               float y = InputPreferences.getButtonY(context, i);
               float w = InputPreferences.getButtonWidth(context, i);
               float h = InputPreferences.getButtonHeight(context, i);
               //int code = InputPreferences.getButtonCode(context, i);
               int map = InputPreferences.getButtonMap(context, i);
               String textureFile = InputPreferences.getButtonTexture(context, i);

               try
               {        
                    if (textureFile != null)
                    {                    
                         InputStream is = mngr.open(textureFile);
                         _bitmaps[map] = BitmapFactory.decodeStream(is);                    
                         is.close();
                    }
               }
               catch (IOException e)
               {
                    e.printStackTrace();
               }  
               _rects[map] = new RectF(x, y, x + w, y + h);
          }          
                    
          // paint
          _paint = new Paint();
          _paint.setColor(Color.GREEN);
          _paint.setStyle(Paint.Style.STROKE);
          _paint.setStrokeWidth(3);          
          
          _paintGameRect = new Paint();
          _paintGameRect.setColor(Color.DKGRAY);
          _paintGameRect.setStyle(Paint.Style.FILL);            
          
          // set renderer
          View _view = new InputSetupView(this);
          setContentView(_view);  
          
          _gestures = new GestureDetector(this);
          
          Builder dialog = new AlertDialog.Builder(this)
                              .setTitle(getString(R.string.app_name) + " Input Config")
                              .setMessage("- Drag controls around to move them\n"+
                                          "- Touch and hold to enter resize mode\n" +
                                          "- In resize mode drag edges to resize\n" +
                                          "- Press 'Back' or select another control to exit resize mode\n" +
                                          "- Saves when you leave this screen")          
                              .setNeutralButton("Ok", null);         
          dialog.show();
     }
     
     
     @Override
     public void onPause()
     {
          super.onPause();
          
          saveInput();
          
          finish();
     }
     
     
     @Override
     public void onStop()
     {
          super.onStop();
          
          saveInput();
          
          finish();
     }
     
     
     @Override
     public boolean onKeyDown(int keyCode, KeyEvent event)
     {
         Log.d(LOG_TAG, "onKeyDown(" + keyCode + ")");
         if (keyCode == KeyEvent.KEYCODE_BACK)
         {
              // check for resizing abort, quick exit too
              if (_resizingRect != null)
              {
                   _resizingRect = null;
                   return true;
              }
              
              // exit, save positions, pass good result to parent
              Intent intent = new Intent();
              setResult(RESULT_OK, intent);
              
              // save input
              saveInput();
              
              finish();
              
              return true;
         }
         
         return super.onKeyDown(keyCode, event);
     }    
     
     
     @Override
     public boolean onTouchEvent(final MotionEvent event)
     {
          int actionMasked = event.getActionMasked();
          if (actionMasked == MotionEvent.ACTION_POINTER_DOWN
                         || actionMasked == MotionEvent.ACTION_DOWN)
          {               
               RectF rect = handleTouchDown(event);
               if (rect == null || rect != _resizingRect)
               {
                    _trackRect = rect;
                    _resizingRect = null;
               }
          }
          else if (actionMasked == MotionEvent.ACTION_POINTER_UP
                         || actionMasked == MotionEvent.ACTION_UP
                         || actionMasked == MotionEvent.ACTION_CANCEL)
          {
               _trackRect = null;
               if (_resizingRect != null)
               {
                    _resizeReady = true;
               }
          }
          else if (actionMasked == MotionEvent.ACTION_MOVE)
          {
               handleOnMove(event);
          }  
                              
          return _gestures.onTouchEvent(event);
     }     
     
     
     private void saveInput()
     {
          Context context = getApplicationContext();
          for (int i = 0; i < _buttonCount; i++)
          {
               InputPreferences.setButton(context, 
                                        _rects[i].left, _rects[i].top, 
                                        _rects[i].right - _rects[i].left, _rects[i].bottom - _rects[i].top, 
                                        i);
          }         

          for (int i = 0; i < _analogCount; i++)
          {
               InputPreferences.setAnalog(context, 
                         _analogRects[i].left, _analogRects[i].top, 
                         _analogRects[i].right - _analogRects[i].left, _analogRects[i].bottom - _analogRects[i].top, 
                         i);
          }
          
          // turn off auto default!
          SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(context);
          Editor edit = prefs.edit();                             
          edit.putBoolean(Preferences.PREF_USE_DEFAULT_INPUT, false);
          edit.commit();          
     }
     

     public RectF handleLongClick(MotionEvent event)
     {
          float x = event.getX();
          float y = event.getY();
          
          for (int i = 0; i < _buttonCount; i++)
          {
               RectF rect = _rects[i];
               if (x >= rect.left && x <= rect.right &&
                   y >= rect.top && y <= rect.bottom)
               {
                    Toast.makeText(this, "Press back or touch a different object to leave resize mode", Toast.LENGTH_LONG).show();
                    return rect;
               }
          }   
          
          for (int i = 0; i < _analogCount; i++)
          {
               RectF rect = _analogRects[i];
               if (x >= rect.left && x <= rect.right &&
                   y >= rect.top && y <= rect.bottom)
               {
                    Toast.makeText(this, "Press back or touch a different object to leave resize mode", Toast.LENGTH_LONG).show();
                    return rect;
               }
          }          
          
          return null;
     }
     
        
     public void handleOnMove(final MotionEvent event)
     {       
          // only do 1 or the other
          if (_resizingRect != null)
          {
               if (_resizeReady)
               {
                    // calculate aspect
                    float width = _resizingRect.right - _resizingRect.left;
                    float height = _resizingRect.bottom - _resizingRect.top;                    
                    float aspect = width / height;                   
                    float halfWidth = width / 2;
                     
                    // @TODO - fix this... must be square dpad
                    float xDist = (event.getX() - _resizingRect.left) / (halfWidth);
                    float yDist = (event.getY() - _resizingRect.top) / halfWidth;
                    if (xDist > 0.75)
                    {
                         float xDelta = event.getX() - _resizingRect.right;
                         float yDelta = xDelta * aspect;
                         
                         // cap on width
                         if ((event.getX() - _resizingRect.left) > (_screenWidth * 0.05))
                         {
                              _resizingRect.right = event.getX();
                                                       
                              if (yDist > 0.5)
                              {
                                   _resizingRect.bottom += yDelta;
                              }
                              else if (yDist < 0.5)
                              {
                                   _resizingRect.top -= yDelta;
                              }
                         }
                    }
                    
                    if (xDist < 0.25)
                    {
                         float xDelta = _resizingRect.left - event.getX();
                         float yDelta = xDelta * aspect;
                         
                         // cap on width
                         if ((_resizingRect.right - event.getX()) > (_screenWidth * 0.05))
                         {                         
                              _resizingRect.left = event.getX();
                              
                              if (yDist > 0.5)
                              {
                                   _resizingRect.bottom += yDelta;
                              }
                              else if (yDist < 0.5)
                              {
                                   _resizingRect.top -= yDelta;
                              }
                         }
                    }                    
               }
          }
          else if (_trackRect != null)
          {
               float width = _trackRect.right - _trackRect.left;
               float height = _trackRect.bottom - _trackRect.top;               
               float x = event.getX() - width / 2;
               float y = event.getY() - height / 2;
               
               _trackRect.left = x;
               _trackRect.top = y;
               _trackRect.right = x + width;
               _trackRect.bottom = y + height;
          }          
     }
     
     
     public RectF handleTouchDown(final MotionEvent event)
     {
          float x = event.getX();
          float y = event.getY();          
          
          for (int i = 0; i < _buttonCount; i++)
          {
               RectF rect = _rects[i];
               if (x >= rect.left && x <= rect.right &&
                   y >= rect.top && y <= rect.bottom)
               {
                    return rect;
               }
          }
          
          for (int i = 0; i < _analogCount; i++)
          {
               RectF rect = _analogRects[i];
               if (x >= rect.left && x <= rect.right &&
                   y >= rect.top && y <= rect.bottom)
               {
                    return rect;
               }
          }          
          
          return null;
     }
               
     
     public boolean onDown(MotionEvent e)
     {
          // TODO Auto-generated method stub
          return false;
     }


     public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX,
               float velocityY)
     {
          // TODO Auto-generated method stub
          return false;
     }


     public void onLongPress(MotionEvent e)
     {
          Log.d(LOG_TAG, "LongPress(" + e + ")");
          
          _resizingRect = handleLongClick(e);
          _resizeReady = false;
     }


     public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX,
               float distanceY)
     {          
          return false;
     }


     public void onShowPress(MotionEvent e)
     {
          // TODO Auto-generated method stub
          
     }


     public boolean onSingleTapUp(MotionEvent e)
     {
          // TODO Auto-generated method stub
          return false;
     }     
     
     
     
     class InputSetupView extends SurfaceView implements SurfaceHolder.Callback
     {
          private TutorialThread _thread;
          
          public InputSetupView(Context context)
          {
               super(context);
               getHolder().addCallback(this);
               _thread = new TutorialThread(getHolder(), this);               
          }
          
          
          //@Override
          public void surfaceChanged(SurfaceHolder holder, int format, int width, int height)
          {
              // TODO Auto-generated method stub
   
          }
   
          //@Override
          public void surfaceCreated(SurfaceHolder holder)
          {
              _thread.setRunning(true);
              _thread.start();
          }
   
          //@Override
          public void surfaceDestroyed(SurfaceHolder holder)
          {
              // simply copied from sample application LunarLander:
              // we have to tell thread to shut down & wait for it to finish, or else
              // it might touch the Surface after we return and explode
              boolean retry = true;
              _thread.setRunning(false);
              while (retry) {
                  try {
                      _thread.join();
                      retry = false;
                  } catch (InterruptedException e) {
                      // we will try it again and again...
                  }
              }
          }
          
          
          @Override
          public void onDraw(Canvas canvas)
          {             
               // render them
               canvas.drawColor(Color.BLACK);
               
               canvas.drawRect(_gameRect, _paintGameRect);
               
               for (int i = 0; i < _buttonCount; i++)
               {
                    if (_bitmaps[i] != null)
                    {
                         canvas.drawBitmap(_bitmaps[i], null, _rects[i], null);
                    }
                    
                    if (_resizingRect == _rects[i])
                    {
                         canvas.drawRect(_rects[i], _paint);                         
                    }
               }          
               
               for (int i = 0; i < _analogCount; i++)
               {
                    if (_analogBitmaps[i] != null)
                    {
                         canvas.drawBitmap(_analogBitmaps[i], null, _analogRects[i], null);
                    }
                    
                    if (_resizingRect == _analogRects[i])
                    {
                         canvas.drawRect(_analogRects[i], _paint);                         
                    }
               }                
          }
          
     }  
     
     
     class TutorialThread extends Thread {
          private SurfaceHolder _surfaceHolder;
          private InputSetupView _panel;
          private boolean _run = false;
   
          public TutorialThread(SurfaceHolder surfaceHolder, InputSetupView panel) {
              _surfaceHolder = surfaceHolder;
              _panel = panel;
          }
   
          public void setRunning(boolean run) {
              _run = run;
          }
   
          @Override
          public void run() {
              Canvas c;
              while (_run) {
                  c = null;
                  try {
                      c = _surfaceHolder.lockCanvas(null);
                      synchronized (_surfaceHolder) {
                          _panel.onDraw(c);
                      }
                  } finally {
                      // do this in a finally so that if an exception is thrown
                      // during the above, we don't leave the Surface in an
                      // inconsistent state
                      if (c != null) {
                          _surfaceHolder.unlockCanvasAndPost(c);
                      }
                  }
              }
          }
      }     

}
