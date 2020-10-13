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
#include "DataCollect.h""

#include <conio.h>
#include "traderApi.h"
#include "define.h"
#include "traderSpi.h"
#include <vector>
#include <map>

//行情类
class HandlerQuote : public CThostFtdcMdSpi
{
private:
	// 指向CThostFtdcMduserApi实例的指针
	CThostFtdcMdApi *m_pUserMdApi;

public:
	// 构造函数，需要一个有效的指向CThostFtdcMduserApi实例的指针
	HandlerQuote(CThostFtdcMdApi *pUserApi) : m_pUserMdApi(pUserApi) {}
	~HandlerQuote() {}
	// 当客户端与交易托管系统建立起通信连接，客户端需要进行登录
	virtual void OnFrontConnected();

	virtual void ReqUserLogin();

	// 当客户端与交易托管系统通信连接断开时，该方法被调用
	virtual void OnFrontDisconnected(int nReason);

	// 当客户端发出登录请求之后，该方法会被调用，通知客户端登录是否成功
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	void SubscribeMarketData();//收行情

	///订阅行情应答
	virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///深度行情通知
	virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);

	///订阅询价请求
	void SubscribeForQuoteRsp();

	///订阅询价应答
	virtual void OnRspSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast);


	///询价通知
	virtual void OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp);


};