import java.awt.*;
import java.awt.event.*;
//import java.applet.*;
import java.text.*;
import javax.swing.*;

/**
 * This applet demonstrates Q-learning for a particular
 * grid world problem. It isn't designed to be general or reusable.
 * <P>
 * Copyright (C) 2003-2006  <A HREF="http://www.cs.ubc.ca/spider/poole/">David Poole</A>.
 * <p>
 * This program gives the GUI. The GUI code is at <A HREF="Q_GUI.java">Q_GUI.java</A>. The controller code is at <A HREF="Q_Controller.java">Q_Controller.java</A>, and the environment simulation is at <A HREF="Q_Env.java">Q_Env.java</A>.
 * <p>
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.
<p>
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
<p>
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.


 * @author David Poole  poole@cs.ubc.ca
 * @version 0.41 2007-09-09 */



public class Q_GUI extends JApplet
{
    Q_Env environment = new Q_Env();
    Q_Controller controller = new Q_Controller(environment);

    JTextField discountField;
    
    JTextField initialValueField;
    JTextField greedyField;
    JTextField stepField;
    JTextField alphaField;
    final int strutSize=15;

    JLabel reportStepsLabel;
    JLabel reportRewardsLabel;

    int sqsize = 60;
    int twid = 5;
    double brightness = 1.0;

    DecimalFormat df = new DecimalFormat("0.##");
    Dimension gridDimension = new Dimension(sqsize*10,sqsize*10);

    GridPanel graphPanel;

