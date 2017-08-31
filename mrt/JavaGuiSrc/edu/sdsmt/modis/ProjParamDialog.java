
/*********************************************************************************
    NAME:	ProjParamDialog.java
	PURPOSE:
		The purpose of this class is to create a GUI interface for application and
		allow clients to edit the projection parameters.


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

import java.lang.reflect.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;

class ProjParamDialog extends JDialog
{
	public final static int CANCEL = 0;
	public final static int OK = 1;
    
    public String s;
    public int i;
    private int result = CANCEL;
    private ModisModel model = ModisModel.getInstance();

    private int projectionId;
    
    private ModisModel model1 = ModisModel.getInstance();
    
    private JTextField[] text = new JTextField[16];

    private JLabel[] label = new JLabel[17];
	
    private String [] datumstrings =
          {"NAD27","NAD83","WGS66","WGS72","WGS84","NoDatum"};
    private String [] sphere_datumstrings = {"NoDatum"};

   private String[][] projInfo = {
      { "", "", "", "", "", "", "", "", "", "", "", "", "", "", "" },

      /* Geographic */
      { "", "", "", "", "", "", "", "", "", "", "", "", "", "", "" },

      /* Hammer */
      { "Sphere","","","","CentMer","","FE","FN","","","","","","","" },

       /* IGH */
      { "Sphere", "", "", "", "", "", "", "", "", "", "", "", "", "", "" },

      /* ISIN */
      { "Sphere","","","","CentMer","","FE","FN","NZone","","RFlag","","",
        "","" },

      /* Lambert Azimuthal */
      { "Sphere","","","","CentLon","CenterLat","FE","FN","","","","","","",""},

      /* LCC */
      {"SMajor","SMinor","STDPR1","STDPR2","CentMer","OriginLat","FE","FN","",
       "","","","","",""},

      /* Mollweide */
      {"Sphere","","","","CentMer","","FE","FN","","","","","","",""},

      /* Polar Stereographic */
      {"SMajor","SMinor","","","LongPol","TrueScale","FE","FN","","","","",
       "","",""},

      /* Sinusoidal */
      {"Sphere","","","","CentMer","","FE","FN","","","","","","",""},

      /* UTM */
      {"Lon/Z","Lat/Z","","","","","","","","","","","","",""},

      /* TM */
      {"SMajor","SMinor","Factor","","CentMer","OriginLat","FE","FN","","",
       "","","","",""},

       /* Albers EA */
      {"SMajor","SMinor","STDPR1","STDPR2","CentMer","OriginLat","FE","FN",
       "","","","","","",""},

      /* Mercator */
      {"SMajor","SMinor","","","CentMer","TrueScale","FE","FN","","","","",
       "","",""},

      /* Equirectangular */
      { "Sphere", "", "", "", "CentMer", "TrueScale", "FE", "FN", "", "", "",
        "",  "",  "",  ""},
	
   };

