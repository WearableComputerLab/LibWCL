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

#ifndef GESTUREIO_H
#define GESTUREIO_H

#include <QtGui>

#include <string>

#include <wcl/gestures/Gestures.h>

using namespace wcl;
using namespace std;

/**
 * Class for saving and loading gestures.
 */
class GestureIO : public QObject
{
	Q_OBJECT

	public:
		bool saveGesture(PointList p, string name);

};

#endif

