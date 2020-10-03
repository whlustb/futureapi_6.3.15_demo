#include "LoginForm.h"
#include <windows.h>
#include <iostream>
#include <string>

LoginForm::LoginForm(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	system("COLOR 0A");
	logfile = fopen("syslog.txt", "w");


}

//登录槽函数。
void LoginForm::Login()
{	
	/****行情相关****/
	//定义行情API,
	string g_chFrontMdaddr = ui.lineEdit_addr_md->text().toStdString();
	cout << "g_chFrontMdaddr = " << g_chFrontMdaddr << "\n" << endl;
	CThostFtdcMdApi  *md_api = CThostFtdcMdApi::CreateFtdcMdApi(".\\flow\\md");
	//定义行情SPI.
	HandlerQuote m_spi(md_api);
	md_api->RegisterSpi(&m_spi);
	md_api->RegisterFront(const_cast<char *>(g_chFrontMdaddr.c_str()));
	md_api->Init();
	WaitForSingleObject(xinhao, INFINITE);

	/****交易相关****/
	//定义交易api.
	string g_chFrontaddr = ui.lineEdit_addr_trade->text().toStdString();
	cout << "g_chFrontaddr = " << g_chFrontaddr << "\n" << endl;
	CTraderApi *api = new CTraderApi;
	api->CreateFtdcTraderApi(".\\flow\\");
	LOG(api->GetApiVersion());
	cout << endl;
	//定义交易spi.
	HandlerTrade spi(api); //将api做为参数传入spi，便于在内部调用。
	api->RegisterSpi(&spi);
	//订阅公有、私有流。
	api->SubscribePrivateTopic(THOST_TERT_QUICK);
	api->SubscribePublicTopic(THOST_TERT_QUICK);
	api->RegisterFront(const_cast<char *>(g_chFrontaddr.c_str()));
	api->Init();
	WaitForSingleObject(g_hEvent, INFINITE);	
	LOG("交易日期:"); 
	LOG(api->GetTradingDay());

	//客户端认证。
	spi.ReqAuthenticate();
	WaitForSingleObject(g_hEvent, INFINITE);
	strcpy_s(g_chBrokerID, ui.lineEdit_broker_id->text().toStdString().c_str()); //BrokerId.
	strcpy_s(g_chUserID, ui.lineEdit_username->text().toStdString().c_str()); //用户ID.
	strcpy_s(g_chInvestorID, ui.lineEdit_username->text().toStdString().c_str()); //investorId也设置为用户id.
	strcpy_s(g_chPassword, ui.lineEdit_password->text().toStdString().c_str()); //密码。
	spi.ReqUserLogin(g_chBrokerID, g_chUserID, g_chPassword);
	WaitForSingleObject(g_hEvent, INFINITE);

	//查询合约
	//strcpy_s(g_chInstrumentID, "rb2101"); //留空时，查询全部合约 。查询后的合约，是自动保存下来的，下次订阅时，也是订阅查询的这些合约 。
	spi.ReqQryInstrument();
	WaitForSingleObject(xinhao, INFINITE);

	//订阅行情
	//m_spi.SubscribeMarketData();
	//_getch();
	//md_api->Release(); 

}


LoginForm::~LoginForm()
{
}



