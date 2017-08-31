/*********************************************************************************
    NAME:		WriteException.class
	PURPOSE:
			The purpose of this class is to derive an exception class for error-handling.
			This class is particularly for I/O write exception thows.

			Exception handling is an important part of Java implementation. Although
			Java VM automatically takes care of all memeory leak problem though its
			garbage colletion, there are lots of other exceptions which need to
			be handled. Using exception class can properly catch all exeptions through
			the mechanism of throw and rethrow.

			Once an exception is caught, it has a couple of choices:
				- It may handle it properly on its own ("eat it").
				- It may re-thow and let the caller handle it.
				- Or it may propergate up and ask appropriate parents to
				  take care of it.


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
			This class derives from Java Exception class to be mostly used for
			i/o write exception handling.
*********************************************************************************/
package edu.sdsmt.modis.resample;

import java.util.*;
import java.io.*;

public class WriteException extends Exception
{
	private int errorId;
	private String module;
	private boolean fatal;
	private String extraMessage;

   /*********************************************************************************
    NAME:		WriteException
	PURPOSE:
				 Constructor-- construct a instance
				 This is an overloaded function.
	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
	public WriteException(int id)
	{
		this("",id,"",false);
	}

   /*********************************************************************************
    NAME:		WriteException
	PURPOSE:
				 Constructor-- construct a instance
				 This is an overloaded function.
	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
	public WriteException(String mod,int id)
	{
		this(mod,id,"",false);
	}

   /*********************************************************************************
    NAME:		WriteException
	PURPOSE:
				 Constructor-- construct a instance
				 This is an overloaded function.
	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
	public WriteException(String mod,int id,boolean deadly)
	{
		this(mod,id,"",false);
	}

   /*********************************************************************************
    NAME:		WriteException
	PURPOSE:
				 Constructor-- construct a instance
				 This is an overloaded function.
				 To define the appropriate default values for error info.
				 All other overloaded constructors finally call this constructor
				 to construct the object.

	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
	public WriteException(String mod,int id,String extra,boolean deadly)
	{
		errorId = id;
		module = mod;
		extraMessage = extra;
		fatal = deadly;
	}

   /*********************************************************************************
    NAME:		 getErrorId
	PURPOSE:
				 Get/Set function to enforce data encapsulation
	RETURN VALUE:
				integer
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
	final public int getErrorId() { return errorId; }

   /*********************************************************************************
    NAME:		 getModule
	PURPOSE:
				 Get/Set function to enforce data encapsulation
				 To return a module name where an error happens.
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
	final public String getModule() { return module; }

   /*********************************************************************************
    NAME:		setModule
	PURPOSE:
				Get/Set function to enforce data encapsulation
				To set a module name
	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    *********************************************************************************/
	final public void setModule(String value) { module = value; }

   /*********************************************************************************
    NAME:		isFatal
	PURPOSE:
				To check to see if the error is fatal
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
	final public boolean isFatal() { return fatal; }

   /*********************************************************************************
    NAME:		getExtraMessage
	PURPOSE:
				Get/Set function to enforce data encapsulation
				To retrieve more detailed error info messages.

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
	final public String getExtraMessage() {	return extraMessage; }
}