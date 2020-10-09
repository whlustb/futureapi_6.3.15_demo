#include "CWorker.h"
#include <QThread>
#include <QDebug>


void CWorker::sayHello(char* param) {
	

	//—” ±1000∫¡√Î
	QThread::msleep(1000);
	qDebug() << "sayHello " << param << QThread::currentThreadId();
	
}