#include "MainForm.h"
#include <iostream>
#include <QTreeView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QTextCodec>
#include <QDoubleValidator>
#include <QRegExpValidator>
#include <QMessageBox>
#include <QRegExp>
#include <QMenu>
#include "LoginForm.h"
#include "MyTreeView.h"
#include <QStringList>
#include "ThostApiStruct_Serielize.h"
#include "CtpArbFunc.h"
#include <QJSEngine>
#include <QThread>
#include "define.h"
#include "HandlerVars.h"

#include<stdio.h>
#include<stdlib.h>

using namespace std;

extern LoginForm* w_login;

QTreeView *treeInst; //合约树。
QLineEdit* setInsEdit; //标记当前要设置的是哪个合约。
QMenu* popMenu; //套利组件上的右键菜单。

// JS引擎、计算JS数学表达式。
QJSEngine* jsEngine;

//析构函数。
MainWindow::~MainWindow() {}

//窗体关闭函数。
void MainWindow::closeEvent(QCloseEvent * event) {
	/*
	QMessageBox::StandardButton result = QMessageBox::question(this, QStr("确认"), QStr("确定要退出本系统吗？"),
		QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
		QMessageBox::No);

	if (result == QMessageBox::Yes)
		event->accept();
	else
		event->ignore();
	*/

	//保存历史套利订单列表。(反序列化)
	QFile file("data.dat");
	file.open(QIODevice::WriteOnly);
	QDataStream stream(&file);
	stream << g_instMap << g_arbPortfList << g_arbOrderMap << g_ArbOrderId << g_orderMap << g_OrderRef;
	file.close();

}

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent) {

	//JS引擎初始化。
	jsEngine = new QJSEngine();

	ui.setupUi(this);
	//添加事件过滤器。
	ui.centralWidget->installEventFilter(this);

}

