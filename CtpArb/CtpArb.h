#pragma once
#pragma execution_character_set("utf-8")

#include <QtWidgets/QMainWindow>
#include <QStandardItemModel>
#include "ui_CtpArb.h"
#include <QDebug>
#include "CtpArbStruct.h"
#include <QList>

class CtpArb : public QMainWindow
{

    Q_OBJECT

public:
    CtpArb(QWidget *parent = Q_NULLPTR);
	void Init();
	~CtpArb();
	void SubscribeOrderInst(); //订阅交易合约行情。

	void SubscribePosInst();

//信号函数。
signals:
	void signal_UpdatePositionProfit(QString instrument_id, double last_price); //更新持仓盈亏(价格有更新时)
	void signal_UpdateArbPrice(QString instrument_id, double last_price); //更新套利组件价差(价格有更新时)


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
	void SubmitArbOrder(); //提交套利单。

	//价格有更新时，行情回报函数，调用的槽函数
	void UpdatePositionProfit(QString instrument_id, double last_price); //更新持仓盈亏(价格有更新时)
	void UpdateArbPrice(QString instrument_id, double last_price); //更新套利组件价差(价格有更新时)



protected:
	bool eventFilter(QObject *obj, QEvent *e); //事件过滤器，

private:
    Ui::CtpArbClass ui;

	ArbPortf FillArbPortf(); //将表单中的数据，填入套利组合对象，
	ArbOrder FillArbOrder(); //将表单中的数据，填入套利组合对象，
	void AddToModelOrderTable(ArbOrder ord); ////将订单数据填充到模型。
	int getNewOrdId(); //获取新的可用订单号。
};  
