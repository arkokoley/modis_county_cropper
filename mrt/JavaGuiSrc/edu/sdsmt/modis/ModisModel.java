
/*********************************************************************************
    NAME:	ModisModel
	PURPOSE:
		The purpose of this class is to create a data model based on the concept of
		document/view architecture in an attempt to isolate the data model from view
		perspective.
		Specifically, provides access to parameter information for the ModisController
		and ModisFrame.

	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development
	  1.00   Fall'00  Shujing Jia			         Java  Modifications

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
        Java is inherently platform indepentent so the compiled byte code can be
    executed on any of platforms (e.g. Windows, Unix, and Linux). Virtually, there
    is no limitation on running Java byte codes.
		However there is compiler requirement regarding JDK package (version 2.0).

	PROJECT:	ModisModel
	NOTES:
		1.  The data model is designed based on document/view architecture to separate
			the data model from view.
		2.  This class contains many dispatching functions to real data container
			(ModisDescriptor.class) and offers an appearance of a delayed processing.

*********************************************************************************/
package edu.sdsmt.modis;

import java.util.*;
import edu.sdsmt.modis.resample.*;

class ModisModel extends Object
{
   /**
    * This is the structure that holds the information
    */
   private ModisDescriptor descriptor = new ModisDescriptor();

   /* define a text pair */
   class TextIdPair
   {
      private String text;
      private int id;

      public TextIdPair(int i, String s)
      {
         id = i;
         text = s;
      }

      final public String getText() { return text; }
      final public int getId() { return id; }
      public String toString() { return getText(); }
   }


   /*
	 * FileType
	 */
   final public static int BAD_FILE_TYPE = 0;
   final public static int MULTIFILE = 1;
   final public static int HDFEOS = 2;
   final public static int GEOTIFF = 3;


   final public static int SPACE_LAT_LON = ModisDescriptor.SPACE_LAT_LON;
   final public static int SPACE_LINE_SAMPLE = ModisDescriptor.SPACE_LINE_SAMPLE;
   final public static int SPACE_PROJ_XY = ModisDescriptor.SPACE_PROJ_XY;

	/**
	 * A reference to the Singleton instance of this class (or null).
	 */
   private static ModisModel instance;

   private int spatialSubset = SPACE_LAT_LON;
   public int mosaic;

   /*********************************************************************************
    NAME:	ModisModel
	PURPOSE:
			 Constructor--private so that the class can enforce the
			 Singleton nature of this class.
	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:

    *********************************************************************************/
   private ModisModel() {}

   /*********************************************************************************
    NAME:		getInstance
	PURPOSE:
				Returns a reference to the Singleton instance of this
				class.  Creates an instance if that hasn't been done yet.
	RETURN VALUE:
				ModisModel
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:

    *********************************************************************************/
   public static ModisModel getInstance()
   {
      if ( instance == null )
      {
         instance = new ModisModel();
      }
      return instance;
   }

   /*********************************************************************************
    NAME:		check
	PURPOSE:
				checks whether the datum is present in the input file
	RETURN VALUE:
				Vector
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:

    *********************************************************************************/
   public boolean check()
   {
     return  descriptor.check();
   }
   
   /*********************************************************************************
    NAME:		uncheck
	PURPOSE:
				unchecks  the datum code is present in the input file
	RETURN VALUE:
				Vector
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:

    *********************************************************************************/
   public void uncheck()
   {
      descriptor.uncheck();
   }

/*********************************************************************************
    NAME:		check1
	PURPOSE:
				checks whether the UTM zone is present in the input file
	RETURN VALUE:
				Vector
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:

    *********************************************************************************/
   public boolean check1()
   {
      return descriptor.check1();
   }

/*********************************************************************************
    NAME:		uncheck1
	PURPOSE:
				unchecks  the UTM zone  present in the input file
	RETURN VALUE:
				Vector
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development
 *                     Karthik Parameswar               Java   Modifications

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:

    *********************************************************************************/
   public void uncheck1()
   {
      descriptor.uncheck1();
   }
   /*********************************************************************************
    NAME:		getAvailableBandList
	PURPOSE:
				creates an empty list for available band
	RETURN VALUE:
				Vector
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:

    *********************************************************************************/
   public Vector getAvailableBandList()
   {
      return new Vector();
   }