//初始化函数。要单独调用，不能放到构造函数，因为里边的一些代码用到了对象本身指针，而构造函数调用时，对象本身还是个空的。
void MainWindow::Init() {

	/********集中绑定槽函数（界面操作响应类）********/
	//登录按扭。
	QObject::connect(ui.actionLogin, SIGNAL(triggered()), this, SLOT(ShowLoginForm()));
	//点击合约1按扭
	QObject::connect(ui.pushButton_ins1, SIGNAL(clicked()), this, SLOT(ShowInstrument()));
	//添加合约2按扭
	QObject::connect(ui.pushButton_ins2, SIGNAL(clicked()), this, SLOT(ShowInstrument()));
	//添加套利组合。
	QObject::connect(ui.pushButton_add_arb, SIGNAL(clicked()), this, SLOT(AddArbPortf()));
	//单击套利组合
	QObject::connect(ui.tableView_arblist, SIGNAL(clicked(QModelIndex)), this, SLOT(ClickArbPortf(QModelIndex)));
	//提交套利单 ，
	QObject::connect(ui.pushButton_submit_order, SIGNAL(clicked()), this, SLOT(SubmitArbOrder()));
	//更新合约
	QObject::connect(ui.actionUpdateInstrument, SIGNAL(triggered()), this, SLOT(UpdateInstrument()));

	/*********绑定自定义信号槽函数（回调函数中触发）********/
	//持仓浮盈更新
	QObject::connect(this, SIGNAL(signal_UpdatePositionProfit(QString, double)), this, SLOT(UpdatePositionProfit(QString, double)));
	//持仓列表更新
	QObject::connect(this, SIGNAL(signal_RefreshPosTable()), this, SLOT(RefreshPosTable()));
	//未成交订单列表更新
	QObject::connect(this, SIGNAL(signal_RefreshOrdTable(QString, QString)), this, SLOT(RefreshOrdTable(QString, QString)));
	//套利组合价差更新
	QObject::connect(this, SIGNAL(signal_UpdateArbPrice(QString, double)), this, SLOT(UpdateArbPrice(QString, double)));

	//反序列化，恢复数据，
	QFile file("data.dat");
	if (file.open(QIODevice::ReadOnly)) {
		QDataStream stream(&file);
		stream >> g_instMap >> g_arbPortfList >> g_arbOrderMap >> g_ArbOrderId >> g_orderMap >> g_OrderRef;
	}


	//合约列表，填充模型。
	FillModelInst(modelIns, g_instMap);

	/*******套利组合列表*******/
	//绑定模型。
	ui.tableView_arblist->setModel(modelArbPortf);
	//设置表头
	QStringList arbHeader;
	arbHeader << QStr("名称") << QStr("合约") << QStr("买") << QStr("卖");
	modelArbPortf->setHorizontalHeaderLabels(arbHeader);
	modelArbPortf->setColumnCount(arbHeader.size()); //设置列数
	ui.tableView_arblist->verticalHeader()->hide();//隐藏行号。
	ui.tableView_arblist->horizontalHeader()->setStyleSheet("border:1px solid #aaaaaa"); //设置表头颜色。
	//填充套利组合列表模型，
	for (int i = 0; i < g_arbPortfList.count(); i++) {
		ArbPortf item = g_arbPortfList[i];
		AddToArbPortfTable(item);
	}

	/************持仓列表***********************/
	//绑定模型。
	ui.tableView_poslist->setModel(modelPosition);
	//设置表头
	QStringList posHeader;
	posHeader << QStr("合约") << QStr("方向") << QStr("持仓") << QStr("持仓均价") << QStr("最新价") << QStr("浮动盈亏") << QStr("今持仓") << QStr("昨持仓");
	modelPosition->setHorizontalHeaderLabels(posHeader);
	modelPosition->setColumnCount(posHeader.size()); //设置列数
	ui.tableView_poslist->verticalHeader()->hide();//隐藏行号。
	ui.tableView_poslist->horizontalHeader()->setStyleSheet("border:1px solid #aaaaaa"); //设置表头颜色。
	//获取持仓信息。
	spi->ReqQryInvestorPosition();
	WaitForSingleObject(g_qEvent, INFINITE);


	/*******本地套利订单列表*******/
	//绑定模型。
	ui.tableView_arbordlist->setModel(modelArbOrder);
	//设置表头
	QStringList ordHeader;
	ordHeader << QStr("订单编号") << QStr("条件") << QStr("成交/数量") << QStr("状态") << QStr("操作");
	modelArbOrder->setHorizontalHeaderLabels(ordHeader);
	modelArbOrder->setColumnCount(ordHeader.size());//设置列数
	ui.tableView_arbordlist->verticalHeader()->hide();//隐藏行号。
	ui.tableView_arbordlist->horizontalHeader()->setStyleSheet("border:1px solid #aaaaaa");//设置表头颜色。
	//ui.tableView_arbordlist->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents); //设置自动列宽。
	ui.tableView_arbordlist->setColumnWidth(1, 300); //设置列宽。
	ui.tableView_arbordlist->horizontalHeader()->setStretchLastSection(true); //设置最后一列stretch.
	//填充订单列表模型，
	for (int i = 0; i < g_arbOrderMap.values().count(); i++) {
		AddToArbOrderTable(g_arbOrderMap.values()[i]);
		md_InstrumentID.push_back(g_arbOrderMap.values()[i].OrdLegList[0].InstrumentID.toStdString());
		md_InstrumentID.push_back(g_arbOrderMap.values()[i].OrdLegList[1].InstrumentID.toStdString());
	}
	
	/**********报单列表，************/
	ui.tableView_ordlist->setModel(modelOrder);//绑定模型。
	QStringList liveordHeader;
	liveordHeader << QStr("订单ID") << QStr("合约") << QStr("买卖") << QStr("开平") << QStr("手数") << QStr("未成交") << QStr("报单价") << QStr("挂单状态") << QStr("成交均价") << QStr("报单时间") << QStr("详细状态");
	modelOrder->setHorizontalHeaderLabels(liveordHeader);
	modelOrder->setColumnCount(liveordHeader.size()); //设置列数
	ui.tableView_ordlist->verticalHeader()->hide();//隐藏行号。
	ui.tableView_ordlist->horizontalHeader()->setStyleSheet("border:1px solid #aaaaaa"); //设置表头颜色。
	ui.tableView_ordlist->setColumnHidden(0, true); //订单ID不显示。

	//填充订单列表模型，
	foreach (Order ord, g_orderMap) {
		RefreshOrdTable(ord.OrderRef, "add");
		md_InstrumentID.push_back(ord.InstrumentID.toStdString());
	}


	/**********订阅套利组合、持仓、订单中所有的合约************/
	md_spi->SubscribeMarketData();

	/*********检验表单数值********/
	QDoubleValidator* doubleValid = new QDoubleValidator(0, 100.0, 3, this);
	ui.lineEdit_open_val->setValidator(doubleValid); //比较条件只能为数字。
	ui.lineEdit_close_val->setValidator(doubleValid); //比较条件只能为数字。
	//检验合约。
	QRegExpValidator* instValid = new QRegExpValidator(QRegExp("[a-zA-Z0-9]+"), this);
	ui.lineEdit_ins1->setValidator(instValid);
	ui.lineEdit_ins2->setValidator(instValid);
	//校验为空，
	QRegExpValidator* noEmptyValid = new QRegExpValidator(QRegExp(".*\\S+.*"), this);
	ui.lineEdit_arb_name->setValidator(noEmptyValid);
	//检验公式内容，（只能用A、B、(),数字，和运算符号）
	QRegExpValidator* formulaValid = new QRegExpValidator(QRegExp("[AB0-9\\(\\)\\+\\-\\*\\/ ]+"), this);
	ui.lineEdit_open_formula->setValidator(formulaValid);
	ui.lineEdit_close_formula->setValidator(formulaValid);

	//添加右键菜单，
	ui.tableView_arblist->setContextMenuPolicy(Qt::CustomContextMenu); //必须设置为个才能捕捉右键点击事件。
	QAction* delAction = new QAction(this);
	delAction->setText(QStr("删除组合"));
	popMenu = new QMenu(ui.tableView_arblist);
	popMenu->addAction(delAction);
	//添加右键菜单事件绑定。
	connect(ui.tableView_arblist, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(ShowContextMenu(QPoint)));
	//添加删除事件处理函数。
	QObject::connect(delAction, SIGNAL(triggered()), this, SLOT(DeleteArbPortf()));

}


