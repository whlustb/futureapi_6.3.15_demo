#pragma once

#include <QtWidgets/QMainWindow>
#include <QStandardItemModel>
#include "ui_MainForm.h"
#include <QDebug>
#include "CtpArbStruct.h"
#include <QList>

class MainWindow : public QMainWindow
{

    Q_OBJECT

public:
    MainWindow(QWidget *parent = Q_NULLPTR);
	void Init();
	~MainWindow();
	void SubscribeOrderInst(); //订阅交易合约行情。
	void SubscribePosInst(); //订阅持仓合约行情。
	void closeEvent(QCloseEvent *event); //重写关闭事件。


//信号函数。
signals:
	void signal_UpdatePositionProfit(QString instrument_id, double last_price); //更新持仓盈亏(价格有更新时)
	void signal_UpdateArbPrice(QString instrument_id, double last_price); //更新套利组件价差(价格有更新时)
	void signal_RefreshPosTable();//填充持仓到模型变量，
	void signal_RefreshOrdTable(QString orderRef, QString type);//提交到线上的订单。

//槽函数定义。	
public slots:
	void ShowLoginForm(); //显示登录窗口。
	void ShowInstrument(); //显示合约树。
	void ChooseInstrument(QModelIndex idx); //选择指定合约	
	void AddArbPortf(); //添加套利组合。
	void ClickArbPortf(QModelIndex); //点击套利组合。
	void ShowContextMenu(QPoint pos); //套利组件列表上显示的右键点击事件。
	void DeleteArbPortf(); //删除套利组合。
	void DeleteArbOrder(); //删除套利订单。
	void RefreshPosTable();
	void RefreshOrdTable(QString orderRef, QString type);
	//填充持仓到模型变量，
	void AddToArbPortfTable(ArbPortf arb); //添加套利组合到模型中。
	void SubmitArbOrder(); //提交套利单。
	void UpdatePositionProfit(QString instrument_id, double last_price); //更新持仓盈亏(价格有更新时)
	void UpdateArbPrice(QString instrument_id, double last_price); //更新套利组件价差(价格有更新时)
	void UpdateInstrument();//更新合约。
	void AddToArbOrderTable(ArbOrder ord); ////将订单数据填充到模型。
	ArbPortf FillArbPortf(); //将表单中的数据，填入套利组合对象，
	ArbOrder FillArbOrder(); //将表单中的数据，填入套利组合对象，

protected:
	bool eventFilter(QObject *obj, QEvent *e); //事件过滤器，

private:
    Ui::CtpArbClass ui;

	
};  
