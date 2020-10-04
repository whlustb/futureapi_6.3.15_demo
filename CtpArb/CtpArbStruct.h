#pragma once

#include <QList>
#include <QString>
#include <QDataStream>
#include <QTime>

//�������ȣ�
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

///�������
struct ArbPortf
{
	QString Name; //���ơ�
	QList<ArbLeg> LegList;
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
	friend QDataStream &operator<<(QDataStream& input, const ArbPortf& dt){
		input << dt.Name << dt.LegList << dt.Offset << dt.SendOrderType \
			<< dt.OpenCondFormula << dt.OpenCondOperator << dt.OpenCondVal \
			<< dt.CloseCondFormula << dt.CloseCondOperator << dt.CloseCondVal \
			<< dt.Times << dt.Loop;
			;
		return input;
	}

	//�����л�
	friend QDataStream &operator>>(QDataStream& output, ArbPortf& dt){
		output >> dt.Name >> dt.LegList >> dt.Offset >> dt.SendOrderType \
			>> dt.OpenCondFormula >> dt.OpenCondOperator >> dt.OpenCondVal \
			>> dt.CloseCondFormula >> dt.CloseCondOperator >> dt.CloseCondVal \
			>> dt.Times >> dt.Loop;
		return output;
	}


};

//��������
struct ArbOrder {
	ArbPortf portf; //������ϡ�
	QTime Time; //�µ�ʱ��
	QList<int> StatusList; //ÿһ�ȵ�״̬����ArbLegs��һһ��Ӧ�ģ�δ�����µ� ���ɽ���

	//���л�
	friend QDataStream &operator<<(QDataStream& input, const ArbOrder& dt) {
		input << dt.portf << dt.Time << dt.StatusList ;
		return input;
	}

	//�����л�
	friend QDataStream &operator>>(QDataStream& output, ArbOrder& dt) {
		output >> dt.portf >> dt.Time >> dt.StatusList;;
		return output;
	}

};