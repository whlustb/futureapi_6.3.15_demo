#include "CWorker.h"
#include <QThread>
#include <QDebug>


void CWorker::sayHello(char* param) {
	

	//��ʱ1000����
	QThread::msleep(1000);
	qDebug() << "sayHello " << param << QThread::currentThreadId();
	
}