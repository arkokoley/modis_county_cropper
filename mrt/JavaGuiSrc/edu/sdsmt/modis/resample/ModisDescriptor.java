/******************************************************************************
   NAME:      ModisDescriptor.class
   PURPOSE:
      The purpose of this class is to create a data container to contain all of
      information necessary for resampler to execute properly. Most of functions
      are invoked by ModisModel class. It attempts to create an image of several
      layers to isolate the data model from view.

   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development
   1.00   Fall'00     Shujing Jia                   Java  Modifications

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    Java is inherently platform indepentent so the compiled byte code can be
    executed on any of platforms (e.g. Windows, Unix, and Linux). Virtually,
    there is no limitation on running Java byte codes.
    However there is compiler requirement regarding JDK package (version 2.0).

   PROJECT: ModisModel
   NOTES:
    1.  This class contains many dispatching functions in response to invocation
        from ModisModel.class and offers an appearance of a delayed processing.

******************************************************************************/
package edu.sdsmt.modis.resample;

import java.io.*;
import java.util.*;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import edu.sdsmt.modis.image.*;

public class ModisDescriptor {
   /*
    * FileOpenType
    */
   final public static int BAD_FILE_MODE = 0;
   final public static int FILE_WRITE_MODE = 1;
   final public static int FILE_READ_MODE = 2;

   /*
    * FileType
    */
   final public static int BAD_FILE_TYPE = 0;
   final public static int MULTIFILE = 1;
   final public static int HDFEOS = 2;
   final public static int GEOTIFF = 3;

   /*
    * DataType
    */
   final public static int BAD_DATA_TYPE = 0;
   final public static int ASCII = 1;
   final public static int INT8 = 2;
   final public static int UINT8 = 3;
   final public static int INT16 = 4;
   final public static int UINT16 = 5;
   final public static int INT32 = 6;
   final public static int UINT32 = 7;
   final public static int FLOAT32 = 8;

   /*
    * ProjectionType
    */
   final public static int BAD_PROJECTION_TYPE = 0;
   final public static int PROJ_GEO = 1;
   final public static int PROJ_HAM = 2;
   final public static int PROJ_IGH = 3;
   final public static int PROJ_ISIN = 4;
   final public static int PROJ_LA = 5;
   final public static int PROJ_LCC = 6;
   final public static int PROJ_MOL = 7;
   final public static int PROJ_PS = 8;
   final public static int PROJ_SIN = 9;
   final public static int PROJ_UTM = 10;
   final public static int PROJ_TM = 11;
   final public static int PROJ_AEA = 12;
   final public static int PROJ_MERC = 13;
   final public static int PROJ_ER = 14;


   /*
    * ResamplingType
    */
   final public static int BAD_RESAMPLING_TYPE = 0;
   final public static int NN = 1;
   final public static int BI = 2;
   final public static int CC = 3;

   /*
    * Output Pixel Size Units
    */
   final public static int BAD_OUTPUT_PIXEL_SIZE_UNIT  = 0;
   final public static int METERS = 1;
   final public static int FEET = 2;
   final public static int DEGREES = 3;
   final public static int ARC_SEC = 4;

   /*
    * CornerType
    */
   final public static int UL = 0;
   final public static int UR = 1;
   final public static int LL = 2;
   final public static int LR = 3;

   /*
    * Spatial Subset
    */
   final public static int BAD_SPATIAL_SUBSET_TYPE = 0;
   final public static int SPACE_LAT_LON = 1;
   final public static int SPACE_LINE_SAMPLE = 2;
   final public static int SPACE_PROJ_XY = 3;

   /* various filenames */
   /**
    * The name of the parameter (.prm) file, if provided.
    */
   private String parameterFilename;
   /**
    * The name of the input file (.hdr or .hdf), if provided.
    */
   private String inputFilename;
   /**
    * The name of the output parameter (.prm) file.
    */
   private String outputFilename;
   /**
    * Are we mosaicking?
    */
   public int mosaic;
   /**
    * The pixel size to generate for the resampled image.  If null
    * or empty String, then use input size.
    */
   private String outputPixelSize;
   //private String outputPixelSizeDefault;
    
   // The type of the datum selected from the combo box (default is
   // WGS84).
   private String Datum = "NoDatum";
    
   //the input datum
   private String InputDatum;


   /* file types (multifile, HDF-EOS, GeoTiff) */
   /**
    * The input file type  (multifile, HDF-EOS, GeoTiff)
    */
   private int inputFileType;
   /**
    * The output file type  (multifile, HDF-EOS, GeoTiff)
    */
   private int outputFileType;

   /* data types (byte, int, etc.) */
   /**
    * The input data type  (byte, int, etc.)
    */
   private int inputDataType;
   /**
    * The output data type  (byte, int, etc.)
    */
   private int outputDataType;

   /**
    * The number of files containing band information.
    */
   private int numFiles;

   /**
    * number of bands in image
    */
   private int nbands;

   /**
    * array[nbands] of false/true
    */
   private boolean[] selectedBands;

   /**
    * array[nbands] of info about each band
    */
   private BandType[] bandinfo;

   /**
    * overall image size (corner points - lat/long in decimal degrees)
    */
   private double[][] inputImageExtent = new double[4][2];  //[4][2];

   /**
    * spatial subset type (BAD_SPATIAL_SUBSET_TYPE, SPACE_LAT_LON,
    *                      SPACE_LINE_SAMPLE, SPACE_PROJ_XY)
    */
   private int spatialsubsetType;

   // Check whether datum exists in the file;
   private boolean DatumOption;
   
   // Check whether UTM zone exists in the file;
   private boolean UtmOption;

   /**
    * spatial subset corner points (UL, UR, LL, LR in lat/long)
    */
   private double[][] cornerPoints = new double[4][2];
   private double[][] gringPoints = new double[4][2];
   private double[][] cornerXYPoints = new double[4][2];
   private boolean bGringExist1, bGringExist2, bGringExist3, bGringExist4;
   private boolean bCornerXYExist1, bCornerXYExist2, bCornerXYExist3, bCornerXYExist4;
       
   /**
    * Array of files to hold the input file names
    */
   public File [] temp = new File[552];
        
   /**Array_index for the File array*/
   public int array_index = 0;

   /**
    * highest rez spatial subset corner points (UL, UR, LL, LR in row/col)
    */
   private int[][] inputCornerPoints = new int[4][2];

   /**
    *  Input projection type (ISIN, GEO, UTM, etc.)
    */
   private int inputProjectionType;
   /**
    *  Output projection type (ISIN, GEO, UTM, etc.)
    */
   private int outputProjectionType;   /**
    * UTM zone (-60, ..., 60)
    */
   private int utmZone = 0;
   
   private int InpututmZone = 0;

   /**
    *  Selected output pixel size unit  (METERS, FEET, DEGREES, ARC_SEC.)
    */
   private int outputPixelSizeUnit;

   /**
    * resampling type (NN, BI, CC)
    */
   private int resamplingType;

   /**
    * array of 15 projection parameters
    */
   private double[] inputProjectionParameters = new double[15];
   private double[] outputProjectionParameters = new double[15];


   private String[]   inputBandNamesArray;
   private int[]      inputNumOfLinesArray;
   private double[]   inputPixelSizeArray;
   private int[]      inputNumOfSamplesArray;
   private String[]   inputDataTypeArray;
   private double[]   inputMinValueArray;
   private double[]   inputMaxValueArray;
   private double[]   inputBackgroundFillArray;

   private String[]   sULArray = new String[2];
   private String[]   sLRArray = new String[2];

   /**
    * Input datum code
    */
   private int inputDatumCode;
        
        private String MosaicFileName;
   /**
    * Output datum code
    */
   private int outputDatumCode;
   /**
    * Input zone code
    */
   private int inputZoneCode;
   /**
    * Output zone code
    */
   private int outputZoneCode;

   /**
    * Input projection info structure for Geolib
    */
   private ProjInfo inProjectionInfo;
   /**
    * Output projection info structure for Geolib
    */
   private ProjInfo outProjectionInfo;

   /**
    * file information for handling multi-file and multiple
    * GeoTIFF outputs
    */
   private OutFileType outputFileInfo;

   /**
    * the number of output files actually written
    */
   private int nfilesOut;
        
   private ImageFrame img = ImageFrame.getInstance();

   /**
    * Constructor for the class.
    */

   /***************************************************************************
   NAME:      ModisDescriptor
   PURPOSE:
             Constructor--do nothing
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   **************************************************************************/
   public ModisDescriptor()
   {
   }

   /***************************************************************************
   NAME:       getOutputPixelSizeUnitString
   PURPOSE:
             To look up output pixel size unit for given ID
   RETURN VALUE:
            String
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   private java.lang.String getOutputPixelSizeUnitString(int pixelSizeUnit) {
      switch (pixelSizeUnit) {
         case METERS:
            return "METERS";
         case FEET:
            return "FEET";
         case DEGREES:
            return "DEGREES";
         case ARC_SEC:
            return "ARC-SEC";
         default:
            return "BAD_OUTPUT_PIXEL_SIZE_UNIT";
      }
   }

   /***************************************************************************
   NAME:      getDataTypeString
   PURPOSE:
             To look up Data Type for given ID
   RETURN VALUE:
            String
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   private java.lang.String getDataTypeString(int dataType) {
      switch (dataType) {
         case ASCII:
            return "ASCII";
         case INT8:
            return "INT8";
         case UINT8:
            return "UINT8";
         case INT16:
            return "INT16";
         case UINT16:
            return "UINT16";
         case INT32:
            return "INT32";
         case UINT32:
            return "UINT32";
         case FLOAT32:
            return "FLOAT32";
         default:
            return "BAD_DATA_TYPE";
      }
   }

   /***************************************************************************
   NAME:       getMaximumLines
   PURPOSE:
             To get maximum number of lines
   RETURN VALUE:
            integer
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public int getMaximumLines() {
      int largest = 0;
      for (int i=0; i < getNumBands(); ++i) {
         if (inputNumOfLinesArray[i] > largest) {
            largest = inputNumOfLinesArray[i];
         }
      }
      return largest;
   }


   /***************************************************************************
   NAME:       getMaximumSamples
   PURPOSE:
             To get maximum number of samples
   RETURN VALUE:
               integer
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public int getMaximumSamples() {
      int largest = 0;
      for (int i=0; i < getNumBands(); ++i) {
         if (inputNumOfSamplesArray[i] > largest) {
            largest = inputNumOfSamplesArray[i];
         }
      }
      return largest;
   }


   /***************************************************************************
   NAME:       getNumFiles
   PURPOSE:
             To get number of files
   RETURN VALUE:
            integer
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public int getNumFiles() {
      return numFiles;
   }


   /***************************************************************************
   NAME:       getProjectionTypeInt
   PURPOSE:
             To look up projection type ID for given projection type string
   RETURN VALUE:
            integer
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   private int getProjectionTypeInt(String s)
         throws IllegalArgumentException {
      if ( s.equals("ISIN") || s.equals("INTEGERIZED_SINUSOIDAL")) {
         return (PROJ_ISIN);
      } else if (s.equals("AEA") || s.equals("ALBERS_EQUAL_AREA")) {
         return (PROJ_AEA);
      } else if (s.equals("ER") || s.equals("EQUIRECTANGULAR")) {
         return (PROJ_ER);
      } else if (s.equals("GEO") || s.equals("GEOGRAPHIC")) {
         return (PROJ_GEO);
      } else if (s.equals("HAM") || s.equals("HAMMER")) {
         return (PROJ_HAM);
      } else if (s.equals("IGH") || s.equals("INTERRUPTED_GOODE_HOMOLOSINE")) {
         return (PROJ_IGH);
      } else if (s.equals("LA") || s.equals("LAMBERT_AZIMUTHAL")) {
         return (PROJ_LA);
      } else if (s.equals("MERCAT") || s.equals("MERCATOR")) {
         return (PROJ_MERC);
      } else if (s.equals("MOL") || s.equals("MOLLWEIDE")) {
         return (PROJ_MOL);
      } else if (s.equals("LCC") || s.equals("LAMBERT_CONFORMAL_CONIC")) {
         return (PROJ_LCC);
      } else if (s.equals("PS") || s.equals("POLAR_STEREOGRAPHIC")) {
         return (PROJ_PS);
      } else if (s.equals("SIN") || s.equals("SINUSOIDAL")) {
         return (PROJ_SIN);
      } else if (s.equals("TM") || s.equals("TRANSVERSE_MERCATOR")) {
         return (PROJ_TM);
      } else if (s.equals("UTM") || s.equals("UNIVERSAL_TRANSVERSE_MERCATOR")) {
         return (PROJ_UTM);
      } else {
         setInputProjectionType(BAD_PROJECTION_TYPE);
         throw new IllegalArgumentException();
      }
   }

   /**************************************************************************
   NAME:       getProjectionTypeString
   PURPOSE:
            To look up projection type string by given projection ID
   RETURN VALUE:
            String
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public java.lang.String getProjectionTypeString(int projType) {
      switch (projType) {
          case PROJ_AEA: return "AEA";
          case PROJ_ER: return "ER";
          case PROJ_GEO: return "GEO";
          case PROJ_HAM: return "HAM";
          case PROJ_IGH: return "IGH";
          case PROJ_ISIN: return "ISIN";
          case PROJ_LA: return "LA";
          case PROJ_LCC: return "LCC";
          case PROJ_MOL: return "MOL";
          case PROJ_MERC: return "MERCAT";
          case PROJ_PS: return "PS";
          case PROJ_SIN: return "SIN";
          case PROJ_UTM: return "UTM";          
          case PROJ_TM: return "TM";
          default: return "BAD_PROJECTION_TYPE";
      }
   }

   /***************************************************************************
   NAME:       getSpatialSubsetType
   PURPOSE:
             Get the currently active spatial subset type.

   RETURN VALUE:
             @return SPACE_LAT_LON if lat/long, else SPACE_LINE_SAMPLE if line/sample, else SPACE_PROJ_XY if proj x/y
             @see #SPACE_LAT_LON
             @see #SPACE_LINE_SAMPLE
             @see #SPACE_PROJ_XY

   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public int getSpatialSubsetType() { return spatialsubsetType; }

   /***************************************************************************
   NAME:                       getSpatialSubsetTypeString
   PURPOSE:
             To retrieve the spatial subset type string
   RETURN VALUE:
             String
                                String
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.   Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT:        ModisTool
   NOTES:

   ***************************************************************************/
   public String getSpatialSubsetTypeString(int ssType)
         throws IllegalArgumentException {
      switch (ssType) {
         case SPACE_LAT_LON:
             return "INPUT_LAT_LONG";
         case SPACE_LINE_SAMPLE:
             return "INPUT_LINE_SAMPLE";
         case SPACE_PROJ_XY:
             return "OUTPUT_PROJ_COORDS";
         default:
                        throw new IllegalArgumentException();
      }
   }

