package edu.sdsmt.modis;

import java.io.*;
import javax.swing.JFileChooser;

/**
 * A class to create a custom dialog box used to specify
 * the default Input, Output and Parameter file directory.
 * This is a JFrame with  JPanels  added to its contentPane.
 * BorderLayout has been used to arrange and resize the
 * various components of this form.
 *
 * @author  Karthik Parameswar
 */
public class SpecifyDirectory extends javax.swing.JFrame {
   
   /**
    * Creates new form SpecifyDirectory.
    */
   
   public SpecifyDirectory(String inout){
      this.inout = inout;
   }
   
   /**
    * Default constructor.
    */
   public SpecifyDirectory(){
      
   }
   
   /**
    * To intialize the GUI components and set up layout for panels.
    * It sets up all component's default states and event triggers.
    */
   public void initComponents() {
      
      inputLabel = new javax.swing.JLabel();
      inputTextField = new javax.swing.JTextField();
      inputPane = new javax.swing.JPanel();
      okButton = new javax.swing.JButton();
      cancelButton = new javax.swing.JButton();
      browseButton = new javax.swing.JButton();
      
      
      
      inputPane.setSize(new java.awt.Dimension(350, 150));
      inputPane.setVisible(true);
      inputPane.setLayout(null);
      inputPane.setLocation(new java.awt.Point(0, 40));
      
      inputPane.add(inputLabel);
      inputPane.add(inputTextField);
      inputPane.add(okButton);
      inputPane.add(cancelButton);
      inputPane.add(browseButton);
      
      if(inout.equals("input")){
         inputLabel.setText("Specify Default Input File Directory");
      }
      if(inout.equals("output")){
         inputLabel.setText("Specify Default Output File Directory");
      }
      if(inout.equals("parameter")){
         inputLabel.setText("Specify Default Parameter File Directory");
      }
      
      inputLabel.setSize(new java.awt.Dimension(290, 20));
      inputLabel.setVisible(true);
      inputLabel.setLocation(new java.awt.Point(10, 5));
      
      inputTextField.setSize(new java.awt.Dimension(310, 25));
      inputTextField.setVisible(true);
      inputTextField.setFont(new java.awt.Font("Bold", 0, 12));
      
      String tmpPathName = null;
      if( inout.equals("input") ) {
         tmpPathName = getInputPathName();
      } else if( inout.equals("output") ) {
         tmpPathName = getOutputPathName();
      } else if( inout.equals("parameter") ) {
         tmpPathName = getParameterPathName();
      }
      if( tmpPathName != null && tmpPathName.length() != 0 )
         inputTextField.setText(tmpPathName);
      
      inputTextField.setEditable(false);
      inputTextField.setBorder(new javax.swing.border.LineBorder(
              new java.awt.Color(0, 0, 0)));
      inputTextField.setLocation(new java.awt.Point(10, 35));
      
      okButton.setSize(new java.awt.Dimension(70, 25));
      okButton.setVisible(true);
      okButton.setEnabled(false);
      
      
      okButton.setText("Ok");
      okButton.setLocation(new java.awt.Point(20, 70));
      
      cancelButton.setSize(new java.awt.Dimension(100, 25));
      cancelButton.setVisible(true);
      cancelButton.setText("Cancel");
      cancelButton.setLocation(new java.awt.Point(100, 70));
      
      browseButton.setSize(new java.awt.Dimension(100, 25));
      browseButton.setVisible(true);
      browseButton.setText("Browse");
      browseButton.setLocation(new java.awt.Point(210, 70));
      
      getContentPane().add(inputLabel);
      getContentPane().add(inputPane);
      setSize(new java.awt.Dimension(350, 150));
      setTitle("edu.sdsmt.modis.ModisFrame");
      setLocation(new java.awt.Point(0, 0));
      setVisible(true);
      
      
      
        /*
        dummyLabel = new javax.swing.JLabel();
        westLabel = new javax.swing.JLabel();
        eastLabel = new javax.swing.JLabel();
        southLabel = new javax.swing.JLabel();
        gridLayoutPanel = new javax.swing.JPanel();
        inputLabel = new javax.swing.JLabel();
        inputTextField = new javax.swing.JTextField();
        borderLayoutPanel = new javax.swing.JPanel();
        northLabel = new javax.swing.JLabel();
        flowLayoutPanel = new javax.swing.JPanel();
        leftLabel = new javax.swing.JLabel();
        okButton = new javax.swing.JButton();
        centreLabel = new javax.swing.JLabel();
        rightLabel = new javax.swing.JLabel();
        cancelButton = new javax.swing.JButton();
        browseButton = new javax.swing.JButton();
         */
      setTitle("Default Directory Location");
      addWindowListener(new java.awt.event.WindowAdapter() {
         public void windowClosing(java.awt.event.WindowEvent evt) {
            exitForm(evt);
         }
      });
/*
        dummyLabel.setPreferredSize(new java.awt.Dimension(150, 5));
        getContentPane().add(dummyLabel, java.awt.BorderLayout.NORTH);
 
        westLabel.setToolTipText("null");
        westLabel.setPreferredSize(new java.awt.Dimension(10, 100));
        getContentPane().add(westLabel, java.awt.BorderLayout.WEST);
 
        eastLabel.setPreferredSize(new java.awt.Dimension(10, 100));
        getContentPane().add(eastLabel, java.awt.BorderLayout.EAST);
 
        southLabel.setPreferredSize(new java.awt.Dimension(150, 10));
        getContentPane().add(southLabel, java.awt.BorderLayout.SOUTH);
 
        gridLayoutPanel.setLayout(new java.awt.GridLayout(3, 0));
 
        if(inout.equals("input")){
            inputLabel.setText("Specify Default Input File Directory");
        }
        if(inout.equals("output")){
           inputLabel.setText("Specify Default Output File Directory");
        }
        if(inout.equals("parameter")){
           inputLabel.setText("Specify Default Parameter File Directory");
        }
 
        inputLabel.setFont(new java.awt.Font("Bold", 1, 14));
        inputLabel.setPreferredSize(new java.awt.Dimension(200, 14));
        inputLabel.setMaximumSize(new java.awt.Dimension(275, 14));
        gridLayoutPanel.add(inputLabel);
 
        inputTextField.setFont(new java.awt.Font("Bold", 0, 12));
        //inputTextField.setPreferredSize(new java.awt.Dimension(300, 11));
        inputTextField.setBorder(new javax.swing.border.LineBorder(
           new java.awt.Color(0, 0, 0)));
        inputTextField.setSize(new java.awt.Dimension(300, 12));
        inputTextField.setEditable(false);
        gridLayoutPanel.add(inputTextField);
 
        borderLayoutPanel.setLayout(new java.awt.BorderLayout());
 
        northLabel.setPreferredSize(new java.awt.Dimension(300, 10));
        borderLayoutPanel.add(northLabel, java.awt.BorderLayout.NORTH);
 
        flowLayoutPanel.setLayout(new java.awt.FlowLayout(
           java.awt.FlowLayout.LEFT, 5, 0));
 
        flowLayoutPanel.add(leftLabel);
 
        okButton.setText("Ok");
        okButton.setEnabled(false);
        flowLayoutPanel.add(okButton);
 
        centreLabel.setPreferredSize(new java.awt.Dimension(14, 17));
        flowLayoutPanel.add(centreLabel);
 
        cancelButton.setText("Cancel");
        flowLayoutPanel.add(cancelButton);
 
        rightLabel.setPreferredSize(new java.awt.Dimension(14, 17));
        flowLayoutPanel.add(centreLabel);
 
        browseButton.setText("Browse");
        flowLayoutPanel.add(browseButton);
 
        borderLayoutPanel.add(flowLayoutPanel, java.awt.BorderLayout.CENTER);
 
        gridLayoutPanel.add(borderLayoutPanel);
 
        getContentPane().add(gridLayoutPanel, java.awt.BorderLayout.CENTER);
 
        //setSize(new Dimension());
        pack();
 */
      
      okButton.addActionListener(new java.awt.event.ActionListener() {
         public void actionPerformed(java.awt.event.ActionEvent e) {
            okButtonActionPerformed(e);
         }
      });
      
      
      cancelButton.addActionListener(new java.awt.event.ActionListener() {
         public void actionPerformed(java.awt.event.ActionEvent e) {
            cancelButtonActionPerformed(e);
         }
      });
      
      
      browseButton.addActionListener(new java.awt.event.ActionListener() {
         public void actionPerformed(java.awt.event.ActionEvent e) {
            browseButtonActionPerformed(e);
         }
      });
      
   }
   
   
   
