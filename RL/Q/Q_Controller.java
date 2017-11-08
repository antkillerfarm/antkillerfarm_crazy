/**
 * This applet demonstrates Q-learning for a particular
 * grid world problem. It isn't designed to be general or reusable.
<p>
 * Copyright (C) 2003-2006  <A HREF="http://www.cs.ubc.ca/spider/poole/">David Poole</A>.
<p>
 * This program gives Q-learning code. The GUI is in <A HREF="Q_GUI.java">Q_GUI.java</A>. The controller code is at <A HREF="Q_Controller.java">Q_Controller.java</A>, and the environment simulation is at <A HREF="Q_Env.java">Q_Env.java</A>.
<p>
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
 * @version 0.4 2006-12-04 */

public class Q_Controller
{
    /**
       The environment in which the controller operates/
    */
    Q_Env environment;

    /**
       The Q values: Q[xpos,ypos,action]
     */
    public double qvalues[][][] = new double[10][10][4];
    /**
       The number of times the agent has been at (xpos,ypos) and done action
     */
    public int visits[][][] = new int[10][10][4];
    public double discount = 0.9;
    public boolean alphaFixed=false;
    double alpha;
    public boolean tracing=false;

    Q_Controller(Q_Env env)
    {
	environment = env;
    }

    /**
     * resets the Q-values
     *
     * sets all of the Q-values to initVal, and all of the visit counts to 0
     * @param initVal   the initial value to set all values to
     */
    public void doreset(double initVal)
    {     
	for (int xval=0 ; xval <10; xval++) {
	    for (int yval=0; yval < 10; yval++) {
		for (int i=0; i<4; i++) {
		    qvalues[xval][yval][i]=initVal;
		    visits[xval][yval][i]=0;
		}
	    }
	}
    }

    /**
     * does one step
     *
     * carries out the action, and sets the discount and the alpha value
     *
     * @param action  the action that the agent does
     * @param newdiscount  the discount to use
     * @param alphaFieldValue the alpha value to use
     */

    public void dostep(int action, double newdiscount,double alphaFieldValue)  { 
	discount = newdiscount;
	alpha = alphaFieldValue;
	dostep(action);
    }

    /**
     * does one step
     *
     * carries out the action
     *
     * @param action  the action that the agent does
     */
    public void dostep(int action)  { 
        int oldX = environment.currX;
	int oldY = environment.currY;
	double reward = environment.dostep(action);

	int newX = environment.currX;
	int newY = environment.currY;

	// update Q values
	double newVal= value(newX,newY);
	double newDatum=reward+discount*newVal;
	visits[oldX][oldY][action]++;
	if (!alphaFixed)
	    alpha = 1.0/visits[oldX][oldY][action];
//	    alpha = 10.0/(9+visits[oldX][oldY][action]);
		
	if (tracing) {
	    System.out.println("("+oldX+","+oldY+") A="+action+" R="+reward+" ("+
			   newX+","+newY+") newDatum="+newDatum);
	    System.out.print("     Qold="+qvalues[oldX][oldY][action]+" Visits="+visits[oldX][oldY][action]);
	}

	qvalues[oldX][oldY][action]=
	    (1-alpha) * qvalues[oldX][oldY][action] 
	    + alpha*newDatum;
	if (tracing) System.out.println(" Qnew="+qvalues[oldX][oldY][action]);
    }

    /**
     * determines the value of a location
     *
     * the value is the maximum, for all actions, of the q-value
     *
     * @param xval the x-coordinate
     * @param yval the y-coordinate
     * @return the value of the (xval,yval) position
     */
    public double value(int xval, int yval) {
	double val=qvalues[xval][yval][3];
	for (int i=2; i>=0; i--) {
	    if(qvalues[xval][yval][i]>val) {
		val=qvalues[xval][yval][i];
	    }
	}
	return val;
    }

    /**
     * does count number of steps
     *
     * whether each step is greedy or random is determine by greedyProb
     * @param count  the number of steps to do
     * @param greedyProb  the probability that is step is chosen greedily
     * @param newdiscount  the discount to use
     * @param alphaFieldValue the alpha value to use
     */
    public void doSteps(int count, double greedyProb, double newdiscount,double alphaFieldValue){
	discount = newdiscount;
	alpha = alphaFieldValue;
	for(int i=0; i<count; i++)
	    {
		double rand = Math.random();
		if (rand<greedyProb)
		    {// act greedily
			int startDir = (int) (Math.random() * 4);
			double bestVal = qvalues[environment.currX][environment.currY][startDir];
			int bestDir = startDir;
			for (int dir=1; dir<4; dir++)
			    {
				startDir = (startDir+1)%4;
				if (qvalues[environment.currX][environment.currY][startDir] > bestVal)
				    {
					bestVal = qvalues[environment.currX][environment.currY][startDir];
					bestDir = startDir;
				    }
			    }
			dostep(bestDir);
		    }
		else
		    { // act randomly
			dostep((int) (Math.random() * 4));
		    }
	    }
    }

}