//事件过滤器，主要是监听鼠标点击合约树控件一外的区域自动关闭，
bool MainWindow::eventFilter(QObject *target, QEvent *event) {
	//treeInst控件显示状态下才去处理。
	if (treeInst != nullptr && treeInst->isVisible()) {
		if (event->type() == QEvent::MouseButtonPress && target->objectName() != treeInst->viewport()->objectName()) {
			QMouseEvent *mouseEvent = (QMouseEvent *)event;
			if (mouseEvent->buttons()&Qt::LeftButton) {
				treeInst->hide();
			}
		}
	}
	return QMainWindow::eventFilter(target, event);
}

//显示登录界面 。
void MainWindow::ShowLoginForm() {
	cout << "hello" << endl;
	w_login->show();
}

//下载合约
void MainWindow::UpdateInstrument() {
	//strcpy_s(g_chInstrumentID, "rb2101"); //留空时，查询全部合约 。查询后的合约，是自动保存下来的，下次订阅时，也是订阅查询的这些合约 。
	spi->ReqQryInstrument();
	WaitForSingleObject(g_qEvent, INFINITE);
	//填充合约树。
	FillModelInst(modelIns, g_instMap);
}

//显示合约信息。 
void MainWindow::ShowInstrument() {
	cout << "中国";
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
	QTextCodec::setCodecForLocale(QTextCodec::codecForLocale());
	//定义树形控件，
	treeInst = new QTreeView(ui.tabWidget_arb_edit);
	treeInst->setHeaderHidden(true); //隐藏头部。
	treeInst->setStyleSheet(QString("background-color:rgb(255, 247, 239);"));
	treeInst->setGeometry(58, 11, 200, 300); //设置树形控件位置。
	//treeInst->setWindowFlag(Qt::WindowCloseButtonHint);
	//treeInst->setWindowFlags(Qt::Popup);
	treeInst->setModel(modelIns);
	treeInst->show();

	//绑定事件过滤器，这样树形控件才可以监听到主窗体上的点击事件（注意要用viewport()才行，），然后才能在点击非主树形控件时，关闭自己。
	treeInst->viewport()->installEventFilter(this);

	//获取信号发送控件。
	QPushButton *btn = qobject_cast<QPushButton *>(sender());
	if (btn->objectName() == "pushButton_ins1") {
		setInsEdit = ui.lineEdit_ins1;
	} else if (btn->objectName() == "pushButton_ins2") {
		setInsEdit = ui.lineEdit_ins2;
	}
	QObject::connect(treeInst, SIGNAL(clicked(QModelIndex)), this, SLOT(ChooseInstrument(QModelIndex)));
}


//选择合约 。
void MainWindow::ChooseInstrument(QModelIndex idx) {
	QStandardItem* item = modelIns->itemFromIndex(idx);
	if (!item->hasChildren()) { //将合约显示在下拉框中。
		setInsEdit->setText(item->text());
		treeInst->hide();
	}
}


