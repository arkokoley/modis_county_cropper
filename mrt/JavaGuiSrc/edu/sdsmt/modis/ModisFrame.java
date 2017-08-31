package edu.sdsmt.modis;

import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.util.*;
import javax.swing.*;
import javax.swing.border.*;
import javax.swing.event.*;
import javax.swing.JToolBar;
import javax.swing.ImageIcon;
import javax.swing.AbstractButton;
import edu.sdsmt.modis.resample.*;
import edu.sdsmt.modis.image.*;

/**
 * NAME:  ModisFrame.java
 * PURPOSE:
 *  The purpose of this class is to create a GUI interface for
 *  application and allow clients to communicate with application.
 *  It collects user inputs and dispatch (propogate) the appropriate
 *  processing along to other objects or classes such as controller,
 *  model, descriptor.
 * 
 *  The design of this class is to construct a view (GUI) based on the
 *  concept of document/view architecture in an attempt to isolate the
 *  view interface from data model perspective.
 * 
 * PROGRAM HISTORY
 *  Version  Date    Programmer                     Code  Reason
 *  -------  ----    ---------------------------    ----  --------------------
 *                   Gregg T. Stubbendieck, Ph.D.   Java  Original development
 *   1.00   Fall'00  S. Jia                         Java  Modifications
 * 
 * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
 *  Java is inherently platform indepentent so the compiled byte code
 *  can be executed on any of platforms (e.g. Windows, Unix, and Linux).
 *  Virtually, there is no limitation on running Java byte codes.
 *  However there is compiler requirement regarding JDK package
 *  (version 2.0).
 * 
 * NOTES:
 *  1. The Graphical User Interface (GUI) design) in this application is
 *  based on Java swing family class. It extends/derives from JFrame by
 *  appropriate customization. It creates a view architecture which
 *  allows the separation of a document/view concept.
 */
public class ModisFrame extends JFrame {
   /**
    * This ModisFrame's singleton instance.
    */
   private static ModisFrame instance = null;
   
   private boolean doSetCornerPoints = false;
   
   /**
    * A reference to the controller for this frame.  It is a
    * Singleton, so there is only one ModisController per
    * executable instance.
    */
   private ModisController controller = ModisController.getInstance();
   private ModisModel model = ModisModel.getInstance();
   private ImageFrame img = ImageFrame.getInstance();
   /**
    * This is the structure that holds the information
    */
   private ModisDescriptor descriptor = new ModisDescriptor();
   
   /** Are we mosaicking? **/
   public int mosaic = 0;     /* 0 = no mosaic, 1 = mosaic */
   
   private StatusDialog statusDialog;

   /**
    *  IMPORTANT: Source code between BEGIN/END comment pair will be
    *  regenerated every time the form is saved. All manual changes will
    *  be overwritten.
    * 
    *  BEGIN GENERATED CODE
    *  
    *  The menu bar object.
    */
   /* member declarations */
   javax.swing.JMenuBar menuBar = new javax.swing.JMenuBar();
   /**
    * The file menu item in the menu bar.
    */
   javax.swing.JMenu jMenuFile = new javax.swing.JMenu();
   /**
    * The open input file item in the file menu.
    */
   javax.swing.JMenuItem jMenuFileOpenInputFile = new javax.swing.JMenuItem();
   /**
    * The specify output item in the file menu.
    */
   javax.swing.JMenuItem jMenuFileSpecifyOutputFile = new javax.swing.JMenuItem();
   javax.swing.JSeparator jMenuFileSeparator1 = new javax.swing.JSeparator();
   javax.swing.JMenuItem jMenuFileLoadParameters = new javax.swing.JMenuItem();
   javax.swing.JMenuItem jMenuFileSaveParameters = new javax.swing.JMenuItem();
   javax.swing.JSeparator jMenuFileSeparator2 = new javax.swing.JSeparator();
   javax.swing.JSeparator jMenuFileSeparator3 = new javax.swing.JSeparator();
   javax.swing.JSeparator jMenuFileSeparator4 = new javax.swing.JSeparator();
   javax.swing.JMenuItem jMenuFileExit = new javax.swing.JMenuItem();
   javax.swing.JMenu jMenuAction = new javax.swing.JMenu();
   javax.swing.JMenuItem jMenuActionRun = new javax.swing.JMenuItem();
   javax.swing.JMenuItem jMenuActionConvertFormat = new javax.swing.JMenuItem();
   javax.swing.JMenu jMenuHelp = new javax.swing.JMenu();
   javax.swing.JMenuItem jMenuHelpAbout = new javax.swing.JMenuItem();
   javax.swing.JLabel jLabel1 = new javax.swing.JLabel();
   
   //added on 05/21/02
   javax.swing.JMenu jMenuOptions = new javax.swing.JMenu();
   javax.swing.JMenuItem jMenuSetInputDirectory = new javax.swing.JMenuItem();
   javax.swing.JMenuItem jMenuSetOutputDirectory = new javax.swing.JMenuItem();
   javax.swing.JMenuItem jMenuSetParameterDirectory = new javax.swing.JMenuItem();
   /*Jscrollpane, Jlist for the input files
    added on 05/16/02
    */
   javax.swing.JList inputfilelist = new javax.swing.JList();
   javax.swing.JScrollPane inputfilelistscrollpane = new javax.swing.JScrollPane();
   
   javax.swing.JPanel inputPane = new javax.swing.JPanel();
   javax.swing.JTextField inputFileNameText = new javax.swing.JTextField();
   javax.swing.JButton openInputFileButton = new javax.swing.JButton();
   javax.swing.JLabel jLabel2 = new javax.swing.JLabel();
   javax.swing.JScrollPane detailScrollPane = new javax.swing.JScrollPane();
   javax.swing.JTextArea detailTextArea = new javax.swing.JTextArea();
   javax.swing.JList availableBandsList = new javax.swing.JList();
   javax.swing.JList selectedBandsList = new javax.swing.JList();
   javax.swing.JScrollPane availableBandsScrollPane = new javax.swing.JScrollPane();
   javax.swing.JScrollPane selectedBandsScrollPane = new javax.swing.JScrollPane();
   javax.swing.JScrollPane modisToolScrollPane = new javax.swing.JScrollPane();
   javax.swing.JLabel jLabel3 = new javax.swing.JLabel();
   javax.swing.JLabel jLabel4 = new javax.swing.JLabel();
   javax.swing.JButton addBandButton = new javax.swing.JButton();
   javax.swing.JButton removeBandButton = new javax.swing.JButton();
   javax.swing.JLabel jLabel5 = new javax.swing.JLabel();
   javax.swing.JComboBox spatialSubsetCombo = new javax.swing.JComboBox();
   javax.swing.JLabel jLabel6 = new javax.swing.JLabel();
   javax.swing.JLabel jLabel7 = new javax.swing.JLabel();
   javax.swing.JLabel availableBandCount = new javax.swing.JLabel();
   javax.swing.JLabel selectedBandCount = new javax.swing.JLabel();
   javax.swing.JLabel yCoordLabel = new javax.swing.JLabel();
   javax.swing.JLabel jLabel9 = new javax.swing.JLabel();
   javax.swing.JLabel jLabel10 = new javax.swing.JLabel();
   javax.swing.JLabel resolutionLabelLabel = new javax.swing.JLabel();
   javax.swing.JLabel resolutionLabel = new javax.swing.JLabel();
   javax.swing.JLabel xCoordLabel = new javax.swing.JLabel();
   
   javax.swing.JTextField pixelSizeText = new javax.swing.JTextField();
   javax.swing.JLabel pixelSizeLabel = new JLabel();
   javax.swing.JTextField outputPixelSizeUnitText = new javax.swing.JTextField();
   
   javax.swing.JTextField startYCoordLLText = new javax.swing.JTextField();
   javax.swing.JTextField startXCoordLLText = new javax.swing.JTextField();
   javax.swing.JTextField endYCoordLLText = new javax.swing.JTextField();
   javax.swing.JTextField endXCoordLLText = new javax.swing.JTextField();
   
   javax.swing.JTextField startYCoordLSText = new javax.swing.JTextField();
   javax.swing.JTextField startXCoordLSText = new javax.swing.JTextField();
   javax.swing.JTextField endYCoordLSText = new javax.swing.JTextField();
   javax.swing.JTextField endXCoordLSText = new javax.swing.JTextField();
   
   javax.swing.JTextField startYCoordXYText = new javax.swing.JTextField();
   javax.swing.JTextField startXCoordXYText = new javax.swing.JTextField();
   javax.swing.JTextField endYCoordXYText = new javax.swing.JTextField();
   javax.swing.JTextField endXCoordXYText = new javax.swing.JTextField();
   
   javax.swing.JButton viewMetadataButton = new javax.swing.JButton();
   javax.swing.JButton viewImageButton = new javax.swing.JButton();
   javax.swing.JLabel jLabel17 = new javax.swing.JLabel();
   javax.swing.JLabel jLabel12 = new javax.swing.JLabel();
   javax.swing.JPanel destinationPane = new javax.swing.JPanel();
   javax.swing.JLabel jLabel11 = new javax.swing.JLabel();
   javax.swing.JComboBox outputFormatCombo = new javax.swing.JComboBox();
   javax.swing.JLabel jLabel13 = new javax.swing.JLabel();
   javax.swing.JComboBox resamplingCombo = new javax.swing.JComboBox();
   javax.swing.JLabel jLabel14 = new javax.swing.JLabel();
   javax.swing.JComboBox projectionCombo = new javax.swing.JComboBox();
   javax.swing.JButton projectionParametersButton = new javax.swing.JButton();
   javax.swing.JButton saveOutputFileButton = new javax.swing.JButton();
   javax.swing.JLabel jLabel15 = new javax.swing.JLabel();
   javax.swing.JTextField outputFileNameText = new javax.swing.JTextField();
   javax.swing.JLabel jLabel8 = new javax.swing.JLabel();
   javax.swing.JLabel jLabel16 = new javax.swing.JLabel();
   javax.swing.JPanel commandPane = new javax.swing.JPanel();
   javax.swing.JPanel modisPane = new javax.swing.JPanel();
   javax.swing.JButton runButton = new javax.swing.JButton();
   javax.swing.JButton exitButton = new javax.swing.JButton();
   javax.swing.JButton convertFormatButton = new javax.swing.JButton();
   javax.swing.JButton loadParametersButton = new javax.swing.JButton();
   javax.swing.JButton saveParametersButton = new javax.swing.JButton();
   javax.swing.JLabel parameterFileNameLabel = new javax.swing.JLabel();
   javax.swing.JTextField parameterFileNameText  = new javax.swing.JTextField();
   
   /*  END GENERATED CODE */
   
   private int prevOutputFileType = -1;
   private int prevResamplingType = -1;
   private int prevSpatialSubsetType = -1;
   private int prevOutputProjType = -1;
   private int prevOutputPixelSizeUnits = -1;
   private String prevOutputPixelSize = "";
   private String outputMosaicFile = null;
   
   private boolean startLoadParameterFile = false;
   private boolean startLoadHeaderFile = false;
   private int startLoadFirstHeaderFile = 0;
   private boolean  bLoadHdrFile = true;
   private boolean  readInputFilesFromTmpMosaic = true;
   private boolean inputFilesEndWithHdf = false;
   private double [][]cornerPoints = new double [4][2];
   private File[] selectedfiles; // array to store the input file names
   
   /**
    * NAME: ModisFrame
    * PURPOSE:
    * Constructor - make it private to enforce singleton
    * RETURN VALUE:
    * PROGRAM HISTORY
    * Version  Date   Programmer                     Code  Reason
    * -------  ----   ---------------------------    ----  --------------------
    *                 Gregg T. Stubbendieck, Ph.D.   Java  Original development
    *
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    *
    * PROJECT: ModisTool
    * NOTES:
    *
    */
   private ModisFrame() {
   }
   
   /**
    * NAME: browseInputFiles()
    * PURPOSE:
    *   Displays a file chooser dialog box for the user to select
    *   an input file. If a file is selected (i.e., the dialog box
    *   is no cancelled), the name is passed to the controller.
    * RETURN VALUE:
    * String
    * PROGRAM HISTORY
    * Version  Date    Programmer                    Code  Reason
    * -------  ----    ---------------------------   ----  --------------------
    *                  Gregg T. Stubbendieck, Ph.D.  Java  Original development
    *      17/05/2002  Karthik.P                     Java  Modifications
    *
    *
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    *
    * PROJECT: ModisTool
    * NOTES: Multiple selection enabled in the JFileChooser
    *        Input files populate a list box
    *
    */
   private String browseInputFiles() {
      JFileChooser chooser;
      File temp = new File(System.getProperty("user.home") +
              System.getProperty("file.separator") +
              "DefaultInputFileLocation");
      
      if(temp.exists()){
         chooser = new JFileChooser(new SpecifyDirectory().getInputPathName());
      } else{
         chooser = new JFileChooser(".");
      }
      
      try {
         //added on 05/18/02
         chooser.setMultiSelectionEnabled(true);
         /* if any files have been selected then set them as selected
          * The users now want to always go to the default input directory.
          * The following line was commented out to allow this.
          */ 
         // chooser.setSelectedFiles(selectedfiles);
      } catch (Exception e) {}
      
      chooser.setFileFilter(new javax.swing.filechooser.FileFilter() {
         public boolean accept(java.io.File f) {
            final String fileName = f.getName().toLowerCase();
            return fileName.endsWith(".hdr") ||
                    fileName.endsWith(".hdf") ||
                    f.isDirectory();
         }
         
         public String getDescription() {
            return "*.hdr; *.hdf";
         }
      });
      
      int chooseResult = chooser.showOpenDialog(this);
      if ( chooseResult == JFileChooser.APPROVE_OPTION ) {  //added on 05/18/02
         selectedfiles = chooser.getSelectedFiles();
         if (selectedfiles.length <= 0) {
            complain("OpenInputFile",
                    "This version of the MRT requires Java version 1.5 " +
                    "or higher.");
            return "";
         }
         
         for (int i = 0; i < selectedfiles.length; ++i) {
            if ( selectedfiles[i].toString().toLowerCase().endsWith(".hdr") ||
                    selectedfiles[i].toString().toLowerCase().endsWith(".hdf"))
            { 
            // added by kp 5/17/02, check the existence of the input file in
            // the specified directory.
               if (!selectedfiles[i].exists()) {
                  complain("OpenInputFile", "Input File does not exist!");
                  return "";
               }
            }
         }
         
         if ( selectedfiles.length > 1)
            mosaic = 1;
         else
            mosaic = 0;
         
         if ( selectedfiles[0].toString().toLowerCase().endsWith(".hdr") ||
                 selectedfiles[0].toString().toLowerCase().endsWith(".hdf")) {
            // check the existence of the input file in the specified directory
            ChooserListModel inputlm = new ChooserListModel();
            inputfilelist.setModel(inputlm);
            inputfilelist.setEnabled(true);
            
            if(selectedfiles.length > 1) {
               for(int i = 0; i<selectedfiles.length; i++)
                  inputlm.addElement(selectedfiles[i]);
               
               /*createMosaicFile creates a file called TmpMosaic.prm and
                 stores the selected file*/
               img.createMosaicFile(selectedfiles, selectedfiles.length);
               
               /* creates a file TmpHdr.hdr */
               String tmpHdrFile = "TmpHdr.hdr";
               img.convertAllHdrToTmpHdr();
               setInputFileName(tmpHdrFile);
               return tmpHdrFile;
            } else {
               inputlm.addElement(selectedfiles[0]);
               
               /*creates a file called TmpMosaic.prm and stores the selected
                file*/
               setInputFileName(selectedfiles[0].toString());
               return selectedfiles[0].toString();
            }
         } else {
            complain("Input file name error",
                    "Invalid file extension -- must be '.hdr', '.hdf'");
            return "";
         }
      }
      return "";
   }
   
