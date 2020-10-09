#pragma once
#include <QObject>
#include <QString>

class CWorker : public QObject {
	Q_OBJECT

	public slots:
	void sayHello(char param[31]);
};

