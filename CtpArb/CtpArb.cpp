﻿#include "CtpArb.h"
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


using namespace std;

/******全局变量******/
QStandardItemModel *modelIns = new QStandardItemModel(); //合约列表模型。
QStandardItemModel *modelArb = new QStandardItemModel(); ; //套利组合列表模型
QStandardItemModel *modelOrd = new QStandardItemModel(); ; //订单列表模型
QList<ArbPortf> arbList; //套利组合列表。
QList<ArbOrder> ordList; //订单列表。
LoginForm* fm; //登录表单，
QTreeView *treeInst; //合约树。
QLineEdit* setInsEdit; //标记当前要设置的是哪个合约。
QMenu* popMenu; //套利组件上的右键菜单。

struct Test {
public:
	char a1[30];
	char* a2;
	char a;
	int b;
	double c;

	//序列化 
	friend QDataStream &operator<<(QDataStream& input, const Test& dt) {
		QString str = QString("北京"); // QString(dt.a2);
		input << QString(dt.a1) << QString("北京") << QChar(dt.a) << qint32(dt.b) << qreal(dt.c);
		return input;
	};

	//反序列化
	friend QDataStream &operator>>(QDataStream& output, Test& dt) {
		QByteArray bts;
		QString str;
		output >> dt.a1 >> str >> dt.a >> dt.b >> dt.c;


		QTextCodec *tc = QTextCodec::codecForName("GBK");
		QString tmpQStr = tc->toUnicode(bts);


		return output;
	};
};

