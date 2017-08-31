/*********************************************************************************
    NAME:		ErrorHandler.class
	PURPOSE:
		The purpose of this class is to create a data container to contain all of
		error information. This class looks a lot like a data structure.

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
*********************************************************************************/
package edu.sdsmt.modis.resample;


public class ErrorHandler
{
	final static public int NO_ERROR =                         0;

	final static public int ERROR_GENERAL =                   -1;
	final static public int ERROR_ASSERT =                    -2;
	final static public int ERROR_ENV =                       -3;
	final static public int ERROR_MEMORY =                    -4;
	final static public int ERROR_THREAD =                    -5;
	final static public int ERROR_SEMAPHORE =                 -6;
	final static public int ERROR_MUTEX =                     -7;

	final static public int ERROR_OPEN_INPUTPAR = 		  -10;
	final static public int ERROR_READ_INPUTPAR =             -11;

	final static public int ERROR_OPEN_OUTPUTPAR =		  -12;
	final static public int ERROR_WRITE_OUTPUTPAR =		  -13;

	final static public int ERROR_OPEN_INPUTIMAGE = 	  -14;
	final static public int ERROR_READ_INPUTIMAGE =		  -15;

	final static public int ERROR_OPEN_OUTPUTIMAGE = 	  -16;
	final static public int ERROR_WRITE_OUTPUTIMAGE =	  -17;

	final static public int ERROR_OPEN_INPUTHEADER =	  -18;
	final static public int ERROR_READ_INPUTHEADER =	  -19;

	final static public int ERROR_OPEN_OUTPUTHEADER =	  -20;
	final static public int ERROR_WRITE_OUTPUTHEADER =	  -21;

	final static public int ERROR_NOCOMMANDLINE_ARGUMENT =    -22;
	final static public int ERROR_NOPARAMFILE_NAME =          -23;
	final static public int ERROR_UNKNOWN_ARGUMENT =          -24;
	final static public int ERROR_INPUT_EXTENSION =           -25;
	final static public int ERROR_OUTPUT_EXTENSION =          -26;
	final static public int ERROR_RESAMPLE_TYPE =             -27;
	final static public int ERROR_PROJECTION_TYPE =           -28;

	final static public int ERROR_INPUTNAME_FIELD =           -29;
	final static public int ERROR_SPECTRALSUB_FIELD =         -30;
	final static public int ERROR_SPATIALSUB_FIELD =          -31;
	final static public int ERROR_OUTPUTNAME_FIELD =          -32;
	final static public int ERROR_RESAMPLE_FIELD =            -33;
	final static public int ERROR_OUTPROJECTION_FIELD =       -34;
	final static public int ERROR_OUTPROJPARAMS_FIELD =       -35;
	final static public int ERROR_DATA_TYPE =                 -36;
	final static public int ERROR_PROJPARAMS_FIELD =          -37;
	final static public int ERROR_PROJPARAMS_VALUE =          -38;
	final static public int ERROR_READ_CORNERS =              -39;
	final static public int ERROR_TOTALBANDS_FIELD =          -40;
	final static public int ERROR_TOTALBANDS_VALUE =          -41;
	final static public int ERROR_NFILES_FIELD =              -42;
	final static public int ERROR_NFILES_VALUE =              -43;
	final static public int ERROR_BGROUP_FIELD =              -44;
	final static public int ERROR_BGROUP_VALUE =              -45;
	final static public int ERROR_NLINES_FIELD =              -46;
	final static public int ERROR_NLINES_VALUE =              -47;
	final static public int ERROR_NSAMPLES_FIELD =            -48;
	final static public int ERROR_NSAMPLES_VALUE =            -49;
	final static public int ERROR_PIXELSIZE_FIELD =           -50;
	final static public int ERROR_PIXELSIZE_VALUE =           -51;
	final static public int ERROR_NBANDS_FIELD =              -52;
	final static public int ERROR_NBANDS_VALUE =              -53;
	final static public int ERROR_EGROUP_FIELD =              -54;
	final static public int ERROR_EGROUP_VALUE =              -55;
	final static public int ERROR_BANDCOUNT =                 -56;
	final static public int ERROR_NO_BANDS =                  -57;
        final static public int ERROR_MISSING_SPATIAL_SUBSET_TYPE = -58;
        final static public int ERROR_SPATIALSUBSET_TYPE =        -59;
        final static public int ERROR_NO_ENDING_QUOTE =           -60;

	final static public int ERROR_PROJECTION =                -70;
	final static public int ERROR_OPEN_DATUMFILE = 		  -71;
	final static public int ERROR_OPEN_SPHEREFILE = 	  -72;
	final static public int ERROR_PROJECTION_MATH = 	  -73;

