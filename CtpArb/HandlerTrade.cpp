#pragma once

#include "HandlerVars.h"
#include "HandlerTrade.h"
#include "MainForm.h"
#include "CtpArbFunc.h"

extern MainWindow* w_main;

//���ӳɹ���
void HandlerTrade::OnFrontConnected()
{
	//strcpy_s(g_NewExecOrderRef, "");
	//strcpy_s(g_NewExecOrderSysID, "");
	g_NewFrontID = 0;
	g_NewSessionID = 0;
	//ReqUserLogin();
	//ReqAuthenticate();
	SetEvent(g_hEvent);
}

//�ͻ�����֤
void HandlerTrade::ReqAuthenticate()
{
	//strcpy_s(g_chUserProductInfo, getConfig("config", "UserProductInfo").c_str());
	CThostFtdcReqAuthenticateField a = { 0 };
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.UserID, g_config.UserID);
	//strcpy_s(a.UserProductInfo, "");
	strcpy_s(a.AuthCode, g_config.AuthCode);
	strcpy_s(a.AppID, g_config.AppID);
	int b = m_pUserApi->ReqAuthenticate(&a, 1);
	printf("\t�ͻ�����֤ = [%d]\n", b);
}

///�ͻ�����֤��Ӧ
void HandlerTrade::OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast)
{
	CTraderSpi::OnRspAuthenticate(pRspAuthenticateField, pRspInfo, nRequestID, bIsLast);
	SetEvent(g_hEvent);
}

void HandlerTrade::RegisterFensUserInfo()
{
	CThostFtdcFensUserInfoField pFensUserInfo = { 0 };
	strcpy_s(pFensUserInfo.BrokerID, g_config.server.BrokerID);
	strcpy_s(pFensUserInfo.UserID, g_config.UserID);
	pFensUserInfo.LoginMode = THOST_FTDC_LM_Trade;
	m_pUserApi->RegisterFensUserInfo(&pFensUserInfo);
}

/*void TradeHandler::OnFrontDisconnected(int nReason)
{
	LOG("<OnFrontDisconnected>\n");
	LOG("\tnReason = %d\n", nReason);
	LOG("</OnFrontDisconnected>\n");
}*/

void HandlerTrade::ReqUserLogin(string brokerId, string uid, string password)
{
	CThostFtdcReqUserLoginField reqUserLogin = { 0 };
	strcpy_s(reqUserLogin.BrokerID, g_config.server.BrokerID);
	strcpy_s(reqUserLogin.UserID, g_config.UserID);
	strcpy_s(reqUserLogin.Password, g_config.Password);
	//strcpy_s(reqUserLogin.ClientIPAddress, "::c0a8:0101");
	//strcpy_s(reqUserLogin.UserProductInfo, "123");
	// ������½����
	m_pUserApi->ReqUserLogin(&reqUserLogin, nRequestID++);
}

void HandlerTrade::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	FrontID = pRspUserLogin->FrontID;
	SessionID = pRspUserLogin->SessionID;
	CTraderSpi::OnRspUserLogin(pRspUserLogin, pRspInfo, nRequestID, bIsLast);
	if (pRspInfo->ErrorID != 0)
		//if (pRspInfo)
	{
		LOG("\tFailed to login, errorcode=[%d]\n \terrormsg=[%s]\n \trequestid = [%d]\n \tchain = [%d]\n",
			pRspInfo->ErrorID, pRspInfo->ErrorMsg, nRequestID, bIsLast);
		//exit(-1);
	}
	SetEvent(g_hEvent);
}

void HandlerTrade::ReqUserLogout()
{
	CThostFtdcUserLogoutField a = { 0 };
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.UserID, g_config.UserID);
	m_pUserApi->ReqUserLogout(&a, nRequestID++);
}

///�ǳ�������Ӧ
void HandlerTrade::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	LOG("<OnRspUserLogout>\n");
	if (pUserLogout)
	{
		LOG("\tBrokerID [%s]\n", pUserLogout->BrokerID);
		LOG("\tUserID [%s]\n", pUserLogout->UserID);
	}
	if (pRspInfo)
	{
		LOG("\tErrorMsg [%s]\n", pRspInfo->ErrorMsg);
		LOG("\tErrorID [%d]\n", pRspInfo->ErrorID);
	}
	LOG("\tnRequestID [%d]\n", nRequestID);
	LOG("\tbIsLast [%d]\n", bIsLast);
	LOG("</OnRspUserLogout>\n");
	api->Release();
}

///����ȷ�Ͻ��㵥
void HandlerTrade::ReqSettlementInfoConfirm()
{
	CThostFtdcSettlementInfoConfirmField Confirm = { 0 };
	///���͹�˾����
	strcpy_s(Confirm.BrokerID, g_config.server.BrokerID);
	///Ͷ���ߴ���
	strcpy_s(Confirm.InvestorID, g_config.UserID);
	m_pUserApi->ReqSettlementInfoConfirm(&Confirm, nRequestID++);
}

///Ͷ���߽�����ȷ����Ӧ
void HandlerTrade::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	CTraderSpi::OnRspSettlementInfoConfirm(pSettlementInfoConfirm, pRspInfo, nRequestID, bIsLast);
	SetEvent(g_hEvent);
}

///�û������������
void HandlerTrade::ReqUserPasswordUpdate()
{
	string newpassword;
	LOG("�������µ�¼���룺\n");
	cin >> newpassword;
	CThostFtdcUserPasswordUpdateField a = { 0 };
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.UserID, g_config.UserID);
	strcpy_s(a.OldPassword, g_config.Password);
	strcpy_s(a.NewPassword, newpassword.c_str());
	int b = m_pUserApi->ReqUserPasswordUpdate(&a, nRequestID++);
	LOG((b == 0) ? "�û������������......���ͳɹ�\n" : "�û������������......����ʧ�ܣ����=[%d]\n", b);
}

///�û��������������Ӧ
void HandlerTrade::OnRspUserPasswordUpdate(CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	CTraderSpi::OnRspUserPasswordUpdate(pUserPasswordUpdate, pRspInfo, nRequestID, bIsLast);
	SetEvent(g_hEvent);
}

///�ʽ��˻������������
void HandlerTrade::ReqTradingAccountPasswordUpdate()
{
	string newpassword;
	LOG("���������ʽ����룺\n");
	cin >> newpassword;
	CThostFtdcTradingAccountPasswordUpdateField a = { 0 };
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.AccountID, g_config.InvestorID);
	strcpy_s(a.OldPassword, g_config.Password);
	strcpy_s(a.NewPassword, newpassword.c_str());
	strcpy_s(a.CurrencyID, "CNY");
	int b = m_pUserApi->ReqTradingAccountPasswordUpdate(&a, nRequestID++);
	LOG((b == 0) ? "�ʽ��˻������������......���ͳɹ�\n" : "�ʽ��˻������������......����ʧ�ܣ����=[%d]\n", b);
}

///�ʽ��˻��������������Ӧ
void HandlerTrade::OnRspTradingAccountPasswordUpdate(CThostFtdcTradingAccountPasswordUpdateField *pTradingAccountPasswordUpdate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	CTraderSpi::OnRspTradingAccountPasswordUpdate(pTradingAccountPasswordUpdate, pRspInfo, nRequestID, bIsLast);
	SetEvent(g_hEvent);
}

///Ԥ��¼��//�޼۵�
void HandlerTrade::ReqParkedOrderInsert()
{
	int limitprice = 0;
	LOG("�������޼۵��۸�(Ĭ��0)\n");
	cin >> limitprice;
	CThostFtdcParkedOrderField a = { 0 };
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.InvestorID, g_config.InvestorID);
	strcpy_s(a.InstrumentID, g_chInstrumentID);
	strcpy_s(a.UserID, g_config.UserID);
	a.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
	a.Direction = THOST_FTDC_D_Buy;
	a.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
	a.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	a.LimitPrice = limitprice;
	a.VolumeTotalOriginal = 1;
	a.TimeCondition = THOST_FTDC_TC_GFD;
	a.VolumeCondition = THOST_FTDC_VC_AV;
	a.MinVolume = 1;
	a.ContingentCondition = THOST_FTDC_CC_Immediately;
	a.StopPrice = 0;
	a.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	a.IsAutoSuspend = 0;
	strcpy_s(a.ExchangeID, g_chExchangeID);
	int b = m_pUserApi->ReqParkedOrderInsert(&a, nRequestID++);
	LOG((b == 0) ? "����¼��Ԥ��......���ͳɹ�\n" : "����¼��Ԥ��......����ʧ�ܣ����=[%d]\n", b);
}

///Ԥ�񳷵�¼������
void HandlerTrade::ReqParkedOrderAction()
{
	CThostFtdcParkedOrderActionField a = { 0 };
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.InvestorID, g_config.InvestorID);
	//strcpy_s(a.OrderRef, "          15");
	strcpy_s(a.ExchangeID, g_chExchangeID);
	/*a.FrontID = 1;
	a.SessionID = -287506422;*/
	strcpy_s(a.OrderSysID, g_chOrderSysID);
	strcpy_s(a.UserID, g_config.UserID);
	strcpy_s(a.InstrumentID, g_chInstrumentID);
	a.ActionFlag = THOST_FTDC_AF_Delete;
	int b = m_pUserApi->ReqParkedOrderAction(&a, nRequestID++);
	LOG((b == 0) ? "����¼��Ԥ�񳷵�......���ͳɹ�\n" : "����¼��Ԥ�񳷵�......����ʧ�ܣ����=[%d]\n", b);
}

///����ɾ��Ԥ��
void HandlerTrade::ReqRemoveParkedOrder()
{
	CThostFtdcRemoveParkedOrderField a = { 0 };
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.InvestorID, g_config.InvestorID);
	strcpy_s(a.ParkedOrderID, g_chParkedOrderID1);
	int b = m_pUserApi->ReqRemoveParkedOrder(&a, nRequestID++);
	LOG((b == 0) ? "����ɾ��Ԥ��......���ͳɹ�\n" : "����ɾ��Ԥ��......����ʧ�ܣ����=[%d]\n", b);
}

///����ɾ��Ԥ�񳷵�
void HandlerTrade::ReqRemoveParkedOrderAction()
{
	CThostFtdcRemoveParkedOrderActionField a = { 0 };
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.InvestorID, g_config.InvestorID);
	strcpy_s(a.ParkedOrderActionID, g_chParkedOrderActionID1);
	int b = m_pUserApi->ReqRemoveParkedOrderAction(&a, nRequestID++);
	LOG((b == 0) ? "����ɾ��Ԥ�񳷵�......���ͳɹ�\n" : "����ɾ��Ԥ�񳷵�......����ʧ�ܣ����=[%d]\n", b);
}


///����¼������
void HandlerTrade::ReqOrderInsert_Ordinary()
{
	system("cls");
	string new_limitprice;
	LOG("������ָ���۸�\n");
	cin >> new_limitprice;

	CThostFtdcInputOrderField ord = { 0 };
	strcpy_s(ord.BrokerID, g_config.server.BrokerID);
	strcpy_s(ord.InvestorID, g_config.InvestorID);
	strcpy_s(ord.InstrumentID, g_chInstrumentID);
	strcpy_s(ord.UserID, g_config.UserID);
	ord.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
	ord.CombOffsetFlag[0] = THOST_FTDC_OF_Open;

	int num1;
Direction:LOG("��ѡ����������\t1.��\t2.��\n");
	cin >> num1;
	if (num1 == 1) {
		ord.Direction = THOST_FTDC_D_Buy;//��
	}
	else if (num1 == 2) {
		ord.Direction = THOST_FTDC_D_Sell;//��
	}
	else {
		LOG("�����������������\n");
		_getch();
		goto Direction;
	}

	int num2;
CombOffsetFlag:LOG("�����뿪ƽ����\t1.����\t2.ƽ��\t3.ǿƽ\t4.ƽ��\t5.ƽ��\t6.ǿ��\t7.����ǿƽ\n");
	cin >> num2;
	if (num2 == 1) {
		ord.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
	}
	else if (num2 == 2) {
		ord.CombOffsetFlag[0] = THOST_FTDC_OF_Close;
	}
	else if (num2 == 3) {
		ord.CombOffsetFlag[0] = THOST_FTDC_OF_ForceClose;
	}
	else if (num2 == 4) {
		ord.CombOffsetFlag[0] = THOST_FTDC_OF_CloseToday;
	}
	else if (num2 == 5) {
		ord.CombOffsetFlag[0] = THOST_FTDC_OF_CloseYesterday;
	}
	else if (num2 == 6) {
		ord.CombOffsetFlag[0] = THOST_FTDC_OF_ForceOff;
	}
	else if (num2 == 7) {
		ord.CombOffsetFlag[0] = THOST_FTDC_OF_LocalForceClose;
	}
	else {
		LOG("�����������������\n");
		_getch();
		goto CombOffsetFlag;
	}

	//ord.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	strcpy_s(ord.CombHedgeFlag, "1");
	//ord.LimitPrice = atoi(getConfig("config", "LimitPrice").c_str());
	ord.LimitPrice = atoi(new_limitprice.c_str());
	ord.VolumeTotalOriginal = 1;
	ord.TimeCondition = THOST_FTDC_TC_GFD;///������Ч
	ord.VolumeCondition = THOST_FTDC_VC_CV;///ȫ������
	ord.MinVolume = 1;
	ord.ContingentCondition = THOST_FTDC_CC_Immediately;
	ord.StopPrice = 0;
	ord.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	ord.IsAutoSuspend = 0;
	strcpy_s(ord.ExchangeID, g_chExchangeID);
	int a = m_pUserApi->ReqOrderInsert(&ord, 1);
	LOG((a == 0) ? "����¼�������޼۵�......���ͳɹ�\n" : "����¼�������޼۵�......����ʧ�ܣ����=[%d]\n", a);
}

