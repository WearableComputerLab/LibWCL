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
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <wcl/camera/CameraException.h>
#include <wcl/camera/VirtualCamera.h>
#include <wcl/camera/CameraFactory.h>

#define WIDTH 640

using namespace std;
using namespace wcl;

Camera *cam;
unsigned char* data;

void usage()
{
    printf("Usage: camera [-l|-h|devicenode]\n"
	   "\n"
	   "eg: camera /dev/video0\n"
	   "\n"
	   "Options:\n"
	   "\n"
	   "-h = List this help\n"
	   "-l = List available cameras then exit\n"
	   "If no device node is given the first available camera is used. If no physical camera exists, the virtual camera is used\n"
	  );
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
	glViewport(0,0, (GLsizei)width, (GLsizei)height);
}

void keyboard(unsigned char key, int w, int h)
{
    if(key==27)
	exit(EXIT_SUCCESS);
}

void listCameras()
{
    cout << "Searching for Cameras..." << endl;
    std::vector<Camera *> cameras = CameraFactory::getCameras(CameraFactory::ALL);
    CameraFactory::printDetails(false);
}

int main(int argc, char** argv)
{
    if(argc >= 2){
       if( strcmp(argv[1],"--help") == 0 || strcmp(argv[1],"-h" ) == 0){
	usage();
	return 1;
       }
       if( strcmp(argv[1],"-l") == 0){
	   listCameras();
	   return 1;
       }
    }

    try {

	if( argc == 2 ){
	    cout << "Attempting to use Camera specified:" << argv[1] <<endl;
	    cam = CameraFactory::getCamera(argv[1], CameraFactory::ALL);
	} else {
	    cout << "No Camera Specified, using CameraFactory to find the first camera..." << endl;
	    cout << "Searching for Cameras..." << endl;
	    std::vector<Camera *> cameras = CameraFactory::getCameras(CameraFactory::ALL);
	    CameraFactory::printDetails(false);

	    if( cameras.size() == 0 ){
		cout << "No physical camera's found, using virtual camera" << endl;
		cam = new VirtualCamera();
	    } else {
		cam = CameraFactory::getCamera(CameraFactory::ALL);
	    }
	}
	if (cam == NULL ){
	    std::cout << "No usable cameras found" << std::endl;
	    exit(0);
	}

	/*
	 * Print out camera details
	 */
	cam->printDetails();

	cout << "Setting Camera to a width of " << WIDTH << " pixels" << endl;

	Camera::Configuration c;
	c.width = WIDTH;
	cam->setConfiguration(cam->findConfiguration(c));

	//Set power frequency compensation to 50 Hz
	//this is noncritical so don't stop the program if it fails
	try {
	cam->setControlValue(Camera::POWER_FREQUENCY, 1);
	}catch(CameraException &e){}

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
	glutIdleFunc(idle);

	// GO!
	glutMainLoop();

	cout << "Deleting Cam" << endl;

	cam->shutdown();
	delete cam;

    }
    catch (Exception &e)
    {
	cout << "Exception Occured: " << e.what() << endl;
    }

    return 0;
}

