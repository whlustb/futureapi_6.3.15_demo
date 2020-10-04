#pragma once
#include <QTreeView>
#include <QMouseEvent>

class MyTreeView :
	public QTreeView
{
	Q_OBJECT

public:
	explicit MyTreeView(QWidget *parent = 0);
	~MyTreeView();

protected:
	virtual void mousePressEvent(QMouseEvent *e);  //���������¼�

signals:
	void clicked();  //�Զ������źţ���mousePressEvent�¼�����ʱ����
};

