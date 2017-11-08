/**
 * This applet demonstrates Q-learning for a particular
 * grid world problem. It isn't designed to be general or reusable.
<p>
 * Copyright (C) 2003-2006  <A HREF="http://www.cs.ubc.ca/spider/poole/">David Poole</A>.
<p>
 * This program gives Q-learning code. The GUI is in <A HREF="Q_GUI.java">Q_GUI.java</A>.  The controller code is at <A HREF="Q_Controller.java">Q_Controller.java</A>, and the environment simulation is at <A HREF="Q_Env.java">Q_Env.java</A>.
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

public class Q_Env
{
    public int numberOfSteps=0;
    public double totalReward=0.0;
    public int currX = (int) (Math.random() * 10);  // current X position
    public int currY = (int) (Math.random() * 10);  // current Y position
    public boolean tracing=false;

    /**
     * resets the steps and the reward.
     */
    public void doreset()
    {     
	numberOfSteps=0;
	totalReward=0.0;
    }



    /**
     * does one step
     *
     * carries out the action
     *
     * @param action  the action that the agent does
     * @return reward received
     */
    public double dostep(int action)  { 
	int actualDirection;
	double reward;
        int newX,newY;
	int rand = (int) (Math.random() * 10); 
	if (rand < 4) actualDirection=rand;
	else actualDirection=action;
	
	reward=0.0;
	if ((currX==8 && currY==7) || (currX==7 && currY==2)) {
            if (currX==8)
		reward=10.0;
	    else
		reward=3.0;
	    // Go to a random corner
	    newX = ((int) (Math.random() * 2)) *9;
	    newY = ((int) (Math.random() * 2)) *9;
	}
	else 
	    {
		if (currX==3 && currY==4)
		    reward = -5.0;
		else if (currX==3 && currY==7) 
		    reward = -10.0;
	    
		// create new position & get hitting-wall rewards
		switch (actualDirection) {
		case 0: // Up
		    if (currY==0) {
			newY=currY;
			newX=currX;
			reward=-1.0;}
		    else {
			newY=currY-1;
			newX=currX;}
		    break;
		case 1: // Right
		    if (currX==9) {
			newY=currY;
			newX=currX;
			reward=-1.0;}
		    else {
			newY=currY;
			newX=currX+1;}
		    break;
		case 2: // Down
		    if (currY==9) {
			newY=currY;
			newX=currX;
			reward=-1.0;}
		    else {
			newY=currY+1;
			newX=currX;}
		    break;
		case 3: // Left
		    if (currX==0) {
			newY=currY;
			newX=currX;
			reward=-1.0;}
		    else {
			newY=currY;
			newX=currX-1;}
		    break;
		default:   // should never occur
		    {
			newX=0;
			newY=0;
			reward=0.0;
		    }
		}
	    }


	numberOfSteps++;
	totalReward+= reward;

	currX=newX;
	currY=newY;
	return reward;
    }


}
