#include <QtCore/QCoreApplication>
#include <QThread>
#include <QDebug>
#include "CWorker.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


	//打印出主线程
	qDebug() << "main thread:" << QThread::currentThreadId();

	//创建thread
	QThread* m_pThread = new QThread;
	m_pThread->start();

	//创建CWorker
	CWorker* m_pWorker = new CWorker;
	m_pWorker->moveToThread(m_pThread);
	QObject::connect(this, SIGNAL(sigSayHello(QString)), m_pWorker, SLOT(sayHello(QString)));

	//发射信号
	emit sigSayHello("fearlazy!");

    return a.exec();
}
