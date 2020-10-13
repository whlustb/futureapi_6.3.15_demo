#include "LoginForm.h"
#include <windows.h>
#include <iostream>
#include <string>
#include "CtpArbFunc.h"
#include "HandlerQuote.h"
#include "HandlerTrade.h"
#include "HandlerVars.h"
#include "main.h"
#include "CtpArbStruct.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>

//���ö���
ConfigInfo g_config;
QList<FrontServer> g_serverList;


LoginForm::LoginForm(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	setWindowModality(Qt::ApplicationModal);

	system("COLOR 0A");

	//��ȡ�����ļ���
	QByteArray data;
	QFile *file = new QFile("config.json");
	if (file->open(QIODevice::ReadOnly | QIODevice::Text)) {
		data = file->readAll();
	}
	file->close();
	//���������ļ�Ϊ����
	QJsonDocument jsonDom = QJsonDocument::fromJson(data);
	QJsonObject root = jsonDom.object();
	char* a = QStrToCh(root["UserProductInfo"].toString());
	g_config.UserProductInfo = a;

	g_config.UserProductInfo = QStrToCh(root["UserProductInfo"].toString());
	g_config.AuthCode = QStrToCh(root["AuthCode"].toString());
	g_config.AppID = QStrToCh(root["AppID"].toString());
	QJsonArray server_arr = root["FrontServers"].toArray();
	for each (QJsonValue val in server_arr) {
		QJsonObject obj = val.toObject();
		FrontServer s;
		s.Name = obj["Name"].toString();
		s.Addr = QStrToCh(obj["Addr"].toString());
		s.MdAddr = QStrToCh(obj["MdAddr"].toString());
		s.BrokerID = QStrToCh(obj["BrokerID"].toString());
		g_serverList.append(s);
		ui.comboBox_server->addItem(QString(s.Name));
	}
}

//��¼�ۺ�����
void LoginForm::Login()
{	
	//��������API/SPI
	md_api = CThostFtdcMdApi::CreateFtdcMdApi(".\\flow\\md");
	md_spi = new HandlerQuote(md_api);
	md_api->RegisterSpi(md_spi);
	//���彻��API/SPI
	api = new CTraderApi();
	api->CreateFtdcTraderApi(".\\flow\\"); //������䣬��Ҫ��Ϊ�˳�ʼ��api�ڲ���CThostFtdcTraderApi�����õģ�ʵ�����ⲿֻ��api.
	spi = new HandlerTrade(api);
	api->RegisterSpi(spi);
	
	LOG("API_version: %s", api->GetApiVersion());

	//���������������Ϣ��
	g_config.server = g_serverList[ui.comboBox_server->currentIndex()];
	g_config.UserID = QStrToCh(ui.lineEdit_username->text());
	g_config.InvestorID = QStrToCh(ui.lineEdit_username->text());
	g_config.Password = QStrToCh(ui.lineEdit_password->text());

	/****�������������****/
	md_api->RegisterFront(g_config.server.MdAddr);
	md_api->Init();
	DWORD r = WaitForSingleObject(g_qEvent, 5*1000);
	if (r != WAIT_OBJECT_0) {
		QMessageBox::warning(this, QStr("����"), QStr("��¼������ʧ��!"));
		md_api->Release();
		return;
	}

	/****���ӽ��׷�����****/
	api->SubscribePrivateTopic(THOST_TERT_QUICK);//���Ĺ�����
	api->SubscribePublicTopic(THOST_TERT_QUICK); //����˽������
	api->RegisterFront(g_config.server.Addr);
	api->Init();
	WaitForSingleObject(g_hEvent, INFINITE);	
	LOG("��������:%s", api->GetTradingDay());

	//�ͻ�����֤��
	spi->ReqAuthenticate();
	WaitForSingleObject(g_hEvent, INFINITE);
	spi->ReqUserLogin(g_config.server.BrokerID, g_config.UserID, g_config.Password);
	WaitForSingleObject(g_hEvent, INFINITE);

	//�رյ�¼����
	this->close();
	//��ʾ�����塣
	w_main = new MainWindow();
	w_main->show();
	w_main->Init();
	
}




LoginForm::~LoginForm()
{

}



