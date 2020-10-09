#include "QtWidgetsApplication1.h"
#include <QtWidgets/QApplication>
#include <QDebug>
#include <string>

 
int main(int argc, char *argv[])
{



	struct MyStruct {
		int a = 10;
	};
	
	QMap<QString, MyStruct*> g_depthMap;
	g_depthMap["hello"] = new MyStruct{11};
	qDebug() << g_depthMap << endl;
	
	MyStruct* old = g_depthMap["hello"];
	qDebug() << g_depthMap << endl;

	g_depthMap["hello"] = new MyStruct{ 33 };
	qDebug() << g_depthMap << endl;

	





    QApplication a(argc, argv);
    QtWidgetsApplication1 w;
    w.show();
    return a.exec();
}
