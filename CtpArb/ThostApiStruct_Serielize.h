#pragma once
#pragma execution_character_set("utf-8")

#include <QDataStream>
#include "ThostFtdcUserApiStruct.h"
#include <iostream>
using namespace std;


//反序列化
QDataStream &operator>>(QDataStream& output, char* dt) {
	QString str;
	output >> str;
	if (str.isNull()) {
		strcpy(dt, "");
	}else {
		QByteArray bts = str.toLatin1();
		strcpy(dt, bts.data());//注意这里是复制，不能用=，否则是得不到的。
	}
	return output;
};

//反序列化
QDataStream &operator>>(QDataStream& output, char& dt) {
	QChar c;
	output >> c;
	dt = c.toLatin1();
	return output;
};

//序列化 
QDataStream &operator<<(QDataStream& input, const CThostFtdcInstrumentField& dt) {
	
	//
	input << QString(dt.InstrumentID) << QString(dt.ExchangeID) << QString(gbk2utf8(dt.InstrumentName)) << QString(dt.ExchangeInstID) << QString(dt.ProductID) << QChar(dt.ProductClass) << (qint32)dt.DeliveryYear
	<< qint32(dt.DeliveryMonth) << qint32(dt.MaxMarketOrderVolume) << qint32(dt.MinMarketOrderVolume) << qint32(dt.MaxLimitOrderVolume) << qint32(dt.MinLimitOrderVolume) << qint32(dt.VolumeMultiple)
	<< dt.PriceTick << QString(dt.CreateDate) << QString(dt.OpenDate) << QString(dt.ExpireDate) << QString(dt.StartDelivDate) << QString(dt.EndDelivDate) <<  QChar(dt.InstLifePhase) << qint32(dt.IsTrading)
	<< QChar(dt.PositionType) << QChar(dt.PositionDateType) << qreal(dt.LongMarginRatio) << qreal(dt.ShortMarginRatio) << QChar(dt.MaxMarginSideAlgorithm);
	QString str = "hello";
	input << QString(dt.UnderlyingInstrID);
	input << qreal(dt.StrikePrice) << QChar(dt.OptionsType) << qreal(dt.UnderlyingMultiple) << QChar(dt.CombinationType);
	
	
	return input;
};

//反序列化
QDataStream &operator>>(QDataStream& output, CThostFtdcInstrumentField& dt) {
	QString InstrumentName;
	output >> dt.InstrumentID >> dt.ExchangeID >> InstrumentName >> dt.ExchangeInstID >> dt.ProductID >> dt.ProductClass >> dt.DeliveryYear;
	output >> dt.DeliveryMonth >> dt.MaxMarketOrderVolume >> dt.MinMarketOrderVolume >> dt.MaxLimitOrderVolume >> dt.MinLimitOrderVolume >> dt.VolumeMultiple;
	output >> dt.PriceTick >> dt.CreateDate >> dt.OpenDate >> dt.ExpireDate >> dt.StartDelivDate >> dt.EndDelivDate >> dt.InstLifePhase >> dt.IsTrading;
	output >> dt.PositionType >> dt.PositionDateType >> dt.LongMarginRatio >> dt.ShortMarginRatio >> dt.MaxMarginSideAlgorithm;
	output >> dt.UnderlyingInstrID;
	output >> dt.StrikePrice >> dt.OptionsType >> dt.UnderlyingMultiple >> dt.CombinationType;

	//必须要转换一下，QString中全部是utf-8的，而貌似char*或者string类型的还是gbk的没有变。
	strcpy(dt.InstrumentName, string(InstrumentName.toLocal8Bit()).data());

	return output;
};