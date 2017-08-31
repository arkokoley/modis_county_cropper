package edu.sdsmt.modis.image;

  
  import java.awt.*;  
  
  /**
   * A canvas that displays an Image.
   * @author Karthik Parameswar
   */
  public class ImageCanvas extends Canvas
  {   
     /**An instance of ImageFrame*/
      private ImageFrame im = ImageFrame.getInstance();
     /**
      * Extends java.awt.Canvas. The image, its width and height are stored
      * internally and used when the image is supposed to be drawn in the
      * paint method.
      * @param Instance of an Image
      * @param width of the image
      * @param height of the image
      */
      public ImageCanvas(Image image, int width, int height)
      {
          super();
          this.image = image;
          this.width = width;
          this.height = height;
          setSize(width, height);
      }
  
      /**
       * Draws image to upper left corner.
       * Overriding the paint method of the Canvas class.
       * We need to draw horizontal and vertical lines on the image to form a grid.
       * @param an instance of Graphics to draw the horizontal and vertical lines.
       */
      public void paint(Graphics g)
      {
          if (image == null)
          {
              super.paint(g);
          }
          else
          {              
              Rectangle rect = getBounds();
              int w = rect.width;
              int h = rect.height;
              
              
              if (w > width && h > height)
              {
                  super.paint(g);                
              }
              g.drawImage(image, 10, 30, this);                      
                              
              g.setColor( Color.cyan);//new Color(255, 0, 0));
              
              for(int i = 0; i< 552; i+=2){               
                      if((im.coordinateValues[i])> -1) {
                          g.fillRect(((im.coordinateValues[i]*20)+11), ((im.coordinateValues[i+1]*20)+31), 19, 19);
                     }                     
                      else{
                          
                          break;
                      }                         
              }
              
             
              /*Drawing horizontal lines
               *The first line has to be drawn carefully
               *The origin of this coordinate system is the canvas and not
               *the rectangle in which the image is enclosed
               */             
              /*
              g.setColor(Color.lightGray); 
              
              g.drawLine(29, 29, 29, 389); 
             
              int horizontalX = 49;
              int horizontalY = 49;
              for(int i =0; i< 35; i++){
                   g.drawLine(horizontalX, 30, horizontalY, 389);
                   horizontalX += 20;
                   horizontalY += 20;
              }
              
              /*Drawing Vertical lines
               *The first line has to be drawn carefully
               *The origin of this coordinate system is the canvas and not
               *the rectangle in which the image is enclosed
               */
            
            /*  g.drawLine(8, 49, 727, 49);
              
              int verticalX = 69;
              int verticalY = 69;
              for(int i =0; i< 17; i++){
                   g.drawLine(9, verticalX, 727, verticalY);
                   verticalX += 20;
                   verticalY += 20;
              }
              */
         }
      }  
      
      /**
       * Sets the Image.
       * Calls the validate method of the component class which
       * ensures that this component has a valid layout.
       * @param instance of an Image 
       */
      public void setImage(Image image)
      {
          this.image = image;
          validate();
      }
  
     /**
      *  The update method of Component  does the following:
      *        Clears this component by filling it with the background color.
      *        Sets the color of the graphics context to be the foreground color of this component.
      *        Calls this component's paint method to completely redraw this component.
      *  Overrides  the update method of the component class.
      * @param an instance of Graphics for updating.
      */
      public void update(Graphics g)
      {
           paint(g);
      }
      /**Instance of an Image*/
      private Image image;
      /**Width of the image*/
      private int width;
      /**Height of the image*/
      private int height;
  }


