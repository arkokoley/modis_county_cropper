/*********************************************************************************
    NAME:	MetadataDialog.java
	PURPOSE:
			The purpose of this class is to create a GUI interface to display
			the more detailed information than the info box. This class contains
			JDialog derived class to properly dislay those detailed data.


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
	    1. The Graphical User Interface (GUI) design) in this application is based
		on Java swing family class. It extends/derives from JDialog by appropriate
		customization.

*********************************************************************************/
package edu.sdsmt.modis;

import java.awt.*;
import java.awt.event.*;
import java.io.*;
import javax.swing.*;
import javax.swing.event.*;

   /*********************************************************************************
    NAME:	 MetadataDialog()
	PURPOSE:
			To customize and subclass from JDialog which is built-in class
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:

    *********************************************************************************/
class MetadataDialog  extends JFrame
{
      private String metadataFilename;
      private JPanel mainPanel;
      private JScrollPane textScrollPane;
      private JTextArea textArea;
      private JTextField searchText;
      private JPanel southPane;
      private JPanel southMiddlePane;
      private JButton findButton;
      private JButton closeButton;
      private boolean fileRead = false;
      private boolean check;

   /*********************************************************************************
    NAME:	 MetadataDialog()
	PURPOSE:
			To construct an the dialog object when invoked

	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:

    *********************************************************************************/
      public MetadataDialog(String filename,boolean s)
      {
		super();
		metadataFilename = filename;
		check = s;
      }

   /*********************************************************************************
    NAME:	 closeAndDispose()
	PURPOSE:
			To close and destroy this dialog window

	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:

    *********************************************************************************/
      private void closeAndDispose()
      {
	 setVisible(false);
         /*
          *Delete TmpMeta.txt
          */
         File tmp = new File("TmpMeta.txt");
         tmp.delete();        
         
	 dispose();
      }

   /*********************************************************************************
    NAME:	 doFind()
	PURPOSE:
				To this is an implementation function, which is called by
				Find button action function

	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:

    *********************************************************************************/
      private void doFind()
      {
	    final String searchString = searchText.getText();
	    final String selectedText = textArea.getSelectedText();
	    final String text = textArea.getText();
	    int index = -1;

	    if ( selectedText == null || !selectedText.equals(searchString))
	    {
	       index = text.indexOf(searchString);
	    }
	    
	   else 
	    {
	       int offset = textArea.getSelectionEnd();
	       index = text.substring(offset).indexOf(searchString);
	       
	       if (index >= 0)
	       {
	         index += offset;
	       }
	       else
	       {
	         index = text.indexOf(searchString);
	       }
	    }

     
        
        
	    if (index >= 0)
	    {
	       textArea.setSelectionStart(index);
	       textArea.setSelectionEnd(index + searchString.length());
	    }
	    
	    else
	    {
	       JOptionPane.showMessageDialog(this,"String not found.");
	    }
      }


   public void initialize()
   {
	 
	 mainPanel = new JPanel();
	 setContentPane(mainPanel);

	 mainPanel.setLayout(new BorderLayout());
	 textScrollPane = new JScrollPane();
	 mainPanel.add(textScrollPane,"Center");
	 textArea = new JTextArea();
	 textArea.setEditable(false);
	 textScrollPane.setViewportView(textArea);

	 southPane = new JPanel(new BorderLayout());
	 mainPanel.add(southPane,"South");

	 southMiddlePane = new JPanel(new FlowLayout(FlowLayout.LEFT));
	 southPane.add(southMiddlePane,"Center");
	 southMiddlePane.add(new JLabel("Find: "));
	 searchText = new JTextField();
	 searchText.setColumns(12);
	 southMiddlePane.add(searchText);
	 findButton = new JButton("Find");
	 findButton.setEnabled(false);
	 southMiddlePane.add(findButton);

	 closeButton = new JButton("Close");
	 southPane.add(closeButton,"East");

	 if(check)
	 {
	 setTitle("Release Notes");
	 setSize(700,500);
	 setLocation(new Point(20,20));
	 }
	 else
	 {
	    setTitle("View Metadata");
	    setSize(700,500);
	    setLocation(new Point(20,20));
	 }

	 readMetadataIntoTextArea();

	 closeButton.addActionListener(new ActionListener()
	    {
		  public void actionPerformed(ActionEvent e)
		  {
		     closeAndDispose();
		  }
	    });
	 addWindowListener(new java.awt.event.WindowAdapter()
	    {
		  public void windowClosing(java.awt.event.WindowEvent e)
		  {
		     closeAndDispose();
		  }
	    });
	    
	    
	 searchText.getDocument().addDocumentListener(new DocumentListener()
	    {
		  public void changedUpdate(DocumentEvent e)
		  {
		     searchTextActivity();
		  }
		  public void removeUpdate(DocumentEvent e)
		  {
		     searchTextActivity();
		  }
		  public void insertUpdate(DocumentEvent e)
		  {
		     searchTextActivity();
		  }
	    });

	 findButton.addActionListener(new ActionListener()
	    {
		  public void actionPerformed(ActionEvent e)
		  {
		     doFind();
		  }
	    });
      }

   /*********************************************************************************
    NAME:		isFileRead()
	PURPOSE:
				To check if the file is read

	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:

    *********************************************************************************/
    public boolean isFileRead()
    {
		return fileRead;
    }

   /*********************************************************************************
    NAME:		readMetadataIntoTextArea()
	PURPOSE:
				To read metadata into the test area.

	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:

    *********************************************************************************/
      private void readMetadataIntoTextArea()
      {
	     fileRead = false;
	     try
	     {
	       File mdFile = new File(metadataFilename);
	       long fileLength = mdFile.length();
	       char[] buf = new char[(int)fileLength];
	       FileReader fr = new FileReader(mdFile);
	       fr.read(buf);
	       textArea.setText(new String(buf));
	       fileRead = true;
	    }
	    catch (IOException e)
	    {
	       JOptionPane.showMessageDialog(this,"Error reading metadata file: " + metadataFilename);
	    } 
      }

      
      private void searchTextActivity()
      {
	    findButton.setEnabled(searchText.getText().length() > 0);
      }
}




