/*
 * MRTManager.java
 *
 * Created on July 1, 2009, 3:36 PM
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
public class MRTManager {
   
   private MRTText txt = null;
   private MRTPrm prm = null;
   private ArrayList<MRTInterval> intervals = null;
   private ArrayList<String> batchLines = null;
   private MRTBatchInfo bi = null;

   
   /** Creates a new instance of MRTManager */
   public MRTManager(MRTBatchInfo batchInfo) {
      bi = batchInfo;
      txt = new MRTText(bi);
      prm = new MRTPrm(bi);
      intervals = new ArrayList<MRTInterval>();
      batchLines = new ArrayList<String>();
   }
   
   public int parseForHdf() {
      return txt.parseForHDF();
   }
   
   public int parsePrmFile() {
      return prm.parsePrmFile();
   }
   
   public int createPrmFiles() {
      ArrayList<MRTShortName> shortnames = txt.getShortNames();
      
      // For each shortname and date, create an interval file.
      // This will represent one run of mrtmosaic and resample.
      
      // Create mrtmosaic parameter file.  This file consists of
      // a group of hdf files per line that will be mosaiced
      // together, by interval.
      
      for( int i = 0; i < shortnames.size(); i++ ) {
         MRTShortName shortname = shortnames.get(i);
         ArrayList<MRTDate> dates = shortname.getList();
         for( int j = 0; j < dates.size(); ++j ) {
            MRTDate date = dates.get(j);
            // We have an interval.  The shortname and date.
            // This porgram allows files to be across mutiple
            // paths, but, only oone output directory will be created
            // for the group.  So use the first directory found.
            
            MRTInterval ival = new MRTInterval(bi);
            String outputDir = bi.getOutputDirectory();
            ival.setPrmDir(outputDir);
            // Create mosaic file name.
            ival.setShortName(shortname.getNShortName());
            ival.setPDate(date.getNDate());
            ival.setOutputExt(prm.getOutputExtension());
            ArrayList<MRTFiles> paths = date.getList();
            for( int k = 0; k < paths.size(); ++k ) {
               MRTFiles path = paths.get(i);
               if( outputDir.length() == 0 ) {
                  outputDir = path.getPath();
                  if( outputDir.length() == 0 )
                     outputDir = new File(
                             System.getProperty("user.dir")).getAbsolutePath();
                  if( !(outputDir.endsWith("\\") || outputDir.endsWith("/")) )
                     outputDir += File.separatorChar;
                  outputDir += "prm";
                  ival.setPrmDir(outputDir);
               }
               // Create and add to the list of files that will create
               // the interval.
               ArrayList<String> files = path.getList();
               for( int l = 0; l < files.size(); ++l ) {
                  ival.addFile(
                          MRTUtil.createMosaicFile(path.getPath(), files.get(l),
                          bi.getScriptType()) );
               }
            }
            intervals.add(ival);
         }
      }
      
      // For each interval...
      batchLines.clear();
      if( bi.isDebug() ) {
         System.out.println("* Intervals created: " + intervals.size());
      }
      for( int i = 0; i < intervals.size(); ++i ) {
         MRTInterval iv = intervals.get(i);
         if( iv.createPrmDir() != 0 )
            return 1;
         if( iv.createMosaic() != 0 )
            return 1;
         if( iv.createResample(prm.getPrmLines()) != 0 )
            return 1;
         String spectralSubset = prm.getSpectralSubset();
         String spectralArg = "";
         if( spectralSubset.length() != 0 ) {
            spectralArg = "-s \"" + spectralSubset + "\" ";
         }
         batchLines.add("mrtmosaic " + spectralArg + "-i " + iv.getMosaicPrmName() + " -o " + 
                 iv.getInputFileName());
         batchLines.add("resample -p " + iv.getResamplePrmName());
         batchLines.add(MRTUtil.geRemoveCommand(bi.getScriptType()) + " " + 
                 iv.getInputFileName());
         batchLines.add("");
      }
      
      return 0;
   }
   
   public int createBatch() {
      String batch = bi.getBatchFileName();
      File m = new File(batch);
      Writer output = null;
      String eol = MRTUtil.getEOL(bi.getScriptType());
      
      if( bi.isDebug() ) {
         System.out.println("* Creating batch file: " + batch);
      }
      
      try {
         output = new BufferedWriter(new FileWriter(m));
         //FileWriter always assumes default encoding is OK!
         for( int i = 0; i <batchLines.size(); ++i ) {
            output.write( batchLines.get(i) + eol);
         }
      } catch (IOException ex) {
         System.err.println( "ERROR: Error writing to "
                 + batch + ": " +
                 (ex.getMessage() == null ?
                    (ex.getCause() == null ? "unknown" :
                       ex.getCause()) : ex.getMessage()) );
         return 1;
      } finally {
         if( output != null ) {
            try {
               output.close();
            } catch (IOException ex) {
               System.err.println( "WARNING: Error closing file: " + batch );
            }
         }
      }
      
      return 0;
   }
}