    public void init()
    {
	graphPanel = new GridPanel();

	//setLayout(new FlowLayout());
	JPanel pan = new JPanel();
	pan.setLayout(new BoxLayout(pan,BoxLayout.Y_AXIS));

        JPanel directionPanel = new JPanel();
	directionPanel.setLayout(new BorderLayout());

	JButton up = new JButton("Up");
	directionPanel.add(up,BorderLayout.NORTH);
	up.addActionListener(new ActionListener()
	    {
		public void actionPerformed(ActionEvent event)
		{
		    dostep(0);
		    repaint();
		}
	    });

 
	JButton right = new JButton("Right");
	directionPanel.add(right,BorderLayout.EAST);
	right.addActionListener(new ActionListener()
	    {
		public void actionPerformed(ActionEvent event)
		{
		    dostep(1);
		    repaint();
		}
	    });


	JButton down = new JButton("Down");
	directionPanel.add(down,BorderLayout.SOUTH);
	down.addActionListener(new ActionListener()
	    {
		public void actionPerformed(ActionEvent event)
		{
		    dostep(2);
		    repaint();
		}
	    });


	JButton left = new JButton("Left");
	directionPanel.add(left,BorderLayout.WEST);
	left.addActionListener(new ActionListener()
	    {
		public void actionPerformed(ActionEvent event)
		{
		    dostep(3);
		    repaint();
		}
	    });


	pan.add(Box.createVerticalGlue());

	pan.add(directionPanel);

	pan.add(Box.createVerticalGlue());
	pan.add(Box.createVerticalStrut(strutSize));

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

	discountField = new JTextField(Double.toString(controller.discount),3);
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
	pan.add(Box.createVerticalStrut(strutSize));

	JPanel stepPanel= new JPanel();

	JButton step = new JButton("Step");
	stepPanel.add(step);

	stepField = new JTextField("400",7);
        stepPanel.add(stepField);

	step.setFont(new Font("Serif",1,20));
	step.addActionListener(new ActionListener()
	    {
		public void actionPerformed(ActionEvent event)
		{
		    doSteps(Integer.parseInt(stepField.getText()));
		    repaint();
		}
	    }
				);
	pan.add(stepPanel);

	JPanel greedyPanel = new JPanel();
	greedyPanel.add(new JLabel("Greedy Exploit"));
	greedyField = new  JTextField("80",2);
	greedyPanel.add(greedyField);
	greedyPanel.add(new JLabel("%"));
	pan.add(greedyPanel);

	pan.add(Box.createVerticalGlue());
	pan.add(Box.createVerticalStrut(strutSize));

	JPanel alphaPanel = new JPanel();
	final JCheckBox alphaCheckBox = new JCheckBox("Fixed alpha =");
	alphaCheckBox.addActionListener(new ActionListener()
	    {
		public void actionPerformed(ActionEvent event)
		{
		    controller.alphaFixed = alphaCheckBox.isSelected();
		}
	    });

	alphaPanel.add(alphaCheckBox);
	alphaField = new JTextField("0.4",3);
	alphaPanel.add(alphaField);
	pan.add(alphaPanel);

	pan.add(Box.createVerticalGlue());
	pan.add(Box.createVerticalStrut(strutSize));

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

	pan.add(Box.createVerticalGlue());
	pan.add(Box.createVerticalStrut(strutSize));

	JPanel reportStepsPanel = new JPanel();
	reportStepsLabel = new JLabel("Number of steps: 0");
        reportStepsPanel.add(reportStepsLabel);
	pan.add(reportStepsPanel);

	JPanel reportRewardsPanel = new JPanel();
	reportRewardsLabel = new JLabel("Total reward received: 0");
        reportRewardsPanel.add(reportRewardsLabel);
	pan.add(reportRewardsPanel);



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


	JPanel sizePanel = brightnessPanel;
	sizePanel.add(new JLabel("Size: "));

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

	final JCheckBox tracingCheckBox = new JCheckBox("Trace on console");
	tracingCheckBox.addActionListener(new ActionListener()
	    {
		public void actionPerformed(ActionEvent event)
		{
		    controller.tracing = tracingCheckBox.isSelected();
		    environment.tracing = tracingCheckBox.isSelected();
		}
	    });

	sizePanel.add(tracingCheckBox);

	//	pan.add(sizePanel);



	getContentPane().add(pan,"East");
	getContentPane().add(brightnessPanel,"South");
	JScrollPane gridScrollPane = new JScrollPane(graphPanel);
	getContentPane().add(gridScrollPane,"Center");
	doreset();
	repaint();  //????
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
	    //Graphics2D g2= (Graphics2D) g;

	    //  DecimalFormat df = new DecimalFormat("0.##");
	    for (int xval=0 ; xval <10; xval++) {
		for (int yval=0; yval < 10; yval++) {
		    double val=controller.value(xval,yval);
		    if (val >= 0.0)
			g.setColor(new Color(0,Math.min((int)(255.0* Math.pow(val/10.0,brightness)),255),0));
		    else
			g.setColor(new Color(Math.min((int)(255.0* Math.pow(-val/10.0,brightness)),255),0,0));
		    g.fillRect(xval*sqsize,yval*sqsize,sqsize,sqsize);
		    g.setColor(Color.blue);
		    if (val==controller.qvalues[xval][yval][0]){
			int uptrix[] = {xval*sqsize+sqsize/2-twid,
					xval*sqsize+sqsize/2+twid,
					xval*sqsize+sqsize/2};
			int uptriy[] = {yval*sqsize+sqsize/2,
					yval*sqsize+sqsize/2,
					yval*sqsize};
			g.fillPolygon(uptrix,uptriy,3);
		    }
		    if (val==controller.qvalues[xval][yval][1]){
			int uptriy[] = {yval*sqsize+sqsize/2-twid,
					yval*sqsize+sqsize/2+twid,
					yval*sqsize+sqsize/2};
			int uptrix[] = {xval*sqsize+sqsize/2,
					xval*sqsize+sqsize/2,
					(xval+1)*sqsize};
			g.fillPolygon(uptrix,uptriy,3);
		    }
		    if (val==controller.qvalues[xval][yval][2]){
			int uptrix[] = {xval*sqsize+sqsize/2-twid,
					xval*sqsize+sqsize/2+twid,
					xval*sqsize+sqsize/2};
			int uptriy[] = {yval*sqsize+sqsize/2,
					yval*sqsize+sqsize/2,
					(yval+1)*sqsize};
			g.fillPolygon(uptrix,uptriy,3);
		    }
		    if (val==controller.qvalues[xval][yval][3]){
			int uptriy[] = {yval*sqsize+sqsize/2-twid,
					yval*sqsize+sqsize/2+twid,
					yval*sqsize+sqsize/2};
			int uptrix[] = {xval*sqsize+sqsize/2,
					xval*sqsize+sqsize/2,
					xval*sqsize};
			g.fillPolygon(uptrix,uptriy,3);
		    }
		    g.setColor(Color.white);
		    g.drawString(df.format(controller.qvalues[xval][yval][0]),xval*sqsize+sqsize/3,yval*sqsize+sqsize/3);
		    g.drawString(df.format(controller.qvalues[xval][yval][1]),xval*sqsize+2+sqsize/2,yval*sqsize+2*sqsize/3);
		    g.drawString(df.format(controller.qvalues[xval][yval][2]),xval*sqsize+sqsize/3,(yval+1)*sqsize-1);
		    g.drawString(df.format(controller.qvalues[xval][yval][3]),xval*sqsize+2,yval*sqsize+2*sqsize/3);

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
	    g.setColor(Color.yellow);
	    g.fillOval(environment.currX*sqsize+sqsize/3, environment.currY*sqsize+sqsize/3, 
		       sqsize-2*(sqsize/3), sqsize-2*(sqsize/3));
	    reportStepsLabel.setText("Number of steps: "+environment.numberOfSteps);
	    reportRewardsLabel.setText("Total reward received: "+environment.totalReward);
	}
    }


    public void doreset()
    {     
	environment.doreset();
	controller.doreset(Double.parseDouble(initialValueField.getText()));
    }

    public void dostep(int action) {
	controller.dostep(action,
		    Double.parseDouble(discountField.getText()),
		    Double.parseDouble(alphaField.getText()));
    }



    private void doSteps(int count) {
	controller.doSteps(count,Double.parseDouble(greedyField.getText())/100.0,
		    Double.parseDouble(discountField.getText()),
		    Double.parseDouble(alphaField.getText()));
		repaint();

    }
    

}

