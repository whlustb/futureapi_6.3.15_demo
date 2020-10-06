#include "HandlerVars.h"

// ����¼������Ƿ���ɵı�־
// Create a manual reset event with no signal
HANDLE g_hEvent = CreateEvent(NULL, false, false, NULL);
/// ��Ա����
TThostFtdcBrokerIDType g_chBrokerID;
/// �����û�����
TThostFtdcUserIDType g_chUserID;
/// �����û�����
TThostFtdcPasswordType g_chPassword;
/// ����������
TThostFtdcExchangeIDType g_chExchangeID;
///��Լ����
TThostFtdcInstrumentIDType	g_chInstrumentID;
///Ͷ���ߴ���
TThostFtdcInvestorIDType g_chInvestorID;
///Ԥ�񳷵����
TThostFtdcParkedOrderActionIDType	g_chParkedOrderActionID1;
///Ԥ�񱨵����
TThostFtdcParkedOrderIDType	g_chParkedOrderID1;
///��������
TThostFtdcOrderRefType	g_chOrderRef;
///ǰ�ñ��
TThostFtdcFrontIDType	g_chFrontID;
///�Ự���
TThostFtdcSessionIDType	g_chSessionID;
///�������
TThostFtdcOrderSysIDType	g_chOrderSysID;
///ֹ���
TThostFtdcPriceType	g_chStopPrice;
///��������
TThostFtdcOrderRefType	g_chQuoteRef;
int FrontID = 0;
int SessionID = 0;
int Limitprice = 0;
int nRequestID = 0;
int chioce_action = 0;//Ϊ0��ȫ����

vector<string> vector_OrderSysID;
vector<string> vector_ExchangeID;
vector<string> vector_InstrumentID;
vector<string> md_InstrumentID;
int action_number;

///ִ����������
TThostFtdcOrderRefType	g_NewExecOrderRef;
///ִ��������
TThostFtdcExecOrderSysIDType	g_NewExecOrderSysID;
///ǰ�ñ��
TThostFtdcFrontIDType	g_NewFrontID;
///�Ự���
TThostFtdcSessionIDType	g_NewSessionID;

//��Ȩ�ԶԳ���Ӧ֪ͨ
///��Ȩ�ԶԳ���
TThostFtdcOrderSysIDType	g_chOptionSelfCloseSysID;
///��Ȩ�ԶԳ�����
TThostFtdcOrderRefType	g_chOptionSelfCloseRef;
///�û��˲�Ʒ��Ϣ
TThostFtdcProductInfoType	g_chUserProductInfo;
///��֤��
TThostFtdcAuthCodeType	g_chAuthCode;
///App����
TThostFtdcAppIDType	g_chAppID;

HANDLE g_qEvent = CreateEvent(NULL, false, false, NULL);


/***********����ȫ�ֱ���API/SPI���*********/
//��������API/SPI
CThostFtdcMdApi* md_api = CThostFtdcMdApi::CreateFtdcMdApi(".\\flow\\md");
HandlerQuote* m_spi = new HandlerQuote(md_api);
//���彻��API/SPI
CTraderApi* api = new CTraderApi();
HandlerTrade* spi = new HandlerTrade(api); //��api��Ϊ��������spi���������ڲ����á�

/******����ȫ�ֱ�������������CTP������Ϣ��******/
QMap<QString, CThostFtdcInstrumentField> g_instMap;