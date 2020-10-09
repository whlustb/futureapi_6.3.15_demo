﻿#include "LoginForm.h"
#include <windows.h>
#include <iostream>
#include <string>
#include "CtpArbFunc.h"
#include "HandlerQuote.h"
#include "HandlerTrade.h"
#include "HandlerVars.h"
#include "main.h"
#include "CtpArbStruct.h"

LoginForm::LoginForm(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	setWindowModality(Qt::ApplicationModal);

	system("COLOR 0A");

}

//登录槽函数。
void LoginForm::Login()
{	
	//初始化API/SPI相关的信息。
	//定义行情API/SPI
	md_api->RegisterSpi(md_spi);
	//定义交易API/SPI
	api->CreateFtdcTraderApi(".\\flow\\");
	LOG(api->GetApiVersion());
	api->RegisterSpi(spi);

	/****行情相关****/
	string g_chFrontMdaddr = ui.lineEdit_addr_md->text().toStdString();
	md_api->RegisterFront(const_cast<char *>(g_chFrontMdaddr.c_str()));
	md_api->Init();
	WaitForSingleObject(g_qEvent, INFINITE);

	/****交易相关****/
	string g_chFrontaddr = ui.lineEdit_addr_trade->text().toStdString();
	//订阅公有、私有流。
	api->SubscribePrivateTopic(THOST_TERT_QUICK);
	api->SubscribePublicTopic(THOST_TERT_QUICK);
	api->RegisterFront(const_cast<char *>(g_chFrontaddr.c_str()));
	api->Init();
	WaitForSingleObject(g_hEvent, INFINITE);	
	LOG("交易日期:"); 
	LOG(api->GetTradingDay());

	//客户端认证。
	spi->ReqAuthenticate();
	WaitForSingleObject(g_hEvent, INFINITE);
	strcpy_s(g_chBrokerID, ui.lineEdit_broker_id->text().toStdString().c_str()); //BrokerId.
	strcpy_s(g_chUserID, ui.lineEdit_username->text().toStdString().c_str()); //用户ID.
	strcpy_s(g_chInvestorID, ui.lineEdit_username->text().toStdString().c_str()); //investorId也设置为用户id.
	strcpy_s(g_chPassword, ui.lineEdit_password->text().toStdString().c_str()); //密码。
	spi->ReqUserLogin(g_chBrokerID, g_chUserID, g_chPassword);
	WaitForSingleObject(g_hEvent, INFINITE);

	//关闭登录窗口
	this->close();
	//显示主窗体。
	w_main = new CtpArb();
	w_main->Init();
	w_main->show();
	
}




LoginForm::~LoginForm()
{

}



