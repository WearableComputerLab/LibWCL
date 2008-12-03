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
#include <iostream>
#include <fstream>

// include openGL headers
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <wcl/uvccam/UVCCamera.h>

using namespace std;
using namespace wcl;

UVCCamera* cam;
unsigned char* data;


/**
 * Converts a single pixel from yuv to rgb
 */
int convertPixel(int y, int u, int v)
{
	unsigned int pixel32 = 0;
	unsigned char *pixel = (unsigned char *)&pixel32;
	int r, g, b;

	r = y + (1.370705 * (v-128));
	g = y - (0.698001 * (v-128)) - (0.337633 * (u-128));
	b = y + (1.732446 * (u-128));

	if(r > 255) r = 255;
	if(g > 255) g = 255;
	if(b > 255) b = 255;
	if(r < 0) r = 0;
	if(g < 0) g = 0;
	if(b < 0) b = 0;

	pixel[0] = r * 220 / 256;
	pixel[1] = g * 220 / 256;
	pixel[2] = b * 220 / 256;

	return pixel32;
}

/**
 * Converts a YUYV buffer to an RGB buffer
 */
int convertImage(unsigned char *yuv, unsigned char *rgb, unsigned int width, unsigned int height)
{
	unsigned int in, out = 0;
	unsigned int pixel_16;
	unsigned char pixel_24[3];
	unsigned int pixel32;
	int y0, u, y1, v;

	for(in = 0; in < width * height * 2; in += 4)
	{
		pixel_16 =
			yuv[in + 3] << 24 |
			yuv[in + 2] << 16 |
			yuv[in + 1] <<  8 |
			yuv[in + 0];

		y0 = (pixel_16 & 0x000000ff);
		u  = (pixel_16 & 0x0000ff00) >>  8;
		y1 = (pixel_16 & 0x00ff0000) >> 16;
		v  = (pixel_16 & 0xff000000) >> 24;

		pixel32 = convertPixel(y0, u, v);
		pixel_24[0] = (pixel32 & 0x000000ff);
		pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
		pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;

		rgb[out++] = pixel_24[0];
		rgb[out++] = pixel_24[1];
		rgb[out++] = pixel_24[2];

		pixel32 = convertPixel(y1, u, v);
		pixel_24[0] = (pixel32 & 0x000000ff);
		pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
		pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;

		rgb[out++] = pixel_24[0];
		rgb[out++] = pixel_24[1];
		rgb[out++] = pixel_24[2];
	}

	return 0;
}


/**
 * Initialise OpenGL
 */
GLvoid init()
{
	glShadeModel(GL_SMOOTH);
	glClearColor(0,0,0,0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	data = new unsigned char[640*480*3];

	// create a texture for the image...
	glTexImage2D(GL_TEXTURE_2D,             //target
				 0,                         //level of detail
				 3,                         //colour components
				 640,                       //width
				 480,                       //height
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

	unsigned char* frame = cam->getFrame();

	convertImage(frame, data, 640, 480);

	glTexSubImage2D(GL_TEXTURE_2D,
	                0,
					0,
					0,
					640,
					480,
					GL_RGB,
					GL_UNSIGNED_BYTE,
					data);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
		glColor3f(1,0,0);
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
}


int main(int argc, char** argv)
{
	try {
		// Open the camera
		cout << "Opening Camera... ";
		cam = new UVCCamera("/dev/video0");
		cout << "Done!" << endl;

		/*
		 * Print out camera details
		 */
		//cam.printDetails();

		/*
		 * set the video format...
		 */
		cout << "Setting camera format... ";
		cam->setFormat(UVCCamera::YUYV, 640, 480);
		cout << "Done!" << endl;

		if (cam->setExposureMode(UVCCamera::AUTO_APERTURE_PRIORITY))
		{
			cout << "Set exposure mode!" << endl;
		}
		else
		{
			cout << "Unable to set exposure mode!" << endl;
		}

		//set power frequency compensation to 50 Hz
		cam->setControlValue(UVCCamera::POWER_FREQUENCY, 1);

		//cout << "about to grab frame, length is: " << cam.getBufferSize() << endl;

		// Create GLUT window
		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
		glutInitWindowSize(640, 480);
		glutInitWindowPosition(100,100);
		glutCreateWindow(argv[0]);

		init();

		// register callbacks
		glutDisplayFunc(display);
		glutReshapeFunc(reshape);
		glutIdleFunc(idle);

		// GO!
		glutMainLoop();

		cout << "Deleting Cam" << endl;

		cam->shutdown();
		delete cam;

	}
	catch (std::string s)
	{
		cout << "Exception Occured: " << s << endl;
	}

	return 0;
}

