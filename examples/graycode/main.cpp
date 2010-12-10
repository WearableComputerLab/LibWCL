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
#define TIMER_VALUE 100
#define CROSS_DENSITY 30

using namespace std;
using namespace wcl;

GLvoid captureTimer(int value );

bool automated=false;
bool capturing=false;
unsigned image=0;
int window1, window2;
GrayCode g(IMAGE_WIDTH, IMAGE_HEIGHT,IMAGE_WIDTH,IMAGE_HEIGHT);

// The in use camera
Camera *camera;

// Buffers for frame detection
unsigned char **buffers;


void init()
{
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
    if(key=='r'){
	g.reset();
	image=0;
	capturing=false;
	cout << "Reset" <<endl;;
    }
    if(key=='v'){
	cout << "Decoding GrayCode Ignoring Camera" << endl;
	g.reset();
	g.decode((const unsigned char **)g.getCodedImages());
    }
    if(key==13){
	if( capturing == true ){
	    if(!g.next()){
		g.reset();
	    }
	}
	capturing=true;
    }
    if(key == 'a' ){
	automated=true;
	g.reset();
	image = 0;
	capturing=true;
	glutTimerFunc(TIMER_VALUE,captureTimer,0);
    }
}

void drawCross(const unsigned x, const unsigned y)
{
    glBegin(GL_LINES);
    glVertex3d(x>5?x-5:0,y,0);
    glVertex3d(x+5,y,0);
    glEnd();
    glBegin(GL_LINES);
    glVertex3d(x,y>5?y-5:0,0);
    glVertex3d(x,y+5,0);
    glEnd();
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

	if( capturing )
	    glDrawPixels( IMAGE_WIDTH, IMAGE_HEIGHT, GL_LUMINANCE, GL_UNSIGNED_BYTE, g.generate());
	else {
	    glDrawPixels( IMAGE_WIDTH, IMAGE_HEIGHT, GL_LUMINANCE, GL_UNSIGNED_BYTE, g.getDebugImage());

	    glOrtho(0, IMAGE_WIDTH, IMAGE_HEIGHT, 0, -1.0, 1.0);

	    for(unsigned y=0; y < IMAGE_HEIGHT; y+=CROSS_DENSITY){
		for(unsigned x=0; x < IMAGE_WIDTH; x+=CROSS_DENSITY){
		    Vector v(2);
		    v[0]=x;
		    v[1]=y;
		    v = g.getRowCol(v);
		    glColor3f(0.3,0.0,0.0);
		    drawCross(x,y);
		    glColor3f(0.0,0.0,1.0);
		    drawCross(v[0],v[1]);
		}
	    }
	}

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
    static bool first=true;
    if(capturing)
    {
	if( display ) {
	    if( !first ){
		if(g.next() == false ){
		    capturing=false;
		    display=true;
		    first=true;
		    automated=false;
		    cout << "Decoding GrayCode" << endl;
		    g.decode((const unsigned char **)buffers);
		    return;
		}
	    } else {
		first=false;
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
	if( automated ){
	    glutTimerFunc(TIMER_VALUE,captureTimer,0);
	}
    }

}

int main(int argc, char** argv)
{
    try {
	if(argc < 2 ){
	    Camera::Configuration config;
	    config.width=IMAGE_WIDTH;
	    config.height=IMAGE_HEIGHT;
	    camera = CameraFactory::findCamera(config);
	} else {
	    camera = CameraFactory::getCamera(argv[1]);
	    Camera::Configuration config = camera->getActiveConfiguration();
	    config.width=IMAGE_WIDTH;
	    config.height=IMAGE_HEIGHT;
	    camera->setConfiguration(config);
	}
    } catch(CameraException &e)
    {
	cout << "CameraException:" << e.what() << endl;
	exit(0);
    }

    if ( camera == NULL ){
	cout << "Camera Not Found" << endl;
	exit(0);
    }

    //Set power frequency compensation to 50 Hz
    //this is noncritical so don't stop the program if it fails
    try {
	camera->setControlValue(Camera::POWER_FREQUENCY, 1);
    }catch(CameraException &e){}

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

    glutMainLoop();
}
