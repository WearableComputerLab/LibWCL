/*-
 * Copyright (c) 2011 Benjamin Close <Benjamin.Close@clearchain.com>
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
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <wcl/Exception.h>
#include <wcl/camera/CameraFactory.h>

using namespace std;
using namespace wcl;

static Camera *cam;
static unsigned int pixelx=-1,pixely=-1;

void usage()
{
    printf("selector [devicenode]\n"
	   "Selector takes a image from a camera and displays it to the screen\n"
	   "Once displayed the user can select a pixel and find out it's x,y & other details\n"
	   "\n"
	   "devicenode - the device to use, if not specified the first available camera will be used\n");
}


/**
 * Initialise OpenGL
 */
GLvoid init()
{
	unsigned char* data;
	glShadeModel(GL_SMOOTH);
	glClearColor(0,0,0,0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	data = new unsigned char[cam->getFormatBufferSize(Camera::RGB8)];

	// create a texture for the image...
	glTexImage2D(GL_TEXTURE_2D,             //target
				 0,                         //level of detail
				 3,                         //colour components
				 cam->getActiveConfiguration().width,     //width
				 cam->getActiveConfiguration().height,    //height
				 0,                         //border
				 GL_RGB,                    //image format
				 GL_UNSIGNED_BYTE,
				 data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	glEnable(GL_TEXTURE_2D);

	delete data;
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
    static int flashstate= 0;
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glEnable(GL_TEXTURE_2D);
    const unsigned char* frame = cam->getFrame(Camera::RGB8);
    glTexSubImage2D(GL_TEXTURE_2D,
		    0,
		    0,
		    0,
		    cam->getActiveConfiguration().width,
		    cam->getActiveConfiguration().height,
		    GL_RGB,
		    GL_UNSIGNED_BYTE,
		    frame);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0,1,0,1);

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

    glDisable(GL_TEXTURE_2D);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,cam->getActiveConfiguration().width,cam->getActiveConfiguration().height,0, 0, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // If a pixel is selected flash the pixel;
    if(pixelx >= 0 ){
	if( flashstate ){
	    glColor3f(1.0,0.0,0.0);
	}
	else {
	    glColor3f(1.0, 1.0, 1.0);
	}
	flashstate = !flashstate;

	glPointSize(2.0);
	glBegin(GL_POINTS);
	glVertex2f(pixelx, pixely);
	glEnd();
    }

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
	glViewport(0,0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int w, int h)
{
    if(key==27)
	exit(EXIT_SUCCESS);
}

void mouse(int button, int state, int x, int y)
{
    unsigned char buffer[3];

    if(state==1){
	pixelx = x;
	pixely = y;
	glReadPixels(x,y,1,1,GL_RGB,GL_UNSIGNED_BYTE,&buffer);

	//these conversion formulas come from wikipedia
	float r = buffer[0];
	float g = buffer[1];
	float b = buffer[2];
	const float wR = 0.299;
	const float wG = 0.114;
	const float wB = 0.587;
	const float uMax = 0.436;
	const float vMax = 0.615;

	float Y = wR* + wG*g + wB*b;
	float U = uMax * ((b - y)/(1-wB));
	float V = vMax * ((r * - y)/(1-wR));

	// Display information to console
	printf("--------------------------\n"
	       "Point: %d x, %d y\n"
	       "RGB: r %f, g %f, b %f\n"
	       "YUV: y %f, u %f, v %f\n",
	       x,y,
	       r,g,b,
	       Y,U,V );
    }
}

int main(int argc, char** argv)
{
    if( argc > 2 || (argc == 2 && strcasecmp(argv[1],"-h") == 0)){
	usage();
	return 1;
    }

    try {
	std::vector<Camera *> cameras =
	    CameraFactory::getCameras();

	if( cameras.size() == 0 ){
	    cout << "No Camera's found" << endl;
	    return 0;
	}

	if( argc ==2 ){
	    cam = CameraFactory::getCamera(argv[1]);
	} else {
	    cam = CameraFactory::getCamera();
	}

	if( cam == NULL ){
	    cout << "Camera not found" << endl;
	    return 1;
	}

	// Create GLUT window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(cam->getActiveConfiguration().width, cam->getActiveConfiguration().height);
	glutCreateWindow(argv[0]);

	init();

	// register callbacks
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutIdleFunc(idle);

	// GO!
	glutMainLoop();
    }
    catch (Exception &e)
    {
	cout << "Exception Occured: " << e.what() << endl;
	return 1;
    }

    cam->shutdown();
    delete cam;

    return 0;
}