///������ֹ��
void HandlerTrade::ReqOrderInsert_Touch()
{
	int new_limitprice;
	LOG("������ָ���۸�limitprice��\n");
	cin >> new_limitprice;

	int new_StopPrice;
	LOG("������ָ���۸�stopprice��\n");
	cin >> new_StopPrice;

	CThostFtdcInputOrderField ord = { 0 };
	strcpy_s(ord.BrokerID, g_config.server.BrokerID);
	strcpy_s(ord.InvestorID, g_config.InvestorID);
	strcpy_s(ord.InstrumentID, g_chInstrumentID);
	strcpy_s(ord.UserID, g_config.UserID);
	//strcpy_s(ord.OrderRef, "");
	ord.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
	ord.Direction = THOST_FTDC_D_Buy;//��
	ord.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
	ord.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	//ord.LimitPrice = atoi(getConfig("config", "LimitPrice").c_str());
	ord.LimitPrice = new_limitprice;
	ord.VolumeTotalOriginal = 1;
	ord.TimeCondition = THOST_FTDC_TC_GFD;///������Ч
	ord.VolumeCondition = THOST_FTDC_VC_AV;///�κ�����
	ord.MinVolume = 1;
	ord.ContingentCondition = THOST_FTDC_CC_Touch;
	ord.StopPrice = new_StopPrice;
	ord.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	ord.IsAutoSuspend = 0;
	strcpy_s(ord.ExchangeID, g_chExchangeID);
	int a = m_pUserApi->ReqOrderInsert(&ord, 1);
	LOG((a == 0) ? "����¼�������޼۵�......���ͳɹ�\n" : "����¼�������޼۵�......����ʧ�ܣ����=[%d]\n", a);
}

///������ֹӯ��
void HandlerTrade::ReqOrderInsert_TouchProfit()
{
	int new_limitprice;
	LOG("������ָ���۸�limitprice��\n");
	cin >> new_limitprice;

	int new_StopPrice;
	LOG("������ָ���۸�stopprice��\n");
	cin >> new_StopPrice;

	CThostFtdcInputOrderField ord = { 0 };
	strcpy_s(ord.BrokerID, g_config.server.BrokerID);
	strcpy_s(ord.InvestorID, g_config.InvestorID);
	strcpy_s(ord.InstrumentID, g_chInstrumentID);
	strcpy_s(ord.UserID, g_config.UserID);
	//strcpy_s(ord.OrderRef, "");
	ord.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
	ord.Direction = THOST_FTDC_D_Buy;//��
	ord.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
	ord.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	//ord.LimitPrice = atoi(getConfig("config", "LimitPrice").c_str());
	ord.LimitPrice = new_limitprice;
	ord.VolumeTotalOriginal = 1;
	ord.TimeCondition = THOST_FTDC_TC_GFD;///������Ч
	ord.VolumeCondition = THOST_FTDC_VC_AV;///ȫ������
	ord.MinVolume = 1;
	ord.ContingentCondition = THOST_FTDC_CC_TouchProfit;
	ord.StopPrice = new_StopPrice;
	ord.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	ord.IsAutoSuspend = 0;
	strcpy_s(ord.ExchangeID, g_chExchangeID);
	int a = m_pUserApi->ReqOrderInsert(&ord, 1);
	LOG((a == 0) ? "����¼�������޼۵�......���ͳɹ�\n" : "����¼�������޼۵�......����ʧ�ܣ����=[%d]\n", a);
}

//ȫ��ȫ��
void HandlerTrade::ReqOrderInsert_VC_CV()
{
	int new_limitprice;
	LOG("������ָ���۸�\n");
	cin >> new_limitprice;

	int insert_num;
	LOG("�������µ�������\n");
	cin >> insert_num;

	CThostFtdcInputOrderField ord = { 0 };
	strcpy_s(ord.BrokerID, g_config.server.BrokerID);
	strcpy_s(ord.InvestorID, g_config.InvestorID);
	strcpy_s(ord.InstrumentID, g_chInstrumentID);
	strcpy_s(ord.UserID, g_config.UserID);
	//strcpy_s(ord.OrderRef, "");
	ord.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
	ord.Direction = THOST_FTDC_D_Buy;//��
	ord.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
	ord.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	//ord.LimitPrice = atoi(getConfig("config", "LimitPrice").c_str());
	ord.LimitPrice = new_limitprice;
	ord.VolumeTotalOriginal = insert_num;
	ord.TimeCondition = THOST_FTDC_TC_GFD;///������Ч
	ord.VolumeCondition = THOST_FTDC_VC_CV;///ȫ������
	ord.MinVolume = 1;
	ord.ContingentCondition = THOST_FTDC_CC_Immediately;
	ord.StopPrice = 0;
	ord.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	ord.IsAutoSuspend = 0;
	strcpy_s(ord.ExchangeID, g_chExchangeID);
	int a = m_pUserApi->ReqOrderInsert(&ord, 1);
	LOG((a == 0) ? "����¼�������޼۵�......���ͳɹ�\n" : "����¼�������޼۵�......����ʧ�ܣ����=[%d]\n", a);
}

//���ɲ���
void HandlerTrade::ReqOrderInsert_VC_AV()
{
	int new_limitprice;
	LOG("������ָ���۸�\n");
	cin >> new_limitprice;

	int insert_num;
	LOG("�������µ�������\n");
	cin >> insert_num;

	CThostFtdcInputOrderField ord = { 0 };
	strcpy_s(ord.BrokerID, g_config.server.BrokerID);
	strcpy_s(ord.InvestorID, g_config.InvestorID);
	strcpy_s(ord.InstrumentID, g_chInstrumentID);
	strcpy_s(ord.UserID, g_config.UserID);
	//strcpy_s(ord.OrderRef, "");
	ord.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
	ord.Direction = THOST_FTDC_D_Buy;//��
	ord.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
	ord.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	//ord.LimitPrice = atoi(getConfig("config", "LimitPrice").c_str());
	ord.LimitPrice = new_limitprice;
	ord.VolumeTotalOriginal = insert_num;
	ord.TimeCondition = THOST_FTDC_TC_GFD;///������Ч
	ord.VolumeCondition = THOST_FTDC_VC_AV;///�κ�����
	ord.MinVolume = 1;
	ord.ContingentCondition = THOST_FTDC_CC_Immediately;
	ord.StopPrice = 0;
	ord.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	ord.IsAutoSuspend = 0;
	strcpy_s(ord.ExchangeID, g_chExchangeID);
	int a = m_pUserApi->ReqOrderInsert(&ord, 1);
	LOG((a == 0) ? "����¼�������޼۵�......���ͳɹ�\n" : "����¼�������޼۵�......����ʧ�ܣ����=[%d]\n", a);
}

//�м۵�
void HandlerTrade::ReqOrderInsert_AnyPrice()
{
	CThostFtdcInputOrderField ord = { 0 };
	strcpy_s(ord.BrokerID, g_config.server.BrokerID);
	strcpy_s(ord.InvestorID, g_config.InvestorID);
	strcpy_s(ord.InstrumentID, g_chInstrumentID);
	strcpy_s(ord.UserID, g_config.UserID);
	//strcpy_s(ord.OrderRef, "");
	ord.OrderPriceType = THOST_FTDC_OPT_AnyPrice;
	ord.Direction = THOST_FTDC_D_Buy;//��
	ord.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
	ord.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	//ord.LimitPrice = atoi(getConfig("config", "LimitPrice").c_str());
	//ord.LimitPrice = new_limitprice;
	ord.LimitPrice = 0;
	ord.VolumeTotalOriginal = 1;
	ord.TimeCondition = THOST_FTDC_TC_IOC;///������ɣ�������
	ord.VolumeCondition = THOST_FTDC_VC_AV;///�κ�����
	ord.MinVolume = 1;
	ord.ContingentCondition = THOST_FTDC_CC_Immediately;
	//ord.StopPrice = 0;
	ord.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	ord.IsAutoSuspend = 0;
	strcpy_s(ord.ExchangeID, g_chExchangeID);
	int a = m_pUserApi->ReqOrderInsert(&ord, 1);
	LOG((a == 0) ? "����¼�������޼۵�......���ͳɹ�\n" : "����¼�������޼۵�......����ʧ�ܣ����=[%d]\n", a);
}

//�м�ת�޼۵�(�н���)
void HandlerTrade::ReqOrderInsert_BestPrice()
{
	CThostFtdcInputOrderField ord = { 0 };
	strcpy_s(ord.BrokerID, g_config.server.BrokerID);
	strcpy_s(ord.InvestorID, g_config.InvestorID);
	strcpy_s(ord.InstrumentID, g_chInstrumentID);
	strcpy_s(ord.UserID, g_config.UserID);
	//strcpy_s(ord.OrderRef, "");
	ord.OrderPriceType = THOST_FTDC_OPT_BestPrice;
	ord.Direction = THOST_FTDC_D_Buy;//��
	ord.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
	ord.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	//ord.LimitPrice = atoi(getConfig("config", "LimitPrice").c_str());
	//ord.LimitPrice = new_limitprice;
	ord.VolumeTotalOriginal = 1;
	ord.TimeCondition = THOST_FTDC_TC_GFD;///������Ч
	ord.VolumeCondition = THOST_FTDC_VC_AV;///�κ�����
	ord.MinVolume = 1;
	ord.ContingentCondition = THOST_FTDC_CC_Immediately;
	ord.StopPrice = 0;
	ord.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	ord.IsAutoSuspend = 0;
	strcpy_s(ord.ExchangeID, g_chExchangeID);
	int a = m_pUserApi->ReqOrderInsert(&ord, 1);
	LOG((a == 0) ? "����¼�������޼۵�......���ͳɹ�\n" : "����¼�������޼۵�......����ʧ�ܣ����=[%d]\n", a);
}

//����ָ��
void HandlerTrade::ReqOrderInsert_Arbitrage()
{
	int new_limitprice;
	LOG("������ָ���۸�\n");
	cin >> new_limitprice;

	CThostFtdcInputOrderField ord = { 0 };
	strcpy_s(ord.BrokerID, g_config.server.BrokerID);
	strcpy_s(ord.InvestorID, g_config.InvestorID);
	strcpy_s(ord.InstrumentID, g_chInstrumentID);
	strcpy_s(ord.UserID, g_config.UserID);
	//strcpy_s(ord.OrderRef, "");
	ord.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
	ord.Direction = THOST_FTDC_D_Buy;//��
	ord.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
	ord.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	//ord.LimitPrice = atoi(getConfig("config", "LimitPrice").c_str());
	ord.LimitPrice = new_limitprice;
	ord.VolumeTotalOriginal = 1;
	ord.TimeCondition = THOST_FTDC_TC_GFD;///������Ч
	ord.VolumeCondition = THOST_FTDC_VC_AV;///�κ�����
	ord.MinVolume = 1;
	ord.ContingentCondition = THOST_FTDC_CC_Immediately;
	ord.StopPrice = 0;
	ord.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	ord.IsAutoSuspend = 0;
	strcpy_s(ord.ExchangeID, g_chExchangeID);
	int a = m_pUserApi->ReqOrderInsert(&ord, 1);
	LOG((a == 0) ? "����¼�������޼۵�......���ͳɹ�\n" : "����¼�������޼۵�......����ʧ�ܣ����=[%d]\n", a);
}

//������
void HandlerTrade::ReqOrderInsert_IsSwapOrder()
{
	int new_limitprice;
	LOG("������ָ���۸�\n");
	cin >> new_limitprice;

	CThostFtdcInputOrderField ord = { 0 };
	strcpy_s(ord.BrokerID, g_config.server.BrokerID);
	strcpy_s(ord.InvestorID, g_config.InvestorID);
	strcpy_s(ord.InstrumentID, g_chInstrumentID);
	strcpy_s(ord.UserID, g_config.UserID);
	//strcpy_s(ord.OrderRef, "");
	ord.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
	ord.Direction = THOST_FTDC_D_Buy;//��
	ord.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
	ord.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	//ord.LimitPrice = atoi(getConfig("config", "LimitPrice").c_str());
	ord.LimitPrice = new_limitprice;
	ord.VolumeTotalOriginal = 1;
	ord.TimeCondition = THOST_FTDC_TC_GFD;///������Ч
	ord.VolumeCondition = THOST_FTDC_VC_AV;///�κ�����
	ord.MinVolume = 1;
	ord.ContingentCondition = THOST_FTDC_CC_Immediately;
	ord.StopPrice = 0;
	ord.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	ord.IsAutoSuspend = 0;
	ord.IsSwapOrder = 1;//��������־
	strcpy_s(ord.ExchangeID, g_chExchangeID);
	int a = m_pUserApi->ReqOrderInsert(&ord, 1);
	LOG((a == 0) ? "����¼�������޼۵�......���ͳɹ�\n" : "����¼�������޼۵�......����ʧ�ܣ����=[%d]\n", a);
}

///������������
void HandlerTrade::ReqOrderAction_Ordinary()
{
	CThostFtdcInputOrderActionField a = { 0 };
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.InvestorID, g_config.InvestorID);
	a.OrderActionRef = 1;
	strcpy_s(a.OrderRef, g_chOrderRef);
	//a.FrontID = g_chFrontID;
	//a.SessionID = g_chSessionID;
	strcpy_s(a.ExchangeID, g_chExchangeID);
	strcpy_s(a.InstrumentID, g_chInstrumentID);
	strcpy_s(a.OrderSysID, g_chOrderSysID);
	a.ActionFlag = THOST_FTDC_AF_Delete;
	strcpy_s(a.UserID, g_config.UserID);
	int ab = m_pUserApi->ReqOrderAction(&a, nRequestID++);
	LOG((ab == 0) ? "������������......���ͳɹ�\n" : "������������......����ʧ�ܣ����=[%d]\n", ab);
}

