/*-
 * Copyright (c) 2010 Michael Marner <michael@20papercups.net>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */



/*
 * Wiimote Example.
 * @author Michael Marner (marnermr@cs.unisa.edu.au)
 */


/*
 * Include System Headers
 */
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <stdint.h>
#include <sstream>


/*
 * Headers for LibWCL
 */
#include <wcl/wiimote/Wiimote.h>
#include <wcl/maths/Vector.h>


using namespace std;
using namespace wcl;



/**
 * Stores an X/Y coordinate, using for ncurses positions.
 */
struct Pos
{
	int x;
	int y;
};



/**
 * Entry Point!
 */
int main(int argc, char** argv)
{
	// lets try using ncurses for this!
	
	//init the screen
	initscr();

	//change the input mode so we don't wait for a newline before we get input
	cbreak();

	//disable local echo
	noecho();

	//make the curser invisible
	curs_set(0);

	/*
	 * Setup the positions of text boxes.
	 */
	int maxY;
	int maxX;

	getmaxyx(stdscr,maxY,maxX);

	Pos statusPos;
	statusPos.y = maxY-1;
	statusPos.x = 0;

	Pos coordPos;
	coordPos.y = 3;
	coordPos.x = 10;

	Pos infoPos;
	infoPos.y = 10;
	infoPos.x = 5;
	
	printw("Simple Wiimote Demo ~ Michael Marner. Press q to Quit");

	mvprintw(statusPos.y, statusPos.x, "Status: Connecting");
	refresh();

	try 
	{
		Wiimote * wiimote = new Wiimote();

		mvprintw(statusPos.y, statusPos.x, "Status: Connected!");
		refresh();

		bool go = true;

		mvprintw(statusPos.y, statusPos.x, "Status: Streaming!");

		//make getch non blocking
		nodelay(stdscr, true);

		//Loop until we are asked to save or quit
		while (go)
		{
			//get a character
			int c = wgetch(stdscr);

			//just quit
			if (c == 'q')
			{
				mvprintw(statusPos.y, statusPos.x, "Status: Bailing!      ");
				refresh();
				go = false;
			}

			mvprintw(coordPos.y, coordPos.x,   "Button 1    %d", wiimote->getButton(Wiimote::BUTTON_1));
			mvprintw(coordPos.y+1, coordPos.x,   "Button 2    %d", wiimote->getButton(Wiimote::BUTTON_2));
			mvprintw(coordPos.y+2, coordPos.x,   "Button A    %d", wiimote->getButton(Wiimote::BUTTON_A));
			mvprintw(coordPos.y+3, coordPos.x,   "Button B    %d", wiimote->getButton(Wiimote::BUTTON_B));
			mvprintw(coordPos.y+4, coordPos.x,   "Button -    %d", wiimote->getButton(Wiimote::BUTTON_MINUS));
			mvprintw(coordPos.y+5, coordPos.x,   "Button +    %d", wiimote->getButton(Wiimote::BUTTON_PLUS));
			mvprintw(coordPos.y+6, coordPos.x,   "Button Home %d", wiimote->getButton(Wiimote::BUTTON_HOME));

			mvprintw(coordPos.y+8, coordPos.x,   "Button C    %d", wiimote->getButton(Wiimote::BUTTON_C));
			mvprintw(coordPos.y+9, coordPos.x,   "Button Z    %d", wiimote->getButton(Wiimote::BUTTON_Z));

			mvprintw(coordPos.y,   coordPos.x+20  ,   "UP    %d", wiimote->getButton(Wiimote::BUTTON_UP));
			mvprintw(coordPos.y+1, coordPos.x+20,   "DOWN  %d", wiimote->getButton(Wiimote::BUTTON_DOWN));
			mvprintw(coordPos.y+2, coordPos.x+20,   "LEFT  %d", wiimote->getButton(Wiimote::BUTTON_LEFT));
			mvprintw(coordPos.y+3, coordPos.x+20,   "RIGHT %d", wiimote->getButton(Wiimote::BUTTON_RIGHT));

			mvprintw(coordPos.y+5, coordPos.x+20,   "Accel.           %3.0f, %3.0f, %3.0f", wiimote->getAccelerometer()[0], wiimote->getAccelerometer()[1], wiimote->getAccelerometer()[2]);
			mvprintw(coordPos.y+6, coordPos.x+20,   "Nunchuk Accel.   %3.0f, %3.0f, %3.0f", wiimote->getNunchukAccelerometer()[0], wiimote->getNunchukAccelerometer()[1], wiimote->getNunchukAccelerometer()[2]);
			mvprintw(coordPos.y+8, coordPos.x+20,   "Nunchuk Stick.   %3.0f, %3.0f", wiimote->getNunchukStick()[0], wiimote->getNunchukStick()[1]);


			uint16_t rawButtons = wiimote->getRawButtonState();
			stringstream ss;
			for(int i = 15; i>=0; i--) {
				if((1<<i)&rawButtons) 
					ss << 1;
				else 
					ss << 0;
			}

			mvprintw(coordPos.y+11, coordPos.x,   "Raw Button State %s", ss.str().c_str());
			wiimote->update();
			refresh();
		}
	}
	catch (...)
	{
		mvprintw(statusPos.y, statusPos.x, "Status: ERROR - Connection Problem, Giving Up!");
		refresh();
		getch();
		endwin();
		return 0;
	}


	getch();
	endwin();

	return 0;

}

