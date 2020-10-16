#pragma once
#include "HandlerVars.h"
#include "HandlerQuote.h"
#include <QThread>
#include "MainForm.h"

using namespace std;

extern MainWindow* w_main;

// 当客户端与交易托管系统建立起通信连接，客户端需要进行登录
void HandlerQuote::OnFrontConnected()
{
	/*strcpy_s(g_config.server.BrokerID, getConfig("config", "BrokerID").c_str());
	strcpy_s(g_config.UserID, getConfig("config", "UserID").c_str());
	strcpy_s(g_config.Password, getConfig("config", "Password").c_str());*/
	ReqUserLogin();
}

void HandlerQuote::ReqUserLogin()
{
	CThostFtdcReqUserLoginField reqUserLogin;
	int num = m_pUserMdApi->ReqUserLogin(&reqUserLogin, 0);
	LOG("\tlogin num = %d\n", num);
}

// 当客户端与交易托管系统通信连接断开时，该方法被调用
void HandlerQuote::OnFrontDisconnected(int nReason)
{
	// 当发生这个情况后，API会自动重新连接，客户端可不做处理
	LOG("<OnFrontDisconnected>\n");
	LOG("\tnReason= = [%d]", nReason);
	LOG("</OnFrontDisconnected>\n");
}

// 当客户端发出登录请求之后，该方法会被调用，通知客户端登录是否成功
void HandlerQuote::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	LOG("OnRspUserLogin:\n");
	LOG("\tErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID,
		pRspInfo->ErrorMsg);
	LOG("\tRequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);
	if (pRspInfo->ErrorID != 0) {
		// 端登失败，客户端需进行错误处理
		LOG("\tFailed to login, errorcode=%d errormsg=%s requestid=%d chain = %d",
			pRspInfo->ErrorID, pRspInfo->ErrorMsg, nRequestID, bIsLast);
		exit(-1);
	}
	SetEvent(g_qEvent);
	//SubscribeMarketData();//订阅行情
	//SubscribeForQuoteRsp();//询价请求
}

///询价通知
void HandlerQuote::OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp) {
	LOG("<OnRtnForQuoteRsp>\n");
	if (pForQuoteRsp) {
		LOG("\tTradingDay = [%s]\n", pForQuoteRsp->TradingDay);
		LOG("\tInstrumentID = [%s]\n", pForQuoteRsp->InstrumentID);
		LOG("\tForQuoteSysID = [%s]\n", pForQuoteRsp->ForQuoteSysID);
		LOG("\tForQuoteTime = [%s]\n", pForQuoteRsp->ForQuoteTime);
		LOG("\tActionDay = [%s]\n", pForQuoteRsp->ActionDay);
		LOG("\tExchangeID = [%s]\n", pForQuoteRsp->ExchangeID);
	}
	LOG("</OnRtnForQuoteRsp>\n");
	SetEvent(g_qEvent);
}


///订阅询价请求
void HandlerQuote::SubscribeForQuoteRsp()
{
	LOG("行情中订阅询价请求\n");
	char **ppInstrumentID = new char*[50];
	ppInstrumentID[0] = const_cast<char *>(g_chInstrumentID); 
	int result = m_pUserMdApi->SubscribeForQuoteRsp(ppInstrumentID, 1);
}

///订阅询价应答
void HandlerQuote::OnRspSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast)
{
	LOG("<OnRspSubForQuoteRsp>\n");
	if (pSpecificInstrument)
	{
		LOG("\tInstrumentID = [%s]\n", pSpecificInstrument->InstrumentID);
	}
	if (pRspInfo)
	{
		LOG("\tErrorMsg = [%s]\n", pRspInfo->ErrorMsg);
		LOG("\tErrorID = [%d]\n", pRspInfo->ErrorID);
	}
	LOG("\tnRequestID = [%d]\n", nRequestID);
	LOG("\tbIsLast = [%d]\n", bIsLast);
	LOG("</OnRspSubForQuoteRsp>\n");
	SetEvent(g_qEvent);
}



