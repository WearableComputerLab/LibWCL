/**
 * A Gesture Recognition system in C++
 * This library is based on the following paper:
 *
 * Wobbrock, J, Wilson, A, Li, Y 2007
 * Gestures without Libraries, Toolkits or Training:
 * A $1 Recognizer for User Interface Prototypes
 * UIST 2007
 *
 * @author Michael Marner (marnermr@cs.unisa.edu.au)
 */

#include <QtGui>
#include <vector>
#include <string>

#include <wcl/gestures/Gestures.h>
#include "GestureIO.h"

#ifndef CANVAS_H
#define CANVAS_H

using namespace std;
using namespace gestures;

class Canvas : public QWidget
{

	Q_OBJECT

	public:
		Canvas();

	public slots:
		void mousePressEvent(QMouseEvent *event);
		void mouseReleaseEvent(QMouseEvent *event);
		void mouseMoveEvent(QMouseEvent *event);
		void paintEvent(QPaintEvent *event);

		void startRecording();
		void setGestureName(QString name);

	signals:
		void gestureRecognised(QString name);

	private:
		bool recording;
		bool tracking;
		QString gestureName;
		PointList lineList;
		Gestures gEngine;
		GestureIO gIO;
		

};

#endif

