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


// ����¼������Ƿ���ɵı�־
// Create a manual reset event with no signal 
extern HANDLE g_hEvent;
/// ����������
extern TThostFtdcExchangeIDType g_chExchangeID;
///��Լ����
extern TThostFtdcInstrumentIDType	g_chInstrumentID;
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


//���鴦��Ⱥ��¼���
extern HANDLE g_qEvent;

/************����ȫ��API��SPI����*******************************/
extern CThostFtdcMdApi* md_api; //����API.
extern HandlerQuote* md_spi; //����SPI.
extern CTraderApi* api; //����API
extern HandlerTrade* spi; //����SPI.

/******����ȫ�ֱ�������������CTP������Ϣ��******/
extern QMap<QString, CThostFtdcInstrumentField> g_instMap; //��Լ��Ϣ��
extern QList<CThostFtdcInvestorPositionField> g_posList; //�ֲ���Ϣ��

extern QMap<QString, Order> g_orderMap; //��ͨ���ض�����Ϣ�б�key:���ض������OrderRef.
extern QMap<QString, CThostFtdcOrderField> g_orderUnfilledMap; //����δ�ɽ�������key:����ID, 

extern QMap<QString, CThostFtdcDepthMarketDataField> g_depthMap; //���µĺ�Լ������顣

extern QList<ArbPortf> g_arbPortfList; //��������б�


extern ConfigInfo g_config; //ȫ�����ñ�����
extern QList<FrontServer> g_serverList; //������IP������Ϣ��


extern OrderWorker* g_orderWorker; //ȫ�����ñ�����

extern int g_OrderRef; //���ض�����ţ�һֱ������
extern int g_ArbOrderId; //����������ţ�һֱ������


extern QStandardItemModel *modelIns; //��Լ�б�ģ�͡�
extern QStandardItemModel *modelPosition; //�ֲ�ģ��
extern QStandardItemModel *modelArbPortf; //�������ģ��
extern QStandardItemModel *modelArbOrder; //����������ģ��
extern QStandardItemModel *modelOrder; //��ͨ����ģ�͡�
extern QList<ArbPortf> g_arbPortfList; //��������б�
extern QMap<QString, ArbOrder> g_arbOrderMap; //���������б�

extern QMap<QString, Order> g_orderMap; //��ϵͳ���µ��б�
extern QMap<QString, CThostFtdcOrderField> g_orderUnfilledMap; //����δ�ɽ�������


extern QMap<char, QString> PosDirectionMap;
extern QMap<char, QString> OrdDirectionMap;
extern QMap<char, QString> OrdOffsetMap;
extern QMap<char, QString> OrdStatusMap;

extern ConfigInfo g_config;
extern QList<FrontServer> g_serverList;