#pragma once
#include <QComboBox>
#include <QMouseEvent>

class MyComboBox :
	public QComboBox
{
	Q_OBJECT

public:
	explicit MyComboBox(QWidget *parent = 0);
	~MyComboBox();
protected:
	virtual void mousePressEvent(QMouseEvent *e);  //���������¼�

signals:
	void clicked();  //�Զ������źţ���mousePressEvent�¼�����ʱ����

};

