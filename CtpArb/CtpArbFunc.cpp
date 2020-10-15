#include "CtpArbFunc.h"
#include <QStandardItem>
#include "functions.h"
#include "HandlerVars.h"
#include <QMap>
#include <QString>
#include "ThostFtdcUserApiStruct.h"

//添加合约树结点。
QStandardItem* addTreeItem(QStandardItem *pitem, char* name) {
	//cout << "子节点数量" << pitem->rowCount();
	for (int i = 0; i < pitem->rowCount(); i++) {
		if (pitem->child(i, 0)->text() == name) {
			return pitem->child(i, 0); //存在时，直接返回。
		}
	}
	//不存在时，添加。
	QStandardItem *item = new QStandardItem(QString(name));
	pitem->appendRow(item);
	return item;
}

//填充合约树。
void FillModelInst(QStandardItemModel *modelIns, QMap<QString, CThostFtdcInstrumentField> g_instMap) {
	foreach(CThostFtdcInstrumentField pInstrument, g_instMap.values()) {
		//根节点。
		QStandardItem *parentItem = modelIns->invisibleRootItem();

		//先找交易所
		parentItem = addTreeItem(parentItem, pInstrument.ExchangeID);

		//使用正则从合约名称中匹配中交易品种。
		QString pname = QString(gbk2utf8(pInstrument.InstrumentName));
		pname.replace(QRegExp("\\d+"), "");

		//找交易品种，
		parentItem = addTreeItem(parentItem, const_cast<char *>(pname.toStdString().c_str()));

		//添加合约日期。
		parentItem = addTreeItem(parentItem, pInstrument.InstrumentID);
	}
}

//一个线程安全的订单号递增函数。


//一个线程安全的订单号递增函数。
int g_OrderRef = 0;
QMutex mutex1;
QString NextOrderRef() {
	mutex1.lock();
	g_OrderRef++;
	mutex1.unlock();
	return QString::number(g_OrderRef);
}

//一个线程安全的订单号递增函数。
int g_ArbOrderId = 0;
QMutex mutex2;
QString NextArbOrderId() {
	mutex2.lock();
	g_ArbOrderId++;
	mutex2.unlock();
	return QString::number(g_ArbOrderId);
}