// IMPORTANT: Source code between BEGIN/END comment pair will be regenerated
// every time the form is saved. All manual changes will be overwritten.
// BEGIN GENERATED CODE
	// member declarations
	javax.swing.JTextField param0Text = new javax.swing.JTextField();
	javax.swing.JTextField param1Text = new javax.swing.JTextField();
	javax.swing.JTextField param2Text = new javax.swing.JTextField();
	javax.swing.JTextField param3Text = new javax.swing.JTextField();
	javax.swing.JTextField param4Text = new javax.swing.JTextField();
	javax.swing.JTextField param5Text = new javax.swing.JTextField();
	javax.swing.JLabel param0Label = new javax.swing.JLabel();
	javax.swing.JLabel param1Label = new javax.swing.JLabel();
	javax.swing.JLabel param2Label = new javax.swing.JLabel();
	javax.swing.JLabel param3Label = new javax.swing.JLabel();
	javax.swing.JLabel param4Label = new javax.swing.JLabel();
	javax.swing.JLabel param5Label = new javax.swing.JLabel();
	javax.swing.JTextField param6Text = new javax.swing.JTextField();
	javax.swing.JLabel param6Label = new javax.swing.JLabel();
	javax.swing.JTextField param7Text = new javax.swing.JTextField();
	javax.swing.JLabel param7Label = new javax.swing.JLabel();
	javax.swing.JTextField param8Text = new javax.swing.JTextField();
	javax.swing.JLabel param8Label = new javax.swing.JLabel();
	javax.swing.JTextField param9Text = new javax.swing.JTextField();
	javax.swing.JLabel param9Label = new javax.swing.JLabel();
	javax.swing.JTextField param10Text = new javax.swing.JTextField();
	javax.swing.JLabel param10Label = new javax.swing.JLabel();
	javax.swing.JTextField param11Text = new javax.swing.JTextField();
	javax.swing.JLabel param11Label = new javax.swing.JLabel();
	javax.swing.JTextField param12Text = new javax.swing.JTextField();
	javax.swing.JLabel param12Label = new javax.swing.JLabel();
	javax.swing.JTextField param13Text = new javax.swing.JTextField();
	javax.swing.JLabel param13Label = new javax.swing.JLabel();
	javax.swing.JTextField param14Text = new javax.swing.JTextField();
	javax.swing.JLabel param14Label = new javax.swing.JLabel();
	javax.swing.JTextField utmText = new javax.swing.JTextField();
	javax.swing.JLabel utmLabel = new javax.swing.JLabel();
	javax.swing.JButton okButton = new javax.swing.JButton();
	javax.swing.JButton cancelButton = new javax.swing.JButton();

        javax.swing.JComboBox sphere_datumcombo =
            new javax.swing.JComboBox(sphere_datumstrings);         
        javax.swing.JComboBox datumcombo =
            new javax.swing.JComboBox(datumstrings);           

        javax.swing.JLabel combolabel = new javax.swing.JLabel();

