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

QTreeView *treeInst; //��Լ����
QLineEdit* setInsEdit; //��ǵ�ǰҪ���õ����ĸ���Լ��
QMenu* popMenu; //��������ϵ��Ҽ��˵���

// JS���桢����JS��ѧ���ʽ��
QJSEngine* jsEngine;

//����������
MainWindow::~MainWindow() {}

//����رպ�����
void MainWindow::closeEvent(QCloseEvent * event) {
	/*
	QMessageBox::StandardButton result = QMessageBox::question(this, QStr("ȷ��"), QStr("ȷ��Ҫ�˳���ϵͳ��"),
		QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
		QMessageBox::No);

	if (result == QMessageBox::Yes)
		event->accept();
	else
		event->ignore();
	*/

	//������ʷ���������б�(�����л�)
	QFile file("data.dat");
	file.open(QIODevice::WriteOnly);
	QDataStream stream(&file);
	stream << g_instMap << g_arbPortfList << g_arbOrderMap << g_ArbOrderId << g_orderMap << g_OrderRef;
	file.close();

}

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent) {

	//JS�����ʼ����
	jsEngine = new QJSEngine();

	ui.setupUi(this);
	//����¼���������
	ui.centralWidget->installEventFilter(this);

}

//��ʼ��������Ҫ�������ã����ܷŵ����캯������Ϊ��ߵ�һЩ�����õ��˶�����ָ�룬�����캯������ʱ���������Ǹ��յġ�
void MainWindow::Init() {

	/********���а󶨲ۺ��������������Ӧ�ࣩ********/
	//��¼��Ť��
	QObject::connect(ui.actionLogin, SIGNAL(triggered()), this, SLOT(ShowLoginForm()));
	//�����Լ1��Ť
	QObject::connect(ui.pushButton_ins1, SIGNAL(clicked()), this, SLOT(ShowInstrument()));
	//��Ӻ�Լ2��Ť
	QObject::connect(ui.pushButton_ins2, SIGNAL(clicked()), this, SLOT(ShowInstrument()));
	//���������ϡ�
	QObject::connect(ui.pushButton_add_arb, SIGNAL(clicked()), this, SLOT(AddArbPortf()));
	//�����������
	QObject::connect(ui.tableView_arblist, SIGNAL(clicked(QModelIndex)), this, SLOT(ClickArbPortf(QModelIndex)));
	//�ύ������ ��
	QObject::connect(ui.pushButton_submit_order, SIGNAL(clicked()), this, SLOT(SubmitArbOrder()));
	//���º�Լ
	QObject::connect(ui.actionUpdateInstrument, SIGNAL(triggered()), this, SLOT(UpdateInstrument()));

	/*********���Զ����źŲۺ������ص������д�����********/
	//�ֲָ�ӯ����
	QObject::connect(this, SIGNAL(signal_UpdatePositionProfit(QString, double)), this, SLOT(UpdatePositionProfit(QString, double)));
	//�ֲ��б����
	QObject::connect(this, SIGNAL(signal_RefreshPosTable()), this, SLOT(RefreshPosTable()));
	//δ�ɽ������б����
	QObject::connect(this, SIGNAL(signal_RefreshOrdTable(QString, QString)), this, SLOT(RefreshOrdTable(QString, QString)));
	//������ϼ۲����
	QObject::connect(this, SIGNAL(signal_UpdateArbPrice(QString, double)), this, SLOT(UpdateArbPrice(QString, double)));

	//�����л����ָ����ݣ�
	QFile file("data.dat");
	if (file.open(QIODevice::ReadOnly)) {
		QDataStream stream(&file);
		stream >> g_instMap >> g_arbPortfList >> g_arbOrderMap >> g_ArbOrderId >> g_orderMap >> g_OrderRef;
	}


	//��Լ�б����ģ�͡�
	FillModelInst(modelIns, g_instMap);

	/*******��������б�*******/
	//��ģ�͡�
	ui.tableView_arblist->setModel(modelArbPortf);
	//���ñ�ͷ
	QStringList arbHeader;
	arbHeader << QStr("����") << QStr("��Լ") << QStr("��") << QStr("��");
	modelArbPortf->setHorizontalHeaderLabels(arbHeader);
	modelArbPortf->setColumnCount(arbHeader.size()); //��������
	ui.tableView_arblist->verticalHeader()->hide();//�����кš�
	ui.tableView_arblist->horizontalHeader()->setStyleSheet("border:1px solid #aaaaaa"); //���ñ�ͷ��ɫ��
	//�����������б�ģ�ͣ�
	for (int i = 0; i < g_arbPortfList.count(); i++) {
		ArbPortf item = g_arbPortfList[i];
		AddToArbPortfTable(item);
	}

	/************�ֲ��б�***********************/
	//��ģ�͡�
	ui.tableView_poslist->setModel(modelPosition);
	//���ñ�ͷ
	QStringList posHeader;
	posHeader << QStr("��Լ") << QStr("����") << QStr("�ֲ�") << QStr("�ֲ־���") << QStr("���¼�") << QStr("����ӯ��") << QStr("��ֲ�") << QStr("��ֲ�");
	modelPosition->setHorizontalHeaderLabels(posHeader);
	modelPosition->setColumnCount(posHeader.size()); //��������
	ui.tableView_poslist->verticalHeader()->hide();//�����кš�
	ui.tableView_poslist->horizontalHeader()->setStyleSheet("border:1px solid #aaaaaa"); //���ñ�ͷ��ɫ��
	//��ȡ�ֲ���Ϣ��
	spi->ReqQryInvestorPosition();
	WaitForSingleObject(g_qEvent, INFINITE);


	/*******�������������б�*******/
	//��ģ�͡�
	ui.tableView_arbordlist->setModel(modelArbOrder);
	//���ñ�ͷ
	QStringList ordHeader;
	ordHeader << QStr("�������") << QStr("����") << QStr("�ɽ�/����") << QStr("״̬") << QStr("����");
	modelArbOrder->setHorizontalHeaderLabels(ordHeader);
	modelArbOrder->setColumnCount(ordHeader.size());//��������
	ui.tableView_arbordlist->verticalHeader()->hide();//�����кš�
	ui.tableView_arbordlist->horizontalHeader()->setStyleSheet("border:1px solid #aaaaaa");//���ñ�ͷ��ɫ��
	//ui.tableView_arbordlist->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents); //�����Զ��п�
	ui.tableView_arbordlist->setColumnWidth(1, 300); //�����п�
	ui.tableView_arbordlist->horizontalHeader()->setStretchLastSection(true); //�������һ��stretch.
	//��䶩���б�ģ�ͣ�
	for (int i = 0; i < g_arbOrderMap.values().count(); i++) {
		AddToArbOrderTable(g_arbOrderMap.values()[i]);
		md_InstrumentID.push_back(g_arbOrderMap.values()[i].OrdLegList[0].InstrumentID.toStdString());
		md_InstrumentID.push_back(g_arbOrderMap.values()[i].OrdLegList[1].InstrumentID.toStdString());
	}
	
	/**********�����б�************/
	ui.tableView_ordlist->setModel(modelOrder);//��ģ�͡�
	QStringList liveordHeader;
	liveordHeader << QStr("����ID") << QStr("��Լ") << QStr("����") << QStr("��ƽ") << QStr("����") << QStr("δ�ɽ�") << QStr("������") << QStr("�ҵ�״̬") << QStr("�ɽ�����") << QStr("����ʱ��") << QStr("��ϸ״̬");
	modelOrder->setHorizontalHeaderLabels(liveordHeader);
	modelOrder->setColumnCount(liveordHeader.size()); //��������
	ui.tableView_ordlist->verticalHeader()->hide();//�����кš�
	ui.tableView_ordlist->horizontalHeader()->setStyleSheet("border:1px solid #aaaaaa"); //���ñ�ͷ��ɫ��
	ui.tableView_ordlist->setColumnHidden(0, true); //����ID����ʾ��

	//��䶩���б�ģ�ͣ�
	foreach (Order ord, g_orderMap) {
		RefreshOrdTable(ord.OrderRef, "add");
		md_InstrumentID.push_back(ord.InstrumentID.toStdString());
	}


	/**********����������ϡ��ֲ֡����������еĺ�Լ************/
	md_spi->SubscribeMarketData();

	/*********�������ֵ********/
	QDoubleValidator* doubleValid = new QDoubleValidator(0, 100.0, 3, this);
	ui.lineEdit_open_val->setValidator(doubleValid); //�Ƚ�����ֻ��Ϊ���֡�
	ui.lineEdit_close_val->setValidator(doubleValid); //�Ƚ�����ֻ��Ϊ���֡�
	//�����Լ��
	QRegExpValidator* instValid = new QRegExpValidator(QRegExp("[a-zA-Z0-9]+"), this);
	ui.lineEdit_ins1->setValidator(instValid);
	ui.lineEdit_ins2->setValidator(instValid);
	//У��Ϊ�գ�
	QRegExpValidator* noEmptyValid = new QRegExpValidator(QRegExp(".*\\S+.*"), this);
	ui.lineEdit_arb_name->setValidator(noEmptyValid);
	//���鹫ʽ���ݣ���ֻ����A��B��(),���֣���������ţ�
	QRegExpValidator* formulaValid = new QRegExpValidator(QRegExp("[AB0-9\\(\\)\\+\\-\\*\\/ ]+"), this);
	ui.lineEdit_open_formula->setValidator(formulaValid);
	ui.lineEdit_close_formula->setValidator(formulaValid);

	//����Ҽ��˵���
	ui.tableView_arblist->setContextMenuPolicy(Qt::CustomContextMenu); //��������Ϊ�����ܲ�׽�Ҽ�����¼���
	QAction* delAction = new QAction(this);
	delAction->setText(QStr("ɾ�����"));
	popMenu = new QMenu(ui.tableView_arblist);
	popMenu->addAction(delAction);
	//����Ҽ��˵��¼��󶨡�
	connect(ui.tableView_arblist, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(ShowContextMenu(QPoint)));
	//���ɾ���¼���������
	QObject::connect(delAction, SIGNAL(triggered()), this, SLOT(DeleteArbPortf()));

}


