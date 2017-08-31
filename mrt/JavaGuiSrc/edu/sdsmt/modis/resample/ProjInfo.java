/*********************************************************************************
    NAME:		ProjInfo.class
	PURPOSE:
		The purpose of this class is to create a data structure to contain all of
		Projection info. This class looks a lot like a data structure.


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


public class ProjInfo
{
	private long projCode;		/* Projection code number                    */
	private long units;             /* Projection units code number              */
	private long zoneCode;		/* Projection zone code (UTM or State Plane) */
	private long datumCode;		/* Projection datum code                     */
	private double[] projCoef = new double[15];	/* 15 projection coefficients                */
} 			/* Structure holding projection parameters   */