CtpArb::CtpArb(QWidget *parent)
    : QMainWindow(parent)
{
	/*
	char cr[16] = {};
	bool aa = cr[0] == '\0';
	bool a = cr == ""; //false
	int i = sizeof(cr); //16
	int i2 = strlen(cr); //6
	QString s = QString(cr); //乱码，
	int a2 = s.count(); //6
	string s2 = string(cr); //乱码。
	QString str2 = QString::fromLocal8Bit(cr); //乱码。
	*/

	/*
	//保存。
	QMap<QString, Test> mp1;
	Test dt;
	QString str = QString("北京");
	QByteArray ba = str.toLatin1(); 

	strcpy(dt.a1, "rb1801");
	strcpy(dt.a2, ba.data());
	dt.a = 'c';
	dt.b = 10;
	dt.c = 0.00000001;
	mp1["test"] = dt;
	QFile f1("instmap.dat");
	f1.open(QIODevice::WriteOnly);
	QDataStream st(&f1);
	st << mp1;
	f1.close();

	QMap<QString, Test> mp2;
	QFile f2("instmap.dat");
	f2.open(QIODevice::ReadOnly);
	QDataStream st2(&f2);
	st2 >> mp2;
	f2.close();
	

	system("pause");
	//if (true) return;
	*/

	


    ui.setupUi(this);	
	
	//添加事件过滤器。
	ui.centralWidget->installEventFilter(this);

	/********集中添加信号绑定********/
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

	//合约列表，反序列化。
	QFile file("instmap.dat");
	file.open(QIODevice::ReadOnly);
	QDataStream stream(&file);
	stream >> g_instMap;
	//填充模型。
	FillModelInst(); 

	/*******套利组合列表*******/
	//绑定模型。
	ui.tableView_arblist->setModel(modelArb); 
	//设置表头
	QStringList arbHeader;
	arbHeader << "名称" << "合约" << "买"  << "卖";
	modelArb->setHorizontalHeaderLabels(arbHeader);
	modelArb->setColumnCount(arbHeader.size()); //设置列数
	ui.tableView_arblist->verticalHeader()->hide();//隐藏行号。
	ui.tableView_arblist->horizontalHeader()->setStyleSheet("border:1px solid #aaaaaa"); //设置表头颜色。
	//读取历史套利组合列表。(反序列化)
	QFile file1("arblist.dat");
	file1.open(QIODevice::ReadOnly);
	QDataStream stream1(&file1);
	stream1 >> arbList;
	//填充套利组合列表模型，
	for (int i = 0; i < arbList.count(); i++) {
		ArbPortf item = arbList[i];
		int row = modelArb->rowCount();
		modelArb->setItem(row, 0, new QStandardItem(item.Name));
		modelArb->setItem(row, 1, new QStandardItem(item.LegList[0].InstrumentID + " / " + item.LegList[1].InstrumentID));
		modelArb->setItem(row, 2, new QStandardItem("N/A"));
		modelArb->setItem(row, 3, new QStandardItem("N/A"));
	}

	/*******订单列表*******/
	//绑定模型。
	ui.tableView_ordlist->setModel(modelOrd);
	//设置表头
	QStringList ordHeader;
	ordHeader << "订单编号" << "条件" << "状态" << "操作" ;
	modelOrd->setHorizontalHeaderLabels(ordHeader);
	modelOrd->setColumnCount(ordHeader.size());//设置列数
	ui.tableView_ordlist->verticalHeader()->hide();//隐藏行号。
	ui.tableView_ordlist->horizontalHeader()->setStyleSheet("border:1px solid #aaaaaa");//设置表头颜色。
	//ui.tableView_ordlist->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents); //设置自动列宽。
	ui.tableView_ordlist->setColumnWidth(1, 300); //设置列宽。
	ui.tableView_ordlist->horizontalHeader()->setStretchLastSection(true); //设置最后一列stretch.
	//读取历史套利组合列表。(反序列化)
	QFile file2("ordlist.dat");
	file2.open(QIODevice::ReadOnly);
	QDataStream stream2(&file2);
	QStandardItemModel it;
	stream2 >> ordList; 
	//填充订单列表模型，
	for (int i = 0; i < ordList.count(); i++) {
		AddToModelOrderTable(ordList[i]);
	}

	/*********检验表单数值********/
	QDoubleValidator* doubleValid = new QDoubleValidator(0, 100.0, 3, this);
	ui.lineEdit_open_val->setValidator(doubleValid); //比较条件只能为数字。
	ui.lineEdit_close_val->setValidator(doubleValid); //比较条件只能为数字。
	//检验合约。
	QRegExpValidator* instValid = new QRegExpValidator(QRegExp("[a-z0-9]+"), this);
	ui.lineEdit_ins1->setValidator(instValid);
	ui.lineEdit_ins2->setValidator(instValid);
	//校验为空，
	QRegExpValidator* noEmptyValid = new QRegExpValidator(QRegExp(".*\\S+.*"), this);
	ui.lineEdit_arb_name->setValidator(noEmptyValid);


	//添加右键菜单，
	ui.tableView_arblist->setContextMenuPolicy(Qt::CustomContextMenu); //必须设置为个才能捕捉右键点击事件。
	QAction* delAction = new QAction(this);
	delAction->setText(QString("删除组合"));
	popMenu = new QMenu(ui.tableView_arblist);
	popMenu->addAction(delAction);
	//添加右键菜单事件绑定。
	connect(ui.tableView_arblist, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(ShowContextMenu(QPoint))); 
	//添加删除事件处理函数。
	QObject::connect(delAction, SIGNAL(triggered()), this, SLOT(DeleteArbPortf()));

}


//析构函数。
CtpArb::~CtpArb()
{
	//保存合约信息，
	/**/
	QFile file("instmap.dat");
	file.open(QIODevice::WriteOnly);
	QDataStream stream(&file);
	stream <<g_instMap;
	file.close();
	

	//保存历史套利组合列表。(反序列化)
	QFile file1("arblist.dat");
	file1.open(QIODevice::WriteOnly);
	QDataStream stream1(&file1);
	stream1 << arbList;

	//保存历史套利订单列表。(反序列化)
	QFile file2("ordlist.dat");
	file2.open(QIODevice::WriteOnly);
	QDataStream stream2(&file2);
	stream2 << ordList;


}