//�¼�����������Ҫ�Ǽ����������Լ���ؼ�һ��������Զ��رգ�
bool MainWindow::eventFilter(QObject *target, QEvent *event) {
	//treeInst�ؼ���ʾ״̬�²�ȥ����
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

//��ʾ��¼���� ��
void MainWindow::ShowLoginForm() {
	cout << "hello" << endl;
	w_login->show();
}

//���غ�Լ
void MainWindow::UpdateInstrument() {
	//strcpy_s(g_chInstrumentID, "rb2101"); //����ʱ����ѯȫ����Լ ����ѯ��ĺ�Լ�����Զ����������ģ��´ζ���ʱ��Ҳ�Ƕ��Ĳ�ѯ����Щ��Լ ��
	spi->ReqQryInstrument();
	WaitForSingleObject(g_qEvent, INFINITE);
	//����Լ����
	FillModelInst(modelIns, g_instMap);
}

//��ʾ��Լ��Ϣ�� 
void MainWindow::ShowInstrument() {
	cout << "�й�";
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
	QTextCodec::setCodecForLocale(QTextCodec::codecForLocale());
	//�������οؼ���
	treeInst = new QTreeView(ui.tabWidget_arb_edit);
	treeInst->setHeaderHidden(true); //����ͷ����
	treeInst->setStyleSheet(QString("background-color:rgb(255, 247, 239);"));
	treeInst->setGeometry(58, 11, 200, 300); //�������οؼ�λ�á�
	//treeInst->setWindowFlag(Qt::WindowCloseButtonHint);
	//treeInst->setWindowFlags(Qt::Popup);
	treeInst->setModel(modelIns);
	treeInst->show();

	//���¼����������������οؼ��ſ��Լ������������ϵĵ���¼���ע��Ҫ��viewport()���У�����Ȼ������ڵ���������οؼ�ʱ���ر��Լ���
	treeInst->viewport()->installEventFilter(this);

	//��ȡ�źŷ��Ϳؼ���
	QPushButton *btn = qobject_cast<QPushButton *>(sender());
	if (btn->objectName() == "pushButton_ins1") {
		setInsEdit = ui.lineEdit_ins1;
	} else if (btn->objectName() == "pushButton_ins2") {
		setInsEdit = ui.lineEdit_ins2;
	}
	QObject::connect(treeInst, SIGNAL(clicked(QModelIndex)), this, SLOT(ChooseInstrument(QModelIndex)));
}


//ѡ���Լ ��
void MainWindow::ChooseInstrument(QModelIndex idx) {
	QStandardItem* item = modelIns->itemFromIndex(idx);
	if (!item->hasChildren()) { //����Լ��ʾ���������С�
		setInsEdit->setText(item->text());
		treeInst->hide();
	}
}


//�����е����ݣ�����������϶���
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




//����������
void MainWindow::AddArbPortf() {
	//У�����
	if (!ui.lineEdit_arb_name->hasAcceptableInput()) {
		QMessageBox::warning(this, QStr("����"), QStr("������Ʋ���Ϊ��"));
		return;
	} else if (!ui.lineEdit_open_val->hasAcceptableInput() || !ui.lineEdit_close_val->hasAcceptableInput()) {
		QMessageBox::warning(this, QStr("����"), QStr("��ʽ��������"));
		return;
	} else if (!ui.lineEdit_ins1->hasAcceptableInput() || !ui.lineEdit_ins2->hasAcceptableInput()) {
		QMessageBox::warning(this, QStr("����"), QStr("��Լ����Ϊ��"));
		return;
	} else if (!g_instMap.contains(ui.lineEdit_ins1->text()) || !g_instMap.contains(ui.lineEdit_ins2->text())) {
		QMessageBox::warning(this, QStr("����"), QStr("��Լ��������"));
		return;
	}

	//ͨ��������̬ʽ���㡢���жϹ�ʽ�Ƿ�����
	QString open_formular = ui.lineEdit_open_formula->text();
	open_formular.replace("A", "1").replace("B", "2");
	QJSValue jval1 = jsEngine->evaluate(open_formular);
	if (jval1.isError()) {
		QMessageBox::warning(this, QStr("����"), QStr("��ʽ����") + jval1.toString());
		return;
	}
	//ͨ��������̬ʽ���㡢���жϹ�ʽ�Ƿ�����
	QString close_formular = ui.lineEdit_close_formula->text();
	close_formular.replace("A", "1").replace("B", "2");
	QJSValue jval2 = jsEngine->evaluate(close_formular);
	if (jval2.isError()) {
		QMessageBox::warning(this, QStr("����"), QStr("��ʽ����") + jval2.toString());
		return;
	}

	//���������������϶���
	ArbPortf item = FillArbPortf();

	//��ӵ��б�
	g_arbPortfList.append(item);
	//����б�ģ�����ݡ�
	AddToArbPortfTable(item);


}

//����Ѿ���ӵ����������
void MainWindow::ClickArbPortf(QModelIndex idx) {
	ArbPortf arb = g_arbPortfList[idx.row()];
	ui.lineEdit_arb_name->setText(arb.Name);
	//��ԼA
	ui.lineEdit_ins1->setText(arb.LegList[0].InstrumentID);
	ui.comboBox_direction1->setCurrentText(arb.LegList[0].Direction);
	ui.spinBox_vol1->setValue(arb.LegList[0].Vol);
	ui.comboBox_ptype1->setCurrentText(arb.LegList[0].PriceType);
	ui.spinBox_slip1->setValue(arb.LegList[0].SlipPoint);
	ui.checkBox_chase1->setChecked(arb.LegList[0].ChasePoint > 0 ? true : false);
	ui.spinBox_chase1_point->setValue(arb.LegList[0].ChasePoint);
	//��ԼB
	ui.lineEdit_ins2->setText(arb.LegList[1].InstrumentID);
	ui.comboBox_direction2->setCurrentText(arb.LegList[1].Direction);
	ui.spinBox_vol2->setValue(arb.LegList[1].Vol);
	ui.comboBox_ptype2->setCurrentText(arb.LegList[1].PriceType);
	ui.spinBox_slip2->setValue(arb.LegList[1].SlipPoint);
	ui.checkBox_chase2->setChecked(arb.LegList[1].ChasePoint > 0 ? true : false);
	ui.spinBox_chase2_point->setValue(arb.LegList[1].ChasePoint);
	//�µ����á�
	ui.comboBox_offset->setCurrentText(arb.Offset);
	ui.comboBox_send_order_type->setCurrentText(arb.SendOrderType);
	//����������
	ui.lineEdit_open_formula->setText(arb.OpenCondFormula);
	ui.comboBox_open_operator->setCurrentText(arb.OpenCondOperator);
	ui.lineEdit_open_val->setText(arb.OpenCondVal);
	//ƽ��������
	ui.lineEdit_close_formula->setText(arb.CloseCondFormula);
	ui.comboBox_close_operator->setCurrentText(arb.CloseCondOperator);
	ui.lineEdit_close_val->setText(arb.CloseCondVal);
	//����������
	ui.spinBox_times->setValue(arb.Times);
	ui.checkBox_loop->setChecked(arb.Loop);
}

//��ʾ�Ҽ��˵���
void MainWindow::ShowContextMenu(QPoint pos) {
	QModelIndex index = ui.tableView_arblist->indexAt(pos);
	if (index.isValid()) {
		popMenu->exec(QCursor::pos()); // �˵����ֵ�λ��Ϊ��ǰ����λ��
	}
}

//ɾ��������ϡ�
void MainWindow::DeleteArbPortf() {
	QModelIndex idx = ui.tableView_arblist->currentIndex();
	int row_num = idx.row();
	modelArbPortf->removeRow(row_num);
	g_arbPortfList.removeAt(row_num);
}
//ɾ������������
void MainWindow::DeleteArbOrder() {
	QModelIndex idx = ui.tableView_arbordlist->currentIndex();

	//��ȡ��������
	QString arbOrderId = modelArbOrder->data(modelArbOrder->index(idx.row(), 0)).toString();

	//ɾ������������
	foreach(QString id, g_arbOrderMap.keys()) {
		if (id == arbOrderId) {
			g_arbOrderMap.remove(id);
			break;
		}
	}
	//ɾ������������ģ�͡�
	for (int i = modelArbOrder->rowCount()-1; i--; i >= 0) {
		if (arbOrderId == modelArbOrder->data(modelArbOrder->index(i, 0)).toString()) {
			modelArbOrder->removeRows(i, 3);
			break;
		}
	}

	//ɾ����ͨ��
	QList<QString> deletedList;
	foreach(QString orderRef, g_orderMap.keys()) {
		if (g_orderMap[orderRef].ArbOrderId == arbOrderId) {
			g_orderMap.remove(orderRef);
			deletedList.append(orderRef);
		}
	}
	//ɾ����ͨ��ģ�͡�
	for (int i = modelOrder->rowCount() - 1; i--; i >= 0) {
		QString orderRef = modelOrder->data(modelOrder->index(i, 0)).toString();
		if(deletedList.contains(orderRef)){
			modelOrder->removeRow(i);
		}
	}

}

//���ֲ�ͬ����ģ����
void MainWindow::RefreshPosTable() {

	//���ֲ��б��б�ģ�ͣ�
	modelPosition->removeRows(0, modelPosition->rowCount()); //����ա�
	for (int i = 0; i < g_posList.count(); i++) {
		CThostFtdcInvestorPositionField item = g_posList[i];
		int row = modelPosition->rowCount();
		double last_price = 0.0;
		double cost = item.OpenCost / (item.Position*g_instMap[item.InstrumentID].VolumeMultiple);
		double profit = (last_price - cost)* item.Position*g_instMap[item.InstrumentID].VolumeMultiple;
		modelPosition->setItem(row, 0, new QStandardItem(QString(item.InstrumentID)));//��Լ
		modelPosition->setItem(row, 1, new QStandardItem(PosDirectionMap[item.PosiDirection])); //��շ���
		modelPosition->setItem(row, 2, new QStandardItem(Num(item.Position))); //�ֲ�
		modelPosition->setItem(row, 3, new QStandardItem(Num(cost))); //�ֲ־���
		modelPosition->setItem(row, 4, new QStandardItem(Num(last_price))); //���¼�
		modelPosition->setItem(row, 5, new QStandardItem(Num(profit))); //��ӯ
		modelPosition->setItem(row, 6, new QStandardItem(Num(item.TodayPosition))); //��ֲ֣�
		modelPosition->setItem(row, 7, new QStandardItem(Num(item.Position - item.TodayPosition))); //��ֲ�
		md_InstrumentID.push_back(item.InstrumentID);
	}
}

//���������ϵ�ģ���С�
void MainWindow::AddToArbPortfTable(ArbPortf item) {
	int row = modelArbPortf->rowCount();
	modelArbPortf->setItem(row, 0, new QStandardItem(item.Name));
	modelArbPortf->setItem(row, 1, new QStandardItem(item.LegList[0].InstrumentID + " / " + item.LegList[1].InstrumentID));
	modelArbPortf->setItem(row, 2, new QStandardItem("N/A"));
	modelArbPortf->setItem(row, 3, new QStandardItem("N/A"));
	md_InstrumentID.push_back(item.LegList[0].InstrumentID.toStdString());
	md_InstrumentID.push_back(item.LegList[1].InstrumentID.toStdString());
}


//�ύ������ ��
void MainWindow::SubmitArbOrder() {
	if (!ui.lineEdit_open_val->hasAcceptableInput() || !ui.lineEdit_close_val->hasAcceptableInput()) {
		QMessageBox::warning(this, "����", "��ʽ��������");
		return;
	} else if (!ui.lineEdit_ins1->hasAcceptableInput() || !ui.lineEdit_ins2->hasAcceptableInput()) {
		QMessageBox::warning(this, "����", "��Լ��������");
		return;
	} else if (!g_instMap.contains(ui.lineEdit_ins1->text()) || !g_instMap.contains(ui.lineEdit_ins2->text())) {
		QMessageBox::warning(this, "����", "��Լ��������");
		return;
	}

	//���������������϶���
	ArbOrder ord = FillArbOrder();
	g_arbOrderMap.insert(ord.Id, ord);
	

	//�½�һ�������������
	QObject::connect(g_orderWorker, SIGNAL(signal_DealArbOrder(QString)), g_orderWorker, SLOT(DealArbOrder(QString)));
	//����������ۺ��������µ��߳�ȥִ�У�
	QThread* m_pThread = new QThread;
	m_pThread->start();
	g_orderWorker->moveToThread(m_pThread);



	AddToArbOrderTable(ord);
}

//�����е����ݣ�����������������
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
	if (ord.Offset == QStr("����")) {
		ord.CondFormula = ui.lineEdit_open_formula->text();
		ord.CondOperator = ui.comboBox_open_operator->currentText();
		ord.CondVal = ui.lineEdit_open_val->text();
	} else if (ord.Offset == QStr("ƽ��")) {
		ord.CondFormula = ui.lineEdit_close_formula->text();
		ord.CondOperator = ui.comboBox_close_operator->currentText();
		ord.CondVal = ui.lineEdit_close_val->text();
	}

	ord.Times = ui.spinBox_times->value();
	ord.Loop = ui.checkBox_loop->isChecked();


	return ord;
}