//将表单中的数据，填入套利组合对象，
ArbPortf MainWindow::FillArbPortf() {
	ArbPortf item;
	item.Name = ui.lineEdit_arb_name->text();

	ArbLeg leg1;
	leg1.InstrumentID = ui.lineEdit_ins1->text();
	leg1.Direction = ui.comboBox_direction1->currentText();
	leg1.Vol = ui.spinBox_vol1->text().toInt();
	leg1.PriceType = ui.comboBox_ptype1->currentText();
	leg1.SlipPoint = ui.spinBox_slip1->value();
	leg1.Chase = ui.checkBox_chase1->isChecked();
	leg1.ChasePoint = ui.spinBox_chase1_point->value();
	item.LegList.append(leg1);

	ArbLeg leg2;
	leg2.InstrumentID = ui.lineEdit_ins2->text();
	leg2.Direction = ui.comboBox_direction2->currentText();
	leg2.Vol = ui.spinBox_vol2->text().toInt();
	leg2.PriceType = ui.comboBox_ptype2->currentText();
	leg2.SlipPoint = ui.spinBox_slip2->value();
	leg2.Chase = ui.checkBox_chase2->isChecked();
	leg2.ChasePoint = ui.spinBox_chase2_point->value();
	item.LegList.append(leg2);

	item.Offset = ui.comboBox_offset->currentText();
	item.SendOrderType = ui.comboBox_send_order_type->currentText();

	item.OpenCondFormula = ui.lineEdit_open_formula->text();
	item.OpenCondOperator = ui.comboBox_open_operator->currentText();
	item.OpenCondVal = ui.lineEdit_open_val->text();
	item.CloseCondFormula = ui.lineEdit_close_formula->text();
	item.CloseCondOperator = ui.comboBox_close_operator->currentText();
	item.CloseCondVal = ui.lineEdit_close_val->text();

	item.Times = ui.spinBox_times->value();
	item.Loop = ui.checkBox_loop->isChecked();

	
	vector<string>::iterator fnd1 = std::find(md_InstrumentID.begin(), md_InstrumentID.end(), leg1.InstrumentID.toStdString());
	if (fnd1 == md_InstrumentID.end()) {
		md_InstrumentID.push_back(leg1.InstrumentID.toStdString());
		md_spi->SubscribeMarketData();
	}
	vector<string>::iterator fnd2 = std::find(md_InstrumentID.begin(), md_InstrumentID.end(), leg2.InstrumentID.toStdString());
	if (fnd2 == md_InstrumentID.end()) {
		md_InstrumentID.push_back(leg2.InstrumentID.toStdString());
		md_spi->SubscribeMarketData();
	}

	return item;
}




//添加套利组合
void MainWindow::AddArbPortf() {
	//校验表单。
	if (!ui.lineEdit_arb_name->hasAcceptableInput()) {
		QMessageBox::warning(this, QStr("错误"), QStr("组件名称不能为空"));
		return;
	} else if (!ui.lineEdit_open_val->hasAcceptableInput() || !ui.lineEdit_close_val->hasAcceptableInput()) {
		QMessageBox::warning(this, QStr("错误"), QStr("公式条件有误"));
		return;
	} else if (!ui.lineEdit_ins1->hasAcceptableInput() || !ui.lineEdit_ins2->hasAcceptableInput()) {
		QMessageBox::warning(this, QStr("错误"), QStr("合约不能为空"));
		return;
	} else if (!g_instMap.contains(ui.lineEdit_ins1->text()) || !g_instMap.contains(ui.lineEdit_ins2->text())) {
		QMessageBox::warning(this, QStr("错误"), QStr("合约编码有误"));
		return;
	}

	//通过算术静态式计算、来判断公式是否有误。
	QString open_formular = ui.lineEdit_open_formula->text();
	open_formular.replace("A", "1").replace("B", "2");
	QJSValue jval1 = jsEngine->evaluate(open_formular);
	if (jval1.isError()) {
		QMessageBox::warning(this, QStr("错误"), QStr("公式有误") + jval1.toString());
		return;
	}
	//通过算术静态式计算、来判断公式是否有误。
	QString close_formular = ui.lineEdit_close_formula->text();
	close_formular.replace("A", "1").replace("B", "2");
	QJSValue jval2 = jsEngine->evaluate(close_formular);
	if (jval2.isError()) {
		QMessageBox::warning(this, QStr("错误"), QStr("公式有误") + jval2.toString());
		return;
	}

	//将表单数据填到套利组合对象。
	ArbPortf item = FillArbPortf();

	//添加到列表。
	g_arbPortfList.append(item);
	//组合列表模型数据。
	AddToArbPortfTable(item);


}

