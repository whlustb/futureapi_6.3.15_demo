#include "CtpArbFunc.h"
#include <QStandardItem>
#include "functions.h"
#include "HandlerVars.h"


//��Ӻ�Լ����㡣
QStandardItem* addTreeItem(QStandardItem *pitem, char* name) {
	//cout << "�ӽڵ�����" << pitem->rowCount();
	for (int i = 0; i < pitem->rowCount(); i++) {
		if (pitem->child(i, 0)->text() == name) {
			return pitem->child(i, 0); //����ʱ��ֱ�ӷ��ء�
		}
	}
	//������ʱ����ӡ�
	QStandardItem *item = new QStandardItem(name);
	pitem->appendRow(item);
	return item;
}

//����Լ����
void FillModelInst() {
	extern QStandardItemModel* modelIns;
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
