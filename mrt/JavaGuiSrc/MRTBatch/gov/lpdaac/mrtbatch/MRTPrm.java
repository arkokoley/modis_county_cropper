/*
 * MRTPrm.java
 *
 * Created on July 2, 2009, 9:32 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package gov.lpdaac.mrtbatch;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;

/**
 *
 * @author mmerritt
 */
public class MRTPrm {
   
   private ArrayList<String> lines = null;
   private String outputExtension = "";
   private String spectralSubset = "";
   private Boolean inputFound = false;
   private Boolean outputFound = false;
   private MRTBatchInfo bi = null;
   
   /** Creates a new instance of MRTPrm */
   public MRTPrm(MRTBatchInfo batchInfo ) {
      lines = new ArrayList<String>();
      bi = batchInfo;
   }
   
   private int addLine(String line) {
      String addLine = line;
      if( !line.startsWith("#") ) {
         String ucline = line.toUpperCase();
         if( ucline.indexOf( bi.getInputName() ) != -1 ) {
            addLine = bi.getInputName() + " = ";
            inputFound = true;
         } else if( ucline.indexOf( bi.getOutputName() ) != -1 ) {
            int idx =  line.lastIndexOf(".");
            if( idx == -1 ) {
               System.out.println( "ERROR: Could not find " + bi.getOutputName() + 
                       "'s file extension." );
               return 1;
            }
            String ext = line.substring(idx);
            if( ext.length() > 1 ) {
               ext = ext.substring(1);
            }
            if( ext.equalsIgnoreCase("TIF") || ext.equalsIgnoreCase("HDF") || ext.equalsIgnoreCase("HDR") )
               outputExtension = ext;
            else {
               System.out.println( "ERROR: Invalid " + bi.getOutputName() + 
                       "'s file extension found." );
               System.out.println( "       Should be \"TIF\", \"HDF\", or \"HDR\"." );
               return 1;
            }
            addLine = bi.getOutputName() + " = ";
            outputFound = true;
         } else if( ucline.indexOf(bi.getSpectralName()) != -1 ) {
            String pstr = MRTUtil.exractFromParentheses(line);
            if( pstr == null ) {
               System.out.println( "ERROR: Could not determine the " + bi.getSpectralName() + 
                       ".  Possibly invalid format." );
               return 1;
            }
            spectralSubset = pstr;
            lines.add(bi.getSpectralName() + " = (" + 
                    MRTUtil.reduceSpectralSubset(spectralSubset) + ")");
            addLine = "#ORIG_SPECTRAL_SUBSET = (" + spectralSubset +")";
         }
      }
      lines.add(addLine);
      return 0;
   }
   
      public int parsePrmFile() {
      String filename = bi.getPrmFileName();
      
      if( bi.isDebug() ) {
         System.out.println("* Processing " + filename + "...");
      }
      
      inputFound = false;
      outputFound = false;
      outputExtension = "";
      
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
               if( addLine(line) != 0 )
                  return 1;
            }
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
      
      if( inputFound == false ) {
         System.err.println( "ERROR: Could not find " + bi.getInputName() +
                 " in parameter file." );
         return 1;
      } else if( outputFound == false ) {
         System.err.println( "ERROR: Could not find " + bi.getOutputName() +
                 " in parameter file." );
         return 1;
      }
      
      if( bi.isDebug() ) {
         System.out.println("Output extension found: " + getOutputExtension());
         displayLines();
      }
      
      return 0;      
   }
   
   public ArrayList<String> getPrmLines() {
      return lines;
   }

   public String getOutputExtension() {
      return outputExtension;
   }

   public void displayLines() {
      for( int i = 0; i < lines.size(); ++i ) {
         System.out.println(lines.get(i));
      }
   }

   public String getSpectralSubset() {
      return spectralSubset;
   }
   
}