//点击已经添加的套利组件。
void MainWindow::ClickArbPortf(QModelIndex idx) {
	ArbPortf arb = g_arbPortfList[idx.row()];
	ui.lineEdit_arb_name->setText(arb.Name);
	//合约A
	ui.lineEdit_ins1->setText(arb.LegList[0].InstrumentID);
	ui.comboBox_direction1->setCurrentText(arb.LegList[0].Direction);
	ui.spinBox_vol1->setValue(arb.LegList[0].Vol);
	ui.comboBox_ptype1->setCurrentText(arb.LegList[0].PriceType);
	ui.spinBox_slip1->setValue(arb.LegList[0].SlipPoint);
	ui.checkBox_chase1->setChecked(arb.LegList[0].ChasePoint > 0 ? true : false);
	ui.spinBox_chase1_point->setValue(arb.LegList[0].ChasePoint);
	//合约B
	ui.lineEdit_ins2->setText(arb.LegList[1].InstrumentID);
	ui.comboBox_direction2->setCurrentText(arb.LegList[1].Direction);
	ui.spinBox_vol2->setValue(arb.LegList[1].Vol);
	ui.comboBox_ptype2->setCurrentText(arb.LegList[1].PriceType);
	ui.spinBox_slip2->setValue(arb.LegList[1].SlipPoint);
	ui.checkBox_chase2->setChecked(arb.LegList[1].ChasePoint > 0 ? true : false);
	ui.spinBox_chase2_point->setValue(arb.LegList[1].ChasePoint);
	//下单设置。
	ui.comboBox_offset->setCurrentText(arb.Offset);
	ui.comboBox_send_order_type->setCurrentText(arb.SendOrderType);
	//开仓条件，
	ui.lineEdit_open_formula->setText(arb.OpenCondFormula);
	ui.comboBox_open_operator->setCurrentText(arb.OpenCondOperator);
	ui.lineEdit_open_val->setText(arb.OpenCondVal);
	//平仓条件。
	ui.lineEdit_close_formula->setText(arb.CloseCondFormula);
	ui.comboBox_close_operator->setCurrentText(arb.CloseCondOperator);
	ui.lineEdit_close_val->setText(arb.CloseCondVal);
	//套利次数。
	ui.spinBox_times->setValue(arb.Times);
	ui.checkBox_loop->setChecked(arb.Loop);
}

//显示右键菜单。
void MainWindow::ShowContextMenu(QPoint pos) {
	QModelIndex index = ui.tableView_arblist->indexAt(pos);
	if (index.isValid()) {
		popMenu->exec(QCursor::pos()); // 菜单出现的位置为当前鼠标的位置
	}
}

//删除套利组合。
void MainWindow::DeleteArbPortf() {
	QModelIndex idx = ui.tableView_arblist->currentIndex();
	int row_num = idx.row();
	modelArbPortf->removeRow(row_num);
	g_arbPortfList.removeAt(row_num);
}
//删除套利订单。
void MainWindow::DeleteArbOrder() {
	QModelIndex idx = ui.tableView_arbordlist->currentIndex();

	//获取套利单号
	QString arbOrderId = modelArbOrder->data(modelArbOrder->index(idx.row(), 0)).toString();

	//删除本地套利单
	foreach(QString id, g_arbOrderMap.keys()) {
		if (id == arbOrderId) {
			g_arbOrderMap.remove(id);
			break;
		}
	}
	//删除本地套利单模型。
	for (int i = modelArbOrder->rowCount()-1; i--; i >= 0) {
		if (arbOrderId == modelArbOrder->data(modelArbOrder->index(i, 0)).toString()) {
			modelArbOrder->removeRows(i, 3);
			break;
		}
	}

	//删除普通单
	QList<QString> deletedList;
	foreach(QString orderRef, g_orderMap.keys()) {
		if (g_orderMap[orderRef].ArbOrderId == arbOrderId) {
			g_orderMap.remove(orderRef);
			deletedList.append(orderRef);
		}
	}
	//删除普通单模型。
	for (int i = modelOrder->rowCount() - 1; i--; i >= 0) {
		QString orderRef = modelOrder->data(modelOrder->index(i, 0)).toString();
		if(deletedList.contains(orderRef)){
			modelOrder->removeRow(i);
		}
	}

}

