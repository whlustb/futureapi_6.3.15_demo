#pragma once
#pragma execution_character_set("utf-8")

#include <QtWidgets/QMainWindow>
#include <QStandardItemModel>
#include "ui_CtpArb.h"
#include <QDebug>
#include "CtpArbStruct.h"
#include <QList>

extern QStandardItemModel *modelInstTree; //合约树形结构数据。
extern QStandardItemModel *modelArbTable; //套利列表表格模型数据，
extern QList<ArbItem> arbList; //套利策略列表。

class CtpArb : public QMainWindow
{
    Q_OBJECT

public:
    CtpArb(QWidget *parent = Q_NULLPTR);
	~CtpArb();

//槽函数定义。	
public slots:
	void ShowLoginForm(); //显示登录窗口。
	void ShowInstrument(); //显示合约树。
	void ChooseInstrument(QModelIndex idx); //选择指定合约
	void AddArbItem(); //添加套利策略。
	void ShowArbItem(QModelIndex); //点击已经添加的套利组件。

protected:
	bool eventFilter(QObject *obj, QEvent *e); //事件过滤器，

private:
    Ui::CtpArbClass ui;
};  
