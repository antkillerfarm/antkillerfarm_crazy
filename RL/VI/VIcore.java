/**
 * This is part of the applet to demonstrates value iteration for a particular
 * grid world problem. It isn't designed to be general or reusable.
 * This is the core part of the code that does the value iteration.
<P>
 * The code is available at <A href="VIcore.java">VIcore.java</A>. 
 * You also need <A href="VIgui.java">VIgui.java</A>. 
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

public class VIcore
{
    // Yes, I know public variables are supposed to be bad form, but it keeps it simple

    // This stores both the Q function and the Value function
    /**
       values[x][y] gives the Value for the (x,y) state
    */
    public double values[][] = new double[10][10];
    /**
       qvalues[x][y][a] gives the Q-value for doing action a in the (x,y) state
    */
    public double qvalues[][][] = new double[10][10][4];
    public double discount = 0.9;
    public boolean absorbing = false;

    /**
     * does one step of value iteration
     *
     * @param newdiscount  the discount to use
     */

    public void dostep(double newdiscount)
    { 
	discount=newdiscount;
	double newvalues[][] = new double[10][10];
	for (int xval=0 ; xval <10; xval++) {
	    for (int yval=0; yval < 10; yval++) {
		qvalues[xval][yval][0] = q(xval,yval,0);
		newvalues[xval][yval]=qvalues[xval][yval][0];
		for (int action=1; action<4; action++) {
		    qvalues[xval][yval][action] = q(xval,yval,action);
		    if (qvalues[xval][yval][action] > newvalues[xval][yval])
			newvalues[xval][yval]=qvalues[xval][yval][action];
		}}}
	values = newvalues;
    }

    /**
       computes the next Q-value from the previous value function
    */
    public double q(int xval, int yval, int action) {
	if (xval==8 && yval==7)
	    {
	    if (absorbing)
		return 10.0;
	    else
		return 10.0 + discount*0.25*(values[0][0]+values[0][9]+values[9][0]+values[9][9]);
	    }
	if (xval==7 && yval==2)
	    {
	    if (absorbing)
		return 3.0;
	    else
		return 3.0 + discount*0.25*(values[0][0]+values[0][9]+values[9][0]+values[9][9]);
	    }

	double newqval = 0.0;

	// determine the part of qvalue that depends on the actual direction
        for (int dir=0; dir < 4; dir++) {
	    double contrib = contribution(xval, yval, dir);
	    if (action == dir)
		newqval += 0.7 * contrib;
	    else 
		newqval += 0.1 * contrib;
	}

	// Add in the rewards that don't depend on the action
	if (xval==3 && yval==4)
	    newqval += -5.0;
	else if (xval==3 && yval==7) 
            newqval += -10.0;

	return newqval;
    }

    /**
       determines the contribution to the q-value if the agent actually went in direction dir from the (xval,yval) location.
       * @param xval the x-position
       * @param yval the y-position
       * @param dir the direction the agent goes (not the action)
     */
    public double contribution(int xval, int yval, int dir) {
	switch (dir){
	case 0:
	    if (yval==0)
		return -1.0 + discount * values[xval][yval];  // crash
	    else
		return discount * values[xval][yval-1];  // no crash
	case 1:
	    if (xval==9)
		return  -1.0 +  discount * values[xval][yval];  // crash
	    else
		return discount * values[xval+1][yval];  // no crash
	case 2:
	    if (yval==9)
		return -1.0 + discount * values[xval][yval];  // crash
	    else
		return discount * values[xval][yval+1];  // no crash
	case 3:
	    if (xval==0)
		return -1.0 + discount * values[xval][yval];  // crash
	    else
		return discount * values[xval-1][yval];  // no crash

	default:  // this should never occur
	    return 0.0;
	}}

    /**
     * resets the Q-values.
     *
     * Sets all of the Q-values to initVal, and all of the visit counts to 0
     * @param initVal   the initial value to set all values to
     */
    public void doreset(double initVal)
    {     
	for (int xval=0 ; xval <10; xval++) {
	    for (int yval=0; yval < 10; yval++) {
		values[xval][yval]=initVal;
		for (int i=0; i<4; i++)
		    qvalues[xval][yval][i]=initVal;
	    }
	}
    }

}