//事件过滤器，主要是监听鼠标点击合约树控件一外的区域自动关闭，
bool CtpArb::eventFilter(QObject *target, QEvent *event)
{
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
void CtpArb::ShowLoginForm()
{
	cout << "hello" << endl;
	fm = new LoginForm();
	(*fm).setWindowModality(Qt::ApplicationModal);
	(*fm).show();
}


//显示合约信息。 
void CtpArb::ShowInstrument()
{
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
	}else if(btn->objectName() == "pushButton_ins2") {
		setInsEdit = ui.lineEdit_ins2;
	}
	QObject::connect(treeInst, SIGNAL(clicked(QModelIndex)), this, SLOT(ChooseInstrument(QModelIndex)));
}


//选择合约 。
void CtpArb::ChooseInstrument(QModelIndex idx)
{
	QStandardItem* item = modelIns->itemFromIndex(idx);
	if (!item->hasChildren()) { //将合约显示在下拉框中。
		setInsEdit->setText(item->text());
		treeInst->hide();
	}
}

//将表单中的数据，填入套利组合对象，
ArbPortf CtpArb::FillArbPortf() {
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
	return item;
}

//将表单中的数据，填入套利订单对象。
ArbOrder CtpArb::FillArbOrder() {
	ArbOrder item;
	item.Name = ui.lineEdit_arb_name->text();

	OrdLeg leg1;
	leg1.InstrumentID = ui.lineEdit_ins1->text();
	leg1.Direction = ui.comboBox_direction1->currentText();
	leg1.Vol = ui.spinBox_vol1->text().toInt();
	leg1.PriceType = ui.comboBox_ptype1->currentText();
	leg1.SlipPoint = ui.spinBox_slip1->value();
	leg1.Chase = ui.checkBox_chase1->isChecked();
	leg1.ChasePoint = ui.spinBox_chase1_point->value();
	item.OrdLegList.append(leg1);

	OrdLeg leg2;
	leg2.InstrumentID = ui.lineEdit_ins2->text();
	leg2.Direction = ui.comboBox_direction2->currentText();
	leg2.Vol = ui.spinBox_vol2->text().toInt();
	leg2.PriceType = ui.comboBox_ptype2->currentText();
	leg2.SlipPoint = ui.spinBox_slip2->value();
	leg2.Chase = ui.checkBox_chase2->isChecked();
	leg2.ChasePoint = ui.spinBox_chase2_point->value();
	item.OrdLegList.append(leg2);

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
	return item;
}


//添加套利组合
void CtpArb::AddArbPortf()
{
	//校验表单。
	if (!ui.lineEdit_arb_name->hasAcceptableInput()) {
		QMessageBox::warning(this, "错误", "组件名称不能为空");
		return;
	}else if (!ui.lineEdit_open_val->hasAcceptableInput() || !ui.lineEdit_close_val->hasAcceptableInput()) {
		QMessageBox::warning(this, "错误", "公式条件有误");
		return;
	}else if (!ui.lineEdit_ins1->hasAcceptableInput() || !ui.lineEdit_ins2->hasAcceptableInput()) {
		QMessageBox::warning(this, "错误", "合约编码有误");
		return;
	}

	//将表单数据填到套利组合对象。
	ArbPortf item = FillArbPortf();

	//添加到列表。
	arbList.append(item);

	//组合列表模型数据。
	int row = modelArb->rowCount();
	modelArb->setItem(row, 0, new QStandardItem(item.Name));
	modelArb->setItem(row, 1, new QStandardItem(item.LegList[0].InstrumentID + " / " + item.LegList[1].InstrumentID));
	modelArb->setItem(row, 2, new QStandardItem("N/A"));
	modelArb->setItem(row, 3, new QStandardItem("N/A"));

}

