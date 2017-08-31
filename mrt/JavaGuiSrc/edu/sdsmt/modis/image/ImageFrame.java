package edu.sdsmt.modis.image;

import java.awt.Dimension;
import java.awt.Frame;
import javax.swing.*;
import java.awt.Image;
import java.awt.ScrollPane;
import java.awt.Toolkit;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;
import java.awt.BorderLayout;
import java.io.*;
import edu.sdsmt.modis.resample.*;

/**
 * To display an image (instance of java.awt.Image} in a window of fixed size.
 * <p>
 * This class is a Singleton.  There will be at most one instance of
 * the class available to an application.  Objects using the Singleton
 * should avoid caching the reference to the instance, since that
 * creates potential existential problems.
 *
 * @author Karthik Parameswar
 */
public class ImageFrame extends JFrame {
   /**
    * A reference to the Image object holding the gif image.
    */
   Image image;
   
   /**
    * A reference to the one and only instance of the class.
    * This is a class level field.
    */
   private static ImageFrame instance = null;
   
   /**
    * The constructor is made private in order to enforce the rule
    * that only this class can create instances of itself.
    * Once created, with this constructor, it is necessary to initialize
    * the object to access the methods.
    */
   private ImageFrame() {}
   
   
   
   /**
    * @return A pointer to the one and only instance of this class.
    *         If one does not already exist, an instance is created
    *         and initialized first.
    */
   public static ImageFrame getInstance() {
      if ( instance == null ) {
         instance = new ImageFrame();
      }
      return instance;
   }
   
