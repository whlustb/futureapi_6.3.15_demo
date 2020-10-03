#include "CtpArb.h"
#include <iostream>
#include <QTreeView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QTextCodec>

#include "LoginForm.h"

using namespace std;

//添加模型数据。
QStandardItemModel *modelInst = new QStandardItemModel();

CtpArb::CtpArb(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);	

	//ui.comboBox_ins1->setCurrentText("hello");
}

LoginForm* fm;

//显示登录界面 。
void CtpArb::ShowLoginForm()
{
	cout << "hello" << endl;
	fm = new LoginForm();
	(*fm).setWindowModality(Qt::ApplicationModal);
	(*fm).show();
}

QTreeView *treeInst;

//标记当前要设置的是哪个合约。
QLineEdit* setInsEdit;

//显示合约信息。 
void CtpArb::ShowInstrument()
{
	cout << "中国";	
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
	QTextCodec::setCodecForLocale(QTextCodec::codecForLocale());	
	//定义树形控件，
	treeInst = new QTreeView(ui.widget_work);
	treeInst->setHeaderHidden(true); //隐藏头部。
	treeInst->setStyleSheet(QString("background-color:rgb(255, 247, 239);"));
	   
	//设置树形控件位置。
	treeInst->setGeometry(58, 11, 200, 300);
	
	ui.treeView->setModel(modelInst);
	treeInst->setModel(modelInst);
	treeInst->show();

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
	QStandardItem* item = modelInst->itemFromIndex(idx);
	if (!item->hasChildren()) { //将合约显示在下拉框中。
		setInsEdit->setText(item->text());
		treeInst->hide();
	}
}
