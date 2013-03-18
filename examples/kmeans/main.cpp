/*
 * Copyright (c) 2013 Michael Marner <michael@20papercups.net>
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

// include openGL headers
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#include <iostream>
#include <vector>

#include <wcl/geometry/KMeans.h>

using namespace wcl;
using namespace std;

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480 


// Globals FTW!
std::vector<wcl::Vector> points;
KMeans *kmeans;

struct Color {
	unsigned char r;
	unsigned char g;
	unsigned char b;
};

std::map<Cluster*, Color> colours;
		

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
}


GLvoid display()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0,WINDOW_WIDTH, 0, WINDOW_HEIGHT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPointSize(2);
	glColor3ub(100,100,100);
	glBegin(GL_POINTS);
	for (vector<wcl::Vector>::iterator it = points.begin(); it < points.end(); ++it) {
		Cluster* pc = kmeans->getCluster(*it);
		if (pc == NULL) {
			glColor3ub(100,100,100);
		}
		else {
			glColor3ub(colours[pc].r, colours[pc].g, colours[pc].b);
		}

		glVertex3d((*it)[0], (*it)[1], 0);
	}
	glEnd();

	glPointSize(6);
	glBegin(GL_POINTS);
	unsigned i=0;
	for (vector<Cluster*>::const_iterator it = kmeans->getClusters().begin(); it < kmeans->getClusters().end(); ++it) {
		glColor3ub(colours[*it].r, colours[*it].g, colours[*it].b);
		glVertex3d((*it)->mean[0], (*it)->mean[1], (*it)->mean[2]);
		i++;
	}
	glEnd();

	glFlush();
	glutSwapBuffers();
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		cout << gluErrorString(error) << endl;
	}

	sleep(1);
	unsigned swaps = kmeans->step();
	clog << swaps << " points swapped." << endl;

}

GLvoid reshape(int width, int height)
{
	glViewport(0,0, (GLsizei)width, (GLsizei)height);
}

int main(int argc, char** argv) {
	if (argc < 2) {
		clog << "Usage: kmeans [clusters]" << endl;
		return 1;
	}
	srand(time(0));
	clog << "Generating a bunch of random points" << endl;

	for (unsigned i = 0; i < 200; ++i) {
		int x = rand() % WINDOW_WIDTH;
		int y = rand() % WINDOW_HEIGHT;
		//clog << x << " " << y << endl;
		points.push_back(wcl::Vector(x,y,0));
	}

	kmeans = new KMeans(points, atoi(argv[1]));

	for (vector<Cluster*>::const_iterator it = kmeans->getClusters().begin(); it < kmeans->getClusters().end(); ++it) {
		Color c;
		c.r = rand() % 255;
		c.g = rand() % 255;
		c.b = rand() % 255;
		colours[*it] = c;
	}

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100,100);
    glutCreateWindow(argv[0]);
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    glutMainLoop();
	return 0;
}

