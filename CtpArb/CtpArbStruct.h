#pragma once

#include <QList>
#include <QString>
#include <QDataStream>
#include <QTime>

//������ϵ��ȣ�
struct ArbLeg {
	QString InstrumentID; //��ԼID
	QString Direction; //���׷����״Σ�
	int Vol; //����
	QString PriceType; //�۸����ͣ����̼ۡ��м�...
	int SlipPoint; //���㣬
	bool Chase; //�Ƿ�׷�ۡ�
	int ChasePoint; //׷��������


	//���л�
	friend QDataStream &operator<<(QDataStream& input, const ArbLeg& dt) {
		input << dt.InstrumentID << dt.Direction << dt.Vol << dt.PriceType << dt.SlipPoint << dt.Chase << dt.ChasePoint;
		return input;
	}

	//�����л�
	friend QDataStream &operator>>(QDataStream& output, ArbLeg& dt) {
		output >> dt.InstrumentID >> dt.Direction >> dt.Vol >> dt.PriceType >> dt.SlipPoint >> dt.Chase >> dt.ChasePoint;
		return output;
	}

};

///������ϡ�
struct ArbPortf
{
	QString Name; //���ơ�
	QString Offset; //����ƽ���Զ���ƽ
	QString SendOrderType; //�µ���ʽ��ȫ��ͬʱ�������ȡ�

	QString OpenCondFormula; //������������ʽ
	QString OpenCondOperator; //�����������ȽϷ���
	QString OpenCondVal; //�����������Ƚ�ֵ

	QString CloseCondFormula; //ƽ����������ʽ
	QString CloseCondOperator; //ƽ���������ȽϷ���
	QString CloseCondVal; //ƽ���������Ƚ�ֵ

	int Times; //������
	bool Loop; //ѭ��������
	QList<ArbLeg> LegList;

	//���л�
	friend QDataStream &operator<<(QDataStream& input, const ArbPortf& dt){
		input << dt.Name << dt.Offset << dt.SendOrderType \
			<< dt.OpenCondFormula << dt.OpenCondOperator << dt.OpenCondVal \
			<< dt.CloseCondFormula << dt.CloseCondOperator << dt.CloseCondVal \
			<< dt.Times << dt.Loop;
		input << dt.LegList;
		return input;
	}

	//�����л�
	friend QDataStream &operator>>(QDataStream& output, ArbPortf& dt){
		output >> dt.Name >> dt.Offset >> dt.SendOrderType \
			>> dt.OpenCondFormula >> dt.OpenCondOperator >> dt.OpenCondVal \
			>> dt.CloseCondFormula >> dt.CloseCondOperator >> dt.CloseCondVal \
			>> dt.Times >> dt.Loop;
		output >> dt.LegList;
		return output;
	}
};

//�������ȣ��̳е���ϵĵ��ȡ�
struct OrdLeg:public ArbLeg {
	int OrderId; //�����������Ķ����š�
	int DealVol; //�ɽ�������
	QString Status; //����״̬��

	//���л�
	friend QDataStream &operator<<(QDataStream& input, const OrdLeg& dt) {
		input << dt.InstrumentID << dt.Direction << dt.Vol << dt.PriceType << dt.SlipPoint << dt.Chase << dt.ChasePoint;
		input << dt.OrderId << dt.DealVol << dt.Status;
		return input;
	}

	//�����л�
	friend QDataStream &operator>>(QDataStream& output, OrdLeg& dt) {
		output >> dt.InstrumentID >> dt.Direction >> dt.Vol >> dt.PriceType >> dt.SlipPoint >> dt.Chase >> dt.ChasePoint;
		output >> dt.OrderId >> dt.DealVol >> dt.Status;
		return output;
	}
};

//���������̳�������������
struct ArbOrder{
	QString Name; //���ơ�
	QString Offset; //����ƽ���Զ���ƽ
	QString SendOrderType; //�µ���ʽ��ȫ��ͬʱ�������ȡ�

	QString CondFormula; //�µ���������ʽ
	QString CondOperator; //�µ��������ȽϷ���
	QString CondVal; //�µ��������Ƚ�ֵ

	int Times; //������
	bool Loop; //ѭ��������

	int Id; //������š�
	QTime Time; //�µ�ʱ��
	QList<OrdLeg> OrdLegList; //���������б�

	//���л�
	friend QDataStream &operator<<(QDataStream& input, const ArbOrder& dt) {
		input << dt.Name << dt.Offset << dt.SendOrderType \
			<< dt.CondFormula << dt.CondOperator << dt.CondVal \
			<< dt.Times << dt.Loop;
		input << dt.Id << dt.Time << dt.OrdLegList;
		return input;
	}

	//�����л�
	friend QDataStream &operator>>(QDataStream& output, ArbOrder& dt) {
		output >> dt.Name >> dt.Offset >> dt.SendOrderType \
			>> dt.CondFormula >> dt.CondOperator >> dt.CondVal \
			>> dt.Times >> dt.Loop;
		output >> dt.Id >> dt.Time >> dt.OrdLegList;
		return output;
	}
};


//ǰ�÷�����
struct FrontServer {
	QString Name;
	char* Addr;
	char* MdAddr;
	char* BrokerID;
};

//�����ļ��ṹ�塣 Ϊ�˷���ʹ�ã�ȫ����������char*�ģ�����name�⡣
struct ConfigInfo {
	char* UserProductInfo;
	char* AuthCode;
	char* AppID;

	char* UserID;
	char* Password;
	char* InvestorID;
	FrontServer server;

};