// END GENERATED CODE

   /*********************************************************************************
    NAME:	 browseInputFiles()
	PURPOSE:
			To construct an editing dialog box for the user to edit
			projection parameters

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
	public ProjParamDialog(Frame owner)
	{
		super(owner,true);

		text[0] = param0Text;
		text[1] = param1Text;
		text[2] = param2Text;
		text[3] = param3Text;
		text[4] = param4Text;
		text[5] = param5Text;
		text[6] = param6Text;
		text[7] = param7Text;
		text[8] = param8Text;
		text[9] = param9Text;
		text[10] = param10Text;
		text[11] = param11Text;
		text[12] = param12Text;
		text[13] = param13Text;
		text[14] = param14Text;
		text[15] = utmText;

		label[0] = param0Label;
		label[1] = param1Label;
		label[2] = param2Label;
		label[3] = param3Label;
		label[4] = param4Label;
		label[5] = param5Label;
		label[6] = param6Label;
		label[7] = param7Label;
		label[8] = param8Label;
		label[9] = param9Label;
		label[10] = param10Label;
		label[11] = param11Label;
		label[12] = param12Label;
		label[13] = param13Label;
		label[14] = param14Label;
		label[15] = utmLabel;
		label[16] = combolabel;		
	}


   /*********************************************************************************
    NAME:		initComponents()
	PURPOSE:
				To initialze an editing dialog box. This function is called after
				the instance is created.

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
	public void initComponents() throws Exception
	{
		// IMPORTANT: Source code between BEGIN/END comment pair will be regenerated
		// every time the form is saved. All manual changes will be overwritten.
		// BEGIN GENERATED CODE
		// the following code sets the frame's initial state
		param0Text.setSize(new java.awt.Dimension(170, 20));
		param0Text.setVisible(true);
		param0Text.setLocation(new java.awt.Point(20, 30));

		param1Text.setSize(new java.awt.Dimension(170, 20));
		param1Text.setVisible(true);
		param1Text.setLocation(new java.awt.Point(200, 30));

		param2Text.setSize(new java.awt.Dimension(170, 20));
		param2Text.setVisible(true);
		param2Text.setLocation(new java.awt.Point(380, 30));

		param3Text.setSize(new java.awt.Dimension(170, 20));
		param3Text.setVisible(true);
		param3Text.setLocation(new java.awt.Point(20, 80));

		param4Text.setSize(new java.awt.Dimension(170, 20));
		param4Text.setVisible(true);
		param4Text.setLocation(new java.awt.Point(200, 80));

		param5Text.setSize(new java.awt.Dimension(170, 20));
		param5Text.setVisible(true);
		param5Text.setLocation(new java.awt.Point(380, 80));

		param0Label.setSize(new java.awt.Dimension(170, 20));
		param0Label.setVisible(true);
		param0Label.setLocation(new java.awt.Point(20, 10));

		param1Label.setSize(new java.awt.Dimension(170, 20));
		param1Label.setVisible(true);
		param1Label.setLocation(new java.awt.Point(200, 10));

		param2Label.setSize(new java.awt.Dimension(168, 22));
		param2Label.setVisible(true);
		param2Label.setLocation(new java.awt.Point(380, 10));

		param3Label.setSize(new java.awt.Dimension(170, 20));
		param3Label.setVisible(true);
		param3Label.setLocation(new java.awt.Point(20, 60));

		param4Label.setSize(new java.awt.Dimension(170, 20));
		param4Label.setVisible(true);
		param4Label.setLocation(new java.awt.Point(200, 60));

		param5Label.setSize(new java.awt.Dimension(170, 20));
		param5Label.setVisible(true);
		param5Label.setLocation(new java.awt.Point(380, 60));

		param6Text.setSize(new java.awt.Dimension(170, 20));
		param6Text.setVisible(true);
		param6Text.setLocation(new java.awt.Point(20, 130));

		param6Label.setSize(new java.awt.Dimension(170, 20));
		param6Label.setVisible(true);
		param6Label.setLocation(new java.awt.Point(20, 110));

		param7Text.setSize(new java.awt.Dimension(170, 20));
		param7Text.setVisible(true);
		param7Text.setLocation(new java.awt.Point(200, 130));

		param7Label.setSize(new java.awt.Dimension(170, 20));
		param7Label.setVisible(true);
		param7Label.setLocation(new java.awt.Point(200, 110));

		param8Text.setSize(new java.awt.Dimension(170, 20));
		param8Text.setVisible(true);
		param8Text.setLocation(new java.awt.Point(380, 130));

		param8Label.setSize(new java.awt.Dimension(170, 20));
		param8Label.setVisible(true);
		param8Label.setLocation(new java.awt.Point(380, 110));

		param9Text.setSize(new java.awt.Dimension(170, 20));
		param9Text.setVisible(true);
		param9Text.setLocation(new java.awt.Point(20, 180));

		param9Label.setSize(new java.awt.Dimension(170, 20));
		param9Label.setVisible(true);
		param9Label.setLocation(new java.awt.Point(20, 160));

		param10Text.setSize(new java.awt.Dimension(170, 20));
		param10Text.setVisible(true);
		param10Text.setLocation(new java.awt.Point(200, 180));

		param10Label.setSize(new java.awt.Dimension(170, 20));
		param10Label.setVisible(true);
		param10Label.setLocation(new java.awt.Point(200, 160));

		param11Text.setSize(new java.awt.Dimension(170, 20));
		param11Text.setVisible(true);
		param11Text.setLocation(new java.awt.Point(380, 180));

		param11Label.setSize(new java.awt.Dimension(170, 20));
		param11Label.setVisible(true);
		param11Label.setLocation(new java.awt.Point(380, 160));

		param12Text.setSize(new java.awt.Dimension(170, 20));
		param12Text.setVisible(true);
		param12Text.setLocation(new java.awt.Point(20, 230));

		param12Label.setSize(new java.awt.Dimension(170, 20));
		param12Label.setVisible(true);
		param12Label.setLocation(new java.awt.Point(20, 210));

		param13Text.setSize(new java.awt.Dimension(170, 20));
		param13Text.setVisible(true);
		param13Text.setLocation(new java.awt.Point(200, 230));

		param13Label.setSize(new java.awt.Dimension(170, 20));
		param13Label.setVisible(true);
		param13Label.setLocation(new java.awt.Point(200, 210));

		param14Text.setSize(new java.awt.Dimension(170, 20));
		param14Text.setVisible(true);
		param14Text.setLocation(new java.awt.Point(380, 230));

		param14Label.setSize(new java.awt.Dimension(170, 20));
		param14Label.setVisible(true);
		param14Label.setLocation(new java.awt.Point(380, 210));

		utmText.setSize(new java.awt.Dimension(40, 25));
		utmText.setVisible(true);
		utmText.setLocation(new java.awt.Point(90, 270));

		utmLabel.setSize(new java.awt.Dimension(70, 20));
		utmLabel.setVisible(true);
		utmLabel.setLocation(new java.awt.Point(20, 270));
		
		combolabel.setSize(new java.awt.Dimension(60,25));
		combolabel.setVisible(true);
		combolabel.setLocation(new java.awt.Point(150,270));

		okButton.setSize(new java.awt.Dimension(70, 25));
		okButton.setVisible(true);
		okButton.setText("OK");
		okButton.setLocation(new java.awt.Point(390, 270));

		cancelButton.setSize(new java.awt.Dimension(80,25));
		cancelButton.setVisible(true);
		cancelButton.setText("Cancel");
		cancelButton.setLocation(new java.awt.Point(470, 270));

		setSize(new java.awt.Dimension(577, 344));
		getContentPane().setLayout(null);
		setTitle("edu.sdsmt.modis.ProjParamFrame");
		setLocation(new java.awt.Point(0, 0));
		getContentPane().add(param0Text);
		getContentPane().add(param1Text);
		getContentPane().add(param2Text);
		getContentPane().add(param3Text);
		getContentPane().add(param4Text);
		getContentPane().add(param5Text);
		getContentPane().add(param0Label);
		getContentPane().add(param1Label);
		getContentPane().add(param2Label);
		getContentPane().add(param3Label);
		getContentPane().add(param4Label);
		getContentPane().add(param5Label);
		getContentPane().add(param6Text);
		getContentPane().add(param6Label);
		getContentPane().add(param7Text);
		getContentPane().add(param7Label);
		getContentPane().add(param8Text);
		getContentPane().add(param8Label);
		getContentPane().add(param9Text);
		getContentPane().add(param9Label);
		getContentPane().add(param10Text);
		getContentPane().add(param10Label);
		getContentPane().add(param11Text);
		getContentPane().add(param11Label);
		getContentPane().add(param12Text);
		getContentPane().add(param12Label);
		getContentPane().add(param13Text);
		getContentPane().add(param13Label);
		getContentPane().add(param14Text);
		getContentPane().add(param14Label);
		getContentPane().add(okButton);
		getContentPane().add(cancelButton);

                if ( model.getOutputProjectionTypeString() == "LA"  ||
                     model.getOutputProjectionTypeString() == "SIN" ||
                     model.getOutputProjectionTypeString() == "IGH" ||
                     model.getOutputProjectionTypeString() == "MOL" ||
                     model.getOutputProjectionTypeString() == "HAM" ||
                     model.getOutputProjectionTypeString() == "ER"  ||
                     model.getOutputProjectionTypeString() == "ISIN" )
                {
                    /* Sphere-based projections */
                    sphere_datumcombo.setSize(new java.awt.Dimension(110,25));
                    sphere_datumcombo.setVisible(true);
                    sphere_datumcombo.setLocation(new java.awt.Point(220,270));
	            getContentPane().add(sphere_datumcombo);
		    sphere_datumcombo.setEnabled(true);
                }
                else
                {
                    /* Ellipse-based projections */
                    datumcombo.setSize(new java.awt.Dimension(110,25));
                    datumcombo.setVisible(true);
                    datumcombo.setLocation(new java.awt.Point(220,270));
	            getContentPane().add(datumcombo);
		    datumcombo.setEnabled(true);
                }

                getContentPane().add(combolabel);
                combolabel.setText("Datum:");
		label[16].setEnabled(true);

    	if (model.getOutputProjectionTypeString() == "UTM" )
	    {
	        getContentPane().add(utmText);
		    getContentPane().add(utmLabel);
            utmLabel.setText("UTM Zone:");
		    text[15].setEnabled(true);
		}
		
		// define action listner and delegate to the appropriate handler
		sphere_datumcombo.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent e) {
				sphere_datumcomboActionPerformed(e);
				
		 }
		});
		
		// define action listner and delegate to the appropriate handler
		datumcombo.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent e) {
				datumcomboActionPerformed(e);
				
		 }
		});
		
		// define action listner and delegate to the appropriate handler
		okButton.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent e) {
				okButtonActionPerformed(e);
			}
		});
		// define action listner and delegate to the appropriate handler
		cancelButton.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent e) {
				cancelButtonActionPerformed(e);
			}
		});
		// define action listner and delegate to the appropriate handler
		addWindowListener(new java.awt.event.WindowAdapter() {
			public void windowClosing(java.awt.event.WindowEvent e) {
				thisWindowClosing(e);
			}
		});
		// END GENERATED CODE
		setTitle("Projection Parameters");

		clearAndDisableAll();
	}

  	private boolean mShown = false;

   /*********************************************************************************
    NAME:		addNotify()
	PURPOSE:
				To add action listner for the appropriate response.

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
	public void addNotify()
	{
		super.addNotify();

		if (mShown)
			return;

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

   /*********************************************************************************
    NAME:		thisWindowClosing()
	PURPOSE:
				To close the window (set inviible) when the close box is clicked

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
	public void thisWindowClosing(java.awt.event.WindowEvent e)
	{
		setVisible(false);
	}

   /*********************************************************************************
    NAME:		cancelButtonActionPerformed()
	PURPOSE:
				To respond to cancel button of this dialog

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
	public void cancelButtonActionPerformed(java.awt.event.ActionEvent e)
	{
		result = CANCEL;
		setVisible(false);
	}

   /*********************************************************************************
    NAME:		clearAndDisableAll()
	PURPOSE:
				To clear and disable all editing box

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
	public void clearAndDisableAll()
	{
		final Class thisClass = getClass();
		for (int i=0; i<15; ++i)
		{
			text[i].setText("0.0");
			label[i].setText("disabled");
			text[i].setEnabled(false);
			label[i].setEnabled(false);
		}
	}

   /*********************************************************************************
    NAME:		getResult()
	PURPOSE:
				To retrieve the result.

	RETURN VALUE:
			String
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
				final means that this function cannot be derived
    *********************************************************************************/
	final int getResult() { return result; }

   /*********************************************************************************
    NAME:		saveValuesToModel()
	PURPOSE:
				To save all values from editing box to model.

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
	public void saveValuesToModel() throws NumberFormatException
	{
		final double[] projValue = new double[15];

		for (int i=0; i<15; ++i)
		{
			if (projInfo[projectionId][i].length() > 0)
			{
				try
				{
					projValue[i] = Double.valueOf(text[i].getText()).doubleValue();
				}
				catch (NumberFormatException e)
				{
					new JOptionPane("Invalid value for parameter: " + projInfo[projectionId][i]).createDialog(this,"Bad Parameter Value").setVisible(true);
					throw e;
				}
			}
			else
			{
				projValue[i] = 0.0;
			}
		}
		for (int i=0; i<15; ++i)
		{
			model.setOutputProjectionParameter(i,projValue[i]);
		}
		if (text[15].getText().length() > 0)
		{
		    int utm = Integer.valueOf(text[15].getText()).intValue();

		    if (utm < -60 || utm > 60)
	        {
		        complain ("Wrong Value","UTM zone must be an integer value from -60 to 60. Please try again...");
		        return;
		    }
		    else
		       model.setUTMZone(Integer.valueOf(text[15].getText()).intValue());
	    }
        
	    
		model1.setDatum(s);
	 }

/*****************************************************************************
        NAME:           sphere_datumcomboActionPerformed()
        PURPOSE:        To respond to OK button on the dialog box
        RETURN VALUE:   String
        PROGRAM HISTORY
        Version  Date     Programmer                  Code Reason
        -------  ----     ------------------------    ---- ------------------
                      Gregg T. Stubbendieck, Ph.D.       Java  Original development

        COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

        PROJECT:        ModisTool
        NOTES:
    ********************************************************************************/
    public void sphere_datumcomboActionPerformed(java.awt.event.ActionEvent event)
        {

        JComboBox cb = (JComboBox)event.getSource();
                i = cb.getSelectedIndex();
                cb.setSelectedItem(cb.getSelectedItem());
                s = (String)cb.getItemAt(i);

    }