///ִ������¼������
void HandlerTrade::ReqExecOrderInsert(int a)
{
	CThostFtdcInputExecOrderField OrderInsert = { 0 };
	strcpy_s(OrderInsert.BrokerID, g_config.server.BrokerID);
	strcpy_s(OrderInsert.InvestorID, g_config.InvestorID);
	strcpy_s(OrderInsert.InstrumentID, g_chInstrumentID);
	strcpy_s(OrderInsert.ExchangeID, g_chExchangeID);
	//strcpy_s(OrderInsert.ExecOrderRef, "00001");
	strcpy_s(OrderInsert.UserID, g_config.UserID);
	OrderInsert.Volume = 1;
	OrderInsert.RequestID = 1;
	OrderInsert.OffsetFlag = THOST_FTDC_OF_Close;//��ƽ��־
	OrderInsert.HedgeFlag = THOST_FTDC_HF_Speculation;//Ͷ���ױ���־
	if (a == 0) {
		OrderInsert.ActionType = THOST_FTDC_ACTP_Exec;//ִ����������
	}
	if (a == 1) {
		OrderInsert.ActionType = THOST_FTDC_ACTP_Abandon;//ִ����������
	}
	OrderInsert.PosiDirection = THOST_FTDC_PD_Long;//�ֲֶ�շ�������
	OrderInsert.ReservePositionFlag = THOST_FTDC_EOPF_Reserve;//��Ȩ��Ȩ���Ƿ����ڻ�ͷ��ı������
	//OrderInsert.ReservePositionFlag = THOST_FTDC_EOPF_UnReserve;//������ͷ��
	OrderInsert.CloseFlag = THOST_FTDC_EOCF_NotToClose;//��Ȩ��Ȩ�����ɵ�ͷ���Ƿ��Զ�ƽ������
	//OrderInsert.CloseFlag = THOST_FTDC_EOCF_AutoClose;//�Զ�ƽ��
	//strcpy_s(OrderInsert.InvestUnitID, "");AccountID
	//strcpy_s(OrderInsert.AccountID, "");
	//strcpy_s(OrderInsert.CurrencyID, "CNY");
	//strcpy_s(OrderInsert.ClientID, "");
	int b = m_pUserApi->ReqExecOrderInsert(&OrderInsert, 1);
	LOG((b == 0) ? "ִ������¼������......���ͳɹ�\n" : "ִ������¼������......����ʧ�ܣ��������=[%d]\n", b);
}

///ִ�������������
void HandlerTrade::ReqExecOrderAction()
{
	CThostFtdcInputExecOrderActionField a = { 0 };
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.InvestorID, g_config.InvestorID);
	a.ExecOrderActionRef = 1;
	strcpy_s(a.ExecOrderRef, g_NewExecOrderRef);
	a.FrontID = g_NewFrontID;
	a.SessionID = g_NewSessionID;
	strcpy_s(a.ExchangeID, g_chExchangeID);
	strcpy_s(a.ExecOrderSysID, g_NewExecOrderSysID);
	a.ActionFlag = THOST_FTDC_AF_Delete;//ɾ��
	strcpy_s(a.UserID, g_config.UserID);
	strcpy_s(a.InstrumentID, g_chInstrumentID);
	//strcpy_s(a.InvestUnitID, "");
	//strcpy_s(a.IPAddress, "");
	//strcpy_s(a.MacAddress, "");
	int b = m_pUserApi->ReqExecOrderAction(&a, 1);
	LOG((b == 0) ? "ִ�������������......���ͳɹ�\n" : "ִ�������������......����ʧ�ܣ��������=[%d]\n", b);
}

//����������������
void HandlerTrade::ReqBatchOrderAction()
{
	CThostFtdcInputBatchOrderActionField a = { 0 };

}

///�����ѯ����
void HandlerTrade::ReqQryOrder()
{
	action_number = 0;
	vector_OrderSysID.clear();
	vector_ExchangeID.clear();
	vector_InstrumentID.clear();
	CThostFtdcQryOrderField a = { 0 };
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.InvestorID, g_config.InvestorID);
	//strcpy_s(a.InstrumentID, g_chInstrumentID);
	strcpy_s(a.ExchangeID, g_chExchangeID);
	int ab = m_pUserApi->ReqQryOrder(&a, nRequestID++);
	LOG((ab == 0) ? "�����ѯ����......���ͳɹ�\n" : "�����ѯ����......����ʧ�ܣ����=[%d]\n", ab);

}

///�����ѯ������Ӧ
void HandlerTrade::OnRspQryOrder(CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pOrder) {
		vector_OrderSysID.push_back(pOrder->OrderSysID);
		vector_ExchangeID.push_back(pOrder->ExchangeID);
		vector_InstrumentID.push_back(pOrder->InstrumentID);

		//���浽���ء����Բ��ֳɽ�������δ�ɽ�������Ȼ�ڹҵ��еĶ�����
		if (pOrder->OrderStatus == THOST_FTDC_OST_PartTradedQueueing || pOrder->OrderStatus == THOST_FTDC_OST_NoTradeQueueing )
		{
			g_orderUnfilledMap[QString(pOrder->OrderSysID)] = *pOrder;
		}
	}
	CTraderSpi::OnRspQryOrder(pOrder, pRspInfo, nRequestID, bIsLast);
	action_number++;
	LOG("\n��ѯ��ţ�\"%d\"\n\n", action_number);
}


///����¼������
void HandlerTrade::ReqOrderInsert_Condition(int select_num)
{
	string limit_price;
	LOG("������ָ���۸�(limitprice):\n");
	cin >> limit_price;

	string stop_price;
	LOG("�����봥���۸�(stopprice):\n");
	cin >> stop_price;

	CThostFtdcInputOrderField a = { 0 };
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.InvestorID, g_config.InvestorID);
	strcpy_s(a.InstrumentID, g_chInstrumentID);
	strcpy_s(a.UserID, g_config.UserID);
	a.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
	a.Direction = THOST_FTDC_D_Buy;//��
	//a.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
	a.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	strcpy_s(a.CombOffsetFlag, "0");
	strcpy_s(a.CombHedgeFlag, "1");
	a.LimitPrice = atoi(limit_price.c_str());
	a.VolumeTotalOriginal = 1;
	a.TimeCondition = THOST_FTDC_TC_GFD;
	a.VolumeCondition = THOST_FTDC_VC_AV;
	a.MinVolume = 0;
	if (select_num == 1)
	{
		a.ContingentCondition = THOST_FTDC_CC_LastPriceGreaterThanStopPrice;
	}
	else if (select_num == 2)
	{
		a.ContingentCondition = THOST_FTDC_CC_LastPriceGreaterEqualStopPrice;
	}
	else if (select_num == 3)
	{
		a.ContingentCondition = THOST_FTDC_CC_LastPriceLesserThanStopPrice;
	}
	else if (select_num == 4)
	{
		a.ContingentCondition = THOST_FTDC_CC_LastPriceLesserEqualStopPrice;
	}
	else if (select_num == 5)
	{
		a.ContingentCondition = THOST_FTDC_CC_AskPriceGreaterThanStopPrice;
	}
	else if (select_num == 6)
	{
		a.ContingentCondition = THOST_FTDC_CC_AskPriceGreaterEqualStopPrice;
	}
	else if (select_num == 7)
	{
		a.ContingentCondition = THOST_FTDC_CC_AskPriceLesserThanStopPrice;
	}
	else if (select_num == 8)
	{
		a.ContingentCondition = THOST_FTDC_CC_AskPriceLesserEqualStopPrice;
	}
	else if (select_num == 9)
	{
		a.ContingentCondition = THOST_FTDC_CC_BidPriceGreaterThanStopPrice;
	}
	else if (select_num == 10)
	{
		a.ContingentCondition = THOST_FTDC_CC_BidPriceGreaterEqualStopPrice;
	}
	else if (select_num == 11)
	{
		a.ContingentCondition = THOST_FTDC_CC_BidPriceLesserThanStopPrice;
	}
	else if (select_num == 12)
	{
		a.ContingentCondition = THOST_FTDC_CC_BidPriceLesserEqualStopPrice;
	}
	a.StopPrice = atoi(stop_price.c_str());
	//itoa(a.StopPrice, const_cast<char *>(getConfig("config", "StopPrice").c_str()), 10);
	a.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	a.IsAutoSuspend = 0;
	strcpy_s(a.ExchangeID, g_chExchangeID);
	int ab = m_pUserApi->ReqOrderInsert(&a, nRequestID++);
	LOG((ab == 0) ? "������������......���ͳɹ�\n" : "������������......����ʧ�ܣ����=[%d]\n", ab);
}

///������������
void HandlerTrade::ReqOrderAction_Condition()
{
	CThostFtdcInputOrderActionField a = { 0 };
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.InvestorID, g_config.InvestorID);
	strcpy_s(a.InstrumentID, g_chInstrumentID);
	strcpy_s(a.UserID, g_config.UserID);
	if (chioce_action == 0)
	{
		a.FrontID = g_chFrontID;
		a.SessionID = g_chSessionID;
		strcpy_s(a.OrderRef, g_chOrderRef);
	}
	if (chioce_action == 1)
	{
		strcpy_s(a.OrderSysID, g_chOrderSysID);
	}
	strcpy_s(a.ExchangeID, g_chExchangeID);
	a.ActionFlag = THOST_FTDC_AF_Delete;
	int ab = m_pUserApi->ReqOrderAction(&a, nRequestID++);
	LOG((ab == 0) ? "������������......���ͳɹ�\n" : "������������......����ʧ�ܣ����=[%d]\n", ab);
}

//������ѯ�ı���
void HandlerTrade::ReqOrderAction_forqry(int action_num)
{
	CThostFtdcInputOrderActionField a = { 0 };
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.InvestorID, g_config.InvestorID);
	strcpy_s(a.UserID, g_config.UserID);

	strcpy_s(a.OrderSysID, vector_OrderSysID.at(action_num - 1).c_str());
	strcpy_s(a.ExchangeID, vector_ExchangeID.at(action_num - 1).c_str());
	strcpy_s(a.InstrumentID, vector_InstrumentID.at(action_num - 1).c_str());

	a.ActionFlag = THOST_FTDC_AF_Delete;
	int ab = m_pUserApi->ReqOrderAction(&a, nRequestID++);
	LOG((ab == 0) ? "������������......���ͳɹ�\n" : "������������......����ʧ�ܣ��������=[%d]\n", ab);
}

///�����ѯ�ɽ�
void HandlerTrade::ReqQryTrade()
{
	CThostFtdcQryTradeField a = { 0 };
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.InvestorID, g_config.InvestorID);
	string instr;
	instr.clear();
	LOG("�������Լ����(��������Ϊ��)\n");
	cin.ignore();
	getline(cin, instr);
	strcpy_s(a.InstrumentID, instr.c_str());

	string Exch;
	Exch.clear();
	LOG("�����뽻��������(��������Ϊ��)\n");
	//cin.ignore();
	getline(cin, Exch);
	strcpy_s(a.ExchangeID, Exch.c_str());
	/*strcpy_s(a.TradeID, "");
	strcpy_s(a.TradeTimeStart, "");
	strcpy_s(a.TradeTimeEnd, "");*/
	int b = m_pUserApi->ReqQryTrade(&a, nRequestID++);
	LOG((b == 0) ? "�����ѯ�ɽ�......���ͳɹ�\n" : "�����ѯ�ɽ�......����ʧ�ܣ��������=[%d]\n", b);
}

///�����ѯԤ��
void HandlerTrade::ReqQryParkedOrder()
{
	CThostFtdcQryParkedOrderField a = { 0 };
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.InvestorID, g_config.InvestorID);
	//strcpy_s(a.InstrumentID, g_chInstrumentID);
	strcpy_s(a.ExchangeID, g_chExchangeID);
	int ab = m_pUserApi->ReqQryParkedOrder(&a, nRequestID++);
	LOG((ab == 0) ? "�����ѯԤ��......���ͳɹ�\n" : "�����ѯԤ��......����ʧ�ܣ����=[%d]\n", ab);
}

//�����ѯ������Ԥ�񳷵�
void HandlerTrade::ReqQryParkedOrderAction()
{
	CThostFtdcQryParkedOrderActionField a = { 0 };
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.InvestorID, g_config.InvestorID);
	strcpy_s(a.InstrumentID, g_chInstrumentID);
	strcpy_s(a.ExchangeID, g_chExchangeID);
	int ab = m_pUserApi->ReqQryParkedOrderAction(&a, nRequestID++);
	LOG((ab == 0) ? "�����ѯ������Ԥ�񳷵�......���ͳɹ�\n" : "�����ѯ������Ԥ�񳷵�......����ʧ�ܣ����=[%d]\n", ab);
}

//�����ѯ�ʽ��˻�
void HandlerTrade::ReqQryTradingAccount()
{
	CThostFtdcQryTradingAccountField a = { 0 };
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.InvestorID, g_config.InvestorID);
	strcpy_s(a.CurrencyID, "CNY");
	int ab = m_pUserApi->ReqQryTradingAccount(&a, nRequestID++);
	LOG((ab == 0) ? "�����ѯ�ʽ��˻�......���ͳɹ�\n" : "�����ѯ�ʽ��˻�......����ʧ�ܣ����=[%d]\n", ab);
}

