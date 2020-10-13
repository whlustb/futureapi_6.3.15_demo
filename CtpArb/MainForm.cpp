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


using namespace std;

/******ȫ�ֱ���******/
QStandardItemModel *modelIns = new QStandardItemModel(); //��Լ�б�ģ�͡�
QStandardItemModel *modelArb = new QStandardItemModel(); ; //��������б�ģ��
QStandardItemModel *modelArbOrd = new QStandardItemModel(); ; //�����������б�ģ��
QStandardItemModel *modelPos = new QStandardItemModel(); ; //�ֲ��б�ģ��
QStandardItemModel *modelLiveOrd = new QStandardItemModel(); ; //����δ�ɽ�����ģ�͡�
QList<ArbPortf> g_arbList; //��������б�
QList<ArbOrder> g_arbordList; //�����б�
QMap<QString, CThostFtdcOrderField> g_liveordMap; //����δ�ɽ������б�


QTreeView *treeInst; //��Լ����
QLineEdit* setInsEdit; //��ǵ�ǰҪ���õ����ĸ���Լ��
QMenu* popMenu; //��������ϵ��Ҽ��˵���

QMap<char,QString> PosDirectionMap{{'1', "NA"},  {'2', QStr("��")},  {'3', QStr("��")}};
QMap<char, QString> OrdDirectionMap{ {'0', QStr("��")},  {'1', QStr("��")}};
QMap<char, QString> OrdOffsetMap{ {'0', QStr("����")},  {'1', QStr("ƽ��")},  {'1', QStr("ǿƽ")},  {'1', QStr("ƽ��")},  {'1', QStr("ƽ��")},  {'1', QStr("ǿ��")} ,  {'1', QStr("����ǿƽ")} };
QMap<char, QString> OrdStatusMap{ {'0', QStr("����")},  {'1', QStr("ƽ��")},  {'1', QStr("ǿƽ")},  {'1', QStr("ƽ��")},  {'1', QStr("ƽ��")},  {'1', QStr("ǿ��")} ,  {'1', QStr("����ǿƽ")} };

