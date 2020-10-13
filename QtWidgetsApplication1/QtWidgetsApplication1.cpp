#include "QtWidgetsApplication1.h"
#include <QThread>
#include <QDebug>
#include <QDateTime>
#include <QString>
#include "CWorker.h"
#include<iostream>
using namespace std;

QtWidgetsApplication1::QtWidgetsApplication1(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	

	
}



//退出前工作。
void QtWidgetsApplication1::SaveAndRelease() {
	qDebug() << "vv";
	
}

QtWidgetsApplication1::~QtWidgetsApplication1(){
	qDebug() << "vv";

}