//������������Ϣ��ӵ�ģ���С�
void MainWindow::AddToArbOrderTable(ArbOrder ord) {

	//�����б�ģ�����ݡ�
	int row = modelArbOrder->rowCount();
	QString title = ord.CondFormula + ord.CondOperator + ord.CondVal + " " + ord.Offset + " " + ord.SendOrderType;

	modelArbOrder->setItem(row, 0, new QStandardItem(ord.Id));
	modelArbOrder->setItem(row, 1, new QStandardItem(title));
	modelArbOrder->setItem(row, 2, new QStandardItem("0/" + Num(ord.Times)));
	modelArbOrder->setItem(row, 3, new QStandardItem(""));

	for (int i = 0; i < ord.OrdLegList.count(); i++) {
		OrdLeg leg = ord.OrdLegList[i];
		QString line = leg.Direction + " " + leg.InstrumentID + Num(leg.Vol) + QStr("�� ") + leg.PriceType + "+" + Num(leg.ChasePoint) + (leg.Chase ? QStr(" ׷��+") + Num(leg.ChasePoint) : QStr(" ��׷��"));
		modelArbOrder->setItem(row + 1 + i, 1, new QStandardItem(line));
		modelArbOrder->setItem(row + 1 + i, 2, new QStandardItem("0/" + Num(leg.Vol)));
	}

	// ���ɾ����Ť��
	QPushButton *delBtn = new QPushButton(QStr("ɾ��"));
	delBtn->setMaximumHeight(20);
	delBtn->setMaximumWidth(100);
	ui.tableView_arbordlist->setIndexWidget(modelArbOrder->index(row, 4), delBtn);
	
	
	
	ui.tableView_arbordlist->
	QObject::connect(delBtn, SIGNAL(clicked()), this, SLOT(DeleteArbOrder())); //���¼���

	//�ϲ���һ�С����һ�С�
	ui.tableView_arbordlist->setSpan(row, 0, ord.OrdLegList.count() + 1, 1);
	ui.tableView_arbordlist->setSpan(row, 4, ord.OrdLegList.count() + 1, 1);
}