extern LoginForm* w_login;

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
	//�����Լ��Ϣ��
	QFile file("instmap.dat");
	file.open(QIODevice::WriteOnly);
	QDataStream stream(&file);
	stream << g_instMap;
	file.close();

	//������ʷ��������б�(�����л�)
	QFile file1("arblist.dat");
	file1.open(QIODevice::WriteOnly);
	QDataStream stream1(&file1);
	stream1 << g_arbList;

	//������ʷ���������б�(�����л�)
	QFile file2("ordlist.dat");
	file2.open(QIODevice::WriteOnly);
	QDataStream stream2(&file2);
	stream2 << g_arbordList;

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
	QObject::connect(this, SIGNAL(triggered()), this, SLOT(ShowLoginForm()));
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
	QObject::connect(this, SIGNAL(signal_RefreshModelPosTable()), this, SLOT(RefreshModelPosTable()));
	//δ�ɽ������б����
	QObject::connect(this, SIGNAL(signal_RefreshModelLiveOrdTable()), this, SLOT(RefreshModelLiveOrdTable()));
	//�����ɽ����۸��¡�
	QObject::connect(this, SIGNAL(signal_UpdateOrdDealPrice(QString oid)), this, SLOT(UpdateOrdDealPrice(QString oid)));
	//������ϼ۲����
	QObject::connect(this, SIGNAL(signal_UpdateArbPrice(QString, double)), this, SLOT(UpdateArbPrice(QString, double)));

	//����������������
	QObject::connect(g_orderWorker, SIGNAL(signal_DealArbOrder(QString, double)), g_orderWorker, SLOT(DealArbOrder(QString, double)));
	//����������ۺ��������µ��߳�ȥִ�У�
	QThread* m_pThread = new QThread;
	m_pThread->start();
	g_orderWorker->moveToThread(m_pThread);

	//��Լ�б������л���
	QFile file("instmap.dat");
	file.open(QIODevice::ReadOnly);
	QDataStream stream(&file);
	stream >> g_instMap;
	//���ģ�͡�
	FillModelInst();

	/*******��������б�*******/
	//��ģ�͡�
	ui.tableView_arblist->setModel(modelArb);
	//���ñ�ͷ
	QStringList arbHeader;
	arbHeader << QStr("����") << QStr("��Լ") << QStr("��") << QStr("��");
	modelArb->setHorizontalHeaderLabels(arbHeader);
	modelArb->setColumnCount(arbHeader.size()); //��������
	ui.tableView_arblist->verticalHeader()->hide();//�����кš�
	ui.tableView_arblist->horizontalHeader()->setStyleSheet("border:1px solid #aaaaaa"); //���ñ�ͷ��ɫ��
	//��ȡ��ʷ��������б�(�����л�)
	QFile file1("arblist.dat");
	file1.open(QIODevice::ReadOnly);
	QDataStream stream1(&file1);
	stream1 >> g_arbList;
	//�����������б�ģ�ͣ�
	for (int i = 0; i < g_arbList.count(); i++) {
		ArbPortf item = g_arbList[i];
		AddToModelArbTable(item);
	}

	/*******�����б�*******/
	//��ģ�͡�
	ui.tableView_arbordlist->setModel(modelArbOrd);
	//���ñ�ͷ
	QStringList ordHeader;
	ordHeader << QStr("�������") << QStr("����") << QStr("״̬") << QStr("����");
	modelArbOrd->setHorizontalHeaderLabels(ordHeader);
	modelArbOrd->setColumnCount(ordHeader.size());//��������
	ui.tableView_arbordlist->verticalHeader()->hide();//�����кš�
	ui.tableView_arbordlist->horizontalHeader()->setStyleSheet("border:1px solid #aaaaaa");//���ñ�ͷ��ɫ��
	//ui.tableView_arbordlist->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents); //�����Զ��п�
	ui.tableView_arbordlist->setColumnWidth(1, 300); //�����п�
	ui.tableView_arbordlist->horizontalHeader()->setStretchLastSection(true); //�������һ��stretch.
	//��ȡ��ʷ��������б�(�����л�)
	QFile file2("ordlist.dat");
	file2.open(QIODevice::ReadOnly);
	QDataStream stream2(&file2);
	QStandardItemModel it;
	stream2 >> g_arbordList;
	//��䶩���б�ģ�ͣ�
	for (int i = 0; i < g_arbordList.count(); i++) {
		AddToModelOrderTable(g_arbordList[i]);
		md_InstrumentID.push_back(g_arbordList[i].OrdLegList[0].InstrumentID.toStdString());
		md_InstrumentID.push_back(g_arbordList[i].OrdLegList[1].InstrumentID.toStdString());
	}

	/************�ֲ��б�***********************/
	//��ģ�͡�
	ui.tableView_poslist->setModel(modelPos);
	//���ñ�ͷ
	QStringList posHeader;
	posHeader << QStr("��Լ") << QStr("����") << QStr("�ֲ�") << QStr("�ֲ־���") << QStr("���¼�") << QStr("����ӯ��") << QStr("��ֲ�") << QStr("��ֲ�");
	modelPos->setHorizontalHeaderLabels(posHeader);
	modelPos->setColumnCount(posHeader.size()); //��������
	ui.tableView_poslist->verticalHeader()->hide();//�����кš�
	ui.tableView_poslist->horizontalHeader()->setStyleSheet("border:1px solid #aaaaaa"); //���ñ�ͷ��ɫ��
	//��ȡ�ֲ���Ϣ��
	spi->ReqQryInvestorPosition();
	WaitForSingleObject(g_qEvent, INFINITE);


	/************δ�ɽ������б�***********************/
	//��ģ�͡�
	ui.tableView_liveordlist->setModel(modelLiveOrd);
	//���ñ�ͷ
	QStringList liveordHeader;
	liveordHeader << QStr("����ID") << QStr("��Լ") << QStr("����") << QStr("��ƽ") << QStr("����") << QStr("δ�ɽ�") << QStr("������") << QStr("�ҵ�״̬") << QStr("�ɽ�����") << QStr("����ʱ��") << QStr("��ϸ״̬");
	modelLiveOrd->setHorizontalHeaderLabels(liveordHeader);
	modelLiveOrd->setColumnCount(liveordHeader.size()); //��������
	ui.tableView_liveordlist->verticalHeader()->hide();//�����кš�
	ui.tableView_liveordlist->horizontalHeader()->setStyleSheet("border:1px solid #aaaaaa"); //���ñ�ͷ��ɫ��
	ui.tableView_liveordlist->setColumnHidden(0, true); //����ID����ʾ��
	//��ȡδ�ɽ�������Ϣ��
	spi->ReqQryOrder();
	WaitForSingleObject(g_qEvent, INFINITE);

	//����������ϡ��ֲ֡����������еĺ�Լ
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
	FillModelInst();
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
	return item;
}

