/*
 * MrtBatchInfo.java
 *
 * Created on June 30, 2009, 8:33 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package gov.lpdaac.mrtbatch;

/**
 *
 * @author mmerritt
 */
public class MRTBatchInfo {
   
   public static enum ScriptType { NOT_DEFINED, BATCH, SCRIPT, CSCRIPT;
   public String value() {return name();}
   public static ScriptType fromValue(String s) { return valueOf(s);} };
   
   private String textFileName = "";
   private String hdfDirName = "";
   private String prmFileName = "";
   private ScriptType scriptType = ScriptType.NOT_DEFINED;
   private String outputDirectory = "";
   private int debug = 0;
   private Boolean skipBad = false;
   private String batchFileName = "";
   
   private final static String inputName = "INPUT_FILENAME";
   private final static String outputName = "OUTPUT_FILENAME";
   private final static String spectralName = "SPECTRAL_SUBSET";
   private static final String version = "1.2";
   
   /** Creates a new instance of MrtBatchInfo */
   public MRTBatchInfo() {
   }

   public String getTextFileName() {
      return textFileName;
   }

   public void setTextFileName(String textFileName) {
      this.textFileName = textFileName;
   }

   public String getPrmFileName() {
      return prmFileName;
   }

   public void setPrmFileName(String prmFileName) {
      this.prmFileName = prmFileName;
   }

   public ScriptType getScriptType() {
      return scriptType;
   }

   public void setScriptType(ScriptType scriptType) {
      this.scriptType = scriptType;
   }

   public String getOutputDirectory() {
      return outputDirectory;
   }

   public void setOutputDirectory(String outputDirectory) {
      this.outputDirectory = outputDirectory;
   }

   public int getDebug() {
      return debug;
   }

   public void setDebug(int debug) {
      this.debug = debug;
   }
   
   public Boolean isDebug() {
      if( debug != 0 )
         return true;
      return false;
   }
   
   public void resetScriptType() {
      if( scriptType == ScriptType.NOT_DEFINED ) {
         if( MRTUtil.isWindows() ) {
            scriptType = ScriptType.BATCH;
         } else {
            scriptType = ScriptType.SCRIPT;
         }
      }
   }
   
   public void showInfo(Boolean dependOnDebug) {
      if( dependOnDebug && !isDebug() )
         return;
      System.out.println("Input text file : " + (this.getTextFileName().length() == 0 ?
         "Not used" : this.getTextFileName()) );
      System.out.println("Input directory : " + (this.getHdfDirName().length() == 0 ?
         "Not used" : this.getHdfDirName()) );
      System.out.println("Input prm file  : " + this.getPrmFileName() );
      System.out.println("Script type     : " + this.getScriptType().value() );
      System.out.println("Output directory: " + (this.getOutputDirectory().length() == 0 ?
         "Specified by path(s) in text file." : this.getOutputDirectory()) );
      System.out.println("Batch file name : " + this.getBatchFileName());
      System.out.println("Skip bad files  : " + (this.isSkipBad() ? "True" : "False") );
      System.out.println("Debug is        : " + (this.isDebug() ? "On" : "Off") );
   }

   public Boolean isSkipBad() {
      return skipBad;
   }

   public void setSkipBad(Boolean skipBad) {
      this.skipBad = skipBad;
   }

   public String getBatchFileName() {
      return (batchFileName.length() == 0 ? "mrtbatch" : batchFileName) + 
              (getScriptType() == MRTBatchInfo.ScriptType.BATCH ? ".bat" : "");
   }

   public void setBatchFileName(String batchFileName) {
      this.batchFileName = batchFileName;
   }

   public static String getInputName() {
      return inputName;
   }

   public static String getOutputName() {
      return outputName;
   }

   public static String getSpectralName() {
      return spectralName;
   }

   public static String getVersion() {
      return version;
   }

   public String getHdfDirName() {
      return hdfDirName;
   }

   public void setHdfDirName(String hdfDirName) {
      this.hdfDirName = hdfDirName;
   }
}
