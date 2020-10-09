#include "CWorker.h"

void CWorker::sayHello(const QString& param) {
	qDebug() << "hello," << param << QThread::currentThreadId();
}