   /*********************************************************************************
    NAME:		getBandList
	PURPOSE:
				To get Band List
	RETURN VALUE:
				Vector
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development
     1.00    10/17    S.  Jia							   modificcation

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:

    *********************************************************************************/
   public Vector getBandList()
   {
      final Vector v = new Vector();
	  String []ss = descriptor.getInputBandNamesArray();
      for (int i=1; i <= getNumBands(); i++)
         v.addElement(ss[i-1]);
      return v;
   }

   /*********************************************************************************
    NAME:		getCornerCoordinate
	PURPOSE:
				To get Corner Coordinate
	RETURN VALUE:
				double
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:

    *********************************************************************************/
   public double getCornerCoordinate( int pointIndex,int coordIndex)
   {
      double[] point = descriptor.getCornerPoint(pointIndex);
      return point[coordIndex];
   }

   /*********************************************************************************
    NAME:		getInputDataTypeString
	PURPOSE:
				To retrieve input data type string from descriptor
	RETURN VALUE:
				String
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development
     1.00    10/17    S.  Jia							   modificcation

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:

    *********************************************************************************/
   public String getInputDataTypeString()
   {
      return descriptor.getInputDataTypeString();
   }

   /*********************************************************************************
    NAME:		getInputDataTypeArray
	PURPOSE:
				To retrieve input data type string from descriptor
	RETURN VALUE:
				String[]
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:

    *********************************************************************************/
   public String[] getInputDataTypeArray ()
   {
      return descriptor.getInputDataTypeArray();
   }
   /*********************************************************************************
    NAME:		getInputPixelSizeArray
	PURPOSE:
				To retrieve input pixel size array from descriptor
	RETURN VALUE:
				double[]
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:

    *********************************************************************************/
   public double[] getInputPixelSizeArray()
   {
      return descriptor.getInputPixelSizeArray();
   }

   /*********************************************************************************
    NAME:		getInputNumOfLinesArray
	PURPOSE:
				To retrieve input number of lines from descriptor
	RETURN VALUE:
				int[]
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:

    *********************************************************************************/
   public int[] getInputNumOfLinesArray()
   {
      return descriptor.getInputNumOfLinesArray();
   }
   /*********************************************************************************
    NAME:		getInputNumOfSamplesArray
	PURPOSE:
				To retrieve  number of samples array from descriptor
	RETURN VALUE:
				int[]
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:

    *********************************************************************************/
   public int[] getInputNumOfSamplesArray()
   {
      return descriptor.getInputNumOfSamplesArray();
   }

   /*********************************************************************************
    NAME:		getInputFilename
	PURPOSE:
				To retrieve input file name string from descriptor
	RETURN VALUE:
				String
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:

    *********************************************************************************/
   public String getInputFilename()
   {
      return descriptor.getInputFilename();
   }
   
   /*Returns the one and only input file name specified by the user*/
    public String getSingleInputFileName()
    {
        return descriptor.getSingleInputFileName();
    }

   /*********************************************************************************
    NAME:	getInputImageExtent
	PURPOSE:
			To retrieve input image extent from descriptor
	RETURN VALUE:
				double[]
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:

    *********************************************************************************/
   public double[] getInputImageExtent(int index)
   {
      return descriptor.getInputImageExtent(index);
   }

   /*********************************************************************************
    NAME:		getMaximumLines
	PURPOSE:
				To return the maximum number of samples in a band for this file
	RETURN VALUE:
				int
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:

    *********************************************************************************/
   public int getMaximumLines()
   {
      return descriptor.getMaximumLines();
   }

   /*********************************************************************************
    NAME:		getMaximumSamples
	PURPOSE:
				To return  the maximum number of samples in a band for this file
	RETURN VALUE:
				int
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:

    *********************************************************************************/
   public int getMaximumSamples()
   {
      return descriptor.getMaximumSamples();
   }

   /*********************************************************************************
    NAME:		getNumBands
	PURPOSE:
				To retrieve numbe string from descriptor
	RETURN VALUE:
				int
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:

    *********************************************************************************/
   public int getNumBands()
   {
      return descriptor.getNumBands();
   }