   /**
    * NAME:               browseOutputFiles()
    * PURPOSE:
    *   Displays a file chooser dialog box for the user to select
    *   an output file. If a file is selected (i.e., the dialog box
    *   is no cancelled), the name is passed to the controller.
    * RETURN VALUE:
    *   String
    * PROGRAM HISTORY
    *   Version  Date  Programmer                    Code  Reason
    *   -------  ----  ---------------------------   ----  --------------------
    *                  Gregg T. Stubbendieck, Ph.D.  Java  Original development
    *
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    *
    * PROJECT: ModisTool
    * NOTES:
    *
    */
   private String browseOutputFiles() {
      
      JFileChooser chooser;
      File temp = new File(System.getProperty("user.home") +
              System.getProperty("file.separator") +
              "DefaultOutputFileLocation");
      
      if(temp.exists()){
         chooser = new JFileChooser(new SpecifyDirectory().getOutputPathName());
      } else{
         chooser = new JFileChooser(".");
      }
      
      chooser.setDialogType(JFileChooser.CUSTOM_DIALOG);
      chooser.setApproveButtonText("Select");
      chooser.setApproveButtonToolTipText("Set output file name");
      chooser.setFileFilter(new javax.swing.filechooser.FileFilter() {
         public boolean accept(java.io.File f) {
            final String fileName = f.getName().toLowerCase();
            return fileName.endsWith(".hdr") ||
                    fileName.endsWith(".hdf") ||
                    fileName.endsWith(".tif") ||
                    f.isDirectory();
         }
         
         public String getDescription() {
            return "*.hdr, *.hdf, *.tif";
         }
      });
      int chooseResult = chooser.showDialog(this,"Select");
      
      if ( chooseResult == JFileChooser.APPROVE_OPTION ) {
         try {
            String s = chooser.getSelectedFile().getCanonicalPath();
            if ( s.toLowerCase().endsWith(".hdr") ||
                    s.toLowerCase().endsWith(".hdf") ||
                    s.toLowerCase().endsWith(".tif") ) {
               setOutputFileName(s);
               return s;
            } else {
               complain("Input file name error",
                       "Please choose a valid file extension " +
                       "(.hdr, .hdf, .tif)!");
               return "";
            }
         } catch(java.io.IOException e) {/* oops */}
      }
      return "";
   }
   
   
   /**
    * NAME: browseParameterFiles
    * PURPOSE:
    *   Displays a file chooser dialog box for the user to select
    *   a parameter (.prm) file. If a file is selected (i.e., the dialog box
    *   is not cancelled), the name is passed to the controller.
    * RETURN VALUE:
    *   String
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    *
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    *
    * PROJECT: ModisTool
    * NOTES:
    *
    */
   private String browseParameterFiles(boolean forRead) {
      int chooseResult = 0;
      JFileChooser chooser;
      File temp = new File(System.getProperty("user.home") +
              System.getProperty("file.separator") +
              "DefaultParameterFileLocation");
      if(temp.exists()){
         chooser  = new JFileChooser(
                           new SpecifyDirectory().getParameterPathName());
      } else{
         chooser = new JFileChooser(".");
      }
      
      chooser.setFileFilter(new javax.swing.filechooser.FileFilter() {
         public boolean accept(java.io.File f) {
            final String fileName = f.getName().toLowerCase();
            return fileName.endsWith(".prm") ||
                    f.isDirectory();
         }
         
         public String getDescription() {
            return "*.prm";
         }
      });
      
      if ( forRead ) {
         chooser.setDialogTitle("Open Parameter File");
         chooseResult = chooser.showOpenDialog(this);
      } else {
         chooser.setDialogTitle("Save Parameter File");
         chooseResult = chooser.showSaveDialog(this);
      }
      
      if ( chooseResult == JFileChooser.APPROVE_OPTION ) {
         try {
            String pathName = chooser.getSelectedFile().getCanonicalPath();
            final String lcPathName = pathName.toLowerCase();
            
            int dotIndex = lcPathName.lastIndexOf('.');
            
            if ( dotIndex > 0) {
               if ( lcPathName.endsWith(".prm") ) {
                  pathName = pathName.substring(0, dotIndex);
                  pathName = pathName + ".prm";
               }
            } else {
               pathName = pathName + ".prm";
            }
            
            //sj 4/01/01
            if ( forRead ) // load the parameter file
            {
            /* check the existence of the parameter file, if not exist, do not
               set the parameter file name in the parameter file text box*/
               File prmFile = new File(pathName);
               if ( !prmFile.exists()) {
                  complain("Error reading .prm file",
                          "The selected .prm file doesn't exist!");
                  return "";
               }
               setParameterFileName(pathName);
               return pathName;
            } else {
               File prm = new File(pathName);
               
               if(prm.exists()) {
                  JOptionPane outputOverwrite = new JOptionPane(
                          "The selected .prm file already exists. Overwrite?",
                          JOptionPane.QUESTION_MESSAGE,
                          JOptionPane.YES_NO_OPTION);
                  JDialog dialog = outputOverwrite.createDialog(this,
                          "Question");
                  dialog.setVisible(true);
                  
                  int optionValue =
                          ((Integer) outputOverwrite.getValue()).intValue();
                  /* if user chose to overwite the output file */
                  if (optionValue == JOptionPane.YES_OPTION)
                  {
                     parameterFileNameText.setText(pathName);
                     setParameterFileName(pathName);
                     return pathName;
                  } else if (optionValue == JOptionPane.NO_OPTION)
                  {
                     /* promt user to input another output filename */
                     JOptionPane.showMessageDialog(this,
                             "Select another file, please...");
                  }
               } else {
                  parameterFileNameText.setText(pathName);
                  setParameterFileName(pathName);
                  return pathName;
               }
            }
         }
         catch(java.io.IOException e) {/* oops */}
      }
      return "";
   }
   
   /**
    * NAME: closeAndDispose()
    * PURPOSE:
    *   To clean up and ready to exit
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    *
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    *
    * PROJECT: ModisTool
    * NOTES:
    *
    */
   public void closeAndDispose() {
      setVisible(false);
      dispose();
      System.exit(0);
   }
   
   /**
    * NAME:       complain
    * PURPOSE:
    *   To pop up message box when exception occurs
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    * 
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    * 
    * PROJECT: ModisTool
    * NOTES:
    * @param title The title of the message box that will be displayed.
    * @param text The text of the message box.
    */
   public void complain(String title, String text) {
      JOptionPane msg = new JOptionPane(text);
      JDialog dialog = msg.createDialog(this,title);
      dialog.setVisible(true);
   }
   
   /**
    *Delete all the Files after executing the resampler.
    *The files deleted are TmpMosaic.prm, TmpMosaic.hdf, TmpMosaic.hdr and
    *all ancillary files created along with TmpMosaic.hdr
    *
    */
   public void deleteAllFiles() {
      /**Delete the TmpMosaic.prm file*/
      File temp = new File("TmpMosaic.prm");
      if (temp.exists())
         temp.delete();
      
      /* Determine the output file path for TmpMosaic.hdf */
      File outputFileExist = new File(model.getOutputFilename());
      String outputFileName = outputFileExist.getAbsolutePath();
      int slashIndex = outputFileName.lastIndexOf(
              System.getProperty("file.separator"));
      String outputFilePath = outputFileName.substring(0, slashIndex+1);
      
      /*Delete the TmpMosaic.hdf file*/
      File tmp = new File(outputFilePath + "TmpMosaic.hdf");
      if(tmp.exists())
         tmp.delete();
      
      /*Delete TmpMosaic.hdr file*/
      File tmpM = new File(outputFilePath + "TmpMosaic.hdr");
      if(tmpM.exists())
         tmpM.delete();
      
      final ChooserListModel inputLm =
              (ChooserListModel) selectedBandsList.getModel();
      
      /*Delete TmpMosaic....dat files*/
      for(int i = 0 ; i < inputLm.getSize(); i++){
         String s = inputLm.getElementAt(i).toString();
         File tempFiles = new File( outputMosaicFile + "." + s + ".dat");
         if (tempFiles.exists())
            tempFiles.delete();
      }
   }
   
   /**
    * NAME: displayStatus
    * PURPOSE:
    *   To display Status in a status dialog
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    * 
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    * 
    * PROJECT: ModisTool
    * NOTES:
    * @param s The string to display in the status dialog.
    */
   public void displayStatus(String s) {
      statusDialog.append(s);
   }
   
   /**
    * NAME:       showStatusWindow
    * PURPOSE:
    *   To pop up a status dialog
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    *
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    *
    * PROJECT: ModisTool
    * NOTES:
    */
   public void showStatusWindow() {
      statusDialog.setVisible(true);
   }
   
   /**
    * NAME: getInstance
    * PURPOSE:
    *   To garantee that there is one and only instance of ModisFrame object
    *   floating around.
    * RETURN VALUE:
    *   ModisFrame
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    * 
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    * 
    * PROJECT: ModisTool
    * NOTES:  This is the "class" member function, can be invoked by class name
    * @return This instance of the ModisFrame.
    */
   public static ModisFrame getInstance() {
      if ( instance == null ) {
         instance = new ModisFrame();
      }
      return instance;
   }
   
   
   /**
    * NAME:       getOutputFormat
    * PURPOSE:
    *   To get Output Format
    * RETURN VALUE:
    *   String
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    * 
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    * 
    * PROJECT: ModisTool
    * NOTES:
    * @return The output format.
    */
   final public String getOutputFormat() {
      Object item = outputFormatCombo.getSelectedItem();
      final String retval;
      
      if ( item == null ) {
         retval = "";
      } else {
         retval = item.toString();
      }
      return retval;
   }
   
   /**
    * NAME: getResamplingType
    * PURPOSE:
    *   To get Resampling Type
    * RETURN VALUE: String
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    * 
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    * 
    * 
    * PROJECT:        ModisTool
    * NOTES:
    * @return The Resampling Type.
    */
   final public String getResamplingType() {
      Object item = resamplingCombo.getSelectedItem();
      final String retval;
      
      if ( item == null ) {
         retval = "";
      } else {
         retval = item.toString();
      }
      return retval;
   }
   
   /**
    * NAME: getSpatialSubsetType
    * PURPOSE:
    *   To get Spatial Subset Type
    * RETURN VALUE:   String
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    * 
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    * 
    * 
    * PROJECT: ModisTool
    * NOTES:
    * @return The Spatial Subset Type
    */
   final public String getSpatialSubsetType() {
      Object item = spatialSubsetCombo.getSelectedItem();
      final String retval;
      
      if ( item == null ) {
         retval = "";
      } else {
         retval = item.toString();
      }
      return retval;
   }
   
   /**
    * NAME: getProjectionType
    * PURPOSE:
    *   To get Projection Type
    * RETURN VALUE: String
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    * 
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    * 
    * PROJECT: ModisTool
    * NOTES:
    * @return The Projection Type.
    */
   final public String getProjectionType() {
      Object item = projectionCombo.getSelectedItem();
      final String retval;
      
      if ( item == null ) {
         retval = "";
      } else {
         retval = item.toString();
      }
      return retval;
   }
   
   /**
    * NAME: inputFileNameTextActivity
    * PURPOSE:
    *   To monitor input File Name Text Activity
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    *
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    *
    * PROJECT: ModisTool
    * NOTES:
    *
    */
   private void inputFileNameTextActivity() {
      openInputFileButton.setEnabled(inputFileNameText.getText().length() > 0);
      model.setInputFilename(inputFileNameText.getText());
   }
   
