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


//交易类
class HandlerTrade : public CTraderSpi
{
private:
	CThostFtdcTraderApi *m_pUserApi;
public:
	HandlerTrade(CThostFtdcTraderApi *pUserApi) : m_pUserApi(pUserApi) {}
	~HandlerTrade() {}
	virtual void OnFrontConnected();

	//客户端认证
	void ReqAuthenticate();

	///客户端认证响应
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

	///登出请求响应
	virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///请求确认结算单
	void ReqSettlementInfoConfirm();

	///投资者结算结果确认响应
	virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///用户口令更新请求
	void ReqUserPasswordUpdate();

	///用户口令更新请求响应
	virtual void OnRspUserPasswordUpdate(CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///资金账户口令更新请求
	void ReqTradingAccountPasswordUpdate();

	///资金账户口令更新请求响应
	virtual void OnRspTradingAccountPasswordUpdate(CThostFtdcTradingAccountPasswordUpdateField *pTradingAccountPasswordUpdate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///预埋单录入//限价单
	void ReqParkedOrderInsert();

	///预埋撤单录入请求
	void ReqParkedOrderAction();

	///请求删除预埋单
	void ReqRemoveParkedOrder();

	///请求删除预埋撤单
	void ReqRemoveParkedOrderAction();

	///报单录入请求
	int ReqOrderInsert_Whl(QString inst, QString direction, QString offset, double price, double vol);

	///报单录入请求
	void ReqOrderInsert_Ordinary();

	///大商所止损单
	void ReqOrderInsert_Touch();

	///大商所止盈单
	void ReqOrderInsert_TouchProfit();

	//全成全撤
	void ReqOrderInsert_VC_CV();

	//部成部撤
	void ReqOrderInsert_VC_AV();

	//市价单
	void ReqOrderInsert_AnyPrice();

	//市价转限价单(中金所)
	void ReqOrderInsert_BestPrice();

	//套利指令
	void ReqOrderInsert_Arbitrage();

	//互换单
	void ReqOrderInsert_IsSwapOrder();

	///报单操作请求
	void ReqOrderAction_Ordinary();

	///执行宣告录入请求
	void ReqExecOrderInsert(int a);

	///执行宣告操作请求
	void ReqExecOrderAction();

	//批量报单操作请求
	void ReqBatchOrderAction();

	///请求查询报单
	void ReqQryOrder();

	///报单录入请求
	void ReqOrderInsert_Condition(int select_num);

	///报单操作请求
	void ReqOrderAction_Condition();

	//撤销查询的报单
	void ReqOrderAction_forqry(int action_num);

	///请求查询成交
	void ReqQryTrade();

	///请求查询预埋单
	void ReqQryParkedOrder();

	//请求查询服务器预埋撤单
	void ReqQryParkedOrderAction();

	//请求查询资金账户
	void ReqQryTradingAccount();

	//请求查询投资者持仓
	void ReqQryInvestorPosition();
	void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);


	//请求查询投资者持仓明细
	void ReqQryInvestorPositionDetail();

	//请求查询交易所保证金率
	void ReqQryExchangeMarginRate();

	//请求查询合约保证金率
	void ReqQryInstrumentMarginRate();

	//请求查询合约手续费率
	void ReqQryInstrumentCommissionRate();

	//请求查询做市商合约手续费率
	void ReqQryMMInstrumentCommissionRate();

	//请求查询做市商期权合约手续费
	void ReqQryMMOptionInstrCommRate();

	//请求查询报单手续费
	void ReqQryInstrumentOrderCommRate();

	//请求查询期权合约手续费
	void ReqQryOptionInstrCommRate();

	//请求查询合约
	void ReqQryInstrument();

	///请求查询合约响应
	virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//请求查询投资者结算结果
	void ReqQrySettlementInfo();

	//请求查询转帐流水
	void ReqQryTransferSerial();

	//请求查询产品
	void ReqQryProduct();

	//请求查询转帐银行
	void ReqQryTransferBank();

	//请求查询交易通知
	void ReqQryTradingNotice();

	//请求查询交易编码
	void ReqQryTradingCode();

	//请求查询结算信息确认
	void ReqQrySettlementInfoConfirm();

	//请求查询产品组
	void ReqQryProductGroup();

	//请求查询投资者单元
	void ReqQryInvestUnit();

	//请求查询经纪公司交易参数
	void ReqQryBrokerTradingParams();

	//请求查询询价
	void ReqQryForQuote();

	//请求查询报价
	void ReqQryQuote();

	///询价录入请求
	void ReqForQuoteInsert();

	///做市商报价录入请求
	void ReqQuoteInsert();

	///报价通知
	virtual void OnRtnQuote(CThostFtdcQuoteField *pQuote);

	//报价撤销
	void ReqQuoteAction();

	//查询最大报单数量请求
	void ReqQueryMaxOrderVolume();

	//请求查询监控中心用户令牌
	void ReqQueryCFMMCTradingAccountToken();

	///报单操作错误回报
	virtual void OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo);

