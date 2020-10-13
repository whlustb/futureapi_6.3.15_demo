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

//计算每腿的设置，返回价格。
double CalLegPrice(OrdLeg leg) {
	double price = 0;
	if (!g_depthMap.contains(leg.InstrumentID)) return 0;
	CThostFtdcDepthMarketDataField depth = g_depthMap[leg.InstrumentID];
	if (leg.PriceType == QStr("对盘价")) {
		price = leg.Direction == QStr("买入") ? (depth.AskVolume1 > 0 ? depth.AskPrice1:0):(depth.BidVolume1 > 0 ? depth.BidPrice1 : 0);
	} else if (leg.PriceType == QStr("排队价")) {
		price = leg.Direction == QStr("买入") ? (depth.BidVolume1 > 0 ? depth.BidPrice1:0):(depth.AskVolume1 > 0 ? depth.AskPrice1 : 0);
	} else if (leg.PriceType == QStr("最新价")) {
		price = g_depthMap[leg.InstrumentID].LastPrice;
	} else if (leg.PriceType == QStr("市价")) {
		price = -1;
	}
	return price;
}


//处理本地套利单。
void OrderWorker::DealArbOrder(QString instrument_id, double last_price) {

	for (int i = 0; i < g_arbordList.count(); i++) {
		ArbOrder* arbOrder = &g_arbordList[i];

		bool exist = false;
		for (int j = 0; j < arbOrder->OrdLegList.count(); j++) {
			OrdLeg leg = arbOrder->OrdLegList[j];
			if (leg.InstrumentID == instrument_id) { //是返回合约变化的订单，判断条件是否满足，
				exist = true;
				break;
			}
		}
		if (!exist) continue;

		//只下单，下完策略就不管事了。
		if (arbOrder->SendOrderType == QStr("全部同时")) {
			//计算条件是否满足。
			QList<OrdLeg> legs = arbOrder->OrdLegList;
			QString cond = arbOrder->CondFormula + arbOrder->CondOperator + arbOrder->CondVal;
			double price1 = CalLegPrice(legs[0]);
			double price2 = CalLegPrice(legs[1]);
			if (price1 == 0 || price2 == 0) { //必须2腿都有价格才处理。
				break;
			}
			cond.replace("A", QString::number(price1)).replace("B", QString::number(price2));
			QJSValue condVal = jsEngine->evaluate(cond);
			if (condVal.toBool()) {
				for (int j = 0; j < legs.count(); j++) {
					OrdLeg* leg = &arbOrder->OrdLegList[j]; //注意这里要用指针去引用，否则在后面是修改不了状态的。
					if (legs[0].Status == "") { //未发出、已撤销，全部用0表示。
						//计算下单价格：
						double ord_price = g_depthMap[leg->InstrumentID].LastPrice;
						int ret = spi->ReqOrderInsert_Whl(leg->InstrumentID, leg->Direction, arbOrder->Offset, ord_price, leg->Vol);
						//标记订单已经下发，
						if (ret == 0) { //0，代表成功。 -1，表示网络连接失败；- 2，表示未处理请求超过许可数； - 3，表示每秒发送请求数超过许可数。
							leg->Status = QStr("已发");
						}
					}
				}
			}
		}
		//先手，先手腿先下单，下单N秒后，如果没有成交，再撤。
		else if (arbOrder->SendOrderType.indexOf(QStr("先手"))==0) {
				//计算条件是否满足。
				QList<OrdLeg> legs = arbOrder->OrdLegList;
				QString cond = arbOrder->CondFormula + arbOrder->CondOperator + arbOrder->CondVal;
				double price1 = CalLegPrice(legs[0]);
				double price2 = CalLegPrice(legs[1]);
				if (price1 == 0 || price2 == 0) { //必须2腿都有价格才处理。
					break;
				}
				cond.replace("A", QString::number(price1)).replace("B", QString::number(price2));
				QJSValue condVal = jsEngine->evaluate(cond);
				if (condVal.toBool()) {
					for (int j = 0; j < legs.count(); j++) {
						OrdLeg* leg = &arbOrder->OrdLegList[j]; //注意这里要用指针去引用，否则在后面是修改不了状态的。
						if (legs[0].Status == "") { //未发出、已撤销，全部用0表示。
							//计算下单价格：
							double ord_price = g_depthMap[leg->InstrumentID].LastPrice;
							int ret = spi->ReqOrderInsert_Whl(leg->InstrumentID, leg->Direction, arbOrder->Offset, ord_price, leg->Vol);
							//标记订单已经下发，
							if (ret == 0) { //0，代表成功。 -1，表示网络连接失败；- 2，表示未处理请求超过许可数； - 3，表示每秒发送请求数超过许可数。
								leg->Status = QStr("已发");
							}
						}
					}
				}
			}

		
	}
}
