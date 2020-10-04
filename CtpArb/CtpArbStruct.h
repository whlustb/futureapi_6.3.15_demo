#pragma once

#include <QList>
#include <QString>
#include <QDataStream>

struct ArbLeg {
	QString InstrumentID; //合约ID
	QString Direction; //交易方向（首次）
	int Vol; //手数
	QString PriceType; //价格类型：对盘价、市价...
	int SlipPoint; //滑点，
	int ChasePoint; //追单点数。

	//序列化
	friend QDataStream &operator<<(QDataStream& input, const ArbLeg& dt) {
		input << dt.InstrumentID << dt.Direction << dt.Vol << dt.PriceType << dt.SlipPoint << dt.ChasePoint;
		return input;
	}

	//反序列化
	friend QDataStream &operator>>(QDataStream& output, ArbLeg& dt) {
		output >> dt.InstrumentID >> dt.Direction >> dt.Vol >> dt.PriceType >> dt.SlipPoint >> dt.ChasePoint;
		return output;
	}

};

///套利对信息，
struct ArbItem
{
	QString Name; //名称。
	QList<ArbLeg> Legs;
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

	//序列化
	friend QDataStream &operator<<(QDataStream& input, const ArbItem& dt){
		input << dt.Name << dt.Legs << dt.Offset << dt.SendOrderType \
			<< dt.OpenCondFormula << dt.OpenCondOperator << dt.OpenCondVal \
			<< dt.CloseCondFormula << dt.CloseCondOperator << dt.CloseCondVal \
			<< dt.Times << dt.Loop;
			;
		return input;
	}

	//反序列化
	friend QDataStream &operator>>(QDataStream& output, ArbItem& dt){
		output >> dt.Name >> dt.Legs >> dt.Offset >> dt.SendOrderType \
			>> dt.OpenCondFormula >> dt.OpenCondOperator >> dt.OpenCondVal \
			>> dt.CloseCondFormula >> dt.CloseCondOperator >> dt.CloseCondVal \
			>> dt.Times >> dt.Loop;
		return output;
	}


};
