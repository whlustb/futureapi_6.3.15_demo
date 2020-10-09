#include <QtCore/QCoreApplication>
#include <QThread>
#include <QDebug>
#include "CWorker.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


	//��ӡ�����߳�
	qDebug() << "main thread:" << QThread::currentThreadId();

	//����thread
	QThread* m_pThread = new QThread;
	m_pThread->start();

	//����CWorker
	CWorker* m_pWorker = new CWorker;
	m_pWorker->moveToThread(m_pThread);
	QObject::connect(this, SIGNAL(sigSayHello(QString)), m_pWorker, SLOT(sayHello(QString)));

	//�����ź�
	emit sigSayHello("fearlazy!");

    return a.exec();
}