   /***************************************************************************
   NAME:       read2dPoint
   PURPOSE:
             To read 2 points from tokenstream
   RETURN VALUE:
            String []
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   private java.lang.String[] read2dPoint(java.io.StreamTokenizer tok,
                                          java.lang.String parmName,
                                          int errorId)
         throws ReadException {
      return read2dPoint(tok,parmName, errorId, false);
   }

   /***************************************************************************
   NAME:       read2dPoint
   PURPOSE:
             To actually read and parse 2 points
             @ overloaded function
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   private java.lang.String[] read2dPoint(java.io.StreamTokenizer tok,
                                          java.lang.String parmName,
                                          int errorId, boolean optional)
         throws ReadException {
      String s;
      int tokenType;
      String sArray[] = new String[2];

      try {
         tokenType = readNextNonCommentToken(tok);
         s = tok.sval;
         if ( tokenType != StreamTokenizer.TT_WORD)
            throw new ReadException(errorId);
         if ( !s.equals(parmName) ) {
            if ( !optional ) {
               throw new ReadException(errorId);
            } else {
               tok.pushBack();
               return null;
            }
         }
 

         tokenType = readNextNonCommentToken(tok);
         s = tok.sval;
         if ( tokenType != StreamTokenizer.TT_WORD || !s.equals("("))
            throw new ReadException(errorId);
         tokenType = readNextNonCommentToken(tok);
         s = tok.sval;
         if ( tokenType != StreamTokenizer.TT_WORD)
            throw new ReadException(errorId);
         sArray[0] = s;

         tokenType = readNextNonCommentToken(tok);
         s = tok.sval;
         if ( tokenType != StreamTokenizer.TT_WORD)
            throw new ReadException(errorId);
         sArray[1] = s;

         // read in )
         tokenType = readNextNonCommentToken(tok);
         s = tok.sval;
         if ( tokenType != StreamTokenizer.TT_WORD ||  !s.equals(")"))
            throw new ReadException(errorId);
      } catch (IOException e) {
         throw new ReadException(errorId);
      }
      return sArray;
   }

   /***************************************************************************
   NAME:       read2dPoint
   PURPOSE:
            To invoke read2dPoint()
            @ an overloaded function

   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   private java.lang.String[] read2dPoint(java.io.StreamTokenizer tok,
                                          int errorId) throws ReadException {
      return read2dPoint(tok, errorId, false);
   }

   /***************************************************************************
   NAME:       read2dPoint
   PURPOSE:
            To really read from StreamTokenizer
            @ an overloaded function

   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   private java.lang.String[] read2dPoint(java.io.StreamTokenizer tok,
                                          int errorId, boolean optional)
         throws ReadException {
      String s;
      int tokenType;
      String sArray[] = new String[2];

      try {
         tokenType = readNextNonCommentToken(tok);
         s = tok.sval;
         if ( tokenType != StreamTokenizer.TT_WORD ||  !s.equals("("))
            throw new ReadException(errorId);

         tokenType = readNextNonCommentToken(tok);
         s = tok.sval;
         if ( tokenType != StreamTokenizer.TT_WORD)
            throw new ReadException(errorId);
         sArray[0] = s;

         tokenType = readNextNonCommentToken(tok);
         s = tok.sval;
         if ( tokenType != StreamTokenizer.TT_WORD)
            throw new ReadException(errorId);

         sArray[1] = s;

         tokenType = readNextNonCommentToken(tok);
         s = tok.sval;
         if ( tokenType != StreamTokenizer.TT_WORD ||  !s.equals(")")) {
            throw new ReadException(errorId);
         }
      } catch (IOException e) {
         throw new ReadException(errorId);
      }

      return sArray;
   }



   /***************************************************************************
   NAME:       readPixelSizeAndUnit
   PURPOSE:  To read pixel size and unit


   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
            02/05/01   Shujing                      Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   private java.lang.String[] readPixelSizeAndUnit(
          java.io.StreamTokenizer tok, int errorId)
         throws ReadException {
      return readPixelSizeAndUnit(tok, errorId, false);
   }

   /***************************************************************************
   NAME:       readPixelSizeAndUnit
   PURPOSE:  TO really read pixel size and unit from StreamTokenizer
             @ an overloaded function

   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
          02/05/01   Shujing Jia                    Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   private java.lang.String[] readPixelSizeAndUnit(
         java.io.StreamTokenizer tok, int errorId,
         boolean optional) throws ReadException {
      String s;
      int tokenType;
      String sArray[] = new String[2];
      tok.eolIsSignificant(true);
      try {

         tokenType = readNextNonCommentToken(tok);
         s = tok.sval;
         if ( tokenType != StreamTokenizer.TT_WORD)
            throw new ReadException(errorId);
         sArray[0] = s;

         tokenType = readNextNonCommentToken(tok);
         s = tok.sval;
            if (tokenType != StreamTokenizer.TT_WORD ) {
                if (tokenType == StreamTokenizer.TT_EOL )
                    sArray[1] = " ";
                else
                    throw new ReadException(errorId);
            } else
             sArray[1] = s;
      
      } catch (IOException e) {
         throw new ReadException(errorId);
      }
      tok.eolIsSignificant(false);
      return sArray;
   }


   /**
    * Reads a Header file (.hdr) and populates
    * corresponding data fields in the current object.
    */
   /**************************************************************************
   NAME:       readHdfHeader
   PURPOSE:
            To read HDF file header and convert .hdf file to .hdr file by
            calling C-module resample.exe
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void readHdfHeader()   throws ReadException {
      String hdfFilename = getInputFilename();

      //sj 3/16/01, check the existence of the input file in the current
      //            directory
      File inputFile = new File (hdfFilename);
      if (!inputFile.exists()) {
         throw new ReadException (hdfFilename,
                                  ErrorHandler.ERROR_FILE_NOT_EXIST);
      } else {

         {
            final Runtime rt = Runtime.getRuntime();
            String hdrFilename = null;
            Process p;
            try {
               String [] cmd = { "resample", "-h", hdfFilename };
               p = rt.exec(cmd);
               InputStream is = p.getInputStream();
               InputStreamReader isr = new InputStreamReader(is);
               BufferedReader br = new BufferedReader(isr);
               hdrFilename = br.readLine();
            } catch (java.io.IOException e) {  
               throw new ReadException (hdfFilename,
                                        ErrorHandler.ERROR_EXE_FILE_NOT_EXIST);
               // return;
            }
            int processReturn = 0;
            try {
               processReturn = p.waitFor();
            } catch (InterruptedException e) {
               processReturn=-9999;
            }

            if (processReturn == 0) {
               setInputFilename("TmpHdr.hdr");
               readHeaderFile();

               // JMW 02/27/01 - try deleting TmpHdr.hdr
               File tempFile= new File(hdrFilename);
               tempFile.delete();

               setInputFilename(hdfFilename);
            }
         }
      }
   }


   /***************************************************************************
   NAME:       consumeComment
   PURPOSE:
            Consume a comment--read all characters up to
            and including the end of line
            and discard them.

   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   private int consumeComment(StreamTokenizer inputTok)
         throws IOException {
      int ttype=0;

      inputTok.eolIsSignificant(true);
      CONSUME_COMMENT_LOOP:
      while (true) {
         ttype = inputTok.nextToken();
         if ( ttype == StreamTokenizer.TT_EOL ||
              ttype == StreamTokenizer.TT_EOF ) {
            break CONSUME_COMMENT_LOOP;
         }
      }
      inputTok.eolIsSignificant(false);
      return ttype;
   }

   /***************************************************************************
   NAME:       parseFieldValue
   PURPOSE:
             To parse the field value
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Shujing Jia                Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   private java.lang.String[] parseDefaultFieldValue (
         java.io.StreamTokenizer inputTok, int fieldSize, String defaultVal,
         int errorId) throws ReadException {

      String []fieldValueArray = new String [fieldSize];
      int tokenType;
      String s;
      try {
         tokenType = readNextNonCommentToken(inputTok);
         if ( tokenType != StreamTokenizer.TT_WORD ) {
            throw new ReadException(errorId);
         }
         s = inputTok.sval;

         if ( !s.equals("(") )
            throw new ReadException(errorId);
      } catch (IOException e) {
         throw new ReadException(errorId);
      }

      int i = 0;
      boolean yes = true;
      while (yes) {
         try  {
            tokenType = readNextNonCommentToken(inputTok);
            if ( tokenType != StreamTokenizer.TT_WORD )
               throw new ReadException(errorId);

            s = inputTok.sval;

            if (s.equals (")"))
               yes = false;
            else if (i < fieldSize)
               fieldValueArray[i++] = s;
         } catch (IOException e) {
            throw new ReadException(errorId);
         } catch (IllegalArgumentException e) {
            throw new ReadException(errorId);
         }
      }

      if (i < fieldSize) {
         for (int j = i; j < fieldSize; j++)
            fieldValueArray[j] = defaultVal;
      }

      return fieldValueArray;
   }

   /***************************************************************************
   NAME:       parseFieldValue
   PURPOSE:
             To parse the field value
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                     Shujing Jia                    Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   private java.lang.String[] parseFieldValue (
         java.io.StreamTokenizer inputTok, int fieldSize, int errorId)
         throws ReadException {

      String []fieldValueArray = new String [fieldSize];
      int tokenType;
      String s;
      try {
         tokenType = readNextNonCommentToken(inputTok);
         if ( tokenType != StreamTokenizer.TT_WORD ) {
            throw new ReadException(errorId);
         }
         s = inputTok.sval;

         if ( !s.equals("(") ) {
            throw new ReadException(errorId);
         }
      } catch (IOException e) {
         throw new ReadException(errorId);
      }

      for (int i=0; i<fieldSize; ++i) {
         try {
            tokenType = readNextNonCommentToken(inputTok);
            if ( tokenType != StreamTokenizer.TT_WORD )
               throw new ReadException(errorId);
            fieldValueArray[i] = inputTok.sval;
         } catch (IOException e) {
            throw new ReadException(errorId);
         } catch (IllegalArgumentException e) {
            throw new ReadException(errorId);
         }
      }

      try {
         tokenType = readNextNonCommentToken(inputTok);
         if ( tokenType != StreamTokenizer.TT_WORD ) {
            throw new ReadException(errorId);
            }
         s = inputTok.sval;
         if ( !s.equals(")") ) {
            throw new ReadException(errorId);
            }
      } catch (IOException e) {
         throw new ReadException(errorId);
      }

   /**/
      return fieldValueArray;
   }


   /***************************************************************************
   NAME:       fieldsErrorCheck
   PURPOSE:
             To check possible error spelling in some digit fields
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Shujing Jia                Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   private void fieldsErrorCheck (String[] sFieldValueArray,
         int size, int errorId) throws ReadException {

      for (int i=0; i<size; i++) {
         String str = sFieldValueArray[i];
         for (int k = 0; k < str.length(); k++) {
            char ch = str.charAt(k);
            if (  (!Character.isDigit(ch)) && ( ch != '-') && ( ch != '.'))
               throw new ReadException (errorId);
         }
      }
   }

   /***************************************************************************
   NAME:       initTokenRead
   PURPOSE:
             To initialize StreamTokenizer by defining some proper parses
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   private StreamTokenizer initTokenRead (Reader fileReader) {
      StreamTokenizer inputTok = new StreamTokenizer(fileReader);
      inputTok.resetSyntax();

      inputTok.whitespaceChars((int)' ',(int)' ');
      inputTok.whitespaceChars((int)'\t',(int)'\t');
      inputTok.whitespaceChars((int)'\n',(int)'\n');
      inputTok.whitespaceChars((int)'\r',(int)'\r');

      inputTok.whitespaceChars((int)'=', (int)'=');
      inputTok.whitespaceChars((int)',', (int)',');

      inputTok.quoteChar((int)'"');

      inputTok.wordChars('a','z');
      inputTok.wordChars('A','Z');
      inputTok.wordChars('_','_');
      inputTok.wordChars('0','9');
      inputTok.wordChars('-','-');
      inputTok.wordChars('.','.');
      inputTok.wordChars('#','#');
      inputTok.wordChars('\\','\\');
      inputTok.wordChars('/','/');
      inputTok.wordChars(':',':');
      inputTok.wordChars('(','(');
      inputTok.wordChars(')',')');

      inputTok.eolIsSignificant(false);
      inputTok.commentChar('#');

      return inputTok;
   }

   /***************************************************************************
   NAME:       readHeaderFile
   PURPOSE:  Reads a Header file (.hdr) and populates
             corresponding data fields in the current object

   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void readHeaderFile() throws ReadException {
      Reader fileReader;
      StreamTokenizer inputTok;
      String str;
      int nfiles;
      String s;
      String[] sArray;
      String[] sFieldValueArray;
      int tokenType;

      setSpatialSubsetType(SPACE_LAT_LON);
      readHeaderNumBands();
      readDatum();
      readUtmZone();
        
      try {
         fileReader = new FileReader(getInputFilename());
      } catch (IOException e) {
         throw new ReadException(ErrorHandler.ERROR_OPEN_INPUTHEADER);
      }
      
      inputTok = initTokenRead (fileReader);

      READ_LOOP: while (true) {   // not end of file
         try {
            tokenType = readNextNonCommentToken(inputTok);
            if (tokenType == StreamTokenizer.TT_EOF) {
               break READ_LOOP;
            } else if ( tokenType != StreamTokenizer.TT_WORD )
               throw new ReadException(ErrorHandler.ERROR_GENERAL);

            s = inputTok.sval;
            
            if (s.toUpperCase().compareTo("PROJECTION_TYPE") == 0) {
               //  read and set projection type
               s = readHeaderProperty(inputTok,
                                      ErrorHandler.ERROR_PROJECTION_TYPE);
               // return the field value
               setInputProjectionType(s.toUpperCase());

            } else if (s.toUpperCase().compareTo("UTM_ZONE") == 0)
               // projection parameters
               readNextNonCommentToken(inputTok);
            
            else if (s.toUpperCase().compareTo("PROJECTION_PARAMETERS") == 0) {
               // projection parameters
               sFieldValueArray = parseFieldValue ( inputTok, 15,
                                         ErrorHandler.ERROR_PROJPARAMS_FIELD);
               try {
                  fieldsErrorCheck (sFieldValueArray, 15,
                                         ErrorHandler.ERROR_PROJPARAMS_FIELD);
               } catch (ReadException e) { throw e; }

               for (int i = 0; i < 15; i++)
                  setInputProjectionParameter(i, sFieldValueArray[i]);
            }
            // GRING_LATLON situation
            else if (s.toUpperCase().compareTo("UL_GRING_LATLON") == 0) {
               // projection parameters
               //optional
               sArray = read2dPoint(inputTok,
                                    ErrorHandler.ERROR_READ_GRINGS, true);
               try {
                  fieldsErrorCheck (sArray, sArray.length,
                                    ErrorHandler.ERROR_READ_GRINGS);
               } catch (ReadException e)  {  throw e;  }
               setGringPoint(UL,sArray);
               bGringExist1 = true;
            } else if (s.toUpperCase().compareTo("UR_GRING_LATLON") == 0) {
               // projection parameters
               sArray = read2dPoint(inputTok,
                                    ErrorHandler.ERROR_READ_GRINGS, true);
               try {
                  fieldsErrorCheck (sArray, sArray.length,
                                    ErrorHandler.ERROR_READ_GRINGS);
               } catch (ReadException e)  {  throw e;  }

               setGringPoint(UR,sArray);
               bGringExist2 = true;
            } else if (s.toUpperCase().compareTo("LL_GRING_LATLON") == 0) {
               // projection parameters
               sArray = read2dPoint(inputTok,
                                    ErrorHandler.ERROR_READ_GRINGS, true);
               try {
                  fieldsErrorCheck (sArray, sArray.length,
                                    ErrorHandler.ERROR_READ_GRINGS);
               } catch (ReadException e)  {  throw e;  }
               
               setGringPoint(LL,sArray);
               bGringExist3 = true;
            } else if (s.toUpperCase().compareTo("LR_GRING_LATLON") == 0) {
               // projection parameters
               sArray = read2dPoint(inputTok,
                                    ErrorHandler.ERROR_READ_GRINGS, true);
               try {
                  fieldsErrorCheck (sArray, sArray.length,
                                    ErrorHandler.ERROR_READ_GRINGS);
               }
               catch (ReadException e)  {  throw e;  }
            
               setGringPoint(LR,sArray);
               bGringExist4 = true;
            } else if (s.toUpperCase().compareTo("UL_CORNER_LATLON") == 0) {
               // CORNER_LATLON
               // projection parameters

               sArray = read2dPoint(inputTok, ErrorHandler.ERROR_READ_CORNERS);
               sULArray = sArray;
               try {
                  fieldsErrorCheck (sArray, sArray.length,
                                    ErrorHandler.ERROR_READ_CORNERS);
               } catch (ReadException e) {  throw e;  }
               setInputImageExtent(UL,sArray);
               setCornerPoint(UL,sArray);
            } else if (s.toUpperCase().compareTo("UR_CORNER_LATLON") == 0) {
               // projection parameters
               sArray = read2dPoint(inputTok, ErrorHandler.ERROR_READ_CORNERS);
               try {
                  fieldsErrorCheck (sArray, sArray.length,
                                    ErrorHandler.ERROR_READ_CORNERS);
               } catch (ReadException e) {  throw e;  }
               setInputImageExtent(UR,sArray);
               setCornerPoint(UR,sArray);
            } else if (s.toUpperCase().compareTo("LL_CORNER_LATLON") == 0) {
               // projection parameters
               sArray = read2dPoint(inputTok, ErrorHandler.ERROR_READ_CORNERS);
               try {
                  fieldsErrorCheck (sArray, sArray.length,
                                    ErrorHandler.ERROR_READ_CORNERS);
               } catch (ReadException e) {  throw e;  }

               setInputImageExtent(LL,sArray);
               setCornerPoint(LL,sArray);
            }
            else if (s.toUpperCase().compareTo("LR_CORNER_LATLON") == 0) {
               // projection parameters
               sArray = read2dPoint(inputTok, ErrorHandler.ERROR_READ_CORNERS);
               sLRArray = sArray;
               try {
                  fieldsErrorCheck (sArray, sArray.length,
                                    ErrorHandler.ERROR_READ_CORNERS);
               } catch (ReadException e) {  throw e;  }
               setInputImageExtent(LR,sArray);
               setCornerPoint(LR,sArray);
            } else if (s.toUpperCase().compareTo("UL_CORNER_XY") == 0) {
               // projection parameters
               // CORNER_XY
               sArray = read2dPoint(inputTok,
                                    ErrorHandler.ERROR_READ_CORNERS, true);
               try {
                  fieldsErrorCheck (sArray, sArray.length,
                                    ErrorHandler.ERROR_READ_CORNERS);
               } catch (ReadException e) {  throw e;  }

               //setInputImageExtent(UL,sArray);
               setCornerXYPoint(UL, sArray);
               bCornerXYExist1 = true;
            } else if (s.toUpperCase().compareTo("UR_CORNER_XY") == 0) {
               // projection parameters
               sArray = read2dPoint(inputTok,
                                    ErrorHandler.ERROR_READ_CORNERS, true);
               try {
                  fieldsErrorCheck (sArray, sArray.length,
                                    ErrorHandler.ERROR_READ_CORNERS);
               }
               catch (ReadException e) {  throw e;  }
               
               setCornerXYPoint(UR,sArray);
               bCornerXYExist2 = true;
            } else if (s.toUpperCase().compareTo("LL_CORNER_XY") == 0) {
               // projection parameters
               sArray = read2dPoint(inputTok,
                                    ErrorHandler.ERROR_READ_CORNERS, true);
               try {
                  fieldsErrorCheck (sArray, sArray.length,
                                    ErrorHandler.ERROR_READ_CORNERS);
               } catch (ReadException e) {  throw e;  }
            
               setCornerXYPoint(LL,sArray);
               bCornerXYExist3 = true;
            } else if (s.toUpperCase().compareTo("LR_CORNER_XY") == 0) {
               // projection parameters
               sArray = read2dPoint(inputTok,
                                    ErrorHandler.ERROR_READ_CORNERS, true);
               try {
                  fieldsErrorCheck (sArray, sArray.length,
                                    ErrorHandler.ERROR_READ_CORNERS);
               } catch (ReadException e) {  throw e;  }
            
               setCornerXYPoint(LR,sArray);
               bCornerXYExist4 = true;
            } else if (s.toUpperCase().compareTo("NBANDS") == 0) {
               // already read
               readNextNonCommentToken(inputTok);
               // Just skip next token
            } else if (s.toUpperCase().compareTo("BANDNAMES") == 0) {
               // projection parameters
               inputBandNamesArray = parseFieldValue ( inputTok, getNumBands(),
                                              ErrorHandler.ERROR_NBANDS_FIELD);
            } else if (s.toUpperCase().compareTo("NLINES") == 0) {
               // projection parameters
               sFieldValueArray = parseFieldValue ( inputTok,
                                getNumBands(), ErrorHandler.ERROR_NLINES_VALUE);
               inputNumOfLinesArray = new int [getNumBands()];
               for (int i = 0; i < getNumBands(); i++)
                  inputNumOfLinesArray[i] =
                                          Integer.parseInt(sFieldValueArray[i]);
            } else if (s.toUpperCase().compareTo("NSAMPLES") == 0) {
               // projection parameters
               inputNumOfSamplesArray = new int [getNumBands()];
               sFieldValueArray = parseFieldValue ( inputTok, getNumBands(),
                                           ErrorHandler.ERROR_NSAMPLES_VALUE);
               for (int i = 0; i < getNumBands(); i++)
                  inputNumOfSamplesArray[i] = 
                                          Integer.parseInt(sFieldValueArray[i]);
            } else if (s.toUpperCase().compareTo("PIXEL_SIZE") == 0) {
               // projection parameters
               inputPixelSizeArray = new double [getNumBands()];
               sFieldValueArray = parseFieldValue ( inputTok,
                            getNumBands(), ErrorHandler.ERROR_PIXELSIZE_FIELD);
               for (int i = 0; i < getNumBands(); i++)
                  inputPixelSizeArray[i] =
                               Float.valueOf(sFieldValueArray[i]).floatValue();
            } else if (s.toUpperCase().compareTo("DATA_TYPE") == 0) {
               // projection parameters
               inputDataTypeArray = parseFieldValue ( inputTok, getNumBands(),
                                                 ErrorHandler.ERROR_DATA_TYPE);
            } else if (s.toUpperCase().compareTo("MIN_VALUE") == 0) {
               // projection parameters
               inputMinValueArray = new double [getNumBands()];
               sFieldValueArray = parseFieldValue ( inputTok, getNumBands(),
                                                 ErrorHandler.ERROR_MIN_VALUE);
               for (int i = 0; i < getNumBands(); i++)
                  inputMinValueArray[i] =
                             Double.valueOf(sFieldValueArray[i]).doubleValue();
            } else if (s.toUpperCase().compareTo("MAX_VALUE") == 0) {
               // projection parameters
               inputMaxValueArray = new double [getNumBands()];
               sFieldValueArray = parseFieldValue ( inputTok, getNumBands(),
                                                  ErrorHandler.ERROR_MAX_VALUE);
               for (int i = 0; i < getNumBands(); i++)
                  inputMaxValueArray[i] =
                              Double.valueOf(sFieldValueArray[i]).doubleValue();
            } else if (s.toUpperCase().compareTo("BACKGROUND_FILL") == 0) {
               // projection parameters
               sFieldValueArray = parseFieldValue ( inputTok, getNumBands(),
                                           ErrorHandler.ERROR_BACKGROUND_FILL);
               inputBackgroundFillArray = new double [getNumBands()];
               for (int i = 0; i < getNumBands(); i++)
                  inputBackgroundFillArray[i] =
                             Double.valueOf(sFieldValueArray[i]).doubleValue();
            }
         } catch (IOException e) {
            throw new ReadException(ErrorHandler.ERROR_GENERAL);
         } catch (IllegalArgumentException e) {
            throw new ReadException(ErrorHandler.ERROR_GENERAL);
         }
      }  // end of while loop
      try {
         fileReader.close();
      } catch (IOException e) {}
   }


   /***************************************************************************
   NAME:       readHeaderNumBands
   PURPOSE:  Reads a Header file (.hdr) and looks specifically for the
              NUMBANDS property.  Once it finds that property, it reads the
              corresponding value and sets the numBands property of the
              descriptor.
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void readHeaderNumBands() throws ReadException {
      Reader fileReader;
      StreamTokenizer inputTok;
      String s;
      int tokenType;
      try   {
         fileReader = new FileReader(getInputFilename());
      } catch (IOException e) {
         throw new ReadException(ErrorHandler.ERROR_OPEN_INPUTHEADER);
      }

      inputTok = initTokenRead (fileReader);

      READ_LOOP: while (true) {      // not end of file
         try {
            tokenType = readNextNonCommentToken(inputTok);
            if (tokenType == StreamTokenizer.TT_EOF)
               break READ_LOOP;
            else if ( tokenType != StreamTokenizer.TT_WORD )
               throw new ReadException(ErrorHandler.ERROR_GENERAL);

            s = inputTok.sval;

            // determine total number of bands
            if (s.toUpperCase().compareTo("NBANDS") == 0) {
               s = readHeaderProperty( inputTok,
                                       ErrorHandler.ERROR_TOTALBANDS_VALUE);
               try {
                  setNumBands(s);
               } catch (IllegalArgumentException e) {
                  throw new ReadException(ErrorHandler.ERROR_TOTALBANDS_FIELD);
               }
               if ( getNumBands() < 1 )
                  throw new ReadException(ErrorHandler.ERROR_TOTALBANDS_VALUE);
            }
         } catch (IOException e) {
            throw new ReadException(ErrorHandler.ERROR_GENERAL);
         } catch (IllegalArgumentException e) {
            throw new ReadException(ErrorHandler.ERROR_GENERAL);
         }
      }  // end of while loop
      try {
         fileReader.close();
      } catch (IOException e) {}
   }
   
      
   /***************************************************************************
   NAME:       readDatum
   PURPOSE:  Reads a Header file (.hdr) and looks specifically for the
              datum property.  Once it finds that property, it reads the
              corresponding value and sets the datum property of the
              descriptor.
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void readDatum() throws ReadException {
      Reader fileReader;
      StreamTokenizer inputTok;
      String s;
      int tokenType;
      try   {
         fileReader = new FileReader(getInputFilename());
      } catch (IOException e) {
         throw new ReadException(ErrorHandler.ERROR_OPEN_INPUTHEADER);
      }

      inputTok = initTokenRead (fileReader);

      READ_LOOP: while (true) {      // not end of file
         try {
            tokenType = readNextNonCommentToken(inputTok);
            if (tokenType == StreamTokenizer.TT_EOF)
               break READ_LOOP;
            else if ( tokenType != StreamTokenizer.TT_WORD )
               throw new ReadException(ErrorHandler.ERROR_GENERAL);

            s = inputTok.sval;

            // determine total number of bands
            if (s.toUpperCase().compareTo("DATUM") == 0) {
                DatumOption = true;
               s = readHeaderProperty( inputTok, ErrorHandler.ERROR_GENERAL);
               try {
                  setInputDatum(s);
                  
               } catch (IllegalArgumentException e) {
                  throw new ReadException(ErrorHandler.ERROR_GENERAL);
               }
            }
         } catch (IOException e) {
            throw new ReadException(ErrorHandler.ERROR_GENERAL);
         } catch (IllegalArgumentException e) {
            throw new ReadException(ErrorHandler.ERROR_GENERAL);
         }
      }  // end of while loop
      try {
         fileReader.close();
      } catch (IOException e) {}
   }
   
   /***************************************************************************
   NAME:      check
   PURPOSE:
            checks whether the datum is present in the input file
   RETURN VALUE:
            Vector
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public boolean check() {
      return  DatumOption;
   }
    
   /***************************************************************************
   NAME:      uncheck
   PURPOSE:
            checks whether the datum code is present in the input file
   RETURN VALUE:
            Vector
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void uncheck() {
      DatumOption = false;
   } 

   /**************************************************************************
   NAME:      check1
   PURPOSE:
            checks whether the UTMZone is present in the input file
   RETURN VALUE:
            Vector
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public boolean check1() {
      return  UtmOption;
   }
   
   /***************************************************************************
   NAME:      check1
   PURPOSE:
            checks whether the UTMZone is present in the input file
   RETURN VALUE:
            Vector
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void uncheck1() {
      UtmOption = false;
   }
      
   /***************************************************************************
   NAME:       readUtmZone
   PURPOSE:  Reads a Header file (.hdr) and looks specifically for the
              UTM zone property.  Once it finds that property, it reads the
              corresponding value and sets the utm zone code property of the
              descriptor.
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void readUtmZone() throws ReadException {
      Reader fileReader;
      StreamTokenizer inputTok;
      String s;
      int tokenType;
      try   {
         fileReader = new FileReader(getInputFilename());
      } catch (IOException e) {
         throw new ReadException(ErrorHandler.ERROR_OPEN_INPUTHEADER);
      }

      inputTok = initTokenRead (fileReader);

      READ_LOOP: while (true) {      // not end of file
         try {
            tokenType = readNextNonCommentToken(inputTok);
            if (tokenType == StreamTokenizer.TT_EOF)
               break READ_LOOP;
            else if ( tokenType != StreamTokenizer.TT_WORD )
               throw new ReadException(ErrorHandler.ERROR_GENERAL);

            s = inputTok.sval;

            // determine total number of bands
            if (s.toUpperCase().compareTo("UTM_ZONE") == 0) {
                UtmOption = true;
               s = readHeaderProperty( inputTok, ErrorHandler.ERROR_GENERAL);
               try {
                  setInputUTMZone(s);
               } catch (IllegalArgumentException e) {
                  throw new ReadException(ErrorHandler.ERROR_GENERAL);
               }
            }
         } catch (IOException e) {
            throw new ReadException(ErrorHandler.ERROR_GENERAL);
         } catch (IllegalArgumentException e) {
            throw new ReadException(ErrorHandler.ERROR_GENERAL);
         }
      }  // end of while loop
      try {
         fileReader.close();
      } catch (IOException e) {}
   }
   
   /***************************************************************************
   NAME:       readParameterFile
   PURPOSE:  Reads a Parameter file (.prm) and populates
             corresponding data fields in the current object.
 
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void readParameterFile(boolean readheader) throws ReadException {

      Reader fileReader;
      StreamTokenizer inputTok;
      String str,s1,s2,s3,s4;
      String s;
      String[] sArray;
      String[] sFieldValueArray;
      boolean dotFound = false;
      boolean pointRead = false;
      int tokenType;
      short totalNumOfRequiredFields = 0;

      boolean bOutputPixelSizeFieldNotExist = true;

      // initialize mosaic to false (0)
      mosaic = 0;

      try   {
         fileReader = new FileReader(getParameterFilename());
      } catch (IOException e) {
         throw new ReadException(ErrorHandler.ERROR_OPEN_INPUTPAR);
      }
      inputTok = initTokenRead (fileReader);

      boolean subsetOption = false;
      boolean projParamOption = false;
      boolean spatialsubsetTypeOption = false;
      boolean ULCornerOption = false;
      boolean LRCornerOption = false;
      boolean resampleTypeOption = false;
      boolean UtmZoneOption = false;
      boolean requireInputFileName = false;
      boolean requireOutputPrjType = false;
      boolean requireOutputFileName = false;
      boolean fieldInputFileName =  true;

      while (true) {       // not end of file
         try {
            tokenType = inputTok.nextToken();
            if (tokenType == StreamTokenizer.TT_EOF) {
               break;   //break;
            } else if ( tokenType != StreamTokenizer.TT_WORD ) {
               // throw new ReadException(ErrorHandler.ERROR_PROJPARAMS_FIELD);
            }
            s = inputTok.sval;
            if (s.toUpperCase().compareTo("INPUT_FILENAMES") == 0) {
               //  read and set projection type

               mosaic = 1;
               requireInputFileName = true;
               fieldInputFileName = false;
               s = readInputFilesFromTmpParam(inputTok, readheader);
               inputTok.pushBack();

               // return the field value
               totalNumOfRequiredFields++;
               setInputFilename(s);

               s = s.toLowerCase();
               if ( s.endsWith(".hdr")) {
                  setInputFileType(MULTIFILE);
               } else if (s.endsWith(".hdf")) {
                  setInputFileType(HDFEOS);
               } else {
                  setInputFileType(BAD_FILE_TYPE);
                  throw new ReadException(ErrorHandler.ERROR_INPUT_EXTENSION);
               }
               if (readheader) {
                  try {
                     switch (getInputFileType()) {
                        case MULTIFILE:
                           readHeaderFile();
                           break;
                        case HDFEOS:
                           readHdfHeader();
                           break;
                        default:
                           throw new ReadException(
                                           ErrorHandler.ERROR_INPUT_EXTENSION);
                     }
                  } catch (ReadException e) {
                     if ( e.getErrorId() != ErrorHandler.ERROR_INPUT_EXTENSION )
                         throw e;
                  }
               }
            } else if ((s.toUpperCase().compareTo("INPUT_FILENAME") == 0) && 
                        fieldInputFileName) {

               requireInputFileName = true;
               fieldInputFileName = false;
               // return the field value
               s = readHeaderProperty(inputTok,
                                      ErrorHandler.ERROR_INPUTNAME_FIELD);
               totalNumOfRequiredFields++;
               setInputFilename(s);

               s = s.toLowerCase();                             

               if ( s.endsWith(".hdr")) {
                     setInputFileType(MULTIFILE);
               } else if (s.endsWith(".hdf")) {
                  setInputFileType(HDFEOS);                                
               } else {
                  setInputFileType(BAD_FILE_TYPE);
                  throw new ReadException(ErrorHandler.ERROR_INPUT_EXTENSION);
               }

               if (readheader) {
                  try {
                     switch (getInputFileType()) {
                        case MULTIFILE:
                           readHeaderFile();
                           break;
                        case HDFEOS:
                           readHdfHeader();
                           break;
                        default:
                           throw new ReadException(
                                           ErrorHandler.ERROR_INPUT_EXTENSION);
                     }
                  } catch (ReadException e) {
                     if( e.getErrorId() != ErrorHandler.ERROR_INPUT_EXTENSION )
                        //throw new ReadException(
                        //                  ErrorHandler.ERROR_READ_INPUTPAR);
                           throw e;
                  }
               }
            } else if (s.toUpperCase().compareTo("SPECTRAL_SUBSET") == 0) {
               // optional selection
               // projection parameters
               
               subsetOption = true;
               sFieldValueArray = parseDefaultFieldValue ( inputTok,
                     getNumBands(), "0", ErrorHandler.ERROR_SPECTRALSUB_FIELD);
               try {
                  fieldsErrorCheck (sFieldValueArray, sFieldValueArray.length,
                                    ErrorHandler.ERROR_SPECTRALSUB_FIELD);
               } catch (ReadException e) {  throw e;  }
               for (int i = 0; i < getNumBands(); i++) {
                  try {
                    int value = Integer.parseInt(sFieldValueArray[i]);
                                                 
                    if ( value == 1 )
                      setSelectedBand(i,true);
                    else if (value == 0)
                      setSelectedBand(i,false);
                    else
                      throw new ReadException(
                                         ErrorHandler.ERROR_SPECTRALSUB_FIELD);
                  } catch (NumberFormatException e) {
                     throw new ReadException(
                                         ErrorHandler.ERROR_SPECTRALSUB_FIELD);
                  }
               }
            } else if (s.toUpperCase().compareTo("SPATIAL_SUBSET_TYPE") == 0) {
               spatialsubsetTypeOption = true;
               s = readHeaderProperty(inputTok,
                                         ErrorHandler.ERROR_SPATIALSUBSET_TYPE);
               // return the field value
               try {
                  setSpatialSubsetType(s.toUpperCase());
               } catch (IllegalArgumentException e) {
                  throw new ReadException(
                                         ErrorHandler.ERROR_SPATIALSUBSET_TYPE);
               }
            } else if(
                   s.toUpperCase().compareTo("SPATIAL_SUBSET_UL_CORNER") == 0) {
               /*
                * determine spatial subsetting:
                * SPATIAL_SUBSET_UL_CORNER = ( lat long )      < float values >
                * SPATIAL_SUBSET_LR_CORNER = ( lat long )
                *              - OR -
                * SPATIAL_SUBSET_UL_CORNER = ( line sample )   < int values >
                * SPATIAL_SUBSET_LR_CORNER = ( line sample )
                *              - OR -
                * SPATIAL_SUBSET_UL_CORNER = ( proj_x proj_y ) < float values >
                * SPATIAL_SUBSET_LR_CORNER = ( proj_x proj_y )
                */
               ULCornerOption = true;
               sArray = read2dPoint(inputTok,
                                    ErrorHandler.ERROR_SPATIALSUB_FIELD, true);
               try {
                  fieldsErrorCheck (sArray, sArray.length,
                                    ErrorHandler.ERROR_SPATIALSUB_FIELD);
               } catch (ReadException e) {  throw e;  }

               if (sArray != null) {
                  pointRead = true;
                  dotFound = sArray[0].indexOf('.') >= 0 ||
                             sArray[1].indexOf('.') >= 0;
                  try {
                     setCornerPoint(UL, sArray);
                  } catch (IllegalArgumentException e) {
                      throw new ReadException(
                                          ErrorHandler.ERROR_SPATIALSUB_FIELD);
                  }
               }
            } else if(
                  s.toUpperCase().compareTo("SPATIAL_SUBSET_LR_CORNER") == 0) {

               LRCornerOption = true;
               sArray = read2dPoint(inputTok,
                                    ErrorHandler.ERROR_SPATIALSUB_FIELD, true);
               try {
                  fieldsErrorCheck (sArray, sArray.length,
                                    ErrorHandler.ERROR_SPATIALSUB_FIELD);
               } catch (ReadException e) {  throw e;  }

               if (sArray != null) {
                  pointRead = true;
                  dotFound = dotFound || sArray[0].indexOf('.') >= 0 ||
                             sArray[1].indexOf('.') >= 0;

                  // store the point value
                  try {
                     setCornerPoint(LR,sArray);
                  } catch (IllegalArgumentException e) {
                     throw new ReadException(
                                         ErrorHandler.ERROR_SPATIALSUB_FIELD);
                  }
               }
            } else if (s.toUpperCase().compareTo("OUTPUT_FILENAME") == 0) {
               /*
                * Read the output filename
                */

               requireOutputFileName = true;
               // return the field value
               s = readHeaderProperty(inputTok,
                                      ErrorHandler.ERROR_OUTPUTNAME_FIELD);
               totalNumOfRequiredFields++;
               if ( getOutputFilename() == null )
                  setOutputFilename( s );

               {
                  s = s.toLowerCase();
                  if ( s.endsWith(".hdr") ) {
                     setOutputFileType(MULTIFILE);
                  } else if ( s.endsWith(".hdf") ) {
                     setOutputFileType(HDFEOS);
                  } else if ( s.endsWith(".tif") ) {
                     setOutputFileType(GEOTIFF);
                  } else
                     throw new ReadException(
                                         ErrorHandler.ERROR_OUTPUT_EXTENSION);
               }
            } else if (s.toUpperCase().compareTo("RESAMPLING_TYPE") == 0) {
               // Read the resampling type
               resampleTypeOption = true;
               // return the field value
               s = readHeaderProperty(inputTok,
                                      ErrorHandler.ERROR_RESAMPLE_FIELD);
               if ( getResamplingType() == BAD_RESAMPLING_TYPE ) {
                  try {
                     setResamplingType(s.toUpperCase());
                  } catch (IllegalArgumentException e) {
                     throw new ReadException(ErrorHandler.ERROR_RESAMPLE_TYPE);
                  }
               }
            } else if(
                    s.toUpperCase().compareTo("OUTPUT_PROJECTION_TYPE") == 0) {

               // Read the output projection type
               requireOutputPrjType = true;
               totalNumOfRequiredFields++;
               // return the field value
               s = readHeaderProperty(inputTok,
                                      ErrorHandler.ERROR_PROJECTION_TYPE);
                         
               {
                  try {
                     setOutputProjectionType(s.toUpperCase());
                   } catch (IllegalArgumentException e) {
                     throw new ReadException(
                                           ErrorHandler.ERROR_PROJECTION_TYPE);
                  }
               }
            } else if(
               s.toUpperCase().compareTo("OUTPUT_PROJECTION_PARAMETERS") == 0) {

               // read projection parameters
               // projection parameters
               projParamOption = true;
               sFieldValueArray = parseDefaultFieldValue ( inputTok, 15,
                                  "0.0", ErrorHandler.ERROR_PROJPARAMS_FIELD);
               try {
                  fieldsErrorCheck (sFieldValueArray, 15,
                                    ErrorHandler.ERROR_PROJPARAMS_FIELD);
               } catch (ReadException e)  {  throw e;  }
               for (int i=0; i<15; i++)
                  setOutputProjectionParameter(i, sFieldValueArray[i]);

            } else if (s.toUpperCase().compareTo("OUTPUT_PIXEL_SIZE") == 0) {
               // added by sj 1/27/01
               // Read the output pixel size--this value is optional
               // projection parameters
               try {
                  sArray = readPixelSizeAndUnit(inputTok,
                              ErrorHandler.ERROR_OUTPUTPIXELSIZE_FIELD, true);
               } catch (ReadException e) {  throw e; }
               if ( sArray != null ) {
                  setOutputPixelSize( sArray[0] );
                  bOutputPixelSizeFieldNotExist = false;
                   if (sArray[1] != " ")
                       setOutputPixelSizeUnit( sArray[1].toUpperCase() );
               }
            } else if (s.toUpperCase().compareTo("UTM_ZONE") == 0) {
               // optional
               s = readHeaderProperty(inputTok, ErrorHandler.ERROR_UTM_ZONE);
               if ( s.length()>0)
                  setUTMZone(Integer.parseInt(s));
            } else if (s.toUpperCase().compareTo("DATUM") == 0) {
               s = readHeaderProperty(inputTok, ErrorHandler.ERROR_DATUM);
               if ( s.length()>0)
                  setDatum(s);
            } else {
               // throw exception error here
            }

         } catch (IOException e) {
            // do nothing
         } catch (IllegalArgumentException e) {
            throw new ReadException(ErrorHandler.ERROR_DATA_TYPE);
         }
      }  // end of while loop

      /* Handle the UL and LR corners */
      if ( ULCornerOption && LRCornerOption && spatialsubsetTypeOption ) {
         if ( getSpatialSubsetType() == SPACE_LAT_LON ||
              getSpatialSubsetType() == SPACE_PROJ_XY ) {
            double[] pt = new double[2];
            pt[0] = 0.0;
            pt[1] = 0.0;
            setCornerPoint(UR,pt);
            setCornerPoint(LL,pt);
         } else {
            double[] ulPt = getCornerPoint(UL);
            double[] lrPt = getCornerPoint(LR);
            double[] urPt = { ulPt[0], lrPt[1] };
            double[] llPt = { lrPt[0], ulPt[1] };

            setCornerPoint(UR,urPt);
            setCornerPoint(LL,llPt);
         }

         /*
          * determine spatial subsetting
          */
         setInputCornerPoint(UL,0,0);
         setInputCornerPoint(UR,0,getBandInfo(0).getNumSamples() - 1);
         setInputCornerPoint(LL,getBandInfo(0).getNumLines()-1,0);
         setInputCornerPoint(LR, getBandInfo(0).getNumLines()-1,
                                 getBandInfo(0).getNumSamples()-1);

         /*
          * Set corner points for bands
         */
         for (int i=0; i < getNumBands(); ++i) {
            final double[] outCornerPt = {0.0, 0.0};
            final BandType band = getBandInfo(i);

            for (int j=0; j < 4; ++j) {
               final int[] pt = getInputCornerPoint(j);
               final int[] newPt = new int[2];
               for (int k=0; k < 2; ++k)
                  newPt[k] = pt[k];
               band.setInputCornerPoint(j,newPt);
               band.setOutputCornerPoint(j,outCornerPt);
            }
         }
      }

      if ( requireInputFileName == false)
          throw new ReadException(ErrorHandler.ERROR_MISSING_INPUT_FILE_NAME);
      if ( requireOutputFileName == false)
          throw new ReadException(ErrorHandler.ERROR_MISSING_OUTPUT_FILE_NAME);
      if ( requireOutputPrjType == false)
          throw new ReadException(ErrorHandler.ERROR_MISSING_OUTPUT_PROJ_TYPE);
      if ( (ULCornerOption && LRCornerOption) && !spatialsubsetTypeOption )
          throw new ReadException(
                               ErrorHandler.ERROR_MISSING_SPATIAL_SUBSET_TYPE);

      // not exist in the .prm
      if (subsetOption == false) {
         for (int i = 0; i < getNumBands(); i++)
             setSelectedBand(i,true);
      }
      if (projParamOption == false) {
         for (int i=0; i<15; i++)
            setOutputProjectionParameter(i, "0.0");
      }
      if (! resampleTypeOption )
         setResamplingType("NEAREST_NEIGHBOR");

      if (! ULCornerOption) {
         setCornerPoint(UL, sULArray);
         setSpatialSubsetType("INPUT_LAT_LONG");
      }
      if (! LRCornerOption) {
         setCornerPoint(LR, sLRArray);
         setSpatialSubsetType("INPUT_LAT_LONG");
      }

      // Done. Close the reader and go home.
      try {
         fileReader.close();
      } catch (IOException e) {}
   }



   /***************************************************************************
   NAME:       readHeaderProperty
   PURPOSE:
            To read header property
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   private String readHeaderProperty(StreamTokenizer tok, String propName,
                                     int errorId)  throws ReadException {
      return readHeaderProperty(tok,propName,errorId,false);
   }

   /***************************************************************************
   NAME:       readNextNonCommentToken
   PURPOSE:
             To parse and skip comment lines
   RETURN VALUE:            integer
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   private int readNextNonCommentToken(StreamTokenizer tok) throws IOException {
      int tokenType = tok.nextToken();
      while ( tokenType == StreamTokenizer.TT_WORD &&
              tok.sval.charAt(0) == '#' ) {
         tokenType = consumeComment(tok);
         if ( tokenType != StreamTokenizer.TT_EOF) {
            tokenType = tok.nextToken();
         }
      }
      return tokenType;
   }

   /***************************************************************************
   NAME:       readHeaderProperty
   PURPOSE:  To really read header property            @ overloaded function

   RETURN VALUE:            String
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   private String readHeaderProperty(StreamTokenizer tok, String propName,
                        int errorId,boolean optional)   throws ReadException {
      String s;
      int tokenType;
      try {
         tokenType = readNextNonCommentToken(tok);
         s = tok.sval;
         if ( tokenType != StreamTokenizer.TT_WORD) {
            if (tokenType == StreamTokenizer.TT_EOF && optional)
               return null;
            throw new ReadException(errorId);
         }

         if ( !s.equals(propName) ) {
            if ( !optional )
               throw new ReadException(errorId);
            else {
               tok.pushBack();
               return null;
            }
         }
      
         tokenType = readNextNonCommentToken(tok);
         s = tok.sval;
         if ( tokenType != StreamTokenizer.TT_WORD)
            throw new ReadException(errorId);
      
      } catch (IOException e) {
         throw new ReadException(errorId);
      }
      return s;
   }



   /***************************************************************************
   NAME:       readHeaderProperty
   PURPOSE:
             To read header property
   RETURN VALUE:            String
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   private String readHeaderProperty(StreamTokenizer tok, int errorId)
         throws ReadException {
      return readHeaderProperty(tok, errorId, false);
   }


   /***************************************************************************
   NAME:       readHeaderProperty
   PURPOSE:
             To read header property
             @ overloaded function
   RETURN VALUE:           String
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   private String readHeaderProperty(StreamTokenizer tok, int errorId,
         boolean optional)  throws ReadException {

      String s;
      int tokenType;

      try {
         tokenType = readNextNonCommentToken(tok);
         s = tok.sval;

         if ( tokenType != StreamTokenizer.TT_WORD) {
            if( tok.ttype != '"' ) 
               throw new ReadException(errorId);
         }
      } catch (IOException e) {
         throw new ReadException(errorId);
      }
      return s;
   }
        
   /***************************************************************************
   NAME:       readInputFileNames
   PURPOSE:
             To read header property
             @ overloaded function
   RETURN VALUE:
             String
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   private String readInputFilesFromTmpParam(StreamTokenizer tok,
         boolean readheader)  throws ReadException {
      String s;
      int tokenType;

      try {     
         tokenType = readNextNonCommentToken(tok);
         s = tok.sval;
                   
         if( s.equals("(") ) {
            tokenType = readNextNonCommentToken(tok);                        
         }
         boolean yes = true;
         while(yes){
            temp[array_index] = new File(tok.sval);
            tokenType = readNextNonCommentToken(tok);

            if ((tok.sval).equals (")"))
               yes = false;
            array_index++; 
         }                     
                    
         if(array_index > 1){                       
            img.createMosaicFile(temp, array_index);  // creates TmpMosaic.prm
            s = img.convertAllHdrToTmpHdr();          // creates TmpHdr.hdr
         } else{
            s = temp[0].toString();
         }
      } catch (IOException e) {
         throw new ReadException(ErrorHandler.ERROR_OPEN_INPUTPAR);
      }

      return s;
   }


   public void setNumFiles(int value) { numFiles = value; }


   /**************************************************************************
   NAME:       getParameterFilename
   PURPOSE:
            To get parameter file name
   RETURN VALUE:           String
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public String getParameterFilename() {
      return parameterFilename;
   }


   /***************************************************************************
   NAME:       setParameterFilename
   PURPOSE:
            To set parameter file name
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void setParameterFilename(String value) {
      parameterFilename = value;
   }

   /***************************************************************************
   NAME:       writeHeaderFile
   PURPOSE:
             To write header file to disk
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void writeHeaderFile() throws WriteException {
      String eol = System.getProperty("line.separator");
      FileWriter fw;
      PrintWriter pw;
      StringBuffer buf = new StringBuffer();

      try {
         String outFilename = getOutputFilename();
         if ( !outFilename.toLowerCase().endsWith(".hdr") ) {
            outFilename = outFilename + ".hdr";
         }
         fw = new FileWriter(getOutputFilename());
         pw = new PrintWriter(fw);
         /*
          * projection type
          */
         pw.println();
         pw.print("PROJECTION_TYPE = ");
         try {
            pw.println(getProjectionTypeString(getOutputProjectionType()));
         } catch (IllegalArgumentException e) {
            throw new WriteException( "WriteHdr",
                ErrorHandler.ERROR_PROJECTION_TYPE,
                ErrorHandler.formalMessage[ErrorHandler.ERROR_PROJECTION_TYPE],
                false);
         }
         /*
          * projection parameters
          */
         pw.println();
         pw.print("PROJECTION_PARAMETERS = (");

         for (int i=0; i < 15; ++i ) {
            if ( i%3 == 0 ) {
               pw.println();
            }
            pw.print(" ");
            pw.print(Double.toString(getOutputProjectionParameter(i)));
         }
         pw.println(" )" + eol);

         /*
          * spatial extents
          */
         double[] ulCorner = getCornerPoint(UL);
         pw.print("UL_CORNER_LATLON = ( ");
         if (getSpatialSubsetType() == SPACE_LAT_LON)
            pw.print(Double.toString(ulCorner[0]) + " " +
                     Double.toString(ulCorner[1]));
         else
            pw.print(Integer.toString((int)ulCorner[0]) + " " +
                     Integer.toString((int)ulCorner[1]));
         pw.println(" )" + eol);

         double[] urCorner = getCornerPoint(UR);
         pw.print("UR_CORNER_LATLON = ( ");
         if ( getSpatialSubsetType() == SPACE_LAT_LON)
            pw.print(Double.toString(urCorner[0]) + " " +
                     Double.toString(urCorner[1]));
         else
            pw.print(Integer.toString((int)urCorner[0]) + " " +
                     Integer.toString((int)urCorner[1]));
         pw.println(" )" + eol);

         double[] llCorner = getCornerPoint(LL);
         pw.print("LL_CORNER_LATLON = ( ");
         if (getSpatialSubsetType() == SPACE_LAT_LON)
            pw.print(Double.toString(llCorner[0]) + " " +
                     Double.toString(llCorner[1]));
         else
            pw.print(Integer.toString((int)urCorner[0]) + " " +
                     Integer.toString((int)urCorner[1]));
         pw.println(" )" + eol);

         double[] lrCorner = getCornerPoint(LR);
         pw.print("LR_CORNER_LATLON = ( ");
         if (getSpatialSubsetType() == SPACE_LAT_LON)
            pw.print(Double.toString(lrCorner[0]) + " " +
                     Double.toString(lrCorner[1]));
         else
            pw.print(Integer.toString((int)lrCorner[0]) + " " +
                     Integer.toString((int)lrCorner[1]));
         pw.println(" )" + eol + eol);


         // ***** UL_GRING_LATLON  ********
         if (bGringExist1) {   // means exists
            ulCorner = getGringPoint(UL);
            pw.print("UL_GRING_LATLON = ( ");
            if (getSpatialSubsetType() == SPACE_LAT_LON)
               pw.print(Double.toString(ulCorner[0]) + " " +
                        Double.toString(ulCorner[1]));
            else
               pw.print(Integer.toString((int)ulCorner[0]) + " " +
                        Integer.toString((int)ulCorner[1]));
            pw.println(" )" + eol);
         }
         if (bGringExist2) {   // means exists
            urCorner = getGringPoint(UR);
            pw.print("UR_GRING_LATLON = ( ");
            if ( getSpatialSubsetType() == SPACE_LAT_LON)
               pw.print(Double.toString(urCorner[0]) + " " +
                        Double.toString(urCorner[1]));
            else
               pw.print(Integer.toString((int)urCorner[0]) + " " +
                        Integer.toString((int)urCorner[1]));
            pw.println(" )" + eol);
         }
         if (bGringExist3) {   // means exists
            llCorner = getGringPoint(LL);
            pw.print("LL_GRING_LATLON = ( ");
            if (getSpatialSubsetType() == SPACE_LAT_LON)
               pw.print(Double.toString(llCorner[0]) + " " +
                        Double.toString(llCorner[1]));
            else
               pw.print(Integer.toString((int)urCorner[0]) + " " +
                        Integer.toString((int)urCorner[1]));
            pw.println(" )" + eol);
         }
         if (bGringExist4) {   // means exists
            lrCorner = getGringPoint(LR);
            pw.print("LR_GRING_LATLON = ( ");
            if (getSpatialSubsetType() == SPACE_LAT_LON)
               pw.print(Double.toString(lrCorner[0]) + " " +
                        Double.toString(lrCorner[1]));
            else
               pw.print(Integer.toString((int)lrCorner[0]) + " " +
                        Integer.toString((int)lrCorner[1]));
            pw.println(" )" + eol + eol);
         }


         // ***** UL_CORNERXY_LATLON  ********
         if (bCornerXYExist1)    // means exists
         {
            ulCorner = getCornerXYPoint(UL);
            pw.print("UL_CORNER_XY = ( ");
            if (getSpatialSubsetType() == SPACE_LAT_LON)
               pw.print(Double.toString(ulCorner[0]) + " " +
                        Double.toString(ulCorner[1]));
            else
               pw.print(Integer.toString((int)ulCorner[0]) + " " +
                        Integer.toString((int)ulCorner[1]));
            pw.println(" )" + eol);
         }
         if (bCornerXYExist2) {   // means exists
            urCorner = getCornerXYPoint(UR);
            pw.print("UR_CORNER_XY = ( ");
            if ( getSpatialSubsetType() == SPACE_LAT_LON)
               pw.print(Double.toString(urCorner[0]) + " " +
                        Double.toString(urCorner[1]));
            else
               pw.print(Integer.toString((int)urCorner[0]) + " " +
                        Integer.toString((int)urCorner[1]));
            pw.println(" )" + eol);
         }
         if (bCornerXYExist3) {   // means exists
            llCorner = getCornerXYPoint(LL);
            pw.print("LL_CORNER_XY = ( ");
            if (getSpatialSubsetType() == SPACE_LAT_LON)
               pw.print(Double.toString(llCorner[0]) + " " +
                        Double.toString(llCorner[1]));
            else
               pw.print(Integer.toString((int)urCorner[0]) + " " +
                        Integer.toString((int)urCorner[1]));
            pw.println(" )" + eol);
         }
         if (bCornerXYExist4) {   // means exists
            lrCorner = getCornerXYPoint(LR);
            pw.print("LR_CORNER_XY = ( ");
            if (getSpatialSubsetType() == SPACE_LAT_LON)
               pw.print(Double.toString(lrCorner[0]) + " " +
                        Double.toString(lrCorner[1]));
            else
               pw.print(Integer.toString((int)lrCorner[0]) + " " +
                        Integer.toString((int)lrCorner[1]));
            pw.println(" )" + eol + eol);
         }

         pw.println ("NBANDS = " + getNumBands() + eol);
         pw.println ("BANDNAMES = ( " + getNumBands() +
                                                " )" + eol);
         // more here ...........................

         /*
          * output data type
          */
         pw.print("DATA_TYPE = ");
         try {
            pw.println(getDataTypeString(getOutputDataType()));
         } catch (IllegalArgumentException e) {
            throw new WriteException( "WriteHdr",
               ErrorHandler.ERROR_DATA_TYPE,
               ErrorHandler.formalMessage[
                  ErrorHandler.ERROR_DATA_TYPE],false);
         }
         /*
          * total bands
          */
         int tbands = 0;
         for (int i=0; i<getNumBands(); ++i) {
            if ( isSelectedBand(i) ) {
               ++tbands;
            }
         }
         if ( tbands == 0 ) {
            throw new WriteException( "WriteHdr",
               ErrorHandler.ERROR_NO_BANDS,
               ErrorHandler.formalMessage[
                  ErrorHandler.ERROR_NO_BANDS],false);
         }
         pw.println();
         pw.print("TOTAL_BANDS = ");
         pw.println(Integer.toString(tbands));
         /*
          * number of files
          */
         int numOutFiles = 0;
         int currFile = -1;
         int[] fileNumber = new int[getNumBands()];
         int[] numFileLines = new int[getNumBands()];
         int[] numFileSamples = new int[getNumBands()];
         double[] filePixelSize = new double[getNumBands()];
         int[] numFileBands = new int[getNumBands()];
         for (int i=0; i<getNumBands(); ++i) {
            if ( isSelectedBand(i) ) {
               BandType band = getBandInfo(i);
               if ( band.getFileNumber() != currFile ) {
                  fileNumber[numOutFiles] = band.getFileNumber();
                  numFileLines[numOutFiles] = band.getNumLines();
                  numFileSamples[numOutFiles] = band.getNumSamples();
                  filePixelSize[numOutFiles] = band.getPixelSize();
                  numFileBands[numOutFiles] = 1;

                  ++numOutFiles;
                  currFile = band.getFileNumber();
               } else {
                  ++numFileBands[numOutFiles-1];
               }
            }
         }
         for (int i=0; i<numOutFiles; ++i) {
            pw.println();
            pw.print("BEGIN_GROUP = FILE");
            pw.println(Integer.toString(i+1));
            pw.print("    NLINES = ");
            pw.println(Integer.toString(numFileLines[i]));
            pw.print("    NSAMPLES = ");
            pw.println(Integer.toString(numFileSamples[i]));
            pw.print("    PIXEL_SIZE = ");
            pw.println(Double.toString(filePixelSize[i]));
            pw.print("    NBANDS = ");
            pw.println(Integer.toString(numFileBands[i]));
            pw.print("END_GROUP = FILE");
            pw.println(Integer.toString(i+1));
            pw.println();
         }
         /*
          * that's it!
          */
         pw.close();
         fw.close();

      } catch (IOException e) {
         throw new WriteException("WritePrm",-1,
            "IOException occurred writing parameter file: "+
            getParameterFilename(),false);
      }
   }

   /***************************************************************************
   NAME:       writeconvertParameterFile
   PURPOSE:
            To write a parameter file to disk
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void writeconvertParameterFile( int mosaic ) throws WriteException {
      FileWriter fw;
      PrintWriter pw;
      StringBuffer buf = new StringBuffer();
      String fileName;
      File mosaicfile;

      try {
         fw = new FileWriter(getParameterFilename());
         pw = new PrintWriter(fw);
         /*
          * input filename
          */
                        
         /*
          * input filenames
          */
         
                        
         /**Read TmpMosaic.prm to get the input file names if that file exists*/
         mosaicfile = new File("TmpMosaic.prm");
         if( mosaic == 1 ) {
            if( ! mosaicfile.exists() ) {
               System.out.println("Error: TmpMosaic.prm does not exist");
            }
            pw.println();
            pw.print("#INPUT_FILENAMES = (");
                            
            RandomAccessFile raf;
                         
            try {
               raf = new RandomAccessFile("TmpMosaic.prm", "rw");
                                
               while((fileName = raf.readLine())!= null){
                  pw.print(" ");
                  pw.print(quoteFilename(fileName));
               }
            } catch (FileNotFoundException e) {
               System.out.println("Error: TmpMosaic.prm does not exist");
            } catch(IOException e) {
               System.out.println("Error: TmpMosaic.prm does not exist");
            }
                        
            pw.print(" ");
            pw.print(")");
                         
            /*
             *Mosaic output file name
             */
            pw.println();
            pw.println();
            pw.print("INPUT_FILENAME = " );
            pw.print(quoteFilename(getMosaicFileName()));
            pw.println();                         
         } else{
            pw.println();
            pw.print("INPUT_FILENAME = " );
            pw.print(quoteFilename(getInputFilename()));
            pw.println(); 
         }
                        
         /*
          * write output filename
          */
         pw.println();
         pw.print("OUTPUT_FILENAME = ");
         String outFilename = getOutputFilename().toLowerCase();
         pw.print(quoteFilename(getOutputFilename()));

         switch (getOutputFileType()) {
            case MULTIFILE:
               if ( !outFilename.endsWith(".hdr")) {
                  setOutputFilename(getOutputFilename()+".hdr");
                  pw.print(".hdr");
               }
               break;
            case HDFEOS:
               if ( !outFilename.endsWith(".hdf")) {
                  setOutputFilename(getOutputFilename()+".hdf");
                  pw.print(".hdf");
               }
               break;
            case GEOTIFF:
               if ( !outFilename.endsWith(".tif")) {
                  setOutputFilename(getOutputFilename()+".tif");
                  pw.print(".tif");
               }
               break;
         }
         pw.println();
         
         /*
          * spectral subset
          */
         pw.println();
         pw.print("SPECTRAL_SUBSET = (");
         for (int i=0; i < getNumBands(); ++i) {
            if( mosaic == 1 ) {    /* mosaicking only output the selected
               bands for processing by the resampler */
               if(isSelectedBand(i)){
                  pw.print(" ");
                  pw.print("1");
               }
            } else {     /* not mosaicking then output all bands as 0s or 1s */
               pw.print(" ");
               pw.print(isSelectedBand(i) ? "1" : "0");
           }
        }
        pw.println(" )");
   
         /*
          * spatial subset type
          */
         pw.println();
         pw.print("SPATIAL_SUBSET_TYPE = ");
         try {
            pw.println(getSpatialSubsetTypeString(getSpatialSubsetType()));
         } catch (IllegalArgumentException e) {
            throw new WriteException(
                                      "WritePrm",
                                      -2,
                                      "Error: bad spatial subset type: "+
                                         getSpatialSubsetType(),false);
         }
         /*
          * spatial subset
          */
         pw.println();
         pw.print("SPATIAL_SUBSET_UL_CORNER = ( ");
         double[] ulCorner = getCornerPoint(UL);
         if ( getSpatialSubsetType() == SPACE_LAT_LON ||
              getSpatialSubsetType() == SPACE_PROJ_XY ) {
            pw.print(Double.toString(ulCorner[0]));
            pw.print(" ");
            pw.print(Double.toString(ulCorner[1]));
         } else {
            pw.print(Integer.toString((int)ulCorner[0]));
            pw.print(" ");
            pw.print(Integer.toString((int)ulCorner[1]));
         }
         pw.println(" )");
         pw.print("SPATIAL_SUBSET_LR_CORNER = ( ");
         double[] lrCorner = getCornerPoint(LR);
         if ( getSpatialSubsetType() == SPACE_LAT_LON ||
              getSpatialSubsetType() == SPACE_PROJ_XY ) {
            pw.print(Double.toString(lrCorner[0]));
            pw.print(" ");
            pw.print(Double.toString(lrCorner[1]));
         } else {
            pw.print(Integer.toString((int)lrCorner[0]));
            pw.print(" ");
            pw.print(Integer.toString((int)lrCorner[1]));
         }
         pw.println(" )");
         
         /*
          * output projection type
          */
         pw.println();
         pw.print("OUTPUT_PROJECTION_TYPE = ");
         try {
            pw.println(getInputProjectionTypeString());
            //getProjectionTypeString(
            //getOutputProjectionType()));
         } catch (IllegalArgumentException e) {
            throw new WriteException(
                   "WritePrm",
                   -3,
                   "Error: bad resampling type: "+
                   getOutputProjectionType(),false);
         }
          
 
         /*
          * output projection parameters
          */
         pw.println();
         pw.print("OUTPUT_PROJECTION_PARAMETERS = ( ");
         
         for (int i=0; i<15; ++i) {
            if ( i % 3 == 0 ) {
               pw.println();
            }
            
            pw.print(" ");
            String value = Double.toString(getInputProjectionParameter(i));
            if (value.endsWith(".0")) {
               value = value.substring(0,value.length()-2);
            }
            pw.print(value);
         }
         
         pw.println(" )");

         // if ( outputPixelSize != null && outputPixelSize.length() > 0)

         // that's it!
         pw.close();
         fw.close();

      } catch (IOException e) {
         throw new WriteException("WritePrm",-1,
            "IOException occurred writing parameter file: "+
            getParameterFilename(),false);
      }
   }


   /***************************************************************************
   NAME:       writeParameterFile
   PURPOSE:
            To write a parameter file to disk
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void writeParameterFile( int mosaic ) throws WriteException {
      FileWriter fw;
      PrintWriter pw;
      StringBuffer buf = new StringBuffer();
      String fileName;
      File mosaicfile;

      try {
         fw = new FileWriter(getParameterFilename());
         pw = new PrintWriter(fw);
         /*
          * input filenames
          */
         
                        
         /**Read TmpMosaic.prm to get the input file names if that file exists*/
         mosaicfile = new File("TmpMosaic.prm");
         if( mosaic == 1 ) {
            if( !mosaicfile.exists() ) {
               System.out.println("Error: TmpMosaic.prm does not exist");
            }
            pw.println();
            pw.print("#The "+ "\"INPUT_FILENAMES\""+
                " field would be commented. If you want to load " +
                "multiple input files please uncomment the " +
                "\"INPUT_FILENAMES\""+
                " field and comment the"+ "\"INPUT_FILENAME\""+" field. ");
            pw.println();
            pw.println("#Also the "+ "\"ORIG_SPECTRAL_SUBSET\"" +
                " field needs to be uncommented and changed to "+
                "\"SPECTRAL_SUBSET\"" + ". The initial "+
                "\"SPECTRAL_SUBSET\"" + " field should be deleted.");
            pw.println();
            pw.println();
            pw.print("#INPUT_FILENAMES = (");
                            
            RandomAccessFile raf;
                         
            try {
               raf = new RandomAccessFile("TmpMosaic.prm", "rw");
                                
               while((fileName = raf.readLine())!= null){
                  /* The file name may be enclosed in quotes. */
                  pw.print(" ");
                  pw.print(fileName);
               }
            } catch (FileNotFoundException e) {
               System.out.println("Error: TmpMosaic.prm does not exist");
            } catch(IOException e) {
               System.out.println("Error: TmpMosaic.prm does not exist");
            }
                        
            pw.print(" ");
            pw.print(")");
                         
            /*
             *Mosaic output file name
             */
            pw.println();
            pw.println();
            pw.print("INPUT_FILENAME = " );
            pw.print(quoteFilename(getMosaicFileName()));
            pw.println();                         
            
         } else{  /* not mosaicking */
            pw.println();
            pw.print("INPUT_FILENAME = " );
            pw.print(quoteFilename(getInputFilename()));
            pw.println(); 
         }
                       
         
         /*
          * spectral subset
          */
         pw.println();
         pw.print("SPECTRAL_SUBSET = (");
         for (int i=0; i < getNumBands(); ++i) {           
            if ( mosaic == 1 ) { /* mosaicking only output the selected
                                    bands for processing by the resampler */
               if(isSelectedBand(i)){
                  pw.print(" ");            
                  pw.print("1");
               }
            } else { /* not mosaicking then output all bands as 0s or 1s */
               pw.print(" ");
               pw.print(isSelectedBand(i) ? "1" : "0");
            }
         }
         pw.println(" )");

         /*
          * original spectral subset (for mosaicking only)
          */
         if ( mosaic == 1 ) {
             pw.print("#ORIG_SPECTRAL_SUBSET = (");
             for (int i=0; i < getNumBands(); ++i) {
                 /* output all bands as 0s or 1s */
                 pw.print(" ");
                 pw.print(isSelectedBand(i) ? "1" : "0");
             }
             pw.println(" )");
         }

         /*
          * spatial subset type
          */
         pw.println();
         pw.print("SPATIAL_SUBSET_TYPE = ");
         try {
            pw.println(getSpatialSubsetTypeString(getSpatialSubsetType()));
         } catch (IllegalArgumentException e) {
            throw new WriteException(
                         "WritePrm",
                         -2,
                         "Error: bad spatial subset type: "+
                         getSpatialSubsetType(),false);
         }
         /*
          * spatial subset
          */
         pw.println();
         pw.print("SPATIAL_SUBSET_UL_CORNER = ( ");
         double[] ulCorner = getCornerPoint(UL);
         if ( getSpatialSubsetType() == SPACE_LAT_LON ||
              getSpatialSubsetType() == SPACE_PROJ_XY ) {
            pw.print(Double.toString(ulCorner[0]));
            pw.print(" ");
            pw.print(Double.toString(ulCorner[1]));
         } else {
            pw.print(Integer.toString((int)ulCorner[0]));
            pw.print(" ");
            pw.print(Integer.toString((int)ulCorner[1]));
         }
         pw.println(" )");
         pw.print("SPATIAL_SUBSET_LR_CORNER = ( ");
         double[] lrCorner = getCornerPoint(LR);
         if ( getSpatialSubsetType() == SPACE_LAT_LON ||
              getSpatialSubsetType() == SPACE_PROJ_XY ) {
            pw.print(Double.toString(lrCorner[0]));
            pw.print(" ");
            pw.print(Double.toString(lrCorner[1]));
         } else {
            pw.print(Integer.toString((int)lrCorner[0]));
            pw.print(" ");
            pw.print(Integer.toString((int)lrCorner[1]));
         }
         pw.println(" )");
         /*
          * write output filename
          */
         pw.println();
         pw.print("OUTPUT_FILENAME = ");
         String outFilename = getOutputFilename().toLowerCase();
         pw.print(quoteFilename(getOutputFilename()));

         switch (getOutputFileType()) {
            case MULTIFILE:
               if ( !outFilename.endsWith(".hdr")) {
                  setOutputFilename(getOutputFilename()+".hdr");
                  pw.print(".hdr");
               }
               break;
            case HDFEOS:
               if ( !outFilename.endsWith(".hdf")) {
                  setOutputFilename(getOutputFilename()+".hdf");
                  pw.print(".hdf");
               }
               break;
            case GEOTIFF:
               if ( !outFilename.endsWith(".tif")) {
                  setOutputFilename(getOutputFilename()+".tif");
                  pw.print(".tif");
               }
               break;
         }
         pw.println();
         /*
          * resampling type
          */
         pw.println();
         pw.print("RESAMPLING_TYPE = ");
         try {
            pw.println(getResamplingTypeString(getResamplingType()));
         } catch (IllegalArgumentException e) {
            throw new WriteException(
               "WritePrm",
               -2,
               "Error: bad resampling type: "+
               getResamplingType(),false);
         }
         /*
          * output projection type
          */
         pw.println();
         pw.print("OUTPUT_PROJECTION_TYPE = ");
         try
         {
            pw.println(
               getProjectionTypeString(
                  getOutputProjectionType()));
         } catch (IllegalArgumentException e) {
            throw new WriteException(
               "WritePrm",
               -3,
               "Error: bad resampling type: "+
               getOutputProjectionType(),false);
         }
         
         
         /*
          * output projection parameters
          */
         pw.println();
         pw.print("OUTPUT_PROJECTION_PARAMETERS = ( ");
         for (int i=0; i<15; ++i) {
            if ( i % 3 == 0 ) {
               pw.println();
            }
            pw.print(" ");
            pw.print(Double.toString(getOutputProjectionParameter(i)));
         }
         pw.println(" )");



         /* 
          * printing the datum to the parameter file
          */
         pw.println();
         pw.print("DATUM = ");
         pw.println(getDatum());


         if(getProjectionTypeString(getOutputProjectionType())==("UTM")) {
            /*
             * save the UTM zone
             */
             if (utmZone >= -60 && utmZone <= 60) {
                pw.println();
                try  {
                   int value = getUTMZone() ;
                   pw.println("UTM_ZONE = " + getUTMZone());
                } catch (IllegalArgumentException e) {
                   throw new WriteException("WritePrm", -1,
                           "Error: bad UTM zone: " +
                           getOutputPixelSizeUnit (), false);
               }
            }
         }
         
         /*
          * save pixel size
          */
         if ( outputPixelSize != null  && outputPixelSize.length() > 0) {
            pw.println();
            try   {
               pw.println("OUTPUT_PIXEL_SIZE = " + getOutputPixelSize());
            } catch (IllegalArgumentException e) {
               throw e;
            }

            // save  pixel size
            //    pw.println();
         }

         // if ( outputPixelSize != null && outputPixelSize.length() > 0)

         // that's it!
         pw.close();
         fw.close();

      } catch (IOException e) {
         throw new WriteException("WritePrm",-1,
            "IOException occurred writing parameter file: "+
            getParameterFilename(),false);
      }
   }

   public String quoteFilename( String fileName ) {
      if( fileName.indexOf(' ') == -1 )
         return fileName;
      return "\"" + fileName + "\"";
   }

   /***************************************************************************
   NAME:       getInputFilename
   PURPOSE:
             To get input file name
   RETURN VALUE:            String
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public String getInputFilename() {
      return inputFilename;
   }
        
   /*Returns the one and only input file name specified by the user*/
   public String getSingleInputFileName() {
            return temp[0].toString();
   }

   /***************************************************************************
   NAME:       setInputFilename
   PURPOSE:
             To set input file name
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void setInputFilename(String value) {
      inputFilename = value;
   }

   /**************************************************************************
   NAME:       getOutputFilename
   PURPOSE:
             To get output file name
   RETURN VALUE:
            String
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public String getOutputFilename() {
      return outputFilename;
   }

   /***************************************************************************
   NAME:       getOutputPixelSize
   PURPOSE:
             To get output pixel size

   RETURN VALUE:            String
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public String getOutputPixelSize() {
      return outputPixelSize;
   }

   /***************************************************************************
   NAME:       setOutputPixelSize
   PURPOSE:
             To set output pixel size
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void setOutputPixelSize(String value) {
      outputPixelSize = value;
   }

   /***************************************************************************
   NAME:       getOutputPixelSizeUnit
   PURPOSE:
             get output pixel size unit

   RETURN VALUE:
            integer
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public int getOutputPixelSizeUnit() {
      return outputPixelSizeUnit;
   }

   /***************************************************************************
   NAME:       getOutputPixelSizeUnitString
   PURPOSE:
             get output pixel size unit string

   RETURN VALUE:
            String
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public String getOutputPixelSizeUnitString() {
      return getOutputPixelSizeUnitString (getOutputPixelSizeUnit());
   }


   /***************************************************************************
   NAME:       setOutputPixelSizeUnit
   PURPOSE:
             set output pixel size unit for given integer value
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void setOutputPixelSizeUnit(int value) {
      outputPixelSizeUnit = value;
   }

   /**************************************************************************
   NAME:       setOutputPixelSizeUnit
   PURPOSE:
              set output pixel size unit for given string value
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void setOutputPixelSizeUnit(String s)
         throws IllegalArgumentException {
      if (s.equals("METERS")) {
         setOutputPixelSizeUnit(METERS);
      } else if (s.equals("FEET")) {
         setOutputPixelSizeUnit(FEET);
      } else if (s.equals("DEGREES")) {
         setOutputPixelSizeUnit(DEGREES);
      } else if (s.equals("ARC-SEC")) {
         setOutputPixelSizeUnit(ARC_SEC);
      } else {
         throw new IllegalArgumentException();
      }
   }


   /***************************************************************************
   NAME:       setOutputFilename
   PURPOSE:
             set output file name
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void setOutputFilename(String value) {
      outputFilename = value;
   }

   /***************************************************************************
   NAME:       getInputFileType
   PURPOSE:
             get input file type
   RETURN VALUE:            integer
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public int getInputFileType() {
      return inputFileType;
   }

   /***************************************************************************
   NAME:       setInputFileType
   PURPOSE:
             set input file type
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void setInputFileType(int value) {
      inputFileType = value;
   }

   /***************************************************************************
   NAME:       getOutputFileType
   PURPOSE:
             get output file type
   RETURN VALUE:            integer
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public int getOutputFileType() {
      return outputFileType;
   }

   /***************************************************************************
   NAME:       setOutputFileType
   PURPOSE:
             set output file type
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void setOutputFileType(int value) {
      outputFileType = value;
   }

   /***************************************************************************
   NAME:       getInputDataType
   PURPOSE:
             get input data type
   RETURN VALUE:                 integer
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public int getInputDataType() {
      return inputDataType;
   }

   /***************************************************************************
   NAME:       getInputDataTypeString
   PURPOSE:
             get input data type string
   RETURN VALUE:            String
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public String getInputDataTypeString() {
      return getDataTypeString(getInputDataType());
   }

   /***************************************************************************
   NAME:       setInputDataType
   PURPOSE:
             set input data type
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void setInputDataType(int value) {
      inputDataType = value;
   }

   /***************************************************************************
   NAME:       getInputDataTypeArray
   PURPOSE:
             get input data type array
   RETURN VALUE:            String[]
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public String[] getInputDataTypeArray() {
      return inputDataTypeArray;
   }

   /***************************************************************************
   NAME:       getInputPixelSizeArray
   PURPOSE:
             get input pixel size array
   RETURN VALUE:            double[]
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public double[] getInputPixelSizeArray() {
      return inputPixelSizeArray;
   }

   /**************************************************************************
   NAME:       getInputNumOfLinesArray
   PURPOSE:
             get the input number of lines
   RETURN VALUE:            int[]
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public int[] getInputNumOfLinesArray() {
      return inputNumOfLinesArray;
   }

   /***************************************************************************
   NAME:       getInputNumOfSamplesArray
   PURPOSE:
             get the input number of samples
   RETURN VALUE:            int[]
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public int[] getInputNumOfSamplesArray() {
     return inputNumOfSamplesArray;
   }

   /***************************************************************************
   NAME:       getInputBandNamesArray
   PURPOSE:
             get input band name RETURN VALUE:
             String[]   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public String[] getInputBandNamesArray() {
     return inputBandNamesArray;
   }

   /***************************************************************************
   NAME:       getInputMinValueArray
   PURPOSE:
             get input minimum value
   RETURN VALUE:            double[]
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public double[] getInputMinValueArray() {
      return inputMinValueArray;
   }

   /***************************************************************************
   NAME:       getInputMaxValueArray
   PURPOSE:
             get input maxmum value
   RETURN VALUE:            double[]
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public double[] getInputMaxValueArray() {
      return inputMaxValueArray;
   }

   /***************************************************************************
   NAME:       getInputBackgroundFillArray
   PURPOSE:
             get input background fill value
   RETURN VALUE:            double[]
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public double[] getInputBackgroundFillArray() {
      return inputBackgroundFillArray;
   }

   /***************************************************************************
   NAME:       setInputDataType
   PURPOSE:
             set input data type
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void setInputDataType(String s) throws IllegalArgumentException {
      if (s.equals("INT8")) {
         setInputDataType(INT8);
      } else if (s.equals("UINT8")) {
         setInputDataType(UINT8);
      } else if (s.equals("INT16")) {
         setInputDataType(INT16);
      } else if (s.equals("UINT16")) {
         setInputDataType(UINT16);
      } else if (s.equals("INT32")) {
         setInputDataType(INT32);
      } else if (s.equals("UINT32")) {
         setInputDataType(UINT32);
      } else if (s.equals("FLOAT32")) {
         setInputDataType(FLOAT32);
      } else {
         throw new IllegalArgumentException();
      }
   }

   /***************************************************************************
   NAME:       getOutputDataType
   PURPOSE:
             get output data type
   RETURN VALUE:            int
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public int getOutputDataType() {
      return outputDataType;
   }

   /***************************************************************************
   NAME:       getOutputDataTypeString
   PURPOSE:
             get output data type
   RETURN VALUE:            String
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public String getOutputDataTypeString() {
      return getDataTypeString(getOutputDataType());
   }


   /***************************************************************************
   NAME:       setOutputDataType
   PURPOSE:
             set output data type for given integer value
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void setOutputDataType(int value) {
      outputDataType = value;
   }

   /***************************************************************************
   NAME:       setOutputDataType
   PURPOSE:
             set output data type for given string value
             @ an overloaded function
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void setOutputDataType(String s) throws IllegalArgumentException {
      if (s.equals("INT8")) {
         setOutputDataType(INT8);
      } else if (s.equals("UINT8")) {
         setOutputDataType(UINT8);
      } else if (s.equals("INT16")) {
         setOutputDataType(INT16);
      } else if (s.equals("UINT16")) {
         setOutputDataType(UINT16);
      } else if (s.equals("INT32")) {
         setOutputDataType(INT32);
      } else if (s.equals("UINT32")) {
         setOutputDataType(UINT32);
      } else if (s.equals("FLOAT32")) {
         setOutputDataType(FLOAT32);
      } else {
         throw new IllegalArgumentException();
      }
   }


   /***************************************************************************
   NAME:       getNumBands
   PURPOSE:
             get the number of bands
   RETURN VALUE:            int
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public int getNumBands() { return nbands; }
        
   public String getMosaicFileName() { return MosaicFileName; }

   /**************************************************************************
   NAME:       setNumBands
   PURPOSE:
             set the number of bands
             @ an overloaded function
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void setNumBands(int value) {
      nbands = value;
      selectedBands = new boolean[nbands];
      bandinfo = new BandType[nbands];
      for (int i=0; i<nbands; ++i) {
         // by default, all bands are selected
         setSelectedBand(i,true);

         // create blank bandinfo entries
         setBandInfo(i, new BandType());
      }
   }

   /***************************************************************************
   NAME:       setNumBands
   PURPOSE:
              set the number of bands
             @ an overloaded function
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void setNumBands(String s) throws IllegalArgumentException {
      try {
         int value = Integer.valueOf(s).intValue();
         setNumBands(value);
      } catch (NumberFormatException e) {
         throw new IllegalArgumentException();
      }
   }
   
   /***************************************************************************
   NAME:       setUTMZone
   PURPOSE:
              set the UTM zone
             @ an overloaded function
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void setUTMZone(String s) throws IllegalArgumentException {
      try {
         int value = Integer.valueOf(s).intValue();
         setUTMZone(value);
      } catch (NumberFormatException e) {
         throw new IllegalArgumentException();
      }
   }

   /**************************************************************************
   NAME:       setInputUTMZone
   PURPOSE:
              set the UTM zone
             @ an overloaded function
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void setInputUTMZone(String s) throws IllegalArgumentException {
      try {
         int value = Integer.valueOf(s).intValue();
         setInputUTMZone(value);
      } catch (NumberFormatException e) {
         throw new IllegalArgumentException();
      }
   }

   /***************************************************************************
   NAME:       isSelectedBand
   PURPOSE:
             To check to see if a band selected
   RETURN VALUE:            boolean
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public boolean isSelectedBand(int index) {
      return selectedBands[index];
   }

   /***************************************************************************
   NAME:       setSelectedBand
   PURPOSE:
             set selected bands
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   **************************************************************************/
   public void setSelectedBand(int index, boolean value) {
      selectedBands[index] = value;
   }

   /**************************************************************************
   NAME:       getBandInfo
   PURPOSE:
             get infomation for the desired band
   RETURN VALUE:            BandType
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public BandType getBandInfo(int index) {
      return bandinfo[index];
   }

   /***************************************************************************
   NAME:       getBandNum
   PURPOSE:
             get the number of bands
   RETURN VALUE:            int
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public int getBandNum(String bandName) {
      for (int i=0; i<inputBandNamesArray.length; ++i) {
         if ( bandName.equals(inputBandNamesArray[i])) {
            return i;
         }
      }
      return -1;
   }


   /***************************************************************************
   NAME:       setBandInfo
   PURPOSE:
            set band information
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   **************************************************************************/
   private void setBandInfo(int index,BandType value) {
      bandinfo[index] = value;
   }

   /**************************************************************************
   NAME:       getInputCornerPoint
   PURPOSE:     get input corner point coordinates
   RETURN VALUE:           int[]
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public int[] getInputCornerPoint(int index) {
      int[] retval = new int[2];
      retval[0] = inputCornerPoints[index][0];
      retval[1] = inputCornerPoints[index][1];
      return retval;
   }

   /***************************************************************************
   NAME:       setInputCornerPoint
   PURPOSE:
             set input corner point
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void setInputCornerPoint(int index,int[] value) {
      inputCornerPoints[index][0] = value[0];
      inputCornerPoints[index][1] = value[1];
   }

   /***************************************************************************
   NAME:       setInputCornerPoint
   PURPOSE: To set input corner point coordinates
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void setInputCornerPoint(int index,int row,int col) {
      int[] pt = { row, col };
      setInputCornerPoint(index,pt);
   }

   /***************************************************************************
   NAME:       getGringPoint
   PURPOSE:
             To retrieve gring points
   RETURN VALUE:
            return int[2] containing point coordinates
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public double[] getGringPoint(int index) {
      double[] retval = new double[2];
      retval[0] = gringPoints[index][0];
      retval[1] = gringPoints[index][1];
      return retval;
   }

   /***************************************************************************
   NAME:       setGringPoint
   PURPOSE:
             Mutator to set gring points
             @param value int[2] containing point coordinates
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void setGringPoint(int index, double[] value) {
      gringPoints[index][0] = value[0];
      gringPoints[index][1] = value[1];
   }

   /***************************************************************************
   NAME:       setGringPoint
   PURPOSE:
             To set gring point
             @param value int[2] containing point coordinates
             @ An overloaded function
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void setGringPoint(int index, String[] value)
         throws IllegalArgumentException {
      try {
         gringPoints[index][0] = Double.valueOf(value[0]).doubleValue();
         gringPoints[index][1] = Double.valueOf(value[1]).doubleValue();
      } catch (Exception e) {
         throw new IllegalArgumentException();
      }
   }

   /***************************************************************************
   NAME:       getCornerXYPoint
   PURPOSE:
             To retrieve corner XY points
   RETURN VALUE:
            return int[2] containing point coordinates

   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public double[] getCornerXYPoint(int index) {
      double[] retval = new double[2];
      retval[0] = cornerXYPoints[index][0];
      retval[1] = cornerXYPoints[index][1];
      return retval;
   }

   /***************************************************************************
   NAME:       setCornerXYPoint
   PURPOSE:
             To set corner XY points
             param value int[2] containing point coordinates
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void setCornerXYPoint(int index, double[] value) {
      cornerXYPoints[index][0] = value[0];
      cornerXYPoints[index][1] = value[1];
   }

   /***************************************************************************
   NAME:       setCornerXYPoint
   PURPOSE:
             Overloaded function to set the corner XY points
             param value int[2] containing point coordinates
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void setCornerXYPoint(int index, String[] value)
         throws IllegalArgumentException {
      try {
         cornerXYPoints[index][0] = Double.valueOf(value[0]).doubleValue();
         cornerXYPoints[index][1] = Double.valueOf(value[1]).doubleValue();
      } catch (Exception e) {
         throw new IllegalArgumentException();
      }
   }

   /**************************************************************************
   NAME:       getCornerPoint
   PURPOSE: To retrieve corner points
   RETURN VALUE:
            return int[2] containing point coordinates
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public double[] getCornerPoint(int index) {
      double[] retval = new double[2];
      retval[0] = cornerPoints[index][0];
      retval[1] = cornerPoints[index][1];
      return retval;
   }

   /**************************************************************************
   NAME:       setCornerPoint
   PURPOSE:
             Overloaded function to set the corner XY points
             param value int[2] containing point coordinates
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void setCornerPoint(int index,double[] value) {
      cornerPoints[index][0] = value[0];
      cornerPoints[index][1] = value[1];
   }

   /**************************************************************************
   NAME:       setCornerPoint
   PURPOSE:
             Overloaded function to set the corner XY points
             param value int[2] containing point coordinates
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void setCornerPoint(int index,String[] value)
        throws IllegalArgumentException {
      try {
         cornerPoints[index][0] = Double.valueOf(value[0]).doubleValue();
         cornerPoints[index][1] = Double.valueOf(value[1]).doubleValue();
      } catch (Exception e) {
         throw new IllegalArgumentException();
      }
   }

   /***************************************************************************
   NAME:       getInputImageExtent
   PURPOSE:
             To retrieve the input image extent
   RETURN VALUE:
            return int[2] containing point coordinates
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public double[] getInputImageExtent(int index) {
      double[] retval = new double[2];
      retval[0] = inputImageExtent[index][0];
      retval[1] = inputImageExtent[index][1];
      return retval;
   }

   /**************************************************************************
   NAME:       setInputImageExtent
   PURPOSE:
             To set image image extent
             param value int[2] containing point coordinates
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void setInputImageExtent(int index,double[] value) {
      inputImageExtent[index][0] = value[0];
      inputImageExtent[index][1] = value[1];
   }

   /**************************************************************************
   NAME:       setInputImageExtent
   PURPOSE:
             Overloaded function to set the in put image extent
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development
   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
   PROJECT: ModisTool
   NOTES:
   ***************************************************************************/
   public void setInputImageExtent(int index,String[] value)
         throws IllegalArgumentException {
      if ( value.length != 2 ) {
         throw new IllegalArgumentException();
      }
      try {
         double[] arg = new double[2];
         arg[0] = Double.valueOf(value[0]).doubleValue();
         arg[1] = Double.valueOf(value[1 ]).doubleValue();
         setInputImageExtent(index,arg);
      } catch (NumberFormatException e) {
         throw new IllegalArgumentException();
      }
   }

   /***************************************************************************
   NAME:       getResamplingType
   PURPOSE:
             To retrieve the resampling type
   RETURN VALUE:
            integer
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development
   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
   PROJECT: ModisTool
   NOTES:
   ***************************************************************************/
   public int getResamplingType() { return resamplingType; }

   /***************************************************************************
   NAME:         getResamplingTypeString
   PURPOSE:
               To retrieve the resampling type string
   RETURN VALUE:  String
            String
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public String getResamplingTypeString(int resType)
      throws IllegalArgumentException {
      switch (resType) {
         case NN:
            return "NEAREST_NEIGHBOR";
         case BI:
            return "BILINEAR";
         case CC:
            return "CUBIC_CONVOLUTION";
         default:
            throw new IllegalArgumentException();
      }
   }

   /***************************************************************************
   NAME:       setResamplingType
   PURPOSE:
             To set the resampling type
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void setResamplingType(int value) { resamplingType = value; }

   /***************************************************************************
   NAME:       setResamplingType
   PURPOSE: Mutator to set resampling type
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void setResamplingType(String s)
         throws IllegalArgumentException {
      if ( s.equals("NN") || s.equals("NEAREST_NEIGHBOR")) {
         setResamplingType(NN);
      } else if (s.equals("BI") || s.equals("BILINEAR")) {
         setResamplingType(BI);
      } else if (s.equals("CC") || s.equals("CUBIC_CONVOLUTION")) {
         setResamplingType(CC);
      } else {
         throw new IllegalArgumentException();
      }
   }


   /***************************************************************************
   NAME:                setSpatialSubsetType
   PURPOSE:
                                 To set the spatial subset type
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                     Gregg T. Stubbendieck, Ph.D.   Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT:        ModisTool
   NOTES:

   ***************************************************************************/
   public void setSpatialSubsetType(int value) { spatialsubsetType = value; }

   /***************************************************************************
   NAME:                setSpatialSubsetType
   PURPOSE:        Mutator to set spatial subset type
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT:        ModisTool
   NOTES:

   ***************************************************************************/
   public void setSpatialSubsetType(String s)
         throws IllegalArgumentException {

      if ( s.equals("INPUT_LAT_LONG")) {
         setSpatialSubsetType(SPACE_LAT_LON);
      } else if (s.equals("INPUT_LINE_SAMPLE")) {
         setSpatialSubsetType(SPACE_LINE_SAMPLE);
      } else if (s.equals("OUTPUT_PROJ_COORDS")) {
         setSpatialSubsetType(SPACE_PROJ_XY);
      } else {
         throw new IllegalArgumentException();
      }
   }

   /***************************************************************************
   NAME:         getInputProjectionParameter
   PURPOSE:    To retrieve the input projection parameter
   RETURN VALUE:  double
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public double getInputProjectionParameter(int index) {
      return inputProjectionParameters[index];
   }

   /***************************************************************************
   NAME:       setInputProjectionParameter
   PURPOSE:
             To set in put projection parameter
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void setInputProjectionParameter(int index,double value) {
      inputProjectionParameters[index] = value;
   }

   /**************************************************************************
   NAME:         setInputProjectionParameter
   PURPOSE:    Mutator to set input projection parameter
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void setInputProjectionParameter(int index,String value)
         throws IllegalArgumentException {

      try {
         double number = Double.valueOf(value).doubleValue();
         setInputProjectionParameter(index,number);
      } catch (NumberFormatException e) {
         throw new IllegalArgumentException();
      }
   }

   /***************************************************************************
   NAME:         getOutputProjectionParameter
   PURPOSE:    To retrieve output projection parameter
   RETURN VALUE:  double
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public double getOutputProjectionParameter(int index) {
      return outputProjectionParameters[index];
   }

   /***************************************************************************
   NAME:       setOutputProjectionParameter
   PURPOSE:
             To set output projection parameter
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void setOutputProjectionParameter(int index,double value) {
      outputProjectionParameters[index] = value;
   }

   /**************************************************************************
   NAME:       setOutputProjectionParameter
   PURPOSE:
             To set output projection parameter
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void setOutputProjectionParameter(int index,String value)
         throws IllegalArgumentException {

      try {
         double number = Double.valueOf(value).doubleValue();
         setOutputProjectionParameter(index,number);
      } catch (NumberFormatException e) {
         throw new IllegalArgumentException();
      }
   }

   /***************************************************************************
   NAME:       getInputDatumCode
   PURPOSE:
             To retrieve the input datum code
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public int getInputDatumCode() { return inputDatumCode; }

   /***************************************************************************
   NAME:       setInputDatumCode
   PURPOSE:
             TO set input datum code
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void setInputDatumCode(int value) { inputDatumCode = value; }

   /**************************************************************************
   NAME:         getOutputDatumCode
   PURPOSE:    To retrieve output data code
   RETURN VALUE:  int
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public int getOutputDatumCode() { return outputDatumCode; }

   /***************************************************************************
   NAME:       setOutputDatumCode
   PURPOSE:
             To set output datum code
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void setOutputDatumCode(int value) { outputDatumCode = value; }

   /***************************************************************************
   NAME:         getInputZoneCode
   PURPOSE:
               To retrieve input zone code
   RETURN VALUE:  int
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development
   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
   PROJECT: ModisTool
   NOTES:
   ***************************************************************************/
   public int getInputZoneCode() { return inputZoneCode; }

   /***************************************************************************
   NAME:       setInputZoneCode
   PURPOSE:
             To set input zone code
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void setInputZoneCode(int value) { inputZoneCode = value; }

   /**************************************************************************
   NAME:         getOutputZoneCode
   PURPOSE:
               To set output zone code
   RETURN VALUE:  int
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public int getOutputZoneCode() { return outputZoneCode; }

   /***************************************************************************
   NAME:         setOutputZoneCode
   PURPOSE:    To set output zone code
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void setOutputZoneCode(int value) { outputZoneCode = value; }

   /***************************************************************************
   NAME:       getInProjectionInfo
   PURPOSE:
             To retrieve the projection info object
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public ProjInfo getInProjectionInfo() { return inProjectionInfo; }

   /***************************************************************************
   NAME:       setInProjectionInfo
   PURPOSE:
             To set projection info
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void setInProjectionInfo(ProjInfo value) {
      inProjectionInfo = value;
   }

   /**************************************************************************
   NAME:       getOutProjectionInfo
   PURPOSE:
             To retrieve outprojection info
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public ProjInfo getOutProjectionInfo() { return outProjectionInfo; }

   /***************************************************************************
   NAME:       setOutProjectionInfo
   PURPOSE:
             To set outprojection info
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void setOutProjectionInfo(ProjInfo value) {
      outProjectionInfo = value;
   }

   /***************************************************************************
   NAME:       getOutputFileInfo
   PURPOSE:
             To retrieve output info
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public OutFileType getOutputFileInfo() { return outputFileInfo; }

   /***************************************************************************
   NAME:       setOutputFileInfo
   PURPOSE:
             To set output file info
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:
   ***************************************************************************/
   public void setOutputFileInfo(OutFileType value) {
      outputFileInfo = value;
   }

   /***************************************************************************
   NAME:         getNumFilesOut
   PURPOSE:
               To retrieve number file out
   RETURN VALUE:  int
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:
   ***************************************************************************/
   public int getNumFilesOut() { return nfilesOut; }

   /***************************************************************************
   NAME:       setNumFilesOut
   PURPOSE:
             To set number of files out
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:
   ***************************************************************************/
   public void setNumFilesOut(int value) { nfilesOut = value; }

   /***************************************************************************
   NAME:         getInputProjectionType
   PURPOSE:    To retrieve input projection type
   RETURN VALUE:  int
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development
   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
   PROJECT: ModisTool
   NOTES:
   ***************************************************************************/
   public int getInputProjectionType() { return inputProjectionType; }

   /***************************************************************************
   NAME:         getInputProjectionTypeString
   PURPOSE:
               To retrieve input projection type string
   RETURN VALUE:  String
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:
   ***************************************************************************/
   public String getInputProjectionTypeString() {
      return getProjectionTypeString(getInputProjectionType());
   }

   /***************************************************************************
   NAME:       setInputProjectionType
   PURPOSE:  To set input projection type
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void setInputProjectionType(int value) {
      inputProjectionType = value;
   }

   /***************************************************************************
   NAME:         setInputProjectionType
   PURPOSE:    Mutator to set input projection Type
   RETURN VALUE:  none
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development
   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
   PROJECT: ModisTool
   NOTES:
   ***************************************************************************/
   public void setInputProjectionType(String s)
         throws IllegalArgumentException {

      setInputProjectionType(getProjectionTypeInt(s));
   }

   /***************************************************************************
   NAME:       getOutputProjectionType
   PURPOSE:
             To retrieve input projection type
   RETURN VALUE:           int
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development
   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
   PROJECT: ModisTool
   NOTES:
   ***************************************************************************/
   public int getOutputProjectionType() { return outputProjectionType; }

   /***************************************************************************
   NAME:         getOutputProjectionTypeString
   PURPOSE:    To retrieve Output Projection Type String
   RETURN VALUE:  String
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:
   ***************************************************************************/
   public String getOutputProjectionTypeString() {
      return getProjectionTypeString(getOutputProjectionType());
   }


   /***************************************************************************
   NAME:         getInputDatum
   PURPOSE:    To retrieve datum
   RETURN VALUE:  String
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:
   ***************************************************************************/
   public String getInputDatum() {
      return InputDatum;
   }
   
   /**************************************************************************
   NAME:         getDatum
   PURPOSE:    To retrieve datum
   RETURN VALUE:  String
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:
   ***************************************************************************/
   public String getDatum() {
      return Datum;
   }

   /***************************************************************************
   NAME:                       getsphereDatumInt
   PURPOSE:                To retrieve Datum value
   RETURN VALUE:   String
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT:        ModisTool
   NOTES:
   ***************************************************************************/
   public int getsphereDatumInt() {
      /* always NoDatum */
      return 5;
   }


   /***************************************************************************
   NAME:         getDatumInt
   PURPOSE:    To retrieve Datum value
   RETURN VALUE:  String
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:
   ***************************************************************************/
   public int getDatumInt() {
      if(Datum.equals("NAD27")) {
          return 0;
      } if(Datum.equals("NAD83")) {
          return 1;
      } else if(Datum.equals("WGS66")) {
          return 2;
      } else if(Datum.equals("WGS72")) {
          return 3;
      } else if(Datum.equals("WGS84")) {
          return 4;
      } else if(Datum.equals("NoDatum")) {
          return 5;
      } else { 
          return 6;
      }
   }
    
   /***************************************************************************
   NAME:       setDatum
   PURPOSE:
             To set the datum
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:
   ***************************************************************************/
   public void setDatum(String value) {
      Datum = value;
   }
   
   /***************************************************************************
   NAME:       setInputDatum
   PURPOSE:
             To set the Input datum
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:
   ***************************************************************************/
   public void setInputDatum(String value) {
      InputDatum = value;
   }
        
   /**
    *To set the Mosaic output file name
    */
   public void setMosaicFileName(String value) {
      MosaicFileName = value;
   }
   
   /***************************************************************************
   NAME:       setOutputProjectionType
   PURPOSE:
             To set output projection type
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:
   ***************************************************************************/
   public void setOutputProjectionType(int value) {
      outputProjectionType = value;
   }

   /***************************************************************************
   NAME:       setOutputProjectionType
   PURPOSE:
             Mutator to set output projection type
   RETURN VALUE:
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:

   ***************************************************************************/
   public void setOutputProjectionType(String s) {
      setOutputProjectionType(getProjectionTypeInt(s));
   }

   /***************************************************************************
   NAME:         setUTMZone
   PURPOSE:    To set UTm zone specified by user in GUI
   RETURN VALUE:  none
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
           02/08/01   Shujing Jia                    Java  Original development
   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
   PROJECT: ModisTool
   NOTES:
   ***************************************************************************/
   public void setUTMZone(int value) {
      utmZone = value;
   }

   /***************************************************************************
   NAME:         setUTMZone
   PURPOSE:    To set UTm zone specified by user in GUI
   RETURN VALUE:  none
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
           02/08/01   Shujing Jia                    Java  Original development
   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
   PROJECT: ModisTool
   NOTES:
   ***************************************************************************/
   public void setInputUTMZone(int value) {
      InpututmZone = value; 
   }

   /***************************************************************************
   NAME:         getUTMZone
   PURPOSE:    To retrieve UTM zone
   RETURN VALUE:  int
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development
   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
   PROJECT: ModisTool
   NOTES:
   ***************************************************************************/
   public int getUTMZone() {
       return utmZone;
   }

   /***************************************************************************
   NAME:         getUTMZone
   PURPOSE:    To retrieve UTM zone
   RETURN VALUE:  int
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development
   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
   PROJECT: ModisTool
   NOTES:
   ***************************************************************************/
   public int getInputUTMZone() {
       return InpututmZone;
   }

   /***************************************************************************
   NAME:       getUTMZoneString
   PURPOSE:    To retrieve UTM zone string
   RETURN VALUE:  String
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development
   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
   PROJECT: ModisTool
   NOTES:
   ***************************************************************************/
   public String getUTMZoneString() {
      if (utmZone >= -60 && utmZone <= 60)
         return Integer.toString(getUTMZone());
      else
         return "";
   }

   /***************************************************************************
   NAME:         getInputBandName
   PURPOSE:
               To retrieve inpout band name
   RETURN VALUE:  String
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development
   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
   PROJECT: ModisTool
   NOTES:
   ***************************************************************************/
   public String getInputBandName (int index) {
      return inputBandNamesArray[index];
   }

   /***************************************************************************
   NAME:         getInputNumLine
   PURPOSE:
               To retrieve input number of lines
   RETURN VALUE:  int
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development
   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
   PROJECT: ModisTool
   NOTES:
   ***************************************************************************/
   public int getInputNumLine(int index) {
      return inputNumOfLinesArray[index];
   }

   /***************************************************************************
   NAME:       getInputNumSample
   PURPOSE:
             To retrieve input number sample
   RETURN VALUE:  int
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development
   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
   PROJECT: ModisTool
   NOTES:
   ***************************************************************************/
   public int getInputNumSample(int index) {
      return inputNumOfSamplesArray[index];
   }

   /***************************************************************************
   NAME:         getInputPixelSize
   PURPOSE:
               To retrieve input pixel size
   RETURN VALUE:  int
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development
   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
   PROJECT: ModisTool
   NOTES:
   ***************************************************************************/
   public double getInputPixelSize(int index) {
      return inputPixelSizeArray[index];
   }

   /***************************************************************************
   NAME:       getInputDataType
   PURPOSE:
             To retrieve input data type
   RETURN VALUE:  String
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development
   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
   PROJECT: ModisTool
   NOTES:
   ***************************************************************************/
   public String getInputDataType (int index) {
      return inputDataTypeArray[index];
   }

   /***************************************************************************
   NAME:         getInputMinValue
   PURPOSE:
               To retrieve input min value
   RETURN VALUE:  double
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development
   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
   PROJECT: ModisTool
   NOTES:
   ***************************************************************************/
   public double getInputMinValue (int index) {
      return inputMinValueArray[index];
   }

   /***************************************************************************
   NAME:       getInputMaxValue
   PURPOSE:
             To retrieve input max value
   RETURN VALUE:  double
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:
   ***************************************************************************/
   public double getInputMaxValue(int index) {
      return inputMaxValueArray[index];
   }

   /***************************************************************************
   NAME:       getInputBackgroundFill
   PURPOSE:
             To retrieve input background fill
   RETURN VALUE:           double
   PROGRAM HISTORY
   Version  Date     Programmer                     Code  Reason
   -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.  Java  Original development

   COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

   PROJECT: ModisTool
   NOTES:
   ***************************************************************************/
   public double getInputBackgroundFill(int index) {
      return inputBackgroundFillArray[index];
   }

}