//�����е����ݣ�����������������
ArbOrder MainWindow::FillArbOrder() {
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
	if (item.Offset == QStr("����")) {
		item.CondFormula = ui.lineEdit_open_formula->text();
		item.CondOperator = ui.comboBox_open_operator->currentText();
		item.CondVal = ui.lineEdit_open_val->text();
	} else if (item.Offset == QStr("ƽ��")) {
		item.CondFormula = ui.lineEdit_close_formula->text();
		item.CondOperator = ui.comboBox_close_operator->currentText();
		item.CondVal = ui.lineEdit_close_val->text();
	}

	item.Times = ui.spinBox_times->value();
	item.Loop = ui.checkBox_loop->isChecked();
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
		QMessageBox::warning(this, QStr("����"), QStr("��ʽ����") + jval1.toString() );
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
	g_arbList.append(item);
	//����б�ģ�����ݡ�
	AddToModelArbTable(item);


}

//����Ѿ���ӵ����������
void MainWindow::ClickArbPortf(QModelIndex idx) {
	ArbPortf arb = g_arbList[idx.row()];
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
	modelArb->removeRow(row_num);
	g_arbList.removeAt(row_num);
}
//ɾ������������
void MainWindow::DeleteArbOrder() {
	QModelIndex idx = ui.tableView_arbordlist->currentIndex();

	//ɾ�������б�
	int oid = modelArbOrd->data(modelArbOrd->index(idx.row(), 0)).toInt();
	for (int i = 0; i < g_arbordList.count(); i++) {
		if (g_arbordList[i].Id == oid) {
			g_arbordList.removeAt(i);
			break;
		}
	}

	//ע����η����棬����������Ϊ��������õ���modelOrd
	//ɾ����ģ���ж�Ӧ��3�С�
	modelArbOrd->removeRows(idx.row(), 3);
}

//���ֲ�ͬ����ģ����
void MainWindow::RefreshModelPosTable() {
	//���ֲ��б��б�ģ�ͣ�
	modelPos->removeRows(0, modelPos->rowCount()); //����ա�
	for (int i = 0; i < g_posList.count(); i++) {
		CThostFtdcInvestorPositionField item = g_posList[i];
		int row = modelPos->rowCount();
		double last_price = 0.0;
		double cost = item.OpenCost / (item.Position*g_instMap[item.InstrumentID].VolumeMultiple);
		double profit = (last_price - cost)* item.Position*g_instMap[item.InstrumentID].VolumeMultiple;
		modelPos->setItem(row, 0, new QStandardItem(QString(item.InstrumentID)));//��Լ
		modelPos->setItem(row, 1, new QStandardItem(PosDirectionMap[item.PosiDirection])); //��շ���
		modelPos->setItem(row, 2, new QStandardItem(QString::number(item.Position))); //�ֲ�
		modelPos->setItem(row, 3, new QStandardItem(QString::number(cost))); //�ֲ־���
		modelPos->setItem(row, 4, new QStandardItem(QString::number(last_price))); //���¼�
		modelPos->setItem(row, 5, new QStandardItem(QString::number(profit))); //��ӯ
		modelPos->setItem(row, 6, new QStandardItem(QString::number(item.TodayPosition))); //��ֲ֣�
		modelPos->setItem(row, 7, new QStandardItem(QString::number(item.Position - item.TodayPosition))); //��ֲ�
		md_InstrumentID.push_back(item.InstrumentID);
	}
}