   /*********************************************************************************
    NAME:		getNumFiles
	PURPOSE:
				To retrieve input data type string from descriptor
	RETURN VALUE:
			int
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
   public int getNumFiles()
   {
      return descriptor.getNumFiles();
   }

   /*********************************************************************************
    NAME:		getOutputDataTypeString
	PURPOSE:
				To retrieve input data type string from descriptor
	RETURN VALUE:
				String
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:

    *********************************************************************************/
   public String getOutputDataTypeString()
   {
      return descriptor.getOutputDataTypeString();
   }

   /*********************************************************************************
    NAME:		getOutputFilename
	PURPOSE:
				Get the current output format as stored in the descriptor.
	RETURN VALUE:
				String
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
   public String getOutputFilename() { return descriptor.getOutputFilename(); }

   /*********************************************************************************
    NAME:		getOutputPixelSize
	PURPOSE:
				Get the current output pixel size as stored in the descriptorr
	RETURN VALUE:
				String
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
   public String getOutputPixelSize() { return descriptor.getOutputPixelSize(); }

   /*********************************************************************************
    NAME:		getOutputFormat
	PURPOSE:
				Get the current output format as stored in the descriptor
	RETURN VALUE:
				String
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
   public int getOutputFormat() { return descriptor.getOutputFileType(); }

   /*********************************************************************************
    NAME:		getOutputFormats()
	PURPOSE:
				To get a Vector of output formats supported by the application.
	RETURN VALUE:
				Vector
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:

    *********************************************************************************/
   public Vector getOutputFormats()
   {
      Vector list = new Vector();

      list.addElement(
         new TextIdPair(ModisDescriptor.GEOTIFF,"GEOTIFF"));
      list.addElement(
         new TextIdPair(ModisDescriptor.HDFEOS,"HDFEOS"));
      list.addElement(
         new TextIdPair(ModisDescriptor.MULTIFILE,"RAW BINARY"));
      return list;
   }


