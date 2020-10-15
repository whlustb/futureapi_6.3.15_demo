#pragma once

#include <QtWidgets/QMainWindow>
#include <QStandardItemModel>
#include "ui_MainForm.h"
#include <QDebug>
#include "CtpArbStruct.h"
#include <QList>

class MainWindow : public QMainWindow
{

    Q_OBJECT

public:
    MainWindow(QWidget *parent = Q_NULLPTR);
	void Init();
	~MainWindow();
	void SubscribeOrderInst(); //���Ľ��׺�Լ���顣
	void SubscribePosInst(); //���ĳֲֺ�Լ���顣
	void closeEvent(QCloseEvent *event); //��д�ر��¼���


//�źź�����
signals:
	void signal_UpdatePositionProfit(QString instrument_id, double last_price); //���³ֲ�ӯ��(�۸��и���ʱ)
	void signal_UpdateArbPrice(QString instrument_id, double last_price); //������������۲�(�۸��и���ʱ)
	void signal_RefreshPosTable();//���ֲֵ�ģ�ͱ�����
	void signal_RefreshOrdTable(QString orderRef, QString type);//�ύ�����ϵĶ�����

//�ۺ������塣	
public slots:
	void ShowLoginForm(); //��ʾ��¼���ڡ�
	void ShowInstrument(); //��ʾ��Լ����
	void ChooseInstrument(QModelIndex idx); //ѡ��ָ����Լ	
	void AddArbPortf(); //���������ϡ�
	void ClickArbPortf(QModelIndex); //���������ϡ�
	void ShowContextMenu(QPoint pos); //��������б�����ʾ���Ҽ�����¼���
	void DeleteArbPortf(); //ɾ��������ϡ�
	void DeleteArbOrder(); //ɾ������������
	void RefreshPosTable();
	void RefreshOrdTable(QString orderRef, QString type);
	//���ֲֵ�ģ�ͱ�����
	void AddToArbPortfTable(ArbPortf arb); //���������ϵ�ģ���С�
	void SubmitArbOrder(); //�ύ��������
	void UpdatePositionProfit(QString instrument_id, double last_price); //���³ֲ�ӯ��(�۸��и���ʱ)
	void UpdateArbPrice(QString instrument_id, double last_price); //������������۲�(�۸��и���ʱ)
	void UpdateInstrument();//���º�Լ��
	void AddToArbOrderTable(ArbOrder ord); ////������������䵽ģ�͡�
	ArbPortf FillArbPortf(); //�����е����ݣ�����������϶���
	ArbOrder FillArbOrder(); //�����е����ݣ�����������϶���

protected:
	bool eventFilter(QObject *obj, QEvent *e); //�¼���������

private:
    Ui::CtpArbClass ui;

	
};  
