#include "HandlerVars.h"

FILE *logfile = fopen("syslog.txt", "w");

// 报单录入操作是否完成的标志
// Create a manual reset event with no signal
HANDLE g_hEvent = CreateEvent(NULL, false, false, NULL);
/// 交易所代码
TThostFtdcExchangeIDType g_chExchangeID;
///合约代码
TThostFtdcInstrumentIDType	g_chInstrumentID;
///预埋撤单编号
TThostFtdcParkedOrderActionIDType	g_chParkedOrderActionID1;
///预埋报单编号
TThostFtdcParkedOrderIDType	g_chParkedOrderID1;
///报单引用
TThostFtdcOrderRefType	g_chOrderRef;
///前置编号
TThostFtdcFrontIDType	g_chFrontID;
///会话编号
TThostFtdcSessionIDType	g_chSessionID;
///报单编号
TThostFtdcOrderSysIDType	g_chOrderSysID;
///止损价
TThostFtdcPriceType	g_chStopPrice;
///报价引用
TThostFtdcOrderRefType	g_chQuoteRef;
int FrontID = 0;
int SessionID = 0;
int Limitprice = 0;
int nRequestID = 0;
int chioce_action = 0;//为0则全部报

vector<string> vector_OrderSysID;
vector<string> vector_ExchangeID;
vector<string> vector_InstrumentID;
vector<string> md_InstrumentID;
int action_number;

///执行宣告引用
TThostFtdcOrderRefType	g_NewExecOrderRef;
///执行宣告编号
TThostFtdcExecOrderSysIDType	g_NewExecOrderSysID;
///前置编号
TThostFtdcFrontIDType	g_NewFrontID;
///会话编号
TThostFtdcSessionIDType	g_NewSessionID;

//期权自对冲响应通知
///期权自对冲编号
TThostFtdcOrderSysIDType	g_chOptionSelfCloseSysID;
///期权自对冲引用
TThostFtdcOrderRefType	g_chOptionSelfCloseRef;

HANDLE g_qEvent = CreateEvent(NULL, false, false, NULL);


/***********定义全局变量API/SPI相关*********/
//定义行情API/SPI
CThostFtdcMdApi* md_api = nullptr;
HandlerQuote* md_spi = nullptr;
//定义交易API/SPI
CTraderApi* api = nullptr;
HandlerTrade* spi = nullptr;

/******定义全局变量、保留部分CTP返回信息，******/
QMap<QString, CThostFtdcInstrumentField> g_instMap;//合约信息。
QList<CThostFtdcInvestorPositionField> g_posList; //持仓信息。
QMap<QString, CThostFtdcDepthMarketDataField> g_depthMap; //最新的合约深度行情。

OrderWorker* g_orderWorker = new OrderWorker(); //定义全局订单处理类。

/******全局变量******/
QStandardItemModel *modelIns = new QStandardItemModel(); //合约列表模型。
QStandardItemModel *modelPosition = new QStandardItemModel(); ; //持仓模型
QStandardItemModel *modelArbPortf = new QStandardItemModel(); ; //套利组合模型
QStandardItemModel *modelArbOrder = new QStandardItemModel(); ; //本地套利单模型
QStandardItemModel *modelOrder = new QStandardItemModel(); ; //普通订单模型。
QList<ArbPortf> g_arbPortfList; //套利组合列表。
QMap<QString, ArbOrder> g_arbOrderMap; //套利订单列表。key:本地套利订单ID.
QMap<QString, Order> g_orderMap; //本系统的下单列表。 key:OrderRef值，
QMap<QString, CThostFtdcOrderField> g_orderUnfilledMap; //线上未成交订单。


QMap<char, QString> PosDirectionMap{ {'1', "NA"},  {'2', QStr("买")},  {'3', QStr("卖")} };
QMap<char, QString> OrdDirectionMap{ {'0', QStr("买")},  {'1', QStr("卖")} };
QMap<char, QString> OrdOffsetMap{ {'0', QStr("开仓")},  {'1', QStr("平仓")},  {'1', QStr("强平")},  {'1', QStr("平今")},  {'1', QStr("平昨")},  {'1', QStr("强减")} ,  {'1', QStr("本地强平")} };
QMap<char, QString> OrdStatusMap{ {'0', QStr("全部成交")},  {'1', QStr("部分成交(队中)")},  {'2', QStr("部分成交(队外)")},  {'3', QStr("排队中")},  {'4', QStr("不在队列中")},  {'5', QStr("撤单")} ,  {'a', QStr("未知")}  ,  {'b', QStr("未触发")},  {'b', QStr("已触发")}  };



//配置对象。
ConfigInfo g_config;
QList<FrontServer> g_serverList;