   /*********************************************************************************
    NAME:		getOutputPixelSizeUnits
	PURPOSE:
				To get a Vector of output pixel size units supported by the
				application.
	RETURN VALUE:
				Vector
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
   public Vector getOutputPixelSizeUnits()
   {
      Vector list = new Vector();
      list.addElement(new TextIdPair(ModisDescriptor.METERS,"METERS"));
      list.addElement(new TextIdPair(ModisDescriptor.FEET,"FEET"));
      list.addElement(new TextIdPair(ModisDescriptor.DEGREES,"DEGREES"));
      list.addElement(new TextIdPair(ModisDescriptor.ARC_SEC,"ARC-SEC"));
      return list;
   }

   /*********************************************************************************
    NAME:		getOutputPixelSizeUnits
	PURPOSE:
				To retrieve the output pixel size unit
	RETURN VALUE:
				int
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
   public int getOutputPixelSizeUnit(){ return descriptor.getOutputPixelSizeUnit(); }

   /*********************************************************************************
    NAME:		getBandNum
	PURPOSE:
				To retrieve the band number
	RETURN VALUE:
				int
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
   public int getBandNum(String bandName)  {  return descriptor.getBandNum(bandName);   }

   /*********************************************************************************
    NAME:		getInputProjectionParameter
	PURPOSE:	To retrieve the input project parameter
	RETURN VALUE:
				double
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
   public double getInputProjectionParameter(int index)
   {
      return descriptor.getInputProjectionParameter(index);
   }

   /*********************************************************************************
    NAME:		getInputProjectionTypeString
	PURPOSE:	To retrieve the input projection type string
	RETURN VALUE:
				String
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
   public String getInputProjectionTypeString()
   {
      return descriptor.getInputProjectionTypeString();
   }

   /*********************************************************************************
    NAME:		getOutputProjectionParameter
	PURPOSE:	To retrieve the output projection parameter
	RETURN VALUE:
				double
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
   public double getOutputProjectionParameter(int index)
   {
      return descriptor.getOutputProjectionParameter(index);
   }

   /*********************************************************************************
    NAME:		getOutputProjectionType
	PURPOSE:	Get current projection type stored in descriptor
	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
   public int getOutputProjectionType()
   {
      return descriptor.getOutputProjectionType();
   }
   /*********************************************************************************
    NAME:		getOutputProjectionTypeString
	PURPOSE:	To retrieve the output projection type string
	RETURN VALUE:
				String
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
   public String getOutputProjectionTypeString()
	{
		return descriptor.getOutputProjectionTypeString();
	}

   /*********************************************************************************
    NAME:		getUTMZone
	PURPOSE:	To retrieve the UTM zone
	RETURN VALUE:
				int
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
   public int getUTMZone()
   {
      return descriptor.getUTMZone();
   }
   
   /*********************************************************************************
    NAME:		getInputUTMZone
	PURPOSE:	To retrieve the Input UTM zone
	RETURN VALUE:
				int
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
   public int getInputUTMZone()
   {
      return descriptor.getInputUTMZone();
   }
   /*********************************************************************************
    NAME:		getUTMZoneString
	PURPOSE:	To retrieve the UTM zone string
	RETURN VALUE:
				String
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
   public String getUTMZoneString()
	{
		return descriptor.getUTMZoneString();
	}
	
        /*********************************************************************************
    NAME:               getsphereDatumInt
        PURPOSE:        To retrieve the datum string
        RETURN VALUE:
                                String
        PROGRAM HISTORY
        Version  Date     Programmer                     Code  Reason
        -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.       Java  Original development

        COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

        PROJECT:        ModisTool
        NOTES:
    *********************************************************************************/
   public int getsphereDatumInt()
        {
                return descriptor.getsphereDatumInt();
        }
	/*********************************************************************************
    NAME:		getDatumInt
	PURPOSE:	To retrieve the datum string
	RETURN VALUE:
				String
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
   public int getDatumInt()
	{
		return descriptor.getDatumInt();
	}

/*********************************************************************************
    NAME:		getDatum
	PURPOSE:	To retrieve the datum
	RETURN VALUE:
				String
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
   

   public String getDatum()
   {
      return descriptor.getDatum();
   }
   
   /*********************************************************************************
    NAME:		getInputDatum
	PURPOSE:	To retrieve the input datum
	RETURN VALUE:
				String
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
   

   public String getInputDatum()
   {
      return descriptor.getInputDatum();
   }
   /*********************************************************************************
    NAME:			getProjectionTypeString
	PURPOSE:		To retrieve projection type string
	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
	public java.lang.String getProjectionTypeString(int projType)
	{
		return descriptor.getProjectionTypeString(projType);
	}
   /*********************************************************************************
    NAME:			getParameterFilename
	PURPOSE:		To retrieve parameter file name
	RETURN VALUE:
					String
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
   public String getParameterFilename()
   {
      return descriptor.getParameterFilename();
   }

   /*********************************************************************************
    NAME:		getProjectionTypes
	PURPOSE:
				To get a vector of projection types supported by the application.
	RETURN VALUE:
				Vector
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:

    *********************************************************************************/
   public Vector getProjectionTypes()
   {
      int i = 15; 
      Vector list = new Vector(i);
      
      list.addElement(
         new TextIdPair(ModisDescriptor.PROJ_AEA,"Albers Equal Area"));
      list.addElement(
         new TextIdPair(ModisDescriptor.PROJ_ER,"Equirectangular"));
      list.addElement(
         new TextIdPair(ModisDescriptor.PROJ_GEO,"Geographic"));
      list.addElement(
         new TextIdPair(ModisDescriptor.PROJ_HAM,"Hammer"));
      list.addElement(
         new TextIdPair(ModisDescriptor.PROJ_IGH,
                        "Interrupted Goode Homolosine"));
      list.addElement(
         new TextIdPair(ModisDescriptor.PROJ_ISIN,"Integerized Sinusoidal"));
      list.addElement(
         new TextIdPair(ModisDescriptor.PROJ_LA,"Lambert Azimuthal"));
      list.addElement(
         new TextIdPair(ModisDescriptor.PROJ_LCC,"Lambert Conformal Conic"));
      list.addElement(
         new TextIdPair(ModisDescriptor.PROJ_MERC,"Mercator"));
      list.addElement(
         new TextIdPair(ModisDescriptor.PROJ_MOL,"Molleweide"));
      list.addElement(
         new TextIdPair(ModisDescriptor.PROJ_PS,"Polar Stereographic"));
      list.addElement(
         new TextIdPair(ModisDescriptor.PROJ_SIN,"Sinusoidal"));
      list.addElement(
         new TextIdPair(ModisDescriptor.PROJ_TM,"Transverse Mercator"));
      list.addElement(
         new TextIdPair(ModisDescriptor.PROJ_UTM,"UTM"));   
      return list;
   }

