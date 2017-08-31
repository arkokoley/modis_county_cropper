/*********************************************************************************
    NAME:	StatusDialog.java
	PURPOSE:
			The purpose of this class is to create a GUI interface for application to
			display the appropriate information.

	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development
	  1.00   Fall'00  S. Jia			             Java  Modifications

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
        Java is inherently platform indepentent so the compiled byte code can be
    executed on any of platforms (e.g. Windows, Unix, and Linux). Virtually, there
    is no limitation on running Java byte codes.
		However there is compiler requirement regarding JDK package (version 2.0).

	PROJECT:	ModisTool
	NOTES:
	    The Graphical User Interface (GUI) design) in this application is based
		on Java swing family class. It extends/derives from JFrame by appropriate
		customization.

*********************************************************************************/
package edu.sdsmt.modis;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;

class StatusDialog extends JFrame
{
    private JPanel mainPanel;
    private JScrollPane textScrollPane;
    private JTextArea textArea;
    private JPanel southPane;
    private JButton closeButton;
    private JButton clearButton;
    public boolean check = false;
   /*********************************************************************************
    NAME:		StatusDialog
	PURPOSE:
				Constructor - to initialize the parent
	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:

    *********************************************************************************/
    public StatusDialog()
    {
		super();
    }

   /*********************************************************************************
    NAME:		close
	PURPOSE:
				To make this dialog invisible
	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:

    *********************************************************************************/
    private void close()
    {
		setVisible(false);
		check = true;
    }

/*********************************************************************************
    NAME:		clear
	PURPOSE:
				To clear this dialog 
	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:

    *********************************************************************************/
    private void clear()
    {
		textArea.setText("");
    }
    
   /*********************************************************************************
    NAME:		append
	PURPOSE:
				To add the text content to the status dialog
	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:

    *********************************************************************************/
    public void append(String s)
    {
		textArea.append(s);
		textArea.setFont(new Font("Courier",Font.PLAIN, 12));
		textArea.setCaretPosition(textArea.getText().length());
    }

   /*********************************************************************************
    NAME:		initialize
	PURPOSE:
				To initialize the various component on the dialog, i. e.
					- layout manager;
					- size and other control-like components
	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:

    *********************************************************************************/
    public void initialize()
    {
		setTitle("Status");
		mainPanel = new JPanel();
		setContentPane(mainPanel);

		mainPanel.setLayout(new BorderLayout());
		textScrollPane = new JScrollPane();
		mainPanel.add(textScrollPane,"Center");
		textArea = new JTextArea();
		textArea.setEditable(false);
		textScrollPane.setViewportView(textArea);

		southPane = new JPanel(new FlowLayout(FlowLayout.RIGHT));
		mainPanel.add(southPane,"South");

		clearButton = new JButton("Clear");
		southPane.add(clearButton);

		closeButton = new JButton("Close");
		southPane.add(closeButton);

		setSize(600,400);
		setLocation(new Point(20,20));

		// add action listner for closeButton
		closeButton.addActionListener(new ActionListener()
		    {
			public void actionPerformed(ActionEvent e)
			{
			    close();
			}
		    });
		// add action listner for clearButton
		clearButton.addActionListener(new ActionListener()
		    {
			public void actionPerformed(ActionEvent e)
			{
			    clear();
			}
		    });
		// add action listner for addWindowListener
		addWindowListener(new java.awt.event.WindowAdapter()
		    {
			public void windowClosing(java.awt.event.WindowEvent e)
			{
			    close();
			}
		    });
    }

}

