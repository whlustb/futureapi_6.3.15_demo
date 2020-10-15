#include "CtpArbFunc.h"
#include <QStandardItem>
#include "functions.h"
#include "HandlerVars.h"
#include <QMap>
#include <QString>
#include "ThostFtdcUserApiStruct.h"

//��Ӻ�Լ����㡣
QStandardItem* addTreeItem(QStandardItem *pitem, char* name) {
	//cout << "�ӽڵ�����" << pitem->rowCount();
	for (int i = 0; i < pitem->rowCount(); i++) {
		if (pitem->child(i, 0)->text() == name) {
			return pitem->child(i, 0); //����ʱ��ֱ�ӷ��ء�
		}
	}
	//������ʱ����ӡ�
	QStandardItem *item = new QStandardItem(QString(name));
	pitem->appendRow(item);
	return item;
}

//����Լ����
void FillModelInst(QStandardItemModel *modelIns, QMap<QString, CThostFtdcInstrumentField> g_instMap) {
	foreach(CThostFtdcInstrumentField pInstrument, g_instMap.values()) {
		//���ڵ㡣
		QStandardItem *parentItem = modelIns->invisibleRootItem();

		//���ҽ�����
		parentItem = addTreeItem(parentItem, pInstrument.ExchangeID);

		//ʹ������Ӻ�Լ������ƥ���н���Ʒ�֡�
		QString pname = QString(gbk2utf8(pInstrument.InstrumentName));
		pname.replace(QRegExp("\\d+"), "");

		//�ҽ���Ʒ�֣�
		parentItem = addTreeItem(parentItem, const_cast<char *>(pname.toStdString().c_str()));

		//��Ӻ�Լ���ڡ�
		parentItem = addTreeItem(parentItem, pInstrument.InstrumentID);
	}
}

//һ���̰߳�ȫ�Ķ����ŵ���������


//һ���̰߳�ȫ�Ķ����ŵ���������
int g_OrderRef = 0;
QMutex mutex1;
QString NextOrderRef() {
	mutex1.lock();
	g_OrderRef++;
	mutex1.unlock();
	return QString::number(g_OrderRef);
}

//һ���̰߳�ȫ�Ķ����ŵ���������
int g_ArbOrderId = 0;
QMutex mutex2;
QString NextArbOrderId() {
	mutex2.lock();
	g_ArbOrderId++;
	mutex2.unlock();
	return QString::number(g_ArbOrderId);
}


