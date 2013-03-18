/*-
 * Copyright (c) 2008 Michael Marner <michael@20papercups.net>
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


// include system headers
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>

// include openGL headers
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#include <wcl/video/VideoDecoder.h>

using namespace std;
using namespace wcl;

VideoDecoder *decoder;

void usage()
{
    printf("Usage: video filename\n");
}


/**
 * Initialise OpenGL
 */
GLvoid init()
{
	unsigned char *data = new unsigned char[decoder->getWidth() * decoder->getHeight() * 3 /* RGB */];
	glShadeModel(GL_SMOOTH);
	glClearColor(0,0,0,0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// create a texture for the image...
	glTexImage2D(GL_TEXTURE_2D,             //target
				 0,                         //level of detail
				 3,                         //colour components
				 decoder->getWidth(),     //width
				 decoder->getHeight(),    //height
				 0,                         //border
				 GL_RGB,                    //image format
				 GL_UNSIGNED_BYTE,
				 data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	glEnable(GL_TEXTURE_2D);

	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0,1,0,1);
	glMatrixMode(GL_MODELVIEW);
	delete [] data;
}


/**
 * Constantly fetch a new image from the camera to display
 */
GLvoid idle()
{
	glutPostRedisplay();
}


/**
 * Display Loop.
 */
GLvoid display()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	const unsigned char* frame = decoder->getFrame();

	if (frame == NULL)
	{
		decoder->rewind();
		return;
	}

	glTexSubImage2D(GL_TEXTURE_2D,
	                0,
					0,
					0,
					decoder->getWidth(),
					decoder->getHeight(),
					GL_RGB,
					GL_UNSIGNED_BYTE,
					frame);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
		glBegin(GL_QUADS);

		glTexCoord2f(0,1);
		glVertex2f(0,0);

		glTexCoord2f(1,1);
		glVertex2f(1,0);

		glTexCoord2f(1,0);
		glVertex2f(1,1);

		glTexCoord2f(0,0);
		glVertex2f(0,1);
	glEnd();


	glRasterPos2d( 0, 0);

	glDisable(GL_TEXTURE_2D);
	char buffer[100];
	sprintf(buffer, "[Video Test] Frame: %lld - %2.2f FPS", 
		decoder->getCurrentFrame(),
		decoder->getFPS());
	for(char *c=buffer; *c != '\0'; c++)
	    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*c);
	glEnable(GL_TEXTURE_2D);

	glFlush();
	glutSwapBuffers();

	// print out any opengl errors to the console
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		cout << gluErrorString(error) << endl;
	}
}


/**
 * This doesn't do what it is supposed to do!
 */
GLvoid reshape(int width, int height)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0,1,0,1);
	glMatrixMode(GL_MODELVIEW);
	glViewport(0,0, (GLsizei)width, (GLsizei)height);
}

void keyboard(unsigned char key, int w, int h)
{
    if(key==27)
	exit(EXIT_SUCCESS);
}

int main(int argc, char** argv)
{
    if(argc < 2 ){
		usage();
		return 1;
    }

	try {
		cout << "Opening Video... ";

		decoder = new VideoDecoder(argv[1],true);

		cout << "Done!" << endl;

		// Create GLUT window
		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
		glutInitWindowSize(decoder->getWidth(), decoder->getHeight());
		glutCreateWindow(argv[0]);

		init();

		// register callbacks
		glutDisplayFunc(display);
		glutReshapeFunc(reshape);
		glutKeyboardFunc(keyboard);
		glutIdleFunc(idle);

		// GO!
		glutMainLoop();

		delete decoder;

	}
	catch (std::string s)
	{
		cout << "Exception Occured: " << s << endl;
	}

	return 0;
}

