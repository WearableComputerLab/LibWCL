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

using namespace std;
using namespace wcl;

Camera *cam;
unsigned char* data;
ARToolKitPlusTracker tracker(640,480, 50);

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
	//glEnable(GL_DEPTH_TEST);
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

	//glEnable(GL_TEXTURE_2D);

//	glMatrixMode(GL_PROJECTION);
//	glLoadMatrixf(tracker.getProjectionMatrix());
	//gluOrtho2D(0,1,0,1);
	glMatrixMode(GL_MODELVIEW);
}

GLvoid reshape(int width, int height)
{
	glViewport(0,0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective( 45.0, width/(GLfloat)height, 1.0f, 5000.0f );
//	glLoadMatrixf(tracker.getProjectionMatrix());
	//glLoadMatrixd(toGL(tracker.getProjectionMatrix()));
	//gluOrtho2D(0,1,0,1);
	glMatrixMode(GL_MODELVIEW);
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
		Camera::convertImageYUV422toRGB8(frame, data, 640, 480);
		break;
	    case Camera::MONO8:
		Camera::convertImageMONO8toRGB8(frame,data,640,480);
		break;
	    case Camera::RGB8:
	    default:
		memcpy(data,frame,cam->getFormatWidth() * cam->getFormatHeight() * 3);
	}

	glTexSubImage2D(GL_TEXTURE_2D,
	                0,
					0,
					0,
					640,
					480,
					GL_RGB,
					GL_UNSIGNED_BYTE,
					data);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
//	gluPerspective( 45.0, width/(GLfloat)height, 1.0f, 5000.0f );
	glLoadMatrixf(tracker.getProjectionMatrix());
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(0.0,0.0,-500.0);
	//glRotatef(180.0, 0.0, 1.0, 0.0);

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

	// Find objects in the scene and render a cube on it
	tracker.update();
	tracker.toString();
	vector<TrackedObject *> objects = tracker.getAllObjects();
	glDisable(GL_TEXTURE_2D);

	glLoadMatrixf(tracker.getModelViewMatrix());
	glColor3f(0.0,1.0, 0.0);
	glutSolidCube(40.0);
	    /*
	for(vector<TrackedObject *>::iterator it = objects.begin();
	    it != objects.end();
	    ++it ){

	    TrackedObject *object = *it;
	    glPushMatrix();
	    SMatrix m(4);
	    
	    m = object->getTransform();
	    for(int c=0; c < 3; c++ ){
		for( int r = 0; r < 3; r++ )
		    m[c][r]=0;
	    }
	    m.print();

	    //glTranslatef(0.0, 0.0, 25.0);
	    glutSolidCube(1.0);
	    glPopMatrix();
	    break;
	}
	    */

	//glEnable(GL_TEXTURE_2D);
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
    /*
    CameraBuffer b;
    FILE *f ;
    f = fopen("image_320_240_8_marker_id_bch_nr0100.raw", "rb");
    unsigned char *buffer = (unsigned char *)malloc(320*240);
    fread(buffer,1, 320*240, f );
    fclose(f);
    b.start = buffer;
    b.length=320*240;

    VirtualCamera *c = new VirtualCamera();
    c->setFrames(&b,1);
    c->setFormat(Camera::MONO8, 320,240);
    */

    try {
    cam = CameraFactory::getCamera();
    cam->setFormat(Camera::MONO8, 640, 480);
    cam->printDetails();

    tracker.setCamera(cam);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(640, 480);
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
