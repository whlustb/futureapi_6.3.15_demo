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

using namespace std;


//������
class HandlerTrade : public CTraderSpi
{
private:
	CThostFtdcTraderApi *m_pUserApi;
public:
	HandlerTrade(CThostFtdcTraderApi *pUserApi) : m_pUserApi(pUserApi) {}
	~HandlerTrade() {}
	virtual void OnFrontConnected();

	//�ͻ�����֤
	void ReqAuthenticate();

	///�ͻ�����֤��Ӧ
	virtual void OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast);

	void RegisterFensUserInfo();

	/*virtual void OnFrontDisconnected(int nReason)
	{
		LOG("<OnFrontDisconnected>\n");
		LOG("\tnReason = %d\n", nReason);
		LOG("</OnFrontDisconnected>\n");
	}*/

	void ReqUserLogin(string brokerId, string uid, string password);

	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	void ReqUserLogout();

	///�ǳ�������Ӧ
	virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///����ȷ�Ͻ��㵥
	void ReqSettlementInfoConfirm();

	///Ͷ���߽�����ȷ����Ӧ
	virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///�û������������
	void ReqUserPasswordUpdate();

	///�û��������������Ӧ
	virtual void OnRspUserPasswordUpdate(CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///�ʽ��˻������������
	void ReqTradingAccountPasswordUpdate();

	///�ʽ��˻��������������Ӧ
	virtual void OnRspTradingAccountPasswordUpdate(CThostFtdcTradingAccountPasswordUpdateField *pTradingAccountPasswordUpdate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///Ԥ��¼��//�޼۵�
	void ReqParkedOrderInsert();

	///Ԥ�񳷵�¼������
	void ReqParkedOrderAction();

	///����ɾ��Ԥ��
	void ReqRemoveParkedOrder();

	///����ɾ��Ԥ�񳷵�
	void ReqRemoveParkedOrderAction();

	///����¼������
	int ReqOrderInsert_Whl(QString inst, QString direction, QString offset, double price, double vol);

	///����¼������
	void ReqOrderInsert_Ordinary();

	///������ֹ��
	void ReqOrderInsert_Touch();

	///������ֹӯ��
	void ReqOrderInsert_TouchProfit();

	//ȫ��ȫ��
	void ReqOrderInsert_VC_CV();

	//���ɲ���
	void ReqOrderInsert_VC_AV();

	//�м۵�
	void ReqOrderInsert_AnyPrice();

	//�м�ת�޼۵�(�н���)
	void ReqOrderInsert_BestPrice();

	//����ָ��
	void ReqOrderInsert_Arbitrage();

	//������
	void ReqOrderInsert_IsSwapOrder();

	///������������
	void ReqOrderAction_Ordinary();

	///ִ������¼������
	void ReqExecOrderInsert(int a);

	///ִ�������������
	void ReqExecOrderAction();

	//����������������
	void ReqBatchOrderAction();

	///�����ѯ����
	void ReqQryOrder();

	///����¼������
	void ReqOrderInsert_Condition(int select_num);

	///������������
	void ReqOrderAction_Condition();

	//������ѯ�ı���
	void ReqOrderAction_forqry(int action_num);

	///�����ѯ�ɽ�
	void ReqQryTrade();

	///�����ѯԤ��
	void ReqQryParkedOrder();

	//�����ѯ������Ԥ�񳷵�
	void ReqQryParkedOrderAction();

	//�����ѯ�ʽ��˻�
	void ReqQryTradingAccount();

	//�����ѯͶ���ֲ߳�
	void ReqQryInvestorPosition();
	void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);


	//�����ѯͶ���ֲ߳���ϸ
	void ReqQryInvestorPositionDetail();

	//�����ѯ��������֤����
	void ReqQryExchangeMarginRate();

	//�����ѯ��Լ��֤����
	void ReqQryInstrumentMarginRate();

	//�����ѯ��Լ��������
	void ReqQryInstrumentCommissionRate();

	//�����ѯ�����̺�Լ��������
	void ReqQryMMInstrumentCommissionRate();

	//�����ѯ��������Ȩ��Լ������
	void ReqQryMMOptionInstrCommRate();

	//�����ѯ����������
	void ReqQryInstrumentOrderCommRate();

	//�����ѯ��Ȩ��Լ������
	void ReqQryOptionInstrCommRate();

	//�����ѯ��Լ
	void ReqQryInstrument();

	///�����ѯ��Լ��Ӧ
	virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//�����ѯͶ���߽�����
	void ReqQrySettlementInfo();

	//�����ѯת����ˮ
	void ReqQryTransferSerial();

	//�����ѯ��Ʒ
	void ReqQryProduct();

	//�����ѯת������
	void ReqQryTransferBank();

	//�����ѯ����֪ͨ
	void ReqQryTradingNotice();

	//�����ѯ���ױ���
	void ReqQryTradingCode();

	//�����ѯ������Ϣȷ��
	void ReqQrySettlementInfoConfirm();

	//�����ѯ��Ʒ��
	void ReqQryProductGroup();

	//�����ѯͶ���ߵ�Ԫ
	void ReqQryInvestUnit();

	//�����ѯ���͹�˾���ײ���
	void ReqQryBrokerTradingParams();

	//�����ѯѯ��
	void ReqQryForQuote();

	//�����ѯ����
	void ReqQryQuote();

	///ѯ��¼������
	void ReqForQuoteInsert();

	///�����̱���¼������
	void ReqQuoteInsert();

	///����֪ͨ
	virtual void OnRtnQuote(CThostFtdcQuoteField *pQuote);

	//���۳���
	void ReqQuoteAction();

	//��ѯ��󱨵���������
	void ReqQueryMaxOrderVolume();

	//�����ѯ��������û�����
	void ReqQueryCFMMCTradingAccountToken();

	///������������ر�
	virtual void OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo);

	///����¼��������Ӧ
	virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast);

	///����¼�����ر�
	virtual void OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo);

	///����֪ͨ
	virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);

	//�ɽ��ر���
	virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);

	///ɾ��Ԥ����Ӧ
	virtual void OnRspRemoveParkedOrder(CThostFtdcRemoveParkedOrderField *pRemoveParkedOrder, CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast);

	///ɾ��Ԥ�񳷵���Ӧ
	virtual void OnRspRemoveParkedOrderAction(CThostFtdcRemoveParkedOrderActionField *pRemoveParkedOrderAction, CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast);

	///Ԥ��¼��������Ӧ
	virtual void OnRspParkedOrderInsert(CThostFtdcParkedOrderField *pParkedOrder, CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast);

	///Ԥ�񳷵�¼��������Ӧ
	virtual void OnRspParkedOrderAction(CThostFtdcParkedOrderActionField *pParkedOrderAction, CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast);

	///�����ѯԤ�񳷵���Ӧ
	virtual void OnRspQryParkedOrderAction(CThostFtdcParkedOrderActionField *pParkedOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///�����ѯԤ����Ӧ
	virtual void OnRspQryParkedOrder(CThostFtdcParkedOrderField *pParkedOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///�����ѯ������Ӧ
	virtual void OnRspQryOrder(CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///ִ������֪ͨ
	virtual void OnRtnExecOrder(CThostFtdcExecOrderField *pExecOrder);

	//�ڻ������ѯ�����������
	void ReqQueryBankAccountMoneyByFuture();

	//�ڻ����������ʽ�ת�ڻ�����
	void ReqFromBankToFutureByFuture();

	//�ڻ������ڻ��ʽ�ת��������
	void ReqFromFutureToBankByFuture();

	//��Ȩ�ԶԳ�¼������
	void ReqOptionSelfCloseInsert();

	///��Ȩ�ԶԳ�֪ͨ
	virtual void OnRtnOptionSelfClose(CThostFtdcOptionSelfCloseField *pOptionSelfClose);

	//��Ȩ�ԶԳ��������
	void ReqOptionSelfCloseAction();

	//�����ѯ��Ȩ�ԶԳ�
	void ReqQryOptionSelfClose();

	///�����ѯ��Ȩ�ԶԳ���Ӧ
	virtual void OnRspQryOptionSelfClose(CThostFtdcOptionSelfCloseField *pOptionSelfClose, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///�����ѯִ������
	void ReqQryExecOrder();

	///��ѯ�����ʽ��˻�
	void ReqQrySecAgentTradingAccount();

	///�����ѯ�����������ʽ�У��ģʽ
	void ReqQrySecAgentCheckMode();

	///ע���û��ն���Ϣ�������м̷�����������ģʽ
	///��Ҫ���ն���֤�ɹ����û���¼ǰ���øýӿ�
	void RegisterUserSystemInfo();

	///�ϱ��û��ն���Ϣ�������м̷���������Ա��¼ģʽ
	///����Ա��¼�󣬿��Զ�ε��øýӿ��ϱ��ͻ���Ϣ
	void SubmitUserSystemInfo();

	///��ѯ�û���ǰ֧�ֵ���֤ģʽ
	void ReqUserAuthMethod();

	///�û�������ȡͼ����֤������
	void ReqGenUserCaptcha();

	///�û�������ȡ������֤������
	void ReqGenUserText();

	///�û���������ͼƬ��֤��ĵ�½����
	void ReqUserLoginWithCaptcha();

	///�û��������ж�����֤��ĵ�½����
	void ReqUserLoginWithText();

	///�û��������ж�̬����ĵ�½����
	void ReqUserLoginWithOTP();

	///�����ѯ������������Ϣ
	void ReqQrySecAgentTradeInfo();


};