   /**
    * NAME: loadHdfeosData()
    * PURPOSE:
    *   To Load HDF EOS data
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    *
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    *
    * PROJECT: ModisTool
    * NOTES:
    *
    */
   public void loadHdfeosData() {
      String hdrFilename = controller.convertHdfToHdr();
      String prevInputFile = model.getInputFilename();
      
      model.setInputFilename(hdrFilename);
      fillGUIInputFromHeaderData();
      
      // JMW 02/27/01 - try deleting TmpHdr.hdr
      File tempFile= new File(hdrFilename);
      tempFile.delete();
      
      model.setInputFilename(prevInputFile);
      setSaveRunEnablements();
   }
   
   
   /**
    * NAME: fillGUIInputFromHeaderData()
    * PURPOSE:
    *   To fill in the appropriate visible components on GUI based on the data
    * from header data.
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    *                   Gail Schmidt                       Ellipsoids are now
    *                                                      datums and supported
    *                                                      for all projections
    *
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    *
    * PROJECT: ModisTool
    * NOTES:
    *
    */
   public void fillGUIInputFromHeaderData() {
      String eol = System.getProperty("line.separator");
      doSetCornerPoints = false;
      
      /**
       * Populate the band list.  HDR files don't have
       * the selected/not selected array, so all bands are
       * selected.
       */
      selectedBandsList.setEnabled(true);
      ChooserListModel selectedLm = new ChooserListModel();
      selectedBandsList.setModel(selectedLm);
      availableBandsList.setEnabled(true);
      availableBandsList.setModel(new ChooserListModel());
      
      final Vector bands = model.getBandList();
      Enumeration enumer = bands.elements();
      while ( enumer.hasMoreElements()) {
         selectedLm.addElement(enumer.nextElement());
      }
      
      addBandButton.setEnabled(false);
      removeBandButton.setEnabled(false);
      
      selectedBandCount.setText(Integer.toString(bands.size()));
      availableBandCount.setText("0");
      
      spatialSubsetCombo.setEnabled(true);
      
      startXCoordLSText.setText("");
      startYCoordLSText.setText("");
      endXCoordLSText.setText("");
      endYCoordLSText.setText("");
      
      startXCoordLLText.setText("");
      startYCoordLLText.setText("");
      endXCoordLLText.setText("");
      endYCoordLLText.setText("");
      
      startXCoordXYText.setText("");
      startYCoordXYText.setText("");
      endXCoordXYText.setText("");
      endYCoordXYText.setText("");
      
      startXCoordLSText.setEnabled(true);
      startYCoordLSText.setEnabled(true);
      endXCoordLSText.setEnabled(true);
      endYCoordLSText.setEnabled(true);
      
      startXCoordLLText.setEnabled(true);
      startYCoordLLText.setEnabled(true);
      endXCoordLLText.setEnabled(true);
      endYCoordLLText.setEnabled(true);
      
      startXCoordXYText.setEnabled(true);
      startYCoordXYText.setEnabled(true);
      endXCoordXYText.setEnabled(true);
      endYCoordXYText.setEnabled(true);
      
      //Need to view Metadata for the selected file only
      viewMetadataButton.setEnabled(false);
      viewImageButton.setEnabled(true);
      saveOutputFileButton.setEnabled(true);
      jMenuFileSpecifyOutputFile.setEnabled(true);
      
      if ( model.getSpatialSubsetType() == ModisModel.SPACE_LAT_LON ) {
         model.setSpatialSubsetType(1);
         startYCoordLLText.setText(
                 Double.toString(model.getCornerCoordinate(0,0)));
         startXCoordLLText.setText(
                 Double.toString(model.getCornerCoordinate(0,1)));
         
         endYCoordLLText.setText(
                 Double.toString(model.getCornerCoordinate(3,0)));
         endXCoordLLText.setText(
                 Double.toString(model.getCornerCoordinate(3,1)));
         
         startYCoordLSText.setText("0");
         startXCoordLSText.setText("0");
         endYCoordLSText.setText(Integer.toString(model.getMaximumLines()-1));
         endXCoordLSText.setText(Integer.toString(model.getMaximumSamples()-1));
         resolutionLabel.setText(
                 Integer.toString(model.getMaximumLines()) + "x" +
                 Integer.toString(model.getMaximumSamples()));
         
         startXCoordLSText.setVisible(false);
         startYCoordLSText.setVisible(false);
         endXCoordLSText.setVisible(false);
         endYCoordLSText.setVisible(false);
         
         startXCoordXYText.setVisible(false);
         startYCoordXYText.setVisible(false);
         endXCoordXYText.setVisible(false);
         endYCoordXYText.setVisible(false);
         
         startXCoordLLText.setVisible(true);
         startYCoordLLText.setVisible(true);
         endXCoordLLText.setVisible(true);
         endYCoordLLText.setVisible(true);
      } else if(model.getSpatialSubsetType() == ModisModel.SPACE_LINE_SAMPLE) {
         model.setSpatialSubsetType(2);
         startYCoordLSText.setText(
                 Long.toString((long)model.getCornerCoordinate(0,0)));
         startXCoordLSText.setText(
                 Long.toString((long)model.getCornerCoordinate(0,1)));
         
         endYCoordLSText.setText(
                 Long.toString((long)model.getCornerCoordinate(3,0)));
         endXCoordLSText.setText(
                 Long.toString((long)model.getCornerCoordinate(3,1)));
         
         startYCoordLLText.setText(
                 Double.toString(model.getInputImageExtent(0)[0]));
         startXCoordLLText.setText(
                 Double.toString(model.getInputImageExtent(0)[1]));
         
         endYCoordLLText.setText(
                 Double.toString(model.getInputImageExtent(3)[0]));
         endXCoordLLText.setText(
                 Double.toString(model.getInputImageExtent(3)[1]));
         
         startXCoordLSText.setVisible(true);
         startYCoordLSText.setVisible(true);
         endXCoordLSText.setVisible(true);
         endYCoordLSText.setVisible(true);
         
         startXCoordLLText.setVisible(false);
         startYCoordLLText.setVisible(false);
         endXCoordLLText.setVisible(false);
         endYCoordLLText.setVisible(false);
         
         startXCoordXYText.setVisible(false);
         startYCoordXYText.setVisible(false);
         endXCoordXYText.setVisible(false);
         endYCoordXYText.setVisible(false);
      } else if ( model.getSpatialSubsetType() == ModisModel.SPACE_PROJ_XY ) {
         model.setSpatialSubsetType(3);
         startYCoordXYText.setText("0.0");
         startXCoordXYText.setText("0.0");
         endYCoordXYText.setText("0.0");
         endXCoordXYText.setText("0.0");
         
         startXCoordXYText.setVisible(true);
         startYCoordXYText.setVisible(true);
         endXCoordXYText.setVisible(true);
         endYCoordXYText.setVisible(true);
         
         startXCoordLLText.setVisible(false);
         startYCoordLLText.setVisible(false);
         endXCoordLLText.setVisible(false);
         endYCoordLLText.setVisible(false);
         
         startXCoordLSText.setVisible(false);
         startYCoordLSText.setVisible(false);
         endXCoordLSText.setVisible(false);
         endYCoordLSText.setVisible(false);
      }
      
      setSpatialSubsetType(model.getSpatialSubsetType());
      
      detailTextArea.setText("");
      
      detailTextArea.append("Input Projection Type: ");
      detailTextArea.append(model.getInputProjectionTypeString());
      detailTextArea.append(eol);
      
      detailTextArea.append("Projection Parameters: (");
      for (int i=0; i<15; ++i) {
         detailTextArea.append(" ");
         String value = Double.toString(model.getInputProjectionParameter(i));
         if (value.endsWith(".0")) {
            value = value.substring(0,value.length()-2);
         }
         detailTextArea.append(value);
      }
      detailTextArea.append(" )" + eol);
      
      detailTextArea.append("Total Number of Bands: ");
      detailTextArea.append(Integer.toString(model.getNumBands()));
      detailTextArea.append(eol);
      
      detailTextArea.append("Data Type: ( " );
      String []ss = model.getInputDataTypeArray();
      int j;
      for (j = 0; j < model.getNumBands()-1; j++)
         detailTextArea.append(ss[j] + ", ");
      detailTextArea.append(ss[j] + " )" + eol);
      
      detailTextArea.append("Pixel size: ( " );
      double []dd = model.getInputPixelSizeArray();
      for (j = 0; j < model.getNumBands(); j++) {
         if ( j > 0 )
            detailTextArea.append(", ");
         
         /* convert to only one decimal number */
         double var = (double)((int)(dd[j] * 10 + 0.5)) / 10.0;
         String value = Double.toString(var);
         detailTextArea.append(value);
      }
      detailTextArea.append(" )" + eol);
      
      detailTextArea.append("Number of lines: ( " );
      int[] ii = model.getInputNumOfLinesArray();
      for (j = 0; j < model.getNumBands()-1; j++)
         detailTextArea.append(ii[j] + ", ");
      detailTextArea.append(ii[j] + " )" + eol);
      
      detailTextArea.append("Number of samples: ( " );
      ii = model.getInputNumOfSamplesArray();
      for (j = 0; j < model.getNumBands()-1; j++)
         detailTextArea.append(ii[j] + ", ");
      detailTextArea.append(ii[j] + " )" + eol);
      
      
      detailTextArea.append("Lat/Long of Upper-Left Corner: ( ");
      detailTextArea.append(
              Double.toString(model.getInputImageExtent(0)[0]));
      detailTextArea.append(" ");
      detailTextArea.append(
              Double.toString(model.getInputImageExtent(0)[1]));
      detailTextArea.append(" )" + eol);
      
      detailTextArea.append("Lat/Long of Upper-Right Corner: ( ");
      detailTextArea.append(
              Double.toString(model.getInputImageExtent(1)[0]));
      detailTextArea.append(" ");
      detailTextArea.append(
              Double.toString(model.getInputImageExtent(1)[1]));
      detailTextArea.append(" )" + eol);
      
      detailTextArea.append("Lat/Long of Lower-Left Corner: ( ");
      detailTextArea.append(
              Double.toString(model.getInputImageExtent(2)[0]));
      detailTextArea.append(" ");
      detailTextArea.append(
              Double.toString(model.getInputImageExtent(2)[1]));
      detailTextArea.append(" )" + eol);
      
      detailTextArea.append("Lat/Long of Lower-Right Corner: ( ");
      detailTextArea.append(
              Double.toString(model.getInputImageExtent(3)[0]));
      detailTextArea.append(" ");
      detailTextArea.append(
              Double.toString(model.getInputImageExtent(3)[1]));
      detailTextArea.append(" )" + eol);
      
      // Add the datum if present
      if(model.check()) {
         detailTextArea.append("Datum: ");
         detailTextArea.append(model.getInputDatum());
         detailTextArea.append(eol);
         model.uncheck();
      }
      
      // Add the utm zone if present
      if(model.getInputProjectionTypeString().equals("UTM")) {
         //bad use of method name.
         if(model.check1()) {
            detailTextArea.append("UTM_Zone:" + model.getInputUTMZone());
            detailTextArea.append(eol);
            model.uncheck1();
         }
      }
      
      
      detailTextArea.append(eol);
      detailTextArea.setCaretPosition(0);
      
      if ( model.getSpatialSubsetType() == ModisModel.SPACE_LAT_LON) {
         startYCoordLLText.setText(
                 Double.toString(model.getCornerCoordinate(0,0)));
         startXCoordLLText.setText(
                 Double.toString(model.getCornerCoordinate(0,1)));
         
         endYCoordLLText.setText(
                 Double.toString(model.getCornerCoordinate(3,0)));
         endXCoordLLText.setText(
                 Double.toString(model.getCornerCoordinate(3,1)));
         
         startXCoordLSText.setVisible(false);
         startYCoordLSText.setVisible(false);
         endXCoordLSText.setVisible(false);
         endYCoordLSText.setVisible(false);
         
         startXCoordXYText.setVisible(false);
         startYCoordXYText.setVisible(false);
         endXCoordXYText.setVisible(false);
         endYCoordXYText.setVisible(false);
         
         startXCoordLLText.setVisible(true);
         startYCoordLLText.setVisible(true);
         endXCoordLLText.setVisible(true);
         endYCoordLLText.setVisible(true);
      } else if (model.getSpatialSubsetType() == ModisModel.SPACE_LINE_SAMPLE){
         startYCoordLSText.setText(
                 Long.toString((long)model.getCornerCoordinate(0,0)));
         startXCoordLSText.setText(
                 Long.toString((long)model.getCornerCoordinate(0,1)));
         
         endYCoordLSText.setText(
                 Long.toString((long)model.getCornerCoordinate(3,0)));
         endXCoordLSText.setText(
                 Long.toString((long)model.getCornerCoordinate(3,1)));
         
         startXCoordLSText.setVisible(true);
         startYCoordLSText.setVisible(true);
         endXCoordLSText.setVisible(true);
         endYCoordLSText.setVisible(true);
         
         startXCoordLLText.setVisible(false);
         startYCoordLLText.setVisible(false);
         endXCoordLLText.setVisible(false);
         endYCoordLLText.setVisible(false);
         
         startXCoordXYText.setVisible(false);
         startYCoordXYText.setVisible(false);
         endXCoordXYText.setVisible(false);
         endYCoordXYText.setVisible(false);
      } else if ( model.getSpatialSubsetType() == ModisModel.SPACE_PROJ_XY ) {
         startYCoordXYText.setText(
                 Double.toString(model.getCornerCoordinate(0,0)));
         startXCoordXYText.setText(
                 Double.toString(model.getCornerCoordinate(0,1)));
         
         endYCoordXYText.setText(
                 Double.toString(model.getCornerCoordinate(3,0)));
         endXCoordXYText.setText(
                 Double.toString(model.getCornerCoordinate(3,1)));
         
         startXCoordXYText.setVisible(true);
         startYCoordXYText.setVisible(true);
         endXCoordXYText.setVisible(true);
         endYCoordXYText.setVisible(true);
         
         startXCoordLLText.setVisible(false);
         startYCoordLLText.setVisible(false);
         endXCoordLLText.setVisible(false);
         endYCoordLLText.setVisible(false);
         
         startXCoordLSText.setVisible(false);
         startYCoordLSText.setVisible(false);
         endXCoordLSText.setVisible(false);
         endYCoordLSText.setVisible(false);
      }
      
      // modified by sj
      projectionParametersButton.setEnabled(true);
      
      outputFormatCombo.setEnabled(true);
      resamplingCombo.setEnabled(true);
      projectionCombo.setEnabled(true);
      
      setSaveRunEnablements();
      doSetCornerPoints = true;
      parameterFileNameText.setText("");
     
      /* 
       * if (! startLoadParameterFile )    // if not loading parameter file
       * { }
       */
   }
   
   
   /**
    * NAME: fillGUIFromParamFile()
    * PURPOSE:
    *   To load parameter data and fill in the GUI.
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D  Java  Original development
    *
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    *
    * PROJECT: ModisTool
    * NOTES:
    *
    */
   public void fillGUIFromParamFile() {
      fillGUIInputFromHeaderData();
      final ChooserListModel slm =
              (ChooserListModel)selectedBandsList.getModel();
      final ChooserListModel alm =
              (ChooserListModel)availableBandsList.getModel();
      
      // inputFileNameText.setText(model.getInputFilename());
      String fileName;
      ChooserListModel inputlm = new ChooserListModel();
      inputfilelist.setModel(inputlm);
      inputfilelist.setEnabled(true);
      
      /**Read TmpMosaic.prm to get the input file names*/
      RandomAccessFile raf;
      if(new File("TmpMosaic.prm").exists()) {
         try {
            raf = new RandomAccessFile("TmpMosaic.prm", "rw");
            
            while((fileName = raf.readLine())!= null){
               inputlm.addElement(fileName);
               
            }
         } catch (FileNotFoundException e) {
            complain("fillGUIFromParamFile",
                    "Error: TmpMosaic.prm does not exist");
            return;
         } catch(IOException e) {
            complain("fillGUIFromParamFile",
                    "Error: TmpMosaic.prm does not exist");
            return;
         }
      } else{
         inputlm.addElement(model.getInputFilename());
      }
      
      /*
       * Set spectral band selections to reflect parameter file settings.
       * Header file reader just puts everything in the selected list by
       * default, so we have to move unselected bands out.
       *
       * get all of the bands from selected list, actually get all of bands by
       * default
       */
      final Vector bandsList = model.getBandList();
      
      //for (int i=model.getNumBands()-1; i>=0; --i) /* in the reverse order */
      for (int i = 0; i < model.getNumBands(); i++)  /* in the normal order */
      {
         /* if not selected, then remove it accordingly */
         if ( !model.isSelectedBand(i) ) {
            final String notSelectedBandName = (String)bandsList.elementAt(i);
            int bandIndex = -1;
            Enumeration enumer = bandsList.elements();
            
            SEARCH_LOOP:
               for (int listIndex = 0; listIndex < slm.getSize(); listIndex++){
                  String listValue = (String)slm.getElementAt(listIndex);
                  if (notSelectedBandName.equals(listValue)) {
                     bandIndex = listIndex;
                     break SEARCH_LOOP;
                  }
               }
               if (bandIndex >= 0) /* if already exists in the selected list */
               {
                  final Object item = slm.getElementAt(bandIndex);
                  /* remove from selected list */
                  slm.removeElementAt(bandIndex);
                  /* add it into the available list */
                  alm.addElement(item);
               }
         }
         parameterFileNameText.setText(model.getParameterFilename());
      }
      selectedBandCount.setText(Integer.toString(slm.getSize()));
      availableBandCount.setText(Integer.toString(alm.getSize()));
      
      // ready to load the output-panel side values from parameter file,
      // i.e. cathd.prm
      fillGUIOutputPanelFromParamFile();
   }
   
   /**
    * NAME: fillGUIOutputPanelFromParamFile()
    * PURPOSE:
    *   To fill the output panel only based on the data from parameter file
    * RETURN VALUE: void
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Shujing Jia                  Java  Refactoring
    *
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    */
   private void fillGUIOutputPanelFromParamFile() {
      //  set the default pixel size unit (GEO--DEGREES, others--METERS) 
      outputFileNameText.setText(model.getOutputFilename());
      
      int outputFormat = model.getOutputFormat();
      ComboBoxModel outputFormatCbm = outputFormatCombo.getModel();
      final int numOutputFormats = outputFormatCbm.getSize();
      OUTPUT_FORMAT_LOOP:
         for (int i=0; i< numOutputFormats; ++i) {
            final ModisModel.TextIdPair item =
                    (ModisModel.TextIdPair)outputFormatCbm.getElementAt(i);
            if (item.getId() == outputFormat) {
               outputFormatCombo.setSelectedIndex(i);
               break OUTPUT_FORMAT_LOOP;
            }
         }
         prevOutputFileType= outputFormatCombo.getSelectedIndex();
         
         // fill in resampling combox
         int resamplingType = model.getResamplingType();
         ComboBoxModel resamplingTypeCbm = resamplingCombo.getModel();
         final int numResamplingTypes = resamplingTypeCbm.getSize();
         RESAMPLING_TYPE_LOOP:
            for (int i=0; i< numResamplingTypes; ++i) {
               final ModisModel.TextIdPair item =
                       (ModisModel.TextIdPair)resamplingTypeCbm.getElementAt(i);
               if (item.getId() == resamplingType) {
                  resamplingCombo.setSelectedIndex(i);        // trigger event
                  break RESAMPLING_TYPE_LOOP;
               }
            }
            prevResamplingType = resamplingCombo.getSelectedIndex();
            
            // fill in Output Projection Type combox
            int projectionType = model.getOutputProjectionType();
            ComboBoxModel projectionTypeCbm = projectionCombo.getModel();
            final int numProjectionTypes = projectionTypeCbm.getSize();
            
            PROJECTION_TYPE_LOOP:
               for (int i=0; i< numProjectionTypes; ++i) {
                  final ModisModel.TextIdPair item =
                          (ModisModel.TextIdPair)
                          projectionTypeCbm.getElementAt(i);
                  if (item.getId() == projectionType) {
                     projectionCombo.setSelectedIndex(i);    // trigger event
                     break PROJECTION_TYPE_LOOP;
                  }
               }
               prevOutputProjType = projectionCombo.getSelectedIndex();
               
               setSaveRunEnablements();
               
               
               if ( model.getOutputPixelSize() !=  null) {
                  
                  pixelSizeText.setText(model.getOutputPixelSize());
                  prevOutputPixelSize = model.getOutputPixelSize();
               }
               if (model.getOutputProjectionTypeString() == "GEO")
                  outputPixelSizeUnitText.setText("degrees");
               else
                  outputPixelSizeUnitText.setText("meters");
   }
   
   /**
    * NAME: outputFileNameTextActivity
    * PURPOSE:
    *   To respond to output file name text box each time when it changes
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    *
    */
   private void outputFileNameTextActivity() {
      model.setOutputFilename(outputFileNameText.getText());
   }
   
   /**
    * NAME: outputFileNameTextActivity
    * PURPOSE:
    *   To respond to output pixel size text box each time when it changes
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    *
    */
   private void pixelSizeTextActivity() {
      String s = pixelSizeText.getText();
      for (short i = 0; i < s.length(); i++) {
         char c = s.charAt(i);
         if (c != '.') {
            if (! Character.isDigit(c)) {
               /* show a message box */
               complain("Typing error...",
                       "Pixel size field must be a number greater than zero. " +
                       "Please try again...");
               return ;
            }
         }
      }
      model.setOutputPixelSize(pixelSizeText.getText());
      prevOutputPixelSize = model.getOutputPixelSize();
   }
   
   /**
    * NAME: populateCombo
    * PURPOSE:
    *   To populate the items for combox on the GUI
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    *
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    *
    * PROJECT: ModisTool
    * NOTES:
    *
    */
   private void populateCombo(JComboBox combo,Vector v) {
      Enumeration enumer = v.elements();
      while (enumer.hasMoreElements()) {
         combo.addItem(enumer.nextElement());
      }
   }
   
   
   /**
    * NAME: setCornerPoints
    * PURPOSE:
    *   To set corner points on GUI
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D  Java  Original development
    *
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    *
    * PROJECT: ModisTool
    * NOTES:
    *
    */
   private void setCornerPoints() {
      if (doSetCornerPoints) {
         try {
            String startY;
            String startX;
            String endY;
            String endX;
            
            if (model.getSpatialSubsetType() == ModisModel.SPACE_LAT_LON) {
               startY = startYCoordLLText.getText();
               startX = startXCoordLLText.getText();
               endY = endYCoordLLText.getText();
               endX = endXCoordLLText.getText();
            } else if ( model.getSpatialSubsetType() ==
                    ModisModel.SPACE_LINE_SAMPLE) {
               startY = startYCoordLSText.getText();
               startX = startXCoordLSText.getText();
               endY = endYCoordLSText.getText();
               endX = endXCoordLSText.getText();
            } else {
               /* model.getSpatialSubsetType() == ModisModel.SPACE_PROJ_XY */
               startY = startYCoordXYText.getText();
               startX = startXCoordXYText.getText();
               endY = endYCoordXYText.getText();
               endX = endXCoordXYText.getText();
            }
            
            double[] startPt = {
               Double.valueOf(startY).doubleValue(),
               Double.valueOf(startX).doubleValue() };
            double[] endPt = {
               Double.valueOf(endY).doubleValue(),
               Double.valueOf(endX).doubleValue() };
            
            cornerPoints[0] = startPt;
            cornerPoints[3] = endPt;
            model.setCornerPoint(0,startPt);
            model.setCornerPoint(3,endPt);
            
            double[] urPt = new double[2];
            double[] llPt = new double[2];
            if (model.getSpatialSubsetType() == ModisModel.SPACE_LAT_LON) {
               urPt[0] = 0.0;
               urPt[1] = 0.0;
               llPt[0] = 0.0;
               llPt[1] = 0.0;
            } else if (model.getSpatialSubsetType() ==
                    ModisModel.SPACE_LINE_SAMPLE) {
               urPt[0] = startPt[0];
               urPt[1] = endPt[1];
               llPt[0] = endPt[0];
               llPt[1] = startPt[1];
            } else {
               /* model.getSpatialSubsetType() == ModisModel.SPACE_PROJ_XY */
               urPt[0] = 0.0;
               urPt[1] = 0.0;
               llPt[0] = 0.0;
               llPt[1] = 0.0;
            }
            
            cornerPoints[1] = urPt;
            cornerPoints[2] = llPt;
            
            model.setCornerPoint(1,urPt);
            model.setCornerPoint(2,llPt);
         } catch (NumberFormatException e) {}
         
         setSaveRunEnablements();
      }
   }
   
   
   /**
    * NAME: setInitialEnablements
    * PURPOSE:
    *   To initialize various states for components on GUI
    * RETURN VALUE:
    *
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    *
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    *
    * PROJECT: ModisTool
    * NOTES:
    *
    */
   public void setInitialEnablements() {
      jMenuFileOpenInputFile.setEnabled(true);
      jMenuFileSpecifyOutputFile.setEnabled(false);
      jMenuFileLoadParameters.setEnabled(true);
      jMenuFileSaveParameters.setEnabled(false);
      jMenuFileExit.setEnabled(true);
      jMenuActionConvertFormat.setEnabled(true);
      jMenuActionRun.setEnabled(true);
      jMenuHelpAbout.setEnabled(true);
      //Readme.setEnabled(true);
      inputFileNameText.setEnabled(true);
      inputFileNameText.setEditable(false);
      openInputFileButton.setEnabled(true);
      viewMetadataButton.setEnabled(false);
      viewImageButton.setEnabled(false);
      
      detailTextArea.setEnabled(true);
      detailTextArea.setEditable(false);
      availableBandsList.setEnabled(false);
      selectedBandsList.setEnabled(false);
      addBandButton.setEnabled(false);
      removeBandButton.setEnabled(false);
      spatialSubsetCombo.setEnabled(false);
      outputFormatCombo.setEnabled(false);
      resamplingCombo.setEnabled(false);
      projectionCombo.setEnabled(false);
      projectionParametersButton.setEnabled(false);
      saveOutputFileButton.setEnabled(false);
      jMenuFileSpecifyOutputFile.setEnabled(false);
      outputFileNameText.setEnabled(true);
      outputFileNameText.setEditable(false);
      runButton.setEnabled(false);
      convertFormatButton.setEnabled(false);
      jMenuActionConvertFormat.setEnabled(false);
      jMenuActionRun.setEnabled(false);
      exitButton.setEnabled(true);
      loadParametersButton.setEnabled(true);
      saveParametersButton.setEnabled(false);
      parameterFileNameText.setEnabled(true);
      parameterFileNameText.setEditable(false);
      
   }
   
