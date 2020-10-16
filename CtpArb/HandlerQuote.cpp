#pragma once
#include "HandlerVars.h"
#include "HandlerQuote.h"
#include <QThread>
#include "MainForm.h"

using namespace std;

extern MainWindow* w_main;

// ���ͻ����뽻���й�ϵͳ������ͨ�����ӣ��ͻ�����Ҫ���е�¼
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

// ���ͻ����뽻���й�ϵͳͨ�����ӶϿ�ʱ���÷���������
void HandlerQuote::OnFrontDisconnected(int nReason)
{
	// ��������������API���Զ��������ӣ��ͻ��˿ɲ�������
	LOG("<OnFrontDisconnected>\n");
	LOG("\tnReason= = [%d]", nReason);
	LOG("</OnFrontDisconnected>\n");
}

// ���ͻ��˷�����¼����֮�󣬸÷����ᱻ���ã�֪ͨ�ͻ��˵�¼�Ƿ�ɹ�
void HandlerQuote::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	LOG("OnRspUserLogin:\n");
	LOG("\tErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID,
		pRspInfo->ErrorMsg);
	LOG("\tRequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);
	if (pRspInfo->ErrorID != 0) {
		// �˵�ʧ�ܣ��ͻ�������д�����
		LOG("\tFailed to login, errorcode=%d errormsg=%s requestid=%d chain = %d",
			pRspInfo->ErrorID, pRspInfo->ErrorMsg, nRequestID, bIsLast);
		exit(-1);
	}
	SetEvent(g_qEvent);
	//SubscribeMarketData();//��������
	//SubscribeForQuoteRsp();//ѯ������
}

///ѯ��֪ͨ
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


///����ѯ������
void HandlerQuote::SubscribeForQuoteRsp()
{
	LOG("�����ж���ѯ������\n");
	char **ppInstrumentID = new char*[50];
	ppInstrumentID[0] = const_cast<char *>(g_chInstrumentID); 
	int result = m_pUserMdApi->SubscribeForQuoteRsp(ppInstrumentID, 1);
}

///����ѯ��Ӧ��
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



/*********�Լ��޸Ľ϶��*********/
//���ĺ�Լ���飬
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
		//500���������ǵ������һ����
		if (i > 0 && i % 500 == 0 || i == md_InstrumentID.size() - 1) {
			int result = m_pUserMdApi->SubscribeMarketData(ppInstrumentID, j);
			LOG((result == 0) ? "������������1......���ͳɹ�\n" : "������������1......����ʧ�ܣ��������=[%d]\n", result);
		}
	}
}

///��������Ӧ��
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


///�������֪ͨ
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
		if (old.LastPrice != pDepthMarketData->LastPrice) { //���¼��б仯ʱ�����½��档
			//֪ͨ���³ֲ�ӯ����
			emit w_main->signal_UpdatePositionProfit(inst_id, pDepthMarketData->LastPrice);

			//֪ͨ�������������
			emit w_main->signal_UpdateArbPrice(inst_id, pDepthMarketData->LastPrice);

			//֪ͨ����
			emit g_orderWorker->signal_DealArbOrder(inst_id);
		}

		//emit w_main->signal_Test(QString(inst_id), pDepthMarketData->LastPrice);
	}
	//LOG("</OnRtnDepthMarketData>\n");

}
