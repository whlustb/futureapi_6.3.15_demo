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
#include "getconfig.h"
#include "traderApi.h"
#include "define.h"
#include "traderSpi.h"
#include <vector>
#include <map>
#include <QStandardItemModel>

#include "functions.h"

extern FILE *logfile;


// 报单录入操作是否完成的标志
// Create a manual reset event with no signal
extern HANDLE g_hEvent;
/// 会员代码
extern TThostFtdcBrokerIDType g_chBrokerID;
/// 交易用户代码
extern TThostFtdcUserIDType g_chUserID;
/// 交易用户密码
extern TThostFtdcPasswordType g_chPassword;
/// 交易所代码
extern TThostFtdcExchangeIDType g_chExchangeID;
///合约代码
extern TThostFtdcInstrumentIDType	g_chInstrumentID;
///投资者代码
extern TThostFtdcInvestorIDType g_chInvestorID;
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
///用户端产品信息
extern TThostFtdcProductInfoType	g_chUserProductInfo;
///认证码
extern TThostFtdcAuthCodeType	g_chAuthCode;
///App代码
extern TThostFtdcAppIDType	g_chAppID;

extern HANDLE xinhao;

extern CTraderApi *pUserApi;