   /**
    * NAME: setInputFileName
    * PURPOSE:
    *   To set Input File Name
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    * 
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    * 
    * PROJECT: ModisTool
    * NOTES:
    * @param value The name of the input file.
    */
   public void setInputFileName(String value) {
      
      model.setInputFilename(value);
   }
   
   /**
    * NAME: setOutputFileName
    * PURPOSE:
    *   To set up output file name
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date  Programmer                   Code  Reason
    *   -------  ----  ---------------------------  ----  --------------------
    *                  Gregg T. Stubbendieck, Ph.D. Java  Original development
    * 
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    * 
    * PROJECT: ModisTool
    * NOTES:
    * @param value The name of the output file.
    */
   public void setOutputFileName(String value) {
      outputFileNameText.setText(value);
      model.setOutputFilename(value);
      setOutputFileNameExtension();
   }
   
   /**
    * NAME: setOutputFileNameExtension
    * PURPOSE:
    *   To parse and set Output File Name Extension
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    *
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    *
    * PROJECT: ModisTool
    * NOTES:
    *
    */
   public void setOutputFileNameExtension() {
      int format = model.getOutputFormat();
      String extension;
      switch (format) {
         case ModisModel.MULTIFILE:
            extension = ".hdr";
            break;
         case ModisModel.HDFEOS:
            extension = ".hdf";
            break;
         case ModisModel.GEOTIFF:
            extension = ".tif";
            break;
         default:
            extension = "";
      }
      
      String outFile = model.getOutputFilename();
      if ( outFile != null) {
         
         int dotIndex = outFile.lastIndexOf('.');
         if (dotIndex >= 0 &&
                 !outFile.toLowerCase().endsWith(extension)) {
            outFile = outFile.substring(0,dotIndex) +
                    extension;
            
            outputFileNameText.setText(outFile);
            
            setOutputFileName(outFile);
         } else if ( dotIndex < 0 && outFile.length() > 0 ) {
            setOutputFileName(outFile + extension);
         }
      }
   }
   
   
   /**
    * NAME: setParameterFileName
    * PURPOSE:
    *   This is a dispatching (delegation) function.
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    * 
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    * 
    * PROJECT: ModisTool
    * NOTES:
    * @param value The name of the parameter file.
    */
   public void setParameterFileName(String value) {
      model.setParameterFilename(value);
   }
   
   /**
    * NAME: setSaveRunEnablements
    * PURPOSE:
    *   To set Save Run Enablements.  The run button will not be enabled until
    *   all output fields are filled.
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    *
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    *
    * PROJECT: ModisTool
    * NOTES:
    */
   private void setSaveRunEnablements() {
      boolean enb = false;
      
      enb = selectedBandsList.getModel().getSize() != 0;
      if (spatialSubsetCombo.getSelectedIndex() >= 0 ) {
         if ( spatialSubsetCombo.getSelectedItem().toString().startsWith(
                 "Input Line")) {
            enb &= startYCoordLSText.getText().length() != 0;
            enb &= startXCoordLSText.getText().length() != 0;
            enb &= endYCoordLSText.getText().length() != 0;
            enb &= endXCoordLSText.getText().length() != 0;
         } else if (
                 spatialSubsetCombo.getSelectedItem().toString().startsWith(
                 "Input Lat")) {
            enb &= startYCoordLLText.getText().length() != 0;
            enb &= startXCoordLLText.getText().length() != 0;
            enb &= endYCoordLLText.getText().length() != 0;
            enb &= endXCoordLLText.getText().length() != 0;
         } else if (
                 spatialSubsetCombo.getSelectedItem().toString().startsWith(
                 "Output Proj")) {
            enb &= startYCoordXYText.getText().length() != 0;
            enb &= startXCoordXYText.getText().length() != 0;
            enb &= endYCoordXYText.getText().length() != 0;
            enb &= endXCoordXYText.getText().length() != 0;
         }
      } else {
         enb = false;
      }
      enb &= outputFileNameText.getText().length() > 0;
      enb &= outputFormatCombo.getSelectedIndex() >= 0;
      enb &= resamplingCombo.getSelectedIndex() >= 0;
      enb &= projectionCombo.getSelectedIndex() >= 0;
      
      projectionParametersButton.setEnabled(true);         // ????
      runButton.setEnabled(enb);
      convertFormatButton.setEnabled(
              outputFileNameText.getText().length() > 0);
      jMenuActionConvertFormat.setEnabled(
              outputFileNameText.getText().length() > 0);
      jMenuActionRun.setEnabled(enb);
      saveParametersButton.setEnabled(enb);
      jMenuFileSaveParameters.setEnabled(enb);
      
   }
   
   /**
    * NAME: setSpatialSubsetType
    * PURPOSE:
    *   To set up Spatial Subset
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    *
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    *
    * PROJECT: ModisTool
    * NOTES:
    */
   private void setSpatialSubsetType(int subsetId) {
      model.setSpatialSubsetType(subsetId);
      
      ComboBoxModel cbm = spatialSubsetCombo.getModel();
      int numElements = cbm.getSize();
      SEARCH_LOOP: for (int i=0; i<numElements; ++i) {
         final ModisModel.TextIdPair tip;
         tip = (ModisModel.TextIdPair)cbm.getElementAt(i);
         if (tip.getId() == subsetId) {
            spatialSubsetCombo.setSelectedIndex(i);
            break SEARCH_LOOP;
         }
      }
      setSaveRunEnablements();
   }
   