//点击已经添加的套利组件。
void CtpArb::ClickArbPortf(QModelIndex idx)
{
	ArbPortf arb = arbList[idx.row()];
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
void CtpArb::ShowContextMenu(QPoint pos)
{
	QModelIndex index = ui.tableView_arblist->indexAt(pos);
	if (index.isValid()){
		popMenu->exec(QCursor::pos()); // 菜单出现的位置为当前鼠标的位置
	}
}

//删除套利组合。
void CtpArb::DeleteArbPortf()
{
	QModelIndex idx = ui.tableView_arblist->currentIndex();
	int row_num = idx.row();
	modelArb->removeRow(row_num);
	arbList.removeAt(row_num);
}
//删除套利订单。
void CtpArb::DeleteArbOrder()
{
	QModelIndex idx = ui.tableView_ordlist->currentIndex();

	//删除订单列表。
	int oid = modelOrd->data(modelOrd->index(idx.row(), 0)).toInt();
	for (int i = 0; i < ordList.count(); i++) {
		if (ordList[i].Id == oid) {
			ordList.removeAt(i);
			break;
		}
	}
	
	//注意这段放下面，否则会出错，因为上面代码用到了modelOrd
	//删除表单模型中对应的3行。
	modelOrd->removeRows(idx.row(), 3);
}

//将订单信息添加到模型中。
void CtpArb::AddToModelOrderTable(ArbOrder ord) {
	//订单列表模型数据。
	int row = modelOrd->rowCount();

	QString title = ord.Offset == "开仓" ? ord.OpenCondFormula + ord.OpenCondOperator + ord.CloseCondVal : ord.CloseCondFormula + ord.CloseCondOperator + ord.CloseCondVal;
	title += ord.SendOrderType;

	modelOrd->setItem(row, 0, new QStandardItem(QString::number(ord.Id)));
	modelOrd->setItem(row, 1, new QStandardItem(title));
	modelOrd->setItem(row, 2, new QStandardItem("0/" + QString::number(ord.Times)));
	//modelOrderTable->setItem(row, 3, new QStandardItem("N/A"));

	for (int i = 0; i < ord.OrdLegList.count(); i++) {
		ArbLeg leg = ord.OrdLegList[i];
		QString line = leg.Direction + " " + leg.InstrumentID + QString::number(leg.Vol) + "手 " + leg.PriceType + "+" + QString::number(leg.ChasePoint) + (leg.Chase ? " 追价+" + QString::number(leg.ChasePoint) : " 不追价");
		modelOrd->setItem(row + 1 + i, 1, new QStandardItem(line));
		modelOrd->setItem(row + 1 + i, 2, new QStandardItem("0/" + QString::number(leg.Vol)));
	}

	// 添加删除按扭。
	QPushButton *delBtn = new QPushButton("删除");
	delBtn->setMaximumHeight(20);
	ui.tableView_ordlist->setIndexWidget(modelOrd->index(row, 3), delBtn);
	QObject::connect(delBtn, SIGNAL(clicked()), this, SLOT(DeleteArbOrder())); //绑定事件。

	//合并第一列、最后一列。
	ui.tableView_ordlist->setSpan(row, 0, ord.OrdLegList.count() + 1, 1);
	ui.tableView_ordlist->setSpan(row, 3, ord.OrdLegList.count() + 1, 1);
}

//获取最大的订单号。
int CtpArb::getNewOrdId() {
	int id = 0;
	for (int i = 0; i < ordList.count(); i++) {
		if (ordList[i].Id > id) {
			id = ordList[i].Id;
		}
	}
	return id + 1;
}

//提交套利单 。
void CtpArb::SubmitArbOrder()
{
	if (!ui.lineEdit_open_val->hasAcceptableInput() || !ui.lineEdit_close_val->hasAcceptableInput()) {
		QMessageBox::warning(this, "错误", "公式条件有误");
		return;
	}
	else if (!ui.lineEdit_ins1->hasAcceptableInput() || !ui.lineEdit_ins2->hasAcceptableInput()) {
		QMessageBox::warning(this, "错误", "合约编码有误");
		return;
	}

	//将表单数据填到套利组合对象。
	ArbOrder ord = FillArbOrder();
	ord.Id = getNewOrdId();
	ord.Time = QTime::currentTime();
	ordList.append(ord);

	//添加到模型。
	AddToModelOrderTable(ord);



}

