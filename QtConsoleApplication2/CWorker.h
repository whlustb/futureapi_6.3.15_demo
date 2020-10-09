#pragma once
#include <QObject>
#include <QString>

class CWorker :
	public QObject {
	void sayHello(const QString & param);
};