	///报单录入请求响应
	virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast);

	///报单录入错误回报
	virtual void OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo);

	///报单通知
	virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);

	//成交回报。
	virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);

	///删除预埋单响应
	virtual void OnRspRemoveParkedOrder(CThostFtdcRemoveParkedOrderField *pRemoveParkedOrder, CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast);

	///删除预埋撤单响应
	virtual void OnRspRemoveParkedOrderAction(CThostFtdcRemoveParkedOrderActionField *pRemoveParkedOrderAction, CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast);

	///预埋单录入请求响应
	virtual void OnRspParkedOrderInsert(CThostFtdcParkedOrderField *pParkedOrder, CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast);

	///预埋撤单录入请求响应
	virtual void OnRspParkedOrderAction(CThostFtdcParkedOrderActionField *pParkedOrderAction, CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast);

	///请求查询预埋撤单响应
	virtual void OnRspQryParkedOrderAction(CThostFtdcParkedOrderActionField *pParkedOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///请求查询预埋单响应
	virtual void OnRspQryParkedOrder(CThostFtdcParkedOrderField *pParkedOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///请求查询报单响应
	virtual void OnRspQryOrder(CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///执行宣告通知
	virtual void OnRtnExecOrder(CThostFtdcExecOrderField *pExecOrder);

	//期货发起查询银行余额请求
	void ReqQueryBankAccountMoneyByFuture();

	//期货发起银行资金转期货请求
	void ReqFromBankToFutureByFuture();

	//期货发起期货资金转银行请求
	void ReqFromFutureToBankByFuture();

	//期权自对冲录入请求
	void ReqOptionSelfCloseInsert();

	///期权自对冲通知
	virtual void OnRtnOptionSelfClose(CThostFtdcOptionSelfCloseField *pOptionSelfClose);

	//期权自对冲操作请求
	void ReqOptionSelfCloseAction();

	//请求查询期权自对冲
	void ReqQryOptionSelfClose();

	///请求查询期权自对冲响应
	virtual void OnRspQryOptionSelfClose(CThostFtdcOptionSelfCloseField *pOptionSelfClose, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///请求查询执行宣告
	void ReqQryExecOrder();

	///查询二代资金账户
	void ReqQrySecAgentTradingAccount();

	///请求查询二级代理商资金校验模式
	void ReqQrySecAgentCheckMode();

	///注册用户终端信息，用于中继服务器多连接模式
	///需要在终端认证成功后，用户登录前调用该接口
	void RegisterUserSystemInfo();

	///上报用户终端信息，用于中继服务器操作员登录模式
	///操作员登录后，可以多次调用该接口上报客户信息
	void SubmitUserSystemInfo();

	///查询用户当前支持的认证模式
	void ReqUserAuthMethod();

	///用户发出获取图形验证码请求
	void ReqGenUserCaptcha();

	///用户发出获取短信验证码请求
	void ReqGenUserText();

	///用户发出带有图片验证码的登陆请求
	void ReqUserLoginWithCaptcha();

	///用户发出带有短信验证码的登陆请求
	void ReqUserLoginWithText();

	///用户发出带有动态口令的登陆请求
	void ReqUserLoginWithOTP();

	///请求查询二级代理商信息
	void ReqQrySecAgentTradeInfo();


};