   /**
    * NAME: initComponents
    * PURPOSE:
    *   To initialize the GUI components and set up layout for panels. It sets
    *   up all component's default states and event triggers.
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date  Programmer         Code  Reason
    *   -------  ----  ----------         ----  ------------------------------
    *   xxx            Gregg              Java  Original development
    *   1.00           S. Jia             Java  refining and modification
    * 
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    * 
    * PROJECT: ModisTool
    * NOTES:
    *   This snippet code uses hard-coded layout, which is very inflexible
    *   and rigid design in terms of further modification and readablity.
    *   Therefore It needs more flexibility for layout management. A good
    *   startegy for a flexible GUI is to set a "smart" layout manager,
    *   which can be the combination of several Java layout managers.
    * @throws java.lang.Exception If anything goes wrong while initializing
    * the components.
    */
   public void initComponents() throws Exception {
      /**
       *  IMPORTANT: Source code between BEGIN/END comment pair will be
       *  regenerated every time the form is saved. All manual changes will
       *  be overwritten.
       * /
       *
       * / * BEGIN GENERATED CODE */
      /* the following code sets the frame's initial state */
      menuBar.setVisible(true);
      menuBar.add(jMenuFile);
      menuBar.add(jMenuAction);
      menuBar.add(jMenuOptions);
      menuBar.add(jMenuHelp);
      
      //added the option menu 05/21/02
      
      jMenuOptions.setVisible(true);
      jMenuOptions.setText("Settings");
      jMenuOptions.setMnemonic('s');
      jMenuOptions.add(jMenuSetInputDirectory);
      jMenuOptions.add(jMenuFileSeparator3);
      jMenuOptions.add(jMenuSetOutputDirectory);
      jMenuOptions.add(jMenuFileSeparator4);
      jMenuOptions.add(jMenuSetParameterDirectory);
      
      jMenuSetInputDirectory.setVisible(true);
      jMenuSetInputDirectory.setText("Set Default Input Files Directory");
      
      jMenuFileSeparator3.setVisible(true);
      
      jMenuSetOutputDirectory.setVisible(true);
      jMenuSetOutputDirectory.setText("Set Default Output File Directory");
      
      jMenuFileSeparator4.setVisible(true);
      
      jMenuSetParameterDirectory.setVisible(true);
      jMenuSetParameterDirectory.setText(
              "Set Default Parameter File Directory");
      
      
      jMenuFile.setVisible(true);
      jMenuFile.setText("File");
      jMenuFile.setMnemonic('f');// shortcut key as f
      jMenuFile.add(jMenuFileOpenInputFile);
      jMenuFile.add(jMenuFileSpecifyOutputFile);
      jMenuFile.add(jMenuFileSeparator1);
      jMenuFile.add(jMenuFileLoadParameters);
      jMenuFile.add(jMenuFileSaveParameters);
      jMenuFile.add(jMenuFileSeparator2);
      jMenuFile.add(jMenuFileExit);
      
      jMenuFileOpenInputFile.setVisible(true);
      jMenuFileOpenInputFile.setText("Open Input File");
      jMenuFileOpenInputFile.setMnemonic('o');
      
      jMenuFileSpecifyOutputFile.setVisible(true);
      jMenuFileSpecifyOutputFile.setText("Specify Output File");
      jMenuFileSpecifyOutputFile.setMnemonic('p');
      
      jMenuFileSeparator1.setVisible(true);
      
      jMenuFileLoadParameters.setVisible(true);
      jMenuFileLoadParameters.setText("Load Parameter File");
      jMenuFileLoadParameters.setMnemonic('l');
      
      jMenuFileSaveParameters.setVisible(true);
      jMenuFileSaveParameters.setText("Save Parameter File");
      jMenuFileSaveParameters.setMnemonic('v');
      
      jMenuFileSeparator2.setVisible(true);
      
      jMenuFileExit.setVisible(true);
      jMenuFileExit.setText("Exit");
      jMenuFileExit.setMnemonic('x');
      
      jMenuAction.setVisible(true);
      jMenuAction.setText("Action");
      jMenuAction.setMnemonic('a');
      jMenuAction.add(jMenuActionRun);
      jMenuAction.add(jMenuActionConvertFormat);
      
      jMenuActionConvertFormat.setVisible(true);
      jMenuActionConvertFormat.setText("Convert Format");
      jMenuActionConvertFormat.setMnemonic('c');
      
      jMenuActionRun.setVisible(true);
      jMenuActionRun.setText("Run");
      jMenuActionRun.setMnemonic('r');
      
      jMenuHelp.setVisible(true);
      jMenuHelp.setText("Help");
      jMenuHelp.setMnemonic('h');
      jMenuHelp.add(jMenuHelpAbout);
      
      
      jMenuHelpAbout.setVisible(true);
      jMenuHelpAbout.setText("About...");
      jMenuHelpAbout.setMnemonic('b');
      
      
      JPanel innerPane = new JPanel();
      innerPane.setVisible(true);
      innerPane.setLayout(null);
      innerPane.setSize(new java.awt.Dimension(765, 725));
      
      innerPane.add(inputPane);
      innerPane.add(jLabel12);
      innerPane.add(destinationPane);
      innerPane.add(jLabel8);
      innerPane.add(jLabel16);
      innerPane.add(commandPane);
      
      jLabel12.setSize(new java.awt.Dimension(90, 20));
      jLabel12.setVisible(true);
      jLabel12.setText("Source");
      jLabel12.setLocation(new java.awt.Point(10,0));
      
      /* define input pane */
      inputPane.setSize(new java.awt.Dimension(440, 700));
      inputPane.setVisible(true);
      inputPane.setLayout(null);
      inputPane.setLocation(new java.awt.Point(0, 20));
      
      //added on 05/16/02
      inputPane.add(jLabel2);
      inputPane.add(openInputFileButton);
      inputPane.add(inputfilelistscrollpane);
      
      inputPane.add(detailScrollPane);
      inputPane.add(availableBandsScrollPane);
      inputPane.add(selectedBandsScrollPane);
      inputPane.add(jLabel3);
      inputPane.add(jLabel4);
      inputPane.add(addBandButton);
      inputPane.add(removeBandButton);
      inputPane.add(jLabel5);
      inputPane.add(spatialSubsetCombo);
      inputPane.add(jLabel6);
      inputPane.add(jLabel7);
      inputPane.add(availableBandCount);
      inputPane.add(selectedBandCount);
      inputPane.add(yCoordLabel);
      inputPane.add(jLabel9);
      inputPane.add(jLabel10);
      inputPane.add(resolutionLabel);
      inputPane.add(resolutionLabelLabel);
      inputPane.add(xCoordLabel);
      inputPane.add(startYCoordLLText);
      inputPane.add(startXCoordLLText);
      inputPane.add(endYCoordLLText);
      inputPane.add(endXCoordLLText);
      inputPane.add(startYCoordLSText);
      inputPane.add(startXCoordLSText);
      inputPane.add(endYCoordLSText);
      inputPane.add(endXCoordLSText);
      inputPane.add(startYCoordXYText);
      inputPane.add(startXCoordXYText);
      inputPane.add(endYCoordXYText);
      inputPane.add(endXCoordXYText);
      inputPane.add(viewMetadataButton);
      inputPane.add(viewImageButton);
      inputPane.add(jLabel17);
      
      jLabel17.setSize(new java.awt.Dimension(148, 20));
      jLabel17.setVisible(true);
      jLabel17.setText("Input Files");
      jLabel17.setLocation(new java.awt.Point(20, 5));
      
      
      jLabel2.setSize(new java.awt.Dimension(130, 30));
      jLabel2.setVisible(true);
      jLabel2.setText("Input File Info:");
      jLabel2.setLocation(new java.awt.Point(20, 200));
      
      openInputFileButton.setSize(new java.awt.Dimension(170, 30));
      openInputFileButton.setVisible(true);
      openInputFileButton.setText("Open Input File ...");
      openInputFileButton.setMnemonic('o');
      openInputFileButton.setToolTipText(
              "Opens an Input file of type .hdr or .hdf");
      openInputFileButton.setLocation(new java.awt.Point(265, 45));
      
      //added on 05/16/02
      inputfilelistscrollpane.setSize(new java.awt.Dimension(240, 160));
      inputfilelistscrollpane.setVisible(true);
      inputfilelist.setVisible(true);
      inputfilelist.setBorder(BorderFactory.createLineBorder(Color.black));
      inputfilelistscrollpane.setViewportView(inputfilelist);
      inputfilelistscrollpane.setLocation(new java.awt.Point(20, 30));
      
      viewMetadataButton.setSize(new java.awt.Dimension(170, 30));
      viewMetadataButton.setVisible(true);
      viewMetadataButton.setText("View Metadata ...");
      viewMetadataButton.setMnemonic('v');
      viewMetadataButton.setLocation(new java.awt.Point(265, 100));
      viewMetadataButton.setToolTipText(
              "View the Meta Data for the given Input files");
      viewMetadataButton.setEnabled(true);
      
      viewImageButton.setSize(new java.awt.Dimension(170, 30));
      viewImageButton.setVisible(true);
      viewImageButton.setText("View Selected Tile   ...");
      viewImageButton.setMnemonic('t');
      viewImageButton.setLocation(new java.awt.Point(265, 155));
      viewImageButton.setToolTipText(
              "View the Selected Tile Image for the given Input files");
      viewImageButton.setEnabled(true);
      
      
      detailScrollPane.setSize(new java.awt.Dimension(400, 145));
      detailScrollPane.setVisible(true);
      detailScrollPane.setLocation(new java.awt.Point(20, 235));
      detailScrollPane.getViewport().add(detailTextArea);
      
      detailTextArea.setVisible(true);
      detailTextArea.setBorder(BorderFactory.createLineBorder(Color.black));
      
      availableBandsScrollPane.setSize(new java.awt.Dimension(160, 100));
      availableBandsScrollPane.setVisible(true);
      availableBandsList.setVisible(true);
      availableBandsList.setBorder(BorderFactory.createLineBorder(Color.black));
      availableBandsScrollPane.setViewportView(availableBandsList);
      availableBandsScrollPane.setLocation(new java.awt.Point(20, 420));
      
      selectedBandsScrollPane.setSize(new java.awt.Dimension(160, 100));
      selectedBandsScrollPane.setVisible(true);
      selectedBandsList.setVisible(true);
      selectedBandsList.setBorder(BorderFactory.createLineBorder(Color.black));
      selectedBandsScrollPane.setViewportView(selectedBandsList);
      selectedBandsScrollPane.setLocation(new java.awt.Point(260, 420));
      
      jLabel3.setSize(new java.awt.Dimension(170, 20));
      jLabel3.setVisible(true);
      jLabel3.setText("Available Bands:        ");
      jLabel3.setLocation(new java.awt.Point(20, 400));
      
      jLabel4.setSize(new java.awt.Dimension(180, 20));
      jLabel4.setVisible(true);
      jLabel4.setText("Selected Bands:          ");
      jLabel4.setLocation(new java.awt.Point(260, 400));
      
      addBandButton.setSize(new java.awt.Dimension(75, 25));
      addBandButton.setVisible(true);
      addBandButton.setText(">>");
      addBandButton.setLocation(new java.awt.Point(183, 440));
      
      removeBandButton.setSize(new java.awt.Dimension(75, 25));
      removeBandButton.setVisible(true);
      removeBandButton.setText("<<");
      removeBandButton.setLocation(new java.awt.Point(183, 480));
      
      jLabel5.setSize(new java.awt.Dimension(120, 20));
      jLabel5.setVisible(true);
      jLabel5.setText("Spatial Subset:");
      jLabel5.setLocation(new java.awt.Point(20, 555));
      
      spatialSubsetCombo.setSize(new java.awt.Dimension(270, 25));
      spatialSubsetCombo.setVisible(true);
      spatialSubsetCombo.setLocation(new java.awt.Point(140, 555));
      
      availableBandCount.setSize(new java.awt.Dimension(60, 20));
      availableBandCount.setVisible(true);
      availableBandCount.setLocation(new java.awt.Point(160, 400));
      
      selectedBandCount.setSize(new java.awt.Dimension(60, 20));
      selectedBandCount.setVisible(true);
      selectedBandCount.setLocation(new java.awt.Point(400, 400));
      
      
      jLabel9.setSize(new java.awt.Dimension(100, 20));
      jLabel9.setVisible(true);
      jLabel9.setText("UL Corner:");
      jLabel9.setLocation(new java.awt.Point(5, 620));
      
      jLabel10.setSize(new java.awt.Dimension(100, 20));
      jLabel10.setVisible(true);
      jLabel10.setText("LR Corner:");
      jLabel10.setLocation(new java.awt.Point(5, 650));
      
      int height = 20;
      int width = 150;
      int x_start1 = 105;
      int x_start2 = 260;
      
      yCoordLabel.setSize(new java.awt.Dimension(width, height));
      yCoordLabel.setVisible(true);
      yCoordLabel.setText("Latitude");
      yCoordLabel.setLocation(new java.awt.Point(x_start1, 600));
      xCoordLabel.setSize(new java.awt.Dimension(width, height));
      xCoordLabel.setVisible(true);
      xCoordLabel.setText("Longitude");
      xCoordLabel.setLocation(new java.awt.Point(x_start2, 600));
      
      startYCoordLLText.setSize(new java.awt.Dimension(width, height));
      startYCoordLLText.setVisible(true);
      startYCoordLLText.setBorder(BorderFactory.createLineBorder(Color.black));
      startYCoordLLText.setLocation(new java.awt.Point(x_start1, 620));
      startXCoordLLText.setSize(new java.awt.Dimension(width, height));
      startXCoordLLText.setVisible(true);
      startXCoordLLText.setBorder(BorderFactory.createLineBorder(Color.black));
      startXCoordLLText.setLocation(new java.awt.Point(x_start2, 620));
      
      endYCoordLLText.setSize(new java.awt.Dimension(width, height));
      endYCoordLLText.setVisible(true);
      endYCoordLLText.setBorder(BorderFactory.createLineBorder(Color.black));
      endYCoordLLText.setLocation(new java.awt.Point(x_start1, 650));
      endXCoordLLText.setSize(new java.awt.Dimension(width, height));
      endXCoordLLText.setVisible(true);
      endXCoordLLText.setBorder(BorderFactory.createLineBorder(Color.black));
      endXCoordLLText.setLocation(new java.awt.Point(x_start2, 650));
      
      startYCoordLSText.setSize(new java.awt.Dimension(width, height));
      startYCoordLSText.setVisible(true);
      startYCoordLSText.setBorder(BorderFactory.createLineBorder(Color.black));
      startYCoordLSText.setLocation(new java.awt.Point(x_start1, 620));
      startXCoordLSText.setSize(new java.awt.Dimension(width, height));
      startXCoordLSText.setVisible(true);
      startXCoordLSText.setBorder(BorderFactory.createLineBorder(Color.black));
      startXCoordLSText.setLocation(new java.awt.Point(x_start2, 620));
      
      endYCoordLSText.setSize(new java.awt.Dimension(width, height));
      endYCoordLSText.setVisible(true);
      endYCoordLSText.setBorder(BorderFactory.createLineBorder(Color.black));
      endYCoordLSText.setLocation(new java.awt.Point(x_start1, 650));
      endXCoordLSText.setSize(new java.awt.Dimension(width, height));
      endXCoordLSText.setVisible(true);
      endXCoordLSText.setBorder(BorderFactory.createLineBorder(Color.black));
      endXCoordLSText.setLocation(new java.awt.Point(x_start2, 650));
      
      startYCoordXYText.setSize(new java.awt.Dimension(width, height));
      startYCoordXYText.setVisible(true);
      startYCoordXYText.setBorder(BorderFactory.createLineBorder(Color.black));
      startYCoordXYText.setLocation(new java.awt.Point(x_start1, 620));
      startXCoordXYText.setSize(new java.awt.Dimension(width, height));
      startXCoordXYText.setVisible(true);
      startXCoordXYText.setBorder(BorderFactory.createLineBorder(Color.black));
      startXCoordXYText.setLocation(new java.awt.Point(x_start2, 620));
      
      endYCoordXYText.setSize(new java.awt.Dimension(width, height));
      endYCoordXYText.setVisible(true);
      endYCoordXYText.setBorder(BorderFactory.createLineBorder(Color.black));
      endYCoordXYText.setLocation(new java.awt.Point(x_start1, 650));
      endXCoordXYText.setSize(new java.awt.Dimension(width, height));
      endXCoordXYText.setVisible(true);
      endXCoordXYText.setBorder(BorderFactory.createLineBorder(Color.black));
      endXCoordXYText.setLocation(new java.awt.Point(x_start2, 650));
      
      /* define destination pane */
      destinationPane.setSize(new java.awt.Dimension(310, 430));
      destinationPane.setVisible(true);
      destinationPane.setLayout(null);
      destinationPane.setLocation(new java.awt.Point(440, 20));
      destinationPane.add(jLabel11);
      destinationPane.add(outputFormatCombo);
      destinationPane.add(jLabel13);
      destinationPane.add(resamplingCombo);
      destinationPane.add(jLabel14);
      destinationPane.add(projectionCombo);
      destinationPane.add(projectionParametersButton);
      destinationPane.add(pixelSizeLabel);
      destinationPane.add(pixelSizeText);
      destinationPane.add(saveOutputFileButton);
      destinationPane.add(jLabel15);
      destinationPane.add(outputFileNameText);
      destinationPane.add(outputPixelSizeUnitText);
      
      int x_start = 15;
      int y_start = 10;
      int lab_width = 290;
      int lab_height = 30;
      int com_width = 290;
      int com_height = 25;
      int seperation = 8;
      int y = y_start;
      
      saveOutputFileButton.setSize(
              new java.awt.Dimension(lab_width, com_height));
      saveOutputFileButton.setVisible(true);
      saveOutputFileButton.setText("Specify Output File ...");
      saveOutputFileButton.setMnemonic('p');
      saveOutputFileButton.setToolTipText(
              "Specify an Output file for the given Input files");
      saveOutputFileButton.setLocation(new java.awt.Point(x_start, y));
      
      jLabel15.setSize(new java.awt.Dimension(lab_width, lab_height));
      jLabel15.setVisible(true);
      jLabel15.setText("Output File");
      y += com_height + seperation;
      jLabel15.setLocation(new java.awt.Point(x_start, y));
      
      outputFileNameText.setSize(new java.awt.Dimension(com_width, com_height));
      outputFileNameText.setVisible(true);
      outputFileNameText.setBackground(Color.white);
      outputFileNameText.setBorder(BorderFactory.createLineBorder(Color.black));
      y += lab_height;
      outputFileNameText.setLocation(new java.awt.Point(x_start, y));
      
      jLabel11.setSize(new java.awt.Dimension(lab_width, lab_height));
      jLabel11.setVisible(true);
      jLabel11.setText("Output File Type");
      y += com_height + seperation;
      jLabel11.setLocation(new java.awt.Point(x_start, y));
      
      outputFormatCombo.setSize(new java.awt.Dimension(com_width, com_height));
      outputFormatCombo.setVisible(true);
      outputFormatCombo.setBorder(
              BorderFactory.createLineBorder(Color.lightGray));
      outputFormatCombo.setBackground(Color.lightGray);
      y += lab_height;
      outputFormatCombo.setLocation(new java.awt.Point(x_start,  y));
      
      jLabel13.setSize(new java.awt.Dimension(lab_width, lab_height));
      jLabel13.setVisible(true);
      jLabel13.setText("Resampling Type");
      y += com_height + seperation;
      jLabel13.setLocation(new java.awt.Point(x_start, y));
      
      resamplingCombo.setSize(new java.awt.Dimension(com_width, com_height));
      resamplingCombo.setVisible(true);
      resamplingCombo.setBackground(Color.lightGray);
      y += lab_height;
      resamplingCombo.setLocation(new java.awt.Point(x_start, y));
      
      jLabel14.setSize(new java.awt.Dimension(lab_width, lab_height));
      jLabel14.setVisible(true);
      jLabel14.setText("Output Projection Type");
      y += com_height + seperation;
      jLabel14.setLocation(new java.awt.Point(x_start, y));
      
      projectionCombo.setSize(new java.awt.Dimension(com_width, com_height));
      projectionCombo.setVisible(true);
      projectionCombo.setBackground(Color.lightGray);
      y += lab_height;
      projectionCombo.setLocation(new java.awt.Point(x_start, y));
      
      projectionParametersButton.setSize(new java.awt.Dimension(
              com_width, com_height));
      projectionParametersButton.setVisible(true);
      projectionParametersButton.setText("Edit Projection Parameters ...");
      projectionParametersButton.setMnemonic('e');
      y += com_height + seperation + 3;
      projectionParametersButton.setLocation(new java.awt.Point(x_start, y));
      
      pixelSizeLabel.setSize(new java.awt.Dimension(lab_width, lab_height));
      pixelSizeLabel.setVisible(true);
      pixelSizeLabel.setText("Output Pixel Size");
      y += com_height + seperation;
      pixelSizeLabel.setLocation(new java.awt.Point(x_start, y));
      
      
      pixelSizeText.setSize(new java.awt.Dimension(100,22));
      pixelSizeText.setVisible(true);
      pixelSizeText.setBorder(BorderFactory.createLineBorder(Color.black));
      y += lab_height;
      pixelSizeText.setLocation(new java.awt.Point(x_start, y));
      
      outputPixelSizeUnitText.setSize(new java.awt.Dimension(55,22));
      outputPixelSizeUnitText.setLocation(new java.awt.Point(130,y));
      outputPixelSizeUnitText.setVisible(true);
      outputPixelSizeUnitText.setEnabled(true);
      outputPixelSizeUnitText.setEditable(false);
      
      jLabel8.setSize(new java.awt.Dimension(120, 20));
      jLabel8.setVisible(true);
      jLabel8.setText("Destination");
      jLabel8.setLocation(new java.awt.Point(450, 0));
      
      jLabel16.setSize(new java.awt.Dimension(120, 20));
      jLabel16.setVisible(true);
      jLabel16.setText("Commands");
      jLabel16.setLocation(new java.awt.Point(450, 457));
      
      /* define command pane */
      commandPane.setSize(new java.awt.Dimension(310, 243));
      commandPane.setVisible(true);
      commandPane.setLayout(null);
      commandPane.setLocation(new java.awt.Point(440,477));
      commandPane.add(runButton);
      commandPane.add(exitButton);
      commandPane.add(convertFormatButton);
      commandPane.add(loadParametersButton);
      commandPane.add(saveParametersButton);
      commandPane.add(parameterFileNameLabel );
      commandPane.add(parameterFileNameText );
      
      runButton.setSize(new java.awt.Dimension(80, 30));
      runButton.setVisible(true);
      runButton.setText("Run");
      runButton.setMnemonic('r');
      runButton.setToolTipText("Run the Modis resampler");
      runButton.setLocation(new java.awt.Point(35, 150));
      
      exitButton.setSize(new java.awt.Dimension(250, 30));
      exitButton.setVisible(true);
      exitButton.setText("Exit");
      exitButton.setToolTipText("Exit the Modis Resampler");
      exitButton.setMnemonic('x');
      exitButton.setLocation(new java.awt.Point(35, 190));
      
      convertFormatButton.setSize(new java.awt.Dimension(160, 30));
      convertFormatButton.setVisible(true);
      convertFormatButton.setText("Convert Format");
      convertFormatButton.setMnemonic('c');
      convertFormatButton.setToolTipText(
              "Run the conversion program resample.exe with the parameters");
      convertFormatButton.setLocation(new java.awt.Point(125, 150));
      
      loadParametersButton.setSize(new java.awt.Dimension(250, 30));
      loadParametersButton.setVisible(true);
      loadParametersButton.setText("Load Parameter File ...");
      loadParametersButton.setToolTipText(
              "Load the Parameter File from the file system");
      loadParametersButton.setMnemonic('l');
      loadParametersButton.setLocation(new java.awt.Point(35, 10));
      
      saveParametersButton.setSize(new java.awt.Dimension(250, 30));
      saveParametersButton.setVisible(true);
      saveParametersButton.setText("Save Parameter File ...");
      saveParametersButton.setToolTipText("Save the Parameter File");
      saveParametersButton.setMnemonic('s');
      saveParametersButton.setLocation(new java.awt.Point(35, 55));
      
      parameterFileNameLabel.setSize(new java.awt.Dimension(250, 15));
      parameterFileNameLabel.setVisible(true);
      parameterFileNameLabel.setText("Parameter File");
      parameterFileNameLabel.setLocation(new java.awt.Point(35, 95));
      
      parameterFileNameText.setSize(new java.awt.Dimension(250, 25));
      parameterFileNameText.setVisible(true);
      parameterFileNameText.setLocation(new java.awt.Point(35, 115));
      
      getContentPane().setLayout(new BorderLayout());
      getContentPane().add(innerPane, BorderLayout.CENTER);
      
      
      setJMenuBar(menuBar);
      setTitle("edu.sdsmt.modis.ModisFrame");
      
      
      setSize(new java.awt.Dimension(765, 760));
      /*getContentPane().setLayout(null);
      setJMenuBar(menuBar);
      setTitle("edu.sdsmt.modis.ModisFrame");
      setLocation(new java.awt.Point(0, 0));
      getContentPane().add(inputPane);
      getContentPane().add(jLabel12);
      getContentPane().add(destinationPane);
      getContentPane().add(jLabel8);
      getContentPane().add(jLabel16);
      getContentPane().add(commandPane);*/
      
      
      /* set up event trigger (listner) for menu item "Open input file"*/
      jMenuFileOpenInputFile.addActionListener(
              new java.awt.event.ActionListener() {
         public void actionPerformed(java.awt.event.ActionEvent e) {
            jMenuFileOpenInputFileActionPerformed(e);
         }
      });
      /* set up event trigger (listner) for menu item "Specify input file"*/
      jMenuFileSpecifyOutputFile.addActionListener(
              new java.awt.event.ActionListener() {
         public void actionPerformed(java.awt.event.ActionEvent e) {
            jMenuFileSpecifyOutputFileActionPerformed(e);
         }
      });
      /* set up event trigger (listner) for menu item "Load Parameters" */
      jMenuFileLoadParameters.addActionListener(
              new java.awt.event.ActionListener() {
         public void actionPerformed(java.awt.event.ActionEvent e) {
            jMenuFileLoadParametersActionPerformed(e);
         }
      });
      /* set up event trigger (listner) for menu item "SaveParameters" */
      jMenuFileSaveParameters.addActionListener(
              new java.awt.event.ActionListener() {
         public void actionPerformed(java.awt.event.ActionEvent e) {
            jMenuFileSaveParametersActionPerformed(e);
         }
      });
      /* set up event trigger (listner) for menu item "Exit" */
      jMenuFileExit.addActionListener(new java.awt.event.ActionListener() {
         public void actionPerformed(java.awt.event.ActionEvent e) {
            jMenuFileExitActionPerformed(e);
         }
      });
      /*  set up event trigger (listner) for menu item "Run" */
      jMenuActionRun.addActionListener(new java.awt.event.ActionListener() {
         public void actionPerformed(java.awt.event.ActionEvent e) {
            jMenuActionRunActionPerformed(e);
         }
      });
      
      /*  set up event trigger (listner) for menu item "Run" */
      jMenuActionConvertFormat.addActionListener(
              new java.awt.event.ActionListener() {
         public void actionPerformed(java.awt.event.ActionEvent e) {
            jMenuConvertFormatActionPerformed(e);
         }
      });
      
      /* set up event trigger (listner) for menu item "About" in menu Help */
      jMenuHelpAbout.addActionListener(new java.awt.event.ActionListener() {
         public void actionPerformed(java.awt.event.ActionEvent e) {
            jMenuActionAboutActionPerformed(e);
         }
      });
      
          /* added on 05/21/02
           set up event trigger (listner) for menu item "Set Directory" in
           menu Setting */
      jMenuSetInputDirectory.addActionListener(
              new java.awt.event.ActionListener() {
         public void actionPerformed(java.awt.event.ActionEvent e) {
            jMenuSetDirectoryActionPerformed(e);
         }
      });
      
         /*added on 05/27/02
          set up event trigger (listner) for menu item "Set Directory" in
          menu Setting */
      jMenuSetOutputDirectory.addActionListener(
              new java.awt.event.ActionListener() {
         public void actionPerformed(java.awt.event.ActionEvent e) {
            jMenuSetDirectoryActionPerformed(e);
         }
      });
      
         /*added on 05/29/02
          set up event trigger (listner) for menu item "Set Directory" 
          in menu Setting */
      jMenuSetParameterDirectory.addActionListener(
              new java.awt.event.ActionListener() {
         public void actionPerformed(java.awt.event.ActionEvent e) {
            jMenuSetDirectoryActionPerformed(e);
         }
      });
      
      /* set up event trigger (listner)for input file name text field */
      startYCoordLLText.addFocusListener(new java.awt.event.FocusAdapter() {
         public void focusLost(java.awt.event.FocusEvent e) {
            cornerTextFocusLost(e);
         }
      });
      
      /* set up event trigger (listner)for input file name text field */
      startXCoordLLText.addFocusListener(new java.awt.event.FocusAdapter() {
         public void focusLost(java.awt.event.FocusEvent e) {
            cornerTextFocusLost(e);
         }
      });
      /* set up event trigger (listner)for input file name text field */
      endYCoordLLText.addFocusListener(new java.awt.event.FocusAdapter() {
         public void focusLost(java.awt.event.FocusEvent e) {
            cornerTextFocusLost(e);
         }
      });
      /* set up event trigger (listner)for input file name text field */
      endXCoordLLText.addFocusListener(new java.awt.event.FocusAdapter() {
         public void focusLost(java.awt.event.FocusEvent e) {
            cornerTextFocusLost(e);
         }
      });
      /* set up event trigger (listner)for input file name text field */
      startYCoordLSText.addFocusListener(new java.awt.event.FocusAdapter() {
         public void focusLost(java.awt.event.FocusEvent e) {
            cornerTextFocusLost(e);
         }
      });
      /* set up event trigger (listner)for input file name text field */
      startXCoordLSText.addFocusListener(new java.awt.event.FocusAdapter() {
         public void focusLost(java.awt.event.FocusEvent e) {
            cornerTextFocusLost(e);
         }
      });
      /* set up event trigger (listner)for input file name text field */
      endYCoordLSText.addFocusListener(new java.awt.event.FocusAdapter() {
         public void focusLost(java.awt.event.FocusEvent e) {
            cornerTextFocusLost(e);
         }
      });
      /* set up event trigger (listner)for input file name text field */
      endXCoordLSText.addFocusListener(new java.awt.event.FocusAdapter() {
         public void focusLost(java.awt.event.FocusEvent e) {
            cornerTextFocusLost(e);
         }
      });
      
      /* set up event trigger (listner)for input file name text field */
      startYCoordXYText.addFocusListener(new java.awt.event.FocusAdapter() {
         public void focusLost(java.awt.event.FocusEvent e) {
            cornerTextFocusLost(e);
         }
      });
      /* set up event trigger (listner)for input file name text field */
      startXCoordXYText.addFocusListener(new java.awt.event.FocusAdapter() {
         public void focusLost(java.awt.event.FocusEvent e) {
            cornerTextFocusLost(e);
         }
      });
      /* set up event trigger (listner)for input file name text field */
      endYCoordXYText.addFocusListener(new java.awt.event.FocusAdapter() {
         public void focusLost(java.awt.event.FocusEvent e) {
            cornerTextFocusLost(e);
         }
      });
      /* set up event trigger (listner)for input file name text field */
      endXCoordXYText.addFocusListener(new java.awt.event.FocusAdapter() {
         public void focusLost(java.awt.event.FocusEvent e) {
            cornerTextFocusLost(e);
         }
      });
      
      /* set up event trigger (listner)for input file name text field */
      inputFileNameText.addFocusListener(new java.awt.event.FocusAdapter() {
         public void focusLost(java.awt.event.FocusEvent e) {
            inputFileNameTextFocusLost(e);
         }
      });
      /* set up event trigger (listner)for "open input file button" */
      openInputFileButton.addActionListener(
              new java.awt.event.ActionListener() {
         public void actionPerformed(java.awt.event.ActionEvent e) {
            openInputFileButtonActionPerformed(e);
         }
      });
      
      /* set up mouse event trigger (listner)for available bands list box */
      inputfilelist.addMouseListener(new java.awt.event.MouseAdapter() {
         public void mousePressed(java.awt.event.MouseEvent e) {
            inputfilelistMousePressed(e);
         }
      });
      /* set up key event trigger (listner) for available bands list box  */
      inputfilelist.addKeyListener(new java.awt.event.KeyAdapter() {
         public void keyPressed(java.awt.event.KeyEvent e) {
            inputfilelistKeyPressed(e);
         }
      });
      /* set up mouse event trigger (listner)for available bands list box */
      availableBandsList.addMouseListener(new java.awt.event.MouseAdapter() {
         public void mousePressed(java.awt.event.MouseEvent e) {
            availableBandsListMousePressed(e);
         }
      });
      /* set up key event trigger (listner) for available bands list box  */
      availableBandsList.addKeyListener(new java.awt.event.KeyAdapter() {
         public void keyPressed(java.awt.event.KeyEvent e) {
            availableBandsListKeyPressed(e);
         }
      });
      /* set up mouse event trigger (listner)for selected bands list box */
      selectedBandsList.addMouseListener(new java.awt.event.MouseAdapter() {
         public void mousePressed(java.awt.event.MouseEvent e) {
            selectedBandsListMousePressed(e);
         }
      });
      /* set up key event trigger (listner) for selected bands list box  */
      selectedBandsList.addKeyListener(new java.awt.event.KeyAdapter() {
         public void keyPressed(java.awt.event.KeyEvent e) {
            selectedBandsListKeyPressed(e);
         }
      });
      /* set up event trigger (listner)for the add band button */
      addBandButton.addActionListener(new java.awt.event.ActionListener() {
         public void actionPerformed(java.awt.event.ActionEvent e) {
            addBandButtonActionPerformed(e);
         }
      });
      /* set up event trigger (listner) for the remove band button */
      removeBandButton.addActionListener(new java.awt.event.ActionListener() {
         public void actionPerformed(java.awt.event.ActionEvent e) {
            removeBandButtonActionPerformed(e);
         }
      });
      /* set up event trigger (listner) for the spacial subset combo */
      spatialSubsetCombo.addItemListener(new java.awt.event.ItemListener() {
         public void itemStateChanged(java.awt.event.ItemEvent e) {
            spatialSubsetComboItemStateChanged(e);
         }
      });
      /* set up event trigger (listner)for the view metadata button */
      viewMetadataButton.addActionListener(new java.awt.event.ActionListener() {
         public void actionPerformed(java.awt.event.ActionEvent e) {
            viewMetadataButtonActionPerformed(e);
         }
      });
      
      /* set up event trigger (listner)for the view image button */
      viewImageButton.addActionListener(new java.awt.event.ActionListener() {
         public void actionPerformed(java.awt.event.ActionEvent e) {
            viewImageButtonActionPerformed(e);
         }
      });
      
      /* set up event trigger (listner)for output format combo */
      outputFormatCombo.addItemListener(new java.awt.event.ItemListener() {
         public void itemStateChanged(java.awt.event.ItemEvent e) {
            outputFormatComboItemStateChanged(e);
         }
      });
      /* set up event trigger (listner)for resampling combo */
      resamplingCombo.addItemListener(new java.awt.event.ItemListener() {
         public void itemStateChanged(java.awt.event.ItemEvent e) {
            resamplingComboItemStateChanged(e);
         }
      });
      /* set up event trigger (listner)for projection combo */
      projectionCombo.addItemListener(new java.awt.event.ItemListener() {
         public void itemStateChanged(java.awt.event.ItemEvent e) {
            projectionComboItemStateChanged(e);
         }
      });
      /* set up event trigger (listner)for projection parameters button */
      projectionParametersButton.addActionListener(
              new java.awt.event.ActionListener() {
         public void actionPerformed(java.awt.event.ActionEvent e) {
            projectionParametersButtonActionPerformed(e);
         }
      });
      
      /* set up event trigger (listner)for save output file button */
      saveOutputFileButton.addActionListener(
              new java.awt.event.ActionListener() {
         public void actionPerformed(java.awt.event.ActionEvent e) {
            saveOutputFileButtonActionPerformed(e);
         }
      });
      
      /* set up event trigger (listner) for the output file name text box */
      outputFileNameText.addFocusListener(new java.awt.event.FocusAdapter() {
         public void focusLost(java.awt.event.FocusEvent e) {
            outputFileNameTextFocusLost(e);
         }
      });
      /* set up event trigger (listner) for the run button */
      runButton.addActionListener(new java.awt.event.ActionListener() {
         public void actionPerformed(java.awt.event.ActionEvent e) {
            runButtonActionPerformed(e);
         }
      });
      
      /* set up event trigger (listner) for the run button */
      exitButton.addActionListener(new java.awt.event.ActionListener() {
         public void actionPerformed(java.awt.event.ActionEvent e) {
            jMenuFileExitActionPerformed(e);
         }
      });
      
      convertFormatButton.addActionListener(
              new java.awt.event.ActionListener(){
         public void actionPerformed(java.awt.event.ActionEvent e) {
            jMenuConvertFormatActionPerformed(e);
         }
      });
      
      /* set up event trigger (listner)for the load parmeters button */
      loadParametersButton.addActionListener(
              new java.awt.event.ActionListener() {
         public void actionPerformed(java.awt.event.ActionEvent e) {
            loadParametersButtonActionPerformed(e);
         }
      });
      /* set up event trigger (listner) for the save parameters button */
      saveParametersButton.addActionListener(
              new java.awt.event.ActionListener() {
         public void actionPerformed(java.awt.event.ActionEvent e) {
            saveParametersButtonActionPerformed(e);
         }
      });
      /* set up event trigger (listner)for the window */
      addWindowListener(new java.awt.event.WindowAdapter() {
         public void windowClosing(java.awt.event.WindowEvent e) {
            thisWindowClosing(e);
         }
      });
      /* END GENERATED CODE */
      
      inputPane.setBorder(new EtchedBorder(EtchedBorder.RAISED));
      destinationPane.setBorder(new EtchedBorder(EtchedBorder.RAISED));
      commandPane.setBorder(new EtchedBorder(EtchedBorder.RAISED));
      
      /* populate combos and no default item selected in combo except for the
       * resampling combo box where the default value is set to nearest
       * neighbour
       */
      populateCombo(outputFormatCombo,model.getOutputFormats());
      outputFormatCombo.setSelectedIndex(-1);
      populateCombo(projectionCombo,model.getProjectionTypes());
      projectionCombo.setSelectedIndex(-1);
      populateCombo(resamplingCombo,model.getResamplingTypes());
      resamplingCombo.setSelectedIndex(1);// default set to Nearest Neighbour
      populateCombo(spatialSubsetCombo,model.getSpatialSubsets());
      spatialSubsetCombo.setSelectedIndex(-1);
      outputPixelSizeUnitText.setText("");
      
      /* set up event trigger (listner) for input file name text box */
      /* Define the action listner for possible change */
      inputFileNameText.getDocument().addDocumentListener(
              new DocumentListener() {
         public void changedUpdate(DocumentEvent e) {
            inputFileNameTextActivity();
         }
         /* Define the action listner for possible change */
         public void removeUpdate(DocumentEvent e) {
            inputFileNameTextActivity();
         }
         /* Define the action listner for possible change */
         public void insertUpdate(DocumentEvent e) {
            inputFileNameTextActivity();
         }
      });
      /* set up event trigger (listner) for output file name text box */
      outputFileNameText.getDocument().addDocumentListener(new DocumentListener() {
         public void changedUpdate(DocumentEvent e) {
            outputFileNameTextActivity();
         }
         public void removeUpdate(DocumentEvent e) {
            outputFileNameTextActivity();
         }
         public void insertUpdate(DocumentEvent e) {
            outputFileNameTextActivity();
         }
      });
      
      /* set up event trigger (listner)for pixel size text box */
      pixelSizeText.getDocument().addDocumentListener(new DocumentListener() {
         public void changedUpdate(DocumentEvent e) {
            pixelSizeTextActivity();
         }
         public void removeUpdate(DocumentEvent e) {
            pixelSizeTextActivity();
         }
         public void insertUpdate(DocumentEvent e) {
            pixelSizeTextActivity();
         }
      });
      
      
      DocumentListener cornerPointListener = new DocumentListener() {
         public void changedUpdate(DocumentEvent e) {
            setCornerPoints();            }
         public void removeUpdate(DocumentEvent e) {
            setCornerPoints();
         }
         public void insertUpdate(DocumentEvent e) {
            setCornerPoints();
         }
      };
      
      /* set up event trigger (listner)*/
      startYCoordLLText.getDocument().addDocumentListener(
              cornerPointListener);
      /* set up event trigger (listner) */
      startXCoordLLText.getDocument().addDocumentListener(
              cornerPointListener);
      /* set up event trigger (listner)*/
      endYCoordLLText.getDocument().addDocumentListener(
              cornerPointListener);
      /* set up event trigger (listner)*/
      endXCoordLLText.getDocument().addDocumentListener(
              cornerPointListener);
      /* set up event trigger (listner)*/
      startYCoordLSText.getDocument().addDocumentListener(
              cornerPointListener);
      /* set up event trigger (listner)*/
      startXCoordLSText.getDocument().addDocumentListener(
              cornerPointListener);
      /* set up event trigger (listner)*/
      endYCoordLSText.getDocument().addDocumentListener(
              cornerPointListener);
      /* set up event trigger (listner)*/
      endXCoordLSText.getDocument().addDocumentListener(
              cornerPointListener);
      startYCoordXYText.getDocument().addDocumentListener(
              cornerPointListener);
      /* set up event trigger (listner)*/
      startXCoordXYText.getDocument().addDocumentListener(
              cornerPointListener);
      /* set up event trigger (listner)*/
      endYCoordXYText.getDocument().addDocumentListener(
              cornerPointListener);
      /* set up event trigger (listner)*/
      endXCoordXYText.getDocument().addDocumentListener(
              cornerPointListener);
      
      statusDialog = new StatusDialog();// Class extends Frame
      statusDialog.initialize();
      setTitle("ModisTool");
   }
   
