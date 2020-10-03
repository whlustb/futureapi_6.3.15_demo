#pragma once
#include "HandlerVars.h"
#include "HandlerQuote.h"

using namespace std;
FILE *logfile;

// ���ͻ����뽻���й�ϵͳ������ͨ�����ӣ��ͻ�����Ҫ���е�¼
void HandlerQuote::OnFrontConnected()
{
	/*strcpy_s(g_chBrokerID, getConfig("config", "BrokerID").c_str());
	strcpy_s(g_chUserID, getConfig("config", "UserID").c_str());
	strcpy_s(g_chPassword, getConfig("config", "Password").c_str());*/
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
	SetEvent(xinhao);
	//SubscribeMarketData();//��������
	//SubscribeForQuoteRsp();//ѯ������
}

void HandlerQuote::SubscribeMarketData()//������
{
	int md_num = 0;
	char **ppInstrumentID = new char*[5000];
	for (int count1 = 0; count1 <= md_InstrumentID.size() / 500; count1++)
	{
		if (count1 < md_InstrumentID.size() / 500)
		{
			int a = 0;
			for (a; a < 500; a++)
			{
				ppInstrumentID[a] = const_cast<char *>(md_InstrumentID.at(md_num).c_str());
				md_num++;
			}
			int result = m_pUserMdApi->SubscribeMarketData(ppInstrumentID, a);
			LOG((result == 0) ? "������������1......���ͳɹ�\n" : "������������1......����ʧ�ܣ��������=[%d]\n", result);
		}
		else if (count1 == md_InstrumentID.size() / 500)
		{
			int count2 = 0;
			for (count2; count2 < md_InstrumentID.size() % 500; count2++)
			{
				ppInstrumentID[count2] = const_cast<char *>(md_InstrumentID.at(md_num).c_str());
				md_num++;
			}
			int result = m_pUserMdApi->SubscribeMarketData(ppInstrumentID, count2);
			LOG((result == 0) ? "������������2......���ͳɹ�\n" : "������������2......����ʧ�ܣ��������=[%d]\n", result);
		}
	}
}

///��������Ӧ��
void HandlerQuote::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	LOG("<OnRspSubMarketData>\n");
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
	LOG("</OnRspSubMarketData>\n");
}

///�������֪ͨ
void HandlerQuote::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	LOG("<OnRtnDepthMarketData>\n");
	if (pDepthMarketData)
	{
		LOG("\tInstrumentID = [%s]\n", pDepthMarketData->InstrumentID);
		LOG("\tExchangeID = [%s]\n", pDepthMarketData->ExchangeID);
		LOG("\tLastPrice = [%.8lf]\n", pDepthMarketData->LastPrice);
		LOG("\tPreSettlementPrice = [%.8lf]\n", pDepthMarketData->PreSettlementPrice);
		LOG("\tOpenPrice = [%.8lf]\n", pDepthMarketData->OpenPrice);
		LOG("\tVolume = [%d]\n", pDepthMarketData->Volume);
		LOG("\tTurnover = [%.8lf]\n", pDepthMarketData->Turnover);
		LOG("\tOpenInterest = [%d]\n", pDepthMarketData->OpenInterest);
	}
	LOG("</OnRtnDepthMarketData>\n");
}

///����ѯ������
void HandlerQuote::SubscribeForQuoteRsp()
{
	LOG("�����ж���ѯ������\n");
	char **ppInstrumentID = new char*[50];
	string g_chInstrumentID = getConfig("config", "InstrumentID");
	ppInstrumentID[0] = const_cast<char *>(g_chInstrumentID.c_str());
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
	SetEvent(xinhao);
}


///ѯ��֪ͨ
void HandlerQuote::OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp)
{
	LOG("<OnRtnForQuoteRsp>\n");
	if (pForQuoteRsp)
	{
		LOG("\tTradingDay = [%s]\n", pForQuoteRsp->TradingDay);
		LOG("\tInstrumentID = [%s]\n", pForQuoteRsp->InstrumentID);
		LOG("\tForQuoteSysID = [%s]\n", pForQuoteRsp->ForQuoteSysID);
		LOG("\tForQuoteTime = [%s]\n", pForQuoteRsp->ForQuoteTime);
		LOG("\tActionDay = [%s]\n", pForQuoteRsp->ActionDay);
		LOG("\tExchangeID = [%s]\n", pForQuoteRsp->ExchangeID);
	}
	LOG("</OnRtnForQuoteRsp>\n");
	SetEvent(xinhao);
}