   /**
    * To calculate the coordinates values for the files specified by the user.
    * This method calls a C executable mrtmosiac with the filename TmpMosaic.prm
    * TmpMosaic.prm as the command line argument. This executable would then
    * create a file "tile.txt" in the package root directory and to it writes
    * the coordintates points of each file in the following format.
    * h, v where
    * h is horizontal component
    * and v is the vertical component
    * @return the name of file.
    * @exception java.io.IOException
    *              if the executable does not exist or is not in your path.
    * @exception InterruptedException
    *                thrown when a thread is waiting, sleeping,
    *                or otherwise paused for a long time and another thread
    *                interrupts it using the interrupt method in class.
    */
   private String calculateCoordinates() {
      final Runtime rt = Runtime.getRuntime();
      String hdrFilename = null;
      Process p;
      
      try {
         String [] cmd = { "mrtmosaic", "-i", "TmpMosaic.prm", "-t" };
         p = rt.exec(cmd);
      } catch (java.io.IOException e) {
         return null;
      }
      
      int processReturn = 0;
      final Process proc = p;
      
      Thread outThread = new Thread(new Runnable() {
         public void run() {
            InputStream is = proc.getInputStream();
            BufferedReader br = new BufferedReader(new InputStreamReader(is));
            char[] buffer = new char[10];
            for (;;) {
               int numread = 0;
               try {
                  numread = br.read(buffer,0,10);
                  if (numread < 0) {
                     break;
                  }
               } catch (IOException e) {}
            }
         }
      }); //end of Thread outThread
      outThread.start();
      
      Thread errThread = new Thread(new Runnable() {
         public void run() {
            InputStream is = proc.getErrorStream();
            BufferedReader br = new BufferedReader(new InputStreamReader(is));
            char[] buffer = new char[10];
            for (;;) {
               try {
                  int numread = br.read(buffer,0,10);
                  if (numread < 0) {
                     break;
                  }
               } catch (IOException e) {}
            }
         }
      });//end of Thread errThread
      errThread.start();
      
      try {
         processReturn = p.waitFor();
      } catch (InterruptedException e) {
         processReturn=-9999;
      }
      
      if (processReturn!= 0) {
         complain("Abnormal Termination", 
                 "Error determining tile locations for each input tile." );
         return " ";
      } else {
         return "tile.txt";
      }
   }
   
   
   public void complain(String title, String text) {
      JOptionPane msg = new JOptionPane(text);
      JDialog dialog = msg.createDialog(this,title);
      dialog.setVisible(true);
   }
   /**
    * To convert all the input files specified by the
    * user into one file called TmpHdr.hdr
    * @return TmpHdr.hdr.
    * @exception java.io.IOException
    *              if the executable does not exist or is not in your path.
    * @exception InterruptedException
    *              thrown when a thread is waiting, sleeping,
    *              or otherwise paused for a long time and another thread
    *              interrupts it using the interrupt method in class.
    */
   public String convertAllHdrToTmpHdr() {
      final String eol = System.getProperty("line.separator");
      final Runtime rt = Runtime.getRuntime();
      String hdrFilename = null;
      Process p;
      
      try {
         String [] cmd = { "mrtmosaic", "-i", "TmpMosaic.prm", "-h" };
         p = rt.exec(cmd);
      } catch (java.io.IOException e) {
         complain("Mosaic executable Failure", 
                 "Mosaic executable does not exist or is not in your path. " +
                 "This exception occured in Image Frame class line 164");
         return null;
      }
      
      int processReturn = 0;
      final Process proc = p;
      
      Thread outThread = new Thread(new Runnable() {
         public void run() {
            InputStream is = proc.getInputStream();
            BufferedReader br = new BufferedReader(new InputStreamReader(is));
            char[] buffer = new char[10];
            for (;;) {
               int numread = 0;
               try {
                  numread = br.read(buffer,0,10);
                  if (numread < 0) {
                     break;
                  }
               } catch (IOException e) {}
            }
         }
      }); //end of Thread outThread
      outThread.start();
      
      Thread errThread = new Thread(new Runnable() {
         public void run() {
            InputStream is = proc.getErrorStream();
            BufferedReader br = new BufferedReader(new InputStreamReader(is));
            char[] buffer = new char[10];
            for (;;) {
               try {
                  int numread = br.read(buffer,0,10);
                  if (numread < 0) {
                     break;
                  }
               } catch (IOException e) {}
            }
         }
      });//end of Thread errThread
      errThread.start();
      
      try {
         processReturn = proc.waitFor();
      } catch (InterruptedException e) {
         processReturn=-9999;
      }
      
      if (processReturn != 0) {
         complain("Exit Failure Error", "    Error:" + eol +
                  "    One of the selected input files does not exist" + eol +
                  "     or the input files are not the same data " + 
                       "product type.");
         return null;
      } else {
         return "TmpHdr.hdr";
      }
   }
   
   
   /**
    * To initialize the components of the Image Window.
    * Centers the window on the desktop.
    */
   public void initComponents() {
      String tileFile = calculateCoordinates();// calls the executable
      
      if (tileFile.equals(" ")){
         
         return;
      }
      readCoordinatesFromFile(tileFile);
      
      //delete the temp file
      File tempFile= new File(tileFile);
      tempFile.delete();
      
      //Reading the image file from the bin directory
      String imagefile = System.getProperty("mrtdatadir");
      if (imagefile == null) {
         try {
            imagefile = System.getenv("MRT_DATA_DIR");
         } catch (SecurityException sex) {
            imagefile = null;
         }
         if( imagefile == null ) {
            try {
               imagefile = System.getenv("MRTDATADIR");
            } catch (SecurityException sex) {
               imagefile = null;
            }
         }
         if( imagefile == null ) {
            complain("ImageInitComponents", 
                    "ERROR: mrtdatadir is not set in the ModisTool.bat " +
                    "(ModisTool for Unix/Linux) file.  One may set " +
                    "the MRT_DATA_DIR or MRTDATADIR environment variable " +
                    "instead." );
            return;
         }
      }
      
      try {
         imagefile = imagefile.substring(0, imagefile.lastIndexOf(
                 System.getProperty("file.separator")));
         
         imagefile = imagefile.concat(System.getProperty("file.separator") +
                 "bin" + System.getProperty("file.separator"));
      } catch (Exception e) {
         complain("ImageInitComponents", "ERROR: mrtdatadir (" +
                 imagefile + ") is not valid in the ModisTool.bat " +
                 "(ModisTool for Unix/Linux) file or the MRT_DATA_DIR or " +
                 "MRTDATADIR environment variable is not valid.");
         return;
      }
      
      File tmp = new File(imagefile + "world.jpg");
      if (!tmp.exists()) {
         complain("ImageInitComponents", "ERROR: " + imagefile +
                 "world.jpg does not exist!");
         return;
      }
      
      image = Toolkit.getDefaultToolkit().getImage(imagefile + "world.jpg");
      getContentPane().setLayout(new BorderLayout());
      int width = image.getWidth(this);
      int height = image.getHeight(this);
      
      addWindowListener(new java.awt.event.WindowAdapter() {
         public void windowClosing(java.awt.event.WindowEvent evt) {
            setVisible(false);
         }
      });
      
      Dimension screen = Toolkit.getDefaultToolkit().getScreenSize();
      int x1 = (screen.width - width) / 2;
      int y1 = (screen.height - height) / 2;
      if (x1 < 0) {
         x1 = 0;
      }
      if (y1 < 0) {
         y1 = 0;
      }
      setLocation(x1, y1);
      try {
         pane.add(close);
         getContentPane().add(new ImageCanvas(image, width, height));
         getContentPane().add(pane, BorderLayout.SOUTH);
      } catch (Exception e) {
      }
      this.setTitle("Image Window");
      this.setSize(748, 470);
      setVisible(true);
      
      
      close.addActionListener(new java.awt.event.ActionListener() {
         public void actionPerformed(java.awt.event.ActionEvent e) {
            setVisible(false);
         }
      });
   }
   