   /**
	 * Get current resampling type stored in descriptor
	 */
   /*********************************************************************************
    NAME:			getResamplingType
	PURPOSE:		Get current resampling type stored in descriptor
	RETURN VALUE:	int
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
   public int getResamplingType() { return descriptor.getResamplingType(); }

   /*********************************************************************************
    NAME:	    getResamplingTypes
	PURPOSE:
				To get a Vector of supported resampling types. This function populates
				data for resampling	types dropdown box.
				To be invoked by ModisFrame
	RETURN VALUE:
				Vector
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
   public Vector getResamplingTypes()
   {
      Vector list = new Vector();
      list.addElement(new TextIdPair(ModisDescriptor.BI,"Bilinear"));
      list.addElement(new TextIdPair(ModisDescriptor.NN,"Nearest Neighbor"));
      list.addElement(new TextIdPair(ModisDescriptor.CC,"Cubic Convolution"));
      return list;
   }

   /*********************************************************************************
    NAME:			getSpatialSubsetType
	PURPOSE:
					To retrieve the currently active spatial subset
	RETURN VALUE:
					int
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
   public int getSpatialSubsetType()
   {
      return descriptor.getSpatialSubsetType();
   }

   /**
	 * Get a Vector of supported spatial subsets
	 */
   /*********************************************************************************
    NAME:			getSpatialSubsets
	PURPOSE:
					To Get a Vector of supported spatial subsets
	RETURN VALUE:
					Vector
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
   public Vector getSpatialSubsets()
   {
      Vector list = new Vector();
      list.addElement(new TextIdPair(ModisDescriptor.SPACE_LAT_LON,"Input Lat/Long"));
      list.addElement(new TextIdPair(ModisDescriptor.SPACE_LINE_SAMPLE,"Input Line/Sample"));
      list.addElement(new TextIdPair(ModisDescriptor.SPACE_PROJ_XY,"Output Projection X/Y"));
      return list;
   }

   /*********************************************************************************
    NAME:			isSelectedBand
	PURPOSE:
					To check if a band is selected
	RETURN VALUE:
					boolean
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
   public boolean isSelectedBand(int i)
   {
      return descriptor.isSelectedBand(i);
   }

   /*********************************************************************************
    NAME:	  readHdrFile
	PURPOSE:  To read header file. This is a dispatching function, passing along
			  to ModisDescriptor for actual file reading (i/o).
			  To be invoked by ModisFrame.
	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:

    *********************************************************************************/
   public void readHdrFile(String fileName) throws ReadException
   {
      // clear out the descriptor by allocating a new object.
      
      descriptor.setInputFilename(fileName);
      try
      {
         descriptor.readHeaderFile();
      }
      catch (ReadException e)
      {
         e.setModule("ReadHdrFile");
         throw e;
      }
   }