//�����ѯͶ���ֲ߳�
void HandlerTrade::ReqQryInvestorPosition()
{
	CThostFtdcQryInvestorPositionField a = { 0 };
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.InvestorID, g_config.InvestorID);
	int b = m_pUserApi->ReqQryInvestorPosition(&a, nRequestID++);
	LOG((b == 0) ? "�����ѯͶ���ֲ߳�......���ͳɹ�\n" : "�����ѯͶ���ֲ߳�......����ʧ�ܣ��������=[%d]\n", b);

	//���²�ѯ�ֲ֣�������ա�
	g_posList.clear();
}

//�����ѯͶ���ֲ֣߳���Ӧ
void HandlerTrade::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	
	//�洢����ȫ�ֱ���
	if (pInvestorPosition && pInvestorPosition->Position > 0) {
		g_posList.append(*pInvestorPosition);
	}
	/*
	LOG("<OnRspQryInvestorPosition>\n");
	if (pInvestorPosition && pInvestorPosition->Position>0) {
		//�洢����ȫ�ֱ���
		g_posList.append(*pInvestorPosition);
		LOG("\tInstrumentID [%s]\n", pInvestorPosition->InstrumentID);
		LOG("\tBrokerID [%s]\n", pInvestorPosition->BrokerID);
		LOG("\tInvestorID [%s]\n", pInvestorPosition->InvestorID);
		LOG("\tTradingDay [%s]\n", pInvestorPosition->TradingDay);
		LOG("\tExchangeID [%s]\n", pInvestorPosition->ExchangeID);
		LOG("\tInvestUnitID [%s]\n", pInvestorPosition->InvestUnitID);
		LOG("\tYdPosition [%d]\n", pInvestorPosition->YdPosition);
		LOG("\tPosition [%d]\n", pInvestorPosition->Position);
		LOG("\tLongFrozen [%d]\n", pInvestorPosition->LongFrozen);
		LOG("\tShortFrozen [%d]\n", pInvestorPosition->ShortFrozen);
		LOG("\tOpenVolume [%d]\n", pInvestorPosition->OpenVolume);
		LOG("\tCloseVolume [%d]\n", pInvestorPosition->CloseVolume);
		LOG("\tSettlementID [%d]\n", pInvestorPosition->SettlementID);
		LOG("\tCombPosition [%d]\n", pInvestorPosition->CombPosition);
		LOG("\tCombLongFrozen [%d]\n", pInvestorPosition->CombLongFrozen);
		LOG("\tCombShortFrozen [%d]\n", pInvestorPosition->CombShortFrozen);
		LOG("\tTodayPosition [%d]\n", pInvestorPosition->TodayPosition);
		LOG("\tStrikeFrozen [%d]\n", pInvestorPosition->StrikeFrozen);
		LOG("\tAbandonFrozen [%d]\n", pInvestorPosition->AbandonFrozen);
		LOG("\tYdStrikeFrozen [%d]\n", pInvestorPosition->YdStrikeFrozen);
		LOG("\tPosiDirection [%c]\n", pInvestorPosition->PosiDirection);
		LOG("\tHedgeFlag [%c]\n", pInvestorPosition->HedgeFlag);
		LOG("\tPositionDate [%c]\n", pInvestorPosition->PositionDate);
		LOG("\tLongFrozenAmount [%.8lf]\n", pInvestorPosition->LongFrozenAmount);
		LOG("\tShortFrozenAmount [%.8lf]\n", pInvestorPosition->ShortFrozenAmount);
		LOG("\tOpenAmount [%.8lf]\n", pInvestorPosition->OpenAmount);
		LOG("\tCloseAmount [%.8lf]\n", pInvestorPosition->CloseAmount);
		LOG("\tPositionCost [%.8lf]\n", pInvestorPosition->PositionCost);
		LOG("\tPreMargin [%.8lf]\n", pInvestorPosition->PreMargin);
		LOG("\tUseMargin [%.8lf]\n", pInvestorPosition->UseMargin);
		LOG("\tFrozenMargin [%.8lf]\n", pInvestorPosition->FrozenMargin);
		LOG("\tFrozenCash [%.8lf]\n", pInvestorPosition->FrozenCash);
		LOG("\tFrozenCommission [%.8lf]\n", pInvestorPosition->FrozenCommission);
		LOG("\tCashIn [%.8lf]\n", pInvestorPosition->CashIn);
		LOG("\tCommission [%.8lf]\n", pInvestorPosition->Commission);
		LOG("\tCloseProfit [%.8lf]\n", pInvestorPosition->CloseProfit);
		LOG("\tPositionProfit [%.8lf]\n", pInvestorPosition->PositionProfit);
		LOG("\tPreSettlementPrice [%.8lf]\n", pInvestorPosition->PreSettlementPrice);
		LOG("\tSettlementPrice [%.8lf]\n", pInvestorPosition->SettlementPrice);
		LOG("\tOpenCost [%.8lf]\n", pInvestorPosition->OpenCost);
		LOG("\tExchangeMargin [%.8lf]\n", pInvestorPosition->ExchangeMargin);
		LOG("\tCloseProfitByDate [%.8lf]\n", pInvestorPosition->CloseProfitByDate);
		LOG("\tCloseProfitByTrade [%.8lf]\n", pInvestorPosition->CloseProfitByTrade);
		LOG("\tMarginRateByMoney [%.8lf]\n", pInvestorPosition->MarginRateByMoney);
		LOG("\tMarginRateByVolume [%.8lf]\n", pInvestorPosition->MarginRateByVolume);
		LOG("\tStrikeFrozenAmount [%.8lf]\n", pInvestorPosition->StrikeFrozenAmount);
		LOG("\tPositionCostOffset [%.8lf]\n", pInvestorPosition->PositionCostOffset);
	}
	if (pRspInfo) {
		LOG("\tErrorMsg [%s]\n", pRspInfo->ErrorMsg);
		LOG("\tErrorID [%d]\n", pRspInfo->ErrorID);
	}
	LOG("\tnRequestID [%d]\n", nRequestID);
	LOG("\tbIsLast [%d]\n", bIsLast);
	LOG("</OnRspQryInvestorPosition>\n");
	*/

	if (bIsLast) {
		SetEvent(g_qEvent);
		//�ֲֻ�ȡ��ϣ�֪ͨ���³ֱֲ��
		emit w_main->signal_RefreshPosTable();
		qDebug() << "ddd";
	}
};



//�����ѯͶ���ֲ߳���ϸ
void HandlerTrade::ReqQryInvestorPositionDetail()
{
	CThostFtdcQryInvestorPositionDetailField a = { 0 };
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.InvestorID, g_config.InvestorID);
	string instr;
	instr.clear();
	cin.ignore();
	LOG("�������Լ����(��������Ϊ��)\n");
	getline(cin, instr);
	strcpy_s(a.InstrumentID, instr.c_str());
	string exch;
	exch.clear();
	cin.ignore();
	LOG("�����뽻��������(��������Ϊ��)��\n");
	getline(cin, exch);
	strcpy_s(a.ExchangeID, exch.c_str());
	//strcpy_s(a.InstrumentID, g_chInstrumentID);
	int b = m_pUserApi->ReqQryInvestorPositionDetail(&a, nRequestID++);
	LOG((b == 0) ? "�����ѯͶ���ֲ߳���ϸ......���ͳɹ�\n" : "�����ѯͶ���ֲ߳���ϸ......����ʧ�ܣ��������=[%d]\n", b);
}

//�����ѯ��������֤����
void HandlerTrade::ReqQryExchangeMarginRate()
{
	CThostFtdcQryExchangeMarginRateField a = { 0 };
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.InstrumentID, g_chInstrumentID);
	a.HedgeFlag = THOST_FTDC_HF_Speculation;//Ͷ��
	int b = m_pUserApi->ReqQryExchangeMarginRate(&a, nRequestID++);
	LOG((b == 0) ? "�����ѯ��������֤����......���ͳɹ�\n" : "�����ѯ��������֤����......����ʧ�ܣ��������=[%d]\n", b);
}

//�����ѯ��Լ��֤����
void HandlerTrade::ReqQryInstrumentMarginRate()
{
	CThostFtdcQryInstrumentMarginRateField a = { 0 };
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.InvestorID, g_config.InvestorID);
	strcpy_s(a.InstrumentID, g_chInstrumentID);
	a.HedgeFlag = THOST_FTDC_HF_Speculation;//Ͷ��
	int b = m_pUserApi->ReqQryInstrumentMarginRate(&a, nRequestID++);
	LOG((b == 0) ? "�����ѯ��Լ��֤����......���ͳɹ�\n" : "�����ѯ��Լ��֤����......����ʧ�ܣ��������=[%d]\n", b);
}

//�����ѯ��Լ��������
void HandlerTrade::ReqQryInstrumentCommissionRate()
{
	CThostFtdcQryInstrumentCommissionRateField a = { 0 };
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.InvestorID, g_config.InvestorID);
	strcpy_s(a.InstrumentID, g_chInstrumentID);
	int b = m_pUserApi->ReqQryInstrumentCommissionRate(&a, nRequestID++);
	LOG((b == 0) ? "�����ѯ��Լ��������......���ͳɹ�\n" : "�����ѯ��Լ��������......����ʧ�ܣ��������=[%d]\n", b);
}

//�����ѯ�����̺�Լ��������
void HandlerTrade::ReqQryMMInstrumentCommissionRate()
{
	CThostFtdcQryMMInstrumentCommissionRateField a = { 0 };
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.InvestorID, g_config.InvestorID);
	strcpy_s(a.InstrumentID, g_chInstrumentID);
	int b = m_pUserApi->ReqQryMMInstrumentCommissionRate(&a, nRequestID++);
	LOG((b == 0) ? "�����ѯ�����̺�Լ��������......���ͳɹ�\n" : "�����ѯ�����̺�Լ��������......����ʧ�ܣ��������=[%d]\n", b);
}

//�����ѯ��������Ȩ��Լ������
void HandlerTrade::ReqQryMMOptionInstrCommRate()
{
	CThostFtdcQryMMOptionInstrCommRateField a = { 0 };
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.InvestorID, g_config.InvestorID);
	strcpy_s(a.InstrumentID, g_chInstrumentID);
	int b = m_pUserApi->ReqQryMMOptionInstrCommRate(&a, nRequestID++);
	LOG((b == 0) ? "�����ѯ��������Ȩ��Լ������......���ͳɹ�\n" : "�����ѯ��������Ȩ��Լ������......����ʧ�ܣ��������=[%d]\n", b);
}

//�����ѯ����������
void HandlerTrade::ReqQryInstrumentOrderCommRate()
{
	CThostFtdcQryInstrumentOrderCommRateField a = { 0 };
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.InvestorID, g_config.InvestorID);
	strcpy_s(a.InstrumentID, g_chInstrumentID);
	int b = m_pUserApi->ReqQryInstrumentOrderCommRate(&a, nRequestID++);
	LOG((b == 0) ? "�����ѯ����������......���ͳɹ�\n" : "�����ѯ����������......����ʧ�ܣ��������=[%d]\n", b);
}

//�����ѯ��Ȩ��Լ������
void HandlerTrade::ReqQryOptionInstrCommRate()
{
	CThostFtdcQryOptionInstrCommRateField a = { 0 };
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.InvestorID, g_config.InvestorID);
	string Inst;
	string Exch;
	string InvestUnit;
	LOG("�������Լ����:(������Ϊ��)");
	cin >> Inst;
	LOG("�����뽻��������:(������Ϊ��)");
	cin >> Exch;
	LOG("������Ͷ���ߵ�Ԫ����:(������Ϊ��)");
	cin >> InvestUnit;
	strcpy_s(a.InstrumentID, Inst.c_str());
	strcpy_s(a.ExchangeID, Exch.c_str());
	strcpy_s(a.InvestUnitID, InvestUnit.c_str());
	int b = m_pUserApi->ReqQryOptionInstrCommRate(&a, nRequestID++);
	LOG((b == 0) ? "�����ѯ��Ȩ��Լ������......���ͳɹ�\n" : "�����ѯ��Ȩ��Լ������......����ʧ�ܣ��������=[%d]\n", b);
}

//�����ѯ��Լ
void HandlerTrade::ReqQryInstrument()
{
	CThostFtdcQryInstrumentField a = { 0 };
	strcpy_s(a.ExchangeID, g_chExchangeID);
	strcpy_s(a.InstrumentID, g_chInstrumentID);
	//strcpy_s(a.ExchangeInstID,"");
	//strcpy_s(a.ProductID, "m");

	int b = m_pUserApi->ReqQryInstrument(&a, nRequestID++);
	LOG((b == 0) ? "�����ѯ��Լ......���ͳɹ�\n" : "�����ѯ��Լ......����ʧ�ܣ��������=[%d]\n", b);
}