   /**
    * Ok Button event.
    * This event retrieves the default directory location from the text field
    * and writes it to a file.
    */
   private void okButtonActionPerformed(java.awt.event.ActionEvent e){
      String defaultpathName = inputTextField.getText();
      if(inout.equals("input")){
         writeToFile(defaultpathName, "DefaultInputFileLocation");
      }
      if(inout.equals("output")){
         writeToFile(defaultpathName, "DefaultOutputFileLocation");
      }
      if(inout.equals("parameter")){
         writeToFile(defaultpathName, "DefaultParameterFileLocation");
      }
      
      setPathName(inputTextField.getText());
      this.setVisible(false);
      
      
   }
   
   /**
    * Cancel Button event.
    */
   private void cancelButtonActionPerformed(java.awt.event.ActionEvent e){
      this.setVisible(false);
   }
   
   /**
    * Browse Button event.
    * Opens a JFileChooser for the user to specify the default input, output
    * or Parameter file directory.
    */
   private void browseButtonActionPerformed(java.awt.event.ActionEvent e){
      String directoryLocation = browseFiles();
      if(directoryLocation != null) {
         inputTextField.setText(directoryLocation);
         okButton.setEnabled(true);
      } else{
         return;
      }
   }
   
   public void setPathName(String value){
      pathName = value;
      
   }
   
