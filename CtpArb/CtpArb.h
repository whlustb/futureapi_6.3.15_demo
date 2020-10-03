#pragma once
#pragma execution_character_set("utf-8")

#include <QtWidgets/QMainWindow>
#include <QStandardItemModel>
#include "ui_CtpArb.h"
#include <QDebug>

extern QStandardItemModel *modelInst; //合约树形结构数据。

class CtpArb : public QMainWindow
{
    Q_OBJECT

public:
    CtpArb(QWidget *parent = Q_NULLPTR);

//槽函数定义。	
public slots:
	void ShowLoginForm();
	void ShowInstrument();
	void ChooseInstrument(QModelIndex idx);

private:
    Ui::CtpArbClass ui;
};  
