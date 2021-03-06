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

#include "Canvas.h"
#include "GestureIO.h"

#include <QPainter>

using namespace wcl;


Canvas::Canvas()
{
	recording = false;
	tracking = false;
}

void Canvas::mousePressEvent(QMouseEvent *event)
{
	qDebug("mousePressed");
	tracking = true;
}

void Canvas::mouseReleaseEvent(QMouseEvent *event)
{
	qDebug("mouseReleased");
	tracking = false;
	
	//prepare the pointlist that we just recorded
	PointList points = gEngine.prepare(lineList);
	qDebug("have prepared the points");
	if (recording)
	{
		//save gesture
		if (gestureName != "")
		{
			gIO.saveGesture(points, gestureName.toAscii().constData());
			gEngine.addTemplate(points, gestureName.toAscii().constData());
		}
		recording = false;
		emit gestureRecognised("");
	}
	else
	{
		//recognise gesture
		std::string gesture = gEngine.recognise(points);
		emit gestureRecognised(QString(gesture.c_str()));
		qDebug("%s",gesture.c_str());
	}
	//clear our line list and repaint
	lineList.clear();
	this->repaint();
}

void Canvas::mouseMoveEvent(QMouseEvent *event)
{
	//if we are currently tracking, push the current mouse location onto
	//our linelist
	if (tracking)
	{
		lineList.push_back(Point(event->x(), event->y()));
		this->repaint();
	}
}

void Canvas::paintEvent(QPaintEvent *event)
{
	/*
	 * Draw each line that we currently have
	 */
	if (lineList.size() > 0)
	{
		QPainter p(this);
		PointList::iterator it = lineList.begin();
		Point first = *it;
		it++;
		for (;it<lineList.end();it++)
		{
			Point p2 = *it;
			p.drawLine(first[0], first[1], p2[0], p2[1]);
			first = p2;
		}
	}
}

void Canvas::startRecording()
{
	this->recording = true;
	//This is a hack to get the status bar to display when we are
	//recording. Really this should be a different signal.
	emit gestureRecognised("RECORDING");
}

void Canvas::setGestureName(QString name)
{
	this->gestureName = name;
}