///�����ѯ��Լ��Ӧ
void HandlerTrade::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{

	LOG("<OnRspQryInstrument>\n");
	if (pInstrument)
	{
		md_InstrumentID.push_back(pInstrument->InstrumentID);
		LOG("\tInstrumentID [%s]\n", pInstrument->InstrumentID);
		LOG("\tExchangeID [%s]\n", pInstrument->ExchangeID);
		LOG("\tInstrumentName [%s]\n", gbk2utf8(pInstrument->InstrumentName));
		LOG("\tExchangeInstID [%s]\n", pInstrument->ExchangeInstID);
		LOG("\tProductID [%s]\n", pInstrument->ProductID);
		LOG("\tCreateDate [%s]\n", pInstrument->CreateDate);

		//������Լ��Ϣ��
		g_instMap[pInstrument->InstrumentID] = *pInstrument;


		/*LOG("\tOpenDate [%s]\n", pInstrument->OpenDate);
		LOG("\tExpireDate [%s]\n", pInstrument->ExpireDate);
		LOG("\tStartDelivDate [%s]\n", pInstrument->StartDelivDate);
		LOG("\tEndDelivDate [%s]\n", pInstrument->EndDelivDate);
		LOG("\tUnderlyingInstrID [%s]\n", pInstrument->UnderlyingInstrID);
		LOG("\tDeliveryYear [%d]\n", pInstrument->DeliveryYear);
		LOG("\tDeliveryMonth [%d]\n", pInstrument->DeliveryMonth);
		LOG("\tMaxMarketOrderVolume [%d]\n", pInstrument->MaxMarketOrderVolume);
		LOG("\tMinMarketOrderVolume [%d]\n", pInstrument->MinMarketOrderVolume);
		LOG("\tMaxLimitOrderVolume [%d]\n", pInstrument->MaxLimitOrderVolume);
		LOG("\tMinLimitOrderVolume [%d]\n", pInstrument->MinLimitOrderVolume);
		LOG("\tVolumeMultiple [%d]\n", pInstrument->VolumeMultiple);
		LOG("\tIsTrading [%d]\n", pInstrument->IsTrading);
		LOG("\tProductClass [%c]\n", pInstrument->ProductClass);
		LOG("\tInstLifePhase [%c]\n", pInstrument->InstLifePhase);
		LOG("\tPositionType [%c]\n", pInstrument->PositionType);
		LOG("\tPositionDateType [%c]\n", pInstrument->PositionDateType);
		LOG("\tMaxMarginSideAlgorithm [%c]\n", pInstrument->MaxMarginSideAlgorithm);
		LOG("\tOptionsType [%c]\n", pInstrument->OptionsType);
		LOG("\tCombinationType [%c]\n", pInstrument->CombinationType);
		LOG("\tPriceTick [%.8lf]\n", pInstrument->PriceTick);
		LOG("\tLongMarginRatio [%.8lf]\n", pInstrument->LongMarginRatio);
		LOG("\tShortMarginRatio [%.8lf]\n", pInstrument->ShortMarginRatio);
		LOG("\tStrikePrice [%.8lf]\n", pInstrument->StrikePrice);
		LOG("\tUnderlyingMultiple [%.8lf]\n", pInstrument->UnderlyingMultiple);*/
	}
	if (pRspInfo)
	{
		LOG("\tErrorMsg [%s]\n", pRspInfo->ErrorMsg);
		LOG("\tErrorID [%d]\n", pRspInfo->ErrorID);
	}
	LOG("\tnRequestID [%d]\n", nRequestID);
	LOG("\tbIsLast [%d]\n", bIsLast);
	LOG("</OnRspQryInstrument>\n");
	if (bIsLast)
	{
		SetEvent(g_qEvent);
	}
}


//�����ѯת����ˮ
void HandlerTrade::ReqQryTransferSerial()
{
	CThostFtdcQryTransferSerialField a = { 0 };
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.AccountID, g_config.InvestorID);
cir1:int bankid;
	LOG("����������Ҫ�Ĳ�ѯ������\n");
	LOG("1.��������\n");
	LOG("2.ũҵ����\n");
	LOG("3.�й�����\n");
	LOG("5.��ͨ����\n");
	LOG("6.��������\n");
	LOG("7.��ҵ����\n");
	LOG("8.�ַ�����\n");
	LOG("9.��������\n");
	LOG("10.�������\n");
	LOG("11.��������\n");
	LOG("12.�������\n");
	LOG("13.ƽ������\n");
	LOG("14.ũ������\n");
	LOG("15.��չ����\n");
	LOG("16.�㷢����\n");
	cin >> bankid;
	if (bankid == 1 | 2 | 3 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16)
	{
		//strcpy_s(a.BankID, itoa(bankid, a.BankID, 10));///���д���
		itoa(bankid, a.BankID, 10);
	}
	else
	{
		LOG("�������������д��롣\n");
		goto cir1;
	}
	int choos;
curr:LOG("��������ִ���\t1.CNY\t2.USD\n");
	cin >> choos;
	switch (choos)
	{
	case 1:
		strcpy_s(a.CurrencyID, "CNY");
		break;
	case 2:
		strcpy_s(a.CurrencyID, "USD");
		break;
	default:
		LOG("��������ȷ�����\n");
		_getch();
		goto curr;
	}
	int b = m_pUserApi->ReqQryTransferSerial(&a, nRequestID++);
	LOG((b == 0) ? "�����ѯת����ˮ......���ͳɹ�\n" : "�����ѯת����ˮ......����ʧ�ܣ��������=[%d]\n", b);
}

//�����ѯ��Ʒ
void HandlerTrade::ReqQryProduct()
{
	CThostFtdcQryProductField a = { 0 };
	strcpy_s(a.ProductID, "sc");
	a.ProductClass = THOST_FTDC_PC_Futures;
	strcpy_s(a.ExchangeID, g_chExchangeID);
	m_pUserApi->ReqQryProduct(&a, nRequestID++);
}

//�����ѯת������
void HandlerTrade::ReqQryTransferBank()
{
	CThostFtdcQryTransferBankField a = { 0 };
	strcpy_s(a.BankID, "3");
	int b = m_pUserApi->ReqQryTransferBank(&a, nRequestID++);
	LOG((b == 0) ? "�����ѯת������......���ͳɹ�\n" : "�����ѯת������......����ʧ�ܣ��������=[%d]\n", b);
}

//�����ѯ����֪ͨ
void HandlerTrade::ReqQryTradingNotice()
{
	CThostFtdcQryTradingNoticeField a = { 0 };
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.InvestorID, g_config.InvestorID);
	int b = m_pUserApi->ReqQryTradingNotice(&a, nRequestID++);
	LOG((b == 0) ? "�����ѯ����֪ͨ......���ͳɹ�\n" : "�����ѯ����֪ͨ......����ʧ�ܣ��������=[%d]\n", b);
}

//�����ѯ���ױ���
void HandlerTrade::ReqQryTradingCode()
{
	CThostFtdcQryTradingCodeField a = { 0 };
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.InvestorID, g_config.InvestorID);
	strcpy_s(a.ExchangeID, g_chExchangeID);
	a.ClientIDType = THOST_FTDC_CIDT_Speculation;
	int b = m_pUserApi->ReqQryTradingCode(&a, nRequestID++);
	LOG((b == 0) ? "�����ѯ���ױ���......���ͳɹ�\n" : "�����ѯ���ױ���......����ʧ�ܣ��������=[%d]\n", b);
}

//�����ѯ������Ϣȷ��
void HandlerTrade::ReqQrySettlementInfoConfirm()
{
	CThostFtdcQrySettlementInfoConfirmField a = { 0 };
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.InvestorID, g_config.InvestorID);
	//strcpy_s(a.AccountID, g_config.InvestorID);
	strcpy_s(a.CurrencyID, "CNY");
	int b = m_pUserApi->ReqQrySettlementInfoConfirm(&a, nRequestID++);
	LOG((b == 0) ? "�����ѯ������Ϣȷ��......���ͳɹ�\n" : "�����ѯ������Ϣȷ��......����ʧ�ܣ��������=[%d]\n", b);
}

void HandlerTrade::OnRspQrySettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	CTraderSpi::OnRspQrySettlementInfoConfirm(pSettlementInfoConfirm, pRspInfo, nRequestID, bIsLast);
	if (!pSettlementInfoConfirm  && !pRspInfo) {//δȷ�Ͻ��㵥ʱ������ʾΪ�յġ�
		LOG("������δȷ�ϡ�ȷ�Ͻ��㵥����");
		ReqSettlementInfoConfirm();
	}
	if (bIsLast) {
		SetEvent(g_hEvent);
	}
}


//�����ѯ��Ʒ��
void HandlerTrade::ReqQryProductGroup()
{
	CThostFtdcQryProductGroupField a = { 0 };

}

//�����ѯͶ���ߵ�Ԫ
void HandlerTrade::ReqQryInvestUnit()
{
	CThostFtdcQryInvestUnitField a = { 0 };
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	//strcpy_s(a.InvestorID, "00402");
	//strcpy_s(a.InvestorID, g_config.InvestorID);
	int b = m_pUserApi->ReqQryInvestUnit(&a, nRequestID++);
	LOG((b == 0) ? "�����ѯͶ���ߵ�Ԫ......���ͳɹ�\n" : "�����ѯͶ���ߵ�Ԫ......����ʧ�ܣ��������=[%d]\n", b);
}

//�����ѯ���͹�˾���ײ���
void HandlerTrade::ReqQryBrokerTradingParams()
{
	CThostFtdcQryBrokerTradingParamsField a = { 0 };
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.InvestorID, g_config.InvestorID);
	strcpy_s(a.CurrencyID, "CNY");
	int b = m_pUserApi->ReqQryBrokerTradingParams(&a, nRequestID++);
	LOG((b == 0) ? "�����ѯ���͹�˾���ײ���......���ͳɹ�\n" : "�����ѯ���͹�˾���ײ���......����ʧ�ܣ��������=[%d]\n", b);
}

//�����ѯѯ��
void HandlerTrade::ReqQryForQuote()
{
	CThostFtdcQryForQuoteField a = { 0 };
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.InvestorID, g_config.InvestorID);
	strcpy_s(a.InstrumentID, g_chInstrumentID);
	strcpy_s(a.ExchangeID, g_chExchangeID);
	strcpy_s(a.InsertTimeStart, "");
	strcpy_s(a.InsertTimeEnd, "");
	strcpy_s(a.InvestUnitID, "");
	int b = m_pUserApi->ReqQryForQuote(&a, nRequestID++);
	LOG((b == 0) ? "�����ѯѯ��......���ͳɹ�\n" : "�����ѯѯ��......����ʧ�ܣ��������=[%d]\n", b);
}

//�����ѯ����
void HandlerTrade::ReqQryQuote()
{
	CThostFtdcQryQuoteField a = { 0 };
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.InvestorID, g_config.InvestorID);
	strcpy_s(a.InstrumentID, g_chInstrumentID);
	strcpy_s(a.ExchangeID, g_chExchangeID);
	strcpy_s(a.QuoteSysID, "");
	strcpy_s(a.InsertTimeStart, "");
	strcpy_s(a.InsertTimeEnd, "");
	strcpy_s(a.InvestUnitID, "");
	int b = m_pUserApi->ReqQryQuote(&a, nRequestID++);
	LOG((b == 0) ? "�����ѯѯ��......���ͳɹ�\n" : "�����ѯѯ��......����ʧ�ܣ��������=[%d]\n", b);
}

///ѯ��¼������
void HandlerTrade::ReqForQuoteInsert()
{
	CThostFtdcInputForQuoteField a = { 0 };
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.InvestorID, g_config.InvestorID);
	strcpy_s(a.InstrumentID, g_chInstrumentID);
	//strcpy_s(a.ForQuoteRef, "");
	strcpy_s(a.UserID, g_config.UserID);
	strcpy_s(a.ExchangeID, g_chExchangeID);
	//strcpy_s(a.InvestUnitID, "");
	//strcpy_s(a.IPAddress, "");
	//strcpy_s(a.MacAddress, "");
	int b = m_pUserApi->ReqForQuoteInsert(&a, nRequestID++);
	LOG((b == 0) ? "ѯ��¼������......���ͳɹ�\n" : "ѯ��¼������......����ʧ�ܣ��������=[%d]\n", b);
}

///�����̱���¼������
void HandlerTrade::ReqQuoteInsert()
{
choose:int choose_Flag;
	LOG("��ȷ�Ͽ�ƽ��־\t1.����\t2.ƽ��\n");
	cin >> choose_Flag;

	if (choose_Flag != 1 && choose_Flag != 2)
	{
		LOG("������ѡ��ƽ��־\n");
		_getch();
		choose_Flag = NULL;
		goto choose;
	}

	int price_bid;
	LOG("����������۸�\n");
	cin >> price_bid;

	int price_ask;
	LOG("������������۸�\n");
	cin >> price_ask;
	LOG("��������Ĭ����1��\n");
	string quoteref;
	LOG("������quoteref��ţ�\n");
	cin >> quoteref;
	string AskOrderRef;
	string BidOrderRef;
	LOG("������AskOrderRef���:\n");
	cin >> AskOrderRef;
	LOG("������BidOrderRef���:\n");
	cin >> BidOrderRef;
	_getch();
	CThostFtdcInputQuoteField t = { 0 };
	strcpy_s(t.BrokerID, g_config.server.BrokerID);
	strcpy_s(t.InvestorID, g_config.InvestorID);
	strcpy_s(t.InstrumentID, g_chInstrumentID);
	strcpy_s(t.ExchangeID, g_chExchangeID);

	strcpy_s(t.QuoteRef, quoteref.c_str());
	strcpy_s(t.UserID, g_config.UserID);
	t.AskPrice = price_ask;
	t.BidPrice = price_bid;
	t.AskVolume = 1;
	t.BidVolume = 1;
	if (choose_Flag == 1)
	{
		t.AskOffsetFlag = THOST_FTDC_OF_Open;///����ƽ��־
		t.BidOffsetFlag = THOST_FTDC_OF_Open;///��ƽ��־
	}
	else if (choose_Flag == 2)
	{
		t.AskOffsetFlag = THOST_FTDC_OF_Close;///����ƽ��־
		t.BidOffsetFlag = THOST_FTDC_OF_Close;///��ƽ��־
	}
	t.AskHedgeFlag = THOST_FTDC_HF_Speculation;///��Ͷ���ױ���־
	t.BidHedgeFlag = THOST_FTDC_HF_Speculation;///��Ͷ���ױ���־

	strcpy_s(t.AskOrderRef, AskOrderRef.c_str());///��������������
	strcpy_s(t.BidOrderRef, BidOrderRef.c_str());///�����򱨵�����
	//strcpy_s(t.ForQuoteSysID, "");///Ӧ�۱��
	//strcpy_s(t.InvestUnitID, "1");///Ͷ�ʵ�Ԫ����
	int a = m_pUserApi->ReqQuoteInsert(&t, 1);
	LOG((a == 0) ? "�����̱���¼������......���ͳɹ�\n" : "�����̱���¼������......����ʧ�ܣ��������=[%d]\n", a);
}