   /*********************************************************************************
    NAME:	  readPrmFile
	PURPOSE:  To read parameter file. This is a dispatching function, passing along
			  to ModisDescriptor for actual file reading (i/o).
			  To be invoked by ModisFrame.
	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
   public void readPrmFile(String fileName, boolean readheader) throws ReadException
   {
      if (readheader)
      {
         // clear out the descriptor by allocating a new object.
         descriptor = new ModisDescriptor();
      }

      descriptor.setParameterFilename(fileName);
      try {
         descriptor.readParameterFile(readheader);
         mosaic = descriptor.mosaic;
      }
      catch (ReadException e)
      {
         e.setModule("ReadPrmFile");
         throw e;
      }
   }

   /*********************************************************************************
    NAME:		setCornerPoint
	PURPOSE:	set corner point by calling ModisDescriptor
	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development
	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
   public void setCornerPoint(int index,double[] value)
   {
      descriptor.setCornerPoint(index,value);
   }

   /*********************************************************************************
    NAME:		setInputFilename
	PURPOSE:	set input file name by calling ModisDescriptor
	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development
	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
   public void setInputFilename(String value)
   {
      descriptor.setInputFilename(value);
   }

   /*********************************************************************************
    NAME:		setOutputFilename
	PURPOSE:	set output file name by calling ModisDescriptor
	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development
	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
   public void setOutputFilename(String value)
   {
      descriptor.setOutputFilename(value);
   }

   /*********************************************************************************
    NAME:		setOutputPixelSize
	PURPOSE:	set output pixel size by calling ModisDescriptor
	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development
	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
   public void setOutputPixelSize(String value)
   {
      descriptor.setOutputPixelSize(value);
   }

   /*********************************************************************************
    NAME:		setOutputPixelSizeUnit
	PURPOSE:	set output pixel size unit by calling ModisDescriptor
	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development
	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
   public void setOutputPixelSizeUnit(int value)
   {
      descriptor.setOutputPixelSizeUnit(value);
   }

   /*********************************************************************************
    NAME:		setOutputFormat
	PURPOSE:	set output file type by calling ModisDescriptor
	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development
	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
   public void setOutputFormat(int value)
   {
      descriptor.setOutputFileType(value);
   }

   /*********************************************************************************
    NAME:		setOutputProjectionParameter
	PURPOSE:	set output projection parameter by calling ModisDescriptor
	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development
	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
   public void setOutputProjectionParameter(int index,double value)
   {
      descriptor.setOutputProjectionParameter(index,value);
   }

   /*********************************************************************************
    NAME:		setOutputProjectionType
	PURPOSE:	set output projection type by calling ModisDescriptor
	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development
	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
   public void setOutputProjectionType(int value)
   {
      descriptor.setOutputProjectionType(value);
   }
   /*********************************************************************************
    NAME:		setUTMZone
	PURPOSE:	To set UTM zone
	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development
	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
   public void setUTMZone(int value)
   {
      descriptor.setUTMZone(value);
   }

   /*********************************************************************************
    NAME:		setDatum
	PURPOSE:	To set datum
	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development
	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
   public void setDatum(String value)
   {
      descriptor.setDatum(value);
   }
   /*********************************************************************************
    NAME:		setParameterFilename
	PURPOSE:	set parameter file name by calling ModisDescriptor
	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development
	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
   public void setParameterFilename(String value)
   {
      descriptor.setParameterFilename(value);
   }
   /*
    * Set Mosaic file name by calling ModisDescriptor
    */
   
   public void setMosaicFileName(String value)
   {
      descriptor.setMosaicFileName(value);
   }
   

   /*********************************************************************************
    NAME:		setResamplingType
	PURPOSE:	set resampling type by calling ModisDescriptor
	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development
	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
   public void setResamplingType(int value)
   {
      descriptor.setResamplingType(value);
   }

   /*********************************************************************************
    NAME:		setSelectedBands
	PURPOSE:	To set Selected Bands
	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:

    *********************************************************************************/
   public void setSelectedBands(Enumeration elem)
   {
      // first clear all previous selections
      for ( int i=0; i < descriptor.getNumBands(); ++i)
      {
         descriptor.setSelectedBand(i,false);
      }
      /*
		 * set selections based on the number at the end of each item.
		 * Items are of format "BANDn" where n is some integer representing
		 * the index of the band.
		 */
      while (elem.hasMoreElements())
      {
         String s = elem.nextElement().toString();
         int bandNum;

         bandNum = descriptor.getBandNum(s);

         descriptor.setSelectedBand(bandNum,true);
      }
   }

   /*********************************************************************************
    NAME:               setSpatialSubsetType
        PURPOSE:        To set spatial subset
        RETURN VALUE:
        PROGRAM HISTORY
        Version  Date     Programmer                     Code  Reason
        -------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.       Java  Original development
        COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
        PROJECT:        ModisTool
        NOTES:
    *********************************************************************************/
   public void setSpatialSubsetType(int value)
   {
      descriptor.setSpatialSubsetType(value);
   }

   /*********************************************************************************
    NAME:		writeconvertParameterFile
	PURPOSE:	To write Parameter File
	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:

    *********************************************************************************/
   public void writeconvertParameterFile( int mosaic )
   {
      try {
         descriptor.writeconvertParameterFile( mosaic );
      }
      catch (WriteException e) {
        System.out.println ("writeconvertParameterFile: oops! " + e.toString());
      }
   }
   

   /*********************************************************************************
    NAME:		writeParameterFile
	PURPOSE:	To write Parameter File
	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:

    *********************************************************************************/
   public void writeParameterFile( int mosaic )
   {
      try {
         descriptor.writeParameterFile( mosaic );
      }
      catch (WriteException e) {
        System.out.println ("writeParameterFile: oops! " + e.toString());
      }
   }
}
