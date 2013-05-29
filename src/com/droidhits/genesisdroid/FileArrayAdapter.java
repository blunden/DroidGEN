package com.droidhits.genesisdroid;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Set;

import com.tizmoplay.androgens.R;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.SectionIndexer;
import android.widget.TextView;

/**
 * Based on tutorial from:
 * @author http://www.dreamincode.net/forums/topic/190013-creating-simple-file-chooser/
 *
 */
public class FileArrayAdapter extends ArrayAdapter<Option> implements SectionIndexer 
{
     HashMap<String, Integer> alphaIndexer; 
     String[] sections;
     
     private Context c;
     private int id;
     private List<Option>items;
     
     public FileArrayAdapter(Context context, int textViewResourceId,
               List<Option> objects) {
          super(context, textViewResourceId, objects);
          c = context;
          id = textViewResourceId;
          items = objects;    
          
          initAlphaIndexer();
     }
     
     private void initAlphaIndexer()
     {
          alphaIndexer = new HashMap<String, Integer>();
          int size = items.size();

          for (int x = 0; x < size; x++) {
               Option o = items.get(x);

               // get the first letter of the store
              String ch =  o.getName().substring(0, 1);
              
              // convert to uppercase otherwise lowercase a -z will be sorted after upper A-Z
              ch = ch.toUpperCase();

              // HashMap will prevent duplicates
              if (!alphaIndexer.containsKey(ch))
              {
                   alphaIndexer.put(ch, x);
              }
          }

          Set<String> sectionLetters = alphaIndexer.keySet();

          // create a list from the set to sort
          ArrayList<String> sectionList = new ArrayList<String>(sectionLetters); 

          Collections.sort(sectionList);

          sections = new String[sectionList.size()];

          sectionList.toArray(sections);          
     }

     public Option getItem(int i)
     {
          return items.get(i);
     }
     @Override
      public View getView(int position, View convertView, ViewGroup parent) {
              View v = convertView;
              if (v == null) {
                  LayoutInflater vi = (LayoutInflater)c.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
                  v = vi.inflate(id, null);
              }
              final Option o = items.get(position);
              if (o != null) {                   
                      TextView t1 = (TextView) v.findViewById(R.id.TextView01);
                      TextView t2 = (TextView) v.findViewById(R.id.TextView02);

                      if(t1!=null)
                      {
                        t1.setText(o.getName());
                      }
                      if(t2!=null)
                      {
                        t2.setText(o.getData());
                      }
                      
              }
              return v;
      }

     public int getPositionForSection(int section)
     {
    	 // TODO - find out why this happens but sometimes it asks for an invalid section
    	 if (section >= sections.length)
    	 {
    		 return 0;
    	 }
         return alphaIndexer.get(sections[section]);
     }

     public int getSectionForPosition(int position)
     {
          /*int prevIndex = 0;
          for(int i = 0; i < sections.length; i++)
          {
              if(getPositionForSection(i) > position && prevIndex <= position)
              {
                  prevIndex = i;
                  break;
              }
              prevIndex = i;
          }
          return prevIndex;*/
          /*String ch = items.get(position).getName().substring(0, 1);
          for (int i = 0; i < sections.length; i++)
          {
               if (sections[i].equals(ch))
               {
                    return i;
               }
          }
          return -1; */
          return 1;
     }

     public Object[] getSections()
     {          
          return sections;
     }
     
     
}

