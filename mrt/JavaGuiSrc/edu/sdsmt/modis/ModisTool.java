
/*********************************************************************************
    NAME:	Class ModisTool
	PURPOSE:
			The purpose of this class is to provide an entry point (main function) to
			start.

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

	PROJECT:	Modis Re-Projection Tool
	NOTES:
		GENERAL COMMENTS ON JAVA CODING STYLE
		-------------------------------------
		1. According to Java coding style, lengthy documents are unnecessary
		for Java codes. 	The Java codes should be well modular
		(usually 5-10 lines each function), and use good name convention. Class name
		and function name should well represent what they do. Thus, only complex
		functions are properly documented.

		2. The most of Java features such as inheretance, complexity encapsulation,
		polymorphism, delegation, and function dispatching are appropriately implenented.
		The concepts of Object-based design (OBD) & Object-oriented Design (OOP) are used
		as necessary.

		3. The one of most important Java features is exception handling. This allows
		user to correctly handle various kinds of exceptions and errors by customizing
		abd overriding the appropriate Expcetion functions. This features are well-handled
		in this implementation. This is important because the application heavily involves
		error-prone user-interaction.

*********************************************************************************/
package edu.sdsmt.modis;

import javax.swing.*;
import java.io.*;

public class ModisTool
{
	private ModisController controller;

   /*********************************************************************************
    NAME:	ModisTool()
	PURPOSE:
			To create an instance of controller and initialize it.

	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
			If exceptions occurs, catch it.

    *********************************************************************************/
	public ModisTool()
	{
		try
		{
			// For native Look and Feel, uncomment the following code.
		
         /*try
           {
           UIManager.setLookAndFeel(UIManager.getLookAndFeel());
           }
           catch (Exception e)
           {
           }*/
			
                 controller = ModisController.getInstance();
                 controller.initialize();
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}

   /*********************************************************************************
    NAME:	main()
	PURPOSE:
			This is the main entry point.  It instantiates an object of ModisTool,
			which in turn instantiates controller.  It takes one argument.

	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
			If exceptions occurs, catch it.
			It takes one argument.

    *********************************************************************************/
	public static void main(String[] args)
	{
          /* start the ModisTool */
	  new ModisTool();
        }
}

