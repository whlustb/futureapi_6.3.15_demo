#pragma once

#include "stdafx.h"
#include <list>
#include <iostream>
#include <string>
#include <stdio.h>
#include <windows.h>
#include <time.h>
#include "ThostFtdcTraderApi.h"
#include "ThostFtdcMdApi.h"
#include <conio.h>
#include "traderApi.h"
#include "define.h"
#include "traderSpi.h"
#include <vector>
#include <map>
#include <QStandardItemModel>
#include "functions.h"
#include "HandlerQuote.h"
#include "HandlerTrade.h"
#include "CtpArbStruct.h"
#include "OrderWorker.h"
#include "ThostFtdcUserApiStruct.h"

extern FILE *logfile;


// 报单录入操作是否完成的标志
// Create a manual reset event with no signal 
extern HANDLE g_hEvent;
/// 交易所代码
extern TThostFtdcExchangeIDType g_chExchangeID;
///合约代码
extern TThostFtdcInstrumentIDType	g_chInstrumentID;
///预埋撤单编号
extern TThostFtdcParkedOrderActionIDType	g_chParkedOrderActionID1;
///预埋报单编号
extern TThostFtdcParkedOrderIDType	g_chParkedOrderID1;
///报单引用
extern TThostFtdcOrderRefType	g_chOrderRef;
///前置编号
extern TThostFtdcFrontIDType	g_chFrontID;
///会话编号
extern TThostFtdcSessionIDType	g_chSessionID;
///报单编号
extern TThostFtdcOrderSysIDType	g_chOrderSysID;
///止损价
extern TThostFtdcPriceType	g_chStopPrice;
///报价引用
extern TThostFtdcOrderRefType	g_chQuoteRef;
extern int FrontID;
extern int SessionID;
extern int Limitprice;
extern int nRequestID;
extern int chioce_action;//为0则全部报

extern vector<string> vector_OrderSysID;
extern vector<string> vector_ExchangeID;
extern vector<string> vector_InstrumentID;
extern vector<string> md_InstrumentID;
extern int action_number;

///执行宣告引用
extern TThostFtdcOrderRefType	g_NewExecOrderRef;
///执行宣告编号
extern TThostFtdcExecOrderSysIDType	g_NewExecOrderSysID;
///前置编号
extern TThostFtdcFrontIDType	g_NewFrontID;
///会话编号
extern TThostFtdcSessionIDType	g_NewSessionID;

//期权自对冲响应通知
///期权自对冲编号
extern TThostFtdcOrderSysIDType	g_chOptionSelfCloseSysID;
///期权自对冲引用
extern TThostFtdcOrderRefType	g_chOptionSelfCloseRef;


//行情处理等候事件，
extern HANDLE g_qEvent;

/************声明全局API及SPI对象*******************************/
extern CThostFtdcMdApi* md_api; //行情API.
extern HandlerQuote* md_spi; //行情SPI.
extern CTraderApi* api; //交易API
extern HandlerTrade* spi; //交易SPI.

/******声明全局变量、保留部分CTP返回信息，******/
extern QMap<QString, CThostFtdcInstrumentField> g_instMap; //合约信息。
extern QList<CThostFtdcInvestorPositionField> g_posList; //持仓信息。

extern QMap<QString, Order> g_orderMap; //普通本地订单信息列表，key:本地订单编号OrderRef.
extern QMap<QString, CThostFtdcOrderField> g_orderUnfilledMap; //线上未成交订单，key:订单ID, 

extern QMap<QString, CThostFtdcDepthMarketDataField> g_depthMap; //最新的合约深度行情。

extern QList<ArbPortf> g_arbPortfList; //套利组合列表。


extern ConfigInfo g_config; //全局配置变量。
extern QList<FrontServer> g_serverList; //服务器IP配置信息。


extern OrderWorker* g_orderWorker; //全局配置变量。

extern int g_OrderRef; //本地订单编号，一直递增。
extern int g_ArbOrderId; //套利订单编号，一直递增。


extern QStandardItemModel *modelIns; //合约列表模型。
extern QStandardItemModel *modelPosition; //持仓模型
extern QStandardItemModel *modelArbPortf; //套利组合模型
extern QStandardItemModel *modelArbOrder; //本地套利单模型
extern QStandardItemModel *modelOrder; //普通订单模型。
extern QList<ArbPortf> g_arbPortfList; //套利组合列表。
extern QMap<QString, ArbOrder> g_arbOrderMap; //套利订单列表。

extern QMap<QString, Order> g_orderMap; //本系统的下单列表。
extern QMap<QString, CThostFtdcOrderField> g_orderUnfilledMap; //线上未成交订单。


extern QMap<char, QString> PosDirectionMap;
extern QMap<char, QString> OrdDirectionMap;
extern QMap<char, QString> OrdOffsetMap;
extern QMap<char, QString> OrdStatusMap;

extern ConfigInfo g_config;
extern QList<FrontServer> g_serverList;