///����֪ͨ
void HandlerTrade::OnRtnQuote(CThostFtdcQuoteField *pQuote)
{
	if (pQuote && strcmp(pQuote->InvestorID, g_config.InvestorID) != 0)
	{
		return;
	}
	else
	{
		CTraderSpi::OnRtnQuote(pQuote);
		//SetEvent(g_hEvent);
	}
}

//���۳���
void HandlerTrade::ReqQuoteAction()
{
	CThostFtdcInputQuoteActionField t = { 0 };
	strcpy_s(t.BrokerID, g_config.server.BrokerID);
	strcpy_s(t.InvestorID, "00404");
	//strcpy_s(t.UserID, g_config.UserID);
	strcpy_s(t.ExchangeID, "SHFE");
	strcpy_s(t.QuoteRef, "           8");
	t.FrontID = 7;
	t.SessionID = 1879781311;
	t.ActionFlag = THOST_FTDC_AF_Delete;
	strcpy_s(t.InstrumentID, "cu1905C55000");
	int a = m_pUserApi->ReqQuoteAction(&t, 1);
	printf("m_pUserApi->ReqQuoteAction = [%d]", a);
}

//��ѯ��󱨵���������
void HandlerTrade::ReqQueryMaxOrderVolume()
{
	CThostFtdcQueryMaxOrderVolumeField a = { 0 };
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.InvestorID, g_config.InvestorID);
	strcpy_s(a.InstrumentID, g_chInstrumentID);
	a.Direction = THOST_FTDC_D_Buy;
	a.OffsetFlag = THOST_FTDC_OF_Open;
	a.HedgeFlag = THOST_FTDC_HF_Speculation;
	a.MaxVolume = 1;
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	int b = m_pUserApi->ReqQueryMaxOrderVolume(&a, nRequestID++);
	LOG((b == 0) ? "��ѯ��󱨵���������......���ͳɹ�\n" : "��ѯ��󱨵���������......����ʧ�ܣ��������=[%d]\n", b);
}

//�����ѯ��������û�����
void HandlerTrade::ReqQueryCFMMCTradingAccountToken()
{
	CThostFtdcQueryCFMMCTradingAccountTokenField a = { 0 };

}



///������������ر�
void HandlerTrade::OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo)
{
	if (pOrderAction && strcmp(pOrderAction->InvestorID, g_config.InvestorID) != 0)
	{
		return;
	}
	else
	{
		CTraderSpi::OnErrRtnOrderAction(pOrderAction, pRspInfo);
		SetEvent(g_hEvent);
	}
}

///����¼��������Ӧ�������CTP��̨���صģ�������������ʱ���Ż���ʾ�����
void HandlerTrade::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast)
{

	if (pInputOrder && strcmp(pInputOrder->InvestorID, g_config.InvestorID) != 0)
	{
		return;
	}
	else
	{
		CTraderSpi::OnRspOrderInsert(pInputOrder, pRspInfo, nRequestID, bIsLast);
	}
}

///����¼�����ر�
void HandlerTrade::OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo)
{
	if (pInputOrder && strcmp(pInputOrder->InvestorID, g_config.InvestorID) != 0)
	{
		return;
	}
	else
	{
		CTraderSpi::OnErrRtnOrderInsert(pInputOrder, pRspInfo);
		SetEvent(g_hEvent);
	}
}

//�Զ����µ� ��
int HandlerTrade::ReqOrderInsert_Whl(QString inst, QString direction, QString offset, double price, double vol, QString OrderRef, QString ArbOrderId, int LegId) {
	CThostFtdcInputOrderField ordInput = { 0 };
	strcpy_s(ordInput.BrokerID, g_config.server.BrokerID); //brokenId.
	strcpy_s(ordInput.InvestorID, g_config.InvestorID); //Ͷ����ID��
	strcpy_s(ordInput.UserID, g_config.UserID); //�û�ID��
	strcpy_s(ordInput.InstrumentID, inst.toStdString().c_str()); //��ԼID.
	strcpy_s(ordInput.OrderRef, OrderRef.toStdString().c_str());//�Զ��嶩���š�
	//1.�� THOST_FTDC_D_Buy; 2.�� THOST_FTDC_D_Sell
	ordInput.Direction = direction == QStr("����") ? THOST_FTDC_D_Buy : THOST_FTDC_D_Sell; //��������
	//1.���� THOST_FTDC_OF_Open 2.ƽ��THOST_FTDC_OF_Close 3.ǿƽTHOST_FTDC_OF_ForceClose 4.ƽ��THOST_FTDC_OF_CloseToday 5.ƽ��THOST_FTDC_OF_CloseYesterday 6.ǿ��THOST_FTDC_OF_ForceOff 7.����ǿƽTHOST_FTDC_OF_LocalForceClose");
	ordInput.CombOffsetFlag[0] = offset == QStr("����") ? THOST_FTDC_OF_Open : THOST_FTDC_OF_Close; //��ƽ��־��
	strcpy_s(ordInput.CombHedgeFlag, "1"); //Ͷ���� 
	ordInput.OrderPriceType = THOST_FTDC_OPT_LimitPrice; //�޼۵���
	ordInput.LimitPrice = price; //�۸�
	ordInput.VolumeTotalOriginal = 1; //������
	ordInput.TimeCondition = THOST_FTDC_TC_GFD;///������Ч
	ordInput.VolumeCondition = THOST_FTDC_VC_AV;///ȫ������
	ordInput.MinVolume = 1;
	ordInput.ContingentCondition = THOST_FTDC_CC_Immediately; //����������
	ordInput.StopPrice = 0;
	ordInput.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	ordInput.IsAutoSuspend = 0;
	strcpy_s(ordInput.ExchangeID, g_chExchangeID);
	//0������ɹ��� -1����ʾ��������ʧ�ܣ�- 2����ʾδ�������󳬹�������� - 3����ʾÿ�뷢�������������������
	int a = api->ReqOrderInsert(&ordInput, 1);

	//������ͨ������Ϣ��
	Order ord = { 0 }; 
	ord.UserID = QString(g_config.UserID);
	ord.OrderRef = OrderRef;
	ord.InstrumentID = inst;
	ord.Price = price;
	ord.VolumeTotalOriginal = vol;
	ord.VolumeTotal = vol;
	ord.Direction = direction;
	ord.Offset = offset;
	ord.OrderSource = "ctpArb";
	ord.ArbOrderId = ArbOrderId;
	ord.LegId = LegId;
	ord.OrderStatus = QChar::fromLatin1('a'); //����δ֪��
	QDateTime tm = QDateTime::currentDateTime();//��ȡϵͳ���ڵ�ʱ��
	ord.InsertDate = tm.toString("yyyy.MM.dd");
	ord.InsertTime = tm.toString("hh:mm:ss");
	//���汨����Ϣ��
	g_orderMap.insert(OrderRef, ord);
	emit w_main->signal_RefreshOrdTable(ord.OrderRef, "add");
	
	return a;
}

///����֪ͨ
void HandlerTrade::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
	if (pOrder && strcmp(pOrder->InvestorID, g_config.InvestorID) != 0)
	{
		return;
	}
	else
	{
		CTraderSpi::OnRtnOrder(pOrder);
		strcpy_s(g_chOrderSysID, pOrder->OrderSysID);
		g_chFrontID = pOrder->FrontID;
		g_chSessionID = pOrder->SessionID;
		strcpy_s(g_chOrderRef, pOrder->OrderRef);
		strcpy_s(g_chExchangeID, pOrder->ExchangeID);
		if (pOrder->OrderStatus == THOST_FTDC_OST_AllTraded)///ȫ���ɽ�
		{
			LOG("����ȫ���ɽ�\n\n");
			//SetEvent(g_hEvent);
		}if (pOrder->OrderStatus == THOST_FTDC_OST_PartTradedQueueing)///���ֳɽ����ڶ�����
		{
			LOG("���ֳɽ����ڶ�����\n\n");
		}if (pOrder->OrderStatus == THOST_FTDC_OST_PartTradedNotQueueing)///���ֳɽ����ڶ�����
		{
			LOG("���ֳɽ����ڶ�����\n\n");
		}if (pOrder->OrderStatus == THOST_FTDC_OST_NoTradeQueueing)///δ�ɽ����ڶ�����
		{
			chioce_action = 0;
			LOG("δ�ɽ����ڶ�����\n\n");
		}if (pOrder->OrderStatus == THOST_FTDC_OST_NoTradeNotQueueing)///δ�ɽ����ڶ�����
		{
			LOG("δ�ɽ����ڶ�����\n\n");
		}if (pOrder->OrderStatus == THOST_FTDC_OST_Canceled)///����
		{
			LOG("����\n\n");
			//SetEvent(g_hEvent);
		}if (pOrder->OrderStatus == THOST_FTDC_OST_Unknown)///δ֪
		{
			LOG("δ֪\n\n");
		}if (pOrder->OrderStatus == THOST_FTDC_OST_NotTouched)///��δ����
		{
			chioce_action = 1;
			LOG("��δ����\n\n");
		}if (pOrder->OrderStatus == THOST_FTDC_OST_Touched)///�Ѵ���
		{
			LOG("�Ѵ���\n\n");
		}

		//���±���״̬��ʣ������
		if (g_orderMap.contains(pOrder->OrderRef)) {
			Order* ord = &g_orderMap[QString(pOrder->OrderRef)];
			ord->VolumeTotal = pOrder->VolumeTotal;
			ord->OrderStatus = pOrder->OrderStatus;

			//����������״̬��ʣ������
			if (g_arbOrderMap.contains(ord->ArbOrderId)) {
				g_arbOrderMap[ord->ArbOrderId].OrdLegList[ord->LegId].DealVol = pOrder->VolumeTotal;
				g_arbOrderMap[ord->ArbOrderId].OrdLegList[ord->LegId].Status = pOrder->OrderStatus;
			}
			//�����źš�
			emit w_main->signal_RefreshOrdTable(ord->OrderRef, "update");
		}

		//����һ���гɽ�����Ҫ��ѯ�ֲ֣�
		if (pOrder->OrderStatus == THOST_FTDC_OST_AllTraded || pOrder->OrderStatus == THOST_FTDC_OST_PartTradedQueueing || pOrder->OrderStatus == THOST_FTDC_OST_NoTradeQueueing)///ȫ���ɽ� ///���ֳɽ�
		{
			LOG("�гɽ�����ѯ�ֲ�");
			ReqQryInvestorPosition();
			//SetEvent(g_hEvent);
		}
	}
}

//�ɽ��ر���
void HandlerTrade::OnRtnTrade(CThostFtdcTradeField *pTrade) {
	if (pTrade && strcmp(pTrade->InvestorID, g_config.InvestorID) != 0) {
		return;
	} else {
		CTraderSpi::OnRtnTrade(pTrade);
		//���±����ɽ�����
		if (g_orderMap.contains(pTrade->OrderRef)) {
			Order* ord = &g_orderMap[QString(pTrade->OrderRef)];
			int vol_deal = ord->VolumeTotalOriginal - ord->VolumeTotal;
			ord->AvgPrice = (ord->AvgPrice * vol_deal + pTrade->Price * pTrade->Volume) / (vol_deal + pTrade->Volume);

			//���������ȳɽ����ۡ�
			if (g_arbOrderMap.contains(ord->ArbOrderId)) {
				g_arbOrderMap[ord->ArbOrderId].OrdLegList[ord->LegId].AvgPrice = ord->AvgPrice;
			}
			//�����źš�
			emit w_main->signal_RefreshOrdTable(ord->OrderRef, "update");
		}


		
	}
}

///ɾ��Ԥ����Ӧ
void HandlerTrade::OnRspRemoveParkedOrder(CThostFtdcRemoveParkedOrderField *pRemoveParkedOrder, CThostFtdcRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast)
{
	if (pRemoveParkedOrder && strcmp(pRemoveParkedOrder->InvestorID, g_config.InvestorID) != 0)
	{
		return;
	}
	else
	{
		strcpy_s(g_chParkedOrderID1, pRemoveParkedOrder->ParkedOrderID);
		CTraderSpi::OnRspRemoveParkedOrder(pRemoveParkedOrder, pRspInfo, nRequestID, bIsLast);
		SetEvent(g_hEvent);
	}
}

///ɾ��Ԥ�񳷵���Ӧ
void HandlerTrade::OnRspRemoveParkedOrderAction(CThostFtdcRemoveParkedOrderActionField *pRemoveParkedOrderAction, CThostFtdcRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast)
{
	if (pRemoveParkedOrderAction && strcmp(pRemoveParkedOrderAction->InvestorID, g_config.InvestorID) != 0)
	{
		return;
	}
	else
	{
		strcpy_s(g_chParkedOrderActionID1, pRemoveParkedOrderAction->ParkedOrderActionID);
		CTraderSpi::OnRspRemoveParkedOrderAction(pRemoveParkedOrderAction, pRspInfo, nRequestID, bIsLast);
		SetEvent(g_hEvent);
	}
}

