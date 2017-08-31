/*
 * MRTBatch.java
 *
 * Created on June 30, 2009, 8:30 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package gov.lpdaac.mrtbatch;

import java.io.File;
import java.util.Map;

/**
 *
 * @author mmerritt
 */
public class MRTBatch {
   
   /**
    * Creates a new instance of MRTBatch
    */
   public MRTBatch() {
   }
   
   /**
    * @param args the command line arguments
    */
   public static void main(String[] args) {
      int err = 0;
      MRTBatchInfo bi = new MRTBatchInfo();
      
      if( processArgs(args, bi) != 0 )
         return;
      
      if( validateArgs(bi) != 0 )
         return;
      
      bi.resetScriptType();
      bi.showInfo(true);
      
      MRTManager mgr = new MRTManager(bi);
      
      if( mgr.parseForHdf() != 0 )
         return;
      
      if( mgr.parsePrmFile() != 0 )
         return;
      
      if( mgr.createPrmFiles() != 0 )
         return;
      
      if( mgr.createBatch() != 0 )
         return;
   }
   
   /**
    * Process the command line arguments and place into the MRTBatchInfo object.
    * @param args  The command line arguments.
    * @param bi    The MRTBatchInfo object that will hold the command line values.
    * @return  0 on success or 1 on error.
    */
   private static int processArgs(String[] args, MRTBatchInfo bi) {
      int i;
      
      if( args.length == 0 ) {
         MRTBatchHelp.DisplayUsage();
         return 1;
      }
      
      // Check for help.
      for( i = 0; i < args.length; ++i ) {
         if( args[i].equals("-?") ||
             args[i].equals("-h") ||
             args[i].equals("--help") ){
            MRTBatchHelp.DisplayUsage();
            return 1;
         }
         if( args[i].equals("-v") || 
                 args[i].equals("--version") ) {
            System.out.println("MRTBatch v" + bi.getVersion());
            return 1;
         }
      }
      
      for( i = 0; i < args.length; ++i ) {
         // Ouput directory (optional)
         if( args[i].equals("-o") ||
             args[i].equals("--output") ) {
            if( i + 1 < args.length ) {
               bi.setOutputDirectory(args[++i]);
            } else {
               System.err.println( "ERROR: Expecting argument after the " + 
                       "-o,--output switch and found none.");
               return 1;
            }
         }
         // Script type (optional)
         if( args[i].equals("-t") ||
             args[i].equals("--type") ) {
            if( i + 1 < args.length ) {
               String str = args[++i];
               if( str.equalsIgnoreCase("BATCH") )
                  bi.setScriptType(MRTBatchInfo.ScriptType.BATCH);
               else if( str.equalsIgnoreCase("SCRIPT") )
                  bi.setScriptType(MRTBatchInfo.ScriptType.SCRIPT);
               else if( str.equalsIgnoreCase("CSCRIPT") )
                  bi.setScriptType(MRTBatchInfo.ScriptType.CSCRIPT);
               else {
                  System.err.println("ERROR: Invalid type found after the " + 
                          "-t,--type switch.");
                  System.err.println("       Valid values are BATCH, SCRIPT, " +
                          "or CSCRIPT, but found \"" + str + "\"." );
                  return 1;
               }
            } else {
               System.err.println("ERROR: Expecting argument after the " +
                       "-t,--type switch and found none.");
               return 1;
            }
         }
         // Text file containing hdf files (required)
         if( args[i].equals("-f") ||
             args[i].equals("--file") ) {
            if( i + 1 < args.length ) {
               bi.setTextFileName(args[++i]);
            } else {
               System.err.println("ERROR: Expecting argument after the " +
                       "-f,--file switch and found none.");
               return 1;
            }
         }
         // HDF diretcory containing hdf files (required)
         if( args[i].equals("-d") ||
             args[i].equals("--dir") ) {
            if( i + 1 < args.length ) {
               bi.setHdfDirName(args[++i]);
            } else {
               System.err.println("ERROR: Expecting argument after the " +
                       "-d,--dir switch and found none.");
               return 1;
            }
         }
         // Parameter file (required)
         if( args[i].equals("-p") ||
             args[i].equals("--prmfile") ) {
            if( i + 1 < args.length ) {
               bi.setPrmFileName(args[++i]);
            } else {
               System.err.println("ERROR: Expecting argument after the " +
                       "-p,--prmfile switch and found none.");
               return 1;
            }
         }
         // Batch file (optional)
         if( args[i].equals("-b") ||
             args[i].equals("--batch") ) {
            if( i + 1 < args.length ) {
               bi.setBatchFileName(args[++i]);
            } else {
               System.err.println("ERROR: Expecting argument after " +
                       "the -b,--batch switch and found none.");
               return 1;
            }
         }

         // Debug (optional)
         if( args[i].equals("-D") ||
             args[i].equals("--debug") ) {
            bi.setDebug(1);
         }
         // SkipBad (optional)
         if( args[i].equals("-s") ||
             args[i].equals("--skipbad") ) {
            bi.setSkipBad(true);
         }
      }
      
      if( bi.isDebug() ) {
        System.out.println("------------------- Properties ----------------------");
        for (Map.Entry<Object, Object> e : System.getProperties().entrySet()) {
           System.out.println(e);
        }
        System.out.println("---------------- End of Properties ------------------");
      }
      
      return 0;
   }

   private static int validateArgs(MRTBatchInfo bi) {
      String str;
      String tstr;
      File f;
      
      str = bi.getTextFileName();
      tstr = bi.getHdfDirName();
      
      if( str.length() == 0 && tstr.length() == 0 ) {
         System.err.println("ERROR: Either the -f,--file switch or the -d,--dir " +
                 "switch is required.");
         return 1;
      }
      if( str.length() > 0 && tstr.length() > 0 ) {
         System.err.println("ERROR: Both the -f,--file and -d,--dir switch cannot " +
                 "be used together.");
         System.err.println("       Please use one or the other.");
         return 1;
      }
      
      if( str.length() > 0 ) {
         f = new File(str);
         if( !f.exists() ) {
            System.err.println("ERROR: Could not find file \"" + str + "\".");
            return 1;
         } else if( !f.isFile() ) {
            System.err.println("ERROR: \"" + str + "\" is not a file.");
            return 1;
         }
      }

      if( tstr.length() > 0 ) {
         f = new File(tstr);
         if( !f.exists() ) {
            System.err.println("ERROR: Could not find directory \"" + tstr + "\".");
            return 1;
         } else if( !f.isDirectory() ) {
            System.err.println("ERROR: \"" + str + "\" is not a directory.");
            return 1;
         }
      }
      
      str = bi.getPrmFileName();
      if( str.length() == 0 ) {
         System.err.println("ERROR: -p,--prmfile switch is required.");
         return 1;
      }
      f = new File(str);
      if( !f.exists() ) {
         System.err.println("ERROR: Could not find prm file \"" + str + "\".");
         return 1;
      } else if( !f.isFile() ) {
         System.err.println("ERROR: The prm file \"" + str + "\" is not a file.");
         return 1;
      }
      
      str = bi.getOutputDirectory();
      if( str.length() != 0 ) {
         f = new File(str);
         if( f.exists() && !f.isDirectory() ) {
            System.err.println("ERROR: The ouput directory specified already exists but");
            System.err.println("       it is not a directory.");
            return 1;
         }
      }
      
      return 0;
   }

}