/*********自己修改较多的*********/
//订阅合约行情，
void HandlerQuote::SubscribeMarketData()
{
	int md_num = 0;
	char **ppInstrumentID = nullptr;
	int j = 0;

	for (int i = 0; i < md_InstrumentID.size(); i++) {
		if (i % 500 == 0) {
			if (ppInstrumentID != nullptr) delete[]ppInstrumentID;
			ppInstrumentID = new char*[500];
			j = 0;
		}
		ppInstrumentID[j++] = const_cast<char *>(md_InstrumentID.at(i).c_str());
		//500个，或者是到了最后一个。
		if (i > 0 && i % 500 == 0 || i == md_InstrumentID.size() - 1) {
			int result = m_pUserMdApi->SubscribeMarketData(ppInstrumentID, j);
			LOG((result == 0) ? "订阅行情请求1......发送成功\n" : "订阅行情请求1......发送失败，错误序号=[%d]\n", result);
		}
	}
}

///订阅行情应答
void HandlerQuote::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	LOG("<OnRspSubMarketData>\n");
	if (pSpecificInstrument) {
		LOG("\tInstrumentID = [%s]\n", pSpecificInstrument->InstrumentID);
	}
	if (pRspInfo) {
		LOG("\tErrorMsg = [%s]\n", pRspInfo->ErrorMsg);
		LOG("\tErrorID = [%d]\n", pRspInfo->ErrorID);
	}
	LOG("\tnRequestID = [%d]\n", nRequestID);
	LOG("\tbIsLast = [%d]\n", bIsLast);
	LOG("</OnRspSubMarketData>\n");
}


///深度行情通知
void HandlerQuote::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData) {
	//QThread::msleep(10 * 1000);

	//QDateTime dateTime = QDateTime::currentDateTime();
	//LOG(dateTime.toString("yyyy/mm/dd, hh:mm:ss.zzz").toStdString().c_str());
	//if (true) return;

	//LOG("<OnRtnDepthMarketData>\n");
	if (pDepthMarketData) {
		LOG("\tInstrumentID = [%s] [%.8lf]\n", pDepthMarketData->InstrumentID, pDepthMarketData->LastPrice);

		/*
		LOG("\tInstrumentID = [%s]\n", pDepthMarketData->InstrumentID);
		LOG("\tExchangeID = [%s]\n", pDepthMarketData->ExchangeID);
		LOG("\tLastPrice = [%.8lf]\n", pDepthMarketData->LastPrice);
		LOG("\tPreSettlementPrice = [%.8lf]\n", pDepthMarketData->PreSettlementPrice);
		LOG("\tOpenPrice = [%.8lf]\n", pDepthMarketData->OpenPrice);
		LOG("\tVolume = [%d]\n", pDepthMarketData->Volume);
		LOG("\tTurnover = [%.8lf]\n", pDepthMarketData->Turnover);
		LOG("\tOpenInterest = [%d]\n", pDepthMarketData->OpenInterest);
		*/

		QString inst_id = QString(pDepthMarketData->InstrumentID);
		CThostFtdcDepthMarketDataField old = g_depthMap[inst_id];
		g_depthMap[inst_id] = *pDepthMarketData;
		if (old.LastPrice != pDepthMarketData->LastPrice) { //最新价有变化时，更新界面。
			//通知更新持仓盈亏。
			emit w_main->signal_UpdatePositionProfit(inst_id, pDepthMarketData->LastPrice);

			//通知更新套利组件。
			emit w_main->signal_UpdateArbPrice(inst_id, pDepthMarketData->LastPrice);

			//通知更新
			emit g_orderWorker->signal_DealArbOrder(inst_id);
		}

		//emit w_main->signal_Test(QString(inst_id), pDepthMarketData->LastPrice);
	}
	//LOG("</OnRtnDepthMarketData>\n");

}
