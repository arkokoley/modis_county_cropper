/*
 * MRTShortName.java
 *
 * Created on July 1, 2009, 1:28 PM
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
public class MRTShortName {
   
   private String shortName = "";
   private String nShortName = "";
   private ArrayList<MRTDate> dates;
   
   /** Creates a new instance of MRTShortName */
   public MRTShortName() {
      dates = new ArrayList<MRTDate>();
   }
   
   public MRTShortName(String shortname) {
      dates = new ArrayList<MRTDate>();
      setShortName(shortname);
   }

   public MRTShortName(String shortname, String date, String path, String file) {
      dates = new ArrayList<MRTDate>();
      setShortName(shortname);
      addDatedFile(date, path, file);
   }

   public String getShortName() {
      return shortName;
   }

   public void setShortName(String shortname) {
      this.shortName = shortname;
      this.nShortName = normalizeShortName(shortname);
   }

   public String getNShortName() {
      return nShortName;
   }
   
   public MRTDate findDate( String date ) {
      MRTDate d = null;
      for( int i = 0; i < dates.size(); ++i ) {
         d = dates.get(i);
         if( d.getNDate().equals(MRTDate.normalizeDate(date)) ) {
            return d;
         }
      }
      return null;
   }

   public int addDatedFile( String date, String path, String file ) {
      MRTDate dlist = findDate(date);
      if( dlist == null ) {
         dates.add(new MRTDate(date, path, file));
         return 1;
      } else {
         return dlist.addFile(path, file);
      }
   }
   
   public ArrayList<MRTDate> getList() {
      return dates;
   }
   
   public static String normalizeShortName( String shortName ) {
      return shortName.toUpperCase();
   }
}
