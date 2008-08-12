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

#include "Canvas.h"


int main(int argc, char** argv)
{
	QApplication app(argc, argv);

	//Create window
	QWidget window;
	window.resize(600,600);

	//Top container will contain the text field and recod button.
	QWidget *topContainer = new QWidget;
	QHBoxLayout *topLayout = new QHBoxLayout;
	QPushButton *record = new QPushButton("Record Gesture");
	QLineEdit *name = new QLineEdit();
	QLabel *label = new QLabel();

	topLayout->addWidget(name);
	topLayout->addWidget(record);
	topContainer->setLayout(topLayout);

	//Main layout contains the top container, canvas and status bar.
	QVBoxLayout *mainLayout = new QVBoxLayout;

	Canvas *canvas = new Canvas;
	//Make Canvas fill up any spare space.
	canvas->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

	//Add widgets to the main layout
	mainLayout->addWidget(topContainer);
	mainLayout->addWidget(canvas);
	mainLayout->addWidget(label);

	window.setLayout(mainLayout);

	//connections
	app.connect(record, SIGNAL(clicked(bool)), canvas, SLOT(startRecording()));
	app.connect(name, SIGNAL(textChanged(QString)), canvas, SLOT(setGestureName(QString)));
	app.connect(canvas, SIGNAL(gestureRecognised(QString)), label, SLOT(setText(QString)));

	//fire everything up.
	window.show();
	return app.exec();
}