   private boolean mShown = false;
   
   /**
    * NAME: addNotify
    * PURPOSE:
    *   To add notify for event purpose
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    *
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    *
    * PROJECT: ModisTool
    * NOTES:
    *
    */
   public void addNotify() {
      super.addNotify();
      if (mShown) return;
      
      // resize frame to account for menubar
      JMenuBar jMenuBar = getJMenuBar();
      if (jMenuBar != null) {
         int jMenuBarHeight = jMenuBar.getPreferredSize().height;
         Dimension dimension = getSize();
         dimension.height += jMenuBarHeight;
         setSize(dimension);
      }
      mShown = true;
   }
   
   /**
    * NAME: thisWindowClosing
    * PURPOSE:
    *   Close the window when the close box is clicked
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    * 
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    * 
    * PROJECT: ModisTool
    * NOTES:
    * @param e The window event object.
    */
   void thisWindowClosing(java.awt.event.WindowEvent e) {
      controller.exitFrame();
   }
   
   /**
    * NAME:               spatialSubsetComboItemStateChanged
    * PURPOSE:
    * To monitor the change of spatial Subset Combo Item State
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    * 
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    * 
    * PROJECT: ModisTool
    * NOTES:
    * @param e An item event object.
    */
   public void spatialSubsetComboItemStateChanged(java.awt.event.ItemEvent e) {
      Object item = spatialSubsetCombo.getSelectedItem();
      if (item != null) {
         ModisModel.TextIdPair tip = (ModisModel.TextIdPair)item;
         int subsetId = tip.getId();
         
         if ( subsetId == ModisModel.SPACE_LAT_LON ) {
            model.setSpatialSubsetType(1);
            yCoordLabel.setText("Latitude");
            xCoordLabel.setText("Longitude");
            
            startXCoordLLText.setVisible(true);
            startYCoordLLText.setVisible(true);
            endXCoordLLText.setVisible(true);
            endYCoordLLText.setVisible(true);
            
            startXCoordLSText.setVisible(false);
            startYCoordLSText.setVisible(false);
            endXCoordLSText.setVisible(false);
            endYCoordLSText.setVisible(false);
            
            startXCoordXYText.setVisible(false);
            startYCoordXYText.setVisible(false);
            endXCoordXYText.setVisible(false);
            endYCoordXYText.setVisible(false);
         } else if (subsetId == ModisModel.SPACE_LINE_SAMPLE) {
            model.setSpatialSubsetType(2);
            yCoordLabel.setText("Line");
            xCoordLabel.setText("Sample");
            
            startXCoordLSText.setVisible(true);
            startYCoordLSText.setVisible(true);
            endXCoordLSText.setVisible(true);
            endYCoordLSText.setVisible(true);
            
            startXCoordLLText.setVisible(false);
            startYCoordLLText.setVisible(false);
            endXCoordLLText.setVisible(false);
            endYCoordLLText.setVisible(false);
            
            startXCoordXYText.setVisible(false);
            startYCoordXYText.setVisible(false);
            endXCoordXYText.setVisible(false);
            endYCoordXYText.setVisible(false);
         } else if (subsetId == ModisModel.SPACE_PROJ_XY) {
            model.setSpatialSubsetType(3);
            yCoordLabel.setText("Proj. X");
            xCoordLabel.setText("Proj. Y");
            
            startXCoordXYText.setVisible(true);
            startYCoordXYText.setVisible(true);
            endXCoordXYText.setVisible(true);
            endYCoordXYText.setVisible(true);
            
            startXCoordLSText.setVisible(false);
            startYCoordLSText.setVisible(false);
            endXCoordLSText.setVisible(false);
            endYCoordLSText.setVisible(false);
            
            startXCoordLLText.setVisible(false);
            startYCoordLLText.setVisible(false);
            endXCoordLLText.setVisible(false);
            endYCoordLLText.setVisible(false);
         }
         model.setSpatialSubsetType(tip.getId());
      }
      setCornerPoints();
   }
   
   /**
    * NAME: resamplingComboItemStateChanged
    * PURPOSE:
    *   To monitor the change of resampling Combo Item State
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    * 
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    * 
    * PROJECT: ModisTool
    * NOTES:
    * @param e An item event object.
    */
   public void resamplingComboItemStateChanged(java.awt.event.ItemEvent e) {
      if (resamplingCombo.getSelectedIndex() >= 0) {
         final ModisModel.TextIdPair item =
                 (ModisModel.TextIdPair)resamplingCombo.getSelectedItem();
         model.setResamplingType(item.getId());
         prevResamplingType = resamplingCombo.getSelectedIndex();
         
      }
      setSaveRunEnablements();
   }
   
   /**
    * NAME: inputFileNameTextFocusLost
    * PURPOSE:
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    * 
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    * 
    * PROJECT:        ModisTool
    * NOTES:
    * @param e An item event object.
    */
   public void inputFileNameTextFocusLost(java.awt.event.FocusEvent e) {
   }
   
