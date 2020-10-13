#include "OrderWorker.h"
#include "QThread"
#include "QDebug"
#include "HandlerVars.h"
#include <QJSValue>
#include <QJSEngine>
#include <QMessageBox>

extern QJSEngine* jsEngine;

OrderWorker::OrderWorker()
{
}
OrderWorker::OrderWorker(QObject *parent)
	: QObject(parent)
{
}

OrderWorker::~OrderWorker()
{
}

//����ÿ�ȵ����ã����ؼ۸�
double CalLegPrice(OrdLeg leg) {
	double price = 0;
	if (!g_depthMap.contains(leg.InstrumentID)) return 0;
	CThostFtdcDepthMarketDataField depth = g_depthMap[leg.InstrumentID];
	if (leg.PriceType == QStr("���̼�")) {
		price = leg.Direction == QStr("����") ? (depth.AskVolume1 > 0 ? depth.AskPrice1:0):(depth.BidVolume1 > 0 ? depth.BidPrice1 : 0);
	} else if (leg.PriceType == QStr("�ŶӼ�")) {
		price = leg.Direction == QStr("����") ? (depth.BidVolume1 > 0 ? depth.BidPrice1:0):(depth.AskVolume1 > 0 ? depth.AskPrice1 : 0);
	} else if (leg.PriceType == QStr("���¼�")) {
		price = g_depthMap[leg.InstrumentID].LastPrice;
	} else if (leg.PriceType == QStr("�м�")) {
		price = -1;
	}
	return price;
}


//��������������
void OrderWorker::DealArbOrder(QString instrument_id, double last_price) {

	for (int i = 0; i < g_arbordList.count(); i++) {
		ArbOrder* arbOrder = &g_arbordList[i];

		bool exist = false;
		for (int j = 0; j < arbOrder->OrdLegList.count(); j++) {
			OrdLeg leg = arbOrder->OrdLegList[j];
			if (leg.InstrumentID == instrument_id) { //�Ƿ��غ�Լ�仯�Ķ������ж������Ƿ����㣬
				exist = true;
				break;
			}
		}
		if (!exist) continue;

		//ֻ�µ���������ԾͲ������ˡ�
		if (arbOrder->SendOrderType == QStr("ȫ��ͬʱ")) {
			//���������Ƿ����㡣
			QList<OrdLeg> legs = arbOrder->OrdLegList;
			QString cond = arbOrder->CondFormula + arbOrder->CondOperator + arbOrder->CondVal;
			double price1 = CalLegPrice(legs[0]);
			double price2 = CalLegPrice(legs[1]);
			if (price1 == 0 || price2 == 0) { //����2�ȶ��м۸�Ŵ���
				break;
			}
			cond.replace("A", QString::number(price1)).replace("B", QString::number(price2));
			QJSValue condVal = jsEngine->evaluate(cond);
			if (condVal.toBool()) {
				for (int j = 0; j < legs.count(); j++) {
					OrdLeg* leg = &arbOrder->OrdLegList[j]; //ע������Ҫ��ָ��ȥ���ã������ں������޸Ĳ���״̬�ġ�
					if (legs[0].Status == "") { //δ�������ѳ�����ȫ����0��ʾ��
						//�����µ��۸�
						double ord_price = g_depthMap[leg->InstrumentID].LastPrice;
						int ret = spi->ReqOrderInsert_Whl(leg->InstrumentID, leg->Direction, arbOrder->Offset, ord_price, leg->Vol);
						//��Ƕ����Ѿ��·���
						if (ret == 0) { //0������ɹ��� -1����ʾ��������ʧ�ܣ�- 2����ʾδ�������󳬹�������� - 3����ʾÿ�뷢�������������������
							leg->Status = QStr("�ѷ�");
						}
					}
				}
			}
		}
		//���֣����������µ����µ�N������û�гɽ����ٳ���
		else if (arbOrder->SendOrderType.indexOf(QStr("����"))==0) {
				//���������Ƿ����㡣
				QList<OrdLeg> legs = arbOrder->OrdLegList;
				QString cond = arbOrder->CondFormula + arbOrder->CondOperator + arbOrder->CondVal;
				double price1 = CalLegPrice(legs[0]);
				double price2 = CalLegPrice(legs[1]);
				if (price1 == 0 || price2 == 0) { //����2�ȶ��м۸�Ŵ���
					break;
				}
				cond.replace("A", QString::number(price1)).replace("B", QString::number(price2));
				QJSValue condVal = jsEngine->evaluate(cond);
				if (condVal.toBool()) {
					for (int j = 0; j < legs.count(); j++) {
						OrdLeg* leg = &arbOrder->OrdLegList[j]; //ע������Ҫ��ָ��ȥ���ã������ں������޸Ĳ���״̬�ġ�
						if (legs[0].Status == "") { //δ�������ѳ�����ȫ����0��ʾ��
							//�����µ��۸�
							double ord_price = g_depthMap[leg->InstrumentID].LastPrice;
							int ret = spi->ReqOrderInsert_Whl(leg->InstrumentID, leg->Direction, arbOrder->Offset, ord_price, leg->Vol);
							//��Ƕ����Ѿ��·���
							if (ret == 0) { //0������ɹ��� -1����ʾ��������ʧ�ܣ�- 2����ʾδ�������󳬹�������� - 3����ʾÿ�뷢�������������������
								leg->Status = QStr("�ѷ�");
							}
						}
					}
				}
			}

		
	}
}
