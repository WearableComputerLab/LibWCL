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
#define MARKER_SIZE 80 

using namespace std;
using namespace wcl;

Camera *cam;
unsigned char* data;
ARToolKitPlusTracker *tracker;

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
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// Create light components
	float ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	float diffuseLight[] = { 0.8f, 0.8f, 0.8, 1.0f };
	float specularLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	float position[] = { -1.5f, 1.0f, -4.0f, 1.0f };

	// Assign created components to GL_LIGHT0
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	data = new unsigned char[cam->getFormatWidth() * cam->getFormatHeight() * 3/*RGB*/];
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

	const unsigned char* frame = cam->getFrame(Camera::RGB8);
	memcpy(data,frame,cam->getFormatWidth() * cam->getFormatHeight() * 3 /*RGB*/);

	// Flip the image, glPixelZoom should do this but for some reason
	// I can't get it to work
	unsigned i=0;
	unsigned height = cam->getFormatHeight();
	unsigned width = cam->getFormatWidth();
	for(unsigned rows=height; rows > height/2; rows--){
	    for(unsigned cols=0; cols < width*3; cols++){
		int source = (rows*width*3) + cols;
		int dest = (i * width*3) + cols;
		unsigned char c = data[dest];
		data[dest]=data[source];
		data[source]=c;
	    }
	    i++;
	}

	glWindowPos4fMESA(1.0, 1.0, 1.0, 1.0);
	glDrawPixels( width, height, GL_RGB, GL_UNSIGNED_BYTE, data );

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixd(toGL(tracker->getProjectionMatrix()));
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Find objects in the scene and render a cube on it
	tracker->update();
	tracker->toString();
	vector<TrackedObject *> objects = tracker->getAllObjects();
	glDisable(GL_TEXTURE_2D);
	float mcolor[] = { 1.0, 1.0, 0.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mcolor);


	for(vector<TrackedObject *>::iterator it = objects.begin();
	    it != objects.end();
	    ++it ){

	    TrackedObject *object = *it;
	    SMatrix m(4);
	    cout << object->toString();

	    m = object->getTransform();
	    glLoadMatrixd(toGL(m));
	    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	    glEnable(GL_LIGHTING);
	    glColor3f(1.0,1.0, 0.0);
		glPushMatrix();
		glTranslatef(0,0,20);
	    glutSolidCube(40.0);
		glPopMatrix();
	    glDisable(GL_LIGHTING);
	    glColor3f(0.0,0.0,1.0);
	    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	    glBegin(GL_QUADS);
	    float size=MARKER_SIZE/2.0;
	    glVertex3f(-size,-size,0.0);
	    glVertex3f(-size,size,0.0);
	    glVertex3f(size,size,0.0);
	    glVertex3f(size,-size,0.0);
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

int main(int argc, char** argv)
{
    try {
    cam = CameraFactory::getCamera();
    if( cam == NULL ){
	printf("No Cameras are available\n");
	return 1;
    }

	float flx;
	float fly;
	float ppx;
	float ppy;

	float k1;
	float k2;
	float p1;
	float p2;

	if (argc > 1)
	{
		cout << "Using camera parameters from command line" << endl;

		if (argc == 9)
		{
			flx = atol(argv[1]);
			fly = atol(argv[2]);
			ppx = atol(argv[3]);
			ppy = atol(argv[4]);
			k1  = atol(argv[5]);
			k2  = atol(argv[6]);
			p1  = atol(argv[7]);
			p2  = atol(argv[8]);
		}
		else
		{
			cout << "Not enough parameters!" << endl;
			cout << "Usage: ./artoolkittest flx fly ppx ppy k1 k2 p1 p2" <<endl;
			return 1;
		}
	}
	else
	{
		cout << "using default parameters, be careful now ya' hear!" <<endl;
	}

    cam->printDetails();
	cam->setFormat(Camera::YUYV422, 640, 480);

	if (argc == 9)
	{
		wcl::Camera::CameraParameters p(flx, fly, ppx, ppy, k1, k2, p1, p2);
		cam->setParameters(p);
	}

    tracker = new ARToolKitPlusTracker(MARKER_SIZE,-1,cam->getFormatWidth(), cam->getFormatHeight());
    tracker->setCamera(cam);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(cam->getFormatWidth(), cam->getFormatHeight());
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