//将持仓同步到模型中
void MainWindow::RefreshPosTable() {

	//填充持仓列表列表模型，
	modelPosition->removeRows(0, modelPosition->rowCount()); //先清空。
	for (int i = 0; i < g_posList.count(); i++) {
		CThostFtdcInvestorPositionField item = g_posList[i];
		int row = modelPosition->rowCount();
		double last_price = 0.0;
		double cost = item.OpenCost / (item.Position*g_instMap[item.InstrumentID].VolumeMultiple);
		double profit = (last_price - cost)* item.Position*g_instMap[item.InstrumentID].VolumeMultiple;
		modelPosition->setItem(row, 0, new QStandardItem(QString(item.InstrumentID)));//合约
		modelPosition->setItem(row, 1, new QStandardItem(PosDirectionMap[item.PosiDirection])); //多空方向
		modelPosition->setItem(row, 2, new QStandardItem(Num(item.Position))); //持仓
		modelPosition->setItem(row, 3, new QStandardItem(Num(cost))); //持仓均价
		modelPosition->setItem(row, 4, new QStandardItem(Num(last_price))); //最新价
		modelPosition->setItem(row, 5, new QStandardItem(Num(profit))); //浮盈
		modelPosition->setItem(row, 6, new QStandardItem(Num(item.TodayPosition))); //今持仓，
		modelPosition->setItem(row, 7, new QStandardItem(Num(item.Position - item.TodayPosition))); //昨持仓
		md_InstrumentID.push_back(item.InstrumentID);
	}
}

//添加套利组合到模型中。
void MainWindow::AddToArbPortfTable(ArbPortf item) {
	int row = modelArbPortf->rowCount();
	modelArbPortf->setItem(row, 0, new QStandardItem(item.Name));
	modelArbPortf->setItem(row, 1, new QStandardItem(item.LegList[0].InstrumentID + " / " + item.LegList[1].InstrumentID));
	modelArbPortf->setItem(row, 2, new QStandardItem("N/A"));
	modelArbPortf->setItem(row, 3, new QStandardItem("N/A"));
	md_InstrumentID.push_back(item.LegList[0].InstrumentID.toStdString());
	md_InstrumentID.push_back(item.LegList[1].InstrumentID.toStdString());
}


//提交套利单 。
void MainWindow::SubmitArbOrder() {
	if (!ui.lineEdit_open_val->hasAcceptableInput() || !ui.lineEdit_close_val->hasAcceptableInput()) {
		QMessageBox::warning(this, "错误", "公式条件有误");
		return;
	} else if (!ui.lineEdit_ins1->hasAcceptableInput() || !ui.lineEdit_ins2->hasAcceptableInput()) {
		QMessageBox::warning(this, "错误", "合约编码有误");
		return;
	} else if (!g_instMap.contains(ui.lineEdit_ins1->text()) || !g_instMap.contains(ui.lineEdit_ins2->text())) {
		QMessageBox::warning(this, "错误", "合约编码有误");
		return;
	}

	//将表单数据填到套利组合对象。
	ArbOrder ord = FillArbOrder();
	g_arbOrderMap.insert(ord.Id, ord);
	

	//新建一个订单处理对象，
	QObject::connect(g_orderWorker, SIGNAL(signal_DealArbOrder(QString)), g_orderWorker, SLOT(DealArbOrder(QString)));
	//将订单处理槽函数移入新的线程去执行，
	QThread* m_pThread = new QThread;
	m_pThread->start();
	g_orderWorker->moveToThread(m_pThread);



	AddToArbOrderTable(ord);
}

//将表单中的数据，填入套利订单对象。
ArbOrder MainWindow::FillArbOrder() {
	ArbOrder ord = { 0 };
	ord.Id = NextArbOrderId();
	ord.Time = QTime::currentTime();

	OrdLeg leg1;
	leg1.InstrumentID = ui.lineEdit_ins1->text();
	leg1.Direction = ui.comboBox_direction1->currentText();
	leg1.Vol = ui.spinBox_vol1->text().toInt();
	leg1.PriceType = ui.comboBox_ptype1->currentText();
	leg1.SlipPoint = ui.spinBox_slip1->value();
	leg1.Chase = ui.checkBox_chase1->isChecked();
	leg1.ChasePoint = ui.spinBox_chase1_point->value();
	leg1.AvgPrice = 0;
	leg1.DealVol = 0;
	ord.OrdLegList.append(leg1);

	OrdLeg leg2;
	leg2.InstrumentID = ui.lineEdit_ins2->text();
	leg2.Direction = ui.comboBox_direction2->currentText();
	leg2.Vol = ui.spinBox_vol2->text().toInt();
	leg2.PriceType = ui.comboBox_ptype2->currentText();
	leg2.SlipPoint = ui.spinBox_slip2->value();
	leg2.Chase = ui.checkBox_chase2->isChecked();
	leg2.ChasePoint = ui.spinBox_chase2_point->value();
	leg2.AvgPrice = 0;
	leg2.DealVol = 0;
	ord.OrdLegList.append(leg2);

	ord.Offset = ui.comboBox_offset->currentText();
	ord.SendOrderType = ui.comboBox_send_order_type->currentText();
	if (ord.Offset == QStr("开仓")) {
		ord.CondFormula = ui.lineEdit_open_formula->text();
		ord.CondOperator = ui.comboBox_open_operator->currentText();
		ord.CondVal = ui.lineEdit_open_val->text();
	} else if (ord.Offset == QStr("平仓")) {
		ord.CondFormula = ui.lineEdit_close_formula->text();
		ord.CondOperator = ui.comboBox_close_operator->currentText();
		ord.CondVal = ui.lineEdit_close_val->text();
	}

	ord.Times = ui.spinBox_times->value();
	ord.Loop = ui.checkBox_loop->isChecked();


	return ord;
}

