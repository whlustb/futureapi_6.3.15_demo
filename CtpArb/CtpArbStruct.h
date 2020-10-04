#pragma once

#include <QList>
#include <QString>
#include <QDataStream>

struct ArbLeg {
	QString InstrumentID; //��ԼID
	QString Direction; //���׷����״Σ�
	int Vol; //����
	QString PriceType; //�۸����ͣ����̼ۡ��м�...
	int SlipPoint; //���㣬
	int ChasePoint; //׷��������

	//���л�
	friend QDataStream &operator<<(QDataStream& input, const ArbLeg& dt) {
		input << dt.InstrumentID << dt.Direction << dt.Vol << dt.PriceType << dt.SlipPoint << dt.ChasePoint;
		return input;
	}

	//�����л�
	friend QDataStream &operator>>(QDataStream& output, ArbLeg& dt) {
		output >> dt.InstrumentID >> dt.Direction >> dt.Vol >> dt.PriceType >> dt.SlipPoint >> dt.ChasePoint;
		return output;
	}

};

///��������Ϣ��
struct ArbItem
{
	QString Name; //���ơ�
	QList<ArbLeg> Legs;
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

	//���л�
	friend QDataStream &operator<<(QDataStream& input, const ArbItem& dt){
		input << dt.Name << dt.Legs << dt.Offset << dt.SendOrderType \
			<< dt.OpenCondFormula << dt.OpenCondOperator << dt.OpenCondVal \
			<< dt.CloseCondFormula << dt.CloseCondOperator << dt.CloseCondVal \
			<< dt.Times << dt.Loop;
			;
		return input;
	}

	//�����л�
	friend QDataStream &operator>>(QDataStream& output, ArbItem& dt){
		output >> dt.Name >> dt.Legs >> dt.Offset >> dt.SendOrderType \
			>> dt.OpenCondFormula >> dt.OpenCondOperator >> dt.OpenCondVal \
			>> dt.CloseCondFormula >> dt.CloseCondOperator >> dt.CloseCondVal \
			>> dt.Times >> dt.Loop;
		return output;
	}


};
