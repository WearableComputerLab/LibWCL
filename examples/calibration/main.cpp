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
#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdlib.h>
#include "GrayCode.h"
#include <wcl/camera/CameraException.h>
#include <wcl/camera/CameraFactory.h>

#define IMAGE_WIDTH 640
#define IMAGE_HEIGHT 480

using namespace std;
using namespace wcl;

Camera *camera;
GrayCode g(IMAGE_WIDTH, IMAGE_HEIGHT);

GLvoid init()
{
	glClearColor(0,0,0,0);
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
	g.next();
	glutPostRedisplay();
    }
}

GLvoid displayProjector()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

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
	glutPostRedisplay();
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

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(IMAGE_WIDTH, IMAGE_HEIGHT);
    glutInitWindowPosition(100,100);
    int window1 = glutCreateWindow(argv[0]);
    init();
    glutDisplayFunc(displayProjector);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    glutInitWindowPosition(100+IMAGE_WIDTH,100);
    int window2 = glutCreateWindow(argv[0]);
    init();
    glutDisplayFunc(displayCamera);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);

    glutMainLoop();
}
