#include "MyTreeView.h"


MyTreeView::MyTreeView(QWidget *parent) :QTreeView(parent)
{
}

MyTreeView::~MyTreeView()
{
}


void MyTreeView::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		emit clicked();  //����clicked�ź�
	}
	QTreeView::mousePressEvent(event);  //�����¼��������ദ����仰����Ҫ�����û�У������޷��������ĵ���¼�
}