//将套利订单信息添加到模型中。
void MainWindow::AddToArbOrderTable(ArbOrder ord) {

	//订单列表模型数据。
	int row = modelArbOrder->rowCount();
	QString title = ord.CondFormula + ord.CondOperator + ord.CondVal + " " + ord.Offset + " " + ord.SendOrderType;

	modelArbOrder->setItem(row, 0, new QStandardItem(ord.Id));
	modelArbOrder->setItem(row, 1, new QStandardItem(title));
	modelArbOrder->setItem(row, 2, new QStandardItem("0/" + Num(ord.Times)));
	modelArbOrder->setItem(row, 3, new QStandardItem(""));

	for (int i = 0; i < ord.OrdLegList.count(); i++) {
		OrdLeg leg = ord.OrdLegList[i];
		QString line = leg.Direction + " " + leg.InstrumentID + Num(leg.Vol) + QStr("手 ") + leg.PriceType + "+" + Num(leg.ChasePoint) + (leg.Chase ? QStr(" 追价+") + Num(leg.ChasePoint) : QStr(" 不追价"));
		modelArbOrder->setItem(row + 1 + i, 1, new QStandardItem(line));
		modelArbOrder->setItem(row + 1 + i, 2, new QStandardItem("0/" + Num(leg.Vol)));
	}

	// 添加删除按扭。
	QPushButton *delBtn = new QPushButton(QStr("删除"));
	delBtn->setMaximumHeight(20);
	delBtn->setMaximumWidth(100);
	ui.tableView_arbordlist->setIndexWidget(modelArbOrder->index(row, 4), delBtn);
	
	
	
	ui.tableView_arbordlist->
	QObject::connect(delBtn, SIGNAL(clicked()), this, SLOT(DeleteArbOrder())); //绑定事件。

	//合并第一列、最后一列。
	ui.tableView_arbordlist->setSpan(row, 0, ord.OrdLegList.count() + 1, 1);
	ui.tableView_arbordlist->setSpan(row, 4, ord.OrdLegList.count() + 1, 1);
}

//计算套利组合的价格。
double CalArbPortfPrice(ArbPortf item, QString side) {
	if (!g_depthMap.contains(item.LegList[0].InstrumentID) || !g_depthMap.contains(item.LegList[1].InstrumentID)) return 0;

	QString open_formular = item.OpenCondFormula;
	CThostFtdcDepthMarketDataField depth1 = g_depthMap[item.LegList[0].InstrumentID];
	CThostFtdcDepthMarketDataField depth2 = g_depthMap[item.LegList[1].InstrumentID];
	if (side == "buy") {
		if (depth1.AskVolume1 <= 0 || depth2.BidVolume1 <= 0) return NULL;
		open_formular.replace("A", Num(depth1.AskPrice1)).replace("B", Num(depth2.BidPrice1));
	} else if (side == "sell") {
		if (depth1.BidVolume1 <= 0 || depth2.AskVolume1 <= 0) return NULL;
		open_formular.replace("A", Num(depth1.BidPrice1)).replace("B", Num(depth2.AskPrice1));
	}
	QJSEngine jsEngine;
	QJSValue jval = jsEngine.evaluate(open_formular);
	if (jval.isError()) {
		return NULL;
	}
	return jval.toNumber();
}


//更新持仓盈亏(价格有更新时)
void MainWindow::UpdatePositionProfit(QString instrument_id, double last_price) {
	//填充持仓列表列表模型， 
	for (int i = 0; i < g_posList.count(); i++) {
		CThostFtdcInvestorPositionField item = g_posList[i];
		if (QString(item.InstrumentID) == instrument_id) {
			double cost = item.OpenCost / (item.Position*g_instMap[item.InstrumentID].VolumeMultiple);
			double profit = (last_price - cost)* item.Position*g_instMap[item.InstrumentID].VolumeMultiple;
			if (item.PosiDirection == THOST_FTDC_PD_Short) profit = -profit;
			modelPosition->setItem(i, 4, new QStandardItem(Num(last_price))); //最新价
			modelPosition->setItem(i, 5, new QStandardItem(Num(profit))); //浮盈
		}
	}
}