///Ԥ��¼��������Ӧ
void HandlerTrade::OnRspParkedOrderInsert(CThostFtdcParkedOrderField *pParkedOrder, CThostFtdcRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast)
{
	if (pParkedOrder && strcmp(pParkedOrder->InvestorID, g_config.InvestorID) != 0)
	{
		return;
	}
	else
	{
		strcpy_s(g_chParkedOrderID1, pParkedOrder->ParkedOrderID);
		CTraderSpi::OnRspParkedOrderInsert(pParkedOrder, pRspInfo, nRequestID, bIsLast);
		SetEvent(g_hEvent);
	}
}

///Ԥ�񳷵�¼��������Ӧ
void HandlerTrade::OnRspParkedOrderAction(CThostFtdcParkedOrderActionField *pParkedOrderAction, CThostFtdcRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast)
{
	if (pParkedOrderAction && strcmp(pParkedOrderAction->InvestorID, g_config.InvestorID) != 0)
	{
		return;
	}
	else
	{
		strcpy_s(g_chParkedOrderActionID1, pParkedOrderAction->ParkedOrderActionID);
		CTraderSpi::OnRspParkedOrderAction(pParkedOrderAction, pRspInfo, nRequestID, bIsLast);
		SetEvent(g_hEvent);
	}
}

///�����ѯԤ�񳷵���Ӧ
void HandlerTrade::OnRspQryParkedOrderAction(CThostFtdcParkedOrderActionField *pParkedOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	CTraderSpi::OnRspQryParkedOrderAction(pParkedOrderAction, pRspInfo, nRequestID, bIsLast);
}

///�����ѯԤ����Ӧ
void HandlerTrade::OnRspQryParkedOrder(CThostFtdcParkedOrderField *pParkedOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	CTraderSpi::OnRspQryParkedOrder(pParkedOrder, pRspInfo, nRequestID, bIsLast);
}


///ִ������֪ͨ
void HandlerTrade::OnRtnExecOrder(CThostFtdcExecOrderField *pExecOrder)
{
	if (pExecOrder) {
		strcpy_s(g_NewExecOrderRef, pExecOrder->ExecOrderRef);
		strcpy_s(g_NewExecOrderSysID, pExecOrder->ExecOrderSysID);
		g_NewFrontID = pExecOrder->FrontID;
		g_NewSessionID = pExecOrder->SessionID;
	}
	CTraderSpi::OnRtnExecOrder(pExecOrder);
}

//�ڻ������ѯ�����������
void HandlerTrade::ReqQueryBankAccountMoneyByFuture()
{
	CThostFtdcReqQueryAccountField a = { 0 };
	int b = m_pUserApi->ReqQueryBankAccountMoneyByFuture(&a, nRequestID++);
	LOG((b == 0) ? "�ڻ������ѯ�����������......���ͳɹ�\n" : "�ڻ������ѯ�����������......����ʧ�ܣ��������=[%d]\n", b);
}

//�ڻ����������ʽ�ת�ڻ�����
void HandlerTrade::ReqFromBankToFutureByFuture()
{
	int output_num;
	LOG("������ת�˽��:");
	cin >> output_num;

	CThostFtdcReqTransferField a = { 0 };
	strcpy_s(a.TradeCode, "202001");///ҵ������
	int bankid = 0;
	while (bankid != 1 & 2 & 3 & 5 & 6 & 7 & 8 & 9 & 10 & 11 & 12 & 13 & 14 & 15 & 16) {
		LOG("����������Ҫ��ת�˵�����\n");
		LOG("1.��������\n");
		LOG("2.ũҵ����\n");
		LOG("3.�й�����\n");
		LOG("5.��ͨ����\n");
		LOG("6.��������\n");
		LOG("7.��ҵ����\n");
		LOG("8.�ַ�����\n");
		LOG("9.��������\n");
		LOG("10.�������\n");
		LOG("11.��������\n");
		LOG("12.�������\n");
		LOG("13.ƽ������\n");
		LOG("14.ũ������\n");
		LOG("15.��չ����\n");
		LOG("16.�㷢����\n");
		cin >> bankid;
		if (bankid == 1 | 2 | 3 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16)
		{
			//strcpy_s(a.BankID, itoa(bankid, a.BankID, 10));///���д���
			itoa(bankid, a.BankID, 10);
		}
		else
		{
			LOG("�������������д��롣\n");
			_getch();
		}
	}


	strcpy_s(a.BankBranchID, "0000");///���̴���
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.TradeDate, "20170829");///��������
	strcpy_s(a.TradeTime, "09:00:00");
	strcpy_s(a.BankSerial, "6889");///������ˮ��
	strcpy_s(a.TradingDay, "20170829");///����ϵͳ���� 
	a.PlateSerial = 5;///����ƽ̨��Ϣ��ˮ��
	a.LastFragment = THOST_FTDC_LF_Yes;///����Ƭ��־ '0'=������Ƭ
	a.SessionID = SessionID;
	//strcpy_s(a.CustomerName, "");///�ͻ�����
	a.IdCardType = THOST_FTDC_ICT_IDCard;///֤������
	a.CustType = THOST_FTDC_CUSTT_Person;///�ͻ�����
	//strcpy_s(a.IdentifiedCardNo, "310115198706241914");///֤������
	/*strcpy_s(a.BankAccount, "123456789");
	strcpy_s(a.BankPassWord, "123456");///��������*/
	strcpy_s(a.BankAccount, "621485212110187");
	//strcpy_s(a.BankPassWord, "092812");///��������--����Ҫ���п�����
	strcpy_s(a.AccountID, g_config.InvestorID);///Ͷ�����ʺ�
	//strcpy_s(a.Password, "092812");///�ڻ�����--�ʽ�����
	strcpy_s(a.Password, "123456");///�ڻ�����--�ʽ�����
	a.InstallID = 1;///��װ���
	a.FutureSerial = 0;///�ڻ���˾��ˮ��
	a.VerifyCertNoFlag = THOST_FTDC_YNI_No;///��֤�ͻ�֤�������־
	strcpy_s(a.CurrencyID, "CNY");///���ִ���
	a.TradeAmount = output_num;///ת�ʽ��
	a.FutureFetchAmount = 0;///�ڻ���ȡ���
	a.CustFee = 0;///Ӧ�տͻ�����
	a.BrokerFee = 0;///Ӧ���ڻ���˾����
	a.SecuPwdFlag = THOST_FTDC_BPWDF_BlankCheck;///�ڻ��ʽ�����˶Ա�־
	a.RequestID = 0;///������
	a.TID = 0;///����ID
	int b = m_pUserApi->ReqFromBankToFutureByFuture(&a, 1);
	LOG((b == 0) ? "�ڻ����������ʽ�ת�ڻ�����......���ͳɹ�\n" : "�ڻ����������ʽ�ת�ڻ�����......����ʧ�ܣ��������=[%d]\n", b);
}

//�ڻ������ڻ��ʽ�ת��������
void HandlerTrade::ReqFromFutureToBankByFuture()
{
	int output_num;
	LOG("������ת�˽��:");
	cin >> output_num;

	CThostFtdcReqTransferField a = { 0 };
	strcpy_s(a.TradeCode, "202002");///ҵ������
bankid_new:int bankid = 0;
	LOG("����������Ҫ��ת�˵�����\n");
	LOG("1.��������\n");
	LOG("2.ũҵ����\n");
	LOG("3.�й�����\n");
	LOG("5.��ͨ����\n");
	LOG("6.��������\n");
	LOG("7.��ҵ����\n");
	LOG("8.�ַ�����\n");
	LOG("9.��������\n");
	LOG("10.�������\n");
	LOG("11.��������\n");
	LOG("12.�������\n");
	LOG("13.ƽ������\n");
	LOG("14.ũ������\n");
	LOG("15.��չ����\n");
	LOG("16.�㷢����\n");
	cin >> bankid;
	if (bankid == 1 | 2 | 3 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16)
	{
		//strcpy_s(a.BankID, itoa(bankid, a.BankID, 10));///���д���
		itoa(bankid, a.BankID, 10);
	}
	else {
		LOG("��������ȷ�ı��\n");
		_getch();
		goto bankid_new;
	}
	strcpy_s(a.BankBranchID, "0000");///���̴���
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	//strcpy_s(a.BankBranchID, "0000");///���з�֧��������
	//strcpy_s(a.TradeDate, "20170829");///��������
	//strcpy_s(a.TradeTime, "09:00:00");
	//strcpy_s(a.BankSerial, "");///������ˮ��
	//strcpy_s(a.TradingDay, "20170829");///����ϵͳ���� 
	//a.PlateSerial= 0;///����ƽ̨��Ϣ��ˮ��
	a.LastFragment = THOST_FTDC_LF_Yes;///����Ƭ��־ '0'=������Ƭ
	a.SessionID = SessionID;
	//strcpy_s(a.CustomerName, "");///�ͻ�����
	a.IdCardType = THOST_FTDC_ICT_IDCard;///֤������
	strcpy_s(a.IdentifiedCardNo, "310115198706241914");///֤������
	strcpy_s(a.BankAccount, "123456789");///�����ʺ�
	//strcpy_s(a.BankPassWord, "123456");///��������
	strcpy_s(a.AccountID, g_config.InvestorID);///Ͷ�����ʺ�
	strcpy_s(a.Password, "123456");///�ڻ�����
	a.InstallID = 1;///��װ���
	a.CustType = THOST_FTDC_CUSTT_Person;
	//a.FutureSerial = 0;///�ڻ���˾��ˮ��
	a.VerifyCertNoFlag = THOST_FTDC_YNI_No;///��֤�ͻ�֤�������־
	strcpy_s(a.CurrencyID, "CNY");///���ִ���
	a.TradeAmount = output_num;///ת�ʽ��
	a.FutureFetchAmount = 0;///�ڻ���ȡ���
	a.CustFee = 0;///Ӧ�տͻ�����
	a.BrokerFee = 0;///Ӧ���ڻ���˾����
	//a.SecuPwdFlag = THOST_FTDC_BPWDF_BlankCheck;///�ڻ��ʽ�����˶Ա�־
	a.RequestID = 0;///������
	a.TID = 0;///����ID
	int b = m_pUserApi->ReqFromFutureToBankByFuture(&a, 1);
	LOG((b == 0) ? "�ڻ������ڻ��ʽ�ת��������......���ͳɹ�\n" : "�ڻ������ڻ��ʽ�ת��������......����ʧ�ܣ��������=[%d]\n", b);
}

//��Ȩ�ԶԳ�¼������
void HandlerTrade::ReqOptionSelfCloseInsert()
{
	CThostFtdcInputOptionSelfCloseField a = { 0 };
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.InvestorID, g_config.InvestorID);
	strcpy_s(a.InstrumentID, g_chInstrumentID);
	strcpy_s(a.OptionSelfCloseRef, "1");
	strcpy_s(a.UserID, g_config.UserID);
	a.Volume = 1;

	int choose_1 = 0;
	while (choose_1 != 1 && choose_1 != 2 && choose_1 != 3 && choose_1 != 4) {
		LOG("��ѡ��Ͷ���ױ���־\n1.Ͷ��\t2.����\t3.�ױ�\t4.������\n");
		cin >> choose_1;
		if (choose_1 == 1) { a.HedgeFlag = THOST_FTDC_HF_Speculation; }
		else if (choose_1 == 2) { a.HedgeFlag = THOST_FTDC_HF_Arbitrage; }
		else if (choose_1 == 3) { a.HedgeFlag = THOST_FTDC_HF_Hedge; }
		else if (choose_1 == 4) { a.HedgeFlag = THOST_FTDC_HF_MarketMaker; }
		else {
			LOG("ѡ�����������ѡ��\n");
			_getch();
		}
	}

	int choose_2 = 0;
	while (choose_2 != 1 && choose_2 != 2 && choose_2 != 3) {
		LOG("��ѡ����Ȩ��Ȩ��ͷ���Ƿ��ԶԳ��־\n1.�ԶԳ���Ȩ��λ\t2.������Ȩ��λ\t3.�ԶԳ�������Լ����ڻ���λ\n");
		cin >> choose_2;
		if (choose_2 == 1) { a.OptSelfCloseFlag = THOST_FTDC_OSCF_CloseSelfOptionPosition; }
		else if (choose_2 == 2) { a.OptSelfCloseFlag = THOST_FTDC_OSCF_ReserveOptionPosition; }
		else if (choose_2 == 3) { a.OptSelfCloseFlag = THOST_FTDC_OSCF_SellCloseSelfFuturePosition; }
		else {
			LOG("ѡ�����������ѡ��\n");
			_getch();
			continue;
		}
	}

	strcpy_s(a.ExchangeID, g_chExchangeID);
	string accountid_new;
	LOG("�������ʽ��˺�:\n");
	cin >> accountid_new;
	strcpy_s(a.AccountID, accountid_new.c_str());
	strcpy_s(a.CurrencyID, "CNY");
	int b = m_pUserApi->ReqOptionSelfCloseInsert(&a, 1);
	LOG((b == 0) ? "��Ȩ�ԶԳ�¼������......���ͳɹ�\n" : "��Ȩ�ԶԳ�¼������......����ʧ�ܣ��������=[%d]\n", b);
}

///��Ȩ�ԶԳ�֪ͨ
void HandlerTrade::OnRtnOptionSelfClose(CThostFtdcOptionSelfCloseField *pOptionSelfClose)
{
	if (pOptionSelfClose) {
		g_chFrontID = pOptionSelfClose->FrontID;
		g_chSessionID = pOptionSelfClose->SessionID;
		strcpy_s(g_chOptionSelfCloseSysID, pOptionSelfClose->OptionSelfCloseSysID);//��Ȩ�ԶԳ���
		strcpy_s(g_chOptionSelfCloseRef, pOptionSelfClose->OptionSelfCloseRef);//��Ȩ�ԶԳ�����
	}
	CTraderSpi::OnRtnOptionSelfClose(pOptionSelfClose);
}

