#include "HandlerVars.h"

FILE *logfile = fopen("syslog.txt", "w");

// ����¼������Ƿ���ɵı�־
// Create a manual reset event with no signal
HANDLE g_hEvent = CreateEvent(NULL, false, false, NULL);
/// ����������
TThostFtdcExchangeIDType g_chExchangeID;
///��Լ����
TThostFtdcInstrumentIDType	g_chInstrumentID;
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

HANDLE g_qEvent = CreateEvent(NULL, false, false, NULL);


/***********����ȫ�ֱ���API/SPI���*********/
//��������API/SPI
CThostFtdcMdApi* md_api = nullptr;
HandlerQuote* md_spi = nullptr;
//���彻��API/SPI
CTraderApi* api = nullptr;
HandlerTrade* spi = nullptr;

/******����ȫ�ֱ�������������CTP������Ϣ��******/
QMap<QString, CThostFtdcInstrumentField> g_instMap;//��Լ��Ϣ��
QList<CThostFtdcInvestorPositionField> g_posList; //�ֲ���Ϣ��
QMap<QString, CThostFtdcDepthMarketDataField> g_depthMap; //���µĺ�Լ������顣

OrderWorker* g_orderWorker = new OrderWorker(); //����ȫ�ֶ��������ࡣ

/******ȫ�ֱ���******/
QStandardItemModel *modelIns = new QStandardItemModel(); //��Լ�б�ģ�͡�
QStandardItemModel *modelPosition = new QStandardItemModel(); ; //�ֲ�ģ��
QStandardItemModel *modelArbPortf = new QStandardItemModel(); ; //�������ģ��
QStandardItemModel *modelArbOrder = new QStandardItemModel(); ; //����������ģ��
QStandardItemModel *modelOrder = new QStandardItemModel(); ; //��ͨ����ģ�͡�
QList<ArbPortf> g_arbPortfList; //��������б�
QMap<QString, ArbOrder> g_arbOrderMap; //���������б�key:������������ID.
QMap<QString, Order> g_orderMap; //��ϵͳ���µ��б� key:OrderRefֵ��
QMap<QString, CThostFtdcOrderField> g_orderUnfilledMap; //����δ�ɽ�������


QMap<char, QString> PosDirectionMap{ {'1', "NA"},  {'2', QStr("��")},  {'3', QStr("��")} };
QMap<char, QString> OrdDirectionMap{ {'0', QStr("��")},  {'1', QStr("��")} };
QMap<char, QString> OrdOffsetMap{ {'0', QStr("����")},  {'1', QStr("ƽ��")},  {'1', QStr("ǿƽ")},  {'1', QStr("ƽ��")},  {'1', QStr("ƽ��")},  {'1', QStr("ǿ��")} ,  {'1', QStr("����ǿƽ")} };
QMap<char, QString> OrdStatusMap{ {'0', QStr("ȫ���ɽ�")},  {'1', QStr("���ֳɽ�(����)")},  {'2', QStr("���ֳɽ�(����)")},  {'3', QStr("�Ŷ���")},  {'4', QStr("���ڶ�����")},  {'5', QStr("����")} ,  {'a', QStr("δ֪")}  ,  {'b', QStr("δ����")},  {'b', QStr("�Ѵ���")}  };



//���ö���
ConfigInfo g_config;
QList<FrontServer> g_serverList;