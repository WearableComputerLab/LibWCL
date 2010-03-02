/*-
 * Copyright (c) 2010 Benjamin Close <Benjamin.Close@clearchain.com>
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
 * Simple test of the ARToolKitPlus Wrappers
 */
#include <iostream>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <wcl/camera/VirtualCamera.h>
#include <wcl/camera/CameraFactory.h>
#include <wcl/tracking/ARToolKitPlusTracker.h>
#define IMAGE_WIDTH 640
#define IMAGE_HEIGHT 480

using namespace std;
using namespace wcl;

Camera *cam;
unsigned char* data;
ARToolKitPlusTracker tracker(100,-1,IMAGE_WIDTH,IMAGE_HEIGHT);

/*
 * OpenGL implementation of glWindowPos*MESA()
 * (Taken from http://cgit.freedesktop.org/mesa/mesa/src/mesa/main/rastpos.c)
 */
void glWindowPos4fMESA( GLfloat x, GLfloat y, GLfloat z, GLfloat w )
{
   GLfloat fx, fy;

   /* Push current matrix mode and viewport attributes */
   glPushAttrib( GL_TRANSFORM_BIT | GL_VIEWPORT_BIT );

   /* Setup projection parameters */
   glMatrixMode( GL_PROJECTION );
   glPushMatrix();
   glLoadIdentity();
   glMatrixMode( GL_MODELVIEW );
   glPushMatrix();
   glLoadIdentity();

   glDepthRange( z, z );
   glViewport( (int) x - 1, (int) y - 1, 2, 2 );

   /* set the raster (window) position */
   fx = x - (int) x;
   fy = y - (int) y;
   glRasterPos4f( fx, fy, 0.0, w );

   /* restore matrices, viewport and matrix mode */
   glPopMatrix();
   glMatrixMode( GL_PROJECTION );
   glPopMatrix();

   glPopAttrib();
}


double * toGL(const SMatrix &m)
{
        return (double *)(transpose(m)[0]);
}

GLvoid idle()
{
	glutPostRedisplay();
}

GLvoid init()
{
	glShadeModel(GL_SMOOTH);
	glClearColor(0,0,0,0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	data = new unsigned char[IMAGE_WIDTH*IMAGE_HEIGHT*3];
}

GLvoid reshape(int width, int height)
{
	glViewport(0,0, (GLsizei)width, (GLsizei)height);
}

void keyboard(unsigned char key, int w, int h)
{
    if(key==27)
	exit(EXIT_SUCCESS);
}

GLvoid display()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	const unsigned char* frame = cam->getFrame();
	switch(cam->getImageFormat()){
	    case Camera::YUYV422:
		Camera::convertImageYUV422toRGB8(frame, data, IMAGE_WIDTH, IMAGE_HEIGHT);
		break;
	    case Camera::MONO8:
		Camera::convertImageMONO8toRGB8(frame,data, IMAGE_WIDTH, IMAGE_HEIGHT);
		break;
	    case Camera::RGB8:
	    default:
		memcpy(data,frame,cam->getFormatWidth() * cam->getFormatHeight() * 3);
	}


	// Flip the image, glPixelZoom should do this but for some reason
	// I can't get it to work
	unsigned i=0;
	for(unsigned rows=IMAGE_HEIGHT; rows > IMAGE_HEIGHT/2; rows--){
	    for(unsigned cols=0; cols < IMAGE_WIDTH*3; cols++){
		int source = (rows*IMAGE_WIDTH*3) + cols;
		int dest = (i * IMAGE_WIDTH*3) + cols;
		unsigned char c = data[dest];
		data[dest]=data[source];
		data[source]=c;
	    }
	    i++;
	}

	glWindowPos4fMESA(1.0, 1.0, 1.0, 1.0);
	glDrawPixels( IMAGE_WIDTH, IMAGE_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, data );

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixd(toGL(tracker.getProjectionMatrix()));
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Find objects in the scene and render a cube on it
	tracker.update();
	tracker.toString();
	vector<TrackedObject *> objects = tracker.getAllObjects();
	glDisable(GL_TEXTURE_2D);

	for(vector<TrackedObject *>::iterator it = objects.begin();
	    it != objects.end();
	    ++it ){

	    TrackedObject *object = *it;
	    SMatrix m(4);
	    cout << object->toString();

	    m = object->getTransform();
	    glLoadMatrixd(toGL(m));
	    glColor3f(1.0,1.0, 0.0);
	    glutSolidCube(40.0);
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

int main(int argc, char** argv)
{
    try {
    cam = CameraFactory::getCamera();
    if( cam == NULL ){
	printf("No Cameras are available\n");
	return 1;
    }

    cam->setFormat(Camera::MONO8, IMAGE_WIDTH, IMAGE_HEIGHT);
    cam->printDetails();

    tracker.setCamera(cam);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(IMAGE_WIDTH, IMAGE_HEIGHT);
    glutInitWindowPosition(100,100);
    glutCreateWindow(argv[0]);
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(idle);
    glutMainLoop();

    } catch (std::string s )
    {
	printf("Exception: %s\n", s.c_str());
    }
}
