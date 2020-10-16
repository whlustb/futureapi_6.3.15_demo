#pragma once

#include <QList>
#include <QString>
#include <QDataStream>
#include <QTime>


//前置服务器
struct FrontServer {
	QString Name;
	char* Addr;
	char* MdAddr;
	char* BrokerID;
};
//配置文件结构体。 为了方便使用，全部用了这种char*的，除了name外。
struct ConfigInfo {
	char* UserProductInfo;
	char* AuthCode;
	char* AppID;

	char* UserID;
	char* Password;
	char* InvestorID;
	FrontServer server;

};


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

///套利组合、
struct ArbPortf
{
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


//普通订单、一旦提交订单、就立即保存、收到回报时，更新本地订单。
struct Order {
	
	///用户ID.
	QString	UserID;
	///报单引用
	QString	OrderRef;
	///合约代码
	QString	InstrumentID;
	///价格
	double Price;
	//成交均价。
	double AvgPrice;
	///报单数量
	int	VolumeTotalOriginal;
	///剩余数量
	int	VolumeTotal;
	///买卖方向
	QString	Direction;
	///开平标志
	QString	Offset;
	///报单来源
	QString	OrderSource;
	///报单状态
	QChar	OrderStatus;
	///报单日期
	QString	InsertDate;
	///委托时间
	QString	InsertTime;

	//记录成交价值、便于计算成交均价，
	double TotalDealed; //

	//记录它所属的套利单信息。
	QString ArbOrderId;
	int LegId;

	//序列化
	friend QDataStream &operator<<(QDataStream& input, const Order& dt) {
		input << dt.UserID << dt.OrderRef << dt.InstrumentID << dt.Price << dt.AvgPrice << dt.VolumeTotalOriginal << dt.VolumeTotal;
		input << dt.Direction << dt.Offset << dt.OrderSource << dt.OrderStatus << dt.InsertDate << dt.InsertTime ;
		input << dt.TotalDealed << dt.ArbOrderId << dt.LegId;
		return input;
	}

	//反序列化
	friend QDataStream &operator>>(QDataStream& output, Order& dt) {
		output >> dt.UserID >> dt.OrderRef >> dt.InstrumentID >> dt.Price >> dt.AvgPrice >> dt.VolumeTotalOriginal >> dt.VolumeTotal;
		output >> dt.Direction >> dt.Offset >> dt.OrderSource >> dt.OrderStatus >> dt.InsertDate >> dt.InsertTime;
		output >> dt.TotalDealed >> dt.ArbOrderId >> dt.LegId;
		return output;
	}

};

//订单单腿，继承的组合的单腿。
struct OrdLeg :public ArbLeg {
	int DealVol; //成交数量。
	double AvgPrice; //成交均价。
	QChar Status; //订单状态。

	QString OrderRef; //最新的这个订单的本地参考编号。。

	//序列化
	friend QDataStream &operator<<(QDataStream& input, const OrdLeg& dt) {
		input << dt.InstrumentID << dt.Direction << dt.Vol << dt.PriceType << dt.SlipPoint << dt.Chase << dt.ChasePoint;
		input << dt.DealVol << dt.AvgPrice << dt.Status << dt.OrderRef;
		return input;
	}

	//反序列化
	friend QDataStream &operator>>(QDataStream& output, OrdLeg& dt) {
		output >> dt.InstrumentID >> dt.Direction >> dt.Vol >> dt.PriceType >> dt.SlipPoint >> dt.Chase >> dt.ChasePoint;
		output >> dt.DealVol >> dt.AvgPrice >> dt.Status >> dt.OrderRef;
		return output;
	}
};


//套利单、继承套利基本配置
struct ArbOrder {
	QString Id; //订单编号。
	QString Offset; //开、平、自动开平
	QString SendOrderType; //下单方式：全部同时、主动腿。

	QString CondFormula; //下单条件：公式
	QString CondOperator; //下单条件。比较符号
	QString CondVal; //下单条件。比较值

	int Times; //份数，
	bool Loop; //循环套利。
	QTime Time; //下单时间

	QList<OrdLeg> OrdLegList; //套利单腿列表。

	//序列化
	friend QDataStream &operator<<(QDataStream& input, const ArbOrder& dt) {
		input << dt.Id << dt.Offset << dt.SendOrderType \
			<< dt.CondFormula << dt.CondOperator << dt.CondVal \
			<< dt.Times << dt.Loop << dt.Time << dt.OrdLegList;
		return input;
	}

	//反序列化
	friend QDataStream &operator>>(QDataStream& output, ArbOrder& dt) {
		output >> dt.Id >> dt.Offset >> dt.SendOrderType \
			>> dt.CondFormula >> dt.CondOperator >> dt.CondVal \
			>> dt.Times >> dt.Loop >> dt.Time >> dt.OrdLegList;
		return output;
	}
};
