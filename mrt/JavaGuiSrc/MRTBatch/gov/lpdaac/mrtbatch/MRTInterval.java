/*
 * MRTInterval.java
 *
 * Created on July 2, 2009, 1:56 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package gov.lpdaac.mrtbatch;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.Writer;
import java.util.ArrayList;

/**
 *
 * @author mmerritt
 */
public class MRTInterval {
   
   private String prmDir = "";
   private ArrayList<String> mosaicList = null;
   private String shortName = "";
   private String pDate = "";
   private String outputExt = "";
   private MRTBatchInfo bi = null;
   
   private static String mosaicFile = "_mosaic.prm";
   private static String prmFile = "_resample.prm";
   private static String defaultInputFilename = "TmpMosaic.hdf";
   
   /** Creates a new instance of MRTInterval */
   public MRTInterval(MRTBatchInfo batchInfo) {
      mosaicList = new ArrayList<String>();
      bi = batchInfo;
   }
   
   
   public int createPrmDir() {
      if( bi.isDebug() ) {
         System.out.println("  Creating directory if it does not exist: " + 
                 getPrmDir());
      }
      File p = new File(getPrmDir());
      if( p.exists() && !p.isDirectory() ) {
         System.err.println("ERROR: Need \"" + getPrmDir() + 
                 "\" to be a directory, but it isn't.");
         return 1;
      }
      if( !p.exists() ) {
         if( !p.mkdirs() ) {
            System.err.println("ERROR: Could not create directory: " + 
                    getPrmDir() );
            return 1;
         }
      }
      return 0;
   }
   
   public int createMosaic() {
      String mosaic = getMosaicPrmName();
      File m = new File(mosaic);
      Writer output = null;
      String eol = MRTUtil.getEOL(bi.getScriptType());
      
      if( bi.isDebug() ) 
         System.out.println("* Creating mosaic parameter file: " + mosaic );
      
      try {
         output = new BufferedWriter(new FileWriter(m));
         //FileWriter always assumes default encoding is OK!
         for( int i = 0; i < mosaicList.size(); ++i ) {
            output.write( mosaicList.get(i) + eol);
         }
      } catch (IOException ex) {
         System.err.println( "ERROR: Error writing to "
                 + mosaic + ": " +
                 (ex.getMessage() == null ?
                    (ex.getCause() == null ? "unknown" :
                       ex.getCause()) : ex.getMessage()) );
         return 1;
      } finally {
         if( output != null ) {
            try {
               output.close();
            } catch (IOException ex) {
               System.err.println( "WARNING: Error closing file: " + mosaic );
            }
         }
      }
      
      return 0;
   }
   
   public int createResample(ArrayList<String> prmLines) {
      String prmfile = getResamplePrmName();
      File m = new File(prmfile);
      Writer output = null;
      String inputFileName = getInputFileName();
      String outputFileName = getOutputFileName();
      String eol = MRTUtil.getEOL(bi.getScriptType());

      if( bi.isDebug() ) {
         System.out.println("* Creating resample parameter file: " + prmfile );
         System.out.println("  " + bi.getInputName() + " = " + inputFileName );
         System.out.println("  " + bi.getOutputName() + " = " + outputFileName );
      }
      
      try {
         output = new BufferedWriter(new FileWriter(m));
         //FileWriter always assumes default encoding is OK!
         for( int i = 0; i < prmLines.size(); ++i ) {
            String line = prmLines.get(i);
            String ucline = line.toUpperCase();
            if( ucline.indexOf( bi.getInputName() ) != -1 ) {
               line = line + inputFileName;
            } else if( ucline.indexOf( bi.getOutputName() ) != -1 ) {
               line = line + outputFileName;
            }
            output.write( line + eol);
         }
      } catch (IOException ex) {
         System.err.println( "ERROR: Error writing to "
                 + prmfile + ": " +
                 (ex.getMessage() == null ?
                    (ex.getCause() == null ? "unknown" :
                       ex.getCause()) : ex.getMessage()) );
         return 1;
      } finally {
         if( output != null ) {
            try {
               output.close();
            } catch (IOException ex) {
               System.err.println( "WARNING: Error closing file: " + prmfile );
            }
         }
      }
      
      return 0;
   }
   
   public void addFile(String file) {
      mosaicList.add(file);
   }
   
   public String getPrmDir() {
      return prmDir;
   }
   
   public void setPrmDir(String prmDir) {
      if( !(prmDir.endsWith("\\") || prmDir.endsWith("/")) )
         this.prmDir = prmDir + File.separatorChar;
      else
         this.prmDir = prmDir;
   }
   
   public String getMosaicPrmName() {
      return MRTUtil.addQuotesIfSpaces(
              getPrmDir() + getShortName() + "." + getPDate() + mosaicFile);
   }
   
   public String getResamplePrmName() {
      return MRTUtil.addQuotesIfSpaces(
              getPrmDir() + getShortName() + "." + getPDate() + prmFile);
   }
   
   public String getInputFileName() {
      return MRTUtil.addQuotesIfSpaces(getPrmDir() + defaultInputFilename);
   }
   
   public String getOutputFileName() {
      return MRTUtil.addQuotesIfSpaces(
              getPrmDir() + getShortName() + "." + getPDate() + "." + getOutputExt());
   }
   
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

   public String getOutputExt() {
      return outputExt;
   }

   public void setOutputExt(String outputExt) {
      this.outputExt = outputExt;
   }
}
