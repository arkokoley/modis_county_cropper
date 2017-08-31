/*
 * MRTFileManager.java
 *
 * Created on July 1, 2009, 3:36 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package gov.lpdaac.mrtbatch;

import java.util.ArrayList;

/**
 *
 * @author mmerritt
 */
public class MRTFileManager {
   
   private ArrayList<MRTShortName> names;
   
   /** Creates a new instance of MRTManager */
   public MRTFileManager() {
      names = new ArrayList<MRTShortName>();
   }
   
   public MRTFileManager(String shortname, String date, String path, String file) {
      names = new ArrayList<MRTShortName>();
   }
   
   public MRTShortName findShortName( String shortname ) {
      MRTShortName s = null;
      for( int i = 0; i < names.size(); ++i ) {
         s = names.get(i);
         if( s.getNShortName().equals(MRTShortName.normalizeShortName(shortname)) ) {
            return s;
         }
      }
      return null;
   }
   
   public int addItem(String shortname, String date, String path, String file) {
      MRTShortName name = findShortName( shortname );
      if( name == null ) {
         names.add(new MRTShortName(shortname,date,path,file));
         return 1;
      } else {
         return name.addDatedFile(date, path, file);
      }
   }
   
   public ArrayList<MRTShortName> getShortNames() {
      return names;
   }
   
   public void displayTree(Boolean normalized) {
      for( MRTShortName s : names ) {
         System.out.println( "ShortName: " + (normalized ? s.getNShortName() : s.getShortName()) );
         ArrayList<MRTDate> dates = s.getList();
         for( MRTDate d : dates ) {
            System.out.println( "  Date: " + (normalized ? d.getNDate() : d.getDate()));
            ArrayList<MRTFiles> paths = d.getList();
            for( MRTFiles p : paths ) {
               System.out.println( "    Path: " + (normalized ? p.getNPath() : p.getPath()));
               ArrayList<String> files = p.getList();
               for( String f : files ) {
                  System.out.println( "      File: " + f.toString() );
               }
            }
         }
      }
   }
   
   public Boolean isEmpty() {
      return names.isEmpty();
   }
   
}