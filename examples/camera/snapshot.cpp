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

#define MAX_RES 1600

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

Camera *cam;

void usage()
{
    printf("snapshot devicenode [filename]\n"
	   "Snapshot takes a image from a camera and displays it to the screen\n"
	   "or saves it to a file if a filename is given. It uses image magic to\n"
	   "convert the image from a rgb image to a jpg"
	   );
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

	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0,1,0,1);
	glMatrixMode(GL_MODELVIEW);
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
	glViewport(0,0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int w, int h)
{
    if(key==27)
	exit(EXIT_SUCCESS);
}

int main(int argc, char** argv)
{
    bool dodisplay = false;
    const char *filename = NULL;

    switch( argc ){
	case 2: dodisplay = true; break;
	case 3: filename=argv[2]; break;
	case 1: /* fallthrough */
	default:
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

	cam = CameraFactory::getCamera(argv[1]);
	Camera::Configuration c;
	c.width = MAX_RES;
	c = cam->findConfiguration(c);
	cam->setConfiguration(c);

	if( dodisplay ){
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

	} else {

	    cout << "Writing Camera Image to: " << filename << endl;

	    stringstream name;
	    name << filename;
	    name << ".rgb";

	    FILE *f = fopen(name.str().c_str(), "w");
	    if ( f == NULL ){
		perror("Failed to open output file");
		return 1;
	    }

	    const unsigned char * frame = cam->getFrame(Camera::RGB8);

	    if(fwrite(frame, cam->getFormatBufferSize(Camera::RGB8), 1, f) <= 0){
		perror("Failed to write output file");
		return 1;
	    }


	    fclose(f);

	    stringstream s;
	    s << "convert ";
	    s << "-size ";
	    s << c.width;
	    s << "x";
	    s << c.height;
	    s << " -depth 8 ";
	    s << name.str();
	    s << " ";
	    s << filename;
	    s << ".jpg";

	    cout << s.str() << endl;

	    if( system(s.str().c_str()) < 0 ){
		cerr << "Failed to convert to jpg: " << s.str().c_str() << endl;
		return 1;
	    }

	    unlink( name.str().c_str() );
	}

	cam->shutdown();
	delete cam;
    }
    catch (Exception e)
    {
	cout << "Exception Occured: " << e.what() << endl;
	return 1;
    }

    return 0;
}