//���������ϵ�ģ���С�
void MainWindow::AddToModelArbTable(ArbPortf item) {
	int row = modelArb->rowCount();
	modelArb->setItem(row, 0, new QStandardItem(item.Name));
	modelArb->setItem(row, 1, new QStandardItem(item.LegList[0].InstrumentID + " / " + item.LegList[1].InstrumentID));
	modelArb->setItem(row, 2, new QStandardItem("N/A"));
	modelArb->setItem(row, 3, new QStandardItem("N/A"));
	md_InstrumentID.push_back(item.LegList[0].InstrumentID.toStdString());
	md_InstrumentID.push_back(item.LegList[1].InstrumentID.toStdString());
}

//��������Ϣ��ӵ�ģ���С�
void MainWindow::AddToModelOrderTable(ArbOrder ord) {
	//�����б�ģ�����ݡ�
	int row = modelArbOrd->rowCount();

	QString title = ord.CondFormula + ord.CondOperator + ord.CondVal;
	title += " " + ord.Offset + " "+ ord.SendOrderType;

	modelArbOrd->setItem(row, 0, new QStandardItem(QString::number(ord.Id)));
	modelArbOrd->setItem(row, 1, new QStandardItem(title));
	modelArbOrd->setItem(row, 2, new QStandardItem("0/" + QString::number(ord.Times)));
	//modelOrderTable->setItem(row, 3, new QStandardItem("N/A"));

	for (int i = 0; i < ord.OrdLegList.count(); i++) {
		ArbLeg leg = ord.OrdLegList[i];
		QString line = leg.Direction + " " + leg.InstrumentID + QString::number(leg.Vol) + QStr("�� ") + leg.PriceType + "+" + QString::number(leg.ChasePoint) + (leg.Chase ? QStr(" ׷��+") + QString::number(leg.ChasePoint) : QStr(" ��׷��"));
		modelArbOrd->setItem(row + 1 + i, 1, new QStandardItem(line));
		modelArbOrd->setItem(row + 1 + i, 2, new QStandardItem("0/" + QString::number(leg.Vol)));
	}

	// ���ɾ����Ť��
	QPushButton *delBtn = new QPushButton(QStr("ɾ��"));
	delBtn->setMaximumHeight(20);
	ui.tableView_arbordlist->setIndexWidget(modelArbOrd->index(row, 3), delBtn);
	QObject::connect(delBtn, SIGNAL(clicked()), this, SLOT(DeleteArbOrder())); //���¼���

	//�ϲ���һ�С����һ�С�
	ui.tableView_arbordlist->setSpan(row, 0, ord.OrdLegList.count() + 1, 1);
	ui.tableView_arbordlist->setSpan(row, 3, ord.OrdLegList.count() + 1, 1);
}

//��ȡ���Ķ����š�
int MainWindow::getNewOrdId() {
	int id = 0;
	for (int i = 0; i < g_arbordList.count(); i++) {
		if (g_arbordList[i].Id > id) {
			id = g_arbordList[i].Id;
		}
	}
	return id + 1;
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
	ord.Id = getNewOrdId();
	ord.Time = QTime::currentTime();
	g_arbordList.append(ord);

	//��ӵ�ģ�͡�
	AddToModelOrderTable(ord);

}

//����������ϵļ۸�
double CalArbPortfPrice(ArbPortf item, QString side) {
	if (!g_depthMap.contains(item.LegList[0].InstrumentID) || !g_depthMap.contains(item.LegList[1].InstrumentID)) return 0;

	QString open_formular = item.OpenCondFormula;
	CThostFtdcDepthMarketDataField depth1 = g_depthMap[item.LegList[0].InstrumentID];
	CThostFtdcDepthMarketDataField depth2 = g_depthMap[item.LegList[1].InstrumentID];
	if (side == "buy") {
		if (depth1.AskVolume1 <= 0 || depth2.BidVolume1 <= 0) return NULL;
		open_formular.replace("A", QString::number(depth1.AskPrice1)).replace("B", QString::number(depth2.BidPrice1));
	} else if (side == "sell") {
		if (depth1.BidVolume1 <= 0 || depth2.AskVolume1 <= 0) return NULL;
		open_formular.replace("A", QString::number(depth1.BidPrice1)).replace("B", QString::number(depth2.AskPrice1));
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
			modelPos->setItem(i, 4, new QStandardItem(QString::number(last_price))); //���¼�
			modelPos->setItem(i, 5, new QStandardItem(QString::number(profit))); //��ӯ
			
		}
	}
}