//��Ȩ�ԶԳ��������
void HandlerTrade::ReqOptionSelfCloseAction()
{
	CThostFtdcInputOptionSelfCloseActionField a = { 0 };
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.InvestorID, g_config.InvestorID);
	//strcpy_s(a.OptionSelfCloseSysID, g_chOptionSelfCloseSysID);//��Ȩ�ԶԳ���
	strcpy_s(a.OptionSelfCloseRef, g_chOptionSelfCloseRef);//��Ȩ�ԶԳ�����
	//a.FrontID = g_chFrontID;
	//a.SessionID = g_chSessionID;
	strcpy_s(a.ExchangeID, g_chExchangeID);
	a.ActionFlag = THOST_FTDC_AF_Delete;
	strcpy_s(a.UserID, g_config.UserID);
	strcpy_s(a.InstrumentID, g_chInstrumentID);
	int b = m_pUserApi->ReqOptionSelfCloseAction(&a, 1);
	LOG((b == 0) ? "��Ȩ�ԶԳ��������......���ͳɹ�\n" : "��Ȩ�ԶԳ��������......����ʧ�ܣ��������=[%d]\n", b);
}

//�����ѯ��Ȩ�ԶԳ�
void HandlerTrade::ReqQryOptionSelfClose()
{
	CThostFtdcQryOptionSelfCloseField a = { 0 };
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.InvestorID, g_config.InvestorID);
	strcpy_s(a.InstrumentID, g_chInstrumentID);
	strcpy_s(a.ExchangeID, g_chExchangeID);
	int b = m_pUserApi->ReqQryOptionSelfClose(&a, 1);
	LOG((b == 0) ? "�����ѯ��Ȩ�ԶԳ�......���ͳɹ�\n" : "�����ѯ��Ȩ�ԶԳ�......����ʧ�ܣ��������=[%d]\n", b);
}

///�����ѯ��Ȩ�ԶԳ���Ӧ
void HandlerTrade::OnRspQryOptionSelfClose(CThostFtdcOptionSelfCloseField *pOptionSelfClose, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pOptionSelfClose) {
		g_chFrontID = pOptionSelfClose->FrontID;
		g_chSessionID = pOptionSelfClose->SessionID;
		strcpy_s(g_chOptionSelfCloseSysID, pOptionSelfClose->OptionSelfCloseSysID);//��Ȩ�ԶԳ���
		strcpy_s(g_chOptionSelfCloseRef, pOptionSelfClose->OptionSelfCloseRef);//��Ȩ�ԶԳ�����
	}
	CTraderSpi::OnRspQryOptionSelfClose(pOptionSelfClose, pRspInfo, nRequestID, bIsLast);
}

///�����ѯִ������
void HandlerTrade::ReqQryExecOrder()
{
	CThostFtdcQryExecOrderField a = { 0 };
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.InvestorID, g_config.InvestorID);
	strcpy_s(a.InstrumentID, g_chInstrumentID);
	strcpy_s(a.ExchangeID, g_chExchangeID);
	strcpy_s(a.ExecOrderSysID, "");
	strcpy_s(a.InsertTimeStart, "");
	strcpy_s(a.InsertTimeEnd, "");
	int b = m_pUserApi->ReqQryExecOrder(&a, 1);
	LOG((b == 0) ? "ִ�������ѯ......���ͳɹ�\n" : "ִ�������ѯ......����ʧ�ܣ��������=[%d]\n", b);
}

///��ѯ�����ʽ��˻�
void HandlerTrade::ReqQrySecAgentTradingAccount()
{
	CThostFtdcQryTradingAccountField a = { 0 };
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.InvestorID, g_config.InvestorID);
	strcpy_s(a.CurrencyID, "CNY");
	a.BizType = THOST_FTDC_BZTP_Future;
	strcpy_s(a.AccountID, g_config.InvestorID);
	int b = m_pUserApi->ReqQrySecAgentTradingAccount(&a, 1);
	LOG((b == 0) ? "��ѯ�����ʽ��˻�......���ͳɹ�\n" : "��ѯ�����ʽ��˻�......����ʧ�ܣ��������=[%d]\n", b);
}

///�����ѯ�����������ʽ�У��ģʽ
void HandlerTrade::ReqQrySecAgentCheckMode()
{
	CThostFtdcQrySecAgentCheckModeField a = { 0 };
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.InvestorID, g_config.InvestorID);
	int b = m_pUserApi->ReqQrySecAgentCheckMode(&a, 1);
	LOG((b == 0) ? "�����ѯ�����������ʽ�У��ģʽ......���ͳɹ�\n" : "�����ѯ�����������ʽ�У��ģʽ......����ʧ�ܣ��������=[%d]\n", b);
}

///ע���û��ն���Ϣ�������м̷�����������ģʽ
///��Ҫ���ն���֤�ɹ����û���¼ǰ���øýӿ�
void HandlerTrade::RegisterUserSystemInfo()
{
	char pSystemInfo[344];
	int len;
	CTP_GetSystemInfo(pSystemInfo, len);

	CThostFtdcUserSystemInfoField a = { 0 };
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.UserID, g_config.UserID);
	memcpy(a.ClientSystemInfo, pSystemInfo, len);
	a.ClientSystemInfoLen = len;

	/*string ip;
	ip.clear();
	cin.ignore();
	LOG("������ip��ַ(��������Ϊ��)\n");
	getline(cin, ip);
	strcpy_s(a.ClientPublicIP, ip.c_str());*/
	strcpy_s(a.ClientPublicIP, "192.168.0.1");//ip��ַ

	//int Port;
	//Port = 0;
	//cin.ignore();
	//LOG("������˿ں�\n");
	//cin >> Port;
	//a.ClientIPPort = Port;//�˿ں�
	a.ClientIPPort = 51305;//�˿ں�

	/*string LoginTime;
	LoginTime.clear();
	cin.ignore();
	LOG("�������¼ʱ��(��������Ϊ��)\n");
	getline(cin, LoginTime);
	strcpy_s(a.ClientPublicIP, LoginTime.c_str());*/
	strcpy_s(a.ClientLoginTime, "20190121");
	strcpy_s(a.ClientAppID, g_config.AppID);
	int b = m_pUserApi->RegisterUserSystemInfo(&a);
	LOG((b == 0) ? "ע���û��ն���Ϣ......���ͳɹ�\n" : "ע���û��ն���Ϣ......����ʧ�ܣ��������=[%d]\n", b);
}

///�ϱ��û��ն���Ϣ�������м̷���������Ա��¼ģʽ
///����Ա��¼�󣬿��Զ�ε��øýӿ��ϱ��ͻ���Ϣ
void HandlerTrade::SubmitUserSystemInfo()
{
	char pSystemInfo[344];
	int len;
	CTP_GetSystemInfo(pSystemInfo, len);

	CThostFtdcUserSystemInfoField a = { 0 };
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.UserID, g_config.UserID);
	memcpy(a.ClientSystemInfo, pSystemInfo, len);
	a.ClientSystemInfoLen = len;

	/*string ip;
	ip.clear();
	cin.ignore();
	LOG("������ip��ַ(��������Ϊ��)\n");
	getline(cin, ip);
	strcpy_s(a.ClientPublicIP, ip.c_str());*/
	strcpy_s(a.ClientPublicIP, "192.168.0.1");//ip��ַ

	//int Port;
	//Port = 0;
	//cin.ignore();
	//LOG("������˿ں�\n");
	//cin >> Port;
	//a.ClientIPPort = Port;//�˿ں�
	a.ClientIPPort = 51305;//�˿ں�

	/*string LoginTime;
	LoginTime.clear();
	cin.ignore();
	LOG("�������¼ʱ��(��������Ϊ��)\n");
	getline(cin, LoginTime);
	strcpy_s(a.ClientPublicIP, LoginTime.c_str());*/
	strcpy_s(a.ClientLoginTime, "20190121");
	strcpy_s(a.ClientAppID, g_config.AppID);
	int b = m_pUserApi->SubmitUserSystemInfo(&a);
	LOG((b == 0) ? "ע���û��ն���Ϣ......���ͳɹ�\n" : "ע���û��ն���Ϣ......����ʧ�ܣ��������=[%d]\n", b);
}

///��ѯ�û���ǰ֧�ֵ���֤ģʽ
void HandlerTrade::ReqUserAuthMethod()
{
	CThostFtdcReqUserAuthMethodField a = { 0 };
	strcpy_s(a.TradingDay, "20190308");
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.UserID, g_config.UserID);
	int b = m_pUserApi->ReqUserAuthMethod(&a, nRequestID++);
	LOG((b == 0) ? "��ѯ�û���ǰ֧�ֵ���֤ģʽ......���ͳɹ�\n" : "��ѯ�û���ǰ֧�ֵ���֤ģʽ......����ʧ�ܣ��������=[%d]\n", b);
}

///�û�������ȡͼ����֤������
void HandlerTrade::ReqGenUserCaptcha()
{
	CThostFtdcReqGenUserCaptchaField a = { 0 };
	strcpy_s(a.TradingDay, "");
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.UserID, g_config.UserID);
	int b = m_pUserApi->ReqGenUserCaptcha(&a, nRequestID++);
	LOG((b == 0) ? "�û�������ȡͼ����֤������......���ͳɹ�\n" : "�û�������ȡͼ����֤������......����ʧ�ܣ��������=[%d]\n", b);
}

///�û�������ȡ������֤������
void HandlerTrade::ReqGenUserText()
{
	CThostFtdcReqGenUserTextField a = { 0 };
	strcpy_s(a.TradingDay, "");
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.UserID, g_config.UserID);
	int b = m_pUserApi->ReqGenUserText(&a, nRequestID++);
	LOG((b == 0) ? "�û�������ȡ������֤������......���ͳɹ�\n" : "�û�������ȡ������֤������......����ʧ�ܣ��������=[%d]\n", b);
}

///�û���������ͼƬ��֤��ĵ�½����
void HandlerTrade::ReqUserLoginWithCaptcha()
{
	CThostFtdcReqUserLoginWithCaptchaField a = { 0 };
	strcpy_s(a.TradingDay, "");
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.UserID, g_config.UserID);
	strcpy_s(a.Password, g_config.Password);
	strcpy_s(a.UserProductInfo, "");
	strcpy_s(a.InterfaceProductInfo, "");
	strcpy_s(a.ProtocolInfo, "");//Э����Ϣ
	strcpy_s(a.MacAddress, "");//Mac��ַ
	strcpy_s(a.ClientIPAddress, "");//�ն�IP��ַ
	strcpy_s(a.LoginRemark, "");//��¼����
	strcpy_s(a.Captcha, "");//ͼ����֤�����������
	a.ClientIPPort = 10203;
	int b = m_pUserApi->ReqUserLoginWithCaptcha(&a, nRequestID++);
	LOG((b == 0) ? "�û���������ͼƬ��֤��ĵ�½����......���ͳɹ�\n" : "�û���������ͼƬ��֤��ĵ�½����......����ʧ�ܣ��������=[%d]\n", b);
}

///�û��������ж�����֤��ĵ�½����
void HandlerTrade::ReqUserLoginWithText()
{
	CThostFtdcReqUserLoginWithTextField a = { 0 };
	strcpy_s(a.TradingDay, "");
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.UserID, g_config.UserID);
	strcpy_s(a.Password, g_config.Password);
	strcpy_s(a.UserProductInfo, "");
	strcpy_s(a.InterfaceProductInfo, "");
	strcpy_s(a.MacAddress, "");
	strcpy_s(a.ClientIPAddress, "");
	strcpy_s(a.LoginRemark, "");
	strcpy_s(a.Text, "");
	a.ClientIPPort = 10000;
	int b = m_pUserApi->ReqUserLoginWithText(&a, nRequestID++);
	LOG((b == 0) ? "�û��������ж�����֤��ĵ�½����......���ͳɹ�\n" :
		"�û��������ж�����֤��ĵ�½����......����ʧ�ܣ��������=[%d]\n", b);
}

///�û��������ж�̬����ĵ�½����
void HandlerTrade::ReqUserLoginWithOTP()
{
	CThostFtdcReqUserLoginWithOTPField a = { 0 };
	strcpy_s(a.TradingDay, "");
	strcpy_s(a.BrokerID, g_config.server.BrokerID);
	strcpy_s(a.UserID, g_config.UserID);
	strcpy_s(a.Password, g_config.Password);
	strcpy_s(a.UserProductInfo, "");
	strcpy_s(a.InterfaceProductInfo, "");
	strcpy_s(a.MacAddress, "");
	strcpy_s(a.ClientIPAddress, "");
	strcpy_s(a.LoginRemark, "");
	strcpy_s(a.OTPPassword, "");
	a.ClientIPPort = 10000;
	int b = m_pUserApi->ReqUserLoginWithOTP(&a, nRequestID++);
	LOG((b == 0) ? "�û��������ж�̬����ĵ�½����......���ͳɹ�\n" : "�û��������ж�̬����ĵ�½����......����ʧ�ܣ��������=[%d]\n", b);
}

///�����ѯ������������Ϣ
void HandlerTrade::ReqQrySecAgentTradeInfo()
{
	CThostFtdcQrySecAgentTradeInfoField a = { 0 };
	strcpy_s(a.BrokerID, "");
	strcpy_s(a.BrokerSecAgentID, "");
	int b = m_pUserApi->ReqQrySecAgentTradeInfo(&a, nRequestID++);
	LOG((b == 0) ? "�����ѯ������������Ϣ......���ͳɹ�\n" : "�����ѯ������������Ϣ......����ʧ�ܣ��������=[%d]\n", b);
}