/********************************************************************************
        NAME:		datumcomboActionPerformed()
	PURPOSE:        To respond to OK button on the dialog box
	RETURN VALUE:   String
	PROGRAM HISTORY
	Version  Date     Programmer                  Code  Reason
	-------  ----     ------------------------    ----  -----------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:
    ********************************************************************************/
    public void datumcomboActionPerformed(java.awt.event.ActionEvent event)
	{
       
        JComboBox cb = (JComboBox)event.getSource();
		i = cb.getSelectedIndex();
		cb.setSelectedItem(cb.getSelectedItem());
		s = (String)cb.getItemAt(i);
		
    }				
   /*********************************************************************************
    NAME:		okButtonActionPerformed()
	PURPOSE:
				To respond to OK button on the dialog box

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
	public void okButtonActionPerformed(java.awt.event.ActionEvent event)
	{
		result = OK;
		try
		{
			saveValuesToModel();
			setVisible(false);
		}
		catch (NumberFormatException e) {}
	}

   /*********************************************************************************
    NAME:		setupFields()
	PURPOSE:
				To set up various fields

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
	public void setupFields(int projId)
	{
		projectionId = projId;
		for (int i=0; i<15; ++i)
		{
			if (projInfo[projId][i].length() > 0)
			{
				double projValue = model.getOutputProjectionParameter(i);
				text[i].setText(Double.toString(projValue));
				label[i].setText(projInfo[projId][i]);
				text[i].setEnabled(true);
				label[i].setEnabled(true);
			}
		}
		if (model.getUTMZoneString().length() > 0)
		{
		    text[15].setText(model.getUTMZoneString());
		}
		
		
                /* Get the datum value but not for sphere-based projections */
                if ( model.getOutputProjectionTypeString() == "LA"  ||
                     model.getOutputProjectionTypeString() == "SIN" ||
                     model.getOutputProjectionTypeString() == "IGH" ||
                     model.getOutputProjectionTypeString() == "MOL" ||
                     model.getOutputProjectionTypeString() == "HAM" ||
                     model.getOutputProjectionTypeString() == "ER"  ||
                     model.getOutputProjectionTypeString() == "ISIN" )
                {
		    datumcombo.setSelectedIndex(model.getsphereDatumInt());
                }
                else
                {
		    datumcombo.setSelectedIndex(model.getDatumInt());
                }
	}		
            


    /*********************************************************************************
    NAME:	complain
    PURPOSE:
		    To pop up message box when exception occurs
    RETURN VALUE:
    PROGRAM HISTORY
    Version  Date     Programmer                     Code  Reason
    -------  ----     ---------------------------    ----  --------------------
                        Gregg T. Stubbendieck, Ph.D.	 Java  Original development

    COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

    PROJECT:	ModisTool
    NOTES:
    *********************************************************************************/
    public void complain(String title, String text)
    {
        JOptionPane msg = new JOptionPane(text);
        JDialog dialog = msg.createDialog(this,title);
        dialog.setVisible(true);
    }
}
