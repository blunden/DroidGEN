

package com.droidhits.genesisdroid;

import com.droidhits.genesisdroid.R;

import android.app.ListActivity;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.graphics.Color;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.util.Log;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.view.inputmethod.InputMethodManager;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.TextView;


class KeyBinding
{
     String name;
     int key;
     boolean adjusting;
     
     public String toString()
     {
          return "Key: " + name + " | Code: " + key;
     }
}


class KeyBindingAdapter extends ArrayAdapter<KeyBinding>
{

	// used to keep selected position in ListView
	private int selectedPos = -1;	// init value for not-selected

	public KeyBindingAdapter(Context context, int textViewResourceId,
							KeyBinding[] objects)
	{
		super(context, textViewResourceId, objects);
	}

	public void setSelectedPosition(int pos)
	{
		selectedPos = pos;
		// inform the view of this change
		notifyDataSetChanged();
	}

	public int getSelectedPosition(){
		return selectedPos;
	}

	@Override
	public View getView(int position, View convertView, ViewGroup parent)
	{
	    View v = convertView;

	    // only inflate the view if it's null
	    if (v == null) {
	        LayoutInflater vi = (LayoutInflater)this.getContext().getSystemService(Context.LAYOUT_INFLATER_SERVICE);
	        v = vi.inflate(R.layout.custom_key_view, null);
	    }

	    // get text view
        TextView label = (TextView)v.findViewById(R.id.txtExample);

        // change the row color based on selected state
        String extra = "";
        if(selectedPos == position)
        {
        	label.setBackgroundColor(this.getContext().getResources().getColor(R.color.orange));
        	v.setBackgroundColor(Color.RED);
        	extra = " (Press any button to set new key) ";
        }
        else
        {
        	label.setBackgroundColor(this.getContext().getResources().getColor(R.color.black));
        	v.setBackgroundColor(Color.BLACK);
        }

        label.setText(this.getItem(position).toString() + extra);
        
       
        return(v);
	}
}


public class KeyboardConfigActivity extends ListActivity
{
     private static final String LOG_TAG = "KeyboardConfigActivity";

     private int _modPos = -1;
     private int _modKeyCode = -1;
     
     private ListView _view = null;
     private KeyBindingAdapter _adapter;
     

     public void onCreate(Bundle state)
     {
          super.onCreate(state);

          // reset input, first load bug...
          SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(getApplicationContext());
          if (prefs.getBoolean(Preferences.PREF_USE_DEFAULT_INPUT, true))
          {
        	  EmulatorButtons.resetInput(this, getApplicationContext());
              
              Editor edit = prefs.edit();               
              edit.putBoolean(Preferences.PREF_USE_DEFAULT_INPUT, false);
              edit.commit();                       	  
          }
          
          reloadButtons();

          _view = getListView();

          _view.setOnItemClickListener(new OnItemClickListener()
          {
               public void onItemClick(AdapterView<?> parent, View view,
                         int position, long id)
               {
            	   Log.d(LOG_TAG, "onItemClick(" + parent + ", " + view + ", " + position + ", " + id + ")");
            	   
                    _modPos = position; 
                                        
                    int pos = parent.getPositionForView(view);
                    
                    _adapter.setSelectedPosition(pos);
               }
          });
                    
          InputMethodManager mgr = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
          mgr.showInputMethodPicker();          
     }
     
     
     @Override
     public void onDestroy()
     {
          Log.d(LOG_TAG, "onDestroy()");
          
          super.onDestroy();                    
     }
     
     
     public void reloadButtons()
     {
          int numButtons = EmulatorButtons.BUTTON_INDEX_COUNT.ordinal();
          KeyBinding names[] = new KeyBinding[numButtons];
          for (EmulatorButtons button : EmulatorButtons.values())
          {
               if (button.ordinal() != numButtons)
               {
                    int keyCode = InputPreferences.getButtonCode(getApplicationContext(), button.ordinal());
                    
                    names[button.ordinal()] = new KeyBinding();
                    names[button.ordinal()].name = button.name();
                    names[button.ordinal()].key = keyCode;
                    names[button.ordinal()].adjusting = false;
               }
          }

          /*Toast.makeText(getApplicationContext(),
                    "States Array lentgh is : " + names.length,
                    Toast.LENGTH_LONG).show();*/
          _adapter = new KeyBindingAdapter(this,
                    						R.layout.custom_key_view,
                    						names);
          setListAdapter(_adapter);          
     }


     @Override
     public boolean dispatchKeyEvent(KeyEvent event)
     {
    	 Log.d(LOG_TAG, "dispatchKeyEvent(" + event + ")");
    	 
         if(event.getKeyCode() == KeyEvent.KEYCODE_VOLUME_UP || 
        		 event.getKeyCode() == KeyEvent.KEYCODE_VOLUME_DOWN ||
        		 event.getKeyCode() == KeyEvent.KEYCODE_MENU)
	      	{
	      		return super.dispatchKeyEvent(event);
	      	}    	 
    	 
      	if (_modPos >= 0)
      	{
              Log.d(LOG_TAG, "NewButton: " + event.getKeyCode());
              _modKeyCode = event.getKeyCode();
              
              InputPreferences.setButton(getApplicationContext(), _modKeyCode, _modPos);
           
           reloadButtons();	    
           
           _modPos = -1;
           _modKeyCode = -1;
           
           _adapter.setSelectedPosition(-1);
           //_adjustingView.setBackgroundColor(color.black);
           
           return true;
      	}          
   
       return super.dispatchKeyEvent(event);    	 
     }
     
     /*@Override
     public boolean onKeyDown(int keyCode, KeyEvent event)
     {
          Log.d(LOG_TAG, "onKeyDown(" + event + ")");

          if(keyCode == KeyEvent.KEYCODE_VOLUME_UP || 
                  keyCode == KeyEvent.KEYCODE_VOLUME_DOWN ||
                  keyCode == KeyEvent.KEYCODE_MENU)
	      	{
	      		return false;
	      	}
	    
	      	if (_modPos >= 0)
	      	{
	              Log.d(LOG_TAG, "NewButton: " + keyCode);
	              _modKeyCode = keyCode;
	              
	              InputPreferences.setButton(getApplicationContext(), _modKeyCode, _modPos);
                  
                  reloadButtons();	    
                  
                  _modPos = -1;
                  _modKeyCode = -1;
                  
                  _adapter.setSelectedPosition(-1);
                  //_adjustingView.setBackgroundColor(color.black);
                  
                  return true;
	      	}          
          
          return super.onKeyDown(keyCode, event);
     }*/


     @Override
     public boolean onCreateOptionsMenu(Menu myMenu)
     {
          MenuInflater inflater = getMenuInflater();
          inflater.inflate(R.menu.keyboard_config_menu, myMenu);

          return true;
     }


     @Override
     public boolean onOptionsItemSelected(MenuItem item)
     {
          switch (item.getItemId())
          {
               /*case R.id.menuCustomKeysShowKeyboard:
                    InputMethodManager inputMgr = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
                    inputMgr.toggleSoftInput(InputMethodManager.SHOW_FORCED,
                              InputMethodManager.HIDE_IMPLICIT_ONLY);
                    return true;*/
               default:
                    return super.onOptionsItemSelected(item);
          }
     }
}
