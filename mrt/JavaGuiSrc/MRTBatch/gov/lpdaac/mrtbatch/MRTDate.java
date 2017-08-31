/*
 * MRTDate.java
 *
 * Created on July 1, 2009, 12:28 PM
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
public class MRTDate {
   
   private String date = "";
   private String nDate = "";
   private ArrayList<MRTFiles> files;
   
   /**
    * Creates a new instance of MRTDate
    */
   public MRTDate() {
      files = new ArrayList<MRTFiles>();
   }

   public MRTDate(String date) {
      files = new ArrayList<MRTFiles>();
      setDate(date);
   }

   public MRTDate(String date, String path, String file) {
      files = new ArrayList<MRTFiles>();
      setDate(date);
      addFile(path, file);
   }

   public String getDate() {
      return date;
   }

   public void setDate(String date) {
      this.date = date;
      this.nDate = normalizeDate(date);
   }

   public String getNDate() {
      return nDate;
   }
   
   public MRTFiles findPath( String path ) {
      MRTFiles p = null;
      for( int i = 0; i < files.size(); ++i ) {
         p = files.get(i);
         if( p.getNPath().equals(MRTFiles.normalizePath(path)) ) {
            return p;
         }
      }
      return null;
   }
   
   public String findFile( String path, String file ) {
      MRTFiles p = findPath(path);
      if( p != null )
         return p.findFile(file);
      return "";
   }

   public int addFile( String path, String file ) {
      MRTFiles flist = findPath(path);
      if( flist == null ) {
         files.add(new MRTFiles(path, file));
         return 1;
      } else {
         return flist.addFile(file);
      }
   }
   
   public ArrayList<MRTFiles> getList() {
      return files;
   }
   
   public static String normalizeDate( String date ) {
      return date.toUpperCase();
   }

}
