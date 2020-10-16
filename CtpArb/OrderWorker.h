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
	void signal_DealArbOrder(QString instrument_id); //处理套利组合订单(价格有更新时)
public slots:
	void DealArbOrder(QString instrument_id); //处理套利组合订单(价格有更新时)
};
