#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QtWidgetsApplication1.h"

class QtWidgetsApplication1 : public QMainWindow
{
	Q_OBJECT

public:
    QtWidgetsApplication1(QWidget *parent = Q_NULLPTR);
	~QtWidgetsApplication1();

private:
    Ui::QtWidgetsApplication1Class ui;

signals:
	void sigSayHello(QString);

public slots:
	void SaveAndRelease();


};
