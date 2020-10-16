#pragma once

#include <QObject>

class OrderWorker : public QObject
{
	Q_OBJECT

public:
	OrderWorker();
	OrderWorker(QObject *parent);
	~OrderWorker();

signals:
	void signal_DealArbOrder(QString instrument_id); //����������϶���(�۸��и���ʱ)
public slots:
	void DealArbOrder(QString instrument_id); //����������϶���(�۸��и���ʱ)
};