	final static public int ERROR_GCTP_PTBREAK =              -74;
	final static public int ERROR_GCTP_OUTNAME =              -75;
	final static public int ERROR_GCTP_TRANSFAIL =            -76;
	final static public int ERROR_GCTP_CONVERGE =             -77;
	final static public int ERROR_GCTP_ROBITER =              -78;
	final static public int ERROR_GCTP_INVITER =              -79;
	final static public int ERROR_GCTP_INDATA =               -80;
	final static public int ERROR_GCTP_BADDMS =               -81;
	final static public int ERROR_GCTP_INUNIT =               -82;
	final static public int ERROR_GCTP_INSYS =                -83;
	final static public int ERROR_GCTP_INCODE =               -84;
	final static public int ERROR_GCTP_INZONE =               -85;
	final static public int ERROR_GCTP_INF =                  -86;
	final static public int ERROR_GCTP_LATITER =              -87;
	final static public int ERROR_GCTP_OUTUNIT =              -88;
	final static public int ERROR_GCTP_OUTSYS =               -89;
	final static public int ERROR_GCTP_OUTCODE =              -90;
	final static public int ERROR_GCTP_OUTZONE =              -91;
	final static public int ERROR_GCTP_POLAR =                -92;
	final static public int ERROR_GCTP_PTPROJ =               -93;
	final static public int ERROR_GCTP_PTCIRC =               -94;
	final static public int ERROR_GCTP_50ITER =               -95;
	final static public int ERROR_GCTP_SPHERE =               -96;
	final static public int ERROR_GCTP_EQLAT  =               -97;
	final static public int ERROR_GCTP_BADZONE =              -98;
	final static public int ERROR_GCTP_SPPAR =                -99;
	final static public int ERROR_GCTP_BADCODE =              -100;
	final static public int ERROR_GCTP_MISPAR =               -101;
	final static public int ERROR_INPUT_WINDOW =              -102;
	final static public int ERROR_OUTPUT_WINDOW =             -103;
        final static public int ERROR_OUTPUTPIXELSIZE_FIELD =     -104;

	final static public int ERROR_MIN_VALUE =		  -105;
	final static public int ERROR_MAX_VALUE =		  -106;
	final static public int ERROR_BACKGROUND_FILL =           -107;
	final static public int ERROR_READ_FILES =		  -108;

	final static public int ERROR_READ_GRINGS =               -109;
        final static public int ERROR_OUTPUTPIXELSIZE_UNIT =      -110;
        final static public int ERROR_MISSING_FIELDS =            -111;
        final static public int ERROR_UTM_ZONE =                  -112;
	final static public int ERROR_FILE_NOT_EXIST =		  -113;
	final static public int ERROR_EXE_FILE_NOT_EXIST =	  -114;

	final static public int ERROR_MISSING_OUTPUT_FILE_NAME =  -115;
	final static public int ERROR_MISSING_OUTPUT_PROJ_TYPE =  -116;
	final static public int ERROR_MISSING_INPUT_FILE_NAME =	  -117;
        final static public int ERROR_DATUM =                     -118;
        final static public int ERROR_EXIT_FAILURE =              -119;
	final static public int NUM_ERROR_CODES =                  119;

