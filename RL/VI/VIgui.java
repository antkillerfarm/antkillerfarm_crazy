
import java.awt.*;
import java.awt.event.*;
import java.applet.*;
import java.text.*;
import javax.swing.*;
import javax.swing.event.*;

/**
 * This applet demonstrates value iteration for a particular
 * grid world problem. It isn't designed to be general or reusable.
 * This code provides the GUI. The value iteration code is in VIcore.java
<P>
 * The code is available at <A href="VIgui.java">VIgui.java</A>. 
 * You also need <A href="VIcore.java">VIcore.java</A>. 
<P>
 * Copyright (C) 2006-2007 David Poole. 
<P>
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.
<P>
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
<P>
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.


 * @author David Poole  poole@cs.ubc.ca
 * @version 0.3 2006-12-04 */


public class VIgui extends JApplet 
{   
    VIcore core = new VIcore();

    JTextField discountField;
    
    JTextField initialValueField;
    JCheckBox absorbingCheckBox;

    int sqsize = 50;
    int twid = 5;
    double brightness = 1.0;

    DecimalFormat df = new DecimalFormat("0.##");
    Dimension gridDimension = new Dimension(sqsize*10,sqsize*10);

    int fontSize = 14;
    Font myFont = new Font("SansSerif", Font.PLAIN, fontSize);

    GridPanel graphPanel;

    public void init()
    {
	graphPanel = new GridPanel();

	JPanel pan = new JPanel();
	pan.setLayout(new BoxLayout(pan,BoxLayout.Y_AXIS));

	pan.add(Box.createVerticalGlue());

	JPanel stepPanel = new JPanel();

	JButton step = new JButton("Step");
	step.setFont(new Font("Serif",1,20));

	class StepListener implements ActionListener
	{
	    public void actionPerformed(ActionEvent event)
	    {
		dostep();
		repaint();
	    }
	}
	step.addActionListener(new StepListener());
	stepPanel.add(step);
	pan.add(stepPanel);



	JPanel discountPanel = new JPanel();
	discountPanel.add(new JLabel("Discount"));

	JButton decrement = new JButton("-");
	discountPanel.add(decrement);
	decrement.addActionListener(new ActionListener()
	    {
		public void actionPerformed(ActionEvent event)
		{
		    discountField.setText(  df.format(Double.parseDouble(discountField.getText())-0.1)); 
		    repaint();
		}
	    }
				    );

	discountField = new JTextField(Double.toString(core.discount),3);
	discountPanel.add(discountField);
    
	JButton increment = new JButton("+");
	discountPanel.add(increment);
	increment.addActionListener(new ActionListener()
	    {
		public void actionPerformed(ActionEvent event)
		{
		    discountField.setText(  df.format(Double.parseDouble(discountField.getText())+0.1)); 
		    repaint();
		}
	    }
				    );

	pan.add(discountPanel);

	pan.add(Box.createVerticalGlue());

	JPanel resetPanel = new JPanel();

	JButton reset = new JButton("Reset");
	reset.setFont(new Font("Serif",1,20));
	reset.addActionListener(new ActionListener()
	    {
		public void actionPerformed(ActionEvent event)
		{
		    doreset();
		    repaint();
		}
	    }
				);

	resetPanel.add(reset);
	pan.add(resetPanel);

	JPanel initialValuePanel = new JPanel();
	initialValuePanel.add(new JLabel("Initial Value"));
	initialValueField = new JTextField("0.0",3);
	initialValuePanel.add(initialValueField);
	pan.add(initialValuePanel);

	JPanel brightnessPanel = new JPanel();
	brightnessPanel.add(new JLabel("Brightness"));

	JButton dimmer = new JButton("-");
	brightnessPanel.add(dimmer);
	dimmer.addActionListener(new ActionListener()
	    {
		public void actionPerformed(ActionEvent event)
		{
		    brightness = brightness*1.1;
		    repaint();
		}
	    });
	JButton resetBrightness = new JButton("0");
	brightnessPanel.add(resetBrightness);
	resetBrightness.addActionListener(new ActionListener()
	    {
		public void actionPerformed(ActionEvent event)
		{
		    brightness = 1.0;
		    repaint();
		}
	    });

	JButton brighter = new JButton("+");
	brightnessPanel.add(brighter);
	brighter.addActionListener(new ActionListener()
	    {
		public void actionPerformed(ActionEvent event)
		{
		    brightness = brightness/1.1;
		    repaint();
		}
	    });

	JPanel fontSizePanel = brightnessPanel;
	fontSizePanel.add(new JLabel("Font Size"));

	JButton smaller = new JButton("-");
	fontSizePanel.add(smaller);
	smaller.addActionListener(new ActionListener()
	    {
		public void actionPerformed(ActionEvent event)
		{
		    fontSize--;
		    myFont = new Font("SansSerif", Font.PLAIN, fontSize);
		    repaint();
		}
	    });
	// 		JButton resetFontSize = new JButton("12");
	// 		fontSizePanel.add(resetFontSize);
	// 		resetFontSize.addActionListener(new ActionListener()
	// 		{
	// 			public void actionPerformed(ActionEvent event)
	// 			{
	// 				fontSize = 12;
	//                              myFont = new Font("SansSerif", Font.PLAIN, fontSize);
	// 				repaint();
	// 			}
	// 		});

	JButton bigger = new JButton("+");
	fontSizePanel.add(bigger);
	bigger.addActionListener(new ActionListener()
	    {
		public void actionPerformed(ActionEvent event)
		{
		    fontSize++;
		    myFont = new Font("SansSerif", Font.PLAIN, fontSize);
		    repaint();
		}
	    });


	JPanel sizePanel = brightnessPanel;
	sizePanel.add(new JLabel("Grid Size: "));

	JButton shrink = new JButton("-");
	sizePanel.add(shrink);
	shrink.addActionListener(new ActionListener()
	    {
		public void actionPerformed(ActionEvent event)
		{
		    sqsize -= 5;
		    gridDimension.setSize(sqsize*10,sqsize*10);
		    graphPanel.setPreferredSize(gridDimension);
		    graphPanel.revalidate();
		    repaint();
		}
	    });

	JButton grow = new JButton("+");
	sizePanel.add(grow);
	grow.addActionListener(new ActionListener()
	    {
		public void actionPerformed(ActionEvent event)
		{
		    sqsize += 5;
		    gridDimension.setSize(sqsize*10,sqsize*10);
		    graphPanel.setPreferredSize(gridDimension);
		    graphPanel.revalidate();
		    repaint();
		}
	    });

	pan.add(sizePanel);

	JPanel absorbingPanel = new JPanel();
	absorbingCheckBox = new JCheckBox("Absorbing States");
	absorbingCheckBox.addActionListener(new ActionListener()
	    {
		public void actionPerformed(ActionEvent event)
		{
		    core.absorbing = absorbingCheckBox.isSelected();
		}
	    });


	absorbingPanel.add(absorbingCheckBox);
	pan.add(absorbingPanel);


	getContentPane().add(pan,"East");
	getContentPane().add(brightnessPanel,"South");
	JScrollPane gridScrollPane = new JScrollPane(graphPanel);
	getContentPane().add(gridScrollPane,"Center");
    }


