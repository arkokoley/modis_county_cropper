/*
 * MRTText.java
 *
 * Created on July 2, 2009, 10:22 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package gov.lpdaac.mrtbatch;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FilenameFilter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.regex.Pattern;

/**
 *
 * @author mmerritt
 */
public class MRTText {
   
   private MRTFileManager mgr = null;
   private MRTBatchInfo bi = null;
   
   Pattern filePattern = null;
   Pattern pDatePattern = null;
   
   private class FileNameParts {
      private String shortName = "";
      private String pDate = "";
      
      public FileNameParts() {};
      
      public String getShortName() {
         return shortName;
      }
      
      public void setShortName(String shortName) {
         this.shortName = shortName;
      }
      
      public String getPDate() {
         return pDate;
      }
      
      public void setPDate(String pDate) {
         this.pDate = pDate;
      }
   }
   
   
   /** Creates a new instance of MRTText */
   public MRTText(MRTBatchInfo bi) {
      mgr = new MRTFileManager();
      this.bi = bi;
      
      filePattern = Pattern.compile("[.]");
      pDatePattern = Pattern.compile("[A|a][0-9]{7}");
   }
   
   public int parseForHDF() {
      if( bi.getTextFileName().length() > 0 )
         return parseTextFile();
      return parseDirectory();
   }
   
   private int parseTextFile() {
      String filename = bi.getTextFileName();
      FileNameParts fileNameParts = new FileNameParts();
      
      if( bi.isDebug() ) {
         System.out.println("* Processing " + filename + "...");
      }
      
      BufferedReader input = null;
      try {
         //use buffering, reading one line at a time
         //FileReader always assumes default encoding is OK!
         input = new BufferedReader( new FileReader(filename) );
         String line = null; //not declared within while loop
         
         /*
          * readLine is a bit quirky :
          * it returns the content of a line MINUS the newline.
          * it returns null only for the END of the stream.
          * it returns an empty String if two newlines appear in a row.
          */
         while (( line = input.readLine()) != null){
            if( line != null ){
               line = line.trim();
               if( line.length() == 0 )
                  continue;
            }
            
            // Remove quotes if present.
            line = MRTUtil.removeQuotes(line);
            // Split the filename from the path.
            String path = "." + (bi.getScriptType() ==
                    MRTBatchInfo.ScriptType.BATCH ? "\\" : "/");
            String file = "";
            int pos, opos;
            
            pos = line.lastIndexOf('\\');
            opos = line.lastIndexOf('/');
            if( opos > pos )
               pos = opos;
            
            if( pos != -1 ) {
               path = line.substring( 0, pos );
               file = line.substring( pos + 1 );
            } else {
               file = line;
            }
            
            if( badFile(file, fileNameParts) ) {
               if( bi.isSkipBad() ) {
                  System.out.println("WARNING: Unexpected file pattern. " + 
                          "Skipping file: " + file);
               } else {
                  System.out.println("ERROR: Unexpected file pattern: " + file);
                  return 1;
               }
            }
            
            mgr.addItem(fileNameParts.getShortName(), fileNameParts.getPDate(),
                    path, file );
         }
      } catch (FileNotFoundException ex) {
         System.err.println( "ERROR: File not found: " + filename );
         return 1;
      } catch (IOException ex){
         System.err.println( "ERROR: Error reading from "
                 + filename + ": " +
                 (ex.getMessage() == null ?
                    (ex.getCause() == null ? "unknown" :
                       ex.getCause()) : ex.getMessage()) );
         return 1;
      } finally {
         try {
            if (input!= null) {
               //flush and close both "input" and its underlying FileReader
               input.close();
            }
         } catch (IOException ex) {
            System.err.println( "WARNING: Error closing file: " + filename );
         }
      }
      
      if( mgr.isEmpty() ) {
         System.err.println( "ERROR: No HDF files were found in file " +
                 "to process: " + filename );
         return 1;
      }
      
      if( bi.isDebug() ) {
         mgr.displayTree(true);
      }
      return 0;
   }
   
   public ArrayList<MRTShortName> getShortNames() {
      return mgr.getShortNames();
   }
   
   private int parseDirectory() {
      String directory = bi.getHdfDirName();
      File dir = new File(directory);
      FileNameParts fileNameParts = new FileNameParts();
      
      FilenameFilter filter = new FilenameFilter() {
         public boolean accept(File dir, String name) {
            String [] fsplit = filePattern.split(name);
            if( fsplit.length < 3 )
               return false;
            if( ! fsplit[fsplit.length - 1].equalsIgnoreCase("hdf") )
               return false;
            return true;
         }
      };
      
      String [] files = dir.list(filter);
      
      for( int i = 0; i < files.length; ++i ) {
         String file = files[i];
         
         if( !badFile(file, fileNameParts) ) {
            mgr.addItem(fileNameParts.getShortName(), fileNameParts.getPDate(),
                    directory, file );
         }
      }
      
      if( mgr.isEmpty() ) {
         System.err.println( "ERROR: No HDF files were found in directory " +
                 "to process: " + directory );
         return 1;
      }
      
      if( bi.isDebug() ) {
         mgr.displayTree(true);
      }
      
      return 0;
   }
 
   private Boolean badFile(String file, FileNameParts parts) {
      String [] fsplit = filePattern.split(file);
      if( fsplit.length < 3 ) {
         return true;
      } else {
         parts.setShortName(fsplit[0]);
         parts.setPDate(fsplit[1]);
         if( !pDatePattern.matcher(parts.getPDate()).matches() )
            return true;
         if( ! fsplit[fsplit.length - 1].equalsIgnoreCase("hdf") )
            return true;
      }
      return false;
   }
   
}
