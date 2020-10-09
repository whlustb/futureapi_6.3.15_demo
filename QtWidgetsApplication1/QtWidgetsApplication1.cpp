#include "QtWidgetsApplication1.h"
#include <QThread>
#include <QDebug>
#include <QDateTime>
#include <QString>

#include "CWorker.h"
#include<iostream>

#include <windows.h>
using namespace std;

QtWidgetsApplication1::QtWidgetsApplication1(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	

	//打印出主线程
	qDebug() << "main thread:" << QThread::currentThreadId();

	//创建thread
	QThread* m_pThread = new QThread;
	m_pThread->start();

	//创建CWorker
	CWorker* m_pWorker = new CWorker;
	m_pWorker->moveToThread(m_pThread);

	QObject::connect(this, SIGNAL(sigSayHello(char[31])), m_pWorker, SLOT(sayHello(char[31])));
	QObject::connect(m_pThread, &QThread::finished, m_pWorker, &QObject::deleteLater);


	QDateTime dt = QDateTime::currentDateTime();
	//qDebug() << dt << m;


	SYSTEMTIME systime1;
	GetLocalTime(&systime1);//本地时间

	//发射信号
	char *a = "111";

	emit sigSayHello(a);

	char *b = "222";



	emit sigSayHello(b);

	SYSTEMTIME systime2;
	GetLocalTime(&systime2);//本地时间

	QDateTime dt2 = QDateTime::currentDateTime();


	qDebug() << dt << endl << dt2 << endl;

	cout << systime1.wSecond << ":" << systime1.wMilliseconds << endl;
	cout << systime2.wSecond << ":" << systime2.wMilliseconds << endl;

	qDebug() << "hello 222222";



	SYSTEMTIME systime;
	GetLocalTime(&systime);//本地时间

}