   /**
    * Method to return the path of the input file directory.
    * @see SpecifyDirectory#getPathName(String)
    */
   public String getInputPathName(){
      
      String inputFileName = "DefaultInputFileLocation";
      
      return getPathName(inputFileName);
   }
   
   /**
    * Method to return the path of the output file directory.
    * @see SpecifyDirectory#getPathName(String)
    */
   public String getOutputPathName(){
      
      String outputFileName = "DefaultOutputFileLocation";
      
      return getPathName(outputFileName);
      
   }
   
   /**
    * Method to return the path of the parameter file directory
    * @see SpecifyDirectory#getPathName(String)
    */
   
   public String getParameterPathName(){
      
      String parameterFileName = "DefaultParameterFileLocation";
      
      return getPathName(parameterFileName);
   }
   
   /**
    * Reads the file from the Users home directory.
    * If the file is empty then current working directory is chosen
    * @ exceptions - IOException when a file is opened
    * @ returns - the path specified by the user
    */
   
   public String getPathName(String fileName){
      
      String pathOfDefaultFiles = System.getProperty("user.home") +
              System.getProperty("file.separator") +fileName ;
      File temp = new File(pathOfDefaultFiles);
      if( !temp.exists() )
         return null;
      
      try {
         RandomAccessFile raf = new RandomAccessFile(temp, "rw");
         length = raf.length();
         
      } catch(IOException e) {
         System.out.println("Error in reading from the temp file");
      }
      
      if(length == 0)
         return pathName;
      return readFile(fileName);
   }
   
   /**
    * To the temp file write the location
    * of the default input file directory.
    * @param directoryname The directory chosen by the user as his default
    * input, output or parameter file directory.
    * @param fileName The name of the file where this directory path is stored.
    */
   private void writeToFile(String directoryname, String fileName){
      
      String pathOfDefaultFiles = System.getProperty("user.home") + 
              System.getProperty("file.separator") +fileName ;
      
      File temp = new File(pathOfDefaultFiles);
      
      try {
         FileOutputStream fos = new FileOutputStream(temp);
         DataOutputStream dos = new DataOutputStream(fos);
         dos.writeUTF(directoryname);
         
         dos.close();
         fos.close();
         
      } catch(IOException e){System.out.println(
              "Error in writing to Default directory file");}
   }
   
   /**
    * Read the temp file to get the location
    * of the default input  file directory.
    * Can afford to use DataInputStream instead of
    * BufferedInputStream since amount of data read is very less.
    * @ returns String - the path where the input, output or parameter files
    * are located
    */
   private String readFile(String fileName){
      
      String pathOfDefaultFiles = System.getProperty("user.home") + 
              System.getProperty("file.separator") +fileName ;
      
      
      try {
         FileInputStream fis = new FileInputStream(pathOfDefaultFiles);
         DataInputStream dis = new DataInputStream(fis);
         path = dis.readUTF();
         
         dis.close();
         fis.close();
         
      } catch(IOException e){
         System.out.println("Error in reading from the Default directory file");
      }
      return path;
      
   }
   