//����������ϵļ۸�
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


//���³ֲ�ӯ��(�۸��и���ʱ)
void MainWindow::UpdatePositionProfit(QString instrument_id, double last_price) {
	//���ֲ��б��б�ģ�ͣ� 
	for (int i = 0; i < g_posList.count(); i++) {
		CThostFtdcInvestorPositionField item = g_posList[i];
		if (QString(item.InstrumentID) == instrument_id) {
			double cost = item.OpenCost / (item.Position*g_instMap[item.InstrumentID].VolumeMultiple);
			double profit = (last_price - cost)* item.Position*g_instMap[item.InstrumentID].VolumeMultiple;
			if (item.PosiDirection == THOST_FTDC_PD_Short) profit = -profit;
			modelPosition->setItem(i, 4, new QStandardItem(Num(last_price))); //���¼�
			modelPosition->setItem(i, 5, new QStandardItem(Num(profit))); //��ӯ
		}
	}
}

//������������۲�(�۸��и���ʱ)
void MainWindow::UpdateArbPrice(QString instrument_id, double last_price) {
	//���ֲ��б��б�ģ�ͣ� 
	for (int i = 0; i < g_arbPortfList.count(); i++) {
		ArbPortf item = g_arbPortfList[i];
		if (item.LegList[0].InstrumentID == instrument_id || item.LegList[1].InstrumentID == instrument_id) {
			double price1 = CalArbPortfPrice(item, "buy");
			double price2 = CalArbPortfPrice(item, "sell");
			modelArbPortf->setItem(i, 2, new QStandardItem(Num(price1))); //���¼�
			modelArbPortf->setItem(i, 3, new QStandardItem(Num(price2))); //���¼�
		}
	}
}


