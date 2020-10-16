#pragma once

#include <QList>
#include <QString>
#include <QDataStream>
#include <QTime>


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


//��ͨ������һ���ύ���������������桢�յ��ر�ʱ�����±��ض�����
struct Order {
	
	///�û�ID.
	QString	UserID;
	///��������
	QString	OrderRef;
	///��Լ����
	QString	InstrumentID;
	///�۸�
	double Price;
	//�ɽ����ۡ�
	double AvgPrice;
	///��������
	int	VolumeTotalOriginal;
	///ʣ������
	int	VolumeTotal;
	///��������
	QString	Direction;
	///��ƽ��־
	QString	Offset;
	///������Դ
	QString	OrderSource;
	///����״̬
	QChar	OrderStatus;
	///��������
	QString	InsertDate;
	///ί��ʱ��
	QString	InsertTime;

	//��¼�ɽ���ֵ�����ڼ���ɽ����ۣ�
	double TotalDealed; //

	//��¼����������������Ϣ��
	QString ArbOrderId;
	int LegId;

	//���л�
	friend QDataStream &operator<<(QDataStream& input, const Order& dt) {
		input << dt.UserID << dt.OrderRef << dt.InstrumentID << dt.Price << dt.AvgPrice << dt.VolumeTotalOriginal << dt.VolumeTotal;
		input << dt.Direction << dt.Offset << dt.OrderSource << dt.OrderStatus << dt.InsertDate << dt.InsertTime ;
		input << dt.TotalDealed << dt.ArbOrderId << dt.LegId;
		return input;
	}

	//�����л�
	friend QDataStream &operator>>(QDataStream& output, Order& dt) {
		output >> dt.UserID >> dt.OrderRef >> dt.InstrumentID >> dt.Price >> dt.AvgPrice >> dt.VolumeTotalOriginal >> dt.VolumeTotal;
		output >> dt.Direction >> dt.Offset >> dt.OrderSource >> dt.OrderStatus >> dt.InsertDate >> dt.InsertTime;
		output >> dt.TotalDealed >> dt.ArbOrderId >> dt.LegId;
		return output;
	}

};

//�������ȣ��̳е���ϵĵ��ȡ�
struct OrdLeg :public ArbLeg {
	int DealVol; //�ɽ�������
	double AvgPrice; //�ɽ����ۡ�
	QChar Status; //����״̬��

	QString OrderRef; //���µ���������ı��زο���š���

	//���л�
	friend QDataStream &operator<<(QDataStream& input, const OrdLeg& dt) {
		input << dt.InstrumentID << dt.Direction << dt.Vol << dt.PriceType << dt.SlipPoint << dt.Chase << dt.ChasePoint;
		input << dt.DealVol << dt.AvgPrice << dt.Status << dt.OrderRef;
		return input;
	}

	//�����л�
	friend QDataStream &operator>>(QDataStream& output, OrdLeg& dt) {
		output >> dt.InstrumentID >> dt.Direction >> dt.Vol >> dt.PriceType >> dt.SlipPoint >> dt.Chase >> dt.ChasePoint;
		output >> dt.DealVol >> dt.AvgPrice >> dt.Status >> dt.OrderRef;
		return output;
	}
};


//���������̳�������������
struct ArbOrder {
	QString Id; //������š�
	QString Offset; //����ƽ���Զ���ƽ
	QString SendOrderType; //�µ���ʽ��ȫ��ͬʱ�������ȡ�

	QString CondFormula; //�µ���������ʽ
	QString CondOperator; //�µ��������ȽϷ���
	QString CondVal; //�µ��������Ƚ�ֵ

	int Times; //������
	bool Loop; //ѭ��������
	QTime Time; //�µ�ʱ��

	QList<OrdLeg> OrdLegList; //���������б�

	//���л�
	friend QDataStream &operator<<(QDataStream& input, const ArbOrder& dt) {
		input << dt.Id << dt.Offset << dt.SendOrderType \
			<< dt.CondFormula << dt.CondOperator << dt.CondVal \
			<< dt.Times << dt.Loop << dt.Time << dt.OrdLegList;
		return input;
	}

	//�����л�
	friend QDataStream &operator>>(QDataStream& output, ArbOrder& dt) {
		output >> dt.Id >> dt.Offset >> dt.SendOrderType \
			>> dt.CondFormula >> dt.CondOperator >> dt.CondVal \
			>> dt.Times >> dt.Loop >> dt.Time >> dt.OrdLegList;
		return output;
	}
};