   /**
    * To browse the file system and specify the input, output or parameter
    * file directory.
    * @returns the directory selected by the user
    */
   private String browseFiles(){
      String tmpPathName = null;
      JFileChooser chooser = null;
      
      if( inout.equals("input") ) {
         tmpPathName = getInputPathName();
      } else if( inout.equals("output") ) {
         tmpPathName = getOutputPathName();
      } else if( inout.equals("parameter") ) {
         tmpPathName = getParameterPathName();
      }
      if( tmpPathName != null && tmpPathName.length() != 0 )
         chooser = new JFileChooser(tmpPathName);
      else
         chooser = new JFileChooser();
      
      chooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
      
      int chooseResult = chooser.showOpenDialog(this);
      if ( chooseResult == JFileChooser.APPROVE_OPTION ) {
         try {
            String s = chooser.getSelectedFile().getCanonicalPath();
            return s;
         }
         
         catch(java.io.IOException e) {/* oops */}
      } else{
         this.setVisible(false);
         return null;
      }
      return "";
      
   }
   
   /**
    * Exit the Application
    * @param java.awt.event.WindowEvent
    */
   private void exitForm(java.awt.event.WindowEvent evt) {
      this.setVisible(false);
      
   }
   
   private javax.swing.JPanel inputPane;
   
   /**Ok Button.*/
   private javax.swing.JButton okButton;
   /**Dummy Label.*/
   private javax.swing.JLabel dummyLabel;
   /**Cancel Button.*/
   private javax.swing.JButton cancelButton;
   /** Browse Button. This button is used to open
    * a JFileChooser so that the user can specify
    * the location default directory for input, output
    * and parameter files*/
   private javax.swing.JButton browseButton;
   /**Input Label. Contains the inputLabel text.*/
   private javax.swing.JLabel inputLabel;
   /**Dummy Label. Does not contain any text.*/
   private javax.swing.JLabel rightLabel;
   /**Dummy Label. Does not contain any text.*/
   private javax.swing.JLabel centreLabel;
   /**Dummy Label. Does not contain any text.*/
   private javax.swing.JLabel leftLabel;
   /**Dummy Label. Does not contain any text.*/
   private javax.swing.JLabel northLabel;
   /**Dummy Label. Does not contain any text.*/
   private javax.swing.JLabel southLabel;
   /**Dummy Label. Does not contain any text.*/
   private javax.swing.JLabel eastLabel;
   /**Dummy Label. Does not contain any text.*/
   private javax.swing.JLabel westLabel;
   /** This panel has flow Layout as its layout manager. This panel embeds the
    * Ok, Cancel and the Browse.
    */
   private javax.swing.JPanel flowLayoutPanel;
   /** This panel has borderLayout as its layout manager. This panel embeds
    * the flowLayout Panel. It also embed some dummy labels*/
   private javax.swing.JPanel borderLayoutPanel;
   /** This panel has gridLayout as its layout manager. This panel embeds the
    * borderLayoutPanel with some dummy labels*/
   private javax.swing.JPanel gridLayoutPanel;
   /**This is an inputTextField which would eventually contain the default
    * directory path of input, output or parameter files*/
   private javax.swing.JTextField inputTextField;
   
   /**This String variable would contain the default directory path of the
    * input, output or parameter files.*/
   private static String pathName;
   /**This String variable would contain the default directory path of the 
    * input, output or parameter files if these are read from the file
    * defaultInputFileLocation, defaultOutputFileLocation,
    * defaultParameterFileLocation respectively.*/
   private static String path;
   /**Returns the length of either defaultInputFileLocation or
    * defaultOutputFileLocation or defaultParameterFileLocation measured in
    * bytes depending upon the value stored in inout.  */
   private static long length;
   /**This String variable will decide whether to open the form for specifying
    * defaultInputfileLocation or defaultOutputfileLocation based on the users
    * choice.*/
   private static String inout;
}
