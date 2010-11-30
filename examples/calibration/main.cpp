/*-
 * Copyright (c) 2010 Benjamin Close <Benjamin.Close@clearchain.com>
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
 * Simple test of the Binary Coded GrayCode Phase Shift code
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "GrayCode.h"
#include <wcl/camera/CameraException.h>
#include <wcl/camera/CameraFactory.h>

#define IMAGE_WIDTH 640
#define IMAGE_HEIGHT 480
#define TIMER_VALUE 200

using namespace std;
using namespace wcl;

bool capturing=false;
unsigned image=0;
int window1, window2;
Camera *camera;
GrayCode g(IMAGE_WIDTH, IMAGE_HEIGHT);
unsigned char **buffers;

GLvoid init()
{
	glClearColor(0,0,0,0);
	buffers = new unsigned char *[g.getRequiredImageCount()];
	for( unsigned i = 0; i < g.getRequiredImageCount(); i++ )
	    buffers[i] = new unsigned char [IMAGE_WIDTH * IMAGE_HEIGHT];
}

GLvoid reshape(int width, int height)
{
	glViewport(0,0, (GLsizei)width, (GLsizei)height);
}

void keyboard(unsigned char key, int w, int h)
{
    if(key==27)
	exit(EXIT_SUCCESS);
    if(key==13){
	g.reset();
	image = 0;
	capturing=true;
    }
}


GLvoid displayProjector()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPixelZoom(1.0f, -1.0f);
	glRasterPos2i(-1, 1);

	glDrawPixels( IMAGE_WIDTH, IMAGE_HEIGHT, GL_LUMINANCE, GL_UNSIGNED_BYTE, g.generate());

	glFlush();
	glutSwapBuffers();
}

GLvoid displayCamera()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPixelZoom(1.0f, -1.0f);
	glRasterPos2i(-1, 1);
	glDrawPixels( IMAGE_WIDTH, IMAGE_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, camera->getFrame(Camera::RGB8));

	glFlush();
	glutSwapBuffers();
}

/**
 * Constantly fetch a new image from the camera to display
 */
GLvoid idle()
{
    glutSetWindow(window1);
    glutPostRedisplay();
    glutSetWindow(window2);
    glutPostRedisplay();
}

GLvoid captureTimer(int value )
{
    static bool display = true;
    if(capturing)
    {
	if( display ) {
	    if(g.next() == false ){
		capturing=false;
		display=true;
		cout << "Decoding GrayCode" << endl;
		g.decode((const unsigned char **)buffers);
		Vector camera(2);
		camera[0] = 100;
		camera[1] = 100;
		Vector v=g.getRowCol(Vector(camera));
		v.print();
	    }
	    glutSetWindow(window1);
	    glutPostRedisplay();
	    glutSetWindow(window2);
	    glutPostRedisplay();
	    display=false;
	} else {
	    cout << "Capturing Image" << image << endl;
	    memcpy(buffers[image],camera->getFrame(Camera::MONO8),IMAGE_WIDTH * IMAGE_HEIGHT);
	    image++;
	    display=true;
	}
    }

    glutTimerFunc(TIMER_VALUE,captureTimer,0);
}


int main(int argc, char** argv)
{
    try {
	Camera::Configuration config;
	config.width=IMAGE_WIDTH;
	config.height=IMAGE_HEIGHT;
	camera = CameraFactory::findCamera(config);
    } catch(CameraException &e)
    {
	cout << "CameraException:" << e.what() << endl;
	exit(0);
    }

    if ( camera == NULL ){
	cout << "Camera Not Found" << endl;
	exit(0);
    }

    init();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(IMAGE_WIDTH, IMAGE_HEIGHT);
    glutInitWindowPosition(100,100);
    window1 = glutCreateWindow(argv[0]);
    glutDisplayFunc(displayProjector);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    glutInitWindowPosition(100+IMAGE_WIDTH,100);
    window2 = glutCreateWindow(argv[0]);
    glutDisplayFunc(displayCamera);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    glutTimerFunc(TIMER_VALUE,captureTimer,0);

    glutMainLoop();
}