   /*********************************************************************************
    NAME:		formalMessage
	PURPOSE:
				 This just defines a string array of message info.
	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
				This is NOT a function definition.
    *********************************************************************************/
	final static public String[] formalMessage =
	{
	    "No Error",			/* zero, not an error? */
	    "General Processing",	/* -1 */
	    "ASSERT",			/* system */
	    "Environmental Variable Not Found",
	    "Memory Allocation Failed",
	    "Error waiting for thread termination",
	    "Semaphore Error",
	    "Mutex Error",
	    "None",
	    "None",
	    "Opening Input Parameter File",	/* -10 *//* general file io */

	    "Reading Input Parameter File",
	    "Opening Output Parameter File",
	    "Writing Output Parameter File",
	    "Opening Input Image File",
	    "Reading Input Image File",
	    "Opening Output Image File",
	    "Writing Output Image File",
	    "Opening Input Header File",
	    "Reading Input Header File",
	    "Opening Output Header File",	                /* -20 */

	    "Writing Output Header File",                   // -21
	    "No Command Line Arguments",	/* command line & prm file */
	    "Missing Or Bad Parameter Filename",
	    "Unknown Command Line Argument",
	    "Bad Or Missing Input Filename Extension",
	    "Bad Or Missing Output Filename Extension",
	    "Bad Or Missing Resample Type",
	    "Bad Or Missing Projection Type",
	    "Bad Or Missing INPUT_FILENAME Field",
	    "Bad Or Missing SPECTRAL_SUBSET Field",	/* -30 */

	    "Bad Or Missing SPATIAL_SUBSET Field",
	    "Bad Or Missing OUTPUT_FILENAME Field",
	    "Bad Or Missing RESAMPLING_TYPE Field",
	    "Bad Or Missing OUTPUT_PROJECTION Field",
	    "Bad Or Missing OUTPUT_PROJECTION_PARAMETERS field",
	    "Bad Or Missing DATA_TYPE Field",
	    "Bad Or Missing PROJECTION_PARAMETERS Field",	/* -37 */
	    "Bad Or Missing PROJECTION_PARAMETERS Values",
	    "Bad Or Missing Spatial Extents Corner",
	    "Bad Or Missing TOTAL_BANDS Field",	/* -40 */

	    "Bad Or Missing TOTAL_BANDS Value",
	    "Bad Or Missing NFILES Field",
	    "Bad Or Missing NFILES Value",
	    "Bad Or Missing BEGIN_GROUP Field",
	    "Bad Or Missing BEGIN_GROUP Value",
	    "Bad Or Missing NLINES Field",
	    "Bad Or Missing NLINES Value",
	    "Bad Or Missing NSAMPLES Field",
	    "Bad Or Missing NSAMPLES Value",
	    "Bad Or Missing PIXEL_SIZE Field",	/* -50 */

	    "Bad Or Missing PIXEL_SIZE Value",
	    "Bad Or Missing NBANDS Field",
	    "Bad Or Missing NBANDS Value",
	    "Bad Or Missing END_GROUP Field",
	    "Bad Or Missing END_GROUP Value",
	    "Total Bands Found Is Inconsistent With NBANDS",
	    "No Bands Selected For Output",
	    "Bad or Missing SPATIAL_SUBSET_TYPE Field",
	    "Invalid SPATIAL_SUBSET_TYPE Value",
	    "Starting quote found with no corresponding ending quote", /* -60 */

	    "None",                     /* -61 */
	    "None",
	    "None",
	    "None",
	    "None",
	    "None",
	    "None",
	    "None",
	    "None",
	    "Projection Processing Error",	/* -70 *//* gctp & geolib */

	    "Open Datum File Error",
	    "Open Spheroid File Error",
	    "Projection Math Error",
	    "Point lies in break",
	    "Output file name not specified",
	    "Projection transformation failed",
	    "Failed to converge after many iterations",
	    "Too many iterations for inverse Robinson",
	    "Too many iterations in inverse",
	    "Input data error",                 // -80

	    "Illegal DMS field",
	    "Inconsistant unit and system codes for input",
	    "Illegal input system code",
	    "Illegal input unit code",
	    "Illegal input zone code",
	    "Point projects into infinity",
	    "Latitude failed to converge after many iterations",
	    "Inconsistant unit and system codes for output",
	    "Illegal output system code",
	    "Illegal output unit code",                      // -90

	    "Illegal output zone code",
	    "Transformation cannot be computed at the poles",
	    "Point cannot be projected",
	    "Point projects into a circle of unacceptable radius",
	    "Fifty iterations performed without conversion",
	    "Spheroid code reset to default",
	    "Equal latitudes for std parallels on opposite sides of equator",
	    "Illegal zone number",
	    "Error opening state plane parameter file",
	    "Illegal source or target unit code",           // -100

	    "Missing projection parameters",
	    "Invalid corner coordinates for input image",
	    "Output window falls outside mapping grid"	/* -103 */,
            "Error reading output pixel size",
	    "Bad Or Missing MIN Field",
	    "Bad Or Missing MAX Field",
	    "Bad Or Missing BACKGROUND FILL Field",
	    "Bad Or Missing input file reading",
	    "Bad Or Missing  Gring_LatLong reading in header file",  // -109
	    "Error: reading output pixel size unit field",	// -110
	    "Error: missing required fields in parameter file.",  // -111
	    "Error reading UTM zone field",  // -112
	    "HDF file does not exist!",	 // -113
	    "resample executable does not exist or is not in your path!",// -114
	    "Missing OUTPUT_FILENAME field in the .prm file!",  // -115
 	    "Missing OUTPUT_PROJECTION_TYPE field in the .prm file!",  // -116
	    "Missing INPUT_FILENAME field in the .prm file!",  // -117
            "Error reading DATUM field",  //-118
	    "Unknown error.", // -119
            "One of the selected input files does not exist or the input files are not the same data product type"//120
	};
}
