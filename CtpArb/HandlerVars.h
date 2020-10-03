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


// ����¼������Ƿ���ɵı�־
// Create a manual reset event with no signal
extern HANDLE g_hEvent;
/// ��Ա����
extern TThostFtdcBrokerIDType g_chBrokerID;
/// �����û�����
extern TThostFtdcUserIDType g_chUserID;
/// �����û�����
extern TThostFtdcPasswordType g_chPassword;
/// ����������
extern TThostFtdcExchangeIDType g_chExchangeID;
///��Լ����
extern TThostFtdcInstrumentIDType	g_chInstrumentID;
///Ͷ���ߴ���
extern TThostFtdcInvestorIDType g_chInvestorID;
///Ԥ�񳷵����
extern TThostFtdcParkedOrderActionIDType	g_chParkedOrderActionID1;
///Ԥ�񱨵����
extern TThostFtdcParkedOrderIDType	g_chParkedOrderID1;
///��������
extern TThostFtdcOrderRefType	g_chOrderRef;
///ǰ�ñ��
extern TThostFtdcFrontIDType	g_chFrontID;
///�Ự���
extern TThostFtdcSessionIDType	g_chSessionID;
///�������
extern TThostFtdcOrderSysIDType	g_chOrderSysID;
///ֹ���
extern TThostFtdcPriceType	g_chStopPrice;
///��������
extern TThostFtdcOrderRefType	g_chQuoteRef;
extern int FrontID;
extern int SessionID;
extern int Limitprice;
extern int nRequestID;
extern int chioce_action;//Ϊ0��ȫ����

extern vector<string> vector_OrderSysID;
extern vector<string> vector_ExchangeID;
extern vector<string> vector_InstrumentID;
extern vector<string> md_InstrumentID;
extern int action_number;

///ִ����������
extern TThostFtdcOrderRefType	g_NewExecOrderRef;
///ִ��������
extern TThostFtdcExecOrderSysIDType	g_NewExecOrderSysID;
///ǰ�ñ��
extern TThostFtdcFrontIDType	g_NewFrontID;
///�Ự���
extern TThostFtdcSessionIDType	g_NewSessionID;

//��Ȩ�ԶԳ���Ӧ֪ͨ
///��Ȩ�ԶԳ���
extern TThostFtdcOrderSysIDType	g_chOptionSelfCloseSysID;
///��Ȩ�ԶԳ�����
extern TThostFtdcOrderRefType	g_chOptionSelfCloseRef;
///�û��˲�Ʒ��Ϣ
extern TThostFtdcProductInfoType	g_chUserProductInfo;
///��֤��
extern TThostFtdcAuthCodeType	g_chAuthCode;
///App����
extern TThostFtdcAppIDType	g_chAppID;

extern HANDLE xinhao;

extern CTraderApi *pUserApi;