   /**
    * NAME: cornerTextFocusLost
    * PURPOSE:
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    * 
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    * 
    * PROJECT:        ModisTool
    * NOTES:
    * @param e An focus event object.
    */
   public void cornerTextFocusLost(java.awt.event.FocusEvent e) {
      
   }
   
   
   /**
    * NAME: openInputFileButtonActionPerformed
    * PURPOSE:
    *   To monitor open Input File Button to see if there is action performed.
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    * 
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    * 
    * PROJECT:        ModisTool
    * NOTES:
    * @param e An action event object.
    */
   public void openInputFileButtonActionPerformed(
           java.awt.event.ActionEvent e) {
      String s = browseInputFiles();
      
      if ( s != null && s.length() > 0 ) {
         bLoadHdrFile = true;
         startLoadParameterFile = false;
         startLoadHeaderFile = true;
         readInputFilesFromTmpMosaic = false;
         startLoadFirstHeaderFile ++;
         controller.loadFile(s);
         
         // If mosaic has already been determined, then don't change it.
         if (mosaic != 1)
            mosaic = controller.mosaic;
         
      } else {
         complain("OpenInputFileActionPerformed", "Input File string is null!");
         return;
      }
      
      File tempFile= new File("TmpHdr.hdr");
      tempFile.delete();
      
      /**Delete the TmpMosaic file*/
      
      File mosaicFile =  new File("TmpMosaic.prm");
      if(mosaicFile.exists())
         mosaicFile.delete();
      
      setSaveRunEnablements();
   }
   
   /**
    * NAME: addBandButtonActionPerformed
    * PURPOSE:
    *   To monitor the add band button to see if there is an action performed.
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    * 
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    * 
    * PROJECT: ModisTool
    * NOTES:
    * @param e An action event object.
    */
   public void addBandButtonActionPerformed(java.awt.event.ActionEvent e) {
      final int[] index = availableBandsList.getSelectedIndices();
      final int numSelected = index.length;
      final ChooserListModel availableLm = (ChooserListModel)
      availableBandsList.getModel();
      final ChooserListModel selectedLm = (ChooserListModel)
      selectedBandsList.getModel();
      
      for (int i= 0; i < numSelected; i++) {
         final Object selectedElement = availableLm.getElementAt(index[i]-i);
         availableLm.removeElementAt(index[i]-i);
         
         selectedLm.insertElementAt(selectedElement,
                 getInsertPosition(selectedLm, selectedElement));
      }
      if (selectedLm.getSize() > 0) {
         removeBandButton.setEnabled(true);
      }
      if (availableLm.getSize() == 0 ) {
         addBandButton.setEnabled(false);
      }
      selectedBandCount.setText(Integer.toString(
              selectedBandsList.getModel().getSize()));
      availableBandCount.setText(Integer.toString(
              availableBandsList.getModel().getSize()));
      
      Vector selected = new Vector();
      for (int i=0; i < selectedLm.getSize(); ++i) {
         selected.addElement(selectedLm.getElementAt(i));
      }
      model.setSelectedBands(selected.elements());
      setSaveRunEnablements();
   }
   
   /**
    * NAME: removeBandButtonActionPerformed
    * PURPOSE:
    *   To monitor remove band button to see if there is action performed.
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    * 
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    * 
    * PROJECT:        ModisTool
    * NOTES:
    * @param e An action event object.
    */
   public void removeBandButtonActionPerformed(java.awt.event.ActionEvent e) {
      final int[] index = selectedBandsList.getSelectedIndices();
      final int numSelected = index.length;
      final ChooserListModel availableLm =
              (ChooserListModel) availableBandsList.getModel();
      final ChooserListModel selectedLm  =
              (ChooserListModel) selectedBandsList.getModel();
      
      for (int i= 0; i < numSelected; i++) {
         final Object selectedElement = selectedLm.getElementAt(index[i]-i);
         selectedLm.removeElementAt(index[i]-i);
         
         
         /* keep the same order as in the header file */
         availableLm.insertElementAt(selectedElement,
                 getInsertPosition(availableLm, selectedElement));
      }
      
      if (availableLm.getSize() > 0)  addBandButton.setEnabled(true);
      if (selectedLm.getSize() == 0 ) removeBandButton.setEnabled(false);
      
      selectedBandCount.setText(Integer.toString(selectedLm.getSize()));
      availableBandCount.setText(Integer.toString(availableLm.getSize()));
      
      Vector selected = new Vector();
      for (int i=0; i < selectedLm.getSize(); ++i) {
         selected.addElement(selectedLm.getElementAt(i));
      }
      model.setSelectedBands(selected.elements());
      setSaveRunEnablements();
   }
   
   /**
    * NAME: getInsertPosition
    * PURPOSE:
    *   To find out the position for insert list item.
    * RETURN VALUE:
    *   integer
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    *
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    *
    * PROJECT: ModisTool
    * NOTES:
    *
    */
   private int getInsertPosition(ChooserListModel lm, Object selectedElement) {
      int selectedElementIndex = model.getBandNum(selectedElement.toString());
      int [] index = new int [ lm.getSize() ];
      
      for (int i = 0; i < lm.getSize(); i++) {
         index[i] = model.getBandNum((lm.getElementAt(i)).toString());
      }
      
      for (int i = 0; i < lm.getSize(); i++)
         if (selectedElementIndex < index[i])   return i;
      
      return lm.getSize();
   }
   
   /**
    * NAME: outputFormatComboItemStateChanged
    * PURPOSE:
    *   To monitor the state change of output format combo item
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    * 
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    * 
    * PROJECT: ModisTool
    * NOTES:
    * @param e An item event object.
    */
   public void outputFormatComboItemStateChanged(java.awt.event.ItemEvent e) {
      if ( outputFormatCombo.getSelectedIndex() >= 0 ) {
         ModisModel.TextIdPair item =
                 (ModisModel.TextIdPair)outputFormatCombo.getSelectedItem();
         int format = item.getId();
         model.setOutputFormat(format);
         prevOutputFileType = outputFormatCombo.getSelectedIndex();
         setOutputFileNameExtension();
         setSaveRunEnablements();
      }
   }
   
   /**
    * NAME: projectionComboItemStateChanged
    * PURPOSE:
    *   To monitor the state change of projection combo item
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    * 
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    * 
    * PROJECT: ModisTool
    * NOTES:
    * @param e An item event object.
    */
   public void projectionComboItemStateChanged(java.awt.event.ItemEvent e) {
      if ( projectionCombo.getSelectedIndex() >= 0 ) {
         ModisModel.TextIdPair item =
                 (ModisModel.TextIdPair)projectionCombo.getSelectedItem();
         if (model.getOutputProjectionType() != item.getId()) {
            model.setOutputProjectionType(item.getId());
            prevOutputProjType = projectionCombo.getSelectedIndex();
            for (int i = 0; i < 15; i++)
               model.setOutputProjectionParameter(i, 0.0);
            model.setUTMZone(0);
         }
         
         /* set the default units (GEO: DEGREES, others: METERS) for the
          * output pixel size */
         /* JMW 05/01/01 - move following out of previous if stmt */
         // if (projectionCombo.getSelectedIndex() != 0 )
         {
            String str = model.getOutputProjectionTypeString();
            if (str.equals("GEO"))
               outputPixelSizeUnitText.setText("degrees");
            else //if (str.length() > 0)
               outputPixelSizeUnitText.setText("meters");   /**/
         }
         
      }
      
      setSaveRunEnablements();
   }
   
   /**
    * NAME: projectionParametersButtonActionPerformed
    * PURPOSE:
    *   To monitor the action of projection Parameters Button and process it
    *   accordingly.
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    * 
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    * 
    * PROJECT:        ModisTool
    * NOTES:
    * @param e An action event object.
    */
   public void projectionParametersButtonActionPerformed(
           java.awt.event.ActionEvent e) {
      ProjParamDialog dialog = new ProjParamDialog(this);
      try {
         dialog.initComponents();
         ModisModel.TextIdPair item =
                 (ModisModel.TextIdPair)projectionCombo.getSelectedItem();
         dialog.setupFields(item.getId());
      } catch (Exception exception) {}
      
      dialog.setVisible(true);
      int result = dialog.getResult();
      if ( result == ProjParamDialog.OK ) {
      } else if ( result == ProjParamDialog.CANCEL ) {
      }
      
      dialog.dispose();
   }
   
   /**
    * NAME: saveOutputFileButtonActionPerformed
    * PURPOSE:
    *   To monitor the action of save output file button and process it
    * accordingly.
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    * 
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    * 
    * PROJECT: ModisTool
    * NOTES:
    * @param e An action event object.
    */
   public void saveOutputFileButtonActionPerformed(
           java.awt.event.ActionEvent e) {
      
      String s = browseOutputFiles();
      s = s.toLowerCase();
      {
         int outputType;
         if (s.endsWith(".tif")) {
            outputType = ModisModel.GEOTIFF;
         } else if (s.endsWith(".hdf")) {
            outputType = ModisModel.HDFEOS;
         } else {
            outputType = ModisModel.MULTIFILE;
         }
         ComboBoxModel cbm = outputFormatCombo.getModel();
         ITEM_LOOP:
            for (int i=0; i<cbm.getSize(); ++i) {
               ModisModel.TextIdPair tip = (ModisModel.TextIdPair)
               cbm.getElementAt(i);
               if (tip.getId() == outputType) {
                  outputFormatCombo.setSelectedIndex(i);
                  break ITEM_LOOP;
               }
            }
      }
      // cancel is clicked    else if ( s.length() == 0 )    return;
      setSaveRunEnablements();
   }
   
   /**
    * NAME: outputFileNameTextFocusLost
    * PURPOSE:
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    * 
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    * 
    * PROJECT:        ModisTool
    * NOTES:
    * @param e A focus event object.
    */
   public void outputFileNameTextFocusLost(java.awt.event.FocusEvent e) {
   }
   
   /**
    * NAME: runButtonActionPerformed
    * PURPOSE:
    *   To monitor if there is run button pressed and process it accordingly.
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    * 
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    * 
    * PROJECT: ModisTool
    * NOTES:
    * @param e An action event object.
    */
   public void runButtonActionPerformed(java.awt.event.ActionEvent e) {
      String eol = System.getProperty("line.separator");
      if((pixelSizeText.getText()).startsWith("-")) {
         /* show a message box */
         JOptionPane.showMessageDialog(this,
                 "Invalid input. Pixel size must be a positive number " +
                 "greater than zero!" + eol + "Please try again.");
         pixelSizeText.setText("");
         return;
      }
      
      for (int i = 0; i < (pixelSizeText.getText()).length(); i++) {
         char c = (pixelSizeText.getText()).charAt(i);
         if (!Character.isDigit(c) && c != '.') {
            /* show a message box */
            JOptionPane.showMessageDialog(this,
                    "Invalid input. Pixel size must be a number!" + eol +
                    "Please try again.");
            pixelSizeText.setText("");
            return;
         }
      }
      
      String prevParameterFile = model.getParameterFilename();
      model.setParameterFilename("TmpParam.prm");
      File outputFileExist = new File(model.getOutputFilename());
      
      /* Determine the output file path for TmpMosaic.hdf */
      String outputFileName = outputFileExist.getAbsolutePath();
      int slashIndex = outputFileName.lastIndexOf(
              System.getProperty("file.separator"));
      String outputFilePath = outputFileName.substring(0, slashIndex+1);
      
      /**
       * File TmpMosaic.prm is deleted at this point.
       * Input File names are read directly from the
       * JList to an array and then createMosaicFile method is
       * called to create the TmpMosaic.prm file.
       */
      final ChooserListModel inputLm =
              (ChooserListModel) inputfilelist.getModel();
      // Theoritically the maximum no. of files that can be specified by the
      // user.
      File[] inputFiles = new File[552];
      
      int numberOfInputFiles = inputLm.getSize();
      
      // String outputMosaicFile = null;
      
      for(int i = 0 ; i < numberOfInputFiles; i++){
         String s  = inputLm.getElementAt(i).toString();
         inputFiles[i] = new File(s);
         s = s.toLowerCase();
         if(s.endsWith("hdf"))
            inputFilesEndWithHdf = true;
         else
            inputFilesEndWithHdf = false;
         
         if(inputFilesEndWithHdf){
            outputMosaicFile = outputFilePath + "TmpMosaic.hdf";
         } else{
            outputMosaicFile = outputFilePath + "TmpMosaic.hdr";
         }
      }
      
      JOptionPane outputOverwrite = new JOptionPane(
              "The selected output file already exists. Overwrite?",
              JOptionPane.WARNING_MESSAGE, JOptionPane.YES_NO_OPTION);
      
      StringBuffer str = new StringBuffer();
      for (int i=0; i < model.getNumBands(); ++i) {
         str.append(model.isSelectedBand(i) ? "1" : "0");
      }
      
      String trimmed = new String(str.toString());
      
      if (outputFileExist.exists()) {
         JDialog dialog = outputOverwrite.createDialog(this,"Warning");
         dialog.setVisible(true);
         
         int optionValue = ((Integer) outputOverwrite.getValue()).intValue();
         if (optionValue == JOptionPane.YES_OPTION)
         {
            /* if user chose to overwite the output file */
            if(numberOfInputFiles > 1){
               img.createMosaicFile(inputFiles, numberOfInputFiles);
               //setting the input file name if there are more than one file.
               model.setMosaicFileName(outputMosaicFile);
               
               controller.writeParameterFile( mosaic );
               controller.runMosaic(outputMosaicFile, trimmed);
               model.setParameterFilename(prevParameterFile);
            } else{
               controller.writeParameterFile( mosaic );
               model.setParameterFilename(prevParameterFile);
               controller.runConversion();
            }
         } else if (optionValue == JOptionPane.NO_OPTION) {
            /* promt user to input another output filename */
            JOptionPane.showMessageDialog(this,
                    "Select another output file, please...");
            outputFileNameText.setText("");
            runButton.setEnabled(false);
            convertFormatButton.setEnabled(false);
            jMenuActionConvertFormat.setEnabled(false);
            jMenuActionRun.setEnabled(false);
            saveParametersButton.setEnabled(false);
         }
      } else {
         if(numberOfInputFiles > 1){
            img.createMosaicFile(inputFiles, numberOfInputFiles);
            //setting the input file name if there are more than one file.
            model.setMosaicFileName(outputMosaicFile); 
            controller.writeParameterFile( mosaic );
            controller.runMosaic(outputMosaicFile, trimmed);
            model.setParameterFilename(prevParameterFile);
         } else{
            controller.writeParameterFile( mosaic );
            model.setParameterFilename(prevParameterFile);
            controller.runConversion();
         }
      }
   }
   
   /**
    * NAME: jMenuConvertFormatActionPerformed(e)
    * PURPOSE:
    *   To monitor if there is convert format button pressed and process it
    *   accordingly.
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    * 
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    * 
    * PROJECT: ModisTool
    * NOTES:
    * @param e An action event object.
    */
   public void jMenuConvertFormatActionPerformed(java.awt.event.ActionEvent e) {
      String eol = System.getProperty("line.separator");
      
      if((pixelSizeText.getText()).startsWith("-")) {
         /* show a message box */
         JOptionPane.showMessageDialog(this,
                 "Invalid input. Pixel size must be a  positive number " +
                 "greater than zero!" + eol + "Please try again.");
         pixelSizeText.setText("");
         return;
      }
      
      for (int i = 0; i < (pixelSizeText.getText()).length(); i++) {
         char c = (pixelSizeText.getText()).charAt(i);
         if (!Character.isDigit(c) && c != '.') {
            /* show a message box */
            JOptionPane.showMessageDialog(this,
                    "Invalid input. Pixel size must be a number!" + eol +
                    "Please try again.");
            pixelSizeText.setText("");
            return;
         }
      }
      
      String prevParameterFile = model.getParameterFilename();
      model.setParameterFilename("TmpParam.prm");
      File outputFileExist = new File(model.getOutputFilename());
      
      /* Determine the output file path for TmpMosaic.hdf */
      String outputFileName = outputFileExist.getAbsolutePath();
      int slashIndex = outputFileName.lastIndexOf(
              System.getProperty("file.separator"));
      String outputFilePath = outputFileName.substring(0, slashIndex+1);
      
      /**
       * File TmpMosaic.prm is deleted at this point.
       * Input File names are read directly from the
       * JList to an array and then createMosaicFile method is
       * called to create the TmpMosaic.prm file. */
      final ChooserListModel inputLm =
              (ChooserListModel) inputfilelist.getModel();
      
      File[] inputFiles = new File[552];
      // Theoritically the maximum no. of files that can be specified by the
      // user.
      int numberOfInputFiles = inputLm.getSize();
      
      String outFile = null;
      
      for(int i = 0 ; i < numberOfInputFiles; i++){
         String s  = inputLm.getElementAt(i).toString();
         inputFiles[i] = new File(s);
         s = s.toLowerCase();
         if(s.endsWith("hdf"))
            inputFilesEndWithHdf = true;
         else
            inputFilesEndWithHdf = false;
         
         if(inputFilesEndWithHdf){
            outFile = outputFilePath + "TmpMosaic.hdf";
         } else{
            outFile = outputFilePath + "TmpMosaic.hdr";
         }
      }
      
      JOptionPane outputOverwrite = new JOptionPane(
              "The selected output file already exists. Overwrite?",
              JOptionPane.WARNING_MESSAGE, JOptionPane.YES_NO_OPTION);
      
      StringBuffer str = new StringBuffer();
      for (int i=0; i < model.getNumBands(); ++i) {
         str.append(model.isSelectedBand(i) ? "1" : "0");
      }
      
      String trimmed = new String(str.toString());
      
      if (outputFileExist.exists()) {
         JDialog dialog = outputOverwrite.createDialog(this,"Warning");
         dialog.setVisible(true);
         
         int optionValue = ((Integer) outputOverwrite.getValue()).intValue();
         if (optionValue == JOptionPane.YES_OPTION)
         {
            /* if user chose to overwite the output file */
            if(numberOfInputFiles > 1){
               img.createMosaicFile(inputFiles, numberOfInputFiles);
               //setting the input file name if there are more than one file.
               model.setMosaicFileName(outFile); 
               controller.writeconvertParameterFile( mosaic );
               controller.runFormatMosaic(outFile, trimmed);
               model.setParameterFilename(prevParameterFile);
            } else{
               controller.writeconvertParameterFile( mosaic );
               model.setParameterFilename(prevParameterFile);
               controller.runFormatConversion();
            }
         } else if (optionValue == JOptionPane.NO_OPTION) {
            /* promt user to input another output filename */
            JOptionPane.showMessageDialog(this,
                    "Select another output file, please...");
            outputFileNameText.setText("");
            runButton.setEnabled(false);
            convertFormatButton.setEnabled(false);
            jMenuActionConvertFormat.setEnabled(false);
            jMenuActionRun.setEnabled(false);
            saveParametersButton.setEnabled(false);
         }
      } else {
         if(numberOfInputFiles > 1){
            img.createMosaicFile(inputFiles, numberOfInputFiles);
            //setting the input file name if there are more than one file.
            model.setMosaicFileName(outFile); 
            controller.writeconvertParameterFile( mosaic );
            controller.runFormatMosaic(outFile, trimmed);
            model.setParameterFilename(prevParameterFile);
         } else{
            controller.writeconvertParameterFile( mosaic );
            model.setParameterFilename(prevParameterFile);
            controller.runFormatConversion();
         }
      }
   }
   
   
   /**
    * NAME: loadParametersButtonActionPerformed
    * PURPOSE:
    *   To monitor if there is load parameter button pressed and process it
    *   accordingly.
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    * 
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    * 
    * PROJECT: ModisTool
    * NOTES:
    * @param e An action event object.
    */
   public void loadParametersButtonActionPerformed(
           java.awt.event.ActionEvent e) {
      String s = browseParameterFiles(true);
      if ( s != null && s.length() > 0 ) {
         bLoadHdrFile = false;
         startLoadParameterFile = true;
         startLoadHeaderFile = false;
         outputFileNameText.setText("");
         pixelSizeText.setText("");
         controller.loadFile(s);
         
         // If mosaic has already been determined, then don't change it.
         if (mosaic != 1)
            mosaic = controller.mosaic;
         parameterFileNameText.setText(model.getParameterFilename());
      }
      
      /**Delete the TmpHdr File*/
      File tempFile= new File("TmpHdr.hdr");
      tempFile.delete();
      
      
      /**Delete the Mosaic File*/
      File mosaicFile =  new File("TmpMosaic.prm");
      mosaicFile.delete();
      
   }
   
