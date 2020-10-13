#include "QtWidgetsApplication1.h"
#include <QtWidgets/QApplication>
#include <QDebug>
#include <string>
#include <QTextCodec>
#include <QString>
#include <QDebug>
#include <iostream>
#include <conio.h>
 
int main(int argc, char *argv[])
{


    QApplication a(argc, argv);
    QtWidgetsApplication1* w1 = new QtWidgetsApplication1();
	QtWidgetsApplication1* w2 = new QtWidgetsApplication1();

	QMainWindow* ImgWindow1 = new QMainWindow();
	QObject::connect(ImgWindow1, SIGNAL(destroyed()), w2, SLOT(SaveAndRelease()));

	ImgWindow1->setAttribute(Qt::WA_DeleteOnClose, true);
	ImgWindow1->show();

	w1->setAttribute(Qt::WA_DeleteOnClose, false);
	w1->setAttribute(Qt::WA_DeleteOnClose, false);

	QObject::connect(w1, SIGNAL(destroyed()), w2, SLOT(SaveAndRelease()));

	//QObject::connect(w2, SIGNAL(destroyed()), w2, SLOT(SaveAndRelease()));

	
	//a.connect(&a, SIGNAL(lastWindowClosed()), w2, SLOT(SaveAndRelease()));

	//emit w.destroyed();

	//w1->show();
	//w2->show();

    return a.exec();

	//delete w1;
	//delete w2;
}
