#pragma once

#include <QList>
#include <QString>
#include <QDataStream>
#include <QTime>

//套利组合单腿，
struct ArbLeg {
	QString InstrumentID; //合约ID
	QString Direction; //交易方向（首次）
	int Vol; //手数
	QString PriceType; //价格类型：对盘价、市价...
	int SlipPoint; //滑点，
	bool Chase; //是否追价。
	int ChasePoint; //追单点数。


	//序列化
	friend QDataStream &operator<<(QDataStream& input, const ArbLeg& dt) {
		input << dt.InstrumentID << dt.Direction << dt.Vol << dt.PriceType << dt.SlipPoint << dt.Chase << dt.ChasePoint;
		return input;
	}

	//反序列化
	friend QDataStream &operator>>(QDataStream& output, ArbLeg& dt) {
		output >> dt.InstrumentID >> dt.Direction >> dt.Vol >> dt.PriceType >> dt.SlipPoint >> dt.Chase >> dt.ChasePoint;
		return output;
	}

};

//套利基本配置，
struct ArbBase {
	QString Name; //名称。
	QString Offset; //开、平、自动开平
	QString SendOrderType; //下单方式：全部同时、主动腿。

	QString OpenCondFormula; //开仓条件：公式
	QString OpenCondOperator; //开仓条件。比较符号
	QString OpenCondVal; //开仓条件。比较值

	QString CloseCondFormula; //平仓条件。公式
	QString CloseCondOperator; //平仓条件。比较符号
	QString CloseCondVal; //平仓条件。比较值

	int Times; //份数，
	bool Loop; //循环套利。
};

///套利组合、继承基本配置 + 套利腿的列表。
struct ArbPortf:public ArbBase
{
	QList<ArbLeg> LegList;

	//序列化
	friend QDataStream &operator<<(QDataStream& input, const ArbPortf& dt){
		input << dt.Name << dt.Offset << dt.SendOrderType \
			<< dt.OpenCondFormula << dt.OpenCondOperator << dt.OpenCondVal \
			<< dt.CloseCondFormula << dt.CloseCondOperator << dt.CloseCondVal \
			<< dt.Times << dt.Loop;
		input << dt.LegList;
		return input;
	}

	//反序列化
	friend QDataStream &operator>>(QDataStream& output, ArbPortf& dt){
		output >> dt.Name >> dt.Offset >> dt.SendOrderType \
			>> dt.OpenCondFormula >> dt.OpenCondOperator >> dt.OpenCondVal \
			>> dt.CloseCondFormula >> dt.CloseCondOperator >> dt.CloseCondVal \
			>> dt.Times >> dt.Loop;
		output >> dt.LegList;
		return output;
	}
};

//订单单腿，继承的组合的单腿。
struct OrdLeg:public ArbLeg {
	int OrderId; //发到交易所的订单号。
	int DealVol; //成交数量。
	QString Status; //订单状态。

	//序列化
	friend QDataStream &operator<<(QDataStream& input, const OrdLeg& dt) {
		input << dt.InstrumentID << dt.Direction << dt.Vol << dt.PriceType << dt.SlipPoint << dt.Chase << dt.ChasePoint;
		input << dt.OrderId << dt.DealVol << dt.Status;
		return input;
	}

	//反序列化
	friend QDataStream &operator>>(QDataStream& output, OrdLeg& dt) {
		output >> dt.InstrumentID >> dt.Direction >> dt.Vol >> dt.PriceType >> dt.SlipPoint >> dt.Chase >> dt.ChasePoint;
		output >> dt.OrderId >> dt.DealVol >> dt.Status;
		return output;
	}
};

//套利单、继承套利基本配置
struct ArbOrder :public ArbBase {
	int Id; //订单编号。
	QTime Time; //下单时间
	QList<OrdLeg> OrdLegList; //套利单腿列表。

	//序列化
	friend QDataStream &operator<<(QDataStream& input, const ArbOrder& dt) {
		input << dt.Name << dt.Offset << dt.SendOrderType \
			<< dt.OpenCondFormula << dt.OpenCondOperator << dt.OpenCondVal \
			<< dt.CloseCondFormula << dt.CloseCondOperator << dt.CloseCondVal \
			<< dt.Times << dt.Loop;
		input << dt.Id << dt.Time << dt.OrdLegList;
		return input;
	}

	//反序列化
	friend QDataStream &operator>>(QDataStream& output, ArbOrder& dt) {
		output >> dt.Name >> dt.Offset >> dt.SendOrderType \
			>> dt.OpenCondFormula >> dt.OpenCondOperator >> dt.OpenCondVal \
			>> dt.CloseCondFormula >> dt.CloseCondOperator >> dt.CloseCondVal \
			>> dt.Times >> dt.Loop;
		output >> dt.Id >> dt.Time >> dt.OrdLegList;
		return output;
	}

};