   /**
    * NAME: saveParametersButtonActionPerformed
    * PURPOSE:
    *   To monitor if there is save parameter button pressed and process it
    *   accordingly.
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    * 
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    * 
    * PROJECT: ModisTool
    * NOTES:
    * @param e An action event object.
    */
   public void saveParametersButtonActionPerformed(
           java.awt.event.ActionEvent e) {
      String eol = System.getProperty("line.separator");
      
      if((pixelSizeText.getText()).startsWith("-")) {
         /* show a message box */
         JOptionPane.showMessageDialog(this,
                 "Invalid input" + eol +
                 " Pixel size must be a  positive number greater than zero!" +
                 eol + "Please try again.");
         pixelSizeText.setText("");
         return;
      }
      
      /*creates a file called TmpMosaic.prm and stores the selected
        file so that the file names can be read from TmpMosaic.prm*/
      final ChooserListModel inputLm =
              (ChooserListModel) inputfilelist.getModel();
      // Theoritically the maximum no. of files that can be specified by the
      // user.
      File[] inputFiles = new File[552];
      int numberOfInputFiles = inputLm.getSize();
      
      
      File outputFileExist = new File(model.getOutputFilename());
      
      /* Determine the output file path for TmpMosaic.hdf */
      String outputFileName = outputFileExist.getAbsolutePath();
      int slashIndex = outputFileName.lastIndexOf(
              System.getProperty("file.separator"));
      String outputFilePath = outputFileName.substring(0, slashIndex+1);
      
      String outFile = null;
      
      for(int i = 0 ; i < numberOfInputFiles; i++){
         String s  = inputLm.getElementAt(i).toString();
         inputFiles[i] = new File(s);
         s = s.toLowerCase();
         if(s.endsWith("hdf"))
            inputFilesEndWithHdf = true;
         
         if(inputFilesEndWithHdf){
            outFile = outputFilePath + "TmpMosaic.hdf";
         } else{
            outFile = outputFilePath + "TmpMosaic.hdr";
         }
      }
      
      
      String s = browseParameterFiles(false);
      s = s.toLowerCase();
      if ( s != null && s.length() > 0 ) {
         // added by sj ??
         String sizeText = pixelSizeText.getText();
         model.setOutputPixelSize(sizeText);
         
         if(numberOfInputFiles > 1){
            img.createMosaicFile(inputFiles, numberOfInputFiles);
            //setting the input file name if there are more than one file.
            model.setMosaicFileName(outFile);
         }
         
         controller.writeParameterFile( mosaic );
         parameterFileNameText.setText(model.getParameterFilename());
      }
   }
   
   /**
    * NAME: jMenuFileOpenInputFileActionPerformed
    * PURPOSE:
    *   To monitor if there is Menu File Open InputFile button pressed and
    *   delegate it accordingly.
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    * 
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    * 
    * PROJECT:  ModisTool
    * NOTES:
    * @param e An action event object.
    */
   public void jMenuFileOpenInputFileActionPerformed(
           java.awt.event.ActionEvent e) {
      openInputFileButtonActionPerformed(e);
   }
   
   /**
    * NAME: jMenuFileSpecifyOutputFileActionPerformed
    * PURPOSE:
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    * 
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    * 
    * PROJECT: ModisTool
    * NOTES:
    * @param e An action event object.
    */
   public void jMenuFileSpecifyOutputFileActionPerformed(
           java.awt.event.ActionEvent e) {
      saveOutputFileButtonActionPerformed(e);
   }
   
   /**
    * NAME: jMenuFileLoadParametersActionPerformed
    * PURPOSE:
    *   To monitor if there is Menu File Load Parameters button pressed and
    *   process it accordingly.
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    * 
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    * 
    * PROJECT: ModisTool
    * NOTES:
    * @param e An action event object.
    */
   public void jMenuFileLoadParametersActionPerformed(
           java.awt.event.ActionEvent e) {
      String s = browseParameterFiles(true);
      if ( s != null && s.length() > 0 ) {
         controller.loadFile(s);
         // If mosaic has already been determined, then don't change it.
         if (mosaic != 1)
            mosaic = controller.mosaic;
         parameterFileNameText.setText(model.getParameterFilename());
      }
   }
   
   /**
    * NAME: jMenuFileSaveParametersActionPerformed
    * PURPOSE:
    *   To monitor if the file save menu button is pressed and map it to
    *   appropriate handler function.
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    * 
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    * 
    * PROJECT: ModisTool
    * NOTES:
    * @param e An action event object.
    */
   public void jMenuFileSaveParametersActionPerformed(
           java.awt.event.ActionEvent e) {
      saveParametersButtonActionPerformed(e);
   }
   
   /**
    * NAME: jMenuFileExitActionPerformed
    * PURPOSE:
    *   To monitor if the file exit menu button is pressed and map it to
    *   appropriate handler function.
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    * 
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    * 
    * PROJECT: ModisTool
    * NOTES:
    * @param e An action event object.
    */
   public void jMenuFileExitActionPerformed(java.awt.event.ActionEvent e) {
      controller.exitFrame();
   }
   
   /**
    * NAME: jMenuActionRunActionPerformed
    * PURPOSE: 
    *   To monitor if the run menu button is pressed and map it to
    *   appropriate handler function.
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    * 
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    * 
    * PROJECT: ModisTool
    * NOTES:
    * @param e An action event object.
    */
   public void jMenuActionRunActionPerformed(java.awt.event.ActionEvent e) {
      runButtonActionPerformed(e);
   }
   
   /**
    * NAME: jMenuActionAboutActionPerformed
    * PURPOSE: 
    *   To display a message box for About and version info.
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    * 
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    * 
    * PROJECT: ModisTool
    * NOTES:
    * @param e An action event object.
    */
   public void jMenuActionAboutActionPerformed(java.awt.event.ActionEvent e) {
      String eol = System.getProperty("line.separator");
      JOptionPane.showMessageDialog(this,
           "    MODIS Reprojection Tool" + eol + 
           "    Version 4.1" + eol +
           "     March 2011");
   }
   
   /**
    * NAME: jMenuSetDirectoryActionPerformed
    * PURPOSE:
    *   To set the default directory for the input, output
    *   and parameter file choosers
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    *         05/21/02  Karthk Parameswar            Java  Modifications
    * 
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    * 
    * PROJECT: ModisTool
    * NOTES:
    * @param e An action event object.
    */
   public void jMenuSetDirectoryActionPerformed(java.awt.event.ActionEvent e) {
      
      SpecifyDirectory directory;
      if(e.getSource() == jMenuSetInputDirectory){
         directory = new SpecifyDirectory("input");
      } else if(e.getSource() == jMenuSetOutputDirectory) {
         directory = new SpecifyDirectory("output");
      } else{
         directory = new SpecifyDirectory("parameter");
      }
      
      directory.initComponents();
      directory.setVisible(true);
   }
   
   /**
    * NAME: selectedBandsListMouseClicked
    * PURPOSE:
    * To monitor if there is selected Bands List Mouse Click and process
    * it accordingly.
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    * 
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    * 
    * PROJECT: ModisTool
    * NOTES:
    * @param e A key event object.
    */
   public void selectedBandsListKeyPressed(java.awt.event.KeyEvent e) {
   }
   
   /**
    * NAME: selectedBandsListMouseClicked
    * PURPOSE:
    *   To monitor if there is selected Bands List Mouse Click and process
    *   it accordingly.
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    * 
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    * 
    * PROJECT: ModisTool
    * NOTES:
    * @param e A mouse event object.
    */
   public void selectedBandsListMouseClicked(java.awt.event.MouseEvent e) {
      int index = selectedBandsList.getSelectedIndex();
      if ( index < 0 ) {
         removeBandButton.setEnabled(false);
      } else {
         removeBandButton.setEnabled(true);
      }
   }
   
   /**
    * NAME: availableBandsListKeyPressed
    * PURPOSE:
    *   To monitor if there is available Bands List Mouse Clicked and process
    *   it accordingly.
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    * 
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    * 
    * PROJECT: ModisTool
    * NOTES:
    * @param e A key event object.
    */
   public void availableBandsListKeyPressed(java.awt.event.KeyEvent e) {
      int index = availableBandsList.getSelectedIndex();
      if ( index < 0 ) {
         addBandButton.setEnabled(false);
      } else {
         addBandButton.setEnabled(true);
      }
   }
   
   /**
    * NAME: availableBandsListMousePressed
    * PURPOSE:
    *   To monitor if there is selected Bands List Mouse Pressed and process
    *   it accordingly.
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    * 
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    * 
    * PROJECT: ModisTool
    * NOTES:
    * @param e A mouse event object.
    */
   public void availableBandsListMousePressed(java.awt.event.MouseEvent e) {
      int index = availableBandsList.getSelectedIndex();
      if ( index < 0 ) {
         addBandButton.setEnabled(false);
      } else {
         addBandButton.setEnabled(true);
      }
   }
   
   /**
    * NAME: selectedBandsListMousePressed
    * PURPOSE:
    *   To monitor if there is save parameter button pressed and process it
    *   accordingly.
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    * 
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    * 
    * PROJECT:        ModisTool
    * NOTES:
    * @param e A mouse event object.
    */
   public void selectedBandsListMousePressed(java.awt.event.MouseEvent e) {
      int index = selectedBandsList.getSelectedIndex();
      if ( index < 0 ) {
         removeBandButton.setEnabled(false);
      } else {
         removeBandButton.setEnabled(true);
      }
   }
   
   /**
    * NAME: availableBandsListMousePressed
    * PURPOSE:
    *   To monitor if there is selected Bands List Mouse Pressed and process
    *   it accordingly.
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    * 
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    * 
    * PROJECT: ModisTool
    * NOTES:
    * @param e A mouse event object.
    */
   public void inputfilelistMousePressed(java.awt.event.MouseEvent e) {
      int index = inputfilelist.getSelectedIndex();
      if ( index < 0 ) {
         viewMetadataButton.setEnabled(false);
         
         
      } else {
         viewMetadataButton.setEnabled(true);
         
      }
   }
   
   /**
    * NAME: inputfilelistMousePressed
    * PURPOSE:
    *   To monitor if there is selected Bands List Mouse Pressed and process
    *   it accordingly.
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    * 
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    * 
    * PROJECT: ModisTool
    * NOTES:
    * @param e A key event object.
    */
   public void inputfilelistKeyPressed(java.awt.event.KeyEvent e) {
      int index = inputfilelist.getSelectedIndex();
      if ( index < 0 ) {
         viewMetadataButton.setEnabled(false);
         
      } else {
         viewMetadataButton.setEnabled(true);
      }
   }
   /**
    * NAME: viewImageButtonActionPerformed
    * PURPOSE:
    *   To monitor if there is view image  button pressed and process it
    *   by showing the selected files in the image
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    * 
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    * 
    * PROJECT:        ModisTool
    * NOTES:
    * @param e An action event object.
    */
   public void viewImageButtonActionPerformed(java.awt.event.ActionEvent e){
      
      
     /* creates a file called TmpMosaic.prm and stores the selected
      * file names. The first case is encountered when the openInputFile
      * button is clicked
      */
      if(!readInputFilesFromTmpMosaic){
         img.createMosaicFile(selectedfiles, selectedfiles.length);
         img.initComponents();
      } else{
         /* File TmpMosaic.prm is deleted at this point.
          * Input File names are read directly from the
          * JList to an array and then createMosaicFile method is
          * called to create the TmpMosaic.prm file. This case is encountered
          * when the Load Parameter button is clicked
          */
         final ChooserListModel inputLm =
                 (ChooserListModel) inputfilelist.getModel();
         // Theoritically the maximum no. of files that can be specified by
         // the user.
         File[] inputFiles = new File[552];
         int numberOfInputFiles = inputLm.getSize();
         
         for(int i = 0 ; i < numberOfInputFiles; i++){
            String s  = inputLm.getElementAt(i).toString();
            inputFiles[i] = new File(s);
         }
         
         img.createMosaicFile(inputFiles, numberOfInputFiles);
         img.initComponents();
      }
      
      /*Delete tmpMosaic.prm*/
      File temp = new File("TmpMosaic.prm");
      temp.delete();
   }
   
   /**
    * NAME: viewMetadataButtonActionPerformed
    * PURPOSE:
    *   To monitor if there is view Metadata  button pressed and process it
    *   accordingly.
    * RETURN VALUE:
    * PROGRAM HISTORY
    *   Version  Date   Programmer                   Code  Reason
    *   -------  ----   ---------------------------  ----  --------------------
    *                   Gregg T. Stubbendieck, Ph.D. Java  Original development
    * 
    * COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
    * 
    * NOTES:
    * @param e An action event object.
    */
   public void viewMetadataButtonActionPerformed(java.awt.event.ActionEvent e)
   {
      StringBuffer mdFilename = new StringBuffer();
      StringBuffer hdr_Filename = new StringBuffer();
      StringBuffer metFilename = new StringBuffer();
      
      //String inFilename = inputFileNameText.getText();
      //same as model.getInputFilename();
      final int index = inputfilelist.getSelectedIndex();
      //if (index >= 0)
      final ChooserListModel inputLm =
              (ChooserListModel) inputfilelist.getModel();
      
      final Object selectedElement = inputLm.getElementAt(index);
      
      String inFilename = selectedElement.toString();
      viewMetadataButton.setEnabled(false);
      
      
      
      int dotIndex = inFilename.lastIndexOf('.');
      if ( dotIndex >= 0) {
         mdFilename.append(inFilename.substring(0,dotIndex));
         hdr_Filename.append(inFilename.substring(0,dotIndex));
         metFilename.append(inFilename.substring(0,dotIndex));
      } else {
         mdFilename.append(inFilename);
         hdr_Filename.append(inFilename);
         metFilename.append(inFilename);
      }
      
      if(inFilename.endsWith(".hdf")) {
         
         
         String metFile = controller.runDump(inFilename);
         
         MetadataDialog md = new MetadataDialog(metFile,false);
         md.initialize();
         if (md.isFileRead()) {
            md.setVisible(true);
         }
         // }
         
          /*
          else{
          if(new File(mdFilename.append(".hdf.met").toString()).exists())
          {
            MetadataDialog md = new MetadataDialog(mdFilename.toString(),false);
            md.initialize();
            if (md.isFileRead())
            {
                md.setVisible(true);
            }
          }
         else if(new File(hdr_Filename.append("_hdf.met").toString()).exists())
         {
            MetadataDialog md=new MetadataDialog(hdr_Filename.toString(),false);
            md.initialize();
            if (md.isFileRead())
            {
                md.show();
            }
         }
         else if(new File(metFilename.append(".met").toString()).exists())
         {
            MetadataDialog md =
                     new MetadataDialog(metFilename.toString(),false);
            md.initialize();
            if (md.isFileRead())
            {
                md.setVisible(true);
            }
         }
           */
         else {
            JOptionPane.showMessageDialog(this,
                    "Metadata file for "+ inFilename +" does not exist" );
         }
      } else if(inFilename.endsWith(".hdr")) {
         
         if(new File(mdFilename.append(".hdr.met").toString()).exists()) {
            
            MetadataDialog md = new MetadataDialog(mdFilename.toString(),false);
            md.initialize();
            if (md.isFileRead()) {
               md.setVisible(true);
            }
         } else if(
                 new File(hdr_Filename.append("_hdr.met").toString()).exists()){
            MetadataDialog md =
                    new MetadataDialog(hdr_Filename.toString(),false);
            md.initialize();
            if (md.isFileRead()) {
               md.setVisible(true);
            }
         } else if(new File(metFilename.append(".met").toString()).exists()) {
            MetadataDialog md = 
                    new MetadataDialog(metFilename.toString(),false);
            md.initialize();
            if (md.isFileRead()) {
               md.setVisible(true);
            }
         }
         
         else {
            JOptionPane.showMessageDialog(this,
                    "Metadata file for "+ inFilename +" does not exist" );
         }
      }
      
      else {
         JOptionPane.showMessageDialog(this,"Invalid Input file type");
      }
   }
}