//������������۲�(�۸��и���ʱ)
void MainWindow::UpdateArbPrice(QString instrument_id, double last_price) {
	//���ֲ��б��б�ģ�ͣ� 
	for (int i = 0; i < g_arbList.count(); i++) {
		ArbPortf item = g_arbList[i];
		if (item.LegList[0].InstrumentID == instrument_id || item.LegList[1].InstrumentID == instrument_id) {
			double price1 = CalArbPortfPrice(item, "buy");
			double price2 = CalArbPortfPrice(item, "sell");
			modelArb->setItem(i, 2, new QStandardItem(QString::number(price1))); //���¼�
			modelArb->setItem(i, 3, new QStandardItem(QString::number(price2))); //���¼�
		}
	}
}


//��δ�ɽ��������µ�ģ����
//liveordHeader << QStr("��Լ") << QStr("����") << QStr("��ƽ") << QStr("����") << QStr("δ�ɽ�") << QStr("������") << QStr("�ҵ�״̬") << QStr("�ɽ�����") << QStr("����ʱ��") << QStr("��ϸ״̬");
void MainWindow::RefreshModelLiveOrdTable() {
	//���ֲ��б��б�ģ�ͣ�
	modelLiveOrd->removeRows(0, modelLiveOrd->rowCount()); //����ա�
	for (int i = 0; i < g_liveordMap.values().count(); i++) {
		CThostFtdcOrderField item = g_liveordMap.values()[i];
		int row = modelLiveOrd->rowCount();
		double last_price = 0.0;
		int col = 0;
		modelLiveOrd->setItem(row, col++, new QStandardItem(QString(item.OrderSysID)));//����ID.
		modelLiveOrd->setItem(row, col++, new QStandardItem(QString(item.InstrumentID)));//��Լ
		modelLiveOrd->setItem(row, col++, new QStandardItem(OrdDirectionMap[item.Direction])); //����
		modelLiveOrd->setItem(row, col++, new QStandardItem(OrdOffsetMap[item.CombOffsetFlag[0]])); //��ƽ
		modelLiveOrd->setItem(row, col++, new QStandardItem(QString::number(item.VolumeTotalOriginal))); //����
		modelLiveOrd->setItem(row, col++, new QStandardItem(QString::number(item.VolumeTotal))); //δ�ɽ�
		modelLiveOrd->setItem(row, col++, new QStandardItem(QString::number(item.LimitPrice))); //�����ۣ�
		modelLiveOrd->setItem(row, col++, new QStandardItem(QStr(item.StatusMsg)));  //�ҵ�״̬��
		modelLiveOrd->setItem(row, col++, new QStandardItem("")); //�ɽ����ۣ�Ҫȥ�ɽ��ر��в��ܸ��¡�
		modelLiveOrd->setItem(row, col++, new QStandardItem(QString(item.InsertTime))); //����ʱ�䣬

		md_InstrumentID.push_back(item.InstrumentID);
	}
}


//���³ɽ����ۡ�
void MainWindow::UpdateOrdDealPrice(QString oid, double price, int vol) {
	CThostFtdcOrderField ord = g_liveordMap[oid];
	for (int i = 0; i<modelLiveOrd->rowCount(); i++) {
		QString id = modelLiveOrd->itemFromIndex(modelLiveOrd->index(i, 9))->text();
		if (id == oid) {
			double price_old = modelLiveOrd->data(modelLiveOrd->index(i, 7)).Double; //�Ѿ��ɽ�����
			int vol_old = modelLiveOrd->data(modelLiveOrd->index(i, 3)).Int - modelLiveOrd->data(modelLiveOrd->index(i, 4)).Int; //�Ѿ��ɽ�������
			double avg_price = (price_old * vol_old + price * vol) / (vol_old + vol);
			modelLiveOrd->setData(modelLiveOrd->index(i, 7), QString::number(avg_price));
		}
	}

}

