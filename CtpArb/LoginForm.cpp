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

//配置对象。
ConfigInfo g_config;
QList<FrontServer> g_serverList;


LoginForm::LoginForm(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	setWindowModality(Qt::ApplicationModal);

	system("COLOR 0A");

	//读取配置文件。
	QByteArray data;
	QFile *file = new QFile("config.json");
	if (file->open(QIODevice::ReadOnly | QIODevice::Text)) {
		data = file->readAll();
	}
	file->close();
	//解析配置文件为对象。
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

//登录槽函数。
void LoginForm::Login()
{	
	//定义行情API/SPI
	md_api = CThostFtdcMdApi::CreateFtdcMdApi(".\\flow\\md");
	md_spi = new HandlerQuote(md_api);
	md_api->RegisterSpi(md_spi);
	//定义交易API/SPI
	api = new CTraderApi();
	api->CreateFtdcTraderApi(".\\flow\\"); //这条语句，主要是为了初始化api内部的CThostFtdcTraderApi而调用的，实际上外部只用api.
	spi = new HandlerTrade(api);
	api->RegisterSpi(spi);
	
	LOG("API_version: %s", api->GetApiVersion());

	//保存服务器配置信息。
	g_config.server = g_serverList[ui.comboBox_server->currentIndex()];
	g_config.UserID = QStrToCh(ui.lineEdit_username->text());
	g_config.InvestorID = QStrToCh(ui.lineEdit_username->text());
	g_config.Password = QStrToCh(ui.lineEdit_password->text());

	/****连接行情服务器****/
	md_api->RegisterFront(g_config.server.MdAddr);
	md_api->Init();
	DWORD r = WaitForSingleObject(g_qEvent, 5*1000);
	if (r != WAIT_OBJECT_0) {
		QMessageBox::warning(this, QStr("错误"), QStr("登录服务器失败!"));
		md_api->Release();
		return;
	}

	/****连接交易服务器****/
	api->SubscribePrivateTopic(THOST_TERT_QUICK);//订阅公有流
	api->SubscribePublicTopic(THOST_TERT_QUICK); //订阅私有流。
	api->RegisterFront(g_config.server.Addr);
	api->Init();
	WaitForSingleObject(g_hEvent, INFINITE);	
	LOG("交易日期:%s", api->GetTradingDay());

	//客户端认证。
	spi->ReqAuthenticate();
	WaitForSingleObject(g_hEvent, INFINITE);
	spi->ReqUserLogin(g_config.server.BrokerID, g_config.UserID, g_config.Password);
	WaitForSingleObject(g_hEvent, INFINITE);

	//关闭登录窗口
	this->close();
	//显示主窗体。
	w_main = new MainWindow();
	w_main->show();
	w_main->Init();
	
}




LoginForm::~LoginForm()
{

}