    private class GridPanel extends JPanel
    {
	public GridPanel()
	{
	    setPreferredSize(gridDimension);
	}

	public void paintComponent(Graphics g)
	{
	    super.paintComponent(g);
	    Graphics2D g2= (Graphics2D) g;
	    g2.setFont(myFont);
	  	
	    for (int xval=0 ; xval <10; xval++) {
		for (int yval=0; yval < 10; yval++) {
		    if (core.values[xval][yval] >= 0.0)
			g.setColor(new Color(0,Math.min((int)(255.0* Math.pow(core.values[xval][yval]/10.0,brightness)),255),0));
		    else
			g.setColor(new Color(Math.min((int)(255.0* Math.pow(-core.values[xval][yval]/10.0,brightness)),255),0,0));
		    g.fillRect(xval*sqsize,yval*sqsize,sqsize,sqsize);
		    g.setColor(Color.blue);
		    if (core.values[xval][yval]==core.qvalues[xval][yval][0]){
			int uptrix[] = {xval*sqsize+sqsize/2-twid,
					xval*sqsize+sqsize/2+twid,
					xval*sqsize+sqsize/2};
			int uptriy[] = {yval*sqsize+sqsize/2,
					yval*sqsize+sqsize/2,
					yval*sqsize};
			g.fillPolygon(uptrix,uptriy,3);
		    }
		    if (core.values[xval][yval]==core.qvalues[xval][yval][1]){
			int uptriy[] = {yval*sqsize+sqsize/2-twid,
					yval*sqsize+sqsize/2+twid,
					yval*sqsize+sqsize/2};
			int uptrix[] = {xval*sqsize+sqsize/2,
					xval*sqsize+sqsize/2,
					(xval+1)*sqsize};
			g.fillPolygon(uptrix,uptriy,3);
		    }
		    if (core.values[xval][yval]==core.qvalues[xval][yval][2]){
			int uptrix[] = {xval*sqsize+sqsize/2-twid,
					xval*sqsize+sqsize/2+twid,
					xval*sqsize+sqsize/2};
			int uptriy[] = {yval*sqsize+sqsize/2,
					yval*sqsize+sqsize/2,
					(yval+1)*sqsize};
			g.fillPolygon(uptrix,uptriy,3);
		    }
		    if (core.values[xval][yval]==core.qvalues[xval][yval][3]){
			int uptriy[] = {yval*sqsize+sqsize/2-twid,
					yval*sqsize+sqsize/2+twid,
					yval*sqsize+sqsize/2};
			int uptrix[] = {xval*sqsize+sqsize/2,
					xval*sqsize+sqsize/2,
					xval*sqsize};
			g.fillPolygon(uptrix,uptriy,3);
		    }
		    g.setColor(Color.white);
		    g.drawString(df.format(core.values[xval][yval]),xval*sqsize+1,(yval+1)*sqsize-1);
		};
	    };
	    g.setColor(new Color(0,0,255));
	    g.drawLine(0,0,0,10*sqsize);
	    g.drawLine(0,0,10*sqsize,0);
	    g.drawLine(10*sqsize,0,10*sqsize,10*sqsize);
	    g.drawLine(0,10*sqsize,10*sqsize,10*sqsize);
	    g.setColor(Color.white);
	    for (int counter = 1 ; counter <= 10; counter++) {
		g.drawLine(sqsize*counter,0,sqsize*counter,10*sqsize);
		g.drawLine(0,sqsize*counter,10*sqsize,sqsize*counter);
	    }
	    g.setColor(Color.black);
	    //g.drawString(df.format(discount),10*sqsize+2,10);
	}
    }

    public void doreset()
    {     
	core.doreset(Double.parseDouble(initialValueField.getText()));
    }

    public void dostep()
    { 
	core.dostep(Double.parseDouble(discountField.getText()));
    }

}