//���������µ�ģ����
//liveordHeader << QStr("��Լ") << QStr("����") << QStr("��ƽ") << QStr("����") << QStr("δ�ɽ�") << QStr("������") << QStr("�ҵ�״̬") << QStr("�ɽ�����") << QStr("����ʱ��") << QStr("��ϸ״̬");
void MainWindow::RefreshOrdTable(QString orderRef, QString type) {
	Order ord = g_orderMap[orderRef];
	//����
	if (type == "add") {
		//�ҵ����
		int row = modelOrder->rowCount();
		int col = 0;
		modelOrder->setItem(row, col++, new QStandardItem(ord.OrderRef));//����orderRef
		modelOrder->setItem(row, col++, new QStandardItem(ord.InstrumentID));//��Լ
		modelOrder->setItem(row, col++, new QStandardItem(ord.Direction)); //����
		modelOrder->setItem(row, col++, new QStandardItem(ord.Offset)); //��ƽ
		modelOrder->setItem(row, col++, new QStandardItem(Num(ord.VolumeTotalOriginal))); //����
		modelOrder->setItem(row, col++, new QStandardItem(Num(ord.VolumeTotal))); //δ�ɽ�
		modelOrder->setItem(row, col++, new QStandardItem(Num(ord.Price))); //�����ۣ�
		modelOrder->setItem(row, col++, new QStandardItem(OrdStatusMap[ord.OrderStatus.toLatin1()]));  //�ҵ�״̬��
		modelOrder->setItem(row, col++, new QStandardItem(Num(ord.AvgPrice))); //�ɽ����ۣ�Ҫȥ�ɽ��ر��в��ܸ��¡�
		modelOrder->setItem(row, col++, new QStandardItem(ord.InsertDate + " " + ord.InsertTime)); //����ʱ�䣬
		md_InstrumentID.push_back(ord.InstrumentID.toStdString().c_str());

	} else if (type == "update") { //���³ɽ��������ɾ��ۡ�״̬��
		//���¹ҵ���
		for (int i = 0; i < modelOrder->rowCount(); i++) {
			QString orderRef = modelOrder->itemFromIndex(modelOrder->index(i, 0))->text();
			if (ord.OrderRef == orderRef) {
				modelOrder->setItem(i, 5, new QStandardItem(Num(ord.VolumeTotal))); //δ�ɽ�
				modelOrder->setItem(i, 7, new QStandardItem(OrdStatusMap[ord.OrderStatus.toLatin1()]));  //�ҵ�״̬��
				modelOrder->setItem(i, 8, new QStandardItem(Num(ord.AvgPrice))); //�ɽ����ۣ�Ҫȥ�ɽ��ر��в��ܸ��¡�
				break;
			}
		}

		//������������
		for (int i = 0; i < modelArbOrder->rowCount(); i++) {
			QString arbOrderId = modelArbOrder->itemFromIndex(modelArbOrder->index(i, 0))->text();
			if (arbOrderId == ord.ArbOrderId) { //�ҵ���������
				ArbOrder arbord = g_arbOrderMap[arbOrderId];
				for (int j = 0; j < arbord.OrdLegList.count(); j++) {//�ҵ������ȡ�
					OrdLeg leg = arbord.OrdLegList[j];
					if (leg.OrderRef == orderRef) {
						QString str = Num(leg.DealVol) + "/" + Num(leg.Vol) + " (" + Num(leg.AvgPrice) + ")";
						modelArbOrder->setItem(i + 1 + j, 2, new QStandardItem(str)); //�ɽ��������۸�
						QString statusStr = OrdStatusMap[leg.Status.toLatin1()];
						modelArbOrder->setItem(i + 1 + j, 3, new QStandardItem(statusStr));  //�ҵ�״̬��
					}
				}
				break;
			}
		}
	}

}

