/*********************************************************************************
    NAME:		BandType.class
	PURPOSE:
		The purpose of this class is to create a data container to contain all of
		Band Type information to describe a band parameters. This class looks
		a lot like a data structure.

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


public class BandType
{
    /* image dimensions */
	private int nlines;
	private int nsamples;
	private int fileNumber;

    /* data types (byte, int, etc.) */
	private int inputDataType;
	private int outputDataType;

    /* pixel size, in meters */
	private double pixelSize;

    /* spatial subset corner points (UL, UR, LL, LR in row/col) */
	private int[][] inputCornerPoints = new int[4][2];

    /* UL, UR, LL, LR spatial subset, computed by GCTP	*/
    /* (units depend on output projection)		*/
	private double[][] outputCornerPoints = new double[4][2];


   /*********************************************************************************
    NAME:		BandType
	PURPOSE:
				 Constructor--do nothing
	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
	public BandType() {}

   /*********************************************************************************
    NAME:		BandType
	PURPOSE:
				Get/Set function to enforce data encapsulation of Java Object-Based
				Design (OBD).

	RETURN VALUE: the number of lines
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
	public int getNumLines() { return nlines; }
   /*********************************************************************************
    NAME:		setNumLines
	PURPOSE:
				Get/Set function to enforce data encapsulation of Java Object-Based
				Design (OBD).

	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
	public void setNumLines(int value) { nlines = value; }

   /*********************************************************************************
    NAME:		getNumSamples
	PURPOSE:
				Get/Set function to enforce data encapsulation of Java Object-Based
				Design (OBD).

	RETURN VALUE:  the number of the samples
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
	public int getNumSamples() { return nsamples; }

   /*********************************************************************************
    NAME:		setNumSamples
	PURPOSE:
				Get/Set function to enforce data encapsulation of Java Object-Based
				Design (OBD).

	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
	public void setNumSamples(int value) { nsamples = value; }

   /*********************************************************************************
    NAME:		getInputDataType
	PURPOSE:
				Get/Set function to enforce data encapsulation of Java Object-Based
				Design (OBD).

	RETURN VALUE: input data type
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
	public int getInputDataType() { return inputDataType; }

   /*********************************************************************************
    NAME:		setInputDataType
	PURPOSE:
				Get/Set function to enforce data encapsulation of Java Object-Based
				Design (OBD).

	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
	public void setInputDataType(int value) { inputDataType = value; }

   /*********************************************************************************
    NAME:		getOutputDataType
	PURPOSE:
				Get/Set function to enforce data encapsulation of Java Object-Based
				Design (OBD).

	RETURN VALUE: outputDataType
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
	public int getOutputDataType() { return outputDataType; }

   /*********************************************************************************
    NAME:		setOutputDataType
	PURPOSE:
				Get/Set function to enforce data encapsulation of Java Object-Based
				Design (OBD).

	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
	public void setOutputDataType(int value) { outputDataType = value; }

   /*********************************************************************************
    NAME:		getPixelSize
	PURPOSE:
				Get/Set function to enforce data encapsulation of Java Object-Based
				Design (OBD).

	RETURN VALUE:  pixelSize
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
	public double getPixelSize() { return pixelSize; }

   /*********************************************************************************
    NAME:		setPixelSize
	PURPOSE:
				Get/Set function to enforce data encapsulation of Java Object-Based
				Design (OBD).

	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
	public void setPixelSize(double value) { pixelSize = value; }

   /*********************************************************************************
    NAME:		getFileNumber
	PURPOSE:
				Get/Set function to enforce data encapsulation of Java Object-Based
				Design (OBD).

	RETURN VALUE:  fileNumber
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
	public int getFileNumber() { return fileNumber; }

   /*********************************************************************************
    NAME:		setFileNumber
	PURPOSE:
				Get/Set function to enforce data encapsulation of Java Object-Based
				Design (OBD).

	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
	public void setFileNumber(int value) { fileNumber = value; }

   /*********************************************************************************
    NAME:		getInputCornerPoint
	PURPOSE:
				Get/Set function to enforce data encapsulation of Java Object-Based
				Design (OBD).

	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
	public int[] getInputCornerPoint(int index)
	{
		int[] retval = new int[2];
		retval[0] = inputCornerPoints[index][0];
		retval[1] = inputCornerPoints[index][1];

		return retval;
	}
   /*********************************************************************************
    NAME:		setInputCornerPoint
	PURPOSE:
				Get/Set function to enforce data encapsulation of Java Object-Based
				Design (OBD).

	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
	public void setInputCornerPoint(int index,int[] point)
	{
		setInputCornerPoint(index,point[0],point[1]);
	}
   /*********************************************************************************
    NAME:		setInputCornerPoint
	PURPOSE:
				Get/Set function to enforce data encapsulation of Java Object-Based
				Design (OBD).

	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
	public void setInputCornerPoint(int index, int a, int b)
	{
		inputCornerPoints[index][0] = a;
		inputCornerPoints[index][1] = b;
	}

   /*********************************************************************************
    NAME:		getOutputCornerPoint
	PURPOSE:
				Get/Set function to enforce data encapsulation of Java Object-Based
				Design (OBD).

	RETURN VALUE: coordinates of output corner point
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
	public double[] getOutputCornerPoint(int index)
	{
		double[] retval = new double[2];
		retval[0] = outputCornerPoints[index][0];
		retval[1] = outputCornerPoints[index][1];

		return retval;
	}
   /*********************************************************************************
    NAME:		setOutputCornerPoint
	PURPOSE:
				Get/Set function to enforce data encapsulation of Java Object-Based
				Design (OBD).

	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
	public void setOutputCornerPoint(int index,double[] point)
	{
		setOutputCornerPoint( index, point[0],point[1]);
	}
   /*********************************************************************************
    NAME:		setOutputCornerPoint
	PURPOSE:
				Get/Set function to enforce data encapsulation of Java Object-Based
				Design (OBD).

	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
	public void setOutputCornerPoint(int index, double a, double b)
	{
		outputCornerPoints[index][0] = a;
		outputCornerPoints[index][1] = b;
	}
}
