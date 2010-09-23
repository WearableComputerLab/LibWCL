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

using namespace wcl;
using namespace std;

/**
 * A canvas that allows recording and recognising widgets
 */
class Canvas : public QWidget
{

	Q_OBJECT

	public:
		Canvas();

	public slots:
		/*
		 * Mouse Events.
		 */
		void mousePressEvent(QMouseEvent *event);
		void mouseReleaseEvent(QMouseEvent *event);
		void mouseMoveEvent(QMouseEvent *event);

		/**
		 * We are overriding the paint function so we can do our drawing.
		 */
		void paintEvent(QPaintEvent *event);

		/**
		 * Start recording a gesture
		 */
		void startRecording();

		/**
		 * Update the name of the gesture we are about to record
		 */
		void setGestureName(QString name);

	signals:
		/**
		 * Emitted when a gesture is recognised.
		 * Emits the name of the gesture.
		 */
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