//更新套利组件价差(价格有更新时)
void MainWindow::UpdateArbPrice(QString instrument_id, double last_price) {
	//填充持仓列表列表模型， 
	for (int i = 0; i < g_arbPortfList.count(); i++) {
		ArbPortf item = g_arbPortfList[i];
		if (item.LegList[0].InstrumentID == instrument_id || item.LegList[1].InstrumentID == instrument_id) {
			double price1 = CalArbPortfPrice(item, "buy");
			double price2 = CalArbPortfPrice(item, "sell");
			modelArbPortf->setItem(i, 2, new QStandardItem(Num(price1))); //最新价
			modelArbPortf->setItem(i, 3, new QStandardItem(Num(price2))); //最新价
		}
	}
}


//将订单更新到模型中
//liveordHeader << QStr("合约") << QStr("买卖") << QStr("开平") << QStr("手数") << QStr("未成交") << QStr("报单价") << QStr("挂单状态") << QStr("成交均价") << QStr("报单时间") << QStr("详细状态");
void MainWindow::RefreshOrdTable(QString orderRef, QString type) {
	Order ord = g_orderMap[orderRef];
	//新增
	if (type == "add") {
		//挂单表格
		int row = modelOrder->rowCount();
		int col = 0;
		modelOrder->setItem(row, col++, new QStandardItem(ord.OrderRef));//订单orderRef
		modelOrder->setItem(row, col++, new QStandardItem(ord.InstrumentID));//合约
		modelOrder->setItem(row, col++, new QStandardItem(ord.Direction)); //买卖
		modelOrder->setItem(row, col++, new QStandardItem(ord.Offset)); //开平
		modelOrder->setItem(row, col++, new QStandardItem(Num(ord.VolumeTotalOriginal))); //手数
		modelOrder->setItem(row, col++, new QStandardItem(Num(ord.VolumeTotal))); //未成交
		modelOrder->setItem(row, col++, new QStandardItem(Num(ord.Price))); //报单价，
		modelOrder->setItem(row, col++, new QStandardItem(OrdStatusMap[ord.OrderStatus.toLatin1()]));  //挂单状态，
		modelOrder->setItem(row, col++, new QStandardItem(Num(ord.AvgPrice))); //成交均价，要去成交回报中才能更新。
		modelOrder->setItem(row, col++, new QStandardItem(ord.InsertDate + " " + ord.InsertTime)); //报单时间，
		md_InstrumentID.push_back(ord.InstrumentID.toStdString().c_str());

	} else if (type == "update") { //更新成交数量、成均价、状态、
		//更新挂单表
		for (int i = 0; i < modelOrder->rowCount(); i++) {
			QString orderRef = modelOrder->itemFromIndex(modelOrder->index(i, 0))->text();
			if (ord.OrderRef == orderRef) {
				modelOrder->setItem(i, 5, new QStandardItem(Num(ord.VolumeTotal))); //未成交
				modelOrder->setItem(i, 7, new QStandardItem(OrdStatusMap[ord.OrderStatus.toLatin1()]));  //挂单状态，
				modelOrder->setItem(i, 8, new QStandardItem(Num(ord.AvgPrice))); //成交均价，要去成交回报中才能更新。
				break;
			}
		}

		//更新套利单表
		for (int i = 0; i < modelArbOrder->rowCount(); i++) {
			QString arbOrderId = modelArbOrder->itemFromIndex(modelArbOrder->index(i, 0))->text();
			if (arbOrderId == ord.ArbOrderId) { //找到套利单，
				ArbOrder arbord = g_arbOrderMap[arbOrderId];
				for (int j = 0; j < arbord.OrdLegList.count(); j++) {//找到套利腿。
					OrdLeg leg = arbord.OrdLegList[j];
					if (leg.OrderRef == orderRef) {
						QString str = Num(leg.DealVol) + "/" + Num(leg.Vol) + " (" + Num(leg.AvgPrice) + ")";
						modelArbOrder->setItem(i + 1 + j, 2, new QStandardItem(str)); //成交数量、价格
						QString statusStr = OrdStatusMap[leg.Status.toLatin1()];
						modelArbOrder->setItem(i + 1 + j, 3, new QStandardItem(statusStr));  //挂单状态，
					}
				}
				break;
			}
		}
	}

}

