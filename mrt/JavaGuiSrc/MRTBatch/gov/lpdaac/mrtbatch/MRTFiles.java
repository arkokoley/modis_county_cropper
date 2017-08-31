/*
 * MRTFiles.java
 *
 * Created on July 1, 2009, 12:07 PM
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
public class MRTFiles {
   
   private String path = "";
   private String npath = "";
   private ArrayList<String> files;
   
   /**
    * Creates a new instance of MRTFiles
    */
   public MRTFiles() {
      files = new ArrayList<String>();
   }
   
   public MRTFiles(String path) {
      files = new ArrayList<String>();
      setPath(path);
   }

   public MRTFiles(String path, String file) {
      files = new ArrayList<String>();
      setPath(path);
      addFile(file);
   }
      
   public String getPath() {
      return path;
   }

   public void setPath(String path) {
      this.path = path;
      this.npath = normalizePath(path);
   }

   public String getNPath() {
      return npath;
   }

   public String findFile(String file) {
      String f = "";
      for( int i = 0; i < files.size(); ++i ) {
         f = files.get(i);
         if( f.toUpperCase().equals(file.toUpperCase()) ) {
            return f;
         }
      }
      return "";
   }
   
   public int getNumberOfFiles() {
      return files.size();
   }
   
   public int addFile( String file ) {
      String f = findFile(file);
      if( f.length() == 0 ) {
         files.add(file);
         return 1;
      }
      return 0;
   }
   
   public ArrayList<String> getList() {
      return files;
   }

   static public String normalizePath(String path) {
      return MRTUtil.createOSPath(path.toUpperCase());
   }

}
