/*
 * MRTUtil.java
 *
 * Created on July 2, 2009, 9:40 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package gov.lpdaac.mrtbatch;

import java.io.File;

/**
 *
 * @author mmerritt
 */
public class MRTUtil {
   
   /** Creates a new instance of MRTUtil */
   public MRTUtil() {
   }
   
   static public String removeQuotes(String str) {
      while( str.startsWith("\"") ) {
         str = str.substring(1);
      }
      while( str.endsWith("\"") ) {
         str = str.substring(str.length() - 1);
      }
      return str;
   }
   
   static public String createMosaicFile( String path, String file,
           MRTBatchInfo.ScriptType stype ) {
      String mfile = MRTUtil.createNormalizedPath(path, stype) + file;
      if( stype == MRTBatchInfo.ScriptType.BATCH ||
              stype == MRTBatchInfo.ScriptType.CSCRIPT ) {
         mfile = mfile.replaceAll("/", "\\\\");
      }
      return addQuotesIfSpaces(mfile);
   }

   static public String createNormalizedPath(String path, 
           MRTBatchInfo.ScriptType stype ) {
      String cyg = "";
      String drive = "";
      
      path = path + ( !(path.endsWith("\\") || path.endsWith("/")) ? 
                 File.separatorChar : "" );
      path = path.replaceAll("\\\\", "/");
      
      if( stype == MRTBatchInfo.ScriptType.BATCH || 
              stype == MRTBatchInfo.ScriptType.CSCRIPT ) {
         if( path.length() > 10 ) {
            cyg = path.substring(0,10);
            if( cyg.equalsIgnoreCase("/cygdrive/") ) {
              drive = path.substring(10,11);
              path = path.replaceFirst(cyg + drive, drive + ":" );
            }
         }
      } else if( stype == MRTBatchInfo.ScriptType.SCRIPT ) {
         if( path.length() > 1 && path.substring(1,2) == ":" ) {
            path = path.substring(2);
         }
         if( path.length() > 10 ) {
            cyg = path.substring(0,10);
            if( cyg.equalsIgnoreCase("/cygdrive/") ) {
               path = path.substring(10);
            }
         }
      }
      return path;
   }
   
   static public String createOSPath(String path) {
      path = path.replaceAll("\\\\", "/");
      if( MRTUtil.isWindows() ) {
         if( path.length() > 10 ) {
            String cyg = path.substring(0,10);
            if( cyg.equalsIgnoreCase("/cygdrive/") ) {
              String drive = path.substring(10,11);
              path = path.replaceFirst(cyg + drive, drive + ":" );
            }
         }
         path = path.replaceAll("/", "\\\\");
      } else {
         if( path.length() > 1 && path.substring(1,2) == ":" ) {
            path = path.substring(2);
         }
      }
      return path;
   }
   
   static public Boolean isWindows() {
      String os = System.getProperty("os.name");
      if( os.length() >= 3 ) {
         os = os.substring(0,3);
         if( os.equalsIgnoreCase("WIN") ) {
            return true;
         }
      }
      return false;
   }
   
   static public Boolean isMac() {
      String os = System.getProperty("os.name");
      if( os.length() >= 3 ) {
         os = os.substring(0,3);
         if( os.equalsIgnoreCase("MAC") ) {
            return true;
         }
      }
      return false;
   }
   
   static public String reduceSpectralSubset( String subset ) {
      String reducedSubset = "";      
      for( int i = 0; i < subset.length(); ++i ) {
         if( subset.charAt(i) == '1' ) {
            if( reducedSubset.length() == 0 )
               reducedSubset = "1";
            else
               reducedSubset = reducedSubset + " 1";
         }
      }
      return reducedSubset;
   }
   
   static public String addQuotesIfSpaces(String file) {
      if( file.contains(" ") )
         return "\"" + file + "\"";
      return file;
   }
   
   static public String getEOL(MRTBatchInfo.ScriptType stype) {
      return (stype == MRTBatchInfo.ScriptType.BATCH ? "\r\n" : (isMac() ? "\r" : "\n"));
   }

   static public String geRemoveCommand(MRTBatchInfo.ScriptType stype) {
      return (stype == MRTBatchInfo.ScriptType.BATCH ? "del" : "rm");
   }
   
   static public String getBatchFileName(MRTBatchInfo.ScriptType stype) {
      return "mrtbatch" + (stype == MRTBatchInfo.ScriptType.BATCH ? ".bat" : "");
   }
   
   static public String exractFromParentheses(String str) {
      int sp;
      int ep;
      
      sp = str.indexOf("(");
      if( sp == -1 )
         return null;
      ep = str.indexOf(")", sp);
      if( sp == -1 )
         return null;
      return str.substring(sp + 1,ep).trim();
   }

}