   /**
    * Reads the coordinates values of the files specified by the user from
    * "tile.txt".  These coordinates values are used to display the selected
    * tile.  The file is parsed and looked for coordinates values.
    * @param Name of the file where the coordinate values are stored
    * @exception IOException when the file is read
    */
   public void readCoordinatesFromFile(String filename){
      
      StreamTokenizer token;
         /*
          * Explicitly intializing the array values to zero.
          */
      for(int i = 0 ; i < coordinateValues.length ; i++)
         coordinateValues[i] = -1;
      
      try{
         
         FileReader fin = new FileReader(filename);
         token = new StreamTokenizer(fin);
         
         int array_index = -1;
         
         READ_LOOP: while (true)	// not end of file
         {
            int tokenType = token.nextToken();
            
            if(tokenType == StreamTokenizer.TT_NUMBER){
               
               coordinateValues[++array_index] = (int)token.nval;
            }
            
            if(tokenType == StreamTokenizer.TT_WORD){
               //do nothing
            }
            
            if (tokenType == StreamTokenizer.TT_EOF){
               break READ_LOOP;
            }
            
         }
         
         
         fin.close();
      } catch(IOException e){complain("Error", "Can't open tile.txt");}
   }
   
   
   /**
    * To convert all the input files specified by the
    * user into one file called TmpHdr.hdr
    * @return TmpHdr.hdr.
    * @exception java.io.IOException
    *              if the executable does not exist or is not in your path.
    * @exception InterruptedException
    *              thrown when a thread is waiting, sleeping,
    *              or otherwise paused for a long time and another thread
    *              interrupts it using the interrupt method in class.
    *
    * public String runMosaic(boolean ishdf, String outputFilePath)
    * {
    * final Runtime rt = Runtime.getRuntime();
    * String outputMosaic = null;
    * if(ishdf){
    * outputMosaic = outputFilePath + "TmpMosaic.hdf";
    * }
    * else{
    * outputMosaic = outputFilePath + "TmpMosaic.hdr";
    * }
    *
    * Process p;
    *
    * try
    * {
    * String [] cmd = {"mrtmosaic", "-i", "TmpMosaic.prm", "-o", outputMosaic};
    * p = rt.exec(cmd);
    * }
    *
    *
    *
    * catch (java.io.IOException e)
    * {
    * complain("Mosaic executable Failure",
    *    "Mosaic executable does not exist or is not in your path. This " +
    *    "exception occured in Image Frame class line 142");
    *
    * return null;
    * }
    *
    *
    * int processReturn = 0;
    *
    *
    * try
    * {
    * processReturn = p.waitFor();
    * }
    *
    * catch (InterruptedException e)
    * {
    * processReturn=-9999;
    *
    * }
    *
    * /*if(processReturn!= 0)
    * {
    *
    *
    * if (processReturn != 0)
    * {
    * complain("Exit Failure Error", "    Error:\n    One of the selected " +
    *    "input files does not exist\n     or the input files are not the " +
    *    "same data product type.");
    *
    * return null;
    * }
    *
    * else
    * {
    *
    * return outputMosaic;
    * }
    *
    * }   */
   
   
   /**
    * Writes the name of input
    * file/ files specified by the user to "TmpMosaic.prm".
    * @param name of the files selected by the user
    * @params length of the array in which these files are stored
    */
   public void createMosaicFile(File[] name, int length) {
      File temp = new File("TmpMosaic.prm");
      FileWriter fos;
      PrintWriter pos;
      String inFilename;
      
      try {
         fos = new FileWriter(temp);
         pos = new PrintWriter(fos);
         
         for(int i=0; i<length; i++){
            
            inFilename = name[i].toString();
            /* If filename contains a space, then surround with quotes. */
            if( inFilename.indexOf(' ') == -1 ) {
               pos.println(inFilename);
            } else {
               pos.println("\"" + inFilename + "\"");
            }
         }
         
         pos.close();
         fos.close();
      }
      
      catch(IOException e) {
         complain("Write Error", "Error in writing to TmpMosaic.prm file");
      }
      
      
   }
   
   /**A JPanel that would hold the button.*/
   private JPanel pane = new JPanel();
   /**A JButton that would close the window.*/
   private JButton close = new JButton("Close ");
   
   /**
    * An integer array containing the coordinate values .
    * The even subscripts coordinateValues[0], coordinateValues[2] contain
    * the h- values and the odd subscripts coordinateValues[1] would contain
    * the v- values.
    */
   int [] coordinateValues = new int[552];
   
}


