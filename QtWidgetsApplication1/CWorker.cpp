#include "CWorker.h"
#include <QThread>
#include <QDebug>
#include <QDateTime>


void CWorker::sayHello(QString param) {
	
	qint64 t = QDateTime::currentMSecsSinceEpoch();
	qDebug() << "inner" << t << endl;

	//��ʱ1000����
	QThread::msleep(1000);
	//qDebug() << "sayHello " << param << QThread::currentThreadId();
	
}