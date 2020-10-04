#include "CtpArb.h"
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


using namespace std;

/******全局变量******/
QStandardItemModel *modelInstTree = new QStandardItemModel(); //合约列表模型。
QStandardItemModel *modelArbTable = new QStandardItemModel(); ; //套利列表模型
QList<ArbPortf> arbList; //套利策略列表。
LoginForm* fm; //登录表单，
QTreeView *treeInst; //合约树。
QLineEdit* setInsEdit; //标记当前要设置的是哪个合约。
QMenu* popMenu; //套利组件上的右键菜单。

CtpArb::CtpArb(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);	
	
	//添加事件过滤器。
	ui.centralWidget->installEventFilter(this);

	/********集中添加信号绑定********/
	//登录按扭。
	QObject::connect(ui.actionLogin, SIGNAL(triggered()), this, SLOT(ShowLoginForm()));
	//添加套利组合。
	QObject::connect(ui.pushButton_add_arb, SIGNAL(clicked()), this, SLOT(AddArbPortf()));
	//单击套利组合
	QObject::connect(ui.tableView_arblist, SIGNAL(clicked(QModelIndex)), this, SLOT(ClickArbPortf(QModelIndex)));


	/*******策略列表*******/
	//绑定模型。
	ui.tableView_arblist->setModel(modelArbTable); 
	//modelArbTable->setHeaderData()
	//设置表头
	QStringList strHeader;
	strHeader << "名称" << "合约" << "买"  << "卖";
	modelArbTable->setHorizontalHeaderLabels(strHeader);
	//设置列数
	modelArbTable->setColumnCount(strHeader.size());
	//设置行数
	//modelArbTable->setRowCount(5);
	//隐藏行号。
	ui.tableView_arblist->verticalHeader()->hide();
	//设置表头颜色。
	ui.tableView_arblist->horizontalHeader()->setStyleSheet("border:1px solid #aaaaaa");

	//检验浮点数值，
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

	//读取历史套利组合列表。(反序列化)
	QFile file("arblist.dat");
	file.open(QIODevice::ReadOnly);
	QDataStream stream(&file);   
	stream >> arbList;

	//填充策略列表模型，
	for (int i = 0; i < arbList.count(); i++) {
		ArbPortf item = arbList[i];
		int row = modelArbTable->rowCount();
		modelArbTable->setItem(row, 0, new QStandardItem(item.Name));
		modelArbTable->setItem(row, 1, new QStandardItem(item.LegList[0].InstrumentID + " / " + item.LegList[1].InstrumentID));
		modelArbTable->setItem(row, 2, new QStandardItem("N/A"));
		modelArbTable->setItem(row, 3, new QStandardItem("N/A"));
	}
	

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
	//保存历史套利组合列表。(反序列化)
	QFile file("arblist.dat");
	file.open(QIODevice::WriteOnly);
	QDataStream stream(&file);
	stream << arbList;

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
	treeInst->setModel(modelInstTree);
	treeInst->show();

	//绑定事件过滤器，这样可以监听到主窗体的事件，注意要用viewport()才行，否则根本监听不到鼠标单击事件。
	//treeInst->installEventFilter(this);
	treeInst->viewport()->installEventFilter(this);

	//ui.splitter->addWidget(treeInst);

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
	QStandardItem* item = modelInstTree->itemFromIndex(idx);
	if (!item->hasChildren()) { //将合约显示在下拉框中。
		setInsEdit->setText(item->text());
		treeInst->hide();
	}
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

	ArbPortf item;
	item.Name = ui.lineEdit_arb_name->text();

	ArbLeg leg1;
	leg1.InstrumentID = ui.lineEdit_ins1->text();
	leg1.Direction = ui.comboBox_direction1->currentText();
	leg1.Vol = ui.spinBox_vol1->text().toInt();
	leg1.PriceType = ui.comboBox_ptype1->currentText();
	leg1.SlipPoint = ui.spinBox_slip1->value();
	leg1.ChasePoint = ui.checkBox_chase1->isChecked() ? ui.spinBox_chase1_point->value() : 0;
	item.LegList.append(leg1);

	ArbLeg leg2;
	leg2.InstrumentID = ui.lineEdit_ins2->text();
	leg2.Direction = ui.comboBox_direction2->currentText();
	leg2.Vol = ui.spinBox_vol2->text().toInt();
	leg2.PriceType = ui.comboBox_ptype2->currentText();
	leg2.SlipPoint = ui.spinBox_slip2->value();
	leg2.ChasePoint = ui.checkBox_chase2->isChecked() ? ui.spinBox_chase2_point->value() : 0;
	item.LegList.append(leg2);

	item.Offset = ui.comboBox_offset->currentText();
	item.SendOrderType = ui.comboBox_send_order_type->currentText();

	item.OpenCondFormula = ui.lineEdit_open_formula->text();
	item.OpenCondOperator = ui.comboBox_open_operator->currentText();
	item.OpenCondVal = ui.lineEdit_open_val->text();
	item.CloseCondFormula = ui.lineEdit_close_formula->text();
	item.CloseCondOperator = ui.comboBox_close_operator->currentText();
	item.CloseCondVal = ui.lineEdit_close_val->text();

	arbList.append(item);

	//策略列表模型数据。
	int row = modelArbTable->rowCount();
	modelArbTable->setItem(row, 0, new QStandardItem(item.Name));
	modelArbTable->setItem(row, 1, new QStandardItem(item.LegList[0].InstrumentID + " / " + item.LegList[1].InstrumentID));
	modelArbTable->setItem(row, 2, new QStandardItem("N/A"));
	modelArbTable->setItem(row, 3, new QStandardItem("N/A"));

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
	modelArbTable->removeRow(row_num);
	arbList.removeAt(